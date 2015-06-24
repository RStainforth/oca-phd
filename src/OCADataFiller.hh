////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCADataFiller.hh
///
/// CLASS: OCA::OCADataFiller.hh
///
/// BRIEF: A class used to take data points and a set
///        of filters and return a 'filtered' data set.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This class acts on two 'Store' type objects.
///         - OCADataStore
///         - OCAFilterStore
///
///         The OCADataFiller object takes each OCADataPoint
///         from the OCADataStore, and passes it to each 
///         OCADataFilter in the OCAFilterStore. If the
///         OCADataPoint passes all the 'check' conditions
///         as determined by the filters, then it is added to
///         a final, newly created OCADataStore object which is
///         consequently returned with the address of the original
///         OCADataStore.
/// 
///         NOTE: a OCAChiSquare object is also required for this
///         to work as some filters make use of the initial chi-square
///         to make a cut.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCADataFiller_
#define _OCADataFiller_

#include "OCADataStore.hh"
#include "OCAFilterStore.hh"
#include "OCAChiSquare.hh"

using namespace std;

namespace OCA{

  class OCADataFiller : public TObject
  {
  public:

    // The constructor and destructor for the OCADataFiller object.
    OCADataFiller(){ }
    ~OCADataFiller(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Filter the current data. This requires a OCAChiSquare object if 
    // one of the filters is a chi square elimination cut
    void FilterData( OCAFilterStore* lFilterStore, 
                     OCADataStore* lDataStore, 
                     OCAChiSquare* lChiSq = NULL );

    ClassDef( OCADataFiller, 1 );

  };

}

#endif
