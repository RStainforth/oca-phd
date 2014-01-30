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

using namespace LOCAS;
using namespace std;

// Default Constructor
LOCASFit::LOCASFit( const char* fitFile )
{

  LOCASDB lDB;
  lDB.LoadRunList( fitFile );
  lDB.SetFile( fitFile );
  

  fFitName = lDB.GetStringField( "FITFILE", "fit_name" );
  fFitTitle = lDB.GetStringField( "FITFILE", "fit_title" );
    
  fListOfRunIDs = lDB.GetRunList();
  fNumberOfRuns = fListOfRunIDs.size();

  for (Int_t iRun = 0; iRun < fNumberOfRuns; iRun++ ){
    fRunReader.Add( fListOfRunIDs[ iRun ] );
  }

  for (Int_t iRun = 0; iRun < fNumberOfRuns; iRun++ ){
    fNDataPointsInFit += ( fRunReader.GetLOCASRun( iRun ) )->GetNPMTs();
  }

  fChiArray = new Float_t[ fNDataPointsInFit ];
  fResArray = new Float_t[ fNDataPointsInFit ];

  fScintVary = lDB.GetBoolField( "FITFILE", "scint_vary" );
  fScintInit = lDB.GetIntField( "FITFILE", "scint_init" );

  fAVVary = lDB.GetBoolField( "FITFILE", "av_vary" );
  fAVInit = lDB.GetIntField( "FITFILE", "av_init" );

  fWaterVary = lDB.GetBoolField( "FITFILE", "water_vary" );
  fWaterInit = lDB.GetIntField( "FITFILE", "water_init" );

  fAngularResponseVary = lDB.GetBoolField( "FITFILE", "ang_resp_vary" );
  fAngularResponseInit = lDB.GetIntField( "FITFILE", "ang_resp_init" );

  fLBDistributionVary = lDB.GetBoolField( "FITFILE", "lb_dist_vary" );
  fLBDistributionInit = lDB.GetIntField( "FITFILE", "lb_dist_init" );


  if ( fScintVary && fAVVary && fWaterVary
       && fAngularResponseVary && fLBDistributionVary ){
    fVaryAll = true;
  }
  else{ fVaryAll = false; }

  fNLBDistributionThetaBins = lDB.GetIntField( "FITFILE", "lb_dist_n_theta_bins" );
  fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", "lb_dist_n_phi_bins" );
  fNPMTsPerLBDistributionBinMin = lDB.GetIntField( "FITFILE", "lb_dist_min_n_pmts" );

  fNAngularResponseBins = lDB.GetIntField( "FITFILE", "ang_resp_n_bins" );
  fNPMTsPerAngularResponseBinMin = lDB.GetIntField( "FITFILE", "ang_resp_min_n_pmts" );

  fChiSquareMaxLimit = lDB.GetDoubleField( "FITFILE", "chisq_max_limit" );
  fNSigma = lDB.GetDoubleField( "FITFILE", "n_sigma" );

  fNParametersInFit = 3
    + fNAngularResponseBins
    + fNLBDistributionThetaBins * fNLBDistributionPhiBins;

  LOCASMath lMath;
  fMrqParameters = lMath.LOCASVector( 1, fNParametersInFit );
  fMrqVary = lMath.LOCASIntVector( 1, fNParametersInFit );

  fMrqCovariance = lMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  fMrqAlpha = lMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );

  SetScintPar( fScintInit );
  SetAVPar( fAVInit );
  SetWaterPar( fWaterInit );

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

