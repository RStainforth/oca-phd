/////////////////////////////////////////////////////////////
//QTree                                                    //
//                                                         //
//QTree forms the basis of the data structure.  QTree      //
//derives from TTree and is made up of branches of events, //
//PMTs, OWLs, etc.                                         //
//QTree contains many useful utilities for quickly         //
//retrieving relevant parts of the data structure.         //
/////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay  06/98                        
//* Copyright (c) 1998-2001 Mark Boulay, All Rights Reserved. 

#include "QTree.h"
#include "TClonesArray.h"

#define NOSTRING -1
#define MULTSTRING -2
#define TIME_DIFF_MAX (Double_t)10.5/1e6 // maximal time window for correlated events in seconds (default: 10.5\mus)

//#define QUEUE_DEBUG  // local debugging

enum NCDEventType{
  PMT       = 0x00, // PMT only event
  SHA       = 0x01, // Shaper event
  MUX       = 0x02, // Mux Event
  pMUX      = 0x04, // partial Mux Event

  MULT_MUX  = 0x10, // more than one Mux in event (either pMux or Mux or both)

  MULT_SHA  = 0x40,  // more than one Shaper in event

  STATUS_DONE = 0x100 // internal status
};

const Int_t kForceRead = BIT(11);
ClassImp(QTree)

  QTree::QTree(const char *name, const char *title) : TTree ( name, title )
{
  //Default constructor for the event tree.
  //Inherits root TTree class, see http://root.cern.ch/root/html/TTree.html.
  //Creates a tree named t, with a buffer size of 32000 ( was 4000 originally ).
  
   PLog("QTree::QTree()",3);

  //initialize pointers
  
   fEvent          = NULL; 
   fLocInBrowser   = -1;
   fEvent_no       = -1;
   fPrescaleStatus = 0;
   fVersionNo      = 8;
   fEvent          = new QEvent(); //create event before branching tree
   fStartDate=fStartTime=fEndDate=fEndTime=0;

   Branch("Events","QEvent",&fEvent,32000,99);   
   fPulseGT.SetDirectory(0);

   fElist           = 0;
}

QTree::QTree()
{
  //Default constructor.

  fEvent_no       = -1;
  fPrescaleStatus = 0;
  fEvent          = new QEvent();
  //fBrowserList.SetName("fBrowserList");
  fStartDate=fStartTime=fEndDate=fEndTime=0;
  fPulseGT.SetDirectory(0);
  fElist           = 0;
}

QTree::~QTree(){
  //QTree destructor.

  PLog("QTree::~QTree",3);
  //if fEvent has been created then delete it. 
  if ( fEvent ) delete fEvent;

  if (fElist){
    for (TObjLink *ll = fElist->FirstLink(); ll; ll = ll->Next())
      delete (QEvent*) ll->GetObject();
    delete fElist;
  }

  fPulseGT.SetDirectory(0);
}

QTree & QTree::operator=(const QTree &rhs )
{
  // QTree assignment operator.  Only copies the tree structure and run header information, 
  // not the events.
 
  fRunHeader      = rhs.fRunHeader;
  fTriggerBank    = rhs.fTriggerBank;
  fCast           = rhs.fCast;
	fSosl           = rhs.fSosl;
  fComments       = rhs.fComments;
  fPulseGT        = rhs.fPulseGT;
  fTitles         = rhs.fTitles;
  fPrescaleWeight = rhs.fPrescaleWeight;
  fVersionNo      = rhs.fVersionNo;
  fLiveTime       = rhs.fLiveTime;
  fStartDate      = rhs.fStartDate;
  fStartTime      = rhs.fStartTime;
  fEndDate        = rhs.fEndDate;
  fEndTime        = rhs.fEndTime; 
  return *this;
}

void QTree::ClearClones()
{
  //Delete the objects in the TClonesArrays;
  fEvent->Clear();
}

#ifndef USE_ROOT_GETENTRY
Int_t QTree::GetEntry(Int_t entry, Int_t getall)
{
    // the ROOT garbage collection will double-delete the QMuxScope TH1's
    // so tell ROOT not to add the TH1's to it's directory so they won't be
    // cleaned up by the garbage collection - PH (on Aksel's advice) 11/17/04
    Bool_t addDir = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);
    Int_t rtnval = TTree::GetEntry(entry,getall);
    TH1::AddDirectory(addDir);
    return(rtnval);
}
#endif

