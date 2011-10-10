#ifndef QUEENS_QPath
#define QUEENS_QPath

//*-- Author : Bryce Moffat
//*-- Copyright (C) 2000 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QPath                                                                //
//                                                                      //
// SNO optics application algorithms: applies QOptics paths and         //
// laserball masks to data from .rdt (root delta-time) files or         //
// QOCATree data structures.                                            //
//                                                                      //
// 27-Oct-2000 - Moffat                                                 //
// Routines to fit source position incorporated into QPath class        //
//                                                                      //
// 17-Dec-2000 - Moffat                                                 //
// Variables and code to study position-related systematics             //
//                                                                      //
// 05.2006 - O.Simard                                                   //
// Cleaned up a bit.                                                    //
// Moved Laserball specific stuff to QLaserballUtil class               //
// accessible via the fQLaserballUtil pointer.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TVector3.h>
#include <TString.h>
#include "QMath.h"

#include <QPMTxyz.h>
#include "QOptics.h"
#include "QOCATree.h"
#include "QOCARun.h"
#include "QOCAPmt.h"
#include "QRdt.h"
#include "QLaserballUtil.h"

// ------------------------------------------------------------------------
//  Path class for optics definition
// ------------------------------------------------------------------------

class QPath;

class QPath: public TObject, QMath {
  
 private:
  Int_t fNumberofruns;      // Number of runs
  Int_t *fRunlist;          // Array of run numbers to process in tree
  Int_t *fRunpasslist;      // Array of runpass numbers to process in tree
  Int_t *fCentrerunlist;    // Array of centre run numbers for each entry in fRunlist[]
  Int_t *fCentrerunindex;   // Array of centre run indices: one per entry in fRunlist[]
  Int_t *fRunposlist;       // Array of run numbers for obtaining positions
  TVector3 *fRunpositions;  // Array of run positions

  QOCARun **fCentrerunptr;  // Array of ptrs to **distinct** central runs

  Int_t fNumberofcentre;    // Number of **distinct** centre runs
  Int_t *fCentreruns;       // Array of **distinct** central run numbers

  Int_t fNCentrerunPmt;		// Number of pmts in the centre run
  Int_t *fCentrerunPmtIndex;	// Array of pmt indices of the centre run

  Float_t fDefaultlambda;   // Default lambda to use if no CAST information
  Float_t fLambda;          // Lambda used to find the efficiencies in titles files (fPmtvarfile)

  TString fRdtdir;          // Directory where .rdt files are stored
  TString fDqxxdir;         // Directory where dqxx files are stored
  TString fNcdPosfile;      // File (with full path) with rsps chcs information
  TString fPmtvarfile;      // File (with full path) with pmt efficiency variation

  Float_t* fPmteff;                 // Temporary storage for pmt efficiency
  Bool_t fEfficienciesLoaded;       // Flag indicating efficiencies have been loaded
  Bool_t fNcdReflectionDone;        // Flag indicating ncd reflections have been checked
  Double_t fNcdReflectivityScale;   // Used to scale NCD reflectivity in QOptics
  Bool_t fReMask;                   // Flag indicating if Bad masks need to be re-applied
  Bool_t fSingleEfficiency;         // Flag to enable single efficiency for all PMTs (!= 1.0)

  TVector *fRSPS;                   //Holds the RSPS vector for the current run

  QOptics *fQOptics;                // QOptics path calculator
  QLaserballUtil* fQLaserballUtil;  // QLaserballUtil access pointer

  //--------------------
  // Laserball position systematics control
  Int_t fPositionsyst;  // Position Systematics: 0=none, -1=smear, +1=shift, +2=multiply
  Float_t fXsyst;       // X position of laserball shift or smear amount
  Float_t fYsyst;       // Y position of laserball shift or smear amount
  Float_t fZsyst;       // Z position of laserball shift or smear amount
  Float_t fRsyst;       // Radial position of laserball shift or smear amount
  Float_t fBallsyst;    // Ball-pmt position shift or smear amount - for solid angle.

