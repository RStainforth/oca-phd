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
  LOCASRunReader lReader( runList[0] );

  // Add the subsequent LOCASRun files to the LOCASRunReader
  for ( Int_t iR = 1; iR < runList.size(); iR++ ){ lReader.Add( runList[ iR ] ); }


}
