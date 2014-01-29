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

  fScintAttVary = lDB.GetBoolField( "FITFILE", "scint_att_vary" );
  fScintAttInit = lDB.GetIntField( "FITFILE", "scint_att_init" );

  fAVAttVary = lDB.GetBoolField( "FITFILE", "av_att_vary" );
  fAVAttInit = lDB.GetIntField( "FITFILE", "av_att_init" );

  fWaterAttVary = lDB.GetBoolField( "FITFILE", "water_att_vary" );
  fWaterAttInit = lDB.GetIntField( "FITFILE", "water_att_init" );

  fAngularResponseVary = lDB.GetBoolField( "FITFILE", "ang_resp_vary" );
  fAngularResponseInit = lDB.GetIntField( "FITFILE", "ang_resp_init" );

  fLBDistributionVary = lDB.GetBoolField( "FITFILE", "lb_dist_vary" );
  fLBDistributionInit = lDB.GetIntField( "FITFILE", "lb_dist_init" );


  if ( fScintAttVary && fAVAttVary && fWaterAttVary
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

  fNParametersInFit = 3
    + fNAngularResponseBins
    + fNLBDistributionThetaBins * fNLBDistributionPhiBins;

  LOCASMath lMath;
  fMrqParameters = lMath.LOCASVector( 1, fNParametersInFit );
  fMrqVary = lMath.LOCASIntVector( 1, fNParametersInFit );

  fMrqCovariance = lMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  fMrqAlpha = lMath.LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );

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
