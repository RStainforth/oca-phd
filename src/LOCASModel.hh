////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModel.hh
///
/// CLASS: LOCAS::LOCASModel
///
/// BRIEF: The base class from which all models used in LOCAS
///        as part of a fitting routine inherit from
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASModel_
#define _LOCASModel_

#include "LOCASModelParameterStore.hh"
#include "LOCASDataPoint.hh"

#include <iostream>

#include <string>

namespace LOCAS{

  class LOCASModel : public TObject
  {
  public:
    
    // The constructors
    LOCASModel(){ };
    LOCASModel( const char* fileName );

    // The destructor - delete the parameters in the model
    ~LOCASModel(){ if ( fParameters != NULL ) { delete[] fParameters; } }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Model prediction for the occupancy ratio for a datapoint
    virtual Float_t ModelPrediction( const LOCASDataPoint& dataPoint ){ return 0.0; }

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetModelName() const { return fModelName; }

    // Return the store of parameters used in the model
    LOCASModelParameterStore GetModelParameterStore() const { return fModelParameterStore; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetModelName( const std::string name ){ fModelName = name; }

    void SetModelParameterStore( const LOCASModelParameterStore& locasParams );

  

  protected:

    std::string fModelName;                          // The model name

    LOCASModelParameterStore fModelParameterStore;   // The colleciton of parameters used in the model
    Double_t* fParameters;                           // The parameter values stored in an array

    ClassDef( LOCASModel, 1 );

  };

}

#endif
