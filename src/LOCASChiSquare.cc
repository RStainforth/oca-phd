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
  Float_t *betaVec2 = LOCASVector( 1, nParameters );
  Float_t **derivativeMatrix2 = LOCASMatrix( 1, nParameters, 1, nParameters );

  // Initialise the vector of derivatives of the data points with respect to the
  // parameters
  dDataValDParameters = LOCASVector( 1, nParameters );

  // Check which of the current parameters are set to vary.
  for ( jVar = 1; jVar <= nParameters; jVar++ ){
    if ( parametersVary[ jVar ] ){ mFit++; }
    dDataValDParameters[jVar] = 0.0;
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

  std::vector< LOCASDataPoint >::iterator iDP;
  for ( iDP = fDataStore->GetLOCASDataPointsIterBegin(); 
        iDP != fDataStore->GetLOCASDataPointsIterEnd(); 
        iDP++ ) {
    
    // Evaluate the model for this data point and calculate the 
    // parameters and the derivative of the this point with respect
    // to each of these parameters
    FitEvaluateModel( *iDP, testParameters, &yMod, dDataValDParameters, nParameters );
    
    // Compute the 1 / sigma^2 value for this particular data point
    dataError2 = 1.0 / ( iDP->GetOccupancyRatioErr() * iDP->GetOccupancyRatioErr() );

    // And compute the difference between the model prediction and the data value
    deltaDataVal = iDP->GetOccupancyRatio() - yMod;
      
    fModel->GetLOCASModelParameterStore()->IdentifyVaryingParameters();
    Int_t nVariablePar = fModel->GetLOCASModelParameterStore()->GetNCurrentVariableParameters();
    Int_t* variableParameterIndex = fModel->GetLOCASModelParameterStore()->GetVariableParameterIndex();
    Int_t* variableParameterMap = fModel->GetLOCASModelParameterStore()->GetVariableParameterMap();

    for (lVar=1; lVar<=nVariablePar; lVar++) {	
      weightVal = dDataValDParameters[variableParameterIndex[lVar]] * dataError2;
      for (mVar=1; mVar<=lVar; mVar++) {
    	jVar = variableParameterIndex[lVar];
    	kVar = variableParameterIndex[mVar];
    	if (kVar<=jVar) derivativeMatrix[variableParameterMap[jVar]][variableParameterMap[kVar]] += weightVal * dDataValDParameters[kVar];
      }
      betaVec[variableParameterMap[variableParameterIndex[lVar]]] += deltaDataVal * weightVal;
    }
    
    chiSquareEntry = deltaDataVal*deltaDataVal*dataError2;
    *chiSquareVal += chiSquareEntry;
    //if (iVar%1==2000) printf("Mod|2000 Value: %d %f %f\n",iVar,dataIndex[iVar],chiSquareEntry);
    //if (fChiArray!=NULL && iVar>=0 && iVar<fNElements) fChiArray[iVar] = chiSquareEntry;
    //if (fResArray!=NULL && iVar>=0 && iVar<fNElements) fResArray[iVar] = deltaDataVal;
  }

  for (jVar=2;jVar<=mFit;jVar++)
    for (kVar=1;kVar<jVar;kVar++) {
      derivativeMatrix[kVar][jVar]=derivativeMatrix[jVar][kVar];
    }
  
  
  LOCASFree_Vector(dDataValDParameters,1,nParameters);
  
  LOCASFree_Matrix(derivativeMatrix2,1,nParameters,1,nParameters);
  LOCASFree_Vector(betaVec2,1,nParameters);

}

//////////////////////////////////////
//////////////////////////////////////

void  LOCASChiSquare::FitEvaluateModel( LOCASDataPoint& dPoint, Float_t testParameters[],
                                        Float_t *modelVal, Float_t dDataValDParameters[], 
                                        Int_t nParameters )
{
 
  // Save parameters and use the ones just passed
  Float_t *tempParameterSave = fModel->GetLOCASModelParameterStore()->GetParametersPtr(); 
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( testParameters );

  // Calculation of the model value
  *modelVal = fModel->ModelPrediction( dPoint, dDataValDParameters );

  // Restore saved parameters
  fModel->GetLOCASModelParameterStore()->SetParametersPtr( tempParameterSave );
  
}
