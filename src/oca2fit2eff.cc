////////////////////////////////////////////////////////////////////
///
/// FILENAME: oca2fit2eff.cc
///
/// EXECUTABLE: oca2fit2eff
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
///         by typing 'oca2fit [path-to-fit-file]' where the
///         the '[path-to-fit-file]' is the full system path
///         to the 'fit-file', typically stored in the 
///         '$OCA_ROOT/data/fit_files/' directory.
///         At the end of this executable the result of the fit
///         are written out to a .root file and place in the
///         'OCA_ROOT/output/fits/' directory.
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
#include "TH1F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TStyle.h"

#include <iostream>
#include <string>
#include <map>

using namespace std;
using namespace OCA;

// Declare the functions which will be used in the executable
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Check that the 'oca2minuit' executable has been used properly 
  if ( argc != 2 ){
    cout << "ocafit: Error! No path specified for the OCA fit-file.\n";
    cout << "Usage: oca2minuit /path/to/fit-file.ratdb\n";
    return 1;
  }

  cout << "\n";
  cout << "###############################" << endl;
  cout << "###### OCA2FIT2EFF START ######" << endl;
  cout << "###############################" << endl;
  cout << "\n";

  /////////////////////////////////////////////////////////////
  ////////////////// RUN AND DATA MANAGEMENT //////////////////
  /////////////////////////////////////////////////////////////

  // Initialise the database loader to parse the 'fit-file' passed 
  // at the command line.
  OCADB lDB;
  lDB.SetFile( argv[1] );
  std::string fitName = lDB.GetStringField( "FITFILE", "fit_name", "fit_setup" );
  // Create the OCAModelParameterStore object which stores
  // the parameters for the optics model.
  OCAModelParameterStore* lParStore = new OCAModelParameterStore( fitName );

  // Add the parameters.
  lParStore->AddParameters( argv[1] );

  // Create the OCAOpticsModel object. This is the object
  // which will use the OCAModelParameter objects to compute
  // a model prediction for the optics model.
  OCAOpticsModel* lModel = new OCAOpticsModel();

  // Set a link to the pointer to the OCAModelParameterStore
  // object.
  lModel->SetOCAModelParameterStore( lParStore );

  // Add all the run files to the OCARunReader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" ); 
  std::string dataSet = lDB.GetStringField( "FITFILE", "data_set", "fit_setup" );
  OCARunReader lReader( runIDs, dataSet );
  
  // Create and add the run information to a OCAPMTStore object.
  OCAPMTStore* lData = new OCAPMTStore();
  lData->AddData( lReader );

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
  OCAPMTStore* ogStore = new OCAPMTStore();
  OCAPMTStore* finalStore = new OCAPMTStore();
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
    if ( iFit == chiSqLims.size() - 1 ){
      *finalStore = *lData;
    }
    *lData = *ogStore;
    
  }

  lChiSq->EvaluateGlobalChiSquare();

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the OCAModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  // Now begin the calculation of the relative PMT efficiencies,
  // the PMT variability and the normalised PMT efficiencies.

  // First calculate the raw efficiencies which is:
  // MPE-Corrected-Occupancy / Model Prediction.


  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = finalStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = finalStore->GetOCAPMTsIterEnd();
  
  // Iterate over all the data points and calculate
  // the estimator of the raw efficiency.
  Float_t modelPrediction = 1.0;
  Float_t dataValue = 1.0;
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {

    // Calculate the model prediction and the data value
    // of the occupancy.
    modelPrediction = lModel->ModelPrediction( *iDP );
    dataValue = iDP->GetMPECorrOccupancy();
    cout << "modelPrediction: " << modelPrediction << endl;
    cout << "dataValue: " << dataValue << endl;
    // Set the ratio of the model / data to be the raw estimator
    // for the PMT efficiency.
    iDP->SetRawEfficiency( dataValue / modelPrediction );

  }

  // Now need to calculate the average of each PMT efficiency across each run.
  Int_t nRuns = (Int_t)runIDs.size();

  // Initialise arrays which are used to calculate the average
  // of the raw efficiency across each run.
  Float_t* rawEffSum = new Float_t[ nRuns ];
  Float_t* rawEffAvg = new Float_t[ nRuns ];
  Int_t* nPMTsPerRun = new Int_t [ nRuns ];
  for ( Int_t iRun = 0; iRun < nRuns; iRun++ ){
    rawEffSum[ iRun ] = 0.0;
    rawEffAvg[ iRun ] = 0.0;
    nPMTsPerRun[ iRun ] = 0;
  }

  // We can use a histogram to store all of the raw efficiencies,
  // and then use the standard deviation from the mean of the histogram
  // distribution to provide a dynamic cut for the PMT variability
  // calculation.
  TH1F* effHistoRaw = new TH1F( "PMT Efficiencies", "PMT Relative Raw Efficiencies", 100, 0.0, 1.0 );

  // Now loop over all the data points and add their raw efficiencies to 
  // the corresponding run entry in 'rawEffSum' and increment 'nPMTsPerRun'
  // accordingly. At the end, divide through each sum for each run by the
  // total number of PMTs, i.e. rawEffAvg[ iRun ] = rawEffSum[ iRun ] / nPMTsPerRun[ iRun ].

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    if ( iDP->GetRawEfficiency() > 0.0 && iDP->GetRawEfficiency() < 10.0 ){
      rawEffSum[ iDP->GetRunIndex() ] += iDP->GetRawEfficiency();
      nPMTsPerRun[ iDP->GetRunIndex() ]++;
      effHistoRaw->Fill( iDP->GetRawEfficiency() );      
    }
  }

  // Now we calculate the distribution of the raw efficiencies by
  // incident PMT angle.

  // Create an array which will store the sum of raw efficiencies by
  // incident angle up to 50-degrees.
  Float_t* pmtAngleEffMean = new Float_t[ 51 ];
  Int_t* pmtNAngle = new Int_t[ 51 ];
  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    pmtAngleEffMean[ iIndex ] = 0.0;
    pmtNAngle[ iIndex ] = 0; 
  }

  // Loop over all the data points, and for efficiencies within
  // 3 standard deviations from the mean either side, include them
  // in the sum for the calculation of the PMT variability.
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    // The incident angle.
    Int_t incAngle = (Int_t)( TMath::ACos( iDP->GetCosTheta() ) * TMath::RadToDeg() );

    pmtAngleEffMean[ incAngle ] += iDP->GetRawEfficiency();
    pmtNAngle[ incAngle ]++;
      
    
  }
  
  // Now calculate the mean of each of the raw efficiency distributions
  // by PMT incident angle.
  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    if ( pmtNAngle != 0 ){
      pmtAngleEffMean[ iIndex ] /= pmtNAngle[ iIndex ];
    }
    else{
      pmtAngleEffMean[ iIndex ] = 0.0;
    }
  }
  
  // Now we calculate the variance of the efficiency distributions
  // in each of the incident angle bins.
  Float_t* pmtAngleEffSigma = new Float_t[ 51 ];
  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    pmtAngleEffSigma[ iIndex ] = 0.0;
  }

  // The sum of the individual variances for each data point...
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    // The incident angle.
    Int_t incAngle = (Int_t)( TMath::ACos( iDP->GetCosTheta() ) * TMath::RadToDeg() );
      
    Float_t sigma = iDP->GetRawEfficiency() - pmtAngleEffMean[ incAngle ];
    Float_t sigma2 = sigma * sigma;
    pmtAngleEffSigma[ incAngle ] += sigma2;
    
  }

  // ...dividing through and square-rooting to compute the
  // standard deviation for each PMT incident angle bin.
  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){

    if ( pmtNAngle[ iIndex ] != 0 ){
      pmtAngleEffSigma[ iIndex ] /= pmtNAngle[ iIndex ];
      pmtAngleEffSigma[ iIndex ] = TMath::Sqrt( pmtAngleEffSigma[ iIndex ] );
    }
    else{
      pmtAngleEffSigma[ iIndex ] = 0.0;
    }

  }

  TGraph* myPlot = new TGraph();

  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    if ( pmtAngleEffMean[ iIndex ] != 0.0
         && (Float_t)( pmtAngleEffSigma[ iIndex ] / pmtAngleEffMean[ iIndex ] ) < 1.0 ){
      myPlot->SetPoint( iIndex, 
                        (Float_t)( iIndex + 0.5 ),
                        (Float_t)( pmtAngleEffSigma[ iIndex ] / pmtAngleEffMean[ iIndex ] ) );
    }
  }
  
  TCanvas* effAngleC = new TCanvas( "effAngleC", "PMT Incident Angle Variability", 600, 400 );
  
  myPlot->SetMarkerStyle( 7 );
  myPlot->SetMarkerColor( 2 );
  myPlot->GetXaxis()->SetTitle( "Indicent PMT Angle [degrees]" );
  myPlot->GetYaxis()->SetTitle( "PMT Variability / degree" );
  myPlot->SetTitle( "PMT Variability" );

  TF1* fitFunc = new TF1("fitFunc", "pol2", 0.0, 45.0 );
  myPlot->Fit("fitFunc");
  gStyle->SetOptFit();

  myPlot->Draw( "AP" );

  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/";
  effAngleC->Print( ( filePath + fitName + "_pmt_variability.eps" ).c_str() );

  vector< OCAPMT >::iterator iDPL;
  vector< OCAPMT >::iterator iDPBeginL = lData->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEndL = lData->GetOCAPMTsIterEnd();

  for ( iDPL = iDPBeginL; iDPL != iDPEndL; iDPL++ ) {

    Int_t incAngle = (Int_t)( TMath::ACos( iDPL->GetCosTheta() ) * TMath::RadToDeg() );
    if ( incAngle >= 0 && incAngle < 51 ){
      Float_t varPar = pmtAngleEffSigma[ incAngle ] / pmtAngleEffMean[ incAngle ];
      iDPL->SetPMTVariability( varPar );
    }
    else{
      iDPL->SetPMTVariability( -1.0 );
    }

  }

  lData->WriteToFile( ( fitName + ".root" ).c_str() );
  finalStore->WriteToFile( ( fitName + "_filtered.root" ).c_str() );

  // Now divide through to calculate the average.  
  for ( Int_t iRun = 0; iRun < nRuns; iRun++ ){
    rawEffAvg[ iRun ] = rawEffSum[ iRun ] / nPMTsPerRun[ iRun ];
  }

  // Now we need to calculate the average PMT efficiency across runs.
  Int_t* pmtIDRun = new Int_t[ 10000 ];
  Float_t* pmtIDEff = new Float_t[ 10000 ];
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    pmtIDRun[ iPMT ] = 0;
    pmtIDEff[ iPMT ] = 0.0;
  }

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    if ( iDP->GetRawEfficiency() > 0.0 && iDP->GetRawEfficiency() < 100000000.0 ){
      pmtIDRun[ iDP->GetID() ]++;
      pmtIDEff[ iDP->GetID() ] += iDP->GetRawEfficiency() / rawEffAvg[ iDP->GetRunIndex() ];
    }

  }

  // Now we normalise through by the number of occurences each PMT makes
  // across the runs in the fit.
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){

    if ( pmtIDRun[ iPMT ] == 0 ){ 
      pmtIDEff[ iPMT ] = 0.0; 
    }

    else{
      Float_t effSum = pmtIDEff[ iPMT ];
      pmtIDEff[ iPMT ] = effSum / pmtIDRun[ iPMT ];
    }

  }

  Int_t nPMTs = 0;
  Float_t effSum = 0.0;
  // Now divide each entry by the total sum.

  // First need to check how many PMTs there are
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){

    if ( pmtIDEff[ iPMT ] != 0.0 ){ 
      nPMTs++; 
      effSum += pmtIDEff[ iPMT ];
    }

  }

  Float_t totalOverNPMTs = effSum / (Float_t)nPMTs;

  // Now normalise through each effictive value.
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){

    if ( pmtIDEff[ iPMT ] != 0.0 ){ 
      pmtIDEff[ iPMT ] /= totalOverNPMTs;
    }

  }

  TH1F* effHisto = new TH1F( "PMT Efficiencies", "PMT Relative Normalised Efficiencies", 200, 0.0, 2.0 );

  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    if ( pmtIDEff[ iPMT ] != 0.0 ){
      effHisto->Fill( pmtIDEff[ iPMT ] );
    }
  }

  TCanvas* tCanvas = new TCanvas( "c1", "PMT Relative Efficiencies", 600, 400 );
  effHisto->GetXaxis()->SetTitle( "Relative PMT Efficiency [Normalised] (a.units)" );
  effHisto->GetYaxis()->SetTitle( "No. PMTs / 0.01" );
  effHisto->GetXaxis()->SetTitleOffset( 1.4 );
  effHisto->GetYaxis()->SetTitleOffset( 1.4 );
  
  effHisto->SetLineColor( 1 );
  effHisto->SetLineWidth( 1 );
  effHisto->Draw();
  tCanvas->Print( (filePath + fitName + "_norm_eff.eps").c_str() );

  TCanvas* tCanvasRaw = new TCanvas( "c1Raw", "PMT Relative Raw Efficiencies", 600, 400 );
  effHistoRaw->GetXaxis()->SetTitle( "Relative PMT Efficiency [Raw] (a.units)" );
  effHistoRaw->GetYaxis()->SetTitle( "No. PMTs / 0.01" );
  effHistoRaw->GetXaxis()->SetTitleOffset( 1.4 );
  effHistoRaw->GetYaxis()->SetTitleOffset( 1.4 );
  
  effHistoRaw->SetLineColor( 2 );
  effHistoRaw->SetLineWidth( 1 );
  effHistoRaw->Draw();
  tCanvasRaw->Print( (filePath + fitName + "_raw_eff.eps").c_str() );
    
  cout << "\n";
  cout << "#############################" << endl;
  cout << "###### OCA2FIT2EFF END ######" << endl;
  cout << "#############################" << endl;
  cout << "\n";
  
}
