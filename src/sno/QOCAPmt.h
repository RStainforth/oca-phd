#ifndef _OCA_PMT_H
#define _OCA_PMT_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCAPmt                                                              //
//                                                                      //
// PMT data structure for use in QOCARun and QOCATree.                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay
//*-- Copyright (c) 1999 SNO Software Project, All rights reserved
//*-- Author : Bryce Moffat - updated 24-Apr-2000 for inclusion in libqoca
//*-- Author : Bryce Moffat - updated 6-Mar-2001 for QOCAFit status flags
//*-- Copyright (c) 2000-2001 CodeSNO. All rights reserved.

#include <TVector.h>
#include <TObject.h>
#include <QSNO.h>
#include <QPMTxyz.h>

// ------------------------------------------------------------------------
//  OCAPmt class definition
// ------------------------------------------------------------------------

class QOCAPmt : public TObject {

 private:
  Int_t   fPmtn;      // Logical channel number of pmt
  Float_t fPmtx;      // PMT X-coordinate (stored for speed when drawing QOCATree)
  Float_t fPmty;      // PMT Y-coordinate
  Float_t fPmtz;      // PMT Z-coordinate

  //Float_t fPmteff;    // Efficiency from central run (occupancy ratio method)
  Float_t fPmteffm;   // Efficiency from model fits
  //Float_t fPmteffq;   // Efficiency from Queen's measurements
  Float_t fPmteffc;   // Efficiency from previous calibration
  //Float_t fThreshold; // Channel threshold (arbitrary units?)
  Float_t fTprompt;   // Time of prompt peak (time of flight corrected to 0)
  Float_t fTflight;   // Time of flight used in making the .rch file (c=22.45cm/ns?)
  Float_t fTwidth;    // Width of prompt peak
  Float_t fNprompt;   // Number of hits in prompt timing window (old "raw2")
  Float_t fOccupancy; // MPE-corrected occupancy of this PMT
  Float_t fOccupancyerr; // MPE-corrected occupancy error for this PMT
  Float_t fOccCorrection; // Ratio of corrected occupancy to the input occupancy
  Float_t fNcdReflCoef;  // Ncd reflection effect before reflectivity is applied

  /*
  // Variables useful for MC
  Float_t fDirect;    // Impact measure of Direct light (prompt)
  Float_t fRayleigh;  // Impact measure of Rayleigh scattered light (prompt)
  Float_t fReflect;   // Impact measure of Reflected light (prompt)
  Float_t fDirTotal;  // Impact measure of Direct light (total)
  Float_t fRayTotal;  // Impact measure of Rayleigh scattered light (total)
  Float_t fRefTotal;  // Impact measure of Reflected light (total)
  */

  Int_t fNtimeW;           // Number of time windows
  Float_t fOccupancyW[7];  // Array of occupancies for different time widths
  Float_t fOccratioW[7];   // Array of occratios for different time widths

  Float_t fCospmt;    // Cos(theta_pmt): incident light dot product PMT normal
  //Float_t fPmtang;    // Angular response of the PMT at cospmt incident angle
  Float_t fTranspwr;  // Fresnel transmission for acrylic vessel (both interfaces)

  Float_t fDd2o;      // Distance through d2o from source to PMT
  Float_t fDh2o;      // Distance through h2o from source to PMT
  Float_t fDacr;      // Distance through acrylic from source to PMT
  Float_t fDpmt;      // Total distance from source to PMT (sum of Dxxx above)

  Float_t fSolidangle;// Solid angle of PMT as seen from the source (incl. refraction)
  Float_t fCospmtavg; // Average cos(theta_PMT) for four offset points on PMT
  Float_t fCosav;     // Cos(theta_AV) for path through AV to centre of PMT
  
  Float_t fPMTrelvecX;// X-component of light vector direction leaving the source towards PMT
  Float_t fPMTrelvecY;// Y-component of light vector direction leaving the source towards PMT
  Float_t fPMTrelvecZ;// Z-component of light vector direction leaving the source towards PMT

  //Float_t fLaserlight;// Relative Laser intensity for this PMT
  Float_t fLasertheta;// Laserball theta for this PMT from this source position
  Float_t fLaserphi;  // Laserball phi for this PMT from this source position
  Float_t fPoschi2;
  Float_t fPosres;


