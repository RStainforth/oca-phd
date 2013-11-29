////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRunReader.hh
///
/// CLASS: LOCAS::LOCASRunReader
///
/// BRIEF: Reader class for the LOCASRun Objects
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: *NEEDS WRITING*
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASRUNREADER_
#define _LOCASRUNREADER_

#include <TObject.h>
#include <TChain.h>
#include <TMinuit.h>

#include <string>
#include <vector>

#include "LOCASRun.hh"

namespace LOCAS{

  class LOCASRunReader : public TObject
  {
  public:
    LOCASRunReader();
    LOCASRunReader( std::vector< Int_t >& runIDs );
    LOCASRunReader( Int_t runID );
    LOCASRunReader( const char* filename );
    virtual ~LOCASRunReader();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void Add( Int_t runID );
    void Add( const char* filename );

    Bool_t CheckForLOCASRun( Int_t runID );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    LOCASRun* GetCurrentLOCASRun(){ return fLOCASRun; }
    void GetCurrentLOCASRun( LOCASRun* lRun ){ lRun = fLOCASRun; }
    
    LOCASPMT* GetCurrentLOCASPMT(){ return fCurrentPMT; }
    void GetCurrentLOCASPMT( LOCASPMT* lPMT ){ lPMT = fCurrentPMT; }

    LOCASRun* GetLOCASRun( Int_t runID );

    Long64_t GetNLOCASRuns(){ return fNLOCASRuns; }

    std::vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetCurrentLOCASRun( Int_t runID ){ fLOCASRun = GetLOCASRun( runID ); }

    

  protected:

    TChain* fLOCASRunT;
    LOCASRun* fLOCASRun;
    
    Long64_t fNext;
    Long64_t fNLOCASRuns;

    std::vector< Int_t > fListOfRunIDs;

    LOCASPMT* fCurrentPMT;

    ClassDef( LOCASRunReader, 0 ) 
    
  };

}

#endif
