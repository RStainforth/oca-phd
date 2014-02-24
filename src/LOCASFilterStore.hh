////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFilterStore.hh
///
/// CLASS: LOCAS::LOCASFilterStore
///
/// BRIEF: A simple class to hold a set of filters which are
///        used to apply cuts to a set of data held in a 
///        LOCASRawDataStore object.
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASFilterStore_
#define _LOCASFilterStore_

#include "LOCASFilter.hh"

#include <string>

namespace LOCAS{

  class LOCASFilterStore : public TObject
  {
  public:

    LOCASFilterStore( const char* fileName, std::string storeName = "" );
    LOCASFilterStore(){ }
    ~LOCASFilterStore(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void AddFilter( LOCASFilter filter ){ fFilters.push_back( filter ); }
    void AddFilters( const char* fileName );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetNFilters(){ return fFilters.size(); }

    std::vector< LOCASFilter >::iterator GetLOCASFiltersIterBegin(){ return fFilters.begin(); }
    std::vector< LOCASFilter >::iterator GetLOCASFiltersIterEnd(){ return fFilters.end(); } 

  private:

    std::string fStoreName;

    std::vector< LOCASFilter > fFilters;

    ClassDef( LOCASFilterStore, 1 )

  };

}

#endif
