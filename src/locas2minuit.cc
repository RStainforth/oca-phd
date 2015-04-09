///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2minuit.cc
///
/// EXECUTABLE: locas2minuit
///
/// BRIEF: This executable performs a minuit minimising routine
///        over the data
///          
/// AUTQHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "LOCASRawDataStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASDB.hh"
#include "LOCASRunReader.hh"
#include "LOCASDataFiller.hh"
#include "LOCASChiSquare.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASOpticsModel.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDataStoreStore.hh"

#include "TMinuit.h"

#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"

#include <iostream>
#include <map>

using namespace std;
using namespace LOCAS;

// Initialise a global LOCASChiSquare object
LOCASChiSquare* lChiSq = new LOCASChiSquare();
LOCASOpticsModel* lModel = new LOCASOpticsModel();
LOCASDataStore* lData = new LOCASDataStore();
LOCASModelParameterStore* lParStore = new LOCASModelParameterStore();

// Declare the functions which will be used in the executable
int main( int argc, char** argv );
//void chisquare( Int_t &npar, Double_t *gin, Double_t &f, Double_t* par, Int_t iflag );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  cout << "\n";
  cout << "################################" << endl;
  cout << "###### LOCAS2MINUIT START ######" << endl;
  cout << "################################" << endl;
  cout << "\n";

  /////////////////////////////////////////////////////////////
  ////////////////// RUN AND DATA MANAGEMENT //////////////////
  /////////////////////////////////////////////////////////////

  //LOCASModelParameterStore* lParStore = new LOCASModelParameterStore();
  lParStore->AddParameters( argv[1] );
  lParStore->PrintParameterInfo();

  //LOCASOpticsModel* lModel = new LOCASOpticsModel();
  lModel->SetLOCASModelParameterStore( lParStore );

  // Initialise the database loader to parse the cardfile passed as the command line
  LOCASDB lDB;
  cout << "Setting lDB File" << endl;
  lDB.SetFile( argv[1] );

  // Add all the run files to the LOCASRunReader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" ); 
  cout << "Creating LOCASRunReader" << endl;
  LOCASRunReader lReader( runIDs );

  // Add the run information to the LOCASDataStore object
  cout << "Adding data to LOCASDataStore" << endl;
  //LOCASDataStore* lData = new LOCASDataStore();
  lData->AddData( lReader );

  //LOCASChiSquare* lChiSq = new LOCASChiSquare();
  lChiSq->SetPointerToData( lData );
  lChiSq->SetPointerToModel( lModel );

  // Initalise a separate storage object for all the filters to cut on
  // the data with
  //cout << "Adding Filters" << endl;
  //LOCASFilterStore lFilterStore( argv[1] );

  // Initalise a data filler object to filter through the raw
  // data using the filters
  //LOCASDataFiller lDataFiller;
  //cout << "DataFiller Created" << endl;
  //lDataFiller.FilterData( lFilterStore, lData, lChiSq );
  //lFilterStore.PrintFilterCutInformation();
  //cout << "DataFiller Filtering" << endl;

  // Write the data to file to begin with
  //lData->WriteToFile("april_datastore.root");

  // // Setup the model to be used in the chisquare function
  // cout << "Model Setup" << endl;
  // lModel->ModelSetup( argv[1] );
  // // Link the chisquare function to the model
  // cout << "Setting Link" << endl;
  // lChiSq->SetPointerToModel( lModel );

  // // Initalise a separate storage object for all the filters to cut on
  // // the data with
  // cout << "Adding Filters" << endl;
  // LOCASFilterStore lFilterStore( argv[1] );

  // ///////////////////////////////////////////////////////////
  // //////// MODEL, INPUT DATA AND CHISQUARE MANAGEMENT ///////
  // ///////////////////////////////////////////////////////////

  // // Initalise a data filler object to filter through the raw
  // // data using the filters
  // LOCASDataFiller lDataFiller;
  // cout << "DataFiller Created" << endl;
  // lDataFiller.FilterData( lFilterStore, lData, lChiSq );
  // lFilterStore.PrintFilterCutInformation();
  // cout << "DataFiller Filtering" << endl;

  // // Write the data to file to begin with
  // lData->WriteToFile("example_datastore.root");

  // // Now that we have the raw data, have performed the initial cuts and prepared the model we can begin to perform the fit!

  // // Retrieve information about the fitting procedure i.e. what subsequent values of the chisquare to cut
  // // on following each round of fitting.
  // std::vector< Double_t > chiSqLims = lDB.GetDoubleVectorField( "FITFILE", "chisq_lims", "fit_procedure" );

  // // Begin the fitting loop, in the first instance
  // // we fit using the initial parameters as initialised in the card file; performing this fit on
  // // the initial LOCASDataStore object of data points.
  // // After the first loop, the chisquare limits are checked again, seeded by the parameters obtained
  // // at the end of the previous fit.

  // for ( Int_t iFit = 0; iFit < chiSqLims.size(); iFit++ ){

  //   // Update the chisquare filter
  //   lFilterStore.UpdateFilter( "filter_chi_square", 
  //                              ( lFilterStore.GetFilter( "filter_chi_square" ) ).GetMinValue(), 
  //                              chiSqLims[ iFit ] );
    
  //   lDataFiller.FilterData( lFilterStore, lData, lChiSq );

  //   // Initalise TMinuit with the number of parameters and assign the function to minimise over
  //   TMinuit *gMinuit = new TMinuit( lModel->GetNParameters() );
  //   gMinuit->SetFCN( chisquare );
    
  //   Double_t arglist[10];
  //   Int_t ierflg = 0;
  //   arglist[0] = 1;
  //   gMinuit->mnexcm( "SET ERR", arglist, 1, ierflg );
    
  //   // Loop over all the parameters in the model and add them to the TMinuit process
  //   std::vector< LOCASModelParameter >::iterator iPar;
    
  //   for ( iPar = lModel->GetParametersIterBegin();
  //         iPar != lModel->GetParametersIterEnd();
  //         iPar++ ){
  //     gMinuit->mnparm( iPar->GetIndex(), iPar->GetParameterName(), 
  //                      iPar->GetInitialValue(), iPar->GetIncrementValue(), 
  //                      iPar->GetMinValue(), iPar->GetMaxValue(), 
  //                      ierflg );
  //   }
    
  //   arglist[0] = 500;
  //   arglist[1] = 1.0;
    
  //   // Perform the minimisation
  //   gMinuit->mnexcm( "MIGRAD", arglist, 2, ierflg );
    
  //   // Print statistics afterwards with the error matrices
  //   Double_t amin, edm, errdef;
  //   Int_t nvpar, nparx, icstat;
  //   gMinuit->mnstat( amin, edm, errdef, nvpar, nparx, icstat );

  // }
    
    cout << "\n";
    cout << "##############################" << endl;
    cout << "###### LOCAS2MINUIT END ######" << endl;
    cout << "##############################" << endl;
    cout << "\n";

}

// The chisqare function to minimis over
// void chisquare( Int_t &npar, Double_t *gin, Double_t &f, Double_t* par, Int_t iflag )
// {   
//   lModel->SetParameters( par );
//   f = lChiSq->EvaluateGlobalChiSquare();
// }
