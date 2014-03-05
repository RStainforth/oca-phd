////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataStoreStore.cc
///
/// CLASS: LOCAS::LOCASDataStoreStore
///
/// BRIEF: Simple class to store a store of a set of data points
///        ready for use in the fitting process. This entire class is
///        mainly used for debugging
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASDataStoreStore.hh"
#include "LOCASDataStore.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <sstream>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataStoreStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStoreStore::LOCASDataStoreStore( std::string storeName )
{

  fStoreName = storeName;
  fDataStores.clear();
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStoreStore::AddDataStore( LOCASDataStore dataStore )
{

  fDataStores.push_back( dataStore );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStoreStore::WriteToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );

  // Loop over all the stores, name them differently, and write them to individual TTrees on the file
  std::vector< LOCASDataStore >::iterator iStore;
  std::string tmpString = "locas_data_";
  std::string tmp = "";
  std::stringstream myStream;

  Int_t nI = 0;
  for ( iStore = fDataStores.begin(); iStore != fDataStores.end(); iStore++ ){
    
    // Get the identifier for this particular store
    myStream << tmpString;
    myStream << nI;
    myStream >> tmp;

    // Declare a new TTree and branch pointing to the data store
    TTree* runTree = new TTree( tmp.c_str(), "LOCAS PMT Data" );
    runTree->Branch( "LOCASDataStoreStore", (*iStore).ClassName(), &(*iStore), 32000, 99 );
    file->cd();

    // Fill the tree and write the store to the file
    runTree->Fill();
    runTree->Write();

    // Clear the string stream to use in the next iteration of the loop
    myStream.clear();
    tmp = "";
    nI++;
  }

  // Close the file
  file->Close();
  delete file;

}

