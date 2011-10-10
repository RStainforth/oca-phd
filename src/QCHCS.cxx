//////////////////////////////////////////////////////////////////////////////
//QCHCS                                                                     //
//                                                                          //
// Allows CHCS banks to be read according to date and time.  A value of 0   //
// for the date gives the first CHCS bank.  Currently accesses banks named  //
// RSPS.  Maybe expand to look for CHCS aswell.                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

//*--Author : Ranpal Dosanjh 2003.09

#include "QCHCS.h"
#include "QSNO.h"

ClassImp(QCHCS)

QCHCS::QCHCS(TString filename, Int_t date, Int_t time)
{
  // Default constructor.
  Initialize();
  LoadTitles(filename,date,time);
  SetChcsBits();

}

QCHCS::QCHCS(TString filename, Int_t jd, Int_t ut1, Int_t ut2)
{
  // Overloaded to allow use of julian day, etc.
  Int_t date, time;
  gSNO->ConvertDate(jd,ut1,ut2,date,time);
  Initialize();
  LoadTitles(filename,date,time);
  SetChcsBits();

}

QCHCS::~QCHCS()
{
  // Destructor.

  if (fBank) delete fBank;
  if (fTitles) delete fTitles;
  if (fBadPmt) delete[] fBadPmt;

}

void QCHCS::Initialize()
{
  // Initialize variables.
  fBank = NULL;
  fTitles = NULL;
  fBadPmt = NULL;
}

void QCHCS::LoadTitles(TString filename, Int_t date, Int_t time)
{
  // Fill fTitles from filename according to calendar date and time.
  // Puts bank into fBank as well.
  // 
  // Currently uses name "RSPS".  Maybe expand to look for "CHCS" first.
  // Leave this for now, since QTitles gives fatal error if bank is not
  // found.

  TString bnam = "RSPS";
  // TString bnam = "CHCS";
  if (fTitles) fTitles = NULL;
  if (date == 0) fTitles = new QTitles((char *)filename.Data(),(char *)bnam.Data(),1);
  else fTitles = new QTitles((char *)filename.Data(),(char *)bnam.Data(),1,date,time);
  fBank = fTitles->GetBank();

  return;
}

void QCHCS::SetChcsBits()
{
  // Uses fBank to set the bits of fBadPmt.
  
  if (fBadPmt) fBadPmt = NULL;

  Int_t ipmt;
  Int_t npmt = 10000;
  Int_t ndata = fBank->icons(1);
  fBadPmt = new Bool_t[npmt];
  for (ipmt=0; ipmt<npmt; ipmt++) fBadPmt[ipmt] = kFALSE;
  for (ipmt=1; ipmt<ndata; ipmt++) fBadPmt[fBank->icons(ipmt+1)] = kTRUE;

  return;

}

Bool_t QCHCS::IsBadPmt(Int_t pmtn)
{
  // Return kTRUE if pmtn is bad according to CHCS, kFALSE otherwise.

  return fBadPmt[pmtn];
}

