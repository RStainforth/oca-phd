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

#include "TROOT.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"
#include "TGraphErrors.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include <RAT/DS/SOC.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/SOCPMT.hh>
#include <RAT/DU/SOCReader.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/PhysicsUtil.hh>

#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"

#define NRUNS 6
#define NPMS 500

using namespace std;

class DiagScan : public TObject {

  public:
    DiagScan();
    DiagScan( const Int_t lambda, const std::string& diagonal, const std::string& scan, const std::string& path );

    virtual ~DiagScan();

    void Initialize();
    void ReadData();
    void Process();
    void FitRatio();
    void Product();

    void SetLambda( const Int_t aNumber );
    void SetScan( const std::string& aString );
    void SetPath( const std::string& aString );
    void SetDiagonal( const std::string& aString );
    void SetDistanceCut( const Float_t aNumber );
    void SetShadowingTolerance( const Float_t aNumber );
    void SetNSigmas( const Int_t aNumber );

    void SelectPMTs( const Int_t nRun, const RAT::DS::SOC& soc );
    void CheckLowOccupancy( const Int_t iRun );

    Float_t  GetAttCoef()     {return fAttCoef;}
    Float_t  GetAttCoefErr()  {return fAttCoefErr;}

  protected: 

    Int_t       fLambda;                      // Wavelength (nm)
    std::string fPath;                        // Path to the directory containing the SOC files
    std::string fScan;                        // Laserball scan, MMMYY
    std::string fPhase;                       // Data-taking phase: water, scintillator, te-loading
    std::string fDiagonal;                    // Diagonal used in the analysis

    Bool_t      lambdaValidity;               // Validity of an user inputted wavelength
    Bool_t      scanValidity;                 // Validity of an user inputted scan
    Bool_t      pathValidity;                 // Validity of an user inputted path to the SOC files
    Bool_t      diagValidity;                 // Validity of an user inputted diagonal to be analysed

    TVector3    fDiagonalVector;              // Vector that describes the diagonal

    Float_t     fDistanceCut;                 // Maximum distance (mm) allowed for PMTs relative to the diagonal
    Double_t    fShadowing;                   // Shadowing tolerances by the detector geometry

    Int_t       fNRuns;                       // Number of runs
    Int_t       fNPulses[NRUNS];              // Number of pulses triggered
    Int_t       fRunID[NRUNS];                // Run IDs
    
    Float_t     fSourceWL[NRUNS];             // Wavelength of each run
    
    TVector3    fSourcePos[NRUNS];            // Source position (mm)
    
    TVector3    fPMTPos[NRUNS][NPMS];         // PMT position (mm)
	
    Float_t     fPMTOcc[NRUNS][NPMS];         // PMT raw occupancy

    Float_t     fPMTCorrections[NRUNS][NPMS]; // Product of the Solid Angle and Fresnel trasmission coefficient for each PMT in each run

    Float_t     fPMTCorrOcc[NRUNS][NPMS];     // PMT corrected occupancy by the solid angle and Fresnel coefficients

    Float_t     fPMTDistInAV[NRUNS][NPMS];    // Light path distance inside the AV from source to each PMT in each run 

    Int_t       fNPMTs[NRUNS];                // Number of NORMAL PMTs, non-shadowed, close to the diagonal in each run

    Int_t       fNSigma;                      // Number of sigmas to exclude PMTs based on their occupancy

    Int_t       fNPairs[NRUNS];               // Number of PMT pairs
    Int_t       fPair1[NRUNS][NPMS];          // List of PMTs in group 1 (Z>0) that have a pair 
    Int_t       fPair2[NRUNS][NPMS];          // List of PMTs in group 2 (Z<0) that have a pair 

    Float_t     fRatio[NRUNS][NPMS];          // Ratio of the corrected occupancies for a PMT pair j in a run i
    Float_t     fProduct[NRUNS][NPMS];        // Product of the corrected occupancies for a PMT pair j in a run i
    Float_t     fRerrors[NRUNS][NPMS];        // Errors for fRatio
    Float_t     fPerrors[NRUNS][NPMS];        // Errors for fProduct

    Float_t     fSumRatio[NRUNS];             // Weighted average of fRatio for all PMT pairs
    Float_t     fSumProduct[NRUNS];           // Weighted average of fProduct for PMT pairs
    Float_t     fSumRerrors[NRUNS];           // Errors for fSumRatio
    Float_t     fSumPerrors[NRUNS];           // Errors for fSumProduct

    Float_t     fDistance[NRUNS];             // Mean value of the light path length difference for one run (mm)

    Float_t     fAttCoef;                     // Attenuation coefficient
    Float_t     fAttCoefErr;                  // Attenuation coefficient error

    ClassDef( DiagScan, 0 )
};

#endif
