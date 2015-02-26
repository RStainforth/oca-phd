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

#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASOpticsModel )

//////////////////////////////////////
//////////////////////////////////////

LOCASOpticsModel::LOCASOpticsModel( const char* fileName )
{

  fParameters = NULL;

  fScintParIndex = -10;
  fAVParIndex = -10;
  fWaterParIndex = -10;

  fScintRSParIndex = -10;
  fAVRSParIndex = -10;
  fWaterRSParIndex = -10;

  fAngularResponseParIndex = -10;
  fLBDistributionParIndex = -10;
  fLBPolynomialParIndex = -10;

  fNAngularResponseBins = -10;
  fNLBDistributionBins = -10;
  fNLBDistributionThetaBins = -10;
  fNLBDistributionPhiBins = -10;
  fNLBPolynomialParameters = -10;

  fModelParameterStore.AddParameters( fileName );

  AllocateParameters();
  InitialiseParameterIndices();
  InitialiseParameters();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASOpticsModel::AllocateParameters()
{

  // Allocate the memory for the correct number of parameters in the model
  Int_t nParams = fModelParameterStore.GetNParameters();
  SetNParameters( nParams );
  if ( fParameters != NULL ) { delete[] fParameters; }
  else{ fParameters = new Double_t[ nParams ]; }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASOpticsModel::InitialiseParameterIndices()
{

  // Iterate through all the parameter objects and add their index values to the model
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
    
    else if ( iPar->GetParameterName() == "par_ang_resp" ){ 
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

    else if( iPar->GetParameterName() == "par_lb_dist" ){
      SetLBDistributionParIndex( iPar->GetIndex() );
      SetNLBDistributionThetaBins( iPar->GetNInGroup() );
      SetNLBDistributionPhiBins( iPar->GetNInGroup() );
      SetNLBDistributionBins( iPar->GetNInGroup() );
    }

    else if ( iPar->GetParameterName() == "par_lb_dist_poly_start" ){ 
      SetLBPolynomialParIndex( iPar->GetIndex() );
      SetNAngularResponseBins( iPar->GetNInGroup() );
    }

    else if ( iPar->GetParameterName() == "par_lb_dist_poly" ){ 
      SetLBPolynomialParIndex( iPar->GetIndex() );
      SetNAngularResponseBins( iPar->GetNInGroup() );
    }
    

  }

}


//////////////////////////////////////
//////////////////////////////////////

void LOCASOpticsModel::InitialiseParameters()
  {

    // Initialise the values of the parameter objects to the entries in the fParameters object array
  Int_t nParams = fModelParameterStore.GetNParameters();

  for ( Int_t iPar = 0; iPar < nParams; iPar++ ){
    fParameters[ iPar ] = 0.0;
  }

  std::vector< LOCASModelParameter >::iterator iPar;

  for ( iPar = fModelParameterStore.GetLOCASModelParametersIterBegin();
        iPar != fModelParameterStore.GetLOCASModelParametersIterEnd();
        iPar++ ){

    if ( ( iPar->GetParameterName() == "par_scint" ) && ( GetScintParIndex() >= 0 ) ){ SetScintPar( iPar->GetInitialValue() ); }
    else if ( ( iPar->GetParameterName() == "par_av" ) && ( GetAVParIndex() >= 0 ) ){ SetAVPar( iPar->GetInitialValue() ); }
    else if ( ( iPar->GetParameterName() == "par_water" ) && ( GetWaterParIndex() >= 0 ) ){ SetWaterPar( iPar->GetInitialValue() ); }

    else if ( ( iPar->GetParameterName() == "par_scint_rs" ) && ( GetScintRSParIndex() >= 0 ) ){ SetScintRSPar( iPar->GetInitialValue() ); }
    else if ( ( iPar->GetParameterName() == "par_av_rs" ) && ( GetAVRSParIndex() >= 0 ) ){ SetAVRSPar( iPar->GetInitialValue() ); }
    else if ( ( iPar->GetParameterName() == "par_water_rs" ) && ( GetWaterRSParIndex() >= 0 ) ){ SetWaterRSPar( iPar->GetInitialValue() ); }

  }

  if ( GetAngularResponseParIndex() >= 0 ){
    Float_t angle = 0.0;
    for ( Int_t iT = 0; iT < GetNAngularResponseBins(); iT++ ){
      
      angle = (Float_t)( iT + 0.5 ) * (Float_t)( 90.0 / GetNAngularResponseBins() );
      
      if ( angle < 36.0 ){ SetPar( GetAngularResponseParIndex() + iT, 1.0 + ( 0.002222 * angle ) );  }
      else{ SetPar( GetAngularResponseParIndex() + iT, 1.0 ); }
      
    }
  }

  if ( GetLBDistributionParIndex() >= 0 ){
    for ( Int_t iT = 0; iT < GetNLBDistributionBins(); iT++ ){ SetPar( GetLBDistributionParIndex() + iT, 1.0 ); }
  }

  if ( GetLBPolynomialParIndex() >= 0 ){
    for ( Int_t iPoly = 0; iPoly < GetNLBPolynomialParameters(); iPoly++ ){ SetPar( GetLBPolynomialParIndex() + iPoly, 1.0 ); }
  }

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelPrediction( const LOCASDataPoint& dataPoint )
{
  Float_t angRespRatio = 1.0;
  Float_t intensityRatio = 1.0;

  Float_t dScintAtt = 0.0;
  Float_t dScintRS = 0.0;
  Float_t dAVAtt = 0.0;
  Float_t dAVRS = 0.0;
  Float_t dWaterAtt = 0.0;
  Float_t dWaterRS = 0.0;

  // This is currently set to 1.0 until the normalisation ratio is better understood
  Float_t normVal = dataPoint.GetLBIntensityNormRatio();

  Float_t dScint = dataPoint.GetDeltaDistInInnerAV();
  if ( GetScintParIndex() >= 0 ){ dScintAtt = dScint / GetScintPar(); }
  if ( GetScintRSParIndex() >= 0 ){ dScintRS = dScint / GetScintRSPar(); }

  Float_t dAV = dataPoint.GetDeltaDistInAV();
  if ( GetAVParIndex() >= 0 ){ dAVAtt = dAV / GetAVPar(); }
  if ( GetAVRSParIndex() >= 0 ){ dAVRS = dAV / GetAVRSPar(); }

  Float_t dWater = dataPoint.GetDeltaDistInWater();
  if ( GetWaterParIndex() >= 0 ){ dWaterAtt = dWater / GetWaterPar(); }
  if ( GetWaterRSParIndex() >= 0 ){ dWaterRS = dWater / GetWaterRSPar(); }

  if ( GetAngularResponseParIndex() >= 0 ){
    Float_t angResp = ModelAngularResponse( dataPoint, 0 );
    Float_t angRespCtr = ModelAngularResponse( dataPoint, 1 );
    angRespRatio = angResp / angRespCtr; 
  }

  if ( GetLBDistributionParIndex() >= 0 ){
    Float_t intensity = ModelLBDistribution( dataPoint, 0 );
    Float_t intensityCtr = ModelLBDistribution( dataPoint, 1 );
    intensityRatio = intensity / intensityCtr;
    if ( GetLBPolynomialParIndex() >= 0 ){
      Float_t intensityPoly = ModelLBPolynomialMask( dataPoint, 0 );
      Float_t intensityCtrPoly = ModelLBPolynomialMask( dataPoint, 1 );
      intensityRatio *= ( intensityPoly / intensityCtrPoly );
    }
  }

  Float_t modelPrediction = normVal * angRespRatio * intensityRatio * 
    TMath::Exp( - ( ( dScintAtt + dScintRS )
                    + ( dAVAtt + dScintRS )
                    + ( dWaterAtt + dWaterRS ) ) );

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
 
Float_t LOCASOpticsModel::ModelLBPolynomialMask( const LOCASDataPoint& dataPoint, Int_t runType )
{

  Float_t lbRelTheta = 0.0;
  
  if ( runType == 0 ){ lbRelTheta = dataPoint.GetLBTheta(); }
  else if ( runType == 1 ){ lbRelTheta = dataPoint.GetCentralLBTheta(); }
  
  Float_t polynomialVal = 1.0;

  for ( Int_t iPar = 0; iPar < fNLBPolynomialParameters; iPar++ ){

    polynomialVal += ( ( fParameters[ GetLBPolynomialParIndex() + iPar ] )
                       * ( TMath::Power( ( 1 + TMath::Cos( lbRelTheta ) ), ( iPar + 1 ) ) ) );
    
  }

  return polynomialVal;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelAngularResponse( const LOCASDataPoint& dataPoint, Int_t runType )
{

  Float_t cosTheta = 0.0;
  
  if ( runType == 0 ) { cosTheta = TMath::Cos( dataPoint.GetIncidentAngle() / ( 180.0 / TMath::Pi() ) ); }
  if ( runType == 1 ) { cosTheta = TMath::Cos( dataPoint.GetCentralIncidentAngle() / ( 180.0 / TMath::Pi() ) ); }

  Float_t angle = ( TMath::ACos( cosTheta ) * ( 180.0 / TMath::Pi() ) );

  Int_t iAng = (Int_t)( angle * GetNAngularResponseBins() / 90.0 );
  
  return fParameters[ fAngularResponseParIndex + iAng ];

}



