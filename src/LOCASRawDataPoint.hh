////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRawDataPoint.hh
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


#ifndef _LOCASRawDataPoint_
#define _LOCASRawDataPoint_

#include "LOCASPMT.hh"
#include <string>

namespace LOCAS{

  class LOCASRawDataPoint : public TObject
  {
  public: 

    LOCASRawDataPoint(){ }
    LOCASRawDataPoint( const LOCASPMT* pmt );
    ~LOCASRawDataPoint(){ }

    LOCASRawDataPoint& operator=( const LOCASRawDataPoint& rhs );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Float_t GetOccRatio() const { return fOccRatio; }

    Float_t GetOccRatioErr() const { return fOccRatioErr; }

    Float_t GetDistInScint() const { return fDistInScint; }
    Float_t GetCentralDistInScint() const { return fCentralDistInScint; }

    Float_t GetDistInAV() const { return fDistInAV; }
    Float_t GetCentralDistInAV() const { return fCentralDistInAV; }

    Float_t GetDistInWater() const { return fDistInWater; }
    Float_t GetCentralDistInWater() const { return fCentralDistInWater; }

    Float_t GetSolidAngle() const { return fSolidAngle; }
    Float_t GetCentralSolidAngle() const { return fCentralSolidAngle; }

    Float_t GetFresnelTCoeff() const { return fFresnelTCoeff; }
    Float_t GetCentralFresnelTCoeff() const { return fCentralFresnelTCoeff; }

    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }

    Float_t GetRawOccupancy() const { return fRawOccupancy; }
    Float_t GetCentralRawOccupancy() const { return fCentralRawOccupancy; }

    Float_t GetMPECorrOccupancy() const { return fMPECorrOccupancy; }
    Float_t GetCentralMPECorrOccupancy() const { return fCentralMPECorrOccupancy; }

