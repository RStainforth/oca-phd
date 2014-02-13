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

#include "TFile.h"

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
    fNDataPointsInFit += ( fRunReader.GetRunEntry( iRun ) )->GetNPMTs();
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
  cout << "HELLO!" << endl;

  fAngularResponseVary = lDB.GetBoolField( "FITFILE", "ang_resp_vary" );
  fAngularResponseInit = lDB.GetDoubleField( "FITFILE", "ang_resp_init" );
  fLBDistributionVary = lDB.GetBoolField( "FITFILE", "lb_dist_vary" );
  fLBDistributionInit = lDB.GetDoubleField( "FITFILE", "lb_dist_init" );
  cout << "HELLO1" << endl;

  // Get the number of theta and phi bins for the laserball distribution 2D historgram
  // and the minimum number of entires required for each bin
  fNLBDistributionThetaBins = lDB.GetIntField( "FITFILE", "lb_dist_n_theta_bins" );
  fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", "lb_dist_n_phi_bins" );
  fNPMTsPerLBDistributionBinMin = lDB.GetIntField( "FITFILE", "lb_dist_min_n_pmts" );

  // Get the number of theta bins for the PMT angular response histogram
  // and the minimum number of entries for each bin
  fNAngularResponseBins = lDB.GetIntField( "FITFILE", "ang_resp_n_bins" );
  fNPMTsPerAngularResponseBinMin = lDB.GetIntField( "FITFILE", "ang_resp_min_n_pmts" );
  cout << "HELLO2" << endl;

  // Get the cut variables (i.e. the variables to exclude PMTs from the fit with)
  // each PMT is cut on the below criteria in LOCASFit::PMTSkip.

  // Maximum initial Chi-Square
  fChiSquareMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_chisq_max" );
  // Minimumm initial Chi-Square
  fChiSquareMinLimit = lDB.GetDoubleField( "FITFILE", "cut_chisq_min" );
  // Number of sigma away from mean occupancy for entire run
  fNSigma = lDB.GetDoubleField( "FITFILE", "cut_n_sigma" );
  fNChiSquare = lDB.GetDoubleField( "FITFILE", "cut_n_chisquare" );
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
  cout << "HELLO3" << endl;
  fNPMTSkip = lDB.GetIntField( "FITFILE", "n_pmts_skip" );
  AllocateParameters();
  PrintInitialisationInfo();

  
  fParamIndex = NULL;
  fParamVarMap = NULL;
  fAngIndex = NULL;

  fNPMTsInFit = 0;

  fChiSquare = 0;

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
  Int_t tmpRun, tmpPMT, iX, nPMTs;

  Float_t pmtResidual, pmtChiSquared, pmtSigma, pmtData;

  Float_t occValErr, occVal, dcOccValMean, dcSigma;

  Int_t counter = 0;

  for ( Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    counter = 0;
    cout << "Doing Run: " << iRun + 1 << " of " << fNRuns << endl;
    fCurrentRun = fRunReader.GetRunEntry( iRun );
    cout << "Run ID is: " << fCurrentRun->GetRunID() << endl;
    nPMTs = fCurrentRun->GetNPMTs();

    // Obtain the mean occRatio for the run
    for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); fiPMT++ ){      
      occVal = ( ( fiPMT->second ).GetMPECorrOccupancy() / ( fiPMT->second ).GetCentralMPECorrOccupancy() );
      dcOccValMean += occVal;
    }
    if( nPMTs != 0 ){ dcOccValMean /= nPMTs; }

    // Obtain the sigma occRatio for the run
    for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); fiPMT++ ){      
      occVal = ( ( fiPMT->second ).GetMPECorrOccupancy() / ( fiPMT->second ).GetCentralMPECorrOccupancy() );
      dcSigma += TMath::Power( (occVal - dcOccValMean), 2 );
    }
    if( nPMTs != 0 ){ dcSigma = TMath::Sqrt( dcSigma / nPMTs ); }

    // Screen PMTs for bad tubes
    for ( Int_t iPMT = 0; iPMT < fCurrentRun->GetNPMTs(); iPMT = iPMT++){
      fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );
      counter++;
      Double_t nPMTsToCheck = (Double_t)( fCurrentRun->GetNPMTs() / 1 );
      if ( counter % 50 == 0 ){ 
	cout << (Double_t)( counter / nPMTsToCheck ) * 100.0 << " % of run " << iRun + 1 << " of " << fNRuns << " complete." << endl; 
      }
      
      Bool_t skipPMT = PMTSkip( fCurrentRun, fCurrentPMT, dcOccValMean, dcSigma );
      if( !skipPMT ){
	
        // Note: Not including PMT variability sigma in here, this was in LOCAS (SNO)
        // but will probably have changed for SNO+ so will need recomputing
        pmtChiSquared = CalculatePMTChiSquare( fCurrentRun, fCurrentPMT );
	pmtSigma = CalculatePMTSigma( fCurrentRun, fCurrentPMT );
	pmtData = CalculatePMTData( fCurrentRun, fCurrentPMT );
		
        if ( fNPMTsInFit >= 0 && fNPMTsInFit < ( fNElements - 1 ) ){	  
	  fChiArray[ fNPMTsInFit + 1 ] = pmtChiSquared;
	  fResArray[ fNPMTsInFit + 1 ] = pmtSigma;	  
	}
	
	fMrqX[ fNPMTsInFit + 1 ] = (Float_t)( ( iPMT ) + ( iRun * 10000 ) );         // 'Special' Indexing Scheme
	fMrqY[ fNPMTsInFit + 1 ] = pmtData;
	fMrqSigma[ fNPMTsInFit + 1 ] = pmtSigma;

	fNPMTsInFit++;
	
      }
    }
  }

  cout << "Number of PMTs in Fit is: " << fNPMTsInFit << endl;

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

  fNPMTsInFit = jVar;
  cout << "Number of PMTs in Fit is: " << fNPMTsInFit << endl;

  Float_t redChiSq = (Double_t)fChiSquare / ( fNPMTsInFit - 3 );
  cout << "redChiSq is: " << redChiSq << endl;

  jVar = 0;
  for ( Int_t iK = 1; iK < fNPMTsInFit; iK++ ){
    
    if ( fChiArray[ iK ]  < redChiSq * fNChiSquare 
	 && fChiArray[ iK ] > fChiSquareMinLimit ){

      fMrqX[ jVar + 1 ] = fMrqX[ iK ];
      fMrqY[ jVar + 1 ] = fMrqY[ iK ];
      fMrqSigma[ jVar + 1 ] = fMrqSigma[ iK ];

      fChiArray[ jVar + 1 ] = fChiArray[ iK ];
      fResArray[ jVar + 1 ] = fResArray[ iK ];
    
      jVar++;
    }
    
  }

  fNPMTsInFit = jVar;
  cout << "Number of PMTs in Fit is: " << fNPMTsInFit << endl;

  fChiSquare = 0.0;
  cout << "fChiSquare is: " << fChiSquare << endl;
  for ( Int_t iP = 1; iP < fNPMTsInFit; iP++ ){
    fChiSquare += (fChiArray[ iP ]);
    // cout << "+= " << fChiArray[ iP ] << endl;
    // cout << fChiSquare << endl;
    // cout << "---------" << endl;
  }
  cout << "fChiSquare is now: " << fChiSquare << endl;
  Float_t redChiSq1 = (Double_t)fChiSquare / ( fNPMTsInFit - 3 );
  cout << "redChiSq is: " << redChiSq1 << endl;
  

  Int_t nLBDistBins = fNLBDistributionPhiBins * fNLBDistributionThetaBins;
  Int_t iAngValid, iLBValid;
  Float_t pmtR, pmtLB;

  Int_t* lbValid = new Int_t[ nLBDistBins ];
  for ( Int_t i = 0; i < nLBDistBins; i++ ){ lbValid[ i ] = 0; }

  Int_t *pmtAngleValid = new Int_t[ fNAngularResponseBins ];
  for (Int_t i = 0; i < fNAngularResponseBins; i++ ){ pmtAngleValid[ i ] = 0; }
  
  for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
    
    //cout << "On PMT: " << iPMT << " of " << fNPMTsInFit << endl;
    iX = (Int_t)fMrqX[ iPMT ];
    tmpRun = (Int_t)( iX / 10000 );
    tmpPMT = (Int_t)( iX % 10000 );
    
    fCurrentRun = fRunReader.GetRunEntry( tmpRun );
    fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );
    
    pmtR = ModelAngularResponse( fCurrentRun, fCurrentPMT, iAngValid, 0 );
    pmtAngleValid[ iAngValid ]++;
    
    pmtLB = ModelLBDistribution( fCurrentRun, fCurrentPMT, iLBValid, 0 );
    lbValid[ iLBValid ]++;
    
  }
  
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

  for ( Int_t iBin = 0; iBin < fNAngularResponseBins; iBin++ ){

    if ( pmtAngleValid[ iBin ] < fNPMTsPerAngularResponseBinMin ){

      fMrqParameters[ GetAngularResponseParIndex() + iBin ] = 1.0;
      fMrqVary[ GetAngularResponseParIndex() + iBin ] = 0;

      if ( pmtAngleValid[ iBin ] > 0 ){
        anglesInvalid++;
      }
    }
  }
  // if ( lbBinsInvalid == 0 && anglesInvalid == 0 ){

  //   jVar = 0;
  //   for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
      
  //     iX = (Int_t)fMrqX[ iPMT ];
  //     tmpRun = (Int_t)( iX / 10000 );
  //     tmpPMT = (Int_t)( iX % 10000 );
  //     fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ tmpRun ] );
  //     fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );

  //     pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );
  //     pmtLB = ModelLBDistribution( fCurrentPMT, iLBValid, 0 );
  //     auxAngleValid = pmtAngleValid[ iAngValid ];

  //     if ( lbValid[ iLBValid ] >= fNPMTsPerLBDistributionBinMin && auxAngleValid >= fNPMTsPerAngularResponseBinMin ){
        
  //       fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
  //       fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
  //       fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
  //       jVar++;
      
  //     }
  //   }
  // }


  if ( fNPMTsInFit > 0 ) fDataScreen = true;
  else fDataScreen = false;

  cout << "Number of PMTs in Fit is: " << fNPMTsInFit << endl;

  for ( Int_t iPM = 1; iPM < fNPMTsInFit; iPM++ ){
    Int_t iX = fMrqX[ iPM ];
    Int_t pmtID = iX % 10000;
    Int_t runN = iX / 10000;
    LOCASRun* lRun = fRunReader.GetRunEntry( runN );
    LOCASPMT lPmt = lRun->GetPMT( pmtID );
    fFitPMTs[ pmtID ] = lPmt;
  }

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFit::PMTSkip( const Int_t iRun, const Int_t iPMT, Float_t mean, Float_t sigma )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );

  Bool_t pmtSkip = false;

  Float_t occVal = ( fCurrentPMT->GetMPECorrOccupancy() ) / ( fCurrentPMT->GetCentralMPECorrOccupancy() );
  Float_t occValErr = flMath.OccRatioErr( fCurrentPMT );

  if ( !fCurrentPMT->GetIsVerified() 
       || !fCurrentPMT->GetCentralIsVerified()
       || fCurrentPMT->GetCHSFlag() == fCHSFlag
       || fCurrentPMT->GetCSSFlag() == fCSSFlag
       || fCurrentPMT->GetCentralCHSFlag() == fCHSFlag
       || fCurrentPMT->GetCentralCSSFlag() == fCHSFlag
       || fCurrentPMT->GetAVHDShadowVal() > fAVHDShadowingMax
       || fCurrentPMT->GetAVHDShadowVal() < fAVHDShadowingMin
       || fCurrentPMT->GetGeometricShadowVal() > fGeoShadowingMax
       || fCurrentPMT->GetGeometricShadowVal() < fGeoShadowingMin
       || fCurrentPMT->GetCentralAVHDShadowVal() > fAVHDShadowingMax
       || fCurrentPMT->GetCentralAVHDShadowVal() < fAVHDShadowingMin
       || fCurrentPMT->GetCentralGeometricShadowVal() > fGeoShadowingMax
       || fCurrentPMT->GetCentralGeometricShadowVal() < fGeoShadowingMin
       || fCurrentPMT->GetMPECorrOccupancy() < fNOccupancy
       || fCurrentPMT->GetCentralMPECorrOccupancy() < fNOccupancy
       || ( occValErr / occVal ) > 0.25
       || (( occVal - mean ) / sigma) > fNSigma
       || occVal < 0.25 
       || occVal > 2.0 ){
    pmtSkip = true;
  }
  
  return pmtSkip;
    
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFit::PMTSkip( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Float_t mean, Float_t sigma )
{

  Bool_t pmtSkip = false;

  Float_t occVal = ( iPMTPtr->GetMPECorrOccupancy() ) / ( iPMTPtr->GetCentralMPECorrOccupancy() );
  Float_t occValErr = flMath.OccRatioErr( iPMTPtr );

  if ( !iPMTPtr->GetIsVerified() 
       || !iPMTPtr->GetCentralIsVerified()
       || iPMTPtr->GetCHSFlag() == fCHSFlag
       || iPMTPtr->GetCSSFlag() == fCSSFlag
       || iPMTPtr->GetCentralCHSFlag() == fCHSFlag
       || iPMTPtr->GetCentralCSSFlag() == fCHSFlag
       || iPMTPtr->GetAVHDShadowVal() > fAVHDShadowingMax
       || iPMTPtr->GetAVHDShadowVal() < fAVHDShadowingMin
       || iPMTPtr->GetGeometricShadowVal() > fGeoShadowingMax
       || iPMTPtr->GetGeometricShadowVal() < fGeoShadowingMin
       || iPMTPtr->GetCentralAVHDShadowVal() > fAVHDShadowingMax
       || iPMTPtr->GetCentralAVHDShadowVal() < fAVHDShadowingMin
       || iPMTPtr->GetCentralGeometricShadowVal() > fGeoShadowingMax
       || iPMTPtr->GetCentralGeometricShadowVal() < fGeoShadowingMin
       || iPMTPtr->GetMPECorrOccupancy() < fNOccupancy
       || iPMTPtr->GetCentralMPECorrOccupancy() < fNOccupancy
       || ( occValErr / occVal ) > 0.25
       || (( occVal - mean ) / sigma) > fNSigma ){
    pmtSkip = true;
  }
  
  return pmtSkip;
    
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelAngularResponse( const Int_t iRun, const Int_t iPMT, Int_t& iAng, Int_t runType )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );

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

