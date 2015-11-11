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
///         '$OCA_SNOPLUS_ROOT/data/fit_files/' directory.
///         At the end of this executable the result of the fit
///         are written out to a .root file and place in the
///         'OCA_SNOPLUS_ROOT/output/fits/' directory.
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
  OCACmdOptions( ) : fFitFileName( "" ), fSystematic( "" ) { }
  std::string fFitFileName, fSystematic;
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
  string fitPath = ( lDB.GetFitFilesDir() + Opts.fFitFileName );
  cout << "Setting Fitfile: " << fitPath << endl;
  lDB.SetFile( fitPath.c_str() );
  cout << "fitName" << endl;
  std::string fitName = lDB.GetStringField( "FITFILE", "fit_name", "fit_setup" );
  cout << "seedFile" << endl;
  std::string seedFile = lDB.GetStringField( "FITFILE", "seed_initial_parameters", "fit_setup" );
  std::string systematicName = Opts.fSystematic;

  // Create the OCAModelParameterStore object which stores
  // the parameters for the optics model.
  OCAModelParameterStore* lParStore = new OCAModelParameterStore( fitName );
  
  // Seed the parameters...
  if ( seedFile != "" ){ lParStore->SeedParameters( seedFile ); }
  // ...or add the parameters as specified in the fit file
  else{ lParStore->AddParameters( fitPath.c_str() ); }

  // Create the OCAOpticsModel object. This is the object
  // which will use the OCAModelParameter objects to compute
  // a model prediction for the optics model.
  OCAOpticsModel* lModel = new OCAOpticsModel();

  cout << "set model" << endl;
  // Set a link to the pointer to the OCAModelParameterStore
  // object.
  lModel->SetOCAModelParameterStore( lParStore );

  // Get the minimum number of PMT angular response and Laserball
  // distribution bin entires required for the parameter associated
  // with each bin to vary in the fit.
  cout << "minPMTEntries" << endl;
  Int_t minPMTEntries = lDB.GetIntField( "FITFILE", "pmt_angular_response_min_bin_entries", "parameter_setup" );
  cout << "minLBEntries" << endl;
  Int_t minLBDistEntries = lDB.GetIntField( "FITFILE", "laserball_distribution_min_bin_entries", "parameter_setup" );
  lModel->SetRequiredNLBDistributionEntries( minLBDistEntries );
  lModel->SetRequiredNPMTAngularRepsonseEntries( minPMTEntries ); 
  // Add all the run files to the OCARunReader object
  cout << "run_ids" << endl;
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
  cout << "data_set" << endl; 
  std::string dataSet = lDB.GetStringField( "FITFILE", "data_set", "fit_setup" );
  OCARunReader lReader;
  lReader.SetBranchName( systematicName );
  lReader.Add( runIDs, dataSet );
  
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
  cout << "chisqLims" << endl;
  std::vector< Double_t > chiSqLims = lDB.GetDoubleVectorField( "FITFILE", "chisq_lims", "fit_procedure" );

  stringstream myStream;
  string myString = "";

  for ( Size_t iFit = 0; iFit < chiSqLims.size(); iFit++ ){

    lChiSq->EvaluateGlobalResidual();

    myStream << chiSqLims[ iFit ];
    myStream >> myString;
    myStream.clear();

    TH1F* resPlot = new TH1F( ( myString + "-plot" ).c_str(), ( myString + "-plot-name" ).c_str(),
                              100, -10.0, 10.0 );
    
    // Update the chisquare filter to a new maximum limit.
    lFilterStore->UpdateFilter( "filter_chi_square", 
                               ( lFilterStore->GetFilter( "filter_chi_square" ) ).GetMinValue(), 
                               chiSqLims[ iFit ] );

    // lFilterStore->UpdateFilter( "filter_dynamic_residual", 
    //                             lChiSq->GetResidualMean() - 2.0 * lChiSq->GetResidualStd(), 
    //                             lChiSq->GetResidualMean() + 2.0 * lChiSq->GetResidualStd() );

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
    if ( iFit == 1 ){
      *ogStore = *lData;
    }
    else{
      *lData = *ogStore;
    }

    vector< OCAPMT >::iterator iDP;
    vector< OCAPMT >::iterator iDPBegin = lData->GetOCAPMTsIterBegin();
    vector< OCAPMT >::iterator iDPEnd = lData->GetOCAPMTsIterEnd();
    for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
      resPlot->Fill( lChiSq->EvaluateResidual( *iDP ) );
    }

    TCanvas* myC = new TCanvas( "myC", "myC", 600, 400 );
    resPlot->Draw();
    myC->Print( ( myString + "-plot.eps" ).c_str() );
    myString.clear();
    
  }

  // Float_t curChiSq = 0.0;
  // Float_t prevChiSq = -10.0;

  // stringstream myStream;
  // string myString = "";

  // Int_t iCount = 0;
  // while ( ( TMath::Abs( curChiSq - prevChiSq ) > 0.001 ) && iCount < 100 ){
  //   myStream << iCount;
  //   myStream >> myString;
  //   myStream.clear();

  //   TH1F* resPlot = new TH1F( ( myString + "-plot" ).c_str(), ( myString + "-plot-name" ).c_str(),
  //                             100, -10.0, 10.0 );

  //   if ( iCount == 0 ){ prevChiSq = -10.0; }
  //   else{ prevChiSq = curChiSq; }
  //   iCount++;
  //   cout << "On iteration: " << iCount << endl;

  //   lChiSq->EvaluateGlobalResidual();
  //   Float_t upp = lChiSq->GetResidualMean() + ( ( ( 100 - iCount ) * 3.0 ) / 100 ) * lChiSq->GetResidualStd();
  //   Float_t lower = lChiSq->GetResidualMean() - ( ( ( 100 - iCount ) * 3.0 ) / 100 ) * lChiSq->GetResidualStd();
  //   cout << "Mean is: : " << lChiSq->GetResidualMean();
  //   cout << "Upper Limit: " << upp << endl;
  //   cout << "Lower Limit: " << lower << endl;
  //   Float_t upperChiLimit = 0.0;
  //   if ( lower * lower >= upp * upp ){ upperChiLimit = lower * lower; }
  //   else{ upperChiLimit = upp * upp; }
    
  //   // Update the chisquare filter to a new maximum limit.
  //   lFilterStore->UpdateFilter( "filter_chi_square", 
  //                               0.0, upperChiLimit );
  //   lFilterStore->UpdateFilter( "filter_dynamic_residual", 
  //                               lower, upp );

  //   lDataFiller->FilterData( lFilterStore, lData, lChiSq );
  //   lFilterStore->PrintFilterCutInformation();
  //   lFilterStore->ResetFilterConditionCounters();

  //   lChiSq->PerformOpticsFit( iCount );
  //   curChiSq = lChiSq->EvaluateGlobalChiSquare() / ( lData->GetNDataPoints() - lParStore->GetNGlobalVariableParameters() - 1 ) ;
  //   cout << "Current ChiSq: " << curChiSq << endl;
  //   cout << "Previous ChiSq: " << prevChiSq << endl;

  //   if ( iCount == 100 - 1 || TMath::Abs( curChiSq - prevChiSq ) < 0.01 ){
  //     *finalStore = *lData;
  //   }

  //   vector< OCAPMT >::iterator iDP;
  //   vector< OCAPMT >::iterator iDPBegin = lData->GetOCAPMTsIterBegin();
  //   vector< OCAPMT >::iterator iDPEnd = lData->GetOCAPMTsIterEnd();
  //   for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
  //     resPlot->Fill( lChiSq->EvaluateResidual( *iDP ) );
  //   }

  //   TCanvas* myC = new TCanvas( "myC", "myC", 600, 400 );
  //   resPlot->Draw();
  //   myC->Print( ( myString + "-plot.eps" ).c_str() );
  //   myString.clear();
    
  //   //*lData = *ogStore;
    
  // } 

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
    modelPrediction = lModel->ModelPrediction( *iDP );
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
    
    // Calculate the model prediction and the data value
    // of the occupancy.
    modelPrediction = lModel->ModelPrediction( *iDP );
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
    }    
  }
  
  exampleHisto->Draw();
  exampleCanvas->Print("example_pmEff.eps");

  TGraph* myPlot = new TGraph();

  for ( Int_t iIndex = 0; iIndex < 90; iIndex++ ){
    //cout << "effHistos[ iIndex ].GetRMS() / effHistos[ iIndex ].GetMean(): " << effHistos[ iIndex ].GetRMS() / effHistos[ iIndex ].GetMean() << endl;
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

  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/";
  effAngleC->Print( ( filePath + fitName + "_pmt_variability.eps" ).c_str() );

  vector< OCAPMT >::iterator iDPL;
  vector< OCAPMT >::iterator iDPBeginL = lData->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEndL = lData->GetOCAPMTsIterEnd();

  for ( iDPL = iDPBeginL; iDPL != iDPEndL; iDPL++ ) {

    Float_t incAngle = TMath::ACos( iDPL->GetCosTheta() ) * TMath::RadToDeg();
    if ( incAngle >= 0.0 && incAngle < 90.0 ){
      Float_t varPar = fitFunc->GetParameter( 0 )
        + ( fitFunc->GetParameter( 1 ) * incAngle )
        + ( fitFunc->GetParameter( 2 ) * incAngle * incAngle );
      //iDPL->SetPMTVariability( varPar );
      iDPL->SetPMTVariability( myPlot->Eval( incAngle ) );
    }
    else{
      iDPL->SetPMTVariability( -1.0 );
    }

  }

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the OCAModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  //lData->WriteToFile( ( fitName + "_oca2fit.root" ).c_str() );

  // Create the full file path for the output fit file.
  string fitROOTPath = lDB.GetOutputDir() + "fits/" + fitName + ".root";
  string fitRATDBPath = lDB.GetOutputDir() + "fits/" + fitName + ".ratdb";

  // Write the fit to a .root file.
  // These .root files are typically held in the
  // '$OCA_SNOPLUS_ROOT/output/fits/' directory.
  lParStore->WriteToROOTFile( fitROOTPath, systematicName );
  lParStore->WriteToRATDBFile( fitRATDBPath.c_str() );

  lData->WriteToFile( ( fitName + ".root" ).c_str() );
  finalStore->WriteToFile( ( fitName + "_filtered.root" ).c_str() );
  
    
  cout << "\n";
  cout << "#############################" << endl;
  cout << "###### OCA2FIT2EFF END ######" << endl;
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
                                  {"systematic", 1, NULL, 's'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:f:s:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); break;
    case 'f': options.fFitFileName = (std::string)optarg; break;
    case 's': options.fSystematic = (std::string)optarg; break;
    }
    
    c = getopt_long(argc, argv, "h:f:s:", opts, &option_index);
  }
  
  stringstream idStream;

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ OCA - oca2fit" << "\n";
  cout << "Description: This executable performs the OCA optics fit. \n";
  cout << "Usage: oca2fit [-h] [-f fit-file-name] [-s systematic]\n";
  cout << " -h, --help            Display this help message and exit \n";
  cout << " -r, --fit-file-name   Set the run ID for the corresponding SOC run file to be processed for a OCARun fit file \n";
  cout << " -s, --systematic      Set the corresponding central run ID file \n";
  
}

//////////////////////
//////////////////////
