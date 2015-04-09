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

LOCASOpticsModel::LOCASOpticsModel()
{

  fModelParameterStore = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

// LOCASOpticsModel::LOCASOpticsModel( const char* fileName )
// {

//   fModelParameterStore = NULL;

// }

// //////////////////////////////////////
// //////////////////////////////////////

// void LOCASOpticsModel::ModelSetup( const char* fileName )
// {

//   fModelParameterStore.AddParameters( fileName );

//   AllocateParameters();
//   InitialiseParameterIndices();
//   InitialiseParameters();

// }

// //////////////////////////////////////
// //////////////////////////////////////

// void LOCASOpticsModel::AllocateParameters()
// {

//   // Allocate the memory for the correct number of parameters in the model
//   Int_t nParams = fModelParameterStore.GetNParameters();
//   SetNParameters( nParams );
//   if ( fParameters != NULL ) { delete[] fParameters; }
//   else{ fParameters = new Double_t[ nParams ]; }

// }

// //////////////////////////////////////
// //////////////////////////////////////

// void LOCASOpticsModel::InitialiseParameterIndices()
// {

//   // Iterate through all the parameter objects and add their index values to the model
//   std::vector< LOCASModelParameter >::iterator iPar;

//   for ( iPar = fModelParameterStore.GetLOCASModelParametersIterBegin();
//         iPar != fModelParameterStore.GetLOCASModelParametersIterEnd();
//         iPar++ ){

//     if ( iPar->GetParameterName() == "par_scint" ){ SetScintParIndex( iPar->GetIndex() ); }
//     else if ( iPar->GetParameterName() == "par_av" ){ SetAVParIndex( iPar->GetIndex() ); }
//     else if ( iPar->GetParameterName() == "par_water" ){ SetWaterParIndex( iPar->GetIndex() ); }

//     else if ( iPar->GetParameterName() == "par_scint_rs" ){ SetScintRSParIndex( iPar->GetIndex() ); }
//     else if ( iPar->GetParameterName() == "par_av_rs" ){ SetAVRSParIndex( iPar->GetIndex() ); }
//     else if ( iPar->GetParameterName() == "par_water_rs" ){ SetWaterRSParIndex( iPar->GetIndex() ); }

//     else if ( iPar->GetParameterName() == "par_ang_resp_start" ){ 
//       SetAngularResponseParIndex( iPar->GetIndex() );
//       SetNAngularResponseBins( iPar->GetNInGroup() );
//     }
    
//     else if ( iPar->GetParameterName() == "par_ang_resp" ){ 
//       SetAngularResponseParIndex( iPar->GetIndex() );
//       SetNAngularResponseBins( iPar->GetNInGroup() );
//     }
    
//     else if ( iPar->GetParameterName() == "par_lb_dist_start" ){ 
//       SetLBDistributionParIndex( iPar->GetIndex() ); 
//       SetNLBDistributionBins( iPar->GetNInGroup() );
      
//       if ( iPar->GetNInGroup() % 36 == 0 ){
//         SetNLBDistributionThetaBins( GetNLBDistributionBins() / 36 );
//         SetNLBDistributionPhiBins( GetNLBDistributionBins() / GetNLBDistributionThetaBins()  );
//       }

//     }

//     else if( iPar->GetParameterName() == "par_lb_dist" ){
//       SetLBDistributionParIndex( iPar->GetIndex() );
//       SetNLBDistributionThetaBins( iPar->GetNInGroup() );
//       SetNLBDistributionPhiBins( iPar->GetNInGroup() );
//       SetNLBDistributionBins( iPar->GetNInGroup() );
//     }

//     else if ( iPar->GetParameterName() == "par_lb_dist_poly_start" ){ 
//       SetLBDistributionMaskParIndex( iPar->GetIndex() );
//       SetNAngularResponseBins( iPar->GetNInGroup() );
//     }

//     else if ( iPar->GetParameterName() == "par_lb_dist_poly" ){ 
//       SetLBDistributionMaskParIndex( iPar->GetIndex() );
//       SetNAngularResponseBins( iPar->GetNInGroup() );
//     }    

//   }

// }


// //////////////////////////////////////
// //////////////////////////////////////

// void LOCASOpticsModel::InitialiseParameters()
// {
  
//   // Initialise the values of the parameter objects to the entries in the fParameters object array
//   Int_t nParams = fModelParameterStore.GetNParameters();
  
//   for ( Int_t iPar = 0; iPar < nParams; iPar++ ){
//     fParameters[ iPar ] = 0.0;
//   }
  
//   std::vector< LOCASModelParameter >::iterator iPar;
  
//   for ( iPar = fModelParameterStore.GetLOCASModelParametersIterBegin();
//           iPar != fModelParameterStore.GetLOCASModelParametersIterEnd();
//         iPar++ ){
    
//     if ( ( iPar->GetParameterName() == "par_scint" ) && ( GetScintParIndex() >= 0 ) ){ SetScintPar( iPar->GetInitialValue() ); }
//     else if ( ( iPar->GetParameterName() == "par_av" ) && ( GetAVParIndex() >= 0 ) ){ SetAVPar( iPar->GetInitialValue() ); }
//     else if ( ( iPar->GetParameterName() == "par_water" ) && ( GetWaterParIndex() >= 0 ) ){ SetWaterPar( iPar->GetInitialValue() ); }
    
//     else if ( ( iPar->GetParameterName() == "par_scint_rs" ) && ( GetScintRSParIndex() >= 0 ) ){ SetScintRSPar( iPar->GetInitialValue() ); }
//     else if ( ( iPar->GetParameterName() == "par_av_rs" ) && ( GetAVRSParIndex() >= 0 ) ){ SetAVRSPar( iPar->GetInitialValue() ); }
//     else if ( ( iPar->GetParameterName() == "par_water_rs" ) && ( GetWaterRSParIndex() >= 0 ) ){ SetWaterRSPar( iPar->GetInitialValue() ); }
    
//   }
    
//   if ( GetAngularResponseParIndex() >= 0 ){
//     Float_t angle = 0.0;
//     for ( Int_t iT = 0; iT < GetNAngularResponseBins(); iT++ ){
      
//       angle = (Float_t)( iT + 0.5 ) * (Float_t)( 90.0 / GetNAngularResponseBins() );
      
//       if ( angle < 36.0 ){ SetPar( GetAngularResponseParIndex() + iT, 1.0 + ( 0.002222 * angle ) );  }
//       else{ SetPar( GetAngularResponseParIndex() + iT, 1.0 ); }
      
//     }
//   }
  
//   if ( GetLBDistributionParIndex() >= 0 ){
//     for ( Int_t iT = 0; iT < GetNLBDistributionBins(); iT++ ){ SetPar( GetLBDistributionParIndex() + iT, 1.0 ); }
//   }
  
//   if ( GetLBDistributionMaskParIndex() >= 0 ){
//     for ( Int_t iPoly = 0; iPoly < GetNLBDistributionMaskParameters(); iPoly++ ){ SetPar( GetLBDistributionMaskParIndex() + iPoly, 1.0 ); }
//   }
  
// }

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelPrediction( const LOCASDataPoint& dataPoint, Float_t* derivativePars )
{

  LOCASModelParameterStore* parPtr = GetLOCASModelParameterStore();
  Float_t angRespRatio = 1.0;
  Float_t intensityRatio = 1.0;

  Float_t dInnerAVExtinction = parPtr->GetInnerAVExtinctionLengthPar();
  Float_t dAVExtinction =  parPtr->GetAVExtinctionLengthPar();
  Float_t dWaterExtinction = parPtr->GetWaterExtinctionLengthPar();

  parPtr->SetCurrentLBRunNormalisationBin( dataPoint.GetRunIndex() );
  Float_t normVal = parPtr->GetLBRunNormalisationPar( dataPoint.GetRunIndex() );

  Float_t dInnerAV = dataPoint.GetDistInInnerAV() - dataPoint.GetCentralDistInInnerAV();
  Float_t dAV = dataPoint.GetDistInAV() - dataPoint.GetCentralDistInAV();
  Float_t dWater = dataPoint.GetDistInWater() - dataPoint.GetCentralDistInWater();

  Float_t angResp = ModelAngularResponse( dataPoint, "off-axis" );
  Float_t angRespCtr = ModelAngularResponse( dataPoint, "central" );
  angRespRatio = angResp / angRespCtr; 
  
  Float_t intensity = ModelLBDistribution( dataPoint, "off-axis" );
  Float_t intensityCtr = ModelLBDistribution( dataPoint, "central" );
  intensityRatio = intensity / intensityCtr;
  Float_t intensityPoly = ModelLBDistributionMask( dataPoint, "off-axis" );
  Float_t intensityCtrPoly = ModelLBDistributionMask( dataPoint, "central" );
  intensityRatio *= ( intensityPoly / intensityCtrPoly );
  
  
  Float_t modelPrediction = normVal * angRespRatio * intensityRatio * 
    TMath::Exp( - ( dInnerAV * ( dInnerAVExtinction )
                    + dAV * ( dAVExtinction )
                    + dWater * ( dWaterExtinction ) ) );

  // Derivatives
  if ( derivativePars != NULL ){

    derivativePars[ parPtr->GetInnerAVExtinctionLengthParIndex() ] = -dInnerAV;
    derivativePars[ parPtr->GetAVExtinctionLengthParIndex() ] = -dAV;
    derivativePars[ parPtr->GetWaterExtinctionLengthParIndex() ] = -dWater;

    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCurrentPMTAngularResponseBin() ] = 0.0;
    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCurrentPMTAngularResponseBin() ] = 1.0 / angResp;

    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCentralCurrentPMTAngularResponseBin() ] = 0.0;
    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCentralCurrentPMTAngularResponseBin() ] -= 1.0 / angRespCtr;

    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCurrentLBDistributionBin() ] = 0.0;
    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCurrentLBDistributionBin() ] = 1.0 / intensity;

    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCentralCurrentLBDistributionBin() ] = 0.0;
    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCentralCurrentLBDistributionBin() ] -= 1.0 / intensityCtr;

    derivativePars[ parPtr->GetLBRunNormalisationParIndex() + parPtr->GetCurrentLBRunNormalisationBin() ] = 1.0 / normVal;

    Double_t* parMask = new Double_t[ 1 + parPtr->GetNLBDistributionMaskParameters() ];
    Double_t* lbCTheta = new Double_t( TMath::Cos( dataPoint.GetLBTheta() ) );
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){ parMask[ 1 + iVal ] = parPtr->GetLBDistributionMaskPar( iVal ); }
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){
      parMask[ 0 ] = (Double_t)iVal;
      derivativePars[ parPtr->GetLBDistributionMaskParIndex() + iVal ] = ModelLBDistributionMaskDeriviative( lbCTheta, parMask ) / intensityPoly;
    }
    delete [] parMask;
    delete lbCTheta;

    Double_t* parMaskCtr = new Double_t[ 1 + parPtr->GetNLBDistributionMaskParameters() ];
    Double_t* lbCThetaCtr = new Double_t( TMath::Cos( dataPoint.GetCentralLBTheta() ) );
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){ parMaskCtr[ 1 + iVal ] = parPtr->GetLBDistributionMaskPar( iVal ); }
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){
      parMaskCtr[ 0 ] = (Double_t)iVal;
      derivativePars[ parPtr->GetLBDistributionMaskParIndex() + iVal ] -= ModelLBDistributionMaskDeriviative( lbCTheta, parMaskCtr ) / intensityCtrPoly;
    }
    delete [] parMaskCtr;
    delete lbCThetaCtr;

    parPtr->IdentifyVaryingParameters();
    Int_t* variableParameterIndex = parPtr->GetVariableParameterIndex();
    for ( Int_t iPar = 1; iPar <= parPtr->GetNCurrentVariableParameters(); iPar++ ){
      derivativePars[ variableParameterIndex[ iPar ] ] *= modelPrediction;
    }

  }
  
  return modelPrediction;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelLBDistribution( const LOCASDataPoint& dataPoint, std::string runType )
{
  LOCASModelParameterStore* parPtr = GetLOCASModelParameterStore();
  Float_t lbRelTheta, lbRelPhi = 0.0;
  
  if ( runType == "off-axis" ){
    lbRelTheta = dataPoint.GetLBTheta();
    lbRelPhi = dataPoint.GetLBPhi();
  }

  else if ( runType == "central" ){
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

  
  Int_t iTheta = (Int_t)( ( 1 + cosTheta ) / 2 * parPtr->GetNLBDistributionCosThetaBins() );
  if ( iTheta < 0 ) iTheta = 0;
  if ( iTheta >= parPtr->GetNLBDistributionCosThetaBins() ) iTheta = parPtr->GetNLBDistributionCosThetaBins() - 1;

  Int_t iPhi = (Int_t)( phi / ( 2 * TMath::Pi() ) * parPtr->GetNLBDistributionPhiBins() );
  if ( iPhi < 0 ) iPhi = 0;
  if ( iPhi >= parPtr->GetNLBDistributionPhiBins() ) iPhi = parPtr->GetNLBDistributionPhiBins() - 1;

  Int_t iLBDist = ( iTheta ) * ( parPtr->GetNLBDistributionPhiBins() + iPhi );
  if ( runType == "off-axis" ){ parPtr->SetCurrentLBDistributionBin( iLBDist ); }
  else if ( runType == "central" ){ parPtr->SetCentralCurrentLBDistributionBin( iLBDist ); }

  Float_t laserlight = parPtr->GetLBDistributionPar( iLBDist );
  
  return laserlight;

}

//////////////////////////////////////
//////////////////////////////////////
 
Float_t LOCASOpticsModel::ModelLBDistributionMask( const LOCASDataPoint& dataPoint, std::string runType )
{

  LOCASModelParameterStore* parPtr = GetLOCASModelParameterStore();
  Float_t lbRelTheta = 0.0;
  
  if ( runType == "off-axis" ){ lbRelTheta = dataPoint.GetLBTheta(); }
  else if ( runType == "central" ){ lbRelTheta = dataPoint.GetCentralLBTheta(); }
  
  Float_t polynomialVal = 1.0;

  for ( Int_t iPar = 0; iPar < parPtr->GetNLBDistributionMaskParameters(); iPar++ ){

    polynomialVal += ( ( parPtr->GetLBDistributionMaskPar( iPar ) )
                       * ( TMath::Power( ( 1 + TMath::Cos( lbRelTheta ) ), ( iPar + 1 ) ) ) );
    
  }

  return polynomialVal;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelLBDistributionMaskDeriviative( Double_t* aPtr, Double_t* parPtr )
{
  
  Int_t iPar = (Int_t)parPtr[ 0 ];
  Double_t cosTheta = aPtr[ 0 ];
  
  Double_t onePlus = 1.0 + cosTheta;
  
  Double_t dlbM = TMath::Power( onePlus, iPar );
  
  return dlbM;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelAngularResponse( const LOCASDataPoint& dataPoint, std::string runType )
{

  LOCASModelParameterStore* parPtr = GetLOCASModelParameterStore();
  Float_t cosTheta = 0.0;
  
  if ( runType == "off-axis" ) { cosTheta = TMath::Cos( dataPoint.GetIncidentAngle() / ( 180.0 / TMath::Pi() ) ); }
  if ( runType == "central" ) { cosTheta = TMath::Cos( dataPoint.GetCentralIncidentAngle() / ( 180.0 / TMath::Pi() ) ); }

  Float_t angle = ( TMath::ACos( cosTheta ) * ( 180.0 / TMath::Pi() ) );

  Int_t iAng = (Int_t)( angle * parPtr->GetNPMTAngularResponseBins() / 90.0 );
  if ( runType == "off-axis" ){ parPtr->SetCurrentPMTAngularRepsonseBin( iAng ); }
  else if ( runType == "central" ){ parPtr->SetCentralCurrentPMTAngularResponseBin( iAng ); }
  
  return parPtr->GetPMTAngularResponsePar( iAng );

}
