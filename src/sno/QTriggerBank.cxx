////////////////////////////////////////////////////////////////////
//SNO Trigger bank.  Contains information from the Zebra          //
//BANK_TRIG.                                                      //
//                                                                //
// Written by:  M. Boulay, Queen's University                     //
//              02/07/1998                                        //
////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay

#include "QTriggerBank.h"

ClassImp(QTriggerBank)

QTriggerBank::QTriggerBank()
{
Initialize();
//Default constructor.
};
QTriggerBank::~QTriggerBank()
{
//Destructor.
};

void QTriggerBank::Initialize()
{
  //Initialize the QTriggerBank object.
 
}

void QTriggerBank::Set(Int_t *x)
{
  //Set the trigger bank.   The following elements will be filled
  //Begin_html
  //<li>  TRIG_MASK      = x[0]
  //<li>  N100LO         = x[1]
  //<li>  N100MED        = x[2]
  //<li>  N100HI         = x[3]
  //<li>  N20            = x[4]
  //<li>  N20LB          = x[5]
  //<li>  ESUMLO         = x[6]
  //<li>  ESUMHI         = x[7]
  //<li>  OWLELO         = x[8]
  //<li>  OWLEHI         = x[9]
  //<li>  OWLN           = x[10]
  //<li>  N100LO_ZERO    = x[11]
  //<li>  N100MED_ZERO   = x[12]
  //<li>  N100HI_ZERO    = x[13]
  //<li>  N20_ZERO       = x[14]
  //<li>  N20LB_ZERO     = x[15]
  //<li>  ESUMLO_ZERO    = x[16]
  //<li>  ESUMHI_ZERO    = x[17]
  //<li>  OWLN_ZERO      = x[18]
  //<li>  OWLELO_ZERO    = x[19]
  //<li>  OWLEHI_ZERO    = x[20]
  //<li>  PULSER_RATE    = x[21]
  //<li>  MTC_CSR        = x[22]
  //<li>  LKOUT_WIDTH    = x[23]
  //<li>  PRESCALE_FRAC  = x[24]
  //<li>  GTID           = x[25]
  //End_html

  TRIG_MASK      = x[0];
  N100LO         = x[1];
  N100MED        = x[2];
  N100HI         = x[3];
  N20            = x[4];
  N20LB          = x[5];
  ESUMLO         = x[6];
  ESUMHI         = x[7];
  OWLELO         = x[8];
  OWLEHI         = x[9];
  OWLN           = x[10];
  N100LO_ZERO    = x[11];
  N100MED_ZERO   = x[12];
  N100HI_ZERO    = x[13];
  N20_ZERO       = x[14];
  N20LB_ZERO     = x[15];
  ESUMLO_ZERO    = x[16];
  ESUMHI_ZERO    = x[17];
  OWLN_ZERO      = x[18];
  OWLELO_ZERO    = x[19];
  OWLEHI_ZERO    = x[20];
  PULSER_RATE    = x[21];
  MTC_CSR        = x[22];
  LKOUT_WIDTH    = x[23];
  PRESCALE_FRAC  = x[24];
  GTID           = x[25];
  
}


