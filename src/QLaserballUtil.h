#ifndef _QLaserballUtil_
#define _QLaserballUtil_

//*-- Author : Olivier Simard
//*-- Copyright (C) 2006 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QLaserballUtil                                                       //
//                                                                      //
// Adapted from Bryce Moffat's code in QPath                            //
// so that it is a separate instance.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TFile.h>

// Global pointer to QLaserballUtil object

class QLaserballUtil;
static QLaserballUtil* gQLaserballUtil;

// Special function to let GetLBMaskF1() to have a function "external" to
// class for creation of the TF1.
Double_t QLaserballUtil_external_lbmaskfunction(Double_t *rs, Double_t *par);
Double_t QLaserballUtil_external_lbmasktheta(Double_t *rs, Double_t *par);
Double_t QLaserballUtil_external_LBMaskModel(Double_t *rs, Double_t *par);

class QLaserballUtil: public TObject {

 private:

  TH2* fLbtphist;        // Laserball asymmetry histogram (cos(theta),phi)
  TH1* fPmtanghist;      // Histogram for PMT angular response
  TF1* fLBMaskTF1;       // Laserball mask one-dimensional function
  TF1* fLBMaskModelTF1;  // Laserball mask one-dimensional function from QOCAFit

  Int_t fNpar;       // Number of laserball parameters (new and old)
  Double_t* fParnew; // New laserball mask parameters
  Double_t* fParold; // Old laserball mask parameters (prior to Feb-2000)

  Bool_t fDebug;   // Flag to be turned on for debugging print statements
  
 public:
  QLaserballUtil();
  ~QLaserballUtil();

  // Laserball asymmetry function, access to external file
  Float_t LBAsymmetry(Float_t costheta, Float_t phi=0);
  void SetLBAsymmetryFile(TFile* fileptr);
  
  void SetLBAsymmetryHist(TH2* lbasstp=NULL) {fLbtphist = lbasstp;}
  TH2* GetLBAsymmetryHist() {return fLbtphist;}

  void SetPMTAngHist(TH1* pmtang=NULL) {fPmtanghist = pmtang;}
  TH1* GetPMTAngHist() {return fPmtanghist;}

  // Laserball analytic mask function
  TF1* GetLBMaskTF1(){return fLBMaskTF1;}
  void SetLBMaskTF1(Int_t version = 0, Bool_t cosine = kTRUE);
  Double_t LBMask(Float_t costheta, Int_t version = 0);

  Float_t GetRunLBOrientation(Int_t run);
  Float_t GetRunLBPhi(Float_t orientation);

  void InitLBParameters();
  void SetDebugFlag(Bool_t aFlag = kFALSE){fDebug = aFlag;}

  // Laserball masking functions
  Double_t x0(Double_t d,Double_t a,Double_t b,Double_t r);
  Double_t areaellipse(Double_t a,Double_t b,Double_t d,Double_t xlo,Double_t xhi);
  Double_t areacircle(Double_t r,Double_t xlo,Double_t xhi);
  Double_t lbmaskfunction(Double_t *rs, Double_t *par);
  Double_t lbmasktheta(Double_t *rs, Double_t *par);

  // Laserball mask model from QOCAFit
  TF1* GetLBMaskModelTF1(){return fLBMaskModelTF1;}
  void SetLBMaskModelTF1(Int_t nlbmask = 0, Double_t* lbmask = NULL);
  Double_t LBMaskModel(Double_t* x, Double_t* par);
  
  ClassDef(QLaserballUtil,0)    // Laserball class for QPath
};

#endif
