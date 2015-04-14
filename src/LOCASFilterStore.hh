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
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: The LOCASFilterStore object stores a collection of
///         LOCASFilter objects. This allows for easy access
///         to more than one LOCASFilter object when passing
///         a series of data points through as part of the top
///         level cuts prior to a fit.
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

    // The constructors and destructor for the LOCASFilterStore object.
    LOCASFilterStore( const char* fileName, std::string storeName = "" );
    LOCASFilterStore(){ }
    ~LOCASFilterStore(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a filter to the collection of filters (this object).
    void AddFilter( LOCASFilter filter );

    // Add a complete set of filters from a given 'fit-file'.
    void AddFilters( const char* fileName );

    // Update the minimum and maximum values of a filter. 
    // This may be useful when more stringent cuts are required part way through
    // a fit. e.g. tighter restrictions on the individual chi-square value
    // of a data point to included in a fit.
    void UpdateFilter( const std::string filterName, 
                       const Double_t min, 
                       const Double_t max );

    // Print the cut information for filters.
    void PrintFilterCutInformation();
    
    // Reset all the condition counters for all the filter currently
    // stored in this object.
    void ResetFilterConditionCounters();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of this LOCASFitlerStore object.
    string GetStoreName() const { return fStoreName; }

    // Get the total number of filters.
    Int_t GetNFilters(){ return fFilters.size(); }

    // Get a particular filter by name.
    LOCASFilter& GetFilter( const std::string filterName );

    // The iterators to the beginning and end of the filter store.
    std::vector< LOCASFilter >::iterator GetLOCASFiltersIterBegin(){ return fFilters.begin(); }
    std::vector< LOCASFilter >::iterator GetLOCASFiltersIterEnd(){ return fFilters.end(); } 

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of this LOCASFilterStore object.
    void SetStoreName ( const string storeName ){ fStoreName = storeName; }

  private:

    std::string fStoreName;                                 // The store name.

    std::vector< LOCASFilter > fFilters;                    // The vector of LOCASFilter objects which perform the top level cuts on the initial data.
    
    std::map< std::string, Int_t > fFilterLookUp;           // A look up map for the filters by filter name

    ClassDef( LOCASFilterStore, 1 )

  };

}

#endif
