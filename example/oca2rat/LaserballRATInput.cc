////////////////////////////////////////////////////////////////////
///
/// FILENAME: LaserballRATInput.cc
///
/// CLASS:  N/A
///
/// BRIEF:  The main purpose of this macro is to create tables in the
///         RATDB format of the laserball parameters from OCA.
///          
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     12/2016 : Ana Inacio - First Revision, new file.
///
/// DETAIL: To use this macro, start ROOT in the terminal by
///         typing 'root'. Once ROOT has started type the following
///         to compile the macro:
///
///         .L LaserballRATInput.cc+
///
///         Once the file has compiled, the tables are created by
///         typing OutputLBTable("<file_name>"):
///
///         ex.: OutputLBTable("oct15_watersin_337")
/// 
////////////////////////////////////////////////////////////////////

#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"

#include "OCAPMTStore.hh"
#include "OCAModelParameterStore.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace OCA;

// Functions in this macro
void GetLBLightDistribution( TFile* dataFile,
                             std::string dataSet,
														 Int_t& type,
                             vector< Float_t >& lbPars );
void GetLBMaskParameters( TFile* dataFile,
                          std::string dataSet,
                          vector< Float_t >& lbPars );
void OutputLBTable( std::string fitName );

//_____________________________________________________________
//

