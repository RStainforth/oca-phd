////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAModelParameterStore.hh
///
/// CLASS: OCA::OCAModelParameterStore
///
/// BRIEF: A simple class to hold a collection of
///        OCAModelParameter objects which are used
///        by the OCAOpticsModel class.
///                  
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///     10/2017 : A.S.Inacio - Adapted code to receive LAA parameters as input.
///
/// DETAIL: This object holds several 
///         OCAModelParameter objects together which collectively
///         can be used by a OCAOpticsModel object to make
///         model predictions for the occuapncy ratio.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAModelParameterStore_
#define _OCAModelParameterStore_

#include "OCAModelParameter.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TMatrix.h"
#include "TVector.h"

#include <string>

using namespace std;

namespace OCA{

  class OCAModelParameterStore : public TObject
  {
  public:

    // The constructor and destructor of the OCAModelParameterStore object.
    OCAModelParameterStore() { }
    OCAModelParameterStore( string& storeName );
    ~OCAModelParameterStore();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a parameter to the store.
    void AddParameter( OCAModelParameter parameter ){ 
      fParameters.push_back( parameter ); 
    }

    // Seed Parameters from a pre-existing file
    Bool_t SeedParameters( string& seedFilename, 
                           string& fitFileName );
			   
    // Seed LB Sinusoidal Wave Parameters from a pre-existing file
    void SeedLBSinWaveParameters( string& fileName );

    // Add a complete set of parameters from a 'fit-file'.
    void AddParameters( string& fileName );

    // Print all the parameter information about each parameter in the store.
    void PrintParameterInfo();
    
    // Write the parameters to a .root file
    void WriteToROOTFile( string& fileName,
                          string& branchName );

    // Write the parameters to a .ocadb file
    void WriteToOCADBFile( const char* fileName = "OCAModelParameterStore.ocadb" );
		
    // Allocate the memory for the parameter arrays.
    void AllocateParameterArrays();

    // Initialise the PMT angular response look-up array.
    // Book keeping for the variable PMT angularresponse bins.
    void InitialisePMTAngularResponseIndex();

    // Identify all the current varying parameters for the last
    // evaluated data point. e.g. Laserball distribution and PMT angular response bins.
    void IdentifyVaryingParameters();

    // Identify all the current base varying parameters.
    // i.e. the parameters which vary for every data point in the fit. e.g. extinction lengths
    void IdentifyBaseVaryingParameters();

    // Identify all global varying parameters across all points.
    void IdentifyGlobalVaryingParameters();

    // After a fit, ensure both the pointer arrays and vector of 
    // OCAModelParameter objects hold the same values.
    void CrossCheckParameters();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of the systematic being applied (if at all).
    string GetSystematicName(){ return fSystematicName; }

    // Return the OCAModelParameter object corresponding to this index.
    OCAModelParameter GetParameter( Int_t parIndex ){ return fParameters[ parIndex ]; }

    // Return the value of a parameter by its index.
    Float_t GetParameterValue( Int_t parIndex ){ return fParameterValues( parIndex ); }

    // // Return the vector of the parameter values.
    TVector GetParameterValues(){ return fParameterValues; }
    
    // Return the value of a covariance matrix element by it's index.
    Float_t GetCovarianceMatrixValue( Int_t rowIndex, Int_t colIndex ){ return fCovarianceMatrixValues[ rowIndex ][ colIndex ]; }

    // // Return the value of a covariance matrix element by it's index.
    TMatrix GetCovarianceMatrixValues(){ return fCovarianceMatrixValues; }

    // Get the boolean flag to denote whether or not the parameters
    // were seeded.
    Bool_t GetSeededParameters(){ return fSeededParameters; }

    // Get the name of the fit which was used as the seed
    string GetSeedFile(){ return fSeedFile; }

    // Get the boolean flag to denote whether or not the water region and inner AV region share the same material (i.e. waterfill).
    Bool_t GetWaterFill(){ return fWaterFill; }

