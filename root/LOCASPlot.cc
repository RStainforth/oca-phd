#include "RAT/DS/Root.hh"
#include "RAT/DS/EV.hh"
#include "RAT/DS/Run.hh"
#include "RAT/DS/LightPath.hh"

#include "TObject.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TLegend.h"

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
LOCASDavVDsc( const char* root_file, 
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
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );
	  lSLP.CalculatePath( startPos, pmtPos );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInScint()/10.0, lRLP.GetDistInAV()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInScint()/10.0, lSLP.GetDistInAV()/10.0 );
	  }
	  lRLP.Clear();
	  lSLP.Clear();

	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-LOCAS-DavVDsc", "LOCAS: AV vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "LOCAS: AV vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.0 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );
  c1->Print(plot_file_name);

  return;

}

void
DavVDsc( const char* root_file, 
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
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){

	  RAT::DS::LightPath* lRLPath = rRun->GetLightPath();
	  RAT::DS::LightPath* lSLPath = rRun->GetLightPath();

	  Double_t distInScint, distInAV, distInWater;
	  Double_t distInScint1, distInAV1, distInWater1;
	  TVector3 hypPMTPos;
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLPath->CalcByPosition( startPos, pmtPos, hypPMTPos, distInScint, distInAV, distInWater, 400.0, 10.0 );
	  lSLPath->CalcByPosition( startPos, pmtPos, distInScint1, distInAV1, distInWater1 );

	  if ( !lRLPath->GetTIR() && !lRLPath->GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, distInScint/10.0, distInAV/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, distInScint1/10.0, distInAV1/10.0 );
	  }
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-DavVDsc", "RAT: AV vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "RAT AV vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );

  c1->Print(plot_file_name);

  return;

}

void
LOCASDh2oVDsc( const char* root_file, 
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
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );
	  lSLP.CalculatePath( startPos, pmtPos );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInScint()/10.0, lRLP.GetDistInWater()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInScint()/10.0, lSLP.GetDistInWater()/10.0 );
	  }
	  lRLP.Clear();
	  lSLP.Clear();

	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-LOCASDh2oVDsc", "LOCAS: Water vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "LOCAS: Water vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "Water Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );
  c1->Print(plot_file_name);

  return;

}

void
Dh2oVDsc( const char* root_file, 
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
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){

	  RAT::DS::LightPath* lRLPath = rRun->GetLightPath();
	  RAT::DS::LightPath* lSLPath = rRun->GetLightPath();

	  Double_t distInScint, distInAV, distInWater;
	  Double_t distInScint1, distInAV1, distInWater1;
	  TVector3 hypPMTPos;
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLPath->CalcByPosition( startPos, pmtPos, hypPMTPos, distInScint, distInAV, distInWater, 400.0, 10.0 );
	  lSLPath->CalcByPosition( startPos, pmtPos, distInScint1, distInAV1, distInWater1 );

	  if ( !lRLPath->GetTIR() && !lRLPath->GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, distInScint/10.0, distInWater/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, distInScint1/10.0, distInWater1/10.0 );
	  }
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-Dh2oVDsc", "RAT: Water vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "RAT Water vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "Water Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );

  c1->Print(plot_file_name);

  return;

}

void
LOCASDavVDh2o( const char* root_file, 
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
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 400.0, 10.0 );
	  lSLP.CalculatePath( startPos, pmtPos );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInWater()/10.0, lRLP.GetDistInAV()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInWater()/10.0, lSLP.GetDistInAV()/10.0 );
	  }
	  lRLP.Clear();
	  lSLP.Clear();

	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-LOCASDavVDh2o", "LOCAS: AV vs. Water Distance", 640, 400 );

  //RLPGraph->SetTitle( "LOCAS: AV vs. Water Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Water Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.60, 0.30, 0.80, 0.50 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );
  c1->Print(plot_file_name);

  return;

}

void
DavVDh2o( const char* root_file, 
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
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){

	  RAT::DS::LightPath* lRLPath = rRun->GetLightPath();
	  RAT::DS::LightPath* lSLPath = rRun->GetLightPath();

	  Double_t distInScint, distInAV, distInWater;
	  Double_t distInScint1, distInAV1, distInWater1;
	  TVector3 hypPMTPos;
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLPath->CalcByPosition( startPos, pmtPos, hypPMTPos, distInScint, distInAV, distInWater, 400.0, 10.0 );
	  lSLPath->CalcByPosition( startPos, pmtPos, distInScint1, distInAV1, distInWater1 );

	  if ( !lRLPath->GetTIR() && !lRLPath->GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, distInWater/10.0, distInAV/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, distInWater1/10.0, distInAV1/10.0 );
	  }
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-DavVDh2o", "RAT: AV vs. Water Distance", 640, 400 );

  //RLPGraph->SetTitle( "RAT: AV vs. Water Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Water Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.60, 0.30, 0.80, 0.50 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );

  c1->Print(plot_file_name);

  return;

}

void
PlotAll( const char* root_file )
{

  LOCASDavVDh2o( root_file, "locas_h2o_vs_av.eps", 0.0, 6005.3 );
  LOCASDavVDsc( root_file, "locas_av_vs_sc.eps", 0.0, 6005.3 );
  LOCASDh2oVDsc( root_file, "locas_h2o_vs_sc.eps", 0.0, 6005.3 );
  DavVDh2o( root_file, "rat_h2o_vs_av.eps", 0.0, 6005.3 );
  DavVDsc( root_file, "rat_av_vs_sc.eps", 0.0, 6005.3 );
  Dh2oVDsc( root_file, "rat_h2o_vs_sc.eps", 0.0, 6005.3 );

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
