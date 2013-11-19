///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2fit.cc
///
/// EXECUTABLE: locas2fit
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: 
///
///////////////////////////////////////////////////////////////////////////////////////

#include "RAT/getopt.h"
#include "RAT/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASLightPath.hh"
#include "LOCASRunReader.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace LOCAS;

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Load the run list
  LOCASDB lDB;
  lDB.LoadRunList( argv[1] );

  // Get the vector of run IDs
  vector< Int_t > runList = lDB.GetRunList();

  // Initialise the LOCASRunReader object with the run ID
  // of the first LOCASRun file in the list
  LOCASRunReader lReader( runList );

  for ( Int_t tK = 0; tK < lReader.GetNLOCASRuns(); tK++ ){
    LOCASRun* runPtr = lReader.GetLOCASRun( runList[ tK ] );
    cout << "The run ID is: " << runPtr->GetRunID() << endl;
  }

  Double_t x[5] = {1.0, 1.0, 1.0, 1.0, 1.0};

  
  Float_t chiSquareRun = lReader.EvaluateRunChiSquare( x, runList[ 0 ] );
  
  cout << "The Chi-Square for run: " << runList[ 0 ] << " was: " << chiSquareRun << endl; 

}
