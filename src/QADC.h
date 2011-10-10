//__________________________________________________________
//
// QADC.h
//*-- Author : Adam Cox
//	UpDates: 
//		1 10/01/02 -- original version
//              2 03/16/03 -- updated move towards QSNO coding standards compliance
//              3 10/08/03 -- becoming compatible with SNOMAN
//              4 01/27/05 -- Added mux event correlation (GTID)pointer (ckrauss@owl.phy.queensu.ca)
//              5 21/10/05 -- Revert GetADCCharge to original meaning
//                            Add GetCharge (calibrated) and GetEnergy
//

#ifndef _QADCRecord_h
#define _QADCRecord_h

#include "TObject.h"

class QADC : public TObject
{
public:
  //friend classes
  friend class MNCD2Root;

  QADC ();
  QADC (const QADC &aQADC);
  QADC (Int_t *ivars);
  QADC (Int_t *ivars, Int_t *jvars, Float_t *fvars);
  virtual ~QADC ();
  Int_t    GetShaperChannelNumber() {return fShaperChannelNumber;}	
  Int_t    GetShaperSlotNumber() {return fShaperSlotNumber;}	

  Float_t  GetEnergy()    {return fEnergy;}
  Float_t  GetCharge()    {return fCalCharge;}
  Int_t    GetADCCharge() {return fADCCharge;}		

  Int_t    GetShaperHardwareAddress() {return fShaperHardwareAddress;}
  Int_t    GetNCDStringNumber() {return fNCDStringNumber;}
  Float_t  GetTSLH() {return fADCtslh;}	
  Int_t    GetCorrMUXGTID() {return fMuxEventGTID;}
  Float_t  GetCorrMUXDeltaT() {return fCorrMuxDeltaT;}

  void SetShaperChannelNumber(Int_t aNumber) {fShaperChannelNumber = aNumber;}
  void SetShaperSlotNumber(Int_t aNumber) {fShaperSlotNumber = aNumber;}

  void SetADCCharge(Int_t aCharge)  {fADCCharge = aCharge;}
  void SetCharge(Float_t  aCharge)  {fCalCharge = aCharge;}
  void SetEnergy(Float_t  anEnergy) {fEnergy    = anEnergy;}

  void SetShaperHardwareAddress(Int_t anId) {fShaperHardwareAddress = anId;}
  void SetNCDStringNumber(Int_t aNumber) {fNCDStringNumber = aNumber;}

  void SetTSLH(Float_t aTime) {fADCtslh = aTime;}
  void SetCorrMUXGTID(Int_t id) {fMuxEventGTID = id;}
  void SetCorrMUXDeltaT(Float_t aDeltaT) {fCorrMuxDeltaT = aDeltaT;}


  Bool_t IsSame(Int_t anIndex, const QADC *aShaper, Int_t aPrintMode=0)const;
  Bool_t IsHe4();  //true if string is a helium-4 string
private:
  const Int_t *Data()const{return (Int_t *)&fEnergy;}
  Int_t DataN()const{int i= ((Int_t *)&fCorrMuxDeltaT)-Data();return i;}
public:
  ClassDef(QADC,9);//ADC class for NCD shaper events
private:  
  //Note-- the following members are linked to the fortran code in SNOMAN.  Their order or type should not
  // be changed unless the NES bank is also changed in SNOMAN.  
  Float_t fEnergy;	        //calibrated ADC energy
  Float_t fCalCharge;	        //calibrated charge (au)
  Int_t   fShaperChannelNumber; //Shaper Channel Number (0 - 7)
  Int_t   fShaperSlotNumber;    //Shaper's slot number (fake, ORCA assinged, VME slot number)
  Int_t   fShaperHardwareAddress;      //Shaper hardware address - obtained from
  Int_t   fNCDStringNumber;     //NCD String number obtained by matching hardware numbers from the NCD Titles banks
  Int_t   fADCCharge;           //uncalibrated ADC charge (aka ADC bin)
  //The following values are not in the Event Structure
  Float_t fADCtslh; 	        //Time since last hit
  Int_t fMuxEventGTID;          //GTID of MUX event correlated to this event
  Float_t fCorrMuxDeltaT;       //Delta t to correlated MUX event
};


#endif



