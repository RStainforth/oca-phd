///////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAFit.hh
///
/// CLASS: OCA::OCAFit
///
/// BRIEF: Levenberg-Marquardt fitter for the data (original oca)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This is the class which implements the Levenberg-Marquardt fit 'a la SNO'
///         The code herein is largely based on the original
///         OCA code from SNO. However it has been implemented
///         to coincide with the RAT libraries for SNO+ and
///         consequently re-written.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAFIT_
#define _OCAFIT_

#include <map>

#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCAMath.hh"
#include "OCARunReader.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TGraph.h"

namespace OCA{

  class OCAFit : public TObject
  {
  public:
    // The constructors
    OCAFit();
    
    // The destructors - See OCAFit.cc file
    ~OCAFit();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Load the cardfile
    void LoadFitFile( const char* fitFile );

    // Initialise the parameters
    void InitialiseParameters();

    // Print information about all the parameters
    void PrintInitialisationInfo();

    // Allocate space for the parameters
    void AllocateParameters();

    // Screen the data and remove PMTs based on certain selection criteria
    void DataScreen( const Float_t chiSqLimit = 100.0 );                              

    // Check whether a PMT should be skipped over
    Bool_t PMTSkip( const OCARun* iRunPtr, const OCAPMT* iPMTPtr, Float_t mean = -1.0e6, Float_t sigma = -1.0e6 );

    // Return the model Angular Response value and Laserball Distribution value
    TH1F* AngularResponseTH1F();
    static Double_t SPMTAngularResponse( Double_t* a, Double_t* par );
    TF1* AngularResponseTF1();
    Float_t ModelAngularResponse( const OCAPMT* iPMTPtr, Int_t& iAng, Int_t runType );
    Float_t ModelLBDistribution( const OCARun* iRunPtr, const OCAPMT* iPMTPtr, Int_t& iLBDist, Int_t runType );
    Float_t ModelLBDistribution( const Float_t cosTheta, const Float_t phi, Int_t& iLBDist );

    Float_t ModelLBDistributionMask( const OCAPMT* iPMTPtr, const Int_t runType );
    Float_t ModelLBDistributionMask( const Float_t cosTheta );


    
    TH2F* ApplyLBDistribution();
    TF1* LBDistributionWaveTF1( const Int_t iTheta );
    TF1* LBDistributionMaskTF1();
    static Float_t SLBDistributionMask( Double_t* aPtr, Double_t* parPtr );
    static Float_t DLBDistributionMask( Double_t* aPtr, Double_t* parPtr );
    static Float_t SLBDistributionWave( Double_t* aPtr, Double_t* parPtr );
    static Float_t DLBDistributionWave( Double_t* aPtr, Double_t* parPtr );

    TGraph* PlotModelROccVsDataROcc();

    // Calculate the individual PMT chisquare and global chisquare
    Float_t CalculatePMTChiSquare( const OCARun* iRunPtr, const OCAPMT* iPMTPtr );
    Float_t CalculateChiSquare();

    // Calculate the error on an individual PMT
    Float_t CalculatePMTSigma( const OCAPMT* iPMTPtr );

    // Calculate the error on an individual PMT
    Float_t CalculatePMTVariability( const OCAPMT* iPMTPtr );

    // Calculate the occupancy ratio (data) value
    Float_t CalculatePMTData( const OCAPMT* iPMTPtr );

    // Calculate the occupancy ratio (model) value
    Float_t ModelPrediction(  const OCARun* iRunPtr, const OCAPMT* iPMTPtr, Int_t nA = 0, Float_t* dyda = NULL );

    // Perform the fit using the Levenberg-Marquardt algorithm
    void PerformFit();

    // Fill information about the base parameters relevent to ALL PMTs
    void FillParameterbase();

    // Fill the angular response loop-up array
    void FillAngIndex();

    // Fill information about the parameters specific to a particualr PMT (i.e. Angular response and laserball distribution intensity)
    void FillParameterPoint();

    // Write the fit to a .root file
    void WriteFitToFile( const char* fileName );

    // Deallocate all data declared on the stack - this is called by the OCAFit destructor
    void DeAllocate();

    // Print the coviariance matrix
    void PrintCovarianceMatrix();
    
    // Print Paramter Information
    void PrintParameterInformation();
    
    ///////////////////////////////////
    /////     FITTING METHODS     /////
    ///////////////////////////////////

    // All fitting methods included here are the original numerical recipes methods
    // as featured in the original OCA code (SNO)
    
