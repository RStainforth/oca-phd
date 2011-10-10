#ifndef _QNCDPositionFit_
#define _QNCDPositionFit_

//*-- Author : Olivier Simard
//*-- Copyright (C) 2006 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QNCDPositionFit                                                      //
//                                                                      //
// Methods to fit NCD counter positions                                 //
// using the optical occupancy data and PMT positions.                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "QNCDArray.h"
#include "QNCD.h"
#include "QOCATree.h"
#include "QOCARun.h"
#include "QOCAPmt.h"
#include "QOCAFit.h"
#include "TF2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph.h"
#include "TMinuit.h"
#include "TMath.h"

class QOCATree;
class QNCDPositionFit;

//extern TMinuit * gMinuit;

// unnormalized gaussian functions
Double_t gauss1d(Double_t* x, Double_t* par);
Double_t gauss2d(Double_t* x, Double_t* par);

class QNCDPositionFit : public QNCDArray {

 private:

  QOCATree* fQOCATree;             // QOCATree containing all optical data
  QOCAFit* fQOCAFit;               // QOCAFit containing laserball distribution
  QNCD* fCurrentNcd;               // Current QNCD object used
  QOCARun* fCurrentRun;            // Current QOCARun object used
  QOCAPmt* fCurrentPmt;            // Current QOCAPmt object used 

  Int_t fNpos;                     // Number of valid source positions for a fit 
  Int_t fRunIndex;
  Int_t fNZsection;                // Number of z-section in a Ncd counters
  Int_t fZsectionIndex;            // Index of a z-section in a Ncd counter

  Float_t fRncd;                   // Radius of a typical Ncd counter
  Float_t fRpmt;                   // Radius of a typical SNO PMT
  Float_t fRlb;                    // Radius of laserball
  Float_t fShadowTolerance;        // Shadow tolerance used in qoptics shadow function
  TVector3 fClosest;               // Vector with closest approach to Ncd surface 
  TVector3 fPosition;              // Source position (from QOCAPmt::GetLaserxyz())
  TVector3 fDirection;             // Light direction vector (from QOptics::GetPMTrelvec())
  Float_t fDistance;               // Source-PMT full optical path distance in cm
  Float_t fCostheta;               // Cos of the incident angle of light on PMT surface

  Int_t fDisplay;                  // Display flag for messages

  // TMinuit related variables 
  Int_t fFitDisplay;               // Display value for messages in TMinuit
  Int_t fErrorType;                // Error type for error calculation (1: likelihood, 2: chisquare)

  // Occupancy Maps
  TProfile* fOccupancy1d;          // Relative Occupancy vs Closeness (fitted position)
  TProfile2D* fOccupancy2d;        // Inverse Relative Occupancy vs (x,y) map (for fitting purposes) 
  Float_t fSurfaceTolerance;       // Surface size of fOccupancy2d
  Int_t fSurfaceNbin;              // 2D bin size in (x,y) in fOccupancy2d

  Float_t fNcdXnominal;            // Default x-coordinate of the current Ncd
  Float_t fNcdYnominal;            // Default y-coordinate of the current Ncd
  Float_t* fNcdXfit;               // Array of Ncd x-coordinates per z-section after fit
  Float_t* fNcdYfit;               // Array of Ncd y-coordinates per z-section after fit
  Float_t* fNcdXerr;               // Array of Ncd x-coordinates stat errors per z-section after fit
  Float_t* fNcdYerr;               // Array of Ncd y-coordinates stat errors per z-section after fit
  Float_t* fNcdXsig;               // Array of Ncd x-coordinates spreads per z-section after fit
  Float_t* fNcdYsig;               // Array of Ncd y-coordinates spreads per z-section after fit
  Float_t* fNcdZmin;               // Array of Ncd z-section lower value
  Float_t* fNcdZmax;               // Array of Ncd z-section upper value
  Int_t* fNPmtperZ;                // Number of Pmts per z-section

