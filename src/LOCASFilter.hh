////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFilter.hh
///
/// CLASS: LOCAS::LOCASFilter
///
/// BRIEF: A simple class to test whether certain
///        values fall within a range of values
///        A collection of LOCASFilters is used
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

    LOCASFilter(){ }
    LOCASFilter( std::string filterName, Float_t valLow, Float_t valHigh );
    ~LOCASFilter(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Bool_t CheckCondition( Float_t val ) const;

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetFilterName() const { return fFilterName; }

    Float_t GetMaxValue() const { return fMaxValue; }
    Float_t GetMinValue() const { return fMinValue; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetFilterName( const std::string theName ){ fFilterName = theName; }

    void SetMaxValue( const Float_t maxVal ){ fMaxValue = maxVal; }
    void SetMinValue( const Float_t minVal ){ fMinValue = minVal; }

  private:
    
    std::string fFilterName;

    Float_t fMaxValue;
    Float_t fMinValue;

    ClassDef( LOCASFilter, 1 );
    
  };
  
}

#endif
