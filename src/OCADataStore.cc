#include "OCADataStore.hh"
#include "OCADataPoint.hh"
#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <fstream>

using namespace OCA;
using namespace std;

ClassImp( OCADataStore )

//////////////////////////////////////
//////////////////////////////////////

OCADataStore::OCADataStore( std::string storeName )
{

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "datastore/" + storeName + ".root";
  ifstream tmpFile( filePath.c_str() );
  if ( tmpFile ){
    
    // Create a new TFile object to open the OCADataStore.
    TFile* dataFile = TFile::Open( filePath.c_str(), "READ" );
    TTree* fileTree = new TTree();
    fileTree = (TTree*)dataFile->Get( ( storeName + ".root;1" ).c_str() );
    OCADataStore* lStore = new OCADataStore();
    fileTree->SetBranchAddress( "OCADataStore", &lStore );
    fileTree->GetEntry( 0 );
    
    vector< OCADataPoint >::iterator iDP;
    vector< OCADataPoint >::iterator iDPBegin = lStore->GetOCADataPointsIterBegin();
    vector< OCADataPoint >::iterator iDPEnd = lStore->GetOCADataPointsIterEnd();
    
    for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
      
      OCADataPoint dp( *iDP );
      AddDataPoint( dp );
      
    }

    fStoreName = lStore->GetStoreName();

    *this = *lStore;

  }

  else{
    // Set the store name and ensure the vector of OCADataPoints
    // is empty.
    fStoreName = storeName;
    fDataPoints.clear();
  }
  
}

//////////////////////////////////////
//////////////////////////////////////

OCADataStore& OCADataStore::operator+=( OCADataStore& rhs )
{

  // Insert all the data points from the right hand side 'rhs' into this
  // vector of data points starting at the end.
  fDataPoints.insert( fDataPoints.end(), rhs.fDataPoints.begin(), rhs.fDataPoints.end() );
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

OCADataStore OCADataStore::operator+( OCADataStore& rhs )
{
  
  // Using the self-addition operator above, add the OCADataPoints
  // from the right hand side 'rhs' into 'this' vector of OCADataPoints.
  return ( *this ) += rhs;
  
}
//////////////////////////////////////
//////////////////////////////////////

OCADataStore& OCADataStore::operator=( const OCADataStore& rhs )
{

  // Attribute the properties of the right hand side OCADataStore, 'rhs'
  // to the properties of this OCADataStore. i.e. equal 'this' to 'rhs'.
  fStoreName = rhs.fStoreName;
  fDataPoints = rhs.fDataPoints;
  fStoreFilePath = rhs.fStoreFilePath;

  return *this;

}


//////////////////////////////////////
//////////////////////////////////////

void OCADataStore::AddDataPoint( OCADataPoint dataPoint )
{

  // Add a data point to the list of current data points held within
  // this OCADataStore.
  fDataPoints.push_back( dataPoint );

}

//////////////////////////////////////
//////////////////////////////////////

void OCADataStore::AddDataPoint( const OCAPMT& lPMT )
{

  // Use the LCOASDataPoint constructor to create a new OCADataPoint
  // from the provided OCAPMT object and add it to the list of current
  // data points held within this OCADataStore.
  OCADataPoint dp( lPMT );
  fDataPoints.push_back( dp );

}

//////////////////////////////////////
//////////////////////////////////////

void OCADataStore::AddData( OCARunReader& lRuns )
{

  // OCAPMT iterator used in below loop
  std::map< Int_t, OCAPMT >::iterator iPMT;

  // Create a new instance of a OCARun object on the stack.
  // This will use the values obtained by OCARun::GetRunEntry below
  // for quick accessing of the data held on files.
  OCARun* lRun = new OCARun();
  
  // Loop through all the OCARun objects
  for ( Int_t iRun = 0; iRun < lRuns.GetNOCARuns(); iRun++ ){

    lRun = lRuns.GetRunEntry( iRun );
    // Loop through all the OCAPMTs
    for ( iPMT = lRun->GetOCAPMTIterBegin();
          iPMT != lRun->GetOCAPMTIterEnd();
          iPMT ++ ){

      // Add the OCAPMT object to the OCADataStore object.
      // This inherently converts the useful information on the OCAPMT
      // object to a OCADataPoint object.
      OCADataPoint dp( iPMT->second );

      // Set here the run index value of the OCADataPoint.
      dp.SetRunIndex( iRun );

      // Add the data point to the current list of data points
      // held in this LOCAsDataStore.
      AddDataPoint( dp );
 
    }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCADataStore::WriteToFile( const string fileName )
{

  // Check that a filename has been provided before continuing
  if ( fileName == NULL ){
    cout << "OCADataStore::WriteToFile: No filename specified, aborting." << endl; return;
  }

  OCADB lDB;
  string outPutDir = lDB.GetOutputDir();
  string filePath = outPutDir + "datastore/" + fileName;

  // Create a new TFile object
  TFile* file = TFile::Open( filePath.c_str(), "RECREATE" );

  // Create the store Tree
  TTree* storeTree = new TTree( fileName.c_str(), fileName.c_str() );

  // Declare a new branch pointing to the data store object
  storeTree->Branch( "OCADataStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  storeTree->Fill();
  storeTree->Write();

  // Close the file
  file->Close();
  delete file;

}
