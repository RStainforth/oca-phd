////////////////////////////////////////////////////
//QSosl                                           //
//                                                //
//QSosl casts SNOMAN data types to be used by     //
//QSNO.                                           //
////////////////////////////////////////////////////

//*---Author: Peter S

#include "QSosl.h"

ClassImp(QSosl)

QSosl::QSosl(){;}
QSosl::~QSosl(){;}
QSosl::QSosl( Int_t *mRawData ){;}

void QSosl::Initialize(){;}
void QSosl::Set( Int_t *mData)
{
  //Set the data words for this bank
  fData.Set(13, mData);
}


Int_t QSosl::icons(Int_t i)
{
  //Return the ith word from the icons array. 
  if ( fData.GetSize() <= i ) return -9999;
  else return QBank::icons(i);
}


Float_t QSosl::rcons(Int_t i)
{
  //Return ith word from rcons array.
  if ( fData.GetSize() <= i ) return -9999.;
  else return QBank::rcons(i);
}

Bool_t QSosl::IsOK()
{
  //Verify that this bank is OK.

  if ( fData.GetSize() <= 0 ) return kFALSE;
  else                        return kTRUE;
}

