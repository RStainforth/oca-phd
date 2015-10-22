//* Author: Peter S.

#ifndef _q_sosl_h
#define _q_sosl_h

#include "QBank.h"

class QSosl : public QBank
{
 public:
  QSosl();
  QSosl( Int_t * rawData );
  ~QSosl();
  //  QSosl( const QSosl &sosl );
  //  QSosl &operator=(const QSosl &rhs );
  Bool_t IsOK();
  Int_t icons(Int_t i);
  Float_t rcons(Int_t i);
  void Initialize();

  void Set( Int_t *x);
  ClassDef(QSosl,1)      //SOSL bank data

		};

#endif

