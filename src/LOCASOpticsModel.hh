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
    LOCASOpticsModel( LOCASModelParameterStore& locasParams, 
                      const std::string modelName = "" );
    ~LOCASOpticsModel(){ delete fParameters; }

    void InitialiseParameters(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Float_t ModelPrediction( const LOCASDataPoint& dataPoint );

    Float_t ModelLBDistribution( const LOCASDataPoint& dataPoint );
    Float_t ModelAngularResponse( const LOCASDataPoint& dataPoint );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

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
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
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
    
    ClassDef( LOCASOpticsModel, 1 );
    
  };
  
}

#endif
