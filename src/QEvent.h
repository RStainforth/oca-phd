#ifndef _QEvent_h
#define _QEvent_h

#include "TF1.h"
#include "TH1.h"
#include "QMCVX.h"
#include "QFit.h"
#include "QRSP.h"
#include "QSnoCal.h"
#include "QTrigger.h"
#include "QADC.h"
#include "QMuxScope.h"
#include "QGlobalShaper.h"
#include "QFTK.h"
class QEvent;
class QSnoCal;
class QMuonFit;

extern "C" void solar_(double &, float *, float &);
// Returns the direction to the Sun for a given Julian Date.

//*-- Author : Mark Boulay 

class QEvent : public QSNO {

public:

  QEvent    ();
  QEvent    (QEvent &anEvent);
  QEvent    (TH1 &aHistogram);
  QEvent &  operator=(QEvent &rhs);
  QEvent    (Int_t *ivars, Float_t *rvars, Double_t *dvars);
  void      Set(Int_t *ivars, Float_t *rvars, Double_t *dvars);
  virtual  ~QEvent();
  void      Initialize();
  void      InitializeAll();
  void      Calibrate();
  Bool_t    IsCalibrated()    { return TestBit(kCalibrated); }
  Bool_t    IsFolder()const        { return kTRUE;                }
  Int_t     GetData     (Int_t *aDataArray, Int_t aMaximum = 0);
  Int_t     PutData     (Int_t *aDataArray);
  void      DumpInfo    ();
  void      Clear        (Option_t *anOption = "");
  void      ClearPMTs    (Option_t *anOption = "");
  void      ClearFITs    (Option_t *anOption = "");
  void      ClearRSPs    (Option_t *anOption = "");
  void      ClearFTKs    (Option_t *anOption = "");
  void      Shrink();
  Bool_t    IsNreEvent(Bool_t ignoreRunNum = kFALSE);
  Int_t     GetCalibratedNCDWaveform(Float_t *delog,Int_t sizeOfArray,Int_t muxScopeEntry,
                                     Float_t a =0, Float_t b = 0, Float_t logOff = 0,
                                     Float_t preTrigOff = 0);  
  
  QPMT    *GetPMT           (Int_t i)const;
  QPMT    *GetOWL           (Int_t i)const;
  QPMT    *GetLG            (Int_t i)const;
  QPMT    *GetBUTT          (Int_t i)const;
  QPMT    *GetNECK          (Int_t i)const;
  QPMT    *GetFECD          (Int_t i)const;
  QPMT    *GetSPARE         (Int_t i)const;
  QPMT    *GetAnyPMT        (Int_t i);
  QPMT    *GetMCPM          (Int_t i)const;
  QMCVX   *GetMCVX          (Int_t i)const;
  QFit    *GetFit           (Int_t i)const;
  QMuonFit*GetMuonFit       (Int_t i)const;
  QRSP    *GetRSP           (Int_t i)const;
  QFTK    *GetFTK           (Int_t i)const;
  QFit    *GetFitByIndex    (Int_t i)const;
  QRSP    *GetRSPByIndex    (Int_t i)const;
  QFTK    *GetFTKByIndex    (Int_t i)const;
  QADC    *GetShaper        (Int_t i)const;
  QMuxScope *GetMuxScope    (Int_t i)const;
  QMuxScope *GetPartialMuxScope(Int_t i)const;
  QGlobalShaper *GetGlobalShaper(Int_t i)const;

  TClonesArray *PMTs;      // List of PMTs for this event.
  TClonesArray *OWLs;      // List of OWLs for this event.
  TClonesArray *LGs;       // List of Low Gain tubes for this event.
  TClonesArray *BUTTs;     // List of BUTTs for this event.
  TClonesArray *NECKs;     // List of NECKs for this event.
  TClonesArray *FECDs;     // List of FECDs for this event.
  TClonesArray *SPAREs;    // List of spare or other PMTs.
  TClonesArray *MCPMs;     // List of Monte-Carlo PMTs for this event.
  TClonesArray *MCVXs;     // List of Monte-Carlo vertices for this event.
  TClonesArray *FITs;      // List of Fits for this event.
  TClonesArray *RSPs;      // List of RSPs for this event.
  TClonesArray *FTKs;      // List of FTKs for this event.
  TClonesArray *Shapers;   // List of NCD shapers for this event
  TClonesArray *MuxScopes; // List of NCD MuxScopes for this event
  TClonesArray *PartialMuxScopes; // List of Partially filled NCD MuxScopes for this event
  TClonesArray *GlobalShapers;    // List of Global Shaper records for this event
  TClonesArray *MuonFits;         //List of Muonfit records