  //--------------------
  // mrqmin variables for fitting laserball position
  QOptics *fQOpticsX;       // QOptics path calculator for X derivative in fit
  QOptics *fQOpticsY;       // QOptics path calculator for Y derivative in fit
  QOptics *fQOpticsZ;       // QOptics path calculator for Z derivative in fit

  Int_t  fFitLBPosition;      // Type of fit (0=none, 1=Direct, 2=direct and QOptics)
  Bool_t fFitLBPositionSetup; // Flag whether fmrqXXX arrays ready for fit

  QOCAPmt *fCurrentpmt;  // PMT being calculated in QPath::FillFromRDT()
  QOCARun *fCurrentrun;  // Run to be fit for position
  Int_t fCurrentrunIndex; // Run index associated with fCurrentrun

  Float_t fVrch;         // Light speed used in rch calculation
  Float_t fVgroupd2o;    // Group light velocity for d2o
  Float_t fVgroupacr;    // Group light velocity for acrylic
  Float_t fVgrouph2o;    // Group light velocity for h2o
  Float_t fVgroupmean;   // Mean group velocity (weighted for central position)

  Float_t fTimeSigma;    // PMT timing sigma (1.6ns)
  Float_t fChisquare;    // Chi-square of the position fit
  Float_t fdelpos;       // Delta source position for derivatives in fit
  std::vector<int> fTimeNbins[10000]; //matrix with n of bins used in time peak search
  std::vector<bool> fTimeNbinsLoaded; //flag 1: loaded nbins from rdt 0:no

  Int_t fnpmts;          // Number of PMT's in the fit/run
  Float_t *fmrqx;        // Index into array of PMT logical channel numbers
  Float_t *fmrqy;        // T_i + D_direct / V_light_rdt for each PMT
  Float_t *fmrqsig;      // Error on each PMT's timing (constant = 1.6ns ?)
  Float_t *fmrqpars;     // Parameters for model
  Int_t   *fmrqvary;     // Flag for variable parameters in mrqmin
  Float_t **fmrqcovar;   // Covariance matrix
  Float_t **fmrqalpha;   // Curvature matrix (?)

  Int_t fPrint;    // Print level: 0 no messages, 1 occasional info, 2 lots, 3 extreme
  Int_t fNcalls;   // Number of calls to mrqfuncs() - used as a diagnostic
  Int_t fNmessage; // Number of calls between message prints - usually 10% of fNpmts
  
  Int_t fNelements;      // Number of elements in fResarray[] and fChiarray[]
  Float_t *fResarray;    // Array of residuals for mrqcof() calls
  Float_t *fChiarray;    // Array of chisquared for mrqcof() calls

  Float_t fTimewinPavr;  // Set to 8.0 ns or higher
  Int_t fRdtfilestyle;   // Rdt file style (see QRdt.cxx)

  Float_t fTimeSigmaNSigma;   // Number of standard deviation to use for PMT time cut

  Bool_t fForceDqxx;     // Flag to force checking dqxx files (1:yes and abort if dqxx not found, 0:no)

 public:
  QPath();
  ~QPath();

