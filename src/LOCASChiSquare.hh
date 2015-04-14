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
  
  class LOCASChiSquare : public TObject
  {
  public:

    // The constructor and destructor for the LOCASChiSquare object
    LOCASChiSquare();
    ~LOCASChiSquare(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Evaluate the chi-square for a single LOCASDataPoint object.
    Float_t EvaluateChiSquare( LOCASDataPoint& dPoint );

    // Evaluate the global chi-square for all the LOCASDataPoint objects
    // that this LOCASChiSquare object has access to, as provided by the
    // the 'fDataStore' LOCASDataStore object pointer private variable.
    Float_t EvaluateGlobalChiSquare();

    // Set the pointer to the LOCASOpticsModel which is used for the
    // model predictions when calculating the chi-square for a data point.
    void SetPointerToModel( LOCASOpticsModel* locasModel ){ fModel = locasModel; }

    // Set the pointer to the LOCASDataStore object which is used
    // as the collection of datapoints when calculating the chi-square.
    void SetPointerToData( LOCASDataStore* locasData ){ fDataStore = locasData; }

    // This evaluates the solution vector and passes through to 
    // FitEvaluateModel the matrix of derivatives to be calculated.
    void FitEvaluation( Float_t testParameters[], Int_t parametersVary[], 
                        Int_t nParameters, Float_t **derivativeMatrix, 
                        Float_t betaVec[], Float_t *chiSquareVal );

    // Evaluates the model for a trial set of parameters 'testParameters'
    // and then calculates the derivatives 'dDataValDParameters' with 
    // respect to each variable parameter relevant to the provided 
    // LOCASDataPoint
    void FitEvaluateModel( LOCASDataPoint& dPoint, Float_t testParameters[],
                           Float_t *modelVal, Float_t dDataValDParameters[], 
                           Int_t nParameters );

    // Perform a single minimisation for the given value
    Int_t Minimise( Float_t testParameters[], Int_t parametersVary[], 
                    Int_t nParameters, Float_t **covarianceMatrix, 
                    Float_t **derivativeMatrix, Float_t *chiSquareVal, 
                    Float_t *aLambdaPar );

    void DoFit( Float_t testParameters[], Int_t parametersVary[], 
                Int_t nParameters, Float_t **covarianceMatrix, 
                Float_t **derivativeMatrix, Float_t *chiSquareVal );
    
    void PerformMinimisation();
    
  private:

    LOCASDataStore* fDataStore;   
    LOCASOpticsModel* fModel;

    ClassDef( LOCASChiSquare, 0 );

  };

}

#endif
