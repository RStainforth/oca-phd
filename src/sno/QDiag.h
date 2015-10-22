#ifndef __QDiag_h__
#define __QDiag_h__

#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TDatime.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TAxis.h"

#include "QGlobals.h"
#include "QPMT.h"
#include "QTree.h"
#include "QOCAFit.h"

#define NRUNS 80
#define NPMS 200

class QDiag : public TObject {

 public:
  QDiag();
  QDiag(char *dataset,int lambda,char *postype);
  QDiag(char *dataset,int lambda,char *postype,char *scanc,float tolerance,char *correction);
  virtual ~QDiag();

  void Initialize();
  void UpdateFileNames();
    
  void ProjectData();
  void ReadData();
  void Process();
  void FitRatio();
  void FitProduct();
  void FitWholeScan();
//  void CheckProduct();  // to do in a near future...
//  void HorizontalScan(); // to do in a near future...

  char*      GetDataDir()	      {return (char*) fDataDir->Data();}
  char*      GetHomeDir()	      {return (char*) fHomeDir->Data();}
  char*      GetDataSet()	      {return (char*) fDataSet->Data();}
  char*      GetPosType()	      {return (char*) fPosType->Data();}
  char*      GetScan()  	      {return (char*) fScan->Data();}
  char*      GetCorrectionType()  {return (char*) fCorrectionType->Data();}
  char*      GetTreeFileName()    {return (char*) fTreeFileName->Data();}
  char*      GetHistoFileName()   {return (char*) fHistoFileName->Data();}
  Int_t      GetLambda()	      {return fLambda;}		 
  Float_t    GetTolerance()	      {return fTolerance;} 	 
  Int_t      GetTotalRuns()	      {return fNruns;}		 
  Int_t      GetRun(Int_t i)	  {return fRun[i];}		 
  TVector3  &GetSource(Int_t i)   {return fSource[i];}    
  TH1F*      GetHcounts(Int_t i)  {return fHcounts[i];}   
  TH1F*      GetHocc(Int_t i)     {return fHocc[i];}   
  Float_t*    GetSumDistance() 	  {return fSumDistance;}
  Float_t*    GetSumRatio()	      {return fSumRatio;}
  Float_t*    GetSumProduct()     {return fSumProduct;}
  Float_t*    GetSumRerrors()     {return fSumRerrors;}
  Float_t*    GetSumPerrors()     {return fSumPerrors;}

  void SetDataDir(char *aDirectory);
  void SetHomeDir(char *aDirectory);
  void SetDataSet(char *aString);
  void SetLambda(int aNumber);
  void SetPosType(char *aString);
  void SetScan(char *aString);
  void SetCorrectionType(char *aString);
  void SetTolerance(Float_t aNumber);
 
 protected:  
  TString   *fDataDir;	        // Root for qpath, qocafit, and histo 
  TString   *fHomeDir;	        // Runlists, output gifs and asciis
  TString   *fDataSet;			// Name of the laserball scan (may02,...)
  TString   *fPosType;			// Type of position fit (fruns or fits)
  TString   *fScan;			    // Direction of the single scan (xzscan)
  TString   *fCorrectionType;	// "none","laser","pmt","all" 
  Int_t      fLambda;			// Wavelength (in nm)
  Float_t    fTolerance;  		// Max distance (cm) of PMTs to axis  
  Float_t    fInvAtt;			// Inverse attenuation length (cm ^-1)
  Float_t    fInvAttErr;		// its error
  
  Int_t      fNpmts;			// Number of PMTs within tolerance
  Int_t      fNpairs;			// Number of PMT pairs within tolerance
  Int_t      fNruns;  			// Runs in file (ProjectData) or in runlist (ReadData)
  Int_t      fRun[NRUNS];		// Array with all run numbers
  TVector3  fSource[NRUNS];	// Array of source position vectors for all runs
  Float_t    fNorm[NRUNS];		// Run normalization (from QOCARun)
  Float_t    fDistance[NPMS][NRUNS]; // Source-PMT distance	
  Float_t    fRatio[NPMS][NRUNS];	 // Corrected occupancy ratio
  Float_t    fProduct[NPMS][NRUNS];  // Normalized sqrt of product
  Float_t    fRerrors[NPMS][NRUNS];  // Errors for fRatio
  Float_t    fPerrors[NPMS][NRUNS];  // Errors for fProduct
  Float_t    fSumDistance[NRUNS];	 // Average fDistance for PMTs within tolerance
  Float_t    fSumRatio[NRUNS];		 // Sum of fRatio for PMTs within tolerance
  Float_t    fSumProduct[NRUNS];	 // Sum of fProduct for PMTs within tolerance
  Float_t    fSumRerrors[NRUNS];	 // Errors for fSumRatio
  Float_t    fSumPerrors[NRUNS];	 // Errors for fSumProduct

  TString   *fTreeFileName;  		 // Name of qpath file with data QOCATree
  TString   *fHistoFileName;   		 // Name of file with 1-D occupancy histos
  TFile     *fHistoFile;     		 // File with 1-D occupancy histos

  TH1F *fHcounts[NRUNS];  // Histograms with Prompt Counts, for Stat. Err.
  TH1F *fHocc[NRUNS];   // Histograms with Solid Angle corrected occupancy
 
  ClassDef(QDiag,0)  //QDiag Base Class

};


#endif // not __QDiag_h__

