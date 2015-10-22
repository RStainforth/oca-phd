//* Author: M. Boulay

#ifndef _qtree_h
#define _qtree_h

#include "QEvent.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TObject.h"
#include "TFile.h"
#include "TNamed.h"
#include "TBuffer.h"
#include "TControlBar.h"
#include "TCut.h"
#include "TEventList.h"
#include "QRunHeader.h"
#include "QTriggerBank.h"
#include "QCast.h"
#include "QSosl.h"
#include "QThresholdBank.h"
#include "TString.h"
#include "TH1.h"
#include "TButton.h"
#include "TArrayI.h"
#include "TList.h"
#include "QGlobals.h"

#include "TTreeFormula.h"
#include "TSystem.h"

//*-- Author : Mark Boulay                          
//* Copyright (c) 1998 Mark Boulay, All rights reserved.      

class QTree : public TTree
{
  
 public:

  QTree                           ();
  QTree                           (const char *name, const char *title);
  virtual ~QTree                  ();
#ifndef USE_ROOT_GETENTRY
  virtual Int_t GetEntry          (Int_t entry=0, Int_t getall=0);
#endif
  QTree &operator=                (const QTree &rhs);
  void  ClearClones               ();
  Int_t GetEvent                  (Int_t mEvent_no);
  void  GetPreviousEventbyTrigger (char *ctrig);
  void  GetNextEventbyTrigger     (char *ctrig);
  Int_t GetNextEvent              ();
  Int_t GetNextEvent              ( char *mCut );
  Int_t GetPreviousEvent          ();
  void SetBrowserList             (TEventList *mEventList );
  void SetComments                ( char *mComments );
  void SetTitles                  (Int_t *mTitles);
  void SetPrescaleStatus          (Int_t mPrescaleStatus);
  Int_t GetPrescaleStatus         ()                                        { return fPrescaleStatus; }
  TArrayI *GetTitles              ()                                        { return &fTitles; }
  void NewPulseGT                 (Int_t nbins, Float_t xlow, Float_t xhigh );
  TH1F *GetPulseGT                ()                                        { return &fPulseGT; }
  void GetNextEventInList         (Int_t delta);
  void EstimateAll                ()                                        { SetEstimate( (Int_t) GetEntries() ); }
  void Update                     (const char *file );
  Int_t GetRun                    ()
  {
   //Return the SNO run number for this tree.
   return fRunHeader.GetRun();
  } 
  Int_t GetNumCrates              ();
  void SetRunHeader               ( Int_t *x);
  void SetRunHeader               ( QRunHeader *RunHeader )                 { fRunHeader = *RunHeader; }
  void SetCast                    ( Int_t *x);
  void SetSosl                    ( Int_t *x);
  QRunHeader *GetRunHeader        ()                                        { return &fRunHeader; }
  QTriggerBank *GetTriggerBank    ()                                        { return &fTriggerBank; }
  QCast *GetCast()                                        { return &fCast; }
  QSosl *GetSosl()                                        { return &fSosl; }
  QEvent    *SetEventAddress      (QEvent *e1) { return SetEventAddress( e1, 0 ); }  // Old style is default
  QEvent    *SetEventAddress      (QEvent *e1, Int_t no_delete );
  QEvent    *AssignEventAddress   ();
  QEvent    *GetQEvent            ();
  void       SetQEvent            (QEvent *e1) {fEvent = e1;}
  Float_t    GetRealTime          ();
  Double_t   GetLiveTime          ()                                        {return fLiveTime; }
  void       SetLiveTime          ( Double_t t )                            { fLiveTime = t; }
  Float_t    GetD2OLevel          (){return 9999;}
  TString   *GetComments          ()                                        { return &fComments; }
  Bool_t     IsFolder             ()const                                   { return kTRUE; }
  void       ReFit                ( char *options = "", Int_t maxevents = 10000 );
  Int_t      Fill                 ();
  Int_t      Fill                 (Int_t purge);
  Int_t      Flush                ();

  Int_t GetStartDate() { return fStartDate; }
  Int_t GetStartTime() { return fStartTime; }
  Int_t GetEndDate  () { return fEndDate;   }
  Int_t GetEndTime  () { return fEndTime;   }

  void SetStartDate(Int_t s) { fStartDate = s; }
  void SetStartTime(Int_t s) { fStartTime = s; }
  void SetEndDate  (Int_t s) { fEndDate   = s; }
  void SetEndTime  (Int_t s) { fEndTime   = s; }
  Bool_t IsSame(QTree *aTree, Int_t aPrintMode=0);
  ClassDef(QTree,7)//SNO event tree
private:
  Int_t      CorrelateNCDList     ();
  Int_t      LinkMuxShaper        (Int_t mux, Int_t sha, Int_t idx_mux, Int_t idx_pmux, Int_t idx_sha);
  Int_t      FastLinkMuxShaper    (QEvent *qm, QEvent *qs, QMuxScope *scope, QADC *shaper);

  QEvent        *fEvent;                   //Current event  
  Int_t          fEvent_no;                //Current event number
  QRunHeader     fRunHeader;               //SNO Run Header
  QTriggerBank   fTriggerBank;             //Trigger Bank (TRIG)
  QCast          fCast;                    //Cast Bank (CAST)
	QSosl          fSosl;                    // Sosl Bank (SOSL)
  TString        fComments;                //Text summary describing this QTree
  TH1F           fPulseGT;                 //Pulse GT Nhit spectrum
  TArrayI        fTitles;                  //TREE titles bank
  Int_t          fLocInBrowser;            //Current location in the event list
  TString        fPrescaleWeight;          //Prescale Weighting factor
  Int_t          fPrescaleStatus;          //Prescale control
  Int_t          fVersionNo;               //QTree version number
  Double_t       fLiveTime;                //Seconds of livetime

  Int_t          fStartDate, fStartTime;   //Start time
  Int_t          fEndDate,   fEndTime;     //End time
  TList         *fElist;                    //! List for delaying output of Events for NCD correlation code 
};


typedef   struct { 
  Int_t flag, stringno;
  Double_t time;
} bookkeep_t;



#endif











