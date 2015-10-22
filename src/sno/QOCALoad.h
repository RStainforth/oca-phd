#ifndef _QOCALOAD_
#define _QOCALOAD_

//*-- Author : Olivier Simard
//*-- Copyright (C) 2003 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCALoad                                                             //
//                                                                      //
// Methods to load (optics) configuration files at run time             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class QOCALoad;

class QOCALoad : public TObject {

 private:

  Int_t fDimension;			// dimensions (line*column) of current file
  Int_t fNline;				// number of lines in current file
  Int_t fNcolumn;			// number of columns in current file
  Char_t* fDirectory;			// directory to load data from
  Char_t* fFilename;			// file name to load data from (local)
  Char_t* fFullname;			// full path file name to load data from
  Char_t* fKeyword;			// keyword (single keyword case)
  Int_t fKeyworddim;			// how many keywords (multiple keyword case)
  Char_t** fListofkeywords;		// multiple keyword storage array
  Int_t* fListofdimensions;
  Char_t* fDir_qruns;			// directory for runlists
  Char_t* fDir_input;			// directory for qocatree inputs
  Char_t* fDir_output;			// directory where resulting files are written
  Char_t* fStep; 			// step keyword
  Char_t* fWlen; 			// wavelength
  Char_t* fMeth; 			// position fit method keyword
  Char_t* fScan; 			// standard scan identifier (month+year)
  Char_t* fM; 				// used to change the scan identifier (miniscans and others)
  Char_t** fFilelist;			// file list produced for QPath
  Char_t* fRunlist;			// runlist used for analysis
  Int_t fNrun;				// number of runs
  Int_t* fRuns;

 public:

  QOCALoad( Char_t* directory = NULL,  Char_t* filename = NULL);
  ~QOCALoad();

  void Initialize();
  void Close();

  // Setters
  // ----------------
  void SetDimension(Int_t dimension = 0){fDimension = dimension;}
  void SetNColumns(Int_t ncols = 0){fNcolumn = ncols;}
  void SetNLines(Int_t nlines = 0){fNline = nlines;}
  void SetFullname(Char_t* directory, Char_t* filename);
  void SetFullname(Char_t* fullname);
  void SetDirectory(Char_t* directory = NULL){fDirectory = directory;}
  void SetFilename(Char_t* filename = NULL){fFilename = filename;}
  void SetKeyword(Char_t* keyword = NULL){fKeyword = keyword;}
  void SetListofKeywords(Char_t** list  = NULL){fListofkeywords = list;}
  void SetArrayofDimensions();
  void SetDirRunList(Char_t* dirq = NULL){fDir_qruns = dirq;}
  void SetDirIn(Char_t* dirin = NULL){fDir_input = dirin;}
  void SetDirOut(Char_t* dirout = NULL){fDir_output = dirout;}
  void SetStep(Char_t* s = NULL){fStep = s;}
  void SetScan(Char_t* s = NULL){fScan = s;}
  void SetMeth(Char_t* m = NULL){fMeth = m;}
  void SetWlen(Char_t* w = NULL){fWlen = w;}
  void SetM(Char_t* m = NULL){fM = m;}
  void SetNrun(Int_t nrun = 0){fNrun = nrun;}
  void SetRunsArray(Int_t* runsarray = NULL){fRuns = runsarray;}

  // Getters
  // ----------------
  Int_t GetDimensionFast(){return fDimension;}
  Int_t GetDimension();
  Int_t GetDimension(Char_t*);
  Int_t GetDimension(Char_t*,  Char_t*);
  Int_t GetNColumns(){return fNcolumn;}
  Int_t GetNLines(){return fNline;}
  Char_t* GetFullname(){return fFullname;}
  Char_t* GetDirectory(){return fDirectory;}
  Char_t* GetFilename(){return fFilename;}
  Char_t* GetKeyword(){return fKeyword;}
  Char_t** GetListofKeywords() {return fListofkeywords;}
  Int_t* GetArrayofDimensions(){return fListofdimensions;}
  Char_t* GetDirRunList(){return fDir_qruns;}
  Char_t* GetDirIn(){return fDir_input;}
  Char_t* GetDirOut(){return fDir_output;}
  Char_t* GetStep(){return fStep;}
  Char_t* GetScan(){return fScan;}
  Char_t* GetMeth(){return fMeth;}
  Char_t* GetWlen(){return fWlen;}
  Char_t* GetM(){return fM;}
  Char_t** GetFileList(){return fFilelist;}
  Char_t* GetRunList(){return fRunlist;}
  Int_t GetNrun(){return fNrun;}
  Int_t* GetRunsArray(){return fRuns;}
  Int_t* GetFitFlags();
  Int_t* GetRuns();
  Int_t* GetFruns();
  Int_t* GetCruns();

  // Others
  // ----------------
  void GetDimensions();
  Char_t** CreateArray();
  Char_t** CreateArray(Char_t*);
  Char_t** CreateArray(Char_t*, Char_t*);
  Char_t*** CreateBigArray();
  Char_t*** CreateBigArray(Char_t*);
  Char_t*** CreateBigArray(Char_t*, Char_t*);
  void CreateListofKeywords();
  Char_t*** LoadOptions(Bool_t);
  Int_t* ConverttoIntegers(Char_t**, Int_t dim = 0);
  Float_t* ConverttoFloats(Char_t**, Int_t dim = 0);
  Double_t* ConverttoDoubles(Char_t**, Int_t dim = 0);
  void DisplayVectorContent(Char_t**);
  void DisplayMatrixContent(Char_t**);
  void ResetNColumns(){SetNColumns();return;}
  void ResetNLines(){SetNLines();return;}
  void ResetNBoth(){SetNLines(); SetNColumns(); return;}
  void AddaColumn(){fNcolumn++;}
  void AddaLine(){fNline++;}
  Int_t* GetLine(Int_t*, Int_t);
  Float_t* GetLine(Float_t*, Int_t);
  Char_t** GetLine(Char_t**, Int_t);
  Int_t* GetColumn(Int_t*, Int_t);
  Float_t* GetColumn(Float_t*, Int_t);
  Char_t** GetColumn(Char_t**, Int_t);
  Char_t* InsertElement(Char_t*, Char_t*, const Char_t*);
  Char_t* AppendElement(const Char_t*, Char_t*, const Char_t*);
  Char_t* AppendElement(const Char_t*, const Char_t*);
  Char_t** InsertElements(Char_t**, Char_t**, Int_t, Int_t, Char_t*);
  void Presentator(Char_t*);
  void VerifyInputs();
  Bool_t SearchforKeyword(Char_t* method = NULL);
  Bool_t CheckKeywords();
  Char_t** LoadFileList();
  Char_t** FilesforPositions();
  Char_t** FilesforTrees();
  Char_t** FilesforFits();
  void LoadRunList(Char_t* runlist = NULL);
  void ProduceRunLists(Char_t* scan = NULL,Int_t* cruns = NULL);

  ClassDef(QOCALoad,0)         // Option Loading Methods for OCA
};

#endif
