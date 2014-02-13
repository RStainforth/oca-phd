/////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: db2soc.cc
///
/// EXECUTABLE: db2soc
///
/// BRIEF: This executable loads information from the RAT or LOCAS DB
///        (data base) or command line and inserts it into 
///        the SOC (.root) file.
///                 
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: Executable to insert values from the RAT or LOCAS 
///         data base or command line into the SOC Run files. 
///         
///         DATA BASE: Updates the following entries on the SOC file:
///
///              1, SOCPMT::relOccSim_fillShadow
///              2, SOCPMT::relOccSim_hdRopeShadow
///
///         Example Usage (at command line):
///
///              db2soc -r [run-id] -f [run-id]
///              e.g. db2soc -r 12121953 -f 12121953
///
///         where '12121953' is the Run-ID of the run SOC file
///         located in ${LOCAS_DATA}/runs/soc whose name is
///         "12121953_Run.root". Values of the entries are obtained
///         by the executable from the following locations:
///
///              1, ${LOCAS_DATA}/shadowing/geo/geo_[run-id].ratdb
///              2, ${LOCAS_DATA}/shadowing/avhd/avhd_[run-id].ratdb
///
///         COMMAND LINE: For testing purposes and due to a non finalised
///         data structure on the SOC file, the following entries may
///         be specifed at the command line:
///
///              1, SOC::sourceID (-s)
///              2, SOC::runID (-i)
///              3, SOC::laserWavelength (-w)
///              4, SOC::sourcePosManip (-x, -y, -z for coordinates)
///              5, SOC::globalTimeOffset (-g)
///
///         Example usage (at command line):
///
///              db2soc -i [run-id] -s [source-run-id] -w [source-wavelength]
///                     -x [source-pos-x] -y [source-pos-y]
///                     -z [source-pos-z] -g [source-global-time-offset]
///
///              e.g. db2soc -i 12121953 -s 420 -w 420 -x 4000 -y 0 -z 0 -g 0
///
///         The above would be specified for a 420 nm laser with run ID 
///         12121953 at a position (4000,0,0) mm with a global time offset
///         of 0 and a source ID (matching that of the laser) of 420.
///
///         NOTE: If you specify a negative numerical value you must end the
///         command line command with the argument termination token '--'
///
///          e.g. db2soc -i 12121953 -s 420 -w 420 -x -4000 -y -1000 -z 500 --
///
//////////////////////////////////////////////////////////////////////////////

#include "RAT/getopt.h"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"

#include "TFile.h"

#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include <stdio.h>
#include <time.h>

using namespace std;
using namespace RAT;
using namespace LOCAS;

// Utility class to parse the command line arguments for this executable
class LOCASCmdOptions 
{
public:
  LOCASCmdOptions( ) : fRID( -1 ), fRIDStr( "" ),
		       fSrcID( -1 ), fSrcRunID( -1 ),
		       fSrcPosX( -1 ), fSrcPosY( -1 ),
		       fSrcPosZ( -1 ), fSrcWL( -1 ),
                       fSrcGTO( -1 ), fSrcFill( -1 ) { }
  Long64_t fRID;
  Int_t fSrcID, fSrcRunID, fSrcWL, fSrcFill;
  Double_t fSrcPosX, fSrcPosY, fSrcPosZ;
  Float_t fSrcGTO;
  std::string fRIDStr;
};

