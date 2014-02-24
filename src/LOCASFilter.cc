////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFilter.cc
///
/// CLASS: LOCAS::LOCASFilter
///
/// BRIEF: A simple class to test whether certain
///        values fall within a range of values
///        A collection of LOCASFilters is used
///        to introduce the cut-flow to a set of
///        LOCASRawDataPoint objects
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASFilter.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASFilter )

//////////////////////////////////////
//////////////////////////////////////

LOCASFilter::LOCASFilter( std::string filterName, Float_t valLow, Float_t valHigh )
{

  SetFilterName( filterName );
  SetMinValue( valLow );
  SetMaxValue( valHigh );

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFilter::CheckCondition( Float_t val ) const
{

  Bool_t passCondition = false;

  // Check whether the 'val' is within the limits of the minimum and maximum values
  if ( ( val < GetMaxValue() ) && ( val > GetMinValue() ) ){ passCondition = true; }
  else{ passCondition = false; }

  return passCondition;

}
