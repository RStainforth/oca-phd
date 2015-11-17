///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: rdt2soc.cc
///
/// EXECUTABLE: rdt2soc
///
/// BRIEF: This executable is designed to convert the original RDT file format
///        to a RAT::DS::SOC object (SOC) file. The original RDT files from SNO
///        are similar to the SOC file formats in SNO+. Unfortunately the RDT files
///        do not contain the complete timing (TACs) information on them, only the prompt
///        (fitted) values for the occupancy and the time. Therefore the resulting SOC
///        files produced by this executable only contain one 'time' value per SOCPMT object.
///        Whilst this isn't ideal, without the original ZDAB files this is the best for now. 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     11/2015 : RPFS - First Revision, new file.
///
/// DETAIL: Executable to process a RDT SNO file and convert it into a 
///         SNO+ SOC file.
///
///         The approach is as follows:
///
///         sno-rdt-file  -----> rdt2soc -----> SOCFile
///
///         Example Usage (at command line):
///
///              rdt2soc -r [main-run-id] -d [date-set]
///
///         soc2locas will then output a file "12121953_OCARun.root" to 
///         ${OCA_SNOPLUS_ROOT}/data/runs/soc/date-set directory 
///
///////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>

#include <getopt.h>
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/DU/Utility.hh"
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/Log.hh"

#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include "QRdt.h"
#include "QDQXX.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace RAT;
using namespace OCA;

