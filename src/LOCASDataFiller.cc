////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataFiller.cc
///
/// CLASS: LOCAS::LOCASDataFiller.cc
///
/// BRIEF: A class used to take raw data points and a set
///        of filters and return a final data set.
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
  
  std::vector< LOCASRawDataPoint >::iterator iD;
  std::vector< LOCASFilter >::iterator iF;
  Bool_t validPoint = true;
  std::string filterName = "";

  for ( iD = dataSt.GetLOCASRawDataPointsIterBegin();
        iD != dataSt.GetLOCASRawDataPointsIterEnd();
        iD++ ){

    validPoint = true;
    for ( iF = filterSt.GetLOCASFiltersIterBegin();
          iF != filterSt.GetLOCASFiltersIterEnd();
          iF++ ){

      filterName = iF->GetFilterName();

      // Filters to check the occupancy ratio, raw occupancy and MPE corrected
      // occupancy
      
      if ( filterName == "filter_mpe_occratio" ){ 
        if ( !iF->CheckCondition( iD->GetMPEOccRatio() ) ){ 
          validPoint = false; 
        } 
      }

      if ( filterName == "filter_raw_occratio" ){ 
        if ( !iF->CheckCondition( iD->GetRawOccRatio() ) ){ 
          validPoint = false; 
        } 
      }
      
      else if ( filterName == "filter_raw_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetRawOccupancy() ) || !iF->CheckCondition( iD->GetCentralRawOccupancy() ) ){ 
          validPoint = false; 
        }
      }
      
      else if ( filterName == "filter_mpe_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetMPECorrOccupancy() ) || !iF->CheckCondition( iD->GetCentralMPECorrOccupancy() ) ){ 
          validPoint = false; 
        }
      }
      
      
      // Filters to check the various distances in the scintillator region, AV and water
      // region in terms of the 'Delta' differences i.e. the modulus difference between
      // the central and off-axis runs
      
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
//////////////////////////////////////

LOCASDataStore LOCASDataFiller::GetData()
{

  return fDataStore;

}
