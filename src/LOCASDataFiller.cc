#include <iostream>

#include "LOCASRunReader.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDataFiller.hh"
#include "LOCASDataStore.hh"
#include "LOCASDataPoint.hh"
#include "LOCASFilter.hh"
#include "LOCASFilterStore.hh"
#include "LOCASChiSquare.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASDataFiller )

//////////////////////////////////////
//////////////////////////////////////

void LOCASDataFiller::FilterData( LOCASFilterStore& filterSt, LOCASDataStore* lDataStore, LOCASChiSquare* lChiSq )
{
  
  // Iterate through each datapoint in the LOCASDataStore object 'fDataStore' and
  // apply all the filters in the passed LOCASFilterStore object, removing datapoints which do
  // not satisfy the filter conditions. The datapoints which pass all the filter criteria are those
  // to be used in the fit.

  // Note: The LOCASChiSquare object here is used for filters that set conditions on the 
  // initial chi-square value of the datapoint against the model and its current set
  // of parameters in the optical model

  // LOCASDataPoint iterator to loop through the data points
  std::vector< LOCASDataPoint >::iterator iD;

  // LOCASFilter iterator to loop through and apply the filters to the datapoints
  std::vector< LOCASFilter >::iterator iF;

  // Filter name - changes based on the filter currently being used in the below loop
  std::string filterName = "";

  Bool_t validPoint = true;

  LOCASDataStore* newStore = new LOCASDataStore();
    
  // Loop through all the datapoints in the LOCASDataStore object
  for ( iD = lDataStore->GetLOCASDataPointsIterBegin();
        iD != lDataStore->GetLOCASDataPointsIterEnd();
        iD++ ){

    validPoint = true;
    
    // Loop through all the filters
    for ( iF = filterSt.GetLOCASFiltersIterBegin();
          iF != filterSt.GetLOCASFiltersIterEnd();
          iF++ ){
      
      // Obtain the name of the filter
      filterName = iF->GetFilterName();
      
      // Filters to check the occupancy ratio and MPE corrected
      // occupancy from the off-axis and central runs
      
      
      if ( filterName == "filter_mpe_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetMPECorrOccupancy() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_mpe_ctr_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetCentralMPECorrOccupancy() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      // Filters to check the various distances in the scintillator region, AV and water
      // region in terms of the 'Delta' differences i.e. the modulus difference between
      // the central and off-axis runs. Also checks the total distance of each light path
      
      else if ( filterName == "filter_deltascint" ){
        Float_t deltaScint = TMath::Abs( iD->GetDistInInnerAV() - iD->GetCentralDistInInnerAV() );
        if ( !iF->CheckCondition( deltaScint ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_deltaav" ){
        Float_t deltaAV = TMath::Abs( iD->GetDistInAV() - iD->GetCentralDistInAV() );
        if ( !iF->CheckCondition( deltaAV ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_deltawater" ){
        Float_t deltaWater = TMath::Abs( iD->GetDistInWater() - iD->GetCentralDistInWater() );
        if ( !iF->CheckCondition( deltaWater ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      // Filters to check against various flags (CSHS, CSS) as well as neck light paths and
      // 'bad' paths and incident angles
      
      else if ( filterName == "filter_pmt_angle" ){ 
        if ( !iF->CheckCondition( iD->GetIncidentAngle() ) || !iF->CheckCondition( iD->GetCentralIncidentAngle() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_solid_angle_ratio" ){ 
        if ( !iF->CheckCondition( iD->GetSolidAngle() / iD->GetCentralSolidAngle() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_chs" ){ 
        if ( !iF->CheckCondition( iD->GetCHSFlag() ) || !iF->CheckCondition( iD->GetCentralCHSFlag() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_css" ){ 
        if ( !iF->CheckCondition( iD->GetCSSFlag() ) || !iF->CheckCondition( iD->GetCentralCSSFlag() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_bad_path" ){ 
        if ( !iF->CheckCondition( iD->GetBadPathFlag() ) || !iF->CheckCondition( iD->GetCentralBadPathFlag() ) ){ 
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }

      
      // Filters to check the current chisquare value of the data point
      
      else if ( filterName == "filter_chi_square" ){
        if ( !iF->CheckCondition( lChiSq->EvaluateChiSquare( *iD ) ) ){
          //lDataStore->EraseDataPoint( iD ); break;
          validPoint = false; break;
        }
      }

      else{
        cout << "LOCASDataFiller: Unknown filter: '" << filterName << "'\n";
        cout << "Try adding filter to LOCAsDataFiller::FilterData.\n";
      }
      
    }
    
    if ( validPoint == true ){
      newStore->AddDataPoint( *iD );
    }
    
  }

  *lDataStore = *newStore;
  delete newStore;
  
}

//////////////////////////////////////
/////////////////////////////////////
