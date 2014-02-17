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

  cout << "\n";
  cout << "#####################################" << endl;
  cout << "######### LOCAS2FITTER Start ########" << endl;
  cout << "#####################################" << endl;
  cout << "\n";


  LOCASFit lFit;
  lFit.LoadFitFile( argv[1] );
  lFit.DataScreen();
  lFit.PerformFit();

  cout << "\n";
  cout << "####################################" << endl;
  cout << "######### LOCAS2FITTER End #########" << endl;
  cout << "####################################" << endl;
  cout << "\n";

  lFit.Plot1DChiSquareScan( "test_1d_chisquare.eps", 1, 1000, 200000, 1000, 20 );
  lFit.PlotROccVals( "test_rocc_vals.eps" );
  return 0;

}
