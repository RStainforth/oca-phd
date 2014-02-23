////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASOpticsModel.hh
///
/// CLASS: LOCAS::LOCASOpticsModel
///
/// BRIEF: The class which defines the optical response model
///        of PMTs. This class inherits from LOCASModel.hh
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

using namespace LOCAS;
using namespace std;

ClassImp( LOCASOpticsModel )

//////////////////////////////////////
//////////////////////////////////////

LOCASOpticsModel::LOCASOpticsModel( LOCASModelParameterStore& locasParams, 
                                    const std::string modelName )
{

  SetModelName( modelName );
  SetModelParameterStore( locasParams );

  Int_t nParams = fModelParameterStore.GetNParameters();
  fParameters = new Double_t[ nParams + 1 ];

  for ( Int_t iPar = 0.0; iPar < nParams; iPar++ ){
    fParameters[ iPar ] = 0.0;
  }

  std::vector< LOCASModelParameter >::iterator iPar;

  for ( iPar = locasParams.GetLOCASModelParametersIterBegin();
        iPar != locasParams.GetLOCASModelParametersIterEnd();
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
    }

  }

  InitialiseParameters();

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelPrediction( const LOCASDataPoint& dataPoint )
{

  return 5;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelLBDistribution( const LOCASDataPoint& dataPoint )
{

  return 10;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASOpticsModel::ModelAngularResponse( const LOCASDataPoint& dataPoint )
{

  return 15;

}



