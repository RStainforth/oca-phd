////////////////////////////////////////////////////////////////////
///
/// FILENAME: LBOrientation.h
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
#include <RAT/DS/FitResult.hh>
#include <RAT/DU/Utility.hh>
#include <RAT/DU/PMTInfo.hh>
#include <RAT/DB.hh>
#include <RAT/PhysicsUtil.hh>

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
    LBOrientation( const Int_t lambda = 505, const std::string& scan = "oct15", const std::string& path = getenv( "OCA_SNOPLUS_DATA" ) + (string) "/runs/soc/" );
    // lambda: is the wavelenght of the runs that are going to be analysed
    // scan: laserball scan to which the runs belong
    // path: is the path to the directory containing the files

    virtual ~LBOrientation();

    void Initialize();
    void ReadData();
    void Ratios();
    void PlotResults();
    void WriteToFile();

    void SetLambda( const Int_t aNumber );
    void SetScan( const std::string& aString );
    void SetPath( const std::string& aString );

  protected: 

    Int_t       fLambda;                                   // Wavelength
    std::string fPath;                                     // Path to SOC files
    std::string fScan;                                     // Laserball scan, MMMYY
    std::string fPhase;                                    // Data-taking phase: water, scintillator, te-loading

    Bool_t      lambdaValidity;                            // Validity of an user inputted wavelength
    Bool_t      scanValidity;                              // Validity of an user inputted scan
    Bool_t      pathValidity;                              // Validity of an user inputted path to the SOC files
    Bool_t      orientationValidity;                       // Validity of the orientation

    Int_t       fOrientation[NRUNS];                       // Source orientation
    Int_t       fNPMTs[NRUNS];                             // # of PMTs
    Int_t       fRun[NRUNS];                               // Run ID
    Float_t     fSourceWL[NRUNS];                          // Source wavelength
    TVector3    fSourcePos[NRUNS];                         // Source position
    TVector3    fSourceDirVec[NRUNS];                      // Source direction vector

    TVector3    fPMTPos[NRUNS][9500];                      // PMT position
    Float_t     fPMTOcc[NRUNS][9500];                      // PMT occupancy, normalized by the integrated run occupancy
    Float_t     fPMTOccErr[NRUNS][9500];                   // PMT occupancy uncertainty    

    Float_t     fAmplitudeFIT13[NTHETA];                   // Amplitudes, for all slices, from the ratio N/S
    Float_t     fPhaseFIT13[NTHETA];                       // Phases, for all slices, from the ratio N/S
    Float_t     fAmplitudeFIT13error[NTHETA];              // Amplitude errors, for all slices, from the ratio N/S
    Float_t     fPhaseFIT13error[NTHETA];                  // Phase errors, for all slices, from the ratio N/S

    Float_t     fAmplitudeFIT20[NTHETA];                   // Amplitudes, for all slices, from the ratio W/E
    Float_t     fPhaseFIT20[NTHETA];                       // Phases, for all slices, from the ratio W/E
    Float_t     fAmplitudeFIT20error[NTHETA];              // Amplitude errors, for all slices, from the ratio W/E
    Float_t     fPhaseFIT20error[NTHETA];                  // Phase errors, for all slices, from the ratio W/E

    Float_t     fAmplitudeFIT[NTHETA];                     // Amplitudes, for all slices, from the combined ratios
    Float_t     fPhaseFIT[NTHETA];                         // Phases, for all slices, from the combined ratios
    Float_t     fAmplitudeFITerror[NTHETA];                // Amplitude errors, for all slices, from the combined ratios
    Float_t     fPhaseFITerror[NTHETA];                    // Phase errors, for all slices, from the combined ratios

    Float_t     fRTheta[NTHETA];                           // Theta bin list
    Float_t     fRPhi[NPHI];                               // Phi bin list
		
    Float_t     fCosthetamin[NTHETA],fCosthetamax[NTHETA]; // cosTheta minimum and maximum for each slice
    Float_t     fPhimin[NPHI],fPhimax[NPHI];               // phi minimum and maximum for each slice

    Float_t     fRatio13[NPHI],fRatio20[NPHI];             // Ratios N/S and W/E (respectively)
    Float_t     fEratio13[NPHI],fEratio20[NPHI];           // Errors of the Ratios N/S and W/E (respectively)

    Float_t     fRatio20_90[NPHI],fEratio20_90[NPHI];      // Ratio W/E and errors shifted in phi by 90 degrees
		
    TGraphErrors *GR13[NTHETA],*GR20[NTHETA];              // Graphs for the N/S and W/E ratios
    TGraphErrors *GR20_90[NTHETA];                         // Graph for the W/E ratio shifted by 90 degrees

    ClassDef(LBOrientation,0)
};

#endif
