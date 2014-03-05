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

#include "LOCASRawDataStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASFilterStore.hh"

namespace LOCAS{

  class LOCASDataFiller : public TObject
  {
  public:

    // The constructors
    LOCASDataFiller( LOCASRawDataStore& dataSt, LOCASFilterStore& filterSt );
    LOCASDataFiller(){ }

    // The destructor - nothing to delete
    ~LOCASDataFiller(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add data and a set of filters to filtrate the data
    void AddData( LOCASRawDataStore& dataSt, LOCASFilterStore& filterSt );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Return the 'filtered' data
    LOCASDataStore GetData();

  private:

    LOCASDataStore fDataStore;

    ClassDef( LOCASDataFiller, 1 );

  };

}

#endif
