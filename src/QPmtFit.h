#ifndef QPmtFit_h
#define QPmtFit_h

//*-- Author : Aksel Hallin

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QPmtFit                                                              //
//                                                                      //
// This class is designed to do fits that extract the PMT positions     //
// and time offsets from an optics scan.  If we had a perfect PMT map   //
// and perfect PCA this routine would not be useful- the point is to    //
// check consistency between the various laserball positions.           //
//
//  One intializes this with a QOCATree-  it goes through, extracts     //
//  all the runs in a scan, and then allows you to fit one or all of the //
// PMT positions.
//////////////////////////////////////////////////////////////////////////

#include "QMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

class QOCATree;

class QPmtFit:public QMath{
 public:
  QPmtFit(QOCATree *tree);
  ~QPmtFit();
  //  void FitPmt(int n);
  void CheckPositions();
  void CheckPmtt();
  void mrqfuncs(float x, Int_t ix, float a[], float *y, float dyda[], int na);
  TH1F *GetDistance(){return &fDistance;}
  TH1F *GetChisq(){return &fChisq;}
  TH1F *GetPmts(){return &fPmts;}
  TH1F *GetCutDistance(){return &fCutDistance;}
  TH1F *GetAngle(){return &fAngle;}
  TH2F *GetDevlb(){return &fDevlb;}
  TH1F *GetXdev(){return &fXdev;}
  TH1F *GetYdev(){return &fYdev;}
  TH1F *GetZdev(){return &fZdev;}
  TVector3 &GetLBPosition(Int_t i){return fLBPositions[i];}
  Int_t GetRunNumber(Int_t i){return fRunNumbers[i];}
  Int_t GetNumberOfRuns(){return fN;}
  void GetData(Int_t Run1, Int_t Run2,float *x, float *t, float *sigma, Int_t &n);

 private:
  int fN;  // number of positions of the laserball
  float *fTimes[10000];   // array of times
  TVector3 *fLBPositions;  // array of laserball positions
  Int_t *fRunNumbers;
  float *fX;               //! data array
  float *fSigma[10000];           //! array of errors
  float *fWidth[10000];           //! array of widths
  float **fCovar;           //! covariance matrix
  float **fAlpha;           //!
  int  fNparameters;
  int fIa[4];
  float fParameters[4];
  float fDtDx;              // 1/mean group velocity
  TH1F fDistance;
  TH1F fChisq;
  TH1F fPmts;
  TH1F fCutDistance;    // Distance for tubes with good chi-square
  TH1F fAngle;
  TH2F fDevlb;      //deviation vs laserball angle
  TH1F fXdev;
  TH1F fYdev;
  TH1F fZdev;
  ClassDef(QPmtFit,0)  // Class for looking for mis-cabelled PMTs from optics scan
  };

#endif

