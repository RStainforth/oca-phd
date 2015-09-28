#include "OCARun.hh"
#include "OCADB.hh"
#include "OCARunReader.hh"

#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <sstream>
#include <string>


using namespace OCA;
using namespace std;

//////////////////////////////////////
//////////////////////////////////////

OCARunReader::OCARunReader()
{


  fBranchName = "OCARun";
  // Create a new TChain object with the name
  // "OCARunT".
  fOCARunT = new TChain( "OCARunT" );
  
  // Create a new OCARun object and set the branch
  // address to the memory allocated to it on the
  // TChain.
  fOCARun = new OCARun();
  fOCARunT->SetBranchAddress( fBranchName.c_str(), &fOCARun );

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of OCARun object to zero to begin with.
  // None have been added as part of the constructor.
  fNOCARuns = 0;

}

//////////////////////////////////////
//////////////////////////////////////

OCARunReader::OCARunReader( vector< Int_t >& runIDs,
                            const string dataSet )
{

  fBranchName = "OCARun";
  // Create a stringstream object and string to help
  // create the file path for the various OCARun files
  // associated with each run ID in the 'runIDs' vector.
  stringstream myStream;
  string runIDStr = "";

  // Create an instance of the OCADB object to 
  // obtain the full system path directory to 
  // the OCARun folder.
  OCADB lDB;
  string ocaRunDir = lDB.GetOCARunDir( dataSet );
  string fExt = "_OCARun.root";
  
  string filename = "";

  // Create a new TChain to store all the different
  // OCARun .root files.
  fOCARunT = new TChain( "OCARunT" );

  // Create a new OCARun object.
  fOCARun = new OCARun();
  fOCARunT->SetBranchAddress( fBranchName.c_str(), &fOCARun );

  // For each run ID in the 'runIDs' vector find the associated
  // OCARun file and add it to the reader.
  for ( Int_t iRun = 0; iRun < (Int_t)runIDs.size(); iRun++ ){

    myStream << runIDs[ iRun ];
    myStream >> runIDStr;

    // The full system path to the OCARun file.
    filename = ( ocaRunDir + runIDStr + fExt );

    // Add the file.
    Add( filename.c_str() );

    // Clear the stringstream and string variables
    // in preparation for the next iteration of the loop.
    myStream.clear();
    runIDStr = "";

  }

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of OCARuns to the number which have
  // been added to the TChain.
  fNOCARuns = fOCARunT->GetEntries();

  // Set the pointer to the current OCARun object to the
  // first entry in the TChain.
  fOCARun = GetOCARun( runIDs[ 0 ] );

  // Set the current PMT pointer to the first PMT in the first OCARun entry.
  map< Int_t, OCAPMT >::iterator iPMT = fOCARun->GetOCAPMTIterBegin();
  fCurrentPMT = &( iPMT->second );
  

}

//////////////////////////////////////
//////////////////////////////////////

OCARunReader::OCARunReader( Int_t runID )
{

  // Create a OCARunReader using just a single OCARun file.
  // This is the same as the above constructor but just for a single
  // OCARun file.

  // The stringstream and string variables for creating
  // the full system path to the OCARun file.
  stringstream myStream;
  myStream << runID;

  string runIDStr = "";
  myStream >> runIDStr;

  // Get the full system path to the OCARun directory.
  OCADB lDB;
  string ocaRunDir = lDB.GetOCARunDir();
  string fExt = "_OCARun.root";
  
  // Create the full path name.
  string filename = ( ocaRunDir + runIDStr + fExt );  

  // Create a TChain object to store the OCARun .root file on.
  fOCARunT = new TChain( "OCARunT" );

  // Create a pointer to the OCARun object.
  fOCARun = new OCARun();
  fOCARunT->SetBranchAddress( fBranchName.c_str(), &fOCARun );

  // Add the OCARun .root file to the OCARunReader.
  Add( filename.c_str() );

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of OCARuns currenty on this reader
  // to the number of entries on the TChain to begin with.
  fNOCARuns = fOCARunT->GetEntries();

  // Set the pointer to the OCARun object.
  fOCARun = GetOCARun( runID );

  // Set the pointer to the current PMT to be the first OCAPMT
  // object on the first (and only in this case) OCARun object
  // on the TChain.
  map< Int_t, OCAPMT >::iterator iPMT = fOCARun->GetOCAPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

}

//////////////////////////////////////
//////////////////////////////////////

OCARunReader::OCARunReader( const char* filename )
{

  // Add the OCARun file directory just using the full
  // system path of the OCARun file.
  fOCARunT = new TChain( "OCARunT" );

  fOCARun = new OCARun();
  fOCARunT->SetBranchAddress( fBranchName.c_str(), &fOCARun );

  fNext = 0;
  fNOCARuns = fOCARunT->GetEntries();

  Add( filename );

  map< Int_t, OCAPMT >::iterator iPMT = fOCARun->GetOCAPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

}

