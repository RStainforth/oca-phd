///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: soc2oca.cc
///
/// EXECUTABLE: soc2oca
///
/// BRIEF: This executable processes SOC run files and outputs
///        OCARun (.root) files. These OCARun files are
///        to be used by OCA in the optics fit.
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
///         central-run file +   ---->  soc2oca ---->  OCARun File
///         wavelength-run file  ---->  
///                             
///         The OCARun file contains all the required PMT information
///         and corrections for the main-run file ONLY.
///
///         Example Usage (at command line):
///
///              soc2oca -r [main-run-id] -c [central-run-id] -w [wavelength-run-id]
///              e.g. soc2oca -r 12121953 -c 30091953 -w 18091989
///
///         soc2oca will then output a file "12121953_OCARun.root" to 
///         ${OCA_DATA}/data/runs/ocarun. 
///
///         Currently BOTH a main-run and central-run file is required. The wavelength
///         run file is optional.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "RAT/DU/Utility.hh"
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"
#include "RAT/DU/ChanHWStatus.hh"
#include "RAT/DU/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/Log.hh"

#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include "TFile.h"
#include "TTree.h"

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace OCA;

// Utility class to parse the command line arguments for this executable
// Current options: 
// -r [Off-Axis-RunID] 
// -c [Central-RunID] 
// -R [Wavelength-Off-Axis-RunID] 
// -C [Wavelength-Central-RunID]
// -l [XY] : Laserball position option
//       X : Off-Axis run laserball position.
//         X = 1 : Off-Axis manipulator laserball position 
//         X = 2 : Off-Axis camera laserball position
//         X = 3 : Off-Axis fitted laserball position
//         X = 4 : Off-Axis fitted laserball position from wavelength run (-R option)
//
//       Y : Central run laserball position.
//         Y = 1 : Central manipulator laserball position
//         Y = 2 : Central camera laserball position
//         Y = 3 : Central fitted laserball position
//         Y = 4 : Central fitted laserball position from wavelength run (-C option)
//
// Note: By convention we will often use the option '-l 44' : wavelenght position fits for both
//       the off-axis and central runs.
// -d [MMYY] : The name of the dataset directory in the ${OCA_ROOT}/data/runs/soc directory
// -s [Systematic-File-Name] : Name of systematic file in the ${OCA_ROOT}/data/systematics directory
// -h : Display help for this executable 
class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fRID( -1 ), fCID( -1 ), 
                     fWRID( -1 ), fWCID( -1 ),
                     fLBPosMode( -1 ),
                     fRIDStr( "" ), fCIDStr( "" ), 
                     fWRIDStr( "" ), fWCIDStr( "" ),
                     fLBPosModeStr( "" ),
                     fMMYY( "" ), fSystematicFilePath( "" ) { }
  Long64_t fRID, fCID, fWRID, fWCID, fLBPosMode;
  std::string fRIDStr, fCIDStr, fWRIDStr, fWCIDStr, fLBPosModeStr;
  std::string fMMYY, fSystematicFilePath;
};

