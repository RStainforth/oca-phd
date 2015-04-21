#include "LOCASModelParameterStore.hh"
#include "LOCASModelParameter.hh"
#include "LOCASDataStore.hh"

#include "LOCASDB.hh"
#include "LOCASMath.hh"

#include "TFile.h"
#include "TTree.h"
#include "TClass.h"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace LOCAS;

ClassImp( LOCASModelParameterStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASModelParameterStore::LOCASModelParameterStore( string storeName )
{

  // Set the store name.
  fStoreName = storeName;

  // Ensure the vector which will hold all the parameter objects
  // is empty to begin with.
  fParameters.clear();

  // Ensure all the pointers are initialised to 'NULL'
  fParametersPtr = NULL;
  fParametersVary = NULL;
  fCovarianceMatrix = NULL;
  fDerivativeMatrix = NULL;
  fVariableParameterIndex = NULL;
  fVariableParameterMap = NULL;
  fPMTAngularResponseIndex = NULL;

  // Set the 'Int_t' type variables to non-interpretive values,
  // i.e. -1.
  fCurrentLBDistributionBin = -1;
  fCentralCurrentLBDistributionBin = -1;
  fCurrentPMTAngularResponseBin = -1;
  fCentralCurrentPMTAngularResponseBin = -1;
  fCurrentLBRunNormalisationBin = -1;
  
  fNLBDistributionMaskParameters = -1;
  fNPMTAngularResponseBins = -1;
  fNLBDistributionCosThetaBins = -1;
  fNLBDistributionPhiBins = -1;
  fNLBRunNormalisations = -1;

  fNParameters = -1;
  fNCurrentVariableParameters = -1;
  fNBaseVariableParameters = -1;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::PrintParameterInfo()
{

  // For each parameter currently held in the store, print
  // out all the information for that parameter.
  vector< LOCASModelParameter >::iterator iPar;

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

  // Create a LOCASDB object so that the 'fit-file' can be read.
  LOCASDB lDB;

  // Tell the LOCASDB object where to read the information from.
  lDB.SetFile( fileName );

  // Initialise the variables which will be defined
  // for each parameter which is to be created in the
  // loops which follow.
  Float_t maxVal = 0.0;
  Float_t minVal = 0.0;
  Float_t initVal = 0.0;
  Float_t incVal = 0.0;
  Int_t nParsInGroup = 0;
  Bool_t varyBool = false;

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
  //            * Number of bins in Phi between 0.0 and 360.0 'fNLBDistributionPhiBins'

  // Loop over each parameter set in the card file and add them/it to the store.
  for ( Int_t iStr = 0; iStr < (Int_t)paramList.size(); iStr++ ){

    // Laserball distribution mask.
    if ( paramList[ iStr ] == "laserball_intensity_mask" ){
      fNLBDistributionMaskParameters = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_parameters" ), "parameter_setup" );
    }

    // PMT angular response.
    else if ( paramList[ iStr ] == "pmt_angular_response" ){
      fNPMTAngularResponseBins = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_bins" ), "parameter_setup" );
    }

    // Laserball distribution hisotgram.
    else if ( paramList[ iStr ] == "laserball_distribution_histogram" ){
      fNLBDistributionCosThetaBins = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_cos_theta_bins" ), "parameter_setup" );
      fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_number_of_phi_bins" ), "parameter_setup" );
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
      else{ cout << "LOCASModelParameterStore::LOCASModelParameterStore: Error, unknown parameter passed" << endl; }

      LOCASModelParameter lParameter( (string)( paramList[ iStr ] ), parIndex, initVal, 
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
        LOCASModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
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
        if ( angleVal < 36.0 ){ initVal = 1.0 + ( 0.002222 * angleVal ); }
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
        LOCASModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, nParsInGroup, parVary );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "laserball_distribution_histogram" ){

      nParsInGroup = fNLBDistributionPhiBins * fNLBDistributionCosThetaBins;

      varyBool = lDB.GetIntField( "FITFILE", (string)( paramList[ iStr ] + "_vary" ), "parameter_setup" );

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

        Int_t parIndex = 3 + 
          fNLBDistributionMaskParameters + 
          fNPMTAngularResponseBins + 
          iPar;
        LOCASModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, nParsInGroup, varyBool );
        AddParameter( lParameter );
        
        lStream.clear();
      }

    }

    if ( paramList[ iStr ] == "laserball_run_normalisation" ){

      nParsInGroup = fNLBRunNormalisations;

      vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
      initVal = lDB.GetDoubleField( "FITFILE", (string)( paramList[ iStr ] + "_initial_value" ), "parameter_setup" );
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
          ( fNLBDistributionPhiBins * fNLBDistributionCosThetaBins ) 
          + iPar;
        LOCASModelParameter lParameter( (string)( paramList[ iStr ] + parStr ), parIndex, initVal, 
                                        minVal, maxVal, incVal, nParsInGroup, varyBool );
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

void LOCASModelParameterStore::WriteToROOTFile( const char* fileName )
{
  
  TFile* file = TFile::Open( fileName, "RECREATE" );

  // Create the parameter Tree
  TTree* parTree = new TTree( fStoreName.c_str(), fStoreName.c_str() );

  TH2F* lbDistribution = GetLBDistributionHistogram();
  file->WriteTObject( lbDistribution );

  TH1F* angularResponse = GetPMTAngularResponseHistogram();
  file->WriteTObject( angularResponse );

  TF1* angularResponseTF1 = GetPMTAngularResponseFunction();
  file->WriteTObject( angularResponseTF1 );

  TF1* lbDistributionTF1 = GetLBDistributionMaskFunction();
  file->WriteTObject( lbDistributionTF1 );

  // Declare a new branch pointing to the parameter store
  parTree->Branch( "LOCASModelParameterStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  parTree->Fill();
  parTree->Write();

  file->Close();
  delete file;

  cout << "LOCAS::LOCASModelParameterStore: Fitted parameters saved to ROOT file:\n";
  cout << fileName << "\n";

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::WriteToRATDBFile( const char* fileName )
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

  roccVals << "\n";

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

  cout << "LOCAS::LOCASModelParameterStore: Fitted parameters saved to RATDB file:\n";
  cout << fileName << "\n";

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::AllocateParameterArrays()
{

  // Allocate memory for the pointer which will hold the parameter values.
  fParametersPtr = LOCASMath::LOCASVector( 1, fNParameters );

  // Allocate memory for the pointer which will hold the varying map
  // for each of the parameters (1: Vary, 0: Fixed)
  fParametersVary = LOCASMath::LOCASIntVector( 1, fNParameters );

  // Allocate the memory for the covariance and derivative matrices.
  fCovarianceMatrix = LOCASMath::LOCASMatrix( 1, fNParameters, 1, fNParameters );
  fDerivativeMatrix = LOCASMath::LOCASMatrix( 1, fNParameters, 1, fNParameters );

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
  // parameter pointer array from the vector of LOCASModelParameter objects.
  // i.e. LOCASModelParameter --> Pointer Array (fParametersPtr).
  // And also assign whether or not the parameter will vary.
  // i.e. LOCASModelParameter --> Pointer Array (fParametersVary);
  vector< LOCASModelParameter >::iterator iPar;
  for ( iPar = GetLOCASModelParametersIterBegin();
        iPar != GetLOCASModelParametersIterEnd();
        iPar++ ){

    // Set the initial value of the parameter in the array.
    fParametersPtr[ iPar->GetIndex() ] = iPar->GetInitialValue();

    // Set whether or not the parameter is required to vary (0: Fixed, 1: Vary).
    if ( iPar->GetVary() ){ fParametersVary[ iPar->GetIndex() ] = 1; }
    else{ fParametersVary[ iPar->GetIndex() ] = 0; }

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::InitialisePMTAngularResponseIndex()
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

void LOCASModelParameterStore::IdentifyVaryingParameters()
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
  
  else{
    parnum = GetLBRunNormalisationParIndex() + fCurrentLBRunNormalisationBin;
    if ( fParametersVary[ parnum ] ){ 
      fVariableParameterIndex[ ++fNCurrentVariableParameters ] = parnum; 
    }
  }
  return;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::IdentifyBaseVaryingParameters()
{

  if ( fVariableParameterIndex != NULL ) { delete[] fVariableParameterIndex; }
  // The number of unique parameters is guaranteed to be less than 
  // the number of total parameters.
  fVariableParameterIndex = new Int_t[ fNParameters ];

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

void LOCASModelParameterStore::CrossCheckParameters()
{

  // Now put each of the initial parameter values into the
  // parameter pointer array from the vector of LOCASModelParameter objects.
  // i.e. LOCASModelParameter --> Pointer Array (fParametersPtr).
  // And also assign whether or not the parameter will vary.
  // i.e. LOCASModelParameter --> Pointer Array (fParametersVary);
  vector< LOCASModelParameter >::iterator iPar;
  for ( iPar = GetLOCASModelParametersIterBegin();
        iPar != GetLOCASModelParametersIterEnd();
        iPar++ ){

    iPar->SetFinalValue( fParametersPtr[ iPar->GetIndex() ] );
    iPar->SetVary( fParametersVary[ iPar->GetIndex() ] );

  }  

}

//////////////////////////////////////
//////////////////////////////////////

TH1F* LOCASModelParameterStore::GetPMTAngularResponseHistogram()
{

  // Returns a pointer to a histogram which contains the binned angular response
  // and error bars (from the diagonal matrix elements).

  // Get the number of bins in the PMT angular response distribution.
  Int_t nBins = GetNPMTAngularResponseBins();

  // Declare a hisotgram object.
  TH1F* hHisto  = new TH1F( "AngRespH", "Relative PMT Angular Response", 
                            nBins, 0, 90 );

  // Loop over each of the parameters and set the error.
  for ( Int_t iBin = 0; iBin < nBins; iBin++ ) {
    hHisto->SetBinContent( iBin + 1.0, GetPMTAngularResponsePar( iBin ) );
    hHisto->SetBinError( iBin + 1.0, GetPMTAngularResponseError( iBin ) );
  }

  // Set the titles of the x and y axes.
  hHisto->GetXaxis()->SetTitle( "Incident PMT Angle (#theta_{PMT})[degrees]" );
  hHisto->GetYaxis()->SetTitle( "Relative PMT Angular Response" );

  // Set a title offset of 120% the default.
  hHisto->GetXaxis()->SetTitleOffset( 1.2 );
  hHisto->GetYaxis()->SetTitleOffset( 1.2 );

  // Return the hisotgram.
  return hHisto;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModelParameterStore::GetPMTAngularResponseError( const Int_t angPar )
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
      printf( "LOCASModelParameterStore::GetPMTAngularResponseError: Error! Requested error for the %i angular response parameter is 0.\n", angPar );
      return 0;
    }

  } 

  // If the requested parameter is out of range for the PMT angular
  // response then return an error message and a return value of 0.
  else {
    printf( "LOCASModelParameterStore::GetPMTAngularResponseError: Error! Requested angular response parameter ( %i ) is out of range [ 0:%d ].\n", angPar, GetNPMTAngularResponseBins() - 1 );
    return 0;
  }

}

//////////////////////////////////////
//////////////////////////////////////

TF1* LOCASModelParameterStore::GetPMTAngularResponseFunction()
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
  TF1* funcObj = new TF1( "Angular Response", SPMTAngularResponse, 0, 90, 1 + GetNPMTAngularResponseBins() + 1 );

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

Double_t LOCASModelParameterStore::SPMTAngularResponse( Double_t* aPtr, Double_t* parPtr )
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

TH2F* LOCASModelParameterStore::GetLBDistributionHistogram()
{

  // Get the number of cos-theta and phi bins
  // in the laserball isotropy distribution.
  Int_t nCThetaBins = GetNLBDistributionCosThetaBins();
  Int_t nPhiBins = GetNLBDistributionPhiBins();

  // Decalre a new 2D histogram with 'Float_t' type entries.
  // Set the ranges as phi : ( 0, 2pi ) and
  // cos-theta : ( -1.0, 1.0 ).
  TH2F* lbDistributionHist = new TH2F( "lbDistributionHist", "Laserball Distribution Histogram",
                                       nPhiBins, 0.0, 2.0 * TMath::Pi(), 
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

  // Set the titles of the x and y axes.
  lbDistributionHist->GetXaxis()->SetTitle( "Laserball Phi_{LB}" );
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

TF1* LOCASModelParameterStore::GetLBDistributionMaskFunction()
{

  // Get the number of mask parameters.
  Int_t nMaskPars = GetNLBDistributionMaskParameters();

  // Put the number into a string object for use in the
  // title later on in this method.
  stringstream tmpStream;
  string nParsStr = "";
  tmpStream << nMaskPars;
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
  lbMaskFunc->GetXaxis()->SetTitle( "Polar Angle Cos#theta_{LB}" );
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

Float_t LOCASModelParameterStore::GetLBDistributionMaskError( const Int_t nVal )
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
    else { printf("LOCASModelParameterStore::GetLBDistributionMaskError: Error! Requested error for the %i laserball distribution mask parameter is 0.\n", nVal ); 
      return 0; 
    }

  } 

  // If the requested parameter is out of range for the laserball
  // distribution mask then return an error message and a return value of 0.
  else {
    printf("LOCASModelParameterStore::GetLBDistributionMaskError: Error! Requested laserball distribution mask parameter ( %i ) is out of range [ 0:%d ].\n", nVal, nMaskPars - 1 );
    return 0;
  }

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModelParameterStore::SLBDistributionMask( Double_t* aPtr, Double_t* parPtr )
{

  // Compute the value of the laserball
  // mask from the parameter array.
  Int_t nPars = (Int_t) parPtr[ 0 ];

  Int_t iVal = 0;
  Float_t lbM = 0.0;
  Float_t onePlus = 1.0 + aPtr[ 0 ];

  // The summation which forms the laserball
  // mask function.
  for ( iVal = nPars-1; iVal >= 0; iVal-- ){
    lbM = lbM * onePlus + parPtr[ 1 + iVal ];
  }

  return lbM;

}

//////////////////////////////////////
//////////////////////////////////////

TH2F* LOCASModelParameterStore::GetLBDistributionIntensityHistogram()
{

  // Get the number of cos-theta and phi bins
  // in the laserball isotropy distribution.
  Int_t nCThetaBins = GetNLBDistributionCosThetaBins();
  Int_t nPhiBins = GetNLBDistributionPhiBins();

  // Decalre a new 2D histogram with 'Float_t' type entries.
  // Set the ranges as phi : ( 0, 2pi ) and
  // cos-theta : ( -1.0, 1.0 ).
  TH2F* lbDistributionIntHist = new TH2F( "lbDistributionHist", "Laserball Distribution Histogram",
                                       nPhiBins, 0.0, 2.0 * TMath::Pi(), 
                                       nCThetaBins, -1.0, 1.0 );

  // Get a pointer to the start of the laserball distribution
  // parameters in the parameter array.
  Float_t* lbDistPtr = &fParametersPtr[ GetLBDistributionParIndex() ];

  // Get the bin width for the cos-theta part of the distribution.
  // This will be used to calculate the corresponding
  // mask multipier for each cos-theta bin.
  Float_t binWidth = 2.0 / nCThetaBins;

  // Loop over all the bins in the distribution and set the
  // corresponding bin entry in the histogram accordingly.
  for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
    for ( Int_t jPhi = 0; jPhi < nPhiBins; jPhi++ ){

      // The value of cos-theta.
      Float_t cTheta = binWidth * iTheta;
      // Initialise the mask value to 1.0 to begin with.
      Float_t polynomialVal = 1.0;
      
      // Loop through all the laserball mask parameters
      // performing the summation of different degree terms
      // The degree will run from 1 to NLBDistributionMaskParameters.
      for ( Int_t iPar = 0; 
            iPar < GetNLBDistributionMaskParameters(); 
            iPar++ ){
        
        polynomialVal += ( ( GetLBDistributionMaskPar( iPar ) )
                           * ( TMath::Power( ( 1 + TMath::Cos( cTheta ) ), 
                                             ( iPar + 1 ) ) ) );
        
      }

      // Set the hisotgram entry, note the 'polynomialVal' multiplier
      // to apply the intensity correction from the laserball mask
      // parameters to the laserball isotropy distribution.
      lbDistributionIntHist->SetCellContent( jPhi + 1, iTheta + 1, 
                                          polynomialVal * lbDistPtr[ iTheta * nPhiBins + jPhi ] );
    }
  }

  // Set the titles of the x and y axes.
  lbDistributionIntHist->GetXaxis()->SetTitle( "Laserball Phi_{LB}" );  
  lbDistributionIntHist->GetYaxis()->SetTitle( "Laserball Cos#theta_{LB}" );

  // Set the title offset to each respective axis as 120% that
  // of its original value.
  lbDistributionIntHist->GetXaxis()->SetTitleOffset( 1.2 );
  lbDistributionIntHist->GetYaxis()->SetTitleOffset( 1.2 );
  
  // Return the histogram.
  return lbDistributionIntHist;

}
