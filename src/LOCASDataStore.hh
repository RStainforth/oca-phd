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
    LOCASDataStore( std::string storeName = "" );
    ~LOCASDataStore(){ };

    LOCASDataStore& operator+=( LOCASDataStore& rhs );
    LOCASDataStore operator+( LOCASDataStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void AddDataPoint( LOCASDataPoint dataPoint );
    void AddDataPoint( LOCASRawDataPoint dataPoint );

    void WriteToFile( const char* fileName = "LOCASDataStore.root" );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetNDataPoints(){ return fDataPoints.size(); }

    std::vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterBegin(){ return fDataPoints.begin(); }
    std::vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterEnd(){ return fDataPoints.end(); }   

  private:

    std::string fStoreName;

    std::vector< LOCASDataPoint > fDataPoints;

    ClassDef( LOCASDataStore, 1 );
    
  };
  
}

#endif