QEvent *QTree::SetEventAddress(QEvent *e1, Int_t no_delete)
{
  //Set the Event branch address. Don't delete existing fEvent if no_delete = 1

  if ( fEvent && !no_delete ) delete fEvent;
  if( e1 ) {
    fEvent = e1;
  } else {
    fEvent = new QEvent();
  }
  SetBranchAddress( "Events", &fEvent );
  fEvent_no = -1;
  return fEvent;
}

QEvent *QTree::AssignEventAddress()
{
  // This is useful for reading chains

  SetBranchAddress( "Events", &fEvent );
  fEvent_no = -1;
  return fEvent;
}

QEvent * QTree::GetQEvent()
{
  //Return the pointer to the current event.
  return fEvent;
}

Int_t QTree::GetEvent(Int_t mEvent_no)
{
  Int_t nbytes;
    { 
      nbytes = GetEntry( mEvent_no );
    }
  fEvent_no = mEvent_no;
  return nbytes;
}

Int_t QTree::GetNextEvent()
{
  //Return the next event in this tree.
  Int_t nbytes = 0;
      if ( fEvent_no < (Int_t)GetEntries() - 1 ) nbytes = GetEvent( ++fEvent_no );
      else
	{
	  printf("End of file reached.\n");
	  nbytes = -1;
	}
  return nbytes;
}

Int_t QTree::GetPreviousEvent()
{
  //Return the previous event in this tree.
  Int_t nbytes = 0;
      if ( fEvent_no > 0 ) nbytes = GetEvent( --fEvent_no );
      else nbytes = -1;
      return nbytes;
}

void QTree::GetPreviousEventbyTrigger(char *ctrig)
{
  //Return the previous event with the given trigger type.
   GetPreviousEvent();
   while( !fEvent->HasTrig(ctrig)  && fEvent_no > 0)
     {
       GetPreviousEvent();
     }
}

void QTree::GetNextEventbyTrigger(char *ctrig)
{
  //Return the next event with the given trigger type.
   GetNextEvent();
   while( !fEvent->HasTrig(ctrig) && fEvent_no < (Int_t)GetEntries()- 1 )
     {
       GetNextEvent();
     }
}

Int_t QTree::GetNumCrates()
{
  //Return the number of crates which are masked in (in the run header).
  Int_t n = 0, i;
  for (i=0; i < 19; i++)
    {
      if ( fRunHeader.GetGTCrateMask()&(Int_t)pow(2.0,i) ) n++;
    }
  return n;
}

Int_t QTree::GetNextEvent( char *mCut )
{
  //Retrieve the next event which satisfies mCut.
  TEventList mList("mList","Next event list");
  //Output events which pass mCut into mList and increment fEvent_no by 1.
  TTree::Draw(">>mList",mCut,"",1000000000,fEvent_no+1);
  GetEvent( mList.GetEntry(0) );
  return 0;
}

Float_t QTree::GetRealTime()
{
  //Return the duration of the run contained in this
  //tree (in seconds), defined as the 10 MHz clock time
  //difference between the 10th and last-10th event.
  Int_t mEvent_no = fEvent_no;
  Float_t t0,tn;
  Float_t JD0;
  //If fEvent has not been initialized then initialize it.
  if ( !fEvent )
    {
      fEvent = new QEvent();
      //tree->SetBranchAddress("Events", &fEvent );
      SetBranchAddress("Events",&fEvent );
    }
  //Get 10th event from tree.
  GetEvent(10);
  t0 = (Float_t)fEvent->GetUT1() + ((Float_t)fEvent->GetUT2())/1e9;
  JD0 = (Float_t)fEvent->GetJulianDate();
  //Get 10th event from end.
  GetEvent( (Int_t)GetEntries() - 10 );
  tn = ( (Float_t)fEvent->GetJulianDate() - JD0)*86400 + (Float_t)fEvent->GetUT1() + (Float_t)fEvent->GetUT2()/1e9;
  GetEvent( mEvent_no );
  return (Float_t)(tn - t0);

}



