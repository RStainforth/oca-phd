////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFitLBPosition.hh
///
/// CLASS: LOCAS::LOCASFitLBPosition
///
/// BRIEF: Object to fit the laserball position for a specific run
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: To be written
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASFITLBPOSITION_
#define _LOCASFITLBPOSITION_

#include "LOCASPMT.hh"
#include "LOCASRun.hh"
#include "LOCASMath.hh"
#include "LOCASRunReader.hh"

#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/GroupVelocity.hh"
#include "RAT/DU/Utility.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/Log.hh"

#include <TVector3.h>
#include <TObject.h>

#include <string>

#include "QDQXX.h"
#include "QOptics.h"

namespace LOCAS{
  
  class LOCASFitLBPosition : public TObject, LOCASMath
  {
  public:
    LOCASFitLBPosition(){ }
    ~LOCASFitLBPosition();

    LOCASFitLBPosition( const LOCASRunReader& lRunReader, const std::string& geoFile );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void InitialiseArrays();
    void ClearArrays();

    void FitLBPosition( const Int_t runID );

    Bool_t SkipPMT( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr );

    void PrintFitInfo();

    // Levenberg-Marquardt Fitting Routines

    Int_t MrqFit(float x[], float y[], float sig[], int ndata, float a[],
                 int ia[], int ma, float **covar, float **alpha, float *chisq );

    Int_t  mrqmin(float x[], float y[], float sig[], int ndata, float a[],
                  int ia[], int ma, float **covar, float **alpha, float *chisq,
                  float *alambda );

    void covsrt(float **covar, int ma, int ia[], int mfit);

    Int_t gaussj(float **a, int n, float **b, int m);

    void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
                int ia[], int ma, float **alpha, float beta[],
                float *chisq);

    void mrqfuncs(Float_t x,Int_t ix,Float_t a[],Float_t *y,
                  Float_t dyda[],Int_t na);

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetGeometryFile( std::string geoFile ){ fGeometryFile = geoFile; }

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetGeometryFile(){ return fGeometryFile; }

    // To obtain the fitted results, first 
    // FitLBPosition must be called
    Double_t GetFittedRadius();
    Double_t GetFittedRadiusError();

    TVector3 GetFittedXYZ();
    TVector3 GetFittedXYZErrors();

    Double_t GetFittedT0();
    Double_t GetFittedT0Error();

    Double_t GetChiSquare();
    Double_t GetReducedChiSquare();

    Double_t GetPromptTimePeakWidthMean(){ return fPromptTimePeakWidthMean; }
    Double_t GetPromptTimePeakWidthSigma(){ return fPromptTimePeakWidthSigma; }

    Float_t** GetCovarianceMatrix(){ return fMrqCovariance; }

    Float_t* GetPromptWidthArray(){ return fPromptWidthArray; }
    Float_t* GetPromptWidthArrayCut(){ return fPromptWidthArrayCut; }

    LOCASRun* GetCurrentRun(){ return fCurrentRun; }



  private:

    LOCASRunReader fRunReader;                  // Private LOCASRunReader object 
    LOCASRun* fCurrentRun;                      // Pointer to the current LOCASRun object
    LOCASPMT* fCurrentPMT;                      // Pointer to the current LOCASPMT object

    std::string fGeometryFile;                  // The name of the goemetry file
                                                // from which the optical media materials were found
    std::string fScintVolMat;                   // The material of the medium inside the AV
    std::string fAVVolMat;                      // The material of the medium of the acrylic vessel
    std::string fCavityVolMat;                  // The material of the medium inside the cavity

    Float_t fVgScint;                           // The group velocity of the scintillator material
    Float_t fVgAV;                              // The group velocity of the acrylic material
    Float_t fVgWater;                           // The group velocity of the water material

    Bool_t fArraysInitialised;                  // TRUE: Arrays initialised ready for fitting FALSE: Not ready

    RAT::DB* fRATDB;                                 // Pointer to the RAT Database     
    RAT::DU::PMTInfo fPMTInfo;                       // PMT Information Object
    RAT::DU::LightPathCalculator fLightPath;         // LightPath Calculator Object
    RAT::DU::LightPathCalculator fLightPathX;        // LightPath Calculator Object - X coordinate
    RAT::DU::LightPathCalculator fLightPathY;        // LightPath Calculator Object - Y coordinate
    RAT::DU::LightPathCalculator fLightPathZ;        // LightPath Calculator Object - Z coordinate
    RAT::DU::GroupVelocity fGVelocity;               // Group Velocity Object

    // The Levenberg-Marquardt working arrays;

    Int_t fNPMTs;                               // Number of PMts int the Run/Fit
    Float_t* fMrqX;                             // Index into array of PMT IDs
    Float_t* fMrqY;                             // T_i + ( D_direct / V_light ) for each PMT i.e. Initial Time plus ToF
    Float_t* fMrqSigma;                         // Error on each of the PMTs timing
    Float_t* fMrqParameters;                    // Parameters for the model, in this case the laser ball
    Int_t* fMrqVary;                          // Flag of which parameters are to vary in the minimising routines
    Float_t** fMrqCovariance;                   // Covariance matrix of the parameters
    Float_t** fMrqCurvature;                    // Curvative matrix (i.e. derivative matrix) of the parameters
    Float_t fChiSquare;                         // The current value of the chi square in the current fit

    Float_t* fChiArray;
    Float_t* fResArray;

    Float_t* fPromptWidthArray;
    Double_t fPromptTimePeakWidthMean;
    Double_t fPromptTimePeakWidthSigma;

    Float_t* fPromptWidthArrayCut;

    TVector3 fCurrentLBPos;
    Int_t fNElements;
    Float_t fDelPos;

    QDQXX fDQXX;

    std::string fDQXXDirPrefix;

    ClassDef(LOCASFitLBPosition,1)

  };

}

#endif
