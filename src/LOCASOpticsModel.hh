////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASOpticsModel.hh
///
/// CLASS: LOCAS::LOCASOpticsModel
///
/// BRIEF: The class which defines the optical response model
///        of PMTs.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASOpticsModel_
#define _LOCASOpticsModel_

#include "LOCASDataPoint.hh"
#include "LOCASModelParameterStore.hh"

namespace LOCAS{

  class LOCASOpticsModel : public TObject
  {
  public:

    // The constructors
    LOCASOpticsModel();
    //LOCASOpticsModel( const char* fileName );

    // The destructor - delete the parameters
    ~LOCASOpticsModel();
 
    // void ModelSetup( const char* fileName );
    
    // void AllocateParameters();
    // void InitialiseParameterIndices();
    // void InitialiseParameters();

    // void ReInitialiseParameters() { fModelParameterStore.ReInitialiseParameters( fParameters ); }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    virtual Float_t ModelPrediction( const LOCASDataPoint& dataPoint, Float_t* derivativePars = NULL );

    Float_t ModelLBDistribution( const LOCASDataPoint& dataPoint, std::string runType );

    Float_t ModelLBDistributionMask( const LOCASDataPoint& dataPoint, std::string runType );
    Float_t ModelLBDistributionMaskDeriviative( Double_t* aPtr, Double_t* parPtr );

    Float_t ModelAngularResponse( const LOCASDataPoint& dataPoint, std::string runType );

    //void IdentifyVaryingParameters();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Float_t GetPar( const Int_t index ) { return fParameters[ index ]; }

    // Float_t GetScintPar() { return fParameters[ fScintParIndex ]; }
    // Float_t GetAVPar() { return fParameters[ fAVParIndex ]; }
    // Float_t GetWaterPar() { return fParameters[ fWaterParIndex ]; }

    // Float_t GetScintRSPar() { return fParameters[ fScintRSParIndex ]; }
    // Float_t GetAVRSPar() { return fParameters[ fAVRSParIndex ]; }
    // Float_t GetWaterRSPar() { return fParameters[ fWaterRSParIndex ]; }

    // Float_t GetAngularResponsePar( const Int_t index ) { return fParameters[ (fAngularResponseParIndex + index) ]; }
    // Float_t GetLBDistributionPar( const Int_t index ) { return fParameters[ GetLBDistributionParIndex() + index ]; }

    // Float_t GetLBNormalisationPar( const Int_t index ) { return fParameters[ (fLBNormalisationParIndex + index) ]; }

    // Int_t GetScintParIndex() const { return fScintParIndex; }
    // Int_t GetAVParIndex() const { return fAVParIndex; }
    // Int_t GetWaterParIndex() const { return fWaterParIndex; }

    // Int_t GetScintRSParIndex() const { return fScintRSParIndex; }
    // Int_t GetAVRSParIndex() const { return fAVRSParIndex; }
    // Int_t GetWaterRSParIndex() const { return fWaterRSParIndex; }

    // Int_t GetAngularResponseParIndex() const { return fAngularResponseParIndex; }
    // Int_t GetLBDistributionParIndex() const { return fLBDistributionParIndex; }
    // Int_t GetLBDistributionMaskParIndex() const { return fLBDistributionMaskParIndex; }

    // Int_t GetLBNormalisationParIndex() const { return fLBNormalisationParIndex; }

    // Int_t GetNAngularResponseBins() const { return fNAngularResponseBins; }
    // Int_t GetNLBDistributionBins() const { return fNLBDistributionBins; }
    // Int_t GetNLBDistributionThetaBins() const { return fNLBDistributionThetaBins; }
    // Int_t GetNLBDistributionPhiBins() const { return fNLBDistributionPhiBins; }
    // Int_t GetNLBDistributionMaskParameters() const { return fNLBDistributionMaskParameters; }

    // Double_t* GetParameters() const { return fParameters; }
    // Int_t GetNParameters() const { return fNParameters; }
    // Int_t GetNCurrentVaryingParameters() const { return fNCurrentVaryingParameters; }

    // std::vector< LOCASModelParameter >::iterator GetParametersIterBegin(){ return fModelParameterStore->GetLOCASModelParametersIterBegin(); }
    // std::vector< LOCASModelParameter >::iterator GetParametersIterEnd(){ return fModelParameterStore->GetLOCASModelParametersIterEnd(); }

    LOCASModelParameterStore* GetLOCASModelParameterStore() { return fModelParameterStore; }
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // void SetParameters( Double_t pars[] ) { fParameters = pars; }
    // void SetPar( const Int_t index, const Float_t val ) { fParameters[ index ] = val; }

    // void SetScintPar( const Float_t val ) { fParameters[ fScintParIndex ] = val; }
    // void SetAVPar( const Float_t val ) { fParameters[ fAVParIndex ] = val; }
    // void SetWaterPar( const Float_t val ) { fParameters[ fWaterParIndex ] = val; }

