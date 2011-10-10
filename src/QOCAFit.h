#ifndef _QOCAFit_
#define _QOCAFit_

//*-- Authors : Bryce Moffat,R.Dosanjh,J.Maneira,O.Simard,N.Barros
//*-- Copyright (C) 2000 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCAFit                                                              //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <math.h>
#include "nrutil.h"
#include "TNamed.h"
#include "TH2.h"
#include "QMath.h"
#include "QPMTxyz.h"
#include "QOCATree.h"

// ------------------------------------------------------------------------------
//  QOCAFit class definition
// ------------------------------------------------------------------------------

class QOCAFit : public TNamed, QMath {

 private:
  Version_t fVersion;// QOCAFit version (for GetXXXpari() to avoid version shear)
  Bool_t fValidPars; // QOCAFit structure has allocated, valid parameters
  Bool_t fTreeSet;   // Data tree and parameters setup for fit?
  Bool_t fDataSetup; // Data setup and filtered for reasonable tubes?
  Bool_t fSingleNorm;// Single or multiple normalizations for off-centre runs
  Int_t fTimeWi;     // Time window index to use for fit: 0=fOccratio or fOccW[1-12]

  Int_t fOccType;    // Sets occupancy variable type 
                     // Occupancy = 0 (whatever is in QOCAPmt::fOccupancy)
                     // OccRatio = 1  (whatever is in QOCAPmt::fOccratio : default)
  
  QOCATree *fTree;  //! Raw data of OCA tree to be fit
  QOCATree *fCtree; //! Raw data of OCA tree of central runs

  QOCARun **fRuns;  //! Array of pointers to OCA runs to be fit (from fTree)

  QOCARun *fCurrentrun;   //! Current OCA run
  QOCAPmt *fCurrentpmt;   //! Current OCA pmt

  QOCARun *fCurrentctrrun; //! Current central OCA run
  QOCAPmt *fCurrentctrpmt; //! Current central OCA pmt

  //-----------------------------------------------------------------------------
  // Cross-indexing scheme for runs/central runs in occupancy ratio method
  Int_t fNumberofruns;   // Number of runs
  Int_t *fRunlist;       //! Array of run numbers to process in tree
  Int_t *fCentrerunlist; //! Array of centre run numbers for each entry in fRunlist[]
  Int_t *fCentrerunindex; //! Array of centre run indices: one per entry in fRunlist[]
  
  QOCARun **fCentrerunptr; //! Array of ptrs to **distinct** central runs
  
  Int_t fNumberofcentre; // Number of **distinct** centre runs
  Int_t *fCentreruns;    //! Array of **distinct** central run numbers

  Int_t *fRunsToDo; //! Array of runs to process (rsd 11/2002)
  Int_t *fRunsToDoIndex; //! Array maps fRunsToDo to fRuns (rsd 11/2002)
  // Int_t *fRunsInFile; // list of runs in input file (rsd 11/2002)
  Int_t fNumberofrunsinfile; // total number of runs in input file (rsd 11/2002)

  //-----------------------------------------------------------------------------
  // Laserball and detector model parameters
  TH2F *fLaserdist; // Laserball angular distribution
  Int_t fNbinstheta; // Number of theta bins for above
  Int_t fNbinsphi;   // Number of phi bins for above
  Int_t fNbinsthetawave; // Number of theta bins for sinusoidal LB dist
  Int_t fNdistwave;  // Number of parameters in a single theta slot for the LB dist

  Int_t fNlbmask;    // Number of parameters for laserball mask in theta or cos(theta)
  Int_t fNangresp;   // Number of PMT angular response parameters

  Int_t fLBdistType;      // Laserball distribution type
                          // 0: binned (default) - fNbinstheta*fNbinsphi
                          // 1: sine - fNbinstheta*fNdistwave
  Int_t fAngrespFuncType; // Angular response: interpolation (0) or binned (1 and 2)

  Int_t fNpars;      // Number of parameters in fit

  Int_t fNdata;      // Number of data points
  Int_t fNpmts;      // (Total) Number of PMTs in the fit (from all runs)

  Float_t fChisquare;  // Last chisquared value (not reduced)
  Float_t fChilimit;   // Limit chisquared value: lower than this only are accepted
  Float_t fChilimmin;  // Minimum value of chisquared for cutting tubes from fit
  Float_t fChimult;    // Multiplier for chisquared/(deg. of freedom) -> fChilimit
  Int_t fChicuttype;   // Cut type: 0, 1 or 2 (see QOCAFit::DataSetup()).

