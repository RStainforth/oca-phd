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
    cout << "Added '" << filterList[ iStr ] << "' filter." << endl;
    cout << "Filter Range (exclusive): (" << minVal << ", " << maxVal << ")" << endl;
    cout << " ----------------- " << endl;

  }

}
