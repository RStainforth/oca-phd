////////////////////////////////////////////////////////////////////
//SNO Database header bank.  Contains information from the Zebra        //
//titles bank DBHDR.                                               //
//                                                                //
// Written by:  M. Boulay, Queen's University                     //
//              02/07/1998                                        //
////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay


#include "QDBHDR.h"

ClassImp(QDBHDR)

QDBHDR::QDBHDR()
{
Initialize();
//Default constructor.
};
QDBHDR::~QDBHDR()
{
//Destructor.
};

void QDBHDR::Initialize()
{
  //Initialize the QThresholdBank object.
}

void QDBHDR::Set(Int_t *x)
{
  //Set the database header  bank. Array x is in the order of the SNO database header.

  START_DATE        = x[0];
  START_TIME        = x[1];
  END_DATE          = x[2];
  END_TIME          = x[3];
  DATA_TYPE         = x[4];
  TASK_TYPE         = x[5];
  FORMAT_NUM        = x[6];
  CREATION_DATE     = x[7];
  CREATION_TIME     = x[8];
  SOURCE_ID         = x[9];
  EFF_START_DATE    = x[10];
  EFF_START_TIME    = x[11];
  EFF_END_DATE      = x[12];
  EFF_END_TIME      = x[13];
  ENTRY_DATE        = x[14];
  ENTRY_TIME        = x[15];
  CHECKSUM1         = x[16];
  CHECKSUM2         = x[17];
  SOURCE            = x[18];
  MODIFIED          = x[19];
  MANAGED_ID        = x[20];
  MBT_PTR           = x[21];
  PRINT_FLAG        = x[22];
  
}


