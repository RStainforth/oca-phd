///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2dataset.cc
///
/// EXECUTABLE: locas2dataset
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "LOCASRawDataStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASDB.hh"
#include "LOCASRunReader.hh"
#include "LOCASDataFiller.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"

#include <iostream>
#include <map>

using namespace std;
using namespace LOCAS;

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  cout << "\n";
  cout << "##############################" << endl;
  cout << "######### DEBUG Start ########" << endl;
  cout << "##############################" << endl;
  cout << "\n";

  LOCASDB lDB;
  lDB.SetFile( argv[1] );

  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
  
  LOCASRunReader lReader( runIDs );
  LOCASRawDataStore lRawDataStore;

  lRawDataStore.AddRawData( lReader );
  lRawDataStore.WriteToFile();

  LOCASFilterStore lFilterStore;
  lFilterStore.AddFilters( argv[1] );

  LOCASFilterStore* lFilterStorePtr = &lFilterStore;
  LOCASRawDataStore* lRawDataStorePtr = &lRawDataStore;

  LOCASDataFiller lDataFiller;
  lDataFiller.AddData( lRawDataStorePtr, lFilterStorePtr );

  LOCASDataStore lDataStore = lDataFiller.GetData();
  lDataStore.WriteToFile();
  
  
  cout << "\n";
  cout << "############################" << endl;
  cout << "######### DEBUG End ########" << endl;
  cout << "############################" << endl;
  cout << "\n";


}
