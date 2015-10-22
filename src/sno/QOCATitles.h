// QOCATitles.h

#ifndef _QOCATITLES_
#define _QOCATITLES_

#include <TROOT.h>
#include <TObject.h>
#include <TDatime.h>
#include <TH1.h>
#include <TF1.h>
#include <TProfile.h>
#include <TFile.h>
#include <QTitles.h>
#include <QBank.h>
#include <QOCAAnalysis.h>

class QOCATitles;

class QOCATitles : public QOCAAnalysis {

 private:

  Char_t* fTimestamp;		// time stamp

  Char_t* fScanTitles;		// Scan for single banks and command files
  Bool_t fFunny337;		// 337 sep00 lower attenuation
  Int_t fNextrapbin;		// number of bins to use in the extrapolation
  Int_t fNangle; 		// number of incident angle bin parameters
  Int_t fNlambda;		// number of lambda bin parameters
  Int_t fNdlam;			// number of wlens slots available in SNOMAN
  Int_t fNpol;			// degree of polynomial in attenuation drift
  Int_t fNpolsnoman;		// number of polynomial slots available in SNOMAN
  Int_t fNormbin;		// normalization bin

  Double_t* fRayleighd2oq;	// rayleigh scattering in d2o (qoptics)
  Double_t* fRayleighacrq;	// rayleigh scattering in acrylic (qoptics)
  Double_t* fRayleighh2oq;	// rayleigh scattering in h2o (qoptics)
  Double_t* fRayleighd2om;	// rayleigh scattering in d2o (measured)
  Double_t* fRayleighacrm;	// rayleigh scattering in acrylic (measured)
  Double_t* fRayleighh2om;	// rayleigh scattering in h2o (measured)
  Double_t fRayfracd2o; 	// amount of Rayleigh removed from the prompt peak
  Double_t fRayfracacr;
  Double_t fRayfrach2o;
  Double_t fRayscaled2o;        // scales the Rayleigh scattering in SNOMAN
  Double_t fRayscaleacr;
  Double_t fRayscaleh2o;
  
  Double_t* fLd2o; 		// attenuation lengths
  Double_t* fLacrLoaded;        // loaded from cardfile (should not be changed)
  Double_t* fLacr;
  Double_t* fLh2o;

  Float_t*** fPoly;		// Polynomials array

  Int_t fNphase;		// number of different phases

  Int_t fMedium_code_hist;	// SNOMAN media codes
  Int_t fMedium_code_d2o;
  Int_t fMedium_code_salt;

  Int_t fAngleparmin;		// incident angle min and max values
  Int_t fAngleparmax;
  Int_t fLambdaparmin;		// wavelength min and max values
  Int_t fLambdaparmax;

  TH1F* fQesnoman;		// pmt quantum efficiency vs wavelength
  TH2F* fPmtresp;		// response vs angle vs wavelength
  TH2F* fPmtrefl;		// reflectivity vs angle vs wavelength
  TH2F* fPmteff;		// pmt efficiencies vs wavelength
  TH2F** fLBdist;		// cos(theta) vs phi laserball distribution
  Double_t** fLBpoly;		// TF1* laserball mask function parameters

  QOCALoad* fQOCALoad;
  Bool_t fUsefit;		// flag that tells the program to read
  				// the parameters from the qocafit files

  Char_t* fFnampmtresp; 	// a snoman pmt_response.dat file
  Int_t fOffset;		// offset when accessing bank info
  Char_t* fRootfile;		// an external rootfile to save additional plots (no extension)
  Int_t fNrootfile;		// keep track of how many rootfiles are being written

 public:

   // constructor and destructor
   QOCATitles(Char_t* optionfile = NULL, Bool_t mode = kTRUE, Bool_t display = kTRUE, Char_t* edirectory = NULL, Char_t* tdirectory = NULL);
   ~QOCATitles();

  // -----------------------------
  // Setters
  void SetFunny(Bool_t funny = kFALSE){fFunny337 = funny;}
  void SetNextrapbin(Int_t n = 0){fNextrapbin = n;}
  void SetNang(Int_t n = 0){fNangle = n;}
  void SetNlam(Int_t n = 0){fNlambda = n;}
  void SetNdlam(Int_t n = 0){fNdlam = n;}
  void SetNpol(Int_t n = 0){fNpol = n;}
  void SetNormbin(Int_t n = 0){fNormbin = n;}
  void SetRayleighd2oq(Double_t* ray = NULL){fRayleighd2oq = ray;}
  void SetRayleighacrq(Double_t* ray = NULL){fRayleighacrq = ray;}
  void SetRayleighh2oq(Double_t* ray = NULL){fRayleighh2oq = ray;}
  void SetRayleighd2om(Double_t* ray = NULL){fRayleighd2om = ray;}
  void SetRayleighacrm(Double_t* ray = NULL){fRayleighacrm = ray;}
  void SetRayleighh2om(Double_t* ray = NULL){fRayleighh2om = ray;}
  void SetRayleighFracd2o(Double_t frac = 0.){fRayfracd2o = frac;}
  void SetRayleighFracacr(Double_t frac = 0.){fRayfracacr = frac;}
  void SetRayleighFrach2o(Double_t frac = 0.){fRayfrach2o = frac;}
  void SetRayleighScaled2o(Double_t scale = 1.){fRayscaled2o = scale;}
  void SetRayleighScaleacr(Double_t scale = 1.){fRayscaleacr = scale;}
  void SetRayleighScaleh2o(Double_t scale = 1.){fRayscaleh2o = scale;}
  void SetAttd2o(Double_t* ld){fLd2o = ld;}
  void SetAttacr(Double_t* la){fLacr = la;}
  void SetAttacrLoaded(Double_t* la){fLacrLoaded = la;}
  void SetAtth2o(Double_t* lh){fLh2o = lh;}
  void SetPoly(Float_t*** pol){fPoly = pol;}
  void SetNPhase(Int_t n = 0){fNphase = n;}
  void SetMediumCodehist(Int_t mc = -1){fMedium_code_hist = mc;}
  void SetMediumCoded2o(Int_t mc = -1){fMedium_code_d2o = mc;}
  void SetMediumCodesalt(Int_t mc = 200){fMedium_code_salt = mc;}
  void SetAngleParLimits(Int_t min = 0, Int_t max = 89){
    fAngleparmin = min, fAngleparmax = max;}
  void SetLambdaParLimits(Int_t min = 220, Int_t max = 710){
    fLambdaparmin = min, fLambdaparmax = max;}
  void SetUseFit(Bool_t use = kFALSE){fUsefit = use;}
  void SetFnamPmtResp(Char_t* file = NULL){fFnampmtresp = file;}
  void SetOffset(Int_t offset = 0){fOffset = offset;}
  void SetRootfile(Char_t* file = "temp"){fRootfile = file;}
  void SetQESnoman(TH1F* qes = NULL){fQesnoman = qes;}
  void SetPmtrTH2F(TH2F* pmtr = NULL){fPmtresp = pmtr;}
  void SetReflTH2F(TH2F* pmtr = NULL){fPmtrefl = pmtr;}
  void SetEfficiencies(TH2F* peff = NULL){fPmteff = peff;}
  void SetLBdist(TH2F** lbd = NULL){fLBdist = lbd;}
  void SetLBpoly(Double_t** lbp = NULL){fLBpoly = lbp;}