    Int_t MrqFit( float x[], float y[], 
                  float sig[], int ndata, 
                  float a[], int ia[], 
                  int ma, float **covar, 
                  float **alpha, float *chisq );
    
    Int_t  mrqmin( float x[], float y[], 
                   float sig[], int ndata, 
                   float a[], int ia[], 
                   int ma, float **covar, 
                   float **alpha, float *chisq,
                   float *alambda );
    
    void covsrt( float **covar, int ma, 
                 int ia[], int mfit );
    
    Int_t gaussj( float **a, int n, 
                  float **b, int m );
    
    void mrqcof( float x[], float y[], 
                 float sig[], int ndata, 
                 float a[], int ia[], 
                 int ma, float **alpha, 
                 float beta[], float *chisq );
    
    void mrqfuncs( Float_t x,Int_t ix,
                   Float_t a[],Float_t *y,
                   Float_t dyda[],Int_t na );
    

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetFitName() const { return fFitName; }
    std::string GetFitTitle() const { return fFitTitle; }

    Bool_t GetValidPars() const { return fValidPars; }
    Bool_t GetDataScreen() const { return fDataScreen; }

    OCARunReader GetRunReader() const { return fRunReader; }

    OCARun* GetCurrentRun() const { return fCurrentRun; }
    OCAPMT* GetCurrentPMT() const { return fCurrentPMT; }

    Int_t GetNRuns() const { return fNRuns; }
    std::vector< Int_t > GetListOfRunIDs() const { return fListOfRunIDs; }
    
    Bool_t GetVaryAll() const { return fVaryAll; }
    Bool_t GetScintVary() const { return fScintVary; }
    Bool_t GetAVVary() const { return fAVVary; }
    Bool_t GetWaterVary() const { return fWaterVary; }

    Bool_t GetScintRSVary() const { return fScintRSVary; }
    Bool_t GetAVRSVary() const { return fAVRSVary; }
    Bool_t GetWaterRSVary() const { return fWaterRSVary; }

    Bool_t GetAngularResponseVary() const { return fAngularResponseVary; }
    Bool_t GetLBDistributionVary() const { return fLBDistributionVary; }

    Bool_t GetLBNormalisationVary() const { return fLBNormalisationVary; }

    Float_t GetScintInit() const { return fScintInit; }
    Float_t GetAVInit() const { return fAVInit; }
    Float_t GetWaterInit() const { return fWaterInit; }

    Float_t GetScintRSInit() const { return fScintRSInit; }
    Float_t GetAVRSInit() const { return fAVRSInit; }
    Float_t GetWaterRSInit() const { return fWaterRSInit; }

    Float_t GetAngularResponseInit() const { return fAngularResponseInit; }
    Float_t GetLBDistributionInit() const { return fLBDistributionInit; }

    Float_t GetLBNormalisationInit() const { return fLBNormalisationInit; }

    TH2F* GetLBDistribution() const { return fLBDistribution; }
    Int_t GetNLBDistributionThetaBins() const { return fNLBDistributionThetaBins; }
    Int_t GetNLBDistributionPhiBins() const { return fNLBDistributionPhiBins; }
    Int_t GetNLBDistributionThetaWaveBins() const { return fNLBDistributionThetaWaveBins; }
    Int_t GetNLBDistributionWave() const { return fNLBDistributionWave; }
    Int_t GetNLBDistributionMaskParameters() const { return fNLBDistributionMaskParameters; }
    Int_t GetNPMTsPerLBDistributionBinMin() const { return fNPMTsPerAngularResponseBinMin; }
    

    TH1F* GetAngularResponse() const { return fAngularResponse; }
    Int_t GetNAngularResponseBins() const { return fNAngularResponseBins; }
    Int_t GetNPMTsPerAngularResponseBinMin() const { return fNPMTsPerAngularResponseBinMin; }

    Int_t GetNParametersInFit() const { return fNParametersInFit; }
    Int_t GetNDataPointsInFit() const { return fNDataPointsInFit; }
    Int_t GetNPMTsInFit() const { return fNPMTsInFit; }
    Int_t GetNPMTSkip() const { return fNPMTSkip; }

    Float_t GetChiSquare() const { return fChiSquare; }
    Float_t GetChiSquareMaxLimit() const { return fChiSquareMaxLimit; }
    Float_t GetChiSquareMinLimit() const { return fChiSquareMinLimit; }

