#include "OCADataFiller.hh"
#include "OCAPMTStore.hh"
#include "OCAPMT.hh"
#include "OCAFilter.hh"
#include "OCAFilterStore.hh"
#include "OCAChiSquare.hh"

#include "TMath.h"

#include <iostream>

using namespace OCA;
using namespace std;

ClassImp( OCADataFiller )

//////////////////////////////////////
//////////////////////////////////////

void OCADataFiller::FilterData( OCAFilterStore* lFilterStore,
                                  OCAPMTStore* lDataStore,
                                  OCAChiSquare* lChiSq )
{

  // Iterate through each datapoint in the OCAPMTStore 
  // object 'lDataStore' and apply all the filters in the 
  // passed OCAFilterStore object 'lFilterStore', removing datapoints 
  // which do not satisfy the individual filter conditions. 
  // The datapoints which pass all the filter criteria are those
  // to be used in the fit.

  // Note: The OCAChiSquare object here is used for filters 
  // that set conditions on the initial chi-square value of 
  // the datapoints against their model prediction using the current set
  // of parameters in the optical model (linked to the OCAChiSquare object)

  // OCAPMT iterator to loop through the data points
  std::vector< OCAPMT >::iterator iD;

  // OCAFilter iterator to loop through and apply the filters 
  // to the datapoints
  std::vector< OCAFilter >::iterator iF;

  // Filter name - changes based on the filter currently being 
  // used in the below loop
  std::string filterName = "";

  // Boolean value to determine if the current datapoint
  // is valid. If it fails a filter check condition in the loop
  // below then it is invalid (false), and therefore not included
  // in the final data set.
  Bool_t validPoint = true;

  // The new OCAPMTStore object which will store all the 
  // OCAPMTs which successfully pass all the filter
  // conditions. This object will be pointed to by the pointer
  // passed into this function
  OCAPMTStore* newStore = new OCAPMTStore();

  Int_t currentRunID = 0;
  Int_t previousRunID = 0;
    
  // Loop through all the datapoints in the OCAPMTStore object
  for ( iD = lDataStore->GetOCAPMTsIterBegin();
        iD != lDataStore->GetOCAPMTsIterEnd();
        iD++ ){

    currentRunID = iD->GetRunID();

    if ( previousRunID != currentRunID
         && previousRunID > 0 && currentRunID > 0 ){
      cout << "Filtered Information for Run: " << previousRunID << endl;
      cout << "Laserball Position: (" 
           << iD->GetLBPos().X() / 10.0
           << ", " << iD->GetLBPos().Y() / 10.0
           << ", " << iD->GetLBPos().Z() / 10.0 
           << " ) cm, Radius = " << iD->GetLBPos().Mag() / 10.0 << " cm" << endl;
      lFilterStore->PrintFilterCutInformation();
      lFilterStore->ResetFilterConditionCounters();
      cout << "----------------------------------------------------\n";
      cout << "####################################################\n";
      cout << "----------------------------------------------------\n";
    }

    validPoint = true;
    
    // Loop through all the filters
    for ( iF = lFilterStore->GetOCAFiltersIterBegin();
          iF != lFilterStore->GetOCAFiltersIterEnd();
          iF++ ){
      
      // Obtain the name of the filter
      filterName = iF->GetFilterName();

      // Filters to check boolean values

      if ( filterName == "filter_bad_path" ){
        if ( !iF->CheckBoolCondition( (Bool_t)iD->GetBadPath() ) ){
          validPoint = false; break;
        }
      }


      else if ( filterName == "filter_ctr_bad_path" ){
        if ( !iF->CheckBoolCondition( (Bool_t)iD->GetCentralBadPath() ) ){
          validPoint = false; break;
        }
      }
      
      // Filters to check the occupancy ratio and MPE corrected
      // occupancy from the off-axis and central runs

      else if ( filterName == "filter_prompt_counts" ){ 
        if ( !iF->CheckCondition( iD->GetPromptPeakCounts() ) ){ 
          validPoint = false; break;
        }
      }

      else if ( filterName == "filter_ctr_prompt_counts" ){ 
        if ( !iF->CheckCondition( iD->GetCentralPromptPeakCounts() ) ){ 
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_mpe_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetMPECorrOccupancy() ) ){ 
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_mpe_ctr_occupancy" ){ 
        if ( !iF->CheckCondition( iD->GetCentralMPECorrOccupancy() ) ){ 
          validPoint = false; break;
        }
      }

      // Filter to check fractional error
      else if ( filterName == "filter_fractional_error" ){
        lChiSq->EvaluateChiSquare( *iD );
        if ( !iF->CheckCondition( iD->GetOccupancyRatioErr() / iD->GetOccupancyRatio() ) ){ 
          validPoint = false; break;
        }
      }
      
      // Filters to check the various distances in the 
      // inner AV, AV and water regions in terms of the 
      // 'Delta' differences i.e. the modulus difference between
      // the central and off-axis runs. 
      // Also checks the total distance of each light path
      
      else if ( filterName == "filter_deltascint" ){
        Float_t deltaScint = TMath::Abs( iD->GetDistInInnerAV() - iD->GetCentralDistInInnerAV() );
        if ( !iF->CheckCondition( deltaScint ) ){ 
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_deltaav" ){
        Float_t deltaAV = TMath::Abs( iD->GetDistInAV() 
                                      - iD->GetCentralDistInAV() );
        if ( !iF->CheckCondition( deltaAV ) ){ 
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_deltawater" ){
        Float_t deltaWater = TMath::Abs( iD->GetDistInWater() 
                                         - iD->GetCentralDistInWater() );
        if ( !iF->CheckCondition( deltaWater ) ){ 
          validPoint = false; break;
        }
      }
      
      // Filters to check against the incident PMT angles and
      // the solid angles.
      
      else if ( filterName == "filter_pmt_angle" ){ 
        if ( !iF->CheckCondition( TMath::ACos( iD->GetCosTheta() ) * TMath::RadToDeg() ) 
             || !iF->CheckCondition( TMath::ACos( iD->GetCosTheta() ) * TMath::RadToDeg() ) ){ 
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_solid_angle_ratio" ){ 
        if ( !iF->CheckCondition( iD->GetSolidAngle() 
                                  / iD->GetCentralSolidAngle() ) ){ 
          validPoint = false; break;
        }
      }

      // Filters to check against various flags (CHS, CSS, 'Bad' lightpath)
      // 'Bad' light paths are deemed so based on the conditions set out
      // in OCAPMT::VerifyPMT(). The flag is set in OCAPMT::ProcessLightPath
      
      else if ( filterName == "filter_chs" ){ 
        if ( !iF->CheckCondition( iD->GetCHSFlag() ) 
             || !iF->CheckCondition( iD->GetCentralCHSFlag() ) ){ 
          validPoint = false; break;
        }
      }
      
      else if ( filterName == "filter_css" ){ 
        if ( !iF->CheckCondition( iD->GetCSSFlag() ) 
             || !iF->CheckCondition( iD->GetCentralCSSFlag() ) ){ 
          validPoint = false; break;
        }
      }

      // Filters to check the current chi-square value of the data point
      
      else if ( filterName == "filter_chi_square" ){
        if ( !iF->CheckCondition( lChiSq->EvaluateChiSquare( *iD ) ) ){
          validPoint = false; break;
        }
      }

      else{
        cout << "OCADataFiller: Unknown filter: '" << filterName << "'\n";
        cout << "Try adding filter to LOCAsDataFiller::FilterData.\n";
      }
      
    }
    
    // If the validPoint boolean is still true, then the current datapoint
    // passed all the filter checks above (as specified in the 'fit-file').
    // Therefore we add this datapoint to the final set
    if ( validPoint == true ){
      newStore->AddDataPoint( *iD );
    }

    previousRunID = iD->GetRunID();
    
  }

  // Set and copy the value pointed to by the passed dataset to be this new
  // 'filtered' data set. This ensures the pointer keeps the same address,
  // but has a new value.
  *lDataStore = *newStore;

  // Delete the memory which temporarily stored this new data set. This is
  // OK because we copied the information in the above line to be linked
  // with the address of the original pointer 'lDataStore'
  delete newStore;
  
}
