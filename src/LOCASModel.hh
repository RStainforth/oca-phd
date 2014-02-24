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
    LOCASModel(){ };
    LOCASModel( const char* fileName );
    ~LOCASModel(){ if ( fParameters != NULL ) { delete[] fParameters; } }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    virtual Float_t ModelPrediction( const LOCASDataPoint& dataPoint ){ return 0.0; }

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetModelName() const { return fModelName; }

    LOCASModelParameterStore GetModelParameterStore() const { return fModelParameterStore; }
    //Double_t* GetParameters() const { return fParameters; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetModelName( const std::string name ){ fModelName = name; }

    void SetModelParameterStore( const LOCASModelParameterStore& locasParams );
    //void SetParameters( const Double_t* params ) { *fParameters = *params; }

  

  protected:

    std::string fModelName;

    LOCASModelParameterStore fModelParameterStore;
    Double_t* fParameters;

    ClassDef( LOCASModel, 1 );

  };

}

#endif