    Float_t GetNSigma() const { return fNSigma; }
    Float_t GetNChiSquare() const { return fNChiSquare; }
    Float_t GetNOccupancy() const { return fNOccupancy; }

    Float_t GetAVHDShadowingMin() const { return fAVHDShadowingMin; }
    Float_t GetAVHDShadowingMax() const { return fAVHDShadowingMax; }
    Float_t GetGeoShadowingMin() const { return fGeoShadowingMin; }
    Float_t GetGeoShadowingMax() const { return fGeoShadowingMax; }

    Bool_t GetCHSFlag() const { return fCHSFlag; }
    Bool_t GetCSSFlag() const { return fCSSFlag; }

    Int_t GetScintParIndex() const { return 1; }
    Int_t GetAVParIndex() const { return 2; }
    Int_t GetWaterParIndex() const { return 3; }

    Int_t GetScintRSParIndex() const { return 4; }
    Int_t GetAVRSParIndex() const { return 5; }
    Int_t GetWaterRSParIndex() const { return 6; }

    Int_t GetAngularResponseParIndex();
    Int_t GetAngularResponse2ParIndex();

    Int_t GetLBDistributionParIndex();
    Int_t GetLBDistributionMaskParIndex();
    Int_t GetLBDistributionWaveParIndex();
    
    Int_t GetLBNormalisationParIndex();

    Float_t GetScintPar() const { return fMrqParameters[ GetScintParIndex() ]; }
    Float_t GetAVPar() const { return fMrqParameters[ GetAVParIndex() ]; }
    Float_t GetWaterPar() const { return fMrqParameters[ GetWaterParIndex() ]; }

    Float_t GetScintRSPar() const { return fMrqParameters[ GetScintRSParIndex() ]; }
    Float_t GetAVRSPar() const { return fMrqParameters[ GetAVRSParIndex() ]; }
    Float_t GetWaterRSPar() const { return fMrqParameters[ GetWaterRSParIndex() ]; }

    Float_t GetAngularResponsePar( const Int_t n ){ return fMrqParameters[ GetAngularResponseParIndex() + n ]; }
    Float_t GetAngularResponseError( const Int_t nVal );
    Float_t GetAngularResponse2Par( const Int_t n ){ return fMrqParameters[ GetAngularResponse2ParIndex() + n ]; }
    Float_t* GetAngularResponse(){ return &fMrqParameters[ GetAngularResponseParIndex() ]; }

    Float_t GetLBDistributionPar( const Int_t n ){ return fMrqParameters[ GetLBDistributionParIndex() + n ]; }
    Float_t GetLBDistributionMaskPar( const Int_t n ){ return fMrqParameters[ GetLBDistributionMaskParIndex() + n ]; }
    Float_t GetLBDistributionWavePar( const Int_t n ){ return fMrqParameters[ GetLBDistributionWaveParIndex() + n ]; }
    Float_t* GetLBDistribution(){ return &fMrqParameters[ GetLBDistributionParIndex() ]; }
    Float_t* GetLBDistributionMask(){ return &fMrqParameters[ GetLBDistributionMaskParIndex() ]; }
    Float_t* GetLBDistributionWave(){ return &fMrqParameters[ GetLBDistributionWaveParIndex() ]; }
    Float_t GetLBDistributionError( const Int_t nVal );
    Float_t GetLBDistributionMaskError( const Int_t nVal );

    Float_t GetLBNormalisationPar( const Int_t n ){ return fMrqParameters[ GetLBNormalisationParIndex() + n ]; }

    Float_t GetMrqX( const Int_t n ){ if ( n == 0 ){ return 0; } else{ return fMrqX[ n ]; } }
    Float_t GetMrqY( const Int_t n ){ if ( n == 0 ){ return 0; } else{ return fMrqY[ n ]; } }
    Float_t GetMrqSigma( const Int_t n ){ if ( n == 0 ){ return 0; } else{ return fMrqSigma[ n ]; } }

    Float_t GetMrqParameter( const Int_t n ){ if ( n == 0 ){ return 0; } else{ return fMrqParameters[ n ]; } }
    Int_t GetMrqVary( const Int_t n ){ if ( n == 0 ){ return 0; } else{ return fMrqVary[ n ]; } }

    Float_t GetMrqCovariance( const Int_t n, const Int_t m ){ if ( n == 0 || m == 0 ){ return 0; } else{ return fMrqCovariance[ n ][ m ]; } }
    Float_t GetMrqAlpha( const Int_t n, const Int_t m ){ if ( n == 0 || m == 0 ){ return 0; } else{ return fMrqAlpha[ n ][ m ]; } }

