////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataPoint.hh
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

#ifndef _LOCASDataPoint_
#define _LOCASDataPoint_

#include "LOCASPMT.hh"
#include "LOCASRun.hh"
#include "LOCASRawDataPoint.hh"

#include "TVector3.h"

#include <string>

namespace LOCAS{

  class LOCASDataPoint : public TObject
  {
  public:

    // Constructors
    LOCASDataPoint(){ };
    LOCASDataPoint( const LOCASRawDataPoint dataPoint );

    // Destructor - nothing to delete
    ~LOCASDataPoint(){ };
    
    // Equality operator
    LOCASDataPoint& operator=( const LOCASDataPoint& rhs );
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Float_t GetMPEOccRatio() const { return fMPEOccRatio; }
    Float_t GetMPEOccRatioErr() const { return fMPEOccRatioErr; }

    Float_t GetMPECorrOccupancy() const { return fMPECorrOccupancy; }
    Float_t GetCentralMPECorrOccupancy() const { return fCentralMPECorrOccupancy; }
    
    Float_t GetDeltaDistInInnerAV() const { return fDeltaDistInInnerAV; }
    Float_t GetDeltaDistInAV() const { return fDeltaDistInAV; }
    Float_t GetDeltaDistInWater() const { return fDeltaDistInWater; }
    Float_t GetTotalDist() const { return fTotalDist; }
    Float_t GetCentralTotalDist() const { return fCentralTotalDist; }

    Float_t GetSolidAngleRatio() const { return fSolidAngleRatio; }
    Float_t GetFresnelTCoeffRatio() const { return fFresnelTCoeffRatio; }

    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    Float_t GetLBPhi() const { return fLBPhi; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetIncidentAngle() const { return fIncidentAngle; }
    Float_t GetCentralIncidentAngle() const { return fCentralIncidentAngle; }

    Float_t GetLBIntensityNormRatio() const { return fLBIntensityNormRatio; }

    Float_t GetModelOccRatio() const { return fModelOccRatio; }

    Float_t GetChiSq() const { return fChiSq; }

    TVector3 GetPMTPos() const { return fPMTPos; }
    TVector3 GetLBPos() const { return fLBPos; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetMPEOccRatio( const Float_t val ) { fMPEOccRatio = val; }
    void SetMPEOccRatioErr( const Float_t val ) { fMPEOccRatioErr = val; }

    void SetMPECorrOccupancy( const Float_t val ){ fMPECorrOccupancy = val; }
    void SetCentralMPECorrOccupancy( const Float_t val ){ fCentralMPECorrOccupancy = val; }
    
    void SetDeltaDistInInnerAV( Float_t val ) { fDeltaDistInInnerAV = val; }
    void SetDeltaDistInAV( Float_t val ) { fDeltaDistInAV = val; }
    void SetDeltaDistInWater( Float_t val ) { fDeltaDistInWater = val; }
    void SetTotalDist( const Float_t val ){ fTotalDist = val; }
    void SetCentralTotalDist( const Float_t val ){ fCentralTotalDist = val; }

    void SetSolidAngleRatio( Float_t val ) { fSolidAngleRatio = val; }
    void SetFresnelTCoeffRatio( Float_t val ) { fFresnelTCoeffRatio = val; }

    void SetLBTheta( Float_t val ) { fLBTheta = val; }
    void SetCentralLBTheta( Float_t val ) { fCentralLBTheta = val; }

    void SetLBPhi( const Float_t val ) { fLBPhi = val; }
    void SetCentralLBPhi( const Float_t val ) { fCentralLBPhi = val; }

    void SetIncidentAngle( const Float_t val ){ fIncidentAngle = val; }
    void SetCentralIncidentAngle( const Float_t val ){ fCentralIncidentAngle = val; }

    void SetLBIntensityNormRatio( const Float_t val ) { fLBIntensityNormRatio = val; }

    void SetModelOccRatio( const Float_t val ){ fModelOccRatio = val; }

    void SetChiSq( const Float_t val ){ fChiSq = val; }
    
    void SetPMTPos( const TVector3& pmtPos ){ fPMTPos = pmtPos; }
    void SetLBPos( const TVector3& lbPos ){ fLBPos = lbPos; }

  private:

    Float_t fMPEOccRatio;         // The MPE corrected occupancy ratio
    Float_t fMPEOccRatioErr;      // The error in the MPE corrected occupancy ratio

    Float_t fMPECorrOccupancy;          // The MPE corrected occupancy of the off-axis run
    Float_t fCentralMPECorrOccupancy;   // The MPE corrected occupancy from the central run

    Float_t fDeltaDistInInnerAV;    // The difference (off-axis - central) in the scintillator region distances
    Float_t fDeltaDistInAV;       // The difference (off-axis - central) in the av region distances
    Float_t fDeltaDistInWater;    // The difference (off-axis - central) in the water region distances
    Float_t fTotalDist;           // The total distance of the light path in the off-axis run
    Float_t fCentralTotalDist;    // The total distance of the light path in the central run

    Float_t fSolidAngleRatio;     // The solid angle ratio (off-axis / central)
    Float_t fFresnelTCoeffRatio;  // The fresnel transmission ratio (off-axis / central)

    Float_t fLBTheta;             // The theta value of the light path leaving the source in the source coordinate system
    Float_t fCentralLBTheta;      // The theta value of the light path leaving the source in the source coordinate system from the central run

    Float_t fLBPhi;               // The phi value of the light path leaving the source in the source coordinate system
    Float_t fCentralLBPhi;        // The phi value of the light path leaving the source in the source coordinate system from the central run

    Float_t fIncidentAngle;       // The incident angle of the light path on the PMT
    Float_t fCentralIncidentAngle;// The incident angle of the light path on the PMt from the central run

    Float_t fLBIntensityNormRatio;// The ratio of the prompt counts from all PMTs in the off-axis run / the same from the central run

    Float_t fModelOccRatio;       // The model prediction for the occupancy ratio

    Float_t fChiSq;               // The chisquare value of this data point

    TVector3 fPMTPos;             // The position of the PMT this datapoint represents   
    TVector3 fLBPos;              // The source position

    ClassDef( LOCASDataPoint, 1 );
    
  };
  
}

#endif
