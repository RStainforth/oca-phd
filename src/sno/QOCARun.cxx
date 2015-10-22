//*--Author : Mark Boulay
//*--Author : Bryce Moffat - updated 24-Apr-2000 for libqoca from libqtree
//*--Copyright (c) 1999 SNO Software Project

////////////////////////////////////////////////////////////////////////////////
// QOCARun                                                                    //
// Data structure for storing pertinent information from a single OCA optical //
// calibration run, along with member functions for manipulting these data.   //
////////////////////////////////////////////////////////////////////////////////


#include "QOCARun.h"

ClassImp(QOCARun)
;
//______________________________________________________________________________
//
// QOCARun class.
//
// Data structure for storing pertinent information from a single OCA optical
// calibration run, along with member functions for manipulting these data.
//
//______________________________________________________________________________
QOCARun::QOCARun()
{
  // Default constructor.
  
  fPmts = NULL;
  fNpmt = 0;
  Initialize();
}
//______________________________________________________________________________
QOCARun::~QOCARun()
{
  // Default destructor.

  delete fPmts;
  fPmts = NULL;
}
//______________________________________________________________________________
QOCARun::QOCARun(const QOCARun &rhs)
{
  // Copy constructor for QOCARun objects
  // Does not copy the array of PMT's

  fRun = rhs.fRun;
  fNpmt = 0;  // No pmt's yet in this run (so, don't use rhs.fNpmt...)
  fLambda = rhs.fLambda;

  fNorm = rhs.fNorm;
  fLaserx = rhs.fLaserx;
  fLasery = rhs.fLasery;
  fLaserz = rhs.fLaserz;
  fLasert = rhs.fLasert;
  fLaserchi2 = rhs.fLaserchi2;
  fFitx = rhs.fFitx;
  fFity = rhs.fFity;
  fFitz = rhs.fFitz;
  fFitt = rhs.fFitt;
  fFitchi2 = rhs.fFitchi2;
  fQx = rhs.fQx;
  fQy = rhs.fQy;
  fQz = rhs.fQz;
  fQt = rhs.fQt;
  fQchi2 = rhs.fQchi2;
  fManipx = rhs.fManipx;
  fManipy = rhs.fManipy;
  fManipz = rhs.fManipz;
  fManipt = rhs.fManipt;
  fManipchi2 = rhs.fManipchi2;

  fBalltheta = rhs.fBalltheta;
  fBallphi = rhs.fBallphi;

  fNpulses = rhs.fNpulses;

  // -- os 24.02.2005
  // the following variables were removed since not used
  // fRate = rhs.fRate;
  // fNd1 = rhs.fNd1;
  // fNd2 = rhs.fNd2;
  // fJdystart = rhs.fJdystart;
  // fJdystop = rhs.fJdystop;
  // fUt1start = rhs.fUt1start;
  // fUt1stop = rhs.fUt1stop;

  fRuncentre = rhs.fRuncentre;

  fCosThetapsMin = rhs.fCosThetapsMin;
  fCosThetapsMax = rhs.fCosThetapsMax;

  fNcdTolerance = rhs.fNcdTolerance;

  fQxerr = rhs.fQxerr;
  fQyerr = rhs.fQyerr;
  fQzerr = rhs.fQzerr;

  fTimeSigmaMean = rhs.fTimeSigmaMean;
  fTimeSigmaSigma = rhs.fTimeSigmaSigma;
}
//______________________________________________________________________________
QOCARun & QOCARun::operator=(const QOCARun &rhs)
{
  // QOCARun assignment operator
  // Does not copy the array of PMT's

  fRun = rhs.fRun;
  fNpmt = 0;  // No pmt's yet in this run (so, don't use rhs.fNpmt...)
  fLambda = rhs.fLambda;

  fNorm = rhs.fNorm;
  fLaserx = rhs.fLaserx;
  fLasery = rhs.fLasery;
  fLaserz = rhs.fLaserz;
  fLasert = rhs.fLasert;
  fLaserchi2 = rhs.fLaserchi2;
  fFitx = rhs.fFitx;
  fFity = rhs.fFity;
  fFitz = rhs.fFitz;
  fFitt = rhs.fFitt;
  fFitchi2 = rhs.fFitchi2;
  fQx = rhs.fQx;
  fQy = rhs.fQy;
  fQz = rhs.fQz;
  fQt = rhs.fQt;
  fQchi2 = rhs.fQchi2;
  fManipx = rhs.fManipx;
  fManipy = rhs.fManipy;
  fManipz = rhs.fManipz;
  fManipt = rhs.fManipt;
  fManipchi2 = rhs.fManipchi2;

  fBalltheta = rhs.fBalltheta;
  fBallphi = rhs.fBallphi;

  fNpulses = rhs.fNpulses;

  // -- os 24.02.2005
  // the following variables were removed since not used
  // fRate = rhs.fRate;
  // fNd1 = rhs.fNd1;
  // fNd2 = rhs.fNd2;
  // fJdystart = rhs.fJdystart;
  // fJdystop = rhs.fJdystop;
  // fUt1start = rhs.fUt1start;
  // fUt1stop = rhs.fUt1stop;

  fRuncentre = rhs.fRuncentre;

  fCosThetapsMin = rhs.fCosThetapsMin;
  fCosThetapsMax = rhs.fCosThetapsMax;

  fNcdTolerance = rhs.fNcdTolerance;

  fQxerr = rhs.fQxerr;
  fQyerr = rhs.fQyerr;
  fQzerr = rhs.fQzerr;

  fTimeSigmaMean = rhs.fTimeSigmaMean;
  fTimeSigmaSigma = rhs.fTimeSigmaSigma;

  if (fPmts) fPmts->Clear();

  return *this;
}
//______________________________________________________________________________
void QOCARun::Initialize()
{
  // Initialize the TClonesArrays
  // Anything created with operator new here must be deleted in destructor.
  
  if (!fPmts) fPmts = new TClonesArray("QOCAPmt",10000);
}
//______________________________________________________________________________
void QOCARun::CopyPMTInfo(QOCARun *ocarun)
{
  // Copies all the PMT-level information from one run to this one.
  //
  // Use this after the copy/assignment operator to obtain a complete copy
  // of all info in the QOCARun (not just the run info in the header.)

  Int_t i;
  for (i=0; i<ocarun->GetNpmt(); i++) AddPMT(ocarun->GetPMT(i));
}
//______________________________________________________________________________
void QOCARun::AddPMT(QOCAPmt *pmt)
{
  // Add this OCAPmt to the list.

  TClonesArray &pmtbundles = *fPmts;
  *(new (pmtbundles[fNpmt]) QOCAPmt() ) = *pmt;
  fNpmt++;
}
//______________________________________________________________________________
QOCAPmt *QOCARun::GetPMT(Int_t i)
{
  // Return the i'th QOCAPmt for this run.
  
  QOCAPmt *pmt = NULL;
  if (i < fNpmt && i >= 0) pmt = (QOCAPmt *) fPmts->At(i);
  return pmt;
}
//______________________________________________________________________________
void QOCARun::Clear(Option_t *option)
{
  // Clear/reset this OCARun.

  SetRun(0);
  SetNpmt(0);
  SetLambda(0);

  SetNorm(0);
  SetLaserxyz(-999,-999,-999);
  SetLasert(-999);
  SetLaserchi2(0);
  SetFitxyz(-999,-999,-999);
  SetFitt(-999);
  SetFitchi2(0);
  SetQFitxyz(-999,-999,-999);
  SetQFitt(-999);
  SetQFitchi2(0);
  SetManipxyz(-999,-999,-999);
  SetManipt(-999);
  SetManipchi2(0);

  SetBalltheta(0);
  SetBallphi(0);

  SetNpulses(0);

  // -- os 24.02.2005
  // the following functions were removed since not used
  // SetRate(0);
  // SetNd1(0);
  // SetNd2(0);
  // SetJdystart(0);
  // SetJdystop(0);
  // SetUt1start(0);
  // SetUt1stop(0);

  SetRuncentre(0);

  SetCosThetapsMin();
  SetCosThetapsMax();

  SetNcdTolerance(0);

  SetQFitxyzerr(-999,-999,-999);

  SetTwidthMean(-999);
  SetTwidthSigma(-999);

  if (fPmts) fPmts->Clear(option);
}
//______________________________________________________________________________
void QOCARun::SetLaserpos(Int_t postype)
{
  // Set the laser position to be used in OCA fits to either:
  //  postype == 0: manipulator position
  //             1: fit position - direct line fit
  //             2: fit position - Qoptics fit
  //  Any other value results in no change to LaserX,Y,Z.

  switch (postype) {
  case 0:
    SetLaserxyz(fManipx,fManipy,fManipz);
    break;
  case 1:
    SetLaserxyz(fFitx,fFity,fFitz);
    break;
  case 2:
    SetLaserxyz(fQx,fQy,fQz);
    break;
  default:
    break;  
  }
}
//______________________________________________________________________________
TVector3 QOCARun::GetLaserxyz()
{
  // Returns the laserball coordinates as a TVector3.

  return TVector3(fLaserx,fLasery,fLaserz);
}
//______________________________________________________________________________
TVector3 QOCARun::GetFitxyz()
{
  // Returns the direct line fit coordinates of laserball for this run.

  return TVector3(fFitx,fFity,fFitz);
}
//______________________________________________________________________________
TVector3 QOCARun::GetQFitxyz()
{
  // Returns the QOptics path fit coordinates of laserball for this run.

  TVector3 p(fQx,fQy,fQz);
  return p;
}
//______________________________________________________________________________
TVector3 QOCARun::GetQFitxyzerr()
{
  // Returns the fit error of the 
  // QOptics path fit coordinates of laserball for this run.

  TVector3 e(fQxerr,fQyerr,fQzerr);
  return e;
}
//______________________________________________________________________________
TVector3 QOCARun::GetManipxyz()
{
  // Returns the manipulator coordinates of laserball for this run.

  TVector3 p(fManipx,fManipy,fManipz);
  return p;
}
//______________________________________________________________________________
void QOCARun::Streamer(TBuffer &R__b)
{
  // Stream an object of class QOCARun.
  //
  // Updated 29-Jun-2001 for ROOT 3.00 "Automatic Schema Evolution" - BA Moffat

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if (R__v > 2) {
      QOCARun::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      return;
    }
    //====process old versions before automatic schema evolution
    TObject::Streamer(R__b);
    R__b >> fRun;
    R__b >> fNpmt;
    R__b >> fLambda;
    R__b >> fNorm;
    R__b >> fLaserx;
    R__b >> fLasery;
    R__b >> fLaserz;
    if (R__v >= 2) {
      R__b >> fLasert;
      R__b >> fLaserchi2;
    } else {
      fLasert = -999;
      fLaserchi2 = 0.0;
    }
    R__b >> fFitx;
    R__b >> fFity;
    R__b >> fFitz;
    if (R__v >= 2) {
      R__b >> fFitt;
      R__b >> fFitchi2;
    } else {
      fFitt = -999;
      fFitchi2 = 0.0;
    }
    if (R__v >= 2) {  // Version 2: store the QOptics path fit _and_ direct path fit
      R__b >> fQx;
      R__b >> fQy;
      R__b >> fQz;
      R__b >> fQt;
      R__b >> fQchi2;
    } else {
      fQx = -999;
      fQy = -999;
      fQz = -999;
      fQt = -999;
      fQchi2 = 0.0;
    }
    R__b >> fManipx;
    R__b >> fManipy;
    R__b >> fManipz;
    if (R__v >= 2) {
      R__b >> fManipt;
      R__b >> fManipchi2;
    } else {
      fManipt = -999;
      fManipchi2 = 0.0;
    }
    R__b >> fBalltheta;
    R__b >> fBallphi;
    R__b >> fNpulses;
    // -- os 24.02.2005
    // the following variables were removed since not used
    // R__b >> fRate;
    // R__b >> fNd1;
    // R__b >> fNd2;
    // R__b >> fJdystart;
    // R__b >> fJdystop;
    // R__b >> fUt1start;
    // R__b >> fUt1stop;

    R__b >> fRuncentre;

    if (R__v >= 4) {  // Version 4: store NCD tolerance and AV reflection cut
      R__b >> fCosThetapsMin;
      R__b >> fCosThetapsMax;
      R__b >> fNcdTolerance;
    } 

    fPmts->Streamer(R__b);
    R__b.CheckByteCount(R__s, R__c, QOCARun::IsA());
    //====end of old versions
  } else {
    QOCARun::Class()->WriteBuffer(R__b,this);
  }
}
