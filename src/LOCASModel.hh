////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModel.hh
///
/// CLASS: LOCAS::LOCASModel
///
/// BRIEF: Class used to process the LOCASRun files
///        and prepare the parameters and values for each PMT
///        required for the model of the optical response.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: 
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASMODEL_
#define _LOCASMODEL_

#include "LOCASRunReader.hh"
#include "LOCASDB.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"

#include <TObject.h>
#include <TChain.h>

#include <string>
#include <vector>

namespace LOCAS{

  class LOCASModel : public LOCASRunReader
  {
  public:

    LOCASModel( std::vector< Int_t >& runIDs ) : LOCASRunReader( runIDs ){ fListOfRunIDs = LOCASRunReader::GetListOfRunIDs(); }
    LOCASModel( Int_t runID ) : LOCASRunReader( runID ){ }
    LOCASModel( const char* filename ) : LOCASRunReader( filename ){ }
    virtual ~LOCASModel(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Float_t EvaluateChiSquare( Double_t* params );
    Float_t EvaluateGlobalChiSquare( Double_t* params );

    Float_t CalcModelROcc( Double_t* params );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    Float_t GetDataROcc();
    Float_t GetROccError();

  private:

    LOCASPMT* fCurrentPMT;
    LOCASRun* fCurrentRun;

    std::vector< Int_t > fListOfRunIDs;

    ClassDef( LOCASModel, 0 )

  };

}

#endif
