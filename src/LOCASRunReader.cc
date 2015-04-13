///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRunReader.cc
///
/// CLASS: LOCAS::LOCASRunReader
///
/// BRIEF: Reader class for the LOCASRun Objects
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <TFile.h>
#include <TObject.h>

#include <iostream>
#include <sstream>
#include <string>

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASRunReader.hh"

#include "TMath.h"
#include "TMinuit.h"

using namespace LOCAS;
using namespace std;

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader()
{

  fLOCASRunT = new TChain( "LOCASRunT" );
  
  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  fNext = 0;
  fNLOCASRuns = fLOCASRunT->GetEntries();

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader( std::vector< Int_t >& runIDs )
{

  stringstream myStream;
  string runIDStr = "";

  LOCASDB lDB;
  string locasRunDir = lDB.GetLOCASRunDir();
  string fExt = "_LOCASRun.root";
  
  string filename = "";

  fLOCASRunT = new TChain( "LOCASRunT" );

  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  for ( Int_t iRun = 0; iRun < (Int_t)runIDs.size(); iRun++ ){

    myStream << runIDs[ iRun ];
    myStream >> runIDStr;
    filename = ( locasRunDir + runIDStr + fExt );
    Add( filename.c_str() );

    myStream.clear();
    runIDStr = "";

  }

  
  fNext = 0;
  fNLOCASRuns = fLOCASRunT->GetEntries();

  fLOCASRun = GetLOCASRun( runIDs[ 0 ] );
  std::map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );
  

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader( Int_t runID )
{
  stringstream myStream;
  myStream << runID;

  string runIDStr = "";
  myStream >> runIDStr;

  LOCASDB lDB;
  string locasRunDir = lDB.GetLOCASRunDir();
  string fExt = "_LOCASRun.root";
  
  string filename = ( locasRunDir + runIDStr + fExt );  

  fLOCASRunT = new TChain( "LOCASRunT" );

  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  Add( filename.c_str() );

  fNext = 0;
  fNLOCASRuns = fLOCASRunT->GetEntries();

  fLOCASRun = GetLOCASRun( runID );
  std::map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader( const char* filename )
{

  fLOCASRunT = new TChain( "LOCASRunT" );

  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  fNext = 0;
  fNLOCASRuns = fLOCASRunT->GetEntries();

  Add( filename );

  std::map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::~LOCASRunReader()
{

  delete fLOCASRunT;
  delete fLOCASRun;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRunReader::Add( Int_t runID )
{

  stringstream myStream;
  myStream << runID;

  string runIDStr = "";
  myStream >> runIDStr;

  LOCASDB lDB;
  string locasRunDir = lDB.GetLOCASRunDir();
  string fExt = "_LOCASRun.root";
  
  string filename = ( locasRunDir + runIDStr + fExt );

  Long64_t total = fNLOCASRuns;

  fLOCASRunT->Add( filename.c_str() );
  fLOCASRunT->GetEntry( total );
  fListOfRunIDs.push_back( fLOCASRun->GetRunID() );

  fNLOCASRuns = fLOCASRunT->GetEntries();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRunReader::Add( const char* filename )
{

  Long64_t total = fNLOCASRuns;

  fLOCASRunT->Add( filename );
  fLOCASRunT->GetEntry( total );
  fListOfRunIDs.push_back( fLOCASRun->GetRunID() );

  fNLOCASRuns = fLOCASRunT->GetEntries();

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASRunReader::CheckForLOCASRun( Int_t runID )
{

  Bool_t existRunID = false;

  for ( Int_t iID = 0; iID < (Int_t)fListOfRunIDs.size(); iID++ ){
    
    if ( fListOfRunIDs[ iID ] == runID ){ existRunID = true; break; }
    else { continue; }

  }

  return existRunID;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun* LOCASRunReader::GetLOCASRun( Int_t runID )
{

  for ( Int_t iRun = 0; iRun < fNLOCASRuns; iRun++ ){
    
    fLOCASRunT->GetEntry( iRun );
    if ( fLOCASRun->GetRunID() == runID ){ break; }
    else { continue; }

  }

  std::map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

  return fLOCASRun;
}




