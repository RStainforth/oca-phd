////////////////////////////////////////////////////////////////////
///
/// FILENAME: PlotModelParameterInformation.cc
///
/// CLASS:  N/A
///
/// BRIEF:  The main purpose of this macro is to demonstrate to 
///         the user how to extract some of the model parameters from the
///         parameter files created following the optical fit.
///
///         In this example ROOT, the parameters are retrieved
///         from information held on the OCA model parameter 
///         store files found in:
///         $OCA_SNOPLUS_ROOT/output/fits
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
///         .L PlotModelParameterInformation.cc+
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
#include "TGraphErrors.h"

#include "OCAPMTStore.hh"
#include "OCAModelParameterStore.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include <string>
#include <vector>

using namespace std;
using namespace OCA;

// Declare the functions in this macro
void CalculatePMTAngularResponse( TFile* dataFile,
                                  std::string dataSet,
                                  vector< Float_t >& pmtRespVal,
                                  vector< Float_t >& pmtRespErrStat,
                                  vector< Float_t >& pmtRespErrSys );
TCanvas* PlotPMTAngularResponse( std::string storeName );
void OutputAttenuationCoefficients( std::string fitName );

// Plot the PMT angular response. The plot will include both the
// statistical and systematic errors.
// 'fitName' - This should be the name of the .root file 
// in the $OCA_SNOPLUS_ROOT/output/fits directory but WITHOUT the '.root'
// suffix on the filename.
TCanvas* PlotPMTAngularResponse( std::string fitName )
{

  // First retrieve the full file path of the file.
  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fitName;
  cout << "Opening File: " << filePath + ".root" << endl;

  // Open the .root file.
  TFile* tmpFile = TFile::Open( ( filePath + ".root" ).c_str() );

  // Create a TGraph errors objects for the statistical and
  // systematic errors individually.
  TGraphErrors* pmtRespValuesStat = new TGraphErrors();
  TGraphErrors* pmtRespValuesSys = new TGraphErrors();
  
  // Calculate the PMT angular response
  // for each systematic...
  vector< Float_t > parVal;
  vector< Float_t > parErrStat;
  vector< Float_t > parErrSys;
  CalculatePMTAngularResponse( tmpFile, fitName, parVal, parErrStat, parErrSys );

  // ...and now plot the nominal response with the statistical
  // and systematic errors.
  for ( Int_t iAng = 0; iAng < (Int_t)parVal.size(); iAng++ ){
    // Plot the PMT response...
    pmtRespValuesStat->SetPoint( iAng, (Float_t)iAng + 0.5, parVal[ iAng ] );
    pmtRespValuesSys->SetPoint( iAng, (Float_t)iAng + 0.5, parVal[ iAng ] );
    
    // ...plot the errors on the PMT response.
    pmtRespValuesStat->SetPointError( iAng, 0.5, parErrStat[ iAng ] );
    pmtRespValuesSys->SetPointError( iAng, 0.5, parErrSys[ iAng ] * parVal[ iAng ] );
  }
  
  // Create a canvas to plot the PMT response
  TCanvas* c1 = new TCanvas( "c1", "Raw Occupancy Ratio [Data]", 600, 400 );

  // Configure the plots and draw them
  pmtRespValuesSys->SetLineWidth( 1 );
  pmtRespValuesSys->SetLineStyle( 1 );
  pmtRespValuesSys->SetLineColor( 2 );
  pmtRespValuesSys->SetMarkerColor( 2 );
  pmtRespValuesSys->SetMarkerStyle( 1 );
  
  pmtRespValuesStat->SetLineWidth( 1 );
  pmtRespValuesStat->SetLineStyle( 1 );
  pmtRespValuesStat->SetLineColor( 2 );
  pmtRespValuesStat->SetMarkerColor( 2 );
  pmtRespValuesStat->SetMarkerStyle( 1  );

  pmtRespValuesSys->SetMaximum( 1.15 );
  pmtRespValuesSys->SetMinimum( 0.99 );
  pmtRespValuesSys->Draw("AP");
  pmtRespValuesSys->SetTitle( "Relative PMT Angular Response" );
  pmtRespValuesSys->GetXaxis()->SetTitle( "Incident Angle at PMT [degrees]" );
  pmtRespValuesSys->GetYaxis()->SetTitle( "Relative PMT Angular Response [a.u.]" );
  pmtRespValuesStat->Draw("P, same");
 
  return c1;
  
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void CalculatePMTAngularResponse( TFile* dataFile,
                                  std::string dataSet,
                                  vector< Float_t >& pmtRespVal,
                                  vector< Float_t >& pmtRespErrStat,
                                  vector< Float_t >& pmtRespErrSys )
{

  // The list of different branches on the parameter file. 
  // Each branch (aside from 'nominal') is a systematic instance
  // of the parameter fit.
  vector< string > branches;
  branches.push_back( "nominal" ); 
  branches.push_back( "laserball_r_scale" );
  branches.push_back( "laserball_r_shift");
  branches.push_back( "laserball_minus_x_shift");
  branches.push_back( "laserball_plus_x_shift");
  branches.push_back( "laserball_minus_y_shift");
  branches.push_back( "laserball_plus_y_shift");
  branches.push_back( "laserball_minus_z_shift");
  branches.push_back( "laserball_plus_z_shift");
  branches.push_back( "lambda_minus_shift");
  branches.push_back( "lambda_plus_shift");
  branches.push_back( "laserball_distribution2");
  branches.push_back( "laserball_distribution_flat");
  branches.push_back( "chi_square_lim_16");
  branches.push_back( "pmt_to_pmt_variability" );
  branches.push_back( "distance_to_pmt" );
  
  // The scaling factors for the systematic variations
  // from each of the respective branches above.
  vector< Float_t > scales;
  scales.push_back( 1.0 ); 
  scales.push_back( 0.2 );
  scales.push_back( 0.40 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 1.0 );
  scales.push_back( 1.0 );
  scales.push_back( 0.05 );
  scales.push_back( 0.05 );
  scales.push_back( 1.0 );
  scales.push_back( 1.0 );
  scales.push_back( 1.0 );

  // A vector to store all the nominal parameter values for
  // the PMT angular response.
  vector< Float_t > nominalVals;
  OCA::OCAModelParameterStore* tmpStore = NULL;
  TTree* tmpTree = NULL;

  // Now loop over each of the branches and calculate the PMT angular repsonse
  for ( Int_t iSys = 0; iSys < (Int_t)branches.size(); iSys++ ){

    // Retrieves the tree based on the name of the systematic currently
    // being used.
    string treeName = ( dataSet + "-" + branches[ iSys ] + ";1" );
    tmpTree = (TTree*)dataFile->Get( treeName.c_str() );
    string valStr = "tmp";
    tmpStore = new OCA::OCAModelParameterStore( valStr );

    // Set the address of the tree to the model parameter store
    // in order to retrieve the parameters
    tmpTree->SetBranchAddress( branches[ iSys ].c_str(), &(tmpStore) );
    tmpTree->GetEntry( 0 );

    // Retrieve the covariance matrix and the parameter values
    TMatrix covMatrix = tmpStore->GetCovarianceMatrixValues();
    TVector parVals = tmpStore->GetParameterValues();

    // In the case of the nominal branch we need to store these
    // so we can compute the relative systematic variations
    if ( branches[ iSys ] == "nominal" ){
      Int_t pmtRespIndex = tmpStore->GetPMTAngularResponseParIndex();
      for ( Int_t iAng = 0; iAng < tmpStore->GetNPMTAngularResponseBins(); iAng++ ){

        // The current index of the iAng-th angular response parameter.
        Int_t iIndex = pmtRespIndex + iAng;
        nominalVals.push_back( parVals[ iIndex ] );
        pmtRespVal.push_back( parVals[ iIndex ] );
        pmtRespErrSys.push_back( 0.0 );
        pmtRespErrStat.push_back( 0.0 );

        Float_t covValue = covMatrix[ iIndex ][ iIndex ];
        if ( covValue > 0.0  ){
          pmtRespErrStat[ iAng ] = TMath::Sqrt( covValue );
        }
        else{
          pmtRespErrStat[ iAng ] =  0.0;
        }
      }
    }

    Int_t pmtRespIndex = tmpStore->GetPMTAngularResponseParIndex();
    for ( Int_t iAng = 0; iAng < tmpStore->GetNPMTAngularResponseBins(); iAng++ ){
      Int_t iIndex = pmtRespIndex + iAng;
      
      // Calculate the fractional difference between the current
      // parameter values and the nominal values in order to determine
      // the systematic variation.
      Float_t curParVal = parVals[ iIndex ];
      Float_t deltaPar = 0.0;
      if ( nominalVals[ iAng ] > 0.0 ){
        deltaPar = ( curParVal - nominalVals[ iAng ] ) / nominalVals[ iAng ];
      }
      else{
        deltaPar = 0.0;
      }

      // Scale the systematic variation by the respective scaling
      // factor.
      Float_t sysVal = scales[ iSys ] * deltaPar;
      
      // Add the systematic error in quadrature.
      pmtRespErrSys[ iAng ] += ( sysVal * sysVal );
      
    }
    tmpTree = NULL;
    delete tmpStore;
    tmpStore = NULL;
    
  }

  // Calculate the systematic error.
  for ( Int_t iAng = 0; iAng < (Int_t)pmtRespVal.size(); iAng++ ){
    if ( pmtRespErrSys[ iAng ] > 0.0 ){
      pmtRespErrSys[ iAng ] = TMath::Sqrt( pmtRespErrSys[ iAng ] );
    }
    else{
      pmtRespErrSys[ iAng ] = 0.0;
    }
  }
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void OutputAttenuationCoefficients( std::string fitName )
{

  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fitName;
  cout << "Opening File: " << filePath + ".root" << endl;

  // Open up the .root file.
  TFile* dataFile = TFile::Open( ( filePath + ".root" ).c_str() );

  // The scaling factors for the systematic variations
  // from each of the respective branches above.
  vector< string > branches;
  branches.push_back( "nominal" ); 
  branches.push_back( "laserball_r_scale" );
  branches.push_back( "laserball_r_shift");
  branches.push_back( "laserball_minus_x_shift");
  branches.push_back( "laserball_plus_x_shift");
  branches.push_back( "laserball_minus_y_shift");
  branches.push_back( "laserball_plus_y_shift");
  branches.push_back( "laserball_minus_z_shift");
  branches.push_back( "laserball_plus_z_shift");
  branches.push_back( "lambda_minus_shift");
  branches.push_back( "lambda_plus_shift");
  branches.push_back( "laserball_distribution2");
  branches.push_back( "laserball_distribution_flat");
  branches.push_back( "chi_square_lim_16");
  branches.push_back( "pmt_to_pmt_variability" );
  branches.push_back( "distance_to_pmt" );
  
  // The scaling factors for the systematic variations
  // from each of the respective branches above.
  vector< Float_t > scales;
  scales.push_back( 1.0 ); 
  scales.push_back( 0.2 );
  scales.push_back( 0.40 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 0.2 );
  scales.push_back( 1.0 );
  scales.push_back( 1.0 );
  scales.push_back( 0.05 );
  scales.push_back( 0.05 );
  scales.push_back( 1.0 );
  scales.push_back( 1.0 );
  scales.push_back( 1.0 );

  vector< Float_t > nominalVals;
  OCA::OCAModelParameterStore* tmpStore = NULL;
  TTree* tmpTree = NULL;

  Float_t statError = 0.0;
  Float_t sysError = 0.0;
  Float_t nominalVal = 0.0;

  for ( Int_t iSys = 0; iSys < (Int_t)branches.size(); iSys++ ){

    string treeName = ( fitName + "-" + branches[ iSys ] + ";1" );
    tmpTree = (TTree*)dataFile->Get( treeName.c_str() );
    string valStr = "tmp";
    tmpStore = new OCA::OCAModelParameterStore( valStr );
    tmpTree->SetBranchAddress( branches[ iSys ].c_str(), &(tmpStore) );
    tmpTree->GetEntry( 0 );

    // Retrieve the covariance matrix and the parameter values
    TMatrix covMatrix = tmpStore->GetCovarianceMatrixValues();
    TVector parVals = tmpStore->GetParameterValues();

    Int_t attnIndex = tmpStore->GetInnerAVAttenuationCoefficientParIndex();

    if ( branches[ iSys ] == "nominal" ){
      nominalVal = parVals[ attnIndex ];
      
      Float_t covValue = covMatrix[ attnIndex ][ attnIndex ];
      if ( covValue > 0.0  ){
        statError = TMath::Sqrt( covValue );
      }
    }
    
    Float_t curParVal = parVals[ attnIndex ];
    Float_t deltaPar = 0.0;
    if ( nominalVal > 0.0 ){
      deltaPar = ( curParVal - nominalVal ) / nominalVal;
    }
    else{
      deltaPar = 0.0;
    }
    Float_t sysVal = scales[ iSys ] * deltaPar;
    
    sysError += ( sysVal * sysVal );

    tmpTree = NULL;
    delete tmpStore;
    tmpStore = NULL;
    
  }

  sysError = TMath::Sqrt( sysError );

  sysError *= nominalVal;

  cout << "#######################" << endl;
  cout << "Inner AV Attenuation Coeffiecient = " 
       << nominalVal 
       << " +/- " << statError << " (stat.)" 
       << " +/- " << sysError << " (sys.) mm-1" << endl;
  cout << "#######################" << endl;

  return;

}