  TClonesArray *GetQPMTs() 
  
  {
    // Return a pointer to the array of PMTs for this event.
    // A specific PMT in the list can be accessed using GetPMT().
    return PMTs;
  }
  TClonesArray *GetQOWLs()
  {
    // Return a pointer to the array of OWLs for this event.
    return OWLs;
  }
  TClonesArray *GetQLGs()
  {
    // Return a pointer to the array of Low Gain tubes for this event.
    return LGs;
  }
  TClonesArray *GetQBUTTs()
  {
    // Return a pointer to the array of BUTT tubes for this event.
    return BUTTs;
  }
  TClonesArray *GetQNECKs()
  {
    // Return a pointer to the array of NECK tubes for this event.
    return NECKs;
  }
  TClonesArray *GetQFECDs()
  {
    // Return a pointer to the array of FECDs for this event.
    return FECDs;
  }
  TClonesArray *GetQSPAREs()
  {
    // Return a pointer to the array of SPAREs for this event.
    return SPAREs;
  }
  TClonesArray *GetQMCPMs()
  {
    // Return a pointer to the array of MCPMs for this event.
    return MCPMs;
  }
  TClonesArray *GetQMCVXs()
  {
    // Return a pointer to the array of MCVXs for this event.
    return MCVXs;
  }
  TClonesArray *GetQFITs()
  {
    // Return a pointer to the array of FITs for this event.
    return FITs;
  }

  TClonesArray *GetQMuonFits(){
    // Return a pointer to the array of MuonFits for this event.
    return MuonFits;
  }

  TClonesArray *GetQRSPs()
    {
    // Return a pointer to the array of RSPs for this event.
    return RSPs;
  }
  TClonesArray *GetQFTKs()
    {
    // Return a pointer to the array of FTKs for this event.
    return FTKs;
  }
  TClonesArray *GetShapers()
    {
      //Return a pointer to the array of NCD Shapers for this event.
      return Shapers;
    }
  TClonesArray *GetMuxScopes()
    {
      //Return a pointer to the array of NCD Shapers for this event.
      return MuxScopes;
    }
  TClonesArray *GetPartialMuxScopes()
    {
      //Return a pointer to the array of NCD Shapers for this event.
      return PartialMuxScopes;
    }

  TClonesArray *GetGlobalShapers()
    {
      return GlobalShapers;
    }

  Int_t     GetMaxPMTsInCrate();
  Int_t     GetMaxPMTsInPC();
  Int_t     GetMaxPMTsInPC(Int_t &aCrate, Int_t &aCard, Int_t &aPC, Int_t &aNTies);
  Int_t     GetMaxLXRailedPMTsInPC(Int_t &aRailedPMTN);
  Int_t     GetNPCs();
  QPMT     *GetQmaxPMT();
  QPMT     *GetZeroRawChargePMT(Int_t &aNPMTs);

  QEvent&    operator+=(QPMT &pmt)      { Add         (&pmt);     return(*this); }
  QEvent&    operator+=(QFit &fit)      { AddQFIT     (&fit);     return(*this); }
  QEvent&    operator+=(QRSP &rsp)      { AddQRSP     (&rsp);     return(*this); }
  QEvent&    operator+=(QFTK &ftk)      { AddQFTK     (&ftk);     return(*this); }
  QEvent&    operator+=(QMCVX &mcvx)    { AddQMCVX    (&mcvx);    return(*this); }
  
