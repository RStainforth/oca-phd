#ifndef _OCA_RUN_H
#define _OCA_RUN_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCARun                                                              //
//                                                                      //
// Run level data structure for use in QOCATree.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay
//*-- Copyright (c) 1999 SNO Software Project, All rights reserved
//*-- Author : Bryce Moffat - updated 24-Apr-2000 for inclusion in libqoca
//*-- Copyright (c) 2000 CodeSNO. All rights reserved.

#include <TVector3.h>
#include <TClonesArray.h>
#include "QOCAPmt.h"

// ------------------------------------------------------------------------
//  OCARun class definition
// ------------------------------------------------------------------------

class QOCARun : public TObject
{
 private:
  Int_t      fRun;    // Run number
  Int_t      fNpmt;   // Number of PMTs stored for this run
  Float_t    fLambda; // Wavelength of laser

  Float_t    fNorm;   // Run laser intensity (normalization)
  Float_t    fLaserx;  // Coordinates used in OCA fits
  Float_t    fLasery;
  Float_t    fLaserz;
  Float_t    fLasert;
  Float_t    fLaserchi2;
  Float_t    fFitx;  // Coordinates obtained by fitting the laserball position (direct)
  Float_t    fFity;
  Float_t    fFitz;
  Float_t    fFitt;
  Float_t    fFitchi2;
  Float_t    fQx;  // Coordinates obtained by fitting the laserball position (QOptics)
  Float_t    fQy;
  Float_t    fQz;
  Float_t    fQt;
  Float_t    fQchi2;
  Float_t    fManipx;  // Coordinates from the manipulator
  Float_t    fManipy;
  Float_t    fManipz;
  Float_t    fManipt;
  Float_t    fManipchi2;

  Float_t    fBalltheta;// Orientation of laserball (usually 0)
  Float_t    fBallphi;// Orientation (0=south, pi/2=west, pi=north, 3*pi/2 = east)

  Int_t      fNpulses; // Number of laser pulses over the run

  // -- os 24.02.2005
  // the following variables were removed since not used
  // Float_t    fRate;    // Laser pulse rate (typically 10 to 48 Hz)
  // Float_t    fNd1; // Neutral density filter settings (0,1,2,3,4,5,6)
  // Float_t    fNd2; // Neutral density filter settings (0.01,0.03,0.2,0.4,0.5,0.6)
  // Int_t      fJdystart;  // Julian date, start of run - from ZDAB/SNOMAN
  // Int_t      fJdystop;
  // Int_t      fUt1start;  // Universal time, start of run - from ZDAB/SNOMAN
  // Int_t      fUt1stop;

  Int_t      fRuncentre; // Central run for occupancy ratio calculations

  // -- os 24.02.2005
  Float_t    fCosThetapsMin; // cosinus of minimum angle between source and pmts
  Float_t    fCosThetapsMax; // cosinus of maximum angle between source and pmts

  Float_t    fNcdTolerance;  // minimum distance between ray and ncd allowed

  Float_t    fQxerr;  // Fit error on coordinates obtained by ...
  Float_t    fQyerr;  // ... fitting the laserball position (QOptics)
  Float_t    fQzerr;

  Float_t    fTimeSigmaMean;  // Average PMT time spread for this run
  Float_t    fTimeSigmaSigma; // Standard deviation of PMT time spread for this run

  TClonesArray *fPmts;  // List of OCAPmt's for this run

 public:
  // Constructor, destructor
  QOCARun();
  ~QOCARun();

  QOCARun(const QOCARun &rhs);
  QOCARun &operator=(const QOCARun &rhs);

  // Setters  
  void SetRun(Int_t run) {fRun = run;}
  void SetNpmt(Int_t n) {fNpmt = n;}
  void SetLambda(Float_t lambda) {fLambda = lambda;}

  void SetNorm(Float_t n) {fNorm = n;}

  void SetLaserpos(Int_t postype = 0);  // pseudo-setter: copies manip or fit into laser
  Float_t GetR() { return sqrt(fLaserx*fLaserx + fLasery*fLasery + fLaserz*fLaserz); }

