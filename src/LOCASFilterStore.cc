////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFilterStore.cc
///
/// CLASS: LOCAS::LOCASFilterStore
///
/// BRIEF: Simple class to store a set filters used
///        against a raw data set to produce a final
///        data set
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

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

  fStoreName = storeName;
  fTopLevelFilters.clear();
  fFitLevelFilters.clear();
  AddTopLevelFilters( fileName );
  AddFitLevelFilters( fileName );
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddTopLevelFilter( LOCASFilter filter )
{
  
  fTopLevelFilters.push_back( filter );
  fTopLevelFilterLookUp[ filter.GetFilterName() ] = (Int_t)( fTopLevelFilters.size() - 1 );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFitLevelFilter( LOCASFilter filter )
{
  
  fFitLevelFilters.push_back( filter );
  fFitLevelFilterLookUp[ filter.GetFilterName() ] = (Int_t)( fFitLevelFilters.size() - 1 );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddTopLevelFilters( const char* fileName )
{

  // Initalise the database object and set the file path
  LOCASDB lDB;
  lDB.SetFile( fileName );

  Float_t maxVal, minVal = 0.0;

  // Obtain a list of the filters to be included in the store from the card file
  std::vector< std::string > filterList = lDB.GetStringVectorField( "FITFILE", "filter_list", "top_level_filter_setup" );

  // Loop over each filter in the list and add it to the LOCASFilterStore
  for ( Int_t iStr = 0; iStr < filterList.size(); iStr++ ){

    // Get the maximum and minimum values
    maxVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_max"), "top_level_filter_setup" );
    minVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_min"), "top_level_filter_setup" );

    // Create the filter object
    LOCASFilter lFilter( filterList[ iStr ], minVal, maxVal );

    // Add the filter
    AddTopLevelFilter( lFilter );

    // Print the information about the filter to the screen
    cout << "Added Top-Level filter '" << filterList[ iStr ] << "' filter." << endl;
    cout << "Filter Range (exclusive): (" << minVal << ", " << maxVal << ")" << endl;
    cout << " ----------------- " << endl;

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFitLevelFilters( const char* fileName )
{

  // Initalise the database object and set the file path
  LOCASDB lDB;
  lDB.SetFile( fileName );

  Float_t maxVal, minVal = 0.0;

  // Obtain a list of the filters to be included in the store from the card file
  std::vector< std::string > filterList = lDB.GetStringVectorField( "FITFILE", "filter_list", "fit_level_filter_setup" );

  // Loop over each filter in the list and add it to the LOCASFilterStore
  for ( Int_t iStr = 0; iStr < filterList.size(); iStr++ ){

    // Get the maximum and minimum values
    maxVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_max"), "fit_level_filter_setup" );
    minVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_min"), "fit_level_filter_setup" );

    // Create the filter object
    LOCASFilter lFilter( filterList[ iStr ], minVal, maxVal );

    // Add the filter
    AddFitLevelFilter( lFilter );

    // Print the information about the filter to the screen
    cout << "Added Fit-Level Filter '" << filterList[ iStr ] << "' filter." << endl;
    cout << "Filter Range (exclusive): (" << minVal << ", " << maxVal << ")" << endl;
    cout << " ----------------- " << endl;

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::UpdateTopLevelFilter( const std::string filterName, const Double_t min, const Double_t max )
{

  // Obtain the look up index for the filter
  Int_t filterIndex = fTopLevelFilterLookUp[ filterName ];
  
  // Update the filter with the new minimum and maximum values
  ( fTopLevelFilters[ filterIndex ] ).SetMinValue( min );
  ( fTopLevelFilters[ filterIndex ] ).SetMaxValue( max );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::UpdateFitLevelFilter( const std::string filterName, const Double_t min, const Double_t max )
{

  // Obtain the look up index for the filter
  Int_t filterIndex = fFitLevelFilterLookUp[ filterName ];
  
  // Update the filter with the new minimum and maximum values
  ( fFitLevelFilters[ filterIndex ] ).SetMinValue( min );
  ( fFitLevelFilters[ filterIndex ] ).SetMaxValue( max );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASFilter& LOCASFilterStore::GetTopLevelFilter( const std::string filterName )
{

  return fTopLevelFilters[ fTopLevelFilterLookUp[ filterName ] ];
  
}

//////////////////////////////////////
//////////////////////////////////////

LOCASFilter& LOCASFilterStore::GetFitLevelFilter( const std::string filterName )
{

  return fFitLevelFilters[ fFitLevelFilterLookUp[ filterName ] ];
  
}


