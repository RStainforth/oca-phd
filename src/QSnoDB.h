#ifndef __QSNODB_CLIENT_H
#define __QSNODB_CLIENT_H


#include "QSnoDBRequest.h"
#include "QBank.h"
#include "QClient.h"
#include "QEvent.h"
#include "TSystem.h"

class QEvent;
class QSnoDB : public QClient
{
 public:
  QSnoDB(const char *server = "localhost");
  ~QSnoDB();
  QBank *GetBank(const char *name, Int_t banknumber, Int_t date, Int_t time, Int_t dtype = 11);
  QBank *GetBank( QSnoDBRequest *request);
  void SetQuiet(Bool_t val) { quiet = val; }
  Bool_t IsServerOK() { return serverOK; }
  ClassDef(QSnoDB,0) //Interface to SNODB (Client)
 private:
  Bool_t quiet;	   // supress printf messages
  Bool_t serverOK; // flag indicating that the server is OK
};

#endif