  // Pmts that lie in a bad optical path
  Int_t   fPanel;      // Acrylic panel number (indexed)
  Bool_t   fBelly;     // kFALSE if ray doesn't go through a belly plate
  Bool_t   fNeck;      // kFALSE if ray doesn't go through the AV neck
  Bool_t   fRope;      // kFALSE if ray doesn't go through a rope
  Bool_t   fPipe;      // kFALSE if ray doesn't go through a reciculation pipe
  Bool_t   fNcd;       // kFALSE if ray doesn't go through an ncd
  Bool_t   fOtherbad;  // kFALSE if ray goes on other type of bogus path
  Bool_t   fBad;       // (belly && neck && rope && pipe && otherbad)
  
  // New cuts - 
  // NCD anchors
  Bool_t fAnchor; // kFALSE of ray doesn't go through an NCD anchor
  // NCD ropes
  Bool_t fNcdRope; // kFALSE of ray doesn't go through an NCD rope

  Int_t   fCentralBad;  // same as fBad of central run

  // Other reasons for bad pmts
  Bool_t  fBasicBad;  // kFALSE if pmt passes basic validity, DQXX, and CHCS

  // Variables for occratio method (comparison to MPE-corrected occupancy
  // for a central run.)
  Float_t fOccratio;    // Occupancy ratio (usually compared ratio with central run)
  Float_t fOccratioerr; // Occupancy ratio error
  Float_t fGeomratio;   // Geometry ratio comparing this run and central run
  Float_t fGeomratioerr;// Geometry ratio error
  Float_t fModel;       // Model prediction for remaining corrections
  Float_t fModelerr;    // Model prediction error
  //Float_t fDd; // Delta (distance in D2O) between this run and central run
  //Float_t fDh; // Delta (distance in H2O) between this run and central run
  //Float_t fDa; // Delta (distance in acrylic) between this run and central run
  
  // Other variables from QPath or other analyses:
  //Float_t fLbmask;

  // Fit results:
  Float_t fChisq;    // Chi-squared value: comparing this pmt to model prediction
  Float_t fResidual; // Residual when comparing pmt-model;  (numerator of chisq)
  Float_t fSigma;    // Error assigned in chisq (denominator)

  Int_t   fFitstatus; // Bits for fit status from QOCAFit

  // To enable direct subtraction of tof used in .rch file.
  // rsd 2003.06
  Float_t fRchtof;    // Time-of-flight subtracted to get time in .rch file.

  Int_t fCrunPmtIndex;	// Index of central run pmt used for occratio, etc.

  // changes for ncd phase -- os 24.02.2005
  Int_t fCounterIndex;  // ncd closets counter index (from 0 to 39)
  Float_t fNcdMinDist;  // path distance to the closest ncd counter

 public:
  QOCAPmt();
  ~QOCAPmt();

  QOCAPmt(const QOCAPmt &rhs);
  QOCAPmt &operator=(const QOCAPmt &rhs);

  void Clear(Option_t *option="");

   // Setters
  void SetPmtn(Int_t lcn) {fPmtn = lcn;}
  void SetPmtx(Float_t x) {fPmtx = x;}
  void SetPmty(Float_t y) {fPmty = y;}
  void SetPmtz(Float_t z) {fPmtz = z;}

  //void SetPmteff(Float_t eff) {fPmteff = eff;}
  void SetPmteffm(Float_t eff) {fPmteffm = eff;}
  //void SetPmteffq(Float_t eff) {fPmteffq = eff;}
  void SetPmteffc(Float_t eff) {fPmteffc = eff;}
  //void SetThreshold(Float_t thresh) {fThreshold = thresh;}
  void SetTprompt(Float_t t) {fTprompt = t;}
  void SetTflight(Float_t t) {fTflight = t;}
  void SetTwidth(Float_t w) {fTwidth = w;}
  void SetNprompt(Float_t n) {fNprompt = n;}
  void SetOccupancy(Float_t o) {fOccupancy = o;}
  void SetOccupancyerr(Float_t oe) {fOccupancyerr = oe;}
  void SetOccCorrection(Float_t oc) {fOccCorrection = oc;}
  void SetNcdReflCoef(Float_t co) {fNcdReflCoef = co;}

  /*
  void SetDirect(Float_t dir) {fDirect = dir;}
  void SetRayleigh(Float_t ray) {fRayleigh = ray;}
  void SetReflect(Float_t ref) {fReflect = ref;}
  void SetDirTotal(Float_t dir) {fDirTotal = dir;}
  void SetRayTotal(Float_t ray) {fRayTotal = ray;}
  void SetRefTotal(Float_t ref) {fRefTotal = ref;}
  */

