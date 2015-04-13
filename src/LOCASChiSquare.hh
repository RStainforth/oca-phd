////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASChiSquare.hh
///
/// CLASS: LOCAS::LOCASChiSquare
///
/// BRIEF: A simple class to compute the global chi-square
///        of an entire data set given a LOCASModel object,
///        a LOCASModelParameterStore object and a LOCASDataStore
///        object
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2014 : RPFS - First Revision, new file.
///
/// DETAIL: *Write at the end!*
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASChiSquare_
#define _LOCASChiSquare_

#include "LOCASOpticsModel.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASDataPoint.hh"
#include "LOCASDataStore.hh"
#include "LOCASMath.hh"

using namespace std;

namespace LOCAS{
  
  class LOCASChiSquare : public TObject, LOCASMath
  {
  public:

    // The constructor and destructor for the LOCASChiSquare object
    LOCASChiSquare();
    ~LOCASChiSquare();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Evaluate the chi-square for a single LOCASDataPoint object.
    Float_t EvaluateChiSquare( LOCASDataPoint& dPoint );

    // Evaluate the global chi-square for all the LOCASDataPoint objects
    // that this LOCASChiSquare object has access to, as provided by the
    // the 'fDataStore' LOCASDataStore object pointer private variable.
    Float_t EvaluateGlobalChiSquare();

    // 
    void InitialiseArrays();

    void SetPointerToModel( LOCASOpticsModel* locasModel ){ fModel = locasModel; }
    void SetPointerToData( LOCASDataStore* locasData ){ fDataStore = locasData; }

    void FitEvaluation( Float_t testParameters[], Int_t parametersVary[], 
                        Int_t nParameters, Float_t **derivativeMatrix, 
                        Float_t betaVec[], Float_t *chiSquareVal );

    void FitEvaluateModel( LOCASDataPoint& dPoint, Float_t testParameters[],
                           Float_t *modelVal, Float_t dDataValDParameters[], 
                           Int_t nParameters );

    Int_t Minimise( Float_t testParameters[], Int_t parametersVary[], 
                    Int_t nParameters, Float_t **covarianceMatrix, 
                   Float_t **derivativeMatrix, Float_t *chiSquareVal, 
                   Float_t *aLambdaPar );

    void DoFit( Float_t testParameters[], Int_t parametersVary[], 
                Int_t nParameters, Float_t **covarianceMatrix, 
                Float_t **derivativeMatrix, Float_t *chiSquareVal );
    
    void PerformMinimisation();
    // Int_t GaussJordanElimination( Float_t** lhsMatrix, Int_t nParameters,
    //                               Float_t** rhsMatrix, Int_t mVectors );
    
  private:

    LOCASDataStore* fDataStore;   
    LOCASOpticsModel* fModel;

    // The arrays used by the Levenburg-Marquadt (Mrq) algorithm to find the parameters
    Float_t* fDataIndex;                                         // [fNDataPointsInFit+1] Index into the PMTs to be used in the fit
    Float_t* fDataVals;                                         // [fNDataPointsInFit+1] Index into the PMTs (from above) corresponding OccRatio
    Float_t* fDataErrors;                                     // [fNDataPointsInFit+1] Error on each PMT occupancy (statistical)

    ClassDef( LOCASChiSquare, 0 );

  };

}

#endif