  // --------------------
  // Main optical data processing/storage functions
  //
  // --
  // Filling data from rdt files
  QOCATree *FillFromRDT(const Char_t *rdtname = "optics_qpath",
			const Char_t *rdttitle = "QOCATree from .rdt");
  QOCATree *FillFromRDT(QRdt *qrdt, 
			const Char_t *rdtname = "optics_qpath",
			const Char_t *rdttitle = "QOCATree from .rdt");
  Bool_t FillRunFromRDT(QRdt *qrdt, QOCARun *ocarun);
  Int_t FillPmtFromRDT(QRdt *qrdt, QOCAPmt *ocapmt, const Int_t pmtn = -1);
  // --
  // Get source positions or fit source positions
  QOCATree *CalculatePositions(QOCATree *ocatree,
			const Char_t *posname = "poptics_qpath",
			const Char_t *postitle = "QPath position fit QOCATree");
  Int_t GetLBPositions(QOCATree *postree);
  Int_t FitLBPosition();
  void FitLBPosition(QOCARun *ocaruncalc, Int_t fitmethod = 0);
  Int_t FitLBPositionSetup(QOCARun *run = NULL);
  TVector3 *GetRunpositions(){return fRunpositions;}
  Bool_t GetFitLBPosition() { return fFitLBPosition; }
  void SetFitLBPosition(Int_t f);
  // --
  // Calculate optical paths with QOptics
  QOCATree *CalculatePaths(QOCATree *ocatree,
			const Char_t *pathname = "poptix_qpath",
			const Char_t *pathtitle = "QPath path calculated QOCATree");
  // --
  // Occupancy ratio to normalization run
  QOCATree *CalculateOccratio(QOCATree *ocatree,
			const Char_t *orname = "optix_qpath",
			const Char_t *ortitle = "QPath processed QOCATree");
  QOCARun *CalculateOccratioRun(QOCARun *orun, QOCARun *crun);
  QOCAPmt *CalculateOccratioPMT(QOCAPmt *opmt, QOCAPmt *cpmt, Double_t cnorm);
  Bool_t SetCentreRuns(QOCATree *ocatree);
  void SetNCentrerunPmt(Int_t ncpmt){fNCentrerunPmt = ncpmt;}
  Int_t  GetNCentrerunPmt(){return fNCentrerunPmt;}
  void SetCentrerunPmtIndex(QOCARun *crun);
  Int_t  GetCentrerunPmtIndex(Int_t pmtn, Int_t lastpmtindex);
  QOCARun **GetCentreRunPtr(){return fCentrerunptr;}

  // --------------------
  // QRdt related functions
  Bool_t IsInvalidQRdt(QRdt *qrdt);
  TString MakeDqxxName(const Int_t runnumber);
  TString MakeDqxxName(const Int_t runnumber, const TString stringcase);
  
  // --------------------
  // Input file settings
  Int_t GetRdtFileStyle(){return fRdtfilestyle;}
  void SetRdtFileStyle(Int_t aStyle = 0){fRdtfilestyle = aStyle;}
  void SetRdtdir(TString aString = "");
  void SetDqxxdir(TString aString = "");
  void SetNcdPositionfile(TString aString = "");
  void SetPmtvarfile(TString aString = "");
  TString GetRdtdir(){return fRdtdir;}
  TString GetDqxxdir(){return fDqxxdir;}
  TString GetNcdPositionfile(){return fNcdPosfile;}
  TString GetPmtvarfile(){return fPmtvarfile;}
  void LoadPmteffTitles();
  Bool_t GetFlagEfficienciesLoaded(){return fEfficienciesLoaded;}
  void SetFlagEfficienciesLoaded(Bool_t aFlag = kFALSE){fEfficienciesLoaded = aFlag;}
  Double_t GetSingleEfficiencyValue(){return fSingleEfficiency;}
  void SetSingleEfficiencyValue(Double_t aValue = 0.99);
  void ScaleEfficiencies(Float_t aValue = 1.);
  Float_t* GetEfficienciesLoaded(){return fPmteff;}
  // --------------------
  // Run lists
  void SetRunList(Int_t n, Int_t *runs, Int_t *cruns, Int_t *fruns=NULL,
  				  Int_t *runpass=NULL);
  void SetRunList(Int_t n, Int_t *runs, Int_t crun=0, Int_t *fruns=NULL,
  				  Int_t *runpass=NULL);
  Int_t GetNumberOfRuns(){return fNumberofruns;}
  Int_t GetNumberOfCentreRuns(){return fNumberofcentre;}
  Int_t *GetRunList(){return fRunlist;}
  Int_t *GetRunPassList(){return fRunpasslist;}
  Int_t *GetCentreRuns(){return fCentreruns;}
  Int_t *GetCentreRunList(){return fCentrerunlist;}
  Int_t *GetCentreRunIndex(){return fCentrerunindex;}
  Int_t *GetRunposlist(){return fRunposlist;}

