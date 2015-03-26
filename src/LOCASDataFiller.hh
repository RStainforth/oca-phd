////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataFiller.hh
///
/// CLASS: LOCAS::LOCASDataFiller.hh
///
/// BRIEF: A class used to take raw data points and a set
///        of filters and return a 'filtered' data set.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataFiller_
#define _LOCASDataFiller_

#include "LOCASDataStore.hh"
#include "LOCASFilterStore.hh"
#include "LOCASRunReader.hh"
#include "LOCASChiSquare.hh"

namespace LOCAS{

  class LOCASDataFiller : public TObject
  {
  public:

    // The constructor
    LOCASDataFiller(){ }

    // The destructor - nothing to delete
    ~LOCASDataFiller(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Re-Filter the current data, requires a chisq object if 
    // one of the filters is a chi square elimination cut
    void FilterData( LOCASFilterStore& filterSt, LOCASDataStore* lDataStore, LOCASChiSquare* lChiSq );

    ClassDef( LOCASDataFiller, 1 );

  };

}

#endif