// Declare the three function prototypes used 
OCACmdOptions ParseArguments( int argc, char** argv );
void ApplySystematic( OCARun* ocaRunPtr, std::string systematicOpt, Double_t systematicVal );
void help();
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );
  
  // Define the run IDs of the off-axis-, central- and wavelength-run files.
  Long64_t rID = Opts.fRID;
  Long64_t cID = Opts.fCID;
  Long64_t wrID = Opts.fWRID;
  Long64_t wcID = Opts.fWCID;
  
  // Define the run IDs of the off-axis-, central- and wavelength-run files.
  // Same as above but as std::string objects.
  std::string rIDStr = Opts.fRIDStr;
  std::string cIDStr = Opts.fCIDStr;
  std::string wrIDStr = Opts.fWRIDStr;
  std::string wcIDStr = Opts.fWCIDStr;

  // Get the directory in MMYY format for where the SOC files are stored.
  std::string dirMMYY = Opts.fMMYY;

  // Get the laserball position mode to use for the off-axis and central runs.
  // These positions form the basis for the calculation of distances,
  // solidangle, Fresnel transmission coefficients, incidents angles etc.
  std::string lbPosMode = Opts.fLBPosModeStr;
  stringstream lbPosStream;
  lbPosStream << ( lbPosMode.c_str() )[ 0 ];
  Int_t rLBPosMode = 0;
  lbPosStream >> rLBPosMode;
  lbPosStream.clear();
  Int_t cLBPosMode = 0;
  lbPosStream << ( lbPosMode.c_str() )[ 1 ];
  lbPosStream >> cLBPosMode;
  lbPosStream.clear();

  // Get the systematics file path
  std::string sysFile = Opts.fSystematicFilePath;
  OCADB lDB;
  lDB.SetFile( ( lDB.GetSystematicsDir() + sysFile ).c_str() );
  std::vector< std::string > sysOpts = lDB.GetStringVectorField( "SYSTEMATICS", "systematics_list", "systematics_setup" );
  std::vector< Float_t > sysVals;
  for ( Int_t iSys = 0; iSys < (Int_t)sysOpts.size(); iSys++ ){
    sysVals.push_back( lDB.GetDoubleField( "SYSTEMATICS", sysOpts[ iSys ], "systematics_setup" ) );
  }

  cout << "\n";
  cout << "###############################" << endl;
  cout << "######## SOC2OCA START ########" << endl;
  cout << "###############################" << endl;
  cout << "\n";
  
  // Check that an off-axis-run ID has been specified.
  if ( rID < 0 ){ cout << "soc2oca error: No off-axis run ID specified. Aborting" << endl; return 1; }
  else{ cout << "Off-axis run ID: " << rID << endl; }

  // Check that a central-run file has been specified.
  if ( cID < 0 ){ cout << "soc2oca error: No central run ID specified. Aborting" << endl; return 1; }
  else{ cout << "Central run ID: " << cID << endl; }

  if ( wrID < 0 && rLBPosMode == 4 ){ 
    cout << "soc2oca error: The requested laserball position for the off-axis run is to use the fitted position\n"; 
    cout << "from the wavelength run '-l [" << rLBPosMode << "]" << cLBPosMode << "' (value = 4), yet no run id for the off-axis wavelength run has been\n";
    cout << "specified. Aborting\n"; return 1;
  }
  else{ cout << "Off-axis wavelength run ID: " << wrID << endl; }

  if ( wcID < 0 && cLBPosMode == 4 ){ 
    cout << "soc2oca error: The requested laserball position for the central run is to use the fitted position\n"; 
    cout << "from the wavelength run '-l " << rLBPosMode << "[" << cLBPosMode << "]' (value = 4), yet no run id for the central wavelength run has been\n";
    cout << "specified. Aborting\n"; return 1;
  }
  else{ cout << "Central wavelength run ID: " << wcID << endl; }
  cout << "--------------------------" << endl;

  cout << "Off-axis run laserball position mode: " << rLBPosMode << endl;
  cout << "Off-axis run will use the ";
  switch ( rLBPosMode ) {
  case 1 : cout << "manipulator coordinates from the run as the laserball position.\n"; break;
  case 2 : cout << "camera determined position from the run as the laserball position.\n"; break;
  case 3 : cout << "fitted position from the run as the laserball position.\n"; break;
  case 4 : cout << "fitted position from the wavelength run as the laserball position.\n"; break;
  }
  cout << "Central run laserball position mode: " << cLBPosMode << endl;
  cout << "Central run will use the ";
  switch ( cLBPosMode ) {
  case 1 : cout << "manipulator coordinates from the run as the laserball position.\n"; break;
  case 2 : cout << "camera determined position from the run as the laserball position.\n"; break;
  case 3 : cout << "fitted position from the run as the laserball position.\n"; break;
  case 4 : cout << "fitted position from the wavelength run as the laserball position.\n"; break;
  }    
  cout << "--------------------------" << endl;
  // Obtain the directory path where the SOC files are located
  // from the environment and create the full filename paths.
  // SOC run filenames currently of the form "[Run-ID]_Run.root"
  string socRunDir = lDB.GetSOCRunDir( dirMMYY );
  string fExt = "_Run.root";

  string rFilename, cFilename, wrFilename, wcFilename;
  
  rFilename = ( socRunDir + rIDStr + fExt ).c_str();
  cFilename = ( socRunDir + cIDStr + fExt ).c_str();
  if( wrID > 0 ){ wrFilename = ( socRunDir + wrIDStr + fExt ).c_str(); }
  if( wcID > 0 ){ wcFilename = ( socRunDir + wcIDStr + fExt ).c_str(); }
  
  // Check that the main-run file exists
  ifstream rfile( rFilename.c_str() );
  if ( !rfile ){    
    cout << "The SOC Run file: " << endl;
    cout << rFilename << endl;
    cout << "does not exist. Aborting." << endl;
    cout << "--------------------------" << endl;
    return 1;
  }
  else{
    cout << "The SOC Run file: " << endl;
    cout << rFilename << endl;
    cout << "will be used as the off-axis run file" << endl;
    cout << "--------------------------" << endl;
  }
  rfile.close();

  // Check that the central-run file exists
  ifstream cfile( cFilename.c_str() );
  if ( !cfile ){ 
    cout << "The SOC Run file: " << endl;
    cout << cFilename << endl;
    cout << "does not exist. Aborting." << endl;
    cout << "--------------------------" << endl;   
    return 1;
  }
  
  else{
    cout << "The SOC Run file: " << endl;
    cout << cFilename << endl;
    cout << "will be used as the central run file" << endl;
    cout << "--------------------------" << endl;
  }
  cfile.close();

  // Check that the wavelength off-axis run file exists
  if ( wrID > 0 ){
    ifstream wrfile( wrFilename.c_str() );
    if ( !wrfile ){ 
      cout << "The SOC Run file: " << endl;
      cout << wrFilename << endl;
      cout << "does not exist. Aborting." << endl;
      cout << "--------------------------" << endl;    
      return 1;
    }

    else{
      cout << "The SOC Run file: " << endl;
      cout << wrFilename << endl;
      cout << "will be used as the wavelength off-axis run file" << endl;
      cout << "--------------------------" << endl;
    }
    wrfile.close();
  }

  // Check that the wavelength central run file exists
  if ( wcID > 0 ){
    ifstream wcfile( wcFilename.c_str() );
    if ( !wcfile ){ 
      cout << "The SOC Run file: " << endl;
      cout << wcFilename << endl;
      cout << "does not exist. Aborting." << endl;
      cout << "--------------------------" << endl;    
      return 1;
    }

    else{
      cout << "The SOC Run file: " << endl;
      cout << wcFilename << endl;
      cout << "will be used as the wavelength central run file" << endl;
      cout << "--------------------------" << endl;
    }
    wcfile.close();
  }

  // Create the OCARun Objects for the off-axis run (lRun) and
  // central (lCRun) runs respectively.
  OCARun* lRunPtr = new OCARun();
  OCARun* lCRunPtr = new OCARun();
  // Set Default run-IDs
  lRunPtr->SetRunID( rID );
  lCRunPtr->SetRunID( cID );

  OCARun* lWRRunPtr = NULL;
  OCARun* lWCRunPtr = NULL;
  
  // Create the SOCReader object <-- This allows for multiple SOC files
  // to be loaded
  cout << "Adding off-axis run SOC file: " << endl;
  cout << rIDStr + (string)"_Run.root" << endl;
  cout << "--------------------------" << endl;
  // Add the main-run to the SOC reader first
  RAT::DU::SOCReader soc( ( socRunDir + rIDStr + (string)"_Run.root" ).c_str() );

  cout << "Adding central run SOC file: " << endl;
  cout << cIDStr + (string)"_Run.root" << endl;
  cout << "--------------------------" << endl;
  // Add the central-run to the SOC reader
  soc.Add( ( socRunDir + cIDStr + (string)"_Run.root" ).c_str() );

  // If a wavelength off-axis run has been specified, add it to the SOCReader
  if ( wrID > 0 && rLBPosMode == 4 ){
    lWRRunPtr = new OCARun();
    lWRRunPtr->SetRunID( wrID );

    cout << "Adding wavelength off-axis run SOC file: " << endl;
    cout << wrIDStr + (string)"_Run.root" << endl;
    cout << "--------------------------" << endl;
    // Add the central-run to the SOC reader
    soc.Add( ( socRunDir + wrIDStr + (string)"_Run.root" ).c_str() );
  }

  // If a wavelength central run has been specified, add it to the SOCReader
  if ( wcID > 0 && cLBPosMode == 4 ){
    lWCRunPtr = new OCARun();
    lWCRunPtr->SetRunID( wcID );

    cout << "Adding wavelength central run SOC file: " << endl;
    cout << wcIDStr + (string)"_Run.root" << endl;
    cout << "--------------------------" << endl;
    // Add the central-run to the SOC reader
    soc.Add( ( socRunDir + wcIDStr + (string)"_Run.root" ).c_str() );
  }

  // Create LightPathCalculator object;
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator();
  RAT::DU::ShadowingCalculator shadowCalc = RAT::DU::Utility::Get()->GetShadowingCalculator();
  RAT::DU::PMTInfo pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  RAT::DU::ChanHWStatus chanHW = RAT::DU::Utility::Get()->GetChanHWStatus();
  shadowCalc.SetAllGeometryTolerances( 150.0 );

  // Now fill the OCARuns objects with the respective information
  // from the SOC files in the SOC reader
  cout << "Now filling run information from off-axis run SOC file...";
  lRunPtr->FillRunInfo( soc, rID, rLBPosMode );
  cout << "done." << endl;
  cout << "Now filling run information from central run SOC file...";
  lCRunPtr->FillRunInfo( soc, cID, cLBPosMode ); 
  cout << "done." << endl;

  if ( wrID > 0 && rLBPosMode == 4){ 
    cout << "Now filling run information from wavelength off-axis run SOC file...";
    lWRRunPtr->FillRunInfo( soc, wrID, 3, false );
    lRunPtr->SetLBPos( lWRRunPtr->GetLBPos() ); 
    lRunPtr->SetLBXPosErr( lWRRunPtr->GetLBXPosErr() );
    lRunPtr->SetLBYPosErr( lWRRunPtr->GetLBYPosErr() );
    lRunPtr->SetLBZPosErr( lWRRunPtr->GetLBZPosErr() );
    cout << "done." << endl;
  }
  delete lWRRunPtr;

  if ( wcID > 0 && cLBPosMode == 4 ){ 
    cout << "Now filling run information from wavelength central run SOC file...";
    lWCRunPtr->FillRunInfo( soc, wcID, 3, false );
    lCRunPtr->SetLBPos( lWCRunPtr->GetLBPos() ); 
    lCRunPtr->SetLBXPosErr( lWCRunPtr->GetLBXPosErr() );
    lCRunPtr->SetLBYPosErr( lWCRunPtr->GetLBYPosErr() );
    lCRunPtr->SetLBZPosErr( lWCRunPtr->GetLBZPosErr() );      
    cout << "done." << endl;
  }
  delete lWCRunPtr;

  cout << "Now filling PMT information from off-axis SOC file...";
  lRunPtr->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, rID );
  cout << "done." << endl;
  cout << "Now filling PMT information from central SOC file...";
  lCRunPtr->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, cID );
  cout << "done." << endl;

  OCARun** sysRuns = new OCARun*[ (Int_t)sysOpts.size() ];
  OCARun** sysRunsCtr = new OCARun*[ (Int_t)sysOpts.size() ];
  for ( Int_t iSys = 0; iSys < (Int_t)sysOpts.size(); iSys++ ){
    sysRuns[ iSys ] = new OCARun( *lRunPtr );
    sysRuns[ iSys ]->CopyOCAPMTInfo( *lRunPtr );
    sysRunsCtr[ iSys ] = new OCARun( *lCRunPtr );
    sysRunsCtr[ iSys ]->CopyOCAPMTInfo( *lCRunPtr );
    cout << "Now applying systematic " << iSys << ": " << sysOpts[ iSys ] << endl;
    ApplySystematic( sysRuns[ iSys ], sysOpts[ iSys ], sysVals[ iSys ] );
    ApplySystematic( sysRunsCtr[ iSys ], sysOpts[ iSys ], sysVals[ iSys ] );
    sysRuns[ iSys ]->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, rID );
    sysRunsCtr[ iSys ]->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, cID );
    sysRuns[ iSys ]->CrossRunFill( sysRunsCtr[ iSys ] );
    cout << "done.\n";
  }

  delete [] sysRunsCtr;
  
  // Now that all the SOC files have been loaded, and the OCARun objects
  // created, the corrections to the main-run entries can be calculated
  // using information from the other two files. The CrossRunFill function will
  // check the Run-IDs of both the central- and wavelength- OCARun objects
  // to see if their data has been entered following the above 'if' statements
  cout << "Now combining off-axis and central run information...";
  lRunPtr->CrossRunFill( lCRunPtr );
  cout << "done.\n" << endl;
  delete lCRunPtr;

  // Now create a OCARun (.root) file to store the information of the main-run

  // Get the directory to where the OCARun file will be written
  // Currently this is ${OCA_DATA}/runs/ocarun
  std::string ocaRunDir = lDB.GetOCARunDir( dirMMYY );
  TFile* file = new TFile( ( ocaRunDir + rIDStr + (string)"_OCARun.root" ).c_str(), "RECREATE" );

  // Create the Run Tree
  TTree* runTree = new TTree( "OCARunT", "OCA Run Tree" );

  // Declare a new branch pointing to the data stored in the lRun object
  runTree->Branch( "OCARun", lRunPtr->ClassName(), &(*lRunPtr), 32000, 99 );
  file->cd();

  for ( Int_t iSys = 0; iSys < (Int_t)sysOpts.size(); iSys++ ){
    runTree->Branch( sysOpts[ iSys ].c_str(), sysRuns[ iSys ]->ClassName(), 
                     &(*sysRuns[ iSys ]), 32000, 99 );
    file->cd();
  }

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete lRunPtr;
  delete [] sysRuns;

  cout << "OCARun file: " << endl;
  cout << ( ocaRunDir + rIDStr + (string)"_OCARun.root" ).c_str() << endl;
  cout << "has been created." << endl;
  cout << "\n";
  cout << "##############################" << endl;
  cout << "######## SOC2OCA END #########" << endl;
  cout << "##############################" << endl;
  cout << "\n";

}

