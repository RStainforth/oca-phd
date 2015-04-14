#include "LOCASFilter.hh"

#include <iostream>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASFilter )

//////////////////////////////////////
//////////////////////////////////////

LOCASFilter::LOCASFilter( const std::string filterName, 
                          const Float_t valLow, 
                          const Float_t valHigh )
{

  // Set the filter name.
  SetFilterName( filterName );

  // Set the minimum and maximum values for this particualr
  // filter to check against.
  SetMinValue( valLow );
  SetMaxValue( valHigh );

  // Set the number of data points which have
  // passed/failed on this particular filter to zero.
  fNumberConditionPasses = 0;
  fNumberConditionFails = 0;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilter::ResetFilter()
{

  // Set the minimum and maximum values of the filter to zero.
  SetMinValue( 0.0 );
  SetMaxValue( 0.0 );
  
  // Set the number of data points which have
  // passed/failed on this particular filter to zero.  
  fNumberConditionPasses = 0;
  fNumberConditionFails = 0;

  // Output a warning message to remind the user.
  cout << "LOCAS::LOCASFilter: " << GetFilterName() << " has been reset.\n"; 

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilter::PrintFilterInformation()
{

  // Print all the current available information about this particualr filter.
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

void LOCASFilter::ResetConditionCounters()
{

  // Set the number of data points which have
  // passed/failed on this particular filter to zero.  
  fNumberConditionPasses = 0; 
  fNumberConditionFails = 0;
  
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFilter::CheckCondition( const Float_t val )
{

  // Assume the pass condition to be false to begin with.
  Bool_t passCondition = false;

  // Check whether the 'val' is within the limits of 
  // the minimum and maximum values as required.

  // If it is within the bounds, set the pass condition to TRUE.
  if ( ( val < GetMaxValue() ) && ( val > GetMinValue() ) ){ 
    fNumberConditionPasses++;
    passCondition = true; 
  }

  // If it is not within the bounds, set the pass condition to FALSE.
  else{ 
    fNumberConditionFails++;
    passCondition = false; 
  }

  // Return the pass condition boolean variable.
  return passCondition;

}
