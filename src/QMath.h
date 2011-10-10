#ifndef _QMath_h
#define _QMath_h
#include <iostream>
#include <Rtypes.h>
#include "QSNO.h"
#include "TH1.h"
#include "TF1.h"
#include <stdlib.h>

class QMath 
 {
  public:
  QMath ();
  virtual ~ QMath ();
  double qsimpInt (double a, double b);
  Int_t GetNSaved(){ return fNSaved; }
  Float_t PeakFind( TH1F * th1, Float_t &tRMS, Float_t &PeakCounts, Float_t nSigmas = 3.0 );
  Float_t PeakFind2( TH1 *th1, Float_t &tRMS, Float_t &PeakCounts, Float_t tWindow = 7.0 );
  Int_t MrqFit(float x[], float y[], float sig[], int ndata, float a[], int ia[],
  int ma, float **covar, float **alpha, float *chisq );

  //NR routines
  void odeint(Double_t ystart[], int nvar, Double_t x1, Double_t x2, Double_t eps, Double_t h1,
        Double_t hmin, int *nok, int *nbad, Int_t nsol = 0, Double_t *xp = 0, Double_t *yp = 0);
  void rkqs(Double_t y[], Double_t dydx[], int n, Double_t *x, Double_t htry, Double_t eps,
        Double_t yscal[], Double_t *hdid, Double_t *hnext);
  void rkck(Double_t y[], Double_t dydx[], int n, Double_t x, Double_t h, Double_t yout[],
        Double_t yerr[]);
  Int_t  mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ia[], int ma, float **covar, float **alpha, float *chisq, float *alambda );
  void covsrt(float **covar, int ma, int ia[], int mfit);
  Int_t gaussj(float **a, int n, float **b, int m);
  virtual void mrqcof(float x[], float y[], float sig[], int ndata, float a[], int ia[], int ma, float **alpha, float beta[], float *chisq);
  Float_t rtbis (Float_t x1, Float_t x2, Float_t xacc );
   Float_t rtbis1 (TF1* f1,Float_t x1, Float_t x2, Float_t xacc );
   void four1(float *data, unsigned int nn, const int isign);
   void realft(float *data, unsigned long n, const int isign);

   //Powell minimizatin and friends.
   void  powell(float p[], float **xi, int n, float ftol, int *iter, float *fret);
   float f1dim(float x);
   float brent_linmin(float ax, float bx, float cx, float tol,float *xmin);
   void  linmin(float p[], float xi[], int n, float *fret);
   void  mnbrak_linmin(float *ax, float *bx, float *cx, float *fa, float *fb, float *fc);

   //Simplex minimization and friends.
   void  amoeba(float **p, float y[], int ndim, float ftol, int *nfunk);
   float amotry(float **p, float y[], float psum[], int ndim,
		int ihi, float fac);

   //Legendre polynomials
   Double_t plgndr(const int l, const int m, const Double_t x);
   virtual float FuncP( float *){printf("Called Qnr::Func\n"); return 0;}

  //NR support routines
  void nrerror(char error_text[]);
  Float_t *vector(long nl, long nh);
  void free_vector(Float_t *v, long nl, long nh);
  Int_t *ivector(long nl, long nh);
  void free_ivector(Int_t *v, long nl, long nh);
  Double_t *dvector(long nl, long nh);
  void free_dvector(Double_t *v, long nl, long nh);
  float **matrix(long nrl, long nrh, long ncl, long nch);
  void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
 
  //Secondary functions
  virtual double func (double){printf("Called QMath::func() \n"); return 0;}
  virtual Float_t rfunc(Float_t){printf("Called QMath::rfunc() \n"); return 0;}
  virtual void mrqfuncs(float, Int_t, float[], float *, float[], int){printf("Called QMath::mrqfuncs()\n");}
  //  virtual void mrqfuncs(float x, Int_t ix, float a[], float dyda[], float &P){printf("Called QMath::mrqfuncs\n");}
  virtual void derivs(Double_t, Double_t *, Double_t *){printf("Called QMath::Derivs()\n");}

ClassDef (QMath, 1) //Math utilities
    protected:
  Int_t fNSaved;  //Number of points in solution to ODE

};

#endif







