#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASRunReader.hh"

#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <sstream>
#include <string>


using namespace LOCAS;
using namespace std;

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader()
{

  // Create a new TChain object with the name
  // "LOCASRunT".
  fLOCASRunT = new TChain( "LOCASRunT" );
  
  // Create a new LOCASRun object and set the branch
  // address to the memory allocated to it on the
  // TChain.
  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of LOCASRun object to zero to begin with.
  // None have been added as part of the constructor.
  fNLOCASRuns = 0;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader( vector< Int_t >& runIDs,
                                const string dataSet )
{

  // Create a stringstream object and string to help
  // create the file path for the various LOCASRun files
  // associated with each run ID in the 'runIDs' vector.
  stringstream myStream;
  string runIDStr = "";

  // Create an instance of the LOCASDB object to 
  // obtain the full system path directory to 
  // the LOCASRun folder.
  LOCASDB lDB;
  string locasRunDir = lDB.GetLOCASRunDir( dataSet );
  string fExt = "_LOCASRun.root";
  
  string filename = "";

  // Create a new TChain to store all the different
  // LOCASRun .root files.
  fLOCASRunT = new TChain( "LOCASRunT" );

  // Create a new LOCASRun object.
  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  // For each run ID in the 'runIDs' vector find the associated
  // LOCASRun file and add it to the reader.
  for ( Int_t iRun = 0; iRun < (Int_t)runIDs.size(); iRun++ ){

    myStream << runIDs[ iRun ];
    myStream >> runIDStr;

    // The full system path to the LOCASRun file.
    filename = ( locasRunDir + runIDStr + fExt );

    // Add the file.
    Add( filename.c_str() );

    // Clear the stringstream and string variables
    // in preparation for the next iteration of the loop.
    myStream.clear();
    runIDStr = "";

  }

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of LOCASRuns to the number which have
  // been added to the TChain.
  fNLOCASRuns = fLOCASRunT->GetEntries();

  // Set the pointer to the current LOCASRun object to the
  // first entry in the TChain.
  fLOCASRun = GetLOCASRun( runIDs[ 0 ] );

  // Set the current PMT pointer to the first PMT in the first LOCASRun entry.
  map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );
  

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader( Int_t runID )
{

  // Create a LOCASRunReader using just a single LOCASRun file.
  // This is the same as the above constructor but just for a single
  // LOCASRun file.

  // The stringstream and string variables for creating
  // the full system path to the LOCASRun file.
  stringstream myStream;
  myStream << runID;

  string runIDStr = "";
  myStream >> runIDStr;

  // Get the full system path to the LOCASRun directory.
  LOCASDB lDB;
  string locasRunDir = lDB.GetLOCASRunDir();
  string fExt = "_LOCASRun.root";
  
  // Create the full path name.
  string filename = ( locasRunDir + runIDStr + fExt );  

  // Create a TChain object to store the LOCASRun .root file on.
  fLOCASRunT = new TChain( "LOCASRunT" );

  // Create a pointer to the LOCASRun object.
  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  // Add the LOCASRun .root file to the LOCASRunReader.
  Add( filename.c_str() );

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of LOCASRuns currenty on this reader
  // to the number of entries on the TChain to begin with.
  fNLOCASRuns = fLOCASRunT->GetEntries();

  // Set the pointer to the LOCASRun object.
  fLOCASRun = GetLOCASRun( runID );

  // Set the pointer to the current PMT to be the first LOCASPMT
  // object on the first (and only in this case) LOCASRun object
  // on the TChain.
  map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::LOCASRunReader( const char* filename )
{

  // Add the LOCASRun file directory just using the full
  // system path of the LOCASRun file.
  fLOCASRunT = new TChain( "LOCASRunT" );

  fLOCASRun = new LOCASRun();
  fLOCASRunT->SetBranchAddress( "LOCASRun", &fLOCASRun );

  fNext = 0;
  fNLOCASRuns = fLOCASRunT->GetEntries();

  Add( filename );

  map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRunReader::~LOCASRunReader()
{

  // Delete the memory associated with the TChain 'fLOCASRunT'
  // and the LOCASRun object pointer 'fLOCASRun'.
  delete fLOCASRunT;
  delete fLOCASRun;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRunReader::Add( Int_t runID )
{

  // Create the full system path of the LOCASRun file.
  stringstream myStream;
  myStream << runID;

  string runIDStr = "";
  myStream >> runIDStr;

  LOCASDB lDB;
  string locasRunDir = lDB.GetLOCASRunDir();
  string fExt = "_LOCASRun.root";
  
  string filename = ( locasRunDir + runIDStr + fExt );

  // Add the file to the reader.
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

  // Get the current total number of LOCASRuns
  // stored on the reader.
  Long64_t total = fNLOCASRuns;

  // Add the file.
  fLOCASRunT->Add( filename );

  // Get the entry and add the run ID on the newly
  // added file to add to the list.
  fLOCASRunT->GetEntry( total );
  fListOfRunIDs.push_back( fLOCASRun->GetRunID() );

  // Recalculate the total number of LOCASRuns held
  // by the reader (this should just increment by one in this case).
  fNLOCASRuns = fLOCASRunT->GetEntries();

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASRunReader::CheckForLOCASRun( Int_t runID )
{

  // Assume the run doesn't exist to begin with.
  Bool_t existRunID = false;

  // Check through each of the run IDs and see if there are
  // any matching files.
  for ( Int_t iID = 0; iID < (Int_t)fListOfRunIDs.size(); iID++ ){
    
    // If there is a matching file, set the boolean
    // variable 'existRunID' to true and break out of the loop.
    if ( fListOfRunIDs[ iID ] == runID ){ existRunID = true; break; }
    else { continue; }

  }

  // Return the boolean value.
  return existRunID;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun* LOCASRunReader::GetLOCASRun( Int_t runID )
{

  // Search through all of the LOCASRun entries currently
  // held on the TChain 'fLOCASRunT'.
  for ( Int_t iRun = 0; iRun < fNLOCASRuns; iRun++ ){
    
    fLOCASRunT->GetEntry( iRun );

    // If the run ID of one of the entries matches that
    // of the specified run ID 'runID', then break out of 
    // the loop. Otherwise continue to check.
    if ( fLOCASRun->GetRunID() == runID ){ break; }
    else { continue; }

  }

  // Set the current PMT pointer to that of the first LOCASPMT object
  // on the current (pointed to) LOCASRun object.
  map< Int_t, LOCASPMT >::iterator iPMT = fLOCASRun->GetLOCASPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

  // Return the pointer to the LOCASRun object.
  return fLOCASRun;

}