  Int_t fMinPMTperangle; // Minimum number of PMT's per angular bin in angular resp.
  Int_t fMinPMTperLBbin; // Minimum number of PMT's per laserball distribution bin
  Bool_t fDynamicOccupancyCut; // cuts PMTs based on occupancy in PmtSkip(mean,sigma)
                               // if set to kTRUE. Otherwise it uses old hardcoded cut.
  Float_t fDynamicSigma; // Number of standard deviations to based 
                         // the cut on in PmtSkip(mean,sigma)

  Int_t fNSkip;     // Number of tubes to skip when calculating model in mrqcof()
  Int_t fNStart;    // Starting tube for loops - usually 0 for the first one!

  //-----------------------------------------------------------------------------
  // Mrq working arrays
  Float_t *fmrqx;    //! [fNdata+1] Index into array of PMT/Run for cross reference
  Float_t *fmrqy;    //! [fNdata+1] Data values for each PMT in the dataset
  Float_t *fmrqsig;  //! [fNdata+1] Error on each PMT occupancy ratio (statistical)

  Float_t *fmrqpars;   //! [fNpars+1] Parameters for model
  Int_t   *fmrqvary;   //! [fNpars+1] Flag for variable parameters in mrqmin
  Float_t **fmrqcovar; //! [fNpars+1][fNpars+1] Covariance matrix
  Float_t **fmrqalpha; //! [fNpars+1][fNpars+1] Curvature matrix (?)

  Float_t *fparscopy;   //! [fNpars+1] Backup space for model parameters when changing # pars
  Int_t   *fvarycopy;   //! [fNpars+1] Backup space when changing # pars
  Float_t **fcovarcopy; //! [fNpars+1][fNpars+1] Backup space when changing # pars
  Float_t **falphacopy; //! [fNpars+1][fNpars+1] Backup space when changing # pars
  Int_t fnparscopy;     //! Number of parameters in backup copy.

  Int_t finorm;  //! Index for normalization derivative to be re-zeroed
  Int_t fiang;   //! Index for PMT angular response to be re-zeroed
  Int_t fciang;  //! Index for PMT angular response from central run to be re-zeroed
  Int_t filbdist;  //! Index for LB dist'n to be re-zeroed
  Int_t fcilbdist; //! Index for LB dist'n from central run to be re-zeroed

  // Variables to keep track of which parameters affect the calculation for a single
  // Model() evaluation for a particular PMT.
  Int_t fparmabase;     //! Number of variable parameters, not including PMTR or LBDIST
  Int_t fparma;         //! Number of variable parameters
  Int_t ***fangindex;   //! Lookup table for unique variable parameters in PMTR
  Int_t *fparindex;     //! Lookup table for ordered variable parameters
  Int_t *fparvarmap;    //! Lookup table for variable parameters: global <--> in the fit
  Bool_t fRepeatoldmrqcof;  //! Flag to run through debugging code in mrqcof

  Int_t fPrint;    // Print level: 0 no messages, 1 occasional info, 2 lots, 3 extreme
  Int_t fNcalls;   // Number of calls to mrqfuncs() - used as a diagnostic
  Int_t fNmessage; // Number of calls between message prints - usually 10% of fNpmts
  
  Int_t fNelements;  // Number of elements in fResarray[] and fChiarray[]
  Float_t *fResarray;  //[fNelements] Array of residuals for mrqcof() calls
  Float_t *fChiarray;  //[fNelements] Array of chisquared for mrqcof() calls

  Int_t fModelErrType; // Type of model error (0=none, 1=theta_pmt, 2=constant, 3=input_theta_pmt)
  Float_t fModelErr;   // Value of constant model error to be used (not saved in file!)
  Float_t fPmtVarPar0; // parameters for PMT variability function with theta_pmt
  Float_t fPmtVarPar1; // parameters for PMT variability function with theta_pmt
  Float_t fPmtVarPar2; // parameters for PMT variability function with theta_pmt

  Int_t fPmtCoordCode;    // Coordinate integer code for Pmt grouping
                          // (x,y,z) = (1,2,3) ; batch3 = 4 ; etc.
  Float_t fPmtCoordCutLo; // Lower  limit on coordinate for Pmt grouping
  Float_t fPmtCoordCutHi; // Higher limit on coordinate for Pmt grouping

