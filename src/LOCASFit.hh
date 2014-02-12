///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFit.hh
///
/// CLASS: LOCAS::LOCASFit
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL:
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASFIT_
#define _LOCASFIT_

#include <map>

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"
#include "LOCASRunReader.hh"

#include "TH1F.h"
#include "TH2F.h"

namespace LOCAS{

  class LOCASFit : public TObject
  {
  public:
    LOCASFit( const char* fitFile );
    virtual ~LOCASFit(){}

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    //void Initialise();
    //void Clear( Option_t* option = "" );
    //void DoFit();

    void DataScreen();                                  // Perform final checks on data before performing the fit
    //void Screen();
    Bool_t PMTSkip( Int_t iRun, Int_t iPMT, Float_t mean, Float_t sigma );
    Float_t ModelPrediction( Int_t iRun, Int_t iPMT, Int_t nA = 0, Float_t* dyda = NULL );
    Float_t CalculatePMTChiSquare( Int_t iRun, Int_t iPMT );                 // Calculate the chisquare
    //void CalculateChiSquare();                        // Calculate the chiSquare
    void AllocateParameters();
    void InitialiseParameters();
    void PrintInitialisationInfo();

    //void CopyParamterValues( LOCASFit* seedFit );    // Copy the parameter values from seedFit to THIS fit
    //void GiveParameterValues( LOCASFit* targetFit ); // Give the parameter values from THIS fit to targetFit

    Float_t ModelAngularResponse( Int_t iRun, Int_t iPMT, Int_t& iAng, Int_t runType );
    Float_t ModelLBDistribution( Int_t iRun, Int_t iPMT, Int_t& iLBDist, Int_t runType );

    void PerformFit();

    void FillParameterbase();
    void FillAngIndex();
    void FillParameterPoint();

    // FITTING METHODS
    Int_t MrqFit(float x[], float y[], float sig[], int ndata, float a[],
		 int ia[], int ma, float **covar, float **alpha, float *chisq );

    Int_t  mrqmin(float x[], float y[], float sig[], int ndata, float a[],
    		  int ia[], int ma, float **covar, float **alpha, float *chisq,
    		  float *alambda );
    
    void covsrt(float **covar, int ma, int ia[], int mfit);
    
    Int_t gaussj(float **a, int n, float **b, int m);
    
    virtual void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
    			int ia[], int ma, float **alpha, float beta[],
    			float *chisq);
    
    virtual void mrqfuncs(Float_t x,Int_t ix,Float_t a[],Float_t *y,
    			  Float_t dyda[],Int_t na);
    

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetFitName(){ return fFitName; }
    std::string GetFitTitle(){ return fFitTitle; }

    Bool_t GetValidPars(){ return fValidPars; }
    Bool_t GetDataScreen(){ return fDataScreen; }

    LOCASRunReader GetRunReader(){ return fRunReader; }

    LOCASRun* GetCurrentRun(){ return fCurrentRun; }
    LOCASPMT* GetCurrentPMT(){ return fCurrentPMT; }
    LOCASRun* GetCurrentCentralRun(){ return fCurrentCentralRun; }
    LOCASPMT* GetCurrentCentralPMT(){ return fCurrentCentralPMT; }
    LOCASRun* GetCurrentWavelengthRun(){ return fCurrentWavelengthRun; }
    LOCASPMT* GetCurrentWavelengthPMT(){ return fCurrentWavelengthPMT; }

