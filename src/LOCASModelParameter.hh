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

    // The constructors
    LOCASModelParameter(){ }
    LOCASModelParameter( std::string parameterName, 
                         Int_t index, Float_t initVal, 
                         Float_t valLow, Float_t valHigh, 
                         Float_t valInc, Int_t nParsInGroup,
                         Bool_t varyBool );

    // The destructor - nothing to delete
    ~LOCASModelParameter(){ };

    // Print all the information about the parameter
    void PrintInfo();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Return the name of the parameter
    std::string GetParameterName() const { return fParameterName; }

    // Return the parameter index for this parameter
    Int_t GetIndex() const { return fIndex; }

    // If the parameter is part of a set of related parameters i.e. Laserball distribution
    // or angular response distribution, return how many parameters are in that set
    Int_t GetNInGroup() const { return fNInGroup; }
    
    // return the increment value of the parameter to be used in a fitting processing step
    Float_t GetIncrementValue() const { return fIncrementValue; }

    // Get the inital and final (fitted/minimised) value of the parameter
    Float_t GetInitialValue() const { return fInitialValue; }
    Float_t GetFinalValue() const { return fFinalValue; }

    // Get the maximum and minimum allowed values of the parameter
    Float_t GetMaxValue() const { return fMaxValue; }
    Float_t GetMinValue() const { return fMinValue; }

    Bool_t GetVary() const { return fVary; }

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

    void SetVary( const Bool_t boolVal ){ fVary = boolVal; }

  private:

    std::string fParameterName;         // The parameter name

    Int_t fIndex;                       // The index of the parameter in the model parameter array

    Int_t fNInGroup;                    // The number of associated parameters (i.e. if this was part of a distribution)

    Float_t fIncrementValue;            // Increment value used by minimiser

    Float_t fInitialValue;              // Initial value used by minimiser
    Float_t fFinalValue;                // Final fitted value of the parameter

    Float_t fMinValue;                  // Minimum allowed value of parameter to be used by minimiser
    Float_t fMaxValue;                  // Maximum allowed value of parameter to be used by minimiser

    Bool_t fVary;                       // Set whether this parameter varies or not

    ClassDef( LOCASModelParameter, 1 );

  };

}

#endif
