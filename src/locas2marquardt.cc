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

  cout << "\n";
  cout << "#######################################" << endl;
  cout << "######## LOCAS2MARQUARDT Start ########" << endl;
  cout << "#######################################" << endl;
  cout << "\n";


  LOCASFit lFit;
  lFit.LoadFitFile( argv[1] );
  lFit.DataScreen( 100.0 );
  lFit.PerformFit();
  lFit.DataScreen( 50.0 );
  lFit.PerformFit();
  lFit.DataScreen( 25.0 );
  lFit.PerformFit();
  lFit.DataScreen( 16.0 );
  lFit.PerformFit();
  lFit.DataScreen( 16.0 );
  lFit.PerformFit();

  
  lFit.WriteFitToFile("example.root");

  TCanvas* canVas = new TCanvas( "LB Distribution Plot", "LB Distribution Plot", 640, 400 ); 
  ( lFit.ApplyLBDistribution() )->Draw( "E" );
  canVas->Print("lbdist.eps");

  TCanvas* canVas1 = new TCanvas( "LB Distribution Mask Function", "LB Distribution Mask Function", 640, 400 ); 
  ( lFit.LBDistributionMaskTF1() )->Draw();
  canVas1->Print("lbwave.eps");

  TCanvas* canVas2 = new TCanvas( "Angular Response Distribution", "Angular Response Distribution", 640, 400 ); 
  ( lFit.AngularResponseTH1F() )->Draw();
  gStyle->SetOptStat( 0 );
  canVas2->Print("angresp.eps");

  cout << "\n";
  cout << "#######################################" << endl;
  cout << "######### LOCAS2MARQUARDT End #########" << endl;
  cout << "#######################################" << endl;
  cout << "\n";

  return 0;

}