  void Add          (QPMT *pmt);
  void AddQPMT      (QPMT *pmt);
  void AddQOWL      (QPMT *pmt);
  void AddQLG       (QPMT *pmt);
  void AddQBUTT     (QPMT *pmt);
  void AddQNECK     (QPMT *pmt);
  void AddQFECD     (QPMT *pmt);
  void AddQSPARE    (QPMT *pmt);
  void AddQMCPM     (QPMT *mpm);
  void AddQMCVX     (QMCVX *mcvx);
  void AddQFIT      (QFit *fit);
  void AddMuonFit   (QMuonFit *fit);
  void AddQRSP      (QRSP *rsp);
  void AddQFTK      (QFTK *ftk);
  void AddShaper    (QADC *shaper);
  void AddMuxScope  (QMuxScope *muxscope);
  void AddPartialMuxScope  (QMuxScope *muxscope);
  void AddGlobalShaper( QGlobalShaper *globalshaper );
  void AddQPMT      (Int_t *ivars, Float_t *rvars);
  void AddQOWL      (Int_t *ivars, Float_t *rvars);
  void AddQLG       (Int_t *ivars, Float_t *xvars);
  void AddQBUTT     (Int_t *ivars, Float_t *xvars);
  void AddQNECK     (Int_t *ivars, Float_t *xvars);
  void AddQFECD     (Int_t *ivars, Float_t *xvars);
  void AddQSPARE    (Int_t *ivars, Float_t *xvars);
  void AddQMCPM     (Int_t *ivars, Float_t *xvars);
  void AddQMCVX     (Double_t aMCTime, Float_t *xvars, Int_t aBit);
  void AddQFIT      (Float_t *xvars, Int_t aBit);
  void AddQRSP      (Float_t *rvars, Int_t aFitIndex);
  void AddQFTK      (Float_t *rvars, Int_t aFitIndex);
  void AddShaper    (Int_t *ivars);
  void AddShaper    (Int_t *ivars, Int_t *jvars, Float_t *fvars);
  void AddMuxScope  (Int_t *ivars,Int_t *jvars,Float_t *fvars);
  void AddMuxScope  (Int_t *ivars,Int_t *jvars,Float_t *fvars,
                     Int_t *kvars, Float_t *rvars);
  void AddPartialMuxScope  (Int_t *ivars, Int_t *jvars, Float_t *fvars);
  void AddPartialMuxScope  (Int_t *ivars, Int_t *jvars, Float_t *fvars, 
                            Int_t *kvars, Float_t *rvars);
  void AddGlobalShaper( Int_t *ivars);

    void AddMuonFit(Int_t * aFit, Int_t *aVertex, Int_t *aTrack, Int_t *aMes,
		  Int_t &aFitter);
 
 QPMT    *AddQPMT();
  QPMT    *AddQOWL();
  QPMT    *AddQLG();
  QPMT    *AddQBUTT();
  QPMT    *AddQNECK();
  QPMT    *AddQFECD();
  QPMT    *AddQSPARE();
  QPMT    *AddQMCPM();
  QMCVX   *AddQMCVX();
  QFit    *AddQFIT();
  QMuonFit *AddMuonFit();
  QRSP    *AddQRSP();
  QFTK    *AddQFTK();
  QADC    *AddShaper();
  QMuxScope *AddMuxScope();
  QMuxScope *AddPartialMuxScope();
  QGlobalShaper *AddGlobalShaper();
  
  void        ShowTrigger();
  QTrigger   *GetQTrigger() { return gQTrigger; }
  Int_t       HasTrig(char *aTrigger);

  //classifier calculations:
  
  void       Solar             (Float_t &u, Float_t &v, Float_t &w, Float_t aTwist = -49.58);
  Float_t    SolarDirection    (Int_t aMethod = 0);
  Float_t    GetThetaSun       (Int_t aMethod = 0, Int_t aFit = 0, Float_t aTwist = -49.58);
  Float_t    GetThetaSun       (QMCVX *, Int_t aMethod = 0);
  Float_t    GetThetaSun       (QFit *, Int_t aMethod = 0);
  Float_t    GetThetaSun       (Float_t uFit, Float_t vFit, Float_t wFit, Int_t aMethod = 0, Float_t atwist = -49.58);

  void CalculateBeta(Float_t *anOutput, TVector3 &aPosition, 
		     Float_t aTime, Float_t aResidualCut=200);
  void CalculateBeta(Float_t *anOutput, Int_t aFitIndex=10, 
		     Float_t aResidualCut=200 );

  Double_t Beta14(Float_t *aBeta);
  Double_t Beta14(Int_t aFitIndex=10);

