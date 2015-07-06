////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAOpticsModel.hh
///
/// CLASS: OCA::OCAOpticsModel
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
///         for the occupancy ratio. For a given OCAPMT
///         object the method OCAModel::ModelPrediction can
///         be used to produce the predicted value for the
///         occupancy ratio.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAOpticsModel_
#define _OCAOpticsModel_

#include "OCAPMT.hh"
#include "OCAModelParameterStore.hh"
#include "OCAPMTStore.hh"

using namespace std;

namespace OCA{

  class OCAOpticsModel : public TObject
  {
  public:

    // The constructor and destructor for the OCAOpticsModel object.
    OCAOpticsModel( const string modelName = "" );
    ~OCAOpticsModel(){ }

    // Identify the number of entries in each PMT angular response bin
    // from the data points in the OCAPMTStore object 'lData'.
    // If the number of entries in a particular bin is less than the
    // required threshold value, then the parameter represented by that
    // bin wil be fixed in the fit.
    void IdentifyVaryingPMTAngularResponseBins( OCAPMTStore* lData );

    // Identify the number of entries in each laserball distribution bin
    // from the data points in the OCAPMTStore object 'lData'.
    // If the number of entries in a particular bin is less than the
    // required threshold value, then the parameter represented by that
    // bin wil be fixed in the fit.
    void IdentifyVaryingLBDistributionBins( OCAPMTStore* lData );

    // Initialise the laserball run normalisation parameters to the initial
    // off-axis values stored on the data point objects.
    void InitialiseLBRunNormalisations( OCAPMTStore* lData );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Given access to a data point, compute the model prediction for the occupancy ratio
    // and its associated derivatives with respect to each parameter.
    Float_t ModelOccRatioPrediction( const OCAPMT& dataPoint, Float_t* derivativePars = NULL );

    // Given access to a data point, compute the model prediction for the occupancy
    // and its associated derivatives with respect to each parameter.
    Float_t ModelPrediction( const OCAPMT& dataPoint );

    // Given access to a data point, compute the model prediction for the laserball distribution
    // based on the run type. runType = "off-axis" or runType = "central"
    // for the off-axis and central runs respectively.
    Float_t ModelLBDistribution( const OCAPMT& dataPoint, std::string runType, 
                                 Float_t* phiInterpolFrac, Float_t* cosThetaInterpolFrac, 
                                 Int_t* binsToDiff );

    // Given access to a data point, compute the model prediction for the laserball distribution
    // maske based on the run type. runType = "off-axis" or runType = "central". 
    // for the off-axis and central runs respectively.
    Float_t ModelLBDistributionMask( const OCAPMT& dataPoint, std::string runType );

    // Compute the derivative of the current laserball distribution mask value
    // with respect to each of the parameters in the mask.
    Float_t ModelLBDistributionMaskDeriviative( Double_t* aPtr, Double_t* parPtr );

    // Given access to a data point, compute the model prediction for the PMT angular response
    // based on the run type. runType = "off-axis" or runType = "central"
    // for the off-axis and central runs respectively.
    Float_t ModelAngularResponse( const OCAPMT& dataPoint, std::string runType, Float_t& interpolFrac );


    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of this OCAOpticsModel object.
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

    // Get the pointer to the OCAModelParameter store which holds the parameter
    // values used by this object to provide the model prediction.
    OCAModelParameterStore* GetOCAModelParameterStore() const { 
      return fModelParameterStore; 
    }
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of this OCAOpticsModel object.
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

    // Set the pointer to the OCAModelParameterStore object which will
    // provide the parameters used in the model prediction.
    void SetOCAModelParameterStore( OCAModelParameterStore* storePtr ){ 
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
    
    OCAModelParameterStore* fModelParameterStore;  // The parameter store of OCAModelParameter 
                                                     // objects.
    
    ClassDef( OCAOpticsModel, 1 );
    
  };
  
}

#endif
