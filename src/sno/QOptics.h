#ifndef QUEENS_QOptics
#define QUEENS_QOptics

// Copyright (C) 1999 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOptics                                                              //
//                                                                      //
// SNO optics: d2o, acrylic, h2o paths for optical analysis.            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <TVector3.h>
#include <QPMTxyz.h>
#include <QMath.h>

#include "QNCDArray.h"
#include "QNCD.h"
#include "QPMTuvw.h"

#ifndef M_PI
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#endif

// 2*pi is not usually defined in math.h
#define M_2PI  6.28318530717958623

// This is the conversion factor to go from nm to eV.
// Conversion factor is (hbar*c) = 197.327053 ev*nm
// additional 2*M_PI is to go from hbar to h
#define eVnmfactor  197.327053*2.0*M_PI

// Media codes for routines tracking rays through multiple media
#define kD2O     0
#define kAcrylic 1
#define kH2O     2

// ------------------------------------------------------------------------
//  Optics class definition
// ------------------------------------------------------------------------

class QOptics: public TObject {
  
 private:
  static Int_t  fNumberOfInstances;    // number of QOptics instances for destructor
  static Bool_t fQOpticsInitialized;   // kTRUE when geometry read in

  static Double_t fAVPanelTheta[11];   // boundaries of AV panel rows
  static Double_t fAVPanelPhi[10][20]; // phi boundaries
  static Int_t    fAVPanelsInRow[10];  // number of panels in each row
  static Int_t    fAVPanelsTotal[10];  // cumulative sum of all panels up to row [i]
  static Double_t fAVInnerRadius;      // acrylic vessel inner radius - (bank GEDS 310)
  static Double_t fAVThickness;        // SNOMAN thickness (slightly > 2.1"=5.3cm?)
  static Double_t fBellyTheta[11];     // Theta of centre of belly plates
  static Double_t fBellyPhi[11];       // Phi of centre of belly plates
  static TVector3 fRopeX[20];          // ropes vertically above this point
  static TVector3 fRopeU;              // vectors are filled in the constructor
  static QMath    *fQM;                // Instance of QMath for rope calculations
  static Double_t fPMTAngRespCos[100]; // PMT angular response, in cos(theta) bins
  static QPMTuvw* fPMTuvw;             // Class and storage of PMT normals
  static Double_t fPMTReflectorRadius; // Radius of reflector bucket
  static Double_t fNCDRadius;          // Radius of NCD counter
  static Double_t fLaserballRadius;    // Radius of laserball
  
  static TVector3 fAnchorsPos[100];    // Positions of the NCD anchors (already corrected to the PSUP reference)
  static Int_t fNumAnchors;            // Number of Anchors loaded

  // Physical characteristics of the anchors
  Double_t fAnchorRadius;              // NCD Anchor Radius (3.81cm)
  Double_t fAnchorHeight;              // NCD Anchor Height (5.715cm)
  Bool_t fAnchor;                      // Variable containing the result of last call to QOptics::IsAnchor(...)
  Double_t fAnchorTolerance;           // Tolerance in light ray path to the position of the NCD anchor

  Bool_t fNcdRope;                // Constains the result of last call to QOptics::IsNcdRope(...)

  Double_t fWavelength;        // wavelength of light (ie. laserball wavelength)

  Double_t nh;                 // light water index of refraction
  Double_t na;                 // acrylic index of refraction
  Double_t nd;                 // heavy water index of refraction

  Double_t rs;                 // source radius
  Double_t r1;                 // inner av radius
  Double_t r2;                 // outer av radius
  Double_t rp;                 // PMT radius

  Double_t thPMTtarget;        // Target angle between source radial vector and
                               //  pmt radial vector (for QOptics::rtsafe())

  TVector3 source;             // source coordinates in manipulator system
  TVector3 psup_centre;        // PSUP centre in manipulator system
  TVector3 psup_centre_target; // PSUP centre target for panel angular aligning,
                               // in PSUP coordinate system
  TVector3 av_centre;          // av centre coordinates in manipulator system

  Int_t    fNumRefract;        // Number of refractions in path: 0=DIRECT, 1=inside, 2=outside
  TVector3 fAcrylicVector;     // Vector from av centre to point where light hits AV
  TVector3 fAcrylicVector2;    // Second vector for source outside AV and ray through d2o
  TVector3 fPMTrelvec;         // Vector direction of light leaving the source
  TVector3 fPMTincident;       // Vector direction of light incident on PMT
  TVector3 fPMTNormal;         // Normal to PMT, if fPMTNormalReady == kTRUE   
  Double_t fTimeOfFlight;      // Set by get distances. returns -1 on error.
  
