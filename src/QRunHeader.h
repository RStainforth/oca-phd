//* Author: M. Boulay
//* Copyright (c) 1998 M. Boulay, All Rights Reserved.

#ifndef _q_runh_h
#define _q_runh_h

#include "QMath.h"
#include <TObject.h>
#include "QSNO.h"

class QRunHeader : public QSNO
{
public:
  QRunHeader();
  QRunHeader( UInt_t * rawData );
  ~QRunHeader();
  QRunHeader( const QRunHeader &header );
  QRunHeader &operator=(const QRunHeader &rhs );
  void Initialize();
  Int_t GetRun();
  Int_t GetDate(){ return Date;}
  Int_t GetTime(){ return mTime;}
  Int_t GetDACVersion(){ return DACVersion;}
  Int_t GetCalTrial(){ return CalTrial;}
  Int_t GetSourceMask(){ return SourceMask;}
  Int_t GetRunMask(){ return RunMask;}
  Int_t GetGTCrateMask(){ return GTCrateMask;}
  Int_t GetFirstGTID() { return FirstGTID; }
  Int_t GetValidGTID() { return ValidGTID; }

  void Set( Int_t *x);
  void Set(UInt_t *y);
  ClassDef(QRunHeader,2)//SNO run header

private:
  Int_t Date;//Date in format yyyymmdd
  Int_t mTime;//Time in format hhmmsscc
  Int_t DACVersion;//DAC Version number
  Int_t RunNumber;//Run number
  Int_t CalTrial;//Calibration trial number
  Int_t SourceMask;//Mask denoting which sources are present
  Int_t RunMask;//Run type
  Int_t GTCrateMask;//Mask denoting which crates are masked in
  Int_t FirstGTID;//first GTID of this run
  Int_t ValidGTID;//first valid GTID of this run
  
};

#endif





