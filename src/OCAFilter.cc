#include "OCAFilter.hh"

#include <iostream>

using namespace OCA;
using namespace std;

ClassImp( OCAFilter )

//////////////////////////////////////
//////////////////////////////////////

OCAFilter::OCAFilter( const std::string filterName, 
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

  // This isn't a Boolean type filter, so set to false.
  fBoolTypeFilter = false;

}

//////////////////////////////////////
//////////////////////////////////////

OCAFilter::OCAFilter( const std::string filterName, 
                          const Bool_t boolVal )
{

  // Set the filter name.
  SetFilterName( filterName );

  // Set the bool value for this particualr
  // filter to check against.
  SetBoolValue( boolVal );

  // Set the number of data points which have
  // passed/failed on this particular filter to zero.
  fNumberConditionPasses = 0;
  fNumberConditionFails = 0;

  // This is a Boolean type filter, so set to true.
  fBoolTypeFilter = true;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAFilter::ResetFilter()
{

  // Set the minimum and maximum values of the filter to zero.
  SetMinValue( 0.0 );
  SetMaxValue( 0.0 );

  // Set the boolean value to false
  SetBoolValue ( false );
  
  // Set the number of data points which have
  // passed/failed on this particular filter to zero.  
  fNumberConditionPasses = 0;
  fNumberConditionFails = 0;

  // Output a warning message to remind the user.
  cout << "OCA::OCAFilter: " << GetFilterName() << " has been reset.\n"; 

}

//////////////////////////////////////
//////////////////////////////////////

void OCAFilter::PrintFilterInformation()
{

  // Print all the current available information about this particualr filter.
  cout << "OCA::OCAFilter: " << GetFilterName();
  if ( fBoolTypeFilter == false ){ 
    cout << " has limits: ( " << GetMinValue() << ", " << GetMaxValue() << " )\n";
  }
  else{
    cout << " is testing for Bool (!=0 => 'true') of type: " << (Int_t)GetBoolValue() << "\n";
  }
  cout  << "---------------------------\n"
        << "Tested " << GetNumberConditionPasses() + GetNumberConditionFails() << " data points.\n"
        << "Passed: " << GetNumberConditionPasses() << "\n"
        << "Failed: " << GetNumberConditionFails() << "\n"
        << "---------------------------\n";
  
}

//////////////////////////////////////
//////////////////////////////////////

void OCAFilter::ResetConditionCounters()
{

  // Set the number of data points which have
  // passed/failed on this particular filter to zero.  
  fNumberConditionPasses = 0; 
  fNumberConditionFails = 0;
  
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t OCAFilter::CheckCondition( const Float_t val )
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

//////////////////////////////////////
//////////////////////////////////////

Bool_t OCAFilter::CheckBoolCondition( const Bool_t boolVal )
{

  // Assume the pass condition to be false to begin with.
  Bool_t passCondition = false;

  // Check whether the 'boolVal' is within the limits of 
  // the minimum and maximum values as required.

  // If it is the same to the filter boolean set the pass condition to TRUE.
  if ( boolVal == fBoolValue ){ 
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
