////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRawDataStore.hh
///
/// CLASS: LOCAS::LOCASRawDataStore
///
/// BRIEF: Simple class to store a set of raw data points
///        ready for processing
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////


#ifndef _LOCASRawDataStore_
#define _LOCASRawDataStore_

#include "LOCASRawDataPoint.hh"
#include "LOCASDataStore.hh"
#include "LOCASFilterStore.hh"
#include "LOCASFilter.hh"

#include <string>

namespace LOCAS{

  class LOCASRawDataStore : public TObject
  {
  public:
    
    // The constructors
    LOCASRawDataStore( LOCASRunReader& runReader, std::string storeName = "" );
    LOCASRawDataStore( std::string storeName = "" );

    // The desturctor - nothing to delete
    ~LOCASRawDataStore(){ };

    // The self-addition and addition operators
    LOCASRawDataStore& operator+=( LOCASRawDataStore& rhs );
    LOCASRawDataStore operator+( LOCASRawDataStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a raw data point
    void AddRawDataPoint( LOCASRawDataPoint dataPoint ){ fRawDataPoints.push_back( dataPoint ); }

    // Add raw data points from a LOCASRunReader object of several runs worth of data
    void AddRawData( LOCASRunReader& runReader );

    // Write the raw data store to a .root file
    void WriteToFile( const char* fileName = "LOCASRawDataStore.root" );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // The the number of raw data points held in the store
    Int_t GetNRawDataPoints(){ return fRawDataPoints.size(); }

    // Get iterators to the beginning and end of the data store
    std::vector< LOCASRawDataPoint >::iterator GetLOCASRawDataPointsIterBegin(){ return fRawDataPoints.begin(); }
    std::vector< LOCASRawDataPoint >::iterator GetLOCASRawDataPointsIterEnd(){ return fRawDataPoints.end(); }   

  private:

    std::string fStoreName;                          // The store name

    std::vector< LOCASRawDataPoint > fRawDataPoints; // The vector of raw data point objects

    ClassDef( LOCASRawDataStore, 1 );
    
  };
  
}

#endif