  // Internal functions used within the code (no user access)
  void PrintParameter(Int_t parindex = 0);

 public:
  QOCAFit(const char *name="qocafit", const char *title="QOCAFit");
  ~QOCAFit();
  void DeAllocate();
  void AllocateParameters();
  void DeAllocateParameters();

  void ReAllocateNorms(Int_t nr);
  
  //--------------------
  // User utility functions
  virtual void SetTree(QOCATree *dataruns, QOCATree *centreruns=NULL);
  virtual void InitParameters(Int_t numberofruns=0);
  virtual void DoFit();
  virtual void DataSetup();
  virtual Float_t CalcChisquare();
  virtual void FitNormalizations();

  void GetParameters(QOCAFit *seed);
  void SetParameters(QOCAFit *target);

  void SetTimeWi(Int_t i);
  Int_t GetTimeWi() {return fTimeWi;}

  void SetAllVary(Bool_t vary = kTRUE);
  void SetD2OVary(Bool_t vary = kTRUE);
  void SetAcrylicVary(Bool_t vary = kTRUE);
  void SetH2OVary(Bool_t vary = kTRUE);
  void SetMaskVary(Bool_t vary = kTRUE);
  void SetAngRespVary(Bool_t vary = kTRUE);
  void SetAngResp2Vary(Bool_t vary = kTRUE);
  void SetLBDistVary(Bool_t vary = kTRUE);
  void SetLBDistWaveVary(Bool_t vary = kTRUE);
  void SetNormsVary(Bool_t vary = kTRUE);
  void SetRayleighD2OVary(Bool_t vary = kTRUE);
  void SetRayleighAcrylicVary(Bool_t vary = kTRUE);
  void SetRayleighH2OVary(Bool_t vary = kTRUE);
  void SetRayleighVary(Bool_t vary = kTRUE);
  void SetSingleNorm(Bool_t singlenorm = kTRUE);

  Bool_t GetVary();
  Bool_t GetD2OVary() { return fmrqvary[Getd2opari()]; }
  Bool_t GetAcrylicVary() { return fmrqvary[Getacrylicpari()]; }
  Bool_t GetH2OVary() { return fmrqvary[Geth2opari()]; }
  Bool_t GetMaskVary();
  Bool_t GetAngRespVary();
  Bool_t GetAngResp2Vary();
  Bool_t GetLBDistVary();
  Bool_t GetLBDistWaveVary();
  Bool_t GetNormsVary();
  Bool_t GetRayleighD2OVary() { return fmrqvary[Getrsd2opari()]; }
  Bool_t GetRayleighAcrylicVary() { return fmrqvary[Getrsacrylicpari()]; }
  Bool_t GetRayleighH2OVary() { return fmrqvary[Getrsh2opari()]; }
  Bool_t GetRayleighVary();

  Bool_t GetSingleNorm() { return fSingleNorm; }

  Int_t GetNparsVariable();
  Int_t GetNparsFixed();

  virtual QOCATree *Apply();
  virtual void ApplyRun(QOCARun *ocarun);

  virtual TH2F *ApplyLaserdist();
  virtual void FillLBDist(TH2F *laserdist);

  TH1F *AngRespTH1F();
  TF1 *AngRespTF1();
  TH1F *AngResp2TH1F();
  TF1 *AngResp2TF1();
  static Double_t sPMTResp(Double_t *a, Double_t *par);

  TF1 *LBMaskTF1();
  static Double_t sLBMask(Double_t *a, Double_t *par);
  static Double_t dLBMask(Double_t *a, Double_t *par);

  TF1 *LBDistWaveTF1(Int_t itheta);
  static Double_t sLBDistWave(Double_t *a, Double_t *par);
  static Double_t dLBDistWave(Double_t *a, Double_t *par);

  //--------------------
  // Optical model functions
  Float_t Model(Int_t irun, Int_t pmtn, Int_t na=0,
		Float_t *dyda = NULL);

  void SetModelErrFunction(Float_t par0, Float_t par1, Float_t par2 = 0.0);
  void GetModelErrFunction();
  Float_t GetModelErrFunctionPar(Int_t parnumber = 0);
  Float_t ModelErr(Int_t irun, Int_t pmtn, Float_t occratio = -1);
  void SetModelErr(Int_t type,Float_t err=0.03);
  Float_t GetModelErr() { return fModelErr; }
  Int_t GetModelErrType() { return fModelErrType; }