  void SetNtimeW(Int_t n);
  void SetOccupancyW(Int_t n,Float_t occ);
  void SetOccupancyW(Float_t *occ);
  void SetOccratioW(Int_t n,Float_t occ);
  void SetOccratioW(Float_t *occ);

  void SetCospmt(Float_t c) {fCospmt = c;}
  //void SetPmtang(Float_t r) {fPmtang = r;}
  void SetTranspwr(Float_t t) {fTranspwr = t;}

  void SetDd2o(Float_t d) {fDd2o = d;}
  void SetDh2o(Float_t d) {fDh2o = d;}
  void SetDacr(Float_t d) {fDacr = d;}
  void SetDpmt(Float_t d) {fDpmt = d;}

  void SetSolidangle(Float_t s) {fSolidangle = s;}
  void SetCospmtavg(Float_t c) {fCospmtavg = c;}
  void SetCosav(Float_t c) {fCosav = c;}

  void SetPMTrelvecX(Float_t x){fPMTrelvecX = x;}
  void SetPMTrelvecY(Float_t y){fPMTrelvecY = y;}
  void SetPMTrelvecZ(Float_t z){fPMTrelvecZ = z;}

  //void SetLaserlight(Float_t l) {fLaserlight = l;}
  void SetLasertheta(Float_t t) {fLasertheta = t;}
  void SetLaserphi(Float_t p) {fLaserphi = p;}

  void SetPanel(Int_t panel) {fPanel = panel;}
  void SetBelly(Bool_t belly) {fBelly = belly;}
  void SetNeck(Bool_t neck) {fNeck = neck;}
  void SetRope(Bool_t rope) {fRope = rope;}
  void SetPipe(Bool_t pipe) {fPipe = pipe;}
  void SetNcd(Bool_t ncd) {fNcd = ncd;}
  void SetAnchor(Bool_t anchor) {fAnchor = anchor;}
  void SetNcdRope(Bool_t ncdrope) {fNcdRope = ncdrope;}
  void SetOtherbad(Bool_t obad) {fOtherbad = obad;}
  void SetBad(Bool_t bad) {fBad = bad;}
  void SetCentralBad(Int_t cbad) {fCentralBad = cbad;}

  void SetBasicBad(Bool_t bad) {fBasicBad = bad;}

  void SetOccratio(Float_t occr) {fOccratio = occr;}
  void SetOccratioerr(Float_t ore) {fOccratioerr = ore;}
  void SetGeomratio(Float_t gr) {fGeomratio = gr;}
  void SetGeomratioerr(Float_t gre) {fGeomratioerr = gre;}
  void SetModel(Float_t m) {fModel = m;}
  void SetModelerr(Float_t me) {fModelerr = me;}
  //void SetDd(Float_t dd) {fDd = dd;}
  //void SetDh(Float_t dh) {fDh = dh;}
  //void SetDa(Float_t da) {fDa = da;}

  //void SetLbmask(Float_t lbm) {fLbmask = lbm;}

  void SetChisq(Float_t chi2) {fChisq = chi2;}
  void SetResidual(Float_t res) {fResidual = res;}
  void SetSigma(Float_t sig) {fSigma = sig;}

  void SetFitstatus(Int_t fs) {fFitstatus = fs;}

  //*******************DG added these

  void SetPoschi2(Float_t c) {fPoschi2 = c;}
  void SetPosres(Float_t d) {fPosres = d;}

  // To enable direct subtraction of tof used in .rch file.
  // rsd 2003.06
  void SetRchtof(Float_t rtof) {fRchtof = rtof;}

  void SetCrunPmtIndex(Int_t icpmt) {fCrunPmtIndex = icpmt;}

  // changes for ncd phase -- os 24.02.2005
  void SetNcdMinDist(Float_t mindist = 999.) {fNcdMinDist = mindist;}
  void SetCounterIndex(Int_t index = -1) {fCounterIndex = index;}


  // Getters
  Int_t GetPmtn() {return fPmtn;}
  Float_t GetPmtx() {return fPmtx;}
  Float_t GetPmty() {return fPmty;}
  Float_t GetPmtz() {return fPmtz;}