    OCARun* GetRunEntry( const Int_t iRun ){ return fRunReader.GetRunEntry( iRun ); }
    Int_t GetRunIndex( const Int_t runID );

    Int_t GetNVariableParameters();

    Float_t* GetMrqX() const { Float_t* mrqX = new Float_t[ ( fNPMTsInFit + 1 ) ]; *mrqX = *fMrqX; return mrqX; }
    Float_t* GetMrqY() const { Float_t* mrqY = new Float_t[ ( fNPMTsInFit + 1 ) ]; *mrqY = *fMrqY; return mrqY; }
    Float_t* GetMrqSigma() const { Float_t* mrqSigma = new Float_t[ ( fNPMTsInFit + 1 ) ]; *mrqSigma = *fMrqSigma; return mrqSigma; }

    std::map< Int_t, OCAPMT >::iterator GetFitPMTsIterBegin(){ return fFitPMTs.begin(); }
    std::map< Int_t, OCAPMT >::iterator GetFitPMTsIterEnd(){ return fFitPMTs.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetScintVary( Bool_t varyBool ){ fMrqVary[ GetScintParIndex() ] = varyBool; }
    void SetAVVary( Bool_t varyBool ){ fMrqVary[ GetAVParIndex() ] = varyBool; }
    void SetWaterVary( Bool_t varyBool ){ fMrqVary[ GetWaterParIndex() ] = varyBool; }

    void SetScintRSVary( Bool_t varyBool ){ fMrqVary[ GetScintRSParIndex() ] = varyBool; }
    void SetAVRSVary( Bool_t varyBool ){ fMrqVary[ GetAVRSParIndex() ] = varyBool; }
    void SetWaterRSVary( Bool_t varyBool ){ fMrqVary[ GetWaterRSParIndex() ] = varyBool; }

    void SetAngularResponseVary( Bool_t varyBool );
    void SetAngularResponse2Vary( Bool_t varyBool );

    void SetLBDistributionVary( Bool_t varyBool );
    void SetLBDistributionMaskVary( Bool_t varyBool );
    void SetLBDistributionWaveVary( Bool_t varyBool );

    void SetLBNormalisationVary( Bool_t varyBool );

    void SetScintPar( Float_t parVal ){ fMrqParameters[ GetScintParIndex() ] = parVal; }
    void SetAVPar( Float_t parVal ){ fMrqParameters[ GetAVParIndex() ] = parVal; }
    void SetWaterPar( Float_t parVal ){ fMrqParameters[ GetWaterParIndex() ] = parVal; }

    void SetScintRSPar( Float_t parVal ){ fMrqParameters[ GetScintRSParIndex() ] = parVal; }
    void SetAVRSPar( Float_t parVal ){ fMrqParameters[ GetAVRSParIndex() ] = parVal; }
    void SetWaterRSPar( Float_t parVal ){ fMrqParameters[ GetWaterRSParIndex() ] = parVal; }

    void SetAngularResponsePar( Int_t n, Float_t val ){ fMrqParameters[ GetAngularResponseParIndex() + n ] = val; }
    void SetAngularResponse2Par( Int_t n, Float_t val ){ fMrqParameters[ GetAngularResponse2ParIndex() + n ] = val; }

    void SetLBDistributionPar( Int_t n, Float_t val ){ fMrqParameters[ GetLBDistributionParIndex() + n ] = val; }
    void SetLBDistributionMaskPar( Int_t n, Float_t val ){ fMrqParameters[ GetLBDistributionMaskParIndex() + n ] = val; }
    void SetLBDistributionWavePar( Int_t n, Float_t val ){ fMrqParameters[ GetLBDistributionWaveParIndex() + n ] = val; }

    void SetLBNormalisationPar( Int_t n, Float_t val ){ fMrqParameters[ GetLBNormalisationParIndex() + n ] = val; }

    void SetMrqParameter( Int_t n, Float_t val ){ if ( n == 0 ){ return; } else{ fMrqParameters[ n ] = val; } }
    void SetMrqParameters( Double_t* pars ){ *fMrqParameters = *pars; }
    void SetMrqVary( Int_t n, Int_t val ){ if ( n == 0 ){ return; } else{ fMrqVary[ n ] = val; } }

  private:

    std::string fFitName;                                    // The name of the fit
    std::string fFitTitle;                                   // The title of the fit

    Bool_t fValidPars;                                       // The OCAFit structure has allocated valid parameters
    Bool_t fDataScreen;                                      // The Data has been screened and filtered for reasonable tubes
    
    OCARunReader fRunReader;                               // The Run reader to go over all the OCARun files

    OCARun* fCurrentRun;                                   // Pointer to the current OCARun object
    OCAPMT* fCurrentPMT;                                   // Pointer to the current OCAPMT object

    Int_t fNRuns;                                            // Number of runs loaded into the fit
    std::vector< Int_t > fListOfRunIDs;                      // List of run IDs in the fit

    Bool_t fVaryAll;                                         // TRUE: All parameters in the fit were varied (FALSE: not)
    Bool_t fScintVary;                                       // Whether the scintillator attenuation length was varied
    Bool_t fAVVary;                                          // Whether the acrylic (AV) attenuation length was varied
    Bool_t fWaterRSVary;                                     // Whether the water attenuation length was varied

    Bool_t fScintRSVary;                                     // Whether the scintillator rayleigh scattering length was varied
    Bool_t fAVRSVary;                                        // Whether the acrylic (AV) rayleigh scattering length was varied
    Bool_t fWaterVary;                                       // Whether the water rayleigh scattering length was varied

    Bool_t fAngularResponseVary;                             // Whether the angular response was varied
    Bool_t fAngularResponse2Vary;                            // Whether the (2nd)angular response was varied 
    Bool_t fLBDistributionVary;                              // Whether the laserball distribution was varied

    Bool_t fLBNormalisationVary;                             // Whether the run normalisations was varied

    Float_t fScintInit;                                     // Initial scintillator (1/attenuation length) value
    Float_t fAVInit;                                        // Initial acrylic (AV) (1/attenuation length) value
    Float_t fWaterInit;                                     // Initial water (1/attenuation length) value

    Float_t fScintRSInit;                                   // Initial scintillator (1/rayleigh scattering length) value
    Float_t fAVRSInit;                                      // Initial acrylic (AV) (1/rayleigh scattering length) value
    Float_t fWaterRSInit;                                   // Initial water (1/rayleigh scattering length) value

    Float_t fAngularResponseInit;                           // Initial angular response value
    Float_t fLBDistributionInit;                            // Initial laserball distribution value

    Float_t fLBNormalisationInit;                           // Initial laserball normalisation value

    
    TH2F* fLBDistribution;                                  // Pointer to the laserball distribution 2D histogram
    Int_t fNLBDistributionThetaBins;                        // Number of theta bins in the laserball distribution histogram
    Int_t fNLBDistributionPhiBins;                          // Number of phi bins in the laserball distribution histogram
    Int_t fNLBDistributionMaskParameters;                   // Mask parameters of the laserbal distribution polynomial
    Int_t fNLBDistributionThetaWaveBins;                    // Number of parameters defining the sine wave
    Int_t fNLBDistributionWave;                             // Distance of the laserball theta wave distribution
    Int_t fNPMTsPerLBDistributionBinMin;                    // Minimum required number of PMTs per bin in the laserball distribution histogram

    TH1F* fAngularResponse;                                 // Pointer to the PMT angular response 1D histogram
    Int_t fNAngularResponseBins;                            // Number of (theta) bins in the angular response histogram
    Int_t fNPMTsPerAngularResponseBinMin;                   // Minimum required number of PMTs per bin in the angular response histogram

    Int_t fNParametersInFit;                                // Total number of parameters in the fit
    Int_t fNDataPointsInFit;                                // Total number of data points (including PMTs which do NOT pass the selection criteria)
    Int_t fNPMTsInFit;                                      // Total number of PMTs in the fit (PMTs which pass the selection criteria)
    Int_t fNPMTSkip;                                        // Number of PMTs to skip through (higher speeds up fitting, but with less data points)

    Float_t fChiSquare;                                     // Most recent ChiSquare value (not reduced)
    Float_t fChiSquareMaxLimit;                             // Maximum value of the chisquare for PMTs to be cut from the fit
    Float_t fChiSquareMinLimit;                             // Minimum value of the chisquare for PMTs to be cut from the fit

    Float_t fNSigma;                                        // The number of standard deviations away from the mean occupancy for a PMT to be cut on
    Float_t fNChiSquare;
    Float_t fNOccupancy;                                    // The number of prompt counts for a PMT to be cut on (see OCAFit::PMTSkip method)

    Float_t fCosThetaMaxLimit;                              // The maximum costheta value for a PMT allowed in the fit
    Float_t fCosThetaMinLimit;                              // The minimum costheta value for a PMT allowed in the fit
    
    Float_t fPMTDataROccMaxLimit;                           // The maximum data relative occupancy for a PMT allowed in the fit
    Float_t fPMTDataROccMinLimit;                           // The minimum data relative occupancy for a PMT allowed in the fit

    Float_t fPMTPosThetaMaxLimit;                           // The maximum PMT position theta value for a PMT allowed in the fit
    Float_t fPMTPosThetaMinLimit;                           // The minimum PMT position theta value for a PMT allowed in the fit
    Float_t fPMTPosPhiMaxLimit;                             // The maximum PMT position phi value for a PMT allowed in the fit
    Float_t fPMTPosPhiMinLimit;                             // The minimum PMT position phi value for a PMT allowed in the fit

    Float_t fAVHDShadowingMin;                              // The minimum value of of the AV hold-down rope shadowing for the PMTs to be cut on
    Float_t fAVHDShadowingMax;                              // The maximum value of of the AV hold-down rope shadowing for the PMTs to be cut on
    Float_t fGeoShadowingMin;                               // The minimum value of of the AV geometry shadowing for the PMTs to be cut on
    Float_t fGeoShadowingMax;                               // The maximum value of of the AV geometry shadowing for the PMTs to be cut on

    Bool_t fCHSFlag;                                        // The CHS Flag status for PMTs to be cut on
    Bool_t fCSSFlag;                                        // The CSS Flag status for PMTs to be cut on

    Int_t fNElements;                                       // The number of elements in the fChiArray[] and fResArray[] array
    Float_t* fChiArray;                                     // [fNElements] Array of chisquared for mrqcof() calls
    Float_t* fResArray;                                     // [fNElements] Array of residuals for mrqcof() calls
    Float_t* fCentralRunNorms;

    Int_t fLBDistributionType;
    Int_t fiAng;                                            // Index for PMT angular response to be re-zeroed
    Int_t fCiAng;                                           // Index for PMT angular response in the central run to be re-zeroed
    Int_t fiLBDist;                                         // Index for laserball distribution to be re-zeroed
    Int_t fCiLBDist;                                        // Index for laserball distribution in the central run to be re-zeroed
    Int_t fiNorm;                                           // Index for laserball normalisation to be re-zeroed

    
    Int_t fParamBase;                                       // Number of variable parameters, not including PMT Response or LB Distribution
    Int_t fParam;                                           // Number of variable parameters, including PMT response and LB distribution
    Int_t*** fAngIndex;                                     // Lookup table for unique variable parameters in the PMT response
    Int_t* fParamIndex;                                     // Lookup table for ordered variable parameters
    Int_t* fParamVarMap;                                    // Lookup table for variable parameters (global)

    // The arrays used by the Levenburg-Marquadt (Mrq) algorithm to find the parameters
    Float_t* fMrqX;                                         // [fNDataPointsInFit+1] Index into the PMTs to be used in the fit
    Float_t* fMrqY;                                         // [fNDataPointsInFit+1] Index into the PMTs (from above) corresponding OccRatio
    Float_t* fMrqSigma;                                     // [fNDataPointsInFit+1] Error on each PMT occupancy (statistical)

    Float_t* fMrqParameters;                                // [fNParameters+1] Parameters for the model
    Int_t* fMrqVary;                                        // [fNParameters+1] Flag for the parameters which vary in the model (from above)
    Float_t** fMrqCovariance;                               // [fNParameters+1][fNParameters+1] Covariance matrix
    Float_t** fMrqAlpha;                                    // [fNParameters+1][fNParameters+1] Curvature matrix

    Int_t fCurrentRunIndex;                                 // Current run index in the OCARun Reader Object

    OCARun** fRunPtrs;
    OCAPMT** fPMTPtrs;

    Int_t fSkipLT25;
    Int_t fSkipGT2;
    Int_t fSkipErrFrac;
    Int_t fSkipBad;
    Int_t fSkipBasicBad;
    Int_t fSkipCentralBad;
    

    std::map< Int_t, OCAPMT > fFitPMTs;                   // Map of PMTs which pass the cut selection and are to be used in the fit
    std::map< Int_t, OCAPMT >::iterator fiPMT;            // Map iterator used when scanning through PMTs

    ClassDef( OCAFit, 1 )

  };

}

#endif
