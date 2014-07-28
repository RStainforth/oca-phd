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
#include "LOCASMath.hh"
#include "LOCASFitLBPosition.hh"
#include "LOCASRunReader.hh"

#include "QRdt.h"
#include "QDQXX.h"

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
  lightPath.LoadShadowingGeometryInfo( "geo/sno_d2o.geo" );
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
  QRdt rQRdt( mainRunFile.c_str() );
  QRdt crQRdt( centralRunFile.c_str() );
  QRdt wrQRdt( wavelengthRunFile.c_str() );

  // Initialise the DQXX objects
  // Set the path prefix for the location of the DQXX files
  // This assumes that the envrionment variable $DQXXDIR is set
  string DQXXDirPrefix = getenv("DQXXDIR");
  DQXXDirPrefix += "/DQXX_00000";
  string rDQXXFile = DQXXDirPrefix + rIDStr + ".dat";
  string crDQXXFile = DQXXDirPrefix + crIDStr + ".dat";
  string wrDQXXFile = DQXXDirPrefix + wrIDStr + ".dat";

  QDQXX rDQXX( rDQXXFile.c_str() );
  QDQXX crDQXX( crDQXXFile.c_str() );
  QDQXX wrDQXX( wrDQXXFile.c_str() );

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
  lRunPtr->SetLBPos( 10.0 * (*rQRdt.GetManipPos()) );
  lRunPtr->SetCentralLBPos( 10.0 * (*crQRdt.GetManipPos()) );
  lRunPtr->SetWavelengthLBPos( 10.0 * (*wrQRdt.GetManipPos()) );

  // Set the laserball Theta,Phi (i.e. the orientation)
  lRunPtr->SetLBTheta( 0.0 );
  lRunPtr->SetLBPhi( ( TMath::Pi() / 2.0 ) * rQRdt.GetOrientation() );
  lRunPtr->SetCentralLBTheta( 0.0 );
  lRunPtr->SetCentralLBPhi( ( TMath::Pi() / 2.0 ) * crQRdt.GetOrientation() );
  lRunPtr->SetWavelengthLBTheta( 0.0 );
  lRunPtr->SetWavelengthLBPhi( ( TMath::Pi() / 2.0 ) * wrQRdt.GetOrientation() );

  /// The possible PMT types in SNO+
  //enum PMTInfo::EPMTType  { NORMAL = 1, OWL = 2, LOWGAIN = 3, BUTT = 4, NECK = 5, CALIB = 6, SPARE = 10, INVALID = 11, BLOWN75 = 12 };

  // // The following assumes that the data on the RDT files is indexed by PMT ID.
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

      // DQXX Flags for KCCC_TUBE_ON_LINE
      Int_t iPMTStatus = KCCC_TUBE_ON_LINE;

      lPMT.SetDQXXFlag( rDQXX.LcnInfo( iPMT, iPMTStatus ) );
      lPMT.SetCentralDQXXFlag( crDQXX.LcnInfo( iPMT, iPMTStatus ) );
      lPMT.SetWavelengthDQXXFlag( wrDQXX.LcnInfo( iPMT, iPMTStatus ) );

      // PMT Position and Normal Vector
      lPMT.SetPos( pmtInfo.GetPosition( iPMT ) );
      lPMT.SetNorm( -1.0 * pmtInfo.GetDirection( iPMT ) );

      // PMT Prompt Peak Times and widths
      lPMT.SetPromptPeakTime( rQRdt.GetTimePeak( (Float_t)iPMT ) );
      lPMT.SetPromptPeakWidth( rQRdt.GetTimeWidth( (Float_t)iPMT ) );
      lPMT.SetCentralPromptPeakTime( crQRdt.GetTimePeak( (Float_t)iPMT ) );
      lPMT.SetCentralPromptPeakWidth( crQRdt.GetTimeWidth( (Float_t)iPMT ) );
      lPMT.SetWavelengthPromptPeakTime( wrQRdt.GetTimePeak( (Float_t)iPMT ) );
      lPMT.SetWavelengthPromptPeakWidth( wrQRdt.GetTimeWidth( (Float_t)iPMT ) );

      // ToF from Manipulator
      lPMT.SetTimeOfFlight( rQRdt.GetRchToF( (Float_t)iPMT ) );
      lPMT.SetCentralTimeOfFlight( crQRdt.GetRchToF( (Float_t)iPMT ) );
      lPMT.SetWavelengthTimeOfFlight( wrQRdt.GetRchToF( (Float_t)iPMT ) );
      
      // PMT Prompt Occupancies
      lPMT.SetOccupancy( rQRdt.GetOccupancy( (Float_t)iPMT ) );
      lPMT.SetOccupancyErr( TMath::Sqrt( lPMT.GetOccupancy() ) );
      lPMT.SetCentralOccupancy( crQRdt.GetOccupancy( (Float_t)iPMT ) );
      lPMT.SetCentralOccupancyErr( TMath::Sqrt( lPMT.GetCentralOccupancy() ) ); 
      lPMT.SetWavelengthOccupancy( wrQRdt.GetOccupancy( (Float_t)iPMT ) );
      lPMT.SetWavelengthOccupancyErr( TMath::Sqrt( lPMT.GetWavelengthOccupancy() ) );

      // Number of laserball pulses
      lPMT.SetNLBPulses( rQRdt.GetNpulses() );
      lPMT.SetCentralNLBPulses( crQRdt.GetNpulses() );
      lPMT.SetWavelengthNLBPulses( wrQRdt.GetNpulses() );

      // MPE corrected occupancies
      lPMT.SetMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( lPMT.GetOccupancy(), lPMT.GetNLBPulses() ) );
      lPMT.SetMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( lPMT.GetOccupancy(), lPMT.GetNLBPulses() ) );
      lPMT.SetMPECorrOccupancyCorr( LOCASMath::MPECorrectedNPromptCorr( lPMT.GetMPECorrOccupancy(), lPMT.GetOccupancy(), lPMT.GetNLBPulses() ) );
      lPMT.SetCentralMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( lPMT.GetCentralOccupancy(), lPMT.GetCentralNLBPulses() ) );
      lPMT.SetCentralMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( lPMT.GetCentralOccupancy(), lPMT.GetCentralNLBPulses() ) );
      lPMT.SetMPECorrOccupancyCorr( LOCASMath::MPECorrectedNPromptCorr( lPMT.GetCentralMPECorrOccupancy(), lPMT.GetCentralOccupancy(), lPMT.GetCentralNLBPulses() ) );
      lPMT.SetWavelengthMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( lPMT.GetWavelengthOccupancy(), lPMT.GetWavelengthNLBPulses() ) );
      lPMT.SetWavelengthMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( lPMT.GetWavelengthOccupancy(), lPMT.GetWavelengthNLBPulses() ) );
      lPMT.SetWavelengthMPECorrOccupancyCorr( LOCASMath::MPECorrectedNPromptCorr( lPMT.GetWavelengthMPECorrOccupancy(), lPMT.GetWavelengthOccupancy(), lPMT.GetWavelengthNLBPulses() ) );

      // Theta, Phi values of the PMT relative to the Laserball Position
      TVector3 lbRelToPMT = ( lPMT.GetPos() - ( lRunPtr->GetLBPos() ) ).Unit();
      TVector3 lbRelToPMTWvl = ( lPMT.GetPos() - ( lRunPtr->GetWavelengthLBPos() ) ).Unit();
      TVector3 lbRelToPMTCtr = ( lPMT.GetPos() - ( lRunPtr->GetCentralLBPos() ) ).Unit();

      ///////// Off-Axis Laserball Theta and Phi Angles //////////

      TVector3 lbAxis( 0.0, 0.0, 1.0 );
      lbAxis.SetPhi( lRunPtr->GetLBPhi() );
      lbAxis.SetTheta( lRunPtr->GetLBTheta() );

      lPMT.SetRelLBTheta( lbRelToPMT.Angle( lbAxis ) );
      Float_t laserPhi = lbRelToPMT.Phi();
      Float_t relLBPhi = fmod( (Float_t)( laserPhi + lbAxis.Phi() ), 2.0 * TMath::Pi() ); 
      lPMT.SetRelLBPhi( relLBPhi );

      ///////// Central Laserball Theta and Phi Angles //////////

      TVector3 lbAxisCtr( 0.0, 0.0, 1.0 );
      lbAxisCtr.SetPhi( lRunPtr->GetCentralLBPhi() );
      lbAxisCtr.SetTheta( lRunPtr->GetCentralLBTheta() );

      lPMT.SetCentralRelLBTheta( lbRelToPMTCtr.Angle( lbAxisCtr ) );
      Float_t laserPhiCtr = lbRelToPMTCtr.Phi();
      Float_t relLBPhiCtr = fmod( (Float_t)( laserPhiCtr + lbAxisCtr.Phi() ), 2.0 * TMath::Pi() ); 
      lPMT.SetCentralRelLBPhi( relLBPhiCtr );

      ///////// Wavelength Laserball Theta and Phi Angles //////////

      TVector3 lbAxisWvl( 0.0, 0.0, 1.0 );
      lbAxisWvl.SetPhi( lRunPtr->GetWavelengthLBPhi() );
      lbAxisWvl.SetTheta( lRunPtr->GetWavelengthLBTheta() );

      lPMT.SetWavelengthRelLBTheta( lbRelToPMTWvl.Angle( lbAxisWvl ) );
      Float_t laserPhiWvl = lbRelToPMTWvl.Phi();
      Float_t relLBPhiWvl = fmod( (Float_t)( laserPhiWvl + lbAxisWvl.Phi() ), 2.0 * TMath::Pi() ); 
      lPMT.SetWavelengthRelLBPhi( relLBPhiWvl );

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

      if ( lightPath.CheckForShadowing() == true 
           || lightPath.GetTIR() == true 
           || lightPath.GetResvHit() == true ){ lPMT.SetBadPath( true ); }
      else{ lPMT.SetBadPath( false ); }
      
      ///////////// CENTRAL /////////////
      lightPath.CalcByPosition( lRunPtr->GetCentralLBPos(), lPMT.GetPos() );
      
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

      if ( lightPath.CheckForShadowing() == true 
           || lightPath.GetTIR() == true
           || lightPath.GetResvHit() == true ){ lPMT.SetCentralBadPath( true ); }
      else{ lPMT.SetCentralBadPath( false ); }

      ///////////// WAVELENGTH /////////////
      lightPath.CalcByPosition( lRunPtr->GetWavelengthLBPos(), lPMT.GetPos() );
      
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

      if ( lightPath.CheckForShadowing() == true 
           || lightPath.GetTIR() == true
           || lightPath.GetResvHit() == true ){ lPMT.SetWavelengthBadPath( true ); }
      else{ lPMT.SetWavelengthBadPath( false ); }

      lPMT.VerifyPMT();

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

  return 0;
  
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
