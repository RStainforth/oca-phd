////////////////////////////////////////////////////////////
// Event class which contains all information for a       //
// single physical event.  Each event contains            //
// TClonesArrays of PMTs, OWLs, etc.                      //
// M. Boulay   06/98                                      //
////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay  
// Copyright (c) 1998-2001 Mark Boulay, All Rights Reserved.                        

#include "QEvent.h"
#include <string.h>
#include "QMath.h"
#include "QMuonFit.h"
#include "QNcdNreEvent.h"
#include "TClonesArray.h"
#include "TMath.h"

ClassImp(QEvent)

#ifndef NO_SOLAR
extern "C" void  solar_(double & , float  *, float &);
#endif

QEvent::QEvent()
{
  // Default constructor calls Initialize().

  ZapFields();
  nPBUNs = 0;
  tNext  = tPrev = 0;
  NCDStatus = NumMuxG = NumMux = NumScope = NCDClock1 = NCDClock2 = NCDLatchRegister = NCDGTID = NCDSync=nMuonFits= MCEventNumber= 0;

  InitializeAll();
}

QEvent::QEvent(QEvent &anEvent):QSNO(anEvent)
{
  // Copy constructor for QEvent objects.

  InitializeAll();                                  // We want to force the creation of 
                                                    // _new_ TClonesArrays here
  *this = anEvent; //  
}

QEvent::QEvent(TH1 &aHistogram)
{
  // Create an event for display with QSnoed in which each element
  // of a histogram is assigned to a pmt.  Normally this only makes 
  // sense for histograms with 1 channel per pmt.

  InitializeAll();                 // We want _new_ TClonesArrays here

  QPMT *pmt;
  Int_t Nhits, pmtno;
  Nhits = aHistogram.GetNbinsX();
  SetEvent_id     (0);
  SetTrig_type    (1);
  SetNhits        (Nhits);
  SetNph          (0);
  SetGtr_time     ((Double_t)0);
  SetDataSet      (0);
  for(Int_t j=0; j < Nhits; j++)
  {
    pmt = AddQPMT();                          // This will set nPMTs as well
    pmt->SetCell(1);
    pmt->Sethl(aHistogram.GetBinContent(j));
    pmtno = j;
    pmt->Setn(pmtno);
    pmt->SetStatusBit(3);
  }
	nPBUNs = nPMTs;
} 


QEvent & QEvent::operator=(QEvent &rhs)
{
  // QEvent assignment operator.

  Initialize();                       // We want to stuff an existing object here. So - if the
                                      // the TClonesArrays exist we use them. If not we make
                                      // new ones

  Clear();
  // We want to overwrite ...

  Event_id   = rhs.Event_id;
  Trig_type  = rhs.Trig_type;
  Nhits      = rhs.Nhits;
  Nph        = rhs.Nph;
  tPrev      = rhs.tPrev;
  tNext      = rhs.tNext;
  Gtr_time   = rhs.Gtr_time;
  JulianDate = rhs.JulianDate;
  UT1        = rhs.UT1;
  UT2        = rhs.UT2;
  Run        = rhs.Run;
  DamnID     = rhs.DamnID;
  DamnID1    = rhs.DamnID1;
  DarnID     = rhs.DarnID;
  EsumPeak   = rhs.EsumPeak;
  EsumDiff   = rhs.EsumDiff;
  EsumInt    = rhs.EsumInt;
  Time10MHz  = rhs.Time10MHz;
  TrigErr    = rhs.TrigErr;
  DataSet    = rhs.DataSet;
  NCDStatus  = rhs.NCDStatus;
  NumMuxG    = rhs.NumMuxG;
  NumMux     = rhs.NumMux;
  NumScope   = rhs.NumScope;
  NCDClock1  = rhs.NCDClock1;
  NCDClock2  = rhs.NCDClock2;
  NCDLatchRegister  = rhs.NCDLatchRegister;
  NCDGTID    = rhs.NCDGTID;
  NCDSync    = rhs.NCDSync;
  nPBUNs     = rhs.nPBUNs;

  Int_t j;
  QPMT *pmt; 
  QMCVX *mcvx;
  QFit *fit;
  QRSP *rsp;
  QFTK *ftk;

  for (j=0; j < rhs.nPMTs; j++) {
    pmt = rhs.GetPMT(j);
    AddQPMT(pmt);
  }
  for (j=0; j < rhs.nOWLs; j++) {
    pmt = rhs.GetOWL(j);
    AddQOWL(pmt);
  }
  for (j=0; j < rhs.nLGs; j++) {
    pmt = rhs.GetLG(j);
    AddQLG(pmt);
  }
  for (j=0; j < rhs.nBUTTs; j++) {
    pmt = rhs.GetBUTT(j);
    AddQBUTT(pmt);
  }
  for (j=0; j < rhs.nNECKs; j++) {
    pmt = rhs.GetNECK(j);
    AddQNECK(pmt);
  }
  for (j=0; j < rhs.nFECDs; j++) {
    pmt = rhs.GetFECD(j);
    AddQFECD(pmt);
  }
  for (j=0; j < rhs.nSPAREs; j++) {
    pmt = rhs.GetSPARE(j);
    AddQSPARE(pmt);
  }
  for (j=0; j < rhs.nMCPMs; j++) {
    pmt = rhs.GetMCPM(j);
    AddQMCPM(pmt);
  }
  for (j=0; j < rhs.nMCVXs; j++) {
    mcvx = rhs.GetMCVX(j);
    AddQMCVX(mcvx);
  }
  for (j=0; j < rhs.nFITs; j++) {
    fit = rhs.GetFit(j);
    AddQFIT(fit);
  }
  for (j=0; j < rhs.nMuonFits; j++) {
    QMuonFit *fit = rhs.GetMuonFit(j);
    AddMuonFit(fit);
  }
  for (j=0; j < rhs.nRSPs; j++) {
    rsp = rhs.GetRSP(j);
    AddQRSP(rsp);
  }
  for (j=0; j < rhs.nFTKs; j++) {
    ftk = rhs.GetFTK(j);
    AddQFTK(ftk);
  }
  for (j=0; j < rhs.nShapers; j++){
    QADC *shaper=rhs.GetShaper(j);
    AddShaper(shaper);
  }
  for (j=0; j < rhs.nMuxScopes; j++){
    QMuxScope *muxscope=rhs.GetMuxScope(j);
    AddMuxScope(muxscope);
  }

  for (j=0; j < rhs.nPartialMuxScopes; j++){
    QMuxScope *muxscope=rhs.GetPartialMuxScope(j);
    AddPartialMuxScope(muxscope);
  }

  for(j=0; j < rhs.nGlobalShapers; j++){
    QGlobalShaper *globalshaper = rhs.GetGlobalShaper(j);
    AddGlobalShaper(globalshaper);
  }
  
  // Copy the fBits word.
  for (j=0; j < 32; j++) {
    if (rhs.TestBit(j)) SetBit(j,kTRUE);
  }
  
  return *this;
}

QEvent::QEvent(Int_t *ivars, Float_t *rvars, Double_t *dvars)
{
  // Constructor which clears the TClonesArrays in the event and then sets non-
	// TClonesArray part of the event
	// Calling code needs to call AddQPMT to fill TClonesArrays.

  Clear();
  Set(ivars, rvars, dvars);
};

void QEvent::Set(Int_t *ivars, Float_t *rvars, Double_t *dvars)
{
  // Set the non-TClonesArray part of the event data. 

  // Calling code must take care of (re-)initializing the 
	// TClonesArrays and their associated counters

	// The arrays rvars and dvars fill the event data as follows:
  //Begin_html

  //<l>Event_id   = ivars[0];
  //<l>Trig_type  = ivars[1];
  //<l>Nhits      = ivars[2];
  //<l>JulianDate = ivars[3];
  //<l>UT1        = ivars[4];
  //<l>UT2        = ivars[5]; 
  //<l>Run        = ivars[6]; 
  //<l>DamnID     = ivars[7];
  //<l>DamnID1    = ivars[8];
  //<l>EsumPeak   = ivars[9];
  //<l>EsumDiff   = ivars[10];
  //<l>EsumInt    = ivars[11];
  //<l>DarnID     = ivars[12];
  //<l>TrigErr    = ivars[13]; 
  //<l>DataSet    = ivars[14];
  //<l>NCDStatus  = ivars[15];
  //<l>NumMuxG    = ivars[16];
  //<l>NumMux     = ivars[17];
  //<l>NumScope   = ivars[18];
  //<l>NCDClock1  = ivars[19];
  //<l>NCDClock2  = ivars[20];
  //<l>NCDLatchRegister= ivars[21];
  //<l>NCDGTID    = ivars[22];
  //<l>NCDSync    = ivars[23];
  //<l>MCEventNumber= ivars[24];
  //<l>Nph        = rvars[0]; 
  //<l>Gtr_time   = dvars[0];
  //<l>Time10MHz  = dvars[1];
  //End_html

  Event_id   =              ivars[0];
  Trig_type  =              ivars[1];
  Nhits      = (Short_t)    ivars[2];
  Nph        =              rvars[0];
  Gtr_time   =              dvars[0];
  Time10MHz  =              dvars[1];
  JulianDate =              ivars[3];
  UT1        =              ivars[4];
  UT2        =              ivars[5]; 
  Run        =              ivars[6];
  DamnID     =              ivars[7];
  DamnID1    =              ivars[8];
  EsumPeak   =              ivars[9];
  EsumDiff   =              ivars[10];
  EsumInt    =              ivars[11]; 
  DarnID     =              ivars[12];
  TrigErr    =              ivars[13];
  DataSet    =              ivars[14]; 
  NCDStatus  = ivars[15];
  NumMuxG    = ivars[16];
  NumMux     = ivars[17];
  NumScope   = ivars[18];
  NCDClock1  = ivars[19];
  NCDClock2  = ivars[20];
  NCDLatchRegister= ivars[21];
  NCDGTID    = ivars[22];
  NCDSync    = ivars[23];
  MCEventNumber    = ivars[24];
}

QEvent::~QEvent()
{
  // Destructor clears the event arrays.
  PLog("QEvent::~QEvent()",3);
  Clear();
  if( PMTs ) delete PMTs;      PMTs   = 0;
  if( OWLs ) delete OWLs;      OWLs   = 0;
  if( LGs ) delete LGs;        LGs    = 0;
  if( BUTTs ) delete BUTTs;    BUTTs  = 0;
  if( NECKs ) delete NECKs;    NECKs  = 0;
  if( FECDs ) delete FECDs;    FECDs  = 0;
  if( SPAREs ) delete SPAREs;  SPAREs = 0;
  if( MCPMs ) delete MCPMs;    MCPMs  = 0;
  if( MCVXs ) delete MCVXs;    MCVXs  = 0;
  if( FITs ) delete FITs;      FITs   = 0;
  if( RSPs ) delete RSPs;      RSPs   = 0;
  if( FTKs ) delete FTKs;      FTKs   = 0;
  if( Shapers) delete Shapers; Shapers =0;
  if(MuxScopes) delete MuxScopes; MuxScopes = 0;
  if(PartialMuxScopes) delete PartialMuxScopes; PartialMuxScopes = 0;
  if(GlobalShapers) delete GlobalShapers; GlobalShapers=0;
  if(MuonFits)delete MuonFits; MuonFits=0;
}

void QEvent::InitializeAll()
{
  // Creates _new_ TClonesArrays and initializes counters

  PMTs   = NULL;
  OWLs   = NULL;
  LGs    = NULL;
  BUTTs  = NULL;
  NECKs  = NULL;
  FECDs  = NULL;
  SPAREs = NULL;
  MCPMs  = NULL;
  MCVXs  = NULL;
  FITs   = NULL;
  RSPs   = NULL;
  FTKs   = NULL;
  Shapers = NULL;
  MuxScopes = NULL;
  PartialMuxScopes = NULL;
  GlobalShapers = NULL;
  MuonFits = NULL;

  nPMTs  = nOWLs = nLGs = nBUTTs = nNECKs = nFECDs = nSPAREs = nMCPMs = nMCVXs = nFITs = nRSPs = nFTKs =
    nShapers = nMuxScopes = nPartialMuxScopes = nGlobalShapers = 0;

	Initialize();
}


void QEvent::Initialize()
{
 // Creates TClonesArrays ( if they don't already exists ).

 PLog("QEvent::Initialize",3);
 if (!PMTs)      PMTs   = new TClonesArray("QPMT",10000);
 if (!OWLs)      OWLs   = new TClonesArray("QPMT",200);
 if (!LGs)       LGs    = new TClonesArray("QPMT",1000);
 if (!BUTTs)     BUTTs  = new TClonesArray("QPMT",100);
 if (!NECKs)     NECKs  = new TClonesArray("QPMT",100);
 if (!FECDs)     FECDs  = new TClonesArray("QPMT",10);
 if (!SPAREs)    SPAREs = new TClonesArray("QPMT",100);
 if (!MCPMs)     MCPMs  = new TClonesArray("QPMT",10000);
 if (!MCVXs)     MCVXs  = new TClonesArray("QMCVX",10000);
 if (!FITs)      FITs   = new TClonesArray("QFit",30);
 if (!MuonFits)  MuonFits=new TClonesArray("QMuonFit",3);
 if (!RSPs)      RSPs   = new TClonesArray("QRSP",30); 
 if (!FTKs)      FTKs   = new TClonesArray("QFTK",30); 
 if (!Shapers)   Shapers= new TClonesArray("QADC",10);
 if (!MuxScopes) MuxScopes= new TClonesArray("QMuxScope",10);
 if (!PartialMuxScopes) PartialMuxScopes= new TClonesArray("QMuxScope",10);
 if (!GlobalShapers) GlobalShapers = new TClonesArray("QGlobalShaper",10);
 
}

void QEvent::Calibrate()
{
  // Calibrate this event with the SNODB calibration constants.
  if (!gCal)
  {
    gCalibrator = new QSnoCal("read");
    gCalibrator->SetMode(kDynamicConstants);
  }
  gCalibrator->Calibrate(this);
}

