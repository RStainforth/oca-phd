////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataFiller.cc
///
/// CLASS: LOCAS::LOCASDataFiller.cc
///
/// BRIEF: A class used to take raw data points and a set
///        of filters and return a 'filtered' data set.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASDataFiller.hh"
#include "LOCASRawDataStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASRawDataPoint.hh"
#include "LOCASDataPoint.hh"
#include "LOCASFilter.hh"
#include "LOCASFilterStore.hh"
#include "LOCASChiSquare.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataFiller )

//////////////////////////////////////
//////////////////////////////////////

LOCASDataFiller::LOCASDataFiller( LOCASRawDataStore& dataSt, LOCASFilterStore& filterSt )
{

  AddData( dataSt, filterSt );

}


//////////////////////////////////////
//////////////////////////////////////

void LOCASDataFiller::AddData( LOCASRawDataStore& dataSt, LOCASFilterStore& filterSt )
{
  
  // Iterate through each raw datapoint and each filter to check if the conditions of each
  // filter for each data point are met. If the conditions are met, the raw datapoint is converted to
  // a standard data point to be used in fitting and is added to the private datastore; 'fDataStore'

  std::vector< LOCASRawDataPoint >::iterator iD;
  std::vector< LOCASFilter >::iterator iF;
  Bool_t validPoint = true;
  std::string filterName = "";

  for ( iD = dataSt.GetLOCASRawDataPointsIterBegin();
        iD != dataSt.GetLOCASRawDataPointsIterEnd();
        iD++ ){

    validPoint = true;
    for ( iF = filterSt.GetLOCASTopLevelFiltersIterBegin();
          iF != filterSt.GetLOCASTopLevelFiltersIterEnd();
          iF++ ){

      filterName = iF->GetFilterName();

      // Filters to check the occupancy ratio and MPE corrected
      // occupancy from the off-axis and central runs

      if ( filterName == "filter_mpe_occratio" ){ 
        if ( !iF->CheckCondition( iD->GetModelCorrOccRatio() ) ){ 
          validPoint = false; 
        } 
      }
      
      else if ( filterName == "filter_mpe_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetMPECorrOccupancy() ) ){ 
          validPoint = false; 
        }
      }

      else if ( filterName == "filter_mpe_ctr_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetCentralMPECorrOccupancy() ) ){ 
          validPoint = false; 
        }
      }

      else if ( filterName == "filter_rel_occ_err" ){
        if ( !iF->CheckCondition( iD->GetMPEOccRatioErr() / iD->GetModelCorrOccRatio() ) ){ 
          validPoint = false; 
        }
      }
      
      
      // Filters to check the various distances in the scintillator region, AV and water
      // region in terms of the 'Delta' differences i.e. the modulus difference between
      // the central and off-axis runs. Also checks the total distance of each light path
      
      else if ( filterName == "filter_deltascint" ){
        Float_t deltaScint = TMath::Abs( iD->GetDistInScint() - iD->GetCentralDistInScint() );
        if ( !iF->CheckCondition( deltaScint ) ){ 
          validPoint = false; 
        }
      }
      
      else if ( filterName == "filter_deltaav" ){
        Float_t deltaAV = TMath::Abs( iD->GetDistInAV() - iD->GetCentralDistInAV() );
        if ( !iF->CheckCondition( deltaAV ) ){ 
          validPoint = false; 
        }
      }
      
      else if ( filterName == "filter_deltawater" ){
        Float_t deltaWater = TMath::Abs( iD->GetDistInWater() - iD->GetCentralDistInWater() );
        if ( !iF->CheckCondition( deltaWater ) ){ 
          validPoint = false;  
        }
      }

      else if ( filterName == "filter_total_dist" ){
        if ( !iF->CheckCondition( iD->GetTotalDist() ) ){ 
          validPoint = false;  
        }
      }
      
      // Filters to check against various flags (CSHS, CSS) as well as neck light paths and
      // 'bad' paths and incident angles

      else if ( filterName == "filter_pmt_angle" ){ 
        if ( !iF->CheckCondition( iD->GetIncidentAngle() ) || !iF->CheckCondition( iD->GetCentralIncidentAngle() ) ){ 
          validPoint = false;
        }
      }

      else if ( filterName == "filter_solid_angle_ratio" ){ 
        if ( !iF->CheckCondition( iD->GetSolidAngle() / iD->GetCentralSolidAngle() ) ){ 
          validPoint = false;
        }
      }
      
      else if ( filterName == "filter_chs" ){ 
        if ( !iF->CheckCondition( iD->GetCHSFlag() ) || !iF->CheckCondition( iD->GetCentralCHSFlag() ) ){ 
          validPoint = false;
        }
      }
      
      else if ( filterName == "filter_css" ){ 
        if ( !iF->CheckCondition( iD->GetCSSFlag() ) || !iF->CheckCondition( iD->GetCentralCSSFlag() ) ){ 
          validPoint = false;  
        }
      }
      
      else if ( filterName == "filter_bad_path" ){ 
        if ( !iF->CheckCondition( iD->GetBadPathFlag() ) || !iF->CheckCondition( iD->GetCentralBadPathFlag() ) ){ 
          validPoint = false;  
        }
      }
      
      else if ( filterName == "filter_neck_path" ){ 
        if ( !iF->CheckCondition( iD->GetNeckFlag() ) || !iF->CheckCondition( iD->GetCentralNeckFlag() ) ){ 
          validPoint = false;  
        }
      }

      // Filter the shadowing values

      else if ( filterName == "filter_avhd_shadow" ){ 
        if ( !iF->CheckCondition( iD->GetAVHDShadowingVal() ) || !iF->CheckCondition( iD->GetCentralAVHDShadowingVal() ) ){ 
          validPoint = false;  
        }
      }

      else if ( filterName == "filter_geo_shadow" ){ 
        if ( !iF->CheckCondition( iD->GetGeometricShadowingVal() ) || !iF->CheckCondition( iD->GetCentralGeometricShadowingVal() ) ){ 
          validPoint = false;  
        }
      }
      
    }
    
    if ( validPoint == true ){ fDataStore.AddDataPoint( *iD ); }
    
  }
  
}


//////////////////////////////////////
/////////////////////////////////////

LOCASDataStore LOCASDataFiller::ReFilterData( LOCASFilterStore& filterSt, LOCASChiSquare& chiSq )
{

  LOCASDataStore newStore;

  // Iterate through each raw datapoint and each filter to check if the conditions of each
  // filter for each data point are met. If the conditions are met, the raw datapoint is converted to
  // a standard data point to be used in fitting and is added to the private datastore; 'fDataStore'

  std::vector< LOCASDataPoint >::iterator iD;
  std::vector< LOCASFilter >::iterator iF;
  Bool_t validPoint = true;
  std::string filterName = "";

  for ( iD = fDataStore.GetLOCASDataPointsIterBegin();
        iD != fDataStore.GetLOCASDataPointsIterEnd();
        iD++ ){

    validPoint = true;
    for ( iF = filterSt.GetLOCASFitLevelFiltersIterBegin();
          iF != filterSt.GetLOCASFitLevelFiltersIterEnd();
          iF++ ){

      filterName = iF->GetFilterName();

      // Filter to check the initial Chi-Square value
      
      if ( filterName == "filter_chi_square" ){        
        Double_t dataPointChiSquare = chiSq.EvaluateChiSquare( *(iD) );
        if ( !iF->CheckCondition( dataPointChiSquare ) ){ 
          validPoint = false; 
        } 
      }

    }

    if ( validPoint == true ){ newStore.AddDataPoint( *iD );}

  }

  return newStore;
}
