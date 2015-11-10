///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: rdt2locas.cc
///
/// EXECUTABLE: rdt2locas
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
///         central-run file +   ---->  soc2locas ---->  OCARun File
///         wavelength-run file  ---->  
///                             
///         The OCARun file contains all the required PMT information
///         and corrections for the main-run file ONLY.
///
///         Example Usage (at command line):
///
///              soc2locas -r [main-run-id] -c [central-run-id] -w [wavelength-run-id]
///              e.g. soc2locas -r 12121953 -c 30091953 -w 18091989
///
///         soc2locas will then output a file "12121953_OCARun.root" to 
///         ${OCA_DATA}/data/runs/locasrun. 
///
///         Currently BOTH a main-run and central-run file is required. The wavelength
///         run file is optional.
///
///////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <sstream>

#include <getopt.h>
#include "RAT/DU/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/DU/Utility.hh"
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/Log.hh"

#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"
#include "OCAMath.hh"
#include "OCARunReader.hh"

#include "QRdt.h"
#include "QOCAReflect.h"
#include "QDQXX.h"
#include "QOptics.h"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#ifdef M_PI
#undef M_PI
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifdef M_PI_2
#undef M_PI_2
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

using namespace std;
using namespace RAT;
using namespace OCA;
using namespace RAT;
using namespace RAT::DS;
using namespace RAT::DU;

// Utility class to parse the command line arguments for this executable
// Current options: -r (Main-Run-ID), -h (Help)
class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fRID( -1 ), fRIDStr( "" ) { }
  Long64_t fRID;
  std::string fRIDStr;
};

// Declare the three function prototypes used 
OCACmdOptions ParseArguments( int argc, char** argv );
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
  db->Load( ( data + "/geo/sno_d2o.geo" ).c_str() );
  db->Load( ( data + "/pmt/snoman.ratdb" ).c_str() );

  RAT::DU::Utility::Get()->BeginOfRun();
  PMTInfo pmtInfo = Utility::Get()->GetPMTInfo();
  LightPathCalculator lightPath = Utility::Get()->GetLightPathCalculator();
  //////////////////////////////////////////////////////////////

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );

  // Define the run IDs of the main-run, central- and wavelength-run files.
  // Same as above but as strings.
  std::string rIDStr = Opts.fRIDStr;

  OCADB lDB;
  std::string rdtDir = lDB.GetRDTRunDir( "oct03" );

  std::string mainRunFile0 = rdtDir + "sno_0" + rIDStr + "_p0.rdt";
  std::string mainRunFile1 = rdtDir + "sno_0" + rIDStr + "_p1.rdt";
  cout << "mainRunFile0: " << mainRunFile0 << endl;
  cout << "mainRunFile1: " << mainRunFile1 << endl;
  std::string mainRunFile;

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
  string DQXXDirPrefix = lDB.GetDQXXDir( "oct03" );
  DQXXDirPrefix += "/DQXX_00000";
  string rDQXXFile = DQXXDirPrefix + rIDStr + ".dat";

  QDQXX rDQXX( rDQXXFile.c_str() );
  Int_t iPMTStatus = KCCC_TUBE_ON_LINE;
  
  SOC* socFile = new SOC();

  // Set the calib values
  socFile->GetCalib().SetID( (Int_t)rQRdt.GetLambda() );
  socFile->GetCalib().SetMode( (Int_t)rQRdt.GetLambda() );
  socFile->GetCalib().SetTime( 350.0 );
  socFile->GetCalib().SetPos( 10.0 * (*rQRdt.GetManipPos()) );
  TVector3 lbDir( 1.0, 0.0, 0.0 );
  lbDir.SetPhi( ( M_PI_2 ) * rQRdt.GetOrientation() );
  socFile->GetCalib().SetDir( lbDir );
  socFile->GetCalib().SetIntensity( -1000 );

  socFile->SetSourceID( "laserball" );
  socFile->SetRunID( Opts.fRID );
  socFile->SetGlobalTimeOffset( socFile->GetCalib().GetTime() );
  socFile->SetNPulsesTriggered( rQRdt.GetNpulses() );

  FitResult socFitResult;
  socFitResult.Reset();
  FitVertex fittedVertex;
  fittedVertex.SetPosition( 10.0 * (*rQRdt.GetFullFitPos()), true );
  fittedVertex.SetPositivePositionError( ROOT::Math::XYZVectorF( 50.0, 50.0, 50.0 ), true );
  fittedVertex.SetNegativePositionError( ROOT::Math::XYZVectorF( 50.0, 50.0, 50.0 ), true );

  fittedVertex.SetTime( 350.0 );
  fittedVertex.SetTimeValid( true );
  fittedVertex.SetPositiveTimeError( 5.0, true );
  fittedVertex.SetNegativeTimeError( 5.0, true );
  socFitResult.SetVertex( 0, fittedVertex );

  socFile->SetFitResult( "lbFit", socFitResult );

  for ( Int_t iPMT = 0; iPMT < (Int_t)pmtInfo.GetCount(); iPMT++ ){

    enum PMTInfo::EPMTType pmtEnum = pmtInfo.GetType( iPMT );

    if( pmtEnum == 1 
        && rDQXX.LcnInfo( iPMT, iPMTStatus ) == 1 ){

      if ( !socFile->SOCPMTExists( iPMT ) ){ socFile->AddSOCPMT( iPMT ); }
      socFile->GetSOCPMT( iPMT ).SetID( iPMT );
      socFile->GetSOCPMT( iPMT ).AddTime( rQRdt.GetTimePeak( (Float_t)iPMT ) );
      socFile->GetSOCPMT( iPMT ).SetTOFManipulator( rQRdt.GetToF( (Float_t)iPMT ) );
      socFile->GetSOCPMT( iPMT ).SetPromptOccupancy( rQRdt.GetOccupancy( (Float_t)iPMT ) );
      socFile->GetSOCPMT( iPMT ).SetTimeCentroid( rQRdt.GetTimePeak( (Float_t)iPMT ),
                                                  rQRdt.GetTimeWidth( (Float_t)iPMT ) ); 
      
      DS::SOCPMT::EPeakFindStatus peakOk = DS::SOCPMT::eSuccess;
      socFile->GetSOCPMT( iPMT ).SetPeakFindOK( peakOk );
  
    }
  }

  std::string socRunDir = lDB.GetSOCRunDir( "oct03" );
  std::string fileName = ( socRunDir + rIDStr + "_Run.root" );
  
  TFile* myFile = new TFile( fileName.c_str(), "RECREATE" );
  TTree* myTree = new TTree( "T", "RAT Tree" );
  myTree->Branch( "soc", socFile->ClassName(), &socFile, 32000, 99 );
  myTree->Fill();
  myTree->Write();
  myFile->Close();

  cout << "The SOC File: " << fileName << " has been created" << endl;

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
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:r:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': cout << "HELP GOES HERE" << endl; break;
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
