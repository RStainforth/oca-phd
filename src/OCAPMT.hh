////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAPMT.hh
///
/// CLASS: OCA::OCAPMT
///
/// BRIEF: PMT level data structure for OCA optics analysis
///        Used as part of the OCARun class
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     05/2014 : RPFS - First Revision, new file.
///
/// DETAIL: This data structure contains all the information
///         for a specific PMT in a run, required as an input
///         to an OCAPMT object for use in an optics 
///         fit/analysis using OCA.
///
///         Herein lies information specific to the PMT itself,
///         such as distances through various media from the source,
///         the PMT ID, solidangle etc.
///
///         Information is stored for all three runs which
///         traditionally are used for the fit:
///                   - The off-axis run.
///                   - The normalising central run.
///                   - The same off-axis run position at a different wavelength (traditionally 505 nm)
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAPMT_
#define _OCAPMT_

#include "RAT/DS/SOCPMT.hh"
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"
#include "RAT/DU/ChanHWStatus.hh"

#include "TVector3.h"
#include "TObject.h"

namespace OCA{

  class OCAPMT : public TObject
  {
  public:

    // The constructors and destructors for the OCAPMT object.
    OCAPMT(){ ClearPMT(); }
    OCAPMT( Int_t pmtID ){ 
      fID = pmtID; 
      ClearPMT(); 
    }
    OCAPMT( const OCAPMT& rhs );
    ~OCAPMT(){ }

    // The equality operator for OCAPMT types.
    OCAPMT& operator=( const OCAPMT& rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    // Initialise and clear all the private member variables
    // to non-physical values before assigning them
    // using OCAPMT::AddSOCPMTData.
    void ClearPMT();

    // Add information from a SOCPMT object to this OCAPMT object
    void AddSOCPMTData( RAT::DS::SOCPMT& socPMT );

    // Process a light path to calculate the distances, solidangle
    // and Fresnel transmission coefficient.
    void ProcessLightPath( RAT::DU::LightPathCalculator& lPath,
                           RAT::DU::ShadowingCalculator& shadCalc,
                           RAT::DU::ChanHWStatus& chanHW );

    // Verify that the fields of the PMT 
    // are complete and have 'reasonable' values. See the method
    // definition for what 'reasonable' means for each member variable.
    void VerifyPMT();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    // Get the ID for the PMT.
    Int_t GetID() const { return fID; }
    
    // Get the run ID for the run that this PMT came from.
    Int_t GetRunID() const { return fRunID; }

    // Get the corresponding central run associated with this PMT.
    Int_t GetCentralRunID() const { return fCentralRunID; }
    
    // Get the corresponding wavelength run associated with this PMT.
    Int_t GetWavelengthRunID() const { return fWavelengthRunID; }

    // Get the PMT type, as determined by the RAT::DU::PMTInfo::GetType utility.
    Int_t GetType() const { return fType; }

    // Get whether or not OCAPMT::VerifyPMT has been called for this
    // PMT and whether the off-axis run values are reasonable.
    Bool_t GetIsVerified() const { return fIsVerified; }

    // Get whether or not OCAPMT::VerifyPMT has been called for this
    // PMT and whether the central run values are reasonable.
    Bool_t GetCentralIsVerified() const { return fCentralIsVerified; }
  
    // Get the position of this PMT as determined by the
    // RAT::DU::PMTInfo::GetPosition utility.
    TVector3 GetPos() const { return fPos; }

    // Get the normal direction of the PMT face as determined
    // be the RAT::DU::PMTInfo::GetDirection utility.
    TVector3 GetNorm() const { return fNorm; }

    // Get the position of the laserball from the run
    // this PMT came from.
    TVector3 GetLBPos() const { return fLBPos; }

    // Get the position of the laserball from the central run
    // this PMT came from.
    TVector3 GetCentralLBPos() const { return fCentralLBPos; }

    // Get the position of the laserball from the wavelength run
    // this PMT came from.
    TVector3 GetWavelengthLBPos() const { return fWavelengthLBPos; }

    // Get the orientation of the laserball from the off-axis run
    // this PMT came from.
    Float_t GetLBOrientation() const { return fLBOrientation; }

    // Get the orientation of the laserball from the central run
    // this PMT came from.
    Float_t GetCentralLBOrientation() const { return fCentralLBOrientation; }

    // Get the orientation of the laserball from the wavelength run
    // this PMT came from.
    Float_t GetWavelengthLBOrientation() const { return fWavelengthLBOrientation; }
    
     // Get the prompt peak time for this PMT in the off-axis run. [ns]
    Float_t GetPromptPeakTime() const { return fPromptPeakTime; }

    // Get the width of the prompt peak for this PMT in the off-axis run. [ns]
    Float_t GetPromptPeakWidth() const { return fPromptPeakWidth; }
    
    // Get the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // off-axis run.
    Float_t GetPromptPeakCounts() const { return fPromptPeakCounts; }
    
    // Get the error on the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // off-axis run.
    Float_t GetPromptPeakCountsErr() const { return fPromptPeakCountsErr; }

    // Get the time of flight from the laserball to the PMT
    // from the off-axis run. [ns]
    Float_t GetTimeOfFlight() const { return fTimeOfFlight; }

    // Get the laserball intensity normalisation value for the 
    // off-axis run associated with this PMT.
    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }

