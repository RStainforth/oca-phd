////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASOpticsModel.hh
///
/// CLASS: LOCAS::LOCASOpticsModel
///
/// BRIEF: The class which defines the optical response model
///        of PMTs. This class inherits from LOCASModel.hh
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
#include "LOCASModel.hh"

namespace LOCAS{

  class LOCASOpticsModel : public LOCASModel
  {
  public:
    LOCASOpticsModel(){ }
    LOCASOpticsModel( const char* fileName );
    ~LOCASOpticsModel(){ if ( fParameters != NULL ){ delete[] fParameters; } }

    void AllocateParameters();
    void InitialiseParameterIndices();
    void InitialiseParameters();

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

    Int_t GetAngularResponsePar( const Int_t index ) { return fParameters[ GetAngularResponseParIndex() + index ]; }
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

    void SetParameters( const Double_t* params ) { *fParameters = *params; }
    void SetNParameters( const Int_t npars ){ fNParameters = npars; }
    
  private:
    
    Int_t fScintParIndex;
    Int_t fAVParIndex;
    Int_t fWaterParIndex;
    
    Int_t fScintRSParIndex;
    Int_t fAVRSParIndex;
    Int_t fWaterRSParIndex;
    
    Int_t fAngularResponseParIndex;
    Int_t fLBDistributionParIndex;
    
    Int_t fNAngularResponseBins;
    Int_t fNLBDistributionBins;
    Int_t fNLBDistributionThetaBins;
    Int_t fNLBDistributionPhiBins;

    std::string fModelName;
    Double_t* fParameters;
    Int_t fNParameters;
    LOCASModelParameterStore fModelParameterStore;
    
    ClassDef( LOCASOpticsModel, 1 );
    
  };
  
}

#endif