// Utility class to parse the command line arguments for this executable
// Current options: -r (Main-Run-ID), -d (date-directory) -h (Help)
class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fRID( -1 ), fRIDStr( "" ), fMMYY( "" ) { }
  Long64_t fRID;
  std::string fRIDStr, fMMYY;
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

  cout << "\n";
  cout << "###############################" << endl;
  cout << "######## RDT2SOC START ########" << endl;
  cout << "###############################" << endl;
  cout << "\n";

  // First load all the PMT and detector information from the RAT database
  // Need to make sure we load the PMT positions as they were from SNO
  // i.e. load the pmt/snoman.ratdb PMT file, not the airfill.ratdb ones used
  // for SNO+.
  DB* db = DB::Get();
  string data = getenv("GLG4DATA");
  Log::Assert( data != "", "DSReader::BeginOfRun: GLG4DATA is empty, where is the data?" );
  db->LoadDefaults();

  // Load the snoman (SNO) PMt positions
  db->Load( ( data + "/geo/sno_d2o.geo" ).c_str() );
  db->Load( ( data + "/pmt/snoman.ratdb" ).c_str() );

  DU::Utility::Get()->BeginOfRun();
  DU::PMTInfo pmtInfo = DU::Utility::Get()->GetPMTInfo();
  //////////////////////////////////////////////////////////////

  // Define the run IDs of the main-run, central- and wavelength-run files.
  // Same as above but as strings.
  string rIDStr = Opts.fRIDStr;

  OCADB lDB;
  string rdtDir = lDB.GetRDTRunDir( Opts.fMMYY );

  // The p0 or p1 denote different 'passes', we alsways want
  // the most recent pass (p1) if it exists.
  string mainRunFile0 = rdtDir + "sno_0" + rIDStr + "_p0.rdt";
  string mainRunFile1 = rdtDir + "sno_0" + rIDStr + "_p1.rdt";
  string mainRunFile;

  // Check that either of the files exist, and select accordingly
  // depending on whether we have a p1 file version or not.
  ifstream fileR0( mainRunFile0.c_str() );
  ifstream fileR1( mainRunFile1.c_str() );
  if ( fileR1.good() ){ mainRunFile = mainRunFile1; }
  if ( !fileR1.good() && fileR0.good() ){ mainRunFile = mainRunFile0; }

  cout << "Off-Axis Run File: " << mainRunFile << endl;

  // Load the QRdt file
  QRdt rQRdt( mainRunFile.c_str() );

  // Initialise the DQXX objects
  // Set the path prefix for the location of the DQXX files
  // This assumes that the envrionment variable $DQXXDIR is set
  string DQXXDirPrefix = lDB.GetDQXXDir( Opts.fMMYY );
  DQXXDirPrefix += "/DQXX_00000";
  string rDQXXFile = DQXXDirPrefix + rIDStr + ".dat";

  QDQXX rDQXX( rDQXXFile.c_str() );
  Int_t iPMTStatus = KCCC_TUBE_ON_LINE;
  
  // Create a new SOC object.
  DS::SOC* socFile = new DS::SOC();

  // Now we can set the values on the SOC data structure.

  // Set the RAT::DS:Calib object values
  socFile->GetCalib().SetID( (Int_t)rQRdt.GetLambda() );
  socFile->GetCalib().SetMode( (Int_t)rQRdt.GetLambda() );
  socFile->GetCalib().SetTime( 350.0 );
  socFile->GetCalib().SetPos( 10.0 * (*rQRdt.GetManipPos()) );
  TVector3 lbDir( 1.0, 0.0, 0.0 );
  lbDir.SetPhi( ( TMath::PiOver2() ) * rQRdt.GetOrientation() );
  socFile->GetCalib().SetDir( lbDir );

  // This field is for MC only, so we set to an unphysical value.
  socFile->GetCalib().SetIntensity( -1000 );

  // Set the source information
  socFile->SetSourceID( "laserball" );
  socFile->SetRunID( Opts.fRID );
  socFile->SetGlobalTimeOffset( socFile->GetCalib().GetTime() );
  socFile->SetNPulsesTriggered( rQRdt.GetNpulses() );

  // Add the fitted laserball position using the conventional RAT::DS::FitResult
  /// data structure.
  DS::FitResult socFitResult;
  socFitResult.Reset();
  DS::FitVertex fittedVertex;
  fittedVertex.SetPosition( 10.0 * (*rQRdt.GetFullFitPos()), true );
  fittedVertex.SetPositivePositionError( ROOT::Math::XYZVectorF( 50.0, 50.0, 50.0 ), true );
  fittedVertex.SetNegativePositionError( ROOT::Math::XYZVectorF( 50.0, 50.0, 50.0 ), true );

  // The 'Time' is just the t0 in the event window, nominall we
  // set this to 350.0 to coincide with the MC data for SNO+.
  fittedVertex.SetTime( 350.0 );
  fittedVertex.SetTimeValid( true );

  // Set erros equal to that of the sliding window (conservative estimate)
  fittedVertex.SetPositiveTimeError( 2.5, true );
  fittedVertex.SetNegativeTimeError( 2.5, true );
  socFitResult.SetVertex( 0, fittedVertex );

  // The name "lbfit" is important, this is what is searched for
  // by the OCARun class during the 'soc2oca' executable.
  socFile->SetFitResult( "lbfit", socFitResult );

  // Now we loop over all the PMT and create RAT::DS::SOCPMT objects and
  // add them to the RAT::DS::SOC object.
  for ( Int_t iPMT = 0; iPMT < (Int_t)pmtInfo.GetCount(); iPMT++ ){

    // Only want to add 'normal' (enum == 1 ) type PMTs at the moment.
    enum DU::PMTInfo::EPMTType pmtEnum = pmtInfo.GetType( iPMT );

    // Only add 'good' PMTs, hence the DQXX check here (DQXX [SNO] = CSS [SNO+]).
    if( pmtEnum == 1 
        && rDQXX.LcnInfo( iPMT, iPMTStatus ) == 1 ){

      // Set the occupancy and timing information.
      if ( !socFile->SOCPMTExists( iPMT ) ){ socFile->AddSOCPMT( iPMT ); }
      socFile->GetSOCPMT( iPMT ).SetID( iPMT );
      socFile->GetSOCPMT( iPMT ).AddTime( rQRdt.GetTimePeak( (Float_t)iPMT ) );
      socFile->GetSOCPMT( iPMT ).SetTOFManipulator( rQRdt.GetToF( (Float_t)iPMT ) );
      socFile->GetSOCPMT( iPMT ).SetPromptOccupancy( rQRdt.GetOccupancy( (Float_t)iPMT ) );
      socFile->GetSOCPMT( iPMT ).SetTimeCentroid( rQRdt.GetTimePeak( (Float_t)iPMT ),
                                                  rQRdt.GetTimeWidth( (Float_t)iPMT ) ); 
      
      // As we are using the fitted values from the RDT files, we assume that
      // they too were successful in fitting.
      DS::SOCPMT::EPeakFindStatus peakOk = DS::SOCPMT::eSuccess;
      socFile->GetSOCPMT( iPMT ).SetPeakFindOK( peakOk );
  
    }
  }

  // Now we retireve the directory information so we now where to
  // save the new SOC file.
  std::string socRunDir = lDB.GetSOCRunDir( Opts.fMMYY );
  std::string fileName = ( socRunDir + rIDStr + "_Run.root" );
  
  // Create the 'T' and 'runT' Trees, the names of the conventional
  // trees on a SOC file as created by RAT. 
  // Note: The 'runT' is a dummy and contains no valuable information
  // as this file wasn't created in RAT, so we have no detector state
  // to put onto it.
  TFile* myFile = new TFile( fileName.c_str(), "RECREATE" );
  myFile->cd();

  // Main 'T' tree for the SOC files.
  TTree* myTree = new TTree( "T", "RAT Tree" );
  myTree->Branch( "soc", socFile->ClassName(), &socFile, 32000, 99 );
  myTree->Fill();
  myTree->Write();

  // Dummy 'runT' tree.
  TTree* runTree = new TTree( "runT", "RAT Run Tree" );
  RAT::DS::Run* runBranch = new RAT::DS::Run();
  runTree->Branch( "run", runBranch->ClassName(), &runBranch, 32000, 99 );
  runTree->Fill();
  runTree->Write();

  // Close the file.
  myFile->Close();
  cout << "The SOC File: " << fileName << " has been created" << endl;

  cout << "\n";
  cout << "#############################" << endl;
  cout << "######## RDT2SOC END ########" << endl;
  cout << "#############################" << endl;
  cout << "\n";

  return 0;  
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
                                  {"date-directory", 1, NULL, 'd'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "hr:d:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); exit(0); break;
    case 'r': options.fRID = atol( optarg ); break;
    case 'd': options.fMMYY = string( optarg ); break;
    }
    
    c = getopt_long(argc, argv, "hr:d:", opts, &option_index);
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
  cout << "SNO+ OCA - rdt2soc" << "\n";
  cout << "Description: This executable converts RDT files from SNO into SOC files for use by 'soc2oca'. \n";
  cout << "Usage: soc2oca [-h] [-r run-id] [-d directory-date]\n";
  cout << " -h, --help                Display this help message and exit \n";
  cout << " -r, --run-id              Set the run ID for the corresponding RDT run file to be processed into a SOC file \n";
  cout << " -d, --date-directory      The name of the folder in ${OCA_SNOPLUS_ROOT}/data/runs/rdt where to obtain the RDT run file from \n";
  
}
