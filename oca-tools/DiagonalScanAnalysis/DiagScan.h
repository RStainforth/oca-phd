////////////////////////////////////////////////////////////////////
///
/// FILENAME: DiagScan.h
///
/// BRIEF: A class to obtain the attenuation coefficient of
///        the inner AV medium.
///                
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     07/2016 : A.S.Inacio - First Version.
///
/// DETAIL: This class extracts the attenuation coefficient
///         of the medium inside the AV using the laserball
///         in diagonal positions. The coefficient is extracted
///         from the ratio of occupancies of two aligned PMTs
///         in oposite sides of the detector. It uses the solid
///         angle and Fresnel transmission coefficients, obtained
///         using LightPathCalculator, to correct the measured
///         occupancies and it uses ShadowingCalculator to decide
///         whether or not a PMT is shadowed and, if it is,
///         the PMT is then excluded from the analysis.
///
////////////////////////////////////////////////////////////////////

#ifndef __DiagScan_h__
#define __DiagScan_h__

#include "TFile.h"
#include "TROOT.h"
#include "TDatime.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TChain.h"
#include "TStyle.h"
#include "TMath.h"
#include "TMatrix.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVector3.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <new>
#include <sys/stat.h>

#include <RAT/DS/SOC.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/SOCPMT.hh>
#include <RAT/DU/SOCReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/PhysicsUtil.hh>

#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"

#define NRUNS 6
#define NPMS  500

using namespace std;

class DiagScan : public TObject {

  public:
    DiagScan();
    DiagScan( Int_t lambda, std::string &diagonal );
    DiagScan( Int_t lambda, std::string &diagonal, std::string &path );
    DiagScan( Int_t lambda,/* Float_t tolerance, Float_t shadowing, */std::string &diagonal );

    // juntar funcao para tolerancias e bool para correcçoes?


    virtual ~DiagScan(); //destructor 

    void Initialize();
    void ReadData();
    void Process();
    void FitRatio();
    void Product();

    void SetAngResponseCoefficient( Int_t aNumber );
    void SetLambda( Int_t aNumber );
    void SetPath( const std::string& path );
    void SetDiagonal( const std::string& diagonal );
    void SetDistanceCut( Float_t aNumber );
    void SetShadowingTolerance( Float_t aNumber );
  

    Float_t GetLBMaskCorrection( Int_t aLambda , Double_t aTheta );

    Float_t         GetAngResponseCoefficient()   {return fAngRespCoefficient;}
    Int_t           GetLambda()                   {return fLambda;}		 
    Float_t         GetDistanceCut()                {return fTolerance;}
    Float_t         GetShadowingTolerance()                {return fShadowing;}
    std::string     GetScan()                     {return fScan;}

    Float_t         GetAttCoef()                  {return fAttCoef;}
    Float_t         GetAttCoefEr()                {return fAttCoefEr;}

  protected: 
  
    std::string  fPath;          // Path to the directory containing the SOC files
    std::string   fScan;                   // 
    std::string  fDiagonal;
    Int_t     fLambda;                  // Wavelength (nm)

    Bool_t      lambdaValidity;                             // Validity of an user inputted wavelength
    Bool_t      pathValidity;                               // Validity of an user inputted path to the SOC files
    Bool_t      diagValidity;                               // Validity of an user inputted diagonal to be analysed
    Float_t   fTolerance;               // Maximum distance (mm) allowed for PMTs relative to the diagonal axis  
    Float_t   fShadowing;               // Shadowing tolerances by the detector geometry

    Int_t     fNRuns;                   // Number of runs
    Int_t     fNPulses[NRUNS];          // Number of pulses triggered
    Int_t     fRunID[NRUNS];            // Run IDs
    
    Double_t  fSourceWL[NRUNS];         // Wavelength of each run
    
    TVector3  fSourcePos[NRUNS];        // Source position (mm)
    
    TVector3  fPMTPos[NRUNS][9100];     // PMT position (mm)
	
    Float_t   fPMTOcc[NRUNS][9100];     // PMT raw occupancy
    
    TVector3  fPMTNorm[NRUNS][9100];    // PMT bucket normal (unit normalised) pointing INTO the detector, towards the AV
    Int_t     fPMTLCN[NRUNS][9100];     // PMT ID number
  
    // Counters to see how many PMTs there are in total
    Int_t     fNPMTs[NRUNS];            // Number of NORMAL PMTs in each run
    Int_t     fNOKPMTs;                 // Total number NORMAL & not shadowed PMTs

    Float_t   fSolidAngle;              // Solid angle
    Float_t   fFresnel;                 // Fresnel transmission coefficient
    Float_t   fAngRespCoefficient;      // PMT angular response coeffcient
    Float_t   fPMTResponse;             // PMT agular response
    Float_t   fMaskDistribution;        // Laserball mask intensity distribution

    Float_t   fPMTs[NRUNS][NPMS][4];    // Saves all the important information about the PMTs selected:
                                        // 0 - Raw occupancy
                                        // 1 - Corrected occupancy
                                        // 2 - Corrections applied to the raw occupancy
                                        // 3 - Distance travelled by light inside the AV from the source to the PMT (mm)
    TVector3  fTolPMTs[NRUNS][NPMS];    // Position vectors of the selected PMTs (mm)

    Int_t     fNPairs[NRUNS];           // Number of PMT pairs
    Int_t     fPair1[NRUNS][NPMS];      // List of PMTs in group 1 (Z>0) that have a pair 
    Int_t     fPair2[NRUNS][NPMS];      // List of PMTs in group 2 (Z<0) that have a pair 

    Float_t   fRatio[NRUNS][NPMS];      // Ratio of the corrected occupancies for a PMT pair j in a run i
    Float_t   fProduct[NRUNS][NPMS];    // Product of the corrected occupancies for a PMT pair j in a run i
    Float_t   fRerrors[NRUNS][NPMS];    // Errors for fRatio
    Float_t   fPerrors[NRUNS][NPMS];    // Errors for fProduct

    Float_t   fSumRatio[NRUNS];         // Weighted average of fRatio for all PMT pairs
    Float_t   fSumProduct[NRUNS];       // Weighted average of fProduct for PMT pairs
    Float_t   fSumRerrors[NRUNS];       // Errors for fSumRatio
    Float_t   fSumPerrors[NRUNS];       // Errors for fSumProduct

    Float_t   fDistance[NRUNS];         // Mean value of the light path length difference for one run (mm)

    Float_t   fAttCoef;                 // Attenuation coefficient
    Float_t   fAttCoefEr;               // Attenuation coefficient error

    ClassDef( DiagScan, 2 )
};

#endif // __DiagScan_h__
