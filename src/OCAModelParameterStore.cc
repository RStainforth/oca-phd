#include "OCAModelParameterStore.hh"
#include "OCAModelParameter.hh"
#include "OCAPMTStore.hh"

#include "OCADB.hh"
#include "OCAMath.hh"

#include "TFile.h"
#include "TTree.h"
#include "TClass.h"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace OCA;

ClassImp( OCAModelParameterStore )

//////////////////////////////////////
//////////////////////////////////////

OCAModelParameterStore::OCAModelParameterStore( string& storeName )
{

  // Set the store name.
  fStoreName = storeName;
  fSystematicName = "";

  fSeededParameters = false;
  fSeedFile = "";

  fWaterFill = false;

  // Ensure the vector which will hold all the parameter objects
  // is empty to begin with.
  fParameters.clear();
  fParameterValues.ResizeTo( 0 );
  fCovarianceMatrixValues.ResizeTo( 0, 0 );

  // Ensure all the pointers are initialised to 'NULL'
  fParametersPtr = NULL;
  fParametersVary = NULL;
  fCovarianceMatrix = NULL;
  fDerivativeMatrix = NULL;
  fVariableParameterIndex = NULL;
  fVariableParameterMap = NULL;
  fPMTAngularResponseIndex = NULL;
  fCurrentAngularResponseBins = new vector< Int_t >[ 12 ];
  fCurrentLBDistributionBins = new vector< Int_t >[ 12 ];

  // Set the 'Int_t' type variables to non-interpretive values,
  // i.e. -1.
  fCurrentLBDistributionBin = -1;
  fCentralCurrentLBDistributionBin = -1;
  fCurrentPMTAngularResponseBin = -1;
  fCentralCurrentPMTAngularResponseBin = -1;
  fCurrentLBRunNormalisationBin = -1;

  fNCurrentVariableParameters = -1;
  fNBaseVariableParameters = -1;
  fNGlobalVariableParameters = -1;
    
  fNLBDistributionMaskParameters = -1;
  fNPMTAngularResponseBins = -1;
  fNLBDistributionCosThetaBins = -1;
  fNLBDistributionPhiBins = -1;
  fNLBRunNormalisations = -1;
  fNLBSinWaveSlices = -1;
  fNLBParametersPerSinWaveSlice = -1;
  fLBDistributionType = -1;
  fNLBDistributionPars = -1;
  
  fNParameters = -1;

  fFinalChiSquare = -10.0;
  fNumberOfDataPoints = -10;
  fReducedChiSquare = -10.0;
  
}

//////////////////////////////////////
//////////////////////////////////////

