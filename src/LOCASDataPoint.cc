////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataPoint.cc
///
/// CLASS: LOCAS::LOCASDataPoint
///
/// BRIEF: Data-level structure for data
///        points. These data points are used in a 
///        chisquare function which is minimised over.
///        These are the individual data points used in a fit
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

  SetMPEOccRatio( ( dataPoint.GetMPEOccRatio() ) );
  SetRawOccRatio( ( dataPoint.GetRawOccRatio() ) );

  SetMPEOccRatioErr( ( dataPoint.GetMPEOccRatioErr() ) );
  SetRawOccRatioErr( ( dataPoint.GetRawOccRatioErr() ) );

  SetDeltaDistInScint( ( dataPoint.GetDistInScint() - dataPoint.GetCentralDistInScint() ) );
  SetDeltaDistInAV( ( dataPoint.GetDistInAV() - dataPoint.GetCentralDistInAV() ) );
  SetDeltaDistInWater( ( dataPoint.GetDistInWater() - dataPoint.GetCentralDistInWater() ) );

  SetSolidAngleRatio( ( dataPoint.GetSolidAngle() / dataPoint.GetCentralSolidAngle() ) );
  SetFresnelTCoeffRatio( ( dataPoint.GetFresnelTCoeff() / dataPoint.GetCentralFresnelTCoeff() ) );

  SetIncidentAngle( dataPoint.GetIncidentAngle() );
  SetCentralIncidentAngle( dataPoint.GetCentralIncidentAngle() );

  SetLBTheta( dataPoint.GetLBTheta() );
  SetCentralLBTheta( dataPoint.GetCentralLBTheta() );

  SetLBPhi( dataPoint.GetLBPhi() );
  SetCentralLBPhi( dataPoint.GetCentralLBPhi() );

  SetLBIntensityNormRatio( ( dataPoint.GetLBIntensityNorm() / dataPoint.GetCentralLBIntensityNorm() ) );

  SetModelCorrMPEOccRatio( GetMPEOccRatio() * ( 1.0 / GetSolidAngleRatio() ) * ( 1.0 / GetFresnelTCoeffRatio() ) );
  SetModelCorrRawOccRatio( GetRawOccRatio() * ( 1.0 / GetSolidAngleRatio() ) * ( 1.0 / GetFresnelTCoeffRatio() ) );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint& LOCASDataPoint::operator=( const LOCASDataPoint& rhs )
{

  SetMPEOccRatio( rhs.GetMPEOccRatio() );
  SetRawOccRatio( rhs.GetRawOccRatio() );

  SetMPEOccRatioErr( rhs.GetMPEOccRatioErr() );
  SetRawOccRatioErr( rhs.GetRawOccRatioErr() );

  SetDeltaDistInScint( rhs.GetDeltaDistInScint() );
  SetDeltaDistInAV( rhs.GetDeltaDistInAV() );
  SetDeltaDistInWater( rhs.GetDeltaDistInWater() );

  SetSolidAngleRatio( rhs.GetSolidAngleRatio() );
  SetFresnelTCoeffRatio( rhs.GetFresnelTCoeffRatio() );

  SetLBTheta( rhs.GetLBTheta() );
  SetCentralLBTheta( rhs.GetCentralLBTheta() );

  SetLBPhi( rhs.GetLBPhi() );
  SetCentralLBPhi( rhs.GetCentralLBPhi() );

  SetIncidentAngle( rhs.GetIncidentAngle() );
  SetCentralIncidentAngle( rhs.GetCentralIncidentAngle() );

  SetLBIntensityNormRatio( rhs.GetLBIntensityNormRatio() );

  SetModelCorrMPEOccRatio( rhs.GetModelCorrMPEOccRatio() );
  SetModelCorrRawOccRatio( rhs.GetModelCorrRawOccRatio() );

  return *this;

}
