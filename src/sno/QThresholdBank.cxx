////////////////////////////////////////////////////////////////////
//SNO Threshold bank.  Contains information from the Zebra        //
//titles bank VTHR (now DQCH).                                               //
//                                                                //
// Written by:  M. Boulay, Queen's University                     //
//              02/07/1998                                        //
////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay

#include "QThresholdBank.h"

ClassImp(QThresholdBank)

QThresholdBank::QThresholdBank()
{
Initialize();
//Default constructor.
};
QThresholdBank::~QThresholdBank()
{
//Destructor.
};

void QThresholdBank::Initialize()
{
  //Initialize the QThresholdBank object.
  fChannelStatus.Set(10000);
}

//void QThresholdBank::Set(Int_t word1, Int_t ncrate,Int_t *x)
//{
  //Set the Threshold bank.   The following element will be set
  //Begin_html
  //<l>Word1     = word1;
  //<l>NUM_CRATE = ncrate;
  //<l>Vthreshold[lpmt] = (Char_t)x[lpmt];
  //End_html

//Word1 = word1;
//  NUM_CRATE = ncrate;
//  Int_t i;
//  for (i =0; i< 10000; i++) Vthreshold[i] = (Char_t)x[i];
//  
//}

//______________________________________________________________________________
void QThresholdBank::Streamer(TBuffer &R__b)
{
   // Stream an object of class QThresholdBank.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
      if (R__v > 2 ) 
	{
	  //Use ROOT 3 automatic I/O
	  QThresholdBank::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
	  return;
	}
      QSNO::Streamer(R__b);
      R__b >> Word1;
      R__b >> NUM_CRATE;
      if (R__v == 1 )
	{
	  char Vthreshold[10000];
	  R__b.ReadStaticArray(Vthreshold);
	}
      else fChannelStatus.Streamer(R__b);
      fDBHDR.Streamer(R__b);
   } else {
     QThresholdBank::Class()->WriteBuffer(R__b, this);
   }
}


