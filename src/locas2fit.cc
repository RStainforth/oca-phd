///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2fit.cc
///
/// EXECUTABLE: locas2fit
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: 
///
///////////////////////////////////////////////////////////////////////////////////////

#include "RAT/getopt.h"
#include "RAT/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/Log.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASLightPath.hh"
#include "LOCASRunReader.hh"
#include "LOCASFit.hh"

#include "TFile.h"

#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TMath.h"
#include "TMinuit.h"

#include "TROOT.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace LOCAS;

int main( int argc, char** argv );
void EvaluateGlobalChiSquare( Int_t &nPar, Double_t* gIn, Double_t &f, Double_t* params, Int_t iFlag );
Float_t CalcModelROcc( Double_t* params );
void PlotChiSquareScan( Int_t parIndex,
                        Float_t startVal,
                        Float_t endVal,
                        Float_t stepVal );

TGraph* graph = new TGraph();
Int_t point = 0;

LOCASFit lFit;


//////////////////////
//////////////////////

int main( int argc, char** argv ){

  lFit.LoadFitFile( argv[1] );
  lFit.DataScreen();

  PlotChiSquareScan( 1, 1500.0, 10000.0, 500.0 );

  lFit.WriteFitToFile( "test_pmts.root" );

}

void EvaluateGlobalChiSquare( Int_t &nPar, Double_t* gIn, Double_t &f, Double_t* params, Int_t iFlag )
{

  lFit.SetMrqParameter( 1, params[0] );
  f = lFit.CalculateChiSquare();
  
}

void PlotChiSquareScan( Int_t parIndex,
                        Float_t startVal,
                        Float_t endVal,
                        Float_t stepVal )
{

  TGraph* graph1 = new TGraph();
  Int_t plotPoint = 0;

  TCanvas* c1 = new TCanvas( "ChiSquare-Scan", "chisquare-scan", 200, 10, 1280, 800 );

  graph1->GetXaxis()->SetTitle("Parameter Value");
  graph1->GetXaxis()->SetTitleOffset( 1.1 );
  graph1->GetYaxis()->SetTitle("ChiSquare");
  graph1->SetMarkerStyle( 2.0 );

  Float_t modelVal = 0.0;
  for ( Float_t iVal = startVal; iVal <= endVal; iVal += stepVal ){
    cout << "modelVal is: " << modelVal << endl;
    lFit.SetMrqParameter( parIndex, 1.0/iVal );
    modelVal = lFit.CalculateChiSquare();
    graph1->SetPoint( plotPoint++, iVal, modelVal );

  }

  graph1->Draw("AP");
  c1->Print("test_scan_plot.eps");


}
