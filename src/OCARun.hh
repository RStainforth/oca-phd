////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCARun.hh
///
/// CLASS: OCA::OCARun
///
/// BRIEF: Run level data structure for OCA optics analysis
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This data structure contains all the information
///         for a specific laserball run required as an input
///         into an optics fit/analysis using OCA.
///
///         Herein lies information specific to the run itself,
///         such as the source (laserball) position, the Run ID,
///         the number of PMTs (OCAPMT objects), and the orientation
///         of the laserball in it's local frame etc.
///         Individual OCAPMTs store the relevant information
///         specific to that PMT. The OCAPMT objects for the run
///         are stored in a std::map< Int_t, OCAPMT > object here
///         and are indexed by the PMT ID.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCARUN_
#define _OCARUN_

#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"
#include "RAT/DU/ChanHWStatus.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DU/SOCReader.hh"

#include "OCAPMT.hh"

#include "TVector3.h"
#include "TObject.h"

#include <map>
#include <string>

using namespace std;

namespace OCA{

  class OCARun : public TObject
  {
  public:

    // The constructors and destructor for the OCARun object.
    OCARun(){
      ClearRun();
      fOCAPMTs.clear();
    }
    OCARun( const OCARun &rhs );
    ~OCARun(){ }
    
    // The equality operator.
    OCARun& operator=( const OCARun &rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    // Initialise and clear all the private member variables
    // to non-physical/interpretive values before assigning them
    // using OCARun::Fill.
    void ClearRun();

    // Fill Run information from a SOC file with run ID 'runID'
    // into the run information stored here (OCARun object).
    void FillRunInfo( RAT::DS::SOC* socPtr,
                      UInt_t runID, Int_t lbPosMode, 
                      Bool_t copyPMTInfo = true );

    // Fill PMT information from a SOC file with run ID 'runID'
    // into the PMT information stored here (OCAPMT objects).
    void FillPMTInfo( RAT::DU::LightPathCalculator& lLP,
                      RAT::DU::ShadowingCalculator& lSC,
                      RAT::DU::ChanHWStatus& lCHS,
                      RAT::DU::PMTInfo& lDB,
                      UInt_t runID );
    
    // Copy the SOCRun information from a SOC object
    // into the run information here.
    void CopySOCRunInfo( RAT::DS::SOC& socRun );

    // Copy the SOCPMT information from a SOC object
    // into the OCAPMT object stored here.
    void CopySOCPMTInfo( RAT::DS::SOC& socRun );
    
    // Copy the OCARun information from a OCARun object
    // into the OCARun information here.
    void CopyOCARunInfo( OCARun& ocaRun );

    // Copy the PMT information from a OCARun object
    // into the PMT information held here.
    void CopyOCAPMTInfo( OCARun& ocaRun );
    
    // Add a single SOCPMT object to the OCAPMTs held by this
    // OCARun object.
    void AddSOCPMT( RAT::DS::SOCPMT& socPMT );

    // Add a pre-existing OCAPMT object to the ones held here.
    void AddOCAPMT( OCAPMT& ocaPMT );
    
    // Remove a PMT by the PMT ID.
    void RemovePMT( const Int_t iPMT );

    // Get a PMT by PMT ID.
    OCAPMT& GetPMT( const Int_t iPMT );

    // Fill information about a central run (cRun) and a
    // wavelength run (wRun) (if specified) into this OCARun
    // object
    void CrossRunFill( OCARun* cRun );

    // Calculate the number of prompt counts detected at each PMT
    // within the run. This must be called after CopySOCPMTInfo()
    void CalculateLBIntensityNorm();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the number of PMT stored in this run.
    Int_t GetNPMTs() { return fOCAPMTs.size(); }
    
    // Get the run ID for the off-axis run associated with this object.
    Int_t GetRunID() const { return fRunID; }

    // Get the run ID for the central run associated with this object.
    Int_t GetCentralRunID() const { return fCentralRunID; }

    // Get the run ID for the wavelength run associated with this object.
    Int_t GetWavelengthRunID() const { return fWavelengthRunID; }
    
    // Get the source ID from the off-axis run.
    string GetSourceID() const { return fSourceID; }
    
    // Get the source ID from the central run.
    string GetCentralSourceID() const { return fCentralSourceID; }

    // Get the source ID from the wavelength run.    
    string GetWavelengthSourceID() const { return fWavelengthSourceID; }

    // Get whether this OCARun object represents an off-axis run.
    Bool_t GetIsMainRun() const { return fIsMainRun; }
    
    // Get whether this LCOASRun object represents a central run.
    Bool_t GetIsCentralRun() const { return fIsCentralRun; }
    
    // Get whether this LOCAsRun object represents a wavelength run.
    Bool_t GetIsWavelengthRun() const { return fIsWavelengthRun; }

    // Get the wavelength of the laser (nm) from the off-axis run.
    Float_t GetLambda() const { return fLambda; } 
    
    // Get the number of pulses from the laser from the off-axis run.
    Float_t GetNLBPulses() const { return fNLBPulses; }

    // Get the number of photons per pulse from the off-axis run.
    Float_t GetIntensity() const { return fIntensity; }

    // Get the global time off-set from the off-axis run.
    Float_t GetGlobalTimeOffset() const { return fGlobalTimeOffset; }
    
    // Get the wavelength of the laser (nm) from the central run.
    Float_t GetCentralLambda() const { return fCentralLambda; }

    // Get the number of pulses from the laser from the central run. 
    Float_t GetCentralNLBPulses() const { return fCentralNLBPulses; }

    // Get the number of photons per pulse from the central run.
    Float_t GetCentralIntensity() const { return fCentralIntensity; }

    // Get the global time off-set from the central run.
    Float_t GetCentralGlobalTimeOffset() const { return fCentralGlobalTimeOffset; }

    // Get the wavelength of the laser (nm) from the wavelength run.
    Float_t GetWavelengthLambda() const { return fWavelengthLambda; }

    // Get the number of pulses from the laser from the wavelength run.
    Float_t GetWavelengthNLBPulses() const { return fWavelengthNLBPulses; }

    // Get the number of photons per pulse from the wavelength run.
    Float_t GetWavelengthIntensity() const { return fWavelengthIntensity; }

    // Get the global time off-set from the wavelength run.
    Float_t GetWavelengthGlobalTimeOffset() const { return fWavelengthGlobalTimeOffset; }

    // Get the laserball intensity normalisation value from the off-axis run.
    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }

