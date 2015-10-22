#include "QPMT.h"
#include <stdlib.h>
//*-- Author : Mark Boulay 06/98

//____________________________________________________________________________
////////////////////////////////////////////////////////
//                                                    //
//QPMT: contains a bundle of information for a single //
//PMT. Also performs time residual calculations.      //
//                                                    //
//Written by M. Boulay 06/98                          //
////////////////////////////////////////////////////////

#include "QPMT.h"
#include <stdlib.h>
ClassImp(QPMT)

QPMT::QPMT(Int_t *ivars, Float_t *rvars)
{
  //Default QPMT constructor.  Fills the PMT-level data
  //as follows:
  //Begin_html
  //<l>Pmtn   =  (QShortInt_t)ivars[0];
  //<l>PmtCell=  (QShortInt_t)ivars[1];
  //<l>PmtStatus=             ivars[2];
  //<l>Pmtihl =  (QShortInt_t)rvars[0];
  //<l>Pmtihs =  (QShortInt_t)rvars[1];
  //<l>Pmtilx =  (QShortInt_t)rvars[2];
  //<l>Pmtit  =  (QShortInt_t)rvars[3];
  //<l>Pmthl  =               rvars[4];
  //<l>Pmths  =               rvars[5];
  //<l>Pmtlx  =               rvars[6];
  //<l>Pmtt   =               rvars[7];
  //<l>PmtEcaVal =            ivars[3];
  //<l>PmtCMOSStatus =        ivars[4];
  //End_html
  Initialize();
  Set(ivars, rvars);

}

QPMT::QPMT()
{
  //QPMT constructor.
  Initialize();
}
QPMT::~QPMT(){
  //Default destructor.
}

QPMT::QPMT( const QPMT &pmt ):TObject(pmt)
{
  //Copy constructor for QPMT objects.
  Pmtn = pmt.Pmtn;
  Pmtit = pmt.Pmtit;
  Pmtihl = pmt.Pmtihl;
  Pmtihs = pmt.Pmtihs;
  Pmtilx = pmt.Pmtilx;
  PmtCell = pmt.PmtCell;
  PmtStatus = pmt.PmtStatus;
  PmtCMOSStatus = pmt.PmtCMOSStatus;
  Pmthl = pmt.Pmthl;
  Pmths = pmt.Pmths;
  Pmtlx = pmt.Pmtlx;
  Pmtt = pmt.Pmtt;
  Pmttslh = pmt.Pmttslh;
  Pmtdelta = pmt.Pmtdelta;
  //SpareI = pmt.SpareI;
  //SpareF = pmt.SpareF;
  PmtEcaVal = pmt.PmtEcaVal;
  Pmtrc = pmt.Pmtrc;
  Int_t j;
  for (j=0; j < 32; j++){
    if ( pmt.TestBit(j) ) SetBit(j);
  }
}

QPMT & QPMT::operator=(const QPMT &rhs)
{
  //QPMT assignment operator.
  Pmtn = rhs.Pmtn;
  Pmtit = rhs.Pmtit;
  Pmtihl = rhs.Pmtihl;
  Pmtihs = rhs.Pmtihs;
  Pmtilx = rhs.Pmtilx;
  PmtCell = rhs.PmtCell;
  PmtStatus = rhs.PmtStatus;
  PmtCMOSStatus = rhs.PmtCMOSStatus;
  Pmthl = rhs.Pmthl;
  Pmths = rhs.Pmths;
  Pmtlx = rhs.Pmtlx;
  Pmtt = rhs.Pmtt;
  Pmttslh = rhs.Pmttslh;
  Pmtdelta = rhs.Pmtdelta;
  //SpareI = rhs.SpareI;
  //SpareF = rhs.SpareF;
  PmtEcaVal = rhs.PmtEcaVal;
  Pmtrc = rhs.Pmtrc; 
  Int_t j;
  for (j=0; j < 32; j++){
    if ( rhs.TestBit(j) ) SetBit(j);
  }
  return *this;
}

void QPMT::Set(Int_t *ivars, Float_t *rvars)
{
  //Set the PMT data members.

  Pmtn =  (QShortInt_t)ivars[0];
  PmtCell=(QShortInt_t)ivars[1];
  PmtStatus=           ivars[2];
  PmtEcaVal =          ivars[3];
  PmtCMOSStatus=       ivars[4];
  Pmtihl=  (QShortInt_t)rvars[0];
  Pmtihs = (QShortInt_t)rvars[1];
  Pmtilx = (QShortInt_t)rvars[2];
  Pmtit =  (QShortInt_t)rvars[3];
  Pmthl =               rvars[4];
  Pmths =               rvars[5];
  Pmtlx=                rvars[6];
  Pmtt=                 rvars[7];
  Pmttslh=              rvars[8];
  Pmtdelta=             rvars[9];
  Pmtrc =              rvars[10];

}


