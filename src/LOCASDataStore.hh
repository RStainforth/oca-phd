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

    // The destructor - nothing to delete
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
    
    // Remove all the datapoints from the current store
    void ClearData(){ fDataPoints.clear(); }

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the number of data points in the store
    Int_t GetNDataPoints(){ return fDataPoints.size(); }

    // Get the iterators to the beginning and end of the data store
    std::vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterBegin(){ return fDataPoints.begin(); }
    std::vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterEnd(){ return fDataPoints.end(); }   

    // These setters are for debugging purposes
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetScintPar( const Float_t val ) { fScintPar = val; }
    void SetAVPar( const Float_t val ) { fAVPar = val; }
    void SetWaterPar( const Float_t val ) { fWaterPar = val; }

    void SetScintRSPar( const Float_t val ) { fScintRSPar = val; }
    void SetAVRSPar( const Float_t val ) { fAVRSPar = val; }
    void SetWaterRSPar( const Float_t val ) { fWaterRSPar = val; }

  private:

    std::string fStoreName;                     // The store name

    std::vector< LOCASDataPoint > fDataPoints;  // The vector of data point objects

    // These variables are for debugging purposes
    Float_t fScintPar;   
    Float_t fAVPar;
    Float_t fWaterPar;
    
    Float_t fScintRSPar;
    Float_t fAVRSPar;
    Float_t fWaterRSPar;

    ClassDef( LOCASDataStore, 1 );
    
  };
  
}

#endif
