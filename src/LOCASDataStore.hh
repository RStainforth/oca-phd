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
#include <map>

namespace LOCAS{

  class LOCASDataStore : public TObject
  {
  public: 
    LOCASDataStore( const char* storeName = NULL );
    ~LOCASDataStore(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void AddDataPoint( LOCASDataPoint dataPoint );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Int_t GetNDataPoints(){ return fDataPoints.size(); }

    std::map< Int_t, LOCASDataPoint >::iterator GetLOCASDataPointsIterBegin(){ return fDataPoints.begin(); }
    std::map< Int_t, LOCASDataPoint >::iterator GetLOCASDataPointsIterEnd(){ return fDataPoints.end(); }   

  private:

    const char* fStoreName;

    Int_t fNDataPoints;

    std::map< Int_t, LOCASDataPoint > fDataPoints;


    ClassDef( LOCASDataStore, 1 );
    
  };
  
}

#endif