    // Get the laserball intensity normalisation value from the central run.
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }
 
    // Get the laserball intensity normalisation value from the wavelength run.
    Float_t GetWavelengthLBIntensityNorm() const { return fWavelengthLBIntensityNorm; }  
    
    // Get the laserball position from the off-axis run (mm).
    TVector3 GetLBPos() const { return fLBPos; } 

    // Get the laserball orientation from the off-axis run.
    Float_t GetLBOrientation() const { return fLBOrientation; }

    // Get the laserball orientation from the central run.
    Float_t GetCentralLBOrientation() const { return fCentralLBOrientation; } 

    // Get the laserball orientation from the wavelength run.
    Float_t GetWavelengthLBOrientation() const { return fWavelengthLBOrientation; }  

    // Get the laserball position from the central run (mm).
    TVector3 GetCentralLBPos() const { return fCentralLBPos; } 

    // Get the laserball position from the wavelength run (mm).
    TVector3 GetWavelengthLBPos() const { return fWavelengthLBPos; } 
    
    // Get the error in the laserball x-position from the off-axis run (mm).
    Float_t GetLBXPosErr() const { return fLBXPosErr; }

    // Get the error in the laserball y-position from the off-axis run (mm).
    Float_t GetLBYPosErr() const { return fLBYPosErr; }

    // Get the error in the laserball z-position from the off-axis run (mm).
    Float_t GetLBZPosErr() const { return fLBZPosErr; }

    // Get the error in the laserball x-position from the central run (mm).
    Float_t GetCentralLBXPosErr() const { return fCentralLBXPosErr; }

    // Get the error in the laserball y-position from the central run (mm).
    Float_t GetCentralLBYPosErr() const { return fCentralLBYPosErr; }

    // Get the error in the laserball z-position from the central run (mm).
    Float_t GetCentralLBZPosErr() const { return fCentralLBZPosErr; }

    // Get the error in the laserball x-position from the wavelength run (mm).
    Float_t GetWavelengthLBXPosErr() const { return fWavelengthLBXPosErr; }

    // Get the error in the laserball y-position from the wavelength run (mm).
    Float_t GetWavelengthLBYPosErr() const { return fWavelengthLBYPosErr; }

    // Get the error in the laserball z-position from the wavelength run (mm).
    Float_t GetWavelengthLBZPosErr() const { return fWavelengthLBZPosErr; }
    
