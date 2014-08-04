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

  QOptics fOpticsR;
  QOptics fOpticsC;
  QOptics fOpticsW;
  fOpticsR.SetFastMode(0,0);
  fOpticsC.SetFastMode(0,0);
  fOpticsW.SetFastMode(0,0);
  QOCAReflect *reflR;
  QOCAReflect *reflC;
  QOCAReflect *reflW;
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

  std::string mainRunFile0 = rdtDir + "sno_0" + rIDStr + "_p0.rdt";
  std::string centralRunFile0 = rdtDir + "sno_0" + crIDStr + "_p0.rdt";
  std::string wavelengthRunFile0 = rdtDir + "sno_0" + wrIDStr + "_p0.rdt";
  std::string mainRunFile1 = rdtDir + "sno_0" + rIDStr + "_p1.rdt";
  std::string centralRunFile1 = rdtDir + "sno_0" + crIDStr + "_p1.rdt";
  std::string wavelengthRunFile1 = rdtDir + "sno_0" + wrIDStr + "_p1.rdt";

  std::string mainRunFile;
  std::string centralRunFile;
  std::string wavelengthRunFile;

  ifstream fileR0( mainRunFile0.c_str() );
  ifstream fileR1( mainRunFile1.c_str() );
  if ( fileR1.good() ){ mainRunFile = mainRunFile1; }
  else if ( !fileR1.good() && fileR0.good() ){ mainRunFile = mainRunFile0; }

  ifstream fileC0( centralRunFile0.c_str() );
  ifstream fileC1( centralRunFile1.c_str() );
  if ( fileC1.good() ){ centralRunFile = centralRunFile1; }
  else if ( !fileC1.good() && fileC0.good() ){ centralRunFile = centralRunFile0; }

  ifstream fileW0( wavelengthRunFile0.c_str() );
  ifstream fileW1( wavelengthRunFile1.c_str() );
  if ( fileW1.good() ){ wavelengthRunFile = wavelengthRunFile1; }
  else if ( !fileW1.good() && fileW0.good() ){ wavelengthRunFile = wavelengthRunFile0; }

  cout << "Off-Axis Run File: " << mainRunFile << endl;
  cout << "Central Run File: " << centralRunFile << endl;
  cout << "Wavelength Run File: " << wavelengthRunFile << endl;

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
  Int_t iPMTStatus = KCCC_TUBE_ON_LINE;
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
  lRunPtr->SetLBPhi( ( M_PI_2 ) * rQRdt.GetOrientation() );
  lRunPtr->SetCentralLBTheta( 0.0 );
  lRunPtr->SetCentralLBPhi( ( M_PI_2 ) * crQRdt.GetOrientation() );
  lRunPtr->SetWavelengthLBTheta( 0.0 );
  lRunPtr->SetWavelengthLBPhi( ( M_PI_2 ) * wrQRdt.GetOrientation() );

  ///////////// OFF AXIS /////////////
  fOpticsR.SetSource( 0.1 * lRunPtr->GetLBPos() );
  fOpticsR.SetIndices( lRunPtr->GetLambda() );
  
  // if ( fOpticsR.GetSource().Mag() > 450.0 ){
  //   reflR = new QOCAReflect();
  //   reflR->SetFastMode(1,1);
  //   reflR->SetSelect(1);
  //   reflR->SetWavelength(fOpticsR.GetWavelength());
  //   reflR->SetTimeWindow((Double_t)8.0);
  //   if ( reflR->SetSourceVector(fOpticsR.GetSource()) ){
  //     Double_t* thps = reflR->GetCThetapsRange();
  //     fOpticsR.SetCosThetapsMin((Double_t) thps[0]);
  //     fOpticsR.SetCosThetapsMax((Double_t) thps[1]);
  //   }
  // }
  // delete reflR; reflR = NULL;
  ///////////// CENTRAL /////////////
  
  fOpticsC.SetSource( 0.1 * lRunPtr->GetCentralLBPos() );
  fOpticsC.SetIndices( lRunPtr->GetCentralLambda() );
  
  // if ( fOpticsC.GetSource().Mag() > 450.0 ){
  //   reflC = new QOCAReflect();
  //   reflC->SetFastMode(1,1);
  //   reflC->SetSelect(1);
  //   reflC->SetWavelength(fOpticsC.GetWavelength());
  //   reflC->SetTimeWindow((Double_t)8.0);
  //   if ( reflC->SetSourceVector(fOpticsC.GetSource()) ){
  //     Double_t* thps = reflC->GetCThetapsRange();
  //     fOpticsC.SetCosThetapsMin((Double_t) thps[0]);
  //     fOpticsC.SetCosThetapsMax((Double_t) thps[1]);
  //   }
  // }
  // delete reflC; reflC = NULL;
  ///////////// WAVELENGTH /////////////
  fOpticsW.SetSource( 0.1 * lRunPtr->GetWavelengthLBPos() );
  fOpticsW.SetIndices( lRunPtr->GetWavelengthLambda() );
  
  // if ( fOpticsW.GetSource().Mag() > 450.0 ){
  //   reflW = new QOCAReflect();
  //   reflW->SetFastMode(1,1);
  //   reflW->SetSelect(1);
  //   reflW->SetWavelength(fOpticsW.GetWavelength());
  //   reflW->SetTimeWindow((Double_t)8.0);
  //   if ( reflW->SetSourceVector(fOpticsW.GetSource()) ){
  //     Double_t* thps = reflW->GetCThetapsRange();
  //     fOpticsW.SetCosThetapsMin((Double_t) thps[0]);
  //     fOpticsW.SetCosThetapsMax((Double_t) thps[1]);
  //   }
  // }
  // delete reflW; reflW = NULL;
  
  /// The possible PMT types in SNO+
  //enum PMTInfo::EPMTType  { NORMAL = 1, OWL = 2, LOWGAIN = 3, BUTT = 4, NECK = 5, CALIB = 6, SPARE = 10, INVALID = 11, BLOWN75 = 12 };

  // // The following assumes that the data on the RDT files is indexed by PMT ID.
  for ( Int_t iPMT = 0; iPMT < pmtInfo.GetCount(); iPMT++ ){

    enum PMTInfo::EPMTType pmtEnum = pmtInfo.GetType( iPMT );

    if( pmtEnum == 1 
        && ( rDQXX.LcnInfo( iPMT, iPMTStatus ) == 1 )
        && ( crDQXX.LcnInfo( iPMT, iPMTStatus ) == 1 )
        && ( wrDQXX.LcnInfo( iPMT, iPMTStatus ) == 1 ) ){
      
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
      //Int_t iPMTStatus = KCCC_TUBE_ON_LINE;

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
      lPMT.SetOccupancyErr( sqrt( lPMT.GetOccupancy() ) );
      lPMT.SetCentralOccupancy( crQRdt.GetOccupancy( (Float_t)iPMT ) );
      lPMT.SetCentralOccupancyErr( sqrt( lPMT.GetCentralOccupancy() ) ); 
      lPMT.SetWavelengthOccupancy( wrQRdt.GetOccupancy( (Float_t)iPMT ) );
      lPMT.SetWavelengthOccupancyErr( sqrt( lPMT.GetWavelengthOccupancy() ) );

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
      Float_t relLBPhi = fmod( (Float_t)( laserPhi + lbAxis.Phi() ), 2.0 * M_PI ); 
      lPMT.SetRelLBPhi( relLBPhi );

      ///////// Central Laserball Theta and Phi Angles //////////

      TVector3 lbAxisCtr( 0.0, 0.0, 1.0 );
      lbAxisCtr.SetPhi( lRunPtr->GetCentralLBPhi() );
      lbAxisCtr.SetTheta( lRunPtr->GetCentralLBTheta() );

      lPMT.SetCentralRelLBTheta( lbRelToPMTCtr.Angle( lbAxisCtr ) );
      Float_t laserPhiCtr = lbRelToPMTCtr.Phi();
      Float_t relLBPhiCtr = fmod( (Float_t)( laserPhiCtr + lbAxisCtr.Phi() ), 2.0 * M_PI ); 
      lPMT.SetCentralRelLBPhi( relLBPhiCtr );

      ///////// Wavelength Laserball Theta and Phi Angles //////////

      TVector3 lbAxisWvl( 0.0, 0.0, 1.0 );
      lbAxisWvl.SetPhi( lRunPtr->GetWavelengthLBPhi() );
      lbAxisWvl.SetTheta( lRunPtr->GetWavelengthLBTheta() );

      lPMT.SetWavelengthRelLBTheta( lbRelToPMTWvl.Angle( lbAxisWvl ) );
      Float_t laserPhiWvl = lbRelToPMTWvl.Phi();
      Float_t relLBPhiWvl = fmod( (Float_t)( laserPhiWvl + lbAxisWvl.Phi() ), 2.0 * M_PI ); 
      lPMT.SetWavelengthRelLBPhi( relLBPhiWvl );


      ///////////// OFF-AXIS /////////////
      
      // Distances through the heavy water, acrylic and water regions
      
      fOpticsR.GetDistances( iPMT );
      lightPath.CalcByPosition( lRunPtr->GetLBPos(), lPMT.GetPos() );
      //lightPath.CalcByPosition( lRunPtr->GetLBPos(), lPMT.GetPos(), 
      //                         lightPath.WavelengthToEnergy( ( lRunPtr->GetLambda() ) * 1e-6 ), 30.0 );
      
      //lPMT.SetDistInScint( 10.0 * fOpticsR.GetDd2o() );
      //lPMT.SetDistInAV( 10.0 * fOpticsR.GetDacrylic() );
      //lPMT.SetDistInWater( 10.0 * fOpticsR.GetDh2o() );
      
      lPMT.SetDistInScint( lightPath.GetDistInScint() );
      lPMT.SetDistInAV( lightPath.GetDistInAV() );
      lPMT.SetDistInWater( lightPath.GetDistInWater() );
      
      lPMT.SetNeckFlag( lightPath.GetXAVNeck() );
      lPMT.SetDistInNeck( lightPath.GetDistInNeckScint() 
                          + lightPath.GetDistInNeckAV() 
                          + lightPath.GetDistInNeckWater() );
      
      lPMT.SetInitialLBVec( lightPath.GetInitialLightVec() );
      lPMT.SetIncidentLBVec( lightPath.GetIncidentVecOnPMT() );
      
      //lightPath.CalculateFresnelTRCoeff();
      //lPMT.SetFresnelTCoeff( lightPath.GetFresnelTCoeff() );
      lPMT.SetFresnelTCoeff( fOpticsR.GetTransPwr() );
      
      //lightPath.CalculateSolidAngle( lPMT.GetNorm(), 0 );
      //lPMT.SetSolidAngle( lightPath.GetSolidAngle() );
      lPMT.SetSolidAngle( fOpticsR.GetPmtSolidAngle( iPMT ) );
      
      //lightPath.CalculateCosThetaPMT( lPMT.GetID() );
      //lPMT.SetCosTheta( lightPath.GetCosThetaAvg() );
      lPMT.SetCosTheta( fOpticsR.GetCtheta() );
      
       if ( fOpticsR.IsBelly() || fOpticsR.IsNeckOptics() ||
            fOpticsR.IsRope() || fOpticsR.IsPipe() ||
            fOpticsR.IsPromptAVRefl() || fOpticsR.IsAnchor()
            || fOpticsR.IsNcd() || fOpticsR.IsNcdRope() ){ lPMT.SetBadPath( true ); }
       else{ lPMT.SetBadPath( false ); }
      //if ( lightPath.CheckForShadowing() == true || lightPath.GetXAVNeck()
      //      || lightPath.GetTIR() || lightPath.GetResvHit() ){ lPMT.SetBadPath( true ); }
      //else{ lPMT.SetBadPath( false ); }
      
      ///////////// CENTRAL /////////////
      fOpticsC.GetDistances( iPMT );
      lightPath.CalcByPosition( lRunPtr->GetCentralLBPos(), lPMT.GetPos() );
      //lightPath.CalcByPosition( lRunPtr->GetCentralLBPos(), lPMT.GetPos(), 
      //                          lightPath.WavelengthToEnergy( ( lRunPtr->GetCentralLambda() ) * 1e-6 ), 30.0 );
      
      //lPMT.SetCentralDistInScint( 10.0 * fOpticsC.GetDd2o() );
      //lPMT.SetCentralDistInAV( 10.0 * fOpticsC.GetDacrylic() );
      //lPMT.SetCentralDistInWater( 10.0 * fOpticsC.GetDh2o() );
      
      lPMT.SetCentralDistInScint( lightPath.GetDistInScint() );
      lPMT.SetCentralDistInAV( lightPath.GetDistInAV() );
      lPMT.SetCentralDistInWater( lightPath.GetDistInWater() );
      
      lPMT.SetCentralNeckFlag( lightPath.GetXAVNeck() );
      lPMT.SetCentralDistInNeck( lightPath.GetDistInNeckScint() 
                                 + lightPath.GetDistInNeckAV() 
                                 + lightPath.GetDistInNeckWater() );
      
      lPMT.SetCentralInitialLBVec( lightPath.GetInitialLightVec() );
      lPMT.SetCentralIncidentLBVec( lightPath.GetIncidentVecOnPMT() );
      
      //lightPath.CalculateFresnelTRCoeff();
      //lPMT.SetCentralFresnelTCoeff( lightPath.GetFresnelTCoeff() );
      lPMT.SetCentralFresnelTCoeff( fOpticsC.GetTransPwr() );
      
      //lightPath.CalculateSolidAngle( lPMT.GetNorm(), 0 );
      //lPMT.SetCentralSolidAngle( lightPath.GetSolidAngle() );
      lPMT.SetCentralSolidAngle( fOpticsC.GetPmtSolidAngle( iPMT ) );
      
      //lightPath.CalculateCosThetaPMT( lPMT.GetID() );
      //lPMT.SetCentralCosTheta( lightPath.GetCosThetaAvg() );
      lPMT.SetCentralCosTheta( fOpticsC.GetCtheta() );
      
       if ( fOpticsC.IsBelly() || fOpticsC.IsNeckOptics() ||
            fOpticsC.IsRope() || fOpticsC.IsPipe() ||
            fOpticsC.IsPromptAVRefl() || fOpticsC.IsAnchor()
            || fOpticsC.IsNcd() || fOpticsC.IsNcdRope() ){ lPMT.SetCentralBadPath( true ); }
       else{ lPMT.SetCentralBadPath( false ); }
       //if ( lightPath.CheckForShadowing() == true || lightPath.GetXAVNeck()
       //     || lightPath.GetTIR() || lightPath.GetResvHit() ){ lPMT.SetCentralBadPath( true ); }
       //else{ lPMT.SetCentralBadPath( false ); }
      ///////////// WAVELENGTH /////////////
      
      fOpticsW.GetDistances( iPMT );
      lightPath.CalcByPosition( lRunPtr->GetWavelengthLBPos(), lPMT.GetPos() );
      //lightPath.CalcByPosition( lRunPtr->GetWavelengthLBPos(), lPMT.GetPos(), 
      //                          lightPath.WavelengthToEnergy( ( lRunPtr->GetWavelengthLambda() ) * 1e-6 ), 30.0 );
      
      //lPMT.SetWavelengthDistInScint( 10.0 * fOpticsW.GetDd2o() );
      //lPMT.SetWavelengthDistInAV( 10.0 * fOpticsW.GetDacrylic() );
      //lPMT.SetWavelengthDistInWater( 10.0 * fOpticsW.GetDh2o() );
      
      lPMT.SetWavelengthDistInScint( lightPath.GetDistInScint() );
      lPMT.SetWavelengthDistInAV( lightPath.GetDistInAV() );
      lPMT.SetWavelengthDistInWater( lightPath.GetDistInWater() );
      
      lPMT.SetWavelengthNeckFlag( lightPath.GetXAVNeck() );
      lPMT.SetWavelengthDistInNeck( lightPath.GetDistInNeckScint() 
                                    + lightPath.GetDistInNeckAV() 
                                    + lightPath.GetDistInNeckWater() );
      
      lPMT.SetWavelengthInitialLBVec( lightPath.GetInitialLightVec() );
      lPMT.SetWavelengthIncidentLBVec( lightPath.GetIncidentVecOnPMT() );
      
      //lightPath.CalculateFresnelTRCoeff();
      //lPMT.SetWavelengthFresnelTCoeff( lightPath.GetFresnelTCoeff() );
      lPMT.SetWavelengthFresnelTCoeff( fOpticsW.GetTransPwr() );
      
      //lightPath.CalculateSolidAngle( lPMT.GetNorm(), 0 );
      //lPMT.SetWavelengthSolidAngle( lightPath.GetSolidAngle() );
      lPMT.SetWavelengthSolidAngle( fOpticsW.GetPmtSolidAngle( iPMT ) );
      
      //lightPath.CalculateCosThetaPMT( lPMT.GetID() );
      //lPMT.SetWavelengthCosTheta( lightPath.GetCosThetaAvg() );
      lPMT.SetWavelengthCosTheta( fOpticsW.GetCtheta() );
      
        if ( fOpticsW.IsBelly() || fOpticsW.IsNeckOptics() ||
             fOpticsW.IsRope() || fOpticsW.IsPipe() ||
             fOpticsW.IsPromptAVRefl() || fOpticsW.IsAnchor()
             || fOpticsW.IsNcd() || fOpticsW.IsNcdRope() ){ lPMT.SetWavelengthBadPath( true ); }
        else{ lPMT.SetWavelengthBadPath( false ); }
        //if ( lightPath.CheckForShadowing() == true || lightPath.GetXAVNeck()
      //      || lightPath.GetTIR() || lightPath.GetResvHit() ){ lPMT.SetWavelengthBadPath( true ); }
      //else{ lPMT.SetWavelengthBadPath( false ); }
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
