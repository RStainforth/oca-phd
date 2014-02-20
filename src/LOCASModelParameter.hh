////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModelParameter.hh
///
/// CLASS: LOCAS::LOCASModelParameter
///
/// BRIEF: A simple class to hold the information about
///        a specific parameter included in the LOCASModel
///        object
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASModelParameter_
#define _LOCASModelParameter_

#include "TObject.h"

#include <string>

namespace LOCAS{

  class LOCASModelParameter : public TObject
  {
  public:

    LOCASModelParameter(){ }
    LOCASModelParameter( std::string parameterName, Float_t initVal, Float_t valLow, Float_t valHigh );
    ~LOCASModelParameter(){ }

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetParameterName() const { return fParameterName; }

    Float_t GetInitialValue() const { return fInitialValue; }

    Float_t GetMaxValue() const { return fMaxValue; }
    Float_t GetMinValue() const { return fMinValue; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetParameterName( const std::string& name ){ fParameterName = name; }

    void SetInitialValue( const Float_t initVal ){ fInitialValue = initVal; }

    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }
    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }

  private:

    std::string fParameterName;

    Float_t fInitialValue;

    Float_t fMinValue;
    Float_t fMaxValue;

    ClassDef( LOCASModelParameter, 1 );

  };

}

#endif
