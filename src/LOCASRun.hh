////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRun.hh
///
/// CLASS: LOCAS::LOCASRun
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This data structure contains all the information
///         for a specific laserball run required as an input
///         into an optics fit/analysis using LOCAS.
///
///         Herein lies information specific to the run itself,
///         such as the source (laserball) position, the Run ID,
///         and the number of PMTs (LOCASPMT objects) etc.
///         Individual LOCASPMTs store the relevant information
///         specific to that PMT. The LOCASPMT objects for the run
///         are stored in a std::map< Int_t, LOCASPMT > object here
///         and are indexed by the PMT ID.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASRUN_
#define _LOCASRUN_

#include <map>

#include <TVector3.h>
#include <TObject.h>
#include <TClonesArray.h>

#include "RAT/DS/SOC.hh"
#include "RAT/SOCReader.hh"

#include "LOCASPMT.hh"

namespace LOCAS{

  class LOCASRun : public TObject
  {
  public:
    LOCASRun();
    virtual ~LOCASRun();
    
    LOCASRun( const LOCASRun &rhs );
    LOCASRun& operator=( const LOCASRun &rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    void Initialise();
    void Clear( Option_t* option="" );
    void Fill( RAT::SOCReader& socR, Int_t runID );
    
    void CopySOCRunInfo( RAT::DS::SOC* socRun );
    void CopySOCPMTInfo( RAT::DS::SOC* socRun );
    
    void CopyLOCASRunInfo( LOCASRun& locasRun );
    void CopyLOCASPMTInfo( LOCASRun& locasPMT );
    
    void AddSOCPMT( RAT::DS::SOCPMT& socPMT );
    void AddLOCASPMT( LOCASPMT& locasPMT );
    
    void RemovePMT( Int_t iPMT );
    LOCASPMT& GetPMT( Int_t iPMT );

    void CrossRunFill( LOCASRun& cRun, LOCASRun& wRun );
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetRunID( Int_t runID ){ fRunID = runID; }
    void SetCentralRunID( Int_t runID ){ fCentralRunID = runID; }
    void SetWavelengthRunID( Int_t runID ){ fWavelengthRunID = runID; }

    void SetSourceID( Int_t sourceID ){ fSourceID = sourceID; }
    void SetCentralSourceID( Int_t sourceID ){ fCentralSourceID = sourceID; }
    void SetWavelengthSourceID( Int_t sourceID ){ fWavelengthSourceID = sourceID; }

    void SetIsMainRun( Bool_t isMain ){ fIsMainRun = isMain; }
    void SetIsCentralRun( Bool_t isCentral ){ fIsCentralRun = isCentral; }
    void SetIsWavelengthRun( Bool_t isWavelength ){ fIsWavelengthRun = isWavelength; }
    
    void SetLambda( Float_t lambda ){ fLambda = lambda; } 
    void SetNLBPulses( Float_t nPulses ){ fNLBPulses = nPulses; }
    void SetCentralLambda( Float_t lambda ){ fCentralLambda = lambda; } 
    void SetCentralNLBPulses( Float_t nPulses ){ fCentralNLBPulses = nPulses; }
    void SetWavelengthLambda( Float_t lambda ){ fWavelengthLambda = lambda; } 
    void SetWavelengthNLBPulses( Float_t nPulses ){ fWavelengthNLBPulses = nPulses; }

    void SetMainLBIntensityNorm( Float_t mRunLI ){ fMainLBIntensityNorm = mRunLI; }
    void SetCentralLBIntensityNorm( Float_t cRunLI ){ fCentralLBIntensityNorm = cRunLI; }
    void SetWavelengthLBIntensityNorm( Float_t wRunLI ){ fWavelengthLBIntensityNorm = wRunLI; }
    
    void SetTimeSigmaMean( Float_t sigmean ){ fTimeSigmaMean = sigmean; }
    void SetTimeSigmaSigma( Float_t sigsigma ){ fTimeSigmaSigma = sigsigma; }
    
    // ABREVIATIONS
    // LB := LaserBall
       
    ////////////////////////////////////////
    
    void SetLBPos( Float_t xPos, Float_t yPos, Float_t zPos ){ 
      fLBPos.SetX( xPos );
      fLBPos.SetY( yPos );
      fLBPos.SetZ( zPos );
    }
    void SetLBPos( TVector3 xyzPos ){ 
      fLBPos = xyzPos;
    }

    void SetCentralLBPos( Float_t xPos, Float_t yPos, Float_t zPos ){ 
      fCentralLBPos.SetX( xPos );
      fCentralLBPos.SetY( yPos );
      fCentralLBPos.SetZ( zPos );
    }
    void SetCentralLBPos( TVector3 xyzPos ){ 
      fCentralLBPos = xyzPos;
    }

    void SetWavelengthLBPos( Float_t xPos, Float_t yPos, Float_t zPos ){ 
      fWavelengthLBPos.SetX( xPos );
      fWavelengthLBPos.SetY( yPos );
      fWavelengthLBPos.SetZ( zPos );
    }
    void SetWavelengthLBPos( TVector3 xyzPos ){ 
      fWavelengthLBPos = xyzPos;
    }
    
    void SetLBXPosErr( Float_t LBXPosErr ){ fLBXPosErr = LBXPosErr; }
    void SetLBYPosErr( Float_t LBYPosErr ){ fLBYPosErr = LBYPosErr; }
    void SetLBZPosErr( Float_t LBZPosErr ){ fLBZPosErr = LBZPosErr; }

    void SetCentralLBXPosErr( Float_t LBXPosErr ){ fCentralLBXPosErr = LBXPosErr; }
    void SetCentralLBYPosErr( Float_t LBYPosErr ){ fCentralLBYPosErr = LBYPosErr; }
    void SetCentralLBZPosErr( Float_t LBZPosErr ){ fCentralLBZPosErr = LBZPosErr; }

    void SetWavelengthLBXPosErr( Float_t LBXPosErr ){ fWavelengthLBXPosErr = LBXPosErr; }
    void SetWavelengthLBYPosErr( Float_t LBYPosErr ){ fWavelengthLBYPosErr = LBYPosErr; }
    void SetWavelengthLBZPosErr( Float_t LBZPosErr ){ fWavelengthLBZPosErr = LBZPosErr; }
    
    void SetLBPosChi2( Float_t LBPosChi2 ){ fLBPosChi2 = LBPosChi2; }
    void SetLBTheta( Float_t LBTheta ){ fLBTheta = LBTheta; }
    void SetLBPhi( Float_t LBPhi ){ fLBPhi = LBPhi; }

    void SetCentralLBPosChi2( Float_t LBPosChi2 ){ fCentralLBPosChi2 = LBPosChi2; }
    void SetCentralLBTheta( Float_t LBTheta ){ fCentralLBTheta = LBTheta; }
    void SetCentralLBPhi( Float_t LBPhi ){ fCentralLBPhi = LBPhi; }

    void SetWavelengthLBPosChi2( Float_t LBPosChi2 ){ fWavelengthLBPosChi2 = LBPosChi2; }
    void SetWavelengthLBTheta( Float_t LBTheta ){ fWavelengthLBTheta = LBTheta; }
    void SetWavelengthLBPhi( Float_t LBPhi ){ fWavelengthLBPhi = LBPhi; }
    
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Int_t GetRunID() const { return fRunID; }
    Int_t GetCentralRunID() const { return fCentralRunID; }
    Int_t GetWavelengthRunID() const { return fWavelengthRunID; }

    Int_t GetSourceID() const { return fSourceID; }
    Int_t GetCentralSourceID() const { return fCentralSourceID; }
    Int_t GetWavelengthSourceID() const { return fWavelengthSourceID; }

    Bool_t GetIsMainRun() const { return fIsMainRun; }
    Bool_t GetIsCentralRun() const { return fIsCentralRun; }
    Bool_t GetIsWavelengthRun() const { return fIsWavelengthRun; }
    
    Int_t GetNPMTs() { return fLOCASPMTs.size(); }
    
    Float_t GetLambda() const { return fLambda; } 
    Float_t GetNLBPulses() const { return fNLBPulses; }
    Float_t GetCentralLambda() const { return fCentralLambda; } 
    Float_t GetCentralNLBPulses() const { return fCentralNLBPulses; }
    Float_t GetWavelengthLambda() const { return fWavelengthLambda; } 
    Float_t GetWavelengthNLBPulses() const { return fWavelengthNLBPulses; }

    Float_t GetMainLBIntensityNorm() const { return fMainLBIntensityNorm; }  
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }  
    Float_t GetWavelengthLBIntensityNorm() const { return fWavelengthLBIntensityNorm; }  
    
