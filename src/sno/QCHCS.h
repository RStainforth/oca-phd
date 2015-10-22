#ifndef __QCHCS_H
#define __QCHCS_H

//*--Author: Ranpal Dosanjh
// wrapper to load and access CHCS banks.

#include "QBank.h"
#include "QTitles.h"

class QCHCS : public TObject
{
 public:
 QCHCS(TString filename, Int_t date=0, Int_t time=0);
 QCHCS(TString filename, Int_t jd, Int_t ut1, Int_t ut2);
 ~QCHCS();
 Bool_t IsBadPmt(Int_t pmtn);

 ClassDef(QCHCS,0) // CHCS bank reader
   
   private:
 QBank *fBank;		// CHCS titles bank
 QTitles *fTitles;	// CHCS titles object
 Bool_t *fBadPmt;	// CHCS decision for each pmt

 void Initialize();
 void LoadTitles(TString filename, Int_t date, Int_t time);
 void SetChcsBits();

};

#endif

