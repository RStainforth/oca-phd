#ifndef _QMOCA_FIT_H
#define _QMOCA_FIT_H

//*-- Author : Mark Boulay
//*-- Author : Bryce Moffat - updated 24-Apr-2000 for inclusion in libqoca
//*-- Copyright (c) 2000 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QMOCAFit                                                             //
//                                                                      //
// Q Mrqmin OCA Fit routines; takes its data from a QOCATree.           //
// Attempts to fit individual PMT efficiencies; awaiting >1Gbyte of     //
// memory on some computer somewhere before it can be used fully.... :) //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#define MIN_PMT_FIT 0
#define MAX_PMT_FIT 2500

#include "QMath.h"
#include "TObject.h"
#include "QOCATree.h"
#include "TH2.h"

// ------------------------------------------------------------------------------
//  QMOCAFit class definition
// ------------------------------------------------------------------------------

class QMOCAFit : public TObject, QMath {

 private:
  QOCATree   *fTree;  //Set of OCA runs to fit
  QOCARun    *fRun;  //Current OCA run
  QOCAPmt    *fPmt;   //Current OCA pmt

  TH2F       *fLaserDist; //Laserball angular distribution
  Int_t      fNbinsTheta; //Number of theta bins for above
  Int_t      fNbinsPhi;   //Number of phi bins for above
  Int_t      fNData;      //Number of data points
  Int_t      fNPars;      //Number of parameters in fit
  Int_t      fNPmts;      //Number of PMTs in the fit
  Int_t      fEffLoc[10000]; //PMT efficiencies
  Float_t    fChisquare;  //Last chisquared value

  //Mrq working arrays
  Float_t * fmrqx;
  Float_t * fmrqy;
  Float_t * fmrqsig;
  Float_t * fmrqpars;
  Int_t   * fmrqvary;

  Float_t ** fmrqcovar;
  Float_t ** fmrqalpha; 

  Float_t    NPij( QOCARun *run, QOCAPmt *pmt, Float_t epsilon_i, Float_t intensity, Float_t LaserLight);

 public:
  QMOCAFit();
  ~QMOCAFit();

  void mrqfuncs(Float_t x, Int_t ix, Float_t a[], Float_t *y, Float_t *dyda, Int_t na);
  void SetTree( QOCATree *T);
  void DoFit();

  ClassDef(QMOCAFit, 0) // Least squares Minuit-based OCA fitter
};

#endif
