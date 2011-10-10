//* Author: M. Boulay
//* Copyright (c) 1998 M. Boulay, All Rights Reserved.

#ifndef _q_trigb_h
#define _q_trigb_h

#include <TObject.h>
#include "QSNO.h"

class QTriggerBank : public QSNO
{
public:
  QTriggerBank();
  ~QTriggerBank();
  void Initialize();
  Int_t GetTRIG_MASK(){return TRIG_MASK;}
  Int_t GetN100LO(){return N100LO;}
  Int_t GetN100MED(){return N100MED;}
  Int_t GetN100HI(){return N100HI;}
  Int_t GetN20(){return N20;}
  Int_t GetN20LB(){return N20LB;}
  Int_t GetESUMLO(){return ESUMLO;}
  Int_t GetESUMHI(){return ESUMHI;}
  Int_t GetOWLELO(){return OWLELO;}
  Int_t GetOWLEHI(){return OWLEHI;}
  Int_t GetOWLN(){return OWLN;}
  Int_t GetN100LO_ZERO(){return N100LO_ZERO;}
  Int_t GetN100MED_ZERO(){return N100MED_ZERO;}
  Int_t GetN100HI_ZERO(){return N100HI_ZERO;}
  Int_t GetN20_ZERO(){return N20_ZERO;}
  Int_t GetN20LB_ZERO(){return N20LB_ZERO;}
  Int_t GetESUMLO_ZERO(){return ESUMLO_ZERO;}
  Int_t GetESUMHI_ZERO(){return ESUMHI_ZERO;}
  Int_t GetOWLN_ZERO(){return OWLN_ZERO;}
  Int_t GetOWLELO_ZERO(){return OWLELO_ZERO;}
  Int_t GetOWLEHI_ZERO(){return OWLEHI_ZERO;}
  Int_t GetPULSER_RATE(){return PULSER_RATE;}
  Int_t GetMTC_CSR(){return MTC_CSR;}
  Int_t GetLKOUT_WIDTH(){return LKOUT_WIDTH;}
  Int_t GetPRESCALE_FRAC(){return PRESCALE_FRAC;}
  Int_t GetGTID(){return GTID;}
  void Set( Int_t *x);
  ClassDef(QTriggerBank,1)//SNO run header

private:
  Int_t TRIG_MASK; //Trigger mask
  Int_t N100LO;//NHIT_100_LO threshold
  Int_t N100MED;//NHIT_100_MED threshold
  Int_t N100HI;//NHIT_100_HI threshold
  Int_t N20; //NHIT_20 threshold
  Int_t N20LB;//NHIT_20_LB threshold
  Int_t ESUMLO;//ESUM_LO threshold
  Int_t ESUMHI;//ESUM_HI threshold
  Int_t OWLELO;//OWLE_LO threshold
  Int_t OWLEHI;//OWLE_HI threshold
  Int_t OWLN;//OWLN threshold
  Int_t N100LO_ZERO;//NHIT_100_LO zero
  Int_t N100MED_ZERO;//NHIT_100_MED zero
  Int_t N100HI_ZERO;//NHIT_100_HI zero
  Int_t N20_ZERO;//NHIT_20 zero
  Int_t N20LB_ZERO;//NHIT_20_LB zero
  Int_t ESUMLO_ZERO;//ESUM_LO zero
  Int_t ESUMHI_ZERO;//ESUM_HI zero
  Int_t OWLN_ZERO;//OWLN zero
  Int_t OWLELO_ZERO;//OWLE_LO zero
  Int_t OWLEHI_ZERO;//OWLE_HI zero
  Int_t PULSER_RATE;//MTCD pulser frequency
  Int_t MTC_CSR;//MTCD control and status register
  Int_t LKOUT_WIDTH;//Lockout width
  Int_t PRESCALE_FRAC;//Prescale setting
  Int_t GTID;//GTID indicating start of validity
};

#endif





