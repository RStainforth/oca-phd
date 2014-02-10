////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRun.cc
///
/// CLASS: LOCAS::LOCASRun
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///        (Full description in LOCASRun.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASLightPath.hh"
#include "LOCASMath.hh"

#include <map>

#include "RAT/DS/SOCPMT.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/SOCReader.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASRun )

// Default Constructor
LOCASRun::LOCASRun()
{  
  // NULL-ify PMT private variables  
  fLOCASPMTs.clear();
  Initialise();
}

//////////////////////////////////////
//////////////////////////////////////

// Default Destructor
LOCASRun::~LOCASRun()
{
  // Delete LOCASPMT array
  
}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun::LOCASRun( const LOCASRun& locasRHS )
{

  // Copies all Run information from locasRHS
  // EXCEPT for the fLOCASPMTs map

  fRunID = locasRHS.fRunID;
  fCentralRunID = locasRHS.fCentralRunID;
  fWavelengthRunID = locasRHS.fWavelengthRunID;

  fSourceID = locasRHS.fSourceID;
  fCentralSourceID = locasRHS.fCentralSourceID;
  fWavelengthSourceID = locasRHS.fWavelengthSourceID;

  fIsMainRun = locasRHS.fIsMainRun;
  fIsCentralRun = locasRHS.fIsCentralRun;
  fIsWavelengthRun = locasRHS.fIsWavelengthRun;

  fLambda = locasRHS.fLambda;
  fNLBPulses = locasRHS.fNLBPulses;
  fCentralLambda = locasRHS.fCentralLambda;
  fCentralNLBPulses = locasRHS.fCentralNLBPulses;
  fWavelengthLambda = locasRHS.fWavelengthLambda;
  fWavelengthNLBPulses = locasRHS.fWavelengthNLBPulses;

  fMainLBIntensityNorm = locasRHS.fMainLBIntensityNorm;
  fCentralLBIntensityNorm = locasRHS.fCentralLBIntensityNorm;
  fWavelengthLBIntensityNorm = locasRHS.fWavelengthLBIntensityNorm;

  fTimeSigmaMean = locasRHS.fTimeSigmaMean;
  fTimeSigmaSigma = locasRHS.fTimeSigmaSigma;

  fLBPos = locasRHS.fLBPos;
  fCentralLBPos = locasRHS.fCentralLBPos;
  fWavelengthLBPos = locasRHS.fWavelengthLBPos;

  fLBXPosErr = locasRHS.fLBXPosErr;
  fLBYPosErr = locasRHS.fLBYPosErr;
  fLBZPosErr = locasRHS.fLBZPosErr;

  fLBPosChi2 = locasRHS.fLBPosChi2;
  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fCentralLBPosChi2 = locasRHS.fCentralLBPosChi2;
  fCentralLBTheta = locasRHS.fCentralLBTheta;
  fCentralLBPhi = locasRHS.fCentralLBPhi;

  fWavelengthLBPosChi2 = locasRHS.fWavelengthLBPosChi2;
  fWavelengthLBTheta = locasRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = locasRHS.fWavelengthLBPhi;


  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun& LOCASRun::operator=( const LOCASRun& locasRHS )
{

  // Copies all Run information from locasRHS
  // EXCEPT for the fLOCASPMTs map

  fRunID = locasRHS.fRunID;
  fCentralRunID = locasRHS.fCentralRunID;
  fWavelengthRunID = locasRHS.fWavelengthRunID;

  fSourceID = locasRHS.fSourceID;
  fCentralSourceID = locasRHS.fCentralSourceID;
  fWavelengthSourceID = locasRHS.fWavelengthSourceID;

  fIsMainRun = locasRHS.fIsMainRun;
  fIsCentralRun = locasRHS.fIsCentralRun;
  fIsWavelengthRun = locasRHS.fIsWavelengthRun;

  fLambda = locasRHS.fLambda;
  fNLBPulses = locasRHS.fNLBPulses;
  fCentralLambda = locasRHS.fCentralLambda;
  fCentralNLBPulses = locasRHS.fCentralNLBPulses;
  fWavelengthLambda = locasRHS.fWavelengthLambda;
  fWavelengthNLBPulses = locasRHS.fWavelengthNLBPulses;

  fMainLBIntensityNorm = locasRHS.fMainLBIntensityNorm;
  fCentralLBIntensityNorm = locasRHS.fCentralLBIntensityNorm;
  fWavelengthLBIntensityNorm = locasRHS.fWavelengthLBIntensityNorm;

  fTimeSigmaMean = locasRHS.fTimeSigmaMean;
  fTimeSigmaSigma = locasRHS.fTimeSigmaSigma;

  fLBPos = locasRHS.fLBPos;
  fCentralLBPos = locasRHS.fCentralLBPos;
  fWavelengthLBPos = locasRHS.fWavelengthLBPos;

  fLBXPosErr = locasRHS.fLBXPosErr;
  fLBYPosErr = locasRHS.fLBYPosErr;
  fLBZPosErr = locasRHS.fLBZPosErr;

  fLBPosChi2 = locasRHS.fLBPosChi2;
  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fCentralLBPosChi2 = locasRHS.fCentralLBPosChi2;
  fCentralLBTheta = locasRHS.fCentralLBTheta;
  fCentralLBPhi = locasRHS.fCentralLBPhi;

  fWavelengthLBPosChi2 = locasRHS.fWavelengthLBPosChi2;
  fWavelengthLBTheta = locasRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = locasRHS.fWavelengthLBPhi;


  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Initialise()
{ 

  SetRunID( 0 );
  SetCentralRunID( 0 );
  SetWavelengthRunID( 0 );

  SetSourceID( 0 );
  SetCentralSourceID( 0 );
  SetWavelengthSourceID( 0 );

  SetIsMainRun( false );
  SetIsCentralRun( false );
  SetIsWavelengthRun( false );

  SetLambda( 0.0 );
  SetNLBPulses( 0.0 );  
  SetCentralLambda( 0.0 );
  SetCentralNLBPulses( 0.0 );
  SetWavelengthLambda( 0.0 );
  SetWavelengthNLBPulses( 0.0 );

  SetMainLBIntensityNorm( 0.0 );
  SetCentralLBIntensityNorm( 0.0 );
  SetWavelengthLBIntensityNorm( 0.0 );

  SetTimeSigmaMean( 0.0 );
  SetTimeSigmaSigma( 0.0 );

  SetLBPos( -9999.9, -9999.9, -9999.9 );
  SetCentralLBPos( -9999.9, -9999.9, -9999.9 );
  SetWavelengthLBPos( -9999.9, -9999.9, -9999.9 );

  SetLBXPosErr( 0.0 );
  SetLBYPosErr( 0.0 );
  SetLBZPosErr( 0.0 );

  SetCentralLBXPosErr( 0.0 );
  SetCentralLBYPosErr( 0.0 );
  SetCentralLBZPosErr( 0.0 );

  SetWavelengthLBXPosErr( 0.0 );
  SetWavelengthLBYPosErr( 0.0 );
  SetWavelengthLBZPosErr( 0.0 );

  SetLBPosChi2( 0.0 );
  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetCentralLBPosChi2( 0.0 );
  SetCentralLBTheta( 0.0 );
  SetCentralLBPhi( 0.0 );

  SetWavelengthLBPosChi2( 0.0 );
  SetWavelengthLBTheta( 0.0 );
  SetWavelengthLBPhi( 0.0 );

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Clear( Option_t* option )
{
  // Reset all the values for this LOCASRun

  SetRunID( 0 );
  SetCentralRunID( 0 );
  SetWavelengthRunID( 0 );

  SetSourceID( 0 );
  SetCentralSourceID( 0 );
  SetWavelengthSourceID( 0 );

  SetIsMainRun( false );
  SetIsCentralRun( false );
  SetIsWavelengthRun( false );

  SetLambda( 0.0 );
  SetNLBPulses( 0.0 );  
  SetCentralLambda( 0.0 );
  SetCentralNLBPulses( 0.0 );
  SetWavelengthLambda( 0.0 );
  SetWavelengthNLBPulses( 0.0 );

  SetMainLBIntensityNorm( 0.0 );
  SetCentralLBIntensityNorm( 0.0 );
  SetWavelengthLBIntensityNorm( 0.0 );

  SetTimeSigmaMean( 0.0 );
  SetTimeSigmaSigma( 0.0 );

  SetLBPos( -9999.9, -9999.9, -9999.9 );
  SetCentralLBPos( -9999.9, -9999.9, -9999.9 );
  SetWavelengthLBPos( -9999.9, -9999.9, -9999.9 );

  SetLBXPosErr( 0.0 );
  SetLBYPosErr( 0.0 );
  SetLBZPosErr( 0.0 );

  SetCentralLBXPosErr( 0.0 );
  SetCentralLBYPosErr( 0.0 );
  SetCentralLBZPosErr( 0.0 );

  SetWavelengthLBXPosErr( 0.0 );
  SetWavelengthLBYPosErr( 0.0 );
  SetWavelengthLBZPosErr( 0.0 );

  SetLBPosChi2( 0.0 );
  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetCentralLBPosChi2( 0.0 );
  SetCentralLBTheta( 0.0 );
  SetCentralLBPhi( 0.0 );

  SetWavelengthLBPosChi2( 0.0 );
  SetWavelengthLBTheta( 0.0 );
  SetWavelengthLBPhi( 0.0 );

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Fill( RAT::SOCReader& socR, Int_t runID )
{

  // First perform the neccessary checks
  RAT::DS::SOC* socPtr = NULL;
  Long64_t NSOCs = socR.GetNSOC();

  for ( Int_t iSOC = 0; iSOC < NSOCs; iSOC++ ){

    socPtr = socR.GetSOC( iSOC );

    if ( socPtr->GetRunID() == runID ){
      socPtr = socR.GetSOC( iSOC );
      break;
    }

    if ( ( iSOC == NSOCs - 1 ) && ( socPtr->GetRunID() != runID ) ){
      std::cout << "No SOC File with specified Run ID found in the SOCReader! Setting to Default" << std::endl;
      socPtr = socR.GetSOC( 0 );
    }

  }

  // Now that a SOC file which matches the run ID specified has been
  // found. We can now begin to fill the LOCASRun object with all the
  // neccessary information.
  CopySOCRunInfo( socPtr );
  CopySOCPMTInfo( socPtr );

  // Create the LOCAS Data base Object (LOCASDB) and
  // load PMT information and detector parameters;
  LOCASDB lDB;
  // Load the RAT PMT Positions, PMT Normals and Types
  lDB.LoadPMTPositions();
  lDB.LoadPMTNormals();
  lDB.LoadPMTTypes();

  // Create an iterator to loop over the PMTs...
  std::map<Int_t, LOCASPMT>::iterator iLP;

  // The PMT ID variable for each PMT in the loop
  Int_t pmtID;
  
  // Create a LOCASLightPath which will be calculated for each 
  // loop ( and inherently each PMT )
  LOCASLightPath lLP;
  
  // Set the PMT positions and normals and then 'feed' the PMT a calculated light path.
  // The PMT 'eats' this light path and calculates it's member variables
  // e.g. distance through scintillator, acrylic and water and various flags based
  // on the quality of the light path.
  for ( iLP = GetLOCASPMTIterBegin(); iLP != GetLOCASPMTIterEnd(); iLP++ ){

    // Get the PMT ID and LOCASPMT object
    pmtID = iLP->first;
    
    ( iLP->second ).SetRunID( runID );
    // Set the Normals and Positions
    ( iLP->second ).SetPos( lDB.GetPMTPosition( pmtID ) );
    ( iLP->second ).SetNorm( lDB.GetPMTNormal( pmtID ) );
    ( iLP->second ).SetType( lDB.GetPMTType( pmtID ) );

    ( iLP->second ).SetNLBPulses( GetNLBPulses() );
    ( iLP->second ).SetLBIntensityNorm( GetMainLBIntensityNorm() );

    LOCASMath lMath;
    ( iLP->second ).SetMPECorrOccupancy( lMath.MPECorrectedNPrompt( ( iLP->second ).GetOccupancy(), GetNLBPulses() ) );
    ( iLP->second ).SetMPECorrOccupancyErr( lMath.MPECorrectedNPromptErr( ( iLP->second ).GetOccupancy(), GetNLBPulses() ) );
    ( iLP->second ).SetMPECorrOccupancyCorr( ( iLP->second ).GetMPECorrOccupancy() / ( iLP->second ).GetOccupancy() );
    
    // Calculate the light path for this source position and PMT
    lLP.CalculatePath( GetLBPos(), GetPMT( iLP->first ).GetPos(), 10.0, GetLambda() );
    
    // 'feed' the light path to the PMT
    ( iLP->second ).ProcessLightPath( lLP );

    TVector3 pmtRelativeVec = lLP.GetInitialLightVec();
    ( iLP->second ).SetRelLBTheta( pmtRelativeVec.Theta() );
    ( iLP->second ).SetRelLBPhi( pmtRelativeVec.Phi() );
    
    // Reset the light path object
    lLP.Clear();

    ( iLP->second ).VerifyPMT();
    
  }

  // Clear the LOCASDB object to free up memory
  lDB.Clear();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCRunInfo( RAT::DS::SOC* socRun )
{

  // Copies all the Run-level information from a SOC file
  // and puts it into (this) LOCASRun object

  SetRunID( socRun->GetRunID() );
  SetSourceID( socRun->GetSourceID() );
  SetLambda( socRun->GetLaserWavelength() );
  SetLBPos( socRun->GetSourcePosManip() ); 

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCPMTInfo( RAT::DS::SOC* socRun )
{

  // Copies all the SOCPMTs information from from a SOC
  // file and copies them into LOCASPMT objects

  std::map<Int_t, RAT::DS::SOCPMT>::iterator iSOCPMT;
  for ( iSOCPMT = socRun->GetSOCPMTIterBegin(); iSOCPMT != socRun->GetSOCPMTIterEnd(); ++iSOCPMT ){
    AddSOCPMT( iSOCPMT->second );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopyLOCASRunInfo( LOCASRun& locasRun )
{

  *this = locasRun;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopyLOCASPMTInfo( LOCASRun& locasRun )
{

  // Copies all the LOCASPMTs information from from a
  // different LOCASRun object and copies them into 
  // new LOCASPMT objects in this LOCASRun

  std::map<Int_t, LOCASPMT>::iterator iLOCASPMT;

  for ( iLOCASPMT = locasRun.GetLOCASPMTIterBegin(); iLOCASPMT != locasRun.GetLOCASPMTIterEnd(); ++iLOCASPMT ){
    AddLOCASPMT( iLOCASPMT->second );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::AddSOCPMT( RAT::DS::SOCPMT& socPMT )
{


  Int_t pmtID = socPMT.GetLCN();

  if( fLOCASPMTs.find( pmtID ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = LOCASPMT( pmtID );
    ( fLOCASPMTs[ pmtID ] ).AddSOCPMTData( socPMT );
  }

  else{
    std::cout << "PMT Not Added - PMT with same ID already exists" << std::endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::AddLOCASPMT( LOCASPMT& locasPMT )
{

  Int_t pmtID = locasPMT.GetID();

  if( fLOCASPMTs.find( locasPMT.GetID() ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = locasPMT;
  }

  else{
    std::cout << "PMT Not Added - PMT with same ID already exists" << std::endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::RemovePMT( Int_t iPMT )
{

  // Remove a LOCAS PMT from the fLOCASPMTs map
  fLOCASPMTs.erase( iPMT );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASRun::GetPMT( Int_t iPMT )
{

  // Return the LOCASPMT with LCN 'iPMT'
  
  if( fLOCASPMTs.find( iPMT ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ iPMT ] = LOCASPMT( iPMT );
  }

  return fLOCASPMTs[ iPMT ];
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CrossRunFill( LOCASRun& cRun, LOCASRun& wRun )
{
  fCentralRunID = cRun.GetRunID();
  fWavelengthRunID = wRun.GetRunID();

  std::map< Int_t, LOCASPMT >::iterator iCPMT;
  for( iCPMT = cRun.GetLOCASPMTIterBegin(); iCPMT != cRun.GetLOCASPMTIterEnd(); iCPMT++ ){
    Int_t pmtID = ( iCPMT->first );
    ( fLOCASPMTs[ pmtID ] ).SetCentralRunID( cRun.GetRunID() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralPromptPeakTime( ( iCPMT->second ).GetPromptPeakTime() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralPromptPeakWidth( ( iCPMT->second ).GetPromptPeakWidth() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralTimeOfFlight( ( iCPMT->second ).GetTimeOfFlight() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancy( ( iCPMT->second ).GetOccupancy() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancyErr( ( iCPMT->second ).GetOccupancyErr() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancyCorr( ( iCPMT->second ).GetOccupancyCorr() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralLBIntensityNorm( ( iCPMT->second ).GetLBIntensityNorm() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralNLBPulses( ( iCPMT->second ).GetNLBPulses() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancy( ( iCPMT->second ).GetMPECorrOccupancy() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancyErr( ( iCPMT->second ).GetMPECorrOccupancyErr() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancyCorr( ( iCPMT->second ).GetMPECorrOccupancyCorr() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralFresnelTCoeff( ( iCPMT->second ).GetFresnelTCoeff() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralDistInScint( ( iCPMT->second ).GetDistInScint() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralDistInAV( ( iCPMT->second ).GetDistInAV() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralDistInWater( ( iCPMT->second ).GetDistInWater() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralDistInNeck( ( iCPMT->second ).GetDistInNeck() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralTotalDist( ( iCPMT->second ).GetTotalDist() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralSolidAngle( ( iCPMT->second ).GetSolidAngle() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralCosTheta( ( iCPMT->second ).GetCosTheta() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralRelLBTheta( ( iCPMT->second ).GetRelLBTheta() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralRelLBPhi( ( iCPMT->second ).GetRelLBPhi() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralAVHDShadowVal( ( iCPMT->second ).GetAVHDShadowVal() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralGeometricShadowVal( ( iCPMT->second ).GetGeometricShadowVal() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralCHSFlag( ( iCPMT->second ).GetCHSFlag() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralCSSFlag( ( iCPMT->second ).GetCSSFlag() );
    
    ( fLOCASPMTs[ pmtID ] ).SetCentralBadPath( ( iCPMT->second ).GetBadPath() );
    ( fLOCASPMTs[ pmtID ] ).SetCentralNeckFlag( ( iCPMT->second ).GetNeckFlag() );
    
  }
  
  std::map< Int_t, LOCASPMT >::iterator iWPMT;
  for( iWPMT = wRun.GetLOCASPMTIterBegin(); iWPMT != wRun.GetLOCASPMTIterEnd(); iWPMT++ ){
    Int_t pmtID = ( iWPMT->first );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthRunID( wRun.GetRunID() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthPromptPeakTime( ( iWPMT->second ).GetPromptPeakTime() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthPromptPeakWidth( ( iWPMT->second ).GetPromptPeakWidth() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthTimeOfFlight( ( iWPMT->second ).GetTimeOfFlight() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancy( ( iWPMT->second ).GetOccupancy() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancyErr( ( iWPMT->second ).GetOccupancyErr() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancyCorr( ( iWPMT->second ).GetOccupancyCorr() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthLBIntensityNorm( ( iWPMT->second ).GetLBIntensityNorm() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthNLBPulses( ( iWPMT->second ).GetNLBPulses() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancy( ( iWPMT->second ).GetMPECorrOccupancy() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancyErr( ( iWPMT->second ).GetMPECorrOccupancyErr() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancyCorr( ( iWPMT->second ).GetMPECorrOccupancyCorr() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthFresnelTCoeff( ( iWPMT->second ).GetFresnelTCoeff() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInScint( ( iWPMT->second ).GetDistInScint() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInAV( ( iWPMT->second ).GetDistInAV() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInWater( ( iWPMT->second ).GetDistInWater() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInNeck( ( iWPMT->second ).GetDistInNeck() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthTotalDist( ( iWPMT->second ).GetTotalDist() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthSolidAngle( ( iWPMT->second ).GetSolidAngle() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthCosTheta( ( iWPMT->second ).GetCosTheta() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthRelLBTheta( ( iWPMT->second ).GetRelLBTheta() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthRelLBPhi( ( iWPMT->second ).GetRelLBPhi() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthAVHDShadowVal( ( iWPMT->second ).GetAVHDShadowVal() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthGeometricShadowVal( ( iWPMT->second ).GetGeometricShadowVal() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthCHSFlag( ( iWPMT->second ).GetCHSFlag() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthCSSFlag( ( iWPMT->second ).GetCSSFlag() );
    
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthBadPath( ( iWPMT->second ).GetBadPath() );
    ( fLOCASPMTs[ pmtID ] ).SetWavelengthNeckFlag( ( iWPMT->second ).GetNeckFlag() );
    
  }

}

//////////////////////////////////////
//////////////////////////////////////


 






