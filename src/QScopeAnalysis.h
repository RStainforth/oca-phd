// 	Author : Laura Stonehill & Adam Cox
//	UpDates: 
//		4/10/03 -- original 
//              6/3/03 -- making more flexible (Adam Cox)
//              7/8/03 -- moving method descriptions inside method definitions for automated documentation
//                        fixed possible infinite loop situation
//                        added optional arguements to BaselineRemoval() and RiseTime()  (Adam Cox)
//              11/30/04 - added EndPosition() method (J Wendland)
//              02/10/05 - set de-preamp to false by default.  I learned that we will not be 
//                         attempted to deconvolve the RC circuits from the waveform. (AC)
//              25/04/05 - enabled baseline removal of non-logged traces (for logamp by-passed data)
//                       - added SetMessages() to enable/disable STDOUT messages. (JW)

#ifndef _QScopeAnalysis_h
#define _QScopeAnalysis_h

#include <TObject.h>
#include "TH1.h"
#include "NCDConstants.h"
#include "TH1C.h"
#include "TH1F.h"
#include "TH1D.h"
#include "QTitles.h"
class QMuxScope;

class QScopeAnalysis : public TObject
{
public:
  QScopeAnalysis();    
  QScopeAnalysis(TH1C *aPulse,Bool_t deLog = 1,Bool_t isZLF = 0,
		 Float_t aRiseTimePercent = 0.5, 
		 Float_t aBaselineRemovalPercent = 0.0, Bool_t dePreamp = 0,
		 Bool_t rmPostBaseline = 0, Bool_t aMessages=1 );
  QScopeAnalysis(TH1C *aPulse,Float_t a, Float_t b, Float_t logoffset, Float_t chargeFactor, 
		 Float_t time, Float_t scopeoffset, Bool_t deLog = 1,
		 Bool_t isZLF = 0,Float_t aRiseTimePercent = 0.5, 
		 Float_t aBaselineRemovalPercent = 0.0, Bool_t dePreamp = 0, 
		 Bool_t rmPostBaseline = 0, Bool_t aMessages=1 );
  void Clear(Option_t *anOption = "");
  virtual ~QScopeAnalysis();

  Bool_t ProcessWaveforms(void);  
  void ClearAnalysisWaveforms(void);  //this sets all the analysis waveforms to zero and sets the number of entries to zero.  this is automatically called by ProcessWaveforms.
  
  void SetMessages(Bool_t aState=kTRUE) { fMessages=aState; };

  //anytime that you call any of the following 'Set' methods, you will need to call ProcessWaveforms() in order
  //to extract the pulse information
  void SetDelogging(Bool_t deLog);
  void SetDePreamp(Bool_t dePreamp);
  void SetRmPostBaseline(Bool_t RmPostBaseline);
  void SetRiseTimePercent(Float_t aPercent);
  void SetBaselineRemovalPercent(Float_t aPercent);
  void SetLogAmpParameters(Float_t a, Float_t b, Float_t logoffset, Float_t chargeFactor, Float_t time, Float_t scopeoffset);
  void SetLogAmpParameter_a(Float_t aValue) {faParameter = aValue; ResetCalculatedValues();}
  void SetLogAmpParameter_b(Float_t aValue) {fbParameter = aValue; ResetCalculatedValues();}
  void SetLogOffset(Float_t aValue) {fLogOffset = aValue; ResetCalculatedValues();}
  void SetLogAmpPreampRC(Float_t aValue) {fpreampRCFactor = aValue; ResetCalculatedValues();}
  void SetLogAmpElecDelayTime(Float_t aValue) {felecDelayTime = aValue; ResetCalculatedValues();}
  void SetScopeOffset(Float_t aValue) {fScopeOffset = aValue; ResetCalculatedValues();}
  void SetIsZeroLevelFilter(Bool_t aState);
  void SetNewScopeTrace(TH1C *aPulse);
  void SetScopeTriggerPosition(Float_t aValue) {fScopeTriggerPosition = aValue; ResetCalculatedValues();}
  