    // Get the number of laserball pulses in the off-axis
    // run associated with this PMT.
    Float_t GetNLBPulses() const { return fNLBPulses; }

    // Get the Multi-Photoelectron corrected occupancy for this PMT
    // for the off-axis run.
    Float_t GetMPECorrOccupancy() const { return fMPECorrOccupancy; }

    // Get the error on the Multi-Photoelectron corrected occupancy 
    // for this PMT for the off-axis run.
    Float_t GetMPECorrOccupancyErr() const { return fMPECorrOccupancyErr; }
    
    // Get the Fresnel transmission coefficient for the light path
    // associated with this PMT from the off-axis run.
    Float_t GetFresnelTCoeff() const { return fFresnelTCoeff; }
    
    // Get the distance in the inner AV region associated with the
    // light path associated with this PMT from the off-axis run.
    Float_t GetDistInInnerAV() const { return fDistInInnerAV; }

    // Get the distance in the AV region associated with the
    // light path associated with this PMT from the off-axis run.
    Float_t GetDistInAV() const { return fDistInAV; }

    // Get the distance in the water region associated with the
    // light path associated with this PMT from the off-axis run.
    Float_t GetDistInWater() const { return fDistInWater; }
    
    // Get the value of the solid angle subtended by the PMT from the
    // laserball position in the off-axis run.
    Float_t GetSolidAngle() const { return fSolidAngle; }

    // Get the value of the cosine of the incident angle of the
    // light path incident on this PMT from the laserball from the
    // off-axis run.
    Float_t GetCosTheta() const { return fCosTheta; }
    
    // Get the value the theta value of the light path from the laserball
    // in the off-axis run relative to the laserball axis.
    Float_t GetRelLBTheta() const { return fRelLBTheta; }

    // Get the value the phi value of the light path from the laserball
    // in the off-axis run relative to the laserball axis.
    Float_t GetRelLBPhi() const { return fRelLBPhi; }
    
    // Get the CHS flag for this PMT from the off-axis run.
    // Note: Same as DQXX for now. Will deprecate one depending
    // on best choice of name.
    Bool_t GetCHSFlag() const { return fCHSFlag; }

    // Get the CSS flag for this PMT from the off-axis run.
    // Note: Same as DQXX for now. Will deprecate one depending
    // on best choice of name.
    Bool_t GetCSSFlag() const { return fCSSFlag; }

    // Get the flag associated with the state of the light path
    // associated with this PMT from the off-axis run.
    Bool_t GetBadPath() const { return fBadPath; }

    // Get the flag associated with whether the light path went 
    // thorugh the neck or not to reach this PMT from the
    // off-axis run.
    Bool_t GetNeckFlag() const { return fNeckFlag; }

    // Get the initial photon vector associated with the lightpath
    // which reached this PMT in the off-axis run.
    TVector3 GetInitialLBVec() const { return fInitialLBVec; }

    // Get the incident photon vector associated with the lightpath
    // which reached this PMT in the off-axis run.
    TVector3 GetIncidentLBVec() const { return fIncidentLBVec; }
    
    /////////////////////////////////////////
    ////////     CENTRAL GETTERS     ////////
    /////////////////////////////////////////

    // Get the prompt peak time for this PMT in the central run. [ns]
    Float_t GetCentralPromptPeakTime() const { return fCentralPromptPeakTime; }
    
