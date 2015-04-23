////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2fit2eff.cc
///
/// EXECUTABLE: locas2fit2eff
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
#include "TH1F.h"
#include "TCanvas.h"
#include "TGraph.h"

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
    cout << "locasfit: Error! No path specified for the LOCAS fit-file.\n";
    cout << "Usage: locas2minuit /path/to/fit-file.ratdb\n";
    return 1;
  }

  cout << "\n";
  cout << "#################################" << endl;
  cout << "###### LOCAS2FIT2EFF START ######" << endl;
  cout << "#################################" << endl;
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

  // Add all the run files to the LOCASRunReader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" ); 
  std::string dataSet = lDB.GetStringField( "FITFILE", "data_set", "fit_setup" );
  LOCASRunReader lReader( runIDs, dataSet );
  
  // Create and add the run information to a LOCASDataStore object.
  LOCASDataStore* lData = new LOCASDataStore();
  lData->AddData( lReader );

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
  LOCASDataStore* ogStore = new LOCASDataStore();
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
    *lData = *ogStore;
    
  }

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the LOCASModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  // Now begin the calculation of the relative PMT efficiencies.

  // First calculate the raw efficiencies which is:
  // MPE-Corrected-Occupancy / Model Prediction.

  vector< LOCASDataPoint >::iterator iDP;
  vector< LOCASDataPoint >::iterator iDPBegin = lData->GetLOCASDataPointsIterBegin();
  vector< LOCASDataPoint >::iterator iDPEnd = lData->GetLOCASDataPointsIterEnd();
  
  Float_t modelPrediction = 1.0;
  Float_t dataValue = 1.0;
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {

    modelPrediction = lModel->ModelPrediction( *iDP );
    dataValue = iDP->GetMPECorrOccupancy();
    iDP->SetRawEfficiency( modelPrediction / dataValue );

  }

  // Now need to calculate the average of each PMT efficiency across each run.
  Int_t nRuns = (Int_t)runIDs.size();
  Float_t* rawEffSum = new Float_t[ nRuns ];
  Float_t* rawEffAvg = new Float_t[ nRuns ];
  Int_t* nPMTsPerRun = new Int_t [ nRuns ];
  for ( Int_t iRun = 0; iRun < nRuns; iRun++ ){
    rawEffSum[ iRun ] = 0.0;
    rawEffAvg[ iRun ] = 0.0;
    nPMTsPerRun[ iRun ] = 0;
  }

  TCanvas* tCanvasRaw = new TCanvas( "c2", "PMT Raw Efficiencies", 400, 600 );
  TH1F* effHistoRaw = new TH1F( "effHistoRaw", "PMT Raw Efficiencies", 1000, 0.0, 10.0 );

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

  effHistoRaw->GetXaxis()->SetTitle( "PMT Raw Efficiency (a.units)" );
  effHistoRaw->GetYaxis()->SetTitle( "N_{PMTs} / 0.01" );
  effHistoRaw->GetXaxis()->SetTitleOffset( 1.4 );
  effHistoRaw->GetYaxis()->SetTitleOffset( 1.4 );
  
  effHistoRaw->SetLineColor( 1 );
  effHistoRaw->SetLineWidth( 1 );
  effHistoRaw->Draw();
  tCanvasRaw->Print( "eff_pmt_histo_raw.eps" );

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

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    Int_t incAngle = (Int_t)iDP->GetIncidentAngle();
    if ( iDP->GetRawEfficiency() > 2.0 && iDP->GetRawEfficiency() < 6.0
         && incAngle >= 0 && incAngle < 51 ){

      pmtAngleEffMean[ incAngle ] += iDP->GetRawEfficiency();
      pmtNAngle[ incAngle ]++;
      
    }
    
  }

  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    if ( pmtNAngle != 0 ){
      pmtAngleEffMean[ iIndex ] /= pmtNAngle[ iIndex ];
    }
    else{
      pmtAngleEffMean[ iIndex ] = 0.0;
    }
  }

  // Now we create the variance
  Float_t* pmtAngleEffSigma = new Float_t[ 51 ];
  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    pmtAngleEffSigma[ iIndex ] = 0.0;
  }

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    Int_t incAngle = (Int_t)iDP->GetIncidentAngle();
    if ( iDP->GetRawEfficiency() > 2.0 && iDP->GetRawEfficiency() < 6.0
         && incAngle >= 0 && incAngle < 51 ){
      
      Float_t sigma = iDP->GetRawEfficiency() - pmtAngleEffMean[ incAngle ];
      Float_t sigma2 = sigma * sigma;
      pmtAngleEffSigma[ incAngle ] += sigma2;
      
    }
    
  }

  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){

    if ( pmtNAngle[ iIndex ] != 0 ){
      pmtAngleEffSigma[ iIndex ] /= pmtNAngle[ iIndex ];
    }
    else{
      pmtAngleEffSigma[ iIndex ] = 0.0;
    }

  }

  for ( Int_t iIndex = 0; iIndex < 51; iIndex++ ){
    cout << "PMT Variability Mean index: " << iIndex << " has mean: " << pmtAngleEffMean[ iIndex ] << endl;
    cout << "PMT Variability Sigma index: " << iIndex << " has sigma: " << pmtAngleEffSigma[ iIndex ] << endl;
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

  myPlot->SetMarkerStyle( 2 );
  myPlot->SetMarkerColor( 2 );
  myPlot->GetXaxis()->SetTitle( "Indicent PMT Angle [degrees]" );
  myPlot->GetYaxis()->SetTitle( "PMT Variability / degree" );
  myPlot->SetTitle( "PMT Variability" );

  myPlot->Draw( "AP" );
  
  effAngleC->Print( "pmt_variability.eps" );

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {

    Int_t incAngle = (Int_t)iDP->GetIncidentAngle();
    if ( incAngle >= 0 && incAngle < 51 ){
      iDP->SetPMTVariability( pmtAngleEffSigma[ incAngle ] / pmtAngleEffMean[ incAngle ] );
    }
    else{
      iDP->SetPMTVariability( -1.0 );
    }

  }

  lData->WriteToFile( ( fitName + ".root" ).c_str() );

  // COMMENTED OUT BELOW IS THE CALCULATION
  // FOR THE NORMALISED RELATIVE PMT EFFICIENCIES.

  // // Now divide through to calculate the average.  
  // for ( Int_t iRun = 0; iRun < nRuns; iRun++ ){
  //   rawEffAvg[ iRun ] = rawEffSum[ iRun ] / nPMTsPerRun[ iRun ];
  // }

  // // Now we need to calculate the average PMT efficiency across runs.
  // Int_t* pmtIDRun = new Int_t[ 10000 ];
  // Float_t* pmtIDEff = new Float_t[ 10000 ];
  // for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
  //   pmtIDRun[ iPMT ] = 0;
  //   pmtIDEff[ iPMT ] = 0.0;
  // }

  // for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
  //   if ( iDP->GetRawEfficiency() > 0.0 && iDP->GetRawEfficiency() < 100000000.0 ){
  //     pmtIDRun[ iDP->GetPMTID() ]++;
  //     pmtIDEff[ iDP->GetPMTID() ] += iDP->GetRawEfficiency() / rawEffAvg[ iDP->GetRunIndex() ];
  //   }

  // }

  // // Now we normalise through by the number of occurences each PMT makes
  // // across the runs in the fit.
  // for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){

  //   if ( pmtIDRun[ iPMT ] == 0 ){ 
  //     pmtIDEff[ iPMT ] = 0.0; 
  //   }

  //   else{
  //     Float_t effSum = pmtIDEff[ iPMT ];
  //     pmtIDEff[ iPMT ] = effSum / pmtIDRun[ iPMT ];
  //   }

  // }

  // Int_t nPMTs = 0;
  // Float_t effSum = 0.0;
  // // Now divide each entry by the total sum.

  // // First need to check how many PMTs there are
  // for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){

  //   if ( pmtIDEff[ iPMT ] != 0.0 ){ 
  //     nPMTs++; 
  //     effSum += pmtIDEff[ iPMT ];
  //   }

  // }

  // Float_t totalOverNPMTs = effSum / (Float_t)nPMTs;

  // // Now normalise through each effictive value.
  // for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){

  //   if ( pmtIDEff[ iPMT ] != 0.0 ){ 
  //     pmtIDEff[ iPMT ] /= totalOverNPMTs;
  //   }

  // }

  // TH1F* effHisto = new TH1F( "effHisto", "PMT Relative Efficiencies", 200, 0.0, 2.0 );

  // for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
  //   if ( pmtIDEff[ iPMT ] != 0.0 ){
  //     effHisto->Fill( pmtIDEff[ iPMT ] );
  //   }
  // }

  // TCanvas* tCanvas = new TCanvas( "c1", "PMT Relative Efficiencies", 400, 600 );
  // effHisto->GetXaxis()->SetTitle( "Relative PMT Efficiency [Normalised] (a.units)" );
  // effHisto->GetYaxis()->SetTitle( "NPMTs / 0.01" );
  // effHisto->GetXaxis()->SetTitleOffset( 1.4 );
  // effHisto->GetYaxis()->SetTitleOffset( 1.4 );
  
  // effHisto->SetLineColor( 2 );
  // effHisto->SetLineWidth( 1 );
  // effHisto->Draw();
  // tCanvas->Print( "eff_pmt_histo.eps" );

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
  cout << "###### LOCAS2FIT2EFF END ######" << endl;
  cout << "###############################" << endl;
  cout << "\n";
  
}