  // -----------------------------
  // Getters
  Bool_t GetFunny(){return fFunny337;}
  Int_t GetNextrapbin(){return fNextrapbin;}
  Int_t GetNang(){return fNangle;}
  Int_t GetNlam(){return fNlambda;}
  Int_t GetNdlam(){return fNdlam;}
  Int_t GetNpol(){return fNpol;}
  Int_t GetNormbin(){return fNormbin;}
  Double_t* GetRayleighd2oq(){return fRayleighd2oq;}
  Double_t* GetRayleighacrq(){return fRayleighacrq;}
  Double_t* GetRayleighh2oq(){return fRayleighh2oq;}
  Double_t* GetRayleighd2om(){return fRayleighd2om;}
  Double_t* GetRayleighacrm(){return fRayleighacrm;}
  Double_t* GetRayleighh2om(){return fRayleighh2om;}
  Double_t GetRayleighFracd2o(){return fRayfracd2o;}
  Double_t GetRayleighFracacr(){return fRayfracacr;}
  Double_t GetRayleighFrach2o(){return fRayfrach2o;}
  Double_t* GetAttd2o(){return fLd2o;}
  Double_t* GetAttacr(){return fLacr;}
  Double_t* GetAttacrLoaded(){return fLacrLoaded;}
  Double_t* GetAtth2o(){return fLh2o;}
  Float_t*** GetPoly(){return fPoly;}
  Int_t GetNPhase(){return fNphase;}
  Int_t GetMediumCodehist(){return fMedium_code_hist;}
  Int_t GetMediumCoded2o(){return fMedium_code_d2o;}
  Int_t GetMediumCodesalt(){return fMedium_code_salt;}
  Bool_t GetUseFit(){return fUsefit;}
  Char_t* GetFnamPmtResp(){return fFnampmtresp;}
  Int_t GetOffset(){return fOffset;}
  Char_t* GetRootfile(){return fRootfile;}
  TH1F* GetQESnoman(){return fQesnoman;}
  TH2F* GetPmtrTH2F(){return fPmtresp;}
  TH2F* GetReflTH2F(){return fPmtrefl;}
  TH2F* GetEfficiencies(){return fPmteff;}
  TH2F** GetLBdist(){return fLBdist;}
  Double_t** GetLBpoly(){return fLBpoly;}

  // -----------------------------
  // Others
  Float_t ValueFromBank(Int_t i, Int_t j, QBank* bank, Int_t offset);
  Bool_t FillFromBank(TH2F* hist, QBank* bank, Int_t offset);
  Bool_t FillFromNormal(TH1F* des, TH2F* src);
  void Diagnostic();
  void BinshiftTH1F(TH1F *hist, Float_t shift, Float_t keepval=1.0);
  Int_t FindLastBin(TH1F *hist, Float_t keepval=1.0);
  Float_t FindScale(TH1F *numer, TH1F *denom, Int_t nbadbins, Int_t lastbin);
  Float_t FindScale(TH1F *numer, TH1F *denom, Int_t nbadbin, Float_t lastx);
  void ExtrapolateTH1F(TH1F *start, TH1F *finish, Int_t edgebin);
  void ExtrapolateTH1F(TH1F *start, TH1F *finish, Float_t edgex);
  void OmnipolateTH2F(TH2F *hist, Int_t nseed, Float_t *xseed, TH1F **hseed);
  void XScaleTH2F(TH2F *hist, TH1F *hscale);

  // common data base header for titles banks
  void DatabaseHeader(FILE* file);

  // attenuation coefficients
  void LoadAttenuations();
  void MediaTitles();

  // attenuation drift
  void LoadPolyTerms(Char_t** files = NULL, Int_t nphase = 3);
  void DriftTitles();

  // pmtr
  void LoadPMTR(Char_t*);
  void PMTRespTitles();

  // laserball distribution
  void LoadLBdist();
  void LBdistTitles();

  // PMT efficiencies
  void LoadEfficiencies();
  void PMTeffvarTitles();

  
  ClassDef(QOCATitles,0) // QOCA Titles Files Generator Class
};

#endif