  Float_t     GetThetaIJ(Int_t aFitIndex);
  Float_t     GetThetaIJ(TVector3 &aPosition, Float_t aTime);

  Double_t    GetDoubleTime();
  void        MakeTime(char *ctime, Int_t timezone = 0);
  void        GetTAC(TH1F *aSoftTACHistogram);
  void        GetTimeResidual(TH1F *aTimeResidualsHistogram, Int_t anIndex=10, Bool_t reset=1);
  void        FillCerenkov(TH1F *aCerenkovDistribution,Int_t aFitIndex);
  Int_t       GetNwin(Float_t aTimeEarly, Float_t aTimeLate, Int_t aFitIndex = 0);
  Float_t     GetPromptWidth(Float_t aTimeEarly = 10, Float_t aTimeLate = 10, Int_t aFitIndex = 2, TH1F *aHistogram = NULL, const char *anOption = "NRQ");
  Float_t     GetMedianPMTTime();
  Float_t     GetTimeRMS();
  Float_t     GetFitChi2(Int_t iFitIndex = 2, Float_t aTimeEarly = 4, Float_t aTimeLate = 8);

  void Fit      (Int_t aFitIndex = 0);  // Warning!  Obsolete code.
  void ReFit    (Int_t aFitIndex = 0);  // Warning!  Obsolete code.

  // Writer functions:
  void SetEvent_id      (Int_t anEventID);
  void SetTrig_type     (Int_t aTriggerType);
  void SetNhits         (Int_t aNHits);
  void SetNph           (Float_t aNPh);
  void SetnPMTs         (Int_t aNPMTs)         { nPMTs = (Short_t)aNPMTs; }
  void SetnPBUNs        (Int_t aNumber)        { nPBUNs = aNumber; }
  void SetnMuxScopes    (Int_t aMuxScopes)     {nMuxScopes = aMuxScopes; }
  void SetnPartialMuxScopes(Int_t aPartialMuxScopes) {nPartialMuxScopes = aPartialMuxScopes; }
  void SetnShapers      (Int_t aShapers)       {nShapers = aShapers; }
  void SetnGlobalShapers(Int_t aGlobalShapers) {nGlobalShapers = aGlobalShapers; }
  void SettPrev         (Float_t aTime)        { tPrev = aTime; }
  void SettNext         (Float_t aTime)        { tNext = aTime; }
  void SetGtr_time      (Double_t aTime);
  void SetJulianDate    (Int_t aJulianDate)    { JulianDate = aJulianDate; }
  void SetUT1           (Int_t aUT1)           { UT1 = aUT1; }
  void SetUT2           (Int_t aUT2)           { UT2 = aUT2; }
  void SetRun           (Int_t aRun)           { Run = aRun; }
  void SetEventIndex    (Int_t anIndex)        { SetUniqueID(anIndex); }
  void SetDamnID        (UInt_t anID)          { DamnID = anID; }
  void SetDamnID1       (UInt_t anID)          { DamnID1 = anID; }
  void SetDarnID        (UInt_t anID)          { DarnID = anID; }
  void SetEsumPeak      (Int_t anESum)         { EsumPeak = anESum; }
  void SetEsumDiff      (Int_t anESum)         { EsumDiff = anESum; }
  void SetEsumInt       (Int_t anESum)         { EsumInt  = anESum; }
  void SetTime10MHz     (Double_t aTime)       { Time10MHz = aTime; }
  void SetTrigErr       (Int_t aTrigger)       { TrigErr = aTrigger; }
  void SetDataSet       (Int_t aDataSet)       { DataSet = aDataSet; } 
  void SetNCDStatus     (Int_t aNCDStatus)     { NCDStatus = aNCDStatus; }
  void SetNumMuxG       (Int_t aNumMuxG)       { NumMuxG = aNumMuxG; }
  void SetNumMux        (Int_t aNumMux)        { NumMux = aNumMux; }
  void SetNumScope      (Int_t aNumScope)      { NumScope = aNumScope; }
  void SetNCDClock1     (Int_t aNCDClock1)     { NCDClock1 = (aNCDClock1 & 0xffffff); }
  void SetNCDClock2     (Int_t aNCDClock2)     { NCDClock2 = aNCDClock2; }
  void SetNCDLatchReg   (Int_t aNCDLatchReg)   { NCDLatchRegister = aNCDLatchReg; }
  void SetNCDGTID       (Int_t aNCDGTID)       { NCDGTID = aNCDGTID; }
  void SetNCDSync       (Int_t aNCDSync)       { NCDSync = aNCDSync; }
  void SetMCEventNumber (Int_t aMCEventNumber) { MCEventNumber = aMCEventNumber; }