//const TTree *QTree::GetTree()const{

//Returns a pointer to the TTree object (for backwards compatibility).
//  return this;
//}

void QTree::SetBrowserList(TEventList * )
{
  //Set the event list used for browsing this tree with QBrowser.
  //fBrowserList  = *mEventList;
  Warning("SetBrowserList","Obsolete"); 
  fLocInBrowser = -1;
};


//Obsolete code!
void QTree::GetNextEventInList(Int_t )
{

  //Retrieve the next event in the QTree EventList.  If delta = -1, retrieve the previous event, delta = +1, retrieve the next event.
  printf("Obsolete code!\n");
  /*
  if ( ! fBrowserList.GetN() ) return;
  if ( fLocInBrowser+delta < 0 || fLocInBrowser+delta > (fBrowserList.GetN() -1) ) return;
  fLocInBrowser += delta;
  GetEvent( fBrowserList.GetEntry( fLocInBrowser ) );
  */
}

void QTree::SetRunHeader(Int_t *x)
{
  //Set the run header for this tree.
  fRunHeader.Set( x );
}

void QTree::SetCast( Int_t *x)
{
  //Set the cast bank for this run.
  fCast.Set( x );
}

void QTree::SetSosl( Int_t *x)
{
  //Set the sosl bank for this run.
  fSosl.Set( x );
}

void QTree::NewPulseGT(Int_t nbins, Float_t xlow, Float_t xhigh)
{
  //Create the Pulse GT histogram.
 
  fPulseGT.SetName("PGT");
  fPulseGT.SetTitle("Low nhit pulse GT spectrum");
  fPulseGT.SetBins(nbins, xlow, xhigh);
}


void QTree::SetTitles(Int_t *x)
{
  //Set the Titles bank values in the QTree.
  //Begin_html
  //<l>fTitles[0] = Print flag
  //<l>fTitles[1] = Soft Nhit cut
  //<l>fTitles[2] = Nhit cut
  //<l>fTitles[3] = Low charge cut
  //<l>fTitles[4] = Prescale factor
  //End_html
 
  fTitles.Set(5,x);
}

void QTree::SetComments( char *mComments )
{
  //Set the text buffer of this tree to mComments.
  
  fComments = mComments;
}
  
Bool_t QTree::IsSame(QTree *aTree, Int_t aPrintMode){
  //Warning!!!  Always returns true.
  QEvent *save=GetQEvent(); QEvent *save2=aTree->GetQEvent();
  QEvent *qev; QEvent qevent; qev=&qevent;
  SetEventAddress(qev, 1);
  QEvent *qev2; QEvent qevent2; qev2=&qevent2;
  aTree->SetEventAddress(qev2, 1);
  int i,j;
  for(j=i=0;i<GetEntries();i++){
    if(i%5000==0)printf("QTree: IsSame Event %d\n",i);
    GetEntry(i);
    aTree->GetEntry(i);
    if(aPrintMode && !qevent.IsSame("",qevent2,aPrintMode)){
      printf("%d\n",i);
      j++; if(j>25)break; // only go for 25 errors
    }
  }
  SetEventAddress(save, 1); aTree->SetEventAddress(save2, 1);

  return kTRUE;
}


void QTree::SetPrescaleStatus(Int_t mPrescaleStatus)
{
  //Set the state of the prescale weighting.  kFalse -> no scaling, kTrue->Weight the low nhit prescaled events by prescale factor (fTitles[2]) for processing the tree with QTree::Draw().

if (mPrescaleStatus != 1)
  {
    fPrescaleStatus = 0;
    fPrescaleWeight = "";
  }else{
    fPrescaleStatus = 1;
    char *tmpchar = new char[256];
    sprintf(tmpchar,"%i*(Nhits <= %i)",GetTitles()->At(4),GetTitles()->At(1));
    fPrescaleWeight = tmpchar;
    delete tmpchar;
  }
}