// Declare the three function prototypes used
LOCASCmdOptions ParseArguments( int argc, char** argv );
void help();

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Parse arguments passed to the command line
  LOCASCmdOptions Opts = ParseArguments( argc, argv );

  // Define the run-ID in Long64_t and string format
  Long64_t rID = Opts.fRID;
  string rIDStr = Opts.fRIDStr;

  Int_t srcID = Opts.fSrcID;
  Int_t srcRunID = Opts.fSrcRunID;
  Int_t srcWL = Opts.fSrcWL;
  Int_t srcFill = Opts.fSrcFill;
  Float_t srcGTO = Opts.fSrcGTO;
  TVector3 srcPos( Opts.fSrcPosX, Opts.fSrcPosY, Opts.fSrcPosZ );

  // Obtain the directory path where the SOC files are located
  // from the environment and create the full filename path.
  // SOC run filenames currently of the form "[Run-ID]_Run.root"
  LOCASDB lDB;
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
  
  // Create a new pointer to a new tree and RAT::DS::SOC object.
  // These are the objects which will overwrite
  // the old one with the new values.
  TTree* socTree = new TTree( "T", "SOC Tree" );

  DS::SOC* socBr = new DS::SOC();

  // Copy the original RAT::DS::SOC information into the
  // new SOC object
  oldSocTree->SetBranchAddress( "soc", &socBr ); 
  oldSocTree->GetEntry( 0 ); 

  // Set SOC Run Information if it has been specified at the command line
  if ( srcFill > 0 ){
    lDB.LoadAVHDRopePMTShadowingVals( srcFill );
    lDB.LoadGeoPMTShadowingVals( srcFill );
  }

  if ( srcID > 0 ){ socBr->SetSourceID( srcID ); }
  if ( srcRunID > 0 ){ socBr->SetRunID( srcRunID ); }
  if ( srcPos.Mag() >= 0.0 ){ socBr->SetSourcePosManip( srcPos ); }
  if ( srcWL > 0 ){ socBr->SetLaserWavelength( srcWL ); }
  if ( srcGTO > 0.0 ){ socBr->SetGlobalTimeOffset( srcGTO ); }

  // Create an iterator to loop over the PMTs stored
  // in the SOC object.
  std::map<Int_t, DS::SOCPMT>::iterator iPMT;

  if ( srcFill > 0 ){
    for ( iPMT = socBr->GetSOCPMTIterBegin(); iPMT != socBr->GetSOCPMTIterEnd(); ++iPMT ){
      
      // Obtain the PMT ID
      Int_t pmtID = iPMT->first;
      
      // Obtain the shadowing values for the specified PMT ID
      Double_t avhdRelOcc = lDB.GetAVHDRopePMTShadowingVal( pmtID );
      Double_t geoRelOcc = lDB.GetGeoPMTShadowingVal( pmtID );
      
      // Set them in the SOCPMT objects contained in the SOC object
      ( socBr->GetSOCPMT( iPMT->first ) ).SetRelOccSim_fullShadow( geoRelOcc );
      ( socBr->GetSOCPMT( iPMT->first ) ).SetRelOccSim_hdRopeShadow( avhdRelOcc );
      
    }
  }

  // Create the new branch on the new tree
  socTree->Branch( "soc", socBr->ClassName(), &socBr, 32000, 99 );
  socTree->Fill();

  // Overwirte the old tree and close the file.
  socTree->Write( "T", TObject::kOverwrite );  
  file->Close();
  
  cout << "File: " << filename << " has been updated" << endl;

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
                                       {"source-run-id", 1, NULL, 's'},
                                       {"source-id", 1, NULL, 'i'},
                                       {"source-pos-x", 1, NULL, 'x'},
				       {"source-pos-y", 1, NULL, 'y'},
				       {"source-pos-z", 1, NULL, 'Z'},
                                       {"source-wavelength", 1, NULL, 'w'},
                                       {"source-global-time-offset", 1, NULL, 'g'},
                                       {"fill-shadowing-values", 1, NULL, 'f'},
                                       {0,0,0,0} };

  
  LOCASCmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:r:s:i:x:y:z:w:g:sh:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); exit( 0 ); break;
    case 'r': options.fRID = atol( RAT::optarg ); break;
    case 's': options.fSrcID = atoi( RAT::optarg ); break;
    case 'i': options.fSrcRunID = atoi( RAT::optarg ); break;
    case 'x': options.fSrcPosX = atof( RAT::optarg ); break;
    case 'y': options.fSrcPosY = atof( RAT::optarg );
    case 'z': options.fSrcPosZ = atof( RAT::optarg ); break;
    case 'w': options.fSrcWL = atoi( RAT::optarg ); break;
    case 'g': options.fSrcGTO = atof( RAT::optarg ); break;
    case 'f': options.fSrcFill = atof( RAT::optarg ); break;

    }
    
    c = getopt_long(argc, argv, "h:r:s:i:x:y:z:w:g:f:", opts, &option_index);
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
  cout << "SNO+ LOCAS - db2soc" << "\n";
  cout << "Description: Executable to update values from the RAT or LOCAS database to the SOC files. \n";
  cout << "Usage: db2soc [-h] [-r run-id] [-s source-run-id] [-i source-id] \n [-x source-pos-x] [-y source-pos-y] [-z source-pos-z] \n [-w source-wavelength] [-g source-global-time-offset] [-f fill-shadowing-values] \n";
  cout << " -h, --help                            Display this help message and exit \n";
  cout << " -r, --run-id                          Set the run ID for the corresponding SOC run file to be updated \n";
  cout << " -s, --source-run-id                   Set the run ID in the SOC file specified by the '-r' option above \n";
  cout << " -i, --source-id                       Set the source ID in the SOC file \n";
  cout << " -x, --source-pos-x                    Set the source manip position [x-coordinate] \n";
  cout << " -x, --source-pos-y                    Set the source manip position [y-coordinate] \n";
  cout << " -x, --source-pos-z                    Set the source manip position [z-coordinate] \n";
  cout << " -w, --source-wavelength               Set the source wavelength \n";
  cout << " -g, --source-global-time-offset       Set the source global time offset \n";
  cout << " -f, --fill-shadowing-values           Set the ID of the corresponding shadowing file to fill from \n";
  
}
