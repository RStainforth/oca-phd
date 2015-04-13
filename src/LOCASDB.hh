////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDB.hh
///
/// CLASS: LOCAS::LOCASDB
///
/// BRIEF: Class used to load data from the RAT or LOCAS database
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2014 : RPFS - First Revision, new file.
///
/// DETAIL: This class loads information from the LOCAS database. 
///         In particular, the purpose of this class is to load 
///         information from a 'fit-file',
///         located in the $LOCAS_ROOT/data/fit_files/ directory.
///         Once the specific fit-file has been assigned using
///         LOCASDB::SetFile, this class essentially provides
///         easy access to any number of data types stored in the
///         fit-file fields using the 'getter' functions
///         e.g. LOCASDB::GetStringField, LOCASDB::GetDoubleVectorField
///
///         In short, this object just allows for easy access to
///         the LOCAS specific database files in the same way that
///         the RAT::RATDB class does for RAT database files.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDB_
#define _LOCASDB_

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

namespace LOCAS{

  class LOCASDB : public TObject
  {
  public:

    // The constructor and destructor for the LOCASDB object
    LOCASDB();       
    ~LOCASDB(){ }
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    // 'Clear' resets all the private variable values to how they
    // were upon the LOCASDB object being created i.e. LOCASDB::LOCASDB() call
    void ClearDB();                      
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    // Get the full path directory to where the SOC files are stored 
    string GetSOCRunDir() const { return fSOCRunDir; }

    // Get the full path directory to where the LOCASRun files are stored
    string GetLOCASRunDir() const { return fLOCASRunDir; }

    // Get the full path directory to where the RDT files are stored
    // Note: RDT files traditionally stored by month and year
    string GetRDTRunDir( const string& month = "oct03" ) const { 
      return fRDTRunDir + month + "/"; 
    }

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

    // Set an arbitrary file in .ratdb format from where this 
    // LOCASDB object loads data from
    void SetFile( const char* file ); 
    
    // Set the current full system path name to the SOC file directory.
    void SetSOCRunDir( const string& dirName ){ fSOCRunDir = dirName; }
    
    // Set the current full system path name to the LOCASRun file directory.
    void SetLOCASRunDir( const string& dirName ){ fLOCASRunDir = dirName; }
    
    // Set the current full system path name to the RDT file directory.
    void SetRDTRunDir( const string& dirName ){ fRDTRunDir = dirName; }
    
  private:
    
    RAT::DB* fRATDB;                 // Pointer to the RAT::DB Object
    RAT::DBLinkPtr fRATDBPtr;        // Pointer to the RAT Database

    string fCurrentFile;             // The full path of the current 'fit-file' in use.
    
    string fSOCRunDir;               // The full system path of the directory where 
                                     // the SOC Run files are held
    string fLOCASRunDir;             // The full system path of the directory where 
                                     // the LOCASRun files are held
    string fRDTRunDir;               // The fill system path of the directory where 
                                     // the RDT files are held
    
    ClassDef( LOCASDB, 1 );
    
  };
}

#endif
