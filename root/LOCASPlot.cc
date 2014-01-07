#include "RAT/DS/Root.hh"
#include "RAT/DS/EV.hh"
#include "RAT/DS/Run.hh"

#include "TObject.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"
#include "TCanvas.h"

#include "LOCASLightPath.hh"


// Function which loads the ROOT file
void
LoadRootFile( const char* lpFile,
              TTree **tree,
              RAT::DS::Root **rDS,
              RAT::DS::Run **rRun);

////////////////////////
// FUNCTION NAMING KEY//
////////////////////////

// Dsc: Distance of Light Path in Scintillator Region
// Dav: Distance of Light Path in Acrylic Vessel
// Dh2o: Distance of Light Path in Water Region 
// V: Versus
// T: PMT Cal Time
// TR: Time Residuals


void
LOCASDscVDav( const char* root_file, 
	      const char* plot_file_name,
	      const Double_t innerR,
	      const Double_t outerR )
{

  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );

  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  LOCAS::LOCASLightPath lRLP;
  LOCAS::LOCASLightPath lSLP;

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

      // Ensure start position is less than "innerR" and 
      // greater than "outerR"
	if( startPos.Mag() < outerR && startPos.Mag() > innerR ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );
	  lSLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() && !lRLP.GetXAVNeck() ){

	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInScint()/10.0, lRLP.GetDistInAV()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInScint()/10.0, lSLP.GetDistInAV()/10.0 );

	  }

	  lRLP.Clear();
	  lSLP.Clear();
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c1", "Scintillator vs. AV Distance", 640, 400 );

  RLPGraph->SetTitle( "Scintillator vs. AV Distance" );
  RLPGraph->GetXaxis()->SetTitle( "AV Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "Water Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitleOffset( 1.15 );

  RLPGraph->SetMarkerColor( 1 );
  SLPGraph->SetMarkerColor( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  c1->Print(plot_file_name);	

}




// Load Root File Definition
void
LoadRootFile( const char* lpFile,
              TTree **tree,
              RAT::DS::Root **rDS,
              RAT::DS::Run **rRun)
{
  TFile *file = new TFile( lpFile );
  (*tree) = (TTree*)file->Get( "T" );
  TTree *runTree = (TTree*)file->Get( "runT" );

  *rDS = new RAT::DS::Root();

  (*tree)->SetBranchAddress( "ds", &(*rDS) );

  *rRun = new RAT::DS::Run();

  runTree->SetBranchAddress( "run", &(*rRun) );
  runTree->GetEntry();
}