    Int_t GetNRuns(){ return fNRuns; }
    std::vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }
    Int_t GetNCentralRuns(){ return fNCentralRuns; }
    std::vector< Int_t > GetListOfCentralRunIDs(){ return fListOfCentralRunIDs; }
    Int_t GetNWavelengthRuns(){ return fNWavelengthRuns; }
    std::vector< Int_t > GetListOfWavelengthRunIDs(){ return fListOfWavelengthRunIDs; }
    
    Bool_t GetVaryAll(){ return fVaryAll; }
    Bool_t GetScintVary(){ return fScintVary; }
    Bool_t GetAVVary(){ return fAVVary; }
    Bool_t GetWaterVary(){ return fWaterVary; }
    Bool_t GetAngularResponseVary(){ return fAngularResponseVary; }
    Bool_t GetLBDistributionVary(){ return fLBDistributionVary; }

    TH2F* GetLBDistribution(){ return fLBDistribution; }
    Int_t GetNLBDistributionThetaBins(){ return fNLBDistributionThetaBins; }
    Int_t GetNLBDistributionPhiBins(){ return fNLBDistributionPhiBins; }
    Int_t GetNPMTsPerLBDistributionBinMin(){ return fNPMTsPerAngularResponseBinMin; }

    Int_t GetNParametersInFit(){ return fNParametersInFit; }

    Int_t GetNDataPointsInFit(){ return fNDataPointsInFit; }
    Int_t GetNPMTsInFit(){ return fNPMTsInFit; }

    Float_t GetChiSquare(){ return fChiSquare; }
    Float_t GetChiSquareMaxLimit(){ return fChiSquareMaxLimit; }
    Float_t GetChiSquareMinLimit(){ return fChiSquareMinLimit; }

    Int_t GetScintParIndex(){ return 1; }
    Int_t GetAVParIndex(){ return 2; }
    Int_t GetWaterParIndex(){ return 3; }
    Int_t GetAngularResponseParIndex();
    Int_t GetLBDistributionParIndex();

    Float_t GetScintPar(){ return fMrqParameters[ GetScintParIndex() ]; }
    Float_t GetAVPar(){ return fMrqParameters[ GetAVParIndex() ]; }
    Float_t GetWaterPar(){ return fMrqParameters[ GetWaterParIndex() ]; }
    Float_t GetAngularResponsePar( Int_t n ){ return fMrqParameters[ GetAngularResponseParIndex() + n ]; }
    Float_t GetLBDistributionPar( Int_t n ){ return fMrqParameters[ GetLBDistributionParIndex() + n ]; }

    Int_t GetMrqX( Int_t n ){ if ( n == 0 ){ return 0; } else{ return fMrqX[ n ]; } }
    LOCASRun* GetRun( Int_t iRun ){ return fRunReader.GetRunEntry( iRun ); }
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    //void SetVaryAll();
    void SetScintVary( Bool_t varyBool ){ fScintVary = true; }
    void SetAVVary( Bool_t varyBool = true ){ fAVVary = true; }
    void SetWaterVary( Bool_t varyBool = true ){ fWaterVary = true; }
    void SetAngularResponseVary( Bool_t varyBool = true ){ fAngularResponseVary = true; }
    void SetLBDistributionVary( Bool_t varyBool = true ){ fLBDistributionVary = true; }

    void SetScintPar( Float_t parVal ){ fMrqParameters[ GetScintParIndex() ] = parVal; }
    void SetAVPar( Float_t parVal ){ fMrqParameters[ GetAVParIndex() ] = parVal; }
    void SetWaterPar( Float_t parVal ){ fMrqParameters[ GetWaterParIndex() ] = parVal; }

    void SetPar( Int_t nIndex, Float_t parVal ){ fMrqParameters[ nIndex ] = parVal; }

    // Notes for Laserball Distribution Model
    // QOCAFit: .cxx Function of CosTheta, Phi

  private:

    std::string fFitName;                                    // The name of the fit
    std::string fFitTitle;                                   // The title of the fit

    Bool_t fValidPars;                                       //! The LOCASFit structure has allocated valid parameters
    Bool_t fDataScreen;                                      //! The Data has been screened and filtered for reasonable tubes
    
    LOCASRunReader fRunReader;                               //! The Run reader to go over all the LOCASRun files
    LOCASRunReader fCentralRunReader;                        //! The Run reader to go over all the Central LOCASRun files
    LOCASRunReader fWavelengthRunReader;                     //! The Run reader to go over all the Wavelength LOCASRun files

    LOCASRun* fCurrentRun;                                   //! Pointer to the current LOCASRun object
    LOCASPMT* fCurrentPMT;                                   //! Pointer to the current LOCASPMT object
    LOCASRun* fCurrentCentralRun;                            //! Pointer to the current central LOCASRun object
    LOCASPMT* fCurrentCentralPMT;                            //! Pointer to the current central LOCASPMT object
    LOCASRun* fCurrentWavelengthRun;                         //! Pointer to the current wavelength LOCASRun object
    LOCASPMT* fCurrentWavelengthPMT;                         //! Pointer to the current wavelength LOCASPMT object

    Int_t fNRuns;                                            // Number of runs loaded into the fit
    Int_t fNCentralRuns;                                     // Number of central runs loaded
    Int_t fNWavelengthRuns;                                  // Number of wavelength runs loaded
    std::vector< Int_t > fListOfRunIDs;                      //! List of run IDs in the fit
    std::vector< Int_t > fListOfCentralRunIDs;               //! List of central run IDs used in the fit
    std::vector< Int_t > fListOfWavelengthRunIDs;            //! List of wavelength run IDs in the fit

    Bool_t fVaryAll;                                         // TRUE: All parameters in the fit were varied (FALSE: not)
    Bool_t fScintVary;                                       // Whether the scintillator attenuation length was varied
    Bool_t fAVVary;                                          // Whether the acrylic (AV) attenuation length was varied
    Bool_t fWaterVary;                                       // Whether the water attenuation length was varied
    Bool_t fAngularResponseVary;                             // Whether the angular response was varied 
    Bool_t fLBDistributionVary;                              // Whether the laserball distribution was varied

    Double_t fScintInit;                                     // Initial scintillator (1/attenuation length) value
    Double_t fAVInit;                                        // Initial acrylic (AV) (1/attenuation length) value
    Double_t fWaterInit;                                     // Initial water (1/attenuation length) value
    Double_t fAngularResponseInit;                           // Initial angular response value
    Double_t fLBDistributionInit;                            // Initial laserball distribution value

    TH2F* fLBDistribution;                                   // Pointer to the laserball distribution 2D histogram
    Int_t fNLBDistributionThetaBins;                         // Number of theta bins in the laserball distribution histogram
    Int_t fNLBDistributionPhiBins;                           // Number of phi bins in the laserball distribution histogram
    Int_t fNPMTsPerLBDistributionBinMin;                     // Minimum required number of PMTs per bin in the laserball distribution histogram

    TH1F* fAngularResponse;                                  // Pointer to the PMT angular response 1D histogram
    Int_t fNAngularResponseBins;                             // Number of (theta) bins in the angular response histogram
    Int_t fNPMTsPerAngularResponseBinMin;                    // Minimum required number of PMTs per bin in the angular response histogram

    Int_t fNParametersInFit;                                 // Total number of parameters in the fit
    Int_t fNDataPointsInFit;                                 // Total number of data points (including PMTs which do NOT pass the selection criteria)
    Int_t fNPMTsInFit;                                       // Total number of PMTs in the fit (PMTs which pass the selection criteria, see LOCASFit::PMTSkip method)
    Int_t fNPMTSkip;                                        // Number of PMTs to skip through (higher speeds up fitting, but with less data points)

    Float_t fChiSquare;                                      // Most recent ChiSquare value (not reduced)
    Float_t fChiSquareMaxLimit;                              // Maximum value of the chisquare for PMTs to be cut from the fit
    Float_t fChiSquareMinLimit;                              // Minimum value of the chisquare for PMTs to be cut from the fit

    Float_t fNSigma;                                         // The number of standard deviations away from the mean occupancy for a PMT to be cut on (see LOCASFit::PMTSkip method)
    Float_t fNOccupancy;                                     // The number of prompt counts for a PMT to be cut on (see LOCASFit::PMTSkip method)

    Float_t fAVHDShadowingMin;                               // The minimum value of of the AV hold-down rope shadowing for the PMTs to be cut on
    Float_t fAVHDShadowingMax;                               // The maximum value of of the AV hold-down rope shadowing for the PMTs to be cut on
    Float_t fGeoShadowingMin;                                // The minimum value of of the AV geometry shadowing for the PMTs to be cut on
    Float_t fGeoShadowingMax;                                // The maximum value of of the AV geometry shadowing for the PMTs to be cut on

    Bool_t fCHSFlag;                                         // The CHS Flag status for PMTs to be cut on
    Bool_t fCSSFlag;                                         // The CSS Flag status for PMTs to be cut on

    Int_t fNElements;                                        //! The number of elements in the fChiArray[] and fResArray[] array
    Float_t* fChiArray;                                      //! [fNElements] Array of chisquared for mrqcof() calls
    Float_t* fResArray;                                      //! [fNElements] Array of residuals for mrqcof() calls

    Int_t fiAng;                                             //! Index for PMT angular response to be re-zeroed
    Int_t fCiAng;                                            //! Index for PMT angular response in the central run to be re-zeroed
    Int_t fiLBDist;                                          //! Index for laserball distribution to be re-zeroed
    Int_t fCiLBDist;                                         //! Index for laserball distribution in the central run to be re-zeroed

    
    Int_t fParamBase;                                        // Number of variable parameters, not including PMT Response or LB Distribution
    Int_t fParam;                                            // Number of variable parameters, including PMT response and LB distribution
    Int_t*** fAngIndex;                                      //! Lookup table for unique variable parameters in the PMT response
    Int_t* fParamIndex;                                      //! Lookup table for ordered variable parameters
    Int_t* fParamVarMap;                                     //! Lookup table for variable parameters (global)

    // The arrays used by the Levenburg-Marquadt (Mrq) algorithm to find the parameters
    Float_t* fMrqX;                                          //! [fNDataPointsInFit+1] Index into the PMTs to be used in the fit
    Float_t* fMrqY;                                          //! [fNDataPointsInFit+1] Index into the PMTs (from above) corresponding OccRatio
    Float_t* fMrqSigma;                                      //! [fNDataPointsInFit+1] Error on each PMT occupancy (statistical)

    Float_t* fMrqParameters;                                 //! [fNParameters+1] Parameters for the model
    Int_t* fMrqVary;                                         //! [fNParameters+1] Flag for the parameters which vary in the model (from above)
    Float_t** fMrqCovariance;                                //! [fNParameters+1][fNParameters+1] Covariance matrix
    Float_t** fMrqAlpha;                                     //! [fNParameters+1][fNParameters+1] Curvature matrix

    std::map< Int_t, LOCASPMT > fFitPMTs;                    // Map of PMTs which pass the cut selection and are to be used in the fit
    std::map< Int_t, LOCASPMT >::iterator fiPMT;             //! Map iterator used when scanning through PMTs

    LOCASMath flMath;  //!

    ClassDef( LOCASFit, 1 )

  };

}

#endif