Float_t LOCASFit::ModelAngularResponse( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Int_t& iAng, Int_t runType )
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

Float_t LOCASFit::ModelLBDistribution( const Int_t iRun, const Int_t iPMT, Int_t& iLBDist, Int_t runType )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );

  Float_t lbTheta, lbPhi, lbRelTheta, lbRelPhi;

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  TVector3 pmtRelVec( 0.0, 0.0, 0.0 );

  if ( runType == 0 ){
    lbTheta = fCurrentRun->GetLBTheta();
    lbPhi = fCurrentRun->GetLBPhi();
    lbRelTheta = fCurrentPMT->GetRelLBTheta();
    lbRelPhi = fCurrentPMT->GetRelLBPhi();
  }

  if ( runType == 1 ){
    lbTheta = fCurrentRun->GetCentralLBTheta();
    lbPhi = fCurrentRun->GetCentralLBPhi();
    lbRelTheta = fCurrentPMT->GetCentralRelLBTheta();
    lbRelPhi = fCurrentPMT->GetCentralRelLBPhi();
  }

  if ( runType == 2 ){
    lbTheta = fCurrentRun->GetWavelengthLBTheta();
    lbPhi = fCurrentRun->GetWavelengthLBPhi();
    lbRelTheta = fCurrentPMT->GetWavelengthRelLBTheta();
    lbRelPhi = fCurrentPMT->GetWavelengthRelLBPhi();
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

Float_t LOCASFit::CalculatePMTChiSquare( const Int_t iRun, const Int_t iPMT )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );

  Float_t modelPred = ModelPrediction( iRun, iPMT );
  Float_t occVal = CalculatePMTData( iRun, iPMT );
  Float_t occValErr = CalculatePMTData( iRun, iPMT );
  Float_t chiSq = ( ( modelPred - occVal ) * ( modelPred - occVal ) ) / ( occValErr * occValErr );

  return chiSq;
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTChiSquare( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Float_t modelPred = ModelPrediction( iRunPtr, iPMTPtr );
  Float_t occVal = CalculatePMTData( iRunPtr, iPMTPtr );
  Float_t occValErr = CalculatePMTData( iRunPtr, iPMTPtr );
  Float_t chiSq = ( ( modelPred - occVal ) * ( modelPred - occVal ) ) / ( occValErr * occValErr );

  return chiSq;
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTSigma( const Int_t iRun, const Int_t iPMT )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );
  Float_t pmtSigma = flMath.OccRatioErr( fCurrentPMT );
  return pmtSigma;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTSigma( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Float_t pmtSigma = flMath.OccRatioErr( iPMTPtr );
  return pmtSigma;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTData( const Int_t iRun, const Int_t iPMT )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );
  Float_t pmtData = ( fCurrentPMT->GetMPECorrOccupancy() / fCurrentPMT->GetCentralMPECorrOccupancy() );
  return pmtData;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTData( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Float_t pmtData = ( iPMTPtr->GetMPECorrOccupancy() / iPMTPtr->GetCentralMPECorrOccupancy() );
  return pmtData;

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
  cout << "(fabs(*chisq - oldchisq) > tol): " << (fabs(*chisq - oldchisq)) << " and tol is: " << tol << endl;
  cout << "gooditer < 4, gooditer is: " << gooditer << endl;
  cout << "(numiter < maxiter): " << "numiter is: " << numiter << " and maxiter is: " << maxiter << endl;
  cout << "retval == 0:, retval is: " << retval << endl;
  cout << "lamda != 0.0:, lamda is: " << lamda << endl;
  lamda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
         && retval == 0 && lamda != 0.0) {
    oldchisq = *chisq;
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda );
    numiter++;
    printf("New chisq = %12.2f with lambda %g \n", *chisq, lamda);
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


  //--------------------
  // Initialization  
  if (*alamda < 0.0) {
    atry=flMath.LOCASVector(1,ma);
    beta=flMath.LOCASVector(1,ma);
    da=flMath.LOCASVector(1,ma);

    for (mfit=0,j=1;j<=ma;j++){
      if (ia[j]) {
        mfit++; 
        
      }
    }

    cout << "Value of mfit is: " << mfit << endl;
    
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
	      cout << "*** Zero covariance diagonal element at j " << endl;
	    } else {
	      cout << "*** Negative covariance diagonal element at j %d (l %d)\n" << endl;
	    }
	  }
    }
  }

  //--------------------
  // Bookkeeping on covariance and derivatives to prepare next parameter set.
  // for (j=0,l=1;l<=ma;l++) {
  //   if (ia[l]) {
  //     for (j++,k=0,m=1;m<=ma;m++) {
  //       if (ia[m]) {
  //         k++;
  //         covar[j][k]=alpha[j][k];
  //       }
  //     }
  //     covar[j][j]=alpha[j][j]*(1.0+(*alamda));
  //     oneda[j][1]=beta[j];
  //     // if(covar[j][j] <= 0.0) {
  //     //   if(covar[j][j] == 0.0) {
          
  //     //   } else {
          
  //     //   }
  //     // }
  //   }
  // }

  cout << "About to Call gaussj" << endl;
  retval = gaussj(covar,mfit,oneda,1);

  
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

  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  float chisqentry;  // chi-squared for single entry in list
  
  float *beta2 = flMath.LOCASVector(1,ma);        // 10-Mar-2001 - Debugging checks - Moffat
  float **alpha2 = flMath.LOCASMatrix(1,ma,1,ma);
  
  dyda=flMath.LOCASVector(1,ma);
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
  for (i=1;i<=ndata;i+=fNPMTSkip) {  // Skip some tubes to increase speed...
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
      }
        beta[fParamVarMap[fParamIndex[l]]] += dy * wt;
    }
      
      //........................................
      //  if (fRepeatoldmrqcof) {
      // for (j=0,l=1;l<=ma;l++) {  // Original Numerical recipes code
      //   if (ia[l]) {
      //     wt=dyda[l]*sig2i;
      //     for (j++,k=0,m=1;m<=l;m++)
      //       if (ia[m]) alpha2[j][++k] += wt*dyda[m];
      //     beta2[j] += dy*wt;
      //   }
      // }
      //  }
    // }
    //........................................
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    // if (fPrint>=3) if (i%1==2000) printf(" %d %f %f\n",i,x[i],chisqentry);
    if (fChiArray!=NULL && i>=0 && i<fNElements) fChiArray[i] = chisqentry;
    if (fResArray!=NULL && i>=0 && i<fNElements) fResArray[i] = dy;
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
  
  flMath.LOCASFree_Vector(dyda,1,ma);
  
  flMath.LOCASFree_Matrix(alpha2,1,ma,1,ma);  // 10-Mar-2001 - Debugging checks - Moffat
  flMath.LOCASFree_Vector(beta2,1,ma);

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

  Float_t *mrqparsave = fMrqParameters; // Save parameters and use the ones just passed
  fMrqParameters = a;

  *y = ModelPrediction( irun, jpmt, na, dyda );  // Derivatives also calculated!
  
  fMrqParameters = mrqparsave; // Restore parameters
  
}