    // Get the pointer to the array of parameters.
    Float_t* GetParametersPtr() { return fParametersPtr; }

    // Get the pointer to the array of the varying parameter indices.
    Int_t* GetParametersVary() { return fParametersVary; }

    // Get the pointer to the array which represents the 
    // matrix of covariances.
    Float_t** GetCovarianceMatrix() { return fCovarianceMatrix; }
    
    // Get the pointer to the array which represents the
    // matrix of derivatives.
    Float_t** GetDerivativeMatrix() { return fDerivativeMatrix; }

    // Get the number of parameters in the store, and consequently.
    Int_t GetNParameters() const { return fNParameters; }

    // Get the current number of varying parameters for the last evaluated data point.
    Int_t GetNCurrentVariableParameters() const { return fNCurrentVariableParameters; }

    // Get the number of varying global parameters across all data points.
    Int_t GetNGlobalVariableParameters() const { return fNGlobalVariableParameters; }

    // Get the parameter indices of the parameter which 
    // vary for the last evaluated data point.
    Int_t* GetVariableParameterIndex() { return fVariableParameterIndex; }

    // Get the map of variable parameter indices (ordered) 
    // for the last evaluated data point.
    Int_t* GetVariableParameterMap() { return fVariableParameterMap; }

    // After evaluating a specific data point, get the current 
    // PMT angular response bin from the off-axis run.
    Int_t GetCurrentPMTAngularResponseBin() const { return fCurrentPMTAngularResponseBin; }

    // Get the current angular response distribution being used for the current data point.
    Int_t GetCurrentPMTAngularResponseDistribution() const { return fCurrentPMTAngularResponseDistribution; }

    // After evaluating a specific data point, get the current 
    // PMT angular response bin from the central run.
    Int_t GetCentralCurrentPMTAngularResponseBin() const { return fCentralCurrentPMTAngularResponseBin; }

    // After evaluating a specific data point, get the current 
    // laserball distribution bin from the off-axis run.
    Int_t GetCurrentLBDistributionBin() const { return fCurrentLBDistributionBin; }

    // After evaluating a specific data point, get the current 
    // laserball distribution bin from the central run.
    Int_t GetCentralCurrentLBDistributionBin() const { return fCentralCurrentLBDistributionBin; }

    // After evaluating a specific data point, get the current 
    // laserball run normalisation bin corresponding to the run index
    // of the data point associated with the run from whence it came.
    Int_t GetCurrentLBRunNormalisationBin() const { return fCurrentLBRunNormalisationBin; }

    // Get the number of laserball mask parameters.
    Int_t GetNLBDistributionMaskParameters() const { return fNLBDistributionMaskParameters; }

    // Get the number of pmt angular response bins.
    Int_t GetNPMTAngularResponseBins() const { return fNPMTAngularResponseBins; }

    // Get the number of pmt angular response distributions.
    Int_t GetNPMTAngularResponseDistributions() const { return fNPMTAngularResponseDistributions; }

    // Get the z-split values for which PMT angular responses are split
    Float_t GetPMTAngularResponseZSplit() const { return fPMTAngularResponseZSplit; }

    // Get the number of laserball distribution cos theta bins.
    Int_t GetNLBDistributionCosThetaBins(){ return fNLBDistributionCosThetaBins; }

    // Get the number of laserball distribution phi bins.
    Int_t GetNLBDistributionPhiBins() const { return fNLBDistributionPhiBins; }

    // Get the number of laserball run normalisations.
    Int_t GetNLBRunNormalisations() const { return fNLBRunNormalisations; }

    // Get the number of laserball run normalisations.
    Int_t GetNLBSinWaveSlices() const { return fNLBSinWaveSlices; }

    // Get the number of laserball run normalisations.
    Int_t GetNLBParametersPerSinWaveSlice() const { return fNLBParametersPerSinWaveSlice; }

    // Get the total number of laserball distribution parameters; regardles sof type (binned or sin-wave)
    Int_t GetNLBDistributionPars() const { return fNLBDistributionPars; }

