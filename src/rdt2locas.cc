///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: rdt2locas.cc
///
/// EXECUTABLE: rdt2locas
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
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/Log.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASLightPath.hh"
#include "LOCASMath.hh"

#include "QRdt.hh"

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

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
  LightPathCalculator lightPath = Utility::Get()->GetLightPathCalculator();
  lightPath.LoadShadowingGeometryInfo();
  //////////////////////////////////////////////////////////////

  //cout << "Number of PMT is: " << pmtInfo.GetCount() << endl;

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
  QRdt rQRdt( mainRunFile.c_str() );
  QRdt crQRdt( centralRunFile.c_str() );
  QRdt wrQRdt( wavelengthRunFile.c_str() );

  // Create the LOCASRun object
  LOCASRun* lRunPtr = new LOCASRun();

  // Set the run IDs for the main, central and wavelength runs
  lRunPtr->SetRunID( Opts.fRID );
  lRunPtr->SetCentralRunID( Opts.fCRID );
  lRunPtr->SetWavelengthRunID( Opts.fWRID );


  // Set the source ID as a string object
  std::string sourceIDStr = "laserball";
  lRunPtr->SetSourceID( sourceIDStr );
  lRunPtr->SetCentralSourceID( sourceIDStr );
  lRunPtr->SetWavelengthSourceID( sourceIDStr );

  // Set the number of pulses from each run as well as the wavelength used for each laser
  lRunPtr->SetLambda( rQRdt.GetLambda() );
  lRunPtr->SetNLBPulses( rQRdt.GetNpulses() );
  lRunPtr->SetCentralLambda( crQRdt.GetLambda() );
  lRunPtr->SetCentralNLBPulses( crQRdt.GetNpulses() );
  lRunPtr->SetWavelengthLambda( wrQRdt.GetLambda() );
  lRunPtr->SetWavelengthNLBPulses( wrQRdt.GetNpulses() );

  // Set the position of the laserball in each of the main, central and wavelength runs
  lRunPtr->SetLBPos( 10.0 * (*rQRdt.GetFullFitPos()) );
  lRunPtr->SetCentralLBPos( 10.0 * (*crQRdt.GetFullFitPos()) );
  lRunPtr->SetWavelengthLBPos( 10.0 * (*wrQRdt.GetFullFitPos()) );

  // Set the laserball Theta,Phi (i.e. the orientation)
  lRunPtr->SetLBTheta( 0.0 );
  lRunPtr->SetLBPhi( rQRdt.GetOrientation() );
  lRunPtr->SetCentralLBTheta( 0.0 );
  lRunPtr->SetCentralLBPhi( crQRdt.GetOrientation() );
  lRunPtr->SetWavelengthLBTheta( 0.0 );
  lRunPtr->SetWavelengthLBPhi( wrQRdt.GetOrientation() );

  /// The possible PMT types in SNO+
  //enum PMTInfo::EPMTType  { NORMAL = 1, OWL = 2, LOWGAIN = 3, BUTT = 4, NECK = 5, CALIB = 6, SPARE = 10, INVALID = 11, BLOWN75 = 12 };

  // The following assumes that the data on the RDT files is indexed by PMT ID.
  for ( Int_t iPMT = 0; iPMT < pmtInfo.GetCount(); iPMT++ ){

    enum PMTInfo::EPMTType pmtEnum = pmtInfo.GetType( iPMT );

    if( pmtEnum == 1 ){
      
      LOCASPMT lPMT( iPMT );
      lPMT.SetID( iPMT );

      // Run IDs
      lPMT.SetRunID( Opts.fRID );
      lPMT.SetCentralRunID( Opts.fCRID );
      lPMT.SetWavelengthRunID( Opts.fWRID );

      // PMT Type
      lPMT.SetType( 1 );

      // Is PMT Verified
      lPMT.SetIsVerified( false );
      lPMT.SetCentralIsVerified( false );
      lPMT.SetWavelengthIsVerified( false );

      // PMT Position and Normal Vector
      lPMT.SetPos( pmtInfo.GetPosition( iPMT ) );
      lPMT.SetNorm( -1.0 * pmtInfo.GetDirection( iPMT ) );

      // PMT Prompt Peak Times and widths
      lPMT.SetPromptPeakTime( rQRdt.GetTimePeak( iPMT ) );
      lPMT.SetPromptPeakWidth( rQRdt.GetTimeWidth( iPMT ) );
      lPMT.SetCentralPromptPeakTime( crQRdt.GetTimePeak( iPMT ) );
      lPMT.SetCentralPromptPeakWidth( crQRdt.GetTimeWidth( iPMT ) );
      lPMT.SetWavelengthPromptPeakTime( wrQRdt.GetTimePeak( iPMT ) );
      lPMT.SetWavelengthPromptPeakWidth( wrQRdt.GetTimeWidth( iPMT ) );

      // ToF from Manipulator
      lPMT.SetTimeOfFlight( rQRdt.GetToF( iPMT ) );
      lPMT.SetCentralTimeOfFlight( crQRdt.GetToF( iPMT ) );
      lPMT.SetWavelengthTimeOfFlight( wrQRdt.GetToF( iPMT ) );
      
      // PMT Prompt Occupancies
      lPMT.SetOccupancy( rQRdt.GetOccupancy( iPMT ) );
      lPMT.SetOccupancyErr( TMath::Sqrt( lPMT.GetOccupancy() ) );
      lPMT.SetCentralOccupancy( crQRdt.GetOccupancy( iPMT ) );
      lPMT.SetCentralOccupancyErr( TMath::Sqrt( lPMT.GetCentralOccupancy() ) ); 
      lPMT.SetWavelengthOccupancy( wrQRdt.GetOccupancy( iPMT ) );
      lPMT.SetWavelengthOccupancyErr( TMath::Sqrt( lPMT.GetWavelengthOccupancy() ) );

      // Number of laserball pulses
      lPMT.SetNLBPulses( rQRdt.GetNpulses() );
      lPMT.SetCentralNLBPulses( crQRdt.GetNpulses() );
      lPMT.SetWavelengthNLBPulses( wrQRdt.GetNpulses() );

      // MPE corrected occupancies
      lPMT.SetMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( lPMT.GetOccupancy(), lPMT.GetNLBPulses() ) );
      lPMT.SetMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( lPMT.GetOccupancy(), lPMT.GetNLBPulses() ) );
      lPMT.SetCentralMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( lPMT.GetCentralOccupancy(), lPMT.GetCentralNLBPulses() ) );
      lPMT.SetCentralMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( lPMT.GetCentralOccupancy(), lPMT.GetCentralNLBPulses() ) );
      lPMT.SetWavelengthMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( lPMT.GetWavelengthOccupancy(), lPMT.GetWavelengthNLBPulses() ) );
      lPMT.SetWavelengthMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( lPMT.GetWavelengthOccupancy(), lPMT.GetWavelengthNLBPulses() ) );

      // Distances through the heavy water, acrylic and water regions

      ///////////// OFF AXIS /////////////
      lightPath.CalcByPosition( lRunPtr->GetLBPos(), lPMT.GetPos() );
      
      lPMT.SetDistInScint( lightPath.GetDistInScint() );
      lPMT.SetDistInAV( lightPath.GetDistInAV() );
      lPMT.SetDistInWater( lightPath.GetDistInWater() );

      lPMT.SetNeckFlag( lightPath.GetXAVNeck() );
      lPMT.SetDistInNeck( lightPath.GetDistInNeckScint() 
                          + lightPath.GetDistInNeckAV() 
                          + lightPath.GetDistInNeckWater() );

      lPMT.SetInitialLBVec( lightPath.GetInitialLightVec() );
      lPMT.SetIncidentLBVec( lightPath.GetIncidentVecOnPMT() );

      lightPath.CalculateFresnelTRCoeff();
      lPMT.SetFresnelTCoeff( lightPath.GetFresnelTCoeff() );

      lightPath.CalculateSolidAngle( lPMT.GetNorm(), 0 );
      lPMT.SetSolidAngle( lightPath.GetSolidAngle() );

      lightPath.CalculateCosThetaPMT( lPMT.GetID() );
      lPMT.SetCosTheta( lightPath.GetCosThetaAvg() );

      if ( lightPath.CheckForShadowing() == true ){ lPMT.SetBadPath( true ); }
      else{ lPMT.SetBadPath( false ); }

      ///////////// CENTRAL /////////////
      TVector3 origin( 0.0, 0.0, 0.0 );
      lightPath.CalcByPosition( origin, lPMT.GetPos() );
      
      lPMT.SetCentralDistInScint( lightPath.GetDistInScint() );
      lPMT.SetCentralDistInAV( lightPath.GetDistInAV() );
      lPMT.SetCentralDistInWater( lightPath.GetDistInWater() );

      lPMT.SetCentralNeckFlag( lightPath.GetXAVNeck() );
      lPMT.SetCentralDistInNeck( lightPath.GetDistInNeckScint() 
                          + lightPath.GetDistInNeckAV() 
                          + lightPath.GetDistInNeckWater() );

      lPMT.SetCentralInitialLBVec( lightPath.GetInitialLightVec() );
      lPMT.SetCentralIncidentLBVec( lightPath.GetIncidentVecOnPMT() );

      lightPath.CalculateFresnelTRCoeff();
      lPMT.SetCentralFresnelTCoeff( lightPath.GetFresnelTCoeff() );

      lightPath.CalculateSolidAngle( lPMT.GetNorm(), 0 );
      lPMT.SetCentralSolidAngle( lightPath.GetSolidAngle() );

      lightPath.CalculateCosThetaPMT( lPMT.GetID() );
      lPMT.SetCentralCosTheta( lightPath.GetCosThetaAvg() );

      if ( lightPath.CheckForShadowing() == true ){ lPMT.SetCentralBadPath( true ); }
      else{ lPMT.SetCentralBadPath( false ); }

      ///////////// WAVELENGTH /////////////
      lightPath.CalcByPosition( lRunPtr->GetLBPos(), lPMT.GetPos() );
      
      lPMT.SetWavelengthDistInScint( lightPath.GetDistInScint() );
      lPMT.SetWavelengthDistInAV( lightPath.GetDistInAV() );
      lPMT.SetWavelengthDistInWater( lightPath.GetDistInWater() );

      lPMT.SetWavelengthNeckFlag( lightPath.GetXAVNeck() );
      lPMT.SetWavelengthDistInNeck( lightPath.GetDistInNeckScint() 
                          + lightPath.GetDistInNeckAV() 
                          + lightPath.GetDistInNeckWater() );

      lPMT.SetWavelengthInitialLBVec( lightPath.GetInitialLightVec() );
      lPMT.SetWavelengthIncidentLBVec( lightPath.GetIncidentVecOnPMT() );

      lightPath.CalculateFresnelTRCoeff();
      lPMT.SetWavelengthFresnelTCoeff( lightPath.GetFresnelTCoeff() );

      lightPath.CalculateSolidAngle( lPMT.GetNorm(), 0 );
      lPMT.SetWavelengthSolidAngle( lightPath.GetSolidAngle() );

      lightPath.CalculateCosThetaPMT( lPMT.GetID() );
      lPMT.SetWavelengthCosTheta( lightPath.GetCosThetaAvg() );

      if ( lightPath.CheckForShadowing() == true ){ lPMT.SetWavelengthBadPath( true ); }
      else{ lPMT.SetWavelengthBadPath( false ); }

      lRunPtr->AddLOCASPMT( lPMT );

    }

  }

  // Now create a LOCASRun (.root) file to store the information of the main-run

  // Get the directory to where the LOCASRun file will be written
  // Currently this is ${LOCAS_DATA}/runs/locasrun
  std::string locasRunDir = lDB.GetLOCASRunDir();
  TFile* file = TFile::Open( ( locasRunDir + rIDStr + (string)"_LOCASRun.root" ).c_str(), "RECREATE" );

  // Create the Run Tree
  TTree* runTree = new TTree( "LOCASRunT", "LOCAS Run Tree" );

  // Declare a new branch pointing to the data stored in the lRun object
  runTree->Branch( "LOCASRun", lRunPtr->ClassName(), &(*lRunPtr), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

  cout << "LOCASRun file: " << endl;
  cout << ( locasRunDir + rIDStr + (string)"_LOCASRun.root" ).c_str() << endl;
  cout << "has been created." << endl;
  cout << "\n";

  
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
