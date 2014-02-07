///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFit.hh
///
/// CLASS: LOCAS::LOCASFit
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"
#include "LOCASFit.hh"

#include <map>

#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#endif

using namespace LOCAS;
using namespace std;

// Default Constructor
LOCASFit::LOCASFit( const char* fitFile )
{

  // Load the fit file (.ratdb format) and set the file from
  // which the fit parameters are found
  LOCASDB lDB;
  //lDB.LoadRunList( fitFile );
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
    fNDataPointsInFit += ( fRunReader.GetLOCASRun( iRun ) )->GetNPMTs();
  }

  // Get the list of Central Run IDs and calculate
  // the total number of central runs
  fListOfCentralRunIDs = lDB.GetIntVectorField( "FITFILE", "central_run_ids" );
  // Add all of the corresponding central run files to the 
  // LOCASRunReader object (LOCAS version of TChain)
  for (Int_t iRun = 0; iRun < fListOfCentralRunIDs.size(); iRun++ ){
    fCentralRunReader.Add( fListOfCentralRunIDs[ iRun ] );
  }

  // Get the list of Wavelength Run IDs and calculate
  // the total number of wavelength runs included in the fit
  fListOfWavelengthRunIDs = lDB.GetIntVectorField( "FITFILE", "wavelength_run_ids" );
  // Add all of the corresponding central run files to the 
  // LOCASRunReader object (LOCAS version of TChain)
  for ( Int_t iRun = 0; iRun < fListOfWavelengthRunIDs.size(); iRun++ ){
    fWavelengthRunReader.Add( fListOfWavelengthRunIDs[ iRun ] );
  }

  
  fChiArray = new Float_t[ fNDataPointsInFit ];
  fResArray = new Float_t[ fNDataPointsInFit ];
  fNElements = fNDataPointsInFit;
  // Set which of the variables in the fit are to be varied
  // and what the starting, initial values of these parameters are
  fScintVary = lDB.GetBoolField( "FITFILE", "scint_vary" );
  fScintInit = lDB.GetDoubleField( "FITFILE", "scint_init" );
  fAVVary = lDB.GetBoolField( "FITFILE", "av_vary" );
  fAVInit = lDB.GetDoubleField( "FITFILE", "av_init" );
  fWaterVary = lDB.GetBoolField( "FITFILE", "water_vary" );
  fWaterInit = lDB.GetDoubleField( "FITFILE", "water_init" );
  fAngularResponseVary = lDB.GetBoolField( "FITFILE", "ang_resp_vary" );
  fAngularResponseInit = lDB.GetDoubleField( "FITFILE", "ang_resp_init" );
  fLBDistributionVary = lDB.GetBoolField( "FITFILE", "lb_dist_vary" );
  fLBDistributionInit = lDB.GetDoubleField( "FITFILE", "lb_dist_init" );

  // If all of the parameters are varied, set this bool TRUE (else, FALSE)
  if ( fScintVary && fAVVary && fWaterVary
       && fAngularResponseVary && fLBDistributionVary ){
    fVaryAll = true;
  }
  else{ fVaryAll = false; }

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
  // each PMT is cut on the below criteria in LOCASFit::ScreenPMT. PMTs which pass
  // these cuts are stored in the std::map< Int_t, LOCASPMT > fFitPMTs
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

  // The total number of parameters in the fit
  fNParametersInFit = 3
    + fNAngularResponseBins
    + fNLBDistributionThetaBins * fNLBDistributionPhiBins;

  // The Levenberg-Marquadt working arrays
  LOCASMath lMath;
  fMrqX = lMath.LOCASVector( 1, fNDataPointsInFit );
  fMrqY = lMath.LOCASVector( 1, fNDataPointsInFit );
  fMrqSigma = lMath.LOCASVector( 1, fNDataPointsInFit );

  fMrqParameters = lMath.LOCASVector( 1, fNParametersInFit );
  fMrqVary = lMath.LOCASIntVector( 1, fNParametersInFit );

  fMrqCovariance = lMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  fMrqAlpha = lMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );

  // Set the initial values (if varied) or the values to use (if not varied)
  // for the model parameters

  // The three reciprical attenuation length initial values;
  SetScintPar( fScintInit );
  if ( fScintVary ){ fMrqVary[ GetScintParIndex() ] = 1; }
  else{ fMrqVary[ GetScintParIndex() ] = 0; }

  SetAVPar( fAVInit );
  if ( fAVVary ){ fMrqVary[ GetAVParIndex() ] = 1; }
  else{ fMrqVary[ GetAVParIndex() ] = 0; }

  SetWaterPar( fWaterInit );
  if ( fWaterVary ){ fMrqVary[ GetWaterParIndex() ] = 1; }
  else{ fMrqVary[ GetWaterParIndex() ] = 0; }

  // The bin values for the angular response histogram
  Float_t angle = 0.0;
  for ( Int_t iT = 0; iT < fNAngularResponseBins; iT++ ){
    angle = ( ( 0.5 + iT ) * 90.0 ) / fNAngularResponseBins;
    if ( angle < 36.0 ){     
      fMrqParameters[ GetAngularResponseParIndex() + iT ] = fAngularResponseInit + ( 0.002222 * angle );
    }

    else{
      fMrqParameters[ GetAngularResponseParIndex() + iT ] = fAngularResponseInit;
    }
  }

  // The bin values for the laserball distribution histogram
  for ( Int_t iT = 0; iT < ( fNLBDistributionThetaBins * fNLBDistributionPhiBins ); iT++ ){
    fMrqParameters[ GetLBDistributionParIndex() + iT ] = fLBDistributionInit;   
  }

}



