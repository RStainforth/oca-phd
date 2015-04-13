#include "RAT/Log.hh"

#include "LOCASDB.hh"

#include <sstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace LOCAS;

ClassImp( LOCASDB );

LOCASDB::LOCASDB()
{
  
  // First need to ensure that all private variables
  // are set to non-interpretive values:
  // i.e. Pointers set to 'NULL'
  //      string objects are empty '""'
  //      TGraphs are empty '*.Set( 0 )'
  //      vector objects are empty '*.clear()'
  // This is handled by 'LOCASDB::Clear()'
  
  Clear();
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::Clear()
{
  
  // Set pointers to NULL
  fRATDB = NULL;
  fRATDBPtr = NULL;
  
  // Set the TGraphs to be empty
  fInnerAVRI.Set( 0 );
  fAVRI.Set( 0 );
  fWaterRI.Set( 0 );
  
  // The directories for the data stores (SOC, LOCASRun and RDT)
  // are set to empty paths.
  fSOCRunDir = "";
  fLOCASRunDir = "";
  fRDTRunDir = "";
  
  // The current 'fit-file' this DB is accessing from
  // has an empty path
  fCurrentFile = "";
  
  // Now setup the pointer 'fRATDB'
  RAT::Log::Init( "/dev/null" );  
  fRATDB = RAT::DB::Get();
  assert( fRATDB );
  
  // Locate the run directories and full path names
  // for the SOC, LOCASRun and RDT files
  fSOCRunDir = getenv( "LOCAS_DATA" ) + (string)"/runs/soc/";
  fLOCASRunDir = getenv( "LOCAS_DATA" ) + (string)"/runs/locasrun/";
  fRDTRunDir = getenv( "LOCAS_DATA" ) + (string)"/runs/rdt/";
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::SetFile( const char* file )
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

string LOCASDB::GetStringField( const string& tableName, 
                                const string& fieldName,
                                const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty string object.
  string resultStr = ""; 
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetStringField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetStringField: Returning empty string." << endl;
    
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

Double_t LOCASDB::GetDoubleField( const string& tableName, 
                                  const string& fieldName,
                                  const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty Double_t object.
  Double_t resultD = 0.0; 
  
  // Check that the current file location exists, if not return error.
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetDoubleField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetDoubleField: Returning type Double_t = 0.0." << endl;
    
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

Int_t LOCASDB::GetIntField( const string& tableName, 
                            const string& fieldName,
                            const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty Int_t object.
  Int_t resultI = 0; 
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetIntField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetIntField: Returning type Int_t = 0." << endl;
    
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

Bool_t LOCASDB::GetBoolField( const string& tableName, 
                              const string& fieldName,
                              const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty Bool_t object (default to 'false')
  Bool_t resultBool = false;
  
  // Check that the current file location exists, if not return error.
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetIntField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetIntField: Returning type Bool_t = false" << endl;
    
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

vector< Int_t > LOCASDB::GetIntVectorField( const string& tableName,  
                                            const string& fieldName, 
                                            const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty vector of type Int_t values.
  vector< Int_t > intVec;
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetIntVectorField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetIntVectorField: Returning type empty vector" << endl;
    
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

vector< Double_t > LOCASDB::GetDoubleVectorField( const string& tableName,  
                                                  const string& fieldName, 
                                                  const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty vector of type Double_t values.
  vector< Double_t > dubVec;
  
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetDoubleVectorField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetDoubleVectorField: Returning type empty vector" <<endl;
    
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

vector< string > LOCASDB::GetStringVectorField( const string& tableName,  
                                                const string& fieldName, 
                                                const string& indexName )
{
  
  // Clear what the current RATDB pointer object is pointing to.
  fRATDB->Clear();
  
  // Create an empty vector of type string values.
  vector< string > strVec;
  
  // Check that the current file location exists, if not return an error.
  if ( fCurrentFile == "" ){
    cout << "LOCASDB::GetStringVectorField: Error, no current file loaded (use LOCASDB::LoadFile)" << endl;
    cout << "LOCASDB::GetStringVectorField: Returning type empty vector" << endl;
    
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