  void SetLaserxyz(TVector3 p) {fLaserx = p(0);  fLasery = p(1);  fLaserz = p(2);}
  void SetLaserxyz(Float_t x, Float_t y, Float_t z) {
    fLaserx = x;  fLasery = y;  fLaserz = z;}
  void SetLaserx(Float_t x) {fLaserx = x;}
  void SetLasery(Float_t y) {fLasery = y;}
  void SetLaserz(Float_t z) {fLaserz = z;}
  void SetLasert(Float_t t) {fLasert = t;}
  void SetLaserchi2(Float_t c) {fLaserchi2 = c;}
  void SetLx(Float_t x) {fLaserx = x;}
  void SetLy(Float_t y) {fLasery = y;}
  void SetLz(Float_t z) {fLaserz = z;}
  void SetLt(Float_t t) {fLasert = t;}
  void SetLchi2(Float_t c) {fLaserchi2 = c;}

  void SetFitxyz(TVector3 p) {fFitx = p(0);  fFity = p(1);  fFitz = p(2);}
  void SetFitxyz(Float_t x, Float_t y, Float_t z) {
    fFitx = x;  fFity = y;  fFitz = z;}
  void SetFitx(Float_t x) {fFitx = x;}
  void SetFity(Float_t y) {fFity = y;}
  void SetFitz(Float_t z) {fFitz = z;}
  void SetFitt(Float_t t) {fFitt = t;}
  void SetFitchi2(Float_t c) {fFitchi2 = c;}
  void SetFx(Float_t x) {fFitx = x;}
  void SetFy(Float_t y) {fFity = y;}
  void SetFz(Float_t z) {fFitz = z;}
  void SetFt(Float_t t) {fFitt = t;}
  void SetFchi2(Float_t c) {fFitchi2 = c;}

  void SetQFitxyz(TVector3 p) {fQx = p(0);  fQy = p(1);  fQz = p(2);}
  void SetQFitxyz(Float_t x, Float_t y, Float_t z) {
    fQx = x;  fQy = y;  fQz = z;}
  void SetQFitx(Float_t x) {fQx = x;}
  void SetQFity(Float_t y) {fQy = y;}
  void SetQFitz(Float_t z) {fQz = z;}
  void SetQFitt(Float_t t) {fQt = t;}
  void SetQFitchi2(Float_t c) {fQchi2 = c;}
  void SetQx(Float_t x) {fQx = x;}
  void SetQy(Float_t y) {fQy = y;}
  void SetQz(Float_t z) {fQz = z;}
  void SetQt(Float_t t) {fQt = t;}
  void SetQchi2(Float_t c) {fQchi2 = c;}

  void SetManipxyz(TVector3 p) {fManipx = p(0);  fManipy = p(1);  fManipz = p(2);}
  void SetManipxyz(Float_t x, Float_t y, Float_t z) {
    fManipx = x;  fManipy = y;  fManipz = z;}
  void SetManipx(Float_t x) {fManipx = x;}
  void SetManipy(Float_t y) {fManipy = y;}
  void SetManipz(Float_t z) {fManipz = z;}
  void SetManipt(Float_t t) {fManipt = t;}
  void SetManipchi2(Float_t c) {fManipchi2 = c;}
  void SetMx(Float_t x) {fManipx = x;}
  void SetMy(Float_t y) {fManipy = y;}
  void SetMz(Float_t z) {fManipz = z;}
  void SetMt(Float_t t) {fManipt = t;}
  void SetMchi2(Float_t c) {fManipchi2 = c;}

  void SetBalltheta(Float_t t) {fBalltheta = t;}
  void SetBallphi(Float_t p) {fBallphi = p;}

  void SetNpulses(Int_t np) {fNpulses = np;}

  // -- os 24.02.2005
  // the following functions were removed since not used
  // void SetRate(Float_t rate) {fRate = rate;}
  // void SetNd1(Float_t nd) {fNd1 = nd;}
  // void SetNd2(Float_t nd) {fNd2 = nd;}
  // void SetJdystart(Int_t jdy) {fJdystart = jdy;}
  // void SetJdystop(Int_t jdy) {fJdystop = jdy;}
  // void SetUt1start(Int_t ut1) {fUt1start = ut1;}
  // void SetUt1stop(Int_t ut1) {fUt1stop = ut1;}

  void SetRuncentre(Int_t crun) {fRuncentre = crun;}

  // -- os 24.02.2005
  void SetCosThetapsMin(Float_t cthpsmin = 999.){fCosThetapsMin = cthpsmin;}
  void SetCosThetapsMax(Float_t cthpsmax = -999.){fCosThetapsMax = cthpsmax;}