//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetAngularResponseParIndex()
{
  return 4;
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBDistributionParIndex()
{
  return ( 4 + fNAngularResponseBins );
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::DataScreen()
{

  Int_t iPMTAngleValid;
  Float_t angle;
  Int_t tmpRun, tmpPMT, iX;

  Float_t pmtResidual, pmtChiSquared, pmtSigma;
  Float_t pmtModelPrediction, pmtData;
  Float_t pmtOccRatio;

  Float_t occVal, dcOccValMean, dcSigma;

  Int_t *pmtAngleValid = new Int_t[ fNAngularResponseBins ];
  for (Int_t i = 0; i < fNAngularResponseBins; i++ ){
    pmtAngleValid[ i ] = 0;   
  }

  for ( Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    
    fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ iRun ] );
    fNDataPointsInFit += fCurrentRun->GetNPMTs();
    std::map< Int_t, LOCASPMT >::iterator iPMT;

    // Obtain the mean occRatio for the run
    for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); ++iPMT ){      
      occVal = ( ( iPMT->second ).GetMPECorrOccupancy() / ( iPMT->second ).GetCentralMPECorrOccupancy() );
      dcOccValMean += occVal;
    }

    if( fCurrentRun->GetNPMTs() != 0 ){
      dcOccValMean /= ( (Float_t)fCurrentRun->GetNPMTs() );
    }

    // Obtain the sigma occRatio for the run
    for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); ++iPMT ){      
      occVal = ( ( iPMT->second ).GetMPECorrOccupancy() / ( iPMT->second ).GetCentralMPECorrOccupancy() );
      dcSigma += TMath::Power( (occVal - dcOccValMean), 2 );
    }

    if( fCurrentRun->GetNPMTs() != 0 ){
      Float_t tmpDCSigma = dcSigma;
      dcSigma = TMath::Sqrt( tmpDCSigma / ( (Float_t)fCurrentRun->GetNPMTs() ) );
    }

    // Screen PMTs for bad tubes
    for ( Int_t iPMT = 0; iPMT < fCurrentRun->GetNPMTs(); iPMT++ ){ 
      fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );
      Bool_t skipPMT = PMTSkip( iRun, iPMT, dcOccValMean, dcSigma );

      if( !skipPMT ){
	cout << "Passed Boolean COndition" << endl;
	pmtModelPrediction = ModelPrediction( iRun, iPMT );
	Float_t dataResult = ( fCurrentPMT->GetMPECorrOccupancy() ) / ( fCurrentPMT->GetCentralMPECorrOccupancy() );
	cout << "Model Prediction is: " << pmtModelPrediction << endl;
	cout << "Data is: N off-axis / N axis: " << ( fCurrentPMT->GetMPECorrOccupancy() ) << " / " << fCurrentPMT->GetCentralMPECorrOccupancy() << endl;
	cout << "Data is: " << ( fCurrentPMT->GetMPECorrOccupancy() ) / ( fCurrentPMT->GetCentralMPECorrOccupancy() ) << endl;
	cout << "##########" << endl;
	pmtResidual = ( pmtModelPrediction - dataResult );
	LOCASMath lMath;

	pmtSigma = lMath.OccRatioErr( fCurrentPMT );

	// Note: Not including PMT variability sigma in here, this was in LOCAS (SNO)
	// but will probably have changed for SNO+ so will need recomputing
	cout << "pmtSigma is: " << pmtSigma << endl;
	cout << "pmtModelPrediction is: " << pmtModelPrediction << endl;
	pmtChiSquared = ( pmtResidual * pmtResidual ) / ( pmtSigma * pmtSigma );

	fChiSquare += pmtChiSquared;
	cout << "Here we are" << endl;
	if ( fNPMTsInFit >= 0 && fNPMTsInFit < ( fNElements - 1 ) ){
	  cout << "Here we are1" << endl;
	  fChiArray[ fNPMTsInFit + 1 ] = pmtChiSquared;
	  cout << "Here we are2" << endl;
	  fResArray[ fNPMTsInFit + 1 ] = pmtSigma;
	  cout << "Here we ar3" << endl;
	}
	
	fMrqX[ fNPMTsInFit + 1 ] = (Int_t)( ( fCurrentPMT->GetID() ) + ( iRun * 10000 ) );         // 'Special' Indexing Scheme
	fMrqY[ fNPMTsInFit + 1 ] = ( ( fCurrentPMT->GetMPECorrOccupancy() ) / ( fCurrentPMT->GetCentralMPECorrOccupancy() ) );
	fMrqSigma[ fNPMTsInFit + 1 ] = pmtSigma;

	fNPMTsInFit++;
	
      }

    }

  }

  Int_t jVar = 0;
  for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){

    iX = (Int_t)fMrqX[ iPMT ];
    tmpRun = (Int_t)( iX / 10000 );
    tmpPMT = (Int_t)( iX % 10000 );
    fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ tmpRun ] );
    fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );

    if ( fChiArray[ iPMT ] < fChiSquareMaxLimit ){

      fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
      fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
      fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
      jVar++;

    }

  }

  fNPMTsInFit = jVar;
  Int_t nLBDistBins = fNLBDistributionPhiBins * fNLBDistributionThetaBins;
  Int_t iAngValid, iLBValid;
  Float_t pmtR, pmtLB;

  Int_t* lbValid = new Int_t[ nLBDistBins ];
  for ( Int_t i = 0; i < nLBDistBins; i++ ) lbValid[ i ] = 0;

  jVar = 0;
  for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){

    iX = (Int_t)fMrqX[ iPMT ];
    tmpRun = (Int_t)( iX / 10000 );
    tmpPMT = (Int_t)( iX % 10000 );
    fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ tmpRun ] );
    fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );

    pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );
    pmtAngleValid[ iAngValid ]++;
    pmtLB = ModelLBDistribution( fCurrentPMT, iLBValid, 0 );
    lbValid[ iLBValid ]++;

    fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
    fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
    fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
    jVar++;
    

  }

  fNPMTsInFit = jVar;

  // LB Distribution
  Int_t lbBinsInvalid = 0;
  for ( Int_t iBin = 0; iBin < nLBDistBins; iBin++ ){
    if ( lbValid[ iBin ] < fNPMTsPerLBDistributionBinMin ){
      fMrqParameters[ GetLBDistributionParIndex() + iBin ] = 1.0;
      fMrqVary[ GetLBDistributionParIndex() + iBin ] = 0;
      if( lbValid[ iBin ] > 0 ){
	lbBinsInvalid++;
      }
    }
  }

  Int_t anglesInvalid = 0;
  Int_t auxAngleValid = 0;
  for ( Int_t iBin = 1; iBin < fNAngularResponseBins; iBin++ ){
    if ( pmtAngleValid[ iBin ] < fNPMTsPerAngularResponseBinMin ){
      fMrqParameters[ GetAngularResponseParIndex() + iBin ] = 1.0;
      fMrqVary[ GetAngularResponseParIndex() + iBin ] = 0;
      if ( pmtAngleValid[ iBin ] > 0 ){
	anglesInvalid++;
      }
    }
  }

  if ( lbBinsInvalid == 0 && anglesInvalid == 0 ){


    jVar = 0;
    for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
      
      iX = (Int_t)fMrqX[ iPMT ];
      tmpRun = (Int_t)( iX / 10000 );
      tmpPMT = (Int_t)( iX % 10000 );
      fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ tmpRun ] );
      fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );

      pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );
      pmtLB = ModelLBDistribution( fCurrentPMT, iLBValid, 0 );
      auxAngleValid = pmtAngleValid[ iAngValid ];

      if ( lbValid[ iLBValid ] >= fNPMTsPerLBDistributionBinMin && auxAngleValid >= fNPMTsPerAngularResponseBinMin ){

	fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
	fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
	fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
	jVar++;
      
      }
    }
  }      
    

  fNPMTsInFit = jVar;


  if ( fNPMTsInFit > 0 ) fDataScreen = true;
  else fDataScreen = false;

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFit::PMTSkip( Int_t iRun, Int_t iPMT, Float_t mean, Float_t sigma )
{

  Bool_t pmtSkip = false;
  fCurrentPMT = &( ( fRunReader.GetLOCASRun( fListOfRunIDs[ iRun ] ) )->GetPMT( iPMT ) );

  LOCASMath lMath;
  Float_t occVal = ( fCurrentPMT->GetMPECorrOccupancy() ) - ( fCurrentPMT->GetCentralMPECorrOccupancy() );
  Float_t occValErr = lMath.OccRatioErr( fCurrentPMT ); 

  //Float_t pmtChiSq = CalculatePMTChiSquare( iRun, iPMT );
  
  if ( fCurrentPMT->GetAVHDShadowVal() > fAVHDShadowingMax
       || fCurrentPMT->GetAVHDShadowVal() < fAVHDShadowingMin
       || fCurrentPMT->GetAVHDShadowVal() > fGeoShadowingMax
       || fCurrentPMT->GetAVHDShadowVal() < fGeoShadowingMin ){
    pmtSkip = true;
  }

  
  return pmtSkip;
    
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelAngularResponse( LOCASPMT* pmt, Int_t& iAng, Int_t runType )
{

  Float_t cosTheta = 1.0;
  if ( runType == 0 ) cosTheta = fCurrentPMT->GetCosTheta();
  if ( runType == 1 ) cosTheta = fCurrentPMT->GetCentralCosTheta();
  if ( runType == 2 ) cosTheta = fCurrentPMT->GetWavelengthCosTheta();
  
  Float_t angle = ( TMath::ACos( cosTheta ) ) * ( 180.0 / TMath::Pi() );
  iAng = (Int_t)( angle * fNAngularResponseBins / 90.0 );
  
  return GetAngularResponsePar( iAng );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistribution( LOCASPMT* locasPMT, Int_t& iLBDist, Int_t runType )
{

  LOCASRun* locasRun = fRunReader.GetLOCASRun( locasPMT->GetRunID() );
  Float_t lbTheta, lbPhi, lbRelTheta, lbRelPhi;

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  TVector3 pmtRelVec( 0.0, 0.0, 0.0 );

  if ( runType == 0 ){
    lbTheta = locasRun->GetLBTheta();
    lbPhi = locasRun->GetLBPhi();
    lbRelTheta = locasPMT->GetRelLBTheta();
    lbRelPhi = locasPMT->GetRelLBPhi();
  }

  if ( runType == 1 ){
    lbTheta = locasRun->GetCentralLBTheta();
    lbPhi = locasRun->GetCentralLBPhi();
    lbRelTheta = locasPMT->GetCentralRelLBTheta();
    lbRelPhi = locasPMT->GetCentralRelLBPhi();
  }

  if ( runType == 2 ){
    lbTheta = locasRun->GetWavelengthLBTheta();
    lbPhi = locasRun->GetWavelengthLBPhi();
    lbRelTheta = locasPMT->GetWavelengthRelLBTheta();
    lbRelPhi = locasPMT->GetWavelengthRelLBPhi();
  }

  lbAxis.SetTheta( lbTheta );
  lbAxis.SetPhi( lbPhi );

  pmtRelVec.SetTheta( lbRelTheta );
  pmtRelVec.SetPhi( lbRelPhi );

  Float_t cosTheta = lbAxis * pmtRelVec;
  Float_t phi = 0.0;
  
  Int_t iTheta = (Int_t)( ( 1 + cosTheta ) / 2 * fNLBDistributionThetaBins );
  Int_t iPhi = (Int_t)( phi / ( 2 * TMath::Pi() ) * fNLBDistributionPhiBins );

  iLBDist = iTheta * fNLBDistributionPhiBins + iPhi;
  return GetLBDistributionPar( iLBDist );

}

//////////////////////////////////////
//////////////////////////////////////

// void LOCASFit::Screen()
// {

//   // Start at one to match to Mrq working arrays
//   Int_t iFitPMT = 1;
//   for (Int_t iRun = 0; iRun < fNRuns; iRun++ ){

//     fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ iRun ] );
//     Int_t nPMTs = fCurrentRun->GetNPMTs();
//     std::map< Int_t, LOCASPMT >::iterator iPMT;
//     Float_t meanOcc, occSigma;

//     // Calculate the mean PMT occupancy
//     for( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); iPMT++ ){
//       fCurrentPMT = &( iPMT->second );
//       meanOcc += fCurrentPMT->GetOccRatio();
//     }
//     meanOcc /= (Float_t)nPMTs;

//     // Calculate s.d. on the PMT occupancy
//     for( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); iPMT++ ){
//       fCurrentPMT = &( iPMT->second );
//       occSigma += TMath::Power( ( fCurrentPMT->GetOccRatio() - meanOcc ), 2 );
//     }
//     occSigma = TMath::Sqrt( occSigma / (Float_t)nPMTs );

//     // Screen each PMT to see if it should be included in the fit
//     for( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); iPMT++ ){
//       fCurrentPMT = &( iPMT->second );
//       if( !PMTSkip( fCurrentPMT, meanOcc, occSigma ) ){
// 	fFitPMTs[ iFitPMT ] = ( iPMT->second );
// 	fMrqX[ iFitPMT ] = iFitPMT;
// 	fMrqY[ iFitPMT ] = ( fCurrentPMT->GetOccRatio() );
// 	fMrqSigma[ iFitPMT ] = ( fCurrentPMT->GetOccRatioErr() );
// 	iFitPMT++;
//       }      
//     }
//   }
//   fNPMTsInFit = fFitPMTs.size();

// }

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTChiSquare( Int_t iRun, Int_t iPMT )
{
  Float_t chiSq = 0.0;

  Float_t modelPred = ModelPrediction( iRun, iPMT );
  LOCASMath lMath;
  Float_t occVal = ( fCurrentPMT->GetMPECorrOccupancy() ) - ( fCurrentPMT->GetCentralMPECorrOccupancy() );
  Float_t occValErr = lMath.OccRatioErr( fCurrentPMT );
  chiSq = ( ( modelPred - occVal ) * ( modelPred - occVal ) ) / ( occValErr * occValErr );

  return chiSq;
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

    if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lamda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  
  lamda = 0;
  mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lamda );
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
  LOCASMath lMath;


  //--------------------
  // Initialization  
  if (*alamda < 0.0) {
    atry=lMath.LOCASVector(1,ma);
    beta=lMath.LOCASVector(1,ma);
    da=lMath.LOCASVector(1,ma);

    for (mfit=0,j=1;j<=ma;j++){
      if (ia[j]) {
	mfit++; 

      }
    }

    oneda=lMath.LOCASMatrix(1,mfit,1,1);
    *alamda=0.001;
    mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq);
    ochisq=(*chisq);
    for (j=1;j<=ma;j++) atry[j]=a[j];
  }

  //--------------------
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

	    } else {

	    }
	  }
    }
  }


  retval = gaussj(covar,mfit,oneda,1);


  for (j=1;j<=mfit;j++) da[j]=oneda[j][1];

  //--------------------
  // Final call to prepare covariance matrix and deallocate memory.
  if (*alamda == 0.0 ) {
    covsrt(covar,ma,ia,mfit);
    lMath.LOCASFree_Matrix(oneda,1,mfit,1,1);
    lMath.LOCASFree_Vector(da,1,ma);
    lMath.LOCASFree_Vector(beta,1,ma);
    lMath.LOCASFree_Vector(atry,1,ma);
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
      for (i=1;i<=ma;i++) { SWAP(covar[i][k],covar[i][j]) }
      for (i=1;i<=ma;i++) { SWAP(covar[k][i],covar[j][i]) }
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

  LOCASMath lMath;
  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  float chisqentry;  // chi-squared for single entry in list
  
  float *beta2 = lMath.LOCASVector(1,ma);        // 10-Mar-2001 - Debugging checks - Moffat
  float **alpha2 = lMath.LOCASMatrix(1,ma,1,ma);
  
  dyda=lMath.LOCASVector(1,ma);
  for (j=1;j<=ma;j++) {
    if (ia[j]) mfit++;
    dyda[j] = 0.0;
  }
  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) {
      alpha[j][k]=0.0;
      alpha2[j][k]=0.0; // Debugging - 10-Mar-2001 - Moffat
    }
    beta[j]=0.0;
    beta2[j]=0.0; // Debugging - 10-Mar-2001 - Moffat
  }
  *chisq=0.0;
  for (i=1;i<=ndata;i++) {  // Skip some tubes to increase speed...
    mrqfuncs( x[i],i,a,&ymod,dyda,ma );
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;
    
    //........................................
    // Loop over all variable parameters is unnecessary in QOCAFit.
    // We only calculate the derivative for 3 attenuations, the LB distribution
    // (<=2 pars), LB mask (<=2 pars) and normalization (1 par) so the loop
    // should be replaced with explicit summations over these derivatives only.
    // 8-Mar-2001 - Bryce Moffat
    //........................................
    FillParameterPoint();
    for (l=1; l<=fParam; l++) {
      //		if(dyda[fparindex[l]]==0.0) printf("mrqcofNull l %d index %d\n",l,fparindex[l]);	
      
      wt = dyda[fParamIndex[l]] * sig2i;
      for (m=1; m<=l; m++) {
    	j = fParamIndex[l];
    	k = fParamIndex[m];
    	if (k<=j) alpha[fParamVarMap[j]][fParamVarMap[k]] += wt * dyda[k];
	beta[fParamVarMap[fParamIndex[l]]] += dy * wt;
      }
      //........................................
      // if (fRepeatoldmrqcof) {
      for (j=0,l=1;l<=ma;l++) {  // Original Numerical recipes code
	if (ia[l]) {
	  wt=dyda[l]*sig2i;
	  for (j++,k=0,m=1;m<=l;m++)
	    if (ia[m]) alpha2[j][++k] += wt*dyda[m];
	  beta2[j] += dy*wt;
	}
      }
    }
    // }
    //........................................
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    // if (fPrint>=3) if (i%1==2000) printf(" %d %f %f\n",i,x[i],chisqentry);
    // if (fChiarray!=NULL && i>=0 && i<fNelements) fChiarray[i] = chisqentry;
    // if (fResarray!=NULL && i>=0 && i<fNelements) fResarray[i] = dy;
  }
  for (j=2;j<=mfit;j++){
    for (k=1;k<j;k++) {
      alpha[k][j]=alpha[j][k];
      // if (fRepeatoldmrqcof) {
      // 	alpha2[k][j]=alpha2[j][k];
      // 	if (alpha[k][j] != alpha2[k][j]) {
      // 	}
      // }
    }
  }
  
  lMath.LOCASFree_Vector(dyda,1,ma);
  
  lMath.LOCASFree_Matrix(alpha2,1,ma,1,ma);  // 10-Mar-2001 - Debugging checks - Moffat
  lMath.LOCASFree_Vector(beta2,1,ma);
}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::mrqfuncs(Float_t x,Int_t ix,Float_t a[],Float_t *y,
			Float_t dyda[],Int_t na)
{
  //Function used by mrqcof()
  //   a[]    = parameters to be fit:
  //              d2o, acrylic, h2o attenuation lengths,
  //              PMT angular response,
  //              laserball mask,
  //              laserball distribution,
  //              run normalizations
  //   y      = modelled value based on input parameters = Nij expected for given PMT
  //   dyda[] = derivative of y w.r.t. input parameters
  //   na     = number of parameters
  //
  // For central runs, use the QOCARun::GetNorm() stored by QPath::Calculate(),
  // which is just the simple average.
  //
  
  ix = (Int_t)x;
  Int_t jpmt = ix%10000;
  Int_t irun = ix/10000;

  Float_t *mrqparsave =   fMrqParameters; // Save parameters and use the ones just passed
  fMrqParameters = a;

  *y = ModelPrediction( irun, jpmt, na, dyda );  // Derivatives also calculated!
  
  fMrqParameters = mrqparsave; // Restore parameters
  
}


