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
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: The LOCASFilter object is used to define a check
///         condition for which a LOCASDataPoint object can be
///         tested against. For example, the fit may require
///         that only data points with an occupancy between
///         value X and Y should be included. Therefore this filter
///         represents that condition with minimum and maximum
///         values. A series of these LOCASFilter options can
///         provide, collectively, the top level selection cuts
///         which the data points are subject to prior to the
///         fit beginning itself.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASFilter_
#define _LOCASFilter_

#include "TObject.h"

#include <string>

using namespace std;

namespace LOCAS{

  class LOCASFilter : public TObject
  {
  public: 

    // The constructors and destructors for the LOCASFilter object
    LOCASFilter(){ }
    LOCASFilter( const string filterName, 
                 const Float_t valLow, 
                 const Float_t valHigh );
    LOCASFilter( const string filterName,
                 const Bool_t boolVal );
    ~LOCASFilter(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Check whether a value is within the limits set by the filter
    // i.e. whether fMinVal < val < fMaxMal = TRUE/FALSE
    Bool_t CheckCondition( Float_t val );

    // Check whether a certain value is coincident with the boolean
    // criteria for 
    Bool_t CheckBoolCondition( Bool_t boolVal );

    // Reset all values except name, minimum/maximum values, filter counters
    void ResetFilter();

    // Print out information for this current filter: Name, Boundaries and number of cut values processed so far
    void PrintFilterInformation();

    // This resets the counters which keep a track of how many data points
    // have passed and failed checks on this particualr filter
    void ResetConditionCounters();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of the filter.
    string GetFilterName() const { return fFilterName; }

    // Get the minimum value for the filter to check against.
    Float_t GetMinValue() const { return fMinValue; }

    // Get the maximum and value for the filter to check against.
    Float_t GetMaxValue() const { return fMaxValue; }

    // Get the boolean value for the filter to check against.
    Bool_t GetBoolValue() const { return fBoolValue; }

    // Get the number of times the filter has passed a check.
    Int_t GetNumberConditionPasses() const { return fNumberConditionPasses; }

    // Get the number of times the filter has failed a check.
    Int_t GetNumberConditionFails() const { return fNumberConditionFails; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    // Set the name of the filter.
    void SetFilterName( const string theName ){ fFilterName = theName; }

    // Set the minimum value for the filter to check against.
    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }

    // Set the maximum value for the filter to check against.
    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }

    // Set the boolean value for the filter to check against.
    void SetBoolValue( const Bool_t boolVal ){ fBoolValue = boolVal; }

  private:
    
    string fFilterName;              // The name of the filter

    Float_t fMinValue;               // The minimum value for the condition to be checked against.
    Float_t fMaxValue;               // The maximum value for the condition to be checked against.

    Float_t fBoolValue;              // The boolean value to be checked against.

    Int_t fNumberConditionPasses;    // The number of times this instance of a filter has passed a check.
    Int_t fNumberConditionFails;     // The number of times this instance of a filter has failed a checkx.

    Bool_t fBoolTypeFilter;          // Private flag to check the filter type.
                                     // True: Boolean type filter, False: Range type.

    ClassDef( LOCASFilter, 1 );
    
  };
  
}

#endif
