////////////////////////////////////////////////////////////////////
///
/// FILENAME: PlotOCADataStoreInformation.cc
///
/// CLASS: N/A
///
/// BRIEF:  The main purpose of this macro is to demonstrate to 
///         the user how to use some of the OCA classes in an
///         analysis.
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
///         .L PlotOCADataStoreInformation.cc+
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
TCanvas* PlotRawOccupancyRatio( std::string storeName );
TCanvas* PlotDistancesInInnerAV( std::string storeName );
TCanvas* PlotDistancesInOuterAV( std::string storeName );
TCanvas* PlotDistancesInAV( std::string storeName );
TCanvas* PlotIncidentAngles( std::string storeName );

// Plot the ratio of un-corrected occupancies for each PMT in the data store.
// By 'un-corrected' this means no accounting for solid angle or fresnel correction.
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
TCanvas* PlotRawOccupancyRatio( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* rOccHisto = new TH1F( "Raw Occupancy Ratio", 
                              "Raw Occupancy Ratio Histogram", 
                              71, -0.02, 1.42 );
  
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    if( iDP->GetMPECorrOccupancy() != 0.0
        && iDP->GetCentralMPECorrOccupancy() != 0.0 ){
      rOccHisto->Fill( iDP->GetMPECorrOccupancy() / iDP->GetCentralMPECorrOccupancy() );
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Raw Occupancy Ratio [Data]", 600, 400 );
  
  rOccHisto->SetLineWidth( 2 );
  rOccHisto->SetLineColor( 2 );
  
  rOccHisto->GetXaxis()->SetTitle( "Raw Occupancy Ratio" );
  rOccHisto->GetYaxis()->SetTitle( "No. PMTs / 0.02" );
  rOccHisto->GetXaxis()->SetTitleOffset( 1.2 );
  rOccHisto->GetYaxis()->SetTitleOffset( 1.2 );
  
  rOccHisto->Draw();
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Plot the distances of the light path lengths in the inner AV region.
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
TCanvas* PlotDistancesInInnerAV( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* innerAVHisto = new TH1F( "Distance in Inner AV", 
                                 "Distance in Inner AV Histogram", 
                                 140, 0.0, 14000.0 );
  
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    if( iDP->GetDistInInnerAV() >= 0.0 ){
      innerAVHisto->Fill( iDP->GetDistInInnerAV() );
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Distance in Inner AV", 600, 400 );
  
  innerAVHisto->SetLineWidth( 2 );
  innerAVHisto->SetLineColor( 2 );
  
  innerAVHisto->GetXaxis()->SetTitle( "Distance in Inner AV [mm]" );
  innerAVHisto->GetYaxis()->SetTitle( "No. PMTs / 100 mm" );
  innerAVHisto->GetXaxis()->SetTitleOffset( 1.2 );
  innerAVHisto->GetYaxis()->SetTitleOffset( 1.2 );
  
  innerAVHisto->Draw();
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Plot the distances of the light path lengths in the outer AV region.
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
TCanvas* PlotDistancesInOuterAV( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* outerAVHisto = new TH1F( "Distance in Outer AV", 
                                 "Distance in Outer AV Histogram", 
                                 40, 0.0, 4000.0 );
  
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    if( iDP->GetDistInWater() >= 0.0 ){
      outerAVHisto->Fill( iDP->GetDistInWater() );
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Distance in Inner AV", 600, 400 );
  
  outerAVHisto->SetLineWidth( 2 );
  outerAVHisto->SetLineColor( 2 );
  
  outerAVHisto->GetXaxis()->SetTitle( "Distance in Outer AV [mm]" );
  outerAVHisto->GetYaxis()->SetTitle( "No. PMTs / 100 mm" );
  outerAVHisto->GetXaxis()->SetTitleOffset( 1.2 );
  outerAVHisto->GetYaxis()->SetTitleOffset( 1.2 );
  
  outerAVHisto->Draw();
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Plot the distances of the light path lengths in the acrylic of the AV region.
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
TCanvas* PlotDistancesInAV( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* avHisto = new TH1F( "Distance in AV", 
                                 "Distance in AV Histogram", 
                                 100, 0.0, 100.0 );
  
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    if( iDP->GetDistInAV() >= 0.0 ){
      avHisto->Fill( iDP->GetDistInAV() );
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Distance in AV", 600, 400 );
  
  avHisto->SetLineWidth( 2 );
  avHisto->SetLineColor( 2 );
  
  avHisto->GetXaxis()->SetTitle( "Distance in AV [mm]" );
  avHisto->GetYaxis()->SetTitle( "No. PMTs / mm" );
  avHisto->GetXaxis()->SetTitleOffset( 1.2 );
  avHisto->GetYaxis()->SetTitleOffset( 1.2 );
  
  avHisto->Draw();
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// Plot the incident angles at the PMTs.
// 'storeName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/datastore directory.
TCanvas* PlotIncidentAngles( std::string storeName )
{
  
  OCAPMTStore* pmtStore = GetPMTStorePtr( storeName );
  
  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = pmtStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = pmtStore->GetOCAPMTsIterEnd();
  
  TH1F* angleHisto = new TH1F( "Incident Angle at PMTs", 
                               "Incident Angle at PMTs Histogram", 
                               91, -0.5, 90.5 );
  
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    if( TMath::ACos( iDP->GetCosTheta() ) * TMath::RadToDeg() >= 0.0 ){
      angleHisto->Fill( TMath::ACos( iDP->GetCosTheta() ) * TMath::RadToDeg() );
    }
  }
  
  TCanvas* c1 = new TCanvas( "c1", "Incident Angle at PMTs", 600, 400 );
  
  angleHisto->SetLineWidth( 2 );
  angleHisto->SetLineColor( 2 );
  
  angleHisto->GetXaxis()->SetTitle( "Incident Angle at PMT [degrees]" );
  angleHisto->GetYaxis()->SetTitle( "No. PMTs / degrees" );
  angleHisto->GetXaxis()->SetTitleOffset( 1.2 );
  angleHisto->GetYaxis()->SetTitleOffset( 1.2 );
  
  angleHisto->Draw();
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