void LOCASFit::DataScreen()
{

  Int_t *pmtAngleValid = new Int_t[ fNAngularResponseBins ];
  Int_t iPMTAngleValid;
  Float_t angle;

  Int_t tmpRun, tmpPMT, iX;

  Float_t pmtResidual, pmtChiSquared, pmtSigma;
  Float_t pmtModelPrediction;
  Float_t pmtOccRatio;

  for (Int_t i = 0; i < fNAngularResponseBins; i++ ){
    pmtAngleValid[ i ] = 0;    
  }

  fChiSquare = 0.0;

  for ( Int_t iRun = 0; iRun < fNumberOfRuns; iRun++ ){
    
    fCurrentRun = fRunReader.GetLOCASRun( fListOfRunIDs[ iRun ] );
    fNDataPointsInFit += fCurrentRun->GetNPMTs();
    std::map< Int_t, LOCASPMT >::iterator iPMT;
    Float_t occVal;
    Float_t dcOccValMean = 1.0;
    Float_t dcSigma = 1.0;

    // Obtain the mean occRatio * LBIntensityNorm for the run
    for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); ++iPMT ){      
      occVal = ( ( iPMT->second ).GetOccRatio() ) * ( ( iPMT->second ).GetLBIntensityNorm() );
      dcOccValMean += occVal;
    }

    if( fCurrentRun->GetNPMTs() != 0 ){
      dcOccValMean /= ( (Float_t)fCurrentRun->GetNPMTs() );
    }

    // Obtain the sigma occRatio * LBIntensityNorm for the run
    for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); ++iPMT ){      
      occVal = ( ( iPMT->second ).GetOccRatio() ) * ( ( iPMT->second ).GetLBIntensityNorm() );
      dcSigma += TMath::Power( (occVal - dcOccValMean), 2 );
    }

    if( fCurrentRun->GetNPMTs() != 0 ){
      Float_t tmpDCSigma = dcSigma;
      dcSigma = TMath::Sqrt( tmpDCSigma / ( (Float_t)fCurrentRun->GetNPMTs() ) );
    }

    // Screen PMTs for bad tubes
    for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); iPMT != fCurrentRun->GetLOCASPMTIterEnd(); ++iPMT ){ 
      LOCASPMT* lPMT = &( iPMT->second );
      Bool_t skipPMT = PMTSkip( lPMT, dcOccValMean, dcSigma );

      if( !skipPMT ){

	pmtModelPrediction = ModelPrediction( lPMT );
	pmtResidual = ( pmtModelPrediction - lPMT->GetOccRatio() );
	pmtSigma = TMath::Sqrt( TMath::Power( ( lPMT->GetOccRatioErr() ), 2 ) );
	// Note: Not including PMT variability sigma in here, this was in LOCAS (SNO)
	// but will probably have changed for SNO+ so will need recomputing
	pmtChiSquared = ( pmtResidual * pmtResidual ) / ( pmtSigma * pmtSigma );

	fChiSquare += pmtChiSquared;

	if ( fNPMTsInFit >= 0 && fNPMTsInFit < ( fNDataPointsInFit - 1 ) ){
	  fChiArray[ fNPMTsInFit + 1 ] = pmtChiSquared;
	  fResArray[ fNPMTsInFit + 1 ] = pmtSigma;
	}
	
	fMrqX[ fNPMTsInFit + 1 ] = (Float_t)( ( lPMT->GetID() ) + ( iRun * 10000 ) );         // 'Special' Indexing Scheme
	fMrqY[ fNPMTsInFit + 1 ] = ( lPMT->GetOccRatio() );
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
  


  fDataScreen = true;
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFit::PMTSkip( LOCASPMT* pmt, Float_t mean, Float_t sigma )
{

  Bool_t pmtSkip = false;
  Float_t occVal = ( pmt->GetOccRatio() ) * ( pmt->GetLBIntensityNorm() ); 
  
  pmtSkip = (Bool_t)( occVal <= 0.0 
		      || TMath::Abs( occVal - mean ) > ( fNSigma * sigma )
		      || ( pmt->GetOccRatioErr() ) / ( pmt->GetOccRatio() ) > 0.25 
		      || !( pmt->GetIsVerified() )
		      || ( pmt->GetBadPath() )
		      || ( pmt->GetNeckFlag() ) );
  
  return pmtSkip;
    
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelPrediction( LOCASPMT* lPMT )
{

  Float_t dScint = lPMT->GetCorrDistInScint();
  Float_t dAV = lPMT->GetCorrDistInAV();
  Float_t dWater = lPMT->GetCorrDistInWater();

  Float_t corrSolidAngle = lPMT->GetCorrSolidAngle();
  Float_t corrFresnelTCoeff = lPMT->GetCorrFresnelTCoeff();

  Float_t angResponse = ModelAngularResponse( lPMT, fAngRespIndex );
  Float_t intensity = ModelLBDistribution( fCurrentRun, lPMT, fLBDistIndex );

  Float_t pmtResponse = angResponse * intensity * corrSolidAngle * corrFresnelTCoeff * TMath::Exp( - ( dScint * GetScintPar() )
												   - ( dAV * GetAVPar() )
												   - ( dWater * GetWaterPar() ) );

  return pmtResponse;
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelAngularResponse( LOCASPMT* lPMT, Int_t& iAng )
{

  Float_t angle = ( TMath::ACos( lPMT->GetCosTheta() ) ) * ( 180.0 / TMath::Pi() );
  iAng = (Int_t)( angle * fNAngularResponseBins / 90.0 );
		    
  return GetAngularResponsePar( iAng );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistribution( LOCASRun* locasRun, LOCASPMT* lPMT, Int_t& iLBDist )
{

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  Float_t lbTheta = locasRun->GetLBTheta();
  Float_t lbPhi = locasRun->GetLBPhi();

  TVector3 pmtRelVec( 0.0, 0.0, 0.0 );
  Float_t pmtTheta = lPMT->GetLBTheta();
  Float_t pmtPhi = lPMT->GetLBPhi();

  lbAxis.SetTheta( lbTheta );
  lbAxis.SetPhi( lbPhi );

  pmtRelVec.SetTheta( pmtTheta );
  pmtRelVec.SetPhi( pmtPhi );

  Float_t cosTheta = lbAxis * pmtRelVec;
  Float_t phi = 0.0;
  
  Int_t iTheta = (Int_t)( ( 1 + cosTheta ) / 2 * fNLBDistributionThetaBins );
  Int_t iPhi = (Int_t)( phi / ( 2 * TMath::Pi() ) * fNLBDistributionPhiBins );

  iLBDist = iTheta * fNLBDistributionPhiBins + iPhi;
  return GetLBDistributionPar( iLBDist );

}
