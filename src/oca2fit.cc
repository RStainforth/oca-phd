////////////////////////////////////////////////////////////////////
///
/// FILENAME: ocafit.cc
///
/// EXECUTABLE: ocafit
///
/// BRIEF: This executable performs the OCA optics fit over a
///        OCAPMTStore object (produced by oca2fit2eff). This executable
///        is only really applicable for actual REAL data (not MC). Currently
///        this is SNO data, :( . In order
///        to do this you need to specify a 'fitfile' located in
///        the ${OCA_SNOPLUS_ROOT}/data/fitfiles directory.
///        The fitfile defines the parameters and initial values
///        to be used in the fit. The results of the fit are written
///        to the ${OCA_SNOPLUS_ROOT}/output/fits directory. The data
///        which was used is stored in a OCAPMTStore object in the
///        ${OCA_SNOPLUS_ROOT}/output/data directory.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This executable performs the optics fit and writes
///         the results out to a file.
///         E.g. at the command line:
///            
///            oca2fit -f oct15_labppo_337.ocadb
///
///         This would run a fit to the model over the files
///         specificed in the ${OCA_SNOPLUS_ROOT}/data/fitfiles
///         directory. An additional option [-s] is also avaialble
///         E.g
///            oca2fit -f oct15_labppo_337.ocadb -s seed_file.root
///
///         The '-s' denotes the option to seed the parameters from values
///         stored on a previous fit file in the ${OCA_SNOPLUS_ROOT}/output/fits
///         directory.
///
////////////////////////////////////////////////////////////////////

#include "OCAPMTStore.hh"
#include "OCADB.hh"
#include "OCARunReader.hh"
#include "OCADataFiller.hh"
#include "OCAChiSquare.hh"
#include "OCAModelParameterStore.hh"
#include "OCAOpticsModel.hh"
#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCAMath.hh"

#include "TTree.h"
#include "TFile.h"
#include "TClass.h"
#include "TH2F.h"
#include "TCanvas.h"

#include <iostream>
#include <string>
#include <map>
#include <getopt.h>

using namespace std;
using namespace OCA;

class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fFitFileName( "" ), fSystematic( "" ), fSeedFile( "" ) { }
  std::string fFitFileName, fSystematic, fSeedFile;
};

