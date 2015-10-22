//* Author: M. Boulay
//* Copyright (c) 1998 M. Boulay, All Rights Reserved.

#ifndef _q_thresh_h
#define _q_thresh_h

#include <TObject.h>
#include "QSNO.h"
#include "QDBHDR.h"
#include <TArrayI.h>
#include <TClass.h>

class QThresholdBank : public QSNO
{
public:
  QThresholdBank();
  ~QThresholdBank();
  void Initialize();
  //void Set(Int_t word1, Int_t ncrates, Int_t *x);
  void SetChannelStatus( Int_t lpmt, Int_t status ){ fChannelStatus.AddAt(lpmt, status); } 
  Int_t GetVThreshold( Int_t lpmt ){ return 0; }
  Int_t GetVThresholdZero( Int_t lpmt ){ return 0; }
  Bool_t IsTubeOn( Int_t lpmt ){ return fChannelStatus.At(lpmt) != 0 ; }
  QDBHDR *GetDBHDR(){ return &fDBHDR; }
  ClassDef(QThresholdBank,3)//DAC Thresholds (now DQCH)

private:
  Int_t Word1;//First word of VTHR bank
  Int_t NUM_CRATE;//Number of crates
  TArrayI fChannelStatus;//32 bit channel status word
  QDBHDR fDBHDR;//Datebase Header
};

#endif





