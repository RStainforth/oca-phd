////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCADataStore.hh
///
/// CLASS: OCA::OCADataStore
///
/// BRIEF: Simple class to store a set of data points
///        ready for use in the fitting process
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAILS: This class holds a collection of OCADataPoint
///          objects. This allows for quick access by the
///          OCAChiSquare object when performing the fit.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCADataStore_
#define _OCADataStore_

#include "OCADataPoint.hh"
#include "OCARunReader.hh"
#include "OCAPMT.hh"

#include <string>
#include <vector>

using namespace std;

namespace OCA{

  class OCADataStore : public TObject
  {
  public: 

    // The consturctor and destructor for the OCADataStore objects
    OCADataStore( const string storeName = "" );
    ~OCADataStore(){ };

    // The self-addition, addition and equality operators
    OCADataStore& operator+=( OCADataStore& rhs );
    OCADataStore operator+( OCADataStore& rhs );
    OCADataStore& operator=( const OCADataStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a data point to the store.
    // Either a pre-existing data point...
    void AddDataPoint( const OCADataPoint dataPoint );

    // ...or a PMT which makes use of the OCADataPoint constructor.
    void AddDataPoint( const OCAPMT& lPMT );

    // Data can also be added from all the OCAPMTs stored in OCARuns
    // within a OCARunReader object.
    void AddData( OCARunReader& lRuns );

    // Write the datastore to a .root file.
    void WriteToFile( const string fileName = "OCADataStore.root" );
    
    // Remove all the datapoints from the current store.
    void ClearData(){ fDataPoints.clear(); }

    // Or remove a particular data point.
    void EraseDataPoint( vector< OCADataPoint >::iterator iDataPoint ){ fDataPoints.erase( iDataPoint ); }

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
    // was added to the OCADataStore originally.
    OCADataPoint GetDataPoint( Int_t iDP ){ return fDataPoints[ iDP ]; }

    // Get the iterators to the beginning and end of the data store
    vector< OCADataPoint >::iterator GetOCADataPointsIterBegin(){ return fDataPoints.begin(); }
    vector< OCADataPoint >::iterator GetOCADataPointsIterEnd(){ return fDataPoints.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of the store.
    void SetStoreName( const string storeName ){ fStoreName = storeName; }

  private:

    string fStoreName;                     // The store name.
    string fStoreFilePath;                 // The full system path of the file.
    vector< OCADataPoint > fDataPoints;  // The vector of data point objects.
    

    

    ClassDef( OCADataStore, 1 );
    
  };
  
}

#endif