  Int_t    fPanelNum;         // set to panel number by GetDistances
  Int_t    fLayerNum;         // set to layer number by GetDistances
  Bool_t   bellyplate;        // set to kTRUE by GetDistances if latest ray passes
                              //   through a bellyplate
  Double_t thbellyrange;      // Theta range for IsBelly(th,ph) (in radians)
  Double_t phbellyrange;      // Phi range for IsBelly(th,ph) (in radians)
  Bool_t   acrylicneck;       // set to kTRUE if latest ray analyzed passes through neck
  Bool_t   fRope;             // set to kTRUE if path intersects rope.
  Double_t ropemindist;       // Minimum distance between ray and rope for "rope crossing"
  Bool_t   fPipe;             // set to kTRUE if path intersects pipe.
  Bool_t   fPromptAVRefl;     // set to kTRUE if path likely contaminated by prompt AV refl
  Bool_t   fNcdOptics;        // set to kTRUE for the NCD-phase optics
  Bool_t   fNcd;       	      //set to kTRUE if path intersects an ncd.
  Double_t fNcdMinDist;       // Minimum distance between ray and ncd for "ncd crossing"
  Double_t fNcdTolerance;   
  static QNCDArray* fNcdArray;// Container of ncd array information
  Int_t    fCounterIndex;     // ncd counter index (from 0 to 39)
  
  Bool_t fFastMode;        // set to kTRUE to skip optical flag settings in GetDistances

  Double_t fDd2o;          // Distance through D2O for last path calculated
  Double_t fDacrylic;      // Distance through Acrylic for last path calculated
  Double_t fDh2o;          // Distance through H2O for last path calculated
  Double_t fCtheta;        // cos(theta_PMT) for last path calculated
  Double_t fCthetaBar;     // Average cos(theta_PMT) for last pmt solid angle calculated
  Double_t fTransPwr;      // net transmitted power for last path calculated
  Double_t fPMTAngResp;    // PMT angular response, beyond geometric cos(theta) factor

  Double_t fCosThetapsMin; // high radius cut: lower limit
  Double_t fCosThetapsMax; // high radius cut: upper limit

  Bool_t fSourceInside;    // set by GetDistances

  Bool_t getDistancesOutside(TVector3 p,TVector3 n);
  Bool_t getDistancesOutside(Int_t pmtn,
		      Double_t &d2o, Double_t &acrylic, Double_t &h2o,
		      Double_t &ctheta, Int_t &avpanel, Double_t &transpwr);
 
 public:
  QOptics(Double_t sx=0,Double_t sy=0,Double_t sz=0); // Read in panel geometry,
  // and allocate storage; also specify light source location
  ~QOptics();            // destructor - deallocate fAVPanelPhi[]
  
  Int_t GetNumRefract();  // Return the number of refractions 0=DIRECT, 1 or 2
  TVector3 GetAcrylic();  // Return vector to refraction point in middle of acrylic
  TVector3 GetAcrylic2(); // Source outside AV: second AV crossing for d2o/AV rays

  TVector3 GetPMTrelvec();
  TVector3 GetPMTincident();

  Int_t GetPanel();
  Int_t GetPanel(Double_t th, Double_t ph);  // Return panel id number.
  Int_t GetLayer();
  Int_t GetLayer(Int_t panelnumber);
  Int_t GetLayer(Double_t th);              // Return panel layer (0=north pole)
  Int_t GetNumberOfPanels();               // Return total number of panels

  Double_t GetAVInnerRadius(){ return fAVInnerRadius; }
  Double_t GetPMTReflectorRadius(){ return fPMTReflectorRadius; }
  Double_t GetNCDRadius(){ return fNCDRadius; }
  Double_t GetLaserballRadius(){ return fLaserballRadius; }

  Bool_t IsBelly();
  Bool_t IsBelly(Int_t panelnumber);
  Bool_t IsBelly(Double_t th, Double_t ph);  
  Bool_t IsNeckOptics();
  Bool_t IsNeckOptics(Double_t th);
  Bool_t IsChimney(TVector3 &a, TVector3 &p, Double_t delrin = -5.0,
		   Double_t delrout = 10.0);
  Bool_t IsNeckPanel(Int_t panelnumber);
  Bool_t IsNeckPanel(Double_t th, Double_t ph);
  Bool_t IsPipe();
  Bool_t IsPipe(TVector3 &a);
  Bool_t IsRope();
  Bool_t IsRope(TVector3 &a, TVector3 &p);
  Bool_t IsPromptAVRefl();
  Bool_t IsPromptAVRefl(TVector3 &s, TVector3 &p); 
  Bool_t IsNcd();
  Bool_t IsNcd(TVector3 &s, TVector3 &d);
  
