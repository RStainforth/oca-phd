/////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: db2soc.cc
///
/// EXECUTABLE: db2soc
///
/// BRIEF: This executable is used to briefly update
///        certain 'empty' fields on the RAT::SOC file.
///                 
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     06/2014 : RPFS - First Revision, new file.
///     02/2015 : RPFS - Update for current verison of RAT as of 02/2015
///
/// DETAIL: Executable to insert values into the SOC Run files.
///         Currently this is only needed to insert the runID into the file
///
///         Example Usage (at command line):
///
///              db2soc -r [run-id]
///              e.g. db2soc -r 12121953
///
///         where '12121953' is the Run-ID of the run SOC file
///         located in ${OCA_DATA}/runs/soc whose name is
///         "12121953_Run.root". This would udpdate the 'runID'
///         private member variable on the soc file.
///
///         COMMAND LINE: For testing purposes and due to a non finalised
///         data structure on the SOC file, the following entries may
///         be specifed at the command line:
///
///              1, SOC::runID (-r)
///
///         Example usage (at command line):
///
///              db2soc -r [run-id]
///
///              e.g. db2soc -r 12121953
///
///         The above would be specified for a run with run ID 12121953
///
///         THIS EXECUTABLE IS SIMPLY PLACE-HOLDER 
///         UNTIL THE RAT DATABASE IS WORKING!
///
//////////////////////////////////////////////////////////////////////////////

#include "RAT/DS/SOC.hh"
#include "RAT/DS/Run.hh"

#include "OCADB.hh"

#include "TFile.h"
#include "TTree.h"

#include <getopt.h>
#include <iostream>
#include <string>
#include <fstream>

#include <stdio.h>
#include <time.h>

using namespace std;
using namespace RAT;
using namespace OCA;

// Utility class to parse the command line arguments for this executable
class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fRID( -1 ), fRIDStr( "" ) { }
  UInt_t fRID;
  std::string fRIDStr;
};

// Declare the three function prototypes used
OCACmdOptions ParseArguments( int argc, char** argv );
void help();

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );

  // Define the run-ID in Long64_t and string format
  UInt_t rID = Opts.fRID;
  string rIDStr = Opts.fRIDStr;

  // Obtain the directory path where the SOC files are located
  // from the environment and create the full filename path.
  // SOC run filenames currently of the form "[Run-ID]_Run.root"
  OCADB lDB;
  string socRunDir = lDB.GetSOCRunDir();
  string fExt = "_Run.root";
  string filename = ( socRunDir + rIDStr + fExt ).c_str();

  // Check that the run file exists
  ifstream ifile( filename.c_str() );
  if ( !ifile ){    
    cout << "The SOC Run file: " << filename << " does not exist. Aborting." << endl;
    return 1;
  }

  // Create a pointer to the SOC file to open and update
  TFile* file = TFile::Open( filename.c_str(), "UPDATE" );

  // Obtain a pointer to the SOC Tree
  TTree* oldSocTree = (TTree*)file->Get( "T" );
  // Obtain a pointer to the RunTree
  TTree* oldRunTree = (TTree*)file->Get( "runT" );
  
  // Create a new pointer to a new tree and RAT::DS::SOC object.
  // These are the objects which will overwrite
  // the old one with the new values.
  TTree* socTree = new TTree( "T", "SOC Tree" );

  // Create a new pointer to a new tree and RAT::DS::Run object.
  TTree* runTree = new TTree( "runT", "RAT Run Tree" );

  DS::SOC* socBr = new DS::SOC();
  DS::Run* runBr = new DS::Run();

  // Copy the original RAT::DS::Run information into the
  // new Run object
  oldRunTree->SetBranchAddress( "run", &runBr ); 
  oldRunTree->GetEntry( 0 ); 

  // Copy the original RAT::DS::SOC information into the
  // new SOC object
  oldSocTree->SetBranchAddress( "soc", &socBr ); 
  oldSocTree->GetEntry( 0 ); 

  if ( rID > 0 ){
    cout << "Setting RunID to: " << rID << endl;
    socBr->SetRunID( rID ); 
    runBr->SetRunID( rID );
  }

  // Create the new branch on the new tree
  socTree->Branch( "soc", socBr->ClassName(), &socBr, 32000, 99 );
  socTree->Fill();

  // Create the new branch on the new tree
  runTree->Branch( "run", runBr->ClassName(), &runBr, 32000, 99 );
  runTree->Fill();

  // Overwirte the old tree and close the file.
  socTree->Write( "T", TObject::kOverwrite );
  // Overwirte the old tree and close the file.
  runTree->Write( "runT", TObject::kOverwrite );   
  file->Close();
  
  cout << "File: " << filename << " has been updated" << endl;

}

///////////////////////////
///                     ///
///  UTILITY FUNCTIONS  ///
///                     ///
///////////////////////////

OCACmdOptions ParseArguments( int argc, char** argv) 
{
  static struct option opts[] = { {"help", 0, NULL, 'h'},
                                       {"run-id", 1, NULL, 'r'},
                                       {0,0,0,0} };

  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:r:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); exit( 0 ); break;
    case 'r': options.fRID = atol( optarg ); break;
    }
    
    c = getopt_long(argc, argv, "h:r:", opts, &option_index);
  }
  
  stringstream idStream;
  
  idStream << options.fRID;
  idStream >> options.fRIDStr;
  idStream.clear();

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ OCA - db2soc" << "\n";
  cout << "Description: Executable to update the runID on the SOC object in the SOC file.\n";
  cout << "Usage: db2soc [-h] [-r run-id]\n";
  cout << " -h, --help                            Display this help message and exit \n";
  cout << " -r, --run-id                          Set the run ID for the corresponding SOC run file to be updated \n";
  
}
