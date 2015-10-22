//* Author: M. Boulay
//* Copyright (c) 1998 M. Boulay, All Rights Reserved.

#ifndef _q_cast_h
#define _q_cast_h

#include "QBank.h"

class QCast : public QBank
{
public:
  QCast();
  QCast( Int_t * rawData );
  ~QCast();
  //  QCast( const QCast &cast );
  //  QCast &operator=(const QCast &rhs );
  Bool_t IsOK();
  Int_t icons(Int_t i);
  Float_t rcons(Int_t i);
  void Initialize();

  void Set( Int_t *x);
  ClassDef(QCast,2)//CAST bank data

};

#endif