///////////////////////////
///                     ///
///  UTILITY FUNCTIONS  ///
///                     ///
///////////////////////////

OCACmdOptions ParseArguments( int argc, char** argv) 
{
  static struct option opts[] = { {"help", 0, NULL, 'h'},
                                  {"off-axis-run-id", 1, NULL, 'r'},
                                  {"central-run-id", 1, NULL, 'c'},
                                  {"wavelength-off-axis-run-id", 1, NULL, 'R'},
                                  {"wavelength-central-run-id", 1, NULL, 'C'},
                                  {"laserball-pos-mode", 1, NULL, 'l'},
                                  {"month-year-directory", 1, NULL, 'd'},
                                  {"systematics-file", 1, NULL, 's'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:r:c:R:C:l:d:s:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': cout << "HELP GOES HERE" << endl; break;
    case 'r': options.fRID = atol( optarg ); break;
    case 'c': options.fCID = atol( optarg ); break;
    case 'R': options.fWRID = atol( optarg ); break;
    case 'C': options.fWCID = atol( optarg ); break;
    case 'l': options.fLBPosMode = atol( optarg ); break;
    case 's': options.fSystematicFilePath = (std::string)optarg; break;
    case 'd': options.fMMYY = (std::string)optarg; break;
    }
    
    c = getopt_long(argc, argv, "h:r:c:R:C:l:d:s:", opts, &option_index);
  }
  
  stringstream idStream;
  
  idStream << options.fRID;
  idStream >> options.fRIDStr;
  idStream.clear();
  
  idStream << options.fCID;
  idStream >> options.fCIDStr;
  idStream.clear();
  
  idStream << options.fWRID;
  idStream >> options.fWRIDStr;
  idStream.clear();

  idStream << options.fWCID;
  idStream >> options.fWCIDStr;
  idStream.clear();

  idStream << options.fLBPosMode;
  idStream >> options.fLBPosModeStr;
  idStream.clear();

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ OCA - soc2oca" << "\n";
  cout << "Description: This executable processes SOC run files and outputs OCARun (.root) files to be used in the optics fit. \n";
  cout << "Usage: soc2oca [-h] [-r run-id] [-c central-run-id] [-w wavelength-run-id] \n";
  cout << " -h, --help                Display this help message and exit \n";
  cout << " -r, --run-id              Set the run ID for the corresponding SOC run file to be processed for a OCARun fit file \n";
  cout << " -c, --central-run-id      Set the corresponding central run ID file \n";
  cout << " -w, --wavelength-run-id   Set the corresponding wavelength run ID file \n";
  
}