    // Get the type of the laserball angular distribution (0: binned, 1: sin-wave)
    Int_t GetLBDistributionType() const { return fLBDistributionType; }

    // Get the index for the inner av extinction length.
    Int_t GetInnerAVExtinctionLengthParIndex() const { return 1; } 

    // Get the inner av extinction length parameter.
    Float_t GetInnerAVExtinctionLengthPar() { return fParametersPtr[ GetInnerAVExtinctionLengthParIndex() ]; }

    // Get the index for the acrylic extinction length parameter.
    Int_t GetAVExtinctionLengthParIndex() const { return 2; }

    // Get the av extinction length parameter.
    Float_t GetAVExtinctionLengthPar() { return fParametersPtr[ GetAVExtinctionLengthParIndex() ]; }

    // Get the index for the water extinction length parameter
    Int_t GetWaterExtinctionLengthParIndex() const { return 3; }

    // Get the water extinction length parameter.
    Float_t GetWaterExtinctionLengthPar() { return fParametersPtr[ GetWaterExtinctionLengthParIndex() ]; }

    // Get the index for the start of the laserball distribution mask parameters.
    Int_t GetLBDistributionMaskParIndex() const { return 3 + 1; }

    // Get the 'iPar'-th laserball distriubtion mask parameter.
    Float_t GetLBDistributionMaskPar( const Int_t iPar ) { return fParametersPtr[ GetLBDistributionMaskParIndex() + iPar ]; }

    // Get the index for the start of the pmt angular response parameters.
    Int_t GetPMTAngularResponseParIndex() const { return 3 + fNLBDistributionMaskParameters + 1; }

    // Get the index for the start of the pmt angular response parameters.
    Int_t GetPMTAngularResponse2ParIndex() const { return 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins + 1; }

    // Get the 'iPar'-th PMT angular response parameter.
    Float_t GetPMTAngularResponsePar( const Int_t iPar ) { return fParametersPtr[ GetPMTAngularResponseParIndex() + iPar ]; }

    // Get the 'iPar'-th PMT angular response2 parameter.
    Float_t GetPMTAngularResponse2Par( const Int_t iPar ) { return fParametersPtr[ GetPMTAngularResponse2ParIndex() + iPar ]; }
    
    // Get the index for the start of the laserball distribution hisotgram parameters.
    Int_t GetLBDistributionParIndex() const { return 3 + fNLBDistributionMaskParameters + ( fNPMTAngularResponseBins * fNPMTAngularResponseDistributions ) + 1; }

    // Get the 'iPar'-th laserball distribution parameter.
    Float_t GetLBDistributionPar( const Int_t iPar ) { return fParametersPtr[ GetLBDistributionParIndex() + iPar ]; } 
    
    // Get the index for the start of the run normalisation parameters.
    Int_t GetLBRunNormalisationParIndex() const { return 3 + fNLBDistributionMaskParameters 
        + ( fNPMTAngularResponseBins * fNPMTAngularResponseDistributions ) + fNLBDistributionPars + 1; }

    // Get the 'iPar'-th laserball normalisation parameter.
    // i.e. The off-axis normalisation value for the 'iPar'-th run in
    // the list of runs included in the fit from the 'fit-file'.
    Float_t GetLBRunNormalisationPar( const Int_t iRun ) { return fParametersPtr[ GetLBRunNormalisationParIndex() + iRun ]; }

    // Return the PMT angular response in the binned form as it is stored
    // in the model.
    TH1F* GetPMTAngularResponseHistogram( const Int_t dist = 0 );

    // Get the error on the 'nVal'-th PMT angular response parameter.
    Float_t GetPMTAngularResponseError( const Int_t nVal, const Int_t dist = 0 );

    // Return the PMT angular response as a function based off of the
    // binned form as it is stored in the model.
    TF1* GetPMTAngularResponseFunction( const Int_t dist = 0 );

