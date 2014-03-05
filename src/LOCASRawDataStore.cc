////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRawDataStore.cc
///
/// CLASS: LOCAS::LOCASRawDataStore
///
/// BRIEF: Simple class to store a set of raw data points
///        ready for processing
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASRawDataStore.hh"
#include "LOCASRunReader.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"

#include "TMath.h"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include <map>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASRawDataStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataStore::LOCASRawDataStore( LOCASRunReader& runReader, std::string storeName )
{
  
  fStoreName = storeName;
  fRawDataPoints.clear();
  AddRawData( runReader );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataStore::LOCASRawDataStore( std::string storeName )
{
  
  fStoreName = storeName;
  fRawDataPoints.clear();
  
}

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataStore& LOCASRawDataStore::operator+=( LOCASRawDataStore& rhs )
{

  fRawDataPoints.insert( fRawDataPoints.end(), rhs.fRawDataPoints.begin(), rhs.fRawDataPoints.end() );
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataStore LOCASRawDataStore::operator+( LOCASRawDataStore& rhs )
{
  
  return (*this)+=rhs;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRawDataStore::AddRawData( LOCASRunReader& runReader )
{

  cout << "LOCASRawDataStore: Adding run data from: " << runReader.GetNLOCASRuns() << " LOCASRuns..." << endl;
  
  // Initalise pointers to the LOCASRun and LOCASPMT objects
  LOCASRun* lRun;
  LOCASPMT* lPMT;
  std::map< Int_t, LOCASPMT >::iterator iPMT;

  // Loop over the runs and the associated pmts and convert them to raw data points
  for ( Int_t iRun = 0; iRun < runReader.GetNLOCASRuns(); iRun++ ){
    lRun = runReader.GetRunEntry( iRun );
    
    for ( iPMT = lRun->GetLOCASPMTIterBegin(); iPMT != lRun->GetLOCASPMTIterEnd(); iPMT++ ){
      lPMT = &( lRun->GetPMT( iPMT->first ) );
      LOCASRawDataPoint dPoint( lPMT, lRun );
      AddRawDataPoint( dPoint );
    }

    cout << "Added Run: " << lRun->GetRunID() << endl;
    cout << " ----------------- " << endl;
  }
  


}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRawDataStore::WriteToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );
  // Create the Run Tree
  TTree* runTree = new TTree( fileName, fileName );

  // Declare a new branch pointing to the data stored in the lRun object
  runTree->Branch( "LOCASRawDataStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

}
