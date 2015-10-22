#ifndef _QLaserballMC_
#define _QLaserballMC_

//*-- Author : Olivier Simard
//*-- Copyright (C) 2006 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QLaserballMC                                                         //
//                                                                      //
// Helping methods for laserball MC.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TVector3.h"
#include "TFile.h"
#include "QOCATree.h"
#include "QOCARun.h"
#include "TRandom.h"
#include "TH2F.h"
#include "TKey.h"

class QLaserballMC;

class QLaserballMC : public TObject {

 private:

  //characters: filenames, directories, and others.
  Char_t* fSetupFile;       // Setup file required to load SNOMAN/QSNO
  Char_t* fBatFile;         // Basic file to run SNOMAN.
  Char_t* fDQXXdir;         // DQXX file repository.
  Char_t* fWdir;            // Working directory
  Char_t* fIdir;            // Input directory
  Char_t* fCmdFile;         // Command file for SNOMAN to execute.
  Char_t* fPhase;           // Phase name for load_xxx_settings.cmd
  Char_t* fPmtVariationFile;// Pmt variation data
  Char_t* fLaserDistFile;   // Laserball distribution data
  Char_t* fLaserDyesFile;   // Laser system dye data
  Char_t* fPmtResponseFile; // Pmt response data
  Char_t* fMediaFile;       // D2O/Acrylic/H2O data
  Char_t* fRchFile;         // Output rch file
  Char_t* fRdtFile;         // Output rdt file
  Char_t* fLogFile;         // SNOMAN output
  Char_t* fSeedFile;        // Seed file for book-keeping
  Char_t* fEnergy;          // Energy as a character string

  // numerical values:
  Int_t fRun;               // Run number integer
  TVector3 fPosition;       // Source center
  Int_t fSourceModel;       // Source geometry code in SNOMAN (point, sphere, disk, etc.)
  Float_t fSourceRadius;    // Source size in cm
  Int_t fNevents;           // Number of events to generate
  Int_t fNeventlimit;       // Maximum number of event per process
  Int_t fNsubruns;          // Number of subruns (if fNevents > fNeventlimit)
  Int_t fIntensity;         // Number of photons generated per event
  Float_t fSourcePhi;       // Source phi orientation (wrt x-axis)
  Float_t fSourceTheta;     // Source theta orientation (wrt z-axis)
  Int_t fSourceSlot;        // Source orientation code (SWNE = 0123)
  Float_t fWlen;            // Monochromatic wavelength in nm
  Int_t fSeed1;             // Input seed1 for MC
  Int_t fSeed2;             // Input seed2 for MC
  Float_t fFactor;          // Multiplicative factor for the number of events
  // flags to turn on/off things:
  Bool_t fDQXX;             // Flag to use QDXX info or not
  Bool_t fTdiff;            // Flag to run tdiff or not (bat file level)
  Bool_t fPosFit;           // Flag to run qoca_positionfit or not (bat file level)
  Bool_t fNcd;              // Flag to include ncd array in geometry
  Bool_t fPmtvar;           // Flag to turn on/off the pmt variations
  Bool_t fPmtbounce;        // Flag to turn on/off grey disk pmt bounce
  Bool_t fPmt3dmodel;       // Flag to turn on/off 3dPMT model (or grey disk)
  Bool_t fRayleigh;         // Flag to turn on/off Rayleigh scattering model
  Bool_t fRsp;              // Flag to turn on/off rsp in SNOMAN
  Bool_t fFresnel;          // Flag to turn on/off Fresnel scattering
  Bool_t fPhotodis;         // Flag to turn on/off photodisintegration

  TRandom* fRandom;         // ROOT random number generator

 public:

  QLaserballMC();
  ~QLaserballMC();