    // void SetScintRSPar( const Float_t val ) { fParameters[ fScintRSParIndex ] = val; }
    // void SetAVRSPar( const Float_t val ) { fParameters[ fAVRSParIndex ] = val; }
    // void SetWaterRSPar( const Float_t val ) { fParameters[ fWaterRSParIndex ] = val; }

    // void SetLBNormalisationPar( const Int_t index, const Float_t val ){ fParameters[ fLBNormalisationParIndex + index ] = val; }
    
    // void SetScintParIndex( const Int_t nIndex ){ fScintParIndex = nIndex; }
    // void SetAVParIndex( const Int_t nIndex ){ fAVParIndex = nIndex; }
    // void SetWaterParIndex( const Int_t nIndex ){ fWaterParIndex = nIndex; }
    
    // void SetScintRSParIndex( const Int_t nIndex ){ fScintRSParIndex = nIndex; }
    // void SetAVRSParIndex( const Int_t nIndex ){ fAVRSParIndex = nIndex; }
    // void SetWaterRSParIndex( const Int_t nIndex ){ fWaterRSParIndex = nIndex; }
    
    // void SetAngularResponseParIndex( const Int_t nIndex ){ fAngularResponseParIndex = nIndex; }
    // void SetLBDistributionParIndex( const Int_t nIndex ){ fLBDistributionParIndex = nIndex; }
    // void SetLBDistributionMaskParIndex( const Int_t nIndex ){ fLBDistributionMaskParIndex = nIndex; }

    // void SetLBNormalisationParIndex( const Int_t nIndex ){ fLBNormalisationParIndex = nIndex; }
    
    // void SetNAngularResponseBins( const Int_t val ){ fNAngularResponseBins = val; }
    // void SetNLBDistributionBins( const Int_t val ){ fNLBDistributionBins = val; }
    // void SetNLBDistributionThetaBins( const Int_t val ){ fNLBDistributionThetaBins = val; }
    // void SetNLBDistributionPhiBins( const Int_t val ){ fNLBDistributionPhiBins = val; }
    // void SetNLBDistributionMaskParameters( const Int_t val ){ fNLBDistributionMaskParameters = val; }

    // void SetNParameters( const Int_t npars ){ fNParameters = npars; }
    // void SetNCurrentVaryingParameters( const Int_t npars ){ fNCurrentVaryingParameters = npars; }

    void SetLOCASModelParameterStore( LOCASModelParameterStore* storePtr ){ fModelParameterStore = storePtr; }
    
  private:
    
    // Int_t fScintParIndex;                            // The index of the scintillator region material attenuation length
    // Int_t fAVParIndex;                               // The index of the av region material attenuation length
    // Int_t fWaterParIndex;                            // The index of the water region material attenuation length
    
    // Int_t fScintRSParIndex;                          // The index of the scintillator region material scattering length
    // Int_t fAVRSParIndex;                             // The index of the av region material scattering length
    // Int_t fWaterRSParIndex;                          // The index of the water region material scattering length
    
    // Int_t fAngularResponseParIndex;                  // The first index of the angular response distribution parameters
    // Int_t fLBDistributionParIndex;                   // The first index of the laserball distribution parameters
    // Int_t fLBDistributionMaskParIndex;               // The first index of the laserball polynomial mask function

    // Int_t fLBNormalisationParIndex;                  // The first index of the laserball normalisation parameters
    
    // Int_t fNAngularResponseBins;                     // The number of angular response bins
    // Int_t fNLBDistributionBins;                      // The number of laserball distribution bins
    // Int_t fNLBDistributionThetaBins;                 // The number of Theta bins in the laserball distribution
    // Int_t fNLBDistributionPhiBins;                   // The number of Phi bins in the laserball distribution
    // Int_t fNLBDistributionMaskParameters;            // The number of laserball distribution mask function parameters

    // Int_t fCurrentLBDistributionBin;                 // The current laserball distribution bin for the off-axis run
    
    // Int_t fCentralCurrentLBDistributionBin;          // The current laserball distribution for the central run

    // Int_t fCurrentAngularResponseBin;                // The current angular response bin for the off-axis run

    // Int_t fCurrentCentralAngularResponseBin;         // The current angular response bin for the off-axis run

    // Int_t fCurrentLBNormalisationBin;                // The current index for the laserball normalisation

    std::string fModelName;                          // The model name
    // Double_t* fParameters;                           // The parameter array

    // Int_t fNParameters;                              // The total number of parameters
    // Int_t fNCurrentVaryingParameters;                // The current number of varying parameters at the current data point
    // Int_t fNBaseVaryingParameters;                   // The base number of varying parameters for all data points
    // Int_t* fParameterIndex;                          // Lookup table for ordered variable parameters
    // Int_t* fVaryingParameterIndex;                   // Lookup table for variable parameters (global)

    LOCASModelParameterStore* fModelParameterStore;  // The parameter store of LOCASModelParameter objects whose values are stored in the fParameters array
    
    ClassDef( LOCASOpticsModel, 1 );
    
  };
  
}

#endif
