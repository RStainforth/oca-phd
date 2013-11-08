////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASPMT.cc
///
/// CLASS: LOCASPMT
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///        (Full description in LOCASPMT.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include <iostream>
#include <vector>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASPMT )

//////////////////////////////////////
//////////////////////////////////////

// Copy Constructor
LOCASPMT::LOCASPMT( const LOCASPMT& rhs )
{
  
  fID = rhs.fID;
  fType = rhs.fType;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fHasEntries = rhs.fHasEntries;

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fOccupancyCorr = rhs.fOccupancyCorr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fLBTheta = rhs.fLBTheta;
  fLBPhi = rhs.fLBPhi;

  fAVHDShadowVal = rhs.fAVHDShadowVal;
  fGeometricShadowVal = rhs.fGeometricShadowVal;

  fAVHDShadowFlag = rhs.fAVHDShadowFlag;
  fGeometricShadowFlag = rhs.fGeometricShadowFlag;

  fAVHDShadowRatio = rhs.fAVHDShadowRatio;
  fAVHDShadowRatioErr = rhs.fAVHDShadowRatioErr;

  fGeometricShadowRatio = rhs.fGeometricShadowRatio;
  fGeometricShadowRatioErr = rhs.fGeometricShadowRatioErr;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;
  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fOccRatio = rhs.fOccRatio;
  fOccRatioErr = rhs.fOccRatioErr; 

  fCorrSolidAngle = rhs.fCorrSolidAngle;

}

//////////////////////////////////////
//////////////////////////////////////