  TH1C* GetRawScopeTrace(void) { return &fRawValues;}
  TH1F* GetIntegratedNCDPulse(void) { return &fIntegratedValues;}
  TH1F* GetAntiLoggedScopeTrace(void) { return &fAntiloggedValues; }
  TH1F* GetBaselineRemovedPulse(void) { return &fBaselineRemovedValues; }
  TH1F* GetInverseAntiLoggedScopeTrace(void) { return &fInverseAntiloggedValues; }
  TH1F* GetNCDPulse(void) {return &fOriginalNCDPulse; }
  void GetLogAmpParameters(Float_t &a,Float_t &b, Float_t &logoffset, Float_t &chargeFactor, Float_t &time, Float_t &scopeoffset);
  Bool_t GetIsDeloggingOn(void) {return fIsDelogging;}
  Bool_t GetIsDePreampOn(void) {return fIsDePreamp;}
  Bool_t GetIsZeroLevelFilter(void) {return fIsZeroLevelFilter;}
  Float_t GetRiseTimePercent(void) {return fRiseTimePercent;}
  Float_t GetBaselineRemovalPercent(void) {return fBaselineRemovalPercent;}
  Bool_t IsGoodLogAmpParameters(void);
  Float_t GetScopeTriggerPosition(void) {return fScopeTriggerPosition;}
  Bool_t GetIsUsingMeasuredScopeOffset(void){return fIsUseMeasuredScopeOffset;}
  Float_t GetMeasuredScopeOffset(void){return fMeasuredScopeOffset;}
  Bool_t GetIsRmPostBaseline(void){return fRmPostBaseline; }

  //use the following methods to retrieve the pulse shape analysis information
  Bool_t GetDidPassZeroLevelFilter(void) {return fPassZLF;}
  Int_t GetPulseStartTime(void) {return fPulseStartTime;}
  Int_t GetPulseEndTime(void) {return fPulseEndTime;}
  Float_t GetCharge(void) {return fCharge;}
  Int_t GetRiseTime(void) {return fRiseTime;}
  
  void GetAltRiseTimeFWHM(Int_t &risetime, Int_t &FWHM);
  Int_t GetTimeBetweenAmplitudes(TH1 *delog, Float_t startPer = 0.10, Float_t endPer = 0.5,
                             Int_t minBin = 1500);
  Int_t GetTimeBetweenAmplitudes(Float_t startPer = 0.10, Float_t endPer = 0.5,
                             Int_t minBin = 1500);

  void ResetCalculatedValues(void);
  void InverseAntiLog(void);
  void SetUseMeasuredScopeOffset(Bool_t anOption);
  void MeasureScopeOffset(void);

  Bool_t RiseTimeAndCharge(void);

  //a stand-alone delogging method
  Bool_t DelogWaveform(TH1 &raw,TH1 &delog,
                       Float_t a, Float_t b, 
                       Float_t logoffset, Float_t scopeoffset,
                       Bool_t delogFinalBin = 0);
  
  void PrintSettingsStatus(void);
  
 private:
  
  void AntiLog(void);
  void AntiPreamp(void);
  void BaselineRemoval(void);
  Float_t ChargeIntegral(void);
  Int_t RiseTime(void);
  void RunningIntegral(void);
  Int_t StartPosition(void);
  Int_t EndPosition(void);
  Bool_t ZeroLevelFilter(void);


  TH1C fRawValues;
  TH1F fAntiloggedValues;
  TH1F fOriginalNCDPulse;
  TH1F fIntegratedValues;
  TH1F fBaselineRemovedValues;
  TH1F fInverseAntiloggedValues;  //this is the fAntiloggedValues histogram that is uncalibrated using the logamp parameters.  If you're using real date (not Monte Carlo), this should match the waveform in fRawValues;

  //  Double_t fChargeIntegralUpperLimit; //bin number  -- what is this? it's not used anywhere, so I'm removing it
  //Int_t fMaximumDeltaCut;         //bins between sucessive zero crossings 
  Float_t fRiseTimePercent; //find rise time to this fraction of total charge 
  Int_t fCurrentWaveformSize;
  
  Float_t faParameter;
  Float_t fbParameter;
  Float_t fLogOffset;
  Float_t fpreampRCFactor;
  Float_t felecDelayTime;
  Float_t fScopeOffset;

  Bool_t fIsZeroLevelFilter;  //if this is set true then the ZeroLevelFilter is turned on... it is on by default
  Bool_t fIsDelogging;
  Bool_t fIsDePreamp;
  Bool_t fRmPostBaseline;
  Float_t fBaselineRemovalPercent;  //average the last fraction of the antilogged waveform to subtract off any remaining baseline
  Bool_t fPassZLF;  //equals true if pulse passes ZeroLevelFilter

  Int_t fPulseStartTime;  //in units of bins!!
  Int_t fPulseEndTime;  //in units of bins!!
  Int_t fRiseTime; //in units of bins!! - need to convert to time
  Float_t fCharge;  //in arbitrary units

  Float_t fScopeTriggerPosition;  //scope trigger position in percentage of full waveform.

  //allow this class to measure the scope offset rather than using the value
  //in the ncla bank.
  Float_t fMeasuredScopeOffset;
  Float_t fMeasuredScopeOffsetUncer;
  Bool_t fIsUseMeasuredScopeOffset;

  // toggle stdout 
  Bool_t fMessages;

  ClassDef(QScopeAnalysis,9);  //NCD waveform analysis class
};


#endif




