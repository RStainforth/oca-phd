////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFilterStore.hh
///
/// CLASS: LOCAS::LOCASFilterStore
///
/// BRIEF: A simple class to hold a set of filters which are
///        used to apply cuts to a set of data held in a 
///        LOCASDataFiller object.
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

    // The constructors
    LOCASFilterStore( const char* fileName, std::string storeName = "" );
    LOCASFilterStore(){ }

    // The destructor - nothing to delete
    ~LOCASFilterStore(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a filter to the collection of filters (this object)
    void AddTopLevelFilter( LOCASFilter filter );
    void AddFitLevelFilter( LOCASFilter filter );

    // Add a complete set of filters from a given card file
    void AddTopLevelFilters( const char* fileName );
    void AddFitLevelFilters( const char* fileName );

    // Update a value of a filter (this may be used when considering sequential cuts)
    void UpdateTopLevelFilter( const std::string filterName, const Double_t min, const Double_t max );
    void UpdateFitLevelFilter( const std::string filterName, const Double_t min, const Double_t max );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the total number of filters
    Int_t GetNTopLevelFilters(){ return fTopLevelFilters.size(); }
    Int_t GetNFitLevelFilters(){ return fFitLevelFilters.size(); }

    LOCASFilter& GetTopLevelFilter( const std::string filterName );
    LOCASFilter& GetFitLevelFilter( const std::string filterName );

    // The iterators to the beginning and end of the top level filter store
    std::vector< LOCASFilter >::iterator GetLOCASTopLevelFiltersIterBegin(){ return fTopLevelFilters.begin(); }
    std::vector< LOCASFilter >::iterator GetLOCASTopLevelFiltersIterEnd(){ return fTopLevelFilters.end(); } 

    // The iterators to the beginning and end of the fit level filter store
    std::vector< LOCASFilter >::iterator GetLOCASFitLevelFiltersIterBegin(){ return fFitLevelFilters.begin(); }
    std::vector< LOCASFilter >::iterator GetLOCASFitLevelFiltersIterEnd(){ return fFitLevelFilters.end(); }

  private:

    std::string fStoreName;                                 // The store name

    std::vector< LOCASFilter > fTopLevelFilters;            // The vector of LOCASFilter objects which perform the top level cuts on the initial raw data
    std::vector< LOCASFilter > fFitLevelFilters;            // The vector of LOCASFilter objects which perform cuts when refitting data
    
    std::map< std::string, Int_t > fTopLevelFilterLookUp;   // A look up map for the top level filters
    std::map< std::string, Int_t > fFitLevelFilterLookUp;   // A look up map for the fit level filters  

    ClassDef( LOCASFilterStore, 1 )

  };

}

#endif