    // Static function used to calculate the PMT angular response function
    // above. (OCAModelParameterStore::GetPMTAngularResponseFunction()).
    // Function returns the PMT angular response corresponding to 
    // angle a[0] in degrees, bounded by 0 and 90 degrees.
    // par[0] specifies number of parameters; par[1] through 
    // par[ par[ 0 ] ] are the parameters themselves.
    static Double_t SPMTAngularResponse( Double_t* a, Double_t* par );

    // Return the laserball isotropy distribution distribution in the
    // 2D histogram form as it is stored in the model.
    TH2F* GetLBDistributionHistogram();

    // Return the laserball mask function as a function based off of the
    // polynomial parameters as stored in the model.
    TF1* GetLBDistributionMaskFunction();

    // Get the error on the 'nVal'-th laserball distribution mask parameter.
    Float_t GetLBDistributionMaskError( const Int_t nVal );

    // Static function used to calculate the laserball mask function
    // above. (OCAModelParameterStore::GetLBDistributionMaskFunction()).
    // Function returns the laserball mask function corresponding to 
    // the polynomial as parameterised by the mask parameters.
    static Float_t SLBDistributionMask( Double_t* aPtr, Double_t* parPtr );

    // Return the combined (histogram + mask) histogram for the combined
    // laserball distribution and intensity based off of the 2D histogram
    // and the mask function parameters as they are stored in the model.
    TH2F* GetLBDistributionIntensityHistogram();

    // Get the iterators to the beginning and end of the parameter store.
    vector< OCAModelParameter >::iterator GetOCAModelParametersIterBegin(){ return fParameters.begin(); }
    vector< OCAModelParameter >::iterator GetOCAModelParametersIterEnd(){ return fParameters.end(); }

    // Get the vector of the current bins which are active on the current data point
    // for the laserball distribution.
    vector< Int_t >* GetCurrentLBDistributionBins(){ return fCurrentLBDistributionBins; }

    // Get the vector of the current bins which are active on the current data point
    // for the angular repsonse distribution.
    vector< Int_t >* GetCurrentAngularResponseBins(){ return fCurrentAngularResponseBins; }


    // Get the final value of the chisquare for this set of parameters.
    Float_t GetFinalChiSquare(){ return fFinalChiSquare; }

    // Get the number of data points from which these parameters were fitted.
    Int_t GetNumberOfDataPoints(){ return fNumberOfDataPoints; }

    // Get the reduced chi square value.
    Float_t GetReducedChiSquare(){ return fReducedChiSquare; }

