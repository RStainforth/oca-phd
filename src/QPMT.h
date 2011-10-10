#ifndef _Pmtbun_h
#define _Pmtbun_h

#include "QSnoDB.h"
#include <TObject.h>
#include <Rtypes.h>
#include "QPMTxyz.h"
#include "QGlobals.h"
#include <math.h>
#include "TArrayI.h"
#include "TArrayF.h"
#include "QBank.h"
#include "QSnoDB.h"

//*-- Author : Mark Boulay                          
//* Copyright (c) 1998 Mark Boulay.   Send comments or bug reports to
// <mailto::mgb@owl.phy.queensu.ca>      
typedef short int QShortInt_t;


class QFit;
class QPMT : public TObject
{
public:
  QPMT();
  QPMT( const QPMT &pmt );
  QPMT &operator=(const QPMT &rhs);
  QPMT(Int_t *ivars, Float_t *rvars);
  virtual ~QPMT();
  void Initialize();
  void Set(Int_t *ivars, Float_t *rvars);

//Time residuals

Float_t TimeResidual(QFit* fit);
Float_t TimeResidual(QFit* fit, Float_t * deriv);
Float_t TimeResidual(Float_t xf, Float_t yf, Float_t zf, Float_t tf);
Float_t TimeResidual(Float_t xf, Float_t yf, Float_t zf, Float_t tf, 
                           Float_t* deriv);
//Writer functions

  void Setn(Int_t mPmtn)		{ Pmtn = (QShortInt_t)mPmtn; }
  void SetCell(Int_t mCell)		{ PmtCell = (QShortInt_t)mCell; }
  void Setit(Int_t mPmtit)		{ Pmtit = (QShortInt_t)mPmtit; }
  void Sett( Float_t mPmtt )	        { Pmtt = mPmtt; }
  void Setihl(Int_t mPmtihl)	        { Pmtihl = (QShortInt_t)mPmtihl; }
  void Setihs(Int_t mPmtihs)	        { Pmtihs = (QShortInt_t)mPmtihs; }
  void Setilx(Int_t mPmtilx)	        { Pmtilx = (QShortInt_t)mPmtilx; }
  void Sethl(Float_t mPmthl)	        { Pmthl = mPmthl; }
  void Seths(Float_t mPmths)	        { Pmths = mPmths; }
  void Setlx(Float_t mPmtlx)	        { Pmtlx = mPmtlx; }
  void SetStatus(Int_t mStatus)	        { PmtStatus = mStatus; }
  void SetCMOSStatus(Int_t mCMOSStatus) { PmtCMOSStatus = mCMOSStatus; }
  void SetEcaVal( Int_t mEca )          { PmtEcaVal = mEca;    }
  void Settslh(Float_t t)               { Pmttslh = t;}
  void Setdelta(Float_t t)              { Pmtdelta = t;}
  void SetStatusBit(Int_t mBit);
  void ClearStatusBit(Int_t mBit);
  void SetCMOSStatusBit(Int_t mBit);
  void ClearCMOSStatusBit(Int_t mBit);
  void Setrc(Float_t anRc)                { Pmtrc = anRc;}
  
//Reader functions

Int_t Getn()const{
  //Returns the PMT logical circuit number, defined as
  //Begin_html
  //<l>Pmtn = 512*Logical Crate + 32*Logical Channel + Logical Card
  //End_html
 return (Int_t)Pmtn;}
Int_t Getit()const{
  //Returns the PMT uncalibrated time.
return (Int_t)Pmtit;}
Int_t Getihl()const{
  //Returns the PMT uncalibrated high gain, long integrated charge.
return (Int_t)Pmtihl;}
Int_t Getihs()const{
  //Return the PMT uncalibrated high gain, short integrated charge.
return (Int_t)Pmtihs;}
Int_t Getilx()const{return Pmtilx;
//Returns the PMT uncalibrated low gain charge.
}
Float_t Gethl()const{
 //Returns the PMT calibrated high gain, long integrated charge.
return Pmthl;}
Float_t Geths()const{
  //Returns the PMT calibrated high gain, short integrated charge.
return Pmths;}
Float_t Getlx()const{return Pmtlx;
// Returns the PMT calibrated low gain charge.
}
Float_t Gett()const{return Pmtt;
//Returns the calibrated PMT time, in nsec.
}
Int_t GetCell()const;
 Int_t GetCCC(){
   //Return the CCC (Card/Crate/Channel) No. for this PMT.
   return GetPMTxyz()->GetCCC( Pmtn );
 }
Int_t GetStatus()const;
Int_t GetCMOSStatus()const;
 Int_t GetEcaVal() const{ return PmtEcaVal; }
Int_t GetStatusBit(Int_t mBit);
Int_t GetCMOSStatusBit(Int_t mBit);
 Float_t Gettslh()const{ return Pmttslh; }
 Float_t Getdelta()const{ return Pmtdelta; }
 Float_t Getrc()const{return Pmtrc;}
 //TArrayI *GetSpareI()const{ return &SpareI; }
 //TArrayF *GetSpareF()const{ return &SpareF; }

Bool_t IsTubeOn();
Bool_t IsChannelOn();
Int_t   GetType()           { return GetPMTxyz()->GetType( Pmtn );          }
Bool_t  IsNormalPMT()       { return GetPMTxyz()->IsNormalPMT( Pmtn );      }
Bool_t  IsOWLPMT()          { return GetPMTxyz()->IsOWLPMT( Pmtn );         }
Bool_t  IsLGPMT()           { return GetPMTxyz()->IsLGPMT( Pmtn );          }
Bool_t  IsCalibrationPMT()  { return GetPMTxyz()->IsCalibrationPMT( Pmtn ); }
Bool_t  IsSparePMT()        { return GetPMTxyz()->IsSparePMT( Pmtn );       }
Bool_t  IsInvalidPMT()      { return GetPMTxyz()->IsInvalidPMT( Pmtn );     }

Float_t GetX(){
//Return the x-coordinate of this PMT.
return GetPMTxyz()->GetX(Pmtn);}
Bool_t Is75Blown()
  {
    //Return the status of this PMT's 75 Ohm terminator.
    return GetPMTxyz()->Is75Blown( Pmtn ); }
Float_t GetY(){
//Return the y-coordinate of this PMT.
return GetPMTxyz()->GetY(Pmtn);}
Float_t GetZ(){
//Return the z-coordinate of this PMT.
return GetPMTxyz()->GetZ(Pmtn);}
Int_t GetCrate(){
//Return the Crate associated with this PMT.
return GetPMTxyz()->GetCrate(Pmtn);}
Int_t GetCard(){
//Return the Card associated with this PMT.
return GetPMTxyz()->GetCard(Pmtn);}
Int_t GetChannel(){
//Return the Channel associated with this PMT.
return GetPMTxyz()->GetChannel(Pmtn);
}
 Int_t GetQueensNo(){
   //Return the Queen's number for this PMT.
   return GetPMTxyz()->GetQueensNo(Pmtn);
 }
 Int_t GetSnomanNo(){
   //Return the Snoman number for this PMT.
   return GetPMTxyz()->GetSnomanNo(Pmtn);
 }
 QPMTxyz *GetPMTxyz(){
//if ( !gPMTxyz ) gPMTxyz = new QPMTxyz("read"); return gPMTxyz;}
return gSNO->GetPMTxyz();
 }
 
