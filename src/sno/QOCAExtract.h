// QOCAExtract.h

#ifndef _QOCAEXTRACT_
#define _QOCAEXTRACT_

#include <QOCAAnalysis.h>

class QOCAExtract;

class QOCAExtract : public QOCAAnalysis {

 private:

  Float_t* fErrfact;  //systematic multiplicative factors
  Char_t** fErrnames; //associated subtree names
  Int_t fDim_errors;  //dimension of the errors file

  Int_t fNbininc,fNbinocc,fNbineff,fNbinpmt;	// number of bins for histograms
  Float_t fMininc,fMinocc,fMineff,fMinpmt;	// minimum bin value
  Float_t fMaxinc,fMaxocc,fMaxeff,fMaxpmt;	// maximum bin value

  Bool_t fDisplay;

  QOCAFit* fQOCAFit;
  QOCALoad* fQOCALoad;

 public:

  QOCAExtract(Char_t* optionfile = NULL, Bool_t mode = kTRUE, Bool_t display = kTRUE,
  		Char_t* rootdirectory = NULL, Char_t* exdirectory = NULL);
  ~QOCAExtract();

  // Histogram producers
  TH1F* GetValues(TFile&, Char_t*, Int_t, Char_t*);

  // Extraction functions
  // ----------------
  void ExtractEfficiencies(Float_t,TH1F* mask = NULL);
  void ExtractEfficiencies(TH1F* mask = NULL);
  void FindAttenuation(QOCAFit*);
  void ExtractAttenuation(Char_t*);
  void ExtractAttenuation();
  void ExtractAngularResponse();
  void ExtractAngularResponse2();
  void ExtractLBdist();
  void ExtractLBpoly();
  void ExtractChiSquare();
  void ExtractAttAverages(Char_t*);
  void ExtractAttAverages();
  void ExtractPmtrAverages();
  void ExtractTimeWindows(Char_t*);
  void ExtractTimeWindows();
  

  // PMT Efficiencies Manipulation
  Float_t* LoadEfficiencies(Char_t* filename, Float_t lambda);
  void UpdateEfficiencies(Char_t* filename, Char_t* treename = "optix");

  // Setters
  // ----------------
  void SetExtractDisplay(Bool_t d = kFALSE){fDisplay = d;}
  void SetIncBins(Int_t nbins = 50, Float_t min = 0, Float_t max = 50)
    {fNbininc = nbins; fMininc = min; fMaxinc = max;}
  void SetOccBins(Int_t nbins = 1000, Float_t min = 0, Float_t max = 5e+03)
    {fNbinocc = nbins; fMinocc = min; fMaxocc = max;}
  void SetEffBins(Int_t nbins = 100, Float_t min = 0, Float_t max = 5)
    {fNbineff = nbins; fMineff = min; fMaxeff = max;}
  void SetPmtBins(Int_t nbins = 10000, Float_t min = 1, Float_t max = 10000)
    {fNbinpmt = nbins; fMinpmt = min; fMaxpmt = max;}

  // Getters
  // ----------------
  Bool_t GetExtractDisplay(){return fDisplay;}
  Int_t GetNbinInc(){return fNbininc;}
  Int_t GetNbinOcc(){return fNbinocc;}
  Int_t GetNbinEff(){return fNbineff;}
  Int_t GetNbinPmt(){return fNbinpmt;}
  Float_t GetMinbinInc(){return fMininc;}
  Float_t GetMinbinOcc(){return fMinocc;}
  Float_t GetMinbinEff(){return fMineff;}
  Float_t GetMinbinPmt(){return fMinpmt;}
  Float_t GetMaxbinInc(){return fMaxinc;}
  Float_t GetMaxbinOcc(){return fMaxocc;}
  Float_t GetMaxbinEff(){return fMaxeff;}
  Float_t GetMaxbinPmt(){return fMaxpmt;}

  // Others
  // ----------------
  void ChooseKind(Int_t);

  ClassDef(QOCAExtract,0) // Optical Parameter Extraction Class
};

#endif
