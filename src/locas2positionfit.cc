///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2positionfit.cc
///
/// EXECUTABLE: locas2positionfit
///
/// BRIEF: This executable takes as input a runlist file, and runs over
///        each run in the scan set to perform a laserball fit of the laserball
///        position
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "LOCASDB.hh"
#include "LOCASRunReader.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASFitLBPosition.hh"

#include <vector>
#include <fstream>

#include <map>


using namespace std;
using namespace RAT;
using namespace LOCAS;

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Initialise the database loader to parse the cardfile passed as the command line
  LOCASDB lDB;
  lDB.SetFile( argv[1] );

  // Add all the run files to the LOCASRunReader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids" );
  LOCASRunReader lReader( runIDs );
  LOCASFitLBPosition* fitLB = new LOCASFitLBPosition( lReader, "geo/sno_d2o.geo" );
  map< Int_t, vector< Double_t > > lbPosFits;

  printf( "Number of runs is: %i \n", (Int_t)runIDs.size() );
  for ( Int_t iRun = 0; iRun < runIDs.size(); iRun++ ){
    printf( "Run Number: %i || Run ID: %i \n", iRun, (Int_t)runIDs[ iRun ] );
    fitLB->FitLBPosition( runIDs[ iRun ] );
  }

  return 0;

}
