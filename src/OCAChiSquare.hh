////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAChiSquare.hh
///
/// CLASS: OCA::OCAChiSquare
///
/// BRIEF: A simple class to compute the global chi-square
///        of an entire data set given a OCAModel object,
///        a OCAModelParameterStore object and a OCAPMTStore
///        object
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: The OCAChiSquare object is used to compute the
///         individual and global chisquare from a set of
///         data points (in the form of OCAPMT objects)
///         stored in a OCAPMTStore object. The model prediction
///         used to compare the data values is a OCAOpticsModel
///         object. The 'data' is the occupancy ratio variable.
/// 
///         Using 'OCAChiSquare::SetPointerToModel' and
///         'OCAChiSquare::SetPointerToData', the OCAChiSquare
///         object has access to both the data and model required
///         to compute the chisquare.
///
///         The chisquare is minimised using the
///         Levenberg-Marquardt algorithm. This is done by calling
///         'OCAChiSquare::DoOpticsFit'. This then calls
///         'OCAChiSquare::PerformMinimisation' which calls
///         'OCAChiSquare::Minimise' many times to minimise the
///         global chisquare. The 'OCAChiSquare::FitEvaluation'
///         and 'OCAChiSquare::FitEvaluateModel' are called 
///         at each iteration to evaulate the current trial
///         solution for the parameters in the optics model.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAChiSquare_
#define _OCAChiSquare_

#include "OCAOpticsModel.hh"
#include "OCAModelParameterStore.hh"
#include "OCAPMT.hh"
#include "OCAPMTStore.hh"
#include "OCAMath.hh"

using namespace std;

namespace OCA{
  
  class OCAChiSquare : public TObject
  {
  public:

    // The constructor and destructor for the OCAChiSquare object
    OCAChiSquare();
    ~OCAChiSquare(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Evaluate the chi-square for a single data-point (OCAPMT object).
    Float_t EvaluateChiSquare( OCAPMT& dPoint );

    // Evaluate the global chi-square for all the OCAPMT objects
    // that this OCAChiSquare object has access to, as provided by the
    // the 'fDataStore' OCAPMTStore object pointer private variable.
    Float_t EvaluateGlobalChiSquare();

    // This evaluates the solution vector and passes through to 
    // FitEvaluateModel the matrix of derivatives to be calculated.
    void FitEvaluation( Float_t testParameters[], Int_t parametersVary[], 
                        Int_t nParameters, Float_t **derivativeMatrix, 
                        Float_t betaVec[], Float_t *chiSquareVal );

    // Evaluates the model for a trial set of parameters 'testParameters'
    // and then calculates the derivatives 'dDataValDParameters' with 
    // respect to each variable parameter relevant to the provided 
    // OCAPMT.
    void FitEvaluateModel( OCAPMT& dPoint, Float_t testParameters[],
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
    void PerformOpticsFit( const int pass );

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the pointer to the OCAOpticsModel which is used for the
    // model predictions when calculating the chi-square for a data point.
    void SetPointerToModel( OCAOpticsModel* ocaModel ){ fModel = ocaModel; }
    
    // Set the pointer to the OCAPMTStore object which is used
    // as the collection of datapoints when calculating the chi-square.
    void SetPointerToData( OCAPMTStore* ocaData ){ fDataStore = ocaData; }
    
  private:

    OCAPMTStore* fDataStore;        // Pointer to the OCAPMTStore object.   
    OCAOpticsModel* fModel;         // Pointer to the OCAOpticsModel object. 
 
    ClassDef( OCAChiSquare, 0 );

  };

}

#endif