Int_t QEvent::GetData(Int_t *aDataArray, Int_t aMaximum)
{
  // Fill the data array with this event info.  If maximum is specified
  // and the number of words needed to store this event is greater than
  // maximum, then return -1, otherwise return 0.

	// ************************************************************************************
	//
	// Don't change the order here unless you update the corresponding code in snomanpp !!!
	// And don't forget to update the constants used for calculating ndata
	//
	// ************************************************************************************
	//

  if (!aDataArray) return -1;

	Int_t l, lmax;
	lmax = ( (Int_t)kDefaultWaveformSize + sizeof(int) - 1 ) / sizeof(int);

	// ndata is the number of words in the buffer ( excluding ndata itself ! )

  Int_t ndata = 30 + 13                                                     // QEvent 
    + 16 * (nPMTs + nOWLs + nLGs + nBUTTs + nNECKs + nFECDs + nSPAREs)      // QPMT
    + ( 23 + QFIT_MAX_FIT_OUTPUTS + 1 + 8 + 1 + QFIT_MAX_BETAS +            // QFit
        1 + QFIT_MAX_RESIDUAL_HARMONICS + 1 + QFIT_MAX_RESIDUALS +
        1 + QFIT_MAX_LEGENDRES + 1 + QFIT_MAX_ANG_CORRELATIONS + 10 ) * nFITs
    + 41 * nRSPs   
    + 12 * nFTKs  
    + 10 * nShapers
    + 2 * nGlobalShapers
    + ( lmax + 21 ) * (nMuxScopes + nPartialMuxScopes);

	//
	// Note that the above value is the _expected_ size of the event. If the event contains
	// partial muxscopes without scope traces the code below will adjust ndata to the _actual_
	// size of the event.
	// 

	// There is no SNOMAN equivalent of the fMuxChannelNumber so allocate 21 words only for
	// the muxscopes

  if ( (aMaximum > 0 ) && ( ndata > aMaximum) )return -1;  // Compare with the _expected_ size of
  Int_t i = 0, j, jmax, n, pmt_type, bit = 0;	             // the event

	Float_t temp;

  QPMT *pmt = NULL;
  QFit *fit = NULL;
  QRSP *rsp = NULL;
  QFTK *ftk = NULL;
  QADC *qadc = NULL;
  QGlobalShaper *qglobalshaper = NULL;

  aDataArray[i++]                 = ndata;
  aDataArray[i++]                 = Event_id;
  aDataArray[i++]                 = Trig_type;
  aDataArray[i++]                 = Nhits;
  *(Float_t *) (&aDataArray[i++]) = Nph;
  *(Double_t *)(&aDataArray[i++]) = Gtr_time;
  i++;
  aDataArray[i++]                 = JulianDate;
  aDataArray[i++]                 = UT1;
  aDataArray[i++]                 = UT2;
  aDataArray[i++]                 = Run;
  aDataArray[i++]                 = DamnID;
  aDataArray[i++]                 = DamnID1;
  aDataArray[i++]                 = DarnID;
  aDataArray[i++]                 = EsumPeak;
  aDataArray[i++]                 = EsumDiff; 
  aDataArray[i++]                 = EsumInt;
  aDataArray[i++]                 = DataSet;
	aDataArray[i++]                 = TrigErr;

  if (TestBit(kCalibrate))    bit =  kCalibrate;
  if (TestBit(kFitFTT))       bit |= kFitFTT;

  aDataArray[i++] = bit;
  aDataArray[i++] = nPMTs;
  aDataArray[i++] = nOWLs;
  aDataArray[i++] = nLGs;
  aDataArray[i++] = nBUTTs;
  aDataArray[i++] = nNECKs;
  aDataArray[i++] = nFECDs;
  aDataArray[i++] = nSPAREs;
  aDataArray[i++] = nFITs;
  aDataArray[i++] = nRSPs;
  aDataArray[i++] = nFTKs;

	aDataArray[i++] = NCDStatus;
	aDataArray[i++] = NumMuxG;
	aDataArray[i++] = NumMux;
	aDataArray[i++] = NumScope;
	aDataArray[i++] = NCDClock1;
	aDataArray[i++] = NCDClock2;
	aDataArray[i++] = NCDLatchRegister;
	aDataArray[i++] = NCDGTID;
	aDataArray[i++] = NCDSync;
	aDataArray[i++] = nShapers;
  aDataArray[i++] = nMuxScopes;
  aDataArray[i++] = nPartialMuxScopes;
  aDataArray[i++] = nGlobalShapers;

  for (pmt_type=0; pmt_type < 7; pmt_type++) {
    if         (pmt_type == 0)    jmax = nPMTs;
    else if    (pmt_type == 1)    jmax = nOWLs;
    else if    (pmt_type == 2)    jmax = nLGs;
    else if    (pmt_type == 3)    jmax = nBUTTs;
    else if    (pmt_type == 4)    jmax = nNECKs;
    else if    (pmt_type == 5)    jmax = nFECDs;
    else                          jmax = nSPAREs;

    for (j=0; j < jmax; j++) {
      if         (pmt_type == 0)    pmt = GetPMT(j);
      else if    (pmt_type == 1)    pmt = GetOWL(j);
      else if    (pmt_type == 2)    pmt = GetLG(j);
      else if    (pmt_type == 3)    pmt = GetBUTT(j);
      else if    (pmt_type == 4)    pmt = GetNECK(j);
      else if    (pmt_type == 5)    pmt = GetFECD(j);
      else                          pmt = GetSPARE(j);

      aDataArray[i++]                = pmt->Getn();
      aDataArray[i++]                = pmt->GetCell();
      aDataArray[i++]                = pmt->GetStatus();
      aDataArray[i++]                = pmt->GetCMOSStatus();
      *(Float_t *)(&aDataArray[i++]) = (float)pmt->Getihl();
      *(Float_t *)(&aDataArray[i++]) = (float)pmt->Getihs();
      *(Float_t *)(&aDataArray[i++]) = (float)pmt->Getilx();
      *(Float_t *)(&aDataArray[i++]) = (float)pmt->Getit();
      *(Float_t *)(&aDataArray[i++]) = pmt->Gethl();
      *(Float_t *)(&aDataArray[i++]) = pmt->Geths();
      *(Float_t *)(&aDataArray[i++]) = pmt->Getlx();
      *(Float_t *)(&aDataArray[i++]) = pmt->Gett();
      *(Float_t *)(&aDataArray[i++]) = pmt->Getdelta();
      *(Float_t *)(&aDataArray[i++]) = pmt->Gettslh();
			aDataArray[i++]                = pmt->GetEcaVal();
			*(Float_t *)(&aDataArray[i++]) = pmt->Getrc();
    }
  }

  for (j=0; j < nFITs; j++) {
    fit = GetFit(j);
   *(Float_t *)(&aDataArray[i++]) = fit->GetX();
   *(Float_t *)(&aDataArray[i++]) = fit->GetY();
   *(Float_t *)(&aDataArray[i++]) = fit->GetZ();
   *(Float_t *)(&aDataArray[i++]) = fit->GetU();
   *(Float_t *)(&aDataArray[i++]) = fit->GetV();
   *(Float_t *)(&aDataArray[i++]) = fit->GetW();
   *(Float_t *)(&aDataArray[i++]) = fit->GetEnergy();
   *(Float_t *)(&aDataArray[i++]) = fit->GetTime();
    aDataArray[i++]               = fit->GetNumPMTsUsed();
   *(Float_t *)(&aDataArray[i++]) = fit->GetQualityOfFit();
    aDataArray[i++]               = fit->GetIndex();
    aDataArray[i++]               = fit->GetIntOK();
    aDataArray[i++]               = fit->GetNumIter();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC11();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC12();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC13();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC14();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC22();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC23();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC24();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC33();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC34();
   *(Float_t *)(&aDataArray[i++]) = fit->GetC44();

   i++;
   n = 0;
   for (Int_t m=0; m < QFIT_MAX_FIT_OUTPUTS; m++ ) {
     temp = fit->GetFitOutput()[m];
     if( temp > -9990. ) ++n;
     *(Float_t *)(&aDataArray[i++]) = temp;
   }
	 *(Float_t *)(&aDataArray[i-QFIT_MAX_FIT_OUTPUTS-1]) = n;

	 *(Float_t *)(&aDataArray[i++]) = 0;
   *(Float_t *)(&aDataArray[i++]) = fit->GetThetaIJ();
	 if( fit->GetThetaIJ() > -999990. ) *(Float_t *)(&aDataArray[i-2]) = 1;

	 *(Float_t *)(&aDataArray[i++]) = 0;
   *(Float_t *)(&aDataArray[i++]) = fit->GetITR();
	 if( fit->GetITR() > -999990. ) *(Float_t *)(&aDataArray[i-2]) = 1;

	 *(Float_t *)(&aDataArray[i++]) = 0;
   *(Float_t *)(&aDataArray[i++]) = fit->GetSol_Dir();
	 if( fit->GetSol_Dir() > -999990. ) *(Float_t *)(&aDataArray[i-2]) = 1;

	 *(Float_t *)(&aDataArray[i++]) = 0;
   *(Float_t *)(&aDataArray[i++]) = fit->GetNeckLikelihood();
	 if( fit->GetNeckLikelihood() > -999990. ) *(Float_t *)(&aDataArray[i-2]) = 1;

   i++;
   n = 0;
   for (Int_t m=0; m < QFIT_MAX_BETAS; m++ ) {
     temp = fit->GetBeta()[m];
     if( temp > -999990. ) ++n;
     *(Float_t *)(&aDataArray[i++]) = temp;
   }
   *(Float_t * )(&aDataArray[i-QFIT_MAX_BETAS-1]) = n;

   ++i;
   n = 0;
   for (Int_t m=0; m < QFIT_MAX_RESIDUAL_HARMONICS; m++ ) {
     temp = fit->GetResidualHarmonics()[m];
     if( temp > -999990. ) ++n;
     *(Float_t *)(&aDataArray[i++]) = temp;
   }
   *(Float_t *)(&aDataArray[i-QFIT_MAX_RESIDUAL_HARMONICS-1]) = n;

   i++;
   n = 0;
   for (Int_t m=0; m < QFIT_MAX_RESIDUALS; m++ ) {
     temp = fit->GetResiduals()[m];
     if( temp > -999990. ) ++n;
     *(Float_t *)&aDataArray[i++] = temp;
   }
   *(Float_t *)(&aDataArray[i-QFIT_MAX_RESIDUALS-1]) = n;

   i++;
   n = 0;
   for (Int_t m=0; m < QFIT_MAX_LEGENDRES; m++ ) {
     temp = fit->GetLegendres()[m];
     if( temp > -999990. ) ++n;
     *(Float_t *)&aDataArray[i++] = temp;
	 }
   *(Float_t *)(&aDataArray[i-QFIT_MAX_LEGENDRES-1]) = n;

   i++;
   n = 0;
   for (Int_t m=0; m < QFIT_MAX_ANG_CORRELATIONS; m++ ) {
     temp = fit->GetAngularCorrelation()[m];
     if( temp > -999990. ) ++n;
     *(Float_t *)&aDataArray[i++] = temp;
   }
   *(Float_t *)(&aDataArray[i-QFIT_MAX_ANG_CORRELATIONS-1]) = n;

   *(Float_t *)(&aDataArray[i++]) = fit->GetPhiKSAllHits();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetPhiKSPromptHits();   
	 *(Float_t *)(&aDataArray[i++]) = fit->GetAngKSAllHits();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetAngKSPromptHits();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetChargeThetaIJ();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetPDTChargeProb();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetPDTMaxCharge();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetPDTnTubes();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetPDTnWindowTubes();
	 *(Float_t *)(&aDataArray[i++]) = fit->GetTimeKS();

  }

  for (j=0; j < nRSPs; j++) {
    rsp = GetRSP(j);
    aDataArray[i++] = rsp->GetFitIndex();                       // Life is easier if we put this first ...
    *(Float_t *)(&aDataArray[i++]) = rsp->GetOpticalResponse();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNwin();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNwin2();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNdark();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNeff();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNcor();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNcorMC();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNonline();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNcal();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNefficient();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNworking();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetEnergy();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetUncertainty();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetQuality();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRd2o();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRacr();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRh2o();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRfresnel();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRmpe();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRpmtr();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetReff();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetDrift();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNhits();
    // Done index already
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNwinAllQ();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNhitsAllQ();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNhitsDQXX();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNwinPt();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetTshift();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetPMTResponse();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetAltEnergy();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNckv();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetResolution();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetFoM();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetNCDShadCor();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetRLambda();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetOmega();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetCKVProb();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetChanEff();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetPMTEff();
    *(Float_t *)(&aDataArray[i++]) = rsp->GetMPE();

	 
  }
  for (j=0; j < nFTKs; j++) {
    ftk = GetFTK(j);
    aDataArray[i++] = ftk->GetFitIndex();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetProb();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetEnergy();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetNegativeUncertainty();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetPositiveUncertainty();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetNEff();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetDirectHitProbability();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetMeanMPE();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetScatteredHitProbability();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetPMTReflectedHitProbability();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetAV1HitProbability();
  *(Float_t *)(&aDataArray[i++]) = ftk->GetAV2HitProbability();
}

	for(j=0; j<nShapers; j++ ) {
		qadc = GetShaper(j);
		aDataArray[i++] = qadc->GetADCCharge();
		aDataArray[i++] = qadc->GetShaperChannelNumber();
		aDataArray[i++] = qadc->GetShaperSlotNumber();
		aDataArray[i++] = qadc->GetShaperHardwareAddress();
		aDataArray[i++] = qadc->GetNCDStringNumber();
		*(Float_t *)(&aDataArray[i++]) = qadc->GetCharge();
		*(Float_t *)(&aDataArray[i++]) = qadc->GetEnergy();
		*(Float_t *)(&aDataArray[i++]) = qadc->GetTSLH();
		aDataArray[i++] = qadc->GetCorrMUXGTID();
		*(Float_t *)(&aDataArray[i++]) = qadc->GetCorrMUXDeltaT();
	}
	
	for( j=0;j<nGlobalShapers;j++) {
		qglobalshaper = GetGlobalShaper(j);
		aDataArray[i++] = qglobalshaper->GetBoardInfo();
		aDataArray[i++] = qglobalshaper->GetGlobalScaler();
	}
	
	for( j=0;j<nMuxScopes;j++ ) {
		QMuxScope *qmux = GetMuxScope(j);
		aDataArray[i++] = qmux->GetStatusWord();
		aDataArray[i++] = qmux->GetMuxChannelMask();
		aDataArray[i++] = qmux->GetScopeNumber();
		aDataArray[i++] = qmux->GetMuxBusNumber();
		aDataArray[i++] = qmux->GetMuxBoxNumber();
		aDataArray[i++] = qmux->GetNCDStringNumber();
		*(Double_t *)(&aDataArray[i++]) = qmux->GetClockRegister();
		++i;
		aDataArray[i++] = qmux->GetLatchRegisterID();
		aDataArray[i++] = qmux->GetSizeOfScopeTrace();
		aDataArray[i++] = qmux->GetScopeChannel();
		l = ( qmux->GetSizeOfScopeTrace() + sizeof(int) - 1 ) / sizeof(int);        // Word-align next item
		memcpy( &(aDataArray[i]), qmux->GetValues()->GetArray(), qmux->GetSizeOfScopeTrace() );   // Inverse of TH1C->Set()
		i += l;
		if( l < lmax ) {
			ndata = ndata - lmax + l;           // ndata is calculated above on the assumption that this MUX has a full
			aDataArray[0] = ndata;              // scope trace - adjust here if this is not the case
		}
		aDataArray[i++] =  qmux->GetGlobalMuxRegister();
		
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpParameter_a();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpParameter_b();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogOffset();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpPreampRCFactor();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpElecDelayTime();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetScopeOffset();
		aDataArray[i++] = qmux->GetCorrShaperADCCharge();
		aDataArray[i++] = qmux->GetCorrShaperGTID();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetCorrShaperDeltaT();
	}
	
	for( j=0;j<nPartialMuxScopes;j++ ) {
		QMuxScope *qmux = GetPartialMuxScope(j);
		aDataArray[i++] = qmux->GetStatusWord();
		aDataArray[i++] = qmux->GetMuxChannelMask();
		aDataArray[i++] = qmux->GetScopeNumber();
		aDataArray[i++] = qmux->GetMuxBusNumber();
		aDataArray[i++] = qmux->GetMuxBoxNumber();
		aDataArray[i++] = qmux->GetNCDStringNumber();
		*(Double_t *)(&aDataArray[i++]) = qmux->GetClockRegister();
		++i;
		aDataArray[i++] = qmux->GetLatchRegisterID();
		aDataArray[i++] = qmux->GetSizeOfScopeTrace();
		aDataArray[i++] = qmux->GetScopeChannel();
		l = ( qmux->GetSizeOfScopeTrace() + sizeof(int) - 1 ) / sizeof(int);        // Word-align next item
		memcpy( &(aDataArray[i]), qmux->GetValues()->GetArray(), qmux->GetSizeOfScopeTrace() );   // Inverse of TH1C->Set()
		i += l;
		if( l < lmax ) {
			ndata = ndata - lmax + l;       // As above
			aDataArray[0] = ndata;
		}
		aDataArray[i++] =  qmux->GetGlobalMuxRegister();
		
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpParameter_a();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpParameter_b();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogOffset();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpPreampRCFactor();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetLogAmpElecDelayTime();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetScopeOffset();
		aDataArray[i++] = qmux->GetCorrShaperADCCharge();
		aDataArray[i++] = qmux->GetCorrShaperGTID();
		*(Float_t *)(&aDataArray[i++]) = qmux->GetCorrShaperDeltaT();
	}
	if( i != ndata ) return( -1 );
	return i;
}

