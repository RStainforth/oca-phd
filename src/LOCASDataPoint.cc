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
  
  //cout << "Test 1" << endl;
  SetRunID( lPMT.GetRunID() );

  SetMPECorrOccupancy( lPMT.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( lPMT.GetCentralMPECorrOccupancy() );

  SetMPECorrOccupancyErr( lPMT.GetMPECorrOccupancyErr() );
  SetCentralMPECorrOccupancyErr( lPMT.GetCentralMPECorrOccupancyErr() );
  //cout << "Test 2" << endl;
  SetDistInInnerAV( lPMT.GetDistInInnerAV() );
  SetCentralDistInInnerAV( lPMT.GetCentralDistInInnerAV() );
  SetDistInAV( lPMT.GetDistInAV() );
  SetCentralDistInAV( lPMT.GetCentralDistInAV() );
  SetDistInWater( lPMT.GetDistInWater() );
  SetCentralDistInWater( lPMT.GetCentralDistInWater() );
  //cout << "Test 3" << endl;
  SetSolidAngle( lPMT.GetSolidAngle() );
  SetCentralSolidAngle( lPMT.GetCentralSolidAngle() );
  //cout << "Test 4" << endl;
  SetFresnelTCoeff( lPMT.GetFresnelTCoeff() );
  SetCentralFresnelTCoeff( lPMT.GetCentralFresnelTCoeff() );

  SetIncidentAngle( TMath::ACos( lPMT.GetCosTheta() ) * 180.0 / TMath::Pi() );
  SetCentralIncidentAngle( TMath::ACos( lPMT.GetCentralCosTheta() ) * 180.0 / TMath::Pi() );
  //cout << "Test 5" << endl;
  SetLBTheta( ( lPMT.GetInitialLBVec() ).Theta() );
  SetCentralLBTheta( ( lPMT.GetCentralInitialLBVec() ).Theta() );
  //cout << "Test 6" << endl;
  SetLBPhi( ( lPMT.GetInitialLBVec() ).Phi() );
  SetCentralLBPhi( ( lPMT.GetCentralInitialLBVec() ).Phi() );
  //cout << "Test 7" << endl;
  SetLBIntensityNorm( lPMT.GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( lPMT.GetCentralLBIntensityNorm() );
  //cout << "Test 8" << endl;
  SetCHSFlag( lPMT.GetDQXXFlag() );
  SetCentralCHSFlag( lPMT.GetCentralDQXXFlag() );

  //SetCSSFlag( lPMT.GetANXXFlag() );
  //SetCentralCSSFlag( lPMT.GetCentralANXXFlag() );
  //cout << "Test 9" << endl;
  SetBadPathFlag( lPMT.GetBadPath() );
  SetCentralBadPathFlag( lPMT.GetCentralBadPath() );
  //cout << "Test 10" << endl;
  
}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint& LOCASDataPoint::operator=( const LOCASDataPoint& rhs )
{

  SetRunID( rhs.GetRunID() );

  SetMPECorrOccupancy( rhs.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( rhs.GetCentralMPECorrOccupancy() );

  SetMPECorrOccupancyErr( rhs.GetMPECorrOccupancyErr() );
  SetCentralMPECorrOccupancyErr( rhs.GetCentralMPECorrOccupancyErr() );

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
