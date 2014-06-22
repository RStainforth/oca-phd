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

#include "TVector3.h"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataPoint )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint::LOCASDataPoint( const LOCASRawDataPoint dataPoint )
{

  SetMPEOccRatio( ( dataPoint.GetModelCorrOccRatio() ) );
  SetMPEOccRatioErr( ( dataPoint.GetMPEOccRatioErr() ) );

  SetMPECorrOccupancy( ( dataPoint.GetMPECorrOccupancy() ) );
  SetCentralMPECorrOccupancy( ( dataPoint.GetCentralMPECorrOccupancy() ) );

  SetDeltaDistInScint( ( dataPoint.GetDistInScint() - dataPoint.GetCentralDistInScint() ) );
  SetDeltaDistInAV( ( dataPoint.GetDistInAV() - dataPoint.GetCentralDistInAV() ) );
  SetDeltaDistInWater( ( dataPoint.GetDistInWater() - dataPoint.GetCentralDistInWater() ) );
  SetTotalDist( dataPoint.GetTotalDist() );
  SetCentralTotalDist( dataPoint.GetCentralTotalDist() );

  SetSolidAngleRatio( ( dataPoint.GetSolidAngle() / dataPoint.GetCentralSolidAngle() ) );
  SetFresnelTCoeffRatio( ( dataPoint.GetFresnelTCoeff() / dataPoint.GetCentralFresnelTCoeff() ) );

  SetIncidentAngle( dataPoint.GetIncidentAngle() );
  SetCentralIncidentAngle( dataPoint.GetCentralIncidentAngle() );

  SetLBTheta( dataPoint.GetLBTheta() );
  SetCentralLBTheta( dataPoint.GetCentralLBTheta() );

  SetLBPhi( dataPoint.GetLBPhi() );
  SetCentralLBPhi( dataPoint.GetCentralLBPhi() );

  SetLBIntensityNormRatio( ( dataPoint.GetLBIntensityNorm() / dataPoint.GetCentralLBIntensityNorm() ) );

  SetPMTPos( dataPoint.GetPMTPos() );
  SetLBPos( dataPoint.GetLBPos() );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASDataPoint& LOCASDataPoint::operator=( const LOCASDataPoint& rhs )
{

  SetMPEOccRatio( rhs.GetMPEOccRatio() );
  SetMPEOccRatioErr( rhs.GetMPEOccRatioErr() );

  SetMPECorrOccupancy( rhs.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( rhs.GetCentralMPECorrOccupancy() );

  SetDeltaDistInScint( rhs.GetDeltaDistInScint() );
  SetDeltaDistInAV( rhs.GetDeltaDistInAV() );
  SetDeltaDistInWater( rhs.GetDeltaDistInWater() );
  SetTotalDist( rhs.GetTotalDist() );
  SetCentralTotalDist( rhs.GetCentralTotalDist() );

  SetSolidAngleRatio( rhs.GetSolidAngleRatio() );
  SetFresnelTCoeffRatio( rhs.GetFresnelTCoeffRatio() );

  SetLBTheta( rhs.GetLBTheta() );
  SetCentralLBTheta( rhs.GetCentralLBTheta() );

  SetLBPhi( rhs.GetLBPhi() );
  SetCentralLBPhi( rhs.GetCentralLBPhi() );

  SetIncidentAngle( rhs.GetIncidentAngle() );
  SetCentralIncidentAngle( rhs.GetCentralIncidentAngle() );

  SetLBIntensityNormRatio( rhs.GetLBIntensityNormRatio() );

  SetModelOccRatio( rhs.GetModelOccRatio() );

  SetChiSq( rhs.GetChiSq() );

  SetPMTPos( rhs.GetPMTPos() );
  SetLBPos( rhs.GetLBPos() );

  return *this;

}