//////////////////////////////////////
//////////////////////////////////////

OCARunReader::~OCARunReader()
{

  // Delete the memory associated with the TChain 'fOCARunT'
  // and the OCARun object pointer 'fOCARun'.
  delete fOCARunT;
  delete fOCARun;

}

//////////////////////////////////////
//////////////////////////////////////

void OCARunReader::Add( Int_t runID )
{

  // Create the full system path of the OCARun file.
  stringstream myStream;
  myStream << runID;

  string runIDStr = "";
  myStream >> runIDStr;

  OCADB lDB;
  string ocaRunDir = lDB.GetOCARunDir();
  string fExt = "_OCARun.root";
  
  string filename = ( ocaRunDir + runIDStr + fExt );

  // Add the file to the reader.
  Long64_t total = fNOCARuns;

  fOCARunT->Add( filename.c_str() );
  fOCARunT->GetEntry( total );
  fListOfRunIDs.push_back( fOCARun->GetRunID() );

  fNOCARuns = fOCARunT->GetEntries();

}

//////////////////////////////////////
//////////////////////////////////////

void OCARunReader::Add( vector< Int_t >& runIDs,
                        const string dataSet )
{

  // Create a stringstream object and string to help
  // create the file path for the various OCARun files
  // associated with each run ID in the 'runIDs' vector.
  stringstream myStream;
  string runIDStr = "";

  // Create an instance of the OCADB object to 
  // obtain the full system path directory to 
  // the OCARun folder.
  OCADB lDB;
  string ocaRunDir = lDB.GetOCARunDir( dataSet );
  string fExt = "_OCARun.root";
  
  string filename = "";

  fOCARunT->SetBranchAddress( fBranchName.c_str(), &fOCARun );

  // For each run ID in the 'runIDs' vector find the associated
  // OCARun file and add it to the reader.
  for ( Int_t iRun = 0; iRun < (Int_t)runIDs.size(); iRun++ ){

    myStream << runIDs[ iRun ];
    myStream >> runIDStr;

    // The full system path to the OCARun file.
    filename = ( ocaRunDir + runIDStr + fExt );

    // Add the file.
    Add( filename.c_str() );

    // Clear the stringstream and string variables
    // in preparation for the next iteration of the loop.
    myStream.clear();
    runIDStr = "";

  }

  // Set the next counter to zero to begin with.
  fNext = 0;

  // Set the number of OCARuns to the number which have
  // been added to the TChain.
  fNOCARuns = fOCARunT->GetEntries();

  // Set the pointer to the current OCARun object to the
  // first entry in the TChain.
  fOCARun = GetOCARun( runIDs[ 0 ] );

  // Set the current PMT pointer to the first PMT in the first OCARun entry.
  map< Int_t, OCAPMT >::iterator iPMT = fOCARun->GetOCAPMTIterBegin();
  fCurrentPMT = &( iPMT->second );
}

//////////////////////////////////////
//////////////////////////////////////

void OCARunReader::Add( const char* filename )
{

  // Get the current total number of OCARuns
  // stored on the reader.
  Long64_t total = fNOCARuns;

  // Add the file.
  fOCARunT->Add( filename );

  // Get the entry and add the run ID on the newly
  // added file to add to the list.
  fOCARunT->GetEntry( total );
  fListOfRunIDs.push_back( fOCARun->GetRunID() );

  // Recalculate the total number of OCARuns held
  // by the reader (this should just increment by one in this case).
  fNOCARuns = fOCARunT->GetEntries();

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t OCARunReader::CheckForOCARun( Int_t runID )
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

OCARun* OCARunReader::GetOCARun( Int_t runID )
{

  // Search through all of the OCARun entries currently
  // held on the TChain 'fOCARunT'.
  for ( Int_t iRun = 0; iRun < fNOCARuns; iRun++ ){
    
    fOCARunT->GetEntry( iRun );

    // If the run ID of one of the entries matches that
    // of the specified run ID 'runID', then break out of 
    // the loop. Otherwise continue to check.
    if ( fOCARun->GetRunID() == runID ){ break; }
    else { continue; }

  }

  // Set the current PMT pointer to that of the first OCAPMT object
  // on the current (pointed to) OCARun object.
  map< Int_t, OCAPMT >::iterator iPMT = fOCARun->GetOCAPMTIterBegin();
  fCurrentPMT = &( iPMT->second );

  // Return the pointer to the OCARun object.
  return fOCARun;

}
