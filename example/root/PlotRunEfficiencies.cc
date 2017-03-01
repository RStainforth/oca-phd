////////////////////////////////////////////////////////////////////
///
/// FILENAME: PlotRunEfficiencies.cc
///
/// CLASS: N/A
///
/// BRIEF: This macro extracts and plots the scan averaged PMT 
///        efficiencies and run PMT efficiencies from the OCA 
///        pmt-variability files stored in:
///        $OCA_SNOPLUS_ROOT/fits/pmt-variability
///          
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     02/2017 :  A.S.Inacio - First Version.
///
/// DETAIL: To use this macro, start ROOT in the terminal by
///         typing 'root'. Once ROOT has started type the following
///         to compile the macro:
///
///         .L PlotRunEfficiencies.cc+
///
///         Once the file has compiled, you may use any of the
///         functions declared in this file to plot information.
///
////////////////////////////////////////////////////////////////////

#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"

#include "OCAPMTStore.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include <string>
#include <vector>

using namespace std;
using namespace OCA;

// Declare the functions in this macro
vector< Float_t > GetRawEffAvg( std::string fileName );
vector< Float_t > GetRunEff( std::string fileName );
vector< Float_t > GetPMTCosTheta( std::string fileName );
vector< Float_t > GetPMTPhi( std::string fileName );

TCanvas* PlotPMTAvgEfficiencies( std::string fileName );
TCanvas* PlotPMTRunEfficiencies( std::string fileName );