/////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelPrediction( const Int_t iRun, const Int_t iPMT, Int_t nA, Float_t* dyda )
{

  fCurrentRun = fRunReader.GetRunEntry( iRun );
  fCurrentPMT = &( fCurrentRun->GetPMT( iPMT ) );

  Bool_t derivatives = false;
  if( nA > 0 && dyda ){
    derivatives = true;
    dyda[ GetAngularResponseParIndex() + fiAng ] = 0;
    dyda[ GetAngularResponseParIndex() + fCiAng ] = 0;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = 0;
    dyda[ GetLBDistributionParIndex() + fCiLBDist ] = 0;
  }


  fiAng = 0;
  fiLBDist = 0;
  Float_t dScint = ( fCurrentPMT->GetDistInScint() ) - ( fCurrentPMT->GetCentralDistInScint() );
  Float_t dAV = ( fCurrentPMT->GetDistInAV() ) - ( fCurrentPMT->GetCentralDistInAV() );
  Float_t dWater = ( fCurrentPMT->GetDistInWater() ) - ( fCurrentPMT->GetCentralDistInWater() );

  Float_t corrSolidAngle = ( fCurrentPMT->GetSolidAngle() ) / ( fCurrentPMT->GetCentralSolidAngle() );
  Float_t corrFresnelTCoeff = ( fCurrentPMT->GetFresnelTCoeff() ) / ( fCurrentPMT->GetCentralFresnelTCoeff() );

  Float_t angResp = ModelAngularResponse( iRun, iPMT, fiAng, 0 );
  Float_t intensity = ModelLBDistribution( iRun, iPMT, fiLBDist, 0 );

  Float_t pmtResponse = angResp * intensity * corrSolidAngle * corrFresnelTCoeff * TMath::Exp( - ( dScint * GetScintPar() )
                                                                                               - ( dAV * GetAVPar() )
                                                                                               - ( dWater * GetWaterPar() ) );
  
  if( derivatives ){

    dyda[ GetScintParIndex() ] = -dScint;
    dyda[ GetAVParIndex() ] = -dAV;
    dyda[ GetWaterParIndex() ] = -dWater;
    dyda[ GetAngularResponseParIndex() + fiAng ] = +1.0 / angResp;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = +1.0 / intensity;

  }

  fCiAng = 0;
  fCiLBDist = 0;
  Float_t angRespCtr = ModelAngularResponse( iRun, iPMT, fCiAng, 1 );
  Float_t intensityCtr = ModelLBDistribution( iRun, iPMT, fCiLBDist, 1 );

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

/////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelPrediction( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Int_t nA, Float_t* dyda )
{

  Bool_t derivatives = false;
  if( nA > 0 && dyda ){
    derivatives = true;
    dyda[ GetAngularResponseParIndex() + fiAng ] = 0;
    dyda[ GetAngularResponseParIndex() + fCiAng ] = 0;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = 0;
    dyda[ GetLBDistributionParIndex() + fCiLBDist ] = 0;
  }


  fiAng = 0;
  fiLBDist = 0;
  Float_t dScint = ( iPMTPtr->GetDistInScint() ) - ( iPMTPtr->GetCentralDistInScint() );
  Float_t dAV = ( iPMTPtr->GetDistInAV() ) - ( iPMTPtr->GetCentralDistInAV() );
  Float_t dWater = ( iPMTPtr->GetDistInWater() ) - ( iPMTPtr->GetCentralDistInWater() );

  Float_t corrSolidAngle = ( iPMTPtr->GetSolidAngle() ) / ( iPMTPtr->GetCentralSolidAngle() );
  Float_t corrFresnelTCoeff = ( iPMTPtr->GetFresnelTCoeff() ) / ( iPMTPtr->GetCentralFresnelTCoeff() );

  Float_t angResp = ModelAngularResponse( iRunPtr, iPMTPtr, fiAng, 0 );
  Float_t intensity = ModelLBDistribution( iRunPtr, iPMTPtr, fiLBDist, 0 );

  Float_t pmtResponse = angResp * intensity * corrSolidAngle * corrFresnelTCoeff * TMath::Exp( - ( dScint * GetScintPar() )
                                                                                               - ( dAV * GetAVPar() )
                                                                                               - ( dWater * GetWaterPar() ) );
  
  if( derivatives ){

    dyda[ GetScintParIndex() ] = -dScint;
    dyda[ GetAVParIndex() ] = -dAV;
    dyda[ GetWaterParIndex() ] = -dWater;
    dyda[ GetAngularResponseParIndex() + fiAng ] = +1.0 / angResp;
    dyda[ GetLBDistributionParIndex() + fiLBDist ] = +1.0 / intensity;

  }

  fCiAng = 0;
  fCiLBDist = 0;
  Float_t angRespCtr = ModelAngularResponse( iRunPtr, iPMTPtr, fCiAng, 1 );
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

/////////////////////////////////////
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
	      //nrerror("gaussj: Singular Matrix-1");
	      //gSNO->Warning("gaussj","Singular Matrix-1");
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
	//gSNO->Warning("gaussj","Singular Matrix-2");
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

  // int *indxc,*indxr,*ipiv;
  // int i,icol,irow,j,k,l,ll;
  // float big,dum,pivinv,swap;
  // Int_t retval = 0;
  // cout << "TEST 1" << endl;
  // indxc=flMath.LOCASIntVector(1,n);
  // cout << "TEST 2" << endl;
  // indxr=flMath.LOCASIntVector(1,n);
  // cout << "TEST 3" << endl;
  // ipiv=flMath.LOCASIntVector(1,n);
  // cout << "TEST 4" << endl;
  // for (j=1;j<=n;j++) ipiv[j]=0;
  // for (i=1;i<=n;i++) {
  //   cout << "TEST 5" << endl;
  //   big=0.0;
  //   for (j=1;j<=n;j++)
  //     cout << "TEST 5i" << endl;    
  //     if (ipiv[j] != 1)
  //      for (k=1;k<=n;k++) {
  // 	  cout << "TEST 5ii" << endl;
  // 	  if (ipiv[k] == 0) {
  // 	    if (fabs(a[j][k]) >= big) {
  // 	      big=fabs(a[j][k]);
  // 	      irow=j;
  // 	      icol=k;
  // 	      cout << "Value of irow is: " << irow << " here!" << endl;
  //           }
  //         } else if (ipiv[k] > 1) 
  //           {
  //             //nrerror("gaussj: Singular Matrix-1");
  //             cout << "Singular Matrix-1" << endl;
  //             retval = -1;
  //           }
  //       }
  //   ++(ipiv[icol]);
  //   if (irow != icol) {
  //     for (l=1;l<=n;l++) { SWAP(a[irow][l],a[icol][l]);     cout << "TEST 5iii" << endl; }
  //     cout << "l is: " << l << " and n is: " << n << " and m is: " << m << endl;

  //     if ( b ) { cout << "b exists!" << endl; }
  //     for (l=1;l<=m;l++) { 
  // 	cout << "l is here: " << l << endl;
  // 	cout << "irow is: " << irow << endl;
  // 	cout << "icol is: " << icol << endl;
  // 	cout << "b[irow][l] is: " << b[irow][l] << " and b[icol][l] is: " << b[icol][l] << endl;
  // 	SWAP(b[irow][l],b[icol][l]);     
  // 	cout << "TEST 5iv" << endl;
  //     }

  //   }
  //   indxr[i]=irow;
  //   indxc[i]=icol;
  //   if (a[icol][icol] == 0.0) 
  //     {
  //       //nrerror("gaussj: Singular Matrix-2");
  //       cout << "Singular Matrix-2" << endl;
  //       retval = -2;
  //     }
  //   pivinv=1.0/a[icol][icol];
  //   a[icol][icol]=1.0;
  //   for (l=1;l<=n;l++) a[icol][l] *= pivinv;     cout << "TEST 5v" << endl;
  //   for (l=1;l<=m;l++) b[icol][l] *= pivinv;    cout << "TEST 5vi" << endl;
  //   for (ll=1;ll<=n;ll++)
  //     cout << "TEST 5vii" << endl;
  //     if (ll != icol) {
  //       dum=a[ll][icol];
  //       a[ll][icol]=0.0;
  //       for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;    cout << "TEST 5viii" << endl;
  //       for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;    cout << "TEST 5ix" << endl;
  //     }
  // }
  // for (l=n;l>=1;l--) {
  //   cout << "TEST 5x" << endl;
  //   if (indxr[l] != indxc[l])
  //     for (k=1;k<=n;k++)
  // 	cout << "TEST 5xi" << endl;
  //       SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  // }
  
  // cout << "TEST 5xii" << endl;
  // flMath.LOCASFree_IntVector(ipiv,1,n);
  // cout << "TEST 5xiii" << endl;
  // flMath.LOCASFree_IntVector(indxr,1,n);
  // cout << "TEST 5xiv" << endl;
  // flMath.LOCASFree_IntVector(indxc,1,n);
  // cout << "Got return value for gaussj" << endl;
  // return retval;
}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PerformFit( )
{


  cout << "About to fill parameter base" << endl;
  FillParameterbase();

  cout << "About to fill AngIndex" << endl;
  FillAngIndex();

  // cout << "Example Parameters Values:" << endl;
  // for ( Int_t iPar = 1; iPar <= fNPMTsInFit; iPar++ ){
  //   cout << iPar << ": X: " << fMrqX[ iPar ] << ", Y: " << fMrqY[ iPar ] << ", Sigma: " << fMrqSigma[ iPar ] << endl;
  //   cout << "ChiSq: " << fChiArray[ iPar ] << endl;

  //   cout << "###########" << endl;
  // }

  MrqFit(fMrqX, fMrqY, fMrqSigma, fNPMTsInFit, fMrqParameters, fMrqVary, fNParametersInFit, fMrqCovariance,
	 fMrqAlpha, &fChiSquare);

  cout << "Example Parameters:" << endl;
  for ( Int_t iPar = 1; iPar <= 50; iPar++ ){
    cout << "Parameter | Vary Flag: " << iPar << " : " << fMrqParameters[ iPar ] << " | " << fMrqVary[ iPar ] << endl;
  }

}

/////////////////////////////////////
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

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillAngIndex( )
{

  //if ( fParamVarMap ) delete[] fParamVarMap;
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

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::AllocateParameters( )
{

  // The total number of parameters in the fit
  fNParametersInFit = 3
    + fNAngularResponseBins
    + fNLBDistributionThetaBins * fNLBDistributionPhiBins;
  
  // The Levenberg-Marquadt working arrays
  fMrqX = flMath.LOCASVector( 1, fNDataPointsInFit );
  fMrqY = flMath.LOCASVector( 1, fNDataPointsInFit );
  fMrqSigma = flMath.LOCASVector( 1, fNDataPointsInFit );
  
  fMrqParameters = flMath.LOCASVector( 1, fNParametersInFit );
  fMrqVary = flMath.LOCASIntVector( 1, fNParametersInFit );
  
  fMrqCovariance = flMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  fMrqAlpha = flMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  
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
    
    if ( fAngularResponseVary ){ fMrqVary[ GetAngularResponseParIndex() + iT ] = 1; }
    else { fMrqVary[ GetAngularResponseParIndex() + iT ] = 0; }
  }
  
  // The bin values for the laserball distribution histogram
  for ( Int_t iT = 0; iT < ( fNLBDistributionThetaBins * fNLBDistributionPhiBins ); iT++ ){
    fMrqParameters[ GetLBDistributionParIndex() + iT ] = fLBDistributionInit;  

    if ( fLBDistributionVary ){ fMrqVary[ GetLBDistributionParIndex() + iT ] = 1; }
    else { fMrqVary[ GetLBDistributionParIndex() + iT ] = 0; } 
  }


}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PrintInitialisationInfo( )
{

  cout << "LOCASFit Parameters Initialised" << endl;
  cout << "Initial parameter values follow..." << endl;

  cout << "Scintillator (1/attenuation) length: " << fScintInit << endl;
  if ( fScintVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Acrylic (AV) (1/attenuation) length: " << fAVInit << endl;
  if ( fAVVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }

  cout << "Water (1/attenuation) length: " << fWaterInit << endl;
  if ( fWaterVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Angular response histogram (theta, 0 - 90 degrees) consists of: " << fNAngularResponseBins << " bins of width " << (Double_t)90.0 / fNAngularResponseBins << " degrees each" << endl;
  if ( fAngularResponseVary ){ cout << "Parameters in the histogram will vary in the fit" << endl; }
  else{ cout << "Parameters in the histogram are fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Laserball distribution 2D histogram (theta 0 - 180 degrees, phi 0 - 360 degrees) consists of: " << fNLBDistributionThetaBins << " x " << fNLBDistributionPhiBins << " bins of width " << (Double_t)180.0 / fNLBDistributionThetaBins << " x " << (Double_t)360.0 / fNLBDistributionPhiBins << " degrees each" << endl;
  if ( fLBDistributionVary ){ cout << "Parameters in the histogram will vary in the fit" << endl; }
  else{ cout << "Parameters in the histogram are fixed in the fit" << endl; }
  cout << " ------------- " << endl;


  cout << " ------------- " << endl;
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
  cout << "Number of parameters allowed to vary in the fit: " << nParVary << endl;
  cout << "Number of parameters fixed in the fit: " << nParFixed << endl;
  
  cout << "Fit will skip every " << fNPMTSkip << " pmts in the fit." << endl;

  cout << " ------------- " << endl;
  cout << " ------------- " << endl;
  cout << "Cut information now follows..." << endl;
  cout << "The chisquare, chi, for each PMT must be: " << fChiSquareMinLimit << " < chi < " << fChiSquareMaxLimit << endl;
  cout << "Each PMT must be within " << fNSigma << " standard deviations from the mean occupancy of it's respective run." << endl;
  cout << "Each PMT must have a minimum prompt count of at least " << fNOccupancy << " accross both off-axis and central runs." << endl;
  cout << "Each PMT must have a AVHD Shadowing value, sh, in the region " << fAVHDShadowingMin << " < sh < " << fAVHDShadowingMax << " accross both off-acis and central runs." << endl;
  cout << "Each PMT must have a Geo Shadowing value, sh, in the region " << fGeoShadowingMin << " < sh < " << fGeoShadowingMax << " accross both off-acis and central runs." << endl;
  cout << "Values for which the PMTs will be cut from the CSS and CHS reports are, CSS: " << fCSSFlag << ", CHS: " << fCHSFlag << endl;
  
}


/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::WriteToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );
  // Create the Run Tree
  TTree* runTree = new TTree( "LOCASRunT", "LOCAS Run Tree" );

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