//Obsolete code
void QTree::ReFit( char *, Int_t)
{
  //Refit events in this tree.  
  printf("Obsolete code!\n");
  /*
  SetBranchStatus("*",0);
  SetBranchStatus("PMTs.Pmtn",1);
  SetBranchStatus("PMTs.Pmtt",1);
  SetBranchStatus("Nhits",1);
  SetBranchStatus("nPMTs",1);
  SetBranchStatus("JulianDate",1);
  SetBranchStatus("FITs",1);
  SetBranchStatus("nFITs",1);
  QEvent *mevent = new QEvent();
  SetEventAddress(mevent);
  Int_t i;
  Int_t nentries = (Int_t)GetEntries();
  if ( fBrowserList.GetN() ) nentries = fBrowserList.GetN();
  if ( nentries > maxevents ) nentries = maxevents;
    {
      for (i=0; i < nentries; i++)
	{
	  if (fBrowserList.GetN() )GetEvent( fBrowserList.GetEntry( i ) );
	  else GetEvent(i);
	  mevent->ReFit();
	}
    }
  delete mevent;
  //tree->SetBranchStatus("*",1);
  SetBranchStatus("*",1);
  */
}

void QTree::Update( const char *file )
{
  //Rescan the tree, recompute nhits based on DQXX banks.

  Int_t nentries = (Int_t)GetEntries();
  QEvent *oldevent = GetQEvent();
  QEvent *event = new QEvent();
  QEvent *newevent = new QEvent();
  SetEventAddress(event);
  TFile f2(file,"CREATE");
  //Create a new tree T2 with name and title of this QTree object.
  QTree *T2 = new QTree(GetName(), GetTitle());
  *T2 = *this;
  T2->SetEventAddress(newevent);
  Int_t i,j;
  QPMT *pmt = 0;
  for (i=0; i < nentries; i++)
    {
      printf("Event %i\n",i);
      GetEvent(i);
      *newevent = *event;
      newevent->GetQPMTs()->Clear(); 
      newevent->SetnPMTs(0);
      for (j=0; j < event->GetnPMTs(); j++)
	{
	  pmt = event->GetPMT(j);
          //If jth pmt exists and is on then add jth pmt to event.
	  if (pmt) { if ( pmt->IsTubeOn() ) newevent->AddQPMT( pmt ); }
	}
        //If newevent has greater than 0 pmts then set Nhits, see QEvent class.
        if ( newevent->GetnPMTs() > 0 ) newevent->SetNhits( newevent->GetQPMTs()->GetEntriesFast() );
	T2->Fill();
    }
  T2->Write();
  T2->Flush();
  f2.Close();
  SetEventAddress(oldevent);
  delete event;
}

//______________________________________________________________________________
void QTree::Streamer(TBuffer &R__b)
{
   // Stream an object of class QTree.  This streamer has been updated for the new ROOT 3 automatic I/O,
   // and is backwards-compatible for pre-version 3 ROOT files.
   // See /root/cern.ch/root/html/TBuffer.html

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
     if ( R__v > 4 )
       {
	 //New ROOT 3 input streamer
	 QTree::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
	 fPulseGT.SetDirectory(0);
	 return;
       }
     else{
       //Old home-made streamer
       TTree::Streamer(R__b);
       R__b >> fEvent;
       R__b >> fEvent_no;
       fRunHeader.Streamer(R__b);
       fTriggerBank.Streamer(R__b);
       if ( R__v > 2 )
	 {
	   fCast.Streamer(R__b);
	 }
       else
	 {
	   QThresholdBank mBank;
	   mBank.Streamer(R__b);
	 }
       fComments.Streamer(R__b);
       fPulseGT.Streamer(R__b);
       fTitles.Streamer(R__b);
       TEventList browser;
       browser.Streamer(R__b);
       
       //Remove items from gDirectory
       if ( gDirectory )
	 {
	   gDirectory->GetList()->Remove( (TObject *)&fPulseGT );
	   gDirectory->GetList()->Remove( (TObject *)&browser );
	 }
       R__b >> fLocInBrowser;
       fPrescaleWeight.Streamer(R__b);
       R__b >> fPrescaleStatus;
       R__b >> fVersionNo;
       if (R__v > 1)
	 {
	   R__b >> fLiveTime;
	 }
       else
	 {
	   fLiveTime = 0;
	 }
       if ( R__v > 3 )
	 {
	   R__b >> fStartDate;
	   R__b >> fStartTime;
	   R__b >> fEndDate;
	   R__b >> fEndTime;
	 }
       else
	 {
	   fStartDate=fStartTime=fEndDate=fEndTime=0;
	 }
     }
     } else {
     //New ROOT 3 write block
     QTree::Class()->WriteBuffer(R__b, this);
   }
}
	  
 