  Bool_t IsNcdOptics(){return fNcdOptics;}
  void SetNcdOptics(Bool_t ncdoptics = kFALSE){fNcdOptics = ncdoptics;}
  void ResetNcdPositions(Char_t* aFile = NULL);
  Double_t GetNcdMinDist(){ return fNcdMinDist; }
  void    SetNcdTolerance(Double_t aTolerance) {fNcdTolerance = aTolerance;}
  Double_t GetNcdTolerance() {return fNcdTolerance;}
  Int_t GetCounterIndex(){return fCounterIndex;}
  QNCDArray* GetQNCDArray(){return fNcdArray;}

  Bool_t IsSourceInside();

  TVector3 GetPMTPosition(Int_t pmtn);
  TVector3 GetPMTNormal(Int_t pmtn);

  QPMTuvw *GetPMTuvw();

  Bool_t GetDistances(Int_t pmtn);
  Bool_t GetDistances(Int_t pmtn,
		      Double_t &d2o,Double_t &acrylic,Double_t &h2o,
		      Double_t &ctheta, Int_t &avpanel,
		      Double_t &transpwr);
  Bool_t GetDistances(TVector3 p,TVector3 n);
  Bool_t GetDistancesToNcd(Int_t aNcdIndex,Double_t aZ = 0.,Double_t anAngle = 0.);
  Double_t GetTimeOfFlight();
  Double_t GetPmtSolidAngle(Int_t pmtn);
  Int_t    GetPmtNcds2DProjection(Int_t pmtn, Double_t *a, Double_t *b, Double_t *theta, Double_t *ncdcx, Double_t *ncdcy, Int_t *ncdst);
  Double_t GetPmtShadow(Int_t pmtn, Bool_t ncdendapprox=kTRUE, Bool_t withpmtrefraction=kTRUE);
  Int_t GetFastShadow(Int_t pmtn, Bool_t *incd);
  Int_t GetPmtNormalProjection(Int_t pmtn, Double_t *a, Double_t *b, Double_t *theta, TVector3 *punormal, TVector3 *xaxis, TVector3 *yaxis, Bool_t withpmtrefraction=kTRUE);
  Int_t GetPmtNormalProjection(Int_t pmtn, Double_t *a, Double_t *b, Double_t *theta, Double_t *x0, Double_t *y0, TVector3 *punormal, TVector3 *xaxis, TVector3 *yaxis);
  Int_t GetPmtPointsNormalProjection(Int_t pmtn, Bool_t refraction, Int_t numpoints, Double_t *xpoints, Double_t *ypoints, TVector3 *punormal, TVector3 *xaxis, TVector3 *yaxis);
  Bool_t Get2DRotProjection(const TVector3 &punormal, const TVector3 &xaxis, const TVector3 &yaxis, const Double_t &theta, TVector3 point, Double_t *x, Double_t *y);
  Int_t GetNcd2DProjection(const TVector3 &punormal, const TVector3 &xaxis, const TVector3 &yaxis, const Double_t &theta, const Int_t ncd, Double_t *ncdx, Double_t *ncdy);
  Int_t GetOtherNcdPoint(const TVector3 &punormal, const TVector3 &ancdpoint, const Bool_t findupper, const Double_t &theta, TVector3 *newpoint);
  void  GetLEIntersection(const Double_t &a, const Double_t &b, const Double_t &x1, const Double_t &y1, const Double_t &x2, const Double_t &y2, Double_t *m, Double_t *c, Double_t *ix1, Double_t *iy1, Double_t *ix2, Double_t *iy2, Int_t *status);
  Double_t GetEllipseIntegral(const Double_t &a, const Double_t &b, const Double_t *xypoints);
  Double_t GetNcdSolidAngle(Int_t aNcdIndex,Double_t aZinitial = -999.,Double_t aZfinal = 999.);

  void ShootToPSUP(TVector3& x, TVector3& p, TVector3& psup);
  void LineToSphere(TVector3& x, TVector3& p, TVector3& xsp, Double_t radius);
  void Refract(Int_t m1, Int_t m2, TVector3& n, TVector3& p1, TVector3& p2, 
	       Double_t& transpwr, Bool_t& tir);

  void SetSource(Double_t sx=0,Double_t sy=0,Double_t sz=0);
  void SetSource(TVector3 s);
  TVector3 GetSource();

  void SetPSUPCentre(Double_t px=0,Double_t py=0,Double_t pz=0);
  void SetPSUPCentre(TVector3 pc);
  TVector3 GetPSUPCentre();

  void SetPSUPCentreTarget(Double_t px=0,Double_t py=0,Double_t pz=0);
  void SetPSUPCentreTarget(TVector3 pct);
  TVector3 GetPSUPCentreTarget();

