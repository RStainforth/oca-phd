#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"

#include "OCADataStore.hh"
#include "OCADataPoint.hh"

#include <string>
#include <vector>

using namespace std;
using namespace OCA;

void PlotNPromptCountsRatio( std::string storeName, const char* plotName )
{

  OCADataStore* lStore = new OCADataStore( storeName );

  vector< OCADataPoint >::iterator iDP;
  vector< OCADataPoint >::iterator iDPBegin = lStore->GetOCADataPointsIterBegin();
  vector< OCADataPoint >::iterator iDPEnd = lStore->GetOCADataPointsIterEnd();

  TH1F* rOccHisto = new TH1F( "NPrompt Ratio", "NPrompt Counts Ratio Histogram", 71, -0.02, 1.42 );

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){

    if( iDP->GetNPromptCounts() != 0.0
        && iDP->GetCentralNPromptCounts() != 0.0 ){
    rOccHisto->Fill( iDP->GetNPromptCounts() / iDP->GetCentralNPromptCounts() );
    }
  }

  TCanvas* c1 = new TCanvas( "c1", "NPrompt Counts Ratio [Data]", 600, 400 );

  rOccHisto->SetLineWidth( 2 );
  rOccHisto->SetLineColor( 2 );

  rOccHisto->GetXaxis()->SetTitle( "NPrompt Counts Ratio Histogram [Data]" );
  rOccHisto->GetYaxis()->SetTitle( "Counts / 0.02" );
  rOccHisto->GetXaxis()->SetTitleOffset( 1.2 );
  rOccHisto->GetYaxis()->SetTitleOffset( 1.2 );

  rOccHisto->Draw();
  
  c1->Print( plotName );

}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void PlotOccRatio( std::string storeName, const char* plotName, Float_t nPromptLow, Float_t nPromptHigh )
{

  OCADataStore* lStore = new OCADataStore( storeName );

  vector< OCADataPoint >::iterator iDP;
  vector< OCADataPoint >::iterator iDPBegin = lStore->GetOCADataPointsIterBegin();
  vector< OCADataPoint >::iterator iDPEnd = lStore->GetOCADataPointsIterEnd();

  TH1F* rOccHisto = new TH1F( "Occ Ratio", "Occ Ratio Histogram", 101, -0.02, 2.0 );

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){

    if( iDP->GetNPromptCounts() >= nPromptLow
        && iDP->GetNPromptCounts() <= nPromptHigh ){
      
      Float_t offAxis = iDP->GetMPECorrOccupancy();
      Float_t central = iDP->GetCentralMPECorrOccupancy();

      Float_t occVar = 1.0;
      occVar *= ( iDP->GetCentralSolidAngle() * iDP->GetCentralFresnelTCoeff() );
      occVar /= ( iDP->GetSolidAngle() * iDP->GetFresnelTCoeff() );
      rOccHisto->Fill( ( offAxis / central ) * occVar );
    }
  }

  TCanvas* c1 = new TCanvas( "c1", "Occupancy Ratio [Data]", 600, 400 );

  rOccHisto->SetLineWidth( 2 );
  rOccHisto->SetLineColor( 2 );

  rOccHisto->GetXaxis()->SetTitle( "Occupancy Ratio Histogram [Data]" );
  rOccHisto->GetYaxis()->SetTitle( "Counts / 0.02" );
  rOccHisto->GetXaxis()->SetTitleOffset( 1.2 );
  rOccHisto->GetYaxis()->SetTitleOffset( 1.2 );

  rOccHisto->Draw();
  
  c1->Print( plotName );

}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void PlotNPromptCounts( std::string storeName, const char* plotName = "nPrompt_counts_histo.eps", 
                        Float_t nPromptLow = 0.0, Float_t nPromptHigh = 8000.0,
                        Float_t histoXLow = 0.0, Float_t histoXHigh = 8000.0, Int_t nBins = 160 );

void PlotNPromptCounts( std::string storeName, const char* plotName, 
                        Float_t nPromptLow, Float_t nPromptHigh,
                        Float_t histoXLow, Float_t histoXHigh, Int_t nBins )
{

  OCADataStore* lStore = new OCADataStore( storeName );

  vector< OCADataPoint >::iterator iDP;
  vector< OCADataPoint >::iterator iDPBegin = lStore->GetOCADataPointsIterBegin();
  vector< OCADataPoint >::iterator iDPEnd = lStore->GetOCADataPointsIterEnd();

  TH1F* rOccHisto = new TH1F( "NPrompt Counts", "NPrompt Counts Histogram", nBins, histoXLow, histoXHigh );

  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    if( iDP->GetNPromptCounts() >= nPromptLow
        && iDP->GetNPromptCounts() <= nPromptHigh ){
      rOccHisto->Fill( iDP->GetNPromptCounts() );
    }
  }

  TCanvas* c1 = new TCanvas( "c1", "NPrompt Counts [Data]", 600, 400 );

  rOccHisto->SetLineWidth( 2 );
  rOccHisto->SetLineColor( 2 );

  rOccHisto->GetXaxis()->SetTitle( "NPrompt Counts Histogram [Data]" );
  rOccHisto->GetYaxis()->SetTitle( "Counts" );
  rOccHisto->GetXaxis()->SetTitleOffset( 1.2 );
  rOccHisto->GetYaxis()->SetTitleOffset( 1.2 );

  rOccHisto->Draw();
  
  c1->Print( plotName );

}
