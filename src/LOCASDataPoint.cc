#include "LOCASPMT.hh"
#include "LOCASDataPoint.hh"

#include "TVector3.h"
#include "TMath.h"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataPoint )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint::LOCASDataPoint( const LOCASPMT& lPMT )
{
  
  SetRunID( lPMT.GetRunID() );
  SetPMTID( lPMT.GetID() );
  // Note: SetRunIndex not set here, it is set when the datapoint
  // is used as part of a data sample in a fit. see LOCAS::LOCASDataFiller

  SetMPECorrOccupancy( lPMT.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( lPMT.GetCentralMPECorrOccupancy() );

  SetMPECorrOccupancyErr( lPMT.GetMPECorrOccupancyErr() );
  SetCentralMPECorrOccupancyErr( lPMT.GetCentralMPECorrOccupancyErr() );

  SetOccupancyRatio( 0.0 );
  SetOccupancyRatioErr( 0.0 );
  SetModelOccupancyRatio( 0.0 );

  SetDistInInnerAV( lPMT.GetDistInInnerAV() );
  SetCentralDistInInnerAV( lPMT.GetCentralDistInInnerAV() );
  SetDistInAV( lPMT.GetDistInAV() );
  SetCentralDistInAV( lPMT.GetCentralDistInAV() );
  SetDistInWater( lPMT.GetDistInWater() );
  SetCentralDistInWater( lPMT.GetCentralDistInWater() );

  SetSolidAngle( lPMT.GetSolidAngle() );
  SetCentralSolidAngle( lPMT.GetCentralSolidAngle() );

  SetFresnelTCoeff( lPMT.GetFresnelTCoeff() );
  SetCentralFresnelTCoeff( lPMT.GetCentralFresnelTCoeff() );

  SetIncidentAngle( TMath::ACos( lPMT.GetCosTheta() ) * 180.0 / TMath::Pi() );
  SetCentralIncidentAngle( TMath::ACos( lPMT.GetCentralCosTheta() ) * 180.0 / TMath::Pi() );

  SetLBTheta( ( lPMT.GetInitialLBVec() ).Theta() );
  SetCentralLBTheta( ( lPMT.GetCentralInitialLBVec() ).Theta() );

  SetLBPhi( ( lPMT.GetInitialLBVec() ).Phi() );
  SetCentralLBPhi( ( lPMT.GetCentralInitialLBVec() ).Phi() );

  SetLBIntensityNorm( lPMT.GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( lPMT.GetCentralLBIntensityNorm() );

  SetCHSFlag( lPMT.GetDQXXFlag() );
  SetCentralCHSFlag( lPMT.GetCentralDQXXFlag() );

  // FIXME CSS and CHS values
  //SetCSSFlag( lPMT.GetANXXFlag() );
  //SetCentralCSSFlag( lPMT.GetCentralANXXFlag() );

  SetBadPathFlag( lPMT.GetBadPath() );
  SetCentralBadPathFlag( lPMT.GetCentralBadPath() );
  
}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint& LOCASDataPoint::operator=( const LOCASDataPoint& rhs )
{

  SetRunID( rhs.GetRunID() );
  SetPMTID( rhs.GetPMTID() );
  SetRunIndex( rhs.GetRunIndex() );

  SetMPECorrOccupancy( rhs.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( rhs.GetCentralMPECorrOccupancy() );

  SetMPECorrOccupancyErr( rhs.GetMPECorrOccupancyErr() );
  SetCentralMPECorrOccupancyErr( rhs.GetCentralMPECorrOccupancyErr() );

  SetOccupancyRatio( rhs.GetOccupancyRatio() );
  SetOccupancyRatioErr( rhs.GetOccupancyRatioErr() );
  SetModelOccupancyRatio( rhs.GetModelOccupancyRatio() );

  SetDistInInnerAV( rhs.GetDistInInnerAV() );
  SetCentralDistInInnerAV( rhs.GetCentralDistInInnerAV() );
  SetDistInAV( rhs.GetDistInAV() );
  SetCentralDistInAV( rhs.GetCentralDistInAV() );
  SetDistInWater( rhs.GetDistInWater() );
  SetCentralDistInWater( rhs.GetCentralDistInWater() );

  SetSolidAngle( rhs.GetSolidAngle() );
  SetCentralSolidAngle( rhs.GetCentralSolidAngle() );

  SetFresnelTCoeff( rhs.GetFresnelTCoeff() );
  SetCentralFresnelTCoeff( rhs.GetCentralFresnelTCoeff() );

  SetLBTheta( rhs.GetLBTheta() );
  SetCentralLBTheta( rhs.GetCentralLBTheta() );

  SetLBPhi( rhs.GetLBPhi() );
  SetCentralLBPhi( rhs.GetCentralLBPhi() );

  SetIncidentAngle( rhs.GetIncidentAngle() );
  SetCentralIncidentAngle( rhs.GetCentralIncidentAngle() );

  SetLBIntensityNorm( rhs.GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( rhs.GetCentralLBIntensityNorm() );

  SetCSSFlag( rhs.GetCSSFlag() );
  SetCentralCSSFlag( rhs.GetCentralCSSFlag() );

  SetCHSFlag( rhs.GetCHSFlag() );
  SetCentralCHSFlag( rhs.GetCentralCHSFlag() );

  SetBadPathFlag( rhs.GetBadPathFlag() );
  SetCentralBadPathFlag( rhs.GetCentralBadPathFlag() );

  return *this;

}