    Float_t GetIncidentAngle() const { return fIncidentAngle; }
    Float_t GetCentralIncidentAngle() const { return fCentralIncidentAngle; }

    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    Float_t GetLBPhi() const { return fLBPhi; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetAVHDShadowingVal() const { return fAVHDShadowingVal; }
    Float_t GetCentralAVHDShadowingVal() const { return fCentralAVHDShadowingVal; }

    Float_t GetGeometricShadowingVal() const { return fGeometricShadowingVal; }
    Float_t GetCentralGeometricShadowingVal() const { return fCentralGeometricShadowingVal; }

    Int_t GetCHSFlag() const { return fCHSFlag; }
    Int_t GetCentralCHSFlag() const { return fCSSFlag; }

    Int_t GetCSSFlag() const { return fCSSFlag; }
    Int_t GetCentralCSSFlag() const { return fCentralCSSFlag; }

    Float_t GetTimeOfFlight() const { return fTimeOfFlight; }
    Float_t GetCentralTimeOfFlight() const { return fCentralTimeOfFlight; }

    Int_t GetBadPathFlag() const { return fBadPathFlag; }
    Int_t GetCentralBadPathFlag() const { return fCentralbadPathFlag; }

    Int_t GetNeckFlag() const { return fNeckFlag; }
    Int_t GetCentralNeckFlag() const { return fCentralNeckFlag; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetOccRatio( const Float_t val ) { fOccRatio = val; }

    void SetOccRatioErr( const Float_t val ) { fOccRatioErr = val; }

    void SetDistInScint( const Float_t val ) { fDistInScint = val; }
    void SetCentralDistInScint( const Float_t val ) { fCentralDistInScint = val; }

    void SetDistInAV( const Float_t val ) { fDistInAV = val; }
    void SetCentralDistInAV( const Float_t val ) { fCentralDistInAV = val; }

    void SetDistInWater( const Float_t val ) { fDistInWater = val; }
    void SetCentralDistInWater( const Float_t val ) { fCentralDistInWater = val; }

    void SetSolidAngle( const Float_t val ) { fSolidAngle = val; }
    void SetCentralSolidAngle( const Float_t val ) { fCentralSolidAngle = val; }

    void SetFresnelTCoeff( const Float_t val ) { fFresnelTCoeff = val; }
    void SetCentralFresnelTCoeff( const Float_t val ) { fCentralFresnelTCoeff = val; }

    void SetLBIntensityNorm( const Float_t val ) { fLBIntensityNorm = val; }
    void SetCentralLBIntensityNorm( const Float_t val ) { fCentralLBIntensityNorm = val; }

    void SetRawOccupancy( const Float_t val ) { fRawOccupancy = val; }
    void SetCentralRawOccupancy( const Float_t val ) { fCentralRawOccupancy = val; }

    void SetMPECorrOccupancy( const Float_t val ) { fMPECorrOccupancy = val; }
    void SetCentralMPECorrOccupancy( const Float_t val ) { fCentralMPECorrOccupancy = val; }

    void SetIncidentAngle( const Float_t val ){ fIncidentAngle = val; }
    void SetCentralIncidentAngle( const Float_t val ){ fCentralIncidentAngle = val; }

    void SetLBTheta( const Float_t val ) { fLBTheta = val; }
    void SetCentralLBTheta( const Float_t val ) { fCentralLBTheta = val; }

    void SetLBPhi( const Float_t val ) { fLBPhi = val; }
    void SetCentralLBPhi( const Float_t val ) { fCentralLBPhi = val; }

    void SetAVHDShadowingVal( const Float_t val ) { fAVHDShadowingVal = val; }
    void SetCentralAVHDShadowingVal( const Float_t val ) { fCentralAVHDShadowingVal = val; }

    void SetGeometricShadowingVal( const Float_t val ) { fGeometricShadowingVal = val; }
    void SetCentralGeometricShadowingVal( const Float_t val ) { fCentralGeometricShadowingVal = val; }

    void SetCHSFlag( const Int_t val ) { fCHSFlag = val; }
    void SetCentralCHSFlag( const Int_t val ) { fCentralCHSFlag = val; }

    void SetCSSFlag( const Int_t val ) { fCSSFlag = val; }
    void SetCentralCSSFlag( const Int_t val ) { fCentralCSSFlag = val; }

    void SetTimeOfFlight( const Float_t val ) { fTimeOfFlight = val; }
    void SetCentralTimeOfFlight( const Float_t val ) { fCentralTimeOfFlight = val; }

    void SetBadPathFlag( const Int_t val ) { fBadPathFlag = val; }
    void SetCentralBadPathFlag( const Int_t val ) { fCentralbadPathFlag = val; }

    void SetNeckFlag( const Int_t val ) { fNeckFlag = val; }
    void SetCentralNeckFlag( const Int_t val ) { fCentralNeckFlag = val; }

  private:

    Float_t fOccRatio;

    Float_t fOccRatioErr;

    Float_t fDistInScint;
    Float_t fCentralDistInScint;

    Float_t fDistInAV;
    Float_t fCentralDistInAV;

    Float_t fDistInWater;
    Float_t fCentralDistInWater;

    Float_t fSolidAngle;
    Float_t fCentralSolidAngle;

    Float_t fFresnelTCoeff;
    Float_t fCentralFresnelTCoeff;

    Float_t fLBIntensityNorm;
    Float_t fCentralLBIntensityNorm;

    Float_t fRawOccupancy;
    Float_t fCentralRawOccupancy;

    Float_t fMPECorrOccupancy;
    Float_t fCentralMPECorrOccupancy;

    Float_t fIncidentAngle;
    Float_t fCentralIncidentAngle;

    Float_t fLBTheta;
    Float_t fCentralLBTheta;

    Float_t fLBPhi;
    Float_t fCentralLBPhi;

    Float_t fAVHDShadowingVal;
    Float_t fCentralAVHDShadowingVal;

    Float_t fGeometricShadowingVal;
    Float_t fCentralGeometricShadowingVal;

    Int_t fCHSFlag;
    Int_t fCentralCHSFlag;

    Int_t fCSSFlag;
    Int_t fCentralCSSFlag;

    Float_t fTimeOfFlight;
    Float_t fCentralTimeOfFlight;

    Int_t fBadPathFlag;
    Int_t fCentralbadPathFlag;

    Int_t fNeckFlag;
    Int_t fCentralNeckFlag;

    ClassDef( LOCASRawDataPoint, 1 );

  };

}

#endif
