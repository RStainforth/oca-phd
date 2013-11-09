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
    void SetSourceID( Int_t sourceID ){ fSourceID = sourceID; }

    void SetIsMainRun( Bool_t isMain ){ fIsMainRun = isMain; }
    void SetIsCentralRun( Bool_t isCentral ){ fIsCentralRun = isCentral; }
    void SetIsWavelengthRun( Bool_t isWavelength ){ fIsWavelengthRun = isWavelength; }
    
    void SetLambda( Float_t lambda ){ fLambda = lambda; }
    void SetLBIntensityNorm( Float_t runLI ){ fLBIntensityNorm = runLI; }  
    void SetNLBPulses( Int_t nPulses ){ fNLBPulses = nPulses; }
    
    void SetTimeSigmaMean( Float_t sigmean ){ fTimeSigmaMean = sigmean; }
    void SetTimeSigmaSigma( Float_t sigsigma ){ fTimeSigmaSigma = sigsigma; }
    
    // ABREVIATIONS
    // LB := LaserBall
    // DirFit := Direct Fit
    // LPFit := Light Path Fit
    // Manip := Manipulator
    
    // Set the fitted laserball type to be used in the LOCAS fit
    // 0 := Direct Straight Line laserball position
    // 1 := LOCAS::LOCASLightPath fitted laserball position
    // 2 := Manipulator coordinates laserball position
    void SetLBPosType( Int_t posType = 2 );
    
    ////////////////////////////////////////
    
    void SetLBPos( Float_t xPos, Float_t yPos, Float_t zPos ){ 
      fLBPos.SetX( xPos );
      fLBPos.SetY( yPos );
      fLBPos.SetZ( zPos );
    }
    void SetLBPos( TVector3 xyzPos ){ 
      fLBPos = xyzPos;
    }
    
    void SetLBXPosErr( Float_t LBXPosErr ){ fLBXPosErr = LBXPosErr; }
    void SetLBYPosErr( Float_t LBYPosErr ){ fLBYPosErr = LBYPosErr; }
    void SetLBZPosErr( Float_t LBZPosErr ){ fLBZPosErr = LBZPosErr; }
    
    void SetLBPosChi2( Float_t LBPosChi2 ){ fLBPosChi2 = LBPosChi2; }
    void SetLBTheta( Float_t LBTheta ){ fLBTheta = LBTheta; }
    void SetLBPhi( Float_t LBPhi ){ fLBPhi = LBPhi; }
    
    ////////////////////////////////////////
    
    void SetDirFitLBPos(  Float_t xPos, Float_t yPos, Float_t zPos ){ 
      fDirFitLBPos.SetX( xPos );
      fDirFitLBPos.SetY( yPos );
      fDirFitLBPos.SetZ( zPos );
    }
    void SetDirFitLBPos( TVector3 xyzPos ){ 
      fDirFitLBPos = xyzPos;
    }
    
    void SetDirFitLBXPosErr( Float_t LBXPosErr ){ fDirFitLBXPosErr = LBXPosErr; }
    void SetDirFitLBYPosErr( Float_t LBYPosErr ){ fDirFitLBYPosErr = LBYPosErr; }
    void SetDirFitLBZPosErr( Float_t LBZPosErr ){ fDirFitLBZPosErr = LBZPosErr; }
    
    void SetDirFitLBPosChi2( Float_t LBPosChi2 ){ fDirFitLBPosChi2 = LBPosChi2; }
    
    ////////////////////////////////////////
    
    void SetLPFitLBPos( Float_t xPos, Float_t yPos, Float_t zPos ){ 
      fLPFitLBPos.SetX( xPos );
      fLPFitLBPos.SetY( yPos );
      fLPFitLBPos.SetZ( zPos );
    }
    void SetLPFitLBPos( TVector3 xyzPos ){ 
      fLPFitLBPos = xyzPos;
    }
    
    void SetLPFitLBXPosErr( Float_t LBXPosErr ){ fLPFitLBXPosErr = LBXPosErr; }
    void SetLPFitLBYPosErr( Float_t LBYPosErr ){ fLPFitLBYPosErr = LBYPosErr; }
    void SetLPFitLBZPosErr( Float_t LBZPosErr ){ fLPFitLBZPosErr = LBZPosErr; }
    
    void SetLPFitLBPosChi2( Float_t LBPosChi2 ){ fLPFitLBPosChi2 = LBPosChi2; }
    
    ////////////////////////////////////////
    
    void SetManipLBPos( Float_t xPos, Float_t yPos, Float_t zPos  ){ 
      fManipLBPos.SetX( xPos );
      fManipLBPos.SetY( yPos );
      fManipLBPos.SetZ( zPos );
    }
    void SetManipLBPos( TVector3 xyzPos ){ 
      fManipLBPos = xyzPos;
    }
    
    void SetManipLBXPosErr( Float_t LBXPosErr ){ fManipLBXPosErr = LBXPosErr; }
    void SetManipLBYPosErr( Float_t LBYPosErr ){ fManipLBYPosErr = LBYPosErr; }
    void SetManipLBZPosErr( Float_t LBZPosErr ){ fManipLBZPosErr = LBZPosErr; }
    
    void SetManipLBPosChi2( Float_t LBPosChi2 ){ fManipLBPosChi2 = LBPosChi2; }
    
    ////////////////////////////////////////
    
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Int_t GetRunID(){ return fRunID; }
    Int_t GetSourceID(){ return fSourceID; }

    Bool_t GetIsMainRun(){ return fIsMainRun; }
    Bool_t GetIsCentralRun(){ return fIsCentralRun; }
    Bool_t GetIsWavelengthRun(){ return fIsWavelengthRun; }
    
    Int_t GetNPMTs(){ return fLOCASPMTs.size(); }
    
    Float_t GetLambda(){ return fLambda; }
    Float_t GetLBIntensityNorm(){ return fLBIntensityNorm; }  
    Int_t GetNLBPulses(){ return fNLBPulses; }
    
    Float_t GetTimeSigmaMean(){ return fTimeSigmaMean; }
    Float_t GetTimeSigmaSigma(){ return fTimeSigmaSigma; }
    
    Int_t GetLBPosType(){ return fLBPosType; }
    
    ////////////////////////////////////////
    
    TVector3 GetLBPos(){ return fLBPos; } 
    
    Float_t GetLBXPosErr(){ return fLBXPosErr; }
    Float_t GetLBYPosErr(){ return fLBYPosErr; }
    Float_t GetLBZPosErr(){ return fLBZPosErr; }
    
    Float_t GetLBPosChi2(){ return fLBPosChi2; }
    Float_t GetLBTheta(){ return fLBTheta; }
    Float_t GetLBPhi(){ return fLBPhi; }
    
    ////////////////////////////////////////
    
    TVector3 GetDirFitLBPos(){ return fDirFitLBPos; }
    
    Float_t GetDirFitLBXPosErr(){ return fDirFitLBXPosErr; }
    Float_t GetDirFitLBYPosErr(){ return fDirFitLBYPosErr; }
    Float_t GetDirFitLBZPosErr(){ return fDirFitLBZPosErr; }
    
    Float_t GetDirFitLBPosChi2(){ return fDirFitLBPosChi2; }
    
    ////////////////////////////////////////
    
    TVector3 GetLPFitLBPos(){ return fLPFitLBPos; }
    
    Float_t GetLPFitLBXPosErr(){ return fLPFitLBXPosErr; }
    Float_t GetLPFitLBYPosErr(){ return fLPFitLBYPosErr; }
    Float_t GetLPFitLBZPosErr(){ return fLPFitLBZPosErr; }
    
    Float_t GetLPFitLBPosChi2(){ return fLPFitLBPosChi2; }
    
    ////////////////////////////////////////
    
    TVector3 GetManipLBPos(){ return fManipLBPos; }
    
    Float_t GetManipLBXPosErr(){ return fManipLBXPosErr; }
    Float_t GetManipLBYPosErr(){ return fManipLBYPosErr; }
    Float_t GetManipLBZPosErr(){ return fManipLBZPosErr; }
    
    Float_t GetManipLBPosChi2(){ return fManipLBPosChi2; }
    
    //////////////////////////////////////// 
    
    std::map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterBegin(){ return fLOCASPMTs.begin(); }
    std::map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterEnd(){ return fLOCASPMTs.end(); }
    
  private:
    
    Int_t fRunID;                          // The Run ID
    Int_t fSourceID;                       // The Source ID
    
    Bool_t fIsMainRun;                     // TRUE: Main Run False: Other
    Bool_t fIsCentralRun;                  // TRUE: Central Run FALSE: Other Run
    Bool_t fIsWavelengthRun;               // TRUE: Wavelength Run FALSE: Other Run
    
    Float_t fLambda;                       // The wavelength of the laser in this run
    Float_t fLBIntensityNorm;              // Intensity Normalisation Factor (i.e. Intensity of laser for this run)
    Float_t fNLBPulses;                    // Number of laserball pulses in this run
    
    Float_t fTimeSigmaMean;                // Average PMT time spread for this run
    Float_t fTimeSigmaSigma;               // Spread of PMT time spreads for this run (i.e. standard deviation)
    
    Int_t fLBPosType;                      // The fitted LaserBall position type to be used in the LOCAS fit
                                           //  0 := Direct Straight Line laserball position
                                           //  1 := RAT::DS::LightPath fitted laserball position
                                           //  2 := Manipulator coordinates laserball position
    
    TVector3 fLBPos;                       // The Laserball position used in the LOCAS fit
    
    Float_t fLBXPosErr;                    // Error on the x laserball coordinate
    Float_t fLBYPosErr;                    // Error on the y laserball coordinate
    Float_t fLBZPosErr;                    // Error on the z laserball coordinate
    
    Float_t fLBPosChi2;                    // Chi-squared of the laserball position
    Float_t fLBTheta;                      // Theta orientation value ( usually fLBTheta=0 )
    Float_t fLBPhi;                        // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )
    
    TVector3 fDirFitLBPos;                 // Direct Fitted LaserBall position
    
    Float_t fDirFitLBXPosErr;        
    Float_t fDirFitLBYPosErr;
    Float_t fDirFitLBZPosErr;
    
    Float_t fDirFitLBPosChi2;
    
    TVector3 fLPFitLBPos;                  // RAT::DS::LightPath fitted LaserBall position
    
    Float_t fLPFitLBXPosErr;
    Float_t fLPFitLBYPosErr;
    Float_t fLPFitLBZPosErr;
    
    Float_t fLPFitLBPosChi2;
    
    TVector3 fManipLBPos;                  // Manipulator LaserBall position
    
    Float_t fManipLBXPosErr;
    Float_t fManipLBYPosErr;
    Float_t fManipLBZPosErr;
    
    Float_t fManipLBPosChi2;
    
    std::map<Int_t, LOCASPMT> fLOCASPMTs;  // Map of LOCASPMTs (i.e. the PMTs in this Run)
    
    ClassDef(LOCASRun,1)
    
  };
  
}

#endif