OCAModelParameterStore::~OCAModelParameterStore()
{

  //if ( fCurrentAngularResponseBins != NULL && fStoreName != "" ){ delete [] fCurrentAngularResponseBins; }
  //if ( fCurrentLBDistributionBins != NULL && fStoreName != "" ){ delete [] fCurrentLBDistributionBins; }

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::PrintParameterInfo()
{

  // For each parameter currently held in the store, print
  // out all the information for that parameter.
  vector< OCAModelParameter >::iterator iPar;

  cout << "Number of Parameters is: " << GetNParameters() << endl;
  cout << "--------------" << endl;

  for ( iPar = GetOCAModelParametersIterBegin();
        iPar != GetOCAModelParametersIterEnd();
        iPar++ ){

    iPar->PrintInfo();

  }

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t OCAModelParameterStore::SeedParameters( string& seedFileName, 
                                               string& fitFileName )
{

  // The idea of this is to seed the parameter values and errors from a previous
  // fit and use them as the initial values for a new fit. So, from the previous
  // fit we retrieve the parameter values and their errors, the actual model setup though
  // is as defined in the fit file. E.g. we can seed the fitted PMT Angular response from a
  // previous fit (where the PMT angular response varied) but it could be set to be
  // fixed in the current fit file. This is why this method feature database calls.

  // Create a OCADB object so that the 'fit-file' can be read.
  OCADB lDB;

  // Tell the OCADB object where to read the information from.
  lDB.SetFile( fitFileName.c_str() );

  fSeededParameters = true;
  fSeedFile = fitFileName;
  fParameters.clear();

  std::string seedFilePath = lDB.GetOutputDir() + "fits/" + seedFileName + ".root";
  cout << "OCAModelParameterStore::SeedParameters: Seeding from:\n";
  cout << seedFilePath << endl;
  
  TFile* tmpFile = TFile::Open( ( seedFilePath ).c_str() );
  TTree* tmpTree = (TTree*)tmpFile->Get( ( seedFileName +  "-nominal;1" ).c_str() );

  OCAModelParameterStore* tmpStore = new OCAModelParameterStore( fStoreName );
  tmpTree->SetBranchAddress( "nominal", &(tmpStore) );
  tmpTree->GetEntry( 0 );

  // Check whether we are attempting a water fill fit.
  fWaterFill = lDB.GetBoolField( "FITFILE", "water_fill", "parameter_setup" );

  fParameters.clear();

  // First retrieve the attenuation length parameters
  vector< OCAModelParameter >::iterator iPar;
  vector< OCAModelParameter >::iterator jPar;
  for ( iPar = tmpStore->GetOCAModelParametersIterBegin();
        iPar != tmpStore->GetOCAModelParametersIterEnd();
        iPar++ ){
    if ( iPar->GetIndex() == GetInnerAVExtinctionLengthParIndex() ){
      fParameters.push_back( *iPar );
      // (-1) because 'push_back' starts at 0
      fParameters[ GetInnerAVExtinctionLengthParIndex() - 1 ].SetVary( lDB.GetBoolField( "FITFILE", "inner_av_extinction_length_vary", "parameter_setup" ) );
    }
    if ( iPar->GetIndex() == GetAVExtinctionLengthParIndex() ){
      fParameters.push_back( *iPar );
      // (-1) because 'push_back' starts at 0
      fParameters[ GetAVExtinctionLengthParIndex() - 1 ].SetVary( lDB.GetBoolField( "FITFILE", "acrylic_extinction_length_vary", "parameter_setup" ) );
    }
    if ( iPar->GetIndex() == GetWaterExtinctionLengthParIndex() ){
        fParameters.push_back( *iPar );
      // (-1) because 'push_back' starts at 0
      fParameters[ GetWaterExtinctionLengthParIndex() - 1 ].SetVary( lDB.GetBoolField( "FITFILE", "water_extinction_length_vary", "parameter_setup" ) );
    }
  }
  
  // (+1 because the first mask parameter is fixed to 1.0)
  Int_t fileNMaskParameters = lDB.GetIntField( "FITFILE", "laserball_intensity_mask_number_of_parameters", "parameter_setup" ) + 1;
  Int_t seedNMaskParameters = tmpStore->GetNLBDistributionMaskParameters();
  if ( fileNMaskParameters == seedNMaskParameters ){
    fNLBDistributionMaskParameters = seedNMaskParameters;
    cout << "Seeding Number of Laserball Mask Parameters: " << fNLBDistributionMaskParameters << endl;
    for ( iPar = tmpStore->GetOCAModelParametersIterBegin();
          iPar != tmpStore->GetOCAModelParametersIterEnd();
          iPar++ ){
      if ( iPar->GetIndex() > 3 
           &&
           iPar->GetIndex() <= 3 + fNLBDistributionMaskParameters ){
        Bool_t varyCond = lDB.GetBoolField( "FITFILE", "laserball_intensity_mask_vary", "parameter_setup" );
        fParameters.push_back( *iPar );
        if ( varyCond && ( iPar->GetIndex() == 3 + 1 ) ){ 
          fParameters[ iPar->GetIndex()-1 ].SetVary( false );
        }
        else if ( varyCond && ( iPar->GetIndex() > 3 + 1 ) ){
          fParameters[ iPar->GetIndex()-1 ].SetVary( varyCond );
        }
        else{
          fParameters[ iPar->GetIndex()-1 ].SetVary( varyCond );
        }
      }
    }
  }
  else{
    cout << "OCAModelParameterStore::SeedParameters: Error, number of mask parameters on fitfile: " << fileNMaskParameters << ", differs from the seed file: " << seedNMaskParameters << ". Aborting..." << endl;
    return false;
  }

  Int_t fileNAngularResponseBins = lDB.GetIntField( "FITFILE", "pmt_angular_response_number_of_bins", "parameter_setup" );
  Int_t seedNAngularResponseBins = tmpStore->GetNPMTAngularResponseBins();
  if ( fileNAngularResponseBins == seedNAngularResponseBins ){
    fNPMTAngularResponseBins = seedNAngularResponseBins;
    cout << "Seeding Number of Angular Response Bins: " << fNPMTAngularResponseBins << endl;
    for ( iPar = tmpStore->GetOCAModelParametersIterBegin();
          iPar != tmpStore->GetOCAModelParametersIterEnd();
          iPar++ ){
      if ( iPar->GetIndex() > 3 + fNLBDistributionMaskParameters 
           &&
           iPar->GetIndex() <= 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins ){
        fParameters.push_back( *iPar );
        fParameters[ iPar->GetIndex()-1 ].SetVary( lDB.GetBoolField( "FITFILE", "pmt_angular_response_vary", "parameter_setup" ) );
      }
    }
  }
  else{
    cout << "OCAModelParameterStore::SeedParameters: Error, number of PMT angular response bin on fitfile: " << fileNAngularResponseBins << ", differs from the seed file: " << seedNAngularResponseBins << ". Aborting..." << endl;
    return false;
  }
  
  Int_t fileLBDistributionType = lDB.GetIntField( "FITFILE", "laserball_distribution_type", "parameter_setup" );
  Int_t seedLBDistributionType = tmpStore->GetLBDistributionType();
  if ( fileLBDistributionType == seedLBDistributionType ){
    cout << "Seeding Laserball DistributionType: " << seedLBDistributionType;
    if ( seedLBDistributionType == 0 ){ 
      cout << " Binned Histogram." << endl; 
      fNLBDistributionCosThetaBins = tmpStore->GetNLBDistributionCosThetaBins();
      fNLBDistributionPhiBins = tmpStore->GetNLBDistributionPhiBins();
      fNLBDistributionPars = tmpStore->GetNLBDistributionPars();
      cout << "Seeding Cos Theta Bins: " << fNLBDistributionCosThetaBins << endl;
      cout << "Seeding Phi Bins: " << fNLBDistributionPhiBins << endl;
      cout << "Seeding Laserball Distribution Pars: " << fNLBDistributionPars << endl;
    }
    if ( seedLBDistributionType == 1 ){ 
      cout << " Sinusoidal Function." << endl; 
      fNLBSinWaveSlices = tmpStore->GetNLBSinWaveSlices();
      fNLBParametersPerSinWaveSlice = tmpStore->GetNLBParametersPerSinWaveSlice();
      fNLBDistributionPars = tmpStore->GetNLBDistributionPars();
      cout << "Seeding Sin Wave Slices: " << fNLBSinWaveSlices << endl;   
      cout << "Seeding Laserball Parameters Per Slice: " << fNLBParametersPerSinWaveSlice << endl;
      cout << "Seeding Laserball Distribution Pars: " << fNLBDistributionPars << endl;
    }
    else{ return false; }
    fLBDistributionType = tmpStore->GetLBDistributionType();
    for ( iPar = tmpStore->GetOCAModelParametersIterBegin();
          iPar != tmpStore->GetOCAModelParametersIterEnd();
          iPar++ ){
      if ( iPar->GetIndex() > 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins
           &&
           iPar->GetIndex() <= 3 + fNLBDistributionMaskParameters + fNPMTAngularResponseBins + fNLBDistributionPars ){
        fParameters.push_back( *iPar );
        fParameters[ iPar->GetIndex()-1 ].SetVary( lDB.GetBoolField( "FITFILE", "laserball_distribution_vary", "parameter_setup" ) );
      }
    }
  }
  else{
    cout << "OCAModelParameterStore::SeedParameters: Error, laserball distribution type on fitfile: " << fileLBDistributionType << ", differs from the seed file: " << seedLBDistributionType << ". Aborting..." << endl;
    return false;
  }

  
  vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
  fNLBRunNormalisations = (Int_t)( runIDs.size() );
  for ( Int_t nNorms = 1; nNorms <= fNLBRunNormalisations; nNorms++ ){
    
    Int_t parIndex = 3 + 
      fNLBDistributionMaskParameters + 
      fNPMTAngularResponseBins + 
      fNLBDistributionPars + 
      nNorms;
    OCAModelParameter lParameter( "laserball_run_normalisations", parIndex, 300.0, 
                                  -100.0, 100.0, -10.0, fNLBRunNormalisations, 
                                  lDB.GetIntField( "FITFILE", "laserball_run_normalisation_vary", "parameter_setup" ) );
    fParameters.push_back( lParameter );
  }
  
  fNParameters = fParameters.size();

  // Resize the parameter vector and covariance matrix, and then zero all the values...
  fParameterValues.ResizeTo( fNParameters + 1 );
  fCovarianceMatrixValues.ResizeTo( fNParameters + 1, fNParameters + 1 );
  for ( Int_t iVal = 0; iVal < fNParameters + 1; iVal++ ){
    fParameterValues( iVal ) = 0.0;
    for ( Int_t jVal = 0; jVal < fNParameters + 1; jVal++ ){
      fCovarianceMatrixValues[ iVal ][ jVal ] = 0.0;
    }
  }

  // Now assign the values where applicable, but omit normalisation parameters.
  TMatrix tmpMatrix = tmpStore->GetCovarianceMatrixValues();
  TVector tmpVector = tmpStore->GetParameterValues();
  for ( Int_t iParam = 1; iParam < GetLBRunNormalisationParIndex(); iParam++ ){
    fParameterValues( iParam ) = tmpVector( iParam );
    for ( Int_t jParam = 1; jParam < GetLBRunNormalisationParIndex(); jParam++ ){
      fCovarianceMatrixValues[ iParam ][ jParam ] = tmpMatrix[ iParam ][ jParam ];
    }
  }

  if ( fWaterFill ){ fParameters[ GetWaterExtinctionLengthParIndex() - 1 ].SetVary( false ); }

  AllocateParameterArrays();
  delete tmpStore;

  return true;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::AddParameters( string& fileName )
{

  // Create a OCADB object so that the 'fit-file' can be read.
  OCADB lDB;

  // Tell the OCADB object where to read the information from.
  lDB.SetFile( fileName.c_str() );

  // Initialise the variables which will be defined
  // for each parameter which is to be created in the
  // loops which follow.
  Float_t maxVal = 0.0;
  Float_t minVal = 0.0;
  Float_t initVal = 0.0;
  Float_t incVal = 0.0;
  Int_t nParsInGroup = 0;
  Bool_t varyBool = false;
  fWaterFill = lDB.GetBoolField( "FITFILE", "water_fill", "parameter_setup" );

  // List of runs.
  vector< Int_t > indexList;

  // String stream and temport string object for string concatenation.
  stringstream lStream;
  string parStr = "";

  // Get the list of the parameters to be included in the parameter store.
  vector< string > paramList = lDB.GetStringVectorField( "FITFILE", "parameters_list", "parameter_setup" );

  // First find out the specifics for the number of bins required 
  // for each parameter type.
  // i.e. - Laserball Mask Function: Number of Parameters 'fNLBDistributionMaskParameters'
  //      - PMT Angular Response: Number of bins between 0-degrees and 90-degrees 'fNPMTAngularResponseBins'
  //      - Laserball Isotropy Distribution 2D ( CosTheta, Phi ) Histogram:
  //            * Number of bins in CosTheta between -1.0 and +1.0 'fNLBDistributionCosThetaBins'
  //            * Number of bins in Phi between -pi and pi 'fNLBDistributionPhiBins'

  // Loop over each parameter set in the card file and add them/it to the store.
  for ( Int_t iStr = 0; iStr < (Int_t)paramList.size(); iStr++ ){

    // Laserball distribution mask.
    if ( paramList[ iStr ] == "laserball_intensity_mask" ){
      fNLBDistributionMaskParameters = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_parameters" ), "parameter_setup" ) + 1;
    }

    // PMT angular response.
    else if ( paramList[ iStr ] == "pmt_angular_response" ){
      fNPMTAngularResponseBins = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_bins" ), "parameter_setup" );
    }

    // Laserball distribution hisotgram.
    else if ( paramList[ iStr ] == "laserball_distribution" ){
      fLBDistributionType = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_type" ), "parameter_setup" );
      if ( fLBDistributionType == 0 ){
        fNLBDistributionCosThetaBins = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_cos_theta_bins" ), "parameter_setup" );
        fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_phi_bins" ), "parameter_setup" );
      }
      else if ( fLBDistributionType == 1 ){
        fNLBSinWaveSlices = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_theta_slices" ), "parameter_setup" );
        fNLBParametersPerSinWaveSlice = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_parameters_per_theta_slice" ), "parameter_setup" );
      }
      else{
        cout << "Error: Unknown laserball type specified: " << fLBDistributionType << endl;
      }
    }

    // Laserball run normalisation.
    else if ( paramList[ iStr ] == "laserball_run_normalisation" ){
      vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
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
      initVal = lDB.GetDoubleField( "FITFILE", (string)( paramList[ iStr ] + "_initial_value" ), "parameter_setup" );

      // Whether or not this parameter will vary in the fit ( 1: Yes, 0: No )
      varyBool = lDB.GetBoolField( "FITFILE", (string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

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
      else{ cout << "OCAModelParameterStore::OCAModelParameterStore: Error, unknown parameter passed" << endl; }

      OCAModelParameter lParameter( (string)( paramList[ iStr ] ), parIndex, initVal, 
                                      minVal, maxVal, incVal, nParsInGroup, varyBool );
      AddParameter( lParameter );

    }

    if ( paramList[ iStr ] == "laserball_intensity_mask" ){

      nParsInGroup = fNLBDistributionMaskParameters;

      varyBool = lDB.GetBoolField( "FITFILE", (string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

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
        OCAModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, nParsInGroup, parVary );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "pmt_angular_response" ){

      nParsInGroup = fNPMTAngularResponseBins;

      varyBool = lDB.GetBoolField( "FITFILE", (string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      Float_t angleVal = 0.0;
      // Now loop over each angular response parameter and initialise its initial values;
      for ( Int_t iPar = 1; iPar <= nParsInGroup; iPar++ ){

        angleVal = ( iPar - 0.5 ) * ( 90.0 / fNPMTAngularResponseBins ); // Centre of each bin...
        if ( angleVal < 36.0 ){ 
          if ( iPar == 1.0 ){ initVal = 1.0; }
          else{
            initVal = 1.0 + ( 0.002222 * angleVal ); 
          }
        }
        else{ initVal = 1.0; }

        // Fix the bin containing the zero degrees value to 1.0
        if ( iPar == 0 ){ initVal = 1.0; }

        Bool_t parVary = varyBool;
        // If the angular response has been set to vary, then the first parameter is held fixed, all others can vary
        if ( varyBool == true && iPar == 0 ){ parVary = false; }

        // The angular response parameters have an absolutel minimum of 1.0, and are allowed to 
        // go up to 2.0, however it usually doesn't get much higher than 1.25
        minVal = 1.0;
        maxVal = 1.25;

        // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
        incVal = -10.0;

        lStream << "_";
        lStream << iPar;
        lStream >> parStr;

        Int_t parIndex = 3 + 
          fNLBDistributionMaskParameters + 
          iPar;
        OCAModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, nParsInGroup, parVary );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "laserball_distribution" ){

      if ( fLBDistributionType == 0 ){
        fNLBDistributionPars = fNLBDistributionPhiBins * fNLBDistributionCosThetaBins;
      }
      if ( fLBDistributionType == 1 ){
        fNLBDistributionPars = fNLBSinWaveSlices * fNLBParametersPerSinWaveSlice;
      }

      varyBool = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

      // Now loop over each angular response parameter and initialise its initial values;
      for ( Int_t iPar = 1; iPar <= fNLBDistributionPars; iPar++ ){
        //Bool_t parVary = varyBool;
        if ( fLBDistributionType == 0 ){ initVal = 1.0; }
        if ( fLBDistributionType == 1 && iPar % 2 == 1 ){ initVal = 0.01; }
        if ( fLBDistributionType == 1 && iPar % 2 == 0 ){ initVal = 1.0; }
        //if ( fLBDistributionType == 1 && iPar == 1 ){ parVary = false; }
        // The laserball distribution parameters in the histogram will not vary beyond 0.0 and 2.0, so set
        // these accordingly
        minVal = 0.0;
        maxVal = 2.0;

        // The increment value is currently not used, so set to something unphysical (might have use for this in a future fitting routine)
        incVal = -10.0;

        lStream << "_";
        lStream << iPar;
        lStream >> parStr;

        Int_t parIndex = 3 + 
          fNLBDistributionMaskParameters + 
          fNPMTAngularResponseBins + 
          iPar;
        OCAModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, fNLBDistributionPars, varyBool );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "laserball_run_normalisation" ){

      nParsInGroup = fNLBRunNormalisations;

      vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
      initVal = 1.0;
      varyBool = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

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

        Int_t parIndex = 3 + 
          fNLBDistributionMaskParameters + 
          fNPMTAngularResponseBins + 
          fNLBDistributionPars + 
          iPar;
        OCAModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, nParsInGroup, varyBool );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

  }

  if ( fWaterFill ){ fParameters[ GetWaterExtinctionLengthParIndex() - 1 ].SetVary( false ); }

  fNParameters = (Int_t)fParameters.size();

  AllocateParameterArrays();
}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::WriteToROOTFile( string& fileName, 
                                              string& branchName )
{
  TFile* file = NULL;

  // Check that the main-run file exists
  ifstream rFile( fileName.c_str() );
  if ( rFile ){ 
    file = new TFile( fileName.c_str(), "UPDATE" );
  }
  else{ 
    file = new TFile( fileName.c_str(), "CREATE" ); 
  }
  rFile.close();

  TTree* parTree = new TTree( ( fStoreName + "-" + branchName ).c_str(), 
                              ( fStoreName + "-" + branchName ).c_str() );

  if ( branchName == "nominal" ){
    TH2F* lbDistribution = GetLBDistributionHistogram();
    if ( fLBDistributionType == 0 ){
      file->WriteTObject( lbDistribution, "Laserball Angular Distribution Histogram (Binned)", "Overwrite" );
      parTree->Branch( "Laserball Angular Distribution Histogram (Binned)", lbDistribution->ClassName(), 
                       &(*lbDistribution), 32000, 99 );
    }
    if ( fLBDistributionType == 1 ){
      file->WriteTObject( lbDistribution, "Laserball Angular Distribution Histogram (Sinusoidal)", "Overwrite" );
      parTree->Branch( "Laserball Angular Distribution Histogram (Sinusoidal)", lbDistribution->ClassName(), 
                       &(*lbDistribution), 32000, 99 );
    }

    
    TH2F* lbDistributionIntensity = GetLBDistributionIntensityHistogram();
    file->WriteTObject( lbDistributionIntensity, "Laserball Intensity Distribution Histogram", "Overwrite" );
    parTree->Branch( "Laserball Intensity Distribution Histogram", lbDistributionIntensity->ClassName(), 
                     &(*lbDistributionIntensity), 32000, 99 );
    
    TH1F* angularResponse = GetPMTAngularResponseHistogram();
    file->WriteTObject( angularResponse, "PMT Angular Response", "Overwrite" );
    parTree->Branch( "PMT Angular Response", angularResponse->ClassName(), 
                     &(*angularResponse), 32000, 99 );
    
    TF1* angularResponseTF1 = GetPMTAngularResponseFunction();
    file->WriteTObject( angularResponseTF1, "PMT Angular Response Function", "Overwrite" );
    parTree->Branch( "PMT Angular Response Function", angularResponseTF1->ClassName(), 
                     &(*angularResponseTF1), 32000, 99 );
    
    TF1* lbDistributionTF1 = GetLBDistributionMaskFunction();
    file->WriteTObject( lbDistributionTF1, "Laserball Distribution Mask", "Overwrite" );
    parTree->Branch( "Laserball Distribution Mask", lbDistributionTF1->ClassName(), 
                     &(*lbDistributionTF1), 32000, 99 );
  }

  // Declare a new branch pointing to the parameter store
  parTree->Branch( branchName.c_str(), (this)->ClassName(), &(*this), 32000, 99 );
  file->cd();
  
  // Fill the tree and write it to the file
  parTree->Fill();
  parTree->Write("", TObject::kOverwrite);

  file->Close();
  delete file;
  cout << "The OCA::OCAModelParameterStore has been written to the TTree:\n";
  cout << ( fStoreName + "-" + branchName ) << "\n";
  cout << "And saved to the ROOT file:\n";
  cout << fileName << "\n";

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::WriteToRATDBFile( const char* fileName )
{

  ofstream roccVals;
  roccVals.precision( 6 );
  roccVals.open ( fileName );
  roccVals << "{\n";
  roccVals << "name : \"FITRESULTS\",\n";
  roccVals << "index : \"" << fStoreName << "\",\n";

  roccVals << "\n";
  
  roccVals << "valid_begin : [0, 0],\n";
  roccVals << "valid_end : [0, 0],\n";
  
  roccVals << "\n";
  roccVals << "// The extinction lengths [mm^-1] for the inner av, av and water regions.\n";
  roccVals << "inner_av_extinction_length : " << GetInnerAVExtinctionLengthPar() << ",\n";
  roccVals << "inner_av_extinction_length_error : " << TMath::Sqrt( fCovarianceMatrix[ GetInnerAVExtinctionLengthParIndex() ][ GetInnerAVExtinctionLengthParIndex() ] ) << ",\n";
  roccVals << "\n";

  roccVals << "acrylic_extinction_length : " << GetAVExtinctionLengthPar() << ",\n";
  roccVals << "acrylic_extinction_length_error : " << TMath::Sqrt( fCovarianceMatrix[ GetAVExtinctionLengthParIndex() ][ GetAVExtinctionLengthParIndex() ] ) << ",\n";

  roccVals << "\n";

  roccVals << "water_extinction_length : " << GetWaterExtinctionLengthPar() << ",\n";
  roccVals << "water_extinction_length_error : " << TMath::Sqrt( fCovarianceMatrix[ GetWaterExtinctionLengthParIndex() ][ GetWaterExtinctionLengthParIndex() ] ) << ",\n";

  roccVals << "\n";
  roccVals << "// The laserball distribution mask parameters.\n";
  Int_t nMaskPars = GetNLBDistributionMaskParameters();
  Float_t* maskPtr = &fParametersPtr[ GetLBDistributionMaskParIndex() ];
  roccVals << "laserball_intensity_mask_number_of_parameters : " << nMaskPars << ",\n";
  roccVals << "laserball_intensity_mask_parameters : [ ";
  for ( Int_t iPar = 0; iPar < nMaskPars; iPar++ ){
    
    if ( iPar == nMaskPars - 1 ){
      roccVals << maskPtr[ iPar ] << " ],\n";
    }
    else{
      roccVals << maskPtr[ iPar ] << ", ";
    }

  }
  roccVals << "laserball_intensity_mask_parameters_errors : [ ";
  for ( Int_t iPar = 0; iPar < nMaskPars; iPar++ ){
    
    if ( iPar == nMaskPars - 1 ){
      roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionMaskParIndex() + iPar ][ GetLBDistributionMaskParIndex() + iPar ] ) << " ],\n";
    }
    else{
      roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionMaskParIndex() + iPar ][ GetLBDistributionMaskParIndex() + iPar ] ) << ", ";
    }

  }

  roccVals << "\n";

  Float_t* lbDistPtr = &fParametersPtr[ GetLBDistributionParIndex() ];
  Int_t lbType = GetLBDistributionType();
  if ( lbType == 0 ){
    Int_t nCThetaBins = GetNLBDistributionCosThetaBins();
    Int_t nPhiBins = GetNLBDistributionPhiBins();
    roccVals << "// The laserball distribution histogram parameters.\n";
    roccVals << "laserball_distribution_histogram_number_of_phi_bins : " << nPhiBins << ",\n";
    roccVals << "laserball_distribution_histogram_number_of_cos_theta_bins : " << nCThetaBins << ",\n";
    roccVals << "laserball_distribution_histogram : [ ";
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t iPhi = 0; iPhi < nPhiBins; iPhi++ ){
        if ( iTheta == nCThetaBins - 1
             && iPhi == nPhiBins - 1 ){
          roccVals << lbDistPtr[ iTheta*nCThetaBins + iPhi ] << " ],\n";
        }
        else{
          roccVals << lbDistPtr[ iTheta*nCThetaBins + iPhi ] << ", ";
        }
      }
      roccVals << "\n";
    }
    roccVals << "laserball_distribution_errors : [ ";
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t iPhi = 0; iPhi < nPhiBins; iPhi++ ){
        if ( iTheta == nCThetaBins - 1
             && iPhi == nPhiBins - 1 ){
          roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionParIndex() + iTheta*nCThetaBins + iPhi ][ GetLBDistributionParIndex() + iTheta*nCThetaBins + iPhi ] ) << " ],\n";
        }
        else{
          roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionParIndex() + iTheta*nCThetaBins + iPhi ][ GetLBDistributionParIndex() + iTheta*nCThetaBins + iPhi ] ) << ", ";
        }
      }
      roccVals << "\n";
    }
  }
  if ( lbType == 1 ){
    Int_t nThetaSlices = GetNLBSinWaveSlices();
    Int_t nParsPerSlice = GetNLBParametersPerSinWaveSlice();
    roccVals << "// The laserball distribution sin-wave parameters.\n";
    roccVals << "laserball_distribution_number_of_cos_theta_slices : " << nThetaSlices << ",\n";
    roccVals << "laserball_distribution_number_of_parameters_per_cos_theta_slice : " << nParsPerSlice << ",\n";
    roccVals << "laserball_distribution_sin_wave : [ ";
    for ( Int_t iPar = 1; iPar <= ( nThetaSlices * nParsPerSlice ); iPar++ ){

      if ( iPar % 2 == 0 && iPar == ( nThetaSlices * nParsPerSlice ) ){
        roccVals << lbDistPtr[ iPar ] << " ],\n";
      }
      if ( iPar % 2 == 0 && iPar % 8 == 0 ){
        roccVals << lbDistPtr[ iPar ] << ",\n";
      }
      else if ( iPar % 2 == 1 ){
        roccVals << lbDistPtr[ iPar ] << ", ";
      }
      else if ( iPar % 2 == 0 ){
        roccVals << lbDistPtr[ iPar ] << ",     ";
      }     
      roccVals << "\n";
    }
    roccVals << "laserball_distribution_errors : [ ";
    for ( Int_t iPar = 1; iPar <= ( nThetaSlices * nParsPerSlice ); iPar++ ){

      if ( iPar % 2 == 0 && iPar == ( nThetaSlices * nParsPerSlice ) ){
        roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionParIndex() + iPar ][ GetLBDistributionParIndex() + iPar ] ) << " ],\n";
      }
      if ( iPar % 2 == 0 && iPar % 8 == 0 ){
        roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionParIndex() + iPar ][ GetLBDistributionParIndex() + iPar ] ) << ",\n";
      }
      else if ( iPar % 2 == 1 ){
        roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionParIndex() + iPar ][ GetLBDistributionParIndex() + iPar ] ) << ", ";
      }
      else if ( iPar % 2 == 0 ){
        roccVals << TMath::Sqrt( fCovarianceMatrix[ GetLBDistributionParIndex() + iPar ][ GetLBDistributionParIndex() + iPar ] ) << ",     ";
      }
    roccVals << "\n";
    } 
    roccVals << "\n";
  }
  roccVals << "// The PMT angular response parameters.\n";
  Int_t nPMTAngPars = GetNPMTAngularResponseBins();
  Float_t* pmtAngPtr = &fParametersPtr[ GetPMTAngularResponseParIndex() ];
  roccVals << "pmt_angular_response_number_of_bins : " << nPMTAngPars << ",\n";
  roccVals << "pmt_angular_response : [ ";
  for ( Int_t iPar = 0; iPar < nPMTAngPars; iPar++ ){
    
    if ( iPar == nPMTAngPars - 1 ){
      roccVals << pmtAngPtr[ iPar ] << " ],\n";
    }
    else{
      roccVals << pmtAngPtr[ iPar ] << ", ";
    }

  }
  roccVals << "pmt_angular_response_errors : [ ";
  for ( Int_t iPar = 0; iPar < nPMTAngPars; iPar++ ){
    
    if ( iPar == nPMTAngPars - 1 ){
      roccVals << TMath::Sqrt( fCovarianceMatrix[ GetPMTAngularResponseParIndex() + iPar ][ GetPMTAngularResponseParIndex() + iPar ] ) << " ],\n";
    }
    else{
      roccVals << TMath::Sqrt( fCovarianceMatrix[ GetPMTAngularResponseParIndex() + iPar ][ GetPMTAngularResponseParIndex() + iPar ] ) << ", ";
    }

  }

  roccVals << "\n}";
  roccVals.close();

  cout << "OCA::OCAModelParameterStore: Fitted parameters saved to RATDB file:\n";
  cout << fileName << "\n";

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::AllocateParameterArrays()
{

  // Allocate memory for the pointer which will hold the parameter values.
  fParametersPtr = OCAMath::OCAVector( 1, fNParameters );

  // Allocate memory for the pointer which will hold the varying map
  // for each of the parameters (1: Vary, 0: Fixed)
  fParametersVary = OCAMath::OCAIntVector( 1, fNParameters );

  // Allocate the memory for the covariance and derivative matrices.
  fCovarianceMatrix = OCAMath::OCAMatrix( 1, fNParameters, 1, fNParameters );
  fDerivativeMatrix = OCAMath::OCAMatrix( 1, fNParameters, 1, fNParameters );

  // First initialise all the values in the above pointers to zero.
  for ( Int_t iIndex = 1; iIndex <= fNParameters; iIndex++ ){
    fParametersPtr[ iIndex ] = 0.0;
    fParametersVary[ iIndex ] = 0;
    for ( Int_t jIndex = 1; jIndex <= fNParameters; jIndex++ ){
      fCovarianceMatrix[ iIndex ][ jIndex ] = 0.0;
      fDerivativeMatrix[ iIndex ][ jIndex ] = 0.0;
    }
  }

  // Now put each of the initial parameter values into the
  // parameter pointer array from the vector of OCAModelParameter objects.
  // i.e. OCAModelParameter --> Pointer Array (fParametersPtr).
  // And also assign whether or not the parameter will vary.
  // i.e. OCAModelParameter --> Pointer Array (fParametersVary);
  vector< OCAModelParameter >::iterator iPar;
  for ( iPar = GetOCAModelParametersIterBegin();
        iPar != GetOCAModelParametersIterEnd();
        iPar++ ){

    // Set the initial value of the parameter in the array.
    if ( fSeededParameters ){ 
      fParametersPtr[ iPar->GetIndex() ] = iPar->GetFinalValue();
      vector< OCAModelParameter >::iterator jPar;
      for ( jPar = GetOCAModelParametersIterBegin();
            jPar != GetOCAModelParametersIterEnd();
            jPar++ ){
        fCovarianceMatrix[ iPar->GetIndex() ][ jPar->GetIndex() ] = fCovarianceMatrixValues[ iPar->GetIndex() ][ jPar->GetIndex() ];
      }
    }
    else{
      fParametersPtr[ iPar->GetIndex() ] = iPar->GetInitialValue();
    }

    // Set whether or not the parameter is required to vary (0: Fixed, 1: Vary).
    if ( iPar->GetVary() ){ 
      fParametersVary[ iPar->GetIndex() ] = 1; 
    }
    else{ 
      fParametersVary[ iPar->GetIndex() ] = 0; 
    }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::InitialisePMTAngularResponseIndex()
{

  // // Allocate memory for the look-up index to be used to 
  // // keep track of the ordered variable parameters in the fit.
  if ( fVariableParameterMap != NULL ){ delete[] fVariableParameterMap; }
  fVariableParameterMap = new Int_t[ fNParameters + 1 ];

  Int_t iVal = 0; 
  Int_t jVal = 0;

  // First assign the order of variability to the variable
  // parameter map for parameters which vary in the fit.
  jVal = 0;
  for ( iVal = 1; iVal <= fNParameters; iVal++ ){
    if ( fParametersVary[ iVal ] ){
      fVariableParameterMap[ iVal ] = ++jVal;
    }
  }
  
  // If the look-up array for the PMT Angular Response already exists,
  // delete it. 
  // As the array is a pointer to an array of arrays each holding an array!
  // We need three loops to deallocate all the memory.
  if ( fPMTAngularResponseIndex != NULL ){
    for ( iVal = 0; iVal < fNPMTAngularResponseBins + 1; iVal++ ){
      for ( jVal = 0; jVal < fNPMTAngularResponseBins + 1; jVal++ ){
	delete[] fPMTAngularResponseIndex[ iVal ][ jVal ];
      }
      delete[] fPMTAngularResponseIndex[ iVal ];
    }
    delete[] fPMTAngularResponseIndex;
  }
  
  // Ensure the pointer to the array is 'NULL'.
  fPMTAngularResponseIndex = NULL;
  
  // Now allocate memory for a new PMT Angular response index look-up.
  // The idea of the index look-up is to keep a track of which bins
  // and how many vary between an evaluation of a data point from the
  // off-axis and central calculations.
  // e.g. data point A may use the 10-th PMT angular response bin 
  //      for the off-axis run, and the 0-th PMT angular response bin
  //      for the central run. So two bins will be associated with that
  //      data point for the PMT angular response.
  //
  //      data point B may use the 0-th bin for BOTH off-axis and
  //      central runs. So only one bin will be associate with that
  //      data point for the PMT angular response.
  fPMTAngularResponseIndex = new Int_t**[ fNPMTAngularResponseBins + 1 ];
  for ( iVal = 0; iVal < fNPMTAngularResponseBins + 1; iVal++ ){
    fPMTAngularResponseIndex[ iVal ] = new Int_t*[ fNPMTAngularResponseBins + 1 ];
    for ( jVal = 0; jVal < fNPMTAngularResponseBins + 1; jVal++ ){ 
      fPMTAngularResponseIndex[ iVal ][ jVal ] = new Int_t[ 4 + 1 ];
    }
  }
  Int_t first = 0;
  Int_t second = 0;

  // Fill the PMT angular response index look-up.
  for ( iVal = 0; iVal <= fNPMTAngularResponseBins; iVal++ ) {
    for ( jVal = 0; jVal <= fNPMTAngularResponseBins; jVal++ ) {
      if ( iVal <= jVal ) { first = iVal; second = jVal; }
      else { first = jVal; second = iVal; }
      fPMTAngularResponseIndex[ iVal ][ jVal ][ 1 ] = first;
      fPMTAngularResponseIndex[ iVal ][ jVal ][ 2 ] = second;
      if ( first == second ) {
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 0 ] = 1;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 1 ] = first;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 2 ] = -1;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 3 ] = -1;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 4 ] = -1;
      } 
      else {
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 0 ] = 2;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 1 ] = first;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 2 ] = second;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 3 ] = -1;
        fPMTAngularResponseIndex[ iVal ][ jVal ][ 4 ] = -1;
      }
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::IdentifyVaryingParameters()
{

  Int_t iVal = 0;

  // Set the number of variable parameters for the current data point to 
  // be equal to the base number of parameters to be gin with
  // i.e. the variables we know vary for all data points.
  fNCurrentVariableParameters = fNBaseVariableParameters;
                                                                           
  Int_t parnum = 0;

  // Fill the variable parameter index with the indices of the parameters
  // which vary for the current data point.
  for ( iVal = 1; iVal <= fPMTAngularResponseIndex[ fCurrentPMTAngularResponseBin ][ fCentralCurrentPMTAngularResponseBin ][ 0 ]; iVal++ ){
    
    parnum = GetPMTAngularResponseParIndex() + fPMTAngularResponseIndex[ fCurrentPMTAngularResponseBin ][ fCentralCurrentPMTAngularResponseBin ][ iVal ];
    if ( fParametersVary[ parnum ] ){
      fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum;
    }

  }
 
  // Identify which bins varied, and in which order for the
  // laserball distribution. Then give the associated parameter indices
  // linked to those bins to the variable parameter index array.
  if ( fLBDistributionType == 0 ){
    Int_t first = 0;
    Int_t second = 0;
    if ( fCurrentLBDistributionBin <= fCentralCurrentLBDistributionBin ){ 
      first = fCurrentLBDistributionBin; 
      second = fCentralCurrentLBDistributionBin; 
    }
    else{ 
      first = fCentralCurrentLBDistributionBin; 
      second = fCurrentLBDistributionBin; 
    }
    
    if ( first != second ){
      parnum = GetLBDistributionParIndex() + first;
      if ( fParametersVary[ parnum ] ){ 
        fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum;
      }
      
      parnum = GetLBDistributionParIndex() + second;
      if ( fParametersVary[ parnum ] ){ 
        fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum;
      }
    }
  }


  if(fLBDistributionType == 1){
    Int_t firstSlice = GetCurrentLBDistributionBin();
    Int_t secondSlice = GetCentralCurrentLBDistributionBin();
    Int_t first = 0;
    Int_t second = 0;
    if ( firstSlice < secondSlice ){ first = firstSlice; second = secondSlice; }
    if ( secondSlice < firstSlice ){ first = secondSlice; second = firstSlice; }
    if ( secondSlice == firstSlice ){ first = firstSlice; }

    if ( first != second ){
      if ( fParametersVary[ GetLBDistributionParIndex() + first*2 ] ){
        fVariableParameterIndex[++fNCurrentVariableParameters] = GetLBDistributionParIndex() + first*2;
      }
      if ( fParametersVary[ GetLBDistributionParIndex() + first*2 + 1 ] ){
        fVariableParameterIndex[++fNCurrentVariableParameters] = GetLBDistributionParIndex() + first*2 + 1;
      }
      if ( fParametersVary[ GetLBDistributionParIndex() + second*2 ] ){
        fVariableParameterIndex[++fNCurrentVariableParameters] = GetLBDistributionParIndex() + second*2;
      }
      if ( fParametersVary[ GetLBDistributionParIndex() + second*2 + 1 ] ){
        fVariableParameterIndex[++fNCurrentVariableParameters] = GetLBDistributionParIndex() + second*2 + 1;
      }
    }
    else{
      if ( fParametersVary[ GetLBDistributionParIndex() + first*2 ] ){
        fVariableParameterIndex[++fNCurrentVariableParameters] = GetLBDistributionParIndex() + first*2;
      }
      if ( fParametersVary[ GetLBDistributionParIndex() + first*2 + 1 ] ){
        fVariableParameterIndex[++fNCurrentVariableParameters] = GetLBDistributionParIndex() + first*2 + 1;
      }
    }
  }


  
  parnum = GetLBRunNormalisationParIndex() + fCurrentLBRunNormalisationBin;
  if ( fParametersVary[ parnum ] ){ 
    fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum; 
  }
  
  return;

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::IdentifyBaseVaryingParameters()
{

  if ( fVariableParameterIndex != NULL ) { delete[] fVariableParameterIndex; }
  // The number of unique parameters is guaranteed to be less than 
  // the number of total parameters.
  fVariableParameterIndex = new Int_t[ fNParameters + 1 ];

  // First set the number of base varying parameters to zero.
  fNBaseVariableParameters = 0;

  // Now check which of the extinction lengths vary in the fit.
  // For those which vary, add their parameter index to the variable parameter
  // index array.
  if( fParametersVary[ GetInnerAVExtinctionLengthParIndex() ] ){ 
    fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetInnerAVExtinctionLengthParIndex();
  }
  if( fParametersVary[ GetAVExtinctionLengthParIndex() ] ){ 
    fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetAVExtinctionLengthParIndex();
  }
  if( fParametersVary[ GetWaterExtinctionLengthParIndex() ] ){ 
    fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetWaterExtinctionLengthParIndex();
  }

  // Now check whether the laserball distribution mask is set to vary
  // in the fit. If so, assign the parameter indices associated with the mask
  // function to the variable parameter index array.
  for ( Int_t iPar = 0; iPar < fNLBDistributionMaskParameters; iPar++ ){
    if ( fParametersVary[ GetLBDistributionMaskParIndex() + iPar ] ){
      fVariableParameterIndex[ ++fNBaseVariableParameters ] = GetLBDistributionMaskParIndex() + iPar;
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::IdentifyGlobalVaryingParameters()
{

  fNGlobalVariableParameters = 0;
  for ( Int_t iPar = 1; iPar <= fNParameters; iPar++ ){
    if ( fParametersVary[ iPar ] == 1 ){ fNGlobalVariableParameters++; }
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCAModelParameterStore::CrossCheckParameters()
{

  // This routine ensures that all the parameters are assigned to their
  // correct values and errors. This also fills in the full covariance
  // matrix, a vector of individual parameter values and the
  // chisquare, and residual chisquare values.
  vector< OCAModelParameter >::iterator iPar;
  for ( iPar = GetOCAModelParametersIterBegin();
        iPar != GetOCAModelParametersIterEnd();
        iPar++ ){

    iPar->SetFinalValue( fParametersPtr[ iPar->GetIndex() ] );
    iPar->SetVary( fParametersVary[ iPar->GetIndex() ] );
    Float_t covValue = fCovarianceMatrix[ iPar->GetIndex() ][ iPar->GetIndex() ];

    // In the case of water fill, we will set the outer water
    // region to the value fitted to the first
    if ( fWaterFill && iPar->GetIndex() == GetWaterExtinctionLengthParIndex() ){
      iPar->SetFinalValue( fParametersPtr[ iPar->GetIndex() ] );
    }
      
    if ( covValue > 0.0 ){
      iPar->SetError( TMath::Sqrt( covValue ) );
    }

  }

  fCovarianceMatrixValues.ResizeTo( 0, 0 );
  fParameterValues.ResizeTo( 0 );
  fCovarianceMatrixValues.ResizeTo( 1 + fNParameters, 1 + fNParameters );
  fParameterValues.ResizeTo( 1 + fNParameters );
  for ( Int_t iVar = 0; iVar <= fNParameters; iVar++ ){
    if ( iVar == 0 ){ fParameterValues( iVar ) = 0.0; }
    else{ fParameterValues( iVar ) = fParametersPtr[ iVar ]; }
    for ( Int_t jVar = 0; jVar <= fNParameters; jVar++ ){
      if ( iVar * jVar == 0 ){ fCovarianceMatrixValues[ iVar ][ jVar ] = 0.0; }
      else{ fCovarianceMatrixValues[ iVar ][ jVar ] = fCovarianceMatrix[ iVar ][ jVar ]; }
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

TH1F* OCAModelParameterStore::GetPMTAngularResponseHistogram()
{

  // Returns a pointer to a histogram which contains the binned angular response
  // and error bars (from the diagonal matrix elements).

  // Get the number of bins in the PMT angular response distribution.
  Int_t nBins = GetNPMTAngularResponseBins();

  // Declare a hisotgram object.
  TH1F* hHisto  = new TH1F( "PMT Angular Response", "Relative PMT Angular Response", 
                            nBins, 0, 90 );

  // Loop over each of the parameters and set the error.
  for ( Int_t iBin = 0; iBin < nBins; iBin++ ) {
    hHisto->SetBinContent( iBin + 1.0, GetPMTAngularResponsePar( iBin ) );
    hHisto->SetBinError( iBin + 1.0, GetPMTAngularResponseError( iBin ) );
  }

  // Set the titles of the x and y axes.
  hHisto->GetXaxis()->SetTitle( "Incident PMT Angle #theta_{PMT} [degrees]" );
  hHisto->GetYaxis()->SetTitle( "Relative PMT Angular Response" );

  // Set a title offset of 120% the default.
  hHisto->GetXaxis()->SetTitleOffset( 1.2 );
  hHisto->GetYaxis()->SetTitleOffset( 1.2 );

  // Return the hisotgram.
  return hHisto;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAModelParameterStore::GetPMTAngularResponseError( const Int_t angPar )
{
  
  // Return the square root of the diagonal element in the
  // covariance matrix of the parameters corresponding to the PMT
  // response bin parameter 'angPar'.
 
  // Check that the request PMT angular response index is between the
  // number of parameters as specified for the PMT angular response in
  // the model.
  if ( angPar >= 0 
       && angPar < GetNPMTAngularResponseBins() ) {

    // Get the diagonal matrix cooresponding to the error^2 corresponding
    // to this parameter.
    Float_t coVar = fCovarianceMatrix[ GetPMTAngularResponseParIndex() + angPar ][ GetPMTAngularResponseParIndex() + angPar ];

    // If the element is positive, return the square root i.e. the error.
    if ( coVar >= 0.0 ){ return sqrt( coVar ); }

    // Otherwise print out a warning message and return an error of 0.
    else {
      printf( "OCAModelParameterStore::GetPMTAngularResponseError: Error! Requested error for the %i angular response parameter is 0.\n", angPar );
      return 0;
    }

  } 

  // If the requested parameter is out of range for the PMT angular
  // response then return an error message and a return value of 0.
  else {
    printf( "OCAModelParameterStore::GetPMTAngularResponseError: Error! Requested angular response parameter ( %i ) is out of range [ 0:%d ].\n", angPar, GetNPMTAngularResponseBins() - 1 );
    return 0;
  }

}

//////////////////////////////////////
//////////////////////////////////////

TF1* OCAModelParameterStore::GetPMTAngularResponseFunction()
{

  // Returns a pointer to a function with parameters equal to angular response
  // parameters from the optics fit. This can be used to plot the current 
  // PMT angular response function.

  // Create an array to the parameter values.
  Double_t* parVals = new Double_t[ 1 + GetNPMTAngularResponseBins() + 1 ];

  // Get the pointer to the PMT angular response parameters.
  Float_t* angResp = &fParametersPtr[ GetPMTAngularResponseParIndex() ];

  // Set the first value to the number of bins.
  parVals[ 0 ] = GetNPMTAngularResponseBins();

  // For each parameter set the corresponding entry in the array
  // pointer.
  for ( Int_t iPar = 0; iPar < GetNPMTAngularResponseBins(); iPar++ ){ 
    parVals[ 1 + iPar ] = angResp[ iPar ]; 
  }

  // Relic from old code for the PMT response type. Needs removing or fixing.
  parVals[ GetNPMTAngularResponseBins() + 1 ] = 2;

  // Create the function object.
  TF1* funcObj = new TF1( "PMT Angular Response Function", SPMTAngularResponse, 0, 90, 1 + GetNPMTAngularResponseBins() + 1 );

  // Set the parameters.
  funcObj->SetParameters( parVals );

  // Set the number of points in the function.
  funcObj->SetNpx( GetNPMTAngularResponseBins() );

  // Set the marker style for the function.
  funcObj->SetMarkerStyle( 20 );

  // Delete the parameter array.
  delete[] parVals;

  // Return the function.
  return funcObj;

}

//////////////////////////////////////
//////////////////////////////////////

Double_t OCAModelParameterStore::SPMTAngularResponse( Double_t* aPtr, Double_t* parPtr )
{

  // Function returns the PMT angular response corresponding to 
  // angle aPtr[ 0 ] in degrees, bounded by 0 and 90 degrees.
  // parPtr[ 0 ] specifies number of parameters; parPtr[ 1 ] through 
  // parPtr[ parPtr[ 0 ] ] are the parameters themselves.

  // Initialise the value of theta for this calculation.
  Float_t thetaVal = aPtr[ 0 ];

  // Check that the supplied value of theta is in range.
  // If it isn't, return 0.0.
  if ( thetaVal < 0.0 || thetaVal >= 90.0 ){
    return 0.0;  // out of range if not in [0:90]
  }
  
  // Get the number of parameters.
  Int_t nPars = (Int_t) parPtr[ 0 ];

  // Bins from 0 to 90 degrees.
  Int_t iPar = (Int_t) ( thetaVal * nPars / 90.0 );

  // Return 0.0 if the thetaVal is greater than 90 degrees
  if ( thetaVal >= ( 90.0 - 0.5 * 90.0 / nPars ) ){ 
    return 0.0;
  }
  
  // Get the parameter index for the parameter value.
  iPar = (Int_t) ( ( thetaVal - 0.5 * 90.0 / nPars ) * ( nPars / 90.0 ) );

  // Return the parameter value.
  return parPtr[ iPar + 1 ];

}

//////////////////////////////////////
//////////////////////////////////////

TH2F* OCAModelParameterStore::GetLBDistributionHistogram()
{

  // Get the number of cos-theta and phi bins
  // in the laserball isotropy distribution.
  Int_t nCThetaBins = 0;GetNLBDistributionCosThetaBins();
  Int_t nPhiBins = 0;GetNLBDistributionPhiBins();
  if ( fLBDistributionType == 0 ){ nCThetaBins = GetNLBDistributionCosThetaBins(); nPhiBins = GetNLBDistributionPhiBins(); }
  if ( fLBDistributionType == 1 ){ nCThetaBins = GetNLBSinWaveSlices(); nPhiBins = GetNLBParametersPerSinWaveSlice(); }
  
  // Decalre a new 2D histogram with 'Float_t' type entries.
  // Set the ranges as phi : ( 0, 2pi ) and
  // cos-theta : ( -1.0, 1.0 ).
  TH2F* lbDistributionHist;

  if ( fLBDistributionType == 0 ){
    lbDistributionHist = new TH2F( "Laserball Angular Distribution Histogram (Binned)", "Laserball Angular Distribution Histogram (Binned)",
                                   nPhiBins, -1 * TMath::Pi(), 1.0 * TMath::Pi(), 
                                   nCThetaBins, -1.0, 1.0 );

    // Get a pointer to the start of the laserball distribution
    // parameters in the parameter array.
    Float_t* lbDistPtr = &fParametersPtr[ GetLBDistributionParIndex() ];
    
    // Loop through each of the cos-theta and phi
    // bins and assign the corresponding parameter to its
    // bin entry in the histogram.
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t jPhi = 0; jPhi < nPhiBins; jPhi++ ){
        
        // Set the bin content to the parameter value for each bin.
        // The (+1) is because the 0-th bin in a ROOT histogram
        // is the underflow bin.
        lbDistributionHist->SetCellContent( jPhi + 1, iTheta + 1, 
                                            lbDistPtr[ iTheta * nPhiBins + jPhi ] );
      }
    }
  }

  if ( fLBDistributionType == 1 ){
    lbDistributionHist = new TH2F( "Laserball Angular Distribution Histogram (Sinusoidal)", "Laserball Angular Distribution Histogram (Sinusoidal)",
                                   36, -1 * TMath::Pi(), 1.0 * TMath::Pi(), 
                                   nCThetaBins, -1.0, 1.0 );

    // Get a pointer to the start of the laserball distribution
    // parameters in the parameter array.
    Float_t* lbDistPtr = &fParametersPtr[ GetLBDistributionParIndex() ];

    // Loop through each of the cos-theta and phi
    // bins and assign the corresponding parameter to its
    // bin entry in the histogram.
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t jPhi = 0; jPhi <= 35; jPhi++ ){
        Float_t phiVal = -1.0 * TMath::Pi() + jPhi * ( ( 2.0 * TMath::Pi() ) / 36 );
        // Set the bin content to the parameter value for each bin.
        // The (+1) is because the 0-th bin in a ROOT histogram
        // is the underflow bin.
        Float_t lbDistVal = 1.0 + lbDistPtr[ iTheta * 2 ] * TMath::Sin( 1.0 * phiVal + lbDistPtr[ iTheta * 2 + 1 ] ); 
        lbDistributionHist->SetCellContent( jPhi + 1, iTheta + 1, 
                                            lbDistVal );
      }
    }
  }

  // Set the titles of the x and y axes.
  lbDistributionHist->GetXaxis()->SetTitle( "Laserball #phi_{LB}" );
  lbDistributionHist->GetYaxis()->SetTitle( "Laserball Cos#theta_{LB}" );

  // Set the title offset to each respective axis as 120% that
  // of its original value.
  lbDistributionHist->GetXaxis()->SetTitleOffset( 1.2 );
  lbDistributionHist->GetYaxis()->SetTitleOffset( 1.2 );

  // Return the histogram.
  return lbDistributionHist;

}

//////////////////////////////////////
//////////////////////////////////////

TF1* OCAModelParameterStore::GetLBDistributionMaskFunction()
{

  // Get the number of mask parameters.
  Int_t nMaskPars = GetNLBDistributionMaskParameters();

  // Put the number into a string object for use in the
  // title later on in this method.
  stringstream tmpStream;
  string nParsStr = "";
  tmpStream << nMaskPars-1;
  tmpStream >> nParsStr;
  
  // Returns a pointer to a function with no parameters, to plot the current
  // laserball mask function polynomial in ( 1 + cos( theta_LB ) ).

  // Create a pointer to an array of the laserball mask parameters.
  Double_t* parVals = new Double_t[ 1 + nMaskPars ];

  // Set the first element in the array to the number of mask parameters.
  parVals[0] = nMaskPars;

  // Create a pointer to an array of the errors on the laserball mask
  // parameters.
  Double_t* errVals = new Double_t[ 1 + nMaskPars ];

  // Set the first element in the array to 0.0 to begin with.
  errVals[0] = 0.0;

  // Create a pointer to the laserball mask parameters.
  Float_t* maskParsPtr = &fParametersPtr[ GetLBDistributionMaskParIndex() ];

  // Loop over all the elements in the arrays and set the parameter
  // value to its corresponding mask parameter value and the
  // error value to the associated error value.
  for ( Int_t iPar = 0; iPar < nMaskPars; iPar++ ){ 
    parVals[ 1 + iPar ] = maskParsPtr[ iPar ]; 
    errVals[ 1 + iPar ] = GetLBDistributionMaskError( iPar ); 
  }

  // Create the function object.
  TF1 *lbMaskFunc = new TF1( "Laserball Distribution Mask", SLBDistributionMask, -1, 1, 1 + nMaskPars );

  // Set the parameter and error values in the function.
  lbMaskFunc->SetParameters( parVals );
  lbMaskFunc->SetParErrors( errVals );

  // Set the number of points and the marker style
  // for the plot.
  lbMaskFunc->SetNpx( 100 );
  lbMaskFunc->SetMarkerStyle( 20 );

  // Set the plot and axes titles.
  string plotTitle = "Laserball Mask Function P_{" + nParsStr + "}";
  lbMaskFunc->SetTitle( plotTitle.c_str() );
  lbMaskFunc->GetXaxis()->SetTitle( "Cos#theta_{LB}" );
  lbMaskFunc->GetYaxis()->SetTitle( "Relative Laserball Intensity" );

  // Set the title offset to 120% of the original.
  lbMaskFunc->GetXaxis()->SetTitleOffset( 1.2 );
  lbMaskFunc->GetYaxis()->SetTitleOffset( 1.2 );

  // Delete the parameter and error arrays before returning
  // the function.
  delete[] parVals; delete[] errVals;

  return lbMaskFunc;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAModelParameterStore::GetLBDistributionMaskError( const Int_t nVal )
{

  // Get the number of mask parameters.
  Int_t nMaskPars = GetNLBDistributionMaskParameters();
  
  // Return the square root of the diagonal element corresponding to the
  // laserball mask parameter 'nVal'.  
  if ( nVal >= 0 && nVal < nMaskPars ) {
    
    Float_t coVar = fCovarianceMatrix[ GetLBDistributionMaskParIndex() + nVal ][ GetLBDistributionMaskParIndex() + nVal ];

    // If the covariance element is greater than 0.0 then
    // return the square root (i.e. the error).
    if ( coVar >= 0.0 ){ return TMath::Sqrt( coVar ); }

    // Otherwise return an error
    else { printf("OCAModelParameterStore::GetLBDistributionMaskError: Error! Requested error for the %i laserball distribution mask parameter is 0.\n", nVal ); 
      return 0; 
    }

  } 

  // If the requested parameter is out of range for the laserball
  // distribution mask then return an error message and a return value of 0.
  else {
    printf("OCAModelParameterStore::GetLBDistributionMaskError: Error! Requested laserball distribution mask parameter ( %i ) is out of range [ 0:%d ].\n", nVal, nMaskPars - 1 );
    return 0;
  }

}

//////////////////////////////////////
//////////////////////////////////////

Float_t OCAModelParameterStore::SLBDistributionMask( Double_t* aPtr, Double_t* parPtr )
{

  // Compute the value of the laserball
  // mask from the parameter array.
  Int_t nPars = (Int_t) parPtr[ 0 ];

  Int_t iVal = 0;
  Float_t lbM = 0.0;
  Float_t onePlus = 1.0 + aPtr[ 0 ];

  // // The summation which forms the laserball
  // // mask function.
  // for ( iVal = nPars-1; iVal >= 0; iVal-- ){
  //   lbM = lbM * onePlus + parPtr[ 1 + iVal ];
  // }

  lbM = 1.0;
  for ( iVal = 1; iVal < nPars; iVal++ ){

    lbM += parPtr[ 1 + iVal ] * TMath::Power( onePlus, iVal );
  }

  return lbM;

}

//////////////////////////////////////
//////////////////////////////////////

TH2F* OCAModelParameterStore::GetLBDistributionIntensityHistogram()
{

  // Get the number of cos-theta and phi bins
  // in the laserball isotropy distribution.
  Int_t nCThetaBins = GetNLBDistributionCosThetaBins();
  Int_t nPhiBins = GetNLBDistributionPhiBins();
  if ( fLBDistributionType == 0 ){ nCThetaBins = GetNLBDistributionCosThetaBins(); nPhiBins = GetNLBDistributionPhiBins(); }
  if ( fLBDistributionType == 1 ){ nCThetaBins = GetNLBSinWaveSlices(); nPhiBins = GetNLBParametersPerSinWaveSlice(); }

  // Decalre a new 2D histogram with 'Float_t' type entries.
  // Set the ranges as phi : ( 0, 2pi ) and
  // cos-theta : ( -1.0, 1.0 ).
  TH2F* lbDistributionIntHist;

  if ( fLBDistributionType == 0 ){
    lbDistributionIntHist = new TH2F( "Laserball Intensity Distribution Histogram", "Laserball Intensity Distribution Histogram",
                                      nPhiBins, -1.0 * TMath::Pi(), TMath::Pi(), 
                                      nCThetaBins, -1.0, 1.0 );
    
    // Get a pointer to the start of the laserball distribution
    // parameters in the parameter array.
    Float_t* lbDistPtr = &fParametersPtr[ GetLBDistributionParIndex() ];
    
    // Get the bin width for the cos-theta part of the distribution.
    // This will be used to calculate the corresponding
    // mask multipier for each cos-theta bin.
    Float_t binWidth = 2.0 / nCThetaBins;
    //Float_t binWidthPhi = ( 2.0 * TMath::Pi() ) / ( nPhiBins );
    
    // Loop over all the bins in the distribution and set the
    // corresponding bin entry in the histogram accordingly.
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t jPhi = 0; jPhi < nPhiBins; jPhi++ ){
        
        // The value of cos-theta.
        Float_t cTheta = -1.0 + ( binWidth * iTheta );
        //Float_t phi = -1.0 * TMath::Pi() + ( binWidthPhi * jPhi );
        
        // Initialise the mask value to 1.0 to begin with.
        Float_t polynomialVal = 0.0;
        Float_t onePlus = 1.0 + cTheta;
        // Loop through all the laserball mask parameters
        // performing the summation of different degree terms
        // The degree will run from 1 to NLBDistributionMaskParameters.
        for ( Int_t iPar = GetNLBDistributionMaskParameters() - 1; 
              iPar >= 0; 
              iPar-- ){
          
          polynomialVal = polynomialVal * onePlus + GetLBDistributionMaskPar( iPar );
          
        }
        
        // Set the hisotgram entry, note the 'polynomialVal' multiplier
        // to apply the intensity correction from the laserball mask
        // parameters to the laserball isotropy distribution.
        lbDistributionIntHist->SetCellContent( jPhi + 1, iTheta + 1, 
                                               polynomialVal * lbDistPtr[ iTheta * nPhiBins + jPhi ] );
      }
    } 
  }
  if ( fLBDistributionType == 1 ){
    lbDistributionIntHist = new TH2F( "lbDistributionIntHist", "Laserball Intensity Distribution Histogram",
                                      36, -1 * TMath::Pi(), 1.0 * TMath::Pi(), 
                                      nCThetaBins, -1.0, 1.0 );
    
    // Get a pointer to the start of the laserball distribution
    // parameters in the parameter array.
    Float_t* lbDistPtr = &fParametersPtr[ GetLBDistributionParIndex() ];

    Float_t binWidth = 2.0 / nCThetaBins;
    
    // Loop through each of the cos-theta and phi
    // bins and assign the corresponding parameter to its
    // bin entry in the histogram.
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t jPhi = 0; jPhi <= 35; jPhi++ ){
        Float_t phiVal = -1.0 * TMath::Pi() + jPhi * ( ( 2.0 * TMath::Pi() ) / 36 );
        
        // The value of cos-theta.
        Float_t cTheta = -1.0 + ( binWidth * iTheta );
        //Float_t phi = -1.0 * TMath::Pi() + ( binWidthPhi * jPhi );
        
        // Initialise the mask value to 1.0 to begin with.
        Float_t polynomialVal = 0.0;
        Float_t onePlus = 1.0 + cTheta;
        // Loop through all the laserball mask parameters
        // performing the summation of different degree terms
        // The degree will run from 1 to NLBDistributionMaskParameters.
        for ( Int_t iPar = GetNLBDistributionMaskParameters() - 1; 
              iPar >= 0; 
              iPar-- ){
          
          polynomialVal = polynomialVal * onePlus + GetLBDistributionMaskPar( iPar );
          
        }
        // Set the bin content to the parameter value for each bin.
        // The (+1) is because the 0-th bin in a ROOT histogram
        // is the underflow bin.
        Float_t lbDistVal = 1.0 +  lbDistPtr[ iTheta * 2 ] * TMath::Sin( 1.0 * phiVal + lbDistPtr[ iTheta * 2 + 1 ] ); 
        lbDistributionIntHist->SetCellContent( jPhi + 1, iTheta + 1, 
                                               lbDistVal * polynomialVal );
      }
    }
  }

  // Set the titles of the x and y axes.
  lbDistributionIntHist->GetXaxis()->SetTitle( "#phi_{LB}" );  
  lbDistributionIntHist->GetYaxis()->SetTitle( "Cos#theta_{LB}" );

  // Set the title offset to each respective axis as 120% that
  // of its original value.
  lbDistributionIntHist->GetXaxis()->SetTitleOffset( 1.2 );
  lbDistributionIntHist->GetYaxis()->SetTitleOffset( 1.2 );
  
  // Return the histogram.
  return lbDistributionIntHist;

}
