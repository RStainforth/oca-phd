////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataPoint.cc
///
/// CLASS: LOCAS::LOCASDataPoint
///
/// BRIEF: Data-level structure for data
///        points. These data-points are fed
///        into the LOCASMethod object alongside a
///        LOCASDataFilter in order to produce a
///        data point set
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASRawDataPoint.hh"
#include "LOCASDataPoint.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataPoint )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint::LOCASDataPoint( const LOCASRawDataPoint dataPoint )
{

  SetOccRatio( ( dataPoint.GetOccRatio() ) );

  SetOccRatioErr( ( dataPoint.GetOccRatioErr() ) );

  SetDeltaDistInScint( ( dataPoint.GetDistInScint() - dataPoint.GetCentralDistInScint() ) );
  SetDeltaDistInAV( ( dataPoint.GetDistInAV() - dataPoint.GetCentralDistInAV() ) );
  SetDeltaDistInWater( ( dataPoint.GetDistInWater() - dataPoint.GetCentralDistInWater() ) );

  SetSolidAngleRatio( ( dataPoint.GetSolidAngle() / dataPoint.GetCentralSolidAngle() ) );
  SetFresnelTCoeffRatio( ( dataPoint.GetFresnelTCoeff() / dataPoint.GetCentralFresnelTCoeff() ) );

  SetLBTheta( dataPoint.GetLBTheta() );
  SetCentralLBTheta( dataPoint.GetCentralLBTheta() );

  SetLBPhi( dataPoint.GetLBPhi() );
  SetCentralLBPhi( dataPoint.GetCentralLBPhi() );

  SetIncidentAngle( dataPoint.GetIncidentAngle() );
  SetCentralIncidentAngle( dataPoint.GetCentralIncidentAngle() );

  SetLBIntensityNormRatio( ( dataPoint.GetLBIntensityNorm() / dataPoint.GetCentralLBIntensityNorm() ) );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint& LOCASDataPoint::operator=( const LOCASDataPoint& rhs )
{

  SetOccRatio( rhs.GetOccRatio() );

  SetOccRatioErr( rhs.GetOccRatioErr() );

  SetDeltaDistInScint( rhs.GetDeltaDistInScint() );
  SetDeltaDistInAV( rhs.GetDeltaDistInAV() );
  SetDeltaDistInWater( rhs.GetDeltaDistInWater() );

  SetSolidAngleRatio( rhs.GetSolidAngleRatio() );
  SetFresnelTCoeffRatio( rhs.GetFresnelTCoeffRatio() );

  SetLBTheta( rhs.GetLBTheta() );
  SetCentralLBTheta( rhs.GetCentralLBTheta() );

  SetLBPhi( rhs.GetLBPhi() );
  SetCentralLBPhi( rhs.GetCentralLBPhi() );

  SetLBIntensityNormRatio( rhs.GetLBIntensityNormRatio() );

  return *this;

}
