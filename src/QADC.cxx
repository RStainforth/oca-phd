//_____________________________________________________
//
// QADC
//*-- Author :  Adam Cox
// 
//  versions: 1 10/24/02 -- original
//            2 3/16/03 -- updated move towards QSNO coding standards compliance
//            3 10/08/03 -- becoming compatible with SNOMAN
//            4 01/27/05 -- Added mux event correlation (GTID)pointer (ckrauss@owl.phy.queensu.ca)
//


#include "QADC.h"
ClassImp(QADC);

QADC::QADC()
{
  return;
}

QADC::QADC(Int_t *ivar)
{
  Float_t *rvar=(Float_t *)ivar;
  fADCCharge = ivar[0];
  fShaperChannelNumber = ivar[1];
  fShaperSlotNumber = ivar[2];
  fShaperHardwareAddress = ivar[3];
  fNCDStringNumber = ivar[4];
  fCalCharge = rvar[5];
  fEnergy = rvar[6];
  fADCtslh = 0;

  // fMuxEventGTID is initialized to KNECL_NO_GTID (=-99999).  Previous 
  // QSNO versions initialized this to 0xFFFFFFFF  
  fMuxEventGTID = -99999;   
  fCorrMuxDeltaT = -999999.;
}

QADC::QADC(Int_t *ivar, Int_t *jvar, Float_t *fvar)
{

  Float_t *rvar=(Float_t *)ivar;
  fADCCharge = ivar[0];
  fShaperChannelNumber = ivar[1];
  fShaperSlotNumber = ivar[2];
  fShaperHardwareAddress = ivar[3];
  fNCDStringNumber = ivar[4];
  fCalCharge = rvar[5];
  fEnergy = rvar[6];
  fADCtslh = 0;

  // jvar[0] stores the ADC charge from NECL bank, which is redundant given 
  // ivar[0] above
  fMuxEventGTID = jvar[1];  
  fCorrMuxDeltaT = fvar[0];
}

QADC::QADC(const QADC& aQADC) : TObject (aQADC)
{
  *this=aQADC;
}

QADC::~QADC()
{
  return;
}

Bool_t QADC::IsSame(Int_t anIndex, const QADC *aShaper, Int_t aPrintMode)const{
  const Int_t *data=Data();
  const Int_t *data2=aShaper->Data();
  int i,j;
  Int_t n=DataN() - 1; //-1 since fADCtslh is not used right now (CKrauss@owl)
  for(j=i=0;i<=n;i++)if(data[i]!=data2[i])j++;
  if(j>0 &&aPrintMode==1){
    printf("QADC:%d  Differences in words ",anIndex);
    for(i=0;i<=n;i++) if (data[i]!=data2[i]) printf(" %d",i);
    printf("\n");
  }
  return (j==0);
}

  Bool_t QADC::IsHe4(){
    return ( fNCDStringNumber==3 || fNCDStringNumber==10 ||
              fNCDStringNumber==20 || fNCDStringNumber==30 );  //true if string is a Helium 4 string
  }