    Float_t GetTimeSigmaMean() const { return fTimeSigmaMean; }
    Float_t GetTimeSigmaSigma() const { return fTimeSigmaSigma; }
    
    ////////////////////////////////////////
    
    TVector3 GetLBPos() const { return fLBPos; } 
    TVector3 GetCentralLBPos() const { return fCentralLBPos; } 
    TVector3 GetWavelengthLBPos() const { return fWavelengthLBPos; } 
    
    Float_t GetLBXPosErr() const { return fLBXPosErr; }
    Float_t GetLBYPosErr() const { return fLBYPosErr; }
    Float_t GetLBZPosErr() const { return fLBZPosErr; }

    Float_t GetCentralLBXPosErr() const { return fCentralLBXPosErr; }
    Float_t GetCentralLBYPosErr() const { return fCentralLBYPosErr; }
    Float_t GetCentralLBZPosErr() const { return fCentralLBZPosErr; }

    Float_t GetWavelengthLBXPosErr() const { return fWavelengthLBXPosErr; }
    Float_t GetWavelengthLBYPosErr() const { return fWavelengthLBYPosErr; }
    Float_t GetWavelengthLBZPosErr() const { return fWavelengthLBZPosErr; }
    
    Float_t GetLBPosChi2() const { return fLBPosChi2; }
    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetLBPhi() const { return fLBPhi; }

