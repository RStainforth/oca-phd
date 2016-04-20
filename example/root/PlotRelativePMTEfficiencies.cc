////////////////////////////////////////////////////////////////////
///
/// FILENAME: PlotRelativePMTEfficiencies.cc
///
/// CLASS: N/A
///
/// BRIEF:  The main purpose of this macro is to demonstrate to 
///         the user how to extract the relative PMT efficiencies
///         from the datastore files following a fit.
///
///         This is an example ROOT macro that plots information held on
///         the OCA datastore files stored in:
///         $OCA_SNOPLUS_ROOT/output/datastore
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2016 : RPFS - First Revision, new file.
///
/// DETAIL: To use this macro, start ROOT in the terminal by
///         typing 'root'. Once ROOT has started type the following
///         to compile the macro:
///
///         .L PlotRelativePMTEfficiencies.cc+
///
///         Once the file has compiled, you may use any of the
///         functions declared in this file to plot information.
///
////////////////////////////////////////////////////////////////////

#include "TH1F.h"
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
OCAPMTStore* GetPMTStorePtr( std::string fileName );
TCanvas* PlotRawRelativePMTEfficiencies( std::string storeName );
TCanvas* PlotNormalisedRelativePMTEfficiencies( std::string storeName );

// Plot the raw relative PMT efficiency estimators from the OCA datastore
// for a particular fit. The 'raw' relative PMT efficiency estimator is that
// as defiend in equation 3.15 in the OCA Documentation.
//
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
//
// NOTE: Only 'filtered' versions of the datastore contain
//       relative PMT efficiency information.
TCanvas* PlotRawRelativePMTEfficiencies( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* rawPMTEff = new TH1F( "Raw Relative PMT Efficiency Estimator", 
                              "Raw Relative PMT Efficiency Estimator", 
                              250, 0.0, 0.5 );
  
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    Float_t rawPMTEffVal = iDP->GetRunEfficiency();
    if( rawPMTEffVal >= 0.0 ){
      rawPMTEff->Fill( rawPMTEffVal );
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Raw PMT Efficiency Estimator", 600, 400 );
  
  rawPMTEff->SetLineWidth( 2 );
  rawPMTEff->SetLineColor( 2 );
  
  rawPMTEff->GetXaxis()->SetTitle( "Raw PMT Efficiency Estimator" );
  rawPMTEff->GetYaxis()->SetTitle( "No. PMTs / 0.004" );
  rawPMTEff->GetXaxis()->SetTitleOffset( 1.2 );
  rawPMTEff->GetYaxis()->SetTitleOffset( 1.2 );
  
  rawPMTEff->Draw();
  gStyle->SetOptStat( 0 );
  cout << "Number of PMTs in fit: " << rawPMTEff->GetEntries() << endl;
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Plot the normalised relative PMT efficiency estimators from the OCA datastore
// for a particular fit. The 'normalised' relative PMT efficiency estimator is that
// as defiend in equation 3.16 in the OCA Documentation. It is normalised across all runs.
// The number of entries in the histogram is therefore the number of unique PMTs
// in the data sample at the end of the fit.
//
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
//
// NOTE: Only 'filtered' versions of the datastore contain
//       relative PMT efficiency information.
TCanvas* PlotNormalisedRelativePMTEfficiencies( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* normPMTEff = new TH1F( "Normalised Relative PMT Efficiency Estimator", 
                              "Normalised Relative PMT Efficiency Estimator", 
                               250, 0.0, 2.0 );
  
  Int_t* uniquePMTs = new Int_t[ 10000 ];
  for ( Int_t iVal = 0; iVal < 10000; iVal++ ){ uniquePMTs[ iVal ] = 0; }

  Float_t pmtEffAvg = 0.0;
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    Float_t normPMTEffVal = iDP->GetScanEfficiency();
    if( normPMTEffVal >= 0.0 && uniquePMTs[ iDP->GetID() ] != 1 ){
      pmtEffAvg += normPMTEffVal;
      uniquePMTs[ iDP->GetID() ] = 1;
    }
  }

  Int_t nUniquePMTs = 0;
  for ( Int_t iVal = 0; iVal < 10000; iVal++ ){
    if ( uniquePMTs[ iVal ] == 1 ){ nUniquePMTs += 1; }
  }
  pmtEffAvg /= nUniquePMTs;

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    Float_t normPMTEffVal = iDP->GetScanEfficiency();
    if( normPMTEffVal >= 0.0 && uniquePMTs[ iDP->GetID() ] == 1 ){
      normPMTEff->Fill( normPMTEffVal / pmtEffAvg );
      uniquePMTs[ iDP->GetID() ] = 2;
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Normalised PMT Efficiency Estimator", 600, 400 );
  
  normPMTEff->SetLineWidth( 2 );
  normPMTEff->SetLineColor( 2 );
  
  normPMTEff->GetXaxis()->SetTitle( "Normalised PMT Efficiency Estimator" );
  normPMTEff->GetYaxis()->SetTitle( "No. PMTs / 0.008" );
  normPMTEff->GetXaxis()->SetTitleOffset( 1.2 );
  normPMTEff->GetYaxis()->SetTitleOffset( 1.2 );
  
  normPMTEff->Draw();
  gStyle->SetOptStat( 0 );
  cout << "Number of unique PMTs in fit: " << normPMTEff->GetEntries() << endl;
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Helper routine to retrieve the OCAPMTStore object from
// the datastore .root file.
OCAPMTStore* GetPMTStorePtr( std::string fileName )
{

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "datastore/" + fileName;
  cout << "Opening File: " << filePath << endl;
  
  // Create a new TFile object to open the OCAPMTStore.
  TFile* dataFile = TFile::Open( filePath.c_str(), "READ" );
  TTree* fileTree = new TTree();
  fileTree = (TTree*)dataFile->Get( ( fileName + ";1" ).c_str() );
  OCAPMTStore* lStore = new OCAPMTStore();
  fileTree->SetBranchAddress( "OCAPMTStore", &lStore );
  fileTree->GetEntry( 0 );

  return lStore;

}