  // --------------------
  // Flag, parameter, and data selection control
  TVector *GetRSPS(){return fRSPS;}
  Int_t GetFlagNcd(){return fQOptics->IsNcdOptics();}
  void  SetFlagNcd(Int_t aFlag = 0){fQOptics->SetNcdOptics((Bool_t)aFlag);}
  Double_t GetNcdTolerance(){return fQOptics->GetNcdTolerance();}
  void  SetNcdTolerance(Double_t tol = 5.0){fQOptics->SetNcdTolerance(tol);}
  Bool_t GetFlagNcdReflectionDone(){return fNcdReflectionDone;}
  void SetFlagNcdReflectionDone(Bool_t aFlag = kFALSE){fNcdReflectionDone = aFlag;}
  Float_t GetTimeWindowPavr(){return fTimewinPavr;}
  void  SetTimeWindowPavr(Float_t tw){fTimewinPavr = tw;}
  Double_t GetNcdReflectivityScale(){return fNcdReflectivityScale;}
  void SetNcdReflectivityScale(Double_t aScale = 1.0){fNcdReflectivityScale = aScale;}
  Bool_t GetReMaskFlag(){return fReMask;}
  void SetReMaskFlag(Bool_t aFlag = kTRUE){fReMask = aFlag;}
  Bool_t GetForceDqxxFlag(){return fForceDqxx;}
  void SetForceDqxxFlag(Bool_t aFlag = kTRUE){fForceDqxx = aFlag;}
  Float_t GetNTimeSigmaCut(){return fTimeSigmaNSigma;}
  void SetNTimeSigmaCut(Float_t nsigma = 3.0){fTimeSigmaNSigma = nsigma;}

  // --------------------
  // QOptics settings
  void SetPSUPCentre(Double_t px=0,Double_t py=0,Double_t pz=0);
  void SetPSUPCentre(TVector3 pc);
  TVector3 GetPSUPCentre(){return fQOptics->GetPSUPCentre();}
  QOptics *GetQOptics(){return fQOptics;}
  void SetQOptics(QOptics *opt){fQOptics = opt;}
  Float_t GetDefaultLambda(){return fDefaultlambda;}
  void SetDefaultLambda(Float_t lambda){fDefaultlambda = lambda;}
  Float_t GetLambda(){return fLambda;}
  void SetLambda(Float_t lambda){fLambda = lambda;}

  // --------------------
  // Laserball position systematics control
  void SetPositionsyst(Int_t ps){ fPositionsyst = ps; }
  Int_t GetPositionsyst(){ return fPositionsyst; }
  void SetAllsyst(Float_t s=0);
  void SetXsyst(Float_t xs) { fXsyst = xs; }
  void SetYsyst(Float_t ys) { fYsyst = ys; }
  void SetZsyst(Float_t zs) { fZsyst = zs; }
  void SetRsyst(Float_t rs) { fRsyst = rs; }
  void SetBallsyst(Float_t bs) { fBallsyst = bs; }
  Float_t GetXsyst() { return fXsyst; }
  Float_t GetYsyst() { return fYsyst; }
  Float_t GetZsyst() { return fZsyst; }
  Float_t GetRsyst() { return fRsyst; }
  Float_t GetBallsyst() { return fBallsyst; }

  // --------------------
  // Laserball position fitting routines
  Float_t GetVrch() { return fVrch; }
  void SetVrch(Float_t v) { fVrch = v; }
  Float_t GetDelPos() { return fdelpos; }
  void SetDelPos(Float_t dp) { fdelpos = dp; }
  Int_t GetPrint() { return fPrint; }
  void SetPrint(Int_t n=1) { fPrint = n; }
  Int_t GetNcalls() { return fNcalls; }
  void SetNcalls(Int_t n=0) { fNcalls = n; }
  Int_t GetNmessage() { return fNmessage; }
  void SetNmessage(Int_t n=5000) { fNmessage = n; }
  Float_t GetParameter(Int_t n) { return fmrqpars[n]; }
  void SetParameter(Int_t n,Float_t value);
  Float_t GetFitX() { return fmrqpars[1]; }
  Float_t GetFitY() { return fmrqpars[2]; }
  Float_t GetFitZ() { return fmrqpars[3]; }
  Float_t GetFitT() { return fmrqpars[4]; }
  Float_t GetFitC() { return fmrqpars[5]; }
  TVector3 GetFitXYZ();

  // --------------------
  // QMath functions, modified to report on QPath variables
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

  ClassDef(QPath,0)    // Path class for optics
};

#endif
