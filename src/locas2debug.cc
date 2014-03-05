///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2debug.cc
///
/// EXECUTABLE: locas2debug
///
/// BRIEF: Debugging executable to test various LOCAS functionality
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
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

int main( int argc, char** argv );
void chisquare( Int_t &npar, Double_t *gin, Double_t &f, Double_t* par, Int_t iflag );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  cout << "\n";
  cout << "##############################" << endl;
  cout << "######### DEBUG Start ########" << endl;
  cout << "##############################" << endl;
  cout << "\n";

  // Initialise the database loader to parse the cardfile
  LOCASDB lDB;
  lDB.SetFile( argv[1] );

  // Add all the run files to the run-reader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" ); 
  LOCASRunReader lReader( runIDs );

  // Create a storage object for all the raw data points and
  // write it to file
  LOCASRawDataStore lRawDataStore( lReader );
  lRawDataStore.WriteToFile();

  // Initalise the storage object for all the filters to cut on
  // the raw data with
  LOCASFilterStore lFilterStore( argv[1] );

  // Initalise the model to be used in the chisquare function
  LOCASOpticsModel lModel( argv[1] );

  
  // Initalise a data filler object to filter through the raw
  // data using the filters
  LOCASDataFiller lDataFiller( lRawDataStore, lFilterStore );

  // Obtain the data to be used in the fitting process
  LOCASDataStore lDataStore = lDataFiller.GetData();
  lDataStore.WriteToFile();

  Int_t nDP = lDataStore.GetNDataPoints();

  // Add the model and data to the chisquare object
  lChiSq->AddModel( lModel );

  TGraph* graph = new TGraph();
  Int_t val = 0;

  LOCASDataStoreStore lStoreStore;

  for ( Float_t par = 5000.0; par <= 20000.0; par += 100.0 ){

    LOCASDataStore lParStore;
    lParStore = lDataStore;
    lChiSq->AddData( lParStore );

    lParStore.SetScintPar( par );
    lParStore.SetAVPar( lModel.GetAVPar() );
    lParStore.SetWaterPar( par );

    lModel.SetScintPar( par );
    lModel.SetWaterPar( par );
    
    Float_t chiSq = lChiSq->EvaluateGlobalChiSquare( lParStore );
    graph->SetPoint( val++, par/1000.0, (Float_t)chiSq / ( nDP - 1 ) );
    
    lStoreStore.AddDataStore( lParStore );
    lChiSq->ClearData();
  }

  lStoreStore.WriteToFile();
  cout << "Number of data points: " << nDP << endl;

  graph->SetTitle( "'lightwater_sno' Attenuation Scan (True Value: 7m)" );
  graph->GetXaxis()->SetTitle( "'lightwater_sno' Attenuation Length (m)" );
  graph->GetYaxis()->SetTitle( "Reduced Chi-Square Value" );
  graph->GetYaxis()->SetTitleOffset( 1.3 );

  TCanvas* canvas = new TCanvas( "chi-sq-plot", "Chi-Square Scan", 640, 400 );

  graph->SetLineWidth( 2 );
  graph->SetMarkerColor( 1 );

  graph->Draw( "ALP" );

  canvas->Print( "test_chi_square_scan.eps" );


  cout << "\n";
  cout << "##############################" << endl;
  cout << "######### DEBUG END ########" << endl;
  cout << "##############################" << endl;
  cout << "\n";

}

// The chisqare function to minimis over
void chisquare( Int_t &npar, Double_t *gin, Double_t &f, Double_t* par, Int_t iflag )
{ 
  f = lChiSq->EvaluateGlobalChiSquare( par ); 
}
