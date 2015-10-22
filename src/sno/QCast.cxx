////////////////////////////////////////////////////
//QCast                                           //
//                                                //
//QCast casts SNOMAN data types to be used by     //
//QSNO.                                           //
////////////////////////////////////////////////////

//*---Author: M.Boulay

#include "QCast.h"

ClassImp(QCast)

QCast::QCast(){;}
QCast::~QCast(){;}
QCast::QCast( Int_t *mRawData ){;}

void QCast::Initialize(){;}
void QCast::Set( Int_t *mData)
{
  //Set the data words for this bank
  fData.Set(15, mData);
}


Int_t QCast::icons(Int_t i)
{
  //Return the ith word from the icons array. 
  if ( fData.GetSize() <= i ) return -9999;
  else return QBank::icons(i);
}


Float_t QCast::rcons(Int_t i)
{
  //Return ith word from rcons array.
  if ( fData.GetSize() <= i ) return -9999.;
  else return QBank::rcons(i);
}

Bool_t QCast::IsOK()
{
  //Verify that this bank is OK.

  if ( fData.GetSize() <= 0 ) return kFALSE;
  else                        return kTRUE;
}
