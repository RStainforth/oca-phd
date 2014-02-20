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

#include "LOCASModelParameter.hh"
#include <string>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASModelParameter )

//////////////////////////////////////
//////////////////////////////////////

LOCASModelParameter::LOCASModelParameter( std::string parameterName, Float_t initVal, Float_t valLow, Float_t valHigh )
{

  SetParameterName( parameterName );
  
  SetInitialValue( initVal );
  
  SetMinValue( valLow );
  SetMaxValue( valHigh );

}
