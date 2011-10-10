//
// NCDConstants.h
//
// Author : Adam Cox
//
// Description: the constants needed to program with the Ncd data stream 
//
// Modified Gersende Prior 02/13/06

#ifndef NCDConstants_H
#define NCDConstants_H

#include "Rtypes.h"

//hardware constants
const Short_t kNumShaperChans = 8;
const Short_t kNumMuxChans = 12; 
const Short_t kNumMuxBox = 8;
const Short_t kNumScopeChans = 4;
const Short_t kNumScopes = 2;
const Short_t kNumShapers = 12;
const Int_t kDefaultWaveformSize = 15000;
const Short_t kNumNCDStrings = 48;   //there are 48 possible electronics channels 
const Short_t kMaxNCDChannel = 47;  //1 - kNumNCDStrings
const Short_t kNumADCBins = 2048;
const Short_t kMaxADCBin = 2047; //1 - kNumADCBins

//log amp calibration fitting routing constants
const Short_t kNumNCDFitParams = 35;  //not all of these parameters are actually fit... some are static numbers
//const Int_t kMaxChiSquared = 10000;
const Int_t kMaxChiSquaredPerNDF = 10000;  //turns out the chi^2 doesn't really matter for our logamp fits  
const Short_t kFitStartPosDifference = 500;
const Short_t kFitStartPosRange = 50;
const Double_t kDefaultFitRangeMinimum = 0;
const Double_t kDefaultFitRangeMaximum = kDefaultWaveformSize;
const Float_t kMaxUncerFactor = .5;  
const Float_t kNCDLogAmpCalibrationLevelCrossingFactor = .55;    
const Short_t kNCDLogAmpCalibrationDefaultNumCrossings = 4;
const Int_t kDefaultCalibStepSize = 10;
const Char_t kNCDLogAmpCalibrationBankName[] = "NCLA";
//const Int_t kNumOutputNCDLogAmpCalibParameters = 9;  //5 + 4 spares  //not really used
const Char_t kNCDLogAmpCalibName[] = "NCDlogAmpfit";
const Double_t kBadCalibrationValue = -9999.;
const Float_t kScopeBinError = 1;  //what is the correct number for this?... it seems like 3 is too big since chi^2/DOF sometimes drops below 1 when the error is 3.
const Int_t kDefaultCalibWidth1 = 320;
const Int_t kDefaultCalibWidth2 = 640;

//current default NCLA version numbers
//see the NCLA bank definition to ensure that the output bank matches the definition
const Short_t kNCDLogAmpBankNumber = 2;  //the NCLA bank number
const Short_t kNCDLogAmpNCLAVersion = 2; //NCLA_VERSION

// Move constants used for bank writing here (B.Jamieson 1/29/2006)
// dimension according to number of versions, since values can change with version
const Int_t kSizeOfLogAmpHeader[2]   = {  20,   20};
const Int_t kNumberWordsPerRecord[2] = {  37,  117};
const Int_t kNumGlobalSpareWords[2]  = {  16,   16};
const Int_t kNumNclaBankParams[2]    = {   5,   11};

// GEDP 111 constants
const Short_t kNumNCDGeomVar = 4;

#endif








