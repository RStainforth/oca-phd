////////////////////////////////////////////////////////////////////////////////////
//QBank                                                                           //
//Structure used to manipulate ZEBRA-style SNODB banks.                           //
//                                                                                //
//Written by M. Boulay Dec/10 98                                                  //
////////////////////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay
#include "QBank.h"

ClassImp(QBank)

QBank *gCRON = NULL;
QBank *gVRLY = NULL;

QBank::QBank(const char *name, Int_t number)
{
  //Default constructor
  fBankNumber  = number;
  fBankName    = name;
  fIOChar      = "";
}

QBank::QBank( const QBank &bank )
{
  //Copy constructor for QBank objects.
  fHeader     = bank.fHeader;
  fBankNumber = bank.fBankNumber;
  fBankName   = bank.fBankName;
  fData       = bank.fData;
}

QBank & QBank::operator=(const QBank &rhs)
{
  //Assignment operator.
  fHeader     = rhs.fHeader;
  fBankNumber = rhs.fBankNumber;
  fBankName   = rhs.fBankName;
  fData       = rhs.fData;
  return *this;
}

QBank::~QBank()
{
  //Destructor
}

Float_t QBank::rcons( Int_t i)
{
 //Retreives bank data element number i. 
 Int_t *ar = fData.GetArray();
 return *(float *)( &ar[i-1] ); 
}

void QBank::Seticons( Int_t i, Int_t iword)
{
  //Set word i to iword.
  if ( fData.GetSize() < i ) fData.Set( fData.GetSize() + 100 );
  fData.AddAt( iword, i-1 );
}

void QBank::Setrcons( Int_t i, Float_t rword )
{
  //Set word i to rword.
  if ( fData.GetSize() < i ) fData.Set( fData.GetSize() + 100 );
  Int_t *raw = fData.GetArray();
  memcpy(&raw[i-1], &rword, 4);
}

Bool_t QBank::IsValid(Int_t date, Int_t time, Int_t datatype)
{
  //Test if this bank's validity range includes date (yyyymmdd) and time (hhmmsscc).  IsValid will return kTRUE if
  //this is a valid bank, or if this is a bank representing a gap in the database for the given data and time.
  
  Int_t EffStartDate = fHeader.GetEffStartDate();
  Int_t EffStartTime = fHeader.GetEffStartTime();
  Int_t EffEndDate   = fHeader.GetEffEndDate();
  Int_t EffEndTime   = fHeader.GetEffEndTime();

  
  //Test the given date and time against the bank's header.

      if ( date < EffStartDate || date > EffEndDate ) return kFALSE;
      else if ( ( date == EffStartDate && time < EffStartTime ) || ( date == EffEndDate && time > EffEndTime ) ) return kFALSE;
      else return kTRUE;
}

Bool_t QBank::IsData()
{
  //Determine if this bank contains data.
  //- now handles case of banks numbered 0 - PH 11/30/00
  if ( GetData()->GetSize() > 0 ) return kTRUE;
  return kFALSE;
}

Bool_t QBank::IsGap()
{
  //Determine if this bank represents a gap in the database.
  if ( fBankNumber < 0 ) return kTRUE;
  return kFALSE;
}

void QBank::Draw(Option_t *)
{
  //Generate a histogram of the data in this bank.
  //useful for homogeneous data.
  Int_t i;
  
  TNtuple ntp("ntp",this->GetBankName(),this->GetBankName());
  Float_t val;
  for (i=1; i < fData.GetSize(); i++)
    {
      val = rcons(i);
      ntp.Fill( &val );
    }
  char cut[100];
  sprintf(cut,"%s != 0",this->GetBankName());
  ntp.SetEstimate( fData.GetSize() );
  ntp.Draw(this->GetBankName(), cut);
}







