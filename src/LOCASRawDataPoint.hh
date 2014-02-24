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


#ifndef _LOCASRawDataPoint_
#define _LOCASRawDataPoint_

#include "LOCASPMT.hh"

#include "TMath.h"

#include <string>

namespace LOCAS{

  class LOCASRawDataPoint : public TObject
  {
  public: 

    // Constructors
    LOCASRawDataPoint(){ }
    LOCASRawDataPoint( const LOCASPMT* pmt );

    // Destructor - nothing to delete
    ~LOCASRawDataPoint(){ }

    // Equality Operator
    LOCASRawDataPoint& operator=( const LOCASRawDataPoint& rhs );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Float_t GetMPEOccRatio() const { return fMPEOccRatio; }
    Float_t GetRawOccRatio() const { return fRawOccRatio; }

    Float_t GetMPEOccRatioErr() const { return fMPEOccRatioErr; }
    Float_t GetRawOccRatioErr() const { return fRawOccRatioErr; }

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

    void SetMPEOccRatio( const Float_t val ) { fMPEOccRatio = val; }
    void SetRawOccRatio( const Float_t val ) { fRawOccRatio = val; }

    void SetMPEOccRatioErr( const Float_t val ) { fMPEOccRatioErr = val; }
    void SetRawOccRatioErr( const Float_t val ) { fRawOccRatioErr = val; }

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

    Float_t fMPEOccRatio;                // The MPE corrected occupancy Ratio
    Float_t fRawOccRatio;                // The Raw occupancy ratio

    Float_t fMPEOccRatioErr;             // The error on the MPE corrected occupancy ratio
    Float_t fRawOccRatioErr;             // The error on the raw occupancy ratio

    Float_t fDistInScint;                // The distance through the scintillator region
    Float_t fCentralDistInScint;         // The distancen through the scintillator region from the central run

    Float_t fDistInAV;                   // The distance through the AV region
    Float_t fCentralDistInAV;            // The distance through the AV region from the central run

    Float_t fDistInWater;                // The distance through the water region
    Float_t fCentralDistInWater;         // The distance through the water region from the central run

    Float_t fSolidAngle;                 // The solid angle
    Float_t fCentralSolidAngle;          // The solid angle from the central run

    Float_t fFresnelTCoeff;              // The combined Fresnel Transmission Coefficient through the scintillator/AV and AV/water regions
    Float_t fCentralFresnelTCoeff;       // The combined Fresnel Transmission Coefficient through the scintillator/AV and AV/water regions from the central run

    Float_t fLBIntensityNorm;            // The number of prompt counts (occupancy) from all PMTs from the run this data point originates from
    Float_t fCentralLBIntensityNorm;     // The number of prompt counts (occupancy) from all PMTs from the run this data point originates from, from the central run
 
    Float_t fRawOccupancy;               // The raw occupancy
    Float_t fCentralRawOccupancy;        // The raw occupancy from the central run

    Float_t fMPECorrOccupancy;           // The MPE corrected occupancy
    Float_t fCentralMPECorrOccupancy;    // The MPE corrected occupancy from the central

    Float_t fIncidentAngle;              // The Incident angle the light path makes with the PMT
    Float_t fCentralIncidentAngle;       // The Incident angle the light path makes with the PMT from the central run

    Float_t fLBTheta;                    // The theta value the light path leaving the source makes in the source coordinate system
    Float_t fCentralLBTheta;             // The theta value the light path leaving the source makes in the source coordinate system from the central run

    Float_t fLBPhi;                      // The phi value the light path leaving the source makes in the source coordinate system
    Float_t fCentralLBPhi;               // The phi value the light path leaving the source makes in the source coordinate system from the central run

    Float_t fAVHDShadowingVal;           // The relative shadowing value for this data point due to the AVHD ropes
    Float_t fCentralAVHDShadowingVal;    // The relative shadowing value for this data point due to the AVHD ropes from the central run

    Float_t fGeometricShadowingVal;        // The relative shadowing value for this data point due to the enveloping AV geoemtry
    Float_t fCentralGeometricShadowingVal;  // The relative shadowing value for this data point due to the enveloping AV geoemtry from the central run

    Int_t fCHSFlag;                      // The CHS flag
    Int_t fCentralCHSFlag;               // The CHS flag from the central run

    Int_t fCSSFlag;                      // The CSS flag
    Int_t fCentralCSSFlag;               // The CSS flag from the central run

    Float_t fTimeOfFlight;               // Time of flight from the source to the PMT [ns]
    Float_t fCentralTimeOfFlight;        // Time of flight from the source to the PMT [ns] from the central run  

    Int_t fBadPathFlag;                  // Bad Path flag
    Int_t fCentralbadPathFlag;           // Bad Path flag from the central run

    Int_t fNeckFlag;                     // Neck path flag
    Int_t fCentralNeckFlag;              // Neck path flag from the central run

    ClassDef( LOCASRawDataPoint, 1 );

  };

}

#endif