  Float_t fMinOcc;                 // Minimum relative occupancy
  Float_t fMaxOcc;                 // Maximum relative occupancy
  Float_t fMaxPmtIncAng;           // Maximum pmt incident angle
  Float_t fMaxSourceRadius;        // Maximum source position radius allowed
  Float_t fMinSourceNcdDistance;   // Goes along with fMinIntersection
  Float_t fMaxSourceNcdDistance;   // Goes along with fMaxIntersection
  Int_t fMinPmtperRun;             // Minimum number of Pmts per Run per Ncd
  Int_t fMinRunperNcd;             // Minimum number of Runs per Ncd

  Int_t fNpmts;                    // Number of PMTs in the fit
  Int_t fNruns;                    // Number of runs in the fit

  Double_t fCloseness;             // Closeness parameter : see CalculateCloseness()
  Double_t fClosenessMin;          // Lowest value of closeness
  Double_t fClosenessMax;          // Largest value of closeness

  Bool_t fContours;                // Flag for contour calculations in Minuit
  Int_t fNContourPoints;           // Number of points used to draw the contour
  TGraph* fContour99;              // 99% C.L. contour
  TGraph* fContour95;              // 95% C.L. contour
  TGraph* fContour90;              // 90% C.L. contour

 public:

  // Constructor and Destructor
  QNCDPositionFit(QOCATree* aQOCATree = NULL, QOCAFit* aQOCAFit = NULL);
  virtual ~QNCDPositionFit();

  // Setters
  void SetQOCATree(QOCATree* aQOCATree);
  void SetQOCARun(Int_t aRunIndex = 0);
  void SetQOCAPmt(Int_t aPmtIndex = 0);
  void SetQNCD(Int_t aNcdIndex = 0);
  void SetNcdRadius(Float_t aNcdradius = 2.54){fRncd = aNcdradius;}
  void SetPmtRadius(Float_t aPmtradius = 13.5){fRpmt = aPmtradius;}
  void SetShadowTolerance(Float_t aShadowTolerance = 0.0){fShadowTolerance = aShadowTolerance;}
  void SetMinimumOccupancy(Float_t aMin = 0.01){fMinOcc = aMin;}
  void SetMaximumOccupancy(Float_t aMax = 10.0){fMaxOcc = aMax;}
  void SetMaxIncidentAngle(Float_t aMax = 0.5*TMath::Pi()){fMaxPmtIncAng = aMax;}
  void SetMinPmtperRun(Int_t aMin = 25){fMinPmtperRun = aMin;}
  void SetMinRunperNcd(Int_t aMin = 5){fMinRunperNcd = aMin;}
  void SetMaximumSourceRadius(Float_t aRadius = 600.0){fMaxSourceRadius = aRadius;}
  void SetMaximumRunNcdDistance(Float_t aDistance = 0.){fMaxSourceNcdDistance = aDistance;}
  void SetMinimumRunNcdDistance(Float_t aDistance = 1e+10){fMinSourceNcdDistance = aDistance;}
  void SetNZsection(Int_t aNumber = 1);
  void SetNZsectionAsym();
  void SetZLimits(Float_t aMin, Float_t aMax);
  void SetZsectionIndex(Int_t anIndex = 0){fZsectionIndex = anIndex;}
  void SetDisplay(Int_t aDisplayValue = 0){fDisplay = aDisplayValue;}
  void SetFitDisplay(Int_t aDisplayValue = -1){fFitDisplay = aDisplayValue;}
  void SetErrorType(Int_t anErrorType = 1){fErrorType = anErrorType;}
  void SetClosenessMin(Double_t aMinimum = -1.){fClosenessMin = aMinimum;}
  void SetClosenessMax(Double_t aMaximum = 1.){fClosenessMax = aMaximum;}
  void SetClosenessRange(Double_t aMinimum = -1.,Double_t aMaximum = 1.){
    fClosenessMin = aMinimum; fClosenessMax = aMaximum;}
  void SetSurfaceTolerance(Float_t aTolerance = 10.){fSurfaceTolerance = aTolerance;}
  void SetSurfaceNbin(Int_t aNumberofbins = 100){fSurfaceNbin = aNumberofbins;}
  void SetContours(Bool_t aValue = kFALSE){fContours = aValue;}
  void SetNContourPoints(Int_t aNumber = 50){fNContourPoints = aNumber;}
  void SetFitPosition(TVector3 aVector);
  void ResetPosition();