  void SetNcdTolerance(Float_t ncdtol = 0.){fNcdTolerance = ncdtol;}

  void SetQFitxerr(Float_t xerr) {fQxerr = xerr;}
  void SetQFityerr(Float_t yerr) {fQyerr = yerr;}
  void SetQFitzerr(Float_t zerr) {fQzerr = zerr;}
  void SetQFitxyzerr(Float_t xerr, Float_t yerr, Float_t zerr) {
    fQxerr = xerr;  fQyerr = yerr;  fQzerr = zerr;}

  void SetTwidthMean(Float_t twmean){fTimeSigmaMean = twmean;}
  void SetTwidthSigma(Float_t twsigma){fTimeSigmaSigma = twsigma;}

  // Getters
  Int_t GetRun() {return fRun;}
  Int_t GetNpmt() {return fNpmt;}
  Float_t GetLambda() {return fLambda;}

  Float_t GetNorm() {return fNorm;}

  TVector3 GetLaserxyz();
  Float_t GetLaserx() {return fLaserx;}
  Float_t GetLasery() {return fLasery;}
  Float_t GetLaserz() {return fLaserz;}
  Float_t GetLaserchi2() {return fLaserchi2;}
  Float_t GetLx() {return fLaserx;}
  Float_t GetLy() {return fLasery;}
  Float_t GetLz() {return fLaserz;}
  Float_t GetLchi2() {return fLaserchi2;}

  TVector3 GetFitxyz();
  Float_t GetFitx() {return fFitx;}
  Float_t GetFity() {return fFity;}
  Float_t GetFitz() {return fFitz;}
  Float_t GetFitchi2() {return fFitchi2;}
  Float_t GetFx() {return fFitx;}
  Float_t GetFy() {return fFity;}
  Float_t GetFz() {return fFitz;}
  Float_t GetFchi2() {return fFitchi2;}

  TVector3 GetQFitxyz();
  Float_t GetQFitx() {return fQx;}
  Float_t GetQFity() {return fQy;}
  Float_t GetQFitz() {return fQz;}
  Float_t GetQFitchi2() {return fQchi2;}
  Float_t GetQx() {return fQx;}
  Float_t GetQy() {return fQy;}
  Float_t GetQz() {return fQz;}
  Float_t GetQt() {return fQt;}
  Float_t GetQchi2() {return fQchi2;}

  TVector3 GetManipxyz();
  Float_t GetManipx() {return fManipx;}
  Float_t GetManipy() {return fManipy;}
  Float_t GetManipz() {return fManipz;}
  Float_t GetManipchi2() {return fManipchi2;}
  Float_t GetMx() {return fManipx;}
  Float_t GetMy() {return fManipy;}
  Float_t GetMz() {return fManipz;}
  Float_t GetMchi2() {return fManipchi2;}

  Float_t GetBalltheta() {return fBalltheta;}
  Float_t GetBallphi() {return fBallphi;}

  Int_t GetNpulses() {return fNpulses;}

  // -- os 24.02.2005
  // the following functions were removed since not used
  // Float_t GetRate() {return fRate;}
  // Float_t GetNd1() {return fNd1;}
  // Float_t GetNd2() {return fNd2;}
  // Int_t GetJdystart() {return fJdystart;}
  // Int_t GetJdystop() {return fJdystop;}
  // Int_t GetUt1start() {return fUt1start;}
  // Int_t GetUt1stop() {return fUt1stop;}

  Int_t GetRuncentre() {return fRuncentre;}

  // -- os 24.02.2005
  Float_t GetCosThetapsMin(){return fCosThetapsMin;}
  Float_t GetCosThetapsMax(){return fCosThetapsMax;}

  Float_t GetNcdTolerance(){return fNcdTolerance;}

  TVector3 GetQFitxyzerr();
  Float_t GetQFitxerr() {return fQxerr;}
  Float_t GetQFityerr() {return fQyerr;}
  Float_t GetQFitzerr() {return fQzerr;}

  Float_t GetTwidthMean() {return fTimeSigmaMean;}
  Float_t GetTwidthSigma() {return fTimeSigmaSigma;}

  // Methods
  void CopyPMTInfo(QOCARun *ocarun);
  void AddPMT(QOCAPmt *pmt);
  QOCAPmt *GetPMT(Int_t i);
  void Clear(Option_t *option = "");
  void Initialize();

  ClassDef(QOCARun,6) // Single OCA run
};

#endif
