#include "LOCASPMT.hh"

#include "TMath.h"

#include <iostream>
#include <vector>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASPMT )

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT::LOCASPMT( const LOCASPMT& rhs )
{
  // Copy constructor, set all the variables
  // from the LOCASPMT object on the right hand side 'rhs'
  // to the variables on the left hand side 'this' LOCASPMT.

  fID = rhs.fID;

  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;
  fCentralIsVerified = rhs.fCentralIsVerified;
  fWavelengthIsVerified = rhs.fWavelengthIsVerified;

  fDQXXFlag = rhs.fDQXXFlag;
  fCentralDQXXFlag = rhs.fCentralDQXXFlag;
  fWavelengthDQXXFlag = rhs.fWavelengthDQXXFlag;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  // Off-axis values.

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fLBIntensityNorm = rhs.fLBIntensityNorm;

  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInInnerAV = rhs.fDistInInnerAV;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

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
  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  fCentralOccupancy = rhs.fCentralOccupancy;
  fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;
  fCentralDistInNeck = rhs.fCentralDistInNeck;
  fCentralTotalDist = rhs.fCentralTotalDist;

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

  // Wavelength values.

  fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;

  fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  fWavelengthDistInInnerAV = rhs.fWavelengthDistInInnerAV;
  fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;

  fWavelengthBadPath = rhs.fWavelengthBadPath;
  fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;

  fWavelengthInitialLBVec = rhs.fWavelengthInitialLBVec;
  fWavelengthIncidentLBVec = rhs.fWavelengthIncidentLBVec;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASPMT::operator=( const LOCASPMT& rhs )
{

  // Equality operator set all the variables
  // from the LOCASPMT object on the right hand side 'rhs'
  // to the variables on the left hand side 'this' LOCASPMT.

  fID = rhs.fID;

  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;
  fCentralIsVerified = rhs.fCentralIsVerified;
  fWavelengthIsVerified = rhs.fWavelengthIsVerified;

  fDQXXFlag = rhs.fDQXXFlag;
  fCentralDQXXFlag = rhs.fCentralDQXXFlag;
  fWavelengthDQXXFlag = rhs.fWavelengthDQXXFlag;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  // Off-axis values.

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fLBIntensityNorm = rhs.fLBIntensityNorm;

  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInInnerAV = rhs.fDistInInnerAV;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

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
  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  fCentralOccupancy = rhs.fCentralOccupancy;
  fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;
  fCentralDistInNeck = rhs.fCentralDistInNeck;
  fCentralTotalDist = rhs.fCentralTotalDist;

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

  // Wavelength values.

  fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;

  fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  fWavelengthDistInInnerAV = rhs.fWavelengthDistInInnerAV;
  fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;

  fWavelengthBadPath = rhs.fWavelengthBadPath;
  fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;

  fWavelengthInitialLBVec = rhs.fWavelengthInitialLBVec;
  fWavelengthIncidentLBVec = rhs.fWavelengthIncidentLBVec;
 
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::ClearPMT()
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
  SetWavelengthIsVerified( false );

  SetDQXXFlag( -1 );
  SetCentralDQXXFlag( -1 );
  SetWavelengthDQXXFlag( -1 );

  TVector3 nullVec( -99999.9, -99999.9, -99999.9 );
  SetPos( nullVec );
  SetNorm( nullVec );

  SetPromptPeakTime( -10.0 );
  SetPromptPeakWidth( -10.0 );
  SetTimeOfFlight( -10.0 );
  SetOccupancy( -10.0 );
  SetOccupancyErr( -10.0 );

  SetLBIntensityNorm( -10.0 );
  SetNLBPulses( -10.0 );

  SetMPECorrOccupancy( -10.0 );
  SetMPECorrOccupancyErr( -10.0 );

  SetFresnelTCoeff( -10.0 );

  SetDistInInnerAV( -10.0 );
  SetDistInAV( -10.0 );
  SetDistInWater( -10.0 );
  SetDistInNeck( -10.0 );
  SetTotalDist( -10.0 );

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
  SetCentralTimeOfFlight( -10.0 );
  SetCentralOccupancy( -10.0 );
  SetCentralOccupancyErr( -10.0 );

  SetCentralLBIntensityNorm( -10.0 );
  SetCentralNLBPulses( -10.0 );

  SetCentralMPECorrOccupancy( -10.0 );
  SetCentralMPECorrOccupancyErr( -10.0 );

  SetCentralFresnelTCoeff( -10.0 );

  SetCentralDistInInnerAV( -10.0 );
  SetCentralDistInAV( -10.0 );
  SetCentralDistInWater( -10.0 );
  SetCentralDistInNeck( -10.0 );
  SetCentralTotalDist( -10.0 );

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

  // Wavelength values.

  SetWavelengthPromptPeakTime( -10.0 );
  SetWavelengthPromptPeakWidth( -10.0 );
  SetWavelengthTimeOfFlight( -10.0 );
  SetWavelengthOccupancy( -10.0 );
  SetWavelengthOccupancyErr( -10.0 );

  SetWavelengthLBIntensityNorm( -10.0 );
  SetWavelengthNLBPulses( -10.0 );

  SetWavelengthMPECorrOccupancy( -10.0 );
  SetWavelengthMPECorrOccupancyErr( -10.0 );

  SetWavelengthFresnelTCoeff( -10.0 );

  SetWavelengthDistInInnerAV( -10.0 );
  SetWavelengthDistInAV( -10.0 );
  SetWavelengthDistInWater( -10.0 );
  SetWavelengthDistInNeck( -10.0 );
  SetWavelengthTotalDist( -10.0 );

  SetWavelengthSolidAngle( -10.0 );
  SetWavelengthCosTheta( -10.0 );

  SetWavelengthRelLBTheta( -10.0 );
  SetWavelengthRelLBPhi( -10.0 );

  SetWavelengthCHSFlag( false );
  SetWavelengthCSSFlag( false );
  SetWavelengthBadPath( false );
  SetWavelengthNeckFlag( false );

  SetWavelengthInitialLBVec( nullVec );
  SetWavelengthIncidentLBVec( nullVec );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::AddSOCPMTData( RAT::DS::SOCPMT& socPMT )
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
  SetOccupancy( socPMT.GetPromptOccupancy() );

  // Set the error on the number of counts in the prompt
  // peak timing window.
  SetOccupancyErr( TMath::Sqrt( fOccupancy ) );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::ProcessLightPath( RAT::DU::LightPathCalculator& lPath )
{

  // First check that the PMT is a normal type PMT i.e. fType == 1.
  if ( fType == 1 ){
    
    // If the light path calculation associated with this
    // PMT was straight, or it encountered shadowing, then
    // set the 'BadPath' boolean.
    if ( lPath.GetStraightLine() 
         || lPath.CheckForShadowing() == true ){ 
      SetBadPath( true ); 
    }

    // If the light path calculation  associated with
    // this PMT intersected the AV neck, then set
    // the 'XAVNeck' flag to indicate this.
    if ( lPath.GetXAVNeck() ){ 
      SetNeckFlag( true );

      // NOTE: Current neck distances need fixing
      // in RAT::DU::LightPathCalculator.
      //SetDistInNeck( lPath.GetDistInNeck() );
    }
    
    // NOTE: Current neck distances need fixing
    // in RAT::DU::LightPathCalculator.
    //SetDistInNeck( lPath.GetDistInNeck() );

    // Set the light path distances through the three
    // different detector materials.
    SetDistInInnerAV( lPath.GetDistInInnerAV() );
    SetDistInAV( lPath.GetDistInAV() );
    SetDistInWater( lPath.GetDistInWater() );
    SetTotalDist( lPath.GetTotalDist() );

    // Set the initial and final photon vectors
    // from the laserball (Initial) to the PMT
    // bucket (Incident).
    SetInitialLBVec( lPath.GetInitialLightVec() );
    SetIncidentLBVec( lPath.GetIncidentVecOnPMT() );

    // Set the relative theta and phi values for
    // the light from the laserball. These theta
    // and phi values are in the local frame of the
    // laserball axis.
    SetRelLBTheta( GetInitialLBVec().Theta() );
    SetRelLBPhi( GetInitialLBVec().Phi() );

    // Set the Fresnel transmission coefficient for the
    // light path associated with this PMT.
    SetFresnelTCoeff( lPath.GetFresnelTCoeff() );

    // Calculate and set the solid angle subtended by this PMT
    // as viewed from the laserball position.
    lPath.CalculateSolidAngle( fNorm, 0 );
    SetSolidAngle( lPath.GetSolidAngle() );

    // Set the cosine of the incident angle of the light
    // arriving at the PMT bucket. 
    // 'RAT::DU::LightPathCalculator::GetCosThetaAvg()'
    // is used because the solid angle calculates an average
    // over four different points to calculate the solid angle.
    // This 'average' value is the most reliable.
    SetCosTheta( lPath.GetCosThetaAvg() );

    // Set the DQXX flag (=1 by default whilst RAT functionality
    // for the SOCPMTs is made compatible with the flags).
    SetDQXXFlag( 1 );

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::VerifyPMT()
{

  // Assume the verification is true to begin with.
  fIsVerified = true;

  // Check that the PMT ID is a sensible value and that
  // the PMT is of type 'normal' (i.e. fType == 1).
  if ( fID < 0 || fID > 10000 ){ 
    SetIsVerified( false ); 
  }
  if ( fType != 1 ){ 
    SetIsVerified( false ); 
  }
  
  // Check the magnitudes of the PMT position and direction normals.
  // The PMT should be in the PSUP so shouldn't have a radii less than
  // 8000.0 mm or greater than 9000.0. (relaxed limits).
  if ( fPos.Mag() < 8000.0 || fPos.Mag() > 9000.0  ){ 
    SetIsVerified( false ); 
  }
  if ( fNorm.Mag() < 0.9 || fNorm.Mag() > 1.1 ){ 
    SetIsVerified( false ); 
  }
  
  // Ensure the prompt peak time is non-zero.
  if ( fPromptPeakTime == 0.0 || fPromptPeakTime > 500.0 ){ 
    SetIsVerified( false ); 
  }

  // Ensure the time of flight isn't larger than a typical
  // event window.
  if ( fTimeOfFlight <= 0.1 || fTimeOfFlight > 500.0 ){ 
    SetIsVerified( false ); 
  }

  // Ensure the occupancy is non-zero.
  if ( fOccupancy <= 0.1 ){ SetIsVerified( false ); }

  // Ensure the multi-photoelectron corrected occupancy is non-zero.
  if ( fMPECorrOccupancy <= 0.0 ){ SetIsVerified( false ); }

  // Ensure the Fresnel transmission coefficients are between
  // 0.0 and 1.0. i.e. (0.0, 1.0]
  if ( fFresnelTCoeff == 0.0 || fFresnelTCoeff > 1.0 ){ 
    SetIsVerified( false ); 
  }

  // Check that the distances in the inner AV, AV 
  // and water regions have sensible values.
  if ( fDistInInnerAV < 0.0 || fDistInInnerAV > 13000.0 ){ 
    SetIsVerified( false ); 
  }
  if ( fDistInAV < 0.0 || fDistInAV > 1000.0 ){ 
    SetIsVerified( false ); 
  }
  if ( fDistInWater < 0.0 || fDistInWater > 10000.0 ){ 
    SetIsVerified( false ); 
  }
  
  // Ensure the solid angle is non-zero.
  if ( fSolidAngle == 0.0 ){ 
    SetIsVerified( false ); 
  }

  // Ensure the cosine of the incident angle at the PMT bucket
  // is within the correct boundaries [0.0, 1.0].
  if ( fCosTheta < 0.0 || fCosTheta > 1.0 ){ 
    SetIsVerified( false ); 
  }
  
}
