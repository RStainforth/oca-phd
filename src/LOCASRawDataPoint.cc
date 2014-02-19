////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRawDataPoint.cc
///
/// CLASS: LOCAS::LOCASRawDataPoint
///
/// BRIEF: Raw data-level structure for data
///        points. These raw data-points are fed
///        into the LOCASDataStore alongside a
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

#include "LOCASPMT.hh"
#include "LOCASRawDataPoint.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASRawDataPoint )

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataPoint::LOCASRawDataPoint( const LOCASPMT* pmtPtr )
{

  SetOccRatio( pmtPtr->GetMPECorrOccupancy() / pmtPtr->GetCentralMPECorrOccupancy() );

  SetDistInScint( pmtPtr->GetDistInScint() );
  SetCentralDistInScint( pmtPtr->GetCentralDistInScint() );

  SetDistInAV( pmtPtr->GetDistInAV() );
  SetCentralDistInAV( pmtPtr->GetCentralDistInAV() );

  SetDistInWater( pmtPtr->GetDistInWater() );
  SetCentralDistInWater( pmtPtr->GetCentralDistInWater() );

  SetLBIntensityNorm( pmtPtr->GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( pmtPtr->GetCentralLBIntensityNorm() );

  SetRawOccupancy( pmtPtr->GetOccupancy() );
  SetCentralRawOccupancy( pmtPtr->GetCentralOccupancy() );

  SetMPECorrOccupancy( pmtPtr->GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( pmtPtr->GetCentralMPECorrOccupancy() );

  SetLBTheta( pmtPtr->GetRelLBTheta() );
  SetCentralLBTheta( pmtPtr->GetCentralRelLBTheta() );

  SetLBPhi( pmtPtr->GetRelLBPhi() );
  SetCentralLBPhi( pmtPtr->GetCentralRelLBPhi() );

  SetAVHDShadowingVal( pmtPtr->GetAVHDShadowVal() );
  SetCentralAVHDShadowingVal( pmtPtr->GetCentralAVHDShadowVal() );

  SetGeometricShadowingVal( pmtPtr->GetGeometricShadowVal() );
  SetCentralGeometricShadowingVal( pmtPtr->GetCentralGeometricShadowVal() );

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
  
}


//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataPoint& LOCASRawDataPoint::operator=( const LOCASRawDataPoint& rhs )
{

  SetOccRatio( rhs.GetMPECorrOccupancy() / rhs.GetCentralMPECorrOccupancy() );

  SetDistInScint( rhs.GetDistInScint() );
  SetCentralDistInScint( rhs.GetCentralDistInScint() );

  SetDistInAV( rhs.GetDistInAV() );
  SetCentralDistInAV( rhs.GetCentralDistInAV() );

  SetDistInWater( rhs.GetDistInWater() );
  SetCentralDistInWater( rhs.GetCentralDistInWater() );

  SetLBIntensityNorm( rhs.GetLBIntensityNorm() );
  SetCentralLBIntensityNorm( rhs.GetCentralLBIntensityNorm() );

  SetRawOccupancy( rhs.GetRawOccupancy() );
  SetCentralRawOccupancy( rhs.GetCentralRawOccupancy() );

  SetMPECorrOccupancy( rhs.GetMPECorrOccupancy() );
  SetCentralMPECorrOccupancy( rhs.GetCentralMPECorrOccupancy() );

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

  return *this;

}
