////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataStore.cc
///
/// CLASS: LOCAS::LOCASDataStore
///
/// BRIEF: Simple class to store a set of data points
///        ready for use in the fitting process
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASDataStore.hh"
#include "LOCASDataPoint.hh"

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

  fStoreName = storeName;
  fDataPoints.clear();
 
}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore& LOCASDataStore::operator+=( LOCASDataStore& rhs )
{

  fDataPoints.insert( fDataPoints.end(), rhs.fDataPoints.begin(), rhs.fDataPoints.end() );
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore LOCASDataStore::operator+( LOCASDataStore& rhs )
{
  
  return (*this)+=rhs;
  
}
//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore& LOCASDataStore::operator=( const LOCASDataStore& rhs )
{

  fStoreName = rhs.fStoreName;
  fDataPoints = rhs.fDataPoints;

  return *this;
}


//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddDataPoint( LOCASDataPoint dataPoint )
{

  fDataPoints.push_back( dataPoint );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddDataPoint( LOCASPMT& lPMT )
{

  LOCASDataPoint dp( lPMT );
  fDataPoints.push_back( dp );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddData( LOCASRunReader& lRuns )
{

  // LOCASPMT iterator used in below loop
  std::map< Int_t, LOCASPMT >::iterator iPMT;
  
  // Loop through all the LOCASRun objects
  for ( Int_t iRun = 0; iRun < lRuns.GetNLOCASRuns(); iRun++ ){

    // Loop through all the LOCASPMTs
    for ( iPMT = lRuns.GetRunEntry( iRun )->GetLOCASPMTIterBegin();
          iPMT != lRuns.GetRunEntry( iRun )->GetLOCASPMTIterEnd();
          iPMT ++ ){

      // Add the LOCASPMT object to the LOCASDataStore object.
      // This inherently converts the useful information on the LOCASPMT
      // object to a LOCASDataPoint object.
      AddDataPoint( iPMT->second ); 
     
    }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::WriteToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );
  // Create the Run Tree
  TTree* runTree = new TTree( fileName, fileName );

  // Declare a new branch pointing to the data store object
  runTree->Branch( "LOCASDataStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

}
