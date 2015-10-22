//*-- Author : Olivier Simard
//*-- Copyright (C) 2004 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCAAnalysis                                                         //
//                                                                      //
// Methods to analyze optical data in a sensible way                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef _QOCAANALYSIS_
#define _QOCAANALYSIS_

//SNO includes
#include <QOCALoad.h>
#include <QPath.h>
#include <QOCAFit.h>
#include <QPMTxyz.h>
//root includes
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <TProfile.h>
#include <TSystem.h>
#include <TVector.h>
#include <TRandom.h>
#include <TEventList.h>
#include <TGClient.h>
#include <TApplication.h>
#include <TObjectTable.h>
#include <TFile.h>

class QOCAAnalysis;

class QOCAAnalysis : public TObject {

 private:

  static Bool_t fInit;		// kTRUE if class has been initialized once
  static Int_t fNinstances;	// number of instances

  Bool_t fMode; 		// running mode (0: analysis, 1: runners (qpath/qocafit))
  Bool_t fFast;			// flag to run faster (no systematics)
  Bool_t fDisplay; 		// messages display

  Char_t** fScans; 		// array of scan identifiers
  Float_t* fWlens;
  Float_t* fDates;
  Int_t* fLimits; 		// time drift fit time limits
  Float_t fFitstart;   		// linear fit variables
  Float_t fFitend;
  Char_t* fFitfunc; 		// function for time drift fit

  Int_t fNscan; 		// dimension of the scan list
  Int_t fNwlen; 		// dimension of the wlen list
  Int_t fNsyst; 		// dimension of the systematics list
  Bool_t fIsSyst;               // flag that identifies a systematic being treated as nominal
  Bool_t fFulloutput;           // flag that sets this and other dependen classes to analyze all systematics

  Int_t fSyst;			// systematic index identifier
  Char_t** fSystnames;		// container for systematic names
  Float_t* fSystfactors;	// systematic errors factors (<= 1.00)
  Bool_t fApply;		// flag for qocafit model-applied trees

  // extraction and plotting flags
  Char_t* fKind; 		// fits, fruns
  Char_t* fMedium; 		// D2O, ACR, H2O
  Float_t fAttenuation; 	// attenuation in selected medium
  Float_t fAttenuationErr;      // attenuation error in selected medium
  Int_t fFits; 			// fits type word
  Int_t fFruns; 		// fruns type word
  Int_t fEff;                   // efficiency type word
  Int_t fMed; 			// medium type word (0:D2O, 1:ACR, 2:H2O)

  Char_t* fIdir; 		// input directory
  Char_t* fOdir; 		// output directory

  Char_t* fIlabel, * fOlabel; 	// characterize the input/output file name
  Char_t* fIformat, * fOformat; // characterize the input/output file format

  QOCALoad* fQOCALoad; 		// loading class pointer
  Float_t fCardVersion;         // optics configuration file (cardfile) version

  Int_t fMasktype; 		// local flag for mask type (ncd, no ncd, etc.)
  Int_t fOcctype; 		// local flag for occupancy type

  Float_t falpha_acrylic;       // acrylic extinction length for RunQOCAFit
  TFile* fin;                   // Input TFile
  TFile* fout;                  // Output TFile

 public:

  // default mode : runners
  // print messages for that class
  QOCAAnalysis(Char_t* optionfile = NULL, Bool_t mode = kFALSE, Bool_t messages = kFALSE); 
  ~QOCAAnalysis();

  
  // Runners
  // ----------------
  void RunQPath(Int_t, Char_t**);
  void RunQOCAFit(Int_t, Char_t**);

  // Runners' functions
  // ----------------
  void PathSystematics(QOCATree*, QPath*, Float_t, Int_t, Char_t**);
  void ModelAppliedQOCATree(QOCAFit*, Char_t* name = "optica");
  void ModelAppliedLBDist(QOCAFit*);
  Float_t* SaveParameters(QOCAFit*);
  Float_t* ChiSquares(QOCAFit*, Int_t, Char_t**, Char_t**);
  void H2O_Acr(QOCAFit*, Float_t*, Int_t, Char_t**,
  	Char_t**);
  void TimeWindows(QOCAFit*, Float_t*, Int_t, Char_t**);
  void ResChiSquares(QOCAFit*, Float_t*, Int_t, Char_t**,
  	Char_t**);
  void LBDistribution(QOCAFit*, Float_t*, Int_t, Char_t**,
  	Char_t**, Int_t, Int_t*);
  void NoPmtpmtVar(QOCAFit*, Int_t, Char_t**, Char_t**,
  	Int_t, Int_t*);
  void FitSystematics(QOCATree*, QOCAFit*,
  	Int_t, Char_t**, Char_t**, Int_t, Int_t*, Int_t);
  void DoQOCAFit(QOCAFit*);

