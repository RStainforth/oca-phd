#include "OCAModelParameterStore.hh"
#include "OCAOpticsModel.hh"
#include "OCAModelParameter.hh"
#include "OCAPMT.hh"

#include <iostream>

#include "TMath.h"

#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"

using namespace OCA;
using namespace std;

ClassImp( OCAOpticsModel )

//////////////////////////////////////
//////////////////////////////////////

OCAOpticsModel::OCAOpticsModel( const string modelName )
{

  // Set the model name.
  fModelName = modelName;

  // Set the default values for the minimum number of entries per
  // PMT angular response bin and laserball distribution bin.
  // The following values are the traditional ones used
  // in the original SNO optics fit. The user can overide
  // these values using the OCAModel::SetRequiredNPMTAngularResponseEntries
  // and OCAModel::SetRequiredNLBDistributionEntries 'setters'.
  fRequiredNPMTAngularResponseEntries = 25;
  fRequiredNLBDistributionEntries = 10;

  // Initialise the pointer to the parameter storage object
  // to 'NULL' to begin with.
  fModelParameterStore = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAOpticsModel::IdentifyVaryingPMTAngularResponseBins( OCAPMTStore* lData )
{

  // Obtain the number of PMT angular response bins.
  Int_t nPMTResponseBins = fModelParameterStore->GetNPMTAngularResponseBins();

  // Create an array which will, for each bin, store the number of entries
  // for the corresponding PMT angular response bin of the same index.
  Int_t* pmtAngValid = new Int_t[ nPMTResponseBins ];

  // Initialise each value in the array to 0 to begin with.
  for ( Int_t iAng = 0; iAng < nPMTResponseBins; iAng++ ){ 
    pmtAngValid[ iAng ] = 0; 
  }

  Float_t interpolFrac = 0.0;
  // Loop over all the data points stored in the OCAPMTStore object.
  std::vector< OCAPMT >::iterator iDP;
  for ( iDP = lData->GetOCAPMTsIterBegin(); 
        iDP != lData->GetOCAPMTsIterEnd(); 
        iDP++ ) {

    // We need to model the PMT angular response first to assign the
    // current PMT angular response bin for this data point.
    // i.e. we need to assign fCurrentPMTAngularResponseBin.
    ModelAngularResponse( *iDP, "off-axis", interpolFrac );

    // For the corresponding entry in the array, i.e. the counter,
    // increment it's value by one.
    pmtAngValid[ fModelParameterStore->GetCurrentPMTAngularResponseBin() ]++;

  }

  // Now loop over all the entries checking how many entries are
  // there for each bin. If the number of entries
  // exceeds the minimum required amount then set that parameter
  // to vary. If not, keep that particular bin in the PMT Angular
  // response fixed. In the special instance that the bin is the
  // zeroth degree bin, keep it always fixed, regardless of how
  // many entries it has.
  for ( Int_t iAng = 0; iAng < nPMTResponseBins; iAng++ ){

    Bool_t parVary = fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetPMTAngularResponseParIndex() + iAng ];

    // If the number of entries is less than the requirement,
    // or if the bin is the first in the PMT angular response
    // distribution ( i.e. iAng = 0 ) then keep the associated
    // parameter fixed in the fit.
    if ( parVary ){
      if ( ( pmtAngValid[ iAng ] < fRequiredNPMTAngularResponseEntries 
             || iAng == 0 ) ){ 
        fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetPMTAngularResponseParIndex() + iAng ] = 0;
      }
      
      // If the number of entries exceeds the required amount
      // then vary the associated parameter in the parameter array.
      else{
        fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetPMTAngularResponseParIndex() + iAng ] = 1;
      }
    }
    else{
      fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetPMTAngularResponseParIndex() + iAng ] = 0;
    }
      

  }

  delete pmtAngValid;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAOpticsModel::IdentifyVaryingLBDistributionBins( OCAPMTStore* lData )
{

  // Obtain the number of laserball distribution bins.
  Int_t nLBDistBins = fModelParameterStore->GetNLBDistributionCosThetaBins() * fModelParameterStore->GetNLBDistributionPhiBins();

  // Create an array which will, for each bin, store the number of entries
  // for the corresponding laserball distribution bin of the same index.
  Int_t* lbAngValid = new Int_t[ nLBDistBins ];

  // Initialise each value in the array to 0 to begin with. 
  for ( Int_t iLB = 0; iLB < nLBDistBins; iLB++ ){ 
    lbAngValid[ iLB ] = 0; 
  }

  // Loop over all the data points stored in the OCAPMTStore object.
  std::vector< OCAPMT >::iterator iDP;
  for ( iDP = lData->GetOCAPMTsIterBegin(); 
        iDP != lData->GetOCAPMTsIterEnd(); 
        iDP++ ) {

    // We need to model the laserball distribution first to assign the
    // current laserball distribution bin for this data point.
    // i.e. we need to assign fCurrentLBDistributionBin.
    Float_t phiFrac[ 2 ];
    Float_t cosThetaFrac[ 2 ];
    Int_t binsToDiff[ 4 ];
    ModelLBDistribution( *iDP, "off-axis", phiFrac, cosThetaFrac, binsToDiff );

    // For the corresponding entry in the array, i.e. the counter,
    // increment it's value by one.
    lbAngValid[ fModelParameterStore->GetCurrentLBDistributionBin() ]++;
    
  }

  // Now loop over all the entries checking how many entries are
  // there for each bin. If the number of entries
  // exceeds the minimum required amount then set that parameter
  // to vary. If not, keep that particular bin in the laserball
  // distribution fixed.
  for ( Int_t iLB = 0; iLB < nLBDistBins; iLB++ ){

    Bool_t parVary = fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBDistributionParIndex() + iLB ];

    // If the number of entries is less than the requirement,
    // or if the bin is the first in the laserball
    // distribution ( i.e. iLB = 0 ) then keep the associated
    // parameter fixed in the fit.

    if ( parVary ){
      if ( lbAngValid[ iLB ] < fRequiredNLBDistributionEntries ){ 
        fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBDistributionParIndex() + iLB ] = 0;
      }
      
      // If the number of entries exceeds the required amount
      // then vary the associated parameter in the parameter array.
      else{
        fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBDistributionParIndex() + iLB ] = 1;
      }
    }
    else{
      fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBDistributionParIndex() + iLB ] = 0;
    }
      
  }

  delete lbAngValid;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAOpticsModel::InitialiseLBRunNormalisations( OCAPMTStore* lData )
{

  Int_t previousRunIndex = -1;
  // Loop over all the data points stored in the OCAPMTStore object.
  std::vector< OCAPMT >::iterator iDP;
  for ( iDP = lData->GetOCAPMTsIterBegin(); 
        iDP != lData->GetOCAPMTsIterEnd(); 
        iDP++ ) {
    
    if ( iDP->GetRunIndex() != previousRunIndex ){
      previousRunIndex = iDP->GetRunIndex();
      fModelParameterStore->SetLBRunNormalisationPar( iDP->GetRunIndex(),
                                                      iDP->GetLBIntensityNorm() );

      Bool_t parVary = fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBRunNormalisationParIndex() + iDP->GetRunIndex() ];
      if ( parVary ){
        fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBRunNormalisationParIndex() + iDP->GetRunIndex() ] = 1;
      }
      else{
        fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBRunNormalisationParIndex() + iDP->GetRunIndex() ] = 0;
      }
        
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAOpticsModel::ModelOccRatioPrediction( const OCAPMT& dataPoint, Float_t* derivativePars )
{

  // Obtain a pointer to the parameters for use in this calculation
  // for the model prediction of the occupancy ratio.
  OCAModelParameterStore* parPtr = GetOCAModelParameterStore();

  vector< Int_t > bins;
  parPtr->SetCurrentLBDistributionBins( bins );
  parPtr->SetCurrentAngularResponseBins( bins );

  // Initialise the off-axis / central ratios for the laserball 
  // distribution and the PMT angular response to 1.0 to begin with.
  // Note: intensity = laserball isotropy * laserball mask.
  Float_t angRespRatio = 1.0;
  Float_t intensityRatio = 1.0;

  // Obtain the current values for the extinction lengths in the
  // inner AV, AV and water regions. The extinction lengths
  // are stored in inverse lengths, mm^-1.
  Float_t dInnerAVExtinction = parPtr->GetInnerAVExtinctionLengthPar();
  Float_t dAVExtinction =  parPtr->GetAVExtinctionLengthPar();
  Float_t dWaterExtinction = parPtr->GetWaterExtinctionLengthPar();

  // For this data point use the run index to assign the correct
  // laserball normalisation for the off-axis run. All the
  // central normalisation values are used in the data value
  // for the occupancy ratio.
  parPtr->SetCurrentLBRunNormalisationBin( dataPoint.GetRunIndex() );

  // Use the obtained normalisation bin obtain the current value for the
  // intensity normalisation for the off-axis run.
  Float_t normVal = parPtr->GetLBRunNormalisationPar( dataPoint.GetRunIndex() );
  Float_t normCtrVal = dataPoint.GetCentralLBIntensityNorm();
  // Obtain the differences between the off-axis and central
  // runs for the distances in the inner AV, AV and water regions.
  Float_t dInnerAV = dataPoint.GetDistInInnerAV() 
    - dataPoint.GetCentralDistInInnerAV();
  Float_t dAV = dataPoint.GetDistInAV() 
    - dataPoint.GetCentralDistInAV();
  Float_t dWater = dataPoint.GetDistInWater() 
    - dataPoint.GetCentralDistInWater();

  Float_t interpolFrac = 0.0;
  Float_t interpolFracCtr = 0.0;
  // Model the angular response for this data point for the
  // off-axis and central runs.
  Float_t angResp = ModelAngularResponse( dataPoint, "off-axis", interpolFrac );
  Float_t angRespCtr = ModelAngularResponse( dataPoint, "central", interpolFracCtr );

  // Compute the PMT angular response ratio.
  angRespRatio = angResp / angRespCtr;
  
  // Model the laserball distribution for this data point
  // for the off-axis and central runs.
  Float_t phiFrac[ 2 ];
  Float_t cosThetaFrac[ 2 ];
  Int_t binsToDiff[ 4 ];
  Float_t phiFracCtr[ 2 ];
  Float_t cosThetaFracCtr[ 2 ];
  Int_t binsToDiffCtr[ 4 ];
  Float_t intensity = ModelLBDistribution( dataPoint, "off-axis", phiFrac, cosThetaFrac, binsToDiff );
  Float_t intensityCtr = ModelLBDistribution( dataPoint, "central", phiFracCtr, cosThetaFracCtr, binsToDiffCtr );

  // Compute the ratio for the isotropy part.
  intensityRatio = intensity / intensityCtr;

  // Model the laserball distribution mask for this data point
  // for the off-axis and central runs.
  Float_t intensityPoly = ModelLBDistributionMask( dataPoint, "off-axis" );
  Float_t intensityCtrPoly = ModelLBDistributionMask( dataPoint, "central" );

  // Modify the laserball distribution ratio by this ratio
  // of the mask function values.
  intensityRatio *= ( intensityPoly / intensityCtrPoly );
  
  // Using all the above calculated values we can compute
  // the model prediction for the occuapncy ratio.
  Float_t modelPrediction = ( normVal / normCtrVal ) * angRespRatio * intensityRatio * 
    TMath::Exp( - ( dInnerAV * ( dInnerAVExtinction )
                    + dAV * ( dAVExtinction )
                    + dWater * ( dWaterExtinction ) ) );

  // If the 'derivativePars' array was specified
  // as part of this method call i.e. the 'derivativePars' pointer
  // exists and is not 'NULL', compute the derivatives
  // with respect to the individual model components.

  if ( derivativePars != NULL ){

    // The derivatives with respect to the extinction lengths in the
    // inner AV, AV and water regions.
    derivativePars[ parPtr->GetInnerAVExtinctionLengthParIndex() ] = -dInnerAV;
    derivativePars[ parPtr->GetAVExtinctionLengthParIndex() ] = -dAV;
    derivativePars[ parPtr->GetWaterExtinctionLengthParIndex() ] = -dWater;

    // The derivative with respect to the PMT angular response
    // from the off-axis run.
    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCurrentPMTAngularResponseBin() ] = 0.0;
    //derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCurrentPMTAngularResponseBin() + 1 ] = 0.0;
    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCurrentPMTAngularResponseBin() ] = 1.0 /  angResp;
    //derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCurrentPMTAngularResponseBin() + 1 ] = interpolFrac / angResp;
    //cout << "( 1.0 - interpolFrac ) / angResp is: "  << ( 1.0 - interpolFrac ) / angResp << endl;
    //cout << "interpolFrac / angResp is: "  << interpolFrac / angResp << endl;
    //cout << "PMT Angular Sum: " << interpolFrac + ( 1.0 - interpolFrac ) << endl;
    // The derivative with respect to the PMT angular response
    // from the central run.
    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCentralCurrentPMTAngularResponseBin() ] = 0.0;
    //derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCentralCurrentPMTAngularResponseBin() + 1 ] = 0.0;
    derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCentralCurrentPMTAngularResponseBin() ] -= 1.0 / angRespCtr;
    //derivativePars[ parPtr->GetPMTAngularResponseParIndex() + parPtr->GetCentralCurrentPMTAngularResponseBin() + 1 ] -= interpolFracCtr /  angRespCtr;
    //cout << "( 1.0 - interpolFracCtr ) / angRespCtr is: "  << ( 1.0 - interpolFracCtr ) / angRespCtr << endl;
    //cout << "interpolFracCtr / angRespCtr is: "  << interpolFracCtr / angRespCtr << endl;
    //cout << "------------------" << endl;
    // The derivative with respect to the laserball 
    // isotropy distribution from the off-axis run.
    // derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCurrentLBDistributionBin() ] = 0.0;
    // derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCurrentLBDistributionBin() ] = 1.0 / intensity;
    //if ( phiFrac[ 0 ] == 0.0 && phiFrac[ 1 ] == 0.0 && cosThetaFrac[ 0 ] == 0.0 && cosThetaFrac[ 1 ] == 0.0 ){
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 0 ] ] = 1.0 / intensity;
      //}
    //else{
    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCurrentLBDistributionBin() ] = 0.0;
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 1 ] ] = 0.0;
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 2 ] ] = 0.0;
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 3 ] ] = 0.0;

    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCurrentLBDistributionBin() ] = 1.0 / intensity; 
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 1 ] ] = ( phiFrac[ 1 ] * cosThetaFrac[ 0 ] ) / ( intensity );     
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 2 ] ] = ( phiFrac[ 0 ] * cosThetaFrac[ 1 ] ) / ( intensity );
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiff[ 3 ] ] = ( phiFrac[ 1 ] * cosThetaFrac[ 1 ] ) / ( intensity );
    //cout << "diffOne: " << ( phiFrac[ 0 ] * cosThetaFrac[ 0 ] ) / ( intensity ) << " and recip: " << 1.0/intensity << " and intensity: " << intensity << endl;
    //cout << "diffTwo: " << ( phiFrac[ 1 ] * cosThetaFrac[ 0 ] ) / ( intensity ) << " and recip: " << 1.0/intensity << endl;
    //cout << "diffThree: " << ( phiFrac[ 0 ] * cosThetaFrac[ 1 ] ) / ( intensity ) << " and recip: " << 1.0/intensity << endl;
    //cout << "diffFour: " << ( phiFrac[ 1 ] * cosThetaFrac[ 1 ] ) / ( intensity ) << " and recip: " << 1.0/intensity << endl;
    //cout << "Total Sum: " << (phiFrac[ 0 ] * cosThetaFrac[ 0 ]) + (phiFrac[ 0 ] * cosThetaFrac[ 1 ]) + (phiFrac[ 1 ] * cosThetaFrac[ 0 ]) + (phiFrac[ 1 ] * cosThetaFrac[ 1 ]) << endl;
    //cout << "-------------" << endl;
      //}
    // The derivative with respect to the laserball 
    // isotropy distribution from the central run.
    // derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCentralCurrentLBDistributionBin() ] = 0.0;
    // derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCentralCurrentLBDistributionBin() ] -= 1.0 / intensityCtr;
    //if ( phiFracCtr[ 0 ] == 0.0 && phiFracCtr[ 1 ] == 0.0 && cosThetaFracCtr[ 0 ] == 0.0 && phiFracCtr[ 1 ] == 0.0 ){
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 0 ] ] -= 1.0 / intensityCtr;
      //}  
    //else{
    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCentralCurrentLBDistributionBin() ] = 0.0;
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 1 ] ] = 0.0;
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 2 ] ] = 0.0;
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 3 ] ] = 0.0;

    derivativePars[ parPtr->GetLBDistributionParIndex() + parPtr->GetCentralCurrentLBDistributionBin() ] -= 1.0 / intensityCtr; 
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 1 ] ] -= ( phiFracCtr[ 1 ] * cosThetaFracCtr[ 0 ] ) / ( intensityCtr );     
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 2 ] ] -= ( phiFracCtr[ 0 ] * cosThetaFracCtr[ 1 ] ) / ( intensityCtr );
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 3 ] ] -= ( phiFracCtr[ 1 ] * cosThetaFracCtr[ 1 ] ) / ( intensityCtr );
    //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 0 ] ] -= 1.0 / ( intensityCtr ); 
      //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 1 ] ] -= ( phiFracCtr[ 1 ] * cosThetaFracCtr[ 0 ] ) / ( intensityCtr );     
      //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 2 ] ] -= ( phiFracCtr[ 0 ] * cosThetaFracCtr[ 1 ] ) / ( intensityCtr );
      //derivativePars[ parPtr->GetLBDistributionParIndex() + binsToDiffCtr[ 3 ] ] -= ( phiFracCtr[ 1 ] * cosThetaFracCtr[ 1 ] ) / ( intensityCtr );
      //}

    // The derivative with respect to the run normalisation
    // from the off-axis run.
    derivativePars[ parPtr->GetLBRunNormalisationParIndex() + parPtr->GetCurrentLBRunNormalisationBin() ] = 1.0 / normVal;

    // Now we compute the derivative with respect to 
    // each of the parameters in the laserball distribution
    // mask from the off-axis run.
    Double_t* parMask = new Double_t[ 1 + parPtr->GetNLBDistributionMaskParameters() ];
    Double_t* lbCTheta = new Double_t( TMath::Cos( dataPoint.GetRelLBTheta() ) );
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){ parMask[ 1 + iVal ] = parPtr->GetLBDistributionMaskPar( iVal ); }
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){
      parMask[ 0 ] = (Double_t)iVal;
      derivativePars[ parPtr->GetLBDistributionMaskParIndex() + iVal ] = ModelLBDistributionMaskDeriviative( lbCTheta, parMask ) / intensityPoly;
    }
    delete [] parMask;
    delete lbCTheta;

    // Now we compute the derivative with respect to 
    // each of the parameters in the laserball distribution
    // mask from the central run.
    Double_t* parMaskCtr = new Double_t[ 1 + parPtr->GetNLBDistributionMaskParameters() ];
    Double_t* lbCThetaCtr = new Double_t( TMath::Cos( dataPoint.GetCentralRelLBTheta() ) );
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){ parMaskCtr[ 1 + iVal ] = parPtr->GetLBDistributionMaskPar( iVal ); }
    for ( Int_t iVal = 0; iVal < parPtr->GetNLBDistributionMaskParameters(); iVal++ ){
      parMaskCtr[ 0 ] = (Double_t)iVal;
      derivativePars[ parPtr->GetLBDistributionMaskParIndex() + iVal ] -= ModelLBDistributionMaskDeriviative( lbCTheta, parMaskCtr ) / intensityCtrPoly;
    }
    delete [] parMaskCtr;
    delete lbCThetaCtr;

    // Identify all the current varying parameters associated
    // with this data point.
    //cout << "derivative Call" << endl;
    parPtr->IdentifyVaryingParameters();

    // For each of the parameter which vary for this
    // data point multiply each of the derviatives by the 
    // model prediction value, thus completing the derivative
    // calculation.
    Int_t* variableParameterIndex = parPtr->GetVariableParameterIndex();
    for ( Int_t iPar = 1; iPar <= parPtr->GetNCurrentVariableParameters(); iPar++ ){
      derivativePars[ variableParameterIndex[ iPar ] ] *= modelPrediction;
    }

  }

  return modelPrediction;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAOpticsModel::ModelPrediction( const OCAPMT& dataPoint )
{
  
  
  // Obtain a pointer to the parameters for use in this calculation
  // for the model prediction of the occupancy ratio.
  OCAModelParameterStore* parPtr = GetOCAModelParameterStore();

  // Obtain the current values for the extinction lengths in the
  // inner AV, AV and water regions. The extinction lengths
  // are stored in inverse lengths, mm^-1.
  Float_t dInnerAVExtinction = parPtr->GetInnerAVExtinctionLengthPar();
  Float_t dAVExtinction =  parPtr->GetAVExtinctionLengthPar();
  Float_t dWaterExtinction = parPtr->GetWaterExtinctionLengthPar();

  // Use the obtained normalisation bin obtain the current value for the
  // intensity normalisation for the off-axis run.
  Float_t normVal = parPtr->GetLBRunNormalisationPar( dataPoint.GetRunIndex() );
  //normVal *= dataPoint.GetCentralLBIntensityNorm();

  // Obtain the distances from the off-axis 
  // runs for the distances in the inner AV, AV and water regions.
  Float_t dInnerAV = dataPoint.GetDistInInnerAV();
  Float_t dAV = dataPoint.GetDistInAV();
  Float_t dWater = dataPoint.GetDistInWater();

  // Get the solid angle and fresnel transmission coefficients.
  Float_t solidAngle = dataPoint.GetSolidAngle();
  Float_t fresnelTCoeff = dataPoint.GetFresnelTCoeff();

  // Model the angular response for this data point for the
  // off-axis run.
  Float_t interpolFrac = 0.0;
  Float_t angResp = ModelAngularResponse( dataPoint, "off-axis", interpolFrac );
  
  // Model the laserball distribution for this data point
  // for the off-axis run.
  Float_t phiFrac[ 2 ];
  Float_t cosThetaFrac[ 2 ];
  Int_t binsToDiff[ 4 ];
  Float_t intensity = ModelLBDistribution( dataPoint, "off-axis", phiFrac, cosThetaFrac, binsToDiff );

  // Model the laserball distribution mask for this data point
  // for the off-axis run.
  Float_t intensityPoly = ModelLBDistributionMask( dataPoint, "off-axis" );

  // Modify the laserball distribution by this the mask function values.
  intensity *= intensityPoly;
  
  // Using all the above calculated values we can compute
  // the model prediction.
  Float_t modelPrediction = normVal * angResp * intensity * solidAngle * fresnelTCoeff
    * TMath::Exp( - ( dInnerAV * ( dInnerAVExtinction )
                      + dAV * ( dAVExtinction )
                      + dWater * ( dWaterExtinction ) ) );

  return modelPrediction;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAOpticsModel::ModelLBDistribution( const OCAPMT& dataPoint, std::string runType, 
                                             Float_t* phiInterpolFrac, Float_t* cosThetaInterpolFrac, 
                                             Int_t* binsToDiff )
{

  // Obtain a pointer to the parameters for use in this calculation
  // for the laserball isotropy distribution model.
  OCAModelParameterStore* parPtr = GetOCAModelParameterStore();

  // Initialise the values of theta and phi; the angles
  // relative to the local laserball frame. In this local
  // laserball frame the direction of the laserball axis
  // defines the z-axis, pointing upwards along the
  // laserball body.
  Float_t lbRelTheta = 0.0;
  Float_t lbRelPhi = 0.0;
  
  // Depending on whether the off-axis or central run
  // options are specified, assign the theta and phi values
  // accordingly.
  if ( runType == "off-axis" ){
    lbRelTheta = dataPoint.GetRelLBTheta();
    lbRelPhi = dataPoint.GetRelLBPhi();
  }

  else if ( runType == "central" ){
    lbRelTheta = dataPoint.GetCentralRelLBTheta();
    lbRelPhi = dataPoint.GetCentralRelLBPhi();
  }

  // Compute the cos theta for the theta coordinate.
  Float_t cosTheta = TMath::Cos( lbRelTheta );
  Float_t phi = lbRelPhi;

  // Some book keeping to ensure that the cosTheta
  // and phi coordinates all have sensible values.
  if ( cosTheta > 1.0 ) cosTheta = 1.0;
  else if ( cosTheta < -1.0 ) cosTheta = -1.0;

  if ( phi > TMath::Pi() ) phi -= 2 * TMath::Pi();
  else if ( phi < -1.0 * TMath::Pi() ) phi += 2 * TMath::Pi();

  // Compute the bin for this value of 'cosTheta'
  
  Int_t iTheta = (Int_t)( ( 1 + cosTheta ) / 2.0 * parPtr->GetNLBDistributionCosThetaBins() );

  // Some book keeping to ensure the cosTheta bin is within the allowed
  // number.
  if ( iTheta < 0 ){ iTheta = 0; }
  if ( iTheta >= parPtr->GetNLBDistributionCosThetaBins() ){
    iTheta = parPtr->GetNLBDistributionCosThetaBins() - 1;
  }

  // Compute the bin for this value of 'phi'
  Int_t iPhi = (Int_t)( ( phi + TMath::Pi() ) / ( 2 * TMath::Pi() ) * parPtr->GetNLBDistributionPhiBins() );

  // Some book keeping to ensure the cosTheta bin is within the allowed
  // number.
  if ( iPhi < 0 ){ iPhi = 0; }
  if ( iPhi >= parPtr->GetNLBDistributionPhiBins() ){
    iPhi = parPtr->GetNLBDistributionPhiBins() - 1;
  }

  // Begin 2D interpolation

  // Using the individual phi and cosTheta bins calculate
  // the overall bin for which the associated parameter from the
  // parameter array can be obtained.
  Int_t iLBDist = iTheta * parPtr->GetNLBDistributionPhiBins() + iPhi;

  // Depending on the run type specified, assign this overall bin
  // value to the current values held by the OCAModelParameterStore
  // object.
  if ( runType == "off-axis" ){ 
    parPtr->SetCurrentLBDistributionBin( iLBDist ); 
  }
  else if ( runType == "central" ){ 
    parPtr->SetCentralCurrentLBDistributionBin( iLBDist ); 
  }

  //binsToDiff[ 0 ] = iLBDist;
  parPtr->GetCurrentLBDistributionBins()->push_back( parPtr->GetLBDistributionParIndex() + iLBDist );
  return parPtr->GetLBDistributionPar( iLBDist );

  //vector< Int_t > bins;
  //parPtr->GetCurrentLBDistributionBins()->insert( parPtr->GetCurrentLBDistributionBins()->begin(), bins.begin(), bins.end() );

  Int_t iLBDistPhiInterpol = iTheta * parPtr->GetNLBDistributionPhiBins() + ( iPhi + 1 );
  Int_t iLBDistThetaInterpol = ( iTheta + 1 ) * parPtr->GetNLBDistributionPhiBins() + iPhi;
  Int_t iLBDistPhiThetaInterpol =  ( iTheta + 1 ) * parPtr->GetNLBDistributionPhiBins() + ( iPhi + 1 );
  if ( iPhi + 1 >= parPtr->GetNLBDistributionPhiBins() 
       &&
       iTheta + 1 >= parPtr->GetNLBDistributionCosThetaBins() ){
    iLBDistPhiThetaInterpol = ( iTheta ) * parPtr->GetNLBDistributionPhiBins() + iPhi;
  }
  if ( iPhi + 1 >= parPtr->GetNLBDistributionPhiBins()
       &&
       iTheta + 1 < parPtr->GetNLBDistributionCosThetaBins() ){
    iLBDistPhiInterpol = iTheta * parPtr->GetNLBDistributionPhiBins() + iPhi;
    iLBDistPhiThetaInterpol =  ( iTheta + 1 ) * parPtr->GetNLBDistributionPhiBins() + iPhi;
  }
  if ( iPhi + 1 < parPtr->GetNLBDistributionPhiBins()
       &&
       iTheta + 1 >= parPtr->GetNLBDistributionCosThetaBins() ){ 
    iLBDistPhiThetaInterpol = ( iTheta ) * parPtr->GetNLBDistributionPhiBins() + ( iPhi + 1 );
    iLBDistThetaInterpol = ( iTheta ) * parPtr->GetNLBDistributionPhiBins() + iPhi;
  }

  binsToDiff[ 0 ] = iLBDist;
  binsToDiff[ 1 ] = iLBDistPhiInterpol;
  binsToDiff[ 2 ] = iLBDistThetaInterpol;
  binsToDiff[ 3 ] = iLBDistPhiThetaInterpol;

  Float_t phiVal = ( ( phi + TMath::Pi() ) / ( 2 * TMath::Pi() ) * parPtr->GetNLBDistributionPhiBins() );
  Float_t cosThetaVal = ( ( 1 + cosTheta ) / 2.0 * parPtr->GetNLBDistributionCosThetaBins() );

  phiInterpolFrac[ 0 ] = ( ceil( phiVal ) - phiVal ) / ( ceil( phiVal ) - floor( phiVal ) );
  phiInterpolFrac[ 1 ] = ( phiVal - floor( phiVal ) ) / ( ceil( phiVal ) - floor( phiVal ) );
  cosThetaInterpolFrac[ 0 ] = ( ceil( cosThetaVal ) - cosThetaVal ) / ( ceil( cosThetaVal ) - floor ( cosThetaVal ) );
  cosThetaInterpolFrac[ 1 ] = ( cosThetaVal - floor( cosThetaVal ) ) / ( ceil( cosThetaVal ) - floor ( cosThetaVal ) );
  
  if ( !parPtr->GetParametersVary()[ parPtr->GetLBDistributionParIndex() + iLBDist ]
       || !parPtr->GetParametersVary()[ parPtr->GetLBDistributionParIndex() + iLBDistPhiInterpol ]
       || !parPtr->GetParametersVary()[ parPtr->GetLBDistributionParIndex() + iLBDistThetaInterpol ]
       || !parPtr->GetParametersVary()[ parPtr->GetLBDistributionParIndex() + iLBDistPhiThetaInterpol ]
       || std::isnan( phiInterpolFrac[ 0 ] * phiInterpolFrac[ 1 ] * cosThetaInterpolFrac[ 0 ] * cosThetaInterpolFrac[ 1 ] )
       || iLBDist == iLBDistPhiInterpol
       || iLBDist == iLBDistThetaInterpol 
       || iLBDist == iLBDistPhiThetaInterpol
       || iLBDistPhiInterpol == iLBDistThetaInterpol
       || iLBDistPhiInterpol == iLBDistPhiThetaInterpol
       || iLBDistThetaInterpol == iLBDistPhiThetaInterpol ){
    
    phiInterpolFrac[ 0 ] = 1.0;
    phiInterpolFrac[ 1 ] = 0.0;
    cosThetaInterpolFrac[ 0 ] = 1.0;
    cosThetaInterpolFrac[ 1 ] = 0.0;
    parPtr->GetCurrentLBDistributionBins()->push_back( parPtr->GetLBDistributionParIndex() + iLBDist );
    return parPtr->GetLBDistributionPar( iLBDist );
    
  }

  // We do interpolation
  else{
    
    Float_t valXY1 = ( phiInterpolFrac[ 0 ] ) * ( parPtr->GetLBDistributionPar( iLBDist ) )
      + ( phiInterpolFrac[ 1 ] ) * ( parPtr->GetLBDistributionPar( iLBDistPhiInterpol ) );
    Float_t valXY2 = ( phiInterpolFrac[ 0 ] ) * ( parPtr->GetLBDistributionPar( iLBDistThetaInterpol ) )
      + ( phiInterpolFrac[ 1 ] ) * ( parPtr->GetLBDistributionPar( iLBDistPhiThetaInterpol ) );
    
    // Final interpolation
    Float_t interpolVal = cosThetaInterpolFrac[ 0 ] * valXY1 + cosThetaInterpolFrac[ 1 ] * valXY2;

    if ( phiInterpolFrac[ 0 ] * cosThetaInterpolFrac[ 0 ] != 0.0 ){
      parPtr->GetCurrentLBDistributionBins()->push_back( parPtr->GetLBDistributionParIndex() + iLBDist );
    }
    // if ( phiInterpolFrac[ 1 ] * cosThetaInterpolFrac[ 0 ] >= 0.0 ){
    //   parPtr->GetCurrentLBDistributionBins()->push_back( parPtr->GetLBDistributionParIndex() + iLBDistPhiInterpol );
    // }
    // if ( phiInterpolFrac[ 0 ] * cosThetaInterpolFrac[ 1 ] >= 0.0 ){
    //   parPtr->GetCurrentLBDistributionBins()->push_back( parPtr->GetLBDistributionParIndex() + iLBDistThetaInterpol );
    // }
    // if ( phiInterpolFrac[ 1 ] * cosThetaInterpolFrac[ 1 ] >= 0.0 ){
    //   parPtr->GetCurrentLBDistributionBins()->push_back( parPtr->GetLBDistributionParIndex() + iLBDistPhiThetaInterpol );
    // }
    //cout << "interpolVal is: " << interpolVal << endl;
    //return interpolVal;
    return parPtr->GetLBDistributionPar( iLBDist );
    
  }
  
  // // Obtain the laserball isotropy distribution parameter associated
  // // with this bin and return it.
  // Float_t laserlight = parPtr->GetLBDistributionPar( iLBDist );
  // return laserlight;
  
}

//////////////////////////////////////
//////////////////////////////////////
 
Float_t OCAOpticsModel::ModelLBDistributionMask( const OCAPMT& dataPoint, std::string runType )
{

  // Obtain a pointer to the parameters for use in this calculation
  // for the laserball mask distribution model.
  OCAModelParameterStore* parPtr = GetOCAModelParameterStore();

  // Initialise the theta in the local laserball frame for this data point.
  Float_t lbRelTheta = 0.0;
  
  // Assign the value of theta based on whether the 
  // off-axis or central run was specified.
  if ( runType == "off-axis" ){ 
    lbRelTheta = dataPoint.GetRelLBTheta(); 
  }

  else if ( runType == "central" ){ 
    lbRelTheta = dataPoint.GetCentralRelLBTheta(); 
  }
  
  // Initialise the mask value to 1.0 to begin with.
  Float_t onePlus = 1.0 + cos( lbRelTheta );
  Float_t polynomialVal = 1.0;
  //Loop through all the laserball mask parameters
  //performing the summation of different degree terms
  //The degree will run from 1 to NLBDistributionMaskParameters.
  // for ( Int_t iPar = parPtr->GetNLBDistributionMaskParameters() - 1; 
  //       iPar >= 0; 
  //       iPar-- ){

  //   polynomialVal = polynomialVal * onePlus + parPtr->GetLBDistributionMaskPar( iPar );
    
  // }

  for ( Int_t iPar = 1; iPar < parPtr->GetNLBDistributionMaskParameters(); iPar++ ){
    polynomialVal += parPtr->GetLBDistributionMaskPar( iPar ) * TMath::Power( onePlus, iPar );
  }

  // Return the prediciton for the mask component of the
  // laserball distribution.
  return polynomialVal;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAOpticsModel::ModelLBDistributionMaskDeriviative( Double_t* aPtr, Double_t* parPtr )
{
  
  // Model the derivative of the laserball mask function.
  Int_t iPar = (Int_t)parPtr[ 0 ];
  Double_t cosTheta = aPtr[ 0 ];
  
  Double_t onePlus = 1.0 + cosTheta;
  
  Double_t dlbM = TMath::Power( onePlus, iPar );
  
  return dlbM;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAOpticsModel::ModelAngularResponse( const OCAPMT& dataPoint, std::string runType, Float_t& interpolFrac )
{
  // Obtain a pointer to the parameters for use in this calculation
  // for the PMT angular response model.
  OCAModelParameterStore* parPtr = GetOCAModelParameterStore();

  // Initialise the value of theta, the incident
  // angle to the PMT to be 0.0 to begin with.
  Float_t angle = 0.0;
  
  // Depending on whether the off-axis or central run
  // options are specified, assign the theta values
  // accordingly. The incident angles on the data
  // points are held in degrees.
  if ( runType == "off-axis" ){ 
    angle = TMath::ACos( dataPoint.GetCosTheta() ) * 180.0 / TMath::Pi();
  }
  else if ( runType == "central" ){ 
    angle = TMath::ACos( dataPoint.GetCentralCosTheta() ) * 180.0 / TMath::Pi(); 
  }

  // Calculate the associated bin representative of this angle.
  Int_t iAng = (Int_t)( angle * parPtr->GetNPMTAngularResponseBins() / 90.0 );
  if ( iAng < 0 ){ iAng = 0; }
  if ( iAng >= parPtr->GetNPMTAngularResponseBins() ){ iAng = parPtr->GetNPMTAngularResponseBins() - 1; }

  // Depending on the run type specified, assign this bin
  // value to the current values held by the OCAModelParameterStore
  // object.
  if ( runType == "off-axis" ){ 
    parPtr->SetCurrentPMTAngularRepsonseBin( iAng ); 
  }
  else if ( runType == "central" ){ 
    parPtr->SetCentralCurrentPMTAngularResponseBin( iAng ); 
  }

  interpolFrac = 0.0;
  parPtr->GetCurrentAngularResponseBins()->push_back( parPtr->GetPMTAngularResponseParIndex() + iAng );
  return  parPtr->GetPMTAngularResponsePar( iAng );

  if ( !fModelParameterStore->GetParametersVary()[ parPtr->GetPMTAngularResponseParIndex() + iAng ]
       ||
       !fModelParameterStore->GetParametersVary()[ parPtr->GetPMTAngularResponseParIndex() + iAng + 1 ] ){
    interpolFrac = 0.0;
    parPtr->GetCurrentAngularResponseBins()->push_back( parPtr->GetPMTAngularResponseParIndex() + iAng );
    return  parPtr->GetPMTAngularResponsePar( iAng ); 
  }
  
  // Return the PMT angular response parameter associated with
  // this bin, and thus with this particular value of the
  // incident angle at the PMT.
  // Interpolations begin:
  Float_t pmtAngularResponse = 1.0;
  Float_t binWidth = 90.0 / parPtr->GetNPMTAngularResponseBins();
  Float_t slope = ( parPtr->GetPMTAngularResponsePar( iAng + 1 ) - parPtr->GetPMTAngularResponsePar( iAng ) ) / binWidth;
  Float_t deltaTheta = ( angle - floor( angle ) );
  interpolFrac = deltaTheta / binWidth;
 
  //vector< Int_t > bins;
  //parPtr->GetCurrentAngularResponseBins()->insert( parPtr->GetCurrentAngularResponseBins()->begin(), bins.begin(), bins.end() );
  pmtAngularResponse = ( parPtr->GetPMTAngularResponsePar( iAng ) * ( 1.0 - interpolFrac )
                         + parPtr->GetPMTAngularResponsePar( iAng + 1 ) * interpolFrac );
  parPtr->GetCurrentAngularResponseBins()->push_back( parPtr->GetPMTAngularResponseParIndex() + iAng );
  parPtr->GetCurrentAngularResponseBins()->push_back( parPtr->GetPMTAngularResponseParIndex() + iAng + 1 );

  return pmtAngularResponse;

}