  void SetAVCentre(Double_t px=0,Double_t py=0,Double_t pz=0);
  void SetAVCentre(TVector3 ac);
  TVector3 GetAVCentre();

  Double_t GetDd2o();
  Double_t GetDacrylic();
  Double_t GetDh2o();
  Double_t GetCtheta();
  Double_t GetCthetaBar();

  Double_t GetTransPwr();
  Double_t GetTransPwr(Double_t n,Double_t np,Double_t incident);
  Double_t GetTransPwrPara(Double_t n,Double_t np,Double_t incident);
  Double_t GetTransPwrPerp(Double_t n,Double_t np,Double_t incident);

  Double_t GetPMTAngResp();
  Double_t GetPMTAngResp(Double_t costheta);

  void SetWavelength(Double_t lambda);
  Double_t GetWavelength();

  void SetIndices(Double_t lambda = 0);
  void SetIndexD2O( Double_t val ){ nd = val; }
  void SetIndexAcrylic( Double_t val ){ na = val; }
  void SetIndexH2O( Double_t val ){ nh = val; }
  Double_t GetIndexD2O(Double_t lambda = 0,Bool_t derivative=kFALSE);
  Double_t GetIndexH2O(Double_t lamdba = 0,Bool_t derivative=kFALSE);
  Double_t GetIndexAcrylic(Double_t lambda = 0,Bool_t derivative=kFALSE);

  Double_t GetRayleighD2O(Double_t lambda = 0);
  Double_t GetRayleighH2O(Double_t lambda = 0);
  Double_t GetRayleighAcrylic(Double_t lambda = 0);

  Double_t GetVgroupD2O (Double_t aLambda = 0);
  Double_t GetVgroupH2O(Double_t aLambda = 0);
  Double_t GetVgroupAcrylic(Double_t aLambda = 0);
  Double_t VgroupD2O (Double_t aLambda = 0);       // Legacy function
  Double_t VgroupH2O(Double_t aLambda = 0);        // Legacy function
  Double_t VgroupAcrylic(Double_t aLambda = 0);    // Legacy function

  Double_t LambdaToOmega(Double_t aLambda = 0);
  Double_t OmegaToLambda(Double_t aOmega);
  Double_t LambdaToEnergy(Double_t aLamda = 0);
  Double_t GetNcdReflectivity(Double_t aLambda = 0);

  void Setthbellyrange(Double_t tbr=9*M_PI/180) { thbellyrange = fabs(tbr); }
  void Setphbellyrange(Double_t pbr=9*M_PI/180) { phbellyrange = fabs(pbr); }
  Double_t Getthbellyrange() { return thbellyrange; }
  Double_t Getphbellyrange() { return phbellyrange; }

  void Setropemindist(Double_t rmd=15) { ropemindist = fabs(rmd); }
  Double_t Getropemindist() { return ropemindist; }

  Double_t GetCosThetapsMin(){return fCosThetapsMin;}
  Double_t GetCosThetapsMax(){return fCosThetapsMax;}
  void SetCosThetapsMin(Double_t cthpsmin = 9999.){fCosThetapsMin=cthpsmin;}
  void SetCosThetapsMax(Double_t cthpsmax = -9999.){fCosThetapsMax=cthpsmax;}

  // Utility Routines for refraction between heavy water/acrylic/light water
  // 1-3: inside and outside
  // 4-5: outside only
  // Residual: difference between thtarget and the sum of (1+2+3) or (1+2+3+4+5)
  Double_t theta1(const double &theta);
  Double_t d1dt(const double &theta);
  Double_t theta2(const double &theta);
  Double_t d2dt(const double &theta);
  Double_t theta3(const double &theta);
  Double_t d3dt(const double &theta);

  Double_t theta4(const double &theta);
  Double_t d4dt(const double &theta);
  Double_t theta5(const double &theta);
  Double_t d5dt(const double &theta);

  Double_t thResidual(const double &theta);
  Double_t dthResidualdth(const double &theta);

  void funcd(double theta,double *f,double *df);
  Double_t rtsafe(Double_t x1, Double_t x2, Double_t xacc);
  
  Bool_t IsAnchor();
  Bool_t IsAnchor(TVector3 &s, TVector3 &d);
  Double_t GetClosestAnchor(TVector3 &s, TVector3 &d, Int_t &index, TVector3 &closest);


  Bool_t IsNcdRope( );
  Bool_t IsNcdRope(TVector3 &s, TVector3 &d);
  Float_t distSegments( TVector3 &p1, TVector3 &p2, TVector3 &s1, TVector3 &s2);

  void SetFastMode(Bool_t aFlag=kFALSE,Bool_t kPrint=kTRUE);
  Bool_t GetFastMode(){return fFastMode;}
  
  ClassDef(QOptics,0)    // Optics class
};

#endif