//______________________________________________________________________________
Int_t QTree::Fill(){
  // QTree overload of TTree::Fill() for adding NCD correlation information to 
  // QTree. (ckrauss 1/26/2005)
  // 03/21/2006:  Previous version had Fill(0).  With NECL bank info in SNOMAN
  //              this is switched back to purge=2. (A. Poon)
  return Fill(2);
}

//______________________________________________________________________________
Int_t QTree::Flush(){
  //Flush has to be called once before the file is closed, otherwise the events in 
  //the event list will be lost.
  return Fill(1);
}

//______________________________________________________________________________
Int_t QTree::Fill(Int_t purge){
  // New Fill routine that keeps events in list until a mux shaper association
  // is found. Fill(2) switches this off (useful for copying runs that already have 
  // Mux shaper relations set or to save time in processing when it is not needed).
  // In ncd standalone runs (run number > 100000) the mux-shaper correlation is not 
  // set (because of missing timing information).

  if (purge == 2 || GetRunHeader()->GetRun() > 100000) // skip correlation for ncd standalone runs
    // regular filling without looking for mux shaper correlations
    return TTree::Fill();
  

  if (!fElist) fElist = new TList;
  int done = 0, rv = 0;

  if (purge == 0){
    //    *last = *fEvent;
    fElist->AddLast(fEvent);
    fEvent = new QEvent();
    if (!fEvent) {
      fprintf(stderr, " QTree: Correlation List, not enough memory error.\n");
      fprintf(stderr, " QTree: Events might have been written out of order!\n");
      return TTree::Fill();
    }
    // Loop over List...
    done = CorrelateNCDList();
  }
  else
    // purge is 1 here and list events are written out  because run is ending
    done = fElist->GetSize();
  
  // Write events that are 'done' to TTree
  for (int i = 0; i < done; i++){
    QEvent *out = (QEvent*) fElist->First();
    QEvent *tmp = fEvent;
    if (out == NULL ) {printf (" DEBUG: We should never be here...\n");break;}
    fEvent = out;
    SetBranchAddress( "Events", &fEvent );
    rv += TTree::Fill();
    delete (QEvent*) fElist->Remove(fElist->FirstLink()); 
    fEvent = tmp;
  }
  SetBranchAddress( "Events", &fEvent );

  return rv;
}

