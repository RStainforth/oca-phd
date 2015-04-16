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
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This class is used for easily reading in the LOCASRun
///         files. This is similar to the SOCReader class used
///         in RAT for RAT::DS::SOC objects but for LOCAS::LOCASRun
///         objects.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASRUNREADER_
#define _LOCASRUNREADER_

#include "LOCASRun.hh"

#include "TObject.h"
#include "TChain.h"

#include <vector>

using namespace std;

namespace LOCAS{

  class LOCASRunReader : public TObject
  {
  public:

    // The constructors and destructor for the LOCASRunReader object.
    LOCASRunReader();
    LOCASRunReader( vector< Int_t >& runIDs );
    LOCASRunReader( Int_t runID );
    LOCASRunReader( const char* filename );
    ~LOCASRunReader();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a LOCASRun file by runID.
    void Add( Int_t runID );

    // Add a LOCASRun file as specified by a list of
    // run IDs in a 'fit-file'.
    void Add( const char* filename );

    // Check to see if a LOCASRun object given by the 'runID'
    // is currently accessible. i.e. has it been 'added' yet.
    Bool_t CheckForLOCASRun( Int_t runID );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Returns a pointer to the current LOCASRun object on the reader.
    LOCASRun* GetCurrentLOCASRun(){ return fLOCASRun; }

    // Returns a pointer to the LOCASRun with run ID 'runID'.
    LOCASRun* GetLOCASRun( Int_t runID );

    // Returns the 'runEntry'-th entry, e.g. for 4 files on the LOCASRunReader
    // object, GetRunEntry( 1 ) would return the SECOND run file 
    // ( 0 being the first ).
    LOCASRun* GetRunEntry( Int_t runEntry ){ 
      fLOCASRunT->GetEntry( runEntry );
      return fLOCASRun;
    } 

    // Returns the number of LOCASRuns on the reader object.
    Long64_t GetNLOCASRuns(){ return fNLOCASRuns; }

    // Returns a vector of runIDs.
    vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the current LOCASRun by run ID.
    void SetCurrentLOCASRun( Int_t runID ){ fLOCASRun = GetLOCASRun( runID ); }

  protected:

    TChain* fLOCASRunT;                   // TChain of the ROOT files added to the LOCASRun reader.
    LOCASRun* fLOCASRun;                  // Pointer to the current LOCASRun object.
    
    Long64_t fNext;                       // Index of the next LOCASRun TTree in the TChain.
    Long64_t fNLOCASRuns;                 // Total number of LOCASRun TTrees in the TChain.

    vector< Int_t > fListOfRunIDs;        // Vector of run IDs whose corresponding ROOT fies
                                          // have been loaded onto the TChain.

    LOCASPMT* fCurrentPMT;                // Pointer to the current LOCASPMT object.

    ClassDef( LOCASRunReader, 0 ) 
    
  };

}

#endif
