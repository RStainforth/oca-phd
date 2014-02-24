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

    LOCASModelParameterStore ( std::string storeName = "" );
    ~LOCASModelParameterStore(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void AddParameter( LOCASModelParameter parameter, Int_t index ){ fParameters.push_back( parameter ); }
    void AddParameters( const char* fileName );

    void PrintParameterInfo();
    void WriteToFile( const char* fileName = "LOCASModelParameterStore.root" );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetNParameters() const { return fNParameters; }

    std::vector< LOCASModelParameter >::iterator GetLOCASModelParametersIterBegin(){ return fParameters.begin(); }
    std::vector< LOCASModelParameter >::iterator GetLOCASModelParametersIterEnd(){ return fParameters.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetNParameters( const Int_t nPars ){ fNParameters = nPars; }

    void SetParameters( const std::vector< LOCAS::LOCASModelParameter >& pars ){ fParameters = pars; }

  private:

    std::string fStoreName;

    Int_t fNParameters;

    std::vector< LOCASModelParameter > fParameters;

    ClassDef( LOCASModelParameterStore, 1 )

  };

}

#endif
