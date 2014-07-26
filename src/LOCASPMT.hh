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
///     02/2014 : RPFS - First Revision, new file. \n
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
    void VerifyPMT();                            // Verify that the fields of the PMT are complete and have 'reasonable' values
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetID( const Int_t pmtID ){ fID = pmtID; }

    void SetRunID( const Int_t runID ){ fRunID = runID; }
    void SetCentralRunID( const Int_t ctrRunID ){ fCentralRunID = ctrRunID; }
    void SetWavelengthRunID( const Int_t wvRunID ){ fWavelengthRunID = wvRunID; }
    void SetType( const Int_t pmtType ){ fType = pmtType; }


    void SetIsVerified( const Bool_t verified ){ fIsVerified = verified; }
    void SetCentralIsVerified( const Bool_t verified ){ fCentralIsVerified = verified; }
    void SetWavelengthIsVerified( const Bool_t verified ){ fWavelengthIsVerified = verified; }

    void SetDQXXFlag( const Int_t val ){ fDQXXFlag = val; }
    void SetCentralDQXXFlag( const Int_t val ){ fCentralDQXXFlag = val; }
    void SetWavelengthDQXXFlag( const Int_t val ){ fWavelengthDQXXFlag = val; }
                                                                          
    void SetPos( const TVector3 xyzPos ){ fPos = xyzPos; }                  
    void SetNorm( const TVector3 uvwOri ){ fNorm = uvwOri; }                
    
    void SetPromptPeakTime( const Float_t tPeak ){ fPromptPeakTime = tPeak; }
    void SetPromptPeakWidth( const Float_t tWidth ){ fPromptPeakWidth = tWidth; }  
    void SetTimeOfFlight( const Float_t tOF ){ fTimeOfFlight = tOF; }
    void SetOccupancy( const Float_t occ ){ fOccupancy = occ; }
    void SetOccupancyErr( const Float_t occErr ){ fOccupancyErr = occErr; }
    void SetOccupancyCorr( const Float_t occCorr ){ fOccupancyCorr = occCorr; }

    void SetLBIntensityNorm( const Float_t intensityNorm ){ fLBIntensityNorm = intensityNorm; }
    void SetNLBPulses( const Float_t nPulses ){ fNLBPulses = nPulses; }

    void SetMPECorrOccupancy( const Float_t mpeCorr ){ fMPECorrOccupancy = mpeCorr; }
    void SetMPECorrOccupancyErr( const Float_t mpeCorrErr ){ fMPECorrOccupancyErr = mpeCorrErr; }
    void SetMPECorrOccupancyCorr( const Float_t mpeCorrCorr ){ fMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetFresnelTCoeff( const Float_t fresnelCoeff ){ fFresnelTCoeff = fresnelCoeff; }
    
    void SetDistInScint( const Float_t distInScint ){ fDistInScint = distInScint; }
    void SetDistInAV( const Float_t distInAV ){ fDistInAV = distInAV; }
    void SetDistInWater( const Float_t distInWater ){ fDistInWater = distInWater; }
    void SetDistInNeck( const Float_t distInNeck ){ fDistInNeck = distInNeck; }
    void SetTotalDist( const Float_t distTotal ){ fTotalDist = distTotal; }
    
    void SetSolidAngle( const Float_t solidA ){ fSolidAngle = solidA; }
    void SetCosTheta( const Float_t cosTheta ){ fCosTheta = cosTheta; }
    
    void SetRelLBTheta( const Float_t theta ){ fRelLBTheta = theta; }
    void SetRelLBPhi( const Float_t phi ){ fRelLBPhi = phi; }

    void SetCHSFlag( const Bool_t boolVal ){ fCHSFlag = boolVal; }
    void SetCSSFlag( const Bool_t boolVal ){ fCSSFlag = boolVal; }

    void SetBadPath( const Bool_t badPath ){ fBadPath = badPath; }
    void SetNeckFlag( const Bool_t neckFlag ){ fNeckFlag = neckFlag; }

    void SetInitialLBVec( const TVector3 vec ){ fInitialLBVec = vec; }
    void SetIncidentLBVec( const TVector3 vec ){ fIncidentLBVec = vec; }

    /////////////////////////////////////////
    ////////     CENTRAL SETTERS     ////////
    /////////////////////////////////////////

    void SetCentralPromptPeakTime( const  Float_t tPeak ){ fCentralPromptPeakTime = tPeak; }
    void SetCentralPromptPeakWidth( const  Float_t tWidth ){ fCentralPromptPeakWidth = tWidth; }  
    void SetCentralTimeOfFlight( const  Float_t tOF ){ fCentralTimeOfFlight = tOF; }
    void SetCentralOccupancy( const  Float_t occ ){ fCentralOccupancy = occ; }
    void SetCentralOccupancyErr( const  Float_t occErr ){ fCentralOccupancyErr = occErr; }
    void SetCentralOccupancyCorr( const  Float_t occCorr ){ fCentralOccupancyCorr = occCorr; }
    void SetCentralLBIntensityNorm( const  Float_t intensityNorm ){ fCentralLBIntensityNorm = intensityNorm; }

    void SetCentralNLBPulses( const  Float_t nPulses ){ fCentralNLBPulses = nPulses; }

    void SetCentralMPECorrOccupancy( const  Float_t mpeCorr ){ fCentralMPECorrOccupancy = mpeCorr; }
    void SetCentralMPECorrOccupancyErr( const  Float_t mpeCorrErr ){ fCentralMPECorrOccupancyErr = mpeCorrErr; }
    void SetCentralMPECorrOccupancyCorr( const  Float_t mpeCorrCorr ){ fCentralMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetCentralFresnelTCoeff( const  Float_t fresnelCoeff ){ fCentralFresnelTCoeff = fresnelCoeff; }
    
    void SetCentralDistInScint( const  Float_t distInScint ){ fCentralDistInScint = distInScint; }
    void SetCentralDistInAV( const  Float_t distInAV ){ fCentralDistInAV = distInAV; }
    void SetCentralDistInWater( const  Float_t distInWater ){ fCentralDistInWater = distInWater; }
    void SetCentralDistInNeck( const  Float_t distInNeck ){ fCentralDistInNeck = distInNeck; }
    void SetCentralTotalDist( const  Float_t distTotal ){ fCentralTotalDist = distTotal; }
    
    void SetCentralSolidAngle( const  Float_t solidA ){ fCentralSolidAngle = solidA; }
    void SetCentralCosTheta( const  Float_t cosTheta ){ fCentralCosTheta = cosTheta; }
    
    void SetCentralRelLBTheta( const  Float_t theta ){ fCentralRelLBTheta = theta; }
    void SetCentralRelLBPhi( const  Float_t phi ){ fCentralRelLBPhi = phi; }

    void SetCentralCHSFlag( const Bool_t boolVal ){ fCentralCHSFlag = boolVal; }
    void SetCentralCSSFlag( const Bool_t boolVal ){ fCentralCSSFlag = boolVal; }
    
    void SetCentralBadPath( const  Bool_t badPath ){ fCentralBadPath = badPath; }
    void SetCentralNeckFlag( const  Bool_t neckFlag ){ fCentralNeckFlag = neckFlag; }

    void SetCentralInitialLBVec( const TVector3 vec ){ fCentralInitialLBVec = vec; }
    void SetCentralIncidentLBVec( const TVector3 vec ){ fCentralIncidentLBVec = vec; }

    ////////////////////////////////////////////
    ////////     WAVELENGTH SETTERS     ////////
    ////////////////////////////////////////////

    void SetWavelengthPromptPeakTime( const  Float_t tPeak ){ fWavelengthPromptPeakTime = tPeak; }
    void SetWavelengthPromptPeakWidth( const  Float_t tWidth ){ fWavelengthPromptPeakWidth = tWidth; }  
    void SetWavelengthTimeOfFlight( const  Float_t tOF ){ fWavelengthTimeOfFlight = tOF; }
    void SetWavelengthOccupancy( const  Float_t occ ){ fWavelengthOccupancy = occ; }
    void SetWavelengthOccupancyErr( const  Float_t occErr ){ fWavelengthOccupancyErr = occErr; }
    void SetWavelengthOccupancyCorr( const  Float_t occCorr ){ fWavelengthOccupancyCorr = occCorr; }
    void SetWavelengthLBIntensityNorm( const  Float_t intensityNorm ){ fWavelengthLBIntensityNorm = intensityNorm; }

    void SetWavelengthNLBPulses( const  Float_t nPulses ){ fWavelengthNLBPulses = nPulses; }

    void SetWavelengthMPECorrOccupancy( const  Float_t mpeCorr ){ fWavelengthMPECorrOccupancy = mpeCorr; }
    void SetWavelengthMPECorrOccupancyErr( const  Float_t mpeCorrErr ){ fWavelengthMPECorrOccupancyErr = mpeCorrErr; }
    void SetWavelengthMPECorrOccupancyCorr( const  Float_t mpeCorrCorr ){ fWavelengthMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetWavelengthFresnelTCoeff( const  Float_t fresnelCoeff ){ fWavelengthFresnelTCoeff = fresnelCoeff; }
    
    void SetWavelengthDistInScint( const  Float_t distInScint ){ fWavelengthDistInScint = distInScint; }
    void SetWavelengthDistInAV( const  Float_t distInAV ){ fWavelengthDistInAV = distInAV; }
    void SetWavelengthDistInWater( const  Float_t distInWater ){ fWavelengthDistInWater = distInWater; }
    void SetWavelengthDistInNeck( const  Float_t distInNeck ){ fWavelengthDistInNeck = distInNeck; }
    void SetWavelengthTotalDist( const  Float_t distTotal ){ fWavelengthTotalDist = distTotal; }
    
    void SetWavelengthSolidAngle( const  Float_t solidA ){ fWavelengthSolidAngle = solidA; }
    void SetWavelengthCosTheta( const  Float_t cosTheta ){ fWavelengthCosTheta = cosTheta; }
    
    void SetWavelengthRelLBTheta( const  Float_t theta ){ fWavelengthRelLBTheta = theta; }
    void SetWavelengthRelLBPhi( const  Float_t phi ){ fWavelengthRelLBPhi = phi; }

    void SetWavelengthCHSFlag( const Bool_t boolVal ){ fWavelengthCHSFlag = boolVal; }
    void SetWavelengthCSSFlag( const Bool_t boolVal ){ fWavelengthCSSFlag = boolVal; }
    
    void SetWavelengthBadPath( const  Bool_t badPath ){ fWavelengthBadPath = badPath; }
    void SetWavelengthNeckFlag( const  Bool_t neckFlag ){ fWavelengthNeckFlag = neckFlag; }

    void SetWavelengthInitialLBVec( const TVector3 vec ){ fWavelengthInitialLBVec = vec; }
    void SetWavelengthIncidentLBVec( const TVector3 vec ){ fWavelengthIncidentLBVec = vec; }
    
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Int_t GetID() const { return fID; }

    Int_t GetRunID() const { return fRunID; }
    Int_t GetCentralRunID() const { return fCentralRunID; }
    Int_t GetWavelengthRunID() const { return fWavelengthRunID; }
    Int_t GetType() const { return fType; }

    Bool_t GetIsVerified() const { return fIsVerified; }
    Bool_t GetCentralIsVerified() const { return fCentralIsVerified; }
    Bool_t GetWavelengthIsVerified() const { return fWavelengthIsVerified; }

    Int_t GetDQXXFlag() const { return fDQXXFlag; }
    Int_t GetCentralDQXXFlag() const { return fCentralDQXXFlag; }
    Int_t GetWavelengthDQXXFlag() const { return fWavelengthDQXXFlag; }
  
    TVector3 GetPos() const { return fPos; }
    TVector3 GetNorm() const { return fNorm; }
    
    Float_t GetPromptPeakTime() const { return fPromptPeakTime; }
    Float_t GetPromptPeakWidth() const { return fPromptPeakWidth; }  
    Float_t GetTimeOfFlight() const { return fTimeOfFlight; }
    Float_t GetOccupancy() const { return fOccupancy; }
    Float_t GetOccupancyErr() const { return fOccupancyErr; }
    Float_t GetOccupancyCorr() const { return fOccupancyCorr; }
    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }

    Float_t GetNLBPulses() const { return fNLBPulses; }

    Float_t GetMPECorrOccupancy() const { return fMPECorrOccupancy; }
    Float_t GetMPECorrOccupancyErr() const { return fMPECorrOccupancyErr; }
    Float_t GetMPECorrOccupancyCorr() const { return fMPECorrOccupancyCorr; }
    
    Float_t GetFresnelTCoeff() const { return fFresnelTCoeff; }
    
    Float_t GetDistInScint() const { return fDistInScint; }
    Float_t GetDistInAV() const { return fDistInAV; }
    Float_t GetDistInWater() const { return fDistInWater; }
    Float_t GetDistInNeck() const { return fDistInNeck; }
    Float_t GetTotalDist() const { return fTotalDist; }
    
    Float_t GetSolidAngle() const { return fSolidAngle; }
    Float_t GetCosTheta() const { return fCosTheta; }
    
    Float_t GetRelLBTheta() const { return fRelLBTheta; }
    Float_t GetRelLBPhi() const { return fRelLBPhi; }
    
    Bool_t GetCHSFlag() const { return fCHSFlag; }
    Bool_t GetCSSFlag() const { return fCSSFlag; }

    Bool_t GetBadPath() const { return fBadPath; }
    Bool_t GetNeckFlag() const { return fNeckFlag; }

    TVector3 GetInitialLBVec() const { return fInitialLBVec; }
    TVector3 GetIncidentLBVec() const { return fIncidentLBVec; }
    
    /////////////////////////////////////////
    ////////     CENTRAL GETTERS     ////////
    /////////////////////////////////////////


    Float_t GetCentralPromptPeakTime() const { return fCentralPromptPeakTime; }
    Float_t GetCentralPromptPeakWidth() const { return fCentralPromptPeakWidth; }  
    Float_t GetCentralTimeOfFlight() const { return fCentralTimeOfFlight; }
    Float_t GetCentralOccupancy() const { return fCentralOccupancy; }
    Float_t GetCentralOccupancyErr() const { return fCentralOccupancyErr; }
    Float_t GetCentralOccupancyCorr() const { return fCentralOccupancyCorr; }
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }

    Float_t GetCentralNLBPulses() const { return fCentralNLBPulses; }

    Float_t GetCentralMPECorrOccupancy() const { return fCentralMPECorrOccupancy; }
    Float_t GetCentralMPECorrOccupancyErr() const { return fCentralMPECorrOccupancyErr; }
    Float_t GetCentralMPECorrOccupancyCorr() const { return fCentralMPECorrOccupancyCorr; }
    
    Float_t GetCentralFresnelTCoeff() const { return fCentralFresnelTCoeff; }
    
    Float_t GetCentralDistInScint() const { return fCentralDistInScint; }
    Float_t GetCentralDistInAV() const { return fCentralDistInAV; }
    Float_t GetCentralDistInWater() const { return fCentralDistInWater; }
    Float_t GetCentralDistInNeck() const { return fCentralDistInNeck; }
    Float_t GetCentralTotalDist() const { return fCentralTotalDist; }
    
    Float_t GetCentralSolidAngle() const { return fCentralSolidAngle; }
    Float_t GetCentralCosTheta() const { return fCentralCosTheta; }
    
    Float_t GetCentralRelLBTheta() const { return fCentralRelLBTheta; }
    Float_t GetCentralRelLBPhi() const { return fCentralRelLBPhi; }
    
    Bool_t GetCentralCHSFlag() const { return fCentralCHSFlag; }
    Bool_t GetCentralCSSFlag() const { return fCentralCSSFlag; }

    Bool_t GetCentralBadPath() const { return fCentralBadPath; }
    Bool_t GetCentralNeckFlag() const { return fCentralNeckFlag; }

    TVector3 GetCentralInitialLBVec() const { return fCentralInitialLBVec; }
    TVector3 GetCentralIncidentLBVec() const { return fCentralIncidentLBVec; }

    ////////////////////////////////////////////
    ////////     WAVELENGTH GETTERS     ////////
    ////////////////////////////////////////////

    Float_t GetWavelengthPromptPeakTime() const { return fWavelengthPromptPeakTime; }
    Float_t GetWavelengthPromptPeakWidth() const { return fWavelengthPromptPeakWidth; }  
    Float_t GetWavelengthTimeOfFlight() const { return fWavelengthTimeOfFlight; }
    Float_t GetWavelengthOccupancy() const { return fWavelengthOccupancy; }
    Float_t GetWavelengthOccupancyErr() const { return fWavelengthOccupancyErr; }
    Float_t GetWavelengthOccupancyCorr() const { return fWavelengthOccupancyCorr; }
    Float_t GetWavelengthLBIntensityNorm() const { return fWavelengthLBIntensityNorm; }

    Float_t GetWavelengthNLBPulses() const { return fWavelengthNLBPulses; }

    Float_t GetWavelengthMPECorrOccupancy() const { return fWavelengthMPECorrOccupancy; }
    Float_t GetWavelengthMPECorrOccupancyErr() const { return fWavelengthMPECorrOccupancyErr; }
    Float_t GetWavelengthMPECorrOccupancyCorr() const { return fWavelengthMPECorrOccupancyCorr; }
    
    Float_t GetWavelengthFresnelTCoeff() const { return fWavelengthFresnelTCoeff; }
    
    Float_t GetWavelengthDistInScint() const { return fWavelengthDistInScint; }
    Float_t GetWavelengthDistInAV() const { return fWavelengthDistInAV; }
    Float_t GetWavelengthDistInWater() const { return fWavelengthDistInWater; }
    Float_t GetWavelengthDistInNeck() const { return fWavelengthDistInNeck; }
    Float_t GetWavelengthTotalDist() const { return fWavelengthTotalDist; }
    
    Float_t GetWavelengthSolidAngle() const { return fWavelengthSolidAngle; }
    Float_t GetWavelengthCosTheta() const { return fWavelengthCosTheta; }
    
    Float_t GetWavelengthRelLBTheta() const { return fWavelengthRelLBTheta; }
    Float_t GetWavelengthRelLBPhi() const { return fWavelengthRelLBPhi; }
    
    Bool_t GetWavelengthCHSFlag() const { return fWavelengthCHSFlag; }
    Bool_t GetWavelengthCSSFlag() const { return fWavelengthCSSFlag; }

    Bool_t GetWavelengthBadPath() const { return fWavelengthBadPath; }
    Bool_t GetWavelengthNeckFlag() const { return fWavelengthNeckFlag; }

    TVector3 GetWavelengthInitialLBVec() const { return fWavelengthInitialLBVec; }
    TVector3 GetWavelengthIncidentLBVec() const { return fWavelengthIncidentLBVec; }
    
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
    Bool_t fCentralIsVerified;
    Bool_t fWavelengthIsVerified;

    Int_t fDQXXFlag;                    // Integer status for DQXX KCCC_TUBE_ON_LINE the PMT from off-axis run
    Int_t fCentralDQXXFlag;             // Integer status for DQXX KCCC_TUBE_ON_LINE the PMT from central run
    Int_t fWavelengthDQXXFlag;          // Integer status for DQXX KCCC_TUBE_ON_LINE the PMT from wavelength run
    
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
    
    Bool_t fCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel
    Bool_t fCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel

    Bool_t fBadPath;                    // Bad Light Path for this Channel (TRUE)
    Bool_t fNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE)

    TVector3 fInitialLBVec;             // Initial (unit) vector direction of light from source
    TVector3 fIncidentLBVec;            // Incident (unit) vector direction on PMT face

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
    
    Bool_t fCentralCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel - From the central run
    Bool_t fCentralCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel - From the central run

    Bool_t fCentralBadPath;                    // Bad Light Path for this Channel (TRUE) - From the central run
    Bool_t fCentralNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE) - From the central run

    TVector3 fCentralInitialLBVec;             // Initial (unit) vector direction of light from source - From the central run
    TVector3 fCentralIncidentLBVec;            // Incident (unit) vector direction on PMT face - From the central run

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
    
    Bool_t fWavelengthCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel- From the wavelength run
    Bool_t fWavelengthCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel- From the wavelength run

    Bool_t fWavelengthBadPath;                    // Bad Light Path for this Channel (TRUE)- From the wavelength run
    Bool_t fWavelengthNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE)- From the wavelength run

    TVector3 fWavelengthInitialLBVec;             // Initial (unit) vector direction of light from source- From the wavelength run
    TVector3 fWavelengthIncidentLBVec;            // Incident (unit) vector direction on PMT face- From the wavelength run
    
    ClassDef( LOCASPMT, 1 );
    
  };

}

#endif

