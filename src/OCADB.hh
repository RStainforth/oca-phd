////////////////////////////////////////////////////////////////////
///
/// FILENAME: OCADB.hh
///
/// CLASS: OCA::OCADB
///
/// BRIEF: Class used to load data from the RAT or OCA database
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This class loads information from the OCA database. 
///         In particular, the purpose of this class is to load 
///         information from a 'fit-file',
///         located in the $OCA_SNOPLUS_ROOT/data/fit_files/ directory.
///         Once the specific fit-file has been assigned using
///         OCADB::SetFile, this class essentially provides
///         easy access to any number of data types stored in the
///         fit-file fields using the 'getter' functions
///         e.g. OCADB::GetStringField, OCADB::GetDoubleVectorField
///
///         In short, this object just allows for easy access to
///         the OCA specific database files in the same way that
///         the RAT::RATDB class does for RAT database files.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCADB_
#define _OCADB_

#include "RAT/DU/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/DB.hh"

#include "TVector3.h"
#include "TGraph.h"

#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace OCA{

  class OCADB : public TObject
  {
  public:

    // The constructor and destructor for the OCADB object
    OCADB();       
    ~OCADB(){ }
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    // 'Clear' resets all the private variable values to how they
    // were upon the OCADB object being created i.e. OCADB::OCADB() call
    void ClearDB();                      
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    // Get the full path directory to where the SOC files are stored. 
    // SOC files traditionally stored by month and year.
    string GetSOCRunDir( const string& monthYY = "" ) const { 
      return fSOCRunDir + monthYY + "/"; 
    }

    // Get the full path directory to where the OCARun files are stored.
    // OCARun files traditionally stored by month and year.
    string GetOCARunDir( const string& monthYY = "" ) const { 
      return fOCARunDir + monthYY + "/"; 
    }

    // Get the full path directory to where the RDT files are stored
    // RDT files traditionally stored by month and year
    string GetRDTRunDir( const string& monthYY = "" ) const { 
      return fRDTRunDir + monthYY + "/"; 
    }

    // Get the full path directory to where the DQXX files are stored
    // DQXX files traditionally stored by month and year
    string GetDQXXDir( const string& monthYY = "" ) const { 
      return fDQXXDir + monthYY + "/"; 
    }

    // Get the full path directory to the output directory for fits
    string GetOutputDir(){ return fOutputDir; }

    // Get the full path directory to the systematics directory
    string GetSystematicsDir(){ return fSystematicsDir; }

    // Get the full path directory to the fit files directory
    string GetFitFilesDir(){ return fFitFilesDir; }
    
    // Get the full path directory to the fit files directory
    string GetLBFilesDir(){ return fLBFilesDir; }

    // Get a field of type 'string' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional)
    string GetStringField( const string& tableName, 
                           const string& fieldName,
                           const string& indexName = "" );
    
    // Get a field of type 'Double_t' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional)
    Double_t GetDoubleField( const string& tableName, 
                             const string& fieldName,
                             const string& indexName = "" );
    
    // Get a field of type 'Int_t' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional)
    Int_t GetIntField( const string& tableName, 
                       const string& fieldName,
                       const string& indexName = "" );
    
    // Get a field of type 'Bool_t' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional)
    // The boolean field in the 'fit-file' will be given in
    // terms of an integer (0: False), (!= 0: True).
    Bool_t GetBoolField( const string& tableName, 
                         const string& fieldName,
                         const string& indexName = "" );
    
    // Get an array of values all of type 'Int_t' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional). The array of
    // values is stored as a vector object.
    vector< Int_t > GetIntVectorField( const string& tableName, 
                                       const string& fieldName,
                                       const string& indexName = "" );
    
    // Get an array of values all of type 'Double_t' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional). The array of
    // values is stored as a vector object.
    vector< Double_t > GetDoubleVectorField( const string& tableName, 
                                             const string& fieldName, 
                                             const string& indexName = "" );
    
    // Get an array of values all of type 'string' from the current 'fit-file'
    // based on the table name, 'tableName', field name 'fieldName
    // and relevant index 'indexName' (optional). The array of
    // values is stored as a vector object.
    vector< string > GetStringVectorField( const string& tableName, 
                                           const string& fieldName, 
                                           const string& indexName = "" );
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set an arbitrary file in .ocadb format from where this 
    // OCADB object loads data from
    void SetFile( const char* file ); 
    
    // Set the current full system path name to the SOC file directory.
    void SetSOCRunDir( const string& dirName ){ fSOCRunDir = dirName; }
    
    // Set the current full system path name to the OCARun file directory.
    void SetOCARunDir( const string& dirName ){ fOCARunDir = dirName; }
    
    // Set the current full system path name to the RDT file directory.
    void SetRDTRunDir( const string& dirName ){ fRDTRunDir = dirName; }
    
  private:
    
    RAT::DB* fRATDB;                 // Pointer to the RAT::DB Object
    RAT::DBLinkPtr fRATDBPtr;        // Pointer to the RAT Database

    string fCurrentFile;             // The full path of the current 'fit-file' in use.
    
    string fSOCRunDir;               // The full system path of the directory where 
                                     // the SOC Run files are held.
    string fOCARunDir;               // The full system path of the directory where 
                                     // the OCARun files are held.
    string fRDTRunDir;               // The full system path of the directory where 
                                     // the RDT files are held.
    string fDQXXDir;                 // The full system path of the directory where 
                                     // the DQXX files are held.
    string fOutputDir;               // The full system path of the directory where 
                                     // the output files (e.g. fits) are held.
    string fSystematicsDir;          // The full system path of the directory where 
                                     // the systematics files are held.
    string fFitFilesDir;             // The full system path of the directory where 
                                     // the fit files are held.
    string fLBFilesDir;              // The full system path of the directory where 
                                     // the LB Distribution files are held.
    
    ClassDef( OCADB, 1 );
    
  };
}

#endif
