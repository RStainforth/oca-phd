///////////////////////////////////////////////////////////////////////////////
//QSnoDBRequest                                                              //
//                                                                           //
//Request packet used for communicating with QSnoDB.                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//*-- Author : Mark Boulay 26/11/98

#include "QSnoDBRequest.h"

ClassImp(QSnoDBRequest)

QSnoDBRequest::QSnoDBRequest(const char * bankname, Int_t banknumber, Int_t date, Int_t time, Int_t dtype)
{
  //Default constructor
  SetBankName( bankname );
  SetBankNumber( banknumber );
  SetDate( date );
  SetTime( time );
  SetDataType( dtype );
  fTitlesFile = kFALSE;
}

QSnoDBRequest::~QSnoDBRequest()
{
  //Destructor
}





