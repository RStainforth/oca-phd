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
#include <map>

namespace LOCAS{

  class LOCASRawDataStore : public TObject
  {
  public: 
    LOCASRawDataStore( const char* storeName = NULL );
    ~LOCASRawDataStore(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void AddRawDataPoint( LOCASRawDataPoint dataPoint );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetNRawDataPoints(){ return fRawDataPoints.size(); }

    std::map< Int_t, LOCASRawDataPoint >::iterator GetLOCASRawDataPointsIterBegin(){ return fRawDataPoints.begin(); }
    std::map< Int_t, LOCASRawDataPoint >::iterator GetLOCASRawDataPointsIterEnd(){ return fRawDataPoints.end(); }   

  private:

    const char* fStoreName;

    Int_t fNRawDataPoints;

    std::map< Int_t, LOCASRawDataPoint > fRawDataPoints;


    ClassDef( LOCASRawDataStore, 1 );
    
  };
  
}

#endif
