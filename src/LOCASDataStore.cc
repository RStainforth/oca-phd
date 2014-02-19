////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataStore.cc
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

#include "LOCASDataStore.hh"
#include "LOCASDataPoint.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataStore::LOCASDataStore( const char* storeName )
{

  fStoreName = storeName;
  fDataPoints.clear();
  fNDataPoints = 0;
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataStore::AddDataPoint( LOCASDataPoint dataPoint )
{

  LOCASDataPoint dPoint = dataPoint;
  fDataPoints[ GetNDataPoints() + 1 ] = dPoint;

}