    // Get the width of the prompt peak for this PMT in the central run. [ns]
    Float_t GetCentralPromptPeakWidth() const { return fCentralPromptPeakWidth; }

    // Get the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // central run.
    Float_t GetCentralPromptPeakCounts() const { return fCentralPromptPeakCounts; }

    // Get the error on the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // central run.
    Float_t GetCentralPromptPeakCountsErr() const { return fCentralPromptPeakCountsErr; }

    // Get the time of flight from the laserball to the PMT
    // from the central run. [ns]
    Float_t GetCentralTimeOfFlight() const { return fCentralTimeOfFlight; }

    // Get the laserball intensity normalisation value for the 
    // central run associated with this PMT.
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }

    // Get the number of laserball pulses in the central
    // run associated with this PMT.
    Float_t GetCentralNLBPulses() const { return fCentralNLBPulses; }

    // Get the Multi-Photoelectron corrected occupancy for this PMT
    // for the central run.
    Float_t GetCentralMPECorrOccupancy() const { return fCentralMPECorrOccupancy; }

    // Get the error on the Multi-Photoelectron corrected occupancy 
    // for this PMT for the central run.
    Float_t GetCentralMPECorrOccupancyErr() const { return fCentralMPECorrOccupancyErr; }
           
    // Get the Fresnel transmission coefficient for the light path
    // associated with this PMT from the central run.
    Float_t GetCentralFresnelTCoeff() const { return fCentralFresnelTCoeff; }
    
    // Get the distance in the inner AV region associated with the
    // light path associated with this PMT from the central run.
    Float_t GetCentralDistInInnerAV() const { return fCentralDistInInnerAV; }

    // Get the distance in the AV region associated with the
    // light path associated with this PMT from the central run.
    Float_t GetCentralDistInAV() const { return fCentralDistInAV; }

    // Get the distance in the water region associated with the
    // light path associated with this PMT from the central run.
    Float_t GetCentralDistInWater() const { return fCentralDistInWater; }
    
    // Get the value of the solid angle subtended by the PMT from the
    // laserball position in the central run.
    Float_t GetCentralSolidAngle() const { return fCentralSolidAngle; }

    // Get the value of the cosine of the incident angle of the
    // light path incident on this PMT from the laserball from the
    // central run.
    Float_t GetCentralCosTheta() const { return fCentralCosTheta; }
    
    // Get the value the theta value of the light path from the laserball
    // in the central run relative to the laserball axis.
    Float_t GetCentralRelLBTheta() const { return fCentralRelLBTheta; }

    // Get the value the phi value of the light path from the laserball
    // in the central run relative to the laserball axis.
    Float_t GetCentralRelLBPhi() const { return fCentralRelLBPhi; }
    
    // Get the CHS flag for this PMT from the central run.
    // Note: Same as DQXX for now. Will deprecate one depending
    // on best choice of name.
    Bool_t GetCentralCHSFlag() const { return fCentralCHSFlag; }

    // Get the CSS flag for this PMT from the central run.
    // Note: Same as DQXX for now. Will deprecate one depending
    // on best choice of name.
    Bool_t GetCentralCSSFlag() const { return fCentralCSSFlag; }

    // Get the flag associated with the state of the light path
    // associated with this PMT from the central run.
    Bool_t GetCentralBadPath() const { return fCentralBadPath; }

    // Get the flag associated with whether the light path went 
    // thorugh the neck or not to reach this PMT from the
    // central run.
    Bool_t GetCentralNeckFlag() const { return fCentralNeckFlag; }

    // Get the initial photon vector associated with the lightpath
    // which reached this PMT in the central run.
    TVector3 GetCentralInitialLBVec() const { return fCentralInitialLBVec; }

    // Get the incident photon vector associated with the lightpath
    // which reached this PMT in the central run.
    TVector3 GetCentralIncidentLBVec() const { return fCentralIncidentLBVec; }

    //////////////////////////////////////////////////////
    ////////     GETTERS FOR FITTING ROUTINES     ////////
    //////////////////////////////////////////////////////

    // Get the occupancy ratio for this PMT
    // Occupancy ratio is MPECorrOccupancy / CentralMPECorrOccupancy
    Float_t GetOccupancyRatio() const { return fOccupancyRatio; }

    // Get the model predicted occupancy ratio for this PMT
    Float_t GetModelOccupancyRatio() const { return fModelOccupancyRatio; }

