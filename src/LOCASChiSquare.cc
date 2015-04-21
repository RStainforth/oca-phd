#include "LOCASChiSquare.hh"
#include "LOCASDataPoint.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASMath.hh"
#include "LOCASOpticsModel.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASChiSquare )

//////////////////////////////////////
//////////////////////////////////////

LOCASChiSquare::LOCASChiSquare()
{

  // Ensure the pointers to the LOCASDataStore
  // and LOCASOpticsModel pointers are initialised to 'NULL'
  fDataStore = NULL;
  fModel = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateChiSquare( LOCASDataPoint& dPoint )
{

  // Calculate the model predicted value for the occupancy
  // ratio at this data point
  Float_t modelVal = fModel->ModelPrediction( dPoint );
  dPoint.SetModelOccupancyRatio( modelVal );

  // Calculate the occupancy ratio from the data for this
  // data point.
  // Note: For now this just returns the statistical error
  // on the occupancy ratio. It does not account for the correction
  // due to the PMT incident angle. This will need to be implemented
  // in the future.
  Float_t dataVal = 0.0;
  Float_t error = 0.0;
  LOCASMath::CalculateMPEOccRatio( dPoint, dataVal, error );
  dPoint.SetOccupancyRatio( dataVal );
  dPoint.SetOccupancyRatioErr( error );

  // Calculate the difference between the model prediction
  // and the data value ( the residual for the chi-square calculation ).
  Float_t residual = ( dataVal - modelVal );

  // Calculate the chi-square value.
  Float_t chiSq =  ( residual * residual ) / ( error * error );

  // Return the chi-square value.
  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateGlobalChiSquare()
{
  
  // Calculate the total chisquare over all datapoints (PMTs) in the dataset.
  Float_t chiSq = 0.0;

  // Create an iterator to loop through all the data points.
  vector< LOCASDataPoint >::iterator iD;

  for ( iD = fDataStore->GetLOCASDataPointsIterBegin();
        iD != fDataStore->GetLOCASDataPointsIterEnd();
        iD++ ){
   
    // Add the chi-square value for this data-point to the overall
    // chi-square.
    chiSq += EvaluateChiSquare( *(iD) );

  }

  // Return the global chi-square value.
  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASChiSquare::FitEvaluation(  Float_t testParameters[], Int_t parametersVary[], 
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
  Float_t *betaVec2 = LOCASMath::LOCASVector( 1, nParameters );
  Float_t **derivativeMatrix2 = LOCASMath::LOCASMatrix( 1, nParameters, 1, nParameters );

  // Initialise the vector of derivatives of the data points with respect to the
  // parameters
  dDataValDParameters = LOCASMath::LOCASVector( 1, nParameters );

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
  vector< LOCASDataPoint >::iterator iDP;
  vector< LOCASDataPoint >::iterator iDPBegin = fDataStore->GetLOCASDataPointsIterBegin();
  vector< LOCASDataPoint >::iterator iDPEnd = fDataStore->GetLOCASDataPointsIterEnd();

  LOCASModelParameterStore* parPtr = new LOCASModelParameterStore();
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    // Evaluate the model for this data point and calculate the 
    // parameters and the derivative of the this point with respect
    // to each of these parameters.
    FitEvaluateModel( *iDP, testParameters, 
                      &yMod, dDataValDParameters, 
                      nParameters );
    
    // Compute the 1 / sigma^2 value for this particular data point
    dataError2 = 1.0 / ( iDP->GetOccupancyRatioErr() 
                         * iDP->GetOccupancyRatioErr() );

    // And compute the difference between the model 
    // prediction and the data value.
    deltaDataVal = iDP->GetOccupancyRatio() - yMod;

    // Now set the parameter pointer to the parameters in the
    // parameter store object.
    parPtr = fModel->GetLOCASModelParameterStore();

    // Identify which of those parameters vary.
    parPtr->IdentifyVaryingParameters();

    // Return the number of variable parameters for the current
    // data point and the arrays which map the indices of those
    // varying parameters to the parameters in the general store.
    Int_t nVariablePar = parPtr->GetNCurrentVariableParameters();
    Int_t* variableParameterIndex = parPtr->GetVariableParameterIndex();
    Int_t* variableParameterMap = parPtr->GetVariableParameterMap();

    // Now loop over all the variable parameters for this data point using the
    // look-up index, 'variableParameterIndex' and calculate their weightings
    // 'weightVal', i.e. the derivative with respect to a particualr parameter
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

    // Add the chisqure entry to the overall chisquared value
    chiSquareEntry = deltaDataVal * deltaDataVal * dataError2;
    *chiSquareVal += chiSquareEntry;
  }

  for ( jVar = 2;jVar<=mFit;jVar++){
    for ( kVar = 1; kVar < jVar; kVar++ ) {
      derivativeMatrix[ kVar ][ jVar ] = derivativeMatrix[ jVar ][ kVar ];
    }
  }
  
  // Free up the memory used by the vectors in this method
  LOCASMath::LOCASFree_Vector( dDataValDParameters, 1 );
  
  LOCASMath::LOCASFree_Matrix( derivativeMatrix2, 1, 1 );
  LOCASMath::LOCASFree_Vector( betaVec2, 1 );


}

//////////////////////////////////////
//////////////////////////////////////

void  LOCASChiSquare::FitEvaluateModel( LOCASDataPoint& dPoint, Float_t testParameters[],
                                        Float_t *modelVal, Float_t dDataValDParameters[], 
                                        Int_t nParameters )
{
 
  // Save the current parameters in the model...
  Float_t *tempParameterSave = new Float_t[ nParameters ];
  *tempParameterSave = *fModel->GetLOCASModelParameterStore()->GetParametersPtr(); 

  // ...and set use the ones just passed to this method; the trial parameters.
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( testParameters );

  // Calculate the model prediction for said trail parameters.
  *modelVal = fModel->ModelPrediction( dPoint, dDataValDParameters );

  // Restore the original, saved parameters
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( tempParameterSave );

  delete tempParameterSave;
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASChiSquare::Minimise( Float_t testParameters[], Int_t parametersVary[], 
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
  // 'LOCASChiSquare::PerformMinimisation' to indicate when, in part,
  // if the difference in the chi-square is small enough to stop the
  // minimisation routine.
  Int_t retval = 0;

  // Variable used to keep a count of the number of varying parameters.
  static Int_t mFit = 0;

  // The old chi-square value after a call of 'LOCASChiSquare::FitEvaluation'. 
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
    aTrial = LOCASMath::LOCASVector( 1, nParameters );
    betaVec = LOCASMath::LOCASVector( 1, nParameters );
    matrixLine = LOCASMath::LOCASVector( 1, nParameters );

    // Loop through all the parameters to see which ones
    // vary in the fit.
    for ( mFit = 0, jVar = 1; jVar <= nParameters; jVar++ ){
      if ( parametersVary[ jVar ] ){
        mFit++;
      }
    }

    // Create a matrix of one line of length equal to the
    // number of varying parameters.
    oneMatrixLine=LOCASMath::LOCASMatrix( 1, mFit, 1, 1 );

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
          cout << "LOCASChiSquare::Minimise: Error! Diagonal covariance matrix element [" << jVar  << ", " << jVar << "] is zero." << endl; 
          cout << "Tip: This is likely due to a parameter being forced to vary which shouldn't have. Check which parameters are varying." << endl;
        } 
        else {
          cout << "LOCASChiSquare::Minimise: Error! Diagonal covariance matrix element [" << jVar  << ", " << jVar << "] is negative." << endl;
        }
      }
    }
  }

  // Perform the Guass Jordan elimination sorting.
  retval = LOCASMath::GaussJordanElimination( covarianceMatrix, mFit,
                                              oneMatrixLine, 1 );
  
  for ( jVar = 1; jVar <= mFit; jVar++ ){ 
    matrixLine[ jVar ] = oneMatrixLine[ jVar ][ 1 ];
  }

  // If aLambdaPar is 0.0, then we can stop minimising and sort the covariance
  // matrix out before finishing.
  if ( *aLambdaPar == 0.0 ) {
    LOCASMath::CovarianceSorting(covarianceMatrix,nParameters,parametersVary,mFit);

    // Deallocate the memory used by the pointer.
    LOCASMath::LOCASFree_Matrix( oneMatrixLine, 1, 1 );
    LOCASMath::LOCASFree_Vector( matrixLine, 1 );
    LOCASMath::LOCASFree_Vector( betaVec, 1 );
    LOCASMath::LOCASFree_Vector( aTrial, 1 );
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

void LOCASChiSquare::PerformMinimisation( Float_t testParameters[], Int_t parametersVary[], 
                            Int_t nParameters, Float_t **covarianceMatrix, 
                            Float_t **derivativeMatrix, Float_t *chiSquareVal )
{
  
  // Fit the data using 'LOCASChiSquare::Minimise' until convergence
  // is achieved.

  // Maximum number of iterations for the fit.
  Int_t maxNIters = 1000;

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
  // in 'LOCASChiSquare::Minimise'.
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
  
  // We're done.  Set lambdaPar = 0.0 and call 'LOCASChiSquare::Minimise'
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

void LOCASChiSquare::PerformOpticsFit()
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

  // Identify the parameters which vary for all the data points.
  // i.e. the global variable parameters such as the extinction lengths.
  fModel->GetLOCASModelParameterStore()->IdentifyBaseVaryingParameters();

  // Initialise the private PMT angular response look-up array.
  fModel->GetLOCASModelParameterStore()->InitialisePMTAngularResponseIndex();
  
  // Get the array of current parameter values.
  Float_t* parameters = fModel->GetLOCASModelParameterStore()->GetParametersPtr();

  // Get the array of corresponding flags to indicate whether or not
  // the parameter of the smae index in the 'parameters' pointer varies or not.
  Int_t* parametersVary = fModel->GetLOCASModelParameterStore()->GetParametersVary();

  // Get the total number of parameters in the model.
  Int_t nParameters = fModel->GetLOCASModelParameterStore()->GetNParameters();

  // Get the pointer to the covariance and derivative matrices.
  Float_t** covarianceMatrix = fModel->GetLOCASModelParameterStore()->GetCovarianceMatrix();
  Float_t** derivativeMatrix = fModel->GetLOCASModelParameterStore()->GetDerivativeMatrix();
  
  // Perform the minimisation for the optics fit.
  PerformMinimisation( parameters, parametersVary, 
                       nParameters, covarianceMatrix,
                       derivativeMatrix, &chiSquare );
  
  // Set the parameters in the parameter store before finishing.
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( parameters );
  
  // Set the covariance matrix in the parameter store before finishing.
  fModel->GetLOCASModelParameterStore()->SetCovarianceMatrix( covarianceMatrix );
  
}
