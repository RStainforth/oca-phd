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
    LOCASModelParameter( std::string parameterName, 
                         Int_t index, Float_t initVal, 
                         Float_t valLow, Float_t valHigh, 
                         Float_t valInc, Int_t nParsInGroup );
    ~LOCASModelParameter(){ };

    void PrintInfo();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetParameterName() const { return fParameterName; }

    Int_t GetIndex() const { return fIndex; }

    Int_t GetNInGroup() const { return fNInGroup; }
    
    Float_t GetIncrementValue() const { return fIncrementValue; }

    Float_t GetInitialValue() const { return fInitialValue; }
    Float_t GetFinalValue() const { return fFinalValue; }

    Float_t GetMaxValue() const { return fMaxValue; }
    Float_t GetMinValue() const { return fMinValue; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetParameterName( const std::string& name ){ fParameterName = name; }

    void SetIndex( const Int_t index ){ fIndex = index; }

    void SetNInGroup( const Int_t n ){ fNInGroup = n; }

    void SetIncrementValue( const Float_t increVal  ){ fIncrementValue = increVal; }

    void SetInitialValue( const Float_t initVal ){ fInitialValue = initVal; }
    void SetFinalValue( const Float_t finalVal ){ fFinalValue = finalVal; }

    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }
    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }

  private:

    std::string fParameterName;

    Int_t fIndex;

    Int_t fNInGroup;

    Float_t fIncrementValue;

    Float_t fInitialValue;
    Float_t fFinalValue;

    Float_t fMinValue;
    Float_t fMaxValue;

    ClassDef( LOCASModelParameter, 1 );

  };

}

#endif