Bool_t QPMT::IsSame(const char *comment, Int_t anIndex,const QPMT *aPmt, Int_t aPrintMode)const
{
  int j=0;
  if (Pmtn!=aPmt->Getn())j++;
  if (Pmtit!=aPmt->Getit())j++;
  if (Pmtihl!=aPmt->Getihl())j++;
  if (Pmtihs!=aPmt->Getihs())j++;
  if (Pmtilx!=aPmt->Getilx())j++;
  if (PmtCell!=aPmt->GetCell())j++;
  if (PmtStatus!=aPmt->GetStatus())j++;
  if (PmtCMOSStatus!=aPmt->GetCMOSStatus())j++;
  if (Pmtt!=aPmt->Gett())j++;
  if (Pmthl!=aPmt->Gethl())j++;
  if (Pmths!=aPmt->Geths())j++;
  if (Pmtlx!=aPmt->Getlx())j++;
  if (Pmttslh!=aPmt->Gettslh())j++;
  if (Pmtdelta!=aPmt->Getdelta())j++;
  if (PmtEcaVal!=aPmt->GetEcaVal())j++;
	if (Pmtrc!=aPmt->Getrc())j++;
  if(j>0 &&aPrintMode==1){
    printf("%s:%d  Differences in words ",comment,anIndex);
    if (Pmtn!=aPmt->Getn())printf(" n");
    if (Pmtit!=aPmt->Getit())printf(" it");
    if (Pmtihl!=aPmt->Getihl())printf(" ihl");
    if (Pmtihs!=aPmt->Getihs())printf(" ihs");
    if (Pmtilx!=aPmt->Getilx())printf(" ilx");
    if (PmtCell!=aPmt->GetCell())printf(" Cell");
    if (PmtStatus!=aPmt->GetStatus())printf(" Status");
    if (PmtCMOSStatus!=aPmt->GetCMOSStatus())printf(" CMOSStatus");
    if (Pmtt!=aPmt->Gett())printf(" t");
    if (Pmthl!=aPmt->Gethl())printf(" hl");
    if (Pmths!=aPmt->Geths())printf(" hs");
    if (Pmtlx!=aPmt->Getlx())printf(" lx");
    if (Pmttslh!=aPmt->Gettslh())printf(" tslh");
    if (Pmtdelta!=aPmt->Getdelta())printf(" delta");
    if (PmtEcaVal!=aPmt->GetEcaVal())printf(" EcaVal");
		if (Pmtrc!=aPmt->Getrc())printf( "Pmtrc");
    printf("\n");
  }
  return (j==0);
}
 
void QPMT::Initialize()
{
  //Initialize the QPMT.
  Pmtn=PmtCell=PmtStatus=PmtEcaVal=PmtCMOSStatus=0;
  Pmtihl=Pmtihs=Pmtilx=Pmtit=0;
  Pmthl=Pmths=Pmtlx=Pmtt=Pmttslh=Pmtdelta=0.0;
}

void QPMT::SetStatusBit(Int_t mBit){
  //Set the mBith'th bit of the status word
  PmtStatus = PmtStatus | (Int_t)pow(2.,(Double_t)mBit);
}
void QPMT::ClearStatusBit(Int_t mBit){
  //Clear the mBith'th bit of the status word
  PmtStatus = PmtStatus & ~(Int_t)pow(2,(Double_t)mBit);
}
Int_t QPMT::GetStatusBit(Int_t mBit){
  //Return the status bit mBit
  return PmtStatus & (Int_t)pow(2,(Double_t)mBit);
}
Int_t QPMT::GetStatus()const{
  //Returns PmtStatus, a copy of KPMT_PF
return PmtStatus;}


void QPMT::SetCMOSStatusBit(Int_t mBit){
  //Set the mBit'th bit of the CMOS status word
  PmtCMOSStatus = PmtCMOSStatus | (Int_t)pow(2.,(Double_t)mBit);
}
void QPMT::ClearCMOSStatusBit(Int_t mBit){
  //Clear the mBit'th bit of the CMOS status word
  PmtCMOSStatus = PmtCMOSStatus & ~(Int_t)pow(2,(Double_t)mBit);
}
Int_t QPMT::GetCMOSStatusBit(Int_t mBit){
  //Return the CMOS status bit mBit
  return PmtCMOSStatus & (Int_t)pow(2,(Double_t)mBit);
}
Int_t QPMT::GetCMOSStatus()const{
  //Returns PmtCMOSStatus, a copy of KPMT_PIF
return PmtCMOSStatus;}


Int_t QPMT::GetCell()const
{
  //Retrun the Cell which was used for this PMT.
  return (Int_t)PmtCell;
}


Bool_t QPMT::IsChannelOn()
{
  //Return the status of this hardware channel based on the DQXX bank.
  return GetPMTxyz()->IsChannelOn(Pmtn);
}

Bool_t QPMT::IsTubeOn()
{
  //Return the status of this PMT based on the DQXX bank.
  //This requires the the channel be on, as well as the HV resistor, threshold within range, etc.
  return GetPMTxyz()->IsTubeOn(Pmtn);
}

