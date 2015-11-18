////////////////////////////////////////////////////////////////////
///
/// FILENAME: oca2fit.cc
///
/// EXECUTABLE: oca2fit
///
/// BRIEF: This executable performs the OCA optics fit. In order
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
///     11/2015 : RPFS - First Revision, new file.
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
///         Once the fit has been completed, the PMT efficiencies are 
///         computed in order to calculate the PMT-PMT variablity. These
///         values are then put onto the OCAPMT objects (the data points
///         in the fit) such that they can be used for a subsequent fit
///         using oca2fit with the '-v' option enabled. It is expected that for MC data
///         the '-v' option will often not be used should be sufficient. 
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
#include <sstream>
#include <map>
#include <getopt.h>

using namespace std;
using namespace OCA;

class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fFitFileName( "" ), fSystematic( "" ), fSeedFile( "" ), fPMTVariability( false ) { }
  std::string fFitFileName, fSystematic, fSeedFile;
  bool fPMTVariability;
};

// Declare the functions which will be used in the executable
OCACmdOptions ParseArguments( int argc, char** argv );
void help();
void CalculatePMTToPMTVariability( OCAPMTStore* finalDataStore, 
                                   OCAPMTStore* fullDataStore,
                                   OCAOpticsModel* ocaModel, 
                                   string& fitNameStr );
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );

  cout << "\n";
  cout << "###############################" << endl;
  cout << "###### OCA2FIT START ######" << endl;
  cout << "###############################" << endl;
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
  Int_t minLBDistEntries = lDB.GetIntField( "FITFILE", "laserball_distribution_min_bin_entries", "parameter_setup" );
  lModel->SetRequiredNLBDistributionEntries( minLBDistEntries );
  lModel->SetRequiredNPMTAngularRepsonseEntries( minPMTEntries );
 
  // Add all the run files to the OCARunReader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
  std::string dataSet = lDB.GetStringField( "FITFILE", "data_set", "fit_setup" );

  // Create and add the run information to a OCAPMTStore object.
  OCAPMTStore* lData;

  if ( !Opts.fPMTVariability ){
    lData = new OCAPMTStore();
    OCARunReader lReader;
    
    if ( systematicName == "distance_to_pmt"
         || systematicName == "laserball_distribution2"
         || systematicName == "laserball_distribution_flat"
         || systematicName == "chi_square_lim_16"
         || systematicName == "chi_square_lim_9"
         || systematicName == "chi_square_lim_res_rms" ){
      lReader.SetBranchName( "nominal" );
    }
    else{ lReader.SetBranchName( systematicName ); }
    lReader.Add( runIDs, dataSet );
    
    lData->AddData( lReader );
  }
  else{
    lData = new OCAPMTStore( fitName );
  }
  // Create a pointer to a new OCAChiSquare and set a link
  // to each of the data and the model.
  OCAChiSquare* lChiSq = new OCAChiSquare();
  lChiSq->SetPointerToData( lData );
  lChiSq->SetPointerToModel( lModel );

  // Initalise a separate storage object for all the filters to cut on
  // the data with
  OCAFilterStore* lFilterStore = new OCAFilterStore( fitPath.c_str() );

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
  Bool_t updateFinalChiSqLim = false;
  std::vector< Double_t > chiSqLims = lDB.GetDoubleVectorField( "FITFILE", "chisq_lims", "fit_procedure" );
  if ( systematicName == "chi_square_lim_16" ){
    cout << "ChiSquare systematic: Setting upper limit to 16.0" << endl;
    Float_t chis[] = { 1000.0, 100.0, 50.0, 25.0, 16.0, 16.0 };
    chiSqLims.resize( 6 );
    chiSqLims.assign( chis, chis + 6 );
  }
  if ( systematicName == "chi_square_lim_9" ){
    cout << "ChiSquare systematic: Setting upper limit to 9.0" << endl;
    Float_t chis[] = { 1000.0, 100.0, 50.0, 25.0, 16.0, 9.0, 9.0 };
    chiSqLims.resize( 7 );
    chiSqLims.assign( chis, chis + 7 );
  }
  Int_t nInitChiSqLims = (Int_t)chiSqLims.size();

  stringstream myStream;
  string myString = "";

  for ( Size_t iFit = 0; iFit < chiSqLims.size(); iFit++ ){

    lChiSq->EvaluateGlobalChiSquareResidual();

    myStream << chiSqLims[ iFit ];
    myStream >> myString;
    myStream.clear();

    TH1F* resPlot = new TH1F( ( myString + "-plot" ).c_str(), ( myString + "-plot-name" ).c_str(),
                              100, -10.0, 10.0 );
    
    // Update the chisquare filter to a new maximum limit.
    lFilterStore->UpdateFilter( "filter_chi_square", 
                               ( lFilterStore->GetFilter( "filter_chi_square" ) ).GetMinValue(), 
                               chiSqLims[ iFit ] );

    // Filter the data.
    lDataFiller->FilterData( lFilterStore, lData, lChiSq );

    // Perform the optics fit.
    lChiSq->PerformOpticsFit( iFit );

    *finalStore = *lData;
    *lData = *ogStore;

    vector< OCAPMT >::iterator iDP;
    vector< OCAPMT >::iterator iDPBegin = lData->GetOCAPMTsIterBegin();
    vector< OCAPMT >::iterator iDPEnd = lData->GetOCAPMTsIterEnd();
    for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
      resPlot->Fill( lChiSq->EvaluateChiSquareResidual( *iDP ) );
    }

    TCanvas* myC = new TCanvas( "myC", "myC", 600, 400 );
    resPlot->Draw();
    myC->Print( ( myString + "-plot.eps" ).c_str() );
    myString.clear();

    if ( ( iFit == nInitChiSqLims - 1 ) && !updateFinalChiSqLim
         && systematicName != "chi_square_lim_16" 
         && systematicName != "chi_square_lim_9" ){
      chiSqLims.push_back( resPlot->GetRMS() * resPlot->GetRMS() );
      chiSqLims.push_back( resPlot->GetRMS() * resPlot->GetRMS() );
      updateFinalChiSqLim = true;
      cout << "oca2fit: Added final chi square limit based on residual RMS^2 = " << resPlot->GetRMS() * resPlot->GetRMS() << endl;
    }
    
  } 

  lChiSq->EvaluateGlobalChiSquare();
  lChiSq->EvaluateGlobalChiSquareResidual();

  if ( Opts.fPMTVariability ){ CalculatePMTToPMTVariability( finalStore, lData, lModel, fitName ); }

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the OCAModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  // Create the full file path for the output fit file.
  string fitROOTPath = lDB.GetOutputDir() + "fits/" + fitName + ".root";

  // Write the fit to a .root file.
  // These .root files are typically held in the
  // '$OCA_SNOPLUS_ROOT/output/fits/' directory.
  lParStore->WriteToROOTFile( fitROOTPath, systematicName );

  if ( systematicName == "nominal" ){
    string fitRATDBPath = lDB.GetOutputDir() + "fits/" + fitName + ".ratdb";
    lParStore->WriteToRATDBFile( fitRATDBPath.c_str() );
  }

  if ( systematicName == "nominal" ){
    lData->WriteToFile( ( fitName + "_unfiltered.root" ).c_str() );
    finalStore->WriteToFile( ( fitName + "_filtered.root" ).c_str() );
  }
    
  cout << "\n";
  cout << "#############################" << endl;
  cout << "###### OCA2FIT END ######" << endl;
  cout << "#############################" << endl;
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
                                  {"pmt-to-pmt-variability", 0, NULL, 'v'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:f:b:s:v", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); break;
    case 'f': options.fFitFileName = (std::string)optarg; break;
    case 'b': options.fSystematic = (std::string)optarg; break;
    case 's': options.fSeedFile = (std::string)optarg; break;
    case 'v': options.fPMTVariability = true; break;
    }
    
    c = getopt_long(argc, argv, "h:f:b:s:v", opts, &option_index);
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
  cout << " -v, --pmt-to-pmt-variability (Optional) Requires no argument. Use in order to disable/enable PMT-to-PMT variability correction in the occupancy ratio calculation. Note: this *only* works if a prior oca2fit without \'-v\' has been done \n";  
  
}