    // Get the chi-square residual for this PMT
    Float_t GetChiSquareResidual() const { return fChiSquareResidual; }

    // Get the error on the occupancy ratio for this PMT
    Float_t GetOccupancyRatioErr() const { return fOccupancyRatioErr; }

    // Get the model predicted occupancy for this PMT
    Float_t GetModelOccupancy() const { return fModelOccupancy; }

    // Get the systematic PMT variability error for this PMT.
    Float_t GetPMTVariability() const { return fPMTVariability; }

    // Get the index of the run as this PMT appeared in the fit.
    Int_t GetRunIndex() const { return fRunIndex; }

    // Get the relative efficiency estimator of the PMT as it appeared in the run in the fit.
    Float_t GetRunEfficiency() const { return fRunEfficiency; }

    // Get the relative efficiency estimator of the PMT normalised to all run instances
    // of this PMT in an entire scan.
    Float_t GetScanEfficiency() const { return fScanEfficiency; }
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    // Set the ID for the PMT.
    void SetID( const Int_t pmtID ){ fID = pmtID; }

    // Set the run ID for the run that this PMT came from.
    void SetRunID( const Int_t runID ){ fRunID = runID; }

    // Set the corresponding central run associated with this PMT.
    void SetCentralRunID( const Int_t ctrRunID ){ fCentralRunID = ctrRunID; }

    // Set the corresponding wavelength run associated with this PMT.
    void SetWavelengthRunID( const Int_t wvRunID ){ fWavelengthRunID = wvRunID; }

    // Set the PMT type, as determined by the RAT::DU::PMTInfo::GetType utility.
    void SetType( const Int_t pmtType ){ fType = pmtType; }

    // Set whether or not OCAPMT::VerifyPMT has been called for this
    // PMT and whether the off-axis run values are reasonable.
    void SetIsVerified( const Bool_t verified ){ fIsVerified = verified; }

    // Set whether or not OCAPMT::VerifyPMT has been called for this
    // PMT and whether the central run values are reasonable.
    void SetCentralIsVerified( const Bool_t verified ){ fCentralIsVerified = verified; }
                            
    // Set the position of this PMT as determined by the
    // RAT::DU::PMTInfo::GetPosition utility.
    void SetPos( const TVector3 xyzPos ){ fPos = xyzPos; }

    // Set the normal direction of the PMT face as determined
    // be the RAT::DU::PMTInfo::GetDirection utility.
    void SetNorm( const TVector3 uvwOri ){ fNorm = uvwOri; } 

    // Set the position of the laserball from the off-axis run
    // this PMT came from.
    void SetLBPos( const TVector3 lbPos ){ fLBPos = lbPos; }

    // Set the position of the laserball from the central run
    // this PMT came from.
    void SetCentralLBPos( const TVector3 lbPos ){ fCentralLBPos = lbPos; }

    // Set the position of the laserball from the wavelength run
    // this PMT came from.
    void SetWavelengthLBPos( const TVector3 lbPos ){ fWavelengthLBPos = lbPos; }

    // Set the orientation of the laserball from the off-axis run
    // this PMT came from.
    void SetLBOrientation( const Float_t lbOri ){ fLBOrientation = lbOri; }

    // Set the orientation of the laserball from the central run
    // this PMT came from.
    void SetCentralLBOrientation( const Float_t lbOri ){ fCentralLBOrientation = lbOri; } 

    // Set the orientation of the laserball from the wavelength run
    // this PMT came from.
    void SetWavelengthLBOrientation( const Float_t lbOri ){ fWavelengthLBOrientation = lbOri; }            
    
    // Set the prompt peak time for this PMT in the off-axis run. [ns]
    void SetPromptPeakTime( const Float_t tPeak ){ fPromptPeakTime = tPeak; }

    // Set the width of the prompt peak for this PMT in the off-axis run. [ns]
    void SetPromptPeakWidth( const Float_t tWidth ){ fPromptPeakWidth = tWidth; }

    // Set the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // off-axis run.
    void SetPromptPeakCounts( const Float_t occ ){ fPromptPeakCounts = occ; }

    // Set the error on the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // off-axis run.
    void SetPromptPeakCountsErr( const Float_t occErr ){ fPromptPeakCountsErr = occErr; }

    // Set the time of flight from the laserball to the PMT
    // from the off-axis run. [ns]
    void SetTimeOfFlight( const Float_t tOF ){ fTimeOfFlight = tOF; }

