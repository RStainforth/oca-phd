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
#include <map>

using namespace std;

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
    void AddFilter( LOCASFilter filter );

    // Add a complete set of filters from a given card file
    void AddFilters( const char* fileName );

    // Update a value of a filter (this may be used when considering sequential cuts)
    void UpdateFilter( const std::string filterName, const Double_t min, const Double_t max );

    // Print the cut information for filters
    void PrintFilterCutInformation();

    void ResetFilterConditionCounters();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the total number of filters
    Int_t GetNFilters(){ return fFilters.size(); }

    LOCASFilter& GetFilter( const std::string filterName );

    // The iterators to the beginning and end of the filter store
    std::vector< LOCASFilter >::iterator GetLOCASFiltersIterBegin(){ return fFilters.begin(); }
    std::vector< LOCASFilter >::iterator GetLOCASFiltersIterEnd(){ return fFilters.end(); } 

  private:

    std::string fStoreName;                                 // The store name

    std::vector< LOCASFilter > fFilters;                    // The vector of LOCASFilter objects which perform the top level cuts on the initial raw data
    
    std::map< std::string, Int_t > fFilterLookUp;           // A look up map for the filters by filter name

    ClassDef( LOCASFilterStore, 1 )

  };

}

#endif
