///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: qrdt2locas.cc
///
/// EXECUTABLE: qrdt2locas
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

#include <iostream>

#include <sstream>

#include "RAT/getopt.h"
#include "RAT/DU/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/DU/Utility.hh"
#include "RAT/Log.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASLightPath.hh"

#include "QRdt.hh"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace LOCAS;
using namespace RAT;
using namespace RAT::DS;
using namespace RAT::DU;

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

  //////////////////////////////////////////////////////////////
  // First load all the PMT information from the RAT database //
  //////////////////////////////////////////////////////////////
  DB* db = DB::Get();
  string data = getenv("GLG4DATA");
  Log::Assert( data != "", "DSReader::BeginOfRun: GLG4DATA is empty, where is the data?" );
  db->LoadDefaults();
  db->Load( db->GetLink("DETECTOR")->GetS( "geo_file" ) );
  db->Load( db->GetLink("DETECTOR")->GetS( "pmt_info_file" ) );

  RAT::DU::Utility::Get()->BeginOfRun();
  PMTInfo pmtInfo = Utility::Get()->GetPMTInfo();
  //////////////////////////////////////////////////////////////

  // Parse arguments passed to the command line
  LOCASCmdOptions Opts = ParseArguments( argc, argv );

  // Define the run IDs of the main-run, central- and wavelength-run files.
  // Same as above but as strings.
  std::string rIDStr = Opts.fRIDStr;
  std::string crIDStr = Opts.fCRIDStr;
  std::string wrIDStr = Opts.fWRIDStr;

  LOCASDB lDB;
  std::string rdtDir = lDB.GetRDTRunDir( "oct03" );

  std::string mainRunFile = rdtDir + "sno_0" + rIDStr + "_p0.rdt";
  std::string centralRunFile = rdtDir + "sno_0" + crIDStr + "_p0.rdt";
  std::string wavelengthRunFile = rdtDir + "sno_0" + wrIDStr + "_p0.rdt";

  cout << mainRunFile << endl;
  cout << centralRunFile << endl;
  cout << wavelengthRunFile << endl;

  // Load the QRdt file
  QRdt myrQRdt( mainRunFile.c_str() );
  QRdt mycrQRdt( centralRunFile.c_str() );
  QRdt mywrQRdt( wavelengthRunFile.c_str() );


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
