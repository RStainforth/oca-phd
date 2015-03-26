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
///     03/2015 : RPFS - Restructred to construct using LOCASPMT objects
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataPoint_
#define _LOCASDataPoint_

#include "LOCASPMT.hh"

#include "TVector3.h"

#include <string>

namespace LOCAS{

  class LOCASDataPoint : public TObject
  {
  public:

    // Constructors
    LOCASDataPoint(){ };
    LOCASDataPoint( const LOCASPMT& lPMT );

    // Destructor - nothing to delete
    ~LOCASDataPoint(){ };
    
    // Equality operator
    LOCASDataPoint& operator=( const LOCASDataPoint& rhs );
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetRunID() const { return fRunID; }

    Float_t GetMPECorrOccupancy() const { return fMPECorrOccupancy; }
    Float_t GetCentralMPECorrOccupancy() const { return fCentralMPECorrOccupancy; }

    Float_t GetMPECorrOccupancyErr() const { return fMPECorrOccupancyErr; }
    Float_t GetCentralMPECorrOccupancyErr() const { return fCentralMPECorrOccupancyErr; }
    
    Float_t GetDistInInnerAV() const { return fDistInInnerAV; }
    Float_t GetCentralDistInInnerAV() const { return fCentralDistInInnerAV; }
    Float_t GetDistInAV() const { return fDistInAV; }
    Float_t GetCentralDistInAV() const { return fCentralDistInAV; }
    Float_t GetDistInWater() const { return fDistInWater; }
    Float_t GetCentralDistInWater() const { return fCentralDistInWater; }

    Float_t GetSolidAngle() const { return fSolidAngle; }
    Float_t GetCentralSolidAngle() const { return fCentralSolidAngle; }

    Float_t GetFresnelTCoeff() const { return fFresnelTCoeff; }
    Float_t GetCentralFresnelTCoeff() const { return fCentralFresnelTCoeff; }

    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    Float_t GetLBPhi() const { return fLBPhi; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetIncidentAngle() const { return fIncidentAngle; }
    Float_t GetCentralIncidentAngle() const { return fCentralIncidentAngle; }

    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }

    Int_t GetCSSFlag() const { return fCSSFlag; }
    Int_t GetCentralCSSFlag() const { return fCentralCSSFlag; }

    Int_t GetCHSFlag() const { return fCHSFlag; }
    Int_t GetCentralCHSFlag() const { return fCentralCHSFlag; }

