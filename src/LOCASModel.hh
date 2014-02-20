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

#include "LOCASModelParameter.hh"

#ifndef _LOCASModel_
#define _LOCASModel_

namespace LOCAS{

  class LOCASModel : public TObject
  {
  public:
    LOCASModel(){ };
    ~LOCASModel(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    virtual void InitialiseParameters();
    virtual void AllocateParameters();

    virtual Float_t ModelPrediction( const LOCASDataPoint& dataPoint );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetModelName() const { return fModelName; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetModelName( const std::string name ){ fModelName = name; }

  private:

    std::string fModelName;

    std::vector< LOCASModelParameter > fParameters;

  };

}

#endif
