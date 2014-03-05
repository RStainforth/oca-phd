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

  fScintPar = -10.0;
  fAVPar = -10.0;
  fWaterPar = -10.0;

  fScintRSPar = -10.0;
  fAVRSPar = -10.0;
  fWaterRSPar = -10.0;

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

void LOCASDataStore::AddDataPoint( LOCASDataPoint dataPoint )
{

  fDataPoints.push_back( dataPoint );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddDataPoint( LOCASRawDataPoint dataPoint )
{

  LOCASDataPoint dp( dataPoint );
  fDataPoints.push_back( dp );

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