//______________________________________________________________________________
void QPMT::Streamer(TBuffer &R__b)
{
   // Stream an object of class QPMT.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
     if (R__v > 5) 
       { 
	 //Use ROOT 3 automatic I/O
	 QPMT::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
	 return;
       }
      TObject::Streamer(R__b);
      R__b >> Pmtn;
      R__b >> Pmtit;
      R__b >> Pmtihl;
      R__b >> Pmtihs;
      R__b >> Pmtilx;
      R__b >> PmtCell;
      R__b >> PmtStatus;
      R__b >> Pmthl;
      R__b >> Pmths;
      R__b >> Pmtlx;
      R__b >> Pmtt;
      if ( R__v > 1 )
	{
	  R__b >> Pmttslh;
	}
      else 
	{
	  Pmttslh = 0;
	}
      if ( R__v > 2 )
	{
	  R__b >> Pmtdelta;
	}
      else
	{
	  Pmtdelta = 0;
	}
      if ( R__v < 4 )
	{
	  TArrayI b1;
	  TArrayF b2;
	  b1.Streamer(R__b);
	  b2.Streamer(R__b);
	}
      if ( R__v > 4 )
	{
	  R__b >> PmtEcaVal;
	}
//       PmtCrate = (QShortInt_t)GetPMTxyz()->GetCrate((Int_t)Pmtn);
//       PmtCard  = (QShortInt_t)GetPMTxyz()->GetCard( (Int_t ) Pmtn );
//       PmtChannel = (QShortInt_t)GetPMTxyz()->GetChannel( (Int_t) Pmtn);
   } else {
     //Use automatic I/O
     QPMT::Class()->WriteBuffer(R__b, this);
   }
}

//______________________________________________________________________________

Float_t QPMT::TimeResidual(QFit* fit) {
  // Calculates the time residual for a PMT relative to a given fit
  // The residual is calculated according to t_pmt - t_ev - |x_pmt - x_ev|/(c/n)
  // CJJ, June 31, 1999

  Float_t xf = fit->GetX();
  Float_t yf = fit->GetY();
  Float_t zf = fit->GetZ();
  Float_t tf = fit->GetTime();

  return TimeResidual(xf,yf,zf,tf);
}

Float_t QPMT::TimeResidual(QFit* fit, Float_t* deriv) {
  // Calculates the time residual for a PMT relative to a given fit
  // The residual is calculated according to t_pmt - t_ev - |x_pmt - x_ev|/(c/n)
  // Also calculates the derivative dy/da_k used in equation 15.5.11 in NRinC
  // time is the deriv[0]; x,y, and z are 1,2, and 3
  // CJJ, June 31, 1999

  Float_t xf = fit->GetX();
  Float_t yf = fit->GetY();
  Float_t zf = fit->GetZ();
  Float_t tf = fit->GetTime();

  return TimeResidual(xf,yf,zf,tf,deriv);
}

Float_t QPMT::TimeResidual(Float_t xf, Float_t yf, Float_t zf, Float_t tf) {
  // Calculates the time residual for a PMT relative to a given position, x,y,z,t
  // The residual is calculated according to t_pmt - t_ev - |x_pmt - x_ev|/(c/n)
  // CJJ, June 31, 1999

  
  Float_t xp = this->GetX();
  Float_t yp = this->GetY();
  Float_t zp = this->GetZ();
  Float_t tp = this->Gett();

  Float_t cbyn = 29.97/1.371;

  Float_t sqroot = (xf-xp)*(xf-xp) + (yf-yp)*(yf-yp) + (zf-zp)*(zf-zp);
  sqroot = sqrt(sqroot);
  Float_t tR = tp - tf -sqroot/cbyn; 
  return tR;

}

Float_t QPMT::TimeResidual(Float_t xf, Float_t yf, Float_t zf, Float_t tf, 
                           Float_t* deriv) {
  // Calculates the time residual for a PMT relative to a given x,y,z,t
  // The residual is calculated according to t_pmt - t_ev - |x_pmt - x_ev|/(c/n)
  // Also calculates the derivative dy/da_k used in equation 15.5.11 in NRinC
  // time is the deriv[0]; x,y, and z are 1,2, and 3
  // CJJ, June 31, 1999

  
  Float_t xp = this->GetX();
  Float_t yp = this->GetY();
  Float_t zp = this->GetZ();
  Float_t tp = this->Gett();

  Float_t cbyn = 29.97/1.371;

  Float_t sqroot = (xf-xp)*(xf-xp) + (yf-yp)*(yf-yp) + (zf-zp)*(zf-zp);
  sqroot = sqrt(sqroot);
  Float_t prefix = -1/(sqroot*cbyn);

  deriv[0] = 1.0;
  deriv[1] = prefix*(xp-xf);
  deriv[2] = prefix*(yp-yf);
  deriv[3] = prefix*(zp-zf);

  Float_t tR = tp - tf -sqroot/cbyn; 
  return tR;

}