 Bool_t IsSame(const char *comment, Int_t j,const QPMT *aPmt, Int_t aPrintMode=0)const;
 
 ClassDef(QPMT,9)//One PMT bundle 

   private:
 QShortInt_t Pmtn;//Logical PMT number
 QShortInt_t Pmtit;//Uncalibrated PMT time (Tac)
 QShortInt_t Pmtihl;//Uncalibrated high gain, long integrated charge
 QShortInt_t Pmtihs;//Uncalibrated high gain, short integrated charge
 QShortInt_t Pmtilx;//Uncalibrated low gain, x-integrated charge
 QShortInt_t PmtCell;//PMT cell number
 //QShortInt_t PmtCrate;//PMT Crate
 //QShortInt_t PmtCard;//PMT Card
 //QShortInt_t PmtChannel;//PMT Channel
 Int_t PmtStatus;//PMT Status Flag (PMT_PF)
 Int_t PmtCMOSStatus;//CMOS Status Flag (KPMT_PIF)
 Float_t Pmthl;//Calibrated high gain, long integrated charge
 Float_t Pmths;//Calibrated high gain, short integrated charge
 Float_t Pmtlx;//Calibrated low gain, x-integrated charge
 Float_t Pmtt;//Calibrated Pmt time (nsec)
 Float_t Pmttslh;//Time since last hit
 Float_t Pmtdelta;//PMT delta (TSLH decay)
 //TArrayI SpareI;//Spare integer words.
 //TArrayF SpareF;//Spare float words.
 Int_t PmtEcaVal; //ECA validation word
 Float_t Pmtrc; // Charge correction for rate dependent shifting
};


#endif



