////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAModelParameter.hh
///
/// CLASS: OCA::OCAModelParameter
///
/// BRIEF: A simple class to hold the information about
///        a specific parameter included in the OCAOpticsModel
///        object.
///                  
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: A single parameter object for a parameter used
///         in the fit. This stores information specific to that
///         parameter. It also includes, if applicable, the number
///         of parameters that this parameter may be a part of
///         if it is a component of a larger set. In this case,
///         see the OCAModelParameter::GetNInGroup() description.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAModelParameter_
#define _OCAModelParameter_

#include "TObject.h"

#include <string>

using namespace std;

namespace OCA{

  class OCAModelParameter : public TObject
  {
  public:

    // The constructors and destructors for the OCAModelParameter object.
    OCAModelParameter(){ }
    OCAModelParameter( string parameterName, 
                       Int_t index, Float_t initVal, 
                       Float_t valLow, Float_t valHigh, 
                       Float_t valInc, Int_t nParsInGroup,
                       Bool_t varyBool );
    ~OCAModelParameter(){ };

    // The equality operator for OCAModelParameter types.
    OCAModelParameter& operator=( const OCAModelParameter& rhs );

    // Print all the information about the parameter.
    void PrintInfo();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of the parameter.
    string GetParameterName() const { return fParameterName; }

    // Get the parameter index for this parameter.
    Int_t GetIndex() const { return fIndex; }

    // If the parameter is part of a set of related parameters
    // get the number of said parameters.
    // i.e. Laserball distribution or PMT angular response
    // return how many parameters are in that set.
    // For example, for a laserball distribution binned
    // in a 2D histogram with (cos-theta, phi) = (12,36)
    // this would return 432 (12 x 36).
    Int_t GetNInGroup() const { return fNInGroup; }
    
    // Get the increment value of the parameter to be used 
    // in a fitting processing step.
    // Note: This currently isn't used in the Levenberg-Marquardt
    // alogorithm, but could in principle be used by something like
    // minuit.
    Float_t GetIncrementValue() const { return fIncrementValue; }

    // Get the inital  value of the parameter (pre-fit).
    Float_t GetInitialValue() const { return fInitialValue; }

    // Get the final (fitted/minimised) value of the parameter (post-fit).
    Float_t GetFinalValue() const { return fFinalValue; }

    // Get the error on the final parameter value
    Float_t GetError(){ return fError; }

    // Get the minimum allowed value of the parameter.
    Float_t GetMinValue() const { return fMinValue; }

    // Get the maximum allowed value of the parameter.
    Float_t GetMaxValue() const { return fMaxValue; }

    // Get whether or not this parameter was allowed to vary
    // in the fit or not. 
    // TRUE: Allowed to vary. 
    // FALSE: Fixed to initial vlaue.
    Bool_t GetVary() const { return fVary; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of the parameter.
    void SetParameterName( const string& name ){ fParameterName = name; }

    // Set the parameter index for this parameter.
    void SetIndex( const Int_t index ){ fIndex = index; }

    // If the parameter is part of a set of related parameters 
    // set the number of said parameters.
    // i.e. Laserball distribution or PMT angular response
    // return how many parameters are in that set.
    // For example, for a laserball distribution binned
    // in a 2D histogram with (cos-theta, phi) = (12,36)
    // this would return 432 (12 x 36).
    void SetNInGroup( const Int_t n ){ fNInGroup = n; }

    // Set the increment value of the parameter to be used 
    // in a fitting processing step.
    // Note: This currently isn't used in the Levenberg-Marquardt
    // alogorithm, but could in principle be used by something like
    // minuit.
    void SetIncrementValue( const Float_t increVal  ){ fIncrementValue = increVal; }

    // Set the inital  value of the parameter (pre-fit).
    void SetInitialValue( const Float_t initVal ){ fInitialValue = initVal; }

    // Set the final (fitted/minimised) value of the parameter (post-fit).
    void SetFinalValue( const Float_t finalVal ){ fFinalValue = finalVal; }

    // Set the error on the final parameter value
    void SetError( const Float_t errVal ){ fError = errVal; }

    // Set the minimum allowed value of the parameter.
    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }
    
    // Set the maximum allowed value of the parameter.
    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }

    // Set whether or not this parameter is allowed to vary
    // in the fit or not. 
    // TRUE: Allowed to vary. 
    // FALSE: Fixed to initial vlaue.
    void SetVary( const Bool_t boolVal ){ fVary = boolVal; }

  private:

    string fParameterName;              // The parameter name

    Int_t fIndex;                       // The index of the parameter in the model parameter array

    Int_t fNInGroup;                    // The number of associated parameters (i.e. if this was part of a distribution)

    Float_t fIncrementValue;            // Increment value used by minimiser

    Float_t fInitialValue;              // Initial value used by minimiser
    Float_t fFinalValue;                // Final fitted value of the parameter

    Float_t fError;                     // The error on the parameter value

    Float_t fMinValue;                  // Minimum allowed value of parameter to be used by minimiser
    Float_t fMaxValue;                  // Maximum allowed value of parameter to be used by minimiser

    Bool_t fVary;                       // Whether this parameter varies or not

    ClassDef( OCAModelParameter, 1 );

  };

}

#endif
