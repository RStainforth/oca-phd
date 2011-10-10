#ifndef _Qtrig_h
#define _Qtrig_h

#include <TObject.h>
#include <TString.h>
#include <stdio.h>
#include "QSNO.h"
#include "QGlobals.h"
//*-- Author : Mark Boulay                          
//* Copyright (c) 1998 Mark Boulay      

class QTrigger : public QSNO
{
public:
  QTrigger();
  ~QTrigger();
  Int_t HasTrig(Int_t mTrigWord, char *cTrig);
  ClassDef(QTrigger,0)//Unpacked trigger word
private:
  char **cTriggerNames;
  //TString cTriggerNames[26];
};

#endif

