    // Set the laserball intensity normalisation value for the 
    // off-axis run associated with this PMT.
    void SetLBIntensityNorm( const Float_t intensityNorm ){ fLBIntensityNorm = intensityNorm; }

    // Set the number of laserball pulses in the off-axis
    // run associated with this PMT.
    void SetNLBPulses( const Float_t nPulses ){ fNLBPulses = nPulses; }

    // Set the Multi-Photoelectron corrected occupancy for this PMT
    // for the off-axis run.
    void SetMPECorrOccupancy( const Float_t mpeCorr ){ fMPECorrOccupancy = mpeCorr; }

    // Set the error on the Multi-Photoelectron corrected occupancy 
    // for this PMT for the off-axis run.
    void SetMPECorrOccupancyErr( const Float_t mpeCorrErr ){ fMPECorrOccupancyErr = mpeCorrErr; }
    
    // Set the Fresnel transmission coefficient for the light path
    // associated with this PMT from the off-axis run.
    void SetFresnelTCoeff( const Float_t fresnelCoeff ){ fFresnelTCoeff = fresnelCoeff; }
    
    // Set the distance in the inner AV region associated with the
    // light path associated with this PMT from the off-axis run.
    void SetDistInInnerAV( const Float_t distInScint ){ fDistInInnerAV = distInScint; }

    // Set the distance in the AV region associated with the
    // light path associated with this PMT from the off-axis run.
    void SetDistInAV( const Float_t distInAV ){ fDistInAV = distInAV; }

    // Set the distance in the water region associated with the
    // light path associated with this PMT from the off-axis run.
    void SetDistInWater( const Float_t distInWater ){ fDistInWater = distInWater; }

    // Set the value of the solid angle subtended by the PMT from the
    // laserball position in the off-axis run.
    void SetSolidAngle( const Float_t solidA ){ fSolidAngle = solidA; }

    // Set the value of the cosine of the incident angle of the
    // light path incident on this PMT from the laserball from the
    // off-axis run.
    void SetCosTheta( const Float_t cosTheta ){ fCosTheta = cosTheta; }
    
    // Set the value the theta value of the light path from the laserball
    // in the off-axis run relative to the laserball axis.
    void SetRelLBTheta( const Float_t theta ){ fRelLBTheta = theta; }

    // Set the value the phi value of the light path from the laserball
    // in the off-axis run relative to the laserball axis.
    void SetRelLBPhi( const Float_t phi ){ fRelLBPhi = phi; }

    // Set the CHS flag for this PMT from the off-axis run.
    // Note: Same as DQXX for now. Will deprecate one depending
    // on best choice of name.
    void SetCHSFlag( const Bool_t boolVal ){ fCHSFlag = boolVal; }

    // Set the CSS flag for this PMT from the off-axis run.
    // Note: Same as ANXX for now. Will deprecate one depending
    // on best choice of name.
    void SetCSSFlag( const Bool_t boolVal ){ fCSSFlag = boolVal; }

    // Set the flag associated with the state of the light path
    // associated with this PMT from the off-axis run.
    void SetBadPath( const Bool_t badPath ){ fBadPath = badPath; }

    // Set the flag associated with whether the light path went 
    // thorugh the neck or not to reach this PMT from the
    // off-axis run.
    void SetNeckFlag( const Bool_t neckFlag ){ fNeckFlag = neckFlag; }

    // Set the initial photon vector associated with the lightpath
    // which reached this PMT in the off-axis run.
    void SetInitialLBVec( const TVector3 vec ){ fInitialLBVec = vec; }

    // Set the incident photon vector associated with the lightpath
    // which reached this PMT in the off-axis run.
    void SetIncidentLBVec( const TVector3 vec ){ fIncidentLBVec = vec; }

    /////////////////////////////////////////
    ////////     CENTRAL SETTERS     ////////
    /////////////////////////////////////////

    // Set the prompt peak time for this PMT in the central run. [ns]
    void SetCentralPromptPeakTime( const  Float_t tPeak ){ fCentralPromptPeakTime = tPeak; }

    // Set the width of the prompt peak for this PMT in the central run. [ns]
    void SetCentralPromptPeakWidth( const  Float_t tWidth ){ fCentralPromptPeakWidth = tWidth; }  

     // Set the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // central run.
    void SetCentralPromptPeakCounts( const  Float_t occ ){ fCentralPromptPeakCounts = occ; }
    