    // Get the theta coordinate of the laserball orientation
    // from the off-axis run.
    Float_t GetLBTheta() const { return fLBTheta; }

    // Get the phi coordinate of the laserball orientation
    // from the off-axis run.
    Float_t GetLBPhi() const { return fLBPhi; }

    // Get the theta coordinate of the laserball orientation
    // from the central run.
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    // Get the phi coordinate of the laserball orientation
    // from the central run.
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    // Get the theta coordinate of the laserball orientation
    // from the wavelength run.
    Float_t GetWavelengthLBTheta() const { return fWavelengthLBTheta; }

    // Get the phi coordinate of the laserball orientation
    // from the wavelength run.
    Float_t GetWavelengthLBPhi() const { return fWavelengthLBPhi; }
    
    // Return the iterators corresponding to the beginning 
    // and end of the PMT map.
    map<Int_t, OCAPMT>::iterator GetOCAPMTIterBegin() { return fOCAPMTs.begin(); }
    map<Int_t, OCAPMT>::iterator GetOCAPMTIterEnd() { return fOCAPMTs.end(); }
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    // Set the run ID for the off-axis run associated with this object.
    void SetRunID( const Int_t runID ){ fRunID = runID; }

    // Set the run ID for the central run associated with this object.
    void SetCentralRunID( const Int_t runID ){ fCentralRunID = runID; }

    // Set the run ID for the wavelength run associated with this object.
    void SetWavelengthRunID( const Int_t runID ){ fWavelengthRunID = runID; }

    // Set the source ID from the off-axis run.
    void SetSourceID( const string sourceID ){ fSourceID = sourceID; }

    // Set the source ID from the central run.
    void SetCentralSourceID( const string sourceID ){ fCentralSourceID = sourceID; }

    // Set the source ID from the wavelength run.
    void SetWavelengthSourceID( const string sourceID ){ fWavelengthSourceID = sourceID; }

    // Set whether this OCARun object represents an off-axis run. 
    void SetIsMainRun( const Bool_t isMain ){ fIsMainRun = isMain; }

    // Set whether this LCOASRun object represents a central run.
    void SetIsCentralRun( const Bool_t isCentral ){ fIsCentralRun = isCentral; }

    // Set whether this LOCAsRun object represents a wavelength run.
    void SetIsWavelengthRun( const Bool_t isWavelength ){ fIsWavelengthRun = isWavelength; }    

    // Set the wavelength of the laser (nm) from the off-axis run.
    void SetLambda( const Float_t lambda ){ fLambda = lambda; } 

    // Set the number of pulses from the laser from the off-axis run.
    void SetNLBPulses( const Float_t nPulses ){ fNLBPulses = nPulses; }

    // Set the number of photons per pulse from the off-axis run.
    void SetIntensity( const Float_t intensity ){ fIntensity = intensity; }

    // Set the global time off-set from the off-axis run.
    void SetGlobalTimeOffset( const Float_t gtOffset ){ fGlobalTimeOffset = gtOffset; }

    // Set the wavelength of the laser (nm) from the central run.
    void SetCentralLambda( const Float_t lambda ){ fCentralLambda = lambda; }

    // Set the number of pulses from the laser from the central run.
    void SetCentralNLBPulses( const Float_t nPulses ){ fCentralNLBPulses = nPulses; }

    // Set the number of photons per pulse from the central run.
    void SetCentralIntensity( const Float_t intensity ){ fCentralIntensity = intensity; }

    // Set the global time off-set from the central run.
    void SetCentralGlobalTimeOffset( const Float_t gtOffset ){ fCentralGlobalTimeOffset = gtOffset; }

    // Set the wavelength of the laser (nm) from the wavelength run.
    void SetWavelengthLambda( const Float_t lambda ){ fWavelengthLambda = lambda; } 

    // Set the number of pulses from the laser from the wavelength run.
    void SetWavelengthNLBPulses( const Float_t nPulses ){ fWavelengthNLBPulses = nPulses; }

    // Set the number of photons per pulse from the wavelength run.
    void SetWavelengthIntensity( const Float_t intensity ){ fWavelengthIntensity = intensity; }

    // Set the global time off-set from the wavelength run.
    void SetWavelengthGlobalTimeOffset( const Float_t gtOffset ){ fWavelengthGlobalTimeOffset = gtOffset; }

    // Set the laserball intensity normalisation value from the off-axis run.
    void SetLBIntensityNorm( const Float_t mRunLI ){ fLBIntensityNorm = mRunLI; }

