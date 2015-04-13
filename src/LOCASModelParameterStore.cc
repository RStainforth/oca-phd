////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModelParameterStore.cc
///
/// CLASS: LOCAS::LOCASModelParameterStore
///
/// BRIEF: A simple class to hold a collection of
///        LOCASModelParameter objects which are used
///        by a LOCASModel class
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASModelParameterStore.hh"
#include "LOCASModelParameter.hh"
#include "LOCASDataStore.hh"

#include "LOCASDB.hh"
#include "LOCASMath.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASModelParameterStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASModelParameterStore::LOCASModelParameterStore( std::string storeName )
{

  fStoreName = storeName;
  fParameters.clear();
  fParametersPtr = NULL;
  fParametersVary = NULL;
  fCovarianceMatrix = NULL;
  fDerivativeMatrix = NULL;
  fVariableParameterIndex = NULL;
  fVariableParameterMap = NULL;
  fPMTAngularResponseIndex = NULL;
  fNParameters = 0;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::PrintParameterInfo()
{

  std::vector< LOCASModelParameter >::iterator iPar;

  cout << "Number of Parameters is: " << GetNParameters() << endl;
  cout << "--------------" << endl;

  for ( iPar = GetLOCASModelParametersIterBegin();
        iPar != GetLOCASModelParametersIterEnd();
        iPar++ ){

    iPar->PrintInfo();

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::AddParameters( const char* fileName )
{

  LOCASDB lDB;
  lDB.SetFile( fileName );

  Float_t maxVal, minVal, initVal, incVal = 0.0;
  Int_t nParsInGroup = 0;
  Bool_t varyBool = false;
  std::vector< Int_t > indexList;
  stringstream lStream;
  string parStr = "";

  // Get a list of the parameters to be included in the parameter store
  std::vector< std::string > paramList = lDB.GetStringVectorField( "FITFILE", "parameters_list", "parameter_setup" );

  // First find out the specifics for the number of bins required for each parameter type.
  // i.e. - Laserball Mask Function: Number of Parameters 'fNLBDistributionMaskParameters'
  //      - PMT Angular Response: Number of bins between 0-degrees and 90-degrees 'fNPMTAngularResponseBins'
  //      - Laserball Isotropy Distribution 2D ( CosTheta, Phi ) Histogram:
  //                  - Number of bins in CosTheta between -1.0 and +1.0 'fNLBDistributionCosThetaBins'
  //                  - Number of bins in Phi between 0.0 and 360.0 'fNLBDistributionPhiBins'

  // Loop over each parameter set in the card file and add them/it to the store
  for ( Int_t iStr = 0; iStr < (Int_t)paramList.size(); iStr++ ){

    if ( paramList[ iStr ] == "laserball_intensity_mask" ){
      fNLBDistributionMaskParameters = lDB.GetIntField( "FITFILE", (std::string)( paramList[ iStr ] + "_number_of_parameters" ), "parameter_setup" );
    }

    else if ( paramList[ iStr ] == "pmt_angular_response" ){
      fNPMTAngularResponseBins = lDB.GetIntField( "FITFILE", (std::string)( paramList[ iStr ] + "_number_of_bins" ), "parameter_setup" );
    }

    else if ( paramList[ iStr ] == "laserball_distribution_histogram" ){
      fNLBDistributionCosThetaBins = lDB.GetIntField( "FITFILE", (std::string)( paramList[ iStr ] + "_number_of_cos_theta_bins" ), "parameter_setup" );
      fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", (std::string)( paramList[ iStr ] + "_number_of_phi_bins" ), "parameter_setup" );
    }

    else if ( paramList[ iStr ] == "laserball_run_normalisation" ){
      std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
      fNLBRunNormalisations = (Int_t)( runIDs.size() );
    }

  }

  // Loop over each parameter set in the card file and add them/it to the store
  for ( Int_t iStr = 0; iStr < (Int_t)paramList.size(); iStr++ ){

    // Setup for the extinction length parameters
    if ( paramList[ iStr ] == "inner_av_extinction_length"
         || paramList[ iStr ] == "acrylic_extinction_length" 
         || paramList[ iStr ] == "water_extinction_length" ){

      // The initial value of the extinction length
      initVal = lDB.GetDoubleField( "FITFILE", (std::string)( paramList[ iStr ] + "_initial_value" ), "parameter_setup" );

      // Whether or not this parameter will vary in the fit ( 1: Yes, 0: No )
      varyBool = lDB.GetBoolField( "FITFILE", (std::string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      // For extinction lengths, the minimum value can't be negative, so for now set to 1.0e-9 (= 1,000,000 m)
      // and for the maximum value set to 1.0 (= 0.001 m)
      minVal = 1.0e-9;
      maxVal = 1.0;
      
      // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
      incVal = -10.0;

      // Each parameter is unique, so nParsInGroup = 1.
      nParsInGroup = 1;

      // Set the indices for the three extinction lengths in the fit: 1: inner_av region, 2: av region, 3: water region
      Int_t parIndex = 0;
      if ( paramList[ iStr ] == "inner_av_extinction_length" ){ parIndex = 1; }
      else if ( paramList[ iStr ] == "acrylic_extinction_length" ){ parIndex = 2; }
      else if ( paramList[ iStr ] == "water_extinction_length" ){ parIndex = 3; }
      else{ cout << "LOCASModelParameterStore::LOCASModelParameterStore: Error, unknown parameter passed" << endl; }

      LOCASModelParameter lParameter( (std::string)( paramList[ iStr ] ), parIndex, initVal, minVal, maxVal, incVal, nParsInGroup, varyBool );
      AddParameter( lParameter );

    }

    if ( paramList[ iStr ] == "laserball_intensity_mask" ){

      nParsInGroup = fNLBDistributionMaskParameters;

      varyBool = lDB.GetBoolField( "FITFILE", (std::string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      // Now loop over each laserball mask parameter and initialise its initial values;
      for ( Int_t iPar = 1; iPar <= nParsInGroup; iPar++ ){

        // Fix the first value of the laserball mask to 1.0, otherwise all other parameters start at 0.0;
        if ( iPar == 1 ){ initVal = 1.0; }
        else { initVal = 0.0; }

        Bool_t parVary = varyBool;
        // If the mask has been set to vary, then the first parameter is held fixed, all others can vary
        if ( iPar == 1 && varyBool == true ){ parVary = false; }
        

        // The mask parameters can be either positive or negative, but generally are single digits
        // to one leading term, so set the minimum and maximum values of these parameters to something
        // sufficiently small and large
        minVal = -100.0;
        maxVal = 100.0;

        // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
        incVal = -10.0;

        lStream << "_";
        lStream << iPar;
        lStream >> parStr;

        Int_t parIndex = 3 + iPar;
        cout << "VaryBool is: " << varyBool << endl;
        LOCASModelParameter lParameter( (std::string)( paramList[ iStr ] + parStr ), parIndex, initVal, minVal, maxVal, incVal, nParsInGroup, parVary );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "pmt_angular_response" ){

      nParsInGroup = fNPMTAngularResponseBins;

      varyBool = lDB.GetBoolField( "FITFILE", (std::string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      Float_t angleVal = 0.0;
      // Now loop over each angular response parameter and initialise its initial values;
      for ( Int_t iPar = 1; iPar <= nParsInGroup; iPar++ ){

        angleVal = ( iPar - 0.5 ) * ( 90.0 / fNPMTAngularResponseBins ); // Centre of each bin...
        if ( angleVal < 36.0 ){ initVal = 1.0 + ( 0.002222 * angleVal ); }
        else{ initVal = 1.0; }

        // Fix the bin containing the zero degrees value to 1.0
        if ( iPar == 1 ){ initVal = 1.0; }

        Bool_t parVary = varyBool;
        // If the angular response has been set to vary, then the first parameter is held fixed, all others can vary
        if ( varyBool == true && iPar == 1 ){ parVary = false; }

        // The angular response parameters have an absolutel minimum of 1.0, and are allowed to 
        // go up to 2.0, however it usually doesn't get much higher than 1.25
        minVal = 1.0;
        maxVal = 1.25;

        // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
        incVal = -10.0;

        lStream << "_";
        lStream << iPar;
        lStream >> parStr;

        Int_t parIndex = 3 + fNLBDistributionMaskParameters + iPar;
        LOCASModelParameter lParameter( (std::string)( paramList[ iStr ] + parStr ), parIndex, initVal, minVal, maxVal, incVal, nParsInGroup, parVary );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "laserball_distribution_histogram" ){

      nParsInGroup = fNLBDistributionPhiBins * fNLBDistributionCosThetaBins;

      varyBool = lDB.GetIntField( "FITFILE", (std::string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      // Now loop over each angular response parameter and initialise its initial values;
      for ( Int_t iPar = 1; iPar <= nParsInGroup; iPar++ ){

        initVal = 1.0;

        // The laserball distribution parameters in the histogram will not vary beyond 0.0 and 2.0, so set
        // these accordingly
        minVal = 0.0;
        maxVal = 2.0;

        // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
        incVal = -10.0;

        lStream << "_";
        lStream << iPar;
        lStream >> parStr;

        Int_t parIndex = 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins + iPar;
        LOCASModelParameter lParameter( (std::string)( paramList[ iStr ] + parStr ), parIndex, initVal, minVal, maxVal, incVal, nParsInGroup, varyBool );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "laserball_run_normalisation" ){

      nParsInGroup = fNLBRunNormalisations;

      std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
      initVal = lDB.GetDoubleField( "FITFILE", (std::string)( paramList[ iStr ] + "_initial_value" ), "parameter_setup" );
      varyBool = lDB.GetIntField( "FITFILE", (std::string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      // Now loop over each angular response parameter and initialise its initial values;
      for ( Int_t iPar = 1; iPar <= nParsInGroup; iPar++ ){

        // The mask parameters can be either positive or negative, but generally are single digits
        // to one leading term, so set the minimum and maximum values of these parameters to something
        // sufficiently small and large
        minVal = -100.0;
        maxVal = 100.0;

        // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
        incVal = -10.0;

        lStream << "_";
        lStream << iPar;
        lStream << "_runID:_";
        lStream << runIDs[ iPar - 1 ];
        lStream >> parStr;

        Int_t parIndex = 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins + ( fNLBDistributionPhiBins * fNLBDistributionCosThetaBins ) + iPar;
        LOCASModelParameter lParameter( (std::string)( paramList[ iStr ] + parStr ), parIndex, initVal, minVal, maxVal, incVal, nParsInGroup, varyBool );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

  }

  fNParameters = (Int_t)fParameters.size();

  AllocateParameterArrays();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::AllocateParameterArrays()
{

  fParametersPtr = LOCASVector( 1, fNParameters );
  fParametersVary = LOCASIntVector( 1, fNParameters );
  fCovarianceMatrix = LOCASMatrix( 1, fNParameters, 1, fNParameters );
  fDerivativeMatrix = LOCASMatrix( 1, fNParameters, 1, fNParameters );

  for ( Int_t iIndex = 1; iIndex <= fNParameters; iIndex++ ){
    for ( Int_t jIndex = 1; jIndex <= fNParameters; jIndex++ ){
      fCovarianceMatrix[ iIndex ][ jIndex ] = 0.0;
      fDerivativeMatrix[ iIndex ][ jIndex ] = 0.0;
    }
  }

  std::vector< LOCASModelParameter >::iterator iPar;
  for ( iPar = GetLOCASModelParametersIterBegin();
        iPar != GetLOCASModelParametersIterEnd();
        iPar++ ){

    fParametersPtr[ iPar->GetIndex() ] = iPar->GetInitialValue();
    if ( iPar->GetVary() ){ fParametersVary[ iPar->GetIndex() ] = 1; }
    else{ fParametersVary[ iPar->GetIndex() ] = 0; }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::InitialisePMTAngularResponseIndex()
{

  if ( fVariableParameterMap ) delete[] fVariableParameterMap;
  fVariableParameterMap = new Int_t[ fNParameters + 1 ];

  Int_t i = 0; 
  Int_t j = 0;

  j = 0;
  for ( i = 1; i <= fNParameters; i++ ) if ( fParametersVary[ i ] ) fVariableParameterMap[ i ] = ++j;
  
  if ( fPMTAngularResponseIndex ){
    for ( i = 0; i < fNPMTAngularResponseBins + 1; i++ ){
      for ( j = 0; j < fNPMTAngularResponseBins + 1; j++ ){
	delete[] fPMTAngularResponseIndex[ i ][ j ];
      }
      delete[] fPMTAngularResponseIndex[ i ];
    }
    delete[] fPMTAngularResponseIndex;
  }
  
  fPMTAngularResponseIndex = NULL;
  
  fPMTAngularResponseIndex = new Int_t**[ fNPMTAngularResponseBins + 1 ];
  for ( i = 0; i < fNPMTAngularResponseBins + 1; i++ ){
    fPMTAngularResponseIndex[ i ] = new Int_t*[ fNPMTAngularResponseBins + 1 ];
    for ( j = 0; j < fNPMTAngularResponseBins + 1; j++ ) fPMTAngularResponseIndex[ i ][ j ] = new Int_t[ 4 + 1 ];
  }

  Int_t first, second;

  for ( i = 0; i <= fNPMTAngularResponseBins; i++ ) {
    for ( j = 0; j <= fNPMTAngularResponseBins; j++ ) {
      if ( i <= j ) { first = i; second = j; }
      else { first = j; second = i; }
      if (first==second) {
        fPMTAngularResponseIndex[i][j][0] = 1;
        fPMTAngularResponseIndex[i][j][1] = first;
        fPMTAngularResponseIndex[i][j][2] = -1;
        fPMTAngularResponseIndex[i][j][3] = -1;
        fPMTAngularResponseIndex[i][j][4] = -1;
      } 
      else {
        fPMTAngularResponseIndex[i][j][0] = 2;
        fPMTAngularResponseIndex[i][j][1] = first;
        fPMTAngularResponseIndex[i][j][2] = second;
        fPMTAngularResponseIndex[i][j][3] = -1;
        fPMTAngularResponseIndex[i][j][4] = -1;
      }
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::IdentifyVaryingParameters()
{

  Int_t i;

  fNCurrentVariableParameters = fNBaseVariableParameters;

  Int_t parnum;

  for ( i = 1; i <= fPMTAngularResponseIndex[ fCurrentPMTAngularResponseBin ][ fCentralCurrentPMTAngularResponseBin ][ 0 ]; i++ ){

    parnum = GetPMTAngularResponseParIndex() + fPMTAngularResponseIndex[ fCurrentPMTAngularResponseBin ][ fCentralCurrentPMTAngularResponseBin ][ i ];
    if ( fParametersVary[ parnum ] ){
      fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum;
    }
  }

  Int_t first, second;
  if ( fCurrentLBDistributionBin <= fCentralCurrentLBDistributionBin ){ first = fCurrentLBDistributionBin; second = fCentralCurrentLBDistributionBin; }
  else{ first = fCentralCurrentLBDistributionBin; second = fCurrentLBDistributionBin; }
  
  if ( first != second ){
    parnum = GetLBDistributionParIndex() + first;
    if ( fParametersVary[ parnum ] ) fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum;
    
    parnum = GetLBDistributionParIndex() + second;
    if ( fParametersVary[ parnum ] ) fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum;
  }
  
  else{
    parnum = GetLBRunNormalisationParIndex() + fCurrentLBRunNormalisationBin;
    if ( fParametersVary[ parnum ] ){ fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum; 
    }
  }
  return;

}
//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::IdentifyBaseVaryingParameters()
{

  if ( fVariableParameterIndex != NULL ) delete[] fVariableParameterIndex;
  fVariableParameterIndex = new Int_t[ fNParameters ];          // The number of unique parameters is guaranteed to be less than 
                                                                // the number of total parameters

  fNBaseVariableParameters = 0;
  if( fParametersVary[ GetInnerAVExtinctionLengthParIndex() ] ) fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetInnerAVExtinctionLengthParIndex();
  if( fParametersVary[ GetAVExtinctionLengthParIndex() ] ) fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetAVExtinctionLengthParIndex();
  if( fParametersVary[ GetWaterExtinctionLengthParIndex() ] ) fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetWaterExtinctionLengthParIndex();


  for ( Int_t iPar = 0; iPar < fNLBDistributionMaskParameters; iPar++ ){
    if ( fParametersVary[ GetLBDistributionMaskParIndex() + iPar ] ){
      fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetLBDistributionMaskParIndex() + iPar;
    }
  }

  printf("fNBaseVariableParameters = %d == \n",fNBaseVariableParameters);
  for (Int_t i = 1; i <= fNBaseVariableParameters; i++){
    printf("%d ",fVariableParameterIndex[ i ]);
    printf("\n");
  }

}

//////////////////////////////////////
//////////////////////////////////////

// void LOCASOpticsModel::IdentifyVaryingPMTAngularResponseBins( LOCASDataStore* lData )
// {
//   Int_t nPMTResponseBins = fModelParameterStore->GetNPMTAngularResponseBins()
//   Int_t* pmtAngValid = new Int_t[ nPMTResponseBins ];
//   for ( Int_t iAng = 0; iAng < nPMTResponseBins; iAng++ ){ 
//     pmtAngValid[ iAng ] = 0; 
//   }

//   std::vector< LOCASDataPoint >::iterator iDP;
//   for ( iDP = lData->GetLOCASDataPointsIterBegin(); 
//         iDP != lData->GetLOCASDataPointsIterEnd(); 
//         iDP++ ) {

//     ModelAngularResponse( *iDP, "off-axis" );
//     pmtAngValid[ fModelParameterStore->GetCurrentPMTAngularResponseBin() ]++;

//   }

//   for ( Int_t iAng = 0; iAng < nPMTResponseBins; iAng++ ){
//     if ( pmtAngValid[ iAng ] < 25 ){ 
//       fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetPMTAngularResponseParIndex() + iAng ] = 0;
//     }
//     else{
//       fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetPMTAngularResponseParIndex() + iAng ] = 1;
//     } 
//   }

// }

// //////////////////////////////////////
// //////////////////////////////////////

// void LOCASOpticsModel::IdentifyVaryingLBDistributionBins( LOCASDataStore* lData )
// {

//   Int_t nLBDistBins = fModelParameterStore->GetNLBDistributionCosThetaBins() * fModelParameterStore->GetNLBDistributionPhiBins();
//   Int_t* lbAngValid = new Int_t[ nLBDistBins ];
//   for ( Int_t iLB = 0; iLB < nLBDistBins; iLB++ ){ lbAngValid[ iLB ] = 0; }

//   std::vector< LOCASDataPoint >::iterator iDP;
//   for ( iDP = lData->GetLOCASDataPointsIterBegin(); 
//         iDP != lData->GetLOCASDataPointsIterEnd(); 
//         iDP++ ) {

//     ModelLBDistribution( *iDP, "off-axis" );
//     lbAngValid[ fModelParameterStore->GetCurrentLBDistributionBin() ]++;
    
//   }

//   for ( Int_t iLB = 0; iLB < nLBDistBins; iLB++ ){
//     if ( lbAngValid[ iLB ] < 25 ){ 
//       fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBDistributionParIndex() + iLB ] = 0;
//     }
//     else{
//       fModelParameterStore->GetParametersVary()[ fModelParameterStore->GetLBDistributionParIndex() + iLB ] = 1;
//     } 
//   }


// }

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::WriteToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );
  // Create the Run Tree
  TTree* runTree = new TTree( fileName, fileName );

  // Declare a new branch pointing to the parameter store
  runTree->Branch( "LOCASModelParameterStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::ReInitialiseParameters( const Double_t* pars )
{

  for ( Int_t iPar = 0; iPar < (Int_t)fParameters.size(); iPar++ ){
    ( fParameters[ iPar ] ).SetInitialValue( pars[ fParameters[ iPar ].GetIndex() ] );
  } 
  
}
