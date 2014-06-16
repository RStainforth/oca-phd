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
    LOCASOpticsModel(){ }
    LOCASOpticsModel( const char* fileName );

    // The destructor - delete the parameters
    ~LOCASOpticsModel(){ if ( fParameters != NULL ){ delete[] fParameters; } }

    
    void AllocateParameters();
    void InitialiseParameterIndices();
    void InitialiseParameters();

    void ReInitialiseParameters() { fModelParameterStore.ReInitialiseParameters( fParameters ); }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    virtual Float_t ModelPrediction( const LOCASDataPoint& dataPoint );

    Float_t ModelLBDistribution( const LOCASDataPoint& dataPoint, Int_t runType );
    Float_t ModelAngularResponse( const LOCASDataPoint& dataPoint, Int_t runType );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Float_t GetPar( const Int_t index ) { return fParameters[ index ]; }

    Int_t GetScintPar() { return fParameters[ fScintParIndex ]; }
    Int_t GetAVPar() { return fParameters[ fAVParIndex ]; }
    Int_t GetWaterPar() { return fParameters[ fWaterParIndex ]; }

    Int_t GetScintRSPar() { return fParameters[ fScintRSParIndex ]; }
    Int_t GetAVRSPar() { return fParameters[ fAVRSParIndex ]; }
    Int_t GetWaterRSPar() { return fParameters[ fWaterRSParIndex ]; }

    Int_t GetAngularResponsePar( const Int_t index ) { return fParameters[ (fAngularResponseParIndex + index) ]; }
    Int_t GetLBDistributionPar( const Int_t index ) { return fParameters[ GetLBDistributionParIndex() + index ]; }

    Int_t GetScintParIndex() const { return fScintParIndex; }
    Int_t GetAVParIndex() const { return fAVParIndex; }
    Int_t GetWaterParIndex() const { return fWaterParIndex; }

    Int_t GetScintRSParIndex() const { return fScintRSParIndex; }
    Int_t GetAVRSParIndex() const { return fAVRSParIndex; }
    Int_t GetWaterRSParIndex() const { return fWaterRSParIndex; }

    Int_t GetAngularResponseParIndex() const { return fAngularResponseParIndex; }
    Int_t GetLBDistributionParIndex() const { return fLBDistributionParIndex; }

    Int_t GetNAngularResponseBins() const { return fNAngularResponseBins; }
    Int_t GetNLBDistributionBins() const { return fNLBDistributionBins; }
    Int_t GetNLBDistributionThetaBins() const { return fNLBDistributionThetaBins; }
    Int_t GetNLBDistributionPhiBins() const { return fNLBDistributionPhiBins; }

    Double_t* GetParameters() const { return fParameters; }
    Int_t GetNParameters() const { return fNParameters; }

    std::vector< LOCASModelParameter >::iterator GetParametersIterBegin(){ return fModelParameterStore.GetLOCASModelParametersIterBegin(); }
    std::vector< LOCASModelParameter >::iterator GetParametersIterEnd(){ return fModelParameterStore.GetLOCASModelParametersIterEnd(); }
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetParameters( const Double_t* pars ) { *fParameters = *pars; }
    void SetPar( const Int_t index, const Float_t val ) { fParameters[ index ] = val; }

    void SetScintPar( const Float_t val ) { fParameters[ fScintParIndex ] = val; }
    void SetAVPar( const Float_t val ) { fParameters[ fAVParIndex ] = val; }
    void SetWaterPar( const Float_t val ) { fParameters[ fWaterParIndex ] = val; }

    void SetScintRSPar( const Float_t val ) { fParameters[ fScintRSParIndex ] = val; }
    void SetAVRSPar( const Float_t val ) { fParameters[ fAVRSParIndex ] = val; }
    void SetWaterRSPar( const Float_t val ) { fParameters[ fWaterRSParIndex ] = val; }
    
    void SetScintParIndex( const Int_t nIndex ){ fScintParIndex = nIndex; }
    void SetAVParIndex( const Int_t nIndex ){ fAVParIndex = nIndex; }
    void SetWaterParIndex( const Int_t nIndex ){ fWaterParIndex = nIndex; }
    
    void SetScintRSParIndex( const Int_t nIndex ){ fScintRSParIndex = nIndex; }
    void SetAVRSParIndex( const Int_t nIndex ){ fAVRSParIndex = nIndex; }
    void SetWaterRSParIndex( const Int_t nIndex ){ fWaterRSParIndex = nIndex; }
    
    void SetAngularResponseParIndex( const Int_t nIndex ){ fAngularResponseParIndex = nIndex; }
    void SetLBDistributionParIndex( const Int_t nIndex ){ fLBDistributionParIndex = nIndex; }
    
    void SetNAngularResponseBins( const Int_t val ){ fNAngularResponseBins = val; }
    void SetNLBDistributionBins( const Int_t val ){ fNLBDistributionBins = val; }
    void SetNLBDistributionThetaBins( const Int_t val ){ fNLBDistributionThetaBins = val; }
    void SetNLBDistributionPhiBins( const Int_t val ){ fNLBDistributionPhiBins = val; }

    void SetNParameters( const Int_t npars ){ fNParameters = npars; }
    
  private:
    
    Int_t fScintParIndex;                 // The index of the scintillator region material attenuation length
    Int_t fAVParIndex;                    // The index of the av region material attenuation length
    Int_t fWaterParIndex;                 // The index of the water region material attenuation length
    
    Int_t fScintRSParIndex;               // The index of the scintillator region material scattering length
    Int_t fAVRSParIndex;                  // The index of the av region material scattering length
    Int_t fWaterRSParIndex;               // The index of the water region material scattering length
    
    Int_t fAngularResponseParIndex;       // The first index of the angular response distribution parameters
    Int_t fLBDistributionParIndex;        // The first index of the laserball distribution parameters
    
    Int_t fNAngularResponseBins;          // The number of angular response bins
    Int_t fNLBDistributionBins;           // The number of laserball distribution bins
    Int_t fNLBDistributionThetaBins;      // The number of Theta bins in the laserball distribution
    Int_t fNLBDistributionPhiBins;        // The number of Phi bins in the laserball distribution

    std::string fModelName;               // The model name
    Double_t* fParameters;                // The parameter array
    Int_t fNParameters;                   // The total number of parameters
    LOCASModelParameterStore fModelParameterStore;   // The parameter store of LOCASModelParameter objects whose values are stored in the fParameters array
    
    ClassDef( LOCASOpticsModel, 1 );
    
  };
  
}

#endif