  Float_t ModelAngResp(QOCAPmt *pmt);
  Float_t ModelAngResp(QOCAPmt *pmt, Int_t &pari, Float_t &interpolfrac);
  Float_t ModelAngResp(Float_t angle);
  //Float_t ModelAngResp(Float_t angle, Int_t &pari, Float_t &interpolfrac);
  Float_t ModelAngResp(Float_t angle, Int_t &pari, Float_t &interpolfrac, Int_t pmtgroup = 1);

  Float_t ModelLBMask(QOCAPmt *pmt);
  Float_t ModelLBMask(Float_t costheta);

  Float_t ModelLBDist(QOCARun *run, QOCAPmt *pmt);
  Float_t ModelLBDist(QOCARun *run, QOCAPmt *pmt, Int_t &pari);
  Float_t ModelLBDist(Float_t costheta, Float_t phi);
  Float_t ModelLBDist(Float_t costheta, Float_t phi, Int_t &pari);

  //--------------------
  // QMath functions, modified to report on QOCAFit variables
  Int_t MrqFit(float x[], float y[], float sig[], int ndata, float a[],
	       int ia[], int ma, float **covar, float **alpha, float *chisq );
  Int_t  mrqmin(float x[], float y[], float sig[], int ndata, float a[],
		int ia[], int ma, float **covar, float **alpha, float *chisq,
		float *alambda );
  void covsrt(float **covar, int ma, int ia[], int mfit);
  Int_t gaussj(float **a, int n, float **b, int m);
  virtual void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
		      int ia[], int ma, float **alpha, float beta[],
		      float *chisq);
  virtual void mrqfuncs(Float_t x,Int_t ix,Float_t a[],Float_t *y,
			Float_t dyda[],Int_t na);

  void SetMrqArrays(Int_t nelements,Float_t *resarray,Float_t *chiarray);
  Int_t GetNelements() { return fNelements; }
  Float_t *GetResarray() { return fResarray; }
  Float_t *GetChiarray() { return fChiarray; }

  //--------------------
  // Special functions to keep track of unique variable parameters for a particular
  // model evaluation - usually < 15 parameters out of several hundred total
  void FillParmabase();
  void FillAngIndex();
  Int_t FillParsPoint();

  void SetRepeatoldmrqcof(Bool_t rom=kTRUE) { fRepeatoldmrqcof = rom; }
  Bool_t GetRepeatoldmrqcof() { return fRepeatoldmrqcof; }

  //--------------------
  // Get/Set functions
  Int_t GetNumberofruns() { return fNumberofruns; }
  void SetNumberofruns(Int_t nr=-1);

  Int_t GetOccType()	  {return fOccType;}
  void SetOccType(Int_t occtype = 0) {fOccType = occtype;}
  Float_t GetOccVariable();
  Float_t GetOccVariableErr();
  Bool_t PmtSkip();
  Bool_t PmtSkip(Float_t mean, Float_t sigma);
  
  void BackupPars();
  void RestorePars(Int_t mid1, Int_t newmid2, Int_t oldmid2,
		   Float_t newval = 0.0, Int_t newvary = 1);

  TH2F *GetLaserdist() { return fLaserdist; }
  Int_t GetNbinstheta() { return fNbinstheta; }
  Int_t GetNbinsphi() { return fNbinsphi; }
  Int_t GetNbinsthetawave() { return fNbinsthetawave; }
  Int_t GetNdistwave() { return fNdistwave; }
  void SetNbinstheta(Int_t nbt);
  void SetNbinsphi(Int_t nbp);
  void SetNbinsthetawave(Int_t nbt);
  void SetNdistwave(Int_t ndw);

  Int_t GetNlbmask() { return fNlbmask; }
  Int_t GetNangresp() { return fNangresp; }
  void SetNlbmask(Int_t nlbm);
  void SetNangresp(Int_t nar);

  void SetAngrespFuncType(Int_t functype = 0);
  Int_t GetAngrespFuncType() { return fAngrespFuncType; }
  void SetLBdistType(Int_t disttype = 0);
  Int_t GetLBdistType() { return fLBdistType; }

  Int_t GetNpars() { return fNpars; }
  Int_t GetNdata() { return fNdata; }
  Int_t GetNpmts() { return fNpmts; }
  void SetNpmts(Int_t n);

  Float_t *Getfmrqx() { return fmrqx; }
  Float_t *Getfmrqy() { return fmrqy; }
  Float_t *Getfmrqsig() { return fmrqsig; }
  Float_t *Getfmrqpars() { return fmrqpars; }
  Int_t *Getfmrqvary() { return fmrqvary; }
  Float_t **Getfmrqcovar() { return fmrqcovar; }
  Float_t **Getfmrqalpha() { return fmrqalpha; }

  Int_t Getfparmabase() { return fparmabase; }
  Int_t Getfparma() { return fparma; }
  Int_t ***Getfangindex() { return fangindex; }
  Int_t *Getfparindex() { return fparindex; }
  Int_t *Getfparvarmap() { return fparvarmap; }

  void Setfmrqx(Float_t *f) { fmrqx = f; }
  void Setfmrqy(Float_t *f) { fmrqy = f; }
  void Setfmrqsig(Float_t *f) { fmrqsig = f; }
  void Setfmrqpars(Float_t *f) { fmrqpars = f; }
  void Setfmrqvary(Int_t *v) { fmrqvary = v; }
  void Setfmrqcovar(Float_t **f) { fmrqcovar = f; }
  void Setfmrqalpha(Float_t **f) { fmrqalpha = f; }

  //--------------------
  // Ncalls for mrqfuncs()
  Int_t GetPrint() { return fPrint; }
  void SetPrint(Int_t n=1) { fPrint = n; }
  Int_t GetNcalls() { return fNcalls; }
  void SetNcalls(Int_t n=0) { fNcalls = n; }
  Int_t GetNmessage() { return fNmessage; }
  void SetNmessage(Int_t n=5000) { fNmessage = n; }

  //--------------------
  // Chisquared limit for pruning outliers
  Float_t GetChisquare() { return fChisquare; }
  void SetChisquare(Float_t c) { fChisquare = c; }
  Float_t GetChilimit() { return fChilimit; }
  void SetChilimit(Float_t cl=100) { fChilimit = cl; }
  Float_t GetChilimmin() { return fChilimmin; }
  void SetChilimmin(Float_t clm=100) { fChilimmin = clm; }
  Float_t GetChimult() { return fChimult; }
  void SetChimult(Float_t cm=10) { fChimult = cm; }
  Int_t GetChicuttype() { return fChicuttype; }
  void SetChicuttype(Int_t ct=0) { fChicuttype = ct; }

  Int_t GetMinPMTperangle() { return fMinPMTperangle; }
  void SetMinPMTperangle(Int_t mp=25) { fMinPMTperangle = mp; }
  Int_t GetMinPMTperLBbin() { return fMinPMTperLBbin; }
  void SetMinPMTperLBbin(Int_t mp=10) { fMinPMTperLBbin = mp; }
  void SetDynamicOccupancyCut(Bool_t flag = kFALSE){fDynamicOccupancyCut = flag;}
  void SetDynamicSigma(Float_t sigma = 3.){fDynamicSigma = sigma;}

  Int_t GetNSkip() { return fNSkip; }
  void SetNSkip(Int_t n) { fNSkip = n; }
  Int_t GetNStart() { return fNStart; }
  void SetNStart(Int_t n) { fNStart = n; }

  //--------------------
  // Main parameters from the array of parameters
  Float_t GetParameter(Int_t n) { return fmrqpars[n]; }
  Float_t Getd2o() { return fmrqpars[Getd2opari()]; }
  Float_t Getacrylic() { return fmrqpars[Getacrylicpari()]; }
  Float_t Geth2o() { return fmrqpars[Geth2opari()]; }

  Float_t Getangresp(Int_t n) { return fmrqpars[Getangresppari()+n]; }
  Float_t *Getangresp() { return &fmrqpars[Getangresppari()]; }
  Float_t Getangresp2(Int_t n) { return fmrqpars[Getangresp2pari()+n]; }
  Float_t *Getangresp2() { return &fmrqpars[Getangresp2pari()]; }

  Float_t Getlbmask(Int_t n) { return fmrqpars[Getlbmaskpari()+n]; }
  Float_t *Getlbmask() { return &fmrqpars[Getlbmaskpari()]; }
  Float_t Getlbdist(Int_t n) { return fmrqpars[Getlbdistpari()+n]; }
  Float_t *Getlbdist() { return &fmrqpars[Getlbdistpari()]; }
  Float_t Getlbdistwave(Int_t n) { return fmrqpars[Getlbdistwavepari()+n]; }
  Float_t *Getlbdistwave() { return &fmrqpars[Getlbdistwavepari()]; }

  Float_t Getnorm(Int_t n) { return fmrqpars[Getnormpari()+n]; }
  Float_t *Getnorm() { return &fmrqpars[Getnormpari()]; }

  Float_t Getrsd2o() { return fmrqpars[Getrsd2opari()]; }
  Float_t Getrsacrylic() { return fmrqpars[Getrsacrylicpari()]; }
  Float_t Getrsh2o() { return fmrqpars[Getrsh2opari()]; }

  //--------------------
  // Parameter "errors" from the covariance matrix diagonal elements
  // NB. Read Numerical Recipes to understand the way these can be interpreted!
  Float_t GetParError(Int_t n);
  Float_t Getd2oerr();
  Float_t Getacrylicerr();
  Float_t Geth2oerr();
  Float_t Getangresperr(Int_t n);
  Float_t Getangresp2err(Int_t n);
  Float_t Getlbmaskerr(Int_t n);
  Float_t Getlbdisterr(Int_t n);
  Float_t Getnormerr(Int_t n);
  Float_t Getrsd2oerr();
  Float_t Getrsacrylicerr();
  Float_t Getrsh2oerr();

  //--------------------
  // Computed indices for parameters in fmrqpars[]
  Int_t Getd2opari()        { return 1; }
  Int_t Getacrylicpari()    { return 2; }
  Int_t Geth2opari()        { return 3; }
  Int_t Getrsd2opari()      { return 4; }
  Int_t Getrsacrylicpari()  { return 5; }
  Int_t Getrsh2opari()      { return 6; }
  Int_t Getlbmaskpari();
  Int_t Getlbdistwavepari();
  Int_t Getangresppari();
  Int_t Getangresp2pari();
  Int_t Getlbdistpari();
  Int_t Getnormpari();
  

  //--------------------
  // Explicit setters
  void SetParameter(Int_t n,Float_t value);
  void Setd2o(Float_t l) { fmrqpars[Getd2opari()] = l; }
  void Setacrylic(Float_t l) { fmrqpars[Getacrylicpari()] = l; }
  void Seth2o(Float_t l) { fmrqpars[Geth2opari()] = l; }
  void Setangresp(Float_t *ar);
  void Setangresp2(Float_t *ar);
  void Setlbmask(Float_t *lbm);
  void Setlbdist(Float_t *lbd);
  void Setlbdistwave(Float_t *lbd);
  void Setlbamplitudes(Float_t *lbd);
  void Setlbphases(Float_t *lbd);
  void Setnorm(Float_t *n);
  void Setrsd2o(Float_t l) { fmrqpars[Getrsd2opari()] = l; }
  void Setrsacrylic(Float_t l) { fmrqpars[Getrsacrylicpari()] = l; }
  void Setrsh2o(Float_t l) { fmrqpars[Getrsh2opari()] = l; }

  //--------------------
  // rsd
  void SetRunsToDo(Int_t n, Int_t runs[]);
  void SetRunsToDoIndex();
  Int_t GetRunsToDoIndex(Int_t i);
  Int_t* GetRunsToDoIndex();
  virtual void InitParameters(Int_t numberofruns, Int_t *runstobedone);

  //--------------------
  // Pmt Grouping
  void SetPmtCoordinateCode(Int_t aCode = 3){ fPmtCoordCode = aCode; }
  Int_t GetPmtCoordinateCode(){ return fPmtCoordCode; }
  void SetPmtCoordinateLo(Float_t aValue = 0.){ fPmtCoordCutLo = aValue; }
  Float_t GetPmtCoordinateLo(){ return fPmtCoordCutLo; }
  void SetPmtCoordinateHi(Float_t aValue = 0.){ fPmtCoordCutHi = aValue; }
  Float_t GetPmtCoordinateHi(){ return fPmtCoordCutHi; }
  void SetPmtCoordinates(Float_t aValueLo,Float_t aValueHi){ fPmtCoordCutLo = aValueLo; fPmtCoordCutHi = aValueHi;}
  Int_t PmtGroup(QOCAPmt *pmt);
  Int_t PmtGroup(Int_t pmtn);
  void PmtGroupCovar();

  ClassDef(QOCAFit,17) // OCA Fitter - Occupancy and OccRatio Methods
};

#endif
