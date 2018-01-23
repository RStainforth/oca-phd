////////////////////////////////////////////////////////////////////
///
/// FILENAME: LBOrientation.C
///
/// BRIEF: A class to extract the parameters that quantify
///        the laserball light distribution, described by 
///        a sinusoidal model.
///                  
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     10/2016 : A.S.Inacio - First Version.
///
/// DETAIL: This class obtains the laserball asymmetry with the 
///         azimuthal angle from the ratio of runs with the 
///         laserball facing opposite directions: N/S and W/E,
///         for different slices of the laserball polar angle.
///         Fitting these results allows to extract the parameters,
///         amplitude and phase, of the sinusoidal model that
///         characterizes the light distribution of the laserball.
///         
////////////////////////////////////////////////////////////////////

#ifndef __LBOrientation_h__
#define __LBOrientation_h__

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include <RAT/DS/SOC.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/SOCPMT.hh>
#include <RAT/DU/SOCReader.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DB.hh>

#include "TROOT.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TMath.h"
#include "TH1F.h"
#include "TVector3.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"

#define NRUNS 4
#define NORIENTATIONS 4
#define NTHETA 24
#define NPHI 36

class LBOrientation : public TObject {

  public:
    LBOrientation();
    LBOrientation( Int_t lambda );
    LBOrientation( Int_t lambda, const std::string& path );

    // lambda: is the wavelenght os the runs that are going to be analysed
    // path: is the path to the directory containing the files

    virtual ~LBOrientation();

    void Initialize();
    void ReadData();
    void Ratios();
    void PlotResults();
    void WriteToFile();
    void SetLambda( Int_t aNumber );
    void SetPath( const std::string& path );

  protected: 

    Int_t       fLambda;                                    // Wavelength
    std::string fPath;                                      // Path to SOC files
    std::string fScan;                                      // Laserball scan, MMMYY
    std::string fPhase;                                     // Data-taking phase: water, scintillator, te-loading

    Bool_t      lambdaValidity;                             // Validity of an user inputted wavelength
    Bool_t      pathValidity;                               // Validity of an user inputted path to the SOC files

    Int_t       fOrientation[NRUNS];                        // Source orientation

    Int_t       fNPMTs0[NTHETA];                            // # of PMTs in run with orientation 0 (E)
    Int_t       fNPMTs1[NTHETA];                            // # of PMTs in run with orientation 1 (N)    
    Int_t       fNPMTs2[NTHETA];                            // # of PMTs in run with orientation 2 (W)
    Int_t       fNPMTs3[NTHETA];                            // # of PMTs in run with orientation 3 (S)

    Int_t       fNPMTs[NRUNS];                              // # of PMTs
    Int_t       fRun[NRUNS];                                // Run ID
    Int_t       fNPulses[NRUNS];                            // Number of pulses triggered
    Double_t    fSourceWL[NRUNS];                           // Source wavelength
    TVector3    fSourcePos[NRUNS];                          // Source position
    TVector3    fSourceDir[NRUNS];                          // Source direction

    TVector3    fPMTPos[NRUNS][9500];                       // PMT position
    Float_t     fPMTOcc[NRUNS][9500];                       // PMT occupancy

    Float_t     fAmplitudeFIT13[NTHETA];                    // Amplitudes, for all slices, from the ratio N/S
    Float_t     fPhaseFIT13[NTHETA];                        // Phases, for all slices, from the ratio N/S
    Float_t     fAmplitudeFIT13error[NTHETA];               // Amplitude errors, for all slices, from the ratio N/S
    Float_t     fPhaseFIT13error[NTHETA];                   // Phase errors, for all slices, from the ratio N/S

    Float_t     fAmplitudeFIT20[NTHETA];                    // Amplitudes, for all slices, from the ratio W/E
    Float_t     fPhaseFIT20[NTHETA];                        // Phases, for all slices, from the ratio W/E
    Float_t     fAmplitudeFIT20error[NTHETA];               // Amplitude errors, for all slices, from the ratio W/E
    Float_t     fPhaseFIT20error[NTHETA];                   // Phase errors, for all slices, from the ratio W/E

    Float_t     fAmplitudeFIT[NTHETA];                      // Amplitudes, for all slices, from the combined ratios
    Float_t     fPhaseFIT[NTHETA];                          // Phases, for all slices, from the combined ratios
    Float_t     fAmplitudeFITerror[NTHETA];                 // Amplitude errors, for all slices, from the combined ratios
    Float_t     fPhaseFITerror[NTHETA];                     // Phase errors, for all slices, from the combined ratios

    Float_t     fRTheta[NTHETA];
    Double_t    fRPhi[NPHI];
		
    Double_t    fCosthetamin[NTHETA],fCosthetamax[NTHETA];  // cosTheta minimum and maximum for each slice
    Double_t    fPhimin[NPHI],fPhimax[NPHI];                // phi minimum and maximum for each slice

    Double_t    fRatio13[NPHI],fRatio20[NPHI];              // Ratios N/S and W/E (respectively)
    Double_t    fEratio13[NPHI],fEratio20[NPHI];            // Errors of the Ratios N/S and W/E (respectively)

    Double_t    fRatio20_90[NPHI],fEratio20_90[NPHI];       // Ratio W/E and errors shifted in phi by 90 degrees
		
    TGraphErrors *GR13[NTHETA],*GR20[NTHETA];
    TGraphErrors *GR20_90[NTHETA];

    ClassDef(LBOrientation,2)
};

#endif