  // ----------------
  // Setters
  void SetMode(Bool_t mode = kFALSE){fMode = mode;}
  void SetFast(Bool_t fast = kFALSE){fFast = fast;}
  void SetMessages(Bool_t mes = kFALSE){fDisplay = mes;}
  void SetSystIndex(Int_t index = 0){fSyst = index;}
  void SetSystNames(Char_t** names){fSystnames = names;}
  void SetNsyst(Int_t dim = 0){fNsyst = dim;}
  void SetIsSyst(Bool_t aFlag = kFALSE){fIsSyst = aFlag;}
  void SetFullOutput(Bool_t aFlag = kFALSE){fFulloutput = aFlag;}
  void SetSystFactors(Float_t* fact){fSystfactors = fact;}
  void SetApply(Bool_t apply){fApply = apply;}
  void SetKind(Char_t* kind = NULL){fKind = kind;}
  void SetMedium(Char_t* medium = NULL){fMedium = medium;}
  void SetAttenuation(Float_t att = 0.0){fAttenuation = att;}
  void SetAttenuationErr(Float_t err = 0.0){fAttenuationErr = err;}
  void SetFitsFlag(Int_t fits = 0){fFits = fits;}
  void SetFrunsFlag(Int_t fruns = 0){fFruns = fruns;}
  void SetEffFlag(Int_t eff = 0){fEff = eff;}
  void SetMediumFlag(Int_t med = 0){fMed = med;}
  void SetFlags(Int_t fits = 0, Int_t fruns = 0, Int_t eff = 0, Int_t med = 0){
    fFits = fits; fFruns = fruns; fEff = eff; fMed = med;}
  void SetIDir(Char_t* dir = NULL){fIdir = dir;}
  void SetODir(Char_t* dir = NULL){fOdir = dir;}
  void SetMaskType(Int_t masktype = 0){fMasktype = masktype;}
  void SetOccType(Int_t occtype = 0){fOcctype = occtype;}
  void SetILabel(Char_t* label = NULL){fIlabel = label;}
  void SetOLabel(Char_t* label = NULL){fOlabel = label;}
  void SetIFormat(Char_t* format = NULL){fIformat = format;}
  void SetOFormat(Char_t* format = NULL){fOformat = format;}
  void SetNscan(Int_t dim = 0){fNscan = dim;}
  void SetScanArray(Char_t** scanarray){fScans = scanarray;}
  void SetNwlen(Int_t dim = 0){fNwlen = dim;}
  void SetWlens(Float_t* wlens){fWlens = wlens;}
  void SetJulianDates(Float_t* jdates){fDates = jdates;}
  void SetTimeLimits(Int_t* tdates){fLimits = tdates;}
  void SetFitStart(Float_t fitstart = 0.0){fFitstart = fitstart;}
  void SetFitEnd(Float_t fitend = 0.0){fFitend = fitend;}
  void SetFitFunction(Char_t* fitfunc = "pol0"){fFitfunc = fitfunc;}
  void SetInputFile(TFile* anInputFile = NULL){fin = anInputFile;}
  void SetOutputFile(TFile* anOutputFile = NULL){fout = anOutputFile;}

  // ----------------
  // Getters
  Int_t GetSystIndex(){return fSyst;}
  Char_t* GetSystName(){return fSystnames[fSyst];}
  Char_t* GetSystName(Int_t isyst){return fSystnames[isyst];}
  Int_t GetNsyst(){return fNsyst;}
  Bool_t GetIsSyst(){return fIsSyst;}
  Bool_t GetFullOutput(){return fFulloutput;}
  Float_t* GetSystFactors(){return fSystfactors;}
  Bool_t GetApply(){return fApply;}
  Char_t* GetKind(){return fKind;}
  Char_t* GetMedium(){return fMedium;}
  Float_t GetAttenuation(){return fAttenuation;}
  Float_t GetAttenuationErr(){return fAttenuationErr;}
  Int_t GetFitsFlag(){return fFits;}
  Int_t GetFrunsFlag(){return fFruns;}
  Int_t GetEffFlag(){return fEff;}
  Int_t GetMediumFlag(){return fMed;}
  Int_t GetFlagsSum(){return fFits + fFruns + fEff;}
  Char_t* GetIDir(){return fIdir;}
  Char_t* GetODir(){return fOdir;}
  Int_t GetMaskType(){return fMasktype;}
  Int_t GetOccType(){return fOcctype;}
  Char_t* GetILabel(){return fIlabel;}
  Char_t* GetOLabel(){return fOlabel;}
  Char_t* GetIFormat(){return fIformat;}
  Char_t* GetOFormat(){return fOformat;}
  Char_t* GetSystOFormat(Int_t isyst = 0,Char_t* oldformat = ".txt");
  Int_t GetNscan() {return fNscan;}
  Char_t** GetScanArray(){return fScans;}
  Int_t GetNwlen() {return fNwlen;}
  Float_t* GetWlens(){return fWlens;}
  Float_t* GetJulianDates(){return fDates;}
  Int_t* GetTimeLimits(){return fLimits;}
  Float_t GetFitStart(){return fFitstart;}
  Float_t GetFitEnd(){return fFitend;}
  Char_t* GetFitFunc(){return fFitfunc;}
  Float_t GetCardfileVersion(){return fCardVersion;}

  // ----------------
  // Others
  void ClearArrays();
  void ResetIO();
  Int_t FindRun(QOCATree* ocatree, Int_t aRunNumber);
  QOCATree* ReplacePositions(QOCATree* oldtree, Char_t* AnInputFile, Int_t aDefaultOption = 2);

  ClassDef(QOCAAnalysis,0) // QOCA Analysis class
};

#endif
