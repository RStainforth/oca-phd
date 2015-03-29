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
///     03/2015 : RPFS - Added counters for passing data points
///
////////////////////////////////////////////////////////////////////

#include "LOCASFilter.hh"

#include <iostream>

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

  fNumberConditionPasses = 0;
  fNumberConditionFails = 0;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilter::ResetFilter()
{

  SetMaxValue( 0.0 );
  SetMinValue( 0.0 );
  
  fNumberConditionPasses = 0;
  fNumberConditionFails = 0;

  cout << "LOCAS::LOCASFilter: " << GetFilterName() << " has been reset.\n"; 

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilter::PrintFilterInformation()
{

  cout << "LOCAS::LOCASFilter: " << GetFilterName() 
       << " has limits: ( " << GetMinValue() << ", " << GetMaxValue() << " )\n"
       << "---------------------------\n"
       << "Tested " << GetNumberConditionPasses() + GetNumberConditionFails() << " data points.\n"
       << "Passed: " << GetNumberConditionPasses() << "\n"
       << "Failed: " << GetNumberConditionFails() << "\n"
       << "---------------------------\n";

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFilter::CheckCondition( Float_t val )
{

  Bool_t passCondition = false;

  // Check whether the 'val' is within the limits of the minimum and maximum values
  if ( ( val < GetMaxValue() ) && ( val > GetMinValue() ) ){ 
    fNumberConditionPasses++;
    passCondition = true; 
  }

  else{ 
    fNumberConditionFails++;
    passCondition = false; 
  }

  return passCondition;

}