// Plot the raw PMT efficiencies from the OCA for a particular fit
// 'fileName' - This should be the name of the PMT variability .root file 
// in the $OCA_SNOPLUS_ROOT/output/fits directory.
TCanvas* PlotPMTAvgEfficiencies( std::string fileName )
{
  
  // Start by plotting the average raw efficiencies of the PMTs,
  // both the distribution and a PMT map.
  vector< Float_t > rawEffAvg   = GetRawEffAvg( fileName );
  vector< Float_t > pmtCosTheta = GetPMTCosTheta( fileName );
  vector< Float_t > pmtPhi      = GetPMTPhi( fileName );
 
  TH1F* HAEFF = new TH1F("Average Efficiency Distribution", "Average Efficiency Distribution", 250, 0.0, 0.5);

  for( Int_t i = 0; i < 10000; i++  ){
    if( rawEffAvg[ i ] > 0 ){
      HAEFF->Fill( rawEffAvg[ i ] );
    }
  }

  TCanvas* c1 = new TCanvas( "c1", "PMT Average Efficiency Distribution", 600, 400 );
  
  HAEFF->SetLineWidth( 2 );
  HAEFF->SetLineColor( 2 );
  
  HAEFF->GetXaxis()->SetTitle( "PMT Average Efficiency Distribution" );
  HAEFF->GetYaxis()->SetTitle( "No. PMTs" );
  HAEFF->GetXaxis()->SetTitleOffset( 1.2 );
  HAEFF->GetYaxis()->SetTitleOffset( 1.2 );
  
  HAEFF->Draw();
  gStyle->SetOptStat( 0 );

  TH2F *HMAP = new TH2F("HMAP","Map of PMT Average Efficiencies",320,-200.,200.,320,-1.2,1.2);

  for( size_t lcn = 0; lcn < 10000; lcn++ ){
    for( Int_t ef = 0; ef < rawEffAvg[ lcn ] * 100; ef++ ){
      HMAP->Fill( (pmtPhi[ lcn ] * ( 180.0 / TMath::Pi() )), pmtCosTheta[ lcn ] );
    }
  }
 
  TCanvas* c2 = new TCanvas( "c2", "PMT Average Efficiency Map", 600, 400 );
  
  HMAP->GetXaxis();
  HMAP->SetXTitle("PMT Position #phi [degrees]");
  HMAP->GetYaxis();
  HMAP->SetYTitle("PMT Position Cos(#theta)");
  HMAP->SetMarkerColor( 1 );
  HMAP->SetMarkerStyle( 1 );
  HMAP->SetMarkerSize( 1 );
  HMAP->SetStats(kFALSE);
  HMAP->Draw("colz");
  HMAP->SetMaximum(22.0);
  HMAP->SetMinimum(0.0);
  c2->Update();
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Plot the raw PMT efficiencies from the OCA for each run of a particular fit
// 'fileName' - This should be the name of the PMT variability .root file 
// in the $OCA_SNOPLUS_ROOT/output/fits directory.
TCanvas* PlotPMTRunEfficiencies( std::string fileName )
{
  char outplots[] = "socmerged_plots.pdf";
  // Start by plotting the raw efficiencies of the PMTs,
  // both the distribution and a PMT map.
  vector< Float_t > runEff      = GetRunEff( fileName );
  vector< Float_t > pmtCosTheta = GetPMTCosTheta( fileName );
  vector< Float_t > pmtPhi      = GetPMTPhi( fileName );
 
  TCanvas* c1 = new TCanvas( "c1", "Run Efficiency Distribution", 600, 400 );
  c1->Print(Form("%s[",outplots));
  TH1F* HREFF[ 39 ];
  for( Int_t nRun = 0; nRun < 39; nRun ++ ){
    Char_t hName1[ 100 ], hName2[ 100 ];
    sprintf( hName1, "Run #%d Efficiency Distribution", (Int_t)(nRun+1) );
    sprintf( hName2, "Run #%d Efficiency Distribution", (Int_t)(nRun+1) );
    HREFF[ nRun ] = new TH1F( hName1, hName2, 250, 0.0, 0.5 );

    for( Int_t i = 0; i < 10000; i++  ){
      if( runEff[ nRun*10000 + i ] > 0 ){
	HREFF[ nRun ]->Fill( runEff[ nRun*10000 + i ] );
      }
    }

    HREFF[ nRun ]->SetLineWidth( 2 );
    HREFF[ nRun ]->SetLineColor( 2 );
  
    HREFF[ nRun ]->GetXaxis()->SetTitle( "Run Efficiency Distribution" );
    HREFF[ nRun ]->GetYaxis()->SetTitle( "No. PMTs" );
    HREFF[ nRun ]->GetXaxis()->SetTitleOffset( 1.2 );
    HREFF[ nRun ]->GetYaxis()->SetTitleOffset( 1.2 );
  
    c1->Clear();
    HREFF[ nRun ]->Draw();
    c1->Print(outplots);

  }
  c1->Print(Form("%s]",outplots));
 
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Get the raw averaged efficiencies from the file and returns them
vector< Float_t > GetRawEffAvg( std::string fileName )
{

  vector< Float_t > efficiencies;

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fileName;
  cout << "Opening File: " << filePath << endl;
  TFile *dataFile = TFile::Open( filePath.c_str(), "READ" );

  TTree *fileTree = new TTree();

  Float_t rawEffAvg[ 10000 ];
  for( Int_t i = 0; i < 10000; i++ ){ rawEffAvg[ i ] = 0; }

  fileTree = (TTree*)dataFile->Get( "pmt-efficiencies;1" );
  fileTree->SetBranchAddress( "rawEffAvg", &rawEffAvg );

  for ( Int_t i = 0; i < 10000; i++ ){

    fileTree->GetEntry( i );

    efficiencies.push_back( rawEffAvg[ i ] );

  }

  fileTree->ResetBranchAddresses();

  return efficiencies;

  dataFile->Close();
  delete dataFile;

}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Get the raw run efficiencies from the file and returns them
vector< Float_t > GetRunEff( std::string fileName )
{

  vector< Float_t > runEfficiencies;

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fileName;
  cout << "Opening File: " << filePath << endl;
  TFile *dataFile = TFile::Open( filePath.c_str(), "READ" );

  TTree *fileTree = new TTree();

  Float_t runEff[ 10000 ];
  for( Int_t i = 0; i < 10000; i++ ){ runEff[ i ] = 0; }

  fileTree = (TTree*)dataFile->Get( "pmt-efficiencies;1" );

  for( Int_t l = 1; l < 40; l++ ){
    Char_t name[ 100 ];
    sprintf( name, "rawEffRun%d", l );
    fileTree->SetBranchAddress( name, &runEff );
    for ( Int_t i = 0; i < 10000; i++ ){

      fileTree->GetEntry( i );

      runEfficiencies.push_back( runEff[ i ] );

    }

    for( Int_t i = 0; i < 10000; i++ ){ runEff[ i ] = 0; }
  }

  fileTree->ResetBranchAddresses();

  return runEfficiencies;

  dataFile->Close();
  delete dataFile;

}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Get the PMT cosTheta coordinate for mapping the efficiencies
vector< Float_t > GetPMTCosTheta( std::string fileName )
{

  vector< Float_t > cosTheta;

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fileName;
  cout << "Opening File: " << filePath << endl;
  TFile *dataFile = TFile::Open( filePath.c_str(), "READ" );

  TTree *fileTree = new TTree();
  Float_t pmtsCosTheta[ 10000 ];
  for( Int_t i = 0; i < 10000; i++ ){ pmtsCosTheta[ i ] = 0; }

  fileTree = (TTree*)dataFile->Get( "pmt-efficiencies;1" );
  fileTree->SetBranchAddress( "pmtsCosTheta", &pmtsCosTheta );

  for ( Int_t i = 0; i < 10000; i++ ){

    fileTree->GetEntry( i );

    cosTheta.push_back( pmtsCosTheta[ i ] );

  }

  fileTree->ResetBranchAddresses();

  return cosTheta;

  dataFile->Close();
  delete dataFile;
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Get the PMT phi coordinate for mapping the efficiencies
vector< Float_t > GetPMTPhi( std::string fileName )
{

  vector< Float_t > phi;

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fileName;
  cout << "Opening File: " << filePath << endl;
  TFile *dataFile = TFile::Open( filePath.c_str(), "READ" );

  TTree *fileTree = new TTree();

  Float_t pmtsPhi[ 10000 ];
  for( Int_t i = 0; i < 10000; i++ ){ pmtsPhi[ i ] = 0; }

  fileTree = (TTree*)dataFile->Get( "pmt-efficiencies;1" );
  fileTree->SetBranchAddress( "pmtsPhi", &pmtsPhi );

  for ( Int_t i = 0; i < 10000; i++ ){

    fileTree->GetEntry( i );

    phi.push_back( pmtsPhi[ i ] );

  }

  fileTree->ResetBranchAddresses();

  return phi;

  dataFile->Close();
  delete dataFile;

}