    Float_t GetCentralLBPosChi2() const { return fCentralLBPosChi2; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetWavelengthLBPosChi2() const { return fWavelengthLBPosChi2; }
    Float_t GetWavelengthLBTheta() const { return fWavelengthLBTheta; }
    Float_t GetWavelengthLBPhi() const { return fWavelengthLBPhi; }
       
    std::map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterBegin() { return fLOCASPMTs.begin(); }
    std::map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterEnd() { return fLOCASPMTs.end(); }
    
  private:
    
    Int_t fRunID;                          // The Run ID
    Int_t fCentralRunID;                   // The Central Run ID
    Int_t fWavelengthRunID;                // The Wavelength Run ID

    Int_t fSourceID;                       // The Source ID
    Int_t fCentralSourceID;                // The Source ID
    Int_t fWavelengthSourceID;             // The Source ID
    
    Bool_t fIsMainRun;                     // TRUE: Main Run False: Other
    Bool_t fIsCentralRun;                  // TRUE: Central Run FALSE: Other Run
    Bool_t fIsWavelengthRun;               // TRUE: Wavelength Run FALSE: Other Run
    
    Float_t fLambda;                       // The wavelength of the laser in this run
    Float_t fNLBPulses;                    // Number of laserball pulses in this run
    Float_t fCentralLambda;                // The wavelength of the laser in the central run
    Float_t fCentralNLBPulses;             // Number of laserball pulses in the central run
    Float_t fWavelengthLambda;             // The wavelength of the laser in the wavelength run
    Float_t fWavelengthNLBPulses;          // Number of laserball pulses in the wavelength run

    Float_t fMainLBIntensityNorm;          // Number of total hits in prompt timing window (for main run)
    Float_t fCentralLBIntensityNorm;       // Number of total hits in prompt timing window (for central run)
    Float_t fWavelengthLBIntensityNorm;    // Number of total hits in prompt timing window (for wavelength run)
    
    Float_t fTimeSigmaMean;                // Average PMT time spread for this run
    Float_t fTimeSigmaSigma;               // Spread of PMT time spreads for this run (i.e. standard deviation)
    
    TVector3 fLBPos;                       // The Laserball position used in the LOCAS fit
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
    
    Float_t fLBPosChi2;                    // Chi-squared of the laserball position
    Float_t fLBTheta;                      // Theta orientation value ( usually fLBTheta=0 )
    Float_t fLBPhi;                        // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fCentralLBPosChi2;             // Chi-squared of the laserball position
    Float_t fCentralLBTheta;               // Theta orientation value ( usually fLBTheta=0 )
    Float_t fCentralLBPhi;                 // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fWavelengthLBPosChi2;          // Chi-squared of the laserball position in the wavelength run
    Float_t fWavelengthLBTheta;            // Theta orientation value ( usually fLBTheta=0 ) in the wavelength run
    Float_t fWavelengthLBPhi;              // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East ) in the wavelength run
        
    std::map<Int_t, LOCASPMT> fLOCASPMTs;  // Map of LOCASPMTs (i.e. the PMTs in this Run)
    
    ClassDef(LOCASRun,1)
    
  };
  
}

#endif