    // Set the error on the number of counts in the prompt peak timing window
    // (window is 4ns either side of prompt peak time mean) from the
    // central run.
    void SetCentralPromptPeakCountsErr( const  Float_t occErr ){ fCentralPromptPeakCountsErr = occErr; }

   // Set the time of flight from the laserball to the PMT
    // from the central run. [ns]
    void SetCentralTimeOfFlight( const  Float_t tOF ){ fCentralTimeOfFlight = tOF; }

    // Set the laserball intensity normalisation value for the 
    // central run associated with this PMT.
    void SetCentralLBIntensityNorm( const  Float_t intensityNorm ){ fCentralLBIntensityNorm = intensityNorm; }

    // Set the number of laserball pulses in the central
    // run associated with this PMT.
    void SetCentralNLBPulses( const  Float_t nPulses ){ fCentralNLBPulses = nPulses; }

    // Set the Multi-Photoelectron corrected occupancy for this PMT
    // for the central run.
    void SetCentralMPECorrOccupancy( const  Float_t mpeCorr ){ fCentralMPECorrOccupancy = mpeCorr; }

    // Set the error on the Multi-Photoelectron corrected occupancy 
    // for this PMT for the central run.
    void SetCentralMPECorrOccupancyErr( const  Float_t mpeCorrErr ){ fCentralMPECorrOccupancyErr = mpeCorrErr; }
    
    // Set the Fresnel transmission coefficient for the light path
    // associated with this PMT from the central run.
    void SetCentralFresnelTCoeff( const  Float_t fresnelCoeff ){ fCentralFresnelTCoeff = fresnelCoeff; }
    
    // Set the distance in the inner AV region associated with the
    // light path associated with this PMT from the central run.
    void SetCentralDistInInnerAV( const  Float_t distInScint ){ fCentralDistInInnerAV = distInScint; }

    // Set the distance in the AV region assocaited with the
    // light path associated with this PMT from the central run.
    void SetCentralDistInAV( const  Float_t distInAV ){ fCentralDistInAV = distInAV; }

    // Set the distance in the water region associated with the
    // light path associated with this PMT from the central run.
    void SetCentralDistInWater( const  Float_t distInWater ){ fCentralDistInWater = distInWater; }
    
    // Set the value of the solid angle subtended by the PMT from the
    // laserball position in the central run.
    void SetCentralSolidAngle( const  Float_t solidA ){ fCentralSolidAngle = solidA; }

    // Set the value of the cosine of the incident angle of the
    // light path incident on this PMT from the laserball from the
    // central run.
    void SetCentralCosTheta( const  Float_t cosTheta ){ fCentralCosTheta = cosTheta; }
    
    // Set the value the theta value of the light path from the laserball
    // in the central run relative to the laserball axis.
    void SetCentralRelLBTheta( const  Float_t theta ){ fCentralRelLBTheta = theta; }

    // Set the value the phi value of the light path from the laserball
    // in the central run relative to the laserball axis.
    void SetCentralRelLBPhi( const  Float_t phi ){ fCentralRelLBPhi = phi; }

    // Set the CHS flag for this PMT from the central run.
    // Note: Same as DQXX for now. Will deprecate one depending
    // on best choice of name.
    void SetCentralCHSFlag( const Bool_t boolVal ){ fCentralCHSFlag = boolVal; }

    // Set the CSS flag for this PMT from the central run.
    // Note: Same as ANXX for now. Will deprecate one depending
    // on best choice of name.
    void SetCentralCSSFlag( const Bool_t boolVal ){ fCentralCSSFlag = boolVal; }
    
    // Set the flag associated with the state of the light path
    // associated with this PMT from the central run.
    void SetCentralBadPath( const  Bool_t badPath ){ fCentralBadPath = badPath; }

    // Set the flag associated with whether the light path went 
    // thorugh the neck or not to reach this PMT from the
    // central run.
    void SetCentralNeckFlag( const  Bool_t neckFlag ){ fCentralNeckFlag = neckFlag; }

    // Set the initial photon vector associated with the lightpath
    // which reached this PMT in the central run.
    void SetCentralInitialLBVec( const TVector3 vec ){ fCentralInitialLBVec = vec; }

    // Set the incident photon vector associated with the lightpath
    // which reached this PMT in the central run.
    void SetCentralIncidentLBVec( const TVector3 vec ){ fCentralIncidentLBVec = vec; }

