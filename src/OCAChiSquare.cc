#include "OCAChiSquare.hh"
#include "OCAPMT.hh"
#include "OCAModelParameterStore.hh"
#include "OCAPMTStore.hh"
#include "OCAMath.hh"
#include "OCAOpticsModel.hh"

using namespace OCA;
using namespace std;

ClassImp( OCAChiSquare )

//////////////////////////////////////
//////////////////////////////////////

OCAChiSquare::OCAChiSquare()
{

  // Ensure the pointers to the OCAPMTStore
  // and OCAOpticsModel pointers are initialised to 'NULL'
  fDataStore = NULL;
  fModel = NULL;

  fChiSq = 0.0;
  fResidualMean = 0.0;
  fResidualStd = 0.0;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAChiSquare::EvaluateResidual( OCAPMT& dPoint )
{

  Float_t modelOccRatio = fModel->ModelOccRatioPrediction( dPoint );

  Float_t occRatio = 0.0;
  Float_t occRatioErr = 0.0;
  OCAMath::CalculateMPEOccRatio( dPoint, occRatio, occRatioErr );

  return ( occRatio - modelOccRatio ) / occRatioErr;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAChiSquare::EvaluateChiSquare( OCAPMT& dPoint )
{

  // Calculate the model predicted value for the occupancy
  // ratio at this data point
  Float_t modelOccRatio = fModel->ModelOccRatioPrediction( dPoint );
  dPoint.SetModelOccupancyRatio( modelOccRatio );

  // Calculate the occupancy ratio from the data for this
  // data point.
  // Note: For now this just returns the statistical error
  // on the occupancy ratio. It does not account for the correction
  // due to the PMT incident angle. This will need to be implemented
  // in the future.
  Float_t occRatio = 0.0;
  Float_t occRatioErr = 0.0;
  OCAMath::CalculateMPEOccRatio( dPoint, occRatio, occRatioErr );

  Float_t occRatioError2 = occRatioErr * occRatioErr;
  Float_t variabilityError2 = TMath::Power( occRatio * OCAMath::CalculatePMTVariabilityError( dPoint ), 2 );

  // Calculate the difference between the model prediction
  // and the data value ( the residual for the chi-square calculation ).
  Float_t residual = ( occRatio - modelOccRatio );
  Float_t chiSq =  ( residual * residual ) / ( occRatioError2 + variabilityError2 );

  // Return the chi-square value.
  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAChiSquare::EvaluateGlobalResidual()
{

  fResidualMean = 0.0;
  fResidualStd = 0.0;

  // Create an iterator to loop through all the data points.
  vector< OCAPMT >::iterator iD;
  Int_t nGoodPoints = 0;
  for ( iD = fDataStore->GetOCAPMTsIterBegin();
        iD != fDataStore->GetOCAPMTsIterEnd();
        iD++ ){
   
    Float_t resVal = EvaluateResidual( *(iD) );
    (*iD).SetChiSquareResidual( resVal );
    if ( !std::isnan( resVal ) && !std::isinf( resVal ) ){
      fResidualMean += resVal;
      nGoodPoints++;
    }

  }

  fResidualMean /= nGoodPoints;

  for ( iD = fDataStore->GetOCAPMTsIterBegin();
        iD != fDataStore->GetOCAPMTsIterEnd();
        iD++ ){
   
    Float_t resStdVal = TMath::Power( EvaluateResidual( *(iD) ) - fResidualMean, 2 );
    if ( !std::isnan( resStdVal ) && !std::isinf( resStdVal ) ){
      fResidualStd += resStdVal;
    }

  }
  
  fResidualStd = TMath::Sqrt( fResidualStd / nGoodPoints );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAChiSquare::EvaluateGlobalChiSquare()
{
  
  // Calculate the total chisquare over all datapoints (PMTs) in the dataset.
  fChiSq = 0.0;

  // Create an iterator to loop through all the data points.
  vector< OCAPMT >::iterator iD;
  for ( iD = fDataStore->GetOCAPMTsIterBegin();
        iD != fDataStore->GetOCAPMTsIterEnd();
        iD++ ){
   
    // Add the chi-square value for this data-point to the overall
    // chi-square.
    if ( !std::isnan( EvaluateChiSquare( *(iD) ) )
         &&
         !std::isinf( EvaluateChiSquare( *(iD) ) ) ){
      fChiSq += EvaluateChiSquare( *(iD) );
    }
  }

  return fChiSq;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAChiSquare::FitEvaluation(  Float_t testParameters[], Int_t parametersVary[], 
                                   Int_t nParameters, Float_t **derivativeMatrix, 
                                   Float_t betaVec[], Float_t *chiSquareVal )
{

  // This routine is inspired by the original 
  // numerical recipes routine 'mrqcof' as featured on page 804 of...
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // --- 'Numerical Recipes: The Art of Scientific Computing' 3rd Edition
  // --- W.H.Press, S.A. Teukolsky, W.T. Vetterling, B.P. Flannery
  // --- Cambridge University Press, 2007
  // --- ISBN-10 0-521-88068-8
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // It has been annotated and rewritten to be consistent with the 
  // programming style of RAT.
  
  // This routine is used in the fitting procedure to evaluate the linearised fitting matrix, 'derivativeMatrix', and the vector 'betaVec' and calculates the chisquare.

  // Simple loop variables used in loops throughout this method when looking
  // over the elements of the current parameters, current guess for the solution
  // and the current values of the elements in the derivative matrices
  Int_t jVar = 0;
  Int_t kVar = 0;
  Int_t lVar = 0;
  Int_t mVar = 0;
  Int_t mFit = 0;
  
  // Variables used in the calculation of the error on the current
  // data point with respect to the current derviative on the parameters
  Float_t yMod = 0.0;
  Float_t weightVal = 0.0;
  Float_t dataError2 = 0.0;
  Float_t deltaDataVal = 0.0;
  Float_t* dDataValDParameters = NULL;
 
  // chi-squared for single entry in list
  Float_t chiSquareEntry = 0.0;
  
  // Second set of 'betaVec' and 'derivativeMatrix' to test against
  Float_t *betaVec2 = OCAMath::OCAVector( 1, nParameters );
  Float_t **derivativeMatrix2 = OCAMath::OCAMatrix( 1, nParameters, 1, nParameters );

  // Initialise the vector of derivatives of the data points with respect to the
  // parameters
  dDataValDParameters = OCAMath::OCAVector( 1, nParameters );
  Float_t* dDataValDParametersTMP = OCAMath::OCAVector( 1, nParameters );  

  // Check which of the current parameters are set to vary.
  for ( jVar = 1; jVar <= nParameters; jVar++ ){
    if ( parametersVary[ jVar ] ){ mFit++; }
    dDataValDParameters[ jVar ] = 0.0;
  }

  // Now loop over the matrix of deriviatives and the guess for the
  // vector solution and ensure all the initial values are set to zero.
  for ( jVar = 1; jVar <= mFit; jVar++ ){
    for ( kVar = 1; kVar <= jVar; kVar++ ){
      derivativeMatrix[ jVar ][ kVar ] = 0.0;
      derivativeMatrix2[ jVar ][ kVar ] = 0.0;
    }
    betaVec[ jVar ] = 0.0;
    betaVec2[ jVar ] = 0.0;
  }

  // Set the current value of the chiquare to zero
  *chiSquareVal = 0.0;
  
  // Create iterators for the loop below.
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = fDataStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = fDataStore->GetOCAPMTsIterEnd();

  OCAModelParameterStore* parPtr = new OCAModelParameterStore();
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
   
    // Evaluate the model for this data point and calculate the 
    // parameters and the derivative of the this point with respect
    // to each of these parameters.
    FitEvaluateModel( *iDP, testParameters, 
                      &yMod, dDataValDParameters, 
                      nParameters );

    // Variability error
    Float_t occRatioErr = 0.0;
    Float_t occRatio = 0.0;
    OCAMath::CalculateMPEOccRatio( *iDP, occRatio, occRatioErr );
    Float_t occRatioErr2 = occRatioErr * occRatioErr;
    Float_t variabilityErr2 = TMath::Power( occRatio * OCAMath::CalculatePMTVariabilityError( *iDP ), 2 );
    //cout << "variabilityErr2: " << variabilityErr2 << endl;
    //cout << "occRatioErr2: " << occRatioErr2 << endl;
    dataError2 = 1.0 / ( occRatioErr2 + variabilityErr2 );
    
    // And compute the difference between the model 
    // prediction and the data value.
    deltaDataVal = occRatio - yMod;

    // Add the chisqure entry to the overall chisquared value
    chiSquareEntry = ( deltaDataVal * deltaDataVal ) * dataError2;
    *chiSquareVal += chiSquareEntry;

    // Now set the parameter pointer to the parameters in the
    // parameter store object.
    parPtr = fModel->GetOCAModelParameterStore();
 
    // Identify which of those parameters vary.
    //cout << "chi square call" << endl;
    parPtr->IdentifyVaryingParameters();

    // Return the number of variable parameters for the current
    // data point and the arrays which map the indices of those
    // varying parameters to the parameters in the general store.
    Int_t nVariablePar = parPtr->GetNCurrentVariableParameters();
    Int_t* variableParameterIndex = parPtr->GetVariableParameterIndex();
    Int_t* variableParameterMap = parPtr->GetVariableParameterMap();

    // Now loop over all the variable parameters for this data point using the
    // look-up index, 'variableParameterIndex' and calculate their weightings
    // 'weightVal', i.e. the derivative with respect to a particular parameter
    // multiplied by the error on that data point
    for ( lVar = 1; lVar <= nVariablePar; lVar++ ) {
	
      weightVal = dDataValDParameters[ variableParameterIndex[ lVar ] ] * dataError2;
      // Now add these 'weightings' by derivative to the corresponding entry
      // in the matrix of derivatives 'derivativeMatrix'
      for ( mVar = 1; mVar <= lVar; mVar++ ) {
        // Identify the respective indices for the parameters being looked at
    	jVar = variableParameterIndex[ lVar ];
    	kVar = variableParameterIndex[ mVar ];

        // Fill the upper diagonal only (kVar <= jVar)
    	if ( kVar <= jVar ){ 
          Int_t varj = variableParameterMap[ jVar ];
          Int_t vark = variableParameterMap[ kVar ];

          derivativeMatrix[ varj ][ vark ] += weightVal * dDataValDParameters[ kVar ];
        }

      }

      // Now adjust the corresponding entry in the solution vector
      Int_t lVarIndex = variableParameterIndex[ lVar ];
      Int_t mapVar = variableParameterMap[ lVarIndex ];
      betaVec[ mapVar ] += deltaDataVal * weightVal;
    }
  }

  for ( jVar = 2; jVar <= mFit; jVar++ ){
    for ( kVar = 1; kVar < jVar; kVar++ ) {
      derivativeMatrix[ kVar ][ jVar ] = derivativeMatrix[ jVar ][ kVar ];
    }
  }
  
  // Free up the memory used by the vectors in this method
  OCAMath::OCAFree_Vector( dDataValDParameters, 1 );
  
  OCAMath::OCAFree_Matrix( derivativeMatrix2, 1, 1 );
  OCAMath::OCAFree_Vector( betaVec2, 1 );


}

//////////////////////////////////////
//////////////////////////////////////

void  OCAChiSquare::FitEvaluateModel( OCAPMT& dPoint, Float_t testParameters[],
                                      Float_t *modelVal, Float_t dDataValDParameters[], 
                                      Int_t nParameters )
{
 
  // Save the current parameters in the model...
  Float_t *tempParameterSave = new Float_t[ nParameters ];
  *tempParameterSave = *fModel->GetOCAModelParameterStore()->GetParametersPtr(); 

  // ...and set use the ones just passed to this method; the trial parameters.
  fModel->GetOCAModelParameterStore()->SetParametersPtr( testParameters );

  // Calculate the model prediction for said trail parameters.
  *modelVal = fModel->ModelOccRatioPrediction( dPoint, dDataValDParameters );

  // Restore the original, saved parameters
  fModel->GetOCAModelParameterStore()->SetParametersPtr( tempParameterSave );

  delete tempParameterSave;
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t OCAChiSquare::Minimise( Float_t testParameters[], Int_t parametersVary[], 
                              Int_t nParameters, Float_t **covarianceMatrix, 
                              Float_t **derivativeMatrix, Float_t *chiSquareVal, 
                              Float_t *aLambdaPar )
{

  // This routine is inspired by the original 
  // numerical recipes routine 'fit' as featured on page 803 of...
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // --- 'Numerical Recipes: The Art of Scientific Computing' 3rd Edition
  // --- W.H.Press, S.A. Teukolsky, W.T. Vetterling, B.P. Flannery
  // --- Cambridge University Press, 2007
  // --- ISBN-10 0-521-88068-8
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // It has been annotated and rewritten to be consistent with the 
  // programming style of RAT.

  // Loop variables for general use in this method.
  Int_t jVar = 0;
  Int_t kVar = 0;
  Int_t lVar = 0;
  Int_t mVar = 0;

  // Return value for the status of the Guass-Jordan elimination procedure
  // performed here. This is passed out of this method to
  // 'OCAChiSquare::PerformMinimisation' to indicate when, in part,
  // if the difference in the chi-square is small enough to stop the
  // minimisation routine.
  Int_t retval = 0;

  // Variable used to keep a count of the number of varying parameters.
  static Int_t mFit = 0;

  // The old chi-square value after a call of 'OCAChiSquare::FitEvaluation'. 
  static Float_t oldChiSquare = 0.0;

  // Pointer to the array of trial parameters.
  static Float_t* aTrial = NULL;

  // Pointer to the solution vector.
  static Float_t* betaVec = NULL;

  // Pointer to an array of values from a matrix line.
  static Float_t* matrixLine = NULL;

  // The matrix as described above, of one line.
  static Float_t** oneMatrixLine = NULL;

  // The lambda parameter as used in the Levenberg-Marquardt
  // algorithm for scaling the changes in the parameters.

  // If it is less than 0.0 then we initialise the vectors
  // and starting parameters.
  if ( *aLambdaPar < 0.0 ) {

    // Allocate memory to the arrays.
    aTrial = OCAMath::OCAVector( 1, nParameters );
    betaVec = OCAMath::OCAVector( 1, nParameters );
    matrixLine = OCAMath::OCAVector( 1, nParameters );

    // Loop through all the parameters to see which ones
    // vary in the fit.
    for ( mFit = 0, jVar = 1; jVar <= nParameters; jVar++ ){
      if ( parametersVary[ jVar ] ){
        mFit++;
      }
    }

    // Create a matrix of one line of length equal to the
    // number of varying parameters.
    oneMatrixLine = OCAMath::OCAMatrix( 1, mFit, 1, 1 );

    // Initialise the lambda parameter to 0.001 to begin with.
    // This corresponds to small initial variations in the parameter
    // changes.
    *aLambdaPar = 0.001;

    // Evaluate the model for these current parameters and compute
    // the solution vector, derivative matrix and chi-square value.
    FitEvaluation( testParameters, parametersVary, 
                   nParameters, derivativeMatrix, 
                   betaVec, chiSquareVal );

    // Set the old chi-square value to the curren value
    // in preparation for the next procedure.
    oldChiSquare = ( *chiSquareVal );

    // Set each of the trial parameters to those just used in the model
    // evaluation.
    for ( jVar = 1; jVar <= nParameters; jVar++ ){ 
      aTrial[ jVar ] = testParameters[ jVar ];
    }
  }

  // Loop through each of the varying parameters and take their
  // corresponding derivatives with respect to the model and use them
  // to calculate the corresponding entries in the covariance matrix.
  for ( jVar = 0, lVar = 1; lVar <= nParameters; lVar++ ) {
    if ( parametersVary[ lVar ] ) {
      for ( jVar++, kVar = 0, mVar = 1; mVar <= nParameters; mVar++ ) {

        if ( parametersVary[ mVar ] ) {
          kVar++;
          covarianceMatrix[ jVar ][ kVar ] = derivativeMatrix[ jVar ][ kVar ];
        }
        
      }

      covarianceMatrix[ jVar ][ jVar ] = derivativeMatrix[ jVar ][ jVar ] 
        * ( 1.0 + ( *aLambdaPar ) );

      oneMatrixLine[ jVar ][ 1 ] = betaVec[ jVar ];

      // If the covariance diagonal element is 0.0, this means
      // the derivative due to a varying parameter is 0.0
      // (which is a contradiction), meaning one of the parameters
      // which has been set to vary shouldn't have. Return an 
      // error accordingly.
      if( covarianceMatrix[ jVar ][ jVar ] <= 0.0 ) {

        if( covarianceMatrix[ jVar ][ jVar ] == 0.0 ) {
          cout << "Derivative is: " << derivativeMatrix[ jVar ][ jVar ] << endl;
          cout << "OCAChiSquare::Minimise: Error! Diagonal covariance matrix element [" << jVar  << ", " << jVar << "] is zero." << endl; 
          cout << "Tip: This is likely due to a parameter being forced to vary which shouldn't have. Check which parameters are varying." << endl;
        } 
        else {
          cout << "OCAChiSquare::Minimise: Error! Diagonal covariance matrix element [" << jVar  << ", " << jVar << "] is negative." << endl;
        }
      }
    }
  }

  // Perform the Guass Jordan elimination sorting.
  retval = OCAMath::GaussJordanElimination( covarianceMatrix, mFit,
                                            oneMatrixLine, 1 );
  
  for ( jVar = 1; jVar <= mFit; jVar++ ){ 
    matrixLine[ jVar ] = oneMatrixLine[ jVar ][ 1 ];
  }

  // If aLambdaPar is 0.0, then we can stop minimising and sort the covariance
  // matrix out before finishing.
  if ( *aLambdaPar == 0.0 ) {
    OCAMath::CovarianceSorting(covarianceMatrix,nParameters,parametersVary,mFit);

    // Deallocate the memory used by the pointer.
    OCAMath::OCAFree_Matrix( oneMatrixLine, 1, 1 );
    OCAMath::OCAFree_Vector( matrixLine, 1 );
    OCAMath::OCAFree_Vector( betaVec, 1 );
    OCAMath::OCAFree_Vector( aTrial, 1 );
    return retval;

  }

  // Add the change in the parameters to the parameter array.
  for ( jVar = 0, lVar = 1; lVar <= nParameters; lVar++ ){
    if ( parametersVary[ lVar ] ) { 
      aTrial[ lVar ] = testParameters[ lVar ] + matrixLine[ ++jVar ];
    }
  } 

  // Perform a final fit evaluation with these parameters.
  FitEvaluation( aTrial, parametersVary, nParameters, 
                 covarianceMatrix, matrixLine, chiSquareVal );
  
  // Check that the newly obtained chi-square value
  // is smaller than the previous one and adjust the lambda
  // variable accordingly (i.e. make the change smaller).
  if ( *chiSquareVal < oldChiSquare ) {

    *aLambdaPar *= 0.1;
    oldChiSquare = ( *chiSquareVal );

    for ( jVar = 0, lVar = 1; lVar <= nParameters; lVar++ ) {
      if ( parametersVary[ lVar ] ) {
        for ( jVar++, kVar = 0, mVar = 1; mVar <= nParameters; mVar++ ) {
          if ( parametersVary[ mVar ] ) {
            kVar++;
            derivativeMatrix[ jVar ][ kVar ] = covarianceMatrix[ jVar ][ kVar ];
          }
        }

        betaVec[ jVar ] = matrixLine[ jVar ];
        testParameters[ lVar ] = aTrial[ lVar ];
      }
    }
  } 

  // If the newly obtained chisquare is not smaller, change the
  // lambda parameter accoridngly (i.e. make it larger)
  // to burrow out of the parameter space to the top again.
  else {
    *aLambdaPar *= 10.0;
    *chiSquareVal=oldChiSquare;
  }

  // Return the status of this minimisation.
  return retval;
 
}

//////////////////////////////////////
//////////////////////////////////////

void OCAChiSquare::PerformMinimisation( Float_t testParameters[], Int_t parametersVary[], 
                                        Int_t nParameters, Float_t **covarianceMatrix, 
                                        Float_t **derivativeMatrix, Float_t *chiSquareVal )
{
  
  // Fit the data using 'OCAChiSquare::Minimise' until convergence
  // is achieved.

  // Maximum number of iterations for the fit.
  Int_t maxNIters = 100;

  // Counter variable for the current number of iterations.
  Int_t numIters = 0;

  // Counter variable for the current number of 'good' iterations.
  // 'good' being the conditional statement in the while loop below.
  Int_t nGoodIters = 0;

  // The return value from the current minimisation.
  Int_t retval = 0;

  // The previous chi-square value.
  Float_t oldChiSquare = 0;

  // The lambda parameter for the Levenberg-marquardt algorithm.
  // This parameter scales the change in the parameters between
  // minimisations. Set to -1.0 to begin with for the intitialisation
  // in 'OCAChiSquare::Minimise'.
  Float_t lambdaPar = -1.0;

  // The accepted change between the new and previous chisquare values
  // for a good iteration.
  Float_t toleranceVal = 1.0;    // Chisquared must change by toleranceVal to warrant another iteration

  // Set the chi-square value to 0.0 to begin with.
  *chiSquareVal = 0.0;

  // Perform the initial minimisation before the while loop
  // below.
  retval = Minimise( testParameters, parametersVary, nParameters, 
                     covarianceMatrix, derivativeMatrix, chiSquareVal, 
                     &lambdaPar );

  // Set the old chi-square to the current value before
  // the next iteration.
  oldChiSquare = *chiSquareVal;

  // Print the intial chi-square value.
  printf("Initial chi-square value = %8.2f\n", *chiSquareVal );
  printf( "---------------------------\n" );
  
  // Next set lambdaPar to 0.01, and iterate until convergence is reached.
  lambdaPar = 0.01;

  // Minimisation loop. Whilst the difference between successive
  // chi-square values is greater than the require tolerance, and
  // whilst the number of iterations is less than the maximum,
  // keep minimising until the conditions are met.
  while ( ( ( TMath::Abs( *chiSquareVal - oldChiSquare ) > toleranceVal 
              || nGoodIters < 4 ) && ( numIters < maxNIters ) )
          && retval == 0 
          && lambdaPar != 0.0 ) {

    // Set the old chi-square value to the current one
    // in preparation for the next minimisation.
    oldChiSquare = *chiSquareVal;

    // Print the lambda parameter which will determine the change
    // in the new parameters to be trailled in the next 'Minimise'
    // call below.
    printf( "Iteration %d with lambda parameter %g.\n", numIters, lambdaPar );
    retval = Minimise( testParameters, parametersVary, nParameters,
                       covarianceMatrix, derivativeMatrix, chiSquareVal,
                       &lambdaPar );
    numIters++;

    // Print the new chi-square value and it's associated lambda parameter.
    printf( "New chi-square = %12.2f.\n", *chiSquareVal );
    printf( "---------------------------\n" );

    // If the new ch-square is less than the previous one, increment
    // the 'good' iteration counter.
    if ( TMath::Abs( oldChiSquare - *chiSquareVal ) < toleranceVal ){ 
      nGoodIters ++; 
    }

    // Otherwise reset the number of good iterations to start
    // again at the beginning of the loop next time round.
    else{ 
      nGoodIters = 0; 
    }

  }
  
  // We're done.  Set lambdaPar = 0.0 and call 'OCAChiSquare::Minimise'
  // one last time.  This attempts to calculate covariance 
  // (covarianceMatrix), and curvature (derivativeMatrix) matrices. 
  // It also frees up allocated memory.
  lambdaPar = 0.0;
  Minimise( testParameters, parametersVary, nParameters, 
            covarianceMatrix, derivativeMatrix, chiSquareVal, 
            &lambdaPar );

}

//////////////////////////////////////
//////////////////////////////////////

void OCAChiSquare::PerformOpticsFit( const Int_t passNum )
{

  // Set the global chi-square value to 0.0 to begin with.
  Float_t chiSquare = 0.0;
  
  // From the data store identify which of the PMT angular
  // response bins and which of the laserball distribution
  // bins will vary in the fit. We only want to vary
  // parameters which are representative of bins with sufficient
  // entries in both the distributions.
  fModel->IdentifyVaryingPMTAngularResponseBins( fDataStore );
  fModel->IdentifyVaryingLBDistributionBins( fDataStore );
  fModel->InitialiseLBRunNormalisations( fDataStore );
  
  // Identify the parameters which vary for all the data points.
  // i.e. the global variable parameters such as the extinction lengths.
  fModel->GetOCAModelParameterStore()->IdentifyBaseVaryingParameters();
  
  // Initialise the private PMT angular response look-up array.
  fModel->GetOCAModelParameterStore()->InitialisePMTAngularResponseIndex();

  // Get the array of current parameter values.
  Float_t* parameters = fModel->GetOCAModelParameterStore()->GetParametersPtr();
  
  // Get the array of corresponding flags to indicate whether or not
  // the parameter of the smae index in the 'parameters' pointer varies or not.
  Int_t* parametersVary = fModel->GetOCAModelParameterStore()->GetParametersVary();
  
  // Get the total number of parameters in the model.
  Int_t nParameters = fModel->GetOCAModelParameterStore()->GetNParameters();
  
  // Get the pointer to the covariance and derivative matrices.
  Float_t** covarianceMatrix = fModel->GetOCAModelParameterStore()->GetCovarianceMatrix();
  Float_t** derivativeMatrix = fModel->GetOCAModelParameterStore()->GetDerivativeMatrix();

  for ( Int_t iPar = 1; iPar <= nParameters; iPar++ ){
    cout << "Parameter: " << iPar << " is: " << parameters[ iPar ] << " with flag: " << parametersVary[ iPar ] << " and error: " << TMath::Sqrt( covarianceMatrix[ iPar ][ iPar ] ) << endl;
  }
  
  // Perform the minimisation for the optics fit.
  PerformMinimisation( parameters, parametersVary, 
                       nParameters, covarianceMatrix,
                       derivativeMatrix, &chiSquare );
  
  // Set the parameters in the parameter store before finishing.
  fModel->GetOCAModelParameterStore()->SetParametersPtr( parameters );
  
  // Set the covariance matrix in the parameter store before finishing.
  fModel->GetOCAModelParameterStore()->SetCovarianceMatrix( covarianceMatrix );

  fModel->GetOCAModelParameterStore()->IdentifyGlobalVaryingParameters();
  Int_t nVaryingParameters = fModel->GetOCAModelParameterStore()->GetNGlobalVariableParameters();
  Int_t nDataPoints = fDataStore->GetNDataPoints();
  // Print the reduced chi-square.
  printf( "Reduced Chi-Square = %.5f / ( %i - %i ) = %.5f\n-----------------------\n",
          chiSquare, nDataPoints, nVaryingParameters,
          chiSquare / ( nDataPoints - nVaryingParameters ) );
  
}
