#include "OCAPMT.hh"

#include "TMath.h"

#include <iostream>
#include <vector>

using namespace OCA;
using namespace std;

ClassImp( OCAPMT )

//////////////////////////////////////
//////////////////////////////////////

OCAPMT::OCAPMT( const OCAPMT& rhs )
{
  // Copy constructor, set all the variables
  // from the OCAPMT object on the right hand side 'rhs'
  // to the variables on the left hand side 'this' OCAPMT.

  fID = rhs.fID;

  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;
  fCentralIsVerified = rhs.fCentralIsVerified;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fLBPos = rhs.fLBPos;
  fCentralLBPos = rhs.fCentralLBPos;
  fWavelengthLBPos = rhs.fWavelengthLBPos;
  fLBOrientation = rhs.fLBOrientation;
  fCentralLBOrientation = rhs.fCentralLBOrientation;
  fWavelengthLBOrientation = rhs.fWavelengthLBOrientation;

  // Off-axis values.

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;

  fPromptPeakCounts = rhs.fPromptPeakCounts;
  fPromptPeakCountsErr = rhs.fPromptPeakCountsErr;

  fTimeOfFlight = rhs.fTimeOfFlight;

  fLBIntensityNorm = rhs.fLBIntensityNorm;
  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInInnerAV = rhs.fDistInInnerAV;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fRelLBTheta = rhs.fRelLBTheta;
  fRelLBPhi = rhs.fRelLBPhi;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;

  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fInitialLBVec = rhs.fInitialLBVec;
  fIncidentLBVec = rhs.fIncidentLBVec;

  // Central values.

  fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  fCentralPromptPeakCounts = rhs.fCentralPromptPeakCounts;
  fCentralPromptPeakCountsErr = rhs.fCentralPromptPeakCountsErr;

  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;

  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;
  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;

  fCentralSolidAngle = rhs.fCentralSolidAngle;
  fCentralCosTheta = rhs.fCentralCosTheta;

  fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  fCentralCHSFlag = rhs.fCentralCHSFlag;
  fCentralCSSFlag = rhs.fCentralCSSFlag;

  fCentralBadPath = rhs.fCentralBadPath;
  fCentralNeckFlag = rhs.fCentralNeckFlag;

  fCentralInitialLBVec = rhs.fCentralInitialLBVec;
  fCentralIncidentLBVec = rhs.fCentralIncidentLBVec;

  fOccupancyRatio = rhs.fOccupancyRatio;
  fModelOccupancyRatio = rhs.fModelOccupancyRatio;
  fOccupancyRatioErr = rhs.fOccupancyRatioErr;

  fPMTVariability = rhs.fPMTVariability;

  fRunIndex = rhs.fRunIndex;

  fRunEfficiency = rhs.fRunEfficiency;
  fScanEfficiency = rhs.fScanEfficiency;

}

//////////////////////////////////////
//////////////////////////////////////

