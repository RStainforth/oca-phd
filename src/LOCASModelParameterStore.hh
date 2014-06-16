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

#include <string>
#include <map>

namespace LOCAS{

  class LOCASModelParameterStore : public TObject
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

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the number of parameters in the store
    Int_t GetNParameters() const { return fNParameters; }

    // Get iterators to the beginning and end of the parameter store
    std::vector< LOCASModelParameter >::iterator GetLOCASModelParametersIterBegin(){ return fParameters.begin(); }
    std::vector< LOCASModelParameter >::iterator GetLOCASModelParametersIterEnd(){ return fParameters.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetNParameters( const Int_t nPars ){ fNParameters = nPars; }

    void SetParameters( const std::vector< LOCAS::LOCASModelParameter >& pars ){ fParameters = pars; }

  private:

    std::string fStoreName;                          // The store name

    Int_t fNParameters;                              // The number of parameters in the store

    std::vector< LOCASModelParameter > fParameters;  // The vector of parameter objects i.e. the store
    Double_t* fParametersPtr;                        // The pointer of parameter values

    ClassDef( LOCASModelParameterStore, 1 )

  };

}

#endif
