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
    void Screen();
    Bool_t PMTSkip( LOCASPMT* pmt, Float_t mean, Float_t sigma );
    Float_t ModelPrediction( LOCASPMT* pmt );
    Float_t ModelPrediction( Int_t iPMT, Int_t nA, Float_t* dyda );
    Float_t CalculateChiSquare( LOCASPMT* pmt );                 // Calculate the chisquare
    //void CalculateChiSquare();                        // Calculate the chiSquare

    //void CopyParamterValues( LOCASFit* seedFit );    // Copy the parameter values from seedFit to THIS fit
    //void GiveParameterValues( LOCASFit* targetFit ); // Give the parameter values from THIS fit to targetFit

    Float_t ModelAngularResponse( LOCASPMT* locasPMT, Int_t& iAng );
    Float_t ModelLBDistribution( LOCASPMT* locasPMT, Int_t& iLBDist );

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

    Int_t GetNumberOfRuns(){ return fNumberOfRuns; }
    std::vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }
    
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

    // Notes for Laserball Distribution Model
    // QOCAFit: .cxx Function of CosTheta, Phi

  private:

    std::string fFitName;
    std::string fFitTitle;

    Bool_t fValidPars;             // The LOCASFit structure has allocated valid parameters
    Bool_t fDataScreen;            // The Data has been screened and filtered for reasonable tubes
    
    LOCASRunReader fRunReader;            // The Run reader to go over all the LOCASRun files
    LOCASRunReader fCentralRunReader;     // The Run reader to go over all the Central LOCASRun files
    LOCASRunReader fWavelengthRunReader;  // The Run reader to go over all the Wavelength LOCASRun files

    LOCASRun* fCurrentRun;     // Pointer to the current LOCASRun object
    LOCASPMT* fCurrentPMT;     // Pointer to the current LOCASPMT object

    Int_t fNumberOfRuns;
    std::vector< Int_t > fListOfRunIDs;
    std::vector< Int_t > fListOfCentralRunIDs;
    std::vector< Int_t > fListOfWavelengthRunIDs;

    Bool_t fVaryAll;
    Bool_t fScintVary;
    Bool_t fAVVary;
    Bool_t fWaterVary;
    Bool_t fAngularResponseVary;
    Bool_t fLBDistributionVary;

    Double_t fScintInit;
    Double_t fAVInit;
    Double_t fWaterInit;
    Double_t fAngularResponseInit;
    Double_t fLBDistributionInit;

    TH2F* fLBDistribution;
    Int_t fNLBDistributionThetaBins;
    Int_t fNLBDistributionPhiBins;
    Int_t fNPMTsPerLBDistributionBinMin;
    Int_t fLBDistIndex;

    TH1F* fAngularResponse;
    Int_t fNAngularResponseBins;
    Int_t fNPMTsPerAngularResponseBinMin;
    Int_t fAngRespIndex;

    Int_t fNParametersInFit;

    Int_t fNDataPointsInFit;
    Int_t fNPMTsInFit;

    Float_t fChiSquare;
    Float_t fChiSquareMaxLimit;
    Float_t fChiSquareMinLimit;

    Float_t fNSigma;

    Float_t fAVHDShadowingMin;
    Float_t fAVHDShadowingMax;
    Float_t fGeoShadowingMin;
    Float_t fGeoShadowingMax;

    Bool_t fCHSFlag;
    Bool_t fCSSFlag;

    Float_t* fChiArray;
    Float_t* fResArray;

    Int_t fiAng;
    Int_t fCiAng;
    Int_t fiLBDist;
    Int_t fCiLBDist;


    // The arrays used by the Levenburg-Marquadt (Mrq) algorithm to find the parameters
    Float_t* fMrqX;
    Float_t* fMrqY;
    Float_t* fMrqSigma;

    Float_t* fMrqParameters;
    Int_t* fMrqVary;
    Float_t** fMrqCovariance;
    Float_t** fMrqAlpha; 

    std::map< Int_t, LOCASPMT > fFitPMTs;

    ClassDef( LOCASFit, 1 )

  };

}

#endif

