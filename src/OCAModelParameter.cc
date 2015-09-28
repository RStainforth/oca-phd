#include "OCAModelParameter.hh"

#include <string>
#include <iostream>

using namespace OCA;
using namespace std;

ClassImp( OCAModelParameter )

//////////////////////////////////////
//////////////////////////////////////

OCAModelParameter::OCAModelParameter( std::string parameterName, 
                                          Int_t index, Float_t initVal, 
                                          Float_t valLow, Float_t valHigh, 
                                          Float_t valInc, Int_t nParsInGroup,
                                          Bool_t varyBool )
{

  // Set the parameter name.
  SetParameterName( parameterName );

  // Set the index for this parameter in relation to the entire
  // set of parameters which will be used in the fit.
  SetIndex( index );

  // Set the number of other parameters in a specific group if applicable.
  // For example, the extinction lengths are all individual parameters
  // and so the value will be 1 (nParsInGroup = 1). But for the Laserball
  // distribution this could be 432 in the case of a 2D histogram with
  // (12x36) binning in (costheta, phi).
  SetNInGroup( nParsInGroup );

  // Set the increment value. 
  // Note: Currently not required by the Levenberg-marquardt algorithm 
  // which is currently used.
  SetIncrementValue( valInc );
  
  // Set the initial value for the parameter.
  SetInitialValue( initVal );

  // Set the final value to 0.0 for now. This will be updated
  // after the fit has been completed.
  SetFinalValue( 0.0 );
  
  // Set the minimum and maximum allowed values for this parameters.
  // Note: Currently not required by the Levenberg-Marquardt algorithm
  // which is currently used.
  SetMinValue( valLow );
  SetMaxValue( valHigh );

  // Set whether or not this parameter will vary in the fit.
  // TRUE: Parameter Varies. 
  // FALSE: Parameter remains fixed and uses the initial value provided
  // above for the entirety of the fit.
  SetVary( varyBool );

}

//////////////////////////////////////
//////////////////////////////////////

OCAModelParameter& OCAModelParameter::operator=( const OCAModelParameter& rhs )
{

  fParameterName = rhs.fParameterName;
  fIndex = rhs.fIndex;
  fNInGroup = rhs.fNInGroup;
  fIncrementValue = rhs.fIncrementValue;
  fInitialValue = rhs.fInitialValue;
  fFinalValue = rhs.fFinalValue;
  fMinValue = rhs.fMinValue;
  fMaxValue = rhs.fMaxValue;
  fVary = rhs.fVary;

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameter::PrintInfo()
{

  // Print all the current information pertaining to this parameter.
  cout << "Parameter Name: " << GetParameterName() << endl;
  cout << "Parameter Index: " << GetIndex() << endl;
  cout << "Parameter Initial Value: " << GetInitialValue() << endl;
  cout << "Parameter Range: [" << GetMinValue() << ", " << GetMaxValue() << "]" << endl;
  cout << "Parameter Increment Value: " << GetIncrementValue() << endl;
  if ( GetVary() ){
    cout << "Parameter varies." << endl;
  }
  else{
    cout << "Parameter is fixed to initial value." << endl;
  }
  cout << "--------------" << endl;

}