    Bool_t GetBadPathFlag() const { return fBadPathFlag; }
    Bool_t GetCentralBadPathFlag() const { return fCentralBadPathFlag; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetRunID( const Int_t val ){ fRunID = val; }

    void SetMPECorrOccupancy( const Float_t val ){ fMPECorrOccupancy = val; }
    void SetCentralMPECorrOccupancy( const Float_t val ){ fCentralMPECorrOccupancy = val; }

    void SetMPECorrOccupancyErr( const Float_t val ){ fMPECorrOccupancyErr = val; }
    void SetCentralMPECorrOccupancyErr( const Float_t val ){ fCentralMPECorrOccupancyErr = val; }
    
    void SetDistInInnerAV( const Float_t val ){ fDistInInnerAV = val; }
    void SetCentralDistInInnerAV( const Float_t val ){ fCentralDistInInnerAV = val; }
    void SetDistInAV( const Float_t val ){ fDistInAV = val; }
    void SetCentralDistInAV( const Float_t val ){ fCentralDistInAV = val; }
    void SetDistInWater( const Float_t val ){ fDistInWater = val; }
    void SetCentralDistInWater( const Float_t val ){ fCentralDistInWater = val; }

    void SetSolidAngle( const Float_t val ){ fSolidAngle = val; }
    void SetCentralSolidAngle( const Float_t val ){ fCentralSolidAngle = val; }

    void SetFresnelTCoeff( const Float_t val ){ fFresnelTCoeff = val; }
    void SetCentralFresnelTCoeff( const Float_t val ){ fCentralFresnelTCoeff = val; }

    void SetLBTheta( const Float_t val ){ fLBTheta = val; }
    void SetCentralLBTheta( const Float_t val ){ fCentralLBTheta = val; }

    void SetLBPhi( const Float_t val ){ fLBPhi = val; }
    void SetCentralLBPhi( const Float_t val ){ fCentralLBPhi = val; }

    void SetIncidentAngle( const Float_t val ){ fIncidentAngle = val; }
    void SetCentralIncidentAngle( const Float_t val ){ fCentralIncidentAngle = val; }

    void SetLBIntensityNorm( const Float_t val ){ fLBIntensityNorm = val; }
    void SetCentralLBIntensityNorm( const Float_t val ){ fCentralLBIntensityNorm = val; }

    void SetCSSFlag( const Int_t val ){ fCSSFlag = val; }
    void SetCentralCSSFlag( const Int_t val ){ fCentralCSSFlag = val; }

    void SetCHSFlag( const Int_t val ){ fCHSFlag = val; }
    void SetCentralCHSFlag( const Int_t val ){ fCentralCHSFlag = val; }

    void SetBadPathFlag( const Bool_t val ){ fBadPathFlag = val; }
    void SetCentralBadPathFlag( const Bool_t val ){ fCentralBadPathFlag = val; }

  private:

    Int_t fRunID;                           // The run ID this datapoint originated from

    Float_t fMPECorrOccupancy;              // The MPE corrected occupancy of the off-axis run
    Float_t fCentralMPECorrOccupancy;       // The MPE corrected occupancy from the central run

    Float_t fMPECorrOccupancyErr;           // The error on the MPE corrected occupancy of the off-axis run
    Float_t fCentralMPECorrOccupancyErr;    // The error on the MPE corrected occupancy from the central run

    Float_t fDistInInnerAV;                 // The distance in the inner AV region from the off-axis run
    Float_t fCentralDistInInnerAV;          // The distance in the inner AV region from the central run
    Float_t fDistInAV;                      // The distance in the AV region from the off-axis run
    Float_t fCentralDistInAV;               // The distance in the AV region from the central run
    Float_t fDistInWater;                   // The distance in the water region from the off-axis run
    Float_t fCentralDistInWater;            // The distance in the water region from the central run

    Float_t fSolidAngle;                    // The solid angle subtended from the source positions from the off-axis run
    Float_t fCentralSolidAngle;             // The solid angle subtended from the source positions from the central run

    Float_t fFresnelTCoeff;                 // The fresnel transmission coefficient from the off-axis run
    Float_t fCentralFresnelTCoeff;          // The fresnel transmission coefficient from the central run

    Float_t fLBTheta;                       // The theta value of the light path leaving the source in the source coordinate system
    Float_t fCentralLBTheta;                // The theta value of the light path leaving the source in the source coordinate system from the central run

    Float_t fLBPhi;                         // The phi value of the light path leaving the source in the source coordinate system
    Float_t fCentralLBPhi;                  // The phi value of the light path leaving the source in the source coordinate system from the central run

    Float_t fIncidentAngle;                 // The incident angle of the light path on the PMT in degrees
    Float_t fCentralIncidentAngle;          // The incident angle of the light path on the PMT from the central run in degrees

    Float_t fLBIntensityNorm;               // The run intensity normalisation value for the off-axis run (calculated by LOCASRun::CalculateLBIntensityNorm)
    Float_t fCentralLBIntensityNorm;        // The run intensity normalisation value for the central run (calculated by LOCASRun::CalculateLBIntensityNorm)

    Int_t fCSSFlag;                         // The CSS flag (ANXX) for the off-axis run
    Int_t fCentralCSSFlag;                  // The CSS flag (ANXX) for the central run
    
    Int_t fCHSFlag;                         // The CHS flag (DQXX) for the off-axis run
    Int_t fCentralCHSFlag;                  // The CHS flag (DQXX) for the central run

    Bool_t fBadPathFlag;                    // The path status from the off-axis run (i.e. shadowed, subject to reflections etc. true: bad, false: good)
    Bool_t fCentralBadPathFlag;             // The path status from the central run (i.e. shadowed, subject to reflections etc. true: bad, false: good)

    ClassDef( LOCASDataPoint, 1 );
    
  };
  
}

#endif
