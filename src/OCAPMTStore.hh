////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAPMTStore.hh
///
/// CLASS: OCA::OCAPMTStore
///
/// BRIEF: Simple class to store a set of data points
///        ready for use in the fitting process
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAILS: This class holds a collection of OCAPMT
///          objects. This allows for quick access by the
///          OCAChiSquare object when performing the fit.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAPMTStore_
#define _OCAPMTStore_

#include "OCAPMT.hh"
#include "OCARunReader.hh"
#include "OCAPMT.hh"

#include <string>
#include <vector>

using namespace std;

namespace OCA{

  class OCAPMTStore : public TObject
  {
  public: 

    // The consturctor and destructor for the OCAPMTStore objects
    OCAPMTStore( const string storeName = "" );
    ~OCAPMTStore(){ };

    // The self-addition, addition and equality operators
    OCAPMTStore& operator+=( OCAPMTStore& rhs );
    OCAPMTStore operator+( OCAPMTStore& rhs );
    OCAPMTStore& operator=( const OCAPMTStore& rhs );

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a data point to the store.
    void AddDataPoint( const OCAPMT dataPoint );

    // Data can also be added from all the OCAPMTs stored in OCARuns
    // within a OCARunReader object.
    void AddData( OCARunReader& lRuns );

    // Write the datastore to a .root file.
    void WriteToFile( const string fileName = "OCAPMTStore.root" );
    
    // Remove all the datapoints from the current store.
    void ClearData(){ fDataPoints.clear(); }

    // Or remove a particular data point.
    void EraseDataPoint( vector< OCAPMT >::iterator iDataPoint ){ fDataPoints.erase( iDataPoint ); }

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
    // was added to the OCAPMTStore originally.
    OCAPMT GetDataPoint( Int_t iDP ){ return fDataPoints[ iDP ]; }

    // Get the iterators to the beginning and end of the data store
    vector< OCAPMT >::iterator GetOCAPMTsIterBegin(){ return fDataPoints.begin(); }
    vector< OCAPMT >::iterator GetOCAPMTsIterEnd(){ return fDataPoints.end(); }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the name of the store.
    void SetStoreName( const string storeName ){ fStoreName = storeName; }

  private:

    string fStoreName;                     // The store name.
    string fStoreFilePath;                 // The full system path of the file.
    vector< OCAPMT > fDataPoints;  // The vector of data point objects.
    
    ClassDef( OCAPMTStore, 1 );
    
  };
  
}

#endif
