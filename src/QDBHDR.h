//* Author: M. Boulay
//* Copyright (c) 1998 M. Boulay, All Rights Reserved.

#ifndef _q_dbhdr_h
#define _q_dbhdr_h

#include <TObject.h>
#include "QSNO.h"
#include "QGlobals.h"

class QSNO;
class QDBHDR : public QSNO
{
public:
  QDBHDR();
  ~QDBHDR();
  void Initialize();
  void Set(Int_t *x);
  Int_t GetStartDate(){ return START_DATE; }
  Int_t GetStartTime(){ return START_TIME; }
  Int_t GetEndDate(){ return END_DATE; }
  Int_t GetEndTime(){ return END_TIME;}
  Int_t GetDataType(){ return DATA_TYPE; }
  Int_t GetTaskType(){ return TASK_TYPE; }
  Int_t GetCreationDate(){ return CREATION_DATE; }
  Int_t GetCreationTime(){ return CREATION_TIME; }
  Int_t GetEffStartDate() { return EFF_START_DATE; }
  Int_t GetEffStartTime() { return EFF_START_TIME; }
  Int_t GetEffEndDate(){ return EFF_END_DATE; }
  Int_t GetEffEndTime(){ return EFF_END_TIME; }
  Int_t GetEntryDate(){ return ENTRY_DATE; }
  void SetEffStartDate( Int_t date ){ EFF_START_DATE = date; }
  void SetEffStartTime( Int_t time ){ EFF_START_TIME = time; }
  void SetEffEndDate( Int_t date ){ EFF_END_DATE = date; }
  void SetEffEndTime( Int_t time ){ EFF_END_TIME = time; }
  ClassDef(QDBHDR,1)//SNO Database header (DBHDR)

private:
  Int_t START_DATE;//Intrinsic start date in form YYYYMMD.
  Int_t START_TIME;//Intrinsic start time in form HHMMSSCC.
  Int_t END_DATE;//Intrinsic end date in form YYYYMMDD.
  Int_t END_TIME;//Intrinsic end time in form HHMMSSCC.
  Int_t DATA_TYPE;//Data type.See Note 3
  Int_t TASK_TYPE;//Task number.  Currently set to zero.  
  Int_t FORMAT_NUM;//Format number. Currently set to zero.  
  Int_t CREATION_DATE;//Date at which bank was created in form YYYYMMDD.
  Int_t CREATION_TIME;//Time at which bank was created in form HHMMSSCC.
  Int_t SOURCE_ID;//To be defined.
  Int_t EFF_START_DATE;//Effective start date in form YYYYMMDD. 
  Int_t EFF_START_TIME;//Effective start time in form HHMMSSCC.
  Int_t EFF_END_DATE;//Effective end date in form YYYYMMDD.
  Int_t EFF_END_TIME;//Effective end time in form HHMMSSCC.
  Int_t ENTRY_DATE;//Date at which bank was entered into the Master
  //     database in form YYYYMMDD.
  Int_t ENTRY_TIME;//Time at which bank was entered into the Master
  //     database in form HHMMSSCC.
  Int_t CHECKSUM1;  //Two word checksum for the data part of Zebra bank (Word1)
  Int_t CHECKSUM2;  //Word 2
  
  Int_t SOURCE;//Source of the bank for current job,= 0 From database, =2 from titles file.
  Int_t MODIFIED;//Number of words modified by SET BANK commands.
  Int_t MANAGED_ID;//Connection Id. 
  Int_t MBT_PTR;//Link to the appropriate entry in MTT's internalManaged Bank Table. 
  Int_t PRINT_FLAG;//Print flag
};

#endif





