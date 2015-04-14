#include "LOCASDataStore.hh"
#include "LOCASDataPoint.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore::LOCASDataStore( std::string storeName )
{

  // Set the store name and ensure the vector of LOCASDataPoints
  // is empty.
  fStoreName = storeName;
  fDataPoints.clear();
 
}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore& LOCASDataStore::operator+=( LOCASDataStore& rhs )
{

  // Insert all the data points from the right hand side 'rhs' into this
  // vector of data points starting at the end.
  fDataPoints.insert( fDataPoints.end(), rhs.fDataPoints.begin(), rhs.fDataPoints.end() );
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore LOCASDataStore::operator+( LOCASDataStore& rhs )
{
  
  // Using the self-addition operator above, add the LOCASDataPoints
  // from the right hand side 'rhs' into 'this' vector of LOCASDataPoints.
  return ( *this ) += rhs;
  
}
//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore& LOCASDataStore::operator=( const LOCASDataStore& rhs )
{

  // Attribute the properties of the right hand side LOCASDataStore, 'rhs'
  // to the properties of this LOCASDataStore. i.e. equal 'this' to 'rhs'.
  fStoreName = rhs.fStoreName;
  fDataPoints = rhs.fDataPoints;

  return *this;

}


//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddDataPoint( LOCASDataPoint dataPoint )
{

  // Add a data point to the list of current data points held within
  // this LOCASDataStore.
  fDataPoints.push_back( dataPoint );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddDataPoint( const LOCASPMT& lPMT )
{

  // Use the LCOASDataPoint constructor to create a new LOCASDataPoint
  // from the provided LOCASPMT object and add it to the list of current
  // data points held within this LOCASDataStore.
  LOCASDataPoint dp( lPMT );
  fDataPoints.push_back( dp );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddData( LOCASRunReader& lRuns )
{

  // LOCASPMT iterator used in below loop
  std::map< Int_t, LOCASPMT >::iterator iPMT;

  // Create a new instance of a LOCASRun object on the stack.
  // This will use the values obtained by LOCASRun::GetRunEntry below
  // for quick accessing of the data held on files.
  LOCASRun* lRun = new LOCASRun();
  
  // Loop through all the LOCASRun objects
  for ( Int_t iRun = 0; iRun < lRuns.GetNLOCASRuns(); iRun++ ){

    lRun = lRuns.GetRunEntry( iRun );
    // Loop through all the LOCASPMTs
    for ( iPMT = lRun->GetLOCASPMTIterBegin();
          iPMT != lRun->GetLOCASPMTIterEnd();
          iPMT ++ ){

      // Add the LOCASPMT object to the LOCASDataStore object.
      // This inherently converts the useful information on the LOCASPMT
      // object to a LOCASDataPoint object.
      LOCASDataPoint dp( iPMT->second );

      // Set here the run index value of the LOCASDataPoint.
      dp.SetRunIndex( iRun );

      // Add the data point to the current list of data points
      // held in this LOCAsDataStore.
      AddDataPoint( dp );
 
    }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::WriteToFile( const char* fileName )
{

  // Check that a filename has been provided before continuing
  if ( fileName == NULL ){
    cout << "LOCASDataStore::WriteToFile: No filename specified, aborting." << endl; return;
  }

  // Create a new TFile object
  TFile* file = TFile::Open( fileName, "RECREATE" );

  // Create the store Tree
  TTree* storeTree = new TTree( fileName, fileName );

  // Declare a new branch pointing to the data store object
  storeTree->Branch( "LOCASDataStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  storeTree->Fill();
  storeTree->Write();

  // Close the file
  file->Close();
  delete file;

}