// Copy Constructor
LOCASPMT& LOCASPMT::operator=( const LOCASPMT& rhs )
{

  fID = rhs.fID;
  fType = rhs.fType;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fHasEntries = rhs.fHasEntries;

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fOccupancyCorr = rhs.fOccupancyCorr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fLBTheta = rhs.fLBTheta;
  fLBPhi = rhs.fLBPhi;

  fAVHDShadowVal = rhs.fAVHDShadowVal;
  fGeometricShadowVal = rhs.fGeometricShadowVal;

  fAVHDShadowFlag = rhs.fAVHDShadowFlag;
  fGeometricShadowFlag = rhs.fGeometricShadowFlag;

  fAVHDShadowRatio = rhs.fAVHDShadowRatio;
  fAVHDShadowRatioErr = rhs.fAVHDShadowRatioErr;

  fGeometricShadowRatio = rhs.fGeometricShadowRatio;
  fGeometricShadowRatioErr = rhs.fGeometricShadowRatioErr;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;
  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fOccRatio = rhs.fOccRatio;
  fOccRatioErr = rhs.fOccRatioErr; 

  fCorrSolidAngle = rhs.fCorrSolidAngle;
 
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::Initalise()
{
  // PMT Position, Normal and Type must be allocated using LOCAS::LOCASDB,
  // They are NOT initialised here!

  SetHasEntries( false );

  SetPromptPeakTime( 0.0 );
  SetPromptPeakWidth( 0.0 );
  SetTimeOfFlight( 0.0 );
  SetOccupancy( 0.0 );
  SetOccupancyErr( 0.0 );
  SetOccupancyCorr( 0.0 );

  SetFresnelTCoeff( 0.0 );

  SetDistInScint( 0.0 );
  SetDistInAV( 0.0 );
  SetDistInWater( 0.0 );
  SetTotalDist( 0.0 );

  SetSolidAngle( 0.0 );
  SetCosTheta( 0.0 );

  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetAVHDShadowVal( 0.0 );
  SetGeometricShadowVal( 0.0 );

  SetAVHDShadowFlag( false );
  SetGeometricShadowFlag( false );

  SetAVHDShadowRatio( 0.0 );
  SetAVHDShadowRatioErr( 0.0 );

  SetGeometricShadowRatio( 0.0 );
  SetGeometricShadowRatioErr( 0.0 );

  SetCHSFlag( false );
  SetCSSFlag( false );
  SetBadPath( false );
  SetNeckFlag( false );

  SetOccRatio( 0.0 );
  SetOccRatioErr( 0.0 );

  SetCorrSolidAngle( 0.0 );

}

//////////////////////////////////////
//////////////////////////////////////

// Copy Constructor
void LOCASPMT::Clear( Option_t* option )
{

  SetID( 0 );
  SetType( 0 );

  SetHasEntries( false );

  TVector3 nullVec( 0.0, 0.0, 0.0 );
  SetPos( nullVec );
  SetNorm( nullVec );

  SetPromptPeakTime( 0.0 );
  SetPromptPeakWidth( 0.0 );
  SetTimeOfFlight( 0.0 );
  SetOccupancy( 0.0 );
  SetOccupancyErr( 0.0 );
  SetOccupancyCorr( 0.0 );

  SetFresnelTCoeff( 0.0 );

  SetDistInScint( 0.0 );
  SetDistInAV( 0.0 );
  SetDistInWater( 0.0 );
  SetTotalDist( 0.0 );

  SetSolidAngle( 0.0 );
  SetCosTheta( 0.0 );

  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetAVHDShadowVal( 0.0 );
  SetGeometricShadowVal( 0.0 );

  SetAVHDShadowFlag( false );
  SetGeometricShadowFlag( false );

  SetAVHDShadowRatio( 0.0 );
  SetAVHDShadowRatioErr( 0.0 );

  SetGeometricShadowRatio( 0.0 );
  SetGeometricShadowRatioErr( 0.0 );

  SetCHSFlag( false );
  SetCSSFlag( false );
  SetBadPath( false );
  SetNeckFlag( false );

  SetOccRatio( 0.0 );
  SetOccRatioErr( 0.0 );

  SetCorrSolidAngle( 0.0 );
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::AddSOCPMTData( RAT::DS::SOCPMT& socPMT )
{
  SetID( socPMT.GetLCN() );
  SetTimeOfFlight( socPMT.GetTOF() );
  SetGeometricShadowVal( socPMT.GetRelOccSim_fullShadow() );
  SetAVHDShadowVal( socPMT.GetRelOccSim_hdRopeShadow() );
  SetOccupancy( socPMT.GetPromptOcc() );
  SetOccupancyErr( TMath::Sqrt( fOccupancy ) );
  SetOccupancyCorr( ( fOccupancy ) / ( ( socPMT.GetTACs() ).size() ) );

  Double_t pmtTACCal = 0.0;
  Double_t pmtTACRMS = 0.0;
  socPMT.CalculateTAC( pmtTACCal, pmtTACRMS );
  SetPromptPeakTime( pmtTACCal );
  SetPromptPeakWidth( pmtTACRMS );

  SetType( 0 );

  TVector3 nullVec( 0.0, 0.0, 0.0 );
  SetPos( nullVec );
  SetNorm( nullVec );

  SetHasEntries( false );

  SetFresnelTCoeff( 0.0 );

  SetDistInScint( 0.0 );
  SetDistInAV( 0.0 );
  SetDistInWater( 0.0 );
  SetTotalDist( 0.0 );

  SetSolidAngle( 0.0 );
  SetCosTheta( 0.0 );

  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetAVHDShadowFlag( false );
  SetGeometricShadowFlag( false );

  SetAVHDShadowRatio( 0.0 );
  SetAVHDShadowRatioErr( 0.0 );

  SetGeometricShadowRatio( 0.0 );
  SetGeometricShadowRatioErr( 0.0 );

  SetCHSFlag( false );
  SetCSSFlag( false );
  SetBadPath( false );
  SetNeckFlag( false );

  SetOccRatio( 0.0 );
  SetOccRatioErr( 0.0 );

  SetCorrSolidAngle( 0.0 );
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

    if ( ( lPath.GetStartPos() ).Mag() < 4000.0 ){
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

  fHasEntries = true;

}


