#ifndef __QBANK_H
#define __QBANK_H

#include "TObject.h"
#include "QDBHDR.h"
#include "TArrayI.h"
#include <TString.h>
#include <TNtuple.h>

//*-- Author : Mark Boulay                          

class QBank : public TObject
{
 public:
  QBank(const char *name = "UNKN" , Int_t number = 1);
  QBank(const QBank &bank);
  QBank &operator=(const QBank &rhs);
  ~QBank();
  TArrayI *GetData(){ return &fData; }
  Int_t GetWord( Int_t i ){ return (Int_t)fData.At(i-1); }
  Int_t icons( Int_t i ){ return (Int_t)fData.At(i-1); }
  Float_t rcons( Int_t i );
  QDBHDR *GetHeader(){ return &fHeader; }
  Int_t GetBankNumber(){ return fBankNumber; }
  const char * GetBankName(){ return (const char *)fBankName; }
  const char * GetIOChar(){ return (const char *)fIOChar; }
  
  void SetBankNumber(Int_t num){ fBankNumber = num; }
  void SetBankName(const char * name ){ fBankName = name; }
  void SetIOChar(const char *iochar){ fIOChar = iochar; }
  void Seticons( Int_t iloc, Int_t iword );
  void Setrcons( Int_t iloc, Float_t rword );
 
  Bool_t IsValid( Int_t date, Int_t time, Int_t dtype );
  Bool_t IsData();
  Bool_t IsGap();
  void Draw(Option_t *);
  
  
  ClassDef(QBank, 1)//Standard titles bank

 protected:
  QDBHDR fHeader;     //Database header
  Int_t fBankNumber;  //Bank number, < 0 for database gap
  TString fBankName;  //Bank name
  TString fIOChar;    //Zebra IO characteristics
  TArrayI fData;      //Bank data
};

#endif








