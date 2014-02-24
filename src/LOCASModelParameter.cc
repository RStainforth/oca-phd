////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModelParameter.cc
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

#include "LOCASModelParameter.hh"
#include <string>

#include <iostream>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASModelParameter )

//////////////////////////////////////
//////////////////////////////////////

LOCASModelParameter::LOCASModelParameter( std::string parameterName, 
                                          Int_t index, Float_t initVal, 
                                          Float_t valLow, Float_t valHigh, 
                                          Float_t valInc, Int_t nParsInGroup )
{

  SetParameterName( parameterName );

  SetIndex( index );

  SetNInGroup( nParsInGroup );

  SetIncrementValue( valInc );
  
  SetInitialValue( initVal );
  SetFinalValue( 0.0 );
  
  SetMinValue( valLow );
  SetMaxValue( valHigh );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameter::PrintInfo()
{

  cout << "Parameter Name: " << GetParameterName() << endl;
  cout << "Parameter Index: " << GetIndex() << endl;
  cout << "Parameter Initial Value: " << GetInitialValue() << endl;
  cout << "Parameter Range: [" << GetMinValue() << ", " << GetMaxValue() << "]" << endl;
  cout << "Parameter Increment Value: " << GetIncrementValue() << endl;
  cout << " -------------- " << endl;

}
