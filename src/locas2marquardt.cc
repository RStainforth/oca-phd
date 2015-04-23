///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2marquardt.cc
///
/// EXECUTABLE: locas2mardquardt
///
/// BRIEF: Fit the data using the original Levenberg-Marquardt algorithm 
///        from the SNO code 
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
#include "TStyle.h"
#include "TCanvas.h"

#include <iostream>

using namespace std;
using namespace LOCAS;

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Check that the 'locas2marquardt' executable has been used properly 
  if ( argc != 2 ){
    cout << "locas2minuit: Error, no path specified for the LOCAS fit-file.\n";
    cout << "usage: locas2marquardt /path/to/fit-file.ratdb\n";
  }

  cout << "\n";
  cout << "#######################################" << endl;
  cout << "######## LOCAS2MARQUARDT Start ########" << endl;
  cout << "#######################################" << endl;
  cout << "\n";


  LOCASFit lFit;

  lFit.LoadFitFile( argv[1] );

  lFit.DataScreen( 1000.0 );
  lFit.PerformFit();
  lFit.DataScreen( 100.0 );
  lFit.PerformFit();
  lFit.DataScreen( 25.0 );
  lFit.PerformFit();
  lFit.DataScreen( 16.0 );
  lFit.PerformFit();
  lFit.DataScreen( 16.0 );
  lFit.PerformFit();
  lFit.PrintParameterInformation();

  TCanvas* canVas = new TCanvas( "LB Distribution Plot", "LB Distribution Plot", 640, 400 ); 
  ( lFit.ApplyLBDistribution() )->Draw( "colz" );
  gStyle->SetOptStat( 0 );
  canVas->Print("lbdist.eps");

  TCanvas* canVas1 = new TCanvas( "LB Distribution Mask Function", "LB Distribution Mask Function", 640, 400 ); 
  ( lFit.LBDistributionMaskTF1() )->Draw();
  gStyle->SetOptStat( 0 );
  canVas1->Print("lbwave.eps");

  TCanvas* canVas2 = new TCanvas( "Angular Response Distribution", "Angular Response Distribution", 640, 400 ); 
  ( lFit.AngularResponseTH1F() )->Draw();
  gStyle->SetOptStat( 0 );
  canVas2->Print("angresp.eps");

  TCanvas* canVas3 = new TCanvas( "Data ROcc vs. Model ROcc", "Data ROcc vs. Model ROcc", 640, 400 ); 
  ( lFit.PlotModelROccVsDataROcc() )->Draw("AP");
  gStyle->SetOptStat( 0 );
  canVas3->Print("modelroccvsdatarocc.eps");

  lFit.WriteFitToFile("SLPFIT2_Fit.root");

  cout << "\n";
  cout << "#######################################" << endl;
  cout << "######### LOCAS2MARQUARDT End #########" << endl;
  cout << "#######################################" << endl;
  cout << "\n";

  return 0;

}
