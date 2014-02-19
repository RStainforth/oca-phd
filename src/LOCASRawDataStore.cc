////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRawDataStore.cc
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

#include "LOCASRawDataStore.hh"
#include "LOCASRawDataPoint.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASRawDataStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASRawDataStore::LOCASRawDataStore( const char* storeName )
{

  fStoreName = storeName;
  fRawDataPoints.clear();
  fNRawDataPoints = 0;
 
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRawDataStore::AddRawDataPoint( LOCASRawDataPoint dataPoint )
{

  LOCASRawDataPoint dPoint = dataPoint;
  fRawDataPoints[ GetNRawDataPoints() + 1 ] = dPoint;

}