  // Reader functions:
  Int_t       GetEvent_id()       { return Event_id; }
  Int_t       GetTrig_type()      { return Trig_type; }
  Int_t       GetNhits()          { return (Int_t)Nhits; }
  Float_t     GetNph()            { return Nph; }
  Int_t       GetnPMTs()          { return (Int_t)nPMTs; }
  Int_t       GetnOWLs()          { return (Int_t)nOWLs; }
  Int_t       GetnLGs()           { return (Int_t)nLGs; }
  Int_t       GetnBUTTs()         { return (Int_t)nBUTTs; }
  Int_t       GetnNECKs()         { return (Int_t)nNECKs; }
  Int_t       GetnFECDs()         { return (Int_t)nFECDs; }
  Int_t       GetnSPAREs()        { return (Int_t)nSPAREs; }
  Int_t       GetnAny()           { return (Int_t)nOWLs + nPMTs + nFECDs + nNECKs + nBUTTs + nLGs + nSPAREs; }
  Int_t       GetnPBUNs()         { return (Int_t)nPBUNs; }
  Int_t       GetnMCPMs()         { return (Int_t)nMCPMs; }
  Int_t       GetnMCVXs()         { return (Int_t)nMCVXs; }
  Int_t       GetnFITs()          { return (Int_t)nFITs; }
  Int_t       GetnMuonFits()      { return (Int_t)nMuonFits; }
  Int_t       GetnRSPs()          { return (Int_t)nRSPs; }
  Int_t       GetnFTKs()          { return (Int_t)nFTKs; }
  Int_t       GetnShapers()       { return (Int_t)nShapers;}
  Int_t       GetnMuxScopes()     { return (Int_t)nMuxScopes;}
  Int_t       GetnPartialMuxScopes() { return (Int_t)nPartialMuxScopes;}
  Int_t       GetnGlobalShapers() { return (Int_t)nGlobalShapers; }
  Float_t     GetXfit();
  Float_t     GetYfit();
  Float_t     GetZfit();
  Float_t     GetTfit();
  Float_t     GetRfit();
  Float_t     GetRgen(Int_t anIndex = 0);
  Float_t     GettPrev()         { return tPrev; }
  Float_t     GettNext()         { return tNext; }
  Double_t    GetGtr_time()      { return Gtr_time; }
  Int_t       GetJulianDate()    { return JulianDate; }
  Int_t       GetUT1()           { return UT1; }
  Int_t       GetUT2()           { return UT2; }
  Int_t       GetRun()           { return (Int_t)Run; }
  Int_t       GetEventIndex()    { return (Int_t)GetUniqueID(); }
  UInt_t      GetDamnID()        { return DamnID; }
  UInt_t      GetDamnID1()       { return DamnID1; }
  UInt_t      GetDarnID()        { return DarnID; }
  Bool_t      TestDamnID(Int_t aCutID);
  Bool_t      TestDamnID1(Int_t aCutID);
  Bool_t      TestDarnID(Int_t aCutID);
  Int_t       GetEsumPeak()     { return EsumPeak; }
  Int_t       GetEsumDiff()     { return EsumDiff; }
  Int_t       GetEsumInt ()     { return EsumInt;  }
  Double_t    GetTime10MHz()    { return Time10MHz; }
  Int_t       GetTrigErr()      { return TrigErr; }
  Int_t       GetDataSet()      { return DataSet; }
  Int_t       GetNCDStatus()    { return NCDStatus; }
  Int_t       GetNumMuxG()      { return NumMuxG; }
  Int_t       GetNumMux()       { return NumMux; }
  Int_t       GetNumScope()     { return NumScope; }
  Int_t       GetNCDClock1()    { return NCDClock1; }
  Int_t       GetNCDClock2()    { return NCDClock2; }
  Int_t       GetNCDLatchReg()  { return NCDLatchRegister; }
  Int_t       GetNCDGTID()      { return NCDGTID; }
  Int_t       GetNCDSync()      { return NCDSync; }
  Int_t       GetMCEventNumber()const{ return MCEventNumber; }
  Bool_t IsSame(const char *comment, QEvent &aQEvent, Int_t aPrintMode=0)const
;
	Bool_t IsSame2( const char *comment, QEvent *anEvent, Int_t aPrintMode = 0, Int_t skipDamnDarn = 0 ) const;
  Bool_t IsHe4();  
private:
  void ZapFields();
  Int_t *Data()const{return (Int_t *)&Event_id;}
  Int_t DataN()const{int i= &MCEventNumber-Data();return i;}
public:
  ClassDef(QEvent,31)  // One triggered event.
private:
  Int_t       Event_id;      // Event id number.
  Int_t       Trig_type;     // Event trigger word.
  Float_t     Nph;           // Integrated charge for this event.
  Short_t     Nhits;         // Number of inward-looking PMTs hit for this event (from SNOMAN).
  Short_t     nPMTs;         // Number of PMTs for this event.
  Short_t     nOWLs;         // Number of OWLs for this event.
  Short_t     nLGs;          // Number of Low Gain tubes for this event;
  Short_t     nBUTTs;        // Number of BUTTs for this event.
  Short_t     nNECKs;        // Number of NECKs for this event.
  Int_t       nFECDs;        // Number of FECDs for this event.
  Short_t     nSPAREs;       // Number of SPARE PMTs for this event.
  Short_t     nPBUNs;        // Number of PMT bundles for this event (from zdab file).
  Int_t     nMCPMs;        // Number of MCPMs for this event.
  Int_t     nMCVXs;        // Number of MCVXs for this event.
  Short_t     nFITs;         // Number of FITs for this event.
  Short_t     nRSPs;         // Number of RSPs for this event.
  Short_t     nFTKs;         // Number of FTKs for this event.
  Short_t     nShapers;      // Number of QADCs (NCD Shapers) for this event.
  Short_t     nMuxScopes;    // Number of MuxScopes for this event.
  Short_t     nPartialMuxScopes; // Number of Partially filled muxscopes
  Short_t     nGlobalShapers; // Number of Global Shaper records
  Short_t     nMuonFits;      // Number of Muon Fit Records
  Float_t     tPrev;         // Previous trigger 
  Float_t     tNext;         // Next trigger 
  Double_t    Gtr_time;      // 50 MHz global trigger time (KEV_GTR).
  Int_t       JulianDate;    // 10 Mhz date stamp (KEV_JDY).
  Int_t       UT1;           // 10 Mhz Universal Time seconds (KEV_UT1).
  Int_t       UT2;           // 10 Mhz Universal Time nanoseconds (KEV_UT2).
  Int_t       Run;           // Run number.
  UInt_t      DamnID;        // Damn word.
  UInt_t      DamnID1;        // Damn word 1.
  UInt_t      DarnID;        // Darn word.
  Int_t       EsumPeak;      // Digitized ESUM peak.
  Int_t       EsumDiff;      // Digitized ESUM derivative.
  Int_t       EsumInt;       // Digitized ESUM integral.
  Double_t    Time10MHz;     // Raw 10 MHz clock time
  Int_t       TrigErr;       // Trigger error bits
  Int_t       DataSet;       // Data Splitter Word
  Int_t       NCDStatus;     // NCD Status: 0=none, 1=good, 2=multi, 3=bad
  Int_t       NumMuxG;       // Number of Mux global records
  Int_t       NumMux;        // Number of Mux records             // this is just the sum of partial and full muxes
  Int_t       NumScope;      // Number of Scope records
  Int_t       NCDClock1;    // High 24 Bits of NCD Clock; 
  Int_t       NCDClock2;    // Low 32 bits of NCD clock; 
  Int_t       NCDLatchRegister;  
  Int_t       NCDGTID;     // if no MTC GTID this fills GTID
  Int_t       NCDSync;     // NCD Sync clear word
  Int_t       MCEventNumber; //the MC event that generated this trigger.
  
};

#endif  // not _QEvent_h
