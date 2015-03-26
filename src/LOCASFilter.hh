////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFilter.hh
///
/// CLASS: LOCAS::LOCASFilter
///
/// BRIEF: A simple class to test whether certain
///        values fall within a range of values
///        A collection of LOCASFilter-s (LOCASFilterStore) is used
///        to introduce the cut-flow to a set of
///        LOCASRawDataPoint objects
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///     03/2015 : RPFS - Added counters for passing data points
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASFilter_
#define _LOCASFilter_

#include "LOCASRawDataPoint.hh"

#include <string>

namespace LOCAS{

  class LOCASFilter : public TObject
  {
  public: 

    // The constructors
    LOCASFilter(){ }
    LOCASFilter( std::string filterName, Float_t valLow, Float_t valHigh );

    // The destructor - nothing to delete
    ~LOCASFilter(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Check whether a value is within the limits set by the filter
    Bool_t CheckCondition( Float_t val );

    // Reset all values except name: minimum/maximum values, filter counters
    void ResetFilter();

    // Print out information for this current filter: Name, Boundaries and number of cut values
    void PrintFilterInformation();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetFilterName() const { return fFilterName; }

    // The maximum and minimum values for the filter to check against
    Float_t GetMaxValue() const { return fMaxValue; }
    Float_t GetMinValue() const { return fMinValue; }

    // The number of times the filter has passed a check, and the number of times it has failed a check.
    Int_t GetNumberConditionPasses() const { return fNumberConditionPasses; }
    Int_t GetNumberConditionFails() const { return fNumberConditionFails; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetFilterName( const std::string theName ){ fFilterName = theName; }

    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }
    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }

  private:
    
    std::string fFilterName;         // The name of the filter

    Float_t fMaxValue;               // The maximum value for the condition to be checked against
    Float_t fMinValue;               // The minimum value for the condition to be checked against

    Int_t fNumberConditionPasses;    // The number of times this instance of a filter has passed a check
    Int_t fNumberConditionFails;     // The number of times this instance of a filter has failed a checkx

    ClassDef( LOCASFilter, 1 );
    
  };
  
}

#endif
