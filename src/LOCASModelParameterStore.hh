////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModelParameterStore.hh
///
/// CLASS: LOCAS::LOCASModelParameterStore
///
/// BRIEF: A simple class to hold a collection of
///        LOCASModelParameter objects which are used
///        by a LOCASModel class
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASModelParameterStore_
#define _LOCASModelParameterStore_

#include "LOCASModelParameter.hh"
#include "LOCASMath.hh"

#include <string>
#include <map>

namespace LOCAS{

  class LOCASModelParameterStore : public TObject, LOCASMath
  {
  public:

    // The constructor
    LOCASModelParameterStore ( std::string storeName = "" );

    // The destructor - nothing to delete
    ~LOCASModelParameterStore(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a parameter to the store
    void AddParameter( LOCASModelParameter parameter, Int_t index ){ fParameters.push_back( parameter ); }

    // Add a complete set of parameters from a cardfile
    void AddParameters( const char* fileName );

    // Print all the parameter information about each parameter in the store
    void PrintParameterInfo();
    
    // Write the parameters to a .root file
    void WriteToFile( const char* fileName = "LOCASModelParameterStore.root" );

    // Reinitialise the initial values of the parameters based on the values in the array; 'pars'
    void ReInitialiseParameters( const Double_t* par );

    // Allocate the memory for the parameter arrays
    void AllocateParameterArrays();

    // Initialise the PMT angular response look-up array ( **FILLANGINDEX** )
    void InitialisePMTAngularResponseIndex();

    // Identify all the current varying parameters ( **FILLPARAMETERPOINT** )
    void IdentifyVaryingParameters();

    // Identify all the current base varying parameters ( **FILLPARAMETERBASE** )
    void IdentifyBaseVaryingParameters();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the pointer to the parameters
    Float_t* GetParametersPtr() { return fParametersPtr; }

    // Get the pointer to the varying parameter index
    Int_t* GetParametersVary() { return fParametersVary; }

    // Get the number of parameters in the store
    Int_t GetNParameters() const { return fNParameters; }

    // Get the current number of varying parameters
    Int_t GetNCurrentVariableParameters() const { return fNCurrentVariableParameters; }

    // Get the parameter indices of the parameter which vary for the last evaluated data point
    Int_t* GetVariableParameterIndex() { return fVariableParameterIndex; }

    // Get the map of variable parameter indices (ordered) for the las evaluated data point
    Int_t* GetVariableParameterMap() { return fVariableParameterMap; }

    // Get the current PMT angular response bin
    Int_t GetCurrentPMTAngularResponseBin() const { return fCurrentPMTAngularResponseBin; }

    // Get the current PMT angular response bin from the central run
    Int_t GetCentralCurrentPMTAngularResponseBin() const { return fCentralCurrentPMTAngularResponseBin; }

    // Get the current laserball distribution bin
    Int_t GetCurrentLBDistributionBin() const { return fCurrentLBDistributionBin; }

    // Get the current laserball distribution bin from the central run
    Int_t GetCentralCurrentLBDistributionBin() const { return fCentralCurrentLBDistributionBin; }

    // Get the current laserball run normalisation bin
    Int_t GetCurrentLBRunNormalisationBin() const { return fCurrentLBRunNormalisationBin; }

    // Get the number of laserball mask parameters
    Int_t GetNLBDistributionMaskParameters() const { return fNLBDistributionMaskParameters; }

    // Get the number of pmt angular response bins
    Int_t GetNPMTAngularResponseBins() const { return fNPMTAngularResponseBins; }

    // Get the number of laserball distribution cos theta bins
    Int_t GetNLBDistributionCosThetaBins(){ return fNLBDistributionCosThetaBins; }

    // Get the number of laserball distribution phi bins
    Int_t GetNLBDistributionPhiBins() const { return fNLBDistributionPhiBins; }

    // Get the number of laserball run normalisations
    Int_t GetNLBRunNormalisations() const { return fNLBRunNormalisations; }

    // Get the index for the inner av extinction length
    Int_t GetInnerAVExtinctionLengthParIndex() const { return 1; }   
    Float_t GetInnerAVExtinctionLengthPar() { return fParametersPtr[ GetInnerAVExtinctionLengthParIndex() ]; }

    // Get the index for the acrylic extinction length parameter
    Int_t GetAVExtinctionLengthParIndex() const { return 2; }
    Float_t GetAVExtinctionLengthPar() { return fParametersPtr[ GetAVExtinctionLengthParIndex() ]; }

    // Get the index for the water extinction length parameter
    Int_t GetWaterExtinctionLengthParIndex() const { return 3; }
    Float_t GetWaterExtinctionLengthPar() { return fParametersPtr[ GetWaterExtinctionLengthParIndex() ]; }

    // Get the index for the start of the laserball distribution mask parameters
    Int_t GetLBDistributionMaskParIndex() const { return 3 + 1; }
    Float_t GetLBDistributionMaskPar( const Int_t iPar ) { return fParametersPtr[ GetLBDistributionMaskParIndex() + iPar ]; }

    // Get the index for the start of the pmt angular response parameters
    Int_t GetPMTAngularResponseParIndex() const { return 3 + fNLBDistributionMaskParameters + 1; }
    Float_t GetPMTAngularResponsePar( const Int_t iPar ) { return fParametersPtr[ GetPMTAngularResponseParIndex() + iPar ]; }
    
    // Get the index for the start of the laserball distribution hisotgram parameters
    Int_t GetLBDistributionParIndex() const { return 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins + 1; }
    Float_t GetLBDistributionPar( const Int_t iPar ) { return fParametersPtr[ GetLBDistributionParIndex() + iPar ]; } 
    
    // Get the index for the start of the run normalisation parameters
    Int_t GetLBRunNormalisationParIndex() const { return 3 + fNLBDistributionMaskParameters 
        + fNPMTAngularResponseBins + ( fNLBDistributionCosThetaBins * fNLBDistributionPhiBins ) + 1; }
    Float_t GetLBRunNormalisationPar( const Int_t iRun ) { return fParametersPtr[ GetLBRunNormalisationParIndex() + iRun ]; }

    // Get iterators to the beginning and end of the parameter store
    std::vector< LOCASModelParameter >::iterator GetLOCASModelParametersIterBegin(){ return fParameters.begin(); }
    std::vector< LOCASModelParameter >::iterator GetLOCASModelParametersIterEnd(){ return fParameters.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetNParameters( const Int_t nPars ){ fNParameters = nPars; }

    void SetCurrentLBDistributionBin( const Int_t iBin ){ fCurrentLBDistributionBin = iBin; }

    void SetCentralCurrentLBDistributionBin( const Int_t iBin ){ fCentralCurrentLBDistributionBin = iBin; }

    void SetCurrentPMTAngularRepsonseBin( const Int_t iBin ){ fCurrentPMTAngularResponseBin = iBin; }

    void SetCentralCurrentPMTAngularResponseBin( const Int_t iBin ){ fCentralCurrentPMTAngularResponseBin = iBin; }

    void SetCurrentLBRunNormalisationBin( const Int_t iBin ) { fCurrentLBRunNormalisationBin = iBin; }

    void SetParameters( const std::vector< LOCAS::LOCASModelParameter >& pars ){ fParameters = pars; }

    void SetParametersPtr( Float_t* pars ) { *fParametersPtr = *pars; }

  private:

    std::string fStoreName;                          // The store name

    Int_t fNLBDistributionMaskParameters;            // The number of laserball mask parameters
    Int_t fNPMTAngularResponseBins;                  // The number of PMT Angular Response Bins
    Int_t fNLBDistributionCosThetaBins;              // The number of bins in cos theta ( -1.0, 1.0 ) for the laserball distribution 2d histogram
    Int_t fNLBDistributionPhiBins;                   // The number of bins in phi ( 0.0, 360.0 ) for the laserball distribution 2d hisotgram
    Int_t fNLBRunNormalisations;                     // The total number of run normalisations ( = number of laserball runs )

    Int_t fNParameters;                              // The number of parameters in the store

    std::vector< LOCASModelParameter > fParameters;  // The vector of parameter objects i.e. the store

    Float_t* fParametersPtr;                        // The pointer of parameter values ( ** FMRQPARAMETERS** )
    Int_t* fParametersVary;                          // Array of which parameters vary ( =1 ) and which do not ( =0 ) ( **FMRQVARY** )

    Int_t fNBaseVariableParameters;                  // The number of base variable parameters, 
                                                     // i.e. the parameters which vary that do not include:
                                                     // - PMT angular response
                                                     // - Laserball distribution histogram 
                                                     // - Laserball Run normalisations ( **FPARAMBASE** )

    Int_t fNCurrentVariableParameters;               // The total number of current variable parameters,
                                                     // i.e. fNBaseVariableParameters + all the variable parameters in the following:
                                                     // - PMT angular response, 
                                                     // - Laserball distribution and run normalisations at the last evaluated data point ( **FPARAM** )

    Int_t*** fPMTAngularResponseIndex;               // Look-up table for unique variable parameters in the PMT angular response ( **FANGINDEX** )

    Int_t* fVariableParameterIndex;                  // Look-up table for the ordered variable parameter indices ( **FPARAMINDEX** )
    Int_t* fVariableParameterMap;                    // Look-up table for the variable parameters (global) ( **FPARAMVARMAP** )

    Int_t fCurrentLBDistributionBin;                 // The current laserball distribution bin for the off-axis run ( **fILBDIST** )
    
    Int_t fCentralCurrentLBDistributionBin;          // The current laserball distribution for the central run ( **fCILBDIST** )

    Int_t fCurrentPMTAngularResponseBin;             // The current angular response bin for the off-axis run ( **fIANG** )

    Int_t fCentralCurrentPMTAngularResponseBin;      // The current angular response bin for the off-axis run ( **fCIANG** )

    Int_t fCurrentLBRunNormalisationBin;             // The current index for the laserball normalisation ( **fINORM** )


    ClassDef( LOCASModelParameterStore, 1 )

  };

}

#endif
