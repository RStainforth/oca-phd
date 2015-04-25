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
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: The LOCASChiSquare object is used to compute the
///         individual and global chisquare from a set of
///         data points (in the form of LOCASDataPoint objects)
///         stored in a LOCASDataStore object. The model prediction
///         used to compare the data values is a LOCASOpticsModel
///         object. The 'data' is the occupancy ratio variable.
/// 
///         Using 'LOCASChiSquare::SetPointerToModel' and
///         'LOCASChiSquare::SetPointerToData', the LOCASChiSquare
///         object has access to both the data and model required
///         to compute the chisquare.
///
///         The chisquare is minimised using the
///         Levenberg-Marquardt algorithm. This is done by calling
///         'LOCASChiSquare::DoOpticsFit'. This then calls
///         'LOCASChiSquare::PerformMinimisation' which calls
///         'LOCASChiSquare::Minimise' many times to minimise the
///         global chisquare. The 'LOCASChiSquare::FitEvaluation'
///         and 'LOCASChiSquare::FitEvaluateModel' are called 
///         at each iteration to evaulate the current trial
///         solution for the parameters in the optics model.
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

    // This evaluates the solution vector and passes through to 
    // FitEvaluateModel the matrix of derivatives to be calculated.
    void FitEvaluation( Float_t testParameters[], Int_t parametersVary[], 
                        Int_t nParameters, Float_t **derivativeMatrix, 
                        Float_t betaVec[], Float_t *chiSquareVal );

    // Evaluates the model for a trial set of parameters 'testParameters'
    // and then calculates the derivatives 'dDataValDParameters' with 
    // respect to each variable parameter relevant to the provided 
    // LOCASDataPoint.
    void FitEvaluateModel( LOCASDataPoint& dPoint, Float_t testParameters[],
                           Float_t *modelVal, Float_t dDataValDParameters[], 
                           Int_t nParameters );

    // Perform a single minimisation for the given value of 'aLambdaPar'.
    Int_t Minimise( Float_t testParameters[], Int_t parametersVary[], 
                    Int_t nParameters, Float_t **covarianceMatrix, 
                    Float_t **derivativeMatrix, Float_t *chiSquareVal, 
                    Float_t *aLambdaPar );

    // Performs many minimisations given the starting parameters
    // and the array to the indices of those parameters which will vary in the
    // the fit.
    void PerformMinimisation( Float_t testParameters[], Int_t parametersVary[], 
                              Int_t nParameters, Float_t **covarianceMatrix, 
                              Float_t **derivativeMatrix, Float_t *chiSquareVal );
    // Performs the optics fit.
    void PerformOpticsFit();

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the pointer to the LOCASOpticsModel which is used for the
    // model predictions when calculating the chi-square for a data point.
    void SetPointerToModel( LOCASOpticsModel* locasModel ){ fModel = locasModel; }
    
    // Set the pointer to the LOCASDataStore object which is used
    // as the collection of datapoints when calculating the chi-square.
    void SetPointerToData( LOCASDataStore* locasData ){ fDataStore = locasData; }
    
  private:

    LOCASDataStore* fDataStore;       // Pointer to the LOCASDataStore object.   
    LOCASOpticsModel* fModel;         // Pointer to the LOCASOpticsModel object. 
 
    ClassDef( LOCASChiSquare, 0 );

  };

}

#endif