    // Get the PMT variability parameters from this data-set
    TVector GetPMTVariabilityParameters(){ return fPMTVariabilityParameters; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of the systematic being applied (if at all).
    void SetSystematicName( string& sysName ){ fSystematicName = sysName; }

    // After evaluating a specific data point, set the current 
    // PMT angular response bin from the off-axis run.
    void SetCurrentPMTAngularRepsonseBin( const Int_t iBin ){ fCurrentPMTAngularResponseBin = iBin; }

    // Set the current angular response distribution being used for the current data point.
    void SetCurrentPMTAngularResponseDistribution( const Int_t iDist ){ fCurrentPMTAngularResponseDistribution = iDist; }

    // After evaluating a specific data point, set the current 
    // PMT angular response bin from the central run.
    void SetCentralCurrentPMTAngularResponseBin( const Int_t iBin ){ fCentralCurrentPMTAngularResponseBin = iBin; }

    // After evaluating a specific data point, set the current 
    // laserball distribtion bin from the off-axis run.
    void SetCurrentLBDistributionBin( const Int_t iBin ){ fCurrentLBDistributionBin = iBin; }

    // After evaluating a specific data point, set the current 
    // laserball distribution bin from the central run.
    void SetCentralCurrentLBDistributionBin( const Int_t iBin ){ fCentralCurrentLBDistributionBin = iBin; }

    // After evaluating a specific data point, set the current 
    // laserball run normalisation bin corresponding to the run index
    // of the data point associated with the run from whence it came.
    void SetCurrentLBRunNormalisationBin( const Int_t iBin ) { fCurrentLBRunNormalisationBin = iBin; }

    // Get the 'iPar'-th laserball distribution parameter.
    void SetLBDistributionPar( const Int_t iPar, const Float_t lbDist ) { fParametersPtr[ GetLBDistributionParIndex() + iPar ] = lbDist; }

    // Get the 'iPar'-th laserball normalisation parameter.
    // i.e. The off-axis normalisation value for the 'iPar'-th run in
    // the list of runs included in the fit from the 'fit-file'.
    void SetLBRunNormalisationPar( const Int_t iRun, const Float_t runNorm ) { fParametersPtr[ GetLBRunNormalisationParIndex() + iRun ] = runNorm; }

    // Set the parameters to be stored by this object to those held in 
    // an vector of other parameters.
    void SetParameters( const vector< OCA::OCAModelParameter >& pars ){ fParameters = pars; }

    // Set the parameter pointer array to those given by the 'pars' array.
    void SetParametersPtr( Float_t* pars ) { fParametersPtr = pars; }

    // Set the covariance matrix pointer array to those given 
    // by the 'covar' array of arrays.
    void SetCovarianceMatrix( Float_t** covar ) { fCovarianceMatrix = covar; }

    // Set the vector of the current bins which are active on the current data point
    // for the laserball distribution.
    void SetCurrentLBDistributionBins( vector< Int_t >& vec ){ *fCurrentLBDistributionBins = vec; }

    // Set the vector of the current bins which are active on the current data point
    // for the angular repsonse distribution.
    void SetCurrentAngularResponseBins( vector< Int_t >& vec ){ *fCurrentAngularResponseBins = vec; }

    // Set the final value of the chisquare for this set of parameters.
    void SetFinalChiSquare( Float_t& chiSq ){ fFinalChiSquare = chiSq; }

    // Set the number of data points from which these parameters were fitted.
    void SetNumberOfDataPoints( Int_t& numDP ){ fNumberOfDataPoints = numDP; }

    // Set the reduced chi square value.
    void SetReducedChiSquare( Float_t& chiRed ){ fReducedChiSquare = chiRed; }

    // Set the PMT variability parameters from this data-set
    void SetPMTVariabilityParameters( Float_t parZero, Float_t parOne, Float_t parTwo ){ 
      fPMTVariabilityParameters( 0 ) = parZero;
      fPMTVariabilityParameters( 1 ) = parOne;
      fPMTVariabilityParameters( 2 ) = parTwo;
    }

  private:

    string fStoreName;                                      // The store name.
    string fSystematicName;                                 // The systematic name.

    Bool_t fSeededParameters;                               // Boolean flag to denote whether or not the parameters were seeded.
    string fSeedFile;                                       // The name of the fit which was used as the seed.

    Bool_t fWaterFill;                                      // Boolean flag to denote wehther or not the inner AV and water regions share the same material (i.e. waterfill). 

    Int_t fCurrentLBDistributionBin;                        //! The current laserball distribution bin for the off-axis run.
    
    Int_t fCentralCurrentLBDistributionBin;                 //! The current laserball distribution for the central run.

    Int_t fCurrentPMTAngularResponseBin;                    //! The current angular response bin for the off-axis run.

    Int_t fCentralCurrentPMTAngularResponseBin;             //! The current angular response bin for the off-axis run.

    Int_t fCurrentPMTAngularResponseDistribution;          //! The current distribution for the PMT angular response (in the case of more than one)

    Int_t fCurrentLBRunNormalisationBin;                    //! The current index for the laserball normalisation.

    Int_t fNLBDistributionMaskParameters;                   // The number of laserball mask parameters.
    Int_t fNPMTAngularResponseBins;                         // The number of PMT Angular Response Bins.
    Int_t fNPMTAngularResponseDistributions;                // The number of PMT Angular Response Distributions (defaults is one, but more than one
                                                            // can be considered for asymmertry studies).
    Float_t fPMTAngularResponseZSplit;                      // In the case of more than one PMT angular response distribution, we can divide them by the
                                                            // z-coordinate in the of the PMT in the PSUP 
    Int_t fNLBDistributionCosThetaBins;                     // The number of bins in cos theta ( -1.0, 1.0 ) for the laserball distribution 2d histogram.
    Int_t fNLBDistributionPhiBins;                          // The number of bins in phi ( -0.0, 360.0 ) for the laserball distribution 2d hisotgram. (NOTE: SNO+ coordinates are ( -180.0, 180.0 )
    Int_t fNLBRunNormalisations;                            // The total number of run normalisations ( = number of laserball runs ).
    Int_t fNLBSinWaveSlices;                                // The total number of slices in theta for the sin-wave laserball angular distribution.
    Int_t fNLBParametersPerSinWaveSlice;                    // The number of parameters per sin wave theta slice in the sin-wave laserball angular distribution.
    Int_t fLBDistributionType;                              // Get the laserball angular distribution type (0: binned, 1: sin-wave).
    Int_t fNLBDistributionPars;                             // Total number of laserball distribution parameters; regardless of the type (binned or sin-wave)

    Int_t fNParameters;                                     // The number of parameters in the store.

    Int_t fNCurrentVariableParameters;                      //! The total number of current variable parameters.

    Int_t fNGlobalVariableParameters;                       // The total global number of variable parameters.

    vector< OCAModelParameter > fParameters;                // The vector of parameter objects i.e. the store.
    TVector fParameterValues;                               // The vector of the direct parameter values.
    TMatrix fCovarianceMatrixValues;                        // The covariance matrix of values.

    Float_t* fParametersPtr;                                //! The pointer of parameter values.
    Int_t* fParametersVary;                                 //! Array of which parameters vary ( =1 ) and which do not ( =0 ).

    Float_t** fCovarianceMatrix;                            //! [ fNParameters + 1 ][ fNParameters + 1 ] Covariance matrix.
    Float_t** fDerivativeMatrix;                            //! [ fNParameters + 1 ][ fNParameters + 1 ] Curvature matrix.

    Int_t fNBaseVariableParameters;                         //! The number of base variable parameters, 
                                                            // i.e. the parameters which vary that do not include:
                                                            // - PMT angular response
                                                            // - Laserball distribution histogram/sinusoidal parameters 
                                                            // - Laserball Run normalisations
                                                            // i.e. fNBaseVariableParameters + all the variable parameters in the following:
                                                            // - PMT angular response, 
                                                            // - Laserball distribution and run normalisations at the last evaluated data point.

    Int_t*** fPMTAngularResponseIndex;                      //! Look-up table for unique variable parameters in the PMT angular response.

    Int_t* fVariableParameterIndex;                         //! Look-up table for the ordered variable parameter indices.
    Int_t* fVariableParameterMap;                           //! Look-up table for the variable parameters (global).

    vector< Int_t >* fCurrentLBDistributionBins;            //!
    vector< Int_t >* fCurrentAngularResponseBins;           //!

    Float_t fFinalChiSquare;                                // Final ChiSquare value evaluated for this data set.
    Int_t fNumberOfDataPoints;                              // The number of data points for which the above value
                                                            // of the chisquare was evaluated.
    Float_t fReducedChiSquare;                              // ( fFinalChiSquare / ( fNumberDataPoints - fNGlobalVariableParameters );

    TVector fPMTVariabilityParameters;                      // This is a vector of 3 parameters which define the PMT 
                                                            // variability polynomial of the form:
                                                            //     pmt_variability( theta ) = p0 
                                                            //                                + p1 * theta
                                                            //                                + p2 * theta * theta
				
    vector< Double_t > fSinWavePars;			    // Stores the LB sin wave parameters if they are seeded 
    vector< Double_t > fSinWaveErr;			    // Stores the LB sin wave parameter errors if they are seeded 

    ClassDef( OCAModelParameterStore, 7 )

  };

}

#endif
