////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASPMT.hh
///
/// CLASS: LOCAS::LOCASPMT
///
/// BRIEF: PMT level data structure for LOCAS optics analysis
///        Used as part of the LOCASRun class
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This data structure contains all the information
///         for a specific PMT in a run, required as an input
///         in an optics fit/analysis using LOCAS.
///
///         Herein lies information specific to the PMT itself,
///         such as distances through various media from the source,
///         the PMT ID, solidangle etc.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASPMT_
#define _LOCASPMT_

#include <RAT/DS/SOCPMT.hh>
#include <TVector3.h>
#include <TObject.h>

#include "LOCASLightPath.hh"

namespace LOCAS{

  class LOCASPMT : public TObject
  {
  public:
    LOCASPMT(){ Initialise(); }
    LOCASPMT( Int_t pmtID ){ fID = pmtID; Initialise(); }
    ~LOCASPMT(){ }
    
    LOCASPMT( const LOCASPMT& rhs );
    LOCASPMT& operator=( const LOCASPMT& rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    void Initialise();
    void Clear( Option_t* option="");
    void AddSOCPMTData( RAT::DS::SOCPMT& socPMT );                    // Add information from a SOCPMT object to this LOCASPMT object
    void ProcessLightPath( LOCASLightPath& lPath );                   // Process a light path to calculate the distances, solidangle etc.
    void VerifyPMT();                                                 // Verify that the fields of the PMT are complete and have 'reasonable' values
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetID( Int_t pmtID ){ fID = pmtID; }
    void SetRunID( Int_t runID ){ fRunID = runID; }
    void SetCentralRunID( Int_t ctrRunID ){ fCentralRunID = ctrRunID; }
    void SetWavelengthRunID( Int_t wvRunID ){ fWavelengthRunID = wvRunID; }
    void SetType( Int_t pmtType ){ fType = pmtType; }


    void SetIsVerified( Bool_t verified ){ fIsVerified = verified; }
                                                                          
    void SetPos( TVector3 xyzPos ){ fPos = xyzPos; }                  
    void SetNorm( TVector3 uvwOri ){ fNorm = uvwOri; }                
    
    void SetPromptPeakTime( Float_t tPeak ){ fPromptPeakTime = tPeak; }
    void SetPromptPeakWidth( Float_t tWidth ){ fPromptPeakWidth = tWidth; }  
    void SetTimeOfFlight( Float_t tOF ){ fTimeOfFlight = tOF; }
    void SetOccupancy( Float_t occ ){ fOccupancy = occ; }
    void SetOccupancyErr( Float_t occErr ){ fOccupancyErr = occErr; }
    void SetOccupancyCorr( Float_t occCorr ){ fOccupancyCorr = occCorr; }
    void SetLBIntensityNorm( Float_t intensityNorm ){ fLBIntensityNorm = intensityNorm; }

    void SetNLBPulses( Float_t nPulses ){ fNLBPulses = nPulses; }

    void SetMPECorrOccupancy( Float_t mpeCorr ){ fMPECorrOccupancy = mpeCorr; }
    void SetMPECorrOccupancyErr( Float_t mpeCorrErr ){ fMPECorrOccupancyErr = mpeCorrErr; }
    void SetMPECorrOccupancyCorr( Float_t mpeCorrCorr ){ fMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetFresnelTCoeff( Float_t fresnelCoeff ){ fFresnelTCoeff = fresnelCoeff; }
    
    void SetDistInScint( Float_t distInScint ){ fDistInScint = distInScint; }
    void SetDistInAV( Float_t distInAV ){ fDistInAV = distInAV; }
    void SetDistInWater( Float_t distInWater ){ fDistInWater = distInWater; }
    void SetDistInNeck( Float_t distInNeck ){ fDistInNeck = distInNeck; }
    void SetTotalDist( Float_t distTotal ){ fTotalDist = distTotal; }
    
    void SetSolidAngle( Float_t solidA ){ fSolidAngle = solidA; }
    void SetCosTheta( Float_t cosTheta ){ fCosTheta = cosTheta; }
    
    void SetRelLBTheta( Float_t theta ){ fRelLBTheta = theta; }
    void SetRelLBPhi( Float_t phi ){ fRelLBPhi = phi; }
    
    void SetAVHDShadowVal( Float_t avhdShadow ){ fAVHDShadowVal = avhdShadow; }
    void SetGeometricShadowVal( Float_t geomShadow ){ fGeometricShadowVal = geomShadow; }
    
    void SetAVHDShadowFlag( Bool_t avhdFlag ){ fAVHDShadowFlag = avhdFlag; }
    void SetGeometricShadowFlag( Bool_t geomFlag ){ fGeometricShadowFlag = geomFlag; }
    
    void SetCHSFlag( Bool_t chsFlag ){ fCHSFlag = chsFlag; }
    void SetCSSFlag( Bool_t cssFlag ){ fCSSFlag = cssFlag; }
    void SetBadPath( Bool_t badPath ){ fBadPath = badPath; }
    void SetNeckFlag( Bool_t neckFlag ){ fNeckFlag = neckFlag; }

    /////////////////////////////////////////
    ////////     CENTRAL SETTERS     ////////
    /////////////////////////////////////////

    void SetCentralPromptPeakTime( Float_t tPeak ){ fCentralPromptPeakTime = tPeak; }
    void SetCentralPromptPeakWidth( Float_t tWidth ){ fCentralPromptPeakWidth = tWidth; }  
    void SetCentralTimeOfFlight( Float_t tOF ){ fCentralTimeOfFlight = tOF; }
    void SetCentralOccupancy( Float_t occ ){ fCentralOccupancy = occ; }
    void SetCentralOccupancyErr( Float_t occErr ){ fCentralOccupancyErr = occErr; }
    void SetCentralOccupancyCorr( Float_t occCorr ){ fCentralOccupancyCorr = occCorr; }
    void SetCentralLBIntensityNorm( Float_t intensityNorm ){ fCentralLBIntensityNorm = intensityNorm; }

    void SetCentralNLBPulses( Float_t nPulses ){ fCentralNLBPulses = nPulses; }

    void SetCentralMPECorrOccupancy( Float_t mpeCorr ){ fCentralMPECorrOccupancy = mpeCorr; }
    void SetCentralMPECorrOccupancyErr( Float_t mpeCorrErr ){ fCentralMPECorrOccupancyErr = mpeCorrErr; }
    void SetCentralMPECorrOccupancyCorr( Float_t mpeCorrCorr ){ fCentralMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetCentralFresnelTCoeff( Float_t fresnelCoeff ){ fCentralFresnelTCoeff = fresnelCoeff; }
    
    void SetCentralDistInScint( Float_t distInScint ){ fCentralDistInScint = distInScint; }
    void SetCentralDistInAV( Float_t distInAV ){ fCentralDistInAV = distInAV; }
    void SetCentralDistInWater( Float_t distInWater ){ fCentralDistInWater = distInWater; }
    void SetCentralDistInNeck( Float_t distInNeck ){ fCentralDistInNeck = distInNeck; }
    void SetCentralTotalDist( Float_t distTotal ){ fCentralTotalDist = distTotal; }
    
    void SetCentralSolidAngle( Float_t solidA ){ fCentralSolidAngle = solidA; }
    void SetCentralCosTheta( Float_t cosTheta ){ fCentralCosTheta = cosTheta; }
    
    void SetCentralRelLBTheta( Float_t theta ){ fCentralRelLBTheta = theta; }
    void SetCentralRelLBPhi( Float_t phi ){ fCentralRelLBPhi = phi; }
    
    void SetCentralAVHDShadowVal( Float_t avhdShadow ){ fCentralAVHDShadowVal = avhdShadow; }
    void SetCentralGeometricShadowVal( Float_t geomShadow ){ fCentralGeometricShadowVal = geomShadow; }
    
    void SetCentralAVHDShadowFlag( Bool_t avhdFlag ){ fCentralAVHDShadowFlag = avhdFlag; }
    void SetCentralGeometricShadowFlag( Bool_t geomFlag ){ fCentralGeometricShadowFlag = geomFlag; }
    
    void SetCentralCHSFlag( Bool_t chsFlag ){ fCentralCHSFlag = chsFlag; }
    void SetCentralCSSFlag( Bool_t cssFlag ){ fCentralCSSFlag = cssFlag; }
    void SetCentralBadPath( Bool_t badPath ){ fCentralBadPath = badPath; }
    void SetCentralNeckFlag( Bool_t neckFlag ){ fCentralNeckFlag = neckFlag; }

    ////////////////////////////////////////////
    ////////     WAVELENGTH SETTERS     ////////
    ////////////////////////////////////////////

    void SetWavelengthPromptPeakTime( Float_t tPeak ){ fWavelengthPromptPeakTime = tPeak; }
    void SetWavelengthPromptPeakWidth( Float_t tWidth ){ fWavelengthPromptPeakWidth = tWidth; }  
    void SetWavelengthTimeOfFlight( Float_t tOF ){ fWavelengthTimeOfFlight = tOF; }
    void SetWavelengthOccupancy( Float_t occ ){ fWavelengthOccupancy = occ; }
    void SetWavelengthOccupancyErr( Float_t occErr ){ fWavelengthOccupancyErr = occErr; }
    void SetWavelengthOccupancyCorr( Float_t occCorr ){ fWavelengthOccupancyCorr = occCorr; }
    void SetWavelengthLBIntensityNorm( Float_t intensityNorm ){ fWavelengthLBIntensityNorm = intensityNorm; }

    void SetWavelengthNLBPulses( Float_t nPulses ){ fWavelengthNLBPulses = nPulses; }

    void SetWavelengthMPECorrOccupancy( Float_t mpeCorr ){ fWavelengthMPECorrOccupancy = mpeCorr; }
    void SetWavelengthMPECorrOccupancyErr( Float_t mpeCorrErr ){ fWavelengthMPECorrOccupancyErr = mpeCorrErr; }
    void SetWavelengthMPECorrOccupancyCorr( Float_t mpeCorrCorr ){ fWavelengthMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetWavelengthFresnelTCoeff( Float_t fresnelCoeff ){ fWavelengthFresnelTCoeff = fresnelCoeff; }
    
    void SetWavelengthDistInScint( Float_t distInScint ){ fWavelengthDistInScint = distInScint; }
    void SetWavelengthDistInAV( Float_t distInAV ){ fWavelengthDistInAV = distInAV; }
    void SetWavelengthDistInWater( Float_t distInWater ){ fWavelengthDistInWater = distInWater; }
    void SetWavelengthDistInNeck( Float_t distInNeck ){ fWavelengthDistInNeck = distInNeck; }
    void SetWavelengthTotalDist( Float_t distTotal ){ fWavelengthTotalDist = distTotal; }
    
    void SetWavelengthSolidAngle( Float_t solidA ){ fWavelengthSolidAngle = solidA; }
    void SetWavelengthCosTheta( Float_t cosTheta ){ fWavelengthCosTheta = cosTheta; }
    
    void SetWavelengthRelLBTheta( Float_t theta ){ fWavelengthRelLBTheta = theta; }
    void SetWavelengthRelLBPhi( Float_t phi ){ fWavelengthRelLBPhi = phi; }
    
    void SetWavelengthAVHDShadowVal( Float_t avhdShadow ){ fWavelengthAVHDShadowVal = avhdShadow; }
    void SetWavelengthGeometricShadowVal( Float_t geomShadow ){ fWavelengthGeometricShadowVal = geomShadow; }
    
    void SetWavelengthAVHDShadowFlag( Bool_t avhdFlag ){ fWavelengthAVHDShadowFlag = avhdFlag; }
    void SetWavelengthGeometricShadowFlag( Bool_t geomFlag ){ fWavelengthGeometricShadowFlag = geomFlag; }
    
    void SetWavelengthCHSFlag( Bool_t chsFlag ){ fWavelengthCHSFlag = chsFlag; }
    void SetWavelengthCSSFlag( Bool_t cssFlag ){ fWavelengthCSSFlag = cssFlag; }
    void SetWavelengthBadPath( Bool_t badPath ){ fWavelengthBadPath = badPath; }
    void SetWavelengthNeckFlag( Bool_t neckFlag ){ fWavelengthNeckFlag = neckFlag; }
    
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Int_t GetID(){ return fID; }
    Int_t GetRunID(){ return fRunID; }
    Int_t GetCentralRunID(){ return fCentralRunID; }
    Int_t GetWavelengthRunID(){ return fWavelengthRunID; }
    Int_t GetType(){ return fType; }

    Bool_t GetIsVerified(){ return fIsVerified; }
    
    TVector3 GetPos(){ return fPos; }
    TVector3 GetNorm(){ return fNorm; }
    
    Float_t GetPromptPeakTime(){ return fPromptPeakTime; }
    Float_t GetPromptPeakWidth(){ return fPromptPeakWidth; }  
    Float_t GetTimeOfFlight(){ return fTimeOfFlight; }
    Float_t GetOccupancy(){ return fOccupancy; }
    Float_t GetOccupancyErr(){ return fOccupancyErr; }
    Float_t GetOccupancyCorr(){ return fOccupancyCorr; }
    Float_t GetLBIntensityNorm(){ return fLBIntensityNorm; }

    Float_t GetNLBPulses(){ return fNLBPulses; }

    Float_t GetMPECorrOccupancy(){ return fMPECorrOccupancy; }
    Float_t GetMPECorrOccupancyErr(){ return fMPECorrOccupancyErr; }
    Float_t GetMPECorrOccupancyCorr(){ return fMPECorrOccupancyCorr; }
    
    Float_t GetFresnelTCoeff(){ return fFresnelTCoeff; }
    
    Float_t GetDistInScint(){ return fDistInScint; }
    Float_t GetDistInAV(){ return fDistInAV; }
    Float_t GetDistInWater(){ return fDistInWater; }
    Float_t GetDistInNeck(){ return fDistInNeck; }
    Float_t GetTotalDist(){ return fTotalDist; }
    
    Float_t GetSolidAngle(){ return fSolidAngle; }
    Float_t GetCosTheta(){ return fCosTheta; }
    
    Float_t GetRelLBTheta(){ return fRelLBTheta; }
    Float_t GetRelLBPhi(){ return fRelLBPhi; }
    
    Float_t GetAVHDShadowVal(){ return fAVHDShadowVal; }
    Float_t GetGeometricShadowVal(){ return fGeometricShadowVal; }
    
    Bool_t GetAVHDShadowFlag(){ return fAVHDShadowFlag; }
    Bool_t GetGeometricShadowFlag(){ return fGeometricShadowFlag; }
    
    Bool_t GetCHSFlag(){ return fCHSFlag; }
    Bool_t GetCSSFlag(){ return fCSSFlag; }
    Bool_t GetBadPath(){ return fBadPath; }
    Bool_t GetNeckFlag(){ return fNeckFlag; }
    
    /////////////////////////////////////////
    ////////     CENTRAL GETTERS     ////////
    /////////////////////////////////////////


    Float_t GetCentralPromptPeakTime(){ return fCentralPromptPeakTime; }
    Float_t GetCentralPromptPeakWidth(){ return fCentralPromptPeakWidth; }  
    Float_t GetCentralTimeOfFlight(){ return fCentralTimeOfFlight; }
    Float_t GetCentralOccupancy(){ return fCentralOccupancy; }
    Float_t GetCentralOccupancyErr(){ return fCentralOccupancyErr; }
    Float_t GetCentralOccupancyCorr(){ return fCentralOccupancyCorr; }
    Float_t GetCentralLBIntensityNorm(){ return fCentralLBIntensityNorm; }

    Float_t GetCentralNLBPulses(){ return fCentralNLBPulses; }

    Float_t GetCentralMPECorrOccupancy(){ return fCentralMPECorrOccupancy; }
    Float_t GetCentralMPECorrOccupancyErr(){ return fCentralMPECorrOccupancyErr; }
    Float_t GetCentralMPECorrOccupancyCorr(){ return fCentralMPECorrOccupancyCorr; }
    
    Float_t GetCentralFresnelTCoeff(){ return fCentralFresnelTCoeff; }
    
    Float_t GetCentralDistInScint(){ return fCentralDistInScint; }
    Float_t GetCentralDistInAV(){ return fCentralDistInAV; }
    Float_t GetCentralDistInWater(){ return fCentralDistInWater; }
    Float_t GetCentralDistInNeck(){ return fCentralDistInNeck; }
    Float_t GetCentralTotalDist(){ return fCentralTotalDist; }
    
    Float_t GetCentralSolidAngle(){ return fCentralSolidAngle; }
    Float_t GetCentralCosTheta(){ return fCentralCosTheta; }
    
    Float_t GetCentralRelLBTheta(){ return fCentralRelLBTheta; }
    Float_t GetCentralRelLBPhi(){ return fCentralRelLBPhi; }
    
    Float_t GetCentralAVHDShadowVal(){ return fCentralAVHDShadowVal; }
    Float_t GetCentralGeometricShadowVal(){ return fCentralGeometricShadowVal; }
    
    Bool_t GetCentralAVHDShadowFlag(){ return fCentralAVHDShadowFlag; }
    Bool_t GetCentralGeometricShadowFlag(){ return fCentralGeometricShadowFlag; }
    
    Bool_t GetCentralCHSFlag(){ return fCentralCHSFlag; }
    Bool_t GetCentralCSSFlag(){ return fCentralCSSFlag; }
    Bool_t GetCentralBadPath(){ return fCentralBadPath; }
    Bool_t GetCentralNeckFlag(){ return fCentralNeckFlag; }

    ////////////////////////////////////////////
    ////////     WAVELENGTH GETTERS     ////////
    ////////////////////////////////////////////

    Float_t GetWavelengthPromptPeakTime(){ return fWavelengthPromptPeakTime; }
    Float_t GetWavelengthPromptPeakWidth(){ return fWavelengthPromptPeakWidth; }  
    Float_t GetWavelengthTimeOfFlight(){ return fWavelengthTimeOfFlight; }
    Float_t GetWavelengthOccupancy(){ return fWavelengthOccupancy; }
    Float_t GetWavelengthOccupancyErr(){ return fWavelengthOccupancyErr; }
    Float_t GetWavelengthOccupancyCorr(){ return fWavelengthOccupancyCorr; }
    Float_t GetWavelengthLBIntensityNorm(){ return fWavelengthLBIntensityNorm; }

    Float_t GetWavelengthNLBPulses(){ return fWavelengthNLBPulses; }

    Float_t GetWavelengthMPECorrOccupancy(){ return fWavelengthMPECorrOccupancy; }
    Float_t GetWavelengthMPECorrOccupancyErr(){ return fWavelengthMPECorrOccupancyErr; }
    Float_t GetWavelengthMPECorrOccupancyCorr(){ return fWavelengthMPECorrOccupancyCorr; }
    
    Float_t GetWavelengthFresnelTCoeff(){ return fWavelengthFresnelTCoeff; }
    
    Float_t GetWavelengthDistInScint(){ return fWavelengthDistInScint; }
    Float_t GetWavelengthDistInAV(){ return fWavelengthDistInAV; }
    Float_t GetWavelengthDistInWater(){ return fWavelengthDistInWater; }
    Float_t GetWavelengthDistInNeck(){ return fWavelengthDistInNeck; }
    Float_t GetWavelengthTotalDist(){ return fWavelengthTotalDist; }
    
    Float_t GetWavelengthSolidAngle(){ return fWavelengthSolidAngle; }
    Float_t GetWavelengthCosTheta(){ return fWavelengthCosTheta; }
    
    Float_t GetWavelengthRelLBTheta(){ return fWavelengthRelLBTheta; }
    Float_t GetWavelengthRelLBPhi(){ return fWavelengthRelLBPhi; }
    
    Float_t GetWavelengthAVHDShadowVal(){ return fWavelengthAVHDShadowVal; }
    Float_t GetWavelengthGeometricShadowVal(){ return fWavelengthGeometricShadowVal; }
    
    Bool_t GetWavelengthAVHDShadowFlag(){ return fWavelengthAVHDShadowFlag; }
    Bool_t GetWavelengthGeometricShadowFlag(){ return fWavelengthGeometricShadowFlag; }
    
    Bool_t GetWavelengthCHSFlag(){ return fWavelengthCHSFlag; }
    Bool_t GetWavelengthCSSFlag(){ return fWavelengthCSSFlag; }
    Bool_t GetWavelengthBadPath(){ return fWavelengthBadPath; }
    Bool_t GetWavelengthNeckFlag(){ return fWavelengthNeckFlag; }
    
  private:
    
    Int_t fID;                          // PMT ID/LCN
    Int_t fRunID;                       // The Run ID
    Int_t fCentralRunID;                // The Central Run ID
    Int_t fWavelengthRunID;             // The Wavelength Run ID
    Int_t fType;                        // The PMT type

    // The following PMT types are defined in 'rat/data/pmt/airfill2.ratdb' found in RAT.

    // 1: Normal
    // 2: OWL
    // 3: Low Gain
    // 4: BUTT
    // 5: Neck
    // 6: Calib Channel
    // 10: Spare
    // 11: Invalid

    Bool_t fIsVerified;                 // TRUE: PMT has sensible values FALSE: Some bad values
                                        // See LOCASPMT::VerifyPMT for details (LOCASPMT.cc)
    
    TVector3 fPos;                      // PMT Position
    TVector3 fNorm;                     // PMT Orientation
    
    Float_t fPromptPeakTime;            // Time of Prompt Peak [ns] 
    Float_t fPromptPeakWidth;           // Width of Prompt Peak [ns]
    Float_t fTimeOfFlight;              // Time of Flight from source to PMT [ns]
    Float_t fOccupancy;                 // Prompt Peak Counts 
    Float_t fOccupancyErr;              // Error on Occupancy of this PMT
    Float_t fOccupancyCorr;             // Ratio of Corrected Occupancy to Input Occupancy
    Float_t fLBIntensityNorm;           // The prompt peak normalisation for the entire run

    Float_t fNLBPulses;                 // Number of LaserBall pulses for this run.

    Float_t fMPECorrOccupancy;          // MPE corrected occupancy
    Float_t fMPECorrOccupancyErr;       // Error on the MPE corrected occupancy
    Float_t fMPECorrOccupancyCorr;      // Ratio of MPE corrected occupancy to input occupancy

    
    Float_t fFresnelTCoeff;             // Acrylic Fresnel transmission coefficient
    
    Float_t fDistInScint;               // Distance through the scintillator region [mm]
    Float_t fDistInAV;                  // Distance through the acrylic region [mm]
    Float_t fDistInWater;               // Distance through the water region [mm]
    Float_t fDistInNeck;                // Distance through the Neck region (if applicable) i.e. fNeckFlag == TRUE
    Float_t fTotalDist;                 // Total distance through the detector
    
    Float_t fSolidAngle;                // Solid Angle subtended by this PMT from source (LaserBall) position
    Float_t fCosTheta;                  // Cosine of light vector incident on the the PMT face
    
    Float_t fRelLBTheta;                // LaserBall Theta value for this PMT from the Laserball position
    Float_t fRelLBPhi;                  // LaserBall Phi value for this PMT from the LaserBall position
    
    Float_t fAVHDShadowVal;             // Relative shadowing value due to the AV HD ropes
    Float_t fGeometricShadowVal;        // Relative shadowing value due to the enveloping AV geometry
    
    Bool_t fAVHDShadowFlag;             // TRUE: Shadowed due to AV HD FALSE: Unshadowed (i.e. unaffected by the AV HD)
    Bool_t fGeometricShadowFlag;        // TRUE: Shadowed due to Geometry FALSE: Unshadowed (i.e. unaffected by the Geoemtry)
    
    Bool_t fCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel
    Bool_t fCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel
    Bool_t fBadPath;                    // Bad Light Path for this Channel (TRUE)
    Bool_t fNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE)

    /////////////////////////////////////
    //////// CENTRAL PMT VALUES /////////
    /////////////////////////////////////

    Float_t fCentralPromptPeakTime;            // Time of Prompt Peak [ns] - From the central run
    Float_t fCentralPromptPeakWidth;           // Width of Prompt Peak [ns] - From the central run
    Float_t fCentralTimeOfFlight;              // Time of Flight from source to PMT [ns] - From the central run
    Float_t fCentralOccupancy;                 // Prompt Peak Counts  - From the central run
    Float_t fCentralOccupancyErr;              // Error on Occupancy of this PMT - From the central run
    Float_t fCentralOccupancyCorr;             // Ratio of Corrected Occupancy to Input Occupancy - From the central run
    Float_t fCentralLBIntensityNorm;           // The prompt peak normalisation for the entire run - From the central run

    Float_t fCentralNLBPulses;                 // Number of LaserBall pulses for this run. - From the central run

    Float_t fCentralMPECorrOccupancy;          // MPE corrected occupancy - From the central run
    Float_t fCentralMPECorrOccupancyErr;       // Error on the MPE corrected occupancy - From the central run
    Float_t fCentralMPECorrOccupancyCorr;      // Ratio of MPE corrected occupancy to input occupancy - From the central run

    
    Float_t fCentralFresnelTCoeff;             // Acrylic Fresnel transmission coefficient - From the central run
    
    Float_t fCentralDistInScint;               // Distance through the scintillator region [mm] - From the central run
    Float_t fCentralDistInAV;                  // Distance through the acrylic region [mm] - From the central run
    Float_t fCentralDistInWater;               // Distance through the water region [mm] - From the central run
    Float_t fCentralDistInNeck;                // Distance through the Neck region (if applicable) i.e. fNeckFlag == TRUE - From the central run
    Float_t fCentralTotalDist;                 // Total distance through the detector - From the central run
    
    Float_t fCentralSolidAngle;                // Solid Angle subtended by this PMT from source (LaserBall) position - From the central run
    Float_t fCentralCosTheta;                  // Cosine of light vector incident on the the PMT face - From the central run
    
    Float_t fCentralRelLBTheta;                // LaserBall Theta value for this PMT from the Laserball position - From the central run
    Float_t fCentralRelLBPhi;                  // LaserBall Phi value for this PMT from the LaserBall position - From the central run
    
    Float_t fCentralAVHDShadowVal;             // Relative shadowing value due to the AV HD ropes - From the central run
    Float_t fCentralGeometricShadowVal;        // Relative shadowing value due to the enveloping AV geometry - From the central run
    
    Bool_t fCentralAVHDShadowFlag;             // TRUE: Shadowed due to AV HD FALSE: Unshadowed (i.e. unaffected by the AV HD) - From the central run
    Bool_t fCentralGeometricShadowFlag;        // TRUE: Shadowed due to Geometry FALSE: Unshadowed (i.e. unaffected by the Geoemtry) - From the central run
    
    Bool_t fCentralCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel - From the central run
    Bool_t fCentralCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel - From the central run
    Bool_t fCentralBadPath;                    // Bad Light Path for this Channel (TRUE) - From the central run
    Bool_t fCentralNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE) - From the central run

    /////////////////////////////////////
    //////// WAVELENGTH PMT VALUES /////////
    /////////////////////////////////////

    Float_t fWavelengthPromptPeakTime;            // Time of Prompt Peak [ns]  - From the wavelength run
    Float_t fWavelengthPromptPeakWidth;           // Width of Prompt Peak [ns]- From the wavelength run
    Float_t fWavelengthTimeOfFlight;              // Time of Flight from source to PMT [ns]- From the wavelength run
    Float_t fWavelengthOccupancy;                 // Prompt Peak Counts - From the wavelength run
    Float_t fWavelengthOccupancyErr;              // Error on Occupancy of this PMT- From the wavelength run
    Float_t fWavelengthOccupancyCorr;             // Ratio of Corrected Occupancy to Input Occupancy- From the wavelength run
    Float_t fWavelengthLBIntensityNorm;           // The prompt peak normalisation for the entire run- From the wavelength run

    Float_t fWavelengthNLBPulses;                 // Number of LaserBall pulses for this run.- From the wavelength run

    Float_t fWavelengthMPECorrOccupancy;          // MPE corrected occupancy- From the wavelength run
    Float_t fWavelengthMPECorrOccupancyErr;       // Error on the MPE corrected occupancy- From the wavelength run
    Float_t fWavelengthMPECorrOccupancyCorr;      // Ratio of MPE corrected occupancy to input occupancy- From the wavelength run

    
    Float_t fWavelengthFresnelTCoeff;             // Acrylic Fresnel transmission coefficient- From the wavelength run
    
    Float_t fWavelengthDistInScint;               // Distance through the scintillator region [mm]- From the wavelength run
    Float_t fWavelengthDistInAV;                  // Distance through the acrylic region [mm]- From the wavelength run
    Float_t fWavelengthDistInWater;               // Distance through the water region [mm]- From the wavelength run
    Float_t fWavelengthDistInNeck;                // Distance through the Neck region (if applicable) i.e. fNeckFlag == TRUE- From the wavelength run
    Float_t fWavelengthTotalDist;                 // Total distance through the detector- From the wavelength run
    
    Float_t fWavelengthSolidAngle;                // Solid Angle subtended by this PMT from source (LaserBall) position- From the wavelength run
    Float_t fWavelengthCosTheta;                  // Cosine of light vector incident on the the PMT face- From the wavelength run
    
    Float_t fWavelengthRelLBTheta;                // LaserBall Theta value for this PMT from the Laserball position- From the wavelength run
    Float_t fWavelengthRelLBPhi;                  // LaserBall Phi value for this PMT from the LaserBall position- From the wavelength run
    
    Float_t fWavelengthAVHDShadowVal;             // Relative shadowing value due to the AV HD ropes- From the wavelength run
    Float_t fWavelengthGeometricShadowVal;        // Relative shadowing value due to the enveloping AV geometry- From the wavelength run
    
    Bool_t fWavelengthAVHDShadowFlag;             // TRUE: Shadowed due to AV HD FALSE: Unshadowed (i.e. unaffected by the AV HD)- From the wavelength run
    Bool_t fWavelengthGeometricShadowFlag;        // TRUE: Shadowed due to Geometry FALSE: Unshadowed (i.e. unaffected by the Geoemtry)- From the wavelength run
    
    Bool_t fWavelengthCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel- From the wavelength run
    Bool_t fWavelengthCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel- From the wavelength run
    Bool_t fWavelengthBadPath;                    // Bad Light Path for this Channel (TRUE)- From the wavelength run
    Bool_t fWavelengthNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE)- From the wavelength run
    
    ClassDef( LOCASPMT, 1 );
    
  };

}

#endif

