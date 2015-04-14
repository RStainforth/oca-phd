////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataFiller.hh
///
/// CLASS: LOCAS::LOCASDataFiller.hh
///
/// BRIEF: A class used to take data points and a set
///        of filters and return a 'filtered' data set.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2014 : RPFS - First Revision, new file.
///
/// DETAIL: This class acts on two 'Store' type objects.
///         - LOCASDataStore
///         - LOCASFilterStore
///
///         The LOCASDataFiller object takes each LOCASDataPoint
///         from the LOCASDataStore, and passes it to each 
///         LOCASDataFilter in the LOCASFilterStore. If the
///         LOCASDataPoint passes all the 'check' conditions
///         as determined by the filters, then it is added to
///         a final, newly created LOCASDataStore object which is
///         consequently returned with the address of the original
///         LOCASDataStore.
/// 
///         NOTE: a LOCASChiSquare object is also required for this
///         to work as some filters make use of the initial chi-square
///         to make a cut.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataFiller_
#define _LOCASDataFiller_

#include "LOCASDataStore.hh"
#include "LOCASFilterStore.hh"
#include "LOCASChiSquare.hh"

using namespace std;

namespace LOCAS{

  class LOCASDataFiller : public TObject
  {
  public:

    // The constructor and destructor for the LOCASDataFiller object.
    LOCASDataFiller(){ }
    ~LOCASDataFiller(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Filter the current data. This requires a LOCASChiSquare object if 
    // one of the filters is a chi square elimination cut
    void FilterData( LOCASFilterStore* lFilterStore, 
                     LOCASDataStore* lDataStore, 
                     LOCASChiSquare* lChiSq = NULL );

    ClassDef( LOCASDataFiller, 1 );

  };

}

#endif