OCAPMT& OCAPMT::operator=( const OCAPMT& rhs )
{

  // Equality operator set all the variables
  // from the OCAPMT object on the right hand side 'rhs'
  // to the variables on the left hand side 'this' OCAPMT.

  fID = rhs.fID;

  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;
  fCentralIsVerified = rhs.fCentralIsVerified;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fLBPos = rhs.fLBPos;
  fCentralLBPos = rhs.fCentralLBPos;
  fWavelengthLBPos = rhs.fWavelengthLBPos;
  fLBOrientation = rhs.fLBOrientation;
  fCentralLBOrientation = rhs.fCentralLBOrientation;
  fWavelengthLBOrientation = rhs.fWavelengthLBOrientation;

  // Off-axis values.

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;

  fPromptPeakCounts = rhs.fPromptPeakCounts;
  fPromptPeakCountsErr = rhs.fPromptPeakCountsErr;

  fTimeOfFlight = rhs.fTimeOfFlight;

  fLBIntensityNorm = rhs.fLBIntensityNorm;
  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInInnerAV = rhs.fDistInInnerAV;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fRelLBTheta = rhs.fRelLBTheta;
  fRelLBPhi = rhs.fRelLBPhi;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;

  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fInitialLBVec = rhs.fInitialLBVec;
  fIncidentLBVec = rhs.fIncidentLBVec;

  // Central values.

  fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  fCentralPromptPeakCounts = rhs.fCentralPromptPeakCounts;
  fCentralPromptPeakCountsErr = rhs.fCentralPromptPeakCountsErr;

  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;

  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;
  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;

  fCentralSolidAngle = rhs.fCentralSolidAngle;
  fCentralCosTheta = rhs.fCentralCosTheta;

  fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  fCentralCHSFlag = rhs.fCentralCHSFlag;
  fCentralCSSFlag = rhs.fCentralCSSFlag;

  fCentralBadPath = rhs.fCentralBadPath;
  fCentralNeckFlag = rhs.fCentralNeckFlag;

  fCentralInitialLBVec = rhs.fCentralInitialLBVec;
  fCentralIncidentLBVec = rhs.fCentralIncidentLBVec;

  fOccupancyRatio = rhs.fOccupancyRatio;
  fModelOccupancyRatio = rhs.fModelOccupancyRatio;
  fOccupancyRatioErr = rhs.fOccupancyRatioErr;

  fPMTVariability = rhs.fPMTVariability;

  fRunIndex = rhs.fRunIndex;

  fRunEfficiency = rhs.fRunEfficiency;
  fScanEfficiency = rhs.fScanEfficiency;

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAPMT::ClearPMT()
{

  // Set all the private member variables
  // to non-interpretive/physical values.

  SetID( -1 );

  SetRunID( -1 );
  SetCentralRunID( -1 );
  SetWavelengthRunID( -1 );

  SetType ( -1 );

  SetIsVerified( false );
  SetCentralIsVerified( false );

  TVector3 nullVec( -99999.9, -99999.9, -99999.9 );
  SetPos( nullVec );
  SetNorm( nullVec );
  SetLBPos( nullVec );
  SetCentralLBPos( nullVec );
  SetWavelengthLBPos( nullVec );
  SetLBOrientation( -10.0 );
  SetCentralLBOrientation( -10.0 );
  SetWavelengthLBOrientation( -10.0 );
  
  SetPromptPeakTime( -10.0 );
  SetPromptPeakWidth( -10.0 );

  SetPromptPeakCounts( -10.0 );
  SetPromptPeakCountsErr( -10.0 );

  SetTimeOfFlight( -10.0 );

  SetLBIntensityNorm( -10.0 );
  SetNLBPulses( -10.0 );

  SetMPECorrOccupancy( -10.0 );
  SetMPECorrOccupancyErr( -10.0 );

  SetFresnelTCoeff( -10.0 );

  SetDistInInnerAV( -10.0 );
  SetDistInAV( -10.0 );
  SetDistInWater( -10.0 );

  SetSolidAngle( -10.0 );
  SetCosTheta( -10.0 );

  SetRelLBTheta( -10.0 );
  SetRelLBPhi( -10.0 );

  SetCHSFlag( false );
  SetCSSFlag( false );

  SetBadPath( false );
  SetNeckFlag( false );

  SetInitialLBVec( nullVec );
  SetIncidentLBVec( nullVec );

  // Central values.

  SetCentralPromptPeakTime( -10.0 );
  SetCentralPromptPeakWidth( -10.0 );
  SetCentralPromptPeakCounts( -10.0 );
  SetCentralPromptPeakCountsErr( -10.0 );

  SetCentralTimeOfFlight( -10.0 );

  SetCentralLBIntensityNorm( -10.0 );
  SetCentralNLBPulses( -10.0 );

  SetCentralMPECorrOccupancy( -10.0 );
  SetCentralMPECorrOccupancyErr( -10.0 );

  SetCentralFresnelTCoeff( -10.0 );

  SetCentralDistInInnerAV( -10.0 );
  SetCentralDistInAV( -10.0 );
  SetCentralDistInWater( -10.0 );

  SetCentralSolidAngle( -10.0 );
  SetCentralCosTheta( -10.0 );

  SetCentralRelLBTheta( -10.0 );
  SetCentralRelLBPhi( -10.0 );

  SetCentralCHSFlag( false );
  SetCentralCSSFlag( false );

  SetCentralBadPath( false );
  SetCentralNeckFlag( false );

  SetCentralInitialLBVec( nullVec );
  SetCentralIncidentLBVec( nullVec );

  SetOccupancyRatio( -1.0 );
  SetModelOccupancyRatio( -1.0 );
  SetOccupancyRatioErr( -1.0 );

  SetPMTVariability( -1.0 );

  SetRunIndex( -1 );

  SetRunEfficiency( -1.0 );
  SetScanEfficiency( -1.0 );

}

//////////////////////////////////////
//////////////////////////////////////

void OCAPMT::AddSOCPMTData( RAT::DS::SOCPMT& socPMT )
{

  // Set the PMT ID.
  SetID( socPMT.GetLCN() );

  // Set the prompt peak time.
  SetPromptPeakTime( socPMT.GetTimeCentroid() );

  // Set the error on the prompt peak time.
  SetPromptPeakWidth( socPMT.GetTimeCentroidError() );

  // Set the time of flight associated with this prompt
  // peak time from the laserball to the PMT.
  SetTimeOfFlight( socPMT.GetTOFManipulator() );

  // Set the number of counts in the prompt peak
  // timing window (4ns either side of the mean).
  SetPromptPeakCounts( socPMT.GetPromptOccupancy() );

  // Set the error on the number of counts in the prompt
  // peak timing window.
  SetPromptPeakCountsErr( TMath::Sqrt( GetPromptPeakCounts() ) );

}

//////////////////////////////////////
//////////////////////////////////////

void OCAPMT::ProcessLightPath( RAT::DU::LightPathCalculator& lPath,
                               RAT::DU::ShadowingCalculator& shadCalc,
                               RAT::DU::ChanHWStatus& chanHW )
{

  // First check that the PMT is a normal type PMT i.e. fType == 1.
  if ( fType == 1 ){
    
    // If the light path calculation associated with this
    // PMT was straight, or it encountered shadowing, then
    // set the 'BadPath' boolean.
    if ( lPath.GetStraightLine() 
         || shadCalc.CheckForShadowing( lPath ) == true ){ 
      SetBadPath( true ); 
    }

    // If the light path calculation  associated with
    // this PMT intersected the AV neck, then set
    // the 'XAVNeck' flag to indicate this.
    if ( lPath.GetXAVNeck() ){ 
      SetNeckFlag( true );

      // NOTE: Current neck distances need fixing
      // in RAT::DU::LightPathCalculator.
      // SetDistInNeck( lPath.GetDistInNeck() );
    }

    // Set the light path distances through the three
    // different detector materials.
    SetDistInInnerAV( lPath.GetDistInInnerAV() );
    SetDistInAV( lPath.GetDistInAV() );
    SetDistInWater( lPath.GetDistInWater() );

    // Set the initial and final photon vectors
    // from the laserball (Initial) to the PMT
    // bucket (Incident).
    SetInitialLBVec( lPath.GetInitialLightVec() );
    SetIncidentLBVec( lPath.GetIncidentVecOnPMT() );

    // Set the Fresnel transmission coefficient for the
    // light path associated with this PMT.
    SetFresnelTCoeff( lPath.GetFresnelTCoeff() );

    // Calculate and set the solid angle subtended by this PMT
    // as viewed from the laserball position.
    lPath.CalculateSolidAngle( -fNorm, 0 );
    SetSolidAngle( lPath.GetSolidAngle() );

    // Set the cosine of the incident angle of the light
    // arriving at the PMT bucket. 
    // 'RAT::DU::LightPathCalculator::GetCosThetaAvg()'
    // is used because the solid angle calculates an average
    // over four different points to calculate the solid angle.
    // This 'average' value is the most reliable.
    SetCosTheta( lPath.GetCosThetaAvg() );

    // Set the channel ahrdware status flag for this PMT in this run.
    if ( chanHW.IsChannelOnline( GetID() ) ){ SetCHSFlag( 1 ); }
    else{ SetCHSFlag( 0 ); }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCAPMT::VerifyPMT()
{

  // Assume the verification is true to begin with.
  fIsVerified = true;

  // Check that the PMT ID is a sensible value and that
  // the PMT is of type 'normal' (i.e. fType == 1).
  if ( fID < 0 || fID > 10000 ){ 
    SetIsVerified( false ); return; 
  }
  if ( fType != 1 ){ 
    SetIsVerified( false ); return;
  }
  
  // Check the magnitudes of the PMT position and direction normals.
  // The PMT should be in the PSUP so shouldn't have a radii less than
  // 8000.0 mm or greater than 9000.0. (relaxed limits).
  if ( fPos.Mag() < 8000.0 || fPos.Mag() > 9000.0  ){ 
    SetIsVerified( false ); return; 
  }
  if ( fNorm.Mag() < 0.9 || fNorm.Mag() > 1.1 ){ 
    SetIsVerified( false ); return;
  }
  
  // Ensure the prompt peak time is non-zero.
  if ( fPromptPeakTime == 0.0 || fPromptPeakTime > 500.0 ){ 
    SetIsVerified( false ); return; 
  }

  // Ensure the time of flight isn't larger than a typical
  // event window.
  if ( fTimeOfFlight <= 0.1 || fTimeOfFlight > 500.0 ){ 
    SetIsVerified( false ); return; 
  }

  // Ensure the prompt peak counts is non-zero.
  if ( fPromptPeakCounts < 0.0 ){ SetIsVerified( false ); return; }

  // Ensure the multi-photoelectron corrected occupancy is non-zero.
  if ( fMPECorrOccupancy < 0.0 ){ SetIsVerified( false ); return; }

  // Ensure the Fresnel transmission coefficients are between
  // 0.0 and 1.0. i.e. (0.0, 1.0]
  if ( fFresnelTCoeff == 0.0 || fFresnelTCoeff > 1.0 ){ 
    SetIsVerified( false ); return;
  }

  // Check that the distances in the inner AV, AV 
  // and water regions have sensible values.
  if ( fDistInInnerAV < 0.0 || fDistInInnerAV > 12100.0 ){ 
    SetIsVerified( false ); return; 
  }
  if ( fDistInAV < 0.0 || fDistInAV > 1000.0 ){ 
    SetIsVerified( false ); return; 
  }
  if ( fDistInWater < 0.0 || fDistInWater > 10000.0 ){ 
    SetIsVerified( false ); return; 
  }
  
  // Ensure the solid angle is non-zero.
  if ( fSolidAngle <= 0.0 ){ 
    SetIsVerified( false ); return; 
  }

  // Ensure the cosine of the incident angle at the PMT bucket
  // is within the correct boundaries [0.0, 1.0].
  if ( fCosTheta < 0.0 || fCosTheta > 1.0 ){ 
    SetIsVerified( false ); return; 
  }

  if ( fCHSFlag != 1 ){
    SetIsVerified( false ); return;
  }
  
}
