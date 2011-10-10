//__________________________________________________________
// QMuxScope.h
//*-- Author : Adam Cox
//	UpDates: 
//		1 10/01/02 -- original version
//              2 3/16/03 -- updated move towards QSNO coding standards compliance
//              3 4/21/03 -- merging with SNOMAN event structure
//              4 10/08/03 -- merging with Aksel's SNOMAN stuff
//              5 01/27/05 -- Added correlation to shaper event           
//

#ifndef _QMuxScope_h
#define _QMuxScope_h

#include <TObject.h>
#include "TH1.h"
#include "NCDConstants.h"
#include "QScopeAnalysis.h"

class QMuxScope : public TObject
{
public:
  
  QMuxScope(Bool_t bUseNewPreTrigOffset = 1);
  QMuxScope(const QMuxScope &aMuxScope);
  QMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars, 
            Bool_t bUseNewPreTrigOffset = 1);
  QMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars, 
						Int_t *kvars, Float_t *rvars, 
            Bool_t bUseNewPreTrigOffset = 1);

  virtual ~QMuxScope();
#ifdef QSNO_MUXSCOPE_CLEAR 
  void Clear(Option_t *anOption = "" );
#endif
#ifdef QSNO_MUXSCOPE_DELETE 
  void Delete(Option_t *anOption = "" );
