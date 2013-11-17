///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: soc2locas.cc
///
/// EXECUTABLE: soc2locas
///
/// BRIEF: This executable processes SOC run files and outputs
///        LOCASRun (.root) files. These LOCASRun files are
///        to be used by LOCAS in the optics fit.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: Executable to process a main-run SOC file, a central-run SOC file
///         and an optional wavelength-run SOC file. Both the central- and
///         wavelength-run files are used calculate corrections to
///         values associated to the main-run file.
///
///         The approach is as follows:
///         
///         main-run file +      ----> 
///         central-run file +   ---->  soc2locas ---->  LOCASRun File
///         wavelength-run file  ---->  
///                             
///         The LOCASRun file contains all the required PMT information
///         and corrections for the main-run file ONLY.
///
///         Example Usage (at command line):
///
///              soc2locas -r [main-run-id] -c [central-run-id] -w [wavelength-run-id]
///              e.g. soc2locas -r 12121953 -c 30091953 -w 18091989
///
///         soc2locas will then output a file "12121953_LOCASRun.root" to 
///         ${LOCAS_DATA}/data/runs/locasrun. 
///
///         Currently BOTH a main-run and central-run file is required. The wavelength
///         run file is optional.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "RAT/getopt.h"
#include "RAT/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASLightPath.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace LOCAS;

// Utility class to parse the command line arguments for this executable
// Current options: -r (Main-Run-ID), -c (Central-Run-ID), -w (Wavelength-Run-ID, -h (Help)
class LOCASCmdOptions 
{
public:
  LOCASCmdOptions( ) : fRID( -1 ), fCRID( -1 ), fWRID( -1 ), fRIDStr( "" ), fCRIDStr( "" ), fWRIDStr( "" ) { }
  Long64_t fRID, fCRID, fWRID;
  std::string fRIDStr, fCRIDStr, fWRIDStr;
};

