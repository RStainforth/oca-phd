////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASChiSquare.cc
///
/// CLASS: LOCAS::LOCASChiSquare
///
/// BRIEF: A simple class to compute the global chi-square
///        of an entire dataset given a LOCASModel object and
///        LOCASModelParameterStore object
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

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

  fDataStore = NULL;
  fModel = NULL;

  fDataIndex = NULL;
  fDataVals = NULL;
  fDataErrors = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASChiSquare::~LOCASChiSquare()
{

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateChiSquare( LOCASDataPoint& dPoint )
{

  Float_t modelVal = fModel->ModelPrediction( dPoint );
  dPoint.SetModelOccupancyRatio( modelVal );

  Float_t dataVal = 0.0;
  Float_t error = 0.0;
  LOCASMath::CalculateMPEOccRatio( dPoint, dataVal, error );
  dPoint.SetOccupancyRatio( dataVal );
  dPoint.SetOccupancyRatioErr( error );

  Float_t residual = ( dataVal - modelVal );
  Float_t chiSq =  ( residual * residual ) / ( error * error );
  cout << "dataVal is: " << dataVal << endl;
  cout << "modelVal is: " << modelVal << endl;

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateGlobalChiSquare()
{
  
  // Calculate the total chisquare over all datapoints (PMTs) in the dataset
  Float_t chiSq = 0.0;
  std::vector< LOCASDataPoint >::iterator iD;

  for ( iD = fDataStore->GetLOCASDataPointsIterBegin();
        iD != fDataStore->GetLOCASDataPointsIterEnd();
        iD++ ){

      chiSq += EvaluateChiSquare( *(iD) );
  }

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASChiSquare::InitialiseArrays()
{

  Int_t nDPs = 1;
  Int_t nDataPoints = fDataStore->GetNDataPoints();
  fDataIndex = LOCASMath::LOCASVector( 1, nDataPoints );
  fDataVals = LOCASMath::LOCASVector( 1, nDataPoints );
  fDataErrors = LOCASMath::LOCASVector( 1, nDataPoints );

  std::vector< LOCASDataPoint >::iterator iDP;
  for ( iDP = fDataStore->GetLOCASDataPointsIterBegin(); 
        iDP != fDataStore->GetLOCASDataPointsIterEnd(); 
        iDP++ ) {
    
    fDataIndex[ nDPs ] = ( 10000.0 * iDP->GetRunIndex() ) + iDP->GetPMTID();
    fDataVals[ nDPs ] = iDP->GetOccupancyRatio();
    fDataErrors[ nDPs ] = iDP->GetOccupancyRatioErr();
    
    nDPs++;
  }
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASChiSquare::FitEvaluation( Float_t dataIndex[], Float_t dataVals[], 
                                     Float_t dataErrors[], Int_t nDataPoints, 
                                     Float_t testParameters[], Int_t parametersVary[], 
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

  // This routine is used in the fittig procedure to evaluate the linearised fitting matrix, 'derivativeMatrix', and the vector 'betaVec' and calculates the chisquare.

  // Simple loop variables used in loops throughout this method when looking
  // over the elements of the current parameters, current guess for the solution
  // and the current values of the elements in the derivative matrices
  Int_t iVar = 0;
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
 
  Float_t chiSquareEntry;  // chi-squared for single entry in list
  
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
  cout << "DataPoint Cycle" << endl;
  
  //std::vector< LOCASDataPoint >::iterator iDP;
  //std::vector< LOCASDataPoint >::iterator iDPBegin = fDataStore->GetLOCASDataPointsIterBegin();
  //std::vector< LOCASDataPoint >::iterator iDPEnd = fDataStore->GetLOCASDataPointsIterEnd();

  LOCASDataPoint* lDP = new LOCASDataPoint();
  Int_t nDPs = fDataStore->GetNDataPoints();
  LOCASDataPoint* dataPoints = new LOCASDataPoint[ nDPs ];
  for ( Int_t iDP = 0; iDP < nDPs; iDP++ ){ dataPoints[ iDP ] = fDataStore->GetDataPoint( iDP ); }
  cout << "nDPs: " << nDPs << endl;
  for ( Int_t iDPInt = 0; 
        iDPInt < nDPs; 
        iDPInt++ ) {
    
    *lDP = dataPoints[ iDPInt ];
    iVar++;
    // Evaluate the model for this data point and calculate the 
    // parameters and the derivative of the this point with respect
    // to each of these parameters
    //cout << "FitEvaluate Model1" << endl;
    FitEvaluateModel( *lDP, testParameters, &yMod, dDataValDParameters, nParameters );
    //cout << "FitEvaluate Model2" << endl;
    
    // Compute the 1 / sigma^2 value for this particular data point
    dataError2 = 1.0 / ( lDP->GetOccupancyRatioErr() * lDP->GetOccupancyRatioErr() );

    // And compute the difference between the model prediction and the data value
    deltaDataVal = lDP->GetOccupancyRatio() - yMod;
    //cout << "Identify Varying Parameters" << endl;
    LOCASModelParameterStore* parPtr = new LOCASModelParameterStore();
    parPtr = fModel->GetLOCASModelParameterStore();
    //cout << "Identify Varying Parameters 0" << endl;
    parPtr->IdentifyVaryingParameters();
    //cout << "Identify Varying Parameters 1" << endl;
    Int_t nVariablePar = parPtr->GetNCurrentVariableParameters();
    //cout << "Identify Varying Parameters 2" << endl;
    Int_t* variableParameterIndex = parPtr->GetVariableParameterIndex();
    //cout << "Identify Varying Parameters 3" << endl;
    Int_t* variableParameterMap = parPtr->GetVariableParameterMap();

    // Now loop over all the variable parameters for this data point using the
    // look-up index, 'variableParameterIndex' and calculate their weightings
    // 'weightVal', i.e. the derivative with respect to a particualr parameter
    // multiplied by the error on that data point
    for ( lVar = 1; lVar <= nVariablePar; lVar++ ) {
      //cout << "Weighted Check" << endl;	
      weightVal = dDataValDParameters[ variableParameterIndex[ lVar ] ] * dataError2;
      // Now add these 'weightings' by derivative to the corresponding entry
      // in the matrix of derivatives 'derivativeMatrix'
      for ( mVar = 1; mVar <= lVar; mVar++ ) {
        // Identify the respective indices for the parameters being looked at
        //cout << "Weighted Check2" << endl;
    	jVar = variableParameterIndex[ lVar ];
    	kVar = variableParameterIndex[ mVar ];
        
        // Fill the upper diagonal only (kVar <= jVar)
    	if ( kVar <= jVar ){ 
          //cout << "Weighted Check3" << endl;
          //cout << "jVar is: " << jVar << " kVar is: " << kVar << endl;
          Int_t varj = variableParameterMap[ jVar ];
          Int_t vark = variableParameterMap[ kVar ];
          //cout << "varj is: " << varj << " vark is: " << vark << endl;
          derivativeMatrix[ varj ][ vark ] += weightVal * dDataValDParameters[ kVar ];
        }
        //cout << "Weighted Check3a" << endl;
      }
      //cout << "Weighted Check3b" << endl;
      // Now adjust the corresponding entry in the solution vector
      Int_t lvar = variableParameterIndex[ lVar ];
      Int_t mapvar = variableParameterMap[ lvar ];
      betaVec[ mapvar ] += deltaDataVal * weightVal;
    }
    //cout << "Weighted Check4" << endl;
    // Add the chisqure entry to the overall chisquared value
    chiSquareEntry = deltaDataVal*deltaDataVal*dataError2;
    *chiSquareVal += chiSquareEntry;
    if ( iVar % 1 == 2000 ) printf( "Mod|2000 Value: %d %f\n", iVar, chiSquareEntry );
    //cout << "Weighted Check5" << endl;
  }

  cout << "Got this far!" << endl;
  for ( jVar = 2;jVar<=mFit;jVar++){
    for ( kVar = 1; kVar < jVar; kVar++ ) {
      derivativeMatrix[ kVar ][ jVar ] = derivativeMatrix[ jVar ][ kVar ];
    }
  }
  
  // Free up the memory used by the vectors in this method
  LOCASMath::LOCASFree_Vector( dDataValDParameters, 1, nParameters );
  
  LOCASMath::LOCASFree_Matrix( derivativeMatrix2, 1, nParameters, 1, nParameters );
  LOCASMath::LOCASFree_Vector( betaVec2, 1, nParameters );

}

//////////////////////////////////////
//////////////////////////////////////

void  LOCASChiSquare::FitEvaluateModel( LOCASDataPoint& dPoint, Float_t testParameters[],
                                        Float_t *modelVal, Float_t dDataValDParameters[], 
                                        Int_t nParameters )
{
 
  // Save parameters and use the ones just passed
  Float_t *tempParameterSave = new Float_t[ nParameters ];
  *tempParameterSave = *fModel->GetLOCASModelParameterStore()->GetParametersPtr(); 
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( testParameters );

  // Calculation of the model value
  *modelVal = fModel->ModelPrediction( dPoint, dDataValDParameters );

  // Restore saved parameters
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( tempParameterSave );
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASChiSquare::Minimise( Float_t dataIndex[], Float_t dataVals[], 
                                Float_t dataErrors[], Int_t nDataPoints, 
                                Float_t testParameters[], Int_t parametersVary[], 
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

  int j,k,l,m, retval = 0;
  static int mfit;
  static float ochisq,*atry,*beta,*da,**oneda;
  cout << "Hello" << endl;
  if (*aLambdaPar < 0.0) {
    atry=LOCASMath::LOCASVector(1,nParameters);
    beta=LOCASMath::LOCASVector(1,nParameters);
    da=LOCASMath::LOCASVector(1,nParameters);
    for (mfit=0,j=1;j<=nParameters;j++){
      if (parametersVary[j]){
        mfit++;
      }
    }
    oneda=LOCASMath::LOCASMatrix(1,mfit,1,1);
    *aLambdaPar=0.001;
    cout << "FitEvaluation" << endl;
    FitEvaluation(dataIndex,dataVals,dataErrors,nDataPoints,testParameters,parametersVary,nParameters,derivativeMatrix,beta,chiSquareVal);
    ochisq=(*chiSquareVal);
    for (j=1;j<=nParameters;j++) atry[j]=testParameters[j];
  }
  for (j=0,l=1;l<=nParameters;l++) {
    if (parametersVary[l]) {
      for (j++,k=0,m=1;m<=nParameters;m++) {
        if (parametersVary[m]) {
          k++;
          covarianceMatrix[j][k]=derivativeMatrix[j][k];
        }
      }
      covarianceMatrix[j][j]=derivativeMatrix[j][j]*(1.0+(*aLambdaPar));
      //printf( "derivativeMatrix[ %i ][ %i ] = %.5f\n", j, j, derivativeMatrix[j][j] );
      oneda[j][1]=beta[j];
      if(covarianceMatrix[j][j] <= 0.0) {
        if(covarianceMatrix[j][j] == 0.0) {
          printf("*** Zero covariance diagonal element at j %d (l %d)\n",j,l);
          printf("*** Bad parameter %d\n",l);
        } else {
          printf("*** Negative covariance diagonal element at j %d (l %d)\n",j,l);
        }
      }
    }
  }
  cout << "GaussJordan" << endl;
  //LOCASMath lMath;
  //retVal = 0;
  retval = GaussJordanElimination(covarianceMatrix,mfit,oneda,1);
  for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
  if (*aLambdaPar == 0.0 ) {
    printf("PRE_COVSRT\n");
    //PrintCovarianceMatrix();
    printf("nParameters is: %i, mfit is: %i\n", nParameters, mfit );
    LOCASMath::CovarianceSorting(covarianceMatrix,nParameters,parametersVary,mfit);
    //printf("POST_COVSRT\n");
    //PrintCovarianceMatrix();
    LOCASMath::LOCASFree_Matrix(oneda,1,mfit,1,1);
    LOCASMath::LOCASFree_Vector(da,1,nParameters);
    LOCASMath::LOCASFree_Vector(beta,1,nParameters);
    LOCASMath::LOCASFree_Vector(atry,1,nParameters);
    return retval;
  }
  for (j=0,l=1;l<=nParameters;l++)
    if (parametersVary[l]) atry[l]=testParameters[l]+da[++j];
  //printf("TRY VECTOR: %8.2f %8.2f %8.2f %8.2f\n",atry[1],atry[2],atry[3],atry[4]);
  FitEvaluation(dataIndex,dataVals,dataErrors,nDataPoints,atry,parametersVary,nParameters,covarianceMatrix,da,chiSquareVal);
  //printf("mrqmin:  chiSquareVal = %f\n",*chiSquareVal);
  if (*chiSquareVal < ochisq) {
    *aLambdaPar *= 0.1;
    ochisq=(*chiSquareVal);
    for (j=0,l=1;l<=nParameters;l++) {
      if (parametersVary[l]) {
        for (j++,k=0,m=1;m<=nParameters;m++) {
          if (parametersVary[m]) {
            k++;
            derivativeMatrix[j][k]=covarianceMatrix[j][k];
          }
        }
        beta[j]=da[j];
        testParameters[l]=atry[l];
      }
    }
  } else {
    *aLambdaPar *= 10.0;
    *chiSquareVal=ochisq;
  }
  return retval;
  

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASChiSquare::DoFit( Float_t dataIndex[], Float_t dataVals[], 
                             Float_t dataErrors[], Int_t nDataPoints, 
                             Float_t testParameters[], Int_t parametersVary[], 
                             Int_t nParameters, Float_t **covarianceMatrix, 
                             Float_t **derivativeMatrix, Float_t *chiSquareVal )
{

  //Fit these data using mrqmin() repeatedly until convergence is achieved.
  
  Int_t maxiter = 1000;
  Int_t numiter = 0;
  Int_t gooditer = 0;
  Int_t retval = 0;

  Float_t oldchisq = 0;
  Float_t lamda = -1.0;
  Float_t tol = 1.0;    // Chisquared must change by tol to warrant another iteration

  *chiSquareVal = 0;

  printf("Calling Minimise for initialization...\n");
  retval = Minimise(dataIndex,dataVals,dataErrors,nDataPoints,testParameters,parametersVary,nParameters,covarianceMatrix,derivativeMatrix,chiSquareVal,&lamda);
  cout << "chiSquareVal val is: " << *chiSquareVal << endl;
  oldchisq = *chiSquareVal;
  printf("CHISQ at origin = %8.2f\n",*chiSquareVal);
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lamda = 0.01;
  cout << "Top of while loop" << endl;
  cout << "fabs(*chiSquareVal - oldchisq)" << fabs(*chiSquareVal - oldchisq) << endl;
  cout << "tol is: " << tol << endl;
  while (((fabs(*chiSquareVal - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
         && retval == 0 && lamda != 0.0) {
    oldchisq = *chiSquareVal;
    printf("Iteration %d with lambda %g...\n",numiter,lamda);
    retval = Minimise(dataIndex,dataVals,dataErrors,nDataPoints,testParameters,parametersVary,nParameters,covarianceMatrix,derivativeMatrix,chiSquareVal,&lamda );
    numiter++;
    printf("New ChiSquare = %12.2f with lambda %g \n", *chiSquareVal, lamda);
    if ( fabs( oldchisq - *chiSquareVal ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lamda = 0 and call Minimise one last time.  This attempts to
  // calculate covariance (covarianceMatrix), and curvature (derivativeMatrix) matrices. It also frees
  // up allocated memory.
  lamda = 0;
  Minimise( dataIndex, dataVals, dataErrors, nDataPoints, testParameters, parametersVary, nParameters, covarianceMatrix, derivativeMatrix, chiSquareVal, &lamda );
  return retval;


}

//////////////////////////////////////
//////////////////////////////////////

void LOCASChiSquare::PerformMinimisation()
{

    Float_t chiSquare = 0.0;
    cout << "LOCASChiSquare::PerformMinimisation: Performing fit...\n";
    cout << "IdentifyVaryingPMTAngularResponseBins" << endl;
    fModel->IdentifyVaryingPMTAngularResponseBins( fDataStore );
    cout << "IdentifyVaryingLBDistributionBins" << endl;
    fModel->IdentifyVaryingLBDistributionBins( fDataStore );
    cout << "IdentifyBaseVaryingParameters" << endl;
    fModel->GetLOCASModelParameterStore()->IdentifyBaseVaryingParameters();
    cout << "InitialisePMTAngularResponseIndex" << endl;
    fModel->GetLOCASModelParameterStore()->InitialisePMTAngularResponseIndex();

    Int_t nDataPoints = fDataStore->GetNDataPoints();
    cout << "nDataPoints" << nDataPoints << endl;

    cout << "parameters" << endl;
    Float_t* parameters = fModel->GetLOCASModelParameterStore()->GetParametersPtr();
    cout << "parametersVary" << endl;
    Int_t* parametersVary = fModel->GetLOCASModelParameterStore()->GetParametersVary();
    Int_t nParameters = fModel->GetLOCASModelParameterStore()->GetNParameters();

    cout << "covarianceMatrix" << endl;
    Float_t** covarianceMatrix = fModel->GetLOCASModelParameterStore()->GetCovarianceMatrix();

    cout << "derivativeMatrix" << endl;
    Float_t** derivativeMatrix = fModel->GetLOCASModelParameterStore()->GetDerivativeMatrix();
    
    // for ( Int_t iPar = 1; iPar <= nParameters; iPar++ ){
    //   cout << "covarianceMatrix index Varies(?): " << parametersVary[ iPar ] << " Index: " << iPar << " Value: " <<  covarianceMatrix[ iPar ][ iPar ] << endl;
    // }
    cout << "nParameters is: " << nParameters << endl;

    cout << "done." << endl;
    // cout << "Some Parameter Values..." << endl;
    // for ( Int_t iPar = 1; iPar <= nParameters; iPar++ ){
    //   if ( parametersVary[ iPar ] ){
    //     cout << " Parameter: " << iPar << " || Value: " << parameters[ iPar ] << endl;
    //   }
    // }
    // cout << " ------------- " << endl;
    Int_t val = DoFit( fDataIndex, fDataVals, 
                       fDataErrors, nDataPoints, 
                       parameters, parametersVary, 
                       nParameters, covarianceMatrix,
                       derivativeMatrix, &chiSquare );

    cout << "done." << endl;
    cout << "Some Parameter Values..." << endl;
    for ( Int_t iPar = 1; iPar <= nParameters; iPar++ ){
      if ( parametersVary[ iPar ] ){
        cout << " Parameter: " << iPar << " || Value: " << parameters[ iPar ] << endl;
      }
    }
    cout << " ------------- " << endl;

    fModel->GetLOCASModelParameterStore()->SetParametersPtr( parameters );
}