  // Getters
  Float_t GetNcdRadius(){return fRncd;}
  Float_t GetPmtRadius(){return fRpmt;}
  Float_t GetShadowTolerance(){return fShadowTolerance;}
  Float_t GetMinimumOccupancy(){return fMinOcc;}
  Float_t GetMaximumOccupancy(){return fMaxOcc;}
  Float_t GetMaxIncidentAngle(){return fMaxPmtIncAng;}
  Int_t GetMinPmtperRun(){return fMinPmtperRun;}
  Int_t GetMinRunperNcd(){return fMinRunperNcd;}
  Float_t GetMaximumSourceRadius(){return fMaxSourceRadius;}
  Float_t GetMaximumRunNcdDistance(){return fMaxSourceNcdDistance;}
  Float_t GetMinimumRunNcdDistance(){return fMinSourceNcdDistance;}
  Int_t GetNZsection(){return fNZsection;}
  Float_t GetZsectionLength(Int_t aZsectionIndex);
  Float_t GetMiddleZ(Int_t aZsectionIndex);
  Int_t GetZsectionIndex(Float_t aZvalue);
  Int_t GetNPmtperZ(Int_t aZsectionIndex);
  Float_t GetNcdNominalX(){return fNcdXnominal;}
  Float_t GetNcdNominalY(){return fNcdYnominal;}
  Float_t GetNcdPositionX(Int_t aZsectionIndex);
  Float_t* GetNcdPositionX(){return fNcdXfit;}
  Float_t GetNcdPositionXerr(Int_t aZsectionIndex);
  Float_t* GetNcdPositionXerr(){return fNcdXerr;}
  Float_t GetNcdPositionY(Int_t aZsectionIndex);
  Float_t* GetNcdPositionY(){return fNcdYfit;}
  Float_t GetNcdPositionYerr(Int_t aZsectionIndex);
  Float_t* GetNcdPositionYerr(){return fNcdYerr;}
  Float_t GetNcdPositionXsig(Int_t aZsectionIndex);
  Float_t* GetNcdPositionXsig(){return fNcdXsig;}
  Float_t GetNcdPositionYsig(Int_t aZsectionIndex);
  Float_t* GetNcdPositionYsig(){return fNcdYsig;}
  Double_t GetCloseness(){return fCloseness;}
  Double_t GetClosenessMin(){return fClosenessMin;}
  Double_t GetClosenessMax(){return fClosenessMax;}
  Float_t GetSurfaceTolerance(){return fSurfaceTolerance;}
  Int_t GetSurfaceNbin(){return fSurfaceNbin;}
  TProfile* GetOccupancyMap1D(){return fOccupancy1d;}
  TProfile2D* GetOccupancyMap2D(){return fOccupancy2d;}
  TGraph* GetContour99(){return fContour99;}
  TGraph* GetContour95(){return fContour95;}
  TGraph* GetContour90(){return fContour90;}
  Int_t GetNpmts(){return fNpmts;}
  Int_t GetNruns(){return fNruns;}

  // Checks
  Bool_t IsOtherNcd();
  Bool_t IsShadowed();
  Double_t* GetShadowLimits();
  void PrintSelection();

  // Validation and Filling functions
  Bool_t ValidatePmt();
  Bool_t ValidateRun();
  Bool_t ValidateNcd();
  void FillNcdPmt();
  void FillNcdRun();
  void FillNcd();
  void ReFillAfterFit();

  // Useful functions
  Double_t CalculateCloseness();
  void RayTrace(Double_t aValue = 0.);
  
  // Fitting functions
  void FitNcdPosition(Int_t aZsectionIndex);
  void FitNcdPosition();
  void FitNcdPosition(TH2D* surface);

  void ResetHistograms();

  ClassDef(QNCDPositionFit,0)    // NCD Position Fitter that uses Laserball Data

};

#endif