//////////////////////
//////////////////////

void ApplySystematic( OCARun* ocaRunPtr, std::string systematicOpt, Double_t systematicVal )
{

  // Laserball position radius scaling 
  // R_lb -> R_b * systematicVal
  if ( systematicOpt == "laserball_r_scale" ){
    ocaRunPtr->SetLBPos( systematicVal * ocaRunPtr->GetLBPos() );
  }

  // Laserball position radius shift 
  // R_lb -> R_b + systematicVal [mm]
  if ( systematicOpt == "laserball_r_shift" ){
    ocaRunPtr->SetLBPos( systematicVal * ocaRunPtr->GetLBPos() );
  }

  // Laserball position x-position +ve shift
  // Rx_lb -> Rx_lb + systematicVal [mm]
  if ( systematicOpt == "laserball_plus_x_shift" ){
    TVector3 lbPos = ocaRunPtr->GetLBPos();
    lbPos.SetX( lbPos.X() + systematicVal );
    ocaRunPtr->SetLBPos( lbPos );
  }
  // Laserball position x-position -ve shift
  // Rx_lb -> Rx_lb - systematicVal [mm]
  if ( systematicOpt == "laserball_minus_x_shift" ){
    TVector3 lbPos = ocaRunPtr->GetLBPos();
    lbPos.SetX( lbPos.X() - systematicVal );
    ocaRunPtr->SetLBPos( lbPos );
  }

  // Laserball position y-position +ve shift
  // Ry_lb -> Ry_lb + systematicVal [mm]
  if ( systematicOpt == "laserball_plus_y_shift" ){
    TVector3 lbPos = ocaRunPtr->GetLBPos();
    lbPos.SetY( lbPos.Y() + systematicVal );
    ocaRunPtr->SetLBPos( lbPos );
  }
  // Laserball position y-position -ve shift
  // Ry_lb -> Ry_lb - systematicVal [mm]
  if ( systematicOpt == "laserball_minus_y_shift" ){
    TVector3 lbPos = ocaRunPtr->GetLBPos();
    lbPos.SetY( lbPos.Y() - systematicVal );
    ocaRunPtr->SetLBPos( lbPos );
  }

  // Laserball position z-position +ve shift
  // Rz_lb -> Rz_lb + systematicVal [mm]
  if ( systematicOpt == "laserball_plus_z_shift" ){
    TVector3 lbPos = ocaRunPtr->GetLBPos();
    lbPos.SetZ( lbPos.Z() + systematicVal );
    ocaRunPtr->SetLBPos( lbPos );
  }
  // Laserball position z-position -ve shift
  // Rz_lb -> Rz_lb - systematicVal [mm]
  if ( systematicOpt == "laserball_minus_z_shift" ){
    TVector3 lbPos = ocaRunPtr->GetLBPos();
    lbPos.SetZ( lbPos.Z() + systematicVal );
    ocaRunPtr->SetLBPos( lbPos );
  }

  // Laserball wavelength +ve shift
  // Lambda_lb -> Lambda_lb + systemacVal[nm]
  if ( systematicOpt == "lambda_plus_shift" ){
    ocaRunPtr->SetLambda( ocaRunPtr->GetLambda() + systematicVal );
  }
  // Laserball wavelength -ve shift
  // Lambda_lb -> Lambda_lb - systemacVal[nm]
  if ( systematicOpt == "lambda_minus_shift" ){
    ocaRunPtr->SetLambda( ocaRunPtr->GetLambda() - systematicVal );
  }

  // Dummy systematic for flat LB distribution
  if ( systematicOpt == "lb_distribution_flat" ){
    cout << "Flat laserball distribution systematic registered. Systematic will be enforced at the fitting stage. Not here" << endl;
  }

  // Dummy systematic for LB distribution squared
  if ( systematicOpt == "lb_distribution_squared" ){
    cout << "Squared laserball distribution systematic registered. Systematic will be enforced at the fitting stage. Not here" << endl;
  }

}
