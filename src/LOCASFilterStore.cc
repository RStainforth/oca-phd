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
  fFilters.clear();
  AddFilters( fileName );
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFilter( LOCASFilter filter )
{
  
  fFilters.push_back( filter );
  fFilterLookUp[ filter.GetFilterName() ] = (Int_t)( fFilters.size() - 1 );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFilters( const char* fileName )
{

  // Initalise the database object and set the file path
  LOCASDB lDB;
  lDB.SetFile( fileName );

  Float_t maxVal, minVal = 0.0;

  // Obtain a list of the filters to be included in the store from the card file
  std::vector< std::string > filterList = lDB.GetStringVectorField( "FITFILE", "filter_list", "filter_setup" );

  // Loop over each filter in the list and add it to the LOCASFilterStore
  for ( Int_t iStr = 0; iStr < filterList.size(); iStr++ ){

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

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::UpdateFilter( const std::string filterName, const Double_t min, const Double_t max )
{

  // Obtain the look up index for the filter
  Int_t filterIndex = fFilterLookUp[ filterName ];
  
  // Update the filter with the new minimum and maximum values
  ( fFilters[ filterIndex ] ).SetMinValue( min );
  ( fFilters[ filterIndex ] ).SetMaxValue( max );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASFilter& LOCASFilterStore::GetFilter( const std::string filterName )
{

  return fFilters[ fFilterLookUp[ filterName ] ];
  
}


