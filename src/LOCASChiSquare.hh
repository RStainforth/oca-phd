////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASChiSquare.hh
///
/// CLASS: LOCAS::LOCASChiSquare
///
/// BRIEF: A simple class to compute the global chi-square
///        of an entire data set given a LOCASModel object and
///        LOCASModelParameterStore object
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASChiSquare_
#define _LOCASChiSquare_

#include "LOCASModel.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASDataPoint.hh"
#include "LOCASDataStore.hh"

namespace LOCAS{
  
  class LOCASChiSquare : public TObject
  {
  public:
    LOCASChiSquare(){ };
    LOCASChiSquare( const LOCASModel& locasModel, const LOCASDataStore& locasData );
    ~LOCASChiSquare(){ };

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Float_t EvaluateChiSquare( const LOCASDataPoint& dPoint );
    Float_t EvaluateGlobalChiSquare();
    Float_t EvaluateGlobalChiSquare( const Double_t* params );

  private:

    LOCASDataStore fDataStore;
    
    LOCASModel fModel;

    ClassDef( LOCASChiSquare, 0 );

  };

}

#endif
