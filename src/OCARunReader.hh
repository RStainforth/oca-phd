////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCARunReader.hh
///
/// CLASS: OCA::OCARunReader
///
/// BRIEF: Reader class for the OCARun Objects
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This class is used for easily reading in the OCARun
///         files. This is similar to the SOCReader class used
///         in RAT for RAT::DS::SOC objects but for OCA::OCARun
///         objects.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCARUNREADER_
#define _OCARUNREADER_

#include "OCARun.hh"

#include "TObject.h"
#include "TChain.h"

#include <vector>

using namespace std;

namespace OCA{

  class OCARunReader : public TObject
  {
  public:

    // The constructors and destructor for the OCARunReader object.
    OCARunReader();
    OCARunReader( vector< Int_t >& runIDs, const string dataSet = "" );
    OCARunReader( Int_t runID );
    OCARunReader( const char* filename );
    ~OCARunReader();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Add a OCARun file by runID.
    void Add( Int_t runID );

    // Add OCARun files by runID.
    void Add( vector< Int_t >& runIDs,
              const string dataSet );

    // Add a OCARun file as specified by a list of
    // run IDs in a 'fit-file'.
    void Add( const char* filename );

    // Check to see if a OCARun object given by the 'runID'
    // is currently accessible. i.e. has it been 'added' yet.
    Bool_t CheckForOCARun( Int_t runID );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Returns a pointer to the current OCARun object on the reader.
    OCARun* GetCurrentOCARun(){ return fOCARun; }

    // Returns a pointer to the OCARun with run ID 'runID'.
    OCARun* GetOCARun( Int_t runID );

    // Returns the 'runEntry'-th entry, e.g. for 4 files on the OCARunReader
    // object, GetRunEntry( 1 ) would return the SECOND run file 
    // ( 0 being the first ).
    OCARun* GetRunEntry( Int_t runEntry ){ 
      fOCARunT->GetEntry( runEntry );
      return fOCARun;
    } 

    // Returns the number of OCARuns on the reader object.
    Long64_t GetNOCARuns(){ return fNOCARuns; }

    // Returns a vector of runIDs.
    vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the current OCARun by run ID.
    void SetCurrentOCARun( Int_t runID ){ fOCARun = GetOCARun( runID ); }

    // Set the branch name, might be different for different systematics
    void SetBranchName( const string& brName ){ fBranchName = brName; }

  protected:

    TChain* fOCARunT;                   // TChain of the ROOT files added to the OCARun reader.
    OCARun* fOCARun;                  // Pointer to the current OCARun object.
    
    Long64_t fNext;                       // Index of the next OCARun TTree in the TChain.
    Long64_t fNOCARuns;                 // Total number of OCARun TTrees in the TChain.

    string fBranchName;                   // Branch name for the OCARun file to be used

    vector< Int_t > fListOfRunIDs;        // Vector of run IDs whose corresponding ROOT fies
                                          // have been loaded onto the TChain.

    OCAPMT* fCurrentPMT;                // Pointer to the current OCAPMT object.

    ClassDef( OCARunReader, 0 ) 
    
  };

}

#endif