    //////////////////////////////////////////////////////
    ////////     SETTERS FOR FITTING ROUTINES     ////////
    //////////////////////////////////////////////////////

    // Set the occupancy ratio for this PMT
    // Occupancy ratio is MPECorrOccupancy / CentralMPECorrOccupancy
    void SetOccupancyRatio( const Float_t occRatio ){ fOccupancyRatio = occRatio; }

    // Set the model predicted occupancy ratio for this PMT
    void SetModelOccupancyRatio( const Float_t occRatio ){ fModelOccupancyRatio = occRatio; }

    // Set the chi-square residual for this PMT
    void SetChiSquareResidual( const Float_t resVal ){ fChiSquareResidual = resVal; }

    // Set the error on the occupancy ratio for this PMT
    void SetOccupancyRatioErr( const Float_t occRatioErr ){ fOccupancyRatioErr = occRatioErr; }

    // Set the model predicted occupancy for this PMT
    void SetModelOccupancy( const Float_t modelOcc ) { fModelOccupancy = modelOcc; }

    // Set the systematic PMT variability error for this PMT.
    void SetPMTVariability( const Float_t pmtVar ){ fPMTVariability = pmtVar; }

    // Set the index of the run as this PMT appeared in the fit.
    void SetRunIndex( const Int_t runIndex ) { fRunIndex = runIndex; }

    // Set the relative efficiency estimator of the PMT as it appeared in the run in the fit.
    void SetRunEfficiency( const Float_t rawEff ) { fRunEfficiency = rawEff; }

    // Set the relative efficiency estimator of the PMT normalised to all run instances
    // of this PMT in an entire scan.
    void SetScanEfficiency( const Float_t scanEff ){ fScanEfficiency = scanEff; }
    
  private:
    
    Int_t fID;                                    // PMT ID/LCN

    Int_t fRunID;                                 // The Run ID
    Int_t fCentralRunID;                          // The Central Run ID
    Int_t fWavelengthRunID;                       // The Wavelength Run ID

    Int_t fType;                                  // The PMT type

    // The following PMT types are defined in 'rat/data/pmt/airfill2.ratdb' found in RAT.

    // 1: Normal
    // 2: OWL
    // 3: Low Gain
    // 4: BUTT
    // 5: Neck
    // 6: Calib Channel
    // 10: Spare
    // 11: Invalid

    Bool_t fIsVerified;                           // TRUE: PMT has sensible values FALSE: Some bad values
                                                  // See OCAPMT::VerifyPMT for details (OCAPMT.cc)
    Bool_t fCentralIsVerified;
    
    TVector3 fPos;                                // PMT Position
    TVector3 fNorm;                               // PMT Orientation
    TVector3 fLBPos;                              // Laserball position.
    TVector3 fCentralLBPos;                       // Laserball position from the central run.
    TVector3 fWavelengthLBPos;                    // Laserball position from the wavelength run.
    Float_t  fLBOrientation;                      // Laserball orientation.
    Float_t  fCentralLBOrientation;               // Laserball orientation from the central run.
    Float_t  fWavelengthLBOrientation;            // Laserball orientation from the wavelength run.
    
    Float_t fPromptPeakTime;                      // Time of Prompt Peak [ns] 
    Float_t fPromptPeakWidth;                     // Width of Prompt Peak [ns]

    Float_t fPromptPeakCounts;                    // Prompt Peak Counts 
    Float_t fPromptPeakCountsErr;                 // Error on Occupancy of this PMT

    Float_t fTimeOfFlight;                        // Time of Flight from source to PMT [ns]

    Float_t fLBIntensityNorm;                     // The prompt peak normalisation for the entire run
    Float_t fNLBPulses;                           // Number of LaserBall pulses for this run.

    Float_t fMPECorrOccupancy;                    // MPE corrected occupancy
    Float_t fMPECorrOccupancyErr;                 // Error on the MPE corrected occupancy
    
    Float_t fFresnelTCoeff;                       // Acrylic Fresnel transmission coefficient
    
    Float_t fDistInInnerAV;                       // Distance through the scintillator region [mm]
    Float_t fDistInAV;                            // Distance through the acrylic region [mm]
    Float_t fDistInWater;                         // Distance through the water region [mm]
    
