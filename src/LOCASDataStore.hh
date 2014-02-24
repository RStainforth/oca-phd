////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataStore.hh
///
/// CLASS: LOCAS::LOCASDataStore
///
/// BRIEF: Simple class to store a set of data points
///        ready for use in the fitting process
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataStore_
#define _LOCASDataStore_

#include "LOCASDataPoint.hh"
#include "LOCASRunReader.hh"

#include <string>

namespace LOCAS{

  class LOCASDataStore : public TObject
  {
  public: 

    // The consturctor
    LOCASDataStore( std::string storeName = "" );

    // The destructor - nothign to delete
    ~LOCASDataStore(){ };

    // The self-addition and addition operators
    LOCASDataStore& operator+=( LOCASDataStore& rhs );
    LOCASDataStore operator+( LOCASDataStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a (raw)data point to the store
    void AddDataPoint( LOCASDataPoint dataPoint );
    void AddDataPoint( LOCASRawDataPoint dataPoint );

    // Write the datastore to a .root file
    void WriteToFile( const char* fileName = "LOCASDataStore.root" );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the number of data points in the store
    Int_t GetNDataPoints(){ return fDataPoints.size(); }

    // Get the iterators to the beginning and end of the data store
    std::vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterBegin(){ return fDataPoints.begin(); }
    std::vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterEnd(){ return fDataPoints.end(); }   

  private:

    std::string fStoreName;                     // The store name

    std::vector< LOCASDataPoint > fDataPoints;  // The vector of data point objects

    ClassDef( LOCASDataStore, 1 );
    
  };
  
}

#endif
