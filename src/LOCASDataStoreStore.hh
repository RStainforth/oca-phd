////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataStoreStore.hh
///
/// CLASS: LOCAS::LOCASDataStoreStore
///
/// BRIEF: Simple class to store a store of a set of data points
///        ready for use in the fitting process. This entire class is
///        mainly used for debugging
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataStoreStore_
#define _LOCASDataStoreStore_

#include "LOCASDataStore.hh"

#include <string>

namespace LOCAS{

  class LOCASDataStoreStore : public TObject
  {
  public:

    //The constructor 
    LOCASDataStoreStore( std::string storeName = "" );

    // The destrcutor - nothing to delete
    ~LOCASDataStoreStore(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a datastore to the store
    void AddDataStore( LOCASDataStore dataStore );

    // Write the store to a .root file
    void WriteToFile( const char* fileName = "LOCASDataStoreStore.root" );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get iterators to the first and last datastore objects in the store
    std::vector< LOCASDataStore >::iterator GetLOCASDataStoresIterBegin(){ return fDataStores.begin(); }
    std::vector< LOCASDataStore >::iterator GetLOCASDataStoresIterEnd(){ return fDataStores.end(); }


  private:

    std::string fStoreName;

    std::vector< LOCASDataStore > fDataStores;

    ClassDef( LOCASDataStoreStore, 1 );

  };

}

#endif