//______________________________________________________________________________
Int_t QTree::CorrelateNCDList(){
  // Adding NCD event correlation to root tree

#ifdef QUEUE_DEBUG
  static int max_queue = 0;
#endif

  Int_t good = 0; // number of events that are done

  bookkeep_t *bookkeep = new bookkeep_t[fElist->GetSize()];
  if (bookkeep == NULL){
    fprintf(stderr, " QTree::CorrelateNCDList() no memory error.\n");
    fprintf(stderr, " Not assigning MUX shaper correlations for %d events.\n", fElist->GetSize());
    return fElist->GetSize();
  }

  Int_t num = 0;
  Double_t stjd = 0, last_t = 0;
  UInt_t last_gtid = 0xffffffff;
  for (TObjLink *ll = fElist->FirstLink(); ll; ll = ll->Next()){
    QEvent *qe = (QEvent*) ll->GetObject();
    if (num == 0) stjd = qe->GetJulianDate();
    Double_t now = (qe->GetJulianDate()-stjd)*86400 +qe->GetUT1() + qe->GetUT2()/1e9; // time in seconds

    bookkeep[num].flag     = PMT;
    bookkeep[num].stringno = NOSTRING;
    bookkeep[num].time     = now;

    // sort out pathological events (PMT orphan spews)
    UInt_t gtid = (UInt_t) qe->GetEvent_id();
    if (last_gtid == gtid && gtid == 0) {
      bookkeep[num].flag |= STATUS_DONE;
      if (num > 0 && !(bookkeep[num-1].flag & STATUS_DONE))
	bookkeep[num-1].flag |= STATUS_DONE;
    }
    last_gtid = gtid;
      


    // Now find mux and shaper events in list
    if (qe->GetnPartialMuxScopes() > 0){
      bookkeep[num].flag |= pMUX; // partial MUX event
      if (qe->GetnPartialMuxScopes() > 1){
	bookkeep[num].flag |= MULT_MUX; // Multiple partial MUXes
	bookkeep[num].stringno = MULTSTRING; // Multiple Strings in event
      }
      if (bookkeep[num].stringno == NOSTRING)
	bookkeep[num].stringno = qe->GetPartialMuxScope(0)->GetNCDStringNumber();
    }
    if (qe->GetnMuxScopes() > 0){
      bookkeep[num].flag |= MUX; // MUX event
      if (qe->GetnMuxScopes() > 1){
	bookkeep[num].flag |= MULT_MUX; // Multiple MUXes
	bookkeep[num].stringno = MULTSTRING; // Multiple Strings in event
      }
      if (bookkeep[num].stringno == NOSTRING)
	bookkeep[num].stringno = qe->GetMuxScope(0)->GetNCDStringNumber();
    }
    if (qe->GetnShapers() > 0) {
      bookkeep[num].flag |= SHA; // Shaper event
      if (qe->GetnShapers() > 1){
	bookkeep[num].flag |= MULT_SHA; // Multiple shapers
	bookkeep[num].stringno = MULTSTRING; // Multiple Strings in event
      }
      if (bookkeep[num].stringno == NOSTRING)
	bookkeep[num].stringno = qe->GetShaper(0)->GetNCDStringNumber();
    }
    if ((bookkeep[num].flag & pMUX) && (bookkeep[num].flag & MUX)) {
      bookkeep[num].flag |= MULT_MUX; // multiple mux (both partial and full)
      bookkeep[num].stringno = MULTSTRING;
    }
    last_t = now;
    num++;
  }


  // the marked events are correlated here by using the bookkeep structure that was just filled
  for (int mux = 0 ; mux < num ; mux++){
    if (bookkeep[mux].flag == PMT || bookkeep[mux].flag & STATUS_DONE )
      bookkeep[mux].flag |= STATUS_DONE; // PMT only events or shapers already correlated
    else{
      if (bookkeep[mux].flag & MUX || bookkeep[mux].flag & pMUX) { //MUX event looking for correlated shaper
	for (int sha = mux+1 ; sha < num; sha++){
	  if (bookkeep[sha].flag & SHA){
	    // most events will not have multiple entries -> they get the fast treatment
	    if (!(bookkeep[sha].flag & MULT_SHA) && !(bookkeep[mux].flag & (MULT_MUX))){ // no multiple events
	      if (bookkeep[mux].stringno == bookkeep[sha].stringno){ // Found correlation!
		if (bookkeep[sha].time-bookkeep[mux].time < TIME_DIFF_MAX){ // less than 10.5 ms between mux and shaper
		  if (bookkeep[mux].flag & MUX)
		    LinkMuxShaper(mux, sha, 0, -1, 0); // link a MUX to a shaper
		  if (bookkeep[mux].flag & pMUX)
		    LinkMuxShaper(mux, sha, -1, 0, 0); // link a partial MUX to a shaper
		  bookkeep[mux].flag |= STATUS_DONE;
		  bookkeep[sha].flag |= STATUS_DONE;
		}
	      }
	    }
	    else{
	      // Now events with multiple shapers/muxes
	      QEvent *qs = (QEvent*)fElist->At(sha);
	      QEvent *qm = (QEvent*)fElist->At(mux);
	      // multiple muxes
	      for (int q = 0 ; q < qm->GetnPartialMuxScopes() ; q++){
		QMuxScope *scope = qm->GetPartialMuxScope(q);
		for (int k = 0 ; k < qs->GetnShapers() ; k++){
		  QADC *shaper = qs->GetShaper(k);
		  if (shaper->GetNCDStringNumber() == scope->GetNCDStringNumber() 
		      && bookkeep[sha].time-bookkeep[mux].time < TIME_DIFF_MAX){
		    FastLinkMuxShaper(qm, qs, scope, shaper); 
		  }
		}
	      }
	      for (int q = 0 ; q < qm->GetnMuxScopes() ; q++){
		QMuxScope *scope = qm->GetMuxScope(q);
		for (int k = 0 ; k < qs->GetnShapers() ; k++){
		  QADC *shaper = qs->GetShaper(k);
		  if (shaper->GetNCDStringNumber() == scope->GetNCDStringNumber() 
		      && bookkeep[sha].time-bookkeep[mux].time < TIME_DIFF_MAX){
		    FastLinkMuxShaper(qm, qs, scope, shaper); 
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    // check, if events are in queue for too long or can not be correlated anyway
    if (!(bookkeep[mux].flag & STATUS_DONE)){
      if (bookkeep[mux].flag & (MUX+pMUX)) {// mux event
	if (bookkeep[num-1].time - bookkeep[mux].time >= TIME_DIFF_MAX){ // time difference to last event in list is .gt. 10.5ms
	  bookkeep[mux].flag |= STATUS_DONE;
	}
      }
      else // not a mux event and a
	if (bookkeep[mux].flag & SHA) {// shaper event -> is done by now...
	  bookkeep[mux].flag |= STATUS_DONE;
	}
    }
    // debuging output
#ifdef QUEUE_DEBUG
    //  if ((bookkeep[mux].flag & 0x0ff) != PMT || num > 5)
    //    printf(" Event %d of %d (flag: %3hd, string %d, dt: %f, t0: %f, GTID: %d)\n", mux, num, bookkeep[mux].flag,  
    //   bookkeep[mux].stringno, 1e6*(bookkeep[mux].time-bookkeep[0].time), bookkeep[0].time, ((QEvent*)fElist->At(mux))->GetEvent_id());
    if (max_queue < num)
      max_queue = num;
#endif
  }
  
  // Mark the first 'good' events for writing to file.
  for (int i = 0 ; i < num ; i++)
     if (bookkeep[i].flag & STATUS_DONE) good++;
    else break;
 
  delete bookkeep;
#ifdef QUEUE_DEBUG
  if (good == num && num > 3)
    cout << " DEBUG: QUEUE of " << num <<  " emptied: maximum events now: " << max_queue << " events " << endl;
#endif

  return good;
}

//______________________________________________________________________________
Int_t QTree::LinkMuxShaper(Int_t mux, Int_t sha, Int_t idx_mux, Int_t idx_pmux, Int_t idx_sha){
  // Private function used by CorrelateNCDList().
  QEvent *qs = (QEvent*)fElist->At(sha);
  QEvent *qm = (QEvent*)fElist->At(mux);
  QMuxScope *scope;
  QADC *shaper = qs->GetShaper(idx_sha);
  if (idx_pmux > -1) scope = qm->GetPartialMuxScope(idx_pmux);
  if (idx_mux > -1) scope = qm->GetMuxScope(idx_mux);

  return FastLinkMuxShaper(qm, qs, scope, shaper);
}


//______________________________________________________________________________
Int_t QTree::FastLinkMuxShaper(QEvent *qmux, QEvent *qshaper, QMuxScope *scope, QADC *shaper){
  // Private function used by CorrelateNCDList().

  if ((UInt_t)scope->GetCorrShaperGTID() == 0xFFFFFFFF && (UInt_t)shaper->GetCorrMUXGTID() == 0xFFFFFFFF){
    Double_t tdiff = (qshaper->GetJulianDate()-qmux->GetJulianDate()) * 24 * 3600 
      + (qshaper->GetUT1()-qmux->GetUT1()) 
      + (qshaper->GetUT2()-qmux->GetUT2())/1e9;
    scope->SetCorrShaperGTID(qshaper->GetEvent_id());
    scope->SetCorrShaperADCCharge(shaper->GetADCCharge());
    scope->SetCorrShaperDeltaT(Float_t(tdiff));
    shaper->SetCorrMUXGTID(qmux->GetEvent_id());
    shaper->SetCorrMUXDeltaT(-1 * Float_t(tdiff));
    //   printf (" DEBUG: LINK SET! String %d \n", scope->GetNCDStringNumber() );
    return 0;
  }
  else 
    return 1;
  
}











