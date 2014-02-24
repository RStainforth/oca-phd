////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataPoint.hh
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

#ifndef _LOCASDataPoint_
#define _LOCASDataPoint_

#include "LOCASPMT.hh"
#include "LOCASRawDataPoint.hh"
#include <string>

namespace LOCAS{

  class LOCASDataPoint : public TObject
  {
  public:
    LOCASDataPoint(){ };
    LOCASDataPoint( const LOCASRawDataPoint dataPoint );
    ~LOCASDataPoint(){ };
    
    LOCASDataPoint& operator=( const LOCASDataPoint& rhs );
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Float_t GetMPEOccRatio() const { return fMPEOccRatio; }
    Float_t GetRawOccRatio() const { return fRawOccRatio; }

    Float_t GetMPEOccRatioErr() const { return fMPEOccRatioErr; }
    Float_t GetRawOccRatioErr() const { return fRawOccRatioErr; }
    
    Float_t GetDeltaDistInScint() const { return fDeltaDistInScint; }
    Float_t GetDeltaDistInAV() const { return fDeltaDistInAV; }
    Float_t GetDeltaDistInWater() const { return fDeltaDistInWater; }

    Float_t GetSolidAngleRatio() const { return fSolidAngleRatio; }
    Float_t GetFresnelTCoeffRatio() const { return fFresnelTCoeffRatio; }

    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    Float_t GetIncidentAngle() const { return fIncidentAngle; }
    Float_t GetCentralIncidentAngle() const { return fCentralIncidentAngle; }

    Float_t GetLBPhi() const { return fLBPhi; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetLBIntensityNormRatio() const { return fLBIntensityNormRatio; }

    Float_t GetModelCorrMPEOccRatio() const { return fModelCorrMPEOccRatio; }
    Float_t GetModelCorrRawOccRatio() const { return fModelCorrRawOccRatio; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetMPEOccRatio( const Float_t val ) { fMPEOccRatio = val; }
    void SetRawOccRatio( const Float_t val ) { fRawOccRatio = val; }

    void SetMPEOccRatioErr( const Float_t val ) { fMPEOccRatioErr = val; }
    void SetRawOccRatioErr( const Float_t val ) { fRawOccRatioErr = val; }
    
    void SetDeltaDistInScint( Float_t val ) { fDeltaDistInScint = val; }
    void SetDeltaDistInAV( Float_t val ) { fDeltaDistInAV = val; }
    void SetDeltaDistInWater( Float_t val ) { fDeltaDistInWater = val; }

    void SetSolidAngleRatio( Float_t val ) { fSolidAngleRatio = val; }
    void SetFresnelTCoeffRatio( Float_t val ) { fFresnelTCoeffRatio = val; }

    void SetLBTheta( Float_t val ) { fLBTheta = val; }
    void SetCentralLBTheta( Float_t val ) { fCentralLBTheta = val; }

    void SetIncidentAngle( const Float_t val ){ fIncidentAngle = val; }
    void SetCentralIncidentAngle( const Float_t val ){ fCentralIncidentAngle = val; }

    void SetLBPhi( const Float_t val ) { fLBPhi = val; }
    void SetCentralLBPhi( const Float_t val ) { fCentralLBPhi = val; }

    void SetLBIntensityNormRatio( const Float_t val ) { fLBIntensityNormRatio = val; }

    void SetModelCorrMPEOccRatio( const Float_t val ){ fModelCorrMPEOccRatio = val; }
    void SetModelCorrRawOccRatio( const Float_t val ){ fModelCorrRawOccRatio = val; }

  private:

    Float_t fMPEOccRatio;
    Float_t fRawOccRatio;

    Float_t fMPEOccRatioErr;
    Float_t fRawOccRatioErr;

    Float_t fDeltaDistInScint;
    Float_t fDeltaDistInAV;
    Float_t fDeltaDistInWater;

    Float_t fSolidAngleRatio;
    Float_t fFresnelTCoeffRatio;

    Float_t fLBTheta;
    Float_t fCentralLBTheta;

    Float_t fIncidentAngle;
    Float_t fCentralIncidentAngle;

    Float_t fLBPhi;
    Float_t fCentralLBPhi;

    Float_t fLBIntensityNormRatio;

    Float_t fModelCorrMPEOccRatio;
    Float_t fModelCorrRawOccRatio;

    ClassDef( LOCASDataPoint, 1 );
    
  };
  
}

#endif
