////////////////////////////////////////////////////////////////////
///
/// FILENAME: runDiagScan.cxx
///
/// BRIEF: auxiliary macro to run DiagScan.cxx and plot the
///        attenuation coefficients in function of
///        wavelength.
///
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     07/2016 : A.S.Inacio - First Version.
///
/// DETAIL: To execute this file, first it is necessary to compile.
///         At ROOT command line:
///
///            .L DiagScan.cxx+
///
///         Then:
///
///            .L runDiagScan.cxx+
///
///         And finally:
///
///            runDiagonalScanAnalysis( <scan>, <diagonal> )
///
////////////////////////////////////////////////////////////////////

#include "DiagScan.h"

#include <stdio.h>

using namespace std;

void runDiagonalScanAnalysis( const std::string& scan = "oct15", const std::string& diagonal = "xpz" ){

  Int_t nLambda = 0;
  std::vector< Int_t > list_wl;

  if( scan == "oct15" ){
    nLambda = 5;
    list_wl.push_back(337);
    list_wl.push_back(339);
    list_wl.push_back(385);
    list_wl.push_back(420);
    list_wl.push_back(505);
  }
  if( scan == "dec17" ){
    nLambda = 6;
    list_wl.push_back(337);
    list_wl.push_back(335);
    list_wl.push_back(385);
    list_wl.push_back(420);
    list_wl.push_back(450);
    list_wl.push_back(500);
  }

  Int_t lLambda[nLambda];
  for( Int_t i = 0; i < nLambda; i++ ){ lLambda[i] = list_wl[i]; }

  Double_t attcoef[nLambda];
  Double_t attcoefEr[nLambda];
  Double_t att_len[nLambda];
  Double_t att_len_err[nLambda];

  // Loop over all wavelengths
  for(Int_t k = 0; k < nLambda; k++){

    DiagScan qd;

    qd.Initialize();
    qd.SetLambda( lLambda[k] );
    qd.SetDiagonal( diagonal );
    qd.SetScan( scan );

    //   qd.SetDistanceCut( value );
    //   qd.SetShadowingTolerance( value );
    //   qd.SetNSigmas( value );
    //   qd.SetPath( path );

    qd.ReadData();
    qd.Process();
    qd.FitRatio();
    qd.Product();

    attcoef[k] = -qd.GetAttCoef();
    attcoefEr[k] = -qd.GetAttCoefErr();
    att_len[k] = 1./attcoef[k];
    att_len_err[k] = att_len[k] * attcoefEr[k];

  }

  // Print the results
  for (int l = 0; l<6; l++){cout << "Att. Length = " << att_len[l] << " +/-" << att_len_err[l] << endl;}
  cout << "\n" <<  endl;
  for (int l = 0; l<6; l++){cout << "Att. Coefficient = " << attcoef[l] << " +/-" << attcoefEr[l] << endl;}

}
