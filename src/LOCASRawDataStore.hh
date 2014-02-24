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
    LOCASRawDataStore( LOCASRunReader& runReader, std::string storeName = "" );
    LOCASRawDataStore( std::string storeName = "" );
    ~LOCASRawDataStore(){ };

    LOCASRawDataStore& operator+=( LOCASRawDataStore& rhs );
    LOCASRawDataStore operator+( LOCASRawDataStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void AddRawDataPoint( LOCASRawDataPoint dataPoint ){ fRawDataPoints.push_back( dataPoint ); }
    void AddRawData( LOCASRunReader& runReader );

    void WriteToFile( const char* fileName = "LOCASRawDataStore.root" );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetNRawDataPoints(){ return fRawDataPoints.size(); }

    std::vector< LOCASRawDataPoint >::iterator GetLOCASRawDataPointsIterBegin(){ return fRawDataPoints.begin(); }
    std::vector< LOCASRawDataPoint >::iterator GetLOCASRawDataPointsIterEnd(){ return fRawDataPoints.end(); }   

  private:

    std::string fStoreName;

    std::vector< LOCASRawDataPoint > fRawDataPoints;

    ClassDef( LOCASRawDataStore, 1 );
    
  };
  
}

#endif
