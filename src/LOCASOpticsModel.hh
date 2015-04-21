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
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This class computes the model predicted value
///         for the occupancy ratio. For a given LOCASDataPoint
///         object the method LOCASModel::ModelPrediction can
///         be used to produce the predicted value for the
///         occupancy ratio.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASOpticsModel_
#define _LOCASOpticsModel_

#include "LOCASDataPoint.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASDataStore.hh"

using namespace std;

namespace LOCAS{

  class LOCASOpticsModel : public TObject
  {
  public:

    // The constructor and destructor for the LOCASOpticsModel object.
    LOCASOpticsModel( const string modelName = "" );
    ~LOCASOpticsModel(){ }

    // Identify the number of entries in each PMT angular response bin
    // from the data points in the LOCASDataStore object 'lData'.
    // If the number of entries in a particular bin is less than the
    // required threshold value, then the parameter represented by that
    // bin wil be fixed in the fit.
    void IdentifyVaryingPMTAngularResponseBins( LOCASDataStore* lData );

    // Identify the number of entries in each laserball distribution bin
    // from the data points in the LOCASDataStore object 'lData'.
    // If the number of entries in a particular bin is less than the
    // required threshold value, then the parameter represented by that
    // bin wil be fixed in the fit.
    void IdentifyVaryingLBDistributionBins( LOCASDataStore* lData );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Given access to a data point, compute the model prediction for the occupancy ratio
    // and its associated derivatives with respect to each parameter.
    Float_t ModelPrediction( const LOCASDataPoint& dataPoint, Float_t* derivativePars = NULL );

    // Given access to a data point, compute the model prediction for the laserball distribution
    // based on the run type. runType = "off-axis" or runType = "central"
    // for the off-axis and central runs respectively.
    Float_t ModelLBDistribution( const LOCASDataPoint& dataPoint, std::string runType );

    // Given access to a data point, compute the model prediction for the laserball distribution
    // maske based on the run type. runType = "off-axis" or runType = "central". 
    // for the off-axis and central runs respectively.
    Float_t ModelLBDistributionMask( const LOCASDataPoint& dataPoint, std::string runType );

    // Compute the derivative of the current laserball distribution mask value
    // with respect to each of the parameters in the mask.
    Float_t ModelLBDistributionMaskDeriviative( Double_t* aPtr, Double_t* parPtr );

    // Given access to a data point, compute the model prediction for the PMT angular response
    // based on the run type. runType = "off-axis" or runType = "central"
    // for the off-axis and central runs respectively.
    Float_t ModelAngularResponse( const LOCASDataPoint& dataPoint, std::string runType );


    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of this LOCASOpticsModel object.
    string GetModelName() const { return fModelName; }

    // Get the minimum number of entries required per bin
    // in the PMT angular response for the parameter which 
    // that bin represents to vary in the fit.
    Int_t GetRequiredNPMTAngularResponseEntries() const { 
      return fRequiredNPMTAngularResponseEntries;
    }

    // Get the minimum number of entries required per bin
    // in the laserball distribution for the parameter which 
    // that bin represents to vary in the fit. 
    Int_t GetRequiredNLBDistributionEntries() const { 
      return fRequiredNLBDistributionEntries;
    }

    // Get the pointer to the LOCASModelParameter store which holds the parameter
    // values used by this object to provide the model prediction.
    LOCASModelParameterStore* GetLOCASModelParameterStore() const { 
      return fModelParameterStore; 
    }
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of this LOCASOpticsModel object.
    void SetModelName( const string modelName ){ fModelName = modelName; }

    // Set the minimum number of entries required per bin
    // in the PMT angular response for the parameter which 
    // that bin represents to vary in the fit.
    void SetRequiredNPMTAngularRepsonseEntries( const Int_t iBin ){ 
      fRequiredNPMTAngularResponseEntries = iBin;
    }

    // Set the minimum number of entries required per bin
    // in the laserball distribution for the parameter which 
    // that bin represents to vary in the fit.    
    void SetRequiredNLBDistributionEntries( const Int_t iBin ){ 
      fRequiredNLBDistributionEntries = iBin;
    }

    // Set the pointer to the LOCASModelParameterStore object which will
    // provide the parameters used in the model prediction.
    void SetLOCASModelParameterStore( LOCASModelParameterStore* storePtr ){ 
      fModelParameterStore = storePtr; 
    }
    
  private:

    string fModelName;                               // The model name.

    Int_t fRequiredNPMTAngularResponseEntries;       // Required number of entries in each
                                                     // PMT angular response bin for the parameter
                                                     // that bin represents to vary in the fit.

    Int_t fRequiredNLBDistributionEntries;           // Required number of entries in each
                                                     // laserball distribution bin for the parameter
                                                     // that bin represents to vary in the fit.
    
    LOCASModelParameterStore* fModelParameterStore;  // The parameter store of LOCASModelParameter 
                                                     // objects.
    
    ClassDef( LOCASOpticsModel, 1 );
    
  };
  
}

#endif
