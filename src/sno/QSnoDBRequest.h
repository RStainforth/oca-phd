#ifndef __QSnoDBRequest_h
#define __QSnoDBRequest_h

#include <TObject.h>
#include "QGlobals.h"
#include <TString.h>
class QSnoDBRequest : public TObject
{
 public:
  QSnoDBRequest(const char *bankname = "UNKN", Int_t banknumber = 1, Int_t date = 19980101, Int_t time = 0,
	       Int_t dtype = 11);
  ~QSnoDBRequest();
  void SetDate(Int_t date){ fDate = date;}
  void SetTime(Int_t time){ fTime = time;}
  void SetBankName( const char *name ){ fBankName = name; }
  void SetBankNumber( Int_t num ){ fBankNumber = num; }
  void SetDataType(Int_t dtype ){ fDataType = dtype; }
  void SetLevel( EQMTLevel level ){ fLevel = level; }
  void SetFile( const char *fname ){ fFileName = fname; fTitlesFile = kTRUE; }
  const char *GetFileName(){ return (const char *)fFileName; }
  Bool_t ReadTitlesFile(){ return fTitlesFile; }
  Int_t GetDate(){ return fDate;}
  Int_t GetTime(){ return fTime;}
  Int_t GetDataType(){ return fDataType;}
  const char *GetBankName(){ return (const char *)fBankName; }
  Int_t GetBankNumber(){ return fBankNumber; }
  Int_t GetLevel(){ return fLevel; }
  ClassDef(QSnoDBRequest, 1) //Request packet for QSnoDB

 private:
  Int_t fDate;
  Int_t fTime;
  Int_t fBankNumber;
  Int_t fDataType;
  EQMTLevel fLevel;
  TString fBankName;
  TString fFileName;
  Bool_t fTitlesFile;

};


#endif
