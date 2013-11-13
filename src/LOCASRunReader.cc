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
///     0X/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <TFile.h>
#include <TObject.h>

#include <iostream>

#include "LOCASRun.hh"
#include "LOCASRunReader.hh"

using namespace LOCAS;
using namespace std;

LOCASRunReader::LOCASRunReader( const char* filename )
{

  fLOCASRunT = new TChain( "LOCASRunT" );

  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  fNext = 0;
  fNLOCASRuns = fLOCASRunT->GetEntries();

  Add( filename );

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

  for ( Int_t iID = 0; iID < fListOfRunIDs.size(); iID++ ){
    
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

  return fLOCASRun;
}


    
    