//////////////////////
//////////////////////

void CalculatePMTToPMTVariability( OCAPMTStore* finalDataStore, 
                                   OCAPMTStore* fullDataStore,
                                   OCAOpticsModel* ocaModel,
                                   string& fitNameStr )
{

  // First calculate the raw efficiencies which is:
  // MPE-Corrected-Occupancy / Model Prediction.
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = finalDataStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = finalDataStore->GetOCAPMTsIterEnd();

  // Iterate over all the data points and calculate
  // the estimator of the raw efficiency.
  Float_t modelPrediction = 1.0;
  Float_t dataValue = 1.0;
  Float_t pmtEff = 0.0;

  TH1F* effHistos = new TH1F[ 90 ];
  for ( Int_t iHist = 0; iHist < 90; iHist++ ){
    effHistos[ iHist ].SetBins( 10000.0, 0.0, 10.0 );
  }

  // Now need to calculate the average of each PMT efficiency across each run.

  // Initialise arrays which are used to calculate the average
  // of the raw efficiency across each run.
  Float_t* rawEffSum = new Float_t[ 10000 ];
  Float_t* rawEffAvg = new Float_t[ 10000 ];
  Float_t rawEffAvgTot = 0.0;
  Int_t* nPMTsPerIndex = new Int_t[ 10000 ];
  Int_t* nUniquePMTs = new Int_t[ 10000 ];
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    rawEffSum[ iPMT ] = 0.0;
    rawEffAvg[ iPMT ] = 0.0;
    nPMTsPerIndex[ iPMT ] = 0;
    nUniquePMTs[ iPMT ] = 0;
  }

  Float_t rawEff = 0.0;
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    // Calculate the model prediction and the data value
    // of the occupancy.
    modelPrediction = ocaModel->ModelPrediction( *iDP );
    dataValue = iDP->GetMPECorrOccupancy();
    rawEff = dataValue / modelPrediction;

    if ( rawEff > 0.0 ){
      nPMTsPerIndex[ iDP->GetID() ]++;
      rawEffAvg[ iDP->GetID() ] += rawEff;
      if ( nUniquePMTs[ iDP->GetID() ] == 0 ){
        nUniquePMTs[ iDP->GetID() ]++;
      }
    }
  }

  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    if ( nPMTsPerIndex[ iPMT ] > 0 ){
      rawEffAvg[ iPMT ] /= nPMTsPerIndex[ iPMT ];
      if ( rawEffAvg[ iPMT ] != 0.0 ){
        rawEffAvgTot += rawEffAvg[ iPMT ];
      }
    }
  }

  Int_t nGoodPMTs = 0;
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    nGoodPMTs += nUniquePMTs[ iPMT ];
  }
  rawEffAvgTot /= (Float_t)nGoodPMTs;


  TCanvas* exampleCanvas = new TCanvas( "example-canvas", "example canvas", 600, 400 );
  TH1F* exampleHisto = new TH1F("example-histo", "example histo", 1000.0, 0.0, 10.0 );
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    
    if ( iDP->GetLBPos().Mag() > 1000.0 ){
      // Calculate the model prediction and the data value
      // of the occupancy.
      modelPrediction = ocaModel->ModelPrediction( *iDP );
      dataValue = iDP->GetMPECorrOccupancy();
      pmtEff = dataValue / modelPrediction;
      
      
      // The incident angle.
      Int_t incAngle = (Int_t)( TMath::ACos( iDP->GetCosTheta() ) * TMath::RadToDeg() );
      Float_t varVal = ( ( pmtEff / rawEffAvg[ iDP->GetID() ] ) / rawEffAvgTot );
      exampleHisto->Fill( varVal );
      Float_t statVal = TMath::Sqrt( 1.0 / iDP->GetPromptPeakCounts() );
      //cout << "StatVal is: " << statVal << endl;
      Float_t histoVal = TMath::Sqrt( ( varVal * varVal ) - ( statVal * statVal ) );
      //cout << "HistoVal is: " << varVal << endl;
      //cout << "------------" << endl;
      
      if ( histoVal > 0.0 && !std::isnan( pmtEff ) && !std::isinf( pmtEff ) ){
        effHistos[ incAngle ].Fill( histoVal );
        iDP->SetRawEfficiency( histoVal );
      }
    }
  }
  
  exampleHisto->Draw();
  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/";
  exampleCanvas->Print( ( filePath + fitNameStr + "_pmt_efficiencies.eps" ).c_str() );
  exampleCanvas->Print( ( filePath + fitNameStr + "_pmt_efficiencies.root" ).c_str() );

  TGraph* myPlot = new TGraph();

  for ( Int_t iIndex = 0; iIndex < 90; iIndex++ ){
    if ( effHistos[ iIndex ].GetMean() != 0.0
         && (Float_t)( effHistos[ iIndex ].GetRMS() / effHistos[ iIndex ].GetMean() ) < 1.0 ){
      myPlot->SetPoint( iIndex, 
                        (Float_t)( iIndex + 0.5 ),
                        (Float_t)( effHistos[ iIndex ].GetRMS() / effHistos[ iIndex ].GetMean() ) );
    }
  }
  
  TCanvas* effAngleC = new TCanvas( "effAngleC", "PMT Incident Angle Variability", 600, 400 );
  
  myPlot->SetMarkerStyle( 7 );
  myPlot->SetMarkerColor( 2 );
  myPlot->GetXaxis()->SetTitle( "Indicent PMT Angle [degrees]" );
  myPlot->GetYaxis()->SetTitle( "PMT Variability / degree" );
  myPlot->SetTitle( "PMT Variability" );

  TF1* fitFunc = new TF1("fitFunc", "pol2", 2.0, 43.0 );
  myPlot->Fit("fitFunc");
  gStyle->SetOptFit();

  myPlot->Draw( "AP" );

  effAngleC->Print( ( filePath + fitNameStr + "_pmt_variability.eps" ).c_str() );
  effAngleC->Print( ( filePath + fitNameStr + "_pmt_variability.root" ).c_str() );

  vector< OCAPMT >::iterator iDPL;
  vector< OCAPMT >::iterator iDPBeginL = fullDataStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEndL = fullDataStore->GetOCAPMTsIterEnd();

  for ( iDPL = iDPBeginL; iDPL != iDPEndL; iDPL++ ) {

    Float_t incAngle = TMath::ACos( iDPL->GetCosTheta() ) * TMath::RadToDeg();
    if ( incAngle >= 0.0 && incAngle < 90.0 ){
      Float_t varPar = fitFunc->GetParameter( 0 )
        + ( fitFunc->GetParameter( 1 ) * incAngle )
        + ( fitFunc->GetParameter( 2 ) * incAngle * incAngle );
      iDPL->SetPMTVariability( varPar );
    }
    else{
      iDPL->SetPMTVariability( -1.0 );
    }

  }
}
