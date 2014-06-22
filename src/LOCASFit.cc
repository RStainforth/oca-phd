///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFit.hh
///
/// CLASS: LOCAS::LOCASFit
///
/// BRIEF: Levenberg-Marquardt fitter for the data (original locas)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"
#include "LOCASFit.hh"

#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

#include <map>

// The 'SWAP' method is used by the LM helper functions
#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#endif

using namespace LOCAS;
using namespace std;

ClassImp( LOCASFit )

// Default Constructor
LOCASFit::LOCASFit()
{

  fFitName = "";
  fFitTitle = "";

  fValidPars = false;
  fDataScreen = false;

  fCurrentRun = NULL;
  fCurrentPMT = NULL;

  fNRuns = -10;

  fVaryAll = false;
  fScintVary = false;
  fAVVary = false;
  fWaterVary = false;
  fScintRSVary = false;
  fAVRSVary = false;
  fWaterRSVary = false;

  fAngularResponseVary = false;
  fLBDistributionVary = false;

  fLBNormalisationVary = false;

  fScintInit = -10.0;
  fAVInit = -10.0;
  fWaterInit = -10.0;
  fScintRSInit = -10.0;
  fAVRSInit = -10.0;
  fWaterRSInit = -10.0;

  fAngularResponseInit = -10.0;
  fLBDistributionInit = -10.0;

  fLBNormalisationInit = -10.0;

  fLBDistribution = NULL;
  fNLBDistributionThetaBins = -10;
  fNLBDistributionPhiBins = -10;
  fNPMTsPerLBDistributionBinMin = 9999;

  fAngularResponse = NULL;
  fNAngularResponseBins = -10;
  fNPMTsPerAngularResponseBinMin = 9999;

  fNParametersInFit = -10;
  fNDataPointsInFit = -10;
  fNPMTsInFit = -10;

  fNPMTSkip = 9999;

  fChiSquare = -10.0;
  fChiSquareMaxLimit = -10.0;
  fChiSquareMinLimit = -10.0;

  fNSigma = -10.0;
  fNChiSquare = -10.0;
  fNOccupancy = 9999.9;

  fAVHDShadowingMin = 10.0;
  fAVHDShadowingMax = -10.0;
  fGeoShadowingMin = 10.0;
  fGeoShadowingMax = -10.0;

  fCHSFlag = false;
  fCSSFlag = false;

  fNElements = -10;
  fChiArray = NULL;
  fResArray = NULL;

  fiAng = -10;
  fCiAng = -10;
  fiLBDist = -10;
  fCiLBDist = -10;
  fiNorm = -10;

  fParamBase = -10;
  fParam = -10;
  fAngIndex = NULL;
  fParamIndex = NULL;
  fParamVarMap = NULL;

  fMrqX = NULL;
  fMrqY = NULL;
  fMrqSigma = NULL;

  fMrqParameters = NULL;
  fMrqVary = NULL;
  fMrqCovariance = NULL;
  fMrqAlpha = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASFit::~LOCASFit()
{

  // Free up all the memory used during the fitting procedure.
  DeAllocate();

}
//////////////////////////////////////
//////////////////////////////////////


void LOCASFit::LoadFitFile( const char* fitFile )
{

  // Load the fit file (.ratdb format) and declare
  // the starting values for the parameters, as well as if they vary
  // in the fit or not and finally the cut criteria for PMTs to be removed
  // from the fit.

  LOCASDB lDB;
  lDB.SetFile( fitFile );

  // Set the fit name and fit title
  fFitName = lDB.GetStringField( "FITFILE", "fit_name" );
  fFitTitle = lDB.GetStringField( "FITFILE", "fit_title" );

  // Get the list of Run IDs and calculate
  // the total number of runs included in the fit
  fListOfRunIDs = lDB.GetIntVectorField( "FITFILE", "run_ids" );
  fNRuns = fListOfRunIDs.size();

  // Add all of the corresponding run files to the 
  // LOCASRunReader object (LOCAS version of TChain)
  for (Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    fRunReader.Add( fListOfRunIDs[ iRun ] );
  }

  // Calculate the total number of PMTs accross all runs
  for (Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    fNDataPointsInFit += ( fRunReader.GetRunEntry( iRun ) )->GetNPMTs();
  }

  // Setup the ChiSquare and Residual Arrays
  fNElements = fNDataPointsInFit;
  fChiArray = new Float_t[ fNElements ];
  fResArray = new Float_t[ fNElements ];

  // The LM working arrays
  fMrqX = flMath.LOCASVector( 1, fNDataPointsInFit );
  fMrqY = flMath.LOCASVector( 1, fNDataPointsInFit );
  fMrqSigma = flMath.LOCASVector( 1, fNDataPointsInFit );

  // Set the values of the LM working arrays to zero.
  for ( Int_t iK = 1; iK <= fNDataPointsInFit; iK++ ){
    fMrqX[ iK ] = 0.0;
    fMrqY[ iK ] = 0.0;
    fMrqSigma[ iK ] = 0.0;
    fChiArray[ iK ] = 0.0;
    fResArray[ iK ] = 0.0;
  }
  

  // Set which of the variables in the fit are to be varied
  // and what the starting, initial values of these parameters are.

  // The (1/attenuation) lengths
  fScintVary = lDB.GetBoolField( "FITFILE", "scint_vary" );
  fScintInit = lDB.GetDoubleField( "FITFILE", "scint_init" );
  fAVVary = lDB.GetBoolField( "FITFILE", "av_vary" );
  fAVInit = lDB.GetDoubleField( "FITFILE", "av_init" );
  fWaterVary = lDB.GetBoolField( "FITFILE", "water_vary" );
  fWaterInit = lDB.GetDoubleField( "FITFILE", "water_init" );

  // The (1/rayleigh-scattering) lengths
  fScintRSVary = lDB.GetBoolField( "FITFILE", "scint_rs_vary" );
  fScintRSInit = lDB.GetDoubleField( "FITFILE", "scint_rs_init" );
  fAVRSVary = lDB.GetBoolField( "FITFILE", "av_rs_vary" );
  fAVRSInit = lDB.GetDoubleField( "FITFILE", "av_rs_init" );
  fWaterRSVary = lDB.GetBoolField( "FITFILE", "water_rs_vary" );
  fWaterRSInit = lDB.GetDoubleField( "FITFILE", "water_rs_init" );

  // The angular response and laserball distributions
  fAngularResponseVary = lDB.GetBoolField( "FITFILE", "ang_resp_vary" );
  fAngularResponseInit = lDB.GetDoubleField( "FITFILE", "ang_resp_init" );

  fLBDistributionVary = lDB.GetBoolField( "FITFILE", "lb_dist_vary" );
  fLBDistributionInit = lDB.GetDoubleField( "FITFILE", "lb_dist_init" );

  // The run (1/normalisation) values
  fLBNormalisationVary = lDB.GetBoolField( "FITFILE", "run_norm_vary" );
  fLBNormalisationInit = lDB.GetDoubleField( "FITFILE", "run_norm_init" );

  // Get the number of theta and phi bins for the laserball distribution 2D historgram
  // and the minimum number of entires required for each bin
  fNLBDistributionThetaBins = lDB.GetIntField( "FITFILE", "lb_dist_n_theta_bins" );
  fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", "lb_dist_n_phi_bins" );
  fNPMTsPerLBDistributionBinMin = lDB.GetIntField( "FITFILE", "lb_dist_min_n_pmts" );

  // Get the number of theta bins for the PMT angular response histogram
  // and the minimum number of entries for each bin
  fNAngularResponseBins = lDB.GetIntField( "FITFILE", "ang_resp_n_bins" );
  fNPMTsPerAngularResponseBinMin = lDB.GetIntField( "FITFILE", "ang_resp_min_n_pmts" );

  // Get the cut variables (i.e. the variables to exclude PMTs from the fit with)
  // each PMT is cut on the below criteria in LOCASFit::PMTSkip and LOCASFit::DataScreen.
  // Where applicable the PMTs are checked against these cuts for both the off-axis AND
  // central runs e.g. the AV Shadowing Values

  // Maximum initial Chi-Square
  fChiSquareMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_chisq_max" );

  // Minimumm initial Chi-Square
  fChiSquareMinLimit = lDB.GetDoubleField( "FITFILE", "cut_chisq_min" );

  // Number of sigma away from mean occupancy for entire run
  fNSigma = lDB.GetDoubleField( "FITFILE", "cut_n_sigma" );

  // Number of occupancy to cut on
  fNOccupancy = lDB.GetIntField( "FITFILE", "cut_n_occupancy" );

  // Minimum AVHD shadowing value
  fAVHDShadowingMin = lDB.GetDoubleField( "FITFILE", "cut_avhd_sh_min" );

  // Maximum AVHD shadowing value
  fAVHDShadowingMax = lDB.GetDoubleField( "FITFILE", "cut_avhd_sh_max" );

  // Minimum Geometric shadowing value
  fGeoShadowingMin = lDB.GetDoubleField( "FITFILE", "cut_geo_sh_min" );

  // Maximum Geometric shadowing value
  fGeoShadowingMax = lDB.GetDoubleField( "FITFILE", "cut_geo_sh_max" );

  // Whether to cut on CHS flag
  fCHSFlag = lDB.GetBoolField( "FITFILE", "cut_chs_flag" );

  // Whether to cut on CSS flag
  fCSSFlag = lDB.GetBoolField( "FITFILE", "cut_css_flag" );

  // The limits on the PMT costheta values
  fCosThetaMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_costheta_max" );
  fCosThetaMinLimit = lDB.GetDoubleField( "FITFILE", "cut_costheta_min" );

  // The limits on the occupancy ratio
  fPMTDataROccMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtrocc_max" );
  fPMTDataROccMinLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtrocc_min" );

  // The limits on the PMT positions (theta and phi limits)
  fPMTPosThetaMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_pmttheta_max" );
  fPMTPosThetaMinLimit = lDB.GetDoubleField( "FITFILE", "cut_pmttheta_min" );
  fPMTPosPhiMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtphi_max" );
  fPMTPosPhiMinLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtphi_min" );


  // Number of PMTs to skip over in the final PMT data set when fitting
  // this is purely to speed things up.
  fNPMTSkip = lDB.GetIntField( "FITFILE", "n_pmts_skip" );

  // Initialise the parameters and print the initialisation info
  InitialiseParameters();
  PrintInitialisationInfo();

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::InitialiseParameters()
{
  
  // Set the initial values (if varied) or the values to use (if not varied)
  // for the model parameters
  AllocateParameters();
  
  // The three attenuation length initial values;
  SetScintPar( fScintInit );
  if ( fScintVary ){ fMrqVary[ GetScintParIndex() ] = 1; }
  else{ fMrqVary[ GetScintParIndex() ] = 0; }

  SetAVPar( fAVInit );
  if ( fAVVary ){ fMrqVary[ GetAVParIndex() ] = 1; }
  else{ fMrqVary[ GetAVParIndex() ] = 0; }

  SetWaterPar( fWaterInit );
  if ( fWaterVary ){ fMrqVary[ GetWaterParIndex() ] = 1; }
  else{ fMrqVary[ GetWaterParIndex() ] = 0; }

  // The three rayleigh scattering length initial values;
  SetScintRSPar( fScintRSInit );
  if ( fScintRSVary ){ fMrqVary[ GetScintRSParIndex() ] = 1; }
  else{ fMrqVary[ GetScintRSParIndex() ] = 0; }

  SetAVRSPar( fAVRSInit );
  if ( fAVRSVary ){ fMrqVary[ GetAVRSParIndex() ] = 1; }
  else{ fMrqVary[ GetAVRSParIndex() ] = 0; }

  SetWaterRSPar( fWaterRSInit );
  if ( fWaterRSVary ){ fMrqVary[ GetWaterRSParIndex() ] = 1; }
  else{ fMrqVary[ GetWaterRSParIndex() ] = 0; }

  // The bin values for the angular response histogram
  Float_t angle = 0.0;
  for ( Int_t iT = 0; iT < fNAngularResponseBins; iT++ ){

    angle = ( (Float_t)( 0.5 + iT ) ) * ( (Float_t)( 90.0 / fNAngularResponseBins ) ); // Centre of each bin...
    if ( angle < 36.0 ){     
      fMrqParameters[ GetAngularResponseParIndex() + iT ] = fAngularResponseInit + ( 0.002222 * angle );
    }   
    else{
      fMrqParameters[ GetAngularResponseParIndex() + iT ] = fAngularResponseInit;
    }

    if( fAngularResponseVary ){ fMrqVary[ GetAngularResponseParIndex() + iT ] = 1; }
    else{ fMrqVary[ GetAngularResponseParIndex() + iT ] = 0; }
  }
  
  // The bin values for the laserball distribution histogram
  for ( Int_t iT = 0; iT < ( fNLBDistributionThetaBins * fNLBDistributionPhiBins ); iT++ ){
    fMrqParameters[ GetLBDistributionParIndex() + iT ] = fLBDistributionInit;
    
    if( fLBDistributionVary ){ fMrqVary[ GetLBDistributionParIndex() + iT ] = 1; }
    else{ fMrqVary[ GetLBDistributionParIndex() + iT ] = 0; }
  }


  // The run normalisations
  Float_t runNorm = 0.0;
  Float_t cenNorm = 0.0;
  Float_t normPar = 0.0;

  for ( Int_t iT = 0; iT < fNRuns; iT++ ){
    fCurrentRun = fRunReader.GetRunEntry( iT );
   
    runNorm = TMath::Power(fCurrentRun->GetLBIntensityNorm(),1.0);
    cout << "runNorm: " << runNorm << endl;
    cenNorm = TMath::Power(fCurrentRun->GetCentralLBIntensityNorm(),1.0);
    cout << "cenNorm: " << cenNorm << endl;

    if ( cenNorm != 0.0 ){ normPar = ( runNorm / cenNorm ); }
    else{ normPar = 1.0; } // Nominal Value

    fMrqParameters[ GetLBNormalisationParIndex() + iT ] = normPar;

    if ( fLBNormalisationVary ){ fMrqVary[ GetLBNormalisationParIndex() + iT ] = 1; }
    else{ fMrqVary[ GetLBNormalisationParIndex() + iT ] = 0; }

    cout << "normPar is: " << normPar << " and iRun is: " << iT << endl;
 
  }

  // NOTE: In this method all the parameters representitive of the angular response
  // histogram and the laserball distribution histogram are set to either vary or not.
  // Later on in DataScreen, each bin is checked for the minimum PMT entries per bin
  // requirement to decide if the value will infact vary or not.

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::AllocateParameters( )
{

  // The total number of parameters in the fit
  fNParametersInFit = 3                                        // Three attenuation lengths
    + 3                                                        // Three Rayleigh scattering lengths
    + fNAngularResponseBins                                    // PMT Angular Response(s) and response bins
    + fNLBDistributionThetaBins * fNLBDistributionPhiBins      // Number of LaserBall Distribution Bins (binned)
    + fNRuns;  
  
  // // The total number of parameters in the fit
  // fNParametersInFit = 3                                        // Three attenuation lengths
  //   + 3                                                        // Three Rayleigh scattering lengths
  //   + 2*fNAngularResponseBins                                  // PMT Angular Response(s) and response bins
  //   + fNLBMaskParameters                                       // Laserball Mask Polynomial ( Function of Theta or Cos(Theta) )
  //   + fNLBDistributionThetaBins * fNLBDistributionPhiBins      // Number of LaserBall Distribution Bins (binned)
  //   + fNLBThetaWaveBins * fNLBDistWave                         // Laserball Distribution ( sine wave )
  //   + fNRuns; 
  // Number of runs ( run normalisations )

  // Initialise memory for the LM working arrays
  fMrqParameters = flMath.LOCASVector( 1, fNParametersInFit );
  fMrqVary = flMath.LOCASIntVector( 1, fNParametersInFit );

  fMrqAlpha = flMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  fMrqCovariance = flMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );

  // Initialise all their values to zero by default
  for ( Int_t i = 1; i <= fNParametersInFit; i++ ){
    fMrqParameters[ i ] = 0.0;
    fMrqVary[ i ] = 0;
    for ( Int_t j = 1; j <= fNParametersInFit; j++ ){
      fMrqAlpha[ i ][ j ] = 0.0;
      fMrqCovariance[ i ][ j ] = 0.0;
    }
  }

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PrintInitialisationInfo( )
{
 
  cout << "LOCASFit Parameters Initialised" << endl;
  cout << "Initial parameter values follow..." << endl;
  cout << " ------------- " << endl;

  cout << "Scintillator (1/attenuation) length: " << fScintInit << endl;
  if ( fScintVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Acrylic (AV) (1/attenuation) length: " << fAVInit << endl;
  if ( fAVVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Water (1/attenuation) length: " << fWaterInit << endl;
  if ( fWaterVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Scintillator (1/rayleigh scattering) length: " << fWaterRSInit << endl;
  if ( fScintRSVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Acrylic (1/rayleigh scattering) length: " << fWaterRSInit << endl;
  if ( fAVRSVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Water (1/rayleigh scattering) length: " << fWaterRSInit << endl;
  if ( fWaterRSVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Angular response histogram (theta, 0 - 90 degrees) consists of: " << fNAngularResponseBins << " bins" << endl;
  cout << "of width " << (Double_t)90.0 / fNAngularResponseBins << " degrees each." << endl;
  if ( fAngularResponseVary ){ cout << "Parameters in the histogram will vary in the fit" << endl; }
  else{ cout << "Parameters in the histogram are fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Laserball distribution 2D histogram (theta 0 - 180 degrees, phi 0 - 360 degrees) consists of: " << fNLBDistributionThetaBins << " x " << fNLBDistributionPhiBins << " bins" << endl;
  cout << "of width " << (Double_t)180.0 / fNLBDistributionThetaBins << " x " << (Double_t)360.0 / fNLBDistributionPhiBins << " degrees each." << endl;
  if ( fLBDistributionVary ){ cout << "Parameters in the histogram will vary in the fit" << endl; }
  else{ cout << "Parameters in the histogram are fixed in the fit" << endl; }

  cout << " ------------- " << endl;
  cout << "Parameter Statistics..." << endl;
  cout << "Total Number of Parameters in the fit: " << fNParametersInFit << endl;
  cout << "Total Number of PMTs across all runs: " << fNDataPointsInFit << endl;

  Int_t nParVary = 0;
  Int_t nParFixed = 0;
  for ( Int_t iP = 1; iP <= fNParametersInFit; iP++ ){
    if ( fMrqVary[ iP ] == 0 ){ nParFixed++; }
    else { nParVary++; }
  }
  cout << "Number of parameters potentially allowed to vary in the fit: " << nParVary << endl;
  cout << "Number of parameters fixed in the fit: " << nParFixed << endl;
  cout << " ------------- " << endl;
  
  cout << "When fitting, fit will skip every " << fNPMTSkip << " pmts in the data set." << endl;

  cout << " ------------- " << endl;

  cout << "Cut information now follows..." << endl;
  cout << " ------------- " << endl;
  cout << "The initial guess chisquare, X, for each PMT must be: " << fChiSquareMinLimit << " < X < " << fChiSquareMaxLimit << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must be within " << fNSigma << " standard deviations from the mean occupancy of it's respective run." << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must have a occupancy count of at least " << fNOccupancy << " accross both off-axis and central runs." << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must have a AVHD Shadowing value, X, in the region " << fAVHDShadowingMin << " < X < " << fAVHDShadowingMax << " accross both off-acis and central runs." << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must have a Geo Shadowing value, X, in the region " << fGeoShadowingMin << " < X < " << fGeoShadowingMax << " accross both off-acis and central runs." << endl;
  cout << " ------------- " << endl;
  cout << "Values for which the PMTs will be cut from the CSS and CHS reports are, CSS: " << fCSSFlag << ", CHS: " << fCHSFlag << endl;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::DataScreen()
{

  cout << " ------------- " << endl;
  cout << "Now screening PMTs across all runs and performing cuts..." << endl;
  cout << " ------------- " << endl;

  Float_t pmtResidual, pmtChiSquared, pmtSigma, pmtData, pmtModel;
  Float_t occValErr, occVal, dcOccValMean, dcSigma;
  Int_t nPMTs, iX, tmpPMT, tmpRun;

  fNPMTsInFit = 0;

  Int_t counter = 0;
  for ( Int_t iRun = 0; iRun < fNRuns; iRun++ ){

    cout << "Checking Run: " << iRun + 1 << " of " << fNRuns << endl;
    fCurrentRun = fRunReader.GetRunEntry( iRun );
    nPMTs = fCurrentRun->GetNPMTs();
    cout << "Run ID is: " << fCurrentRun->GetRunID() << endl;
    cout << " ------------- " << endl;

    ///////////////////////////////////////////
    ///////////////////////////////////////////

    dcOccValMean = 0.0;
    // Obtain the mean occRatio for the run
    for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); fiPMT++ ){      
      occVal = ( ( fiPMT->second ).GetMPECorrOccupancy() / ( fiPMT->second ).GetCentralMPECorrOccupancy() );
      occVal *= fMrqParameters[ GetLBNormalisationParIndex() + iRun ];
      dcOccValMean += occVal;
    }
    if( nPMTs != 0 ){ dcOccValMean /= nPMTs; }

    dcSigma = 0.0;
    // Obtain the sigma occRatio for the run
    for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); fiPMT++ ){      
      occVal = ( ( fiPMT->second ).GetMPECorrOccupancy() / ( fiPMT->second ).GetCentralMPECorrOccupancy() );
      occVal *= fMrqParameters[ GetLBNormalisationParIndex() + iRun ];
      dcSigma += TMath::Power( (occVal - dcOccValMean), 2 );
    }
    if( nPMTs != 0 ){ dcSigma = TMath::Sqrt( dcSigma / nPMTs ); }

    ///////////////////////////////////////////
    ///////////////////////////////////////////

    // Screen PMTs for bad tubes
    counter = 0;
    for ( Int_t iPMT = 0; iPMT < fCurrentRun->GetNPMTs(); iPMT++){
      fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );
      
      counter++;
      Double_t nPMTsToCheck = (Double_t)( fCurrentRun->GetNPMTs() );
      if ( counter % 500 == 0 ){ 
        cout << (Double_t)( counter / nPMTsToCheck ) * 100.0 << " % of run " << iRun + 1 << " of " << fNRuns << " complete." << endl; 
      }
      
      // Check to see if the PMT meets the cut criteria
      Bool_t skipPMT = PMTSkip( fCurrentRun, fCurrentPMT, dcOccValMean, dcSigma );

      if( !skipPMT ){
        
        // Note: Not including PMT variability sigma in here, this was in LOCAS (SNO)
        // but will probably have changed for SNO+ so will need recomputing
        pmtChiSquared = CalculatePMTChiSquare( fCurrentRun, fCurrentPMT );
        pmtSigma = CalculatePMTSigma( fCurrentPMT );
        pmtData = CalculatePMTData( fCurrentPMT );
        pmtModel = ModelPrediction( fCurrentRun, fCurrentPMT );
        pmtResidual = ( pmtData - pmtModel );
        
        if ( fNPMTsInFit >= 0 && fNPMTsInFit < fNElements - 1 ){
          fChiArray[ fNPMTsInFit + 1 ] = pmtChiSquared;
          fResArray[ fNPMTsInFit + 1 ] = pmtResidual;
        }	  
        
        
        fMrqX[ fNPMTsInFit + 1 ] = (Float_t)( ( iPMT ) + ( iRun * 10000 ) );         // 'Special' Indexing Scheme
        fMrqY[ fNPMTsInFit + 1 ] = pmtData;
        fMrqSigma[ fNPMTsInFit + 1 ] = pmtSigma;
        
        fNPMTsInFit++;
        
      }
    }
    cout << " ------------- " << endl;
  }

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  
  cout << "Current number of PMTs in Fit is: " << fNPMTsInFit << endl;
  cout << "Removing dud data values...";
  Int_t jVar = 0;
  for ( Int_t iK = 1; iK < fNPMTsInFit; iK++ ){
    
    if ( !isnan( fMrqX[ iK ] ) 
	 && !isnan( fMrqY[ iK ] ) 
         && !isnan( fMrqSigma[ iK ] )
         && !isnan( fChiArray[ iK ] )
         && !isnan( fResArray[ iK ] )){
      
      fMrqX[ jVar + 1 ] = fMrqX[ iK ];
      fMrqY[ jVar + 1 ] = fMrqY[ iK ];
      fMrqSigma[ jVar + 1 ] = fMrqSigma[ iK ];
      
      fChiArray[ jVar + 1 ] = fChiArray[ iK ];
      fChiSquare += fChiArray[ iK ];
      fResArray[ jVar + 1 ] = fResArray[ iK ];
      
      jVar++;
      
    }
  }
  cout << "done." << endl;
  cout << " ------------- " << endl;
  fNPMTsInFit = jVar;

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Current number of PMTs in Fit is: " << fNPMTsInFit << endl;
  cout << "Removing PMTs with high chisquare contributions...";
  jVar = 0;
  for ( Int_t iK = 1; iK < fNPMTsInFit; iK++ ){
    
    if ( fChiArray[ iK ]  < fChiSquareMaxLimit 
	 && fChiArray[ iK ] > fChiSquareMinLimit ){

      fMrqX[ jVar + 1 ] = fMrqX[ iK ];
      fMrqY[ jVar + 1 ] = fMrqY[ iK ];
      fMrqSigma[ jVar + 1 ] = fMrqSigma[ iK ];

      fChiArray[ jVar + 1 ] = fChiArray[ iK ];
      fResArray[ jVar + 1 ] = fResArray[ iK ];
    
      jVar++;
    }
    
  }
  cout << "done." << endl;
  cout << " ------------- " << endl;
  fNPMTsInFit = jVar;

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  

  Int_t nLBDistBins = fNLBDistributionPhiBins * fNLBDistributionThetaBins;
  Int_t iAngValid, iLBValid;
  Float_t pmtR, pmtLB;

  // Now we loop through all the angular response and laserball distribution bins
  // to count the number of entries in each one.

  Int_t* lbValid = new Int_t[ nLBDistBins ];
  for ( Int_t i = 0; i < nLBDistBins; i++ ){ lbValid[ i ] = 0; }

  Int_t *pmtAngleValid = new Int_t[ fNAngularResponseBins ];
  for (Int_t i = 0; i < fNAngularResponseBins; i++ ){ pmtAngleValid[ i ] = 0; }

  cout << "Current number of PMTs in Fit is: " << fNPMTsInFit << endl;
  cout << "Counting entries in each parameter distribution histogram...";
  
  for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
    iX = (Int_t)fMrqX[ iPMT ];
    tmpRun = (Int_t)( iX / 10000 );
    tmpPMT = (Int_t)( iX % 10000 );
    
    fCurrentRun = fRunReader.GetRunEntry( tmpRun );
    fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );
    
    pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );
    pmtAngleValid[ iAngValid ]++;
    
    pmtLB = ModelLBDistribution( fCurrentRun, fCurrentPMT, iLBValid, 0 );
    lbValid[ iLBValid ]++;    
  }

  cout << "done." << endl;
  cout << " ------------- " << endl;

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Counting number of invalid bins in laserball distribution..." << endl;
  
  // LB Distribution
  Int_t lbBinsInvalid = 0;
  
  for ( Int_t iBin = 0; iBin < nLBDistBins; iBin++ ){       
    if ( lbValid[ iBin ] < fNPMTsPerLBDistributionBinMin ){

      fMrqVary[ GetLBDistributionParIndex() + iBin ] = 0; 
      fMrqParameters[ GetLBDistributionParIndex() + iBin ] = fLBDistributionInit;
 
      if( lbValid[ iBin ] > 0 ){ 
        lbBinsInvalid++; 
        printf( "Invalid laserball distribution bin %d, itheta %d iphi %d: only %d PMT's\n",
               iBin,
               iBin / fNLBDistributionThetaBins,
               iBin % fNLBDistributionPhiBins, 
               lbValid[ iBin ] );
      }	
    }
  }

  cout << "done." << endl;
  cout << "Number of invalid laserball disitrubtion bins: " << lbBinsInvalid << endl;
  cout << " ------------- " << endl;

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Counting number of invalid bins in angular response distribution..." << endl;

  // Angular Response
  Int_t anglesInvalid = 0;

  for ( Int_t iBin = 0; iBin < fNAngularResponseBins; iBin++ ){      
    if ( pmtAngleValid[ iBin ] < fNPMTsPerAngularResponseBinMin ){

      fMrqVary[ GetAngularResponseParIndex() + iBin ] = 0;
      fMrqParameters[ GetAngularResponseParIndex() + iBin ] = fAngularResponseInit;
	
      if ( pmtAngleValid[ iBin ] > 0 ){ 
        anglesInvalid++; 
        printf("Invalid angular response bin %d, angle %6.2f: only %d PMT's\n",
               iBin,
               iBin * (90.0 / fNAngularResponseBins ),
               pmtAngleValid[ iBin ] );
      }
    }      
  } 

  cout << "done." << endl;
  cout << "Number of invalid angular response distribution  bins: " << anglesInvalid << endl;
  cout << " ------------- " << endl; 

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Removing PMTs with which don't meet minimum binning requirements..." << endl;
  Int_t auxAnglesValid = 0;
  if ( anglesInvalid == 0 && lbBinsInvalid == 0 ){
    cout << "All bins for the PMT Angular Response and Laserball Distribution are filled" << endl;
  }
  else{
    jVar = 0;
    for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
      iX = (Int_t)fMrqX[ iPMT ];
      tmpRun = (Int_t)( iX / 10000 );
      tmpPMT = (Int_t)( iX % 10000 );
      
      fCurrentRun = fRunReader.GetRunEntry( tmpRun );
      fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );
      
      pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );     
      pmtLB = ModelLBDistribution( fCurrentRun, fCurrentPMT, iLBValid, 0 );
      auxAnglesValid = pmtAngleValid[ iAngValid ];

      if( lbValid[ iLBValid ] >= fNPMTsPerLBDistributionBinMin 
	  && auxAnglesValid >= fNPMTsPerAngularResponseBinMin ){

        fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
        fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
        fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
        
        jVar++;
      }
    }   
    fNPMTsInFit = jVar;
  }
  
  cout << "done." << endl;
  cout << " ------------- " << endl;
  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  
  if ( fNPMTsInFit > 0 ){ fDataScreen = true; }
  else{ 
    fDataScreen = false; 
    cout << "No PMTs passed the cut criteria. Aborting." << endl;
    return; 
  }

  cout << "Final number of PMTs to be used in Fit is: " << fNPMTsInFit << endl;
  cout << "Loading these PMTs into memory..." << endl;

  // All the PMTs which are to be used in the fit are then stored
  // in a std::map< Int_t, LOCASPMT > map (fFitPMTs)
  for ( Int_t iPM = 1; iPM <= fNPMTsInFit; iPM++ ){

    Int_t iX = fMrqX[ iPM ];
    Int_t pmtID = iX % 10000;
    Int_t runN = iX / 10000;
    fCurrentRun = fRunReader.GetRunEntry( runN );
    LOCASPMT lPMT = fCurrentRun->GetPMT( pmtID );
    fFitPMTs[ iX ] = lPMT;

  }

  cout << "done.\n" << endl;
  cout << " ------------- " << endl;
  cout << "Data Screen Complete" << endl;
  cout << " ------------- " << endl;
  cout << " ------------- " << endl;

  delete lbValid;
  delete pmtAngleValid;

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFit::PMTSkip( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Float_t mean, Float_t sigma )
{

  Bool_t pmtSkip = false;

  Float_t pmtData = CalculatePMTData( iPMTPtr );
  Float_t pmtSigma = CalculatePMTSigma( iPMTPtr );
  Float_t chiSqVal = CalculatePMTChiSquare( iRunPtr, iPMTPtr );
  TVector3 pmtPos = iPMTPtr->GetPos();

  if ( iPMTPtr->GetBadPath()
       || iPMTPtr->GetCentralBadPath()
       || iPMTPtr->GetMPECorrOccupancy() < fNOccupancy
       || iPMTPtr->GetCentralMPECorrOccupancy() < fNOccupancy 
       || chiSqVal < fChiSquareMinLimit
       || chiSqVal > fChiSquareMaxLimit 
       || pmtData > fPMTDataROccMaxLimit
       || pmtData < fPMTDataROccMinLimit){ 
    pmtSkip = true; 
  }
       
  return pmtSkip;
    
}


//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelAngularResponse( const LOCASPMT* iPMTPtr, Int_t& iAng, Int_t runType )
{

  Float_t cosTheta = 1.0;
  if ( runType == 0 ) cosTheta = iPMTPtr->GetCosTheta();
  if ( runType == 1 ) cosTheta = iPMTPtr->GetCentralCosTheta();
  if ( runType == 2 ) cosTheta = iPMTPtr->GetWavelengthCosTheta();
  
  Float_t angle = ( TMath::ACos( cosTheta ) ) * ( 180.0 / TMath::Pi() );
  iAng = (Int_t)( angle * fNAngularResponseBins / 90.0 );
  
  return GetAngularResponsePar( iAng );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistribution( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Int_t& iLBDist, Int_t runType )
{

  Float_t lbTheta, lbPhi, lbRelTheta, lbRelPhi;

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  TVector3 pmtRelVec( 0.0, 0.0, 0.0 );

  if ( runType == 0 ){
    lbTheta = iRunPtr->GetLBTheta();
    lbPhi = iRunPtr->GetLBPhi();
    lbRelTheta = iPMTPtr->GetRelLBTheta();
    lbRelPhi = iPMTPtr->GetRelLBPhi();
  }

  if ( runType == 1 ){
    lbTheta = iRunPtr->GetCentralLBTheta();
    lbPhi = iRunPtr->GetCentralLBPhi();
    lbRelTheta = iPMTPtr->GetCentralRelLBTheta();
    lbRelPhi = iPMTPtr->GetCentralRelLBPhi();
  }

  if ( runType == 2 ){
    lbTheta = iRunPtr->GetWavelengthLBTheta();
    lbPhi = iRunPtr->GetWavelengthLBPhi();
    lbRelTheta = iPMTPtr->GetWavelengthRelLBTheta();
    lbRelPhi = iPMTPtr->GetWavelengthRelLBPhi();
  }

  Float_t cosTheta = 0.0;
  Float_t phi = 0.0;

  if ( lbTheta != 0.0 ){
    lbAxis.SetTheta( lbTheta );
    lbAxis.SetPhi( lbPhi );

    pmtRelVec.SetTheta( lbRelTheta );
    pmtRelVec.SetPhi( lbRelPhi );

    cosTheta = lbAxis * pmtRelVec;
    phi = 0.0;
  }

  else{
    cosTheta = TMath::Cos( lbRelTheta );
    phi = fmod( (Double_t) ( lbPhi + lbRelPhi ), 2.0 * TMath::Pi() );
  }

  if ( cosTheta > 1.0 ) cosTheta = 1.0;
  else if ( cosTheta < -1.0 ) cosTheta = -1.0;

  if ( phi > 2 * TMath::Pi() ) phi -= 2 * TMath::Pi();
  else if ( phi < 0 ) phi += 2 * TMath::Pi();

  
  
  Int_t iTheta = (Int_t)( ( 1 + cosTheta ) / 2 * fNLBDistributionThetaBins );
  if ( iTheta < 0 ) iTheta = 0;
  if ( iTheta >= fNLBDistributionThetaBins ) iTheta = fNLBDistributionThetaBins - 1;

  Int_t iPhi = (Int_t)( phi / ( 2 * TMath::Pi() ) * fNLBDistributionPhiBins );
  if ( iPhi < 0 ) iPhi = 0;
  if ( iPhi >= fNLBDistributionPhiBins ) iPhi = fNLBDistributionPhiBins - 1;

  iLBDist = iTheta * fNLBDistributionPhiBins + iPhi;
  Float_t laserlight = GetLBDistributionPar( iLBDist );
  return laserlight;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTChiSquare( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Float_t modelPred = ModelPrediction( iRunPtr, iPMTPtr );
  Float_t occVal = CalculatePMTData( iPMTPtr );
  Float_t occValErr = CalculatePMTSigma( iPMTPtr );
  Float_t chiSq = ( ( modelPred - occVal ) * ( modelPred - occVal ) ) / ( occValErr * occValErr );

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculateChiSquare()
{

  Float_t chiSquare = 0.0;

  Int_t iX, jRun;

  if ( !fDataScreen ){ cout << "LOCASFit::CalculateChiSquare: Error, run LOCASFit::DataScreen first"; }
  else{

    for ( fiPMT = fFitPMTs.begin(); fiPMT != fFitPMTs.end(); fiPMT++ ){

      iX = ( fiPMT->first );
      jRun = iX / 10000;
      fCurrentPMT = &( fiPMT->second );
      fCurrentRun = fRunReader.GetRunEntry( jRun );

  
      chiSquare += CalculatePMTChiSquare( fCurrentRun, fCurrentPMT );
    }
  }

  return chiSquare;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTSigma( const LOCASPMT* iPMTPtr )
{

  Float_t pmtSigma = flMath.OccRatioErr( iPMTPtr );
  return pmtSigma;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTData( const LOCASPMT* iPMTPtr )
{

  Float_t pmtData = ( iPMTPtr->GetMPECorrOccupancy() / iPMTPtr->GetCentralMPECorrOccupancy() );
  return pmtData;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelPrediction( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Int_t nA, Float_t* dyda )
{

  fiAng = 0;
  fiLBDist = 0;
  fiNorm = GetRunIndex( iRunPtr->GetRunID() );

  Bool_t derivatives = false;
  if( nA > 0 && dyda != NULL ){
    derivatives = true;

    dyda[ GetLBNormalisationParIndex() + fiNorm ] = 0;
    dyda[ GetAngularResponseParIndex() + fiAng ] = 0;
    dyda[ GetAngularResponseParIndex() + fCiAng ] = 0;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = 0;
    dyda[ GetLBDistributionParIndex() + fCiLBDist ] = 0;

  }
 
  Float_t normVal = ( iPMTPtr->GetNLBPulses() / iPMTPtr->GetCentralNLBPulses() ) * GetLBNormalisationPar( fiNorm );
  //cout << "normVal is: " << normVal << endl;

  Float_t solidARatio = ( iPMTPtr->GetSolidAngle() / iPMTPtr->GetCentralSolidAngle() );
  //cout << "solidARatio is: " << solidARatio << endl;

  Float_t fresnelTRatio = ( iPMTPtr->GetFresnelTCoeff() / iPMTPtr->GetCentralFresnelTCoeff() );
  //cout << "Fresnel T Ratio is: " << fresnelTRatio << endl;

  Float_t dScint = ( iPMTPtr->GetDistInScint() ) - ( iPMTPtr->GetCentralDistInScint() );
  Float_t dAV = ( iPMTPtr->GetDistInAV() ) - ( iPMTPtr->GetCentralDistInAV() );
  Float_t dWater = ( iPMTPtr->GetDistInWater() ) - ( iPMTPtr->GetCentralDistInWater() );

  Float_t angResp = ModelAngularResponse( iPMTPtr, fiAng, 0 );
  //cout << "angResponse is: " << angResp << endl;
  Float_t intensity = ModelLBDistribution( iRunPtr, iPMTPtr, fiLBDist, 0 );
  //cout << "intensity is: " << intensity << endl;

  Float_t pmtResponse = normVal * angResp * intensity * solidARatio * fresnelTRatio 
    * TMath::Exp( - ( dScint * ( GetScintPar() + GetScintRSPar() ) )
                  - ( dAV * ( GetAVPar() + GetAVRSPar() ) )
                  - ( dWater * ( GetWaterPar() + GetWaterRSPar() ) ) );

  if( derivatives ){

    dyda[ GetLBNormalisationParIndex() + fiNorm ] = +1.0 / normVal;
    dyda[ GetScintParIndex() ] = -dScint;
    dyda[ GetAVParIndex() ] = -dAV;
    dyda[ GetWaterParIndex() ] = -dWater;

    dyda[ GetScintRSParIndex() ] = -dScint;
    dyda[ GetAVRSParIndex() ] = -dAV;
    dyda[ GetWaterRSParIndex() ] = -dWater;

    dyda[ GetAngularResponseParIndex() + fiAng ] = +1.0 / angResp;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = +1.0 / intensity;

  }

  fCiAng = 0;
  fCiLBDist = 0;
  Float_t angRespCtr = ModelAngularResponse( iPMTPtr, fCiAng, 1 );
  Float_t intensityCtr = ModelLBDistribution( iRunPtr, iPMTPtr, fCiLBDist, 1 );

  Float_t pmtResponseCtr = angRespCtr * intensityCtr;

  if( derivatives ){

    dyda[ GetAngularResponseParIndex() + fCiAng ] -= 1.0 / angRespCtr;
    dyda[ GetLBDistributionParIndex() + fCiLBDist ] -= 1.0 / intensityCtr;

  }
  Float_t modelValue = pmtResponse / pmtResponseCtr;

  if( derivatives ){
    FillParameterPoint();
    for ( Int_t i = 1; i <= fParam; i++ ){
      dyda[ fParamIndex[ i ] ] *= modelValue;
    }
  }

  return modelValue;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PerformFit()
{

  if ( fDataScreen == true && fFitPMTs.size() > 0 ){

    fChiSquare = 0.0;
    cout << "LOCASFit::PerformFit: Performing fit...";
    FillParameterbase();
    FillAngIndex();
    
    Int_t val = MrqFit(fMrqX, fMrqY, fMrqSigma, fNPMTsInFit, fMrqParameters, fMrqVary, fNParametersInFit, fMrqCovariance,
                       fMrqAlpha, &fChiSquare);

    cout << "done." << endl;
    cout << " ------------- " << endl;
  }
  else{ return; }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillParameterbase( )
{

  if ( fParamIndex != NULL ) delete[] fParamIndex;
  fParamIndex = new Int_t[ fNParametersInFit ];          // The number of unique parameters is guaranteed to be less than 
                                                         // the number of total parameters

  fParamBase = 0;
  if( GetScintVary() ) fParamIndex[ ++fParamBase ] = GetScintParIndex();
  if( GetAVVary() ) fParamIndex[ ++fParamBase ] = GetAVParIndex();
  if( GetWaterVary() ) fParamIndex[ ++fParamBase ] = GetWaterParIndex();
  if( GetScintRSVary() ) fParamIndex[ ++fParamBase ] = GetScintRSParIndex();
  if( GetAVRSVary() ) fParamIndex[ ++fParamBase ] = GetAVRSParIndex();
  if( GetWaterRSVary() ) fParamIndex[ ++fParamBase ] = GetWaterRSParIndex();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillAngIndex( )
{

  if ( fParamVarMap != NULL ) delete[] fParamVarMap;
  fParamVarMap = new Int_t[ fNParametersInFit + 1 ];

  Int_t i, j;

  j = 0;
  for ( i = 1; i <= fNParametersInFit; i++ ) if ( fMrqVary[ i ] ) fParamVarMap[ i ] = ++j;
  
  if ( fAngIndex != NULL ){
    for ( i = 0; i < fNAngularResponseBins + 1; i++ ){
      for ( j = 0; j < fNAngularResponseBins + 1; j++ ){
	delete[] fAngIndex[ i ][ j ];
      }
      delete[] fAngIndex[ i ];
    }
    delete[] fAngIndex;
  }
  
  fAngIndex = NULL;
  
  fAngIndex = new Int_t**[ fNAngularResponseBins + 1 ];
  for ( i = 0; i < fNAngularResponseBins + 1; i++ ){
    fAngIndex[ i ] = new Int_t*[ fNAngularResponseBins + 1 ];
    for ( j = 0; j < fNAngularResponseBins + 1; j++ ) fAngIndex[ i ][ j ] = new Int_t[ 4 + 1 ];
  }

  Int_t first, second;

  for ( i = 0; i <= fNAngularResponseBins; i++ ) {
    for ( j = 0; j <= fNAngularResponseBins; j++ ) {
      if ( i <= j ) { first = i; second = j; }
      else { first = j; second = i; }
      if (first==second) {
        fAngIndex[i][j][0] = 1;
        fAngIndex[i][j][1] = first;
        fAngIndex[i][j][2] = -1;
        fAngIndex[i][j][3] = -1;
        fAngIndex[i][j][4] = -1;
      } 
      else {
        fAngIndex[i][j][0] = 2;
        fAngIndex[i][j][1] = first;
        fAngIndex[i][j][2] = second;
        fAngIndex[i][j][3] = -1;
        fAngIndex[i][j][4] = -1;
      }
    }
  }
  
}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillParameterPoint()
{

  Int_t i;

  fParam = fParamBase;

  Int_t parnum;
  for ( i = 1; i <= fAngIndex[ fiAng ][ fCiAng ][ 0 ]; i++ ){
    parnum = GetAngularResponseParIndex() + fAngIndex[ fiAng ][ fCiAng ][ i ];
    if ( fMrqVary[ parnum ] ){
      fParamIndex[ ++fParam ] = parnum;
    }
  }
  Int_t first, second;
  if ( fiLBDist <= fCiLBDist ){ first = fiLBDist; second = fCiLBDist; }
  else{ first = fCiLBDist; second = fiLBDist; }

  if ( first != second ){
    parnum = GetLBDistributionParIndex() + first;
    if ( fMrqVary[ parnum ] ) fParamIndex[ ++fParam ] = parnum;

    parnum = GetLBDistributionParIndex() + second;
    if ( fMrqVary[ parnum ] ) fParamIndex[ ++fParam ] = parnum;
  }

  else{
    parnum = GetLBDistributionParIndex() + fiLBDist;
    if ( fMrqVary[ parnum ] ) fParamIndex[ ++fParam ] = parnum;
  }

  parnum = GetLBNormalisationParIndex() + fiNorm;
  if( fMrqVary[ parnum ] ) fParamIndex[ ++fParam ] = parnum;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::WriteFitToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );
  // Create the Run Tree
  TTree* runTree = new TTree( fFitName.c_str(), fFitTitle.c_str() );

  // Declare a new branch pointing to the data stored in the lRun object
  runTree->Branch( "LOCASFit", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::DeAllocate()
{

  // Free up all the memory used during the fitting procedure.
  // This method is called by the LOCASFit desctructor.

  if ( fMrqX ){ flMath.LOCASFree_Vector( fMrqX, 1, fNDataPointsInFit ); }
  fMrqX = NULL;

  if ( fMrqY ){ flMath.LOCASFree_Vector( fMrqY, 1, fNDataPointsInFit ); }
  fMrqY = NULL;

  if ( fMrqSigma ){ flMath.LOCASFree_Vector( fMrqSigma, 1, fNDataPointsInFit ); }
  fMrqSigma = NULL;

  fCurrentRun = NULL;
  fCurrentPMT = NULL;

  if ( fChiArray != NULL ){ delete[] fChiArray; }
  fChiArray = NULL;
  if ( fResArray != NULL ){ delete[] fResArray; }
  fResArray = NULL;

  if ( fMrqParameters ){ flMath.LOCASFree_Vector( fMrqParameters, 1, fNParametersInFit ); }
  fMrqParameters = NULL;

  if ( fMrqVary ){ flMath.LOCASFree_IntVector( fMrqVary, 1, fNParametersInFit ); }
  fMrqVary = NULL;

  if ( fMrqCovariance ){ flMath.LOCASFree_Matrix( fMrqCovariance, 1, fNParametersInFit, 1, fNParametersInFit ); }
  fMrqCovariance = NULL;

  if ( fMrqAlpha ){ flMath.LOCASFree_Matrix( fMrqAlpha, 1, fNParametersInFit, 1, fNParametersInFit ); }
  fMrqAlpha = NULL;

  if ( fParamIndex != NULL ){ delete[] fParamIndex; }
  fParamIndex = NULL;
  if ( fParamVarMap != NULL ){ delete[] fParamVarMap; }
  fParamVarMap = NULL;

  if ( fAngIndex ){
    for ( Int_t i = 0; i < fNAngularResponseBins + 1; i++ ){
      for ( Int_t j = 0; j < fNAngularResponseBins + 1; j++ ){
  	delete[] fAngIndex[ i ][ j ];
      }
      delete[] fAngIndex[ i ];
    }
    delete[] fAngIndex;
  }
  fAngIndex = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::MrqFit( float x[], float y[], float sig[], int ndata, float a[],
			int ia[], int ma, float **covar, float **alpha, float *chisq )
{
  //Fit these data using mrqmin() repeatedly until convergence is achieved.
  
  Int_t maxiter = 1000;
  Int_t numiter = 0;
  Int_t gooditer = 0;
  Int_t retval = 0;

  Float_t oldchisq = 0;
  Float_t lamda = -1.0;
  Float_t tol = 1.0;    // Chisquared must change by tol to warrant another iteration

  *chisq = 0;

  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda);
  oldchisq = *chisq;
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lamda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
         && retval == 0 && lamda != 0.0) {
    oldchisq = *chisq;
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda );
    numiter++;
    printf("New ChiSquare = %12.2f with lambda %g \n", *chisq, lamda);
    if ( fabs( oldchisq - *chisq ) < tol ) gooditer++;
    else gooditer = 0;
  }
  
  // We're done.  Set lamda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  lamda = 0;
  retval = mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lamda );
  return retval;
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::mrqmin(float x[], float y[], float sig[], int ndata, float a[],
    		  int ia[], int ma, float **covar, float **alpha, float *chisq,
    		  float *alamda )
{
  // Minimization routine for a single iteration over the data points.
  
  // Required helper routines:
  // void covsrt(float **covar, int ma, int ia[], int mfit);
  // void gaussj(float **a, int n, float **b, int m);
  // void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
  //	int ia[], int ma, float **alpha, float beta[], float *chisq,
  //	void (*funcs)(float, float [], float *, float [], int));

  int j,k,l,m, retval = 0;
  static int mfit;
  static float ochisq,*atry,*beta,*da,**oneda;
  
  
  //--------------------
  // Initialization  
  if (*alamda < 0.0) {
    atry=flMath.LOCASVector(1,ma);
    beta=flMath.LOCASVector(1,ma);
    da=flMath.LOCASVector(1,ma);
    printf("Varying parameter indices:\n");
    for (mfit=0,j=1;j<=ma;j++){
      if (ia[j]) {
        mfit++; 
        printf("\tParameter %.3d varies\n",j);
      }
    }
    
    oneda=flMath.LOCASMatrix(1,mfit,1,1);
    *alamda=0.001;
    mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq);
    ochisq=(*chisq);
    for (j=1;j<=ma;j++) atry[j]=a[j];
  }
  
  // Bookkeeping on covariance and derivatives to prepare next parameter set.
  for (j=0,l=1;l<=ma;l++) {
    if (ia[l]) {
      for (j++,k=0,m=1;m<=ma;m++) {
        if (ia[m]) {
          k++;
          covar[j][k]=alpha[j][k];
        }
      }
      covar[j][j]=alpha[j][j]*(1.0+(*alamda));
      oneda[j][1]=beta[j];
	  if(covar[j][j] <= 0.0) {
	    if(covar[j][j] == 0.0) {
	      printf("*** Zero covariance diagonal element at j %d (l %d)\n",j,l);
	      printf("*** Bad parameter %d\n",l);
	    } else {
	      printf("*** Negative covariance diagonal element at j %d (l %d)\n",j,l);
	    }
	  }
    }
  }
  
  printf("Inverting the solution matrix in QOCAFit::mrqmin()...");
  retval = gaussj(covar,mfit,oneda,1);
  if (retval<0) printf("error %d...",retval);
  printf("done.\n");
  
  for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
  //--------------------
  // Final call to prepare covariance matrix and deallocate memory.
  if (*alamda == 0.0 ) {
    covsrt(covar,ma,ia,mfit);
    flMath.LOCASFree_Matrix(oneda,1,mfit,1,1);
    flMath.LOCASFree_Vector(da,1,ma);
    flMath.LOCASFree_Vector(beta,1,ma);
    flMath.LOCASFree_Vector(atry,1,ma);
    return retval;
  }
  
  //--------------------
  // Set up the trial parameters and try them
  for (j=0,l=1;l<=ma;l++)
    if (ia[l]) atry[l]=a[l]+da[++j];
  
  mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq);
  
  if (*chisq < ochisq) {
    *alamda *= 0.1;
    ochisq=(*chisq);
    for (j=0,l=1;l<=ma;l++) {
      if (ia[l]) {
        for (j++,k=0,m=1;m<=ma;m++) {
          if (ia[m]) {
            k++;
            alpha[j][k]=covar[j][k];
          }
        }
        beta[j]=da[j];
        a[l]=atry[l];
      }
    }
  } else {
    *alamda *= 10.0;
    *chisq=ochisq;
  }
  return retval;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::covsrt(float **covar, int ma, int ia[], int mfit)
{
  // Covariance matrix sorting.  Helper routine for mrqmin()

  int i,j,k;
  float swap;
  
  for (i=mfit+1;i<=ma;i++)
    for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
  k=mfit;
  for (j=ma;j>=1;j--) {
    if (ia[j]) {
      for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j]) 
        for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
	  k--;
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::mrqcof(float x[], float y[], float sig[], int ndata, float a[],
    			int ia[], int ma, float **alpha, float beta[],
    			float *chisq)
{

  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  float chisqentry;  // chi-squared for single entry in list
  
  float *beta2 = flMath.LOCASVector(1,ma);
  float **alpha2 = flMath.LOCASMatrix(1,ma,1,ma);
  dyda=flMath.LOCASVector(1,ma);
  for (j=1;j<=ma;j++) {
    if (ia[j]) mfit++;
    dyda[j] = 0.0;
  }

  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) {
      alpha[j][k]=0.0;
      alpha2[j][k]=0.0;
    }
    beta[j]=0.0;
    beta2[j]=0.0;
  }
  *chisq=0.0;
  for (i=1;i<=ndata;i+=fNPMTSkip) {  // Skip some tubes to increase speed...
    mrqfuncs( x[i],i,a,&ymod,dyda,ma );
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;

    FillParameterPoint();
    for (l=1; l<=fParam; l++) {	
      
      wt = dyda[fParamIndex[l]] * sig2i;
      for (m=1; m<=l; m++) {
    	j = fParamIndex[l];
    	k = fParamIndex[m];
    	if (k<=j) alpha[fParamVarMap[j]][fParamVarMap[k]] += wt * dyda[k];
      }
      beta[fParamVarMap[fParamIndex[l]]] += dy * wt;
    }
      
    // for (j=0,l=1;l<=ma;l++) {  // Original Numerical recipes code
    //   if (ia[l]) {
	// wt=dyda[l]*sig2i;
	// for (j++,k=0,m=1;m<=l;m++)
	//   if (ia[m]) alpha2[j][++k] += wt*dyda[m];
	// beta2[j] += dy*wt;
    //   }
    // }
    
    //........................................
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    if (i%1==2000) printf(" %d %f %f\n",i,x[i],chisqentry);
    if (fChiArray!=NULL && i>=0 && i<fNElements) fChiArray[i] = chisqentry;
    if (fResArray!=NULL && i>=0 && i<fNElements) fResArray[i] = dy;
  }

  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) {
      alpha[k][j]=alpha[j][k];
    }
  
  flMath.LOCASFree_Vector(dyda,1,ma);
  
  flMath.LOCASFree_Matrix(alpha2,1,ma,1,ma);
  flMath.LOCASFree_Vector(beta2,1,ma);

}
//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::mrqfuncs(Float_t x,Int_t ix,Float_t a[],Float_t *y,
                          Float_t dyda[],Int_t na)
{
  
  ix = (Int_t)x;
  Int_t jpmt = ix%10000;
  Int_t irun = ix/10000;

  fCurrentRun = fRunReader.GetRunEntry( irun );
  fCurrentPMT = &fCurrentRun->GetPMT( jpmt );

  Float_t *mrqparsave = fMrqParameters; // Save parameters and use the ones just passed
  fMrqParameters = a;

  *y = ModelPrediction( fCurrentRun, fCurrentPMT, na, dyda );  // Derivatives also calculated!
  
  fMrqParameters = mrqparsave; // Restore parameters
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::gaussj(float **a, int n, float **b, int m)
{
  // Gauss-Jordan matrix solution helper routine for mrqmin.

  int *indxc,*indxr,*ipiv;
  int i,icol,irow,j,k,l,ll;
  float big,dum,pivinv,swap;
  Int_t retval = 0;
  indxc=flMath.LOCASIntVector(1,n);
  indxr=flMath.LOCASIntVector(1,n);
  ipiv=flMath.LOCASIntVector(1,n);
  for (j=1;j<=n;j++) ipiv[j]=0;
  for (i=1;i<=n;i++) {
    big=0.0;
    for (j=1;j<=n;j++)
      if (ipiv[j] != 1)
        for (k=1;k<=n;k++) {
          if (ipiv[k] == 0) {
            if (fabs(a[j][k]) >= big) {
              big=fabs(a[j][k]);
              irow=j;
              icol=k;
            }
          } else if (ipiv[k] > 1) 
            {
              cout << "gaussj: Singular Matrix-1" << endl;
              retval = -1;
            }
        }
    ++(ipiv[icol]);
    if (irow != icol) {
      for (l=1;l<=n;l++) { SWAP(a[irow][l],a[icol][l]) }
      for (l=1;l<=m;l++) { SWAP(b[irow][l],b[icol][l]) }
    }
    indxr[i]=irow;
    indxc[i]=icol;
    if (a[icol][icol] == 0.0) 
      {
        cout << "gaussj: Singular Matrix-2" << endl;;
        retval = -2;
      }
    pivinv=1.0/a[icol][icol];
    a[icol][icol]=1.0;
    for (l=1;l<=n;l++) a[icol][l] *= pivinv;
    for (l=1;l<=m;l++) b[icol][l] *= pivinv;
    for (ll=1;ll<=n;ll++)
      if (ll != icol) {
        dum=a[ll][icol];
        a[ll][icol]=0.0;
        for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
        for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
      }
  }
  for (l=n;l>=1;l--) {
    if (indxr[l] != indxc[l])
      for (k=1;k<=n;k++)
        SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  }
  
  flMath.LOCASFree_IntVector(ipiv,1,n);
  flMath.LOCASFree_IntVector(indxr,1,n);
  flMath.LOCASFree_IntVector(indxc,1,n);
  return retval;

}

/////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetNVariableParameters()
{

  Int_t nParVary = 0;
  Int_t nParFixed = 0;
  for ( Int_t iP = 1; iP <= fNParametersInFit; iP++ ){
    if ( fMrqVary[ iP ] == 0 ){ nParFixed++; }
    else { nParVary++; }
  }

  return nParVary;

}


//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetAngularResponseParIndex()
{

  return 7;

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBDistributionParIndex()
{

  return ( 7 + fNAngularResponseBins );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBNormalisationParIndex()
{

  return ( 7 + fNAngularResponseBins + ( fNLBDistributionPhiBins * fNLBDistributionThetaBins ) );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetRunIndex( const Int_t runID )
{

  for ( Int_t iG = 0; iG < fNRuns; iG++ ){

    if( fListOfRunIDs[ iG ] == runID ){ return iG; }
    if( iG == fNRuns - 1 ){
      cout << "LOCASFit::GetRunEntry: Error: No Index found for given run ID" << endl;
      return 0;
    }
  }
  return 0;

}