/////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelPrediction( Int_t iRun, Int_t iPMT, Int_t nA, Float_t* dyda )
{
  Int_t i;
  Int_t arPari;
  Bool_t derivatives = false;
  if( nA > 0 && dyda ){
    derivatives = true;
    dyda[ GetAngularResponseParIndex() + fiAng ] = 0;
    dyda[ GetAngularResponseParIndex() + fCiAng ] = 0;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = 0;
    dyda[ GetLBDistributionParIndex() + fCiLBDist ] = 0;
  }
  else derivatives = false;
  fCurrentRun = fRunReader.GetLOCASRun( iRun );
  fCurrentPMT = &( fRunReader.GetLOCASRun( iRun ) )->GetPMT( iPMT );
  
  arPari = GetAngularResponseParIndex();

  fiAng = 0;
  fiLBDist = 0;
  Float_t dScint = ( fCurrentPMT->GetDistInScint() ) - ( fCurrentPMT->GetCentralDistInScint() );
  Float_t dAV = ( fCurrentPMT->GetDistInAV() ) - ( fCurrentPMT->GetCentralDistInAV() );
  Float_t dWater = ( fCurrentPMT->GetDistInWater() ) - ( fCurrentPMT->GetCentralDistInWater() );

  Float_t corrSolidAngle = ( fCurrentPMT->GetSolidAngle() ) / ( fCurrentPMT->GetSolidAngle() );
  Float_t corrFresnelTCoeff = ( fCurrentPMT->GetFresnelTCoeff() ) / ( fCurrentPMT->GetFresnelTCoeff() );

  Float_t angResp = ModelAngularResponse( fCurrentPMT, fiAng, 0 );
  Float_t intensity = ModelLBDistribution( fCurrentPMT, fiLBDist, 0 );

  Float_t pmtResponse = angResp * intensity * corrSolidAngle * corrFresnelTCoeff * TMath::Exp( - ( dScint * GetScintPar() )
											       - ( dAV * GetAVPar() )
											       - ( dWater * GetWaterPar() ) );
 
  if( derivatives ){

    dyda[ GetScintParIndex() ] = -dScint;
    dyda[ GetAVParIndex() ] = -dAV;
    dyda[ GetWaterParIndex() ] = -dWater;
    dyda[ arPari + fiAng ] = +1.0 / angResp;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = +1.0 / intensity;

  }

  fCiAng = 0;
  fCiLBDist = 0;
  Float_t angRespCtr = ModelAngularResponse( fCurrentPMT, fCiAng, 1 );

  Float_t intensityCtr = ModelLBDistribution( fCurrentPMT, fCiLBDist, 1 );


  Float_t pmtResponseCtr = angRespCtr * intensityCtr;

  if( derivatives ){
    dyda[ arPari + fCiAng ] -= 1.0 / angRespCtr;
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

/////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::gaussj(float **a, int n, float **b, int m)
{
  // Gauss-Jordan matrix solution helper routine for mrqmin.
  LOCASMath lMath;

  int *indxc,*indxr,*ipiv;
  int i,icol,irow,j,k,l,ll;
  float big,dum,pivinv,swap;
  Int_t retval = 0;
  indxc=lMath.LOCASIntVector(1,n);
  indxr=lMath.LOCASIntVector(1,n);
  ipiv=lMath.LOCASIntVector(1,n);
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
	      //nrerror("gaussj: Singular Matrix-1");
	      cout << "Singular Matrix-1" << endl;
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
	//nrerror("gaussj: Singular Matrix-2");
	cout << "Singular Matrix-2" << endl;
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
  
  lMath.LOCASFree_IntVector(ipiv,1,n);
  lMath.LOCASFree_IntVector(indxr,1,n);
  lMath.LOCASFree_IntVector(indxc,1,n);
  return retval;
}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PerformFit( )
{


  FillParameterbase();
  FillAngIndex();

  MrqFit(fMrqX, fMrqY, fMrqSigma, fNPMTsInFit, fMrqParameters, fMrqVary, fNParametersInFit, fMrqCovariance,
	 fMrqAlpha, &fChiSquare);

  cout << "Example parameters:" << endl;
  cout << "Scintillator: " << fMrqParameters[1] << endl;
  cout << "Acrylic: " << fMrqParameters[2] << endl;
  cout << "Water: " << fMrqParameters[3] << endl;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillParameterbase( )
{

  if ( fParamIndex ) delete[] fParamIndex;
  fParamIndex = new Int_t[ fNParametersInFit ];           // The number of unique parameters is guaranteed to be less than 
                                                          // the number of unique parameters

  fParamBase = 0;
  if( GetScintVary() ) fParamIndex[ ++fParamBase ] = GetScintParIndex();
  if( GetAVVary() ) fParamIndex[ ++fParamBase ] = GetAVParIndex();
  if( GetWaterVary() ) fParamIndex[ ++fParamBase ] = GetWaterParIndex();

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillAngIndex( )
{

  if ( fParamVarMap ) delete[] fParamVarMap;
  fParamVarMap = new Int_t[ fNParametersInFit + 1 ];

  Int_t i, j;

  j = 0;
  for ( i = 1; i <= fNParametersInFit; i++ ) if ( fMrqVary[ i ] ) fParamVarMap[ i ] = ++j;

  if ( fAngIndex ){
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
      else { first = j; second =i; }
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
      fParamIndex[ ++fParam] = parnum;
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

}

