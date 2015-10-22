#ifndef QLBPositionFit_h
#define QLBPositionFit_h

//*-- Author : Aksel Hallin

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QLBPositionFit                                                              //
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
#include "QOptics.h"

class QPmtFit;

const int  fNparameters=10;

class QLBPositionFit:public QMath{
 public:
  QLBPositionFit(QPmtFit &aFit);
  ~QLBPositionFit();
  void DoFit(Int_t Run1, Int_t Run2, TH1F &aPullHist, Float_t &aChiSquare);
  void mrqfuncs(float x, Int_t ix, float a[], float *y, float dyda[], int na);
  void SetMode(Int_t aMode){fMode=aMode;}
  TVector3 &GetLBPosition1(){return fLBPosition1;}
  TVector3 &GetLBPosition2(){return fLBPosition2;}
  Float_t GetCovariance(int i,int j){return fCovar[i+1][j+1];}
  Float_t GetAlpha(int i,int j){return fAlpha[i+1][j+1];}
  void SetFastDistances(Int_t aMode){fFastDistances=aMode;}
  //  void GetDirections(Int_t aPmt);
  void SetSource(TVector3 &aSourcePosition, Int_t aRun);
  void GetDistances(Int_t aPmt, Int_t aRun, Double_t &aDd2o, 
		    Double_t &aDacrylic, Double_t &aDh2o);
  void CalculateChiSquare(Int_t iparam,TH1F &chis);
  Float_t GetTriggerOffset(){return fParameters[9];}
  Int_t GetN(){return fN;}

 private:
  int fMode; //0 for const. speed of light, 1 for QOptics
  QPmtFit &fFit;  //PmtFit that is used to access data
  Int_t fRun1; // Index of first run
  Int_t fRun2; // Index of second rund
  QOptics fOptics;
  QOptics fOptics2;  //one QOptics obect for each laserball
  
  Int_t fN; // Number of PMTs
  Float_t fX[10000];  // array of pmt indices
  Float_t fTimes[10000];   // array of times
  TVector3 fPmt[10000];  // array of pmt positions
  float fSigma[10000];           //! array of errors
  float **fCovar;           //! covariance matrix
  float **fAlpha;           //!
  int fIa[fNparameters];
  float fParameters[fNparameters];
  float fDtDx;              // 1/mean group velocity
  float fDtDx0;
  float fDtDxH2O;           // water dtdx
  float fDtDxAcrylic;
  float fDtDxD2O;
  TVector3 fLBPosition1;    // Laserball position 1
  TVector3 fLBPosition2;    // Laserball position 2
  TVector3 fP, fV1,fV2,fVN1, fVN2;  // working vectors for Mrqfunc
  
  Int_t fSourceInitialized; //true after first pass through QOptics
  Int_t fFastDistances; // 0=Use QOptics::GetDistances, 1= QLBpositionFIt::GetDistances
  TVector3 *fSourcePositions; //record of source positions for calc. distances
  Float_t *fD2ODistances[10000];
  Float_t *fH2ODistances[10000];
  Float_t *fAcrylicDistances[10000];
  TVector3 *fDirections[10000];
  TVector3 *fOffset;
  ClassDef(QLBPositionFit,0)  //Fit the laserball position based on time differences between two runs
  };

#endif