    // Set the laserball intensity normalisation value from the central run.
    void SetCentralLBIntensityNorm( const Float_t cRunLI ){ fCentralLBIntensityNorm = cRunLI; }

    // Set the laserball intensity normalisation value from the wavelength run.
    void SetWavelengthLBIntensityNorm( const Float_t wRunLI ){ fWavelengthLBIntensityNorm = wRunLI; }
    
    ////////////////////////////////////////
    // ABREVIATIONS
    // LB := LaserBall       
    ////////////////////////////////////////
    
    // Set the laserball position from the off-axis run (mm).
    void SetLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fLBPos.SetX( xPos );
      fLBPos.SetY( yPos );
      fLBPos.SetZ( zPos );
    }
    void SetLBPos( const TVector3 xyzPos ){ 
      fLBPos = xyzPos;
    }

    // Set the laserball position from the central run (mm).
    void SetCentralLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fCentralLBPos.SetX( xPos );
      fCentralLBPos.SetY( yPos );
      fCentralLBPos.SetZ( zPos );
    }
    void SetCentralLBPos( const TVector3 xyzPos ){ 
      fCentralLBPos = xyzPos;
    }

    // Set the laserball position from the wavelength run (mm).
    void SetWavelengthLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fWavelengthLBPos.SetX( xPos );
      fWavelengthLBPos.SetY( yPos );
      fWavelengthLBPos.SetZ( zPos );
    }
    void SetWavelengthLBPos( const TVector3 xyzPos ){ 
      fWavelengthLBPos = xyzPos;
    }
    
    // Set the error in the laserball x-position from the off-axis run (mm).
    void SetLBXPosErr( const Float_t LBXPosErr ){ fLBXPosErr = LBXPosErr; }

    // Set the error in the laserball y-position from the off-axis run (mm).
    void SetLBYPosErr( const Float_t LBYPosErr ){ fLBYPosErr = LBYPosErr; }

    // Set the error in the laserball z-position from the off-axis run (mm).
    void SetLBZPosErr( const Float_t LBZPosErr ){ fLBZPosErr = LBZPosErr; }


    // Set the error in the laserball x-position from the central run (mm).
    void SetCentralLBXPosErr( const Float_t LBXPosErr ){ fCentralLBXPosErr = LBXPosErr; }

    // Set the error in the laserball y-position from the central run (mm).
    void SetCentralLBYPosErr( const Float_t LBYPosErr ){ fCentralLBYPosErr = LBYPosErr; }

    // Set the error in the laserball z-position from the central run (mm).
    void SetCentralLBZPosErr( const Float_t LBZPosErr ){ fCentralLBZPosErr = LBZPosErr; }

    // Set the error in the laserball x-position from the wavelength run (mm).
    void SetWavelengthLBXPosErr( const Float_t LBXPosErr ){ fWavelengthLBXPosErr = LBXPosErr; }

    // Set the error in the laserball y-position from the wavelength run (mm).
    void SetWavelengthLBYPosErr( const Float_t LBYPosErr ){ fWavelengthLBYPosErr = LBYPosErr; }

    // Set the error in the laserball z-position from the wavelength run (mm).
    void SetWavelengthLBZPosErr( const Float_t LBZPosErr ){ fWavelengthLBZPosErr = LBZPosErr; }

    // Set the laserball orientation from the off-axis run.
    void SetLBOrientation( const Float_t lbOri ) { fLBOrientation = lbOri; }

    // Set the laserball orientation from the central run.
    void SetCentralLBOrientation( const Float_t lbOri ) { fCentralLBOrientation = lbOri; } 

    // Set the laserball orientation from the wavelength run.
    void SetWavelengthLBOrientation( const Float_t lbOri ) { fWavelengthLBOrientation = lbOri; } 
    
    // Set the theta coordinate of the laserball orientation
    // from the off-axis run.
    void SetLBTheta( const Float_t LBTheta ){ fLBTheta = LBTheta; }

    // Set the phi coordinate of the laserball orientation
    // from the off-axis run.
    void SetLBPhi( const Float_t LBPhi ){ fLBPhi = LBPhi; }

    // Set the theta coordinate of the laserball orientation
    // from the central run.
    void SetCentralLBTheta( const Float_t LBTheta ){ fCentralLBTheta = LBTheta; }

    // Set the phi coordinate of the laserball orientation
    // from the central run.
    void SetCentralLBPhi( const Float_t LBPhi ){ fCentralLBPhi = LBPhi; }

    // Set the theta coordinate of the laserball orientation
    // from the wavelength run.
    void SetWavelengthLBTheta( const Float_t LBTheta ){ fWavelengthLBTheta = LBTheta; }

    // Set the phi coordinate of the laserball orientation
    // from the wavelength run.
    void SetWavelengthLBPhi( const Float_t LBPhi ){ fWavelengthLBPhi = LBPhi; }
    
  private:
    
    Int_t fRunID;                          // The Run ID
    Int_t fCentralRunID;                   // The Central Run ID
    Int_t fWavelengthRunID;                // The Wavelength Run ID

    string fSourceID;                      // The Source ID
    string fCentralSourceID;               // The Central Run Source ID
    string fWavelengthSourceID;            // The Wavelength Run Source ID
    
    Bool_t fIsMainRun;                     // TRUE: Main Run False: Other
    Bool_t fIsCentralRun;                  // TRUE: Central Run FALSE: Other Run
    Bool_t fIsWavelengthRun;               // TRUE: Wavelength Run FALSE: Other Run
    
    Float_t fLambda;                       // The wavelength of the laser in this run
    Float_t fNLBPulses;                    // Number of laserball pulses in this run
    Float_t fIntensity;                    // Number of photons per pulse.      
    Float_t fGlobalTimeOffset;             // Global time offset in this run.

    Float_t fCentralLambda;                // The wavelength of the laser in the central run
    Float_t fCentralNLBPulses;             // Number of laserball pulses in the central run
    Float_t fCentralIntensity;             // Number of photons per pulse in the central run      
    Float_t fCentralGlobalTimeOffset;      // Global time offset in the central run

    Float_t fWavelengthLambda;             // The wavelength of the laser in the wavelength run
    Float_t fWavelengthNLBPulses;          // Number of laserball pulses in the wavelength run
    Float_t fWavelengthIntensity;          // Number of photons per pulse in the wavelength run     
    Float_t fWavelengthGlobalTimeOffset;   // Global time offset in the wavelength run

    Float_t fLBIntensityNorm;              // Number of total hits in prompt timing window (for main run)
    Float_t fCentralLBIntensityNorm;       // Number of total hits in prompt timing window (for central run)
    Float_t fWavelengthLBIntensityNorm;    // Number of total hits in prompt timing window (for wavelength run)
    
    TVector3 fLBPos;                       // The Laserball position used in the OCA fit
    TVector3 fCentralLBPos;                // The Laserball position used in the central run
    TVector3 fWavelengthLBPos;             // The Laserball position used in the wavelength run
    
    Float_t fLBXPosErr;                    // Error on the x laserball coordinate
    Float_t fLBYPosErr;                    // Error on the y laserball coordinate
    Float_t fLBZPosErr;                    // Error on the z laserball coordinate

    Float_t fCentralLBZPosErr;             // Error on the z laserball coordinate in the central run
    Float_t fCentralLBXPosErr;             // Error on the x laserball coordinate in the central run
    Float_t fCentralLBYPosErr;             // Error on the y laserball coordinate in the central run

    Float_t fWavelengthLBZPosErr;          // Error on the z laserball coordinate in the wavelength run
    Float_t fWavelengthLBXPosErr;          // Error on the x laserball coordinate in the wavelength run
    Float_t fWavelengthLBYPosErr;          // Error on the y laserball coordinate in the wavelength run

    Float_t fLBOrientation;                // The orientation of the laserball from the off-axis run.
    Float_t fCentralLBOrientation;         // The orientation of the laserball from the central run.
    Float_t fWavelengthLBOrientation;      // The orientation of the laserball from the wavelength run.
    
    Float_t fLBTheta;                      // Theta orientation value ( usually fLBTheta=0 )
    Float_t fLBPhi;                        // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fCentralLBTheta;               // Theta orientation value ( usually fLBTheta=0 )
    Float_t fCentralLBPhi;                 // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fWavelengthLBTheta;            // Theta orientation value ( usually fLBTheta=0 ) in the wavelength run
    Float_t fWavelengthLBPhi;              // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East ) in the wavelength run
        
    map<Int_t, OCAPMT> fOCAPMTs;       // Map of OCAPMTs (i.e. the PMTs in this (Main) Run)
    
    ClassDef(OCARun,1)
    
  };
  
}

#endif
