////////////////////////////////////////////////////////////////////
///
/// FILENAME: locasfit.cc
///
/// EXECUTABLE: locasfit
///
/// BRIEF: The class which defines the optical response model
///        of PMTs.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This executable performs the optics fit and writes
///         the results out to a file. This executable is started
///         by typing 'locas2fit [path-to-fit-file]' where the
///         the '[path-to-fit-file]' is the full system path
///         to the 'fit-file', typically stored in the 
///         '$LOCAS_ROOT/data/fit_files/' directory.
///         At the end of this executable the result of the fit
///         are written out to a .root file and place in the
///         'LOCAS_ROOT/output/fits/' directory.
///
////////////////////////////////////////////////////////////////////

#include "LOCASDataStore.hh"
#include "LOCASDB.hh"
#include "LOCASRunReader.hh"
#include "LOCASDataFiller.hh"
#include "LOCASChiSquare.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASOpticsModel.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"

#include "TTree.h"
#include "TFile.h"
#include "TClass.h"
#include "TH2F.h"
#include "TCanvas.h"

#include <iostream>
#include <string>
#include <map>

using namespace std;
using namespace LOCAS;

// Declare the functions which will be used in the executable
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Check that the 'locas2minuit' executable has been used properly 
  if ( argc != 2 ){
    cout << "locas2fit: Error! No path specified for the LOCAS fit-file.\n";
    cout << "Usage: locas2fit /path/to/fit-file.ratdb\n";
    return 1;
  }

  cout << "\n";
  cout << "#############################" << endl;
  cout << "###### LOCAS2FIT START ######" << endl;
  cout << "#############################" << endl;
  cout << "\n";

  /////////////////////////////////////////////////////////////
  ////////////////// RUN AND DATA MANAGEMENT //////////////////
  /////////////////////////////////////////////////////////////

  // Initialise the database loader to parse the 'fit-file' passed 
  // at the command line.
  LOCASDB lDB;
  lDB.SetFile( argv[1] );
  std::string fitName = lDB.GetStringField( "FITFILE", "fit_name", "fit_setup" );

  // Create the LOCASModelParameterStore object which stores
  // the parameters for the optics model.
  LOCASModelParameterStore* lParStore = new LOCASModelParameterStore( fitName );

  // Add the parameters.
  lParStore->AddParameters( argv[1] );

  // Create the LOCASOpticsModel object. This is the object
  // which will use the LOCASModelParameter objects to compute
  // a model prediction for the optics model.
  LOCASOpticsModel* lModel = new LOCASOpticsModel();

  // Set a link to the pointer to the LOCASModelParameterStore
  // object.
  lModel->SetLOCASModelParameterStore( lParStore );

  // Get the minimum number of PMT angular response and Laserball
  // distribution bin entires required for the parameter associated
  // with each bin to vary in the fit.
  Int_t minPMTEntries = lDB.GetIntField( "FITFILE", "pmt_angular_response_min_bin_entries", "parameter_setup" );
  cout << "minPMT Entries is: " << minPMTEntries << endl;
  Int_t minLBDistEntries = lDB.GetIntField( "FITFILE", "laserball_distribution_histogram_min_bin_entries", "parameter_setup" );
  lModel->SetRequiredNLBDistributionEntries( minLBDistEntries );
  lModel->SetRequiredNPMTAngularRepsonseEntries( minPMTEntries );  
  
  // Create and add the run information to a LOCASDataStore object.
  LOCASDataStore* lData = new LOCASDataStore( fitName );

  //cout << "LOCASDataStore 1 has: " << lData->GetNDataPoints() << endl;
  //cout << "LOCASDataStore 2 has: " << lData2->GetNDataPoints() << endl;

  // Create a pointer to a new LOCASChiSquare and set a link
  // to each of the data and the model.
  LOCASChiSquare* lChiSq = new LOCASChiSquare();
  lChiSq->SetPointerToData( lData );
  lChiSq->SetPointerToModel( lModel );

  // Initalise a separate storage object for all the filters to cut on
  // the data with
  LOCASFilterStore* lFilterStore = new LOCASFilterStore( argv[1] );

  // Initalise a data filler object to filter through the raw
  // data using the filters
  LOCASDataFiller* lDataFiller = new LOCASDataFiller();

  // Backup the original data store which is cut on at the top
  // level as part of each loop iteration below.
  LOCASDataStore* ogStore = new LOCASDataStore( fitName );
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
    lChiSq->PerformOpticsFit();

    // Set the data to the original set of data point values.
    if ( iFit != chiSqLims.size() - 1 ){
      *lData = *ogStore;
    }    
  }

  lChiSq->EvaluateGlobalChiSquare();

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the LOCASModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  Float_t** covMatrix = lParStore->GetCovarianceMatrix();
  Int_t nParameters = lParStore->GetNParameters();

  TH2F* testHisto = new TH2F( "2dHisto", "Covariance Matrix", 90, 1, 90, 90, 1, 90 );
  for ( Int_t iPar = 1; iPar <= 90; iPar++ ){
    for ( Int_t jPar = 1; jPar <= 90; jPar++ ){
      testHisto->SetCellContent( iPar, jPar, covMatrix[ iPar ][ jPar ] );
      
    }
  }

  TCanvas* c1 = new TCanvas( "c1", "2d-histo", 600, 400 );

  testHisto->Draw("COLZ");
  c1->Print("test_histo_gram.eps");

  lData->WriteToFile( "example_finished_data.root" );

  // Create the full file path for the output fit file.
  string fitROOTPath = lDB.GetOutputDir() + "fits/" + fitName + ".root";
  string fitRATDBPath = lDB.GetOutputDir() + "fits/" + fitName + ".ratdb";

  // Write the fit to a .root file.
  // These .root files are typically held in the
  // '$LOCAS_ROOT/output/fits/' directory.
  lParStore->WriteToROOTFile( fitROOTPath.c_str() );
  lParStore->WriteToRATDBFile( fitRATDBPath.c_str() );
    
  cout << "\n";
  cout << "###############################" << endl;
  cout << "###### LOCAS2FIT END ######" << endl;
  cout << "###############################" << endl;
  cout << "\n";
  
}