Int_t QEvent::PutData(Int_t *aDataArray)
{
	// Extract event info from the the data array and assign it to an event.
	//
	// ************************************************************************************
	//
	// Make sure this code is in synch with GetData !!! 
	//
	// ************************************************************************************
	//

	Clear();
	Int_t i = 0, j, l, jmax, pmt_type, ndata;
	QPMT *pmt = new QPMT();
	QFit *fit = new QFit();
	QRSP *rsp = new QRSP();
	QFTK *ftk = new QFTK();
	QADC *qadc = new QADC();
	QGlobalShaper *qglobalshaper = new QGlobalShaper();
	QMuxScope *qmux = new QMuxScope();

	Int_t NumPMTs, NumOWLs, NumLGs, NumBUTTs, NumNECKs, NumFECDs, NumSPAREs, NumFITs, NumRSPs, NumFTKs;
	Int_t NumShapers, NumMuxScopes, NumPartialMuxScopes, NumGlobalShapers;

	ndata      = aDataArray[i++];
	Event_id   = aDataArray[i++];
	Trig_type  = aDataArray[i++];
	Nhits      = aDataArray[i++];
	Nph        = *(Float_t *) (&aDataArray[i++]);
	Gtr_time   = *(Double_t *)(&aDataArray[i++]);
	i++;
	JulianDate = aDataArray[i++];
	UT1        = aDataArray[i++];
	UT2        = aDataArray[i++];
	Run        = aDataArray[i++];
	DamnID     = aDataArray[i++];
	DamnID1    = aDataArray[i++];
	DarnID     = aDataArray[i++];
	EsumPeak   = aDataArray[i++];
	EsumDiff   = aDataArray[i++];
	EsumInt    = aDataArray[i++];
	DataSet    = aDataArray[i++];
	TrigErr    = aDataArray[i++];	

	SetBit(kCalibrate | kFitFTT, kFALSE );
	SetBit(aDataArray[i++]);
	
	NumPMTs   = aDataArray[i++];
	NumOWLs   = aDataArray[i++];
	NumLGs    = aDataArray[i++];
	NumBUTTs  = aDataArray[i++];
	NumNECKs  = aDataArray[i++];
	NumFECDs  = aDataArray[i++];
	NumSPAREs = aDataArray[i++];
	NumFITs   = aDataArray[i++];
	NumRSPs   = aDataArray[i++];
	NumFTKs   = aDataArray[i++];
	nPBUNs = NumPMTs + NumOWLs + NumLGs + NumBUTTs + NumNECKs + NumFECDs + NumSPAREs;
	
	NCDStatus = aDataArray[i++];
	NumMuxG =  aDataArray[i++];
	NumMux = aDataArray[i++];
	NumScope = aDataArray[i++];
	NCDClock1 = aDataArray[i++];
	NCDClock2 = aDataArray[i++];
	NCDLatchRegister = aDataArray[i++];
	NCDGTID = aDataArray[i++];
	NCDSync = aDataArray[i++];
	NumShapers = aDataArray[i++];
	NumMuxScopes = aDataArray[i++];
	NumPartialMuxScopes = aDataArray[i++];
	NumGlobalShapers = aDataArray[i++];
	
	for (pmt_type=0; pmt_type < 7; pmt_type++) {
		if         (pmt_type == 0)    jmax = NumPMTs;
		else if    (pmt_type == 1)    jmax = NumOWLs;
		else if    (pmt_type == 2)    jmax = NumLGs;
		else if    (pmt_type == 3)    jmax = NumBUTTs;
		else if    (pmt_type == 4)    jmax = NumNECKs;
		else if    (pmt_type == 5)    jmax = NumFECDs;
		else                          jmax = NumSPAREs;
		
		for (j=0; j < jmax; j++) {
			pmt->Setn         (aDataArray[i++]);
			pmt->SetCell      (aDataArray[i++]);
			pmt->SetStatus    (aDataArray[i++]);
			pmt->SetCMOSStatus(aDataArray[i++]);
			pmt->Setihl       ((Int_t)*(Float_t *)(&aDataArray[i++]));     // Arghhh.....
			pmt->Setihs       ((Int_t)*(Float_t *)(&aDataArray[i++]));
			pmt->Setilx       ((Int_t)*(Float_t *)(&aDataArray[i++]));
			pmt->Setit        ((Int_t)*(Float_t *)(&aDataArray[i++]));
			pmt->Sethl        (*(Float_t *)(&aDataArray[i++]));
			pmt->Seths        (*(Float_t *)(&aDataArray[i++]));
			pmt->Setlx        (*(Float_t *)(&aDataArray[i++]));
			pmt->Sett         (*(Float_t *)(&aDataArray[i++]));
			pmt->Setdelta     (*(Float_t *)(&aDataArray[i++]));
			pmt->Settslh      (*(Float_t *)(&aDataArray[i++]));
			pmt->SetEcaVal    (aDataArray[i++]);
			pmt->Setrc        (*(Float_t *)(&aDataArray[i++]));
			
			if         (pmt_type == 0)    AddQPMT(pmt);
			else if    (pmt_type == 1)    AddQOWL(pmt);
			else if    (pmt_type == 2)    AddQLG(pmt);
			else if    (pmt_type == 3)    AddQBUTT(pmt);
			else if    (pmt_type == 4)    AddQNECK(pmt);
			else if    (pmt_type == 5)    AddQFECD(pmt);
			else                          AddQSPARE(pmt);
		}
	}
	
	for (j=0; j < NumFITs; j++) {
		fit->SetX               (*(Float_t *)(&aDataArray[i++]));
		fit->SetY               (*(Float_t *)(&aDataArray[i++]));
		fit->SetZ               (*(Float_t *)(&aDataArray[i++]));
		fit->SetU               (*(Float_t *)(&aDataArray[i++]));
		fit->SetV               (*(Float_t *)(&aDataArray[i++]));
		fit->SetW               (*(Float_t *)(&aDataArray[i++]));
		fit->SetEnergy          (*(Float_t *)(&aDataArray[i++]));
		fit->SetTime            (*(Float_t *)(&aDataArray[i++]));
		fit->SetNumPMTsUsed     (aDataArray[i++]); //(*(Float_t *)(&aDataArray[i++]));
		fit->SetQualityOfFit    (*(Float_t *)(&aDataArray[i++]));
		fit->SetIndex           (aDataArray[i++]);
		fit->SetIntOK           (aDataArray[i++]);
		fit->SetName            ("FIT");
		fit->SetNumIter         (aDataArray[i++]);
		fit->SetC11             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC12             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC13             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC14             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC22             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC23             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC24             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC33             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC34             (*(Float_t *)(&aDataArray[i++]));
		fit->SetC44             (*(Float_t *)(&aDataArray[i++]));
		
		fit->SetFitOutput((Float_t *)&aDataArray[i]);
		i += QFIT_MAX_FIT_OUTPUTS+1;
		
		i++;
		fit->SetThetaIJ(*(Float_t *)&aDataArray[i++]);
		i++;
		fit->SetITR(*(Float_t *)&aDataArray[i++]);
		i++;
		fit->SetSolDir(*(Float_t *)&aDataArray[i++]);
		i++;
		fit->SetNeckLikelihood(*(Float_t *)&aDataArray[i++]);
		
		fit->SetBeta((Float_t *)&aDataArray[i]);
		i += QFIT_MAX_BETAS+1;
		fit->SetResidualHarmonics((Float_t *)&aDataArray[i]);
		i += QFIT_MAX_RESIDUAL_HARMONICS+1;
		fit->SetResiduals((Float_t *)&aDataArray[i]);
		i += QFIT_MAX_RESIDUALS+1;
		fit->SetLegendres((Float_t *)&aDataArray[i]);
		i += QFIT_MAX_LEGENDRES+1;
		fit->SetAngularCorrelation((Float_t *)&aDataArray[i]);
		i += QFIT_MAX_ANG_CORRELATIONS+1;

		fit->SetPhiKSAllHits(*(Float_t *)&aDataArray[i++]);
		fit->SetPhiKSPromptHits(*(Float_t *)&aDataArray[i++]);
		fit->SetAngKSAllHits(*(Float_t *)&aDataArray[i++]);
		fit->SetAngKSPromptHits(*(Float_t *)&aDataArray[i++]);
		fit->SetChargeThetaIJ(*(Float_t *)&aDataArray[i++]);
		fit->SetPDTChargeProb(*(Float_t *)&aDataArray[i++]);
		fit->SetPDTMaxCharge(*(Float_t *)&aDataArray[i++]);
		fit->SetPDTnTubes(*(Float_t *)&aDataArray[i++]);
		fit->SetPDTnWindowTubes(*(Float_t *)&aDataArray[i++]);
		fit->SetTimeKS(*(Float_t *)&aDataArray[i++]);
		
		AddQFIT(fit);
	}
	
	for (j=0; j < NumRSPs; j++) {
		rsp->SetFitIndex           (aDataArray[i++]);
		rsp->SetOpticalResponse    (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNwin               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNwin2              (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNdark              (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNeff               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNcor               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNcorMC             (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNonline            (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNcal               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNefficient         (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNworking           (*(Float_t *)(&aDataArray[i++]));
		rsp->SetEnergy             (*(Float_t *)(&aDataArray[i++]));
		rsp->SetUncertainty        (*(Float_t *)(&aDataArray[i++]));
		rsp->SetQuality            (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRd2o               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRacr               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRh2o               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRfresnel           (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRmpe               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRpmtr              (*(Float_t *)(&aDataArray[i++]));
		rsp->SetReff               (*(Float_t *)(&aDataArray[i++]));
		// We did index already ..
		rsp->SetDrift              (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNhits              (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNwinAllQ           (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNhitsAllQ          (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNhitsDQXX          (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNwinPt             (*(Float_t *)(&aDataArray[i++]));
		rsp->SetTshift             (*(Float_t *)(&aDataArray[i++]));
		rsp->SetPMTResponse        (*(Float_t *)(&aDataArray[i++]));
		rsp->SetAltEnergy          (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNckv               (*(Float_t *)(&aDataArray[i++]));
		rsp->SetResolution         (*(Float_t *)(&aDataArray[i++]));
		rsp->SetFoM                (*(Float_t *)(&aDataArray[i++]));
		rsp->SetNCDShadCor         (*(Float_t *)(&aDataArray[i++]));
		rsp->SetRLambda            (*(Float_t *)(&aDataArray[i++]));
		rsp->SetOmega              (*(Float_t *)(&aDataArray[i++]));
		rsp->SetCKVProb            (*(Float_t *)(&aDataArray[i++]));
		rsp->SetChanEff            (*(Float_t *)(&aDataArray[i++]));
		rsp->SetPMTEff             (*(Float_t *)(&aDataArray[i++]));
		rsp->SetMPE                (*(Float_t *)(&aDataArray[i++]));
		
		AddQRSP(rsp);
	}
	
	for (j=0; j < NumFTKs; j++) {
		ftk->SetFitIndex(aDataArray[i++]);
		ftk->SetProb         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetEnergy         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetNegativeUncertainty         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetPositiveUncertainty         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetNEff         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetDirectHitProbability         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetMeanMPE         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetScatteredHitProbability         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetPMTReflectedHitProbability         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetAV1HitProbability         (*(Float_t *)(&aDataArray[i++]));
		ftk->SetAV2HitProbability         (*(Float_t *)(&aDataArray[i++]));
		
		AddQFTK(ftk);
	}
	
	for(j=0;j<NumShapers;j++) {
		qadc->SetADCCharge(aDataArray[i++]);
		qadc->SetShaperChannelNumber(aDataArray[i++]);
		qadc->SetShaperSlotNumber(aDataArray[i++]);
		qadc->SetShaperHardwareAddress(aDataArray[i++]);
		qadc->SetNCDStringNumber(aDataArray[i++]);
		qadc->SetCharge(*(Float_t *)(&aDataArray[i++]));
		qadc->SetEnergy(*(Float_t *)(&aDataArray[i++]));
		qadc->SetTSLH(*(Float_t *)(&aDataArray[i++]));
		qadc->SetCorrMUXGTID(aDataArray[i++]);
		qadc->SetCorrMUXDeltaT(*(Float_t *)(&aDataArray[i++]));
		AddShaper(qadc);
	}
	
	for(j=0; j<NumGlobalShapers;j++) {
		qglobalshaper->SetBoardInfo(aDataArray[i++]);
		qglobalshaper->SetGlobalScaler(aDataArray[i++]);
		
		AddGlobalShaper( qglobalshaper);
	}
	
	for(j=0; j<NumMuxScopes; j++ ) {
		qmux->SetStatusWord(aDataArray[i++]);
		qmux->SetMuxChannelMask(aDataArray[i++]);
		qmux->SetScopeNumber(aDataArray[i++]);
		qmux->SetMuxBusNumber(aDataArray[i++]);
		qmux->SetMuxBoxNumber(aDataArray[i++]);
		qmux->SetNCDStringNumber(aDataArray[i++]);
		qmux->SetClockRegister(*(Double_t *)(&aDataArray[i++]));
		i++;
		qmux->SetLatchRegisterID(aDataArray[i++]);
		qmux->SetSizeOfScopeTrace(aDataArray[i++]);
		qmux->SetScopeChannel(aDataArray[i++]);
		l = ( qmux->GetSizeOfScopeTrace() + 3 ) / 4;        // Next item is word-aligned
		qmux->GetValues()->Set( qmux->GetSizeOfScopeTrace(), (Char_t *)&aDataArray[i] );
		i += l;
		qmux->SetGlobalMuxRegister(aDataArray[i++]);
		qmux->SetMuxChannel( -1 );                     // No SNOMAN equivalent
		
		qmux->SetLogAmpParameter_a(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogAmpParameter_b(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogOffset(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogAmpPreampRCFactor(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogAmpElecDelayTime(*(Float_t *)(&aDataArray[i++]));
		qmux->SetScopeOffset(*(Float_t *)(&aDataArray[i++]));
		qmux->SetCorrShaperADCCharge(aDataArray[i++]);
		qmux->SetCorrShaperGTID(aDataArray[i++]);
		qmux->SetCorrShaperDeltaT(aDataArray[i++]);
		AddMuxScope( qmux );
	}
	
	for(j=0; j<NumPartialMuxScopes; j++ ) {
		qmux->SetStatusWord(aDataArray[i++]);
		qmux->SetMuxChannelMask(aDataArray[i++]);
		qmux->SetScopeNumber(aDataArray[i++]);
		qmux->SetMuxBusNumber(aDataArray[i++]);
		qmux->SetMuxBoxNumber(aDataArray[i++]);
		qmux->SetNCDStringNumber(aDataArray[i++]);
		qmux->SetClockRegister((Double_t)aDataArray[i++]);
		i++;
		qmux->SetLatchRegisterID(aDataArray[i++]);
		qmux->SetSizeOfScopeTrace(aDataArray[i++]);
		qmux->SetScopeChannel(aDataArray[i++]);
		l = ( qmux->GetSizeOfScopeTrace() + 3 ) / 4;        // Next item is word-aligned
		qmux->GetValues()->Set( qmux->GetSizeOfScopeTrace(), (Char_t *)&aDataArray[i] );
		i += l;
		qmux->SetGlobalMuxRegister(aDataArray[i++]);
		qmux->SetMuxChannel( -1 );                     // No SNOMAN equivalent
		
		qmux->SetLogAmpParameter_a(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogAmpParameter_b(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogOffset(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogAmpPreampRCFactor(*(Float_t *)(&aDataArray[i++]));
		qmux->SetLogAmpElecDelayTime(*(Float_t *)(&aDataArray[i++]));
		qmux->SetScopeOffset(*(Float_t *)(&aDataArray[i++]));
		qmux->SetCorrShaperADCCharge(aDataArray[i++]);
		qmux->SetCorrShaperGTID(aDataArray[i++]);
		qmux->SetCorrShaperDeltaT(aDataArray[i++]);
		AddPartialMuxScope( qmux );
	}
	
	delete pmt;
	delete fit;
	delete rsp;
	delete ftk;
	delete qadc;
	delete qglobalshaper;
	delete qmux;

	if( i != ndata ) return( -1 );
	return( i );

}

void QEvent::DumpInfo()
{
  // Dump the PMT information for this event.
  QPMT *pmt = NULL;
  Int_t j;
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    printf("%6i %6i %6i %6i %6i %8.2f %8.2f %8.2f %8.2f\n",pmt->Getn(),pmt->Getihs(),pmt->Getihl(),pmt->Getilx(),pmt->Getit(),pmt->Geths(),pmt->Gethl(),pmt->Getlx(),pmt->Gett());
  }
  pmt = NULL;
}

void QEvent::Clear(Option_t *anOption)
{
  // Reset this event.

  if (PMTs)      { PMTs->Clear(anOption);      nPMTs   = 0; }
  if (OWLs)      { OWLs->Clear(anOption);      nOWLs   = 0; }
  if (LGs )      { LGs->Clear(anOption);       nLGs    = 0; }
  if (BUTTs)     { BUTTs->Clear(anOption);     nBUTTs  = 0; }
  if (NECKs)     { NECKs->Clear(anOption);     nNECKs  = 0; }
  if (FECDs)     { FECDs->Clear(anOption);     nFECDs  = 0; }
  if (SPAREs)    { SPAREs->Clear(anOption);    nSPAREs = 0; }
  if (MCPMs)     { MCPMs->Clear(anOption);     nMCPMs  = 0; }
  if (MCVXs)     { MCVXs->Clear(anOption);     nMCVXs  = 0; }
  if (FITs)      { FITs->Clear(anOption);      nFITs   = 0; }
  if (MuonFits)  { MuonFits->Clear(anOption);  nMuonFits   = 0; }
  if (RSPs)      { RSPs->Clear(anOption);      nRSPs   = 0; }
  if (FTKs)      { FTKs->Clear(anOption);      nFTKs   = 0; }
  if (Shapers)   { Shapers->Clear(anOption);   nShapers   = 0; }

  if (MuxScopes) {
#ifndef USE_TCLONES_CLEAR
    MuxScopes->Delete( "" );
#else
    MuxScopes->Clear( (anOption && *anOption) ? anOption : "C" );
#endif
    nMuxScopes = 0;
  }

  if (PartialMuxScopes) {
#ifndef USE_TCLONES_CLEAR
    PartialMuxScopes->Delete( "" );
#else
    PartialMuxScopes->Clear( (anOption && *anOption) ? anOption : "C" );
#endif
    nPartialMuxScopes = 0;
  }

  if (GlobalShapers) { GlobalShapers->Clear(anOption); nGlobalShapers = 0; }
}

void QEvent::ClearFITs(Option_t *anOption)
{
  // Reset the fit banks for this event.
  if (FITs) { FITs->Clear(anOption); nFITs = 0; }
}
void QEvent::ClearRSPs(Option_t *anOption)
{
  // Reset the RSP banks for this event.
  if (RSPs) { RSPs->Clear(anOption); nRSPs = 0; }
}
void QEvent::ClearFTKs(Option_t *anOption)
{
  // Reset the FTK banks for this event.
  if (FTKs) { FTKs->Clear(anOption); nFTKs = 0; }
}
void QEvent::ClearPMTs(Option_t *anOption)
{
  // Reset the PMT banks for this event.
  if (PMTs) { PMTs->Clear(anOption); nPMTs = 0; }
}
void QEvent::Shrink()
{
  // Resize the TClonesArrays to the actual size used.
  PMTs->Compress();
  OWLs->Compress();
  LGs->Compress();
  BUTTs->Compress();
  NECKs->Compress();
  FECDs->Compress();
  SPAREs->Compress();
  MCPMs->Compress();
  MCVXs->Compress();
  FITs->Compress();
  RSPs->Compress();
  FTKs->Compress();
  Shapers->Compress();
  GlobalShapers->Compress();
  MuxScopes->Compress();
  PartialMuxScopes->Compress();
  MuonFits->Compress();
}

void QEvent::ZapFields(){
  Int_t *first=Data();
  Int_t length=DataN();
  Int_t i;
  for(i=0;i<=length;i++)first[i]=0;
}
Bool_t QEvent::IsSame2( const char* comment, QEvent *anEvent, Int_t aPrintMode, Int_t skipDamnDarn ) const
{
	//
	// This code compares the `real' part of a QEvent. If  skipDamnDarn  is non-zero we
	// don't damand that the DAMN/DARN word are equal. Set  aPrintMode !=0  for verbose
	// output
	//

	Int_t j = 0;

	if( Event_id != anEvent->GetEvent_id() ) j++;
	if( Trig_type != anEvent->Trig_type ) j++;
	if( Nph != anEvent->Nph ) j++;
	if( Nhits != anEvent->Nhits ) j++;
	if( nPMTs != anEvent->nPMTs ) j++;
	if( nOWLs != anEvent->nOWLs ) j++;
	if( nLGs != anEvent->nLGs ) j++;
	if( nBUTTs != anEvent->nBUTTs ) j++;
	if( nNECKs != anEvent->nNECKs ) j++;
	if( nFECDs != anEvent->nFECDs ) j++;
	if( nSPAREs != anEvent->nSPAREs ) j++;
	if( nPBUNs != anEvent->nPBUNs ) j++;
	if( nMCPMs != anEvent->nMCPMs ) j++;
	if( nMCVXs != anEvent->nMCVXs ) j++;
	if( nFITs != anEvent->nFITs ) j++;
	if( nRSPs != anEvent->nRSPs ) j++;
	if( nFTKs != anEvent->nFTKs ) j++;
	if( nShapers != anEvent->nShapers ) j++;
	if( nMuxScopes != anEvent->nMuxScopes ) j++;
	if( nPartialMuxScopes != anEvent->nPartialMuxScopes ) j++;
	if( nGlobalShapers != anEvent->nGlobalShapers ) j++;
	if( nMuonFits != anEvent->nMuonFits ) j++;
	if( tPrev != anEvent->tPrev ) j++;
	if( tNext != anEvent->tNext ) j++;
	if( Gtr_time != anEvent->Gtr_time ) j++;
	if( JulianDate != anEvent->JulianDate ) j++;
	if( UT1 != anEvent->UT1 ) j++;
	if( UT1 != anEvent->UT1 ) j++;
	if( Run != anEvent->Run ) j++;
	if( !skipDamnDarn ) {
		if( DamnID != anEvent->DamnID ) j++;
		if( DamnID1 != anEvent->DamnID1 ) j++;
		if( DarnID != anEvent->DarnID ) j++;
	}
	if( EsumPeak != anEvent->EsumPeak ) j++;
	if( EsumDiff != anEvent->EsumDiff ) j++;
	if( EsumInt != anEvent->EsumInt ) j++;
	if( Time10MHz != anEvent->Time10MHz ) j++;
	if( DataSet != anEvent->DataSet ) j++;
	if( NCDStatus != anEvent->NCDStatus ) j++;
	if( NumMuxG != anEvent->NumMuxG ) j++;
	if( NumMux != anEvent->NumMux ) j++;
	if( NumScope != anEvent->NumScope ) j++;
	if( NCDClock1 != anEvent->NCDClock1 ) j++;
	if( NCDClock2 != anEvent->NCDClock2 ) j++;
	if( NCDLatchRegister != anEvent->NCDLatchRegister ) j++;
	if( NCDGTID != anEvent->NCDGTID ) j++;
	if( NCDSync != anEvent->NCDSync ) j++;
	if( MCEventNumber != anEvent->MCEventNumber ) j++;
	
	if( aPrintMode && j ) {
		printf( "%s", comment );
		if( Event_id != anEvent->GetEvent_id() ) printf( " Event_id" );
		if( Trig_type != anEvent->Trig_type ) printf( " Trig_type" );
		if( Nph != anEvent->Nph ) printf( " Nph" );
		if( Nhits != anEvent->Nhits ) printf( " Nhits");
		if( nPMTs != anEvent->nPMTs ) printf( " nPMTs" );
		if( nOWLs != anEvent->nOWLs ) printf( " nOWLs" );
		if( nLGs != anEvent->nLGs ) printf( " nLGs" );
		if( nBUTTs != anEvent->nBUTTs ) printf( " nBUTTs" );
		if( nNECKs != anEvent->nNECKs ) printf( " nNECKs" );
		if( nFECDs != anEvent->nFECDs ) printf( " nFECDs" );
		if( nSPAREs != anEvent->nSPAREs ) printf( " nSPAREs" );
		if( nPBUNs != anEvent->nPBUNs ) printf( " nPBUNs" );
		if( nMCPMs != anEvent->nMCPMs ) printf( " nMCPMs" );
		if( nMCVXs != anEvent->nMCVXs ) printf( " nMCVXs" );
		if( nFITs != anEvent->nFITs ) printf( " nFITs" );
		if( nRSPs != anEvent->nRSPs ) printf( " nRSPs" );
		if( nFTKs != anEvent->nFTKs ) printf( " nFTKs" );
		if( nShapers != anEvent->nShapers ) printf( " nShapers" );
		if( nMuxScopes != anEvent->nMuxScopes ) printf( " nMuxScopes" );
		if( nPartialMuxScopes != anEvent->nPartialMuxScopes ) printf( " nPartialMuxScopes" );
		if( nGlobalShapers != anEvent->nGlobalShapers ) printf( " nGlobalShapers" );
		if( nMuonFits != anEvent->nMuonFits ) printf( " nMuonFits" );
		if( tPrev != anEvent->tPrev ) printf( " tPrev" );
		if( tNext != anEvent->tNext ) printf( " tNext" );
		if( Gtr_time != anEvent->Gtr_time ) printf( " Gtr_time" );
		if( JulianDate != anEvent->JulianDate ) printf( " JulianDate" );
		if( UT1 != anEvent->UT1 ) printf( " UT1" );
		if( UT1 != anEvent->UT1 ) printf( " UT2" );
		if( Run != anEvent->Run ) printf( "Run" );
		if( !skipDamnDarn ) {
			if( DamnID != anEvent->DamnID ) printf( " DamnID" );
			if( DamnID1 != anEvent->DamnID1 ) printf( "DamnID1" );
			if( DarnID != anEvent->DarnID ) printf( " DarnID" );
		}
		if( EsumPeak != anEvent->EsumPeak ) printf( " EsumPeak" );
		if( EsumDiff != anEvent->EsumDiff ) printf( " EsumDiff" );
		if( EsumInt != anEvent->EsumInt ) printf( " EsumInt" );
		if( Time10MHz != anEvent->Time10MHz ) printf( " Time10MHz" );
		if( DataSet != anEvent->DataSet ) printf( " DataSet" );
		if( NCDStatus != anEvent->NCDStatus ) printf( " NCDStatus" );
		if( NumMuxG != anEvent->NumMuxG ) printf( " NumMuxG" );
		if( NumMux != anEvent->NumMux ) printf( " NumMux" );
		if( NumScope != anEvent->NumScope ) printf( " NumScope" );
		if( NCDClock1 != anEvent->NCDClock1 ) printf( "NCDClock1" );
		if( NCDClock2 != anEvent->NCDClock2 ) printf( " NCDClock2" );
		if( NCDLatchRegister != anEvent->NCDLatchRegister ) printf( " NCDLatchRegister" );
		if( NCDGTID != anEvent->NCDGTID ) printf( " NCDGTID" );
		if( NCDSync != anEvent->NCDSync ) printf( " NCDSync" );
		if( MCEventNumber != anEvent->MCEventNumber ) printf( " MCEventNumber" );
		printf( "\n" );
	}
	return( j==0 );
}

Bool_t QEvent::IsSame(const char *comment, QEvent &anEvent, Int_t aPrintMode)const
{
  // Compare two QEvents.  Used to check whether "innocuous" changes to code have changed the tree.  
  //Returns true if events are different.  
  //Print mode .  0= no printing, 1= print all differences

  Int_t *first=Data();  // first word in event structure
  Int_t length=DataN();  //length of buffer
  Int_t i,j;
  Int_t *otherData=anEvent.Data();
  Bool_t same;
	Int_t iDamnID = (Int_t *)&DamnID - first;
	Int_t iDamnID1 = (Int_t *)&DamnID1 - first;
	Int_t iDarnID = (Int_t *)&DarnID - first;

  for(j=i=0;i<=length;i++)if(i!=iDamnID && i!=iDamnID1 && i != iDarnID && first[i]!=otherData[i])j++;
  if(j>0&&aPrintMode==1){
    printf("%s QEvent: %d differences: Words: ",comment, j);
    for(i=0;i<length;i++)if(first[i]!=otherData[i])printf(" %d",i);
    printf("\n");
  }
  same= (j==0);
  const QPMT *pmt;
  const QPMT *pmt2; 
  const QMCVX *mcvx;
  const QFit *fit;
  const QRSP *rsp;
  const QFTK *ftk;

  for (j=0; j < anEvent.nPMTs  && nPMTs==anEvent.nPMTs; j++) {
    pmt = anEvent.GetPMT(j);
    pmt2= GetPMT(j);
    same &= pmt2->IsSame("Inward",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nOWLs && nOWLs==anEvent.nOWLs; j++) {
    pmt = anEvent.GetOWL(j);
    pmt2= GetOWL(j);
    same &= pmt2->IsSame("Owl",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nLGs  && nLGs==anEvent.nLGs; j++) {
    pmt = anEvent.GetLG(j);
    pmt2= GetLG(j);
    same &= pmt2->IsSame("LG",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nBUTTs && nBUTTs==anEvent.nBUTTs; j++) {
    pmt = anEvent.GetBUTT(j);
    pmt2= GetBUTT(j);
    same &= pmt2->IsSame("BUTT",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nNECKs && nNECKs==anEvent.nNECKs; j++) {
    pmt = anEvent.GetNECK(j);
    pmt2= GetNECK(j);
    same &= pmt2->IsSame("NECK",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nFECDs&& nFECDs== anEvent.nFECDs; j++) {
    pmt = anEvent.GetFECD(j);
    pmt2= GetFECD(j);
    same &= pmt2->IsSame("FECD",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nSPAREs && nSPAREs==anEvent.nSPAREs; j++) {
    pmt = anEvent.GetSPARE(j);
    pmt2= GetSPARE(j);
    same &= pmt2->IsSame("SPARE",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nMCPMs && nMCPMs==anEvent.nMCPMs; j++) {
    pmt = anEvent.GetMCPM(j);
    pmt2= GetMCPM(j);
    same &= pmt2->IsSame("MCPM",j,pmt,aPrintMode);
  }
  for (j=0; j < anEvent.nMCVXs && nMCVXs==anEvent.nMCVXs; j++) {
    mcvx = anEvent.GetMCVX(j);
    const QMCVX *mcvx2= GetMCVX(j);
    same &= mcvx2->IsSame(j,mcvx,aPrintMode);
  }
  for (j=0; j < anEvent.nFITs && nFITs==anEvent.nFITs; j++) {
    fit = anEvent.GetFit(j);
    const QFit *fit2= GetFit(j);
    same &= fit2->IsSame(j,fit,aPrintMode);
  }
  for (j=0; j < anEvent.nRSPs && nRSPs==anEvent.nRSPs; j++) {
    rsp = anEvent.GetRSP(j);
    const QRSP * rsp2= GetRSP(j);
    same &= rsp2->IsSame(j,rsp,aPrintMode);
  }
  for (j=0; j < anEvent.nFTKs && nFTKs==anEvent.nFTKs; j++) {
    ftk = anEvent.GetFTK(j);
    const QFTK * ftk2= GetFTK(j);
    same &= ftk2->IsSame(j,ftk,aPrintMode);
  }

  for (j=0; j < anEvent.nShapers && nShapers==anEvent.nShapers; j++){
    const QADC *shaper=anEvent.GetShaper(j);
    const QADC *shaper2= GetShaper(j);
    same &= shaper2->IsSame(j,shaper,aPrintMode);
  }

  for (j=0; j < anEvent.nMuxScopes && nMuxScopes==anEvent.nMuxScopes; j++){
    const QMuxScope *muxscope=anEvent.GetMuxScope(j);
    const QMuxScope *muxscope2= GetMuxScope(j);
    same &= muxscope2->IsSame(j,muxscope,aPrintMode);
  }

  for (j=0; j < anEvent.nPartialMuxScopes && nPartialMuxScopes==
	 anEvent.nPartialMuxScopes; j++){
    const QMuxScope *muxscope=anEvent.GetPartialMuxScope(j);
    const QMuxScope *muxscope2= GetPartialMuxScope(j);
    same &= muxscope2->IsSame(j,muxscope,aPrintMode);
  }

  for(j=0; j < anEvent.nGlobalShapers  && nGlobalShapers==
	anEvent.nGlobalShapers; j++){
    const QGlobalShaper *globalshaper=anEvent.GetGlobalShaper(j);
    const QGlobalShaper *globalshaper2= GetGlobalShaper(j);
    same &= globalshaper2->IsSame(j,globalshaper,aPrintMode);
  }

  // Copy the fBits word.
  //  for (j=0; j < 32; j++) {
  //  if (anEvent.TestBit(j)) SetBit(j,kTRUE);
  // }
  return same;
}

Bool_t QEvent::IsHe4(){
  // returns true if the event contains a He4 Shaper, MuxScope or Partial MuxScope
  int j;
  Bool_t result=FALSE;
  for(j=0;j<nMuxScopes;j++)if(GetMuxScope(j)->IsHe4())result=TRUE;
  for(j=0;j<nPartialMuxScopes;j++)if(GetPartialMuxScope(j)->IsHe4())result=TRUE;
  for(j=0;j<nShapers;j++)if(GetShaper(j)->IsHe4())result=TRUE;
  return result;
}

void QEvent::Add(QPMT *pmt)
{
  // Add a PMT, LG, OWL, BUTT or NECK tube.
  // Uses gPMTxyz to determine tube type.
  if (!gPMTxyz) {
    gSNO->GetPMTxyz();     // Create a global gPMTxyz if it does not exist.
    if (!gPMTxyz) return;  // Give up if no gPMTxyz object found.
  }

  Int_t tube_type = gPMTxyz->GetType(pmt->Getn());
  if (tube_type == KUTL_CCC_CALIBRATION)        AddQFECD(pmt);
  else if (tube_type == KUTL_CCC_PMT_NORMAL)    AddQPMT(pmt);
  else if (tube_type == KUTL_CCC_PMT_OWL)       AddQOWL(pmt);
  else if (tube_type == KUTL_CCC_PMT_LG)        AddQLG(pmt);
  else if (tube_type == KUTL_CCC_PMT_BUTT)      AddQBUTT(pmt);
  else if (tube_type == KUTL_CCC_PMT_NECK)      AddQNECK(pmt);
  else                                          AddQSPARE(pmt);
}

void QEvent::AddQPMT(QPMT *pmt)
{
  // Add pmt to the list of PMTs.
  TClonesArray &pmtbundles = *PMTs;
  *(new (pmtbundles[nPMTs++]) QPMT()) = *pmt;
}
void QEvent::AddQPMT(Int_t *ivars, Float_t *rvars)
{
  // Add a PMT bundle to the current event.
  TClonesArray &pmtbundles = *PMTs;
  new (pmtbundles[nPMTs++]) QPMT(ivars, rvars);
}
QPMT *QEvent::AddQPMT()
{
  // Add an uninitialized PMT bundle to the current event.
  TClonesArray &pmtbundles = *PMTs;
  new (pmtbundles[nPMTs++]) QPMT();
  return (QPMT *)PMTs->At(nPMTs - 1);
}

void QEvent::AddQOWL(QPMT *pmt)
{
  // Add pmt to the list of OWLs.
  TClonesArray &owls = *OWLs;
  *(new (owls[nOWLs++] ) QPMT() ) = *pmt;
}
void QEvent::AddQOWL(Int_t *ivars, Float_t *rvars)
{
  // Add a PMT OWL bundle to the current event.
  TClonesArray &owlbundles = *OWLs;
  new (owlbundles[nOWLs++]) QPMT(ivars, rvars);
}
QPMT *QEvent::AddQOWL()
{
  // Add an uninitialized PMT bundle to the current event.
  TClonesArray &owls = *OWLs;
  new (owls[nOWLs++]) QPMT();
  return (QPMT *)OWLs->At(nOWLs - 1);
}

void QEvent::AddQLG(QPMT *pmt)
{
  // Add pmt to the list of LGs.
  TClonesArray &lgs = *LGs;
  *(new (lgs[nLGs++] ) QPMT() ) = *pmt;
}
void QEvent::AddQLG(Int_t *ivars, Float_t *rvars)
{
  // Add a PMT bundle to the current event.
  TClonesArray &pmtbundles = *LGs;
  new (pmtbundles[nLGs++]) QPMT(ivars, rvars);
}
QPMT *QEvent::AddQLG()
{
  // Add an uninitialized PMT bundle to the current event.
  TClonesArray &pmtbundles = *LGs;
  new (pmtbundles[nLGs++]) QPMT();
  return (QPMT *)LGs->At(nLGs - 1);
}

void QEvent::AddQBUTT(QPMT *pmt)
{
  // Add pmt to the list of BUTTs.
  TClonesArray &butts = *BUTTs;
  *(new (butts[nBUTTs++] ) QPMT() ) = *pmt;
}
void QEvent::AddQBUTT(Int_t *ivars, Float_t *rvars)
{
  // Add a PMT bundle to the current event.
  TClonesArray &pmtbundles = *BUTTs;
  new (pmtbundles[nBUTTs++]) QPMT(ivars, rvars);
}
QPMT *QEvent::AddQBUTT()
{
  // Add an uninitialized PMT bundle to the current event.
  TClonesArray &pmtbundles = *BUTTs;
  new (pmtbundles[nBUTTs++]) QPMT();
  return (QPMT *)BUTTs->At(nBUTTs - 1);
}

void QEvent::AddQNECK(QPMT *pmt)
{
  // Add pmt to the list of NECKs.
  TClonesArray &necks = *NECKs;
  *(new (necks[nNECKs++] ) QPMT() ) = *pmt;
}
void QEvent::AddQNECK(Int_t *ivars, Float_t *rvars)
{
  // Add a PMT bundle to the current event.
  TClonesArray &pmtbundles = *NECKs;
  new (pmtbundles[nNECKs++]) QPMT(ivars, rvars);
}
QPMT *QEvent::AddQNECK()
{
  // Add an uninitialized PMT bundle to the current event.
  TClonesArray &pmtbundles = *NECKs;
  new (pmtbundles[nNECKs++]) QPMT();
  return (QPMT *)NECKs->At(nNECKs - 1);
}

void QEvent::AddQFECD(QPMT *pmt)
{
  // Add pmt to the list of FECDs.
  TClonesArray &fecds = *FECDs;
  *(new (fecds[nFECDs++] ) QPMT() ) = *pmt;
}
void QEvent::AddQFECD(Int_t *ivars, Float_t *rvars)
{
  // Add a PMT bundle to the current event.
  TClonesArray &fecds = *FECDs;
  new (fecds[nFECDs++]) QPMT(ivars,rvars);
}
QPMT *QEvent::AddQFECD()
{
  // Add an uninitialized PMT bundle to the current event.
  TClonesArray &fecds = *FECDs;
  new (fecds[nFECDs++]) QPMT();
  return (QPMT *)FECDs->At(nFECDs - 1);
}
  
void QEvent::AddQSPARE(QPMT *pmt)
{
  // Add pmt to the list of SPAREs.
  TClonesArray &spares = *SPAREs;
  *(new (spares[nSPAREs++] ) QPMT() ) = *pmt;
}
void QEvent::AddQSPARE(Int_t *ivars, Float_t *rvars)
{
  // Add a SPARE PMT bundle to the current event.
  TClonesArray &spares = *SPAREs;
  new (spares[nSPAREs++]) QPMT(ivars,rvars);
}
QPMT *QEvent::AddQSPARE()
{
  // Add an uninitialized SPARE PMT bundle to the current event.
  TClonesArray &spares = *SPAREs;
  new (spares[nSPAREs++]) QPMT();
  return (QPMT *)SPAREs->At(nSPAREs - 1);
}

void QEvent::AddQMCPM(QPMT *pmt)
{
  // Add pmt to the list of MCPMs.
  TClonesArray &mcpms = *MCPMs;
  *(new (mcpms[nMCPMs++]) QPMT() ) = *pmt;
}
void QEvent::AddQMCPM(Int_t *ivars, Float_t *rvars)
{
  // Add an MCPM bundle to the current event.
  TClonesArray &pmtbundles = *MCPMs;
  new (pmtbundles[nMCPMs++]) QPMT(ivars, rvars);
}
QPMT *QEvent::AddQMCPM()
{
  // Add an uninitialized MCPM bundle to the current event.
  TClonesArray &pmtbundles = *MCPMs;
  new (pmtbundles[nMCPMs++]) QPMT();
  return (QPMT *)MCPMs->At(nMCPMs - 1);
}

void QEvent::AddQMCVX(QMCVX *mcvx)
{
  // Add the QMCVX to the event.
  TClonesArray &mcvxs = *MCVXs;
  *(new (mcvxs[nMCVXs++]) QMCVX()) = *mcvx;
}
void QEvent::AddQMCVX(Double_t aMCTime, Float_t *xvars, Int_t aBit)
{
  // Add an initialized FIT to the current event.
  TClonesArray &mcvxs = *MCVXs;
  new (mcvxs[nMCVXs++]) QMCVX(aMCTime,xvars,aBit);
}
QMCVX *QEvent::AddQMCVX()
{
  // Add an MCVX to the current event.
  TClonesArray &mcvxs = *MCVXs;
  new (mcvxs[nMCVXs++]) QMCVX();
  return (QMCVX *)MCVXs->At(nMCVXs - 1);
}

void QEvent::AddQFIT(QFit *fit)
{
  // Add the QFit fit to the event.
  TClonesArray &eventfit = *FITs;
  *(new (eventfit[nFITs++]) QFit()) = *fit;
}
void QEvent::AddQFIT(Float_t *xvars, Int_t aBit)
{
  // Add an initialized FIT to the current event.
  TClonesArray &eventfit = *FITs;
  new (eventfit[nFITs++]) QFit(xvars, aBit);
}
QFit *QEvent::AddQFIT()
{
  // Add a FIT to the current event.
  TClonesArray &eventfit = *FITs;
  new (eventfit[nFITs++]) QFit();
  return (QFit *)FITs->At(nFITs - 1);
}

void QEvent::AddMuonFit(QMuonFit *fit)
{
  // Add the QMuonFit to the event.
  TClonesArray &muonfit = *MuonFits;
  *(new (muonfit[nMuonFits++]) QMuonFit()) = *fit;
}
void QEvent::AddMuonFit(Int_t * aFit, Int_t *aVertex, Int_t *aTrack, Int_t *aMes, Int_t &aFitter){
  // Add an initialized MuonFit to the current event.
  TClonesArray &muonfit = *MuonFits;
  new (muonfit[nMuonFits++]) QMuonFit(aFit, aVertex, aTrack,aMes,aFitter);
}
QMuonFit *QEvent::AddMuonFit()
{
  // Add a FIT to the current event.
  TClonesArray &muonfit = *MuonFits;
  new (muonfit[nMuonFits++]) QFit();
  return (QMuonFit *)MuonFits->At(nMuonFits - 1);
}

void QEvent::AddQRSP(QRSP *rsp)
{
  // Add the QRSP rsp to the event.
  TClonesArray &eventrsp = *RSPs;
  *(new (eventrsp[nRSPs++]) QRSP()) = *rsp;
}
void QEvent::AddQRSP(Float_t *rvars, Int_t aFitIndex)
{
  // Add an initialized RSP to the event.
  TClonesArray &eventrsp = *RSPs;
  new (eventrsp[nRSPs++]) QRSP(rvars, aFitIndex);
}
QRSP *QEvent::AddQRSP()
{
  // Add a QRSP to the current event.
  TClonesArray &eventrsp = *RSPs;
  new (eventrsp[nRSPs++]) QRSP();
  return (QRSP *)RSPs->At(nRSPs - 1);
}

void QEvent::AddQFTK(QFTK *ftk)
{
  // Add the QFTK ftk to the event.
  TClonesArray &eventftk = *FTKs;
  *(new (eventftk[nFTKs++]) QFTK()) = *ftk;
}
void QEvent::AddQFTK(Float_t *rvars, Int_t aFitIndex)
{
  // Add an initialized FTK to the event.
  TClonesArray &eventftk = *FTKs;
  new (eventftk[nFTKs++]) QFTK(rvars, aFitIndex);
}
QFTK *QEvent::AddQFTK()
{
  // Add a QFTK to the current event.
  TClonesArray &eventftk = *FTKs;
  new (eventftk[nFTKs++]) QFTK();
  return (QFTK *)FTKs->At(nFTKs - 1);
}

void QEvent::AddMuxScope(QMuxScope *ms)
{
  // Add the QMuxscope ms to the event.
  TClonesArray &eventms = *MuxScopes; 
  *(new (eventms[nMuxScopes++]) QMuxScope()) = *ms;
}

void QEvent::AddMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars)
{
  // Add an initialized MuxScope to the event.
  TClonesArray &eventms = *MuxScopes;
  new (eventms[nMuxScopes++]) QMuxScope(ivars, jvars, fvars);
}

void QEvent::AddMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars,
                         Int_t *kvars, Float_t *rvars)
{
  // Add an initialized MuxScope to the event.
  TClonesArray &eventms = *MuxScopes;
  new (eventms[nMuxScopes++]) QMuxScope(ivars, jvars, fvars, kvars, rvars);
}

QMuxScope *QEvent::AddMuxScope()
{
  // Add a QMuxScope to the current event.
  TClonesArray &eventms = *MuxScopes;
  new (eventms[nMuxScopes++]) QMuxScope();
  return (QMuxScope *)MuxScopes->At(nMuxScopes - 1);
}

void QEvent::AddPartialMuxScope(QMuxScope *ms)
{
  // Add the Partial QMuxscope ms to the event.
  TClonesArray &eventms = *PartialMuxScopes; 
  *(new (eventms[nPartialMuxScopes++]) QMuxScope()) =*ms;
}

void QEvent::AddPartialMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars)
{
  // Add an initialized MuxScope to the event.
  TClonesArray &eventms = *PartialMuxScopes;
  new (eventms[nPartialMuxScopes++]) QMuxScope(ivars, jvars, fvars);
}

void QEvent::AddPartialMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars,
                                Int_t *kvars, Float_t *rvars)
{
  // Add an initialized MuxScope to the event.
  TClonesArray &eventms = *PartialMuxScopes;
  new (eventms[nPartialMuxScopes++]) QMuxScope(ivars, jvars, fvars, kvars, rvars);
}  

QMuxScope *QEvent::AddPartialMuxScope()
{
  // Add a QMuxScope to the current event.
  TClonesArray &eventms = *PartialMuxScopes;
  new (eventms[nPartialMuxScopes++]) QMuxScope();
  return (QMuxScope *)PartialMuxScopes->At(nMuxScopes - 1);
}


void QEvent::AddShaper(QADC *sh)
{
  // Add an ncd shaper (QADC) to the event.
  *(new ((*Shapers)[nShapers++]) QADC()) = *sh;
}

void QEvent::AddShaper(Int_t *ivars)
{
  // Add an initialized Shaper to the event.
  new ((*Shapers)[nShapers++]) QADC(ivars);
}

void QEvent::AddShaper(Int_t *ivars, Int_t *jvars, Float_t *fvars)
{
  // Add an initialized Shaper to the event.
  new ((*Shapers)[nShapers++]) QADC(ivars, jvars, fvars);
} 

QADC *QEvent::AddShaper()
{
  // Add a QADC (Shaper) to the current event.
  new ((*Shapers)[nShapers++]) QADC();
  return (QADC *)Shapers->At(nShapers - 1);
}

void QEvent::AddGlobalShaper(QGlobalShaper *globalshaper)
{
  *(new ((*GlobalShapers)[nGlobalShapers++]) QGlobalShaper()) = *globalshaper;
}

void QEvent::AddGlobalShaper(Int_t *ivars)
{
  new ((*GlobalShapers)[nGlobalShapers++]) QGlobalShaper(ivars);
}

QGlobalShaper *QEvent::AddGlobalShaper()
{
  new ((*GlobalShapers)[nGlobalShapers++]) QGlobalShaper();
  return (QGlobalShaper *)GlobalShapers->At(nGlobalShapers - 1);
}

QPMT *QEvent::GetPMT(Int_t i)const
{
  // Return the i'th PMT for this event.
  QPMT *pmt = NULL;
  if (i < nPMTs) pmt = (QPMT *)PMTs->At(i);
  return pmt;
}
QPMT *QEvent::GetOWL(Int_t i)const
{
  // Return the i'th OWL for this event.
  QPMT *pmt = NULL;
  if (i < nOWLs) pmt = (QPMT *)OWLs->At(i);
  return pmt;
}
QPMT *QEvent::GetLG(Int_t i)const
{
  // Return the i'th LG tube for this event.
  QPMT *pmt = NULL;
  if (i < nLGs) pmt = (QPMT *)LGs->At(i);
  return pmt;
}
QPMT *QEvent::GetBUTT(Int_t i)const
{
  // Return the i'th BUTT tube for this event.
  QPMT *pmt = NULL;
  if (i < nBUTTs) pmt = (QPMT *)BUTTs->At(i);
  return pmt;
}
QPMT *QEvent::GetNECK(Int_t i)const
{
  // Return the i'th NECK tube for this event.
  QPMT *pmt = NULL;
  if (i < nNECKs) pmt = (QPMT *)NECKs->At(i);
  return pmt;
}
QPMT *QEvent::GetFECD(Int_t i)const
{
  // Return the i'th FECD for this event.
  QPMT *pmt = NULL;
  if (i < nFECDs) pmt = (QPMT *)FECDs->At(i);
  return pmt;
}
QPMT *QEvent::GetSPARE(Int_t i)const
{
  // Return the i'th SPARE pmt for this event.
  QPMT *pmt = NULL;
  if (i < nSPAREs) pmt = (QPMT *)SPAREs->At(i);
  return pmt;

}

QPMT *QEvent::GetAnyPMT(Int_t i)
{
  // Return the i'th pmt bundle regardless of it's type.
  if (i < nPMTs)                                            return GetPMT      (i);
  if (i < nPMTs+nOWLs)                                      return GetOWL      (i - nPMTs);
  if (i < nPMTs+nOWLs+nLGs)                                 return GetLG       (i - nPMTs - nOWLs);
  if (i < nPMTs+nOWLs+nLGs+nBUTTs)                          return GetBUTT     (i - nPMTs - nOWLs - nLGs);
  if (i < nPMTs+nOWLs+nLGs+nBUTTs+nNECKs)                   return GetNECK     (i - nPMTs - nOWLs - nLGs - nBUTTs);
  if (i < nPMTs+nOWLs+nLGs+nBUTTs+nNECKs+nFECDs)            return GetFECD     (i - nPMTs - nOWLs - nLGs - nBUTTs - nNECKs);
  if (i < nPMTs+nOWLs+nLGs+nBUTTs+nNECKs+nFECDs+nSPAREs)    return GetSPARE    (i - nPMTs - nOWLs - nLGs - nBUTTs - nNECKs - nFECDs);
  return NULL;
}

QPMT *QEvent::GetMCPM(Int_t i)const
{
  // Return the i'th MCPM for this event.
  QPMT *pmt = NULL;
  if (i < nMCPMs) pmt = (QPMT *)MCPMs->At(i);
  return pmt;
}
QMCVX *QEvent::GetMCVX(Int_t i)const
{
  // Return the i'th QMCVX for this event.
  QMCVX *mcvx = NULL;
  if (i < nMCVXs) mcvx = (QMCVX *)MCVXs->At(i);
  return mcvx;
}
QFit *QEvent::GetFit(Int_t i)const
{
  // Return the i'th QFit for this event.
  QFit *fit = NULL;
  if (i < nFITs) fit = (QFit *)FITs->At(i);
  return fit;
}
QMuonFit *QEvent::GetMuonFit(Int_t i)const
{
  // Return the i'th QFit for this event.
  QMuonFit *fit=NULL;
  if (i < nMuonFits) fit = (QMuonFit *)MuonFits->At(i);
  return fit;
}
QRSP *QEvent::GetRSP(Int_t i)const
{
  // Return the i'th QRSP for this event.
  QRSP *rsp = NULL;
  if (i < nRSPs) rsp = (QRSP *)RSPs->At(i);
  return rsp;
}

QFTK *QEvent::GetFTK(Int_t i)const
{
  // Return the i'th QFTK for this event.
  QFTK *ftk = NULL;
  if (i < nFTKs) ftk = (QFTK *)FTKs->At(i);
  return ftk;
}

QMuxScope *QEvent::GetMuxScope(Int_t i)const
{
  // Return the i'th NCD QMuxScope for this event.
  QMuxScope *ms = NULL;
  if (i < nMuxScopes) ms = (QMuxScope *)MuxScopes->At(i);
  return ms;
}

QMuxScope *QEvent::GetPartialMuxScope(Int_t i)const
{
  // Return the i'th NCD QMuxScope for this event.
  QMuxScope *ms = NULL;
  if (i < nPartialMuxScopes) ms = (QMuxScope *)PartialMuxScopes->At(i);
  return ms;
}

QADC *QEvent::GetShaper(Int_t i)const
{
  // Return the i'th QADC for this event.
  QADC *a = NULL;
  if (i < nShapers) a = (QADC *)Shapers->At(i);
  return a;
}

QGlobalShaper *QEvent::GetGlobalShaper(Int_t i)const
{
  QGlobalShaper *a = NULL;
  if (i < nGlobalShapers) a = (QGlobalShaper *)GlobalShapers->At(i);
  return a;
}


QFit *QEvent::GetFitByIndex(Int_t i)const
{
  // Return the fit with fIndex = fitter_index.  Returns NULL if no such fit exits.
  QFit *fit = NULL, *theFit = NULL;
  Int_t j;
  for (j=0; j < nFITs; j++) {
    fit = (QFit *)FITs->At(j);
    if (fit) { if (fit->GetIndex() == i) theFit = fit; }
  }
  return theFit;
}

QRSP *QEvent::GetRSPByIndex(Int_t i)const
{
  // Return RSP bank for fitter fIndex i.
  QRSP *rsp = NULL, * anRSP = NULL;
  Int_t j;
  for (j=0; j< nRSPs; j++) {
    anRSP = (QRSP *)RSPs->At(j);
    if (anRSP->GetFitIndex() == i) rsp = anRSP;
  }
  anRSP = NULL;
  return rsp;
}
QFTK *QEvent::GetFTKByIndex(Int_t i)const
{
  // Return FTK bank for fitter fIndex i.
  QFTK *ftk = NULL, * anFTK = NULL;
  Int_t j;
  for (j=0; j< nFTKs; j++) {
    anFTK = (QFTK *)FTKs->At(j);
    if (anFTK->GetFitIndex() == i) ftk = anFTK;
  }
  anFTK = NULL;
  return ftk;
}



QPMT *QEvent::GetQmaxPMT()
{
  // Return the PMT from this event with the maximum uncalibrated charge.
  // Charges below 300 are sent to above 4096.
  Int_t tempQ = 0, maxQ = 0;
  QPMT *mPMT  = 0, *maxQPMT = 0;
  for(Int_t i=0; i < Nhits; i++) {
    mPMT  = (QPMT *)PMTs->At(i);
    tempQ = mPMT->Getihl();
    if (tempQ < 300 ) tempQ += 4095;
    if (tempQ > maxQ ) {
      maxQ    = tempQ;
      maxQPMT = mPMT;
    }
  }
  return maxQPMT;
}

Int_t QEvent::GetMaxPMTsInCrate()
{
  // Return the maximum number of PMTs in any crate.
  Int_t i = 0, maxpmts = 0;
  Int_t incrate[19];
  for (i=0; i < 19; i++) { incrate[i] = 0; }
  QPMT *pmt;

  for (i=0; i < nPMTs; i++) {
    pmt = (QPMT *)GetQPMTs()->At(i);
    incrate[pmt->GetCrate()]++;
  }

  for (i=0; i < 19; i++) { if (incrate[i] > maxpmts) maxpmts = incrate[i]; }
  return maxpmts;
}

Int_t QEvent::GetMaxPMTsInPC()
{
  // Return the maximum number of PMTs in any Paddle card.
  Int_t i = 0, j = 0, ipc = 0, maxpmts = 0;
  Int_t inPC[64];
  QPMT *pmt;
  for (i=0; i < 19; i++) {  // Loop over all crates.
    for (j=0; j < 64; j++) { inPC[j] = 0; }  // Clear the 64 paddle cards in each crate.
    for (j=0; j < nPMTs; j++) {
      pmt = (QPMT *)GetQPMTs()->At(j);
      ipc = 4*pmt->GetCard() + pmt->GetChannel()/8;
      if (i == pmt->GetCrate()) inPC[ipc]++;
    }
    for (j=0; j < 64; j++) {
      if (inPC[j] > maxpmts) maxpmts = inPC[j];
    }
  }
  return maxpmts;
}

Int_t QEvent::GetMaxPMTsInPC(Int_t &aCrate, Int_t &aCard, Int_t &aPC, Int_t &aNTies)
{
  // Return the maximum number of PMTs in any paddle card.
  // Sets CrateofMax, CardofMax, PCofMax, and Ties
  Int_t i = 0, j = 0, ipc = 0, maxpmts = 0;
  Int_t inPC[64];
  aCrate = -1; aPC = -1; aNTies = 0;
  QPMT *pmt;
  for (i=0; i < 19; i++) {  // Loop over all crates.
    for (j=0; j < 64; j++) { inPC[j] = 0; }  // Clear the 64 paddle cards in each crate.
    for (j=0; j < nPMTs; j++) {
      pmt = (QPMT *)GetQPMTs()->At(j);
      ipc = 4*pmt->GetCard() + pmt->GetChannel()/8;
      if (i == pmt->GetCrate()) inPC[ipc]++;
    }
    for (j=0; j < 64; j++) {
      if (inPC[j] > maxpmts) {
        maxpmts = inPC[j];
        aCrate = i;
        aPC = j;
        aNTies = 0;
      }
      if (inPC[j] == maxpmts) aNTies++;
    }
  }
  aCard = aPC/4;
  aPC = aPC%4;
  return maxpmts;
}

Int_t QEvent::GetMaxLXRailedPMTsInPC(Int_t &aRailedPMTN)
{
  // Return the maximum number of PMTs in any paddle card with Pmtlx < -200.
  // Note that this is not necessarily the railed PMT but rather the middle
  // PMT in the railed paddle card.  Sets Railed PMT number.
  Int_t i = 0, j = 0, ipc = 0, maxpmts = 0, maxedCrate = 0, maxedPC = 0;
  Int_t inPC[64];
  QPMT *pmt;
  for (i=0; i < 19; i++) {  // Loop over all crates.
    for (j=0; j < 64; j++) { inPC[j] = 0; }  // Clear the 64 paddle cards in each crate.
    for (j=0; j < nPMTs; j++) {
      pmt = (QPMT *)GetQPMTs()->At(j);
      ipc = 4*pmt->GetCard() + pmt->GetChannel()/8;
      if (i == pmt->GetCrate() && pmt->Getlx() < -200 && pmt->Getlx() > -1000) inPC[ipc]++;
    }
    for (j=0; j < 64; j++) {
      if (inPC[j] > maxpmts) {
        maxpmts = inPC[j];
        maxedCrate = i;
        maxedPC = j;
      }
    }
  }
  aRailedPMTN = maxedCrate*512 + maxedPC*8 + 3;
  return maxpmts;
}

Int_t QEvent::GetNPCs()
{
  // Return the number of paddle cards which have registered at least one hit.
  Int_t inPC[19][64], nPCs = 0;
  Int_t i, icrate = 0, j = 0, ipc = 0;
  QPMT *pmt = NULL;
  for (i=0; i < 19; i++) {  //Loop over all crates.
    for (j=0; j < 64; j++) { inPC[i][j] = 0; } // Clear the 64 paddle cards in each crate.
  }     
  for (j=0; j < nPMTs; j++) {
    pmt = (QPMT *)GetQPMTs()->At(j);
    ipc = 4*pmt->GetCard() + pmt->GetChannel()/8;
    icrate = pmt->GetCrate();
    if (inPC[icrate][ipc] == 0) { 
      inPC[icrate][ipc] = 1;
      nPCs++;
    }
  }
  return nPCs;
}

QPMT *QEvent::GetZeroRawChargePMT(Int_t &aNZeroChargePMTs)
{
  // Return the PMT, if any, with all three uncalibrated charges near zero.
  // The return value of aNZeroChargePMTs is given by
  //Begin_html

  //<li>0 if no such PMTs are found
  //<li>1 if one PMT matches this condition
  //<li>n = number of PMTs which match this condition.
  //End_html
  
  Int_t i, sum = 0, sum_old = 8191;
  aNZeroChargePMTs = 0;
  QPMT *pmt = NULL, *web_pmt = NULL;
  for (i=0; i < nPMTs; i++) {
    pmt = GetPMT(i);      
    if (pmt->Getilx() == 0) {
      sum = pmt->Getihs() + pmt->Getihl();
      if (sum < sum_old) {
        web_pmt = pmt;
        aNZeroChargePMTs++;
        sum_old = sum;
      }
    }
  }
  return web_pmt;
}

void QEvent::ShowTrigger()
{
  // Display a list of triggers which have fired for this event.
  if (HasTrig("NHIT_100_LO"))     printf("NHIT_100_LO\n");
  if (HasTrig("NHIT_100_MED"))    printf("NHIT_100_MED\n");
  if (HasTrig("NHIT_100_HI"))     printf("NHIT_100_HI\n");
  if (HasTrig("NHIT_20"))         printf("NHIT_20\n");
  if (HasTrig("NHIT_20_LB"))      printf("NHIT_20_LB\n");
  if (HasTrig("ESUM_LO"))         printf("ESUM_LO\n");
  if (HasTrig("ESUM_HI"))         printf("ESUM_HI\n");
  if (HasTrig("OWLN"))            printf("OWLN\n"); 
  if (HasTrig("OWLE_LO"))         printf("OWLE_LO");
  if (HasTrig("OWLE_HI"))         printf("OWLE_HI\n");
  if (HasTrig("PULSE_GT"))        printf("PULSE_GT\n");
  if (HasTrig("PRESCALE"))        printf("PRESCALE\n");
  if (HasTrig("PEDESTAL"))        printf("PEDESTAL\n");
  if (HasTrig("PONG"))            printf("PONG\n");
  if (HasTrig("SYNC"))            printf("SYNC\n");
  if (HasTrig("EXT_ASYNC"))       printf("EXT_ASYNC\n");
  if (HasTrig("EXT2"))            printf("EXT2\n");
  if (HasTrig("EXT3"))            printf("EXT3\n");
  if (HasTrig("EXT4"))            printf("EXT4\n");
  if (HasTrig("EXT5"))            printf("EXT5\n");
  if (HasTrig("EXT6"))            printf("EXT6\n");
  if (HasTrig("EXT7"))            printf("EXT7\n");
  if (HasTrig("SPECIAL_RAW"))     printf("SPECIAL RAW\n");
  if (HasTrig("NCD"))             printf("NCD\n");
  if (HasTrig("SOFT_GT"))         printf("SOFT_GT\n");
  if (HasTrig("MISTRIGGER"))      printf("MISTRIGGER\n"); 
}
Int_t QEvent::HasTrig(char * aTrigger)
{
  // Boolean function which tests if this event contains
  // the trigger named aTrigger.  See QTrigger for trigger names.
  return GetQTrigger()->HasTrig(Trig_type,aTrigger);
}

void QEvent::Solar(Float_t &u, Float_t &v, Float_t &w, Float_t aTwist)
{
  //This routine returns u, v, and w- the direction cosine to the sun.  Normally in QSNO
  // USE_SNOMAN_SOLAR is not set- so this returns the results from Phil's SolarDirectionPH.  
  // If one recompiles QSNO with the USE_SNOMAN_SOLAR flag, this will return 
  //the snoman values.

#ifdef USE_SNOMAN_SOLAR
  // Return the direction to the Sun from SNOMAN's solar routine.
  Float_t thetwist = -1*aTwist;
  Float_t *dir_sun = new Float_t[3];
  Double_t eventjdy = (Double_t)JulianDate + (Double_t)UT1/86400 + ((Double_t)(UT2))/(1e9*86400);
  solar_(eventjdy,dir_sun,thetwist);
  u = dir_sun[0];
  v = dir_sun[1];
  w = dir_sun[2];
  delete[] dir_sun;
#else // USE_SNOMAN_SOLAR
  //These lines call QSNO's solardirection routine.
  //
  TVector3 mSunDir = QSNO::SolarDirectionPH(JulianDate,UT1,UT2,aTwist);
  u = mSunDir.x();
  v = mSunDir.y();
  w = mSunDir.z();
#endif // USE_SNOMAN_SOLAR
}

Float_t QEvent::SolarDirection(Int_t aMethod)
{
  // Return the cosine of the angle between the Sun and the detector's z-axis for this event.
  return GetThetaSun(0,0,1,aMethod);
}

Float_t QEvent::GetThetaSun(Int_t aMethod, Int_t aFitIndex, Float_t aTwist)
{
  // Return the cosine of the angle between the Sun 
  // and the direction of the first fit for this event.  
  if (aFitIndex < 0) {
    if (nMCVXs < 1) return -1.1;
    QMCVX *mcvx = GetMCVX(0);  // Use the first MCVX bank.
    return GetThetaSun(mcvx->GetU(),mcvx->GetV(),mcvx->GetW(),aMethod,aTwist );
  }
  QFit *fit = (QFit *)GetFitByIndex(aFitIndex);
  if (fit) return GetThetaSun(fit->GetU(),fit->GetV(),fit->GetW(),aMethod,aTwist);
  return -1.1;
}

Float_t QEvent::GetThetaSun(QMCVX *mcvx, Int_t aMethod)
{
  // Return the cosine of the angle between the Sun and the specified Monte-Carlo vertex.
  if (mcvx) return GetThetaSun(mcvx->GetU(),mcvx->GetV(),mcvx->GetW(),aMethod);
  else return -1.1;
}

Float_t QEvent::GetThetaSun(QFit *fit, Int_t aMethod)
{
  // Return the cosine of the angle between the Sun 
  // and the direction of the specified fit for this event.  
  if (fit) return GetThetaSun(fit->GetU(),fit->GetV(),fit->GetW(),aMethod);
  else return -1.1;
}

Float_t QEvent::GetThetaSun(Float_t uFit, Float_t vFit, Float_t wFit, Int_t aMethod, Float_t aTwist)
{
  // Return the cosine of the angle to the Sun for Ufit, Vfit, Wfit.
  //Begin_html

  //<p>
  //For aMethod = 0 -> use QSNO::SolarDirectionPH
  //            = 1 -> use SNOMAN's solar routine
  //            = 2 -> use circular orbit approximation
  //</p>
  //End_html

  Float_t direction = -1.1;
  if (aMethod == 0 || aMethod ==2) {  // Use QSNO::SolarDirection.PH
    TVector3 mNuDir(uFit, vFit, wFit);
    TVector3 mSunDir;
    if(aMethod==0)mSunDir= QSNO::SolarDirectionPH(JulianDate,UT1,UT2,aTwist);
    else mSunDir= QSNO::SolarDirection(JulianDate,UT1,UT2,aTwist);
    direction = -1*mSunDir.Dot(mNuDir);
  }
  else if (aMethod == 1) {  // Use SNOMAN's solar routine.
#ifdef USE_SNOMAN_SOLAR
    Double_t eventjdy = (Double_t)JulianDate + (Double_t)UT1/86400 + ((Double_t)(UT2))/(1e9*86400);
    Float_t *dir_sun = new Float_t[3];
    Float_t thetwist = -1*aTwist;
    solar_(eventjdy,dir_sun,thetwist);
    direction = dir_sun[0]*uFit + dir_sun[1]*vFit + dir_sun[2]*wFit;
    delete[] dir_sun;
#else 
    Error("GetThetaSun","Not compiled with USE_SNOMAN_SOLAR switch!");
#endif
  }
  else Error("GetThetaSun","Unkown solar direction calculation mode: %i\n",aMethod);
  return direction;
}

Float_t QEvent::GetThetaIJ(TVector3 &aPosition, Float_t aTime)
{
  // Return the isotropy parameter Theta_IJ for PMT hits
  // within 10 ns of the prompt light peak
  Int_t  i, j, npoints = 0;
  Float_t c_d2o = 21.87;  // Cerenkov average opening angle for n = 1.371.
  Float_t ct, tresi, tresj, theta_ij = 0;
  QPMT *pmti, *pmtj;
  for (i=0; i < nPMTs; i++) {
    pmti = GetPMT(i);
    TVector3 pmtipos(pmti->GetX(), pmti->GetY(), pmti->GetZ());
    TVector3 pmtiprime = pmtipos - aPosition;
    tresi = pmti->Gett() - aTime - pmtiprime.Mag()/c_d2o;
    for (j=0; j < nPMTs; j++) {
      pmtj = GetPMT(j);
      TVector3 pmtjpos(pmtj->GetX(), pmtj->GetY(), pmtj->GetZ());
      TVector3 pmtjprime = pmtjpos - aPosition;
      ct = pmtiprime.Dot(pmtjprime)/(pmtjprime.Mag()*pmtiprime.Mag());
      tresj = pmtj->Gett() - aTime -pmtjprime.Mag()/c_d2o;
      if (fabs(tresi) < 10 && fabs(tresj) < 10 && i != j) {
	theta_ij += acos(ct);
	npoints++;
      }
    }
  }
  if (npoints > 0) theta_ij = theta_ij/(Float_t)npoints;
  else theta_ij = -9998;
  return theta_ij;
}

Float_t QEvent::GetThetaIJ(Int_t aFitIndex)
{
  // Return the isotropy parameter Theta_IJ for PMT hits
  // within 10 ns of the prompt light peak

  if (nFITs < 1) return -1;
  QFit *fit = GetFitByIndex(aFitIndex);
  if (!fit) return -1;
  TVector3 fitpos(fit->GetX(), fit->GetY(), fit->GetZ());
  return GetThetaIJ(fitpos,fit->GetTime());
}


void QEvent::CalculateBeta(Float_t *anOutput, Int_t aFitIndex, 
			   Float_t aResidualCut ){
  if (nFITs < 1) return;
  QFit *fit = GetFitByIndex(aFitIndex);
  if (!fit) return;
  TVector3 fitpos(fit->GetX(), fit->GetY(), fit->GetZ());
  CalculateBeta(anOutput, fitpos, fit->GetTime(), aResidualCut);
}

void QEvent::CalculateBeta( Float_t *anOutput, TVector3 &aPosition, 
			    Float_t aTime, Float_t aResidualCut ){

  //Calculate beta parameters, for pmts within a "aResidualCut", given in 
  //  centimeters.  Output the results in the anOutput array, which must
  // be allocated and have room for five entries.

  const Float_t par_mean[5]={0.078, 0.019, 0.019, 0.015, 0.019};
  const Float_t par_disp[5]={0.03,  0.021, 0.018, 0.017, 0.017};
  QMath math;
  Int_t  i, j;
  Float_t c_d2o = 21.87;  // speed of light for n = 1.371.
  Float_t ct, tresi, tresj;

  int l;
  for(l=0;l<5;l++)anOutput[l]=0;

  int npmts=0;
  QPMT *pmti, *pmtj;
  for (i=0; i < nPMTs; i++) {
    pmti = GetPMT(i);
    TVector3 pmtipos(pmti->GetX(), pmti->GetY(), pmti->GetZ());
    TVector3 pmtiprime = pmtipos - aPosition;
    tresi = (pmti->Gett() - aTime)*c_d2o - pmtiprime.Mag();
    if(fabs(tresi)>aResidualCut)continue;
    ++npmts;
    for (j=i+1; j < nPMTs; j++) {
      pmtj = GetPMT(j);
      TVector3 pmtjpos(pmtj->GetX(), pmtj->GetY(), pmtj->GetZ());
      TVector3 pmtjprime = pmtjpos - aPosition;
      tresj = (pmtj->Gett() - aTime)*c_d2o -pmtjprime.Mag();
      if(fabs(tresj)>aResidualCut)continue;
      ct = pmtiprime*pmtjprime/(pmtjprime.Mag()*pmtiprime.Mag());
      for(l=0;l<5;l++)anOutput[l]+=math.plgndr(l+1,0,ct);
    }
  }

  for(l=0;l<5;l++)anOutput[l]*=(2*l+3)/(2*TMath::Pi()*npmts*(npmts-1));
  for(l=0;l<5;l++)anOutput[l]=(anOutput[l]-par_mean[l])/(2*par_disp[l])+0.5;
}

Double_t QEvent::Beta14(Float_t *aBeta){
  return (4*TMath::Pi()) * (0.02*aBeta[0]+0.016 + 4.0/9.0*(0.034*aBeta[3]-0.002));
}

Double_t QEvent::Beta14(Int_t aFitIndex){
  QFit *qf=GetFitByIndex(aFitIndex);
  if(qf==NULL)return -9999;
  return Beta14(qf->GetBeta());
}

void QEvent::MakeTime(char *aTime, Int_t aTimeZone)
{
  // Fill aTime with this event's time.  TimeZone {1,0} -> {UT, Local Time}.
  // Default is to output local time.
  //Begin_html

  //<p>
  //For aTimeZone = 0 -> use Local Time
  //              = 1 -> use UT
  //</p>
  //End_html

  const Int_t sno_jdy_offset = 1825;  // The number of days between Jan 1, 1970 and Jan 1, 1975.
  Int_t d, m, y;
  GetDate(JulianDate,d,m,y);
  time_t gmt_t;
  gmt_t = (time_t)((JulianDate+sno_jdy_offset)*86400 + UT1);
  struct tm *ltime;
  if (aTimeZone == 1)    ltime = gmtime       ((const time_t *)&gmt_t);
  else                   ltime = localtime    ((const time_t *)&gmt_t);
  char *eventtime;
  eventtime = asctime((const struct tm *)ltime);
  strcpy(aTime,eventtime);  
}

Double_t QEvent::GetDoubleTime()
{
  // Get double-precision event time in seconds since Jan 1, 1996.
  // Limited in accuracy by the precision of a double to about 30 nanoseconds.
  const Int_t date_offset = 7671;  // Julian date of Jan 1, 1996.
  return((GetJulianDate() - date_offset)*(double)86400 + GetUT1() + GetUT2()*1e-9);
}

void QEvent::GetTAC(TH1F *aSoftTACHistogram)
{
  // Fill histogram with the raw TAC histogram.
  if (!aSoftTACHistogram) return;
  Int_t i;
  QPMT *pmt = NULL;
  aSoftTACHistogram->Reset();
  for (i=0; i < nPMTs; i++) {
    pmt = GetPMT(i);
    aSoftTACHistogram->Fill(pmt->Gett());
  }
}

void QEvent::GetTimeResidual(TH1F *aTimeResidualsHistogram, Int_t anIndex, Bool_t reset)
{
  // Fill histogram with the time residuals for this event.  If reset is 
  //true, reset the histogram for each event (this is the default).
  //anIndex is the fitIndex.  
  if (!aTimeResidualsHistogram)  return;
  //  if (nFITs < 1) return;
  if(reset)aTimeResidualsHistogram->Reset();
  Int_t j;
  QFit *fit = GetFitByIndex(anIndex);
  if(fit==NULL)return;
  QPMT *pmt = NULL;
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    aTimeResidualsHistogram->Fill(pmt->TimeResidual(fit));
  }
}

void QEvent::FillCerenkov(TH1F *aCerenkovDistribution,Int_t aFitIndex)
{
  // Fill histogram with the cerenkov distribution for this event.
  // Call this function for each event and you obtain the Cerenkov distribution
  // for the entire run.
  if (!aCerenkovDistribution)  return;
  QFit *fit=GetFitByIndex(aFitIndex);
  if (fit==NULL) return;
  Int_t j;
  TVector3 fitpos(fit->GetX(),fit->GetY(),fit->GetZ());
  TVector3 fitdir(fit->GetU(), fit->GetV(), fit->GetW());
  TVector3 p;
  QPMT *pmt = NULL;
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    p=TVector3(pmt->GetX(), pmt->GetY(),pmt->GetZ());
    p=(p-fitpos).Unit(); 
    aCerenkovDistribution->Fill(fitdir*p);
  }
}

Int_t QEvent::GetNwin(Float_t aTimeEarly, Float_t aTimeLate, Int_t aFitIndex)
{
  // Return the number of in-time hits for the indexed fit specified.
  if (nFITs <= aFitIndex) return -1;
  Int_t j, nwin = 0;
  Float_t c_d2o = 22.45;
  Float_t tres, tpmt, tfit, ttransit;
  QPMT *pmt = NULL;
  QFit *fit = GetFit(aFitIndex);
  tfit = fit->GetTime();
  TVector3 rfit(fit->GetX(), fit->GetY(), fit->GetZ());
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    tpmt = pmt->Gett();
    TVector3 rpmt(pmt->GetX(), pmt->GetY(), pmt->GetZ());
    ttransit = (rpmt - rfit).Mag()/c_d2o;
    tres = tpmt - tfit - ttransit;
    if (tres > -1*aTimeEarly && tres < aTimeLate) nwin++;
  }
  return nwin;
}

Float_t QEvent::GetPromptWidth(Float_t aTimeEarly, Float_t aTimeLate, Int_t aFitIndex, TH1F *aHistogram, const char *anOption)
{
  // Return the width of the prompt time peak for this event.
  QFit *fit = GetFitByIndex(aFitIndex);
  if (!fit)  return -1.;
  QPMT *pmt = NULL;
  Int_t j;
  Float_t c_d2o = 29.98/1.371;
  Float_t timeres;
  TVector3 pmtPrime;
  TF1 fff("fff","gaus",-1*aTimeEarly,aTimeLate);
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    pmtPrime.SetXYZ(pmt->GetX() - fit->GetX(), pmt->GetY() - fit->GetY(), pmt->GetZ() - fit->GetZ());
    timeres = pmt->Gett() - fit->GetTime() - pmtPrime.Mag()/c_d2o;
    if (timeres > -1*aTimeEarly && timeres < aTimeLate) aHistogram->Fill(timeres);
  }
  aHistogram->Fit("fff",anOption);
  return (Float_t)fff.GetParameter(2);
}

Float_t QEvent::GetMedianPMTTime()
{
  // Return the median PMT time for this event.
  if (nPMTs == 0) return -9995;
  Int_t j;
  Int_t   *mTimeIndex = new Int_t[nPMTs];
  Float_t *mTimeArray = new Float_t[nPMTs];
  QPMT *pmt;
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    mTimeArray[j] = pmt->Gett();
  }
  TMath::Sort((Int_t)nPMTs,mTimeArray,mTimeIndex,kFALSE);
  Float_t mTmedian;
  Int_t   mIndex;
  if (nPMTs%2 == 0) {
    mIndex   = nPMTs/2 - 1;
    mTmedian = (mTimeArray[mTimeIndex[mIndex]] + mTimeArray[mTimeIndex[mIndex + 1]])/2;
  }
  else {
    mIndex   = nPMTs/2;
    mTmedian = mTimeArray[mTimeIndex[mIndex]];
  }
  delete[] mTimeArray;
  delete[] mTimeIndex;
  return mTmedian;
}

Float_t QEvent::GetTimeRMS()
{
  // Return the RMS time of all normal PMTs in this event.
  Int_t j, npoints = 0;
  Float_t t = 0, t_total = 0, t2_total = 0, t_rms = 0;
  QPMT *pmt = NULL;
  for (j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    t   = pmt->Gett();
    if (t > -1000) {
      t_total  += t;
      t2_total += t*t;
      npoints++;
    }
  }
  if (npoints > 2) t_rms = sqrt((t2_total - pow(t_total,2)/(float)npoints)/((float)npoints - 1));
  return t_rms;
}

Float_t QEvent::GetFitChi2(Int_t aFitIndex, Float_t aTimeEarly, Float_t aTimeLate)
{
  // Return the chi-squared for the specified fit index.
  Int_t      j, nused = 0;
  Float_t    cd2o = 29.98/1.337;
  Float_t    tres, chi2 = 0, sigma = 1.61;
  QPMT *pmt = NULL;
  QFit *fit = NULL;
  fit = GetFitByIndex(aFitIndex);
  if (!fit) return -1;
  TVector3 pmtPrime;
  for(j=0; j < nPMTs; j++) {
    pmt = GetPMT(j);
    pmtPrime.SetXYZ(pmt->GetX() - fit->GetX(), pmt->GetY() - fit->GetY(), pmt->GetZ() - fit->GetZ());
    tres = pmt->Gett() - fit->GetTime() - pmtPrime.Mag()/cd2o;
    if (tres > -1*aTimeEarly && tres < aTimeLate) {
      nused++;
      chi2 += pow(tres/sigma,2);
    }
  }
  if (nused > 4)    chi2 /= nused - 4;
  else              chi2  = -1;
  return chi2;
}

void QEvent::Fit(Int_t)  // argument was aFitIndex
{
  // Fit this event, and add a new QFit structure.
  // Warning!  Obsolete code.
  printf("Obsolete code called!\n");
  /*
    QFit *fit = AddQFIT();
    if (!gQMTF) gQMTF = new QMTF();
    gQMTF->DoFit(this);
    gQMTF->FillQFit(fit);
    if (aFitIndex) fit->SetIndex(aFitIndex);
  */
}
void QEvent::ReFit(Int_t)  //argument was aFitIndex
{ 
  // Clear the FITs for this event, and refit.
  // Warning!  Obsolete code.
  printf("Obsolete code called!\n");
  /*
    Int_t i;
    QFit *fit = 0;
    if (aFitIndex) { 
    for (i=0; i < nFITs; i++) {
    fit = (QFit *)FITs->At(i);
    if (fit->GetIndex() == aFitIndex) { FITs->RemoveAt( i ); nFITs--; }
    }
    }
    else ClearFITs();
    Fit(aFitIndex);  // Call to an obsolete function.
  */
}

// Writer functions:
void QEvent::SetEvent_id     (Int_t anEventID)       { Event_id = anEventID; }
void QEvent::SetTrig_type    (Int_t aTriggerType)    { Trig_type  = aTriggerType; }
void QEvent::SetNhits        (Int_t aNHits)          { Nhits = (Int_t)aNHits; }
void QEvent::SetNph          (Float_t aNPh)          { Nph = aNPh; }
void QEvent::SetGtr_time(Double_t aTime)
{
  // Takes the KEV_GTR word from snoman as a double (in nsec).
  Gtr_time = aTime;
}

// Reader functions:
Float_t QEvent::GetXfit()
{
  // Return the fit X-coordinate for this event.
  // Currently returns the first QFit entry.
  TClonesArray *mfits = GetQFITs();
  if (mfits && nFITs > 0) return ((QFit *)mfits->At(0))->GetX();
  else return -10000;
}
Float_t QEvent::GetYfit()
{
  // Return the fit Y-coordinate for this event.
  // Currently returns the first QFit entry.
  TClonesArray *mfits = GetQFITs();
  if (mfits && nFITs > 0) return ((QFit *)mfits->At(0))->GetY();
  else return -10000;
}
Float_t QEvent::GetZfit()
{
  // Return the fit Y-coordinate for this event.
  // Currently returns the first QFit entry.
  TClonesArray *mfits = GetQFITs();
  if (mfits && nFITs > 0) return ((QFit *)mfits->At(0))->GetZ();
  else return -10000;
}
Float_t QEvent::GetTfit()
{
  // Return the fit TAC time for this event.
  // Currently returns the first QFit entry.
  TClonesArray *mfits = GetQFITs();
  if (mfits) return ((QFit *)mfits->At(0))->GetTime();
  else return -1.0;
}

Float_t QEvent::GetRfit()
{
  // Return the radius of the event measured in PMT coordinates.
  // The services of QFit are preferable to this function.
  if (nFITs > 0) return sqrt(pow(GetXfit(),2) + pow(GetYfit(),2) + pow(GetZfit(),2));
  else return -1.0;
}
Float_t QEvent::GetRgen(Int_t anIndex)
{
  // Return the generated radius for MCVX index for this event.
  QMCVX *mcvx = GetMCVX(anIndex);
  if (mcvx) return sqrt(pow(mcvx->GetX(),2) + pow(mcvx->GetY(),2) + pow(mcvx->GetZ(),2));
  else return -1.0;
}

Bool_t QEvent::TestDamnID(Int_t aCutID)
{
  // Test the DamnID bit.
  if (DamnID&aCutID) return kTRUE;
  return kFALSE;
}
Bool_t QEvent::TestDamnID1(Int_t aCutID)
{
  // Test the DamnID1 bit.
  if (DamnID1&aCutID) return kTRUE;
  return kFALSE;
}
Bool_t QEvent::TestDarnID(Int_t aCutID)
{
  // Test the DarnID bit.
  if (DarnID&aCutID) return kTRUE;
  return kFALSE;
}

Bool_t QEvent::IsNreEvent(Bool_t ignoreRunNum)
{
  // Returns true if QNcdNreEvent::IsNreEvent returns true.
  // The NRE task was turned on starting with a test run 63137.
  // This method will return false if the event has a fRun < 63137
  // unless 'ignoreRunNum' is set to true.
  // If the argument 'ignoreRunNum' is true, then test the pulse
  // for NRE event even if the event occured before run 63137.
  // This can be used for testing the sacrifice of this
  // method/pulse type cut.
  //    Author: Adam Cox
  
  Bool_t fIsNreEvent = kFALSE;
  
  //the first test of NRE pulses was run 63137.  the NRE task probably
  //wasn't fully implemented until 63613 (according to my notes), but
  //we check the pulse anyways.
  if( (ignoreRunNum || GetRun() >= 63137) && GetnMuxScopes()){
    
    QNcdNreEvent qnre;
    for(Int_t i = 0; i < GetnMuxScopes(); i++){

      if( qnre.IsNreEvent(GetMuxScope(i)) ) fIsNreEvent = kTRUE;  
    }
  }
  
  return fIsNreEvent;
}

Int_t QEvent::GetCalibratedNCDWaveform(Float_t *delogArray,Int_t sizeOfArray,Int_t muxScope,
                                        Float_t a, Float_t b, Float_t logOff, Float_t preTrigOff)
{
  // This method will take the waveform found in the QMuxScope instance (as returned by
  // QEvent::GetMuxScope(muxScope) and delog the waveform using either the logamp
  // parameters already in the QMuxScope instance or the ones provided as arguments.  The 
  // delogged waveform will then be copied into the Float_t *data array.  
  // This method will return the following value if one of the following conditions is true:
  // returns -1 -> if there are no QMuxScope events in this QEvent (QEvent::GetnMuxScopes == 0)
  // returns -2 -> if muxScope is not valid (muxScope > QEvent::GetnMuxScopes() - 1)
  // returns -3 -> if QEvent::GetMuxScope(muxScope) returns NULL pointer
  // returns -4 -> if new QScopeAnalysis pointer is NULL
  // returns -5 -> if sizeOfArray != QMuxScope::GetSizeOfScopeTrace()
  // returns -6 -> if the logamp parameters are "bad" (*more on this below)
  // returns -7 -> if QMuxScope::GetValues() returns NULL pointer
  // returns -8 -> if QScopeAnalysis::DelogWaveform(...) returns false
  // Otherwise, this method will fill *data and return true (== 1).
  //
  // The user may specify the logamp paramaters they wish to be used to delog the waveform.
  // This is probably most useful when the NCLA bank parameters haven't already been
  // loaded into this event. If the logamp parameter argument (a, b, logOff, preTrigOff),
  // is zero, then the value already loaded in QMuxScope is used. By default, all of the 
  // logamp parameter arguments for the method are zero, which means that all of the 
  // values in QMuxScope are used.
  
  QMuxScope* qmx = NULL;
  QScopeAnalysis *qana = NULL;
  TH1C *hist = NULL;
  
  if(GetnMuxScopes() == 0) return -1;
  if(muxScope > GetnMuxScopes() - 1) return -2;
  
  qmx = GetMuxScope(muxScope);
  
  if(qmx == NULL) return -3;

  // Check if the logamp parameters passed as arguments are the same as those
  // already loaded in QMuxScope. If not print the two set of parameters and
  // uses the ones passed as arguments
  /*
  if(a != 0 && a != qmx->GetLogAmpParameter_a())
    printf("Different a parameter: NEMS QMuxScope %f %f\n",a,qmx->GetLogAmpParameter_a());
  if(b != 0 && b != qmx->GetLogAmpParameter_b())
    printf("Different b parameter: NEMS QMuxScope %f %f\n",b,qmx->GetLogAmpParameter_b());
  if(logOff != 0 && logOff != qmx->GetLogOffset())
    printf("Different c parameter: NEMS QMuxScope %f %f\n",a,qmx->GetLogOffset());
  if(preTrigOff != 0 && preTrigOff != qmx->GetScopeOffset())
    printf("Different Vpretrig parameter: NEMS QMuxScope %f %f\n",preTrigOff,qmx->GetScopeOffset());
   */
  if(a!=0) qmx->SetLogAmpParameter_a( a );
  if(b!=0) qmx->SetLogAmpParameter_b( b );
  if(logOff!=0) qmx->SetLogOffset( logOff );
  if(preTrigOff!=0) qmx->SetScopeOffset( preTrigOff );
  
  qana = new QScopeAnalysis();
  if( qana == NULL ) return -4;
  
  if(sizeOfArray != qmx->GetSizeOfScopeTrace()) {
    if(qana != NULL) delete qana;
    return -5;  
  }
  
  if(!qmx->IsGoodLogAmpParameters()) {
    if(qana != NULL) delete qana;
    return -6;  
  }
  
  
  hist = qmx->GetValues();
  if(hist == NULL) {
    if(qana != NULL) delete qana;
    return -7;  
  }
  
  
  TH1F delog("delog","delog",sizeOfArray,0,sizeOfArray);
  
  if(!qana->DelogWaveform(*hist,delog,
                          qmx->GetLogAmpParameter_a(),
                          qmx->GetLogAmpParameter_b(),
                          qmx->GetLogOffset(),
                          qmx->GetScopeOffset()
                          )
     ) {
    if(qana != NULL) delete qana;
    return -8;  
  }
  
  for(Int_t i = 0; i < sizeOfArray; i++) {
    delogArray[i] = delog.GetBinContent(i);
  }
  
  if(qana != NULL) delete qana;
  
  return 1;
}


//______________________________________________________________________________
void QEvent::Streamer(TBuffer &R__b)
{
  // Stream an object of class QEvent.
  // Root automatic I/O is used for qsno versions above 18

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s,&R__c); 
    if (R__v > 18) {  // Use ROOT 3 automatic I/O.
      QEvent::Class()->ReadBuffer(R__b,this,R__v,R__s,R__c);
      return;
    }
    QSNO::Streamer(R__b);
    PMTs->Streamer(R__b); 
    OWLs->Streamer(R__b);
    LGs->Streamer(R__b);
    if (R__v > 12)    SPAREs->Streamer(R__b);
    else              SPAREs = 0;
    if (R__v > 9) {
      BUTTs->Streamer(R__b);
      NECKs->Streamer(R__b);
    }
    else {
      BUTTs = NULL;
      NECKs = NULL;
    }
    if (R__v > 1)    FECDs->Streamer(R__b);
    else             FECDs = 0;
    FITs->Streamer(R__b);
    if (R__v >= 6)     MCVXs->Streamer(R__b);
    if (R__v >= 7)     RSPs->Streamer(R__b);
    if (R__v >= 17)    MCPMs->Streamer(R__b);
    R__b >> nPMTs;
    R__b >> nOWLs;
    R__b >> nLGs;
    if (R__v > 9) {
      R__b >> nBUTTs;
      R__b >> nNECKs;
    }
    else {
      nBUTTs = 0;
      nNECKs = 0;
    }
    if (R__v > 1)    R__b >> nFECDs;
    else             nFECDs = 0;
    if (R__v > 12) {
      R__b >> nSPAREs;
      R__b >> nPBUNs; 
    }
    else {
      nSPAREs = 0;
      nPBUNs  = 0;
    }
    R__b >> nFITs;
    if (R__v >= 6)     R__b >> nMCVXs;
    if (R__v >= 7)     R__b >> nRSPs;
    if (R__v >= 17)    R__b >> nMCPMs;
    R__b >> Event_id;
    R__b >> Trig_type;
    R__b >> Nhits;
    R__b >> Nph;
    if (R__v >= 11) {
      R__b >> tPrev;
      R__b >> tNext;
    }
    else {
      tPrev = 0;
      tNext = 0;
    }
    R__b >> Gtr_time;
    R__b >> JulianDate;
    R__b >> UT1;
    R__b >> UT2;
    if (R__v == 1) {
      R__b >> nLGs;
      R__b >> nLGs;
      nLGs = 0;
    }
    if (R__v < 3) {
      Int_t *r1 = 0; Float_t *r2 = 0;
      R__b.ReadArray(r1);
      R__b.ReadArray(r2);
    }
    if (R__v >= 4)    R__b >> Run;
    if (R__v > 7)     R__b >> DamnID;
    if (R__v > 13)    R__b >> DarnID;
    if (R__v > 8) {
      R__b >> EsumPeak;
      R__b >> EsumDiff;
      R__b >> EsumInt;
    }
    if (R__v > 15)    R__b >> Time10MHz;
    else              Time10MHz = 0;
    if (R__v > 16)    R__b >> TrigErr;
    else              TrigErr = 0;
  } 
  else QEvent::Class()->WriteBuffer(R__b,this);
}
