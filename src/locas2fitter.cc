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

#include "LOCASFit.hh"

#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TMath.h"

#include <iostream>

using namespace std;
using namespace LOCAS;

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){


  LOCASFit lFit;
  lFit.LoadFitFile( argv[1] );
  lFit.DataScreen();
  lFit.PerformFit();

  cout << "Finished Executable" << endl;

  cout << "Finished Executable" << endl;

  cout << "Finished Executable" << endl;
  return 0;
  //lFit.WriteToFile( "test.root" );

  // Float_t chiSq = 0.0;

  // TGraph* chiSquarePlot = new TGraph();
  // Int_t pointVal = 0;
  
  // for ( Float_t par = 0.000001; par <= 0.001; par += 0.000001 ){
  //   lFit.SetPar( 1, par );
  //   chiSq = 0.0;
  //   cout << (Double_t)( ( par ) / ( 0.00000001 ) )*100 << "% Complete" << endl;
    
  //   for ( Int_t iPMT = 1; iPMT <= lFit.GetNPMTsInFit(); iPMT++ ){    
  //     Int_t iX = lFit.GetMrqX( iPMT );
  //     Int_t pmtID = iX % 10000;
  //     Int_t iRun = iX / 10000;
  //     Float_t chiSqPMT = lFit.CalculatePMTChiSquare( iRun, pmtID );
  //     chiSq += chiSqPMT;       
  //   }
    
  //   cout << "Reduced ChiSquare is: (" << chiSq << ") / (" << lFit.GetNPMTsInFit() << "-1) = " << ( chiSq / ( lFit.GetNPMTsInFit() - 1 ) ) << endl; 
  //   chiSquarePlot->SetPoint( pointVal++, par, chiSq );
  // }

  
  
  // TCanvas* c1 = new TCanvas( "ChiSquare-Scan", "ChiSquare Scan", 600, 400 );
  
  // chiSquarePlot->GetXaxis()->SetTitle( "Scintillator (1/#alpha_scint)" );
  // chiSquarePlot->GetYaxis()->SetTitle( "ChiSquare" );
  
  // chiSquarePlot->SetMarkerColor( 2 );
  // chiSquarePlot->SetMarkerStyle( 6 );
  // chiSquarePlot->SetMarkerSize( 2 );
  // chiSquarePlot->SetLineWidth( 2 );
  // chiSquarePlot->SetLineColor( 1 );
  
  // chiSquarePlot->Draw( "ALP" );
  
  // c1->Print("test_plot.eps");

}