    Float_t fSolidAngle;                          // Solid Angle subtended by this PMT from source (LaserBall) position
    Float_t fCosTheta;                            // Cosine of light vector incident on the the PMT face

    Float_t fRelLBTheta;                          // LaserBall Theta value for this PMT from the Laserball position
    Float_t fRelLBPhi;                            // LaserBall Phi value for this PMT from the LaserBall position
    
    Bool_t fCHSFlag;                              // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel
    Bool_t fCSSFlag;                              // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel

    Bool_t fBadPath;                              // Bad Light Path for this Channel (TRUE)
    Bool_t fNeckFlag;                             // If the Light Path for this PMT enters the neck (TRUE)

    TVector3 fInitialLBVec;                       // Initial (unit) vector direction of light from source
    TVector3 fIncidentLBVec;                      // Incident (unit) vector direction on PMT face

    /////////////////////////////////////
    //////// CENTRAL PMT VALUES /////////
    /////////////////////////////////////

    Float_t fCentralPromptPeakTime;               // Time of Prompt Peak [ns] - From the central run
    Float_t fCentralPromptPeakWidth;              // Width of Prompt Peak [ns] - From the central run

    Float_t fCentralPromptPeakCounts;             // Prompt Peak Counts  - From the central run
    Float_t fCentralPromptPeakCountsErr;          // Error on Occupancy of this PMT - From the central run

    Float_t fCentralTimeOfFlight;                 // Time of Flight from source to PMT [ns] - From the central run

    Float_t fCentralLBIntensityNorm;              // The prompt peak normalisation for the entire run - From the central run
    Float_t fCentralNLBPulses;                    // Number of LaserBall pulses for this run. - From the central run

    Float_t fCentralMPECorrOccupancy;             // MPE corrected occupancy - From the central run
    Float_t fCentralMPECorrOccupancyErr;          // Error on the MPE corrected occupancy - From the central run
    
    Float_t fCentralFresnelTCoeff;                // Acrylic Fresnel transmission coefficient - From the central run
    
    Float_t fCentralDistInInnerAV;                // Distance through the scintillator region [mm] - From the central run
    Float_t fCentralDistInAV;                     // Distance through the acrylic region [mm] - From the central run
    Float_t fCentralDistInWater;                  // Distance through the water region [mm] - From the central run
    
    Float_t fCentralSolidAngle;                   // Solid Angle subtended by this PMT from source (LaserBall) position - From the central run
    Float_t fCentralCosTheta;                     // Cosine of light vector incident on the the PMT face - From the central run

    Float_t fCentralRelLBTheta;                   // LaserBall Theta value for this PMT from the Laserball position - From the central run
    Float_t fCentralRelLBPhi;                     // LaserBall Phi value for this PMT from the LaserBall position - From the central run
    
    Bool_t fCentralCHSFlag;                       // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel - From the central run
    Bool_t fCentralCSSFlag;                       // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel - From the central run

    Bool_t fCentralBadPath;                       // Bad Light Path for this Channel (TRUE) - From the central run
    Bool_t fCentralNeckFlag;                      // If the Light Path for this PMT enters the neck (TRUE) - From the central run

    TVector3 fCentralInitialLBVec;                // Initial (unit) vector direction of light from source - From the central run
    TVector3 fCentralIncidentLBVec;               // Incident (unit) vector direction on PMT face - From the central run

    ///////////////////////////////////////////
    //////// VALUES FOR FITTING ROUTINES //////
    ///////////////////////////////////////////

    Float_t fOccupancyRatio;                       // Occupancy ratio of fMPECorrOccupancy / fCentralMPECorrOccupancy
    Float_t fModelOccupancyRatio;                  // Model Occupancy ratio
    Float_t fOccupancyRatioErr;                    // Error on the (data) occupancy ratio
    Float_t fChiSquareResidual;                    // The ChiSquare residual value

    Float_t fModelOccupancy;                       // Model predicted occupancy.
    
    Float_t fPMTVariability;                       // Systematic PMT variability error.

    Int_t fRunIndex;                               // The index of the run as it appeared in the fit.

    Float_t fRunEfficiency;                        // The relative efficiency estimator of the PMT as it appeared in the run in the fit.

    Float_t fScanEfficiency;                       // The relative efficiency estimator of the PMT normalised to all run instances
                                                   // of this PMT in an entire scan.
    
    ClassDef( OCAPMT, 1 );
    
  };

}

#endif
