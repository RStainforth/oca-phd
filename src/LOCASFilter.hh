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
    Bool_t CheckCondition( Float_t val ) const;

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetFilterName() const { return fFilterName; }

    // The maximum and minimum values for the filter to check against
    Float_t GetMaxValue() const { return fMaxValue; }
    Float_t GetMinValue() const { return fMinValue; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetFilterName( const std::string theName ){ fFilterName = theName; }

    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }
    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }

  private:
    
    std::string fFilterName; // The name of the filter

    Float_t fMaxValue;       // The maximum value for the condition to be checked against
    Float_t fMinValue;       // The minimum value for the condition to be checked against

    ClassDef( LOCASFilter, 1 );
    
  };
  
}

#endif
