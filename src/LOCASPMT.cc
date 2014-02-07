////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASPMT.cc
///
/// CLASS: LOCAS::LOCASPMT
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///        (Full description in LOCASPMT.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file.
///
////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASMath.hh"
#include <iostream>
#include <vector>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASPMT );

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT::LOCASPMT( const LOCASPMT& rhs )
{
  
  fID = rhs.fID;
  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fOccupancyCorr = rhs.fOccupancyCorr;
  fLBIntensityNorm = rhs.fLBIntensityNorm;

  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;
  fMPECorrOccupancyCorr = rhs.fMPECorrOccupancyCorr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fRelLBTheta = rhs.fRelLBTheta;
  fRelLBPhi = rhs.fRelLBPhi;

  fAVHDShadowVal = rhs.fAVHDShadowVal;
  fGeometricShadowVal = rhs.fGeometricShadowVal;

  fAVHDShadowFlag = rhs.fAVHDShadowFlag;
  fGeometricShadowFlag = rhs.fGeometricShadowFlag;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;
  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;


  fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  fCentralOccupancy = rhs.fCentralOccupancy;
  fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  fCentralOccupancyCorr = rhs.fCentralOccupancyCorr;
  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;
  fCentralMPECorrOccupancyCorr = rhs.fCentralMPECorrOccupancyCorr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInScint = rhs.fCentralDistInScint;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;
  fCentralDistInNeck = rhs.fCentralDistInNeck;
  fCentralTotalDist = rhs.fCentralTotalDist;

  fCentralSolidAngle = rhs.fCentralSolidAngle;
  fCentralCosTheta = rhs.fCentralCosTheta;

  fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  fCentralAVHDShadowVal = rhs.fCentralAVHDShadowVal;
  fCentralGeometricShadowVal = rhs.fCentralGeometricShadowVal;

  fCentralAVHDShadowFlag = rhs.fCentralAVHDShadowFlag;
  fCentralGeometricShadowFlag = rhs.fCentralGeometricShadowFlag;

  fCentralCHSFlag = rhs.fCentralCHSFlag;
  fCentralCSSFlag = rhs.fCentralCSSFlag;
  fCentralBadPath = rhs.fCentralBadPath;
  fCentralNeckFlag = rhs.fCentralNeckFlag;

  fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  fWavelengthOccupancyCorr = rhs.fWavelengthOccupancyCorr;
  fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;
  fWavelengthMPECorrOccupancyCorr = rhs.fWavelengthMPECorrOccupancyCorr;

  fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  fWavelengthDistInScint = rhs.fWavelengthDistInScint;
  fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  fWavelengthAVHDShadowVal = rhs.fWavelengthAVHDShadowVal;
  fWavelengthGeometricShadowVal = rhs.fWavelengthGeometricShadowVal;

  fWavelengthAVHDShadowFlag = rhs.fWavelengthAVHDShadowFlag;
  fWavelengthGeometricShadowFlag = rhs.fWavelengthGeometricShadowFlag;

  fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;
  fWavelengthBadPath = rhs.fWavelengthBadPath;
  fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASPMT::operator=( const LOCASPMT& rhs )
{

  fID = rhs.fID;
  fRunID = rhs.fRunID;
  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fOccupancyCorr = rhs.fOccupancyCorr;
  fLBIntensityNorm = rhs.fLBIntensityNorm;

  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;
  fMPECorrOccupancyCorr = rhs.fMPECorrOccupancyCorr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fRelLBTheta = rhs.fRelLBTheta;
  fRelLBPhi = rhs.fRelLBPhi;

  fAVHDShadowVal = rhs.fAVHDShadowVal;
  fGeometricShadowVal = rhs.fGeometricShadowVal;

  fAVHDShadowFlag = rhs.fAVHDShadowFlag;
  fGeometricShadowFlag = rhs.fGeometricShadowFlag;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;
  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  fCentralOccupancy = rhs.fCentralOccupancy;
  fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  fCentralOccupancyCorr = rhs.fCentralOccupancyCorr;
  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;
  fCentralMPECorrOccupancyCorr = rhs.fCentralMPECorrOccupancyCorr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInScint = rhs.fCentralDistInScint;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;
  fCentralDistInNeck = rhs.fCentralDistInNeck;
  fCentralTotalDist = rhs.fCentralTotalDist;

  fCentralSolidAngle = rhs.fCentralSolidAngle;
  fCentralCosTheta = rhs.fCentralCosTheta;

  fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  fCentralAVHDShadowVal = rhs.fCentralAVHDShadowVal;
  fCentralGeometricShadowVal = rhs.fCentralGeometricShadowVal;

  fCentralAVHDShadowFlag = rhs.fCentralAVHDShadowFlag;
  fCentralGeometricShadowFlag = rhs.fCentralGeometricShadowFlag;

  fCentralCHSFlag = rhs.fCentralCHSFlag;
  fCentralCSSFlag = rhs.fCentralCSSFlag;
  fCentralBadPath = rhs.fCentralBadPath;
  fCentralNeckFlag = rhs.fCentralNeckFlag;

  fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  fWavelengthOccupancyCorr = rhs.fWavelengthOccupancyCorr;
  fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;
  fWavelengthMPECorrOccupancyCorr = rhs.fWavelengthMPECorrOccupancyCorr;

  fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  fWavelengthDistInScint = rhs.fWavelengthDistInScint;
  fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  fWavelengthAVHDShadowVal = rhs.fWavelengthAVHDShadowVal;
  fWavelengthGeometricShadowVal = rhs.fWavelengthGeometricShadowVal;

  fWavelengthAVHDShadowFlag = rhs.fWavelengthAVHDShadowFlag;
  fWavelengthGeometricShadowFlag = rhs.fWavelengthGeometricShadowFlag;

  fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;
  fWavelengthBadPath = rhs.fWavelengthBadPath;
  fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;
 
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::Initialise()
{

  SetID( 0.0 );
  SetRunID( 0.0 );
  SetType ( 0 );

  SetIsVerified( false );

  TVector3 nullVec( 0.0, 0.0, 0.0 );
  SetPos( nullVec );
  SetNorm( nullVec );

  SetPromptPeakTime( 0.0 );
  SetPromptPeakWidth( 0.0 );
  SetTimeOfFlight( 0.0 );
  SetOccupancy( 0.0 );
  SetOccupancyErr( 0.0 );
  SetOccupancyCorr( 0.0 );
  SetLBIntensityNorm( 0.0 );

  SetNLBPulses( 0.0 );

  SetMPECorrOccupancy( 0.0 );
  SetMPECorrOccupancyErr( 0.0 );
  SetMPECorrOccupancyCorr( 0.0 );

  SetFresnelTCoeff( 0.0 );

  SetDistInScint( 0.0 );
  SetDistInAV( 0.0 );
  SetDistInWater( 0.0 );
  SetDistInNeck( 0.0 );
  SetTotalDist( 0.0 );

  SetSolidAngle( 0.0 );
  SetCosTheta( 0.0 );

  SetRelLBTheta( 0.0 );
  SetRelLBPhi( 0.0 );

  SetAVHDShadowVal( 0.0 );
  SetGeometricShadowVal( 0.0 );

  SetAVHDShadowFlag( false );
  SetGeometricShadowFlag( false );

  SetCHSFlag( false );
  SetCSSFlag( false );
  SetBadPath( false );
  SetNeckFlag( false );

  SetCentralPromptPeakTime( 0.0 );
  SetCentralPromptPeakWidth( 0.0 );
  SetCentralTimeOfFlight( 0.0 );
  SetCentralOccupancy( 0.0 );
  SetCentralOccupancyErr( 0.0 );
  SetCentralOccupancyCorr( 0.0 );
  SetCentralLBIntensityNorm( 0.0 );

  SetCentralNLBPulses( 0.0 );

  SetCentralMPECorrOccupancy( 0.0 );
  SetCentralMPECorrOccupancyErr( 0.0 );
  SetCentralMPECorrOccupancyCorr( 0.0 );

  SetCentralFresnelTCoeff( 0.0 );

  SetCentralDistInScint( 0.0 );
  SetCentralDistInAV( 0.0 );
  SetCentralDistInWater( 0.0 );
  SetCentralDistInNeck( 0.0 );
  SetCentralTotalDist( 0.0 );

  SetCentralSolidAngle( 0.0 );
  SetCentralCosTheta( 0.0 );

  SetCentralRelLBTheta( 0.0 );
  SetCentralRelLBPhi( 0.0 );

  SetCentralAVHDShadowVal( 0.0 );
  SetCentralGeometricShadowVal( 0.0 );

  SetCentralAVHDShadowFlag( false );
  SetCentralGeometricShadowFlag( false );

  SetCentralCHSFlag( false );
  SetCentralCSSFlag( false );
  SetCentralBadPath( false );
  SetCentralNeckFlag( false );

  SetWavelengthPromptPeakTime( 0.0 );
  SetWavelengthPromptPeakWidth( 0.0 );
  SetWavelengthTimeOfFlight( 0.0 );
  SetWavelengthOccupancy( 0.0 );
  SetWavelengthOccupancyErr( 0.0 );
  SetWavelengthOccupancyCorr( 0.0 );
  SetWavelengthLBIntensityNorm( 0.0 );

  SetWavelengthNLBPulses( 0.0 );

  SetWavelengthMPECorrOccupancy( 0.0 );
  SetWavelengthMPECorrOccupancyErr( 0.0 );
  SetWavelengthMPECorrOccupancyCorr( 0.0 );

  SetWavelengthFresnelTCoeff( 0.0 );

  SetWavelengthDistInScint( 0.0 );
  SetWavelengthDistInAV( 0.0 );
  SetWavelengthDistInWater( 0.0 );
  SetWavelengthDistInNeck( 0.0 );
  SetWavelengthTotalDist( 0.0 );

  SetWavelengthSolidAngle( 0.0 );
  SetWavelengthCosTheta( 0.0 );

  SetWavelengthRelLBTheta( 0.0 );
  SetWavelengthRelLBPhi( 0.0 );

  SetWavelengthAVHDShadowVal( 0.0 );
  SetWavelengthGeometricShadowVal( 0.0 );

  SetWavelengthAVHDShadowFlag( false );
  SetWavelengthGeometricShadowFlag( false );

  SetWavelengthCHSFlag( false );
  SetWavelengthCSSFlag( false );
  SetWavelengthBadPath( false );
  SetWavelengthNeckFlag( false );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::Clear( Option_t* option )
{

  Initialise();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::AddSOCPMTData( RAT::DS::SOCPMT& socPMT )
{

  SetID( socPMT.GetLCN() );
  SetRunID( 0 );
  SetType( 0 );                                  // SetType is done by LOCAS::LOCASRun::Fill

  SetIsVerified( false );                        // SetIsVerified is done by LOCASRun::Fill

  TVector3 nullVec( 0.0, 0.0, 0.0 );
  SetPos( nullVec );                             // SetPos is done by LOCAS::LOCASRun::Fill
  SetNorm( nullVec );                            // SetNorm is done by LOCAS::LOCASRun::Fill

  Double_t pmtTACCal = 0.0;
  Double_t pmtTACRMS = 0.0;
  socPMT.CalculateTAC( pmtTACCal, pmtTACRMS );

  SetPromptPeakTime( pmtTACCal );
  SetPromptPeakWidth( pmtTACRMS );
  SetTimeOfFlight( socPMT.GetTOF() );
  SetOccupancy( socPMT.GetPromptOcc() );
  SetOccupancyErr( TMath::Sqrt( fOccupancy ) );
  SetOccupancyCorr( ( fOccupancy ) / ( ( socPMT.GetTACs() ).size() ) );
  SetLBIntensityNorm( 0.0 );                    // SetLBIntensityNorm is done by LOCAS::LOCASRun::Fill

  SetNLBPulses( 0.0 );                          // SetNLBPulses is done by LOCAS::LOCASRun::Fill


  SetMPECorrOccupancy( 0.0 );                   // SetMPECorrOccupancy is done by LOCAS::LOCASRun::Fill
  SetMPECorrOccupancyErr( 0.0 );                // SetMPECorrOccupancyErr is done by LOCAS::LOCASRun::Fill
  SetMPECorrOccupancyCorr( 0.0 );               // SetMPECorrOccupancyCorr is done by LOCAS::LOCASRun::Fill
		       
  SetFresnelTCoeff( 0.0 );                      // SetFresnelTCOeff is done by LOCASPMT::ProcessLightPath

  SetDistInScint( 0.0 );                        // SetDistInScint is done by LOCASPMT::ProcessLightPath
  SetDistInAV( 0.0 );                           // SetDistInAV is done by LOCASPMT::ProcessLightPath
  SetDistInWater( 0.0 );                        // SetDistInWater is done by LOCASPMT::ProcessLightPath
  SetDistInNeck( 0.0 );                         // SetDistInNeck is done by LOCASPMT::ProcessLightPath
  SetTotalDist( 0.0 );                          // SetTotalDist is done by LOCASPMT::ProcessLightPath

  SetSolidAngle( 0.0 );                         // SetSolidAngle is done by LOCASPMT::ProcessLightPath
  SetCosTheta( 0.0 );                           // SetCosTheta is done by LOCASPMT::ProcessLightPath

  SetRelLBTheta( 0.0 );                            // SetLBTheta is done by LOCAS::LOCASRun::Fill
  SetRelLBPhi( 0.0 );                              // SetLBPhi is done by LOCAS::LOCASRun::Fill

  SetGeometricShadowVal( socPMT.GetRelOccSim_fullShadow() );
  SetAVHDShadowVal( socPMT.GetRelOccSim_hdRopeShadow() );

  SetAVHDShadowFlag( false );                   // SetAVHDShadowFlag is done by LOCAS::LOCASRun::Fill
  SetGeometricShadowFlag( false );              // SetGeometricShadowFlag is done by LOCAS::LOCASRun::Fill

  SetCHSFlag( false );                          // SetCHSFlag is done by LOCAS::LOCASRun::Fill
  SetCSSFlag( false );                          // SetCSSFlag is done by LOCAS::LOCASRun::Fill
  SetBadPath( false );                          // SetBadPath is done by LOCASPMT::ProcessLightPath
  SetNeckFlag( false );                         // SetNeckFlag is done by LOCASPMT::ProcessLightPath

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::ProcessLightPath( LOCASLightPath& lPath )
{

  if ( fType == 1 ){
    if ( lPath.GetTIR() || lPath.GetResvHit() ){ fBadPath = true; }
    if ( lPath.GetXAVNeck() ){ 
      fNeckFlag = true;
      fDistInNeck = lPath.GetDistInNeck();
    }
    
    fDistInNeck = lPath.GetDistInNeck();
    fDistInScint = lPath.GetDistInScint();
    fDistInAV = lPath.GetDistInAV();
    fDistInWater = lPath.GetDistInWater();
    fTotalDist = fDistInScint + fDistInAV + fDistInWater;

    if ( lPath.GetTIR() || lPath.GetResvHit() ){
      fBadPath = true;
    }

    fNeckFlag = lPath.GetXAVNeck();
    lPath.CalculateFresnelTRCoeff();
    fFresnelTCoeff = lPath.GetFresnelTCoeff();

    if ( ( lPath.GetStartPos() ).Mag() < 4500.0 ){
      lPath.CalculateSolidAngle( fNorm, 0 );
      fSolidAngle = lPath.GetSolidAngle();
      fCosTheta = lPath.GetCosThetaAvg();
    }
    else{
      lPath.CalculateSolidAngle( fNorm, 80 );
      fSolidAngle = lPath.GetSolidAngle();
      fCosTheta = lPath.GetCosThetaAvg();
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::VerifyPMT()
{
  fIsVerified = true;

  if ( fID < 1 || fID > 10000 ){ fIsVerified == false; }
  if ( fType < 1 || fType > 11 ){ fIsVerified == false; }
  
  if ( fPos.Mag() < 6000.0 || fPos.Mag() > 9000.0  ){ fIsVerified = false; }
  if ( fNorm.Mag() < 0.9 || fNorm.Mag() > 1.1 ){ fIsVerified = false; }
  
  if ( fPromptPeakTime == 0.0 || fPromptPeakTime > 410.0 ){ fIsVerified = false; }
  if ( fTimeOfFlight == 0.0 || fTimeOfFlight > 410.0 ){ fIsVerified = false; }
  if ( fOccupancy == 0.0 ){ fIsVerified = false; }
  if ( fOccupancyErr == 0.0 ){ fIsVerified = false; }
  if ( fOccupancyCorr == 0.0 ){ fIsVerified = false; }
  if ( fLBIntensityNorm == 0.0 ){ fIsVerified = false; }

  if ( fNLBPulses == 0.0 ){ fIsVerified = false; }

  if ( fMPECorrOccupancy == 0.0 ){ fIsVerified = false; }
  if ( fMPECorrOccupancyErr == 0.0 ){ fIsVerified = false; }
  if ( fMPECorrOccupancyCorr == 0.0 ){ fIsVerified = false; }

  if ( fFresnelTCoeff == 0.0 || fFresnelTCoeff > 1.0 ){ fIsVerified = false; }

  if ( fDistInScint == 0.0 || fDistInScint > 12000.0 ){ fIsVerified = false; }
  if ( fDistInAV == 0.0 || fDistInAV > 1000.0 ){ fIsVerified = false; }
  if ( fDistInWater == 0.0 || fDistInWater > 10000.0 ){ fIsVerified = false; }
  // DistInNeck is optional - doesn't need to be checked here
  // TotalDist is implicit - doesn't need to be checked here

  if ( fSolidAngle == 0.0 ){ fIsVerified = false; }
  if ( fCosTheta < 0.0 || fCosTheta > 1.0 ){ fIsVerified = false; }

  if ( fAVHDShadowVal < 0.0 || fAVHDShadowVal > 3.0 ){ fIsVerified = false; }
  if ( fGeometricShadowVal < 0.0 || fGeometricShadowVal > 3.0 ){ fIsVerified = false; }

  // CHSFlag doesn't need to be checked here
  // CSSFlag doesn't need to be checked here
  // BadPath doesn't need to be checked here
  // NeckFlag doesn't need to be checked here
  
  // all Corr* values don't need to be checked here
  // As all Corr* values are dependent on pre-verified values
  // done in respective runs

}


