#include "RAT/Log.hh"

#include "OCADB.hh"

#include <sstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace OCA;

ClassImp( OCADB )

OCADB::OCADB()
{
  
  // First need to ensure that all private variables
  // are set to non-interpretive values:
  // i.e. Pointers set to 'NULL'
  //      string objects are empty '""'
  //      TGraphs are empty '*.Set( 0 )'
  //      vector objects are empty '*.clear()'
  // This is handled by 'OCADB::ClearDB()'
  
  ClearDB();
  
}

//////////////////////////////////////
//////////////////////////////////////

void OCADB::ClearDB()
{
  
  // Set pointers to NULL
  fRATDB = NULL;
  fRATDBPtr = NULL;
  
  // The directories for the data stores (SOC, OCARun and RDT)
  // are set to empty paths.
  fSOCRunDir = "";
  fOCARunDir = "";
  fRDTRunDir = "";
  fOutputDir = "";
  fSystematicsDir = "";
  
  // The current 'fit-file' this DB is accessing from
  // has an empty path
  fCurrentFile = "";
  
  // Now setup the pointer 'fRATDB'
  RAT::Log::Init( "/dev/null" );  
  fRATDB = RAT::DB::Get();
  assert( fRATDB );
  
  // Locate the run directories and full path names
  // for the SOC, OCARun and RDT files
  fSOCRunDir = getenv( "OCA_SNOPLUS_DATA" ) + (string)"/runs/soc/";
  fOCARunDir = getenv( "OCA_SNOPLUS_DATA" ) + (string)"/runs/ocarun/";
  fRDTRunDir = getenv( "OCA_SNOPLUS_DATA" ) + (string)"/runs/rdt/";
  fOutputDir = getenv( "OCA_SNOPLUS_ROOT" ) + (string)"/output/";
  fSystematicsDir = getenv( "OCA_SNOPLUS_DATA" ) + (string)"/systematics/";
  
}

//////////////////////////////////////
//////////////////////////////////////

void OCADB::SetFile( const char* file )
{
  
  // Create the string stream object. This will allow us to convert
  // a 'const char*' type value into a 'string' type
  stringstream myStream;
  
  // Ensure the current file is set to nothing before resetting it.
  fCurrentFile = "";
  
  // 'Eat' the file path to the stringstream object
  myStream << file;
  
  // 'Feed' the filepath into the current file private variable
  // as a string object
  myStream >> fCurrentFile;
  
}

//////////////////////////////////////
//////////////////////////////////////

string OCADB::GetStringField( const string& tableName, 
                              const string& fieldName,
                              const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty string object.
  string resultStr = ""; 
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetStringField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetStringField: Returning empty string." << endl;
    
    return resultStr;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  // If so, retrive and return the string value 
  // as stored in the required field.
  resultStr = fRATDBPtr->GetS( fieldName );
  return resultStr;
  
}

//////////////////////////////////////
//////////////////////////////////////

Double_t OCADB::GetDoubleField( const string& tableName, 
                                const string& fieldName,
                                const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty Double_t object.
  Double_t resultD = 0.0; 
  
  // Check that the current file location exists, if not return error.
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetDoubleField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetDoubleField: Returning type Double_t = 0.0." << endl;
    
    return resultD;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName  );
  assert( fRATDBPtr );
  
  // If so, retrive and return the Double_t value 
  // as stored in the required field.
  resultD = fRATDBPtr->GetD( fieldName );
  return resultD;
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t OCADB::GetIntField( const string& tableName, 
                          const string& fieldName,
                          const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty Int_t object.
  Int_t resultI = 0; 
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetIntField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetIntField: Returning type Int_t = 0." << endl;
    
    return resultI;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  // If so, retrive and return the Int_t value 
  // as stored in the required field.
  resultI = fRATDBPtr->GetI( fieldName );
  return resultI;
  
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t OCADB::GetBoolField( const string& tableName, 
                            const string& fieldName,
                            const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty Bool_t object (default to 'false')
  Bool_t resultBool = false;
  
  // Check that the current file location exists, if not return error.
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetIntField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetIntField: Returning type Bool_t = false" << endl;
    
    return resultBool;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  // If so, retrive and return the Int_t value 
  // as stored in the required field, and convert
  // to a Bool_t accordingly
  Int_t resultI = 0;
  resultI = fRATDBPtr->GetI( fieldName );
  
  // Conversion of Int type to bool
  if ( resultI == 0 ){ resultBool = false; }
  else if ( resultI != 0 ){ resultBool = true; }
  
  return resultBool;
  
}

//////////////////////////////////////
//////////////////////////////////////

vector< Int_t > OCADB::GetIntVectorField( const string& tableName,  
                                          const string& fieldName, 
                                          const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty vector of type Int_t values.
  vector< Int_t > intVec;
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetIntVectorField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetIntVectorField: Returning type empty vector" << endl;
    
    return intVec;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  // If so, retrive and return the Int_t value array 
  // as stored in the required field.
  intVec = fRATDBPtr->GetIArray( fieldName );
  
  return intVec;
  
}

//////////////////////////////////////
//////////////////////////////////////

vector< Double_t > OCADB::GetDoubleVectorField( const string& tableName,  
                                                const string& fieldName, 
                                                const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty vector of type Double_t values.
  vector< Double_t > dubVec;
  
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetDoubleVectorField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetDoubleVectorField: Returning type empty vector" <<endl;
    
    return dubVec;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  // If so, retrive and return the Double_t value array 
  // as stored in the required field.
  dubVec = fRATDBPtr->GetDArray( fieldName );
  
  return dubVec;
  
}

//////////////////////////////////////
//////////////////////////////////////

vector< string > OCADB::GetStringVectorField( const string& tableName,  
                                              const string& fieldName, 
                                              const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty vector of type string values.
  vector< string > strVec;
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "OCADB::GetStringVectorField: Error, no current file loaded (use OCADB::LoadFile)" << endl;
    cout << "OCADB::GetStringVectorField: Returning type empty vector" << endl;
    
    return strVec;
  }
  
  // Load the file.
  fRATDB->LoadFile( fCurrentFile );
  
  // Once the table is loaded, check that the table name and
  // index field both exist.
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  // If so, retrive and return the string value array 
  // as stored in the required field.
  strVec = fRATDBPtr->GetSArray( fieldName );
  
  return strVec;
  
}
