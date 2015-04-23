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
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAILS: This class holds a collection of LOCASDataPoint
///          objects. This allows for quick access by the
///          LOCASChiSquare object when performing the fit.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataStore_
#define _LOCASDataStore_

#include "LOCASDataPoint.hh"
#include "LOCASRunReader.hh"
#include "LOCASPMT.hh"

#include <string>
#include <vector>

using namespace std;

namespace LOCAS{

  class LOCASDataStore : public TObject
  {
  public: 

    // The consturctor and destructor for the LOCASDataStore objects
    LOCASDataStore( const string storeName = "" );
    ~LOCASDataStore(){ };

    // The self-addition, addition and equality operators
    LOCASDataStore& operator+=( LOCASDataStore& rhs );
    LOCASDataStore operator+( LOCASDataStore& rhs );
    LOCASDataStore& operator=( const LOCASDataStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a data point to the store.
    // Either a pre-existing data point...
    void AddDataPoint( const LOCASDataPoint dataPoint );

    // ...or a PMT which makes use of the LOCASDataPoint constructor.
    void AddDataPoint( const LOCASPMT& lPMT );

    // Data can also be added from all the LOCASPMTs stored in LOCASRuns
    // within a LOCASRunReader object.
    void AddData( LOCASRunReader& lRuns );

    // Write the datastore to a .root file.
    void WriteToFile( const string fileName = "LOCASDataStore.root" );
    
    // Remove all the datapoints from the current store.
    void ClearData(){ fDataPoints.clear(); }

    // Or remove a particular data point.
    void EraseDataPoint( vector< LOCASDataPoint >::iterator iDataPoint ){ fDataPoints.erase( iDataPoint ); }

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the name of the store.
    string GetStoreName() const { return fStoreName; }

    // Get the full system path of the store.
    string GetStoreFilePath() const { return fStoreFilePath; }

    // Get the number of data points in the store.
    Int_t GetNDataPoints(){ return fDataPoints.size(); }

    // Return a data point by the index i.e. the order the data point
    // was added to the LOCASDataStore originally.
    LOCASDataPoint GetDataPoint( Int_t iDP ){ return fDataPoints[ iDP ]; }

    // Get the iterators to the beginning and end of the data store
    vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterBegin(){ return fDataPoints.begin(); }
    vector< LOCASDataPoint >::iterator GetLOCASDataPointsIterEnd(){ return fDataPoints.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of the store.
    void SetStoreName( const string storeName ){ fStoreName = storeName; }

  private:

    string fStoreName;                     // The store name.
    string fStoreFilePath;                 // The full system path of the file.
    vector< LOCASDataPoint > fDataPoints;  // The vector of data point objects.
    

    

    ClassDef( LOCASDataStore, 1 );
    
  };
  
}

#endif