  //Float_t GetPmteff() {return fPmteff;}
  Float_t GetPmteffm() {return fPmteffm;}
  //Float_t GetPmteffq() {return fPmteffq;}
  Float_t GetPmteffc() {return fPmteffc;}
  //Float_t GetThreshold() {return fThreshold;}
  Float_t GetTprompt() {return fTprompt;}
  Float_t GetTflight() {return fTflight;}
  Float_t GetTwidth() {return fTwidth;}
  Float_t GetNprompt() {return fNprompt;}
  Float_t GetOccupancy() {return fOccupancy;}
  Float_t GetOccupancyerr() {return fOccupancyerr;}
  Float_t GetOccCorrection() {return fOccCorrection;}
  Float_t GetNcdReflCoef() {return fNcdReflCoef;}

  /*
  Float_t GetDirect() {return fDirect;}
  Float_t GetRayleigh() {return fRayleigh;}
  Float_t GetReflect() {return fReflect;}
  Float_t GetDirTotal() {return fDirTotal;}
  Float_t GetRayTotal() {return fRayTotal;}
  Float_t GetRefTotal() {return fRefTotal;}
  */

  Int_t GetNtimeW() {return fNtimeW;}
  Float_t GetOccupancyW(Int_t n);
  Float_t *GetOccupancyW();
  Float_t GetOccratioW(Int_t n);
  Float_t *GetOccratioW();

  Float_t GetCospmt() {return fCospmt;}
  //Float_t GetPmtang() {return fPmtang;}
  Float_t GetTranspwr() {return fTranspwr;}

  Float_t GetDd2o() {return fDd2o;}
  Float_t GetDh2o() {return fDh2o;}
  Float_t GetDacr() {return fDacr;}
  Float_t GetDpmt() {return fDpmt;}

  Float_t GetSolidangle() {return fSolidangle;}
  Float_t GetCospmtavg() {return fCospmtavg;}
  Float_t GetCosav() {return fCosav;}

  Float_t GetPMTrelvecX(){return fPMTrelvecX;}
  Float_t GetPMTrelvecY(){return fPMTrelvecY;}
  Float_t GetPMTrelvecZ(){return fPMTrelvecZ;}

  //Float_t GetLaserlight() {return fLaserlight;}
  Float_t GetLasertheta() {return fLasertheta;}
  Float_t GetLaserphi() {return fLaserphi;}

  Int_t GetPanel() {return fPanel;}
  Bool_t GetBelly() {return fBelly;}
  Bool_t GetNeck() {return fNeck;}
  Bool_t GetRope() {return fRope;}
  Bool_t GetPipe() {return fPipe;}
  Bool_t GetNcd() {return fNcd;}
  Bool_t GetAnchor() {return fAnchor;}
  Bool_t GetNcdRope() {return fNcdRope;}
  Bool_t GetOtherbad() {return fOtherbad;}
  Bool_t GetBad() {return fBad;}
  Int_t GetCentralBad() {return fCentralBad;}

  Bool_t GetBasicBad() {return fBasicBad;}
  Bool_t IsBasicBad() {return fBasicBad;}

  Float_t GetOccratio() {return fOccratio;}
  Float_t GetOccratioerr() {return fOccratioerr;}
  Float_t GetGeomratio() {return fGeomratio;}
  Float_t SetGeomratioerr() {return fGeomratioerr;}
  Float_t GetModel() {return fModel;}
  Float_t GetModelerr() {return fModelerr;}
  //Float_t GetDd() {return fDd;}
  //Float_t GetDh() {return fDh;}
  //Float_t GetDa() {return fDa;}

  //Float_t GetLbmask() {return fLbmask;}

  Float_t GetChisq() {return fChisq;}
  Float_t GetResidual() {return fResidual;}
  Float_t GetSigma() {return fSigma;}

  Int_t GetFitstatus() {return fFitstatus;}

  //*************************DG added these

  Float_t GetPoschi2() {return fPoschi2;}
  Float_t GetPosres() {return fPosres;}

  // To enable direct subtraction of tof used in .rch file.
  // rsd 2003.06
  Float_t GetRchtof() {return fRchtof;}

  Int_t GetCrunPmtIndex() {return fCrunPmtIndex;}

  // changes for ncd phase -- os 24.02.2005
  Float_t GetNcdMinDist() {return fNcdMinDist;}
  Int_t GetCounterIndex() {return fCounterIndex;}

  ClassDef(QOCAPmt,15) // OCA information for one PMT
};

#endif