void OutputLBTable( std::string fitName ){

  // First retrieve the full file path of the file.
  OCADB lDB;
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/" + fitName;
  cout << "Opening File: " << filePath + ".root" << endl;

  // Open the .root file.
  TFile* tmpFile = TFile::Open( ( filePath + ".root" ).c_str() );

  // Get Laserball parameters (angular distribution and mask)
  vector< Float_t > maskVal;
  vector< Float_t > distVal;
	Int_t lbType;
	
	GetLBLightDistribution( tmpFile, fitName, lbType, distVal );
	GetLBMaskParameters( tmpFile, fitName, maskVal );
	
  ofstream roccVals;
  roccVals.precision( 6 );
  roccVals.open ( "laserballRAT.txt", std::ios_base::app );
	
	string dataSet = fitName;
	dataSet.erase (5,13);
	std::transform(dataSet.begin(), dataSet.end(),dataSet.begin(), ::toupper);
	string wl = fitName;
	wl.erase (0,15);
	
	cout << dataSet << "    " << wl << endl;
	
	string index;
	
	if(wl == "337"){ index = "N2"; }
	if(wl == "369"){ index = "PBD"; }
	if(wl == "385"){ index = "BBQ"; }
	if(wl == "420"){ index = "BIS-MSB"; }
	if(wl == "505"){ index = "COUMARIN-500"; }
	
	// Writing the Laserball values in the LASERBALL.ratdb format
	
	roccVals << "{\n";
	roccVals << "type: \"LBDYEANGLE\",\n";
	roccVals << "version: 1,\n";
	roccVals << "index: \"" << dataSet << ":" << index << "\",\n";
	roccVals << "run_range: [,],\n";
	roccVals << "pass: 0,\n";
	roccVals << "timestamp: \"\",\n";
	roccVals << "comment: \"\",\n";
	
	roccVals << "\n";
	
	roccVals << "// Coefficients for laserball mask (theta) order 0 is first, 4 is last\n";
	Int_t nMaskPars = maskVal.size();
  roccVals << "lb_mask_coef: [";
  for ( Int_t iPar = 0; iPar < nMaskPars; iPar++ ){
    if ( iPar == nMaskPars - 1 ){
      roccVals << maskVal[ iPar ] << "],\n";
    }
    else{
      roccVals << maskVal[ iPar ] << ", ";
		}
  }

	roccVals << "\n";
	
  if ( lbType == 0 ){
    Int_t nCThetaBins = 12;
    Int_t nPhiBins = 36;
		
		roccVals << "lb_ang_intensity: [\n";
//		roccVals << "//         ********************************************\n";
//		roccVals << "//         ******** Values at Wavelength (...) ********\n";
//		roccVals << "//         ********************************************\n";
		roccVals << "// Laserball distribution grid 10 deg bins in phi\n";
    for ( Int_t iTheta = 0; iTheta < nCThetaBins; iTheta++ ){
      for ( Int_t iPhi = 0; iPhi < nPhiBins; iPhi++ ){
        if ( iTheta == nCThetaBins - 1
             && iPhi == nPhiBins - 1 ){
          roccVals << distVal[ iTheta*nPhiBins + iPhi ] << " ],\n";
        }
        else{
          roccVals << distVal[ iTheta*nPhiBins + iPhi ] << ", ";
        }
      }
      roccVals << "\n";
    }
  }
  
  if ( lbType == 1 ){
	 
	  roccVals << "// Coefficients for the sinusoidal laserball mask distribution.\n";
	  roccVals << "// 24 theta bins in total with two parameters (amplitude & phase) per theta bin.\n";
	 
    Int_t nThetaSlices = 24;
    Int_t nParsPerSlice = 2;
    roccVals << "lb_ang_sincoef: [";
    for ( Int_t iPar = 1; iPar <= ( nThetaSlices * nParsPerSlice ); iPar++ ){

      if ( iPar % 2 == 0 && iPar == ( nThetaSlices * nParsPerSlice ) ){
        roccVals << distVal[ iPar-1 ] << " ],\n";
      }
      if ( iPar % 8 == 0 && iPar < ( nThetaSlices * nParsPerSlice ) && iPar != 0){
        roccVals << distVal[ iPar-1 ] << ",\n";
      }
      else if ( iPar % 2 == 1 && iPar < ( nThetaSlices * nParsPerSlice )){
        roccVals << distVal[ iPar-1 ] << ", ";
      }
      else if ( iPar % 2 == 0  && iPar < ( nThetaSlices * nParsPerSlice )){
        roccVals << distVal[ iPar-1 ] << ",      ";
      }     
    }
    roccVals << "\n";
  }
  
  roccVals << "\n}";

  roccVals << "\n";
  roccVals.close();

  cout << "OutputLBTable: Laserball parameters saved to file:\n";
  cout << "laserballRAT.txt" << "\n";
	
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

void GetLBLightDistribution( TFile* dataFile,
                             std::string dataSet,
														 Int_t& type,
                             vector< Float_t >& lbPars )
{

  string branch = "nominal";
	
	OCA::OCAModelParameterStore* tmpStore = NULL;
  TTree* tmpTree = NULL;
	string treeName = ( dataSet + "-" + branch + ";1" );
  tmpTree = (TTree*)dataFile->Get( treeName.c_str() );
  string valStr = "tmp";
  tmpStore = new OCA::OCAModelParameterStore( valStr );
	
	tmpTree->SetBranchAddress( branch.c_str(), &(tmpStore) );
  tmpTree->GetEntry( 0 );
	
	TVector parVals = tmpStore->GetParameterValues();
	Int_t lbDistIndex = tmpStore->GetLBDistributionParIndex();
	type = tmpStore->GetLBDistributionType();
  for ( Int_t iAng = 0; iAng < tmpStore->GetNLBDistributionPars(); iAng++ ){

    // The current index of the iAng-th angular response parameter.
    Int_t iIndex = lbDistIndex + iAng;
    lbPars.push_back( parVals[ iIndex ] );
  }
  
  tmpTree = NULL;
  delete tmpStore;
  tmpStore = NULL;
 
}
                             
////////////////////////////////////////////////////
////////////////////////////////////////////////////                            
                             
void GetLBMaskParameters( TFile* dataFile,
                          std::string dataSet,
                          vector< Float_t >& lbPars )
{

  string branch = "nominal";
	
	OCA::OCAModelParameterStore* tmpStore = NULL;
  TTree* tmpTree = NULL;
	string treeName = ( dataSet + "-" + branch + ";1" );
  tmpTree = (TTree*)dataFile->Get( treeName.c_str() );
  string valStr = "tmp";
  tmpStore = new OCA::OCAModelParameterStore( valStr );
	
	tmpTree->SetBranchAddress( branch.c_str(), &(tmpStore) );
  tmpTree->GetEntry( 0 );
	
	TVector parVals = tmpStore->GetParameterValues();
	Int_t lbMaskIndex = tmpStore->GetLBDistributionMaskParIndex();
  for ( Int_t iAng = 0; iAng < tmpStore->GetNLBDistributionMaskParameters(); iAng++ ){

    // The current index of the iAng-th angular response parameter.
    Int_t iIndex = lbMaskIndex + iAng;
    lbPars.push_back( parVals[ iIndex ] );
  }
  
  tmpTree = NULL;
  delete tmpStore;
  tmpStore = NULL;
 
}