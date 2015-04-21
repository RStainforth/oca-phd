#include "LOCASFilterStore.hh"
#include "LOCASFilter.hh"

#include "LOCASDB.hh"

#include <string>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASFilterStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASFilterStore::LOCASFilterStore( const char* fileName, std::string storeName )
{

  // Set the store name
  fStoreName = storeName;

  // Ensure the vector which holds all of the filters
  // is empty to begin with.
  fFilters.clear();

  // Add all the filters as defined in the 'fit-file' to the above
  // private store 'fFilters'.
  AddFilters( fileName );
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFilter( LOCASFilter filter )
{
  
  // Add a single filter to the current vector of pre-existing filters.
  fFilters.push_back( filter );

  // Set the lookup map accordingly so we can find 
  // this filter in the future by name.
  fFilterLookUp[ filter.GetFilterName() ] = (Int_t)( fFilters.size() - 1 );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFilters( const char* fileName )
{

  // Initalise the database object and set the file path
  LOCASDB lDB;
  lDB.SetFile( fileName );

  // Initialise the minimm and maximum values of the filter which
  // is about to be added.
  Float_t maxVal = 0.0; 
  Float_t minVal = 0.0;

  // Obtain a list of the filters to be included in 
  // the store from the 'fit-file'.
  std::vector< std::string > filterList = lDB.GetStringVectorField( "FITFILE", "filter_list", "filter_setup" );

  // Loop over each filter in the list and add it to the LOCASFilterStore
  for ( Int_t iStr = 0; iStr < (Int_t)filterList.size(); iStr++ ){

    // Get the maximum and minimum values
    maxVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_max"), "filter_setup" );
    minVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_min"), "filter_setup" );

    // Create the filter object
    LOCASFilter lFilter( filterList[ iStr ], minVal, maxVal );

    // Add the filter
    AddFilter( lFilter );

    // Print the information about the filter to the screen
    cout << "Added filter '" << filterList[ iStr ] << "' filter." << endl;
    cout << "Filter Range (exclusive): (" << minVal << ", " << maxVal << ")" << endl;
    cout << " ----------------- " << endl;

  }

  // Obtain a list of the boolean filters to be included in 
  // the store from the 'fit-file'.
  std::vector< std::string > boolFilterList = lDB.GetStringVectorField( "FITFILE", "filter_list", "bool_filter_setup" );

  Bool_t boolVal = false;

  // Loop over each filter in the list and add it to the LOCASFilterStore
  for ( Int_t iStr = 0; iStr < (Int_t)boolFilterList.size(); iStr++ ){

    // Get the maximum and minimum values
    boolVal = lDB.GetBoolField( "FITFILE", (std::string)( boolFilterList[ iStr ] ), "bool_filter_setup" );

    // Create the filter object
    LOCASFilter lFilter( boolFilterList[ iStr ], boolVal );

    // Add the filter
    AddFilter( lFilter );

    // Print the information about the filter to the screen
    cout << "Added boolean filter '" << boolFilterList[ iStr ] << "' filter." << endl;
    cout << "Boolean filter tests for bool (!=0 => true) of type: (" << (Int_t)boolVal << ")" << endl;
    cout << " ----------------- " << endl;

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::UpdateFilter( const std::string filterName, 
                                     const Double_t min, 
                                     const Double_t max )
{

  // Obtain the look up index for the filter
  Int_t filterIndex = fFilterLookUp[ filterName ];
  
  // Update the filter with the new minimum and maximum values
  ( fFilters[ filterIndex ] ).SetMinValue( min );
  ( fFilters[ filterIndex ] ).SetMaxValue( max );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::PrintFilterCutInformation()
{

  // Loop over each filter currently stored by this object
  // and print information about each one.
  std::vector< LOCASFilter >::iterator iF;

  for ( iF = GetLOCASFiltersIterBegin();
        iF != GetLOCASFiltersIterEnd();
        iF++ ){

    iF->PrintFilterInformation();

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::ResetFilterConditionCounters()
{

  // Loop over each filter currently stored by this object
  // and reset the condition counters for each one.
  std::vector< LOCASFilter >::iterator iF;

  for ( iF = GetLOCASFiltersIterBegin();
        iF != GetLOCASFiltersIterEnd();
        iF++ ){

    iF->ResetConditionCounters();

  }

}

//////////////////////////////////////
//////////////////////////////////////

LOCASFilter& LOCASFilterStore::GetFilter( const std::string filterName )
{

  // Return a filter by name.
  return fFilters[ fFilterLookUp[ filterName ] ];
  
}
