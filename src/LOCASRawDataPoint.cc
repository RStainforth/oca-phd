////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRawDataPoint.cc
///
/// CLASS: LOCAS::LOCASRawDataPoint
///
/// BRIEF: Raw data-level structure for data
///        points. Many raw data points are held in
///        a LOCASRawDataStore object in order to apply a
///        collection of cuts/filters (LOCASFilterStore)
///        to obtain a data set ready for use in a fit
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASPMT.hh"
#include "LOCASMath.hh"
#include "LOCASRun.hh"
#include "LOCASRawDataPoint.hh"

#include "TVector3.h"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASRawDataPoint )

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataPoint::LOCASRawDataPoint( const LOCASPMT* pmtPtr, const LOCASRun* runPtr )
{

  SetRunID( runPtr->GetRunID() );

  SetMPEOccRatio( pmtPtr->GetMPECorrOccupancy() / pmtPtr->GetCentralMPECorrOccupancy() ); 
  SetMPEOccRatioErr( LOCASMath::OccRatioErr( pmtPtr ) );

  SetMPECorrOccupancy( pmtPtr->GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( pmtPtr->GetCentralMPECorrOccupancy() );
  
  SetDistInScint( pmtPtr->GetDistInScint() );
  SetCentralDistInScint( pmtPtr->GetCentralDistInScint() );
  SetDistInAV( pmtPtr->GetDistInAV() );
  SetCentralDistInAV( pmtPtr->GetCentralDistInAV() );
  SetDistInWater( pmtPtr->GetDistInWater() );
  SetCentralDistInWater( pmtPtr->GetCentralDistInWater() );
  SetTotalDist( pmtPtr->GetTotalDist() );
  SetCentralTotalDist( pmtPtr->GetCentralTotalDist() );

  SetLBIntensityNorm( pmtPtr->GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( pmtPtr->GetCentralLBIntensityNorm() );

  if ( ( pmtPtr->GetCosTheta() <= 1.0 ) && ( pmtPtr->GetCosTheta() >= 0.0 ) ){
    SetIncidentAngle( TMath::ACos( pmtPtr->GetCosTheta() ) * ( 180.0 / TMath::Pi() ) );
  }
  else{ SetIncidentAngle( -10.0 ); }

  if ( ( pmtPtr->GetCentralCosTheta() <= 1.0 ) && ( pmtPtr->GetCentralCosTheta() >= 0.0 ) ){
    SetCentralIncidentAngle( TMath::ACos( pmtPtr->GetCentralCosTheta() ) * ( 180.0 / TMath::Pi() ) );
  }
  else{ SetCentralIncidentAngle( -10.0 ); }

  SetSolidAngle( pmtPtr->GetSolidAngle() );
  SetCentralSolidAngle( pmtPtr->GetCentralSolidAngle() );

  SetFresnelTCoeff( pmtPtr->GetFresnelTCoeff() );
  SetCentralFresnelTCoeff( pmtPtr->GetCentralFresnelTCoeff() );

  SetLBTheta( pmtPtr->GetRelLBTheta() );
  SetCentralLBTheta( pmtPtr->GetCentralRelLBTheta() );

  SetLBPhi( pmtPtr->GetRelLBPhi() );
  SetCentralLBPhi( pmtPtr->GetCentralRelLBPhi() );

  if ( pmtPtr->GetCHSFlag() ){ SetCHSFlag( 1 ); }
  else{ SetCHSFlag( 0 ); }

  if ( pmtPtr->GetCentralCHSFlag() ){ SetCentralCHSFlag( 1 ); }
  else{ SetCentralCHSFlag( 0 ); }

  if ( pmtPtr->GetCSSFlag() ){ SetCSSFlag( 1 ); }
  else{ SetCentralCSSFlag( 0 ); }

  if ( pmtPtr->GetCentralCSSFlag() ){ SetCentralCSSFlag( 1 ); }
  else{ SetCentralCSSFlag( 0 ); }

  SetTimeOfFlight( pmtPtr->GetTimeOfFlight() );
  SetCentralTimeOfFlight( pmtPtr->GetCentralTimeOfFlight() );

  if ( pmtPtr->GetBadPath() ){ SetBadPathFlag( 1 ); }
  else{ SetBadPathFlag( 0 ); }

  if ( pmtPtr->GetCentralBadPath() ){ SetCentralBadPathFlag( 1 ); }
  else{ SetCentralBadPathFlag( 0 ); }

  if ( pmtPtr->GetNeckFlag() ){ SetNeckFlag( 1 ); }
  else{ SetNeckFlag( 0 ); }

  if ( pmtPtr->GetCentralNeckFlag() ){ SetCentralNeckFlag( 1 ); }
  else{ SetCentralNeckFlag( 0 ); }

  SetPMTPos( pmtPtr->GetPos() );

  SetLBPos( runPtr->GetLBPos() );

  SetModelCorrOccRatio( ( ( GetCentralSolidAngle() * GetCentralFresnelTCoeff() ) /
                          ( GetSolidAngle() * GetFresnelTCoeff() ) )
                        * ( GetMPEOccRatio() ) );
  
}


//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataPoint& LOCASRawDataPoint::operator=( const LOCASRawDataPoint& rhs )
{

  SetRunID( rhs.GetRunID() );

  SetMPEOccRatio( rhs.GetMPECorrOccupancy() / rhs.GetCentralMPECorrOccupancy() );
  SetMPEOccRatioErr( rhs.GetMPEOccRatioErr() );

  SetMPECorrOccupancy( rhs.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( rhs.GetCentralMPECorrOccupancy() );

  SetModelCorrOccRatio( rhs.GetModelCorrOccRatio() );

  SetDistInScint( rhs.GetDistInScint() );
  SetCentralDistInScint( rhs.GetCentralDistInScint() );
  SetDistInAV( rhs.GetDistInAV() );
  SetCentralDistInAV( rhs.GetCentralDistInAV() );
  SetDistInWater( rhs.GetDistInWater() );
  SetCentralDistInWater( rhs.GetCentralDistInWater() );
  SetTotalDist( rhs.GetTotalDist() );
  SetCentralTotalDist( rhs.GetCentralTotalDist() );

  SetSolidAngle( rhs.GetSolidAngle() );
  SetCentralSolidAngle( rhs.GetCentralSolidAngle() );

  SetFresnelTCoeff( rhs.GetFresnelTCoeff() );
  SetCentralFresnelTCoeff( rhs.GetCentralFresnelTCoeff() );

  SetLBIntensityNorm( rhs.GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( rhs.GetCentralLBIntensityNorm() );

  SetIncidentAngle( rhs.GetIncidentAngle() );
  SetCentralIncidentAngle( rhs.GetCentralIncidentAngle() );

  SetLBTheta( rhs.GetLBTheta() );
  SetCentralLBTheta( rhs.GetCentralLBTheta() );

  SetLBPhi( rhs.GetLBPhi() );
  SetCentralLBPhi( rhs.GetCentralLBPhi() );

  SetAVHDShadowingVal( rhs.GetAVHDShadowingVal() );
  SetCentralAVHDShadowingVal( rhs.GetCentralAVHDShadowingVal() );

  SetGeometricShadowingVal( rhs.GetGeometricShadowingVal() );
  SetCentralGeometricShadowingVal( rhs.GetCentralGeometricShadowingVal() );

  SetCHSFlag( rhs.GetCHSFlag() );
  SetCentralCHSFlag( rhs.GetCentralCHSFlag()  );
  
  SetCSSFlag( rhs.GetCSSFlag() );
  SetCentralCSSFlag( rhs.GetCentralCSSFlag()  );
  
  SetTimeOfFlight( rhs.GetTimeOfFlight() );
  SetCentralTimeOfFlight( rhs.GetCentralTimeOfFlight() );
  
  SetBadPathFlag( rhs.GetBadPathFlag() );
  SetCentralBadPathFlag( rhs.GetCentralBadPathFlag()  );

  SetNeckFlag( rhs.GetNeckFlag() );
  SetCentralNeckFlag( rhs.GetCentralNeckFlag() );

  SetPMTPos( rhs.GetPMTPos() );

  SetLBPos( rhs.GetLBPos() );

  return *this;

}