// Declare the three function prototypes used 
LOCASCmdOptions ParseArguments( int argc, char** argv );
void help();
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){
  
  Bool_t mrBool = false;
  Bool_t crBool = false;
  Bool_t wrBool = false;
  Bool_t secondFile = false;

  // Parse arguments passed to the command line
  LOCASCmdOptions Opts = ParseArguments( argc, argv );
  
  // Define the run IDs of the main-run, central- and wavelength-run files.
  Long64_t rID = Opts.fRID;
  Long64_t crID = Opts.fCRID;
  Long64_t wrID = Opts.fWRID;
  
  // Define the run IDs of the main-run, central- and wavelength-run files.
  // Same as above but as strings.
  std::string rIDStr = Opts.fRIDStr;
  std::string crIDStr = Opts.fCRIDStr;
  std::string wrIDStr = Opts.fWRIDStr;
  
  // Check that a main-run file has been specified.
  if ( rID < 0 ){ cout << "No Run-ID specified. Aborting" << endl; return 1; }
  else{ mrBool = true; }

  // Check whether a second run file has been specified (either a central- or wavelength-run file)
  if ( crID < 0 && wrID < 0 ){ cout << "No Central/Wavelength-Run ID specified. Aborting" << endl; return 1; }
  else{ secondFile = true; }

  // If a second file exists - check which type it is
  if ( secondFile ){
    if ( crID < 0 ){ cout << "No Central-Run ID specified." << endl; }
    else{ crBool = true; }

    if ( wrID < 0 ){ cout << "No Wavelength-Run ID specified." << endl; }
    else{ wrBool = true; }
  }

  // This shouldn't be caught - but just in case
  else{ 
    cout << "No Central/Wavelength-Run ID specified. Aborting" << endl; return 1;
  }
    
  
  // Obtain the directory path where the SOC files are located
  // from the environment and create the full filename paths.
  // SOC run filenames currently of the form "[Run-ID]_Run.root"
  LOCASDB lDB;
  string socRunDir = lDB.GetSOCRunDir();
  string fExt = "_Run.root";

  string rFilename, crFilename, wrFilename;
  
  rFilename = ( socRunDir + rIDStr + fExt ).c_str();
  if ( crBool ){ crFilename = ( socRunDir + crIDStr + fExt ).c_str(); }
  if( wrBool ){ wrFilename = ( socRunDir + wrIDStr + fExt ).c_str(); }
  
  // Check that the main-run file exists
  ifstream rfile( rFilename.c_str() );
  if ( !rfile ){    
    cout << "The SOC Run file: " << rFilename << " does not exist. Aborting." << endl;
    return 1;
  }
  rfile.close();

  // Check that the central-run file exists
  ifstream crfile( crFilename.c_str() );
  if ( !crfile ){    
    cout << "The Central SOC Run file: " << crFilename << " does not exist. Aborting." << endl;
    return 1;
  }
  crfile.close();

  // Check that the wavelength-run file exists
  ifstream wrfile( wrFilename.c_str() );
  if ( !wrfile ){    
    cout << "The Wavelength SOC Run file: " << wrFilename << " does not exist. Aborting." << endl;
    return 1;
  }
  wrfile.close();

  // Create the LOCASRun Objects for the run (lRun),
  // central (lCRun) and wavelength (lWRun) runs respectively.
  LOCASRun lRun;
  LOCASRun lCRun;
  LOCASRun lWRun;
  // Set Default run-IDs and Run-Types
  lRun.SetRunID( rID );
  lRun.SetIsMainRun( mrBool );

  lCRun.SetRunID( crID );
  lCRun.SetIsCentralRun( crBool );

  lWRun.SetRunID( wrID );
  lWRun.SetIsWavelengthRun( wrBool );
  
  // Create the SOCReader object <-- This allows for multiple SOC files
  // to be loaded
  cout << "Adding main-run SOC file: " << rIDStr + (string)"_Run.root" << endl;
  // Add the main-run to the SOC reader first
  RAT::SOCReader soc( ( socRunDir + rIDStr + (string)"_Run.root" ).c_str() );

  // If a central-run has been specified, add it to the SOCReader
  if ( crBool ){
    cout << "Adding central-run SOC file: " << crIDStr + (string)"_Run.root" << endl;
    // Add the central-run to the SOC reader
    soc.Add( ( socRunDir + crIDStr + (string)"_Run.root" ).c_str() );
  }

  // If a wavelength-run has been specified, add it to the SOCReader
  if ( wrBool ){
    cout << "Adding wavelength-run SOC file: " << wrIDStr + (string)"_Run.root" << endl;
    // Add the central-run to the SOC reader
    soc.Add( ( socRunDir + wrIDStr + (string)"_Run.root" ).c_str() );
  }

  // Now fill the LOCASRuns objects with the respective information
  // from the SOC files in the SOC reader
  lRun.Fill( soc, rID );
  if ( crBool ){ lCRun.Fill( soc, crID ); }
  if ( wrBool ){ lWRun.Fill( soc, wrID ); }

  lRun.SetCentralLBIntensityNorm( lCRun.GetMainLBIntensityNorm() );
  lRun.SetWavelengthLBIntensityNorm( lWRun.GetMainLBIntensityNorm() );


  // Now that all the SOC files have been loaded, and the LOCASRun objects
  // created, the corrections to the main-run entries can be calculated
  // using information from the other two files. The CrossRunFill function will
  // check the Run-IDs of both the central- and wavelength- LOCASRun objects
  // to see if their data has been entered following the above 'if' statements
  //cout << "Now Performing CrossRunFill" << endl;
  // // CALCULATION OF CROSS-RUN INFORMATION GOES HERE //
  // // START //
  lRun.CrossRunFill( lCRun, lWRun );
  // // END //


  // Now create a LOCASRun (.root) file to store the information of the main-run

  // Get the directory to where the LOCASRun file will be written
  // Currently this is ${LOCAS_DATA}/runs/locasrun
  std::string locasRunDir = lDB.GetLOCASRunDir();
  TFile* file = TFile::Open( ( locasRunDir + rIDStr + (string)"_LOCASRun.root" ).c_str(), "RECREATE" );

  // Create the Run Tree
  TTree* runTree = new TTree( "LOCASRunT", "LOCAS Run Tree" );

  // Declare a new branch pointing to the data stored in the lRun object
  runTree->Branch( "LOCASRun", lRun.ClassName(), &lRun, 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

  cout << "LOCASRun file: " << ( locasRunDir + rIDStr + (string)"_LOCASRun.root" ).c_str() << " has been created." << endl;

}

///////////////////////////
///                     ///
///  UTILITY FUNCTIONS  ///
///                     ///
///////////////////////////

LOCASCmdOptions ParseArguments( int argc, char** argv) 
{
  static struct RAT::option opts[] = { {"help", 0, NULL, 'h'},
                                       {"run-id", 1, NULL, 'r'},
                                       {"central-run-id", 1, NULL, 'c'},
                                       {"wavelength-run-id", 1, NULL, 'w'},
                                       {0,0,0,0} };
  
  LOCASCmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:r:c:w:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': cout << "HELP GOES HERE" << endl; break;
    case 'r': options.fRID = atol( RAT::optarg ); break;
    case 'c': options.fCRID = atol( RAT::optarg ); break;
    case 'w': options.fWRID = atol( RAT::optarg ); break;
    }
    
    c = getopt_long(argc, argv, "h:r:c:w:", opts, &option_index);
  }
  
  stringstream idStream;
  
  idStream << options.fRID;
  idStream >> options.fRIDStr;
  idStream.clear();
  
  idStream << options.fCRID;
  idStream >> options.fCRIDStr;
  idStream.clear();
  
  idStream << options.fWRID;
  idStream >> options.fWRIDStr;
  idStream.clear();

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ LOCAS - soc2locas" << "\n";
  cout << "Description: This executable processes SOC run files and outputs LOCASRun (.root) files to be used in the optics fit. \n";
  cout << "Usage: soc2locas [-h] [-r run-id] [-c central-run-id] [-w wavelength-run-id] \n";
  cout << " -h, --help                Display this help message and exit \n";
  cout << " -r, --run-id              Set the run ID for the corresponding SOC run file to be processed for a LOCASRun fit file \n";
  cout << " -c, --central-run-id      Set the corresponding central run ID file \n";
  cout << " -w, --wavelength-run-id   Set the corresponding wavelength run ID file \n";
  
}
