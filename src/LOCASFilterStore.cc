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

LOCASFilterStore::LOCASFilterStore( std::string storeName )
{

  fStoreName = storeName;
  fFilters.clear();
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFilterStore::AddFilters( const char* fileName )
{

  LOCASDB lDB;
  lDB.SetFile( fileName );

  Float_t maxVal, minVal = 0.0;
  std::vector< std::string > filterList = lDB.GetStringVectorField( "FITFILE", "filter_list", "filter_setup" );
  for ( Int_t iStr = 0; iStr < filterList.size(); iStr++ ){

    maxVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_max"), "filter_setup" );
    minVal = lDB.GetDoubleField( "FITFILE", (std::string)( filterList[ iStr ] + "_min"), "filter_setup" );

    LOCASFilter lFilter( filterList[ iStr ], minVal, maxVal );
    AddFilter( lFilter );

    cout << "Added '" << filterList[ iStr ] << "' filter." << endl;
    cout << "Filter Range: " << maxVal << " <--> " << minVal << endl;
    cout << " ----------------- " << endl;

  }

}
