////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASOpticsModel.hh
///
/// CLASS: LOCAS::LOCASOpticsModel
///
/// BRIEF: The class which defines the optical response model
///        of PMTs.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASOpticsModel.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASModelParameter.hh"
#include "LOCASDataPoint.hh"

#include <iostream>

#include "TMath.h"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASOpticsModel )

//////////////////////////////////////
//////////////////////////////////////

LOCASOpticsModel::LOCASOpticsModel( const char* fileName )
{

  fParameters = NULL;
  fModelParameterStore.AddParameters( fileName );

  AllocateParameters();
  InitialiseParameterIndices();
  InitialiseParameters();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASOpticsModel::AllocateParameters()
{

  Int_t nParams = fModelParameterStore.GetNParameters();
  SetNParameters( nParams );
  if ( fParameters != NULL ) { delete[] fParameters; }
  else{ fParameters = new Double_t[ nParams ]; }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASOpticsModel::InitialiseParameterIndices()
{

  std::vector< LOCASModelParameter >::iterator iPar;

  for ( iPar = fModelParameterStore.GetLOCASModelParametersIterBegin();
        iPar != fModelParameterStore.GetLOCASModelParametersIterEnd();
        iPar++ ){

    if ( iPar->GetParameterName() == "par_scint" ){ SetScintParIndex( iPar->GetIndex() ); }
    else if ( iPar->GetParameterName() == "par_av" ){ SetAVParIndex( iPar->GetIndex() ); }
    else if ( iPar->GetParameterName() == "par_water" ){ SetWaterParIndex( iPar->GetIndex() ); }

    else if ( iPar->GetParameterName() == "par_scint_rs" ){ SetScintRSParIndex( iPar->GetIndex() ); }
    else if ( iPar->GetParameterName() == "par_av_rs" ){ SetAVRSParIndex( iPar->GetIndex() ); }
    else if ( iPar->GetParameterName() == "par_water_rs" ){ SetWaterRSParIndex( iPar->GetIndex() ); }

    else if ( iPar->GetParameterName() == "par_ang_resp_start" ){ 
      SetAngularResponseParIndex( iPar->GetIndex() );
      SetNAngularResponseBins( iPar->GetNInGroup() );
    }
    else if ( iPar->GetParameterName() == "par_lb_dist_start" ){ 
      SetLBDistributionParIndex( iPar->GetIndex() ); 
      SetNLBDistributionBins( iPar->GetNInGroup() );
      
      if ( iPar->GetNInGroup() % 36 == 0 ){
        SetNLBDistributionThetaBins( GetNLBDistributionBins() / 36 );
        SetNLBDistributionPhiBins( GetNLBDistributionBins() / GetNLBDistributionThetaBins()  );

      }
    }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASOpticsModel::InitialiseParameters()
{

  Int_t nParams = fModelParameterStore.GetNParameters();

  for ( Int_t iPar = 0; iPar < nParams; iPar++ ){
    fParameters[ iPar ] = 0.0;
  }

  std::vector< LOCASModelParameter >::iterator iPar;

  for ( iPar = fModelParameterStore.GetLOCASModelParametersIterBegin();
        iPar != fModelParameterStore.GetLOCASModelParametersIterEnd();
        iPar++ ){

    if ( iPar->GetParameterName() == "par_scint" ){ SetScintPar( iPar->GetInitialValue() ); }
    else if ( iPar->GetParameterName() == "par_av" ){ SetAVPar( iPar->GetInitialValue() ); }
    else if ( iPar->GetParameterName() == "par_water" ){ SetWaterPar( iPar->GetInitialValue() ); }

    else if ( iPar->GetParameterName() == "par_scint_rs" ){ SetScintRSPar( iPar->GetInitialValue() ); }
    else if ( iPar->GetParameterName() == "par_av_rs" ){ SetAVRSPar( iPar->GetInitialValue() ); }
    else if ( iPar->GetParameterName() == "par_water_rs" ){ SetWaterRSPar( iPar->GetInitialValue() ); }

  }

  Float_t angle = 0.0;
  for ( Int_t iT = 0; iT < GetNAngularResponseBins(); iT++ ){

    angle = (Float_t)( iT + 0.5 ) * (Float_t)( 90.0 / GetNAngularResponseBins() );

    if ( angle < 36.0 ){ SetPar( GetAngularResponseParIndex() + iT, 1.0 + ( 0.002222 * angle ) ); }
    else{ SetPar( GetAngularResponseParIndex() + iT, 1.0 ); }

  }

  for ( Int_t iT = 0; iT < GetNLBDistributionBins(); iT++ ){ SetPar( GetLBDistributionParIndex() + iT, 1.0 ); }

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelPrediction( const LOCASDataPoint& dataPoint )
{

  Float_t dScint = dataPoint.GetDeltaDistInScint();
  Float_t dAV = dataPoint.GetDeltaDistInAV();
  Float_t dWater = dataPoint.GetDeltaDistInWater();

  Float_t angResp = ModelAngularResponse( dataPoint, 0 );
  Float_t angRespCtr = ModelAngularResponse( dataPoint, 1 );

  Float_t intensity = ModelLBDistribution( dataPoint, 0 );
  Float_t intensityCtr = ModelLBDistribution( dataPoint, 1 );

  Float_t intensityRatio = 0.0;
  Float_t angRespRatio = 0.0;

  angRespRatio = angResp / angRespCtr; 
  intensityRatio = intensity / intensityCtr;

  Float_t modelPrediction = angRespRatio * intensityRatio * 
    TMath::Exp( - ( dScint * ( ( 1.0 / GetScintPar() ) + ( 1.0 / GetScintRSPar() ) )
                    + dAV * ( ( 1.0 / GetAVPar() ) + ( 1.0 / GetAVRSPar() ) )
                    + dWater * ( ( 1.0 / GetWaterPar() ) + ( 1.0 / GetWaterRSPar() ) ) ) );

  return modelPrediction;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelLBDistribution( const LOCASDataPoint& dataPoint, Int_t runType )
{

  Float_t lbRelTheta, lbRelPhi = 0.0;

  if ( runType == 0 ){
    lbRelTheta = dataPoint.GetLBTheta();
    lbRelPhi = dataPoint.GetLBPhi();
  }

  else if ( runType == 1 ){
    lbRelTheta = dataPoint.GetCentralLBTheta();
    lbRelPhi = dataPoint.GetCentralLBPhi();
  }

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  TVector3 pmtRelVec( 0.0, 0.0, 0.0 );

  pmtRelVec.SetTheta( lbRelTheta );
  pmtRelVec.SetPhi( lbRelPhi );

  Float_t cosTheta = 0.0;
  Float_t phi = 0.0;

  cosTheta = lbAxis * pmtRelVec;
  phi = 0.0;

  if ( cosTheta > 1.0 ) cosTheta = 1.0;
  else if ( cosTheta < -1.0 ) cosTheta = -1.0;

  if ( phi > 2 * TMath::Pi() ) phi -= 2 * TMath::Pi();
  else if ( phi < 0 ) phi += 2 * TMath::Pi();

  
  Int_t iTheta = (Int_t)( ( 1 + cosTheta ) / 2 * GetNLBDistributionThetaBins() );
  if ( iTheta < 0 ) iTheta = 0;
  if ( iTheta >= GetNLBDistributionThetaBins() ) iTheta = GetNLBDistributionThetaBins() - 1;

  Int_t iPhi = (Int_t)( phi / ( 2 * TMath::Pi() ) * GetNLBDistributionPhiBins() );
  if ( iPhi < 0 ) iPhi = 0;
  if ( iPhi >= GetNLBDistributionPhiBins() ) iPhi = GetNLBDistributionPhiBins() - 1;

  Int_t iLBDist = ( iTheta ) * ( GetNLBDistributionPhiBins() + iPhi );

  Float_t laserlight = GetLBDistributionPar( iLBDist );
  
  return laserlight;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelAngularResponse( const LOCASDataPoint& dataPoint, Int_t runType )
{

  Float_t cosTheta = 0.0;
  
  if ( runType == 0 ) { cosTheta = TMath::Cos( dataPoint.GetIncidentAngle() ); }
  if ( runType == 1 ) { cosTheta = TMath::Cos( dataPoint.GetCentralIncidentAngle() ); }

  Float_t angle = ( TMath::ACos( cosTheta ) ) * ( 180.0 / TMath::Pi() );
  Int_t iAng = (Int_t)( angle * GetNAngularResponseBins() / 90.0 );
  
  return GetAngularResponsePar( iAng );

}



