//*--Author : Mark Boulay
//*--Author : Bryce Moffat - updated 24-Apr-2000 for libqoca from libqtree
//*--Author : Bryce Moffat - updated 6-Mar-2001 for QOCAFit status word
//*--Copyright (c) 1999-2001 SNO Software Project

////////////////////////////////////////////////////////////////////////////////
// QOCAPmt                                                                    //
//                                                                            //
// Data structure and methods useful for storing OCA data for individual PMTs.//
////////////////////////////////////////////////////////////////////////////////

#include "QOCAPmt.h"

ClassImp(QOCAPmt)
;
//______________________________________________________________________________
//
// QOCAPmt class.
//
// Data structure and methods useful for storing OCA data for individual PMTs.
//
//______________________________________________________________________________
QOCAPmt::QOCAPmt()
{
  // Default constructor.

  fNtimeW = 0;

}
//______________________________________________________________________________
QOCAPmt::~QOCAPmt()
{
  // Default destructor.

}
//______________________________________________________________________________
QOCAPmt::QOCAPmt(const QOCAPmt &rhs)
{
  // Copy constructor
  // Copies all data members for the pmt

  fPmtn = rhs.fPmtn;
  fPmtx = rhs.fPmtx;
  fPmty = rhs.fPmty;
  fPmtz = rhs.fPmtz;

  //fPmteff = rhs.fPmteff;
  fPmteffm = rhs.fPmteffm;
  //fPmteffq = rhs.fPmteffq;
  fPmteffc = rhs.fPmteffc;
  //fThreshold = rhs.fThreshold;
  fTprompt = rhs.fTprompt;
  fTflight = rhs.fTflight;
  fTwidth = rhs.fTwidth;
  fNprompt = rhs.fNprompt;
  fOccupancy = rhs.fOccupancy;
  fOccupancyerr = rhs.fOccupancyerr;
  fOccCorrection = rhs.fOccCorrection;
  fNcdReflCoef = rhs.fNcdReflCoef;

  /*
  // Version 3 - 10-Nov-2000 - Add provisions for Rayleigh and Reflected light
  fRayleigh = rhs.fRayleigh;
  fReflect = rhs.fReflect;

  // Version 7 - 2003.09.22 - More MC info a la above
  fDirect = rhs.fDirect;
  fDirTotal = rhs.fDirTotal;
  fRayTotal = rhs.fRayTotal;
  fRefTotal = rhs.fRefTotal;
  */

  // Version 3 - 9-Nov-2000 - Add multiple occupancy time windows
  fNtimeW = rhs.fNtimeW;
  for (Int_t i=0; i<fNtimeW; i++) {
  		fOccupancyW[i] = rhs.fOccupancyW[i];
  		fOccratioW[i] = rhs.fOccratioW[i];
  }

  fCospmt = rhs.fCospmt;
  //fPmtang = rhs.fPmtang;
  fTranspwr = rhs.fTranspwr;

  fDd2o = rhs.fDd2o;
  fDh2o = rhs.fDh2o;
  fDacr = rhs.fDacr;
  fDpmt = rhs.fDpmt;

  // Version 2 - 1-Nov-2000 - Added "real" solid angle calculation variables
  fSolidangle = rhs.fSolidangle;
  fCospmtavg = rhs.fCospmtavg;
  fCosav = rhs.fCosav;

  fPMTrelvecX = rhs.fPMTrelvecX;
  fPMTrelvecY = rhs.fPMTrelvecY;
  fPMTrelvecZ = rhs.fPMTrelvecZ;

  //fLaserlight = rhs.fLaserlight;
  fLasertheta = rhs.fLasertheta;
  fLaserphi = rhs.fLaserphi;

  fPanel = rhs.fPanel;
  fBelly = rhs.fBelly;
  fNeck  = rhs.fNeck;
  fRope  = rhs.fRope;
  fPipe  = rhs.fPipe;
  fNcd   = rhs.fNcd;
  fAnchor   = rhs.fAnchor;
  fNcdRope   = rhs.fNcdRope;
  fOtherbad = rhs.fOtherbad;
  fBad = rhs.fBad;

  fCentralBad = rhs.fCentralBad;

  // Version 7 - 2003.09.22 - pmt fails basic requirements
  fBasicBad = rhs.fBasicBad;

  fOccratio = rhs.fOccratio;
  fOccratioerr = rhs.fOccratioerr;
  fGeomratio = rhs.fGeomratio;
  fGeomratioerr = rhs.fGeomratioerr;
  fModel = rhs.fModel;
  fModelerr = rhs.fModelerr;
  //fDd = rhs.fDd;
  //fDh = rhs.fDh;
  //fDa = rhs.fDa;

  //fLbmask = rhs.fLbmask;

  fChisq = rhs.fChisq;
  fResidual = rhs.fResidual;
  fSigma = rhs.fSigma;

  fFitstatus = rhs.fFitstatus;

  //************* DG added these for filling the timing resids and chi of fit

  fPoschi2 = rhs.fPoschi2;
  fPosres = rhs.fPosres;

  fRchtof = rhs.fRchtof;
  fCrunPmtIndex = rhs.fCrunPmtIndex;

  fCounterIndex = rhs.fCounterIndex;
  fNcdMinDist = rhs.fNcdMinDist;
}
//______________________________________________________________________________
QOCAPmt & QOCAPmt::operator=(const QOCAPmt &rhs)
{
  // QOCAPmt assignment operator
  // Copies all data members for the pmt

  fPmtn = rhs.fPmtn;
  fPmtx = rhs.fPmtx;
  fPmty = rhs.fPmty;
  fPmtz = rhs.fPmtz;

  //fPmteff = rhs.fPmteff;
  fPmteffm = rhs.fPmteffm;
  //fPmteffq = rhs.fPmteffq;
  fPmteffc = rhs.fPmteffc;
  //fThreshold = rhs.fThreshold;
  fTprompt = rhs.fTprompt;
  fTflight = rhs.fTflight;
  fTwidth = rhs.fTwidth;
  fNprompt = rhs.fNprompt;
  fOccupancy = rhs.fOccupancy;
  fOccupancyerr = rhs.fOccupancyerr;
  fOccCorrection = rhs.fOccCorrection;
  fNcdReflCoef = rhs.fNcdReflCoef;

  /*
  // Version 3 - 10-Nov-2000 - Add provisions for Rayleigh and Reflected light
  fRayleigh = rhs.fRayleigh;
  fReflect = rhs.fReflect;

  // Version 7 - 2003.09.22 - More MC info a la above
  fDirect = rhs.fDirect;
  fDirTotal = rhs.fDirTotal;
  fRayTotal = rhs.fRayTotal;
  fRefTotal = rhs.fRefTotal;
  */

  // Version 3 - 9-Nov-2000 - Add multiple occupancy time windows
  fNtimeW = rhs.fNtimeW;
  for (Int_t i=0; i<fNtimeW; i++) {
  		fOccupancyW[i] = rhs.fOccupancyW[i];
  		fOccratioW[i] = rhs.fOccratioW[i];
   }

  fCospmt = rhs.fCospmt;
  //fPmtang = rhs.fPmtang;
  fTranspwr = rhs.fTranspwr;

  fDd2o = rhs.fDd2o;
  fDh2o = rhs.fDh2o;
  fDacr = rhs.fDacr;
  fDpmt = rhs.fDpmt;

  // Version 2 - 1-Nov-2000 - Added "real" solid angle calculation variables
  fSolidangle = rhs.fSolidangle;
  fCospmtavg = rhs.fCospmtavg;
  fCosav = rhs.fCosav;

  fPMTrelvecX = rhs.fPMTrelvecX;
  fPMTrelvecY = rhs.fPMTrelvecY;
  fPMTrelvecZ = rhs.fPMTrelvecZ;

  //fLaserlight = rhs.fLaserlight;
  fLasertheta = rhs.fLasertheta;
  fLaserphi = rhs.fLaserphi;

  fPanel = rhs.fPanel;
  fBelly = rhs.fBelly;
  fNeck = rhs.fNeck;
  fRope = rhs.fRope;
  fPipe = rhs.fPipe;
  fNcd = rhs.fNcd;
  fAnchor = rhs.fAnchor;
  fNcdRope = rhs.fNcdRope;
  fOtherbad = rhs.fOtherbad;
  fBad = rhs.fBad;

  fCentralBad = rhs.fCentralBad;

  fBasicBad = rhs.fBasicBad;

  fOccratio = rhs.fOccratio;
  fOccratioerr = rhs.fOccratioerr;
  fGeomratio = rhs.fGeomratio;
  fGeomratioerr = rhs.fGeomratioerr;
  fModel = rhs.fModel;
  fModelerr = rhs.fModelerr;
  //fDd = rhs.fDd;
  //fDh = rhs.fDh;
  //fDa = rhs.fDa;

  //fLbmask = rhs.fLbmask;

  fChisq = rhs.fChisq;
  fResidual = rhs.fResidual;
  fSigma = rhs.fSigma;

  fFitstatus = rhs.fFitstatus;

  //******************  DG added these to add pos resid and chi2 to tree

  fPoschi2 = rhs.fPoschi2;
  fPosres = rhs.fPosres;

  fRchtof = rhs.fRchtof;
  fCrunPmtIndex = rhs.fCrunPmtIndex;

  fCounterIndex = rhs.fCounterIndex;
  fNcdMinDist = rhs.fNcdMinDist;

  return *this;
}
//______________________________________________________________________________
void QOCAPmt::Clear(Option_t *option)
{
  // Clear all values

   SetPmtn(0);
   SetPmtx(0);
   SetPmty(0);
   SetPmtz(0);

   //SetPmteff(0);
   SetPmteffm(0);
   //SetPmteffq(0);
   SetPmteffc(0);
   //SetThreshold(0);
   SetTprompt(0);
   SetTflight(0);
   SetTwidth(0);
   SetNprompt(0);
   SetOccupancy(0);
   SetOccupancyerr(0);
   SetOccCorrection(1);
   SetNcdReflCoef(0);

   /*
   SetDirect(0);
   SetRayleigh(0);
   SetReflect(0);
   SetDirTotal(0);
   SetRayTotal(0);
   SetRefTotal(0);
   */

   SetNtimeW(0);
   
   SetCospmt(0);
   //SetPmtang(0);
   SetTranspwr(0);

   SetDd2o(0);
   SetDh2o(0);
   SetDacr(0);
   SetDpmt(0);

   // Version 2 - 1-Nov-2000 - Added "real" solid angle calculation variables
   SetSolidangle(0);
   SetCospmtavg(0);
   SetCosav(0);

   SetPMTrelvecX(0);
   SetPMTrelvecY(0);
   SetPMTrelvecZ(0);

   //SetLaserlight(0);
   SetLasertheta(0);
   SetLaserphi(0);

   SetPanel(0);
   SetBelly(0);
   SetNeck(0);
   SetRope(0);
   SetPipe(0);
   SetNcd(0);
   SetOtherbad(0);
   SetBad(0);

   SetCentralBad(0);
   SetBasicBad(kFALSE);

   SetOccratio(0);
   SetOccratioerr(0);
   SetGeomratio(0);
   SetGeomratioerr(0);
   SetModel(0);
   SetModelerr(0);
   //SetDd(0);
   //SetDh(0);
   //SetDa(0);

   //SetLbmask(0);

   SetChisq(0);
   SetResidual(0);
   SetSigma(0);

   SetFitstatus(0);

   //*******************DG added these 

   SetPoschi2(0);
   SetPosres(0);

   SetRchtof(0);
   SetCrunPmtIndex(0);

   SetNcdMinDist(999);
   SetCounterIndex(-1);
}
//______________________________________________________________________________
void QOCAPmt::SetNtimeW(Int_t n)
{
  // Set (or reset) the number of time integration windows.
  // If the windows were allocated to this size, keep them.
  // Otherwise, reallocate as necessary.
  // This is a once-only operation - you can't add to the size of the array here!
  // Negative or zero argument indicates no time windows needed.

  if (n<=0) fNtimeW = 0;
//  else if (n>=12) fNtimeW = 12;
  else if (n>=7) fNtimeW = 7;
  else fNtimeW = n;

  return;
}
//______________________________________________________________________________
void QOCAPmt::SetOccupancyW(Int_t n,Float_t occ)
{
  // Set the occupancy if 0<=n<fNtimeW.

  if (n<0 || n>fNtimeW) return;
  fOccupancyW[n] = occ;
}
//______________________________________________________________________________
void QOCAPmt::SetOccupancyW(Float_t *occ)
{
  // Set the occupancy array to the passed array.
  // This is a copy operation: the elements are copied and the external pointer
  // can be reused.

  if (!occ) return;

  Int_t i;
  for (i=0; i<fNtimeW; i++) fOccupancyW[i] = occ[i];
}
//______________________________________________________________________________
void QOCAPmt::SetOccratioW(Int_t n,Float_t occ)
{
  // Set the occratio if 0<=n<fNtimeW.

  if (n<0 || n>fNtimeW) return;
  fOccratioW[n] = occ;
}
//______________________________________________________________________________
void QOCAPmt::SetOccratioW(Float_t *occ)
{
  // Set the occratio array to the passed array.
  // This is a copy operation: the elements are copied and the external pointer
  // can be reused.

  if (!occ) return;

  Int_t i;
  for (i=0; i<fNtimeW; i++) fOccratioW[i] = occ[i];
}
//______________________________________________________________________________
Float_t QOCAPmt::GetOccupancyW(Int_t n)
{
  // Get the occupancy entry n for 0<=n<fNtimeW.
  // Return -1 if out of range or no occ wide data.

  if (n<0 || n>fNtimeW) return -1.0;
  return fOccupancyW[n];
}
//______________________________________________________________________________
Float_t *QOCAPmt::GetOccupancyW()
{
  // Return the pointer fOccupancyW.

  return fOccupancyW;
}
//______________________________________________________________________________
Float_t QOCAPmt::GetOccratioW(Int_t n)
{
  // Get the occratio entry n for 0<=n<fNtimeW.
  // Return -1 if out of range or no occ wide data.

  if (n<0 || n>fNtimeW) return -1.0;
  return fOccratioW[n];
}
//______________________________________________________________________________
Float_t *QOCAPmt::GetOccratioW()
{
  // Return the pointer fOccratioW.

  return fOccratioW;
}
//______________________________________________________________________________
void QOCAPmt::Streamer(TBuffer &R__b)
{
  // Stream an object of class QOCAPmt.
  //
  // Updated 29-Jun-2001 for ROOT 3.00 "Automatic Schema Evolution" - BA Moffat
  
  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if (R__v > 4) {
      QOCAPmt::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      return;
    }
    //====process old versions before automatic schema evolution
    TObject::Streamer(R__b);
    R__b >> fPmtn;
    R__b >> fPmtx;
    R__b >> fPmty;
    R__b >> fPmtz;
    //R__b >> fPmteff;
    R__b >> fPmteffm;
    //R__b >> fPmteffq;
    R__b >> fPmteffc;
    //R__b >> fThreshold;
    R__b >> fTprompt;
    R__b >> fTflight;
    R__b >> fTwidth;
    R__b >> fNprompt;
    
    /*
    if (R__v >= 7) {  // Add MC info
      R__b >> fDirect;
      R__b >> fRayleigh; // I wonder if I should put this here...
      R__b >> fReflect;  // I wonder if I should put this here...
      R__b >> fDirTotal;
      R__b >> fRayTotal;
      R__b >> fRefTotal;
    }
    */

    R__b >> fOccupancy;
    R__b >> fOccupancyerr;
    
    if (R__v == 3) {     // Version 3: prompt window width variations (1 to 25ns ?)
      fNtimeW = R__b.ReadStaticArray(fOccupancyW);
      //R__b >> fRayleigh;  // Version 3: Rayleigh and reflected light
      //R__b >> fReflect;
    } else if (R__v <= 6) {
      fNtimeW = 0;
      //fRayleigh = 0;
      //fReflect = 0;
    } else {
      fNtimeW = 0;
    }
    
    R__b >> fCospmt;
    //R__b >> fPmtang;
    R__b >> fTranspwr;
    R__b >> fDd2o;
    R__b >> fDh2o;
    R__b >> fDacr;
    R__b >> fDpmt;
    //R__b >> fLaserlight;
    R__b >> fLasertheta;
    R__b >> fLaserphi;
    R__b >> fPanel;
    R__b >> fBelly;
    R__b >> fNeck;
    R__b >> fRope;
    R__b >> fPipe;
    R__b >> fNcd;
    R__b >> fAnchor;
    R__b >> fNcdRope;
    R__b >> fOtherbad;
    R__b >> fBad;
    R__b >> fCentralBad;
    R__b >> fBasicBad;
    R__b >> fOccratio;
    R__b >> fOccratioerr;
    R__b >> fGeomratio;
    R__b >> fGeomratioerr;
    R__b >> fModel;
    R__b >> fModelerr;
    //R__b >> fDd;
    //R__b >> fDh;
    //R__b >> fDa;
    //R__b >> fLbmask;
    R__b >> fChisq;
    R__b >> fResidual;
    R__b >> fSigma;
    
    if (R__v >= 4) {  // Add QOCAFit status word
      R__b >> fFitstatus;
    }
    
    if (R__v >= 6) {  // Add .rch time-of-flight info
      R__b >> fRchtof;
    }

    if (R__v >= 8) {  // Add centre run pmt index
      R__b >> fCrunPmtIndex;
    }

    if (R__v >= 11) {  // Add NCD closets counter and distance
                       // and individual position fit chi^2
      R__b >> fCounterIndex;
      R__b >> fNcdMinDist;
    }
    
    if (R__v >= 12) {  // Add light vector leaving the source components
      R__b >> fPMTrelvecX;
      R__b >> fPMTrelvecY;
      R__b >> fPMTrelvecZ;
    }

    if (R__v >= 13) {  // Add any kind of occupancy correction
      R__b >> fOccCorrection;
    }

    if (R__v >= 14) {  // Add Ncd reflection coefficient
      R__b >> fNcdReflCoef;
    }

    Float_t dummy;
    if (R__v == 3) {          // Take out redundant fSinalpha, fSinbeta
      R__b >> fSolidangle;
      R__b >> fCospmtavg;
      R__b >> fCosav;
    } else if (R__v == 2) {   // New in version 2 of QOCAPmt: solid angle calc'n
      R__b >> fSolidangle;
      R__b >> fCospmtavg;
      R__b >> dummy;  // used to be fSinalpha
      R__b >> dummy;  // used to be fSinbeta
      R__b >> fCosav;

      //******************DG added these here

      R__b >> fPoschi2;
      R__b >> fPosres;

    } else {
      fSolidangle = 1.0;
      fCospmtavg = fCospmt;
      fCosav = 1.0;
    }
    R__b.CheckByteCount(R__s, R__c, QOCAPmt::IsA());
    //====end of old versions
  } else {
    QOCAPmt::Class()->WriteBuffer(R__b,this);
  }
}
//________________________________________________________________________