#endif
  void Initialize();
  QMuxScope & operator = ( const QMuxScope &aQmux );

  void DrawRecordedPulse(){fValues.Draw();}
  void DrawRealPulse();
  Bool_t DeleteQScopeAnalysis();

  Int_t GetStatusWord(void) {return fStatusWord;}
  Int_t GetLatchRegisterID(void) {return fLatchRegisterID;}
  Int_t GetMuxBusNumber(void) {return fMuxBusNumber;}  
  Int_t GetMuxBoxNumber(void) {return fMuxBoxNumber;}  
  Int_t GetMuxChannelMask(void) {return fMuxChannelMask;}  //which channels have events
  Int_t GetMuxChannel(void);  //returns a -1 if multiple mux channels fired in this event  
  Int_t GetScopeNumber(void) {return fScopeNumber;}    //which scope received data
  Int_t GetScopeChannel(void) {return fScopeChannel;}       //which channel received data
  Int_t GetSizeOfScopeTrace(void) {return fSizeOfScopeTrace;}
  Stat_t GetScopeTraceBinContent(Int_t aBin) {return fValues.GetBinContent(aBin);}  

  Int_t GetNCDStringNumber(void) {return fNCDStringNumber;}
  Double_t GetClockRegister(void) {return fClockRegister;}
  Int_t GetGlobalMuxRegister(void) {return fGlobalMuxRegister;}
  TH1C* GetValues(void) {return &fValues;}                 // histogram of raw scope values
  Bool_t IsMultipleMuxChannels(void);
  Float_t GetLogAmpParameter_a(void) {return fLogAmpParam_a;}
  Float_t GetLogAmpParameter_b(void) {return fLogAmpParam_b;}
  Float_t GetLogOffset(void) {return fLogOffset;}
  Float_t GetScopeOffset(Int_t fBins = 1000);
  Float_t GetLogAmpPreampRCFactor(void) {return fLogAmpPreampRCFactor;}
  Float_t GetLogAmpElecDelayTime(void) {return fLogAmpElecDelayTime;}
  Bool_t IsGoodLogAmpParameters(void);
  void   GetAltRiseTimeFWHM(Int_t &riset, Int_t &FWHM);
  Int_t GetCorrShaperGTID() {return fShapereventGTID;};
  Int_t GetCorrShaperADCCharge() {return fShaperADCcharge;};
  Float_t GetCorrShaperDeltaT() {return fShaperDeltaT;}

  //methods that use QScopeAnalysis
  Float_t GetCharge(void);
  Int_t GetRiseTime(void);
  Int_t GetPulseStartTime(void);
  
  QScopeAnalysis* GetScopeAnalysis(void){return fQScopeAnalysis;}

  void SetStatusWord(Int_t aStatus) { fStatusWord = aStatus;}
  void SetLatchRegisterID(Int_t aVal) {fLatchRegisterID = aVal;}
  void SetMuxBusNumber(Int_t aNum) {fMuxBusNumber = aNum;}
  void SetMuxBoxNumber(Int_t aNum ) {fMuxBoxNumber = aNum;}
  void SetMuxChannelMask(Int_t aMask) {fMuxChannelMask = aMask;}
  void SetMuxChannel(Int_t aMuxChannelNumber) {fMuxChannelNumber = aMuxChannelNumber;}
  void SetScopeNumber(Int_t aSelection) {fScopeNumber = aSelection;}  // 0 is scope A, 1 is scope B
  void SetScopeChannel(Int_t aChan) {fScopeChannel = aChan;}
  void SetSizeOfScopeTrace(Int_t aSize) {fSizeOfScopeTrace = aSize;}
  void SetScopeTraceBinContent(Int_t aBin,Char_t aValue) {fValues.SetBinContent(aBin,aValue);} // See comment above ( FindBin() )
  void SetNCDStringNumber(Int_t aNum) {fNCDStringNumber = aNum;}
  void SetClockRegister(Double_t aClock) {fClockRegister = aClock;}
  void SetGlobalMuxRegister(Int_t aGlobalMuxRegister) {fGlobalMuxRegister = aGlobalMuxRegister;}
  Bool_t SetLogAmpParameter_a(Float_t aValue);
  Bool_t SetLogAmpParameter_b(Float_t aValue);
  Bool_t SetLogOffset(Float_t aValue);
  Bool_t SetLogAmpPreampRCFactor(Float_t aValue);
  Bool_t SetLogAmpElecDelayTime(Float_t aValue);
  Bool_t SetScopeOffset(Float_t aValue);

  Bool_t InitQScopeAnalysis(void);
  void SetCorrShaperADCCharge(Int_t adc) {fShaperADCcharge = adc;};
  void SetCorrShaperGTID(Int_t id) {fShapereventGTID = id;};
  void SetCorrShaperDeltaT(Float_t aDeltaT) {fShaperDeltaT = aDeltaT;}

  Bool_t IsSame(Int_t anIndex, const QMuxScope *aMuxScope, Int_t aPrintMode=0)const;
  Bool_t IsHe4();  //true if string is a Helium 4 string
  
  Bool_t GetIsPreTrigCalculated(void) {return bCalcNewOffset;}
  void SetIsPreTrigCalculated(Bool_t opt = true) {bCalcNewOffset = opt;}
  
  ClassDef(QMuxScope,14);//Scope data class for NCD mux system and waveform data

 private:    
  const Int_t *Data()const{return &fStatusWord;}
  Int_t DataN()const{int i= ((Int_t *)&fShaperDeltaT)-Data();return i;}
  Float_t CalculatePreTrigBaseline(Int_t fBins = 1000);// calculates scope offset .. or VpreTrig
 
  //The following order and naming of variables MUST match the event structure in SNOMAN
  //for FORTRAN/C++ analysis compatibility

  Int_t fStatusWord;          //lowest bit holds mux data status, second lowest bit holds scope data status == KNEMS_STATUS
  Int_t fMuxChannelMask;      //which channels have events
  Int_t fScopeNumber;         //which scope received data
  Int_t fMuxBusNumber;        //Mux Bus number 
  Int_t fMuxBoxNumber;        //Mux Box number (hardware jumper settings on back of MUX board)
  Int_t fNCDStringNumber;     //NCD String number
  
  Double_t fClockRegister;    //Upper 24 bits of clock when scope trace was acquired
  Int_t fLatchRegisterID;     //Latch Register ID when scope trace was acquired
  Int_t fSizeOfScopeTrace;    //size of waveform in bytes
  Int_t fScopeChannel;        //which channel received data

  TH1C fValues;               // histogram of scope values
  Int_t fMuxChannelNumber;    //this value will be -1 if there are multiple channels fired in the same event
  Int_t fGlobalMuxRegister;   // Copy of corresponding Global Mux record
  
  Float_t fLogAmpParam_a;        //a
  Float_t fLogAmpParam_b;        //b
  Float_t fLogOffset;           //c_chan
  Float_t fLogAmpPreampRCFactor;  //don't worry about this one - no longer used
  Float_t fLogAmpElecDelayTime; //Dt
  Float_t fScopeOffset;       //Vpretrig

  Int_t fShaperADCcharge;     //uncalibrated ADC charge of correlated shaper event
  Int_t fShapereventGTID;     //GTID of shaper event correlated with this event
  Float_t fShaperDeltaT;      //Delta t to correlated shaper event

  QScopeAnalysis* fQScopeAnalysis;
  
  Bool_t bCalcNewOffset;  //if this is true (which it is by default), the pre-trigger baseline will be calcuated for each waveform rather than using the value in the logamp calibrations
};


#endif