  // Setters
  void SetSetupFile(Char_t* aFilename = NULL){fSetupFile = aFilename;}
  void SetBatFile(Char_t* aFilename = NULL){fBatFile = aFilename;}
  void SetCmdFile(Char_t* aFilename = NULL){fCmdFile = aFilename;}
  void SetPmtVariationFile(Char_t* aFilename = NULL){fPmtVariationFile = aFilename;}
  void SetLaserDistFile(Char_t* aFilename = NULL){fLaserDistFile = aFilename;}
  void SetLaserDyesFile(Char_t* aFilename = NULL){fLaserDyesFile = aFilename;}
  void SetPmtResponseFile(Char_t* aFilename = NULL){fPmtResponseFile = aFilename;}
  void SetMediaFile(Char_t* aFilename = NULL){fMediaFile = aFilename;}
  void SetRchFile(Char_t* aFilename = NULL){fRchFile = aFilename;}
  void SetRdtFile(Char_t* aFilename = NULL){fRdtFile = aFilename;}
  void SetLogFile(Char_t* aFilename = NULL){fLogFile = aFilename;}
  void SetSeedFile(Char_t* aFilename = NULL){fSeedFile = aFilename;}
  void SetDQXXDir(Char_t* aDirectory = NULL){fDQXXdir = aDirectory;}
  void SetWorkingDir(Char_t* aDirectory = NULL){fWdir = aDirectory;}
  void SetInputDir(Char_t* aDirectory = NULL){fIdir = aDirectory;}
  void SetPhase(Char_t* aSnoPhase = "d2o_2"){fPhase = aSnoPhase;}
  void SetSourceModel(Int_t aModel = 170){fSourceModel = aModel;}
  void SetSourceRadius(Float_t aRadius = 5.45){fSourceRadius = aRadius;}
  void SetNevents(Int_t aNumber = 50000){fNevents = aNumber;}
  void SetNeventLimit(Int_t aNumber = 25000){fNeventlimit = aNumber;}
  void SetNsubruns(Int_t aNumber = 2){fNsubruns = aNumber;}
  void SetIntensity(Int_t aNumber = 2500){fIntensity = aNumber;}
  void SetFactor(Float_t aNumber = 1.){fFactor = aNumber;}
  void SetSourceSlot(Int_t aNumber = 0){fSourceSlot = aNumber;}
  void SetSourcePhi(Float_t aNumber = 0.){fSourcePhi = aNumber;}
  void SetSourceTheta(Float_t aNumber = 0.){fSourceTheta = aNumber;}
  void SetPosition(TVector3 pos){fPosition = pos;}
  void SetDQXXFlag(Bool_t aFlag = kTRUE){fDQXX = aFlag;}
  void SetTdiffFlag(Bool_t aFlag = kFALSE){fTdiff = aFlag;}
  void SetPositionFitFlag(Bool_t aFlag = kFALSE){fPosFit = aFlag;}
  void SetNcdFlag(Bool_t aFlag = kFALSE){fNcd = aFlag;}
  void SetPmtVariationFlag(Bool_t aFlag = kFALSE){fPmtvar = aFlag;}
  void Set3DPmtModelFlag(Bool_t aFlag = kFALSE){fPmt3dmodel = aFlag;}
  void SetPmtBounce(Bool_t aFlag = kFALSE){fPmtbounce = aFlag;}
  void SetRayleighScatteringFlag(Bool_t aFlag = kTRUE){fRayleigh = aFlag;}
  void SetRspFlag(Bool_t aFlag = kTRUE){fRsp = aFlag;}
  void SetFresnelScatteringFlag(Bool_t aFlag = kTRUE){fFresnel = aFlag;}
  void SetPhotodisintegrationFlag(Bool_t aFlag = kTRUE){fPhotodis = aFlag;}

  // Others
  void ProduceMCScan(Char_t* aFileType, Char_t* aScan, Char_t* aKind = "fruns", Int_t aWlen = 420, Char_t* aTree = "optics");
  void WriteBatFile();
  void WriteCommandFile();
  void LoadRunInfo(QOCARun* ocarun);
  void WriteSplitCommandFiles(Int_t aNumber = 1);
  void ReconstructRch(Int_t aRunNumber);
  void ReconstructRch(Char_t* aString);
  void SetSeeds(Int_t aRunNumber = 0);
  Double_t LambdaToEnergy(Float_t aLambda = 0.);

  ClassDef(QLaserballMC,0)    // Laserball MC Class

};

#endif