// Declare the functions which will be used in the executable
OCACmdOptions ParseArguments( int argc, char** argv );
void help();
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );

  cout << "\n";
  cout << "#############################" << endl;
  cout << "###### OCA2FIT START ######" << endl;
  cout << "#############################" << endl;
  cout << "\n";

  /////////////////////////////////////////////////////////////
  ////////////////// RUN AND DATA MANAGEMENT //////////////////
  /////////////////////////////////////////////////////////////

  // Initialise the database loader to parse the 'fit-file' passed 
  // at the command line.
  OCADB lDB;
  string fitPath = ( lDB.GetFitFilesDir() + Opts.fFitFileName );
  cout << "Setting Fitfile: " << fitPath << endl;
  lDB.SetFile( fitPath.c_str() );
  std::string fitName = lDB.GetStringField( "FITFILE", "fit_name", "fit_setup" );
  std::string seedFile = Opts.fSeedFile;
  std::string systematicName = Opts.fSystematic;

  // Create the OCAModelParameterStore object which stores
  // the parameters for the optics model.
  OCAModelParameterStore* lParStore = new OCAModelParameterStore( fitName );
  lParStore->SetSystematicName( systematicName );

  // Seed the parameters...
  if ( seedFile != "" ){ 
    if ( !lParStore->SeedParameters( seedFile, fitPath ) ){
      cout << "Unsuccessful seed! Abort" << endl;
      return 1;
    }
  }
  // ...or add the parameters as specified in the fit file
  else{ lParStore->AddParameters( fitPath ); }

  // Create the OCAOpticsModel object. This is the object
  // which will use the OCAModelParameter objects to compute
  // a model prediction for the optics model.
  OCAOpticsModel* lModel = new OCAOpticsModel();

  // Set a link to the pointer to the OCAModelParameterStore
  // object.
  lModel->SetOCAModelParameterStore( lParStore );

  // Note this only changes the state of the parameters for the following
  // systematics: distance_to_pmt, laserball_intensity2, laserball_intensity_flat
  // All other systematics have their changes applied at the OCARun level.
  lModel->ApplySystematics();

  // Get the minimum number of PMT angular response and Laserball
  // distribution bin entires required for the parameter associated
  // with each bin to vary in the fit.
  Int_t minPMTEntries = lDB.GetIntField( "FITFILE", "pmt_angular_response_min_bin_entries", "parameter_setup" );
  cout << "TEST2" << endl;
  Int_t minLBDistEntries = lDB.GetIntField( "FITFILE", "laserball_distribution_min_bin_entries", "parameter_setup" );
  cout << "TEST3" << endl;
  lModel->SetRequiredNLBDistributionEntries( minLBDistEntries );
  lModel->SetRequiredNPMTAngularRepsonseEntries( minPMTEntries );  
  
  // Create and add the run information to a OCAPMTStore object.
  OCAPMTStore* lData = new OCAPMTStore( fitName );

  // Create a pointer to a new OCAChiSquare and set a link
  // to each of the data and the model.
  OCAChiSquare* lChiSq = new OCAChiSquare();
  lChiSq->SetPointerToData( lData );
  lChiSq->SetPointerToModel( lModel );

  // Initalise a separate storage object for all the filters to cut on
  // the data with
  OCAFilterStore* lFilterStore = new OCAFilterStore( argv[1] );

  // Initalise a data filler object to filter through the raw
  // data using the filters
  OCADataFiller* lDataFiller = new OCADataFiller();

  // Backup the original data store which is cut on at the top
  // level as part of each loop iteration below.
  OCAPMTStore* ogStore = new OCAPMTStore( fitName );
  *ogStore = *lData;
  lModel->InitialiseLBRunNormalisations( lData );

  // Retrieve information about the fitting procedure 
  // i.e. what subsequent values of the chisquare to cut on 
  // following each round of fitting.
  std::vector< Double_t > chiSqLims = lDB.GetDoubleVectorField( "FITFILE", "chisq_lims", "fit_procedure" );

  for ( Size_t iFit = 0; iFit < chiSqLims.size(); iFit++ ){
    
    // Update the chisquare filter to a new maximum limit.
    lFilterStore->UpdateFilter( "filter_chi_square", 
                               ( lFilterStore->GetFilter( "filter_chi_square" ) ).GetMinValue(), 
                               chiSqLims[ iFit ] );
    
    // Filter the data.
    lDataFiller->FilterData( lFilterStore, lData, lChiSq );

    // Print the filter cut information.
    lFilterStore->PrintFilterCutInformation();

    // Print how amny data points passed/failed this round of
    // filter cuts.
    lFilterStore->ResetFilterConditionCounters();

    // Perform the optics fit.
    lChiSq->PerformOpticsFit( iFit );

    // Set the data to the original set of data point values.
    if ( iFit != chiSqLims.size() - 1 ){
      *lData = *ogStore;
    }    
  }

  lChiSq->EvaluateGlobalChiSquare();

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the OCAModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  lData->WriteToFile( ( fitName + "_oca2fit.root" ).c_str() );

  // Create the full file path for the output fit file.
  string fitROOTPath = lDB.GetOutputDir() + "fits/" + fitName + ".root";
  string fitRATDBPath = lDB.GetOutputDir() + "fits/" + fitName + ".ratdb";

  // Write the fit to a .root file.
  // These .root files are typically held in the
  // '$OCA_SNOPLUS_ROOT/output/fits/' directory.
  string par = "";
  lParStore->WriteToROOTFile( fitROOTPath, par  );
  lParStore->WriteToRATDBFile( fitRATDBPath.c_str() );
    
  cout << "\n";
  cout << "###############################" << endl;
  cout << "###### OCA2FIT END ######" << endl;
  cout << "###############################" << endl;
  cout << "\n";
  
}

///////////////////////////
///                     ///
///  UTILITY FUNCTIONS  ///
///                     ///
///////////////////////////

OCACmdOptions ParseArguments( int argc, char** argv) 
{
  static struct option opts[] = { {"help", 0, NULL, 'h'},
                                  {"fit-file-name", 1, NULL, 'f'},
                                  {"systematic-branch", 1, NULL, 'b'},
                                  {"seed-file", 1, NULL, 's'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:f:b:s:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); break;
    case 'f': options.fFitFileName = (std::string)optarg; break;
    case 'b': options.fSystematic = (std::string)optarg; break;
    case 's': options.fSeedFile = (std::string)optarg; break;
    }
    
    c = getopt_long(argc, argv, "h:f:b:s:", opts, &option_index);
  }
  
  stringstream idStream;

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ OCA - oca2fit" << "\n";
  cout << "Description: This executable performs the OCA Optics fit. \n";
  cout << "Usage: oca2fit [-f fit-file-name] [-b branch-systematic] [-s seed-file (optional)]\n";
  cout << " -r, --fit-file-name          The name of the fit file in the ${OCA_SNOPLUS_ROOT}/data/fitfiles directory \n";
  cout << " -b, --systematic-branch      Set the systematic branch on the OCARun files on which to perform the fit over \n";
  cout << " -s, --seed-file              (Optional) The name of a previous fit from which to seed in the ${OCA_SNOPLUS_ROOT}/output/fits directory \n"; 
  
}

//////////////////////
//////////////////////
