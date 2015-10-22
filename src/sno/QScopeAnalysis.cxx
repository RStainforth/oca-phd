
//
// QScopeAnalysis
// Author: Laura Stonehill and Adam Cox

//
//-------------------------------------------------------------------
// This class is used for simple analysis of scope waveforms.  There are various
// options within this class, most of which are turned off by default and are
// considered to be the accepted method for pulse shape analysis.
// 

#include "QMuxScope.h"
#include "stdio.h"
#include "NCDConstants.h"
#include "QScopeAnalysis.h"
#include "TF1.h"
#include "Riostream.h"

ClassImp(QScopeAnalysis);

void QScopeAnalysis::Clear(Option_t *anOption)
{
}


QScopeAnalysis::QScopeAnalysis()
{
  fCurrentWaveformSize = (int)kDefaultWaveformSize;
  SetLogAmpParameters(0.,0.,0.,0.,0.,0.);
  SetDelogging(1);
  SetDePreamp(0);
  SetRmPostBaseline(0);
  SetScopeTriggerPosition(0.10);
  SetIsZeroLevelFilter(0);
  SetRiseTimePercent(0);
  SetBaselineRemovalPercent(0);
  SetUseMeasuredScopeOffset(0);
  ResetCalculatedValues();
}


QScopeAnalysis::QScopeAnalysis(TH1C *aPulse,Bool_t deLog,Bool_t isZLF,Float_t aRTPercent,Float_t aBLRPercent,Bool_t dePreamp, Bool_t rmPostBaseline, Bool_t aMessages)
{
  SetMessages(aMessages);

  if(aPulse != NULL) fCurrentWaveformSize = aPulse->GetNbinsX();
  else fCurrentWaveformSize = (int)kDefaultWaveformSize;
  SetNewScopeTrace(aPulse);
  SetLogAmpParameters(0.,0.,0.,0.,0.,0.);
  SetScopeTriggerPosition(0.10);  //this should be determined from the NQxx banks
  SetDelogging(deLog);
  SetDePreamp(dePreamp);
  SetRmPostBaseline(rmPostBaseline);
  SetIsZeroLevelFilter(isZLF);
  SetRiseTimePercent(aRTPercent);
  SetBaselineRemovalPercent(aBLRPercent);
  SetUseMeasuredScopeOffset(0);
  ResetCalculatedValues();

  //As you'll notice in the .h file, I've turned off the zero level filter because it doesn't always work right.  oscillatory noise event should integrate to very small amplitudes.  also, during commissioning, we shouldn't be cutting out any wavforms yet.
}

QScopeAnalysis::QScopeAnalysis(TH1C *aPulse,Float_t a, Float_t b, Float_t logoffset, Float_t chargeFactor,
			       Float_t time, Float_t scopeoffset,Bool_t deLog,Bool_t isZLF,Float_t aRTPercent,Float_t aBLRPercent,Bool_t dePreamp, Bool_t rmPostBaseline, Bool_t aMessages)
{
  SetMessages(aMessages);

  if(aPulse != NULL) fCurrentWaveformSize = aPulse->GetNbinsX();
  else fCurrentWaveformSize = kDefaultWaveformSize;
  SetNewScopeTrace(aPulse);
  SetLogAmpParameters(a,b,logoffset,chargeFactor,time,scopeoffset);
  SetScopeTriggerPosition(0.10); //this should be determined from the NQxx bank!
  SetDelogging(deLog);
  SetDePreamp(dePreamp);
  SetRmPostBaseline(rmPostBaseline);
  SetIsZeroLevelFilter(isZLF);
  SetRiseTimePercent(aRTPercent);
  SetBaselineRemovalPercent(aBLRPercent);
  SetUseMeasuredScopeOffset(0);

  ResetCalculatedValues();
  ProcessWaveforms();

  //As you'll notice in the .h file, I've turned off the zero level filter because it doesn't always work right.  oscillatory noise event should integrate to very small amplitudes.  also, during commissioning, we shouldn't be cutting out any wavforms yet.

}

QScopeAnalysis::~QScopeAnalysis()
{
  Clear();
}


void QScopeAnalysis::SetNewScopeTrace(TH1C* newPulse)
{
  
  fRawValues.SetBins(fCurrentWaveformSize,0,fCurrentWaveformSize);
  fAntiloggedValues.SetBins(fCurrentWaveformSize,0,fCurrentWaveformSize);
  fIntegratedValues.SetBins(fCurrentWaveformSize,0,fCurrentWaveformSize);
  fBaselineRemovedValues.SetBins(fCurrentWaveformSize,0,fCurrentWaveformSize);
  fInverseAntiloggedValues.SetBins(fCurrentWaveformSize,0,fCurrentWaveformSize);
  fOriginalNCDPulse.SetBins(fCurrentWaveformSize,0,fCurrentWaveformSize);

  fRawValues.AddDirectory(0); // ALH- 1/27/05  The addDirectory needs to 
  // preceed Copy; else we leave the TH1C in the object table. I would suggest
  // that all the AddDirectory's should get done in the constructors.

  if(newPulse != NULL) newPulse->Copy(fRawValues);
  //  fRawValues.SetEntries(fCurrentWaveformSize);  TH1::SetBinContent now increments the number of Entries
  //measure the scope offset here so that it only has to happen once every time a new scope trace is loaded
  //MeasureScopeOffset();

  fAntiloggedValues.UseCurrentStyle();
  //  fAntiloggedValues.SetEntries(fCurrentWaveformSize);
  fAntiloggedValues.AddDirectory(0);

  fIntegratedValues.UseCurrentStyle();
  //  fIntegratedValues.SetEntries(fCurrentWaveformSize);
  fIntegratedValues.AddDirectory(0);

  fBaselineRemovedValues.UseCurrentStyle();
  //  fBaselineRemovedValues.SetEntries(fCurrentWaveformSize);
  fBaselineRemovedValues.AddDirectory(0);

  fInverseAntiloggedValues.UseCurrentStyle();
  //  fInverseAntiloggedValues.SetEntries(fCurrentWaveformSize);
  fInverseAntiloggedValues.AddDirectory(0);

  fOriginalNCDPulse.UseCurrentStyle();
  //  fAntiloggedValues.SetEntries(fCurrentWaveformSize);
  fOriginalNCDPulse.AddDirectory(0);

  
  ResetCalculatedValues();
}

void QScopeAnalysis::SetBaselineRemovalPercent(Float_t aPercent)
{
  //set the percent of the post pulse baseline to use in the baseline removal routine
  //ResetCalculatedValues is called at the end of this method
  
  //just in case someone screws up and puts in a value > 1
  if(aPercent <= 1){
    fBaselineRemovalPercent = aPercent;
  }
  else fBaselineRemovalPercent = 0.3;  //default value
  
  if(aPercent < 0) fBaselineRemovalPercent = 0;
  //reset calculated values  - waveforms must be reprocessed when this method is called
  ResetCalculatedValues();  
}

void QScopeAnalysis::SetLogAmpParameters(Float_t a, Float_t b, Float_t logoffset, Float_t chargeFactor, Float_t time, Float_t scopeoffset)
{
  //set the logamp parameters by hand.
  //ResetCalculatedValues is called at the end of this method

  faParameter = a;
  fbParameter = b;
  fLogOffset = logoffset;
  fpreampRCFactor = chargeFactor;
  felecDelayTime = time;
  fScopeOffset = scopeoffset;

  ResetCalculatedValues(); 
}

void QScopeAnalysis::GetLogAmpParameters(Float_t &a,Float_t &b, Float_t &logoffset, Float_t &chargeFactor, Float_t &time, Float_t &scopeoffset)
{
  a = faParameter;
  b = fbParameter;
  logoffset = fLogOffset;
  chargeFactor = fpreampRCFactor;
  time = felecDelayTime;
  scopeoffset = fScopeOffset;
}

Bool_t QScopeAnalysis::IsGoodLogAmpParameters(void)
{
  Bool_t theReturn;
  //old test included the preampRC factor and the elecDelayTime, which have yet to be
  //used anywhere in the code, except for the now defunt DePreamp method
  /*if(faParameter == kBadCalibrationValue || fbParameter == kBadCalibrationValue ||
     fLogOffset == kBadCalibrationValue || fpreampRCFactor == kBadCalibrationValue ||
     felecDelayTime == kBadCalibrationValue || fScopeOffset == kBadCalibrationValue) theReturn = false;
  */
  
  //no longer care about preampRCFactor or the elecDelayTime
  if(faParameter == kBadCalibrationValue || fbParameter == kBadCalibrationValue ||
     fLogOffset == kBadCalibrationValue || 
     fScopeOffset == kBadCalibrationValue) theReturn = false;
  
  else if(faParameter*fbParameter*fLogOffset*fScopeOffset == 0.0) theReturn = false;
  else theReturn = true;  //could put in some sanity checks here since we know approximately what the ranges of the
  //logamp parameters should be.  We should at least know the sign of the parameters!!

  return theReturn;

}

void QScopeAnalysis::SetIsZeroLevelFilter(Bool_t aState)
{
  //Turns the use of the ZeroLevelFilter on or off  (1 = on, 0 = off)
  //ResetCalculatedValues is called at the end of this method
  fIsZeroLevelFilter = aState;
  ResetCalculatedValues(); 
}

void QScopeAnalysis::SetRiseTimePercent(Float_t aPercent)
{
  //Changes the percent of total energy that is used to define the pulse width
  //ResetCalculatedValues is called at the end of this method

  fRiseTimePercent = aPercent;
  ResetCalculatedValues(); 
}

void QScopeAnalysis::SetDelogging(Bool_t deLog)
{
  //Turns delogging on or off depending upon the deLog value (1 = on, 0 = off).  
  //ResetCalculatedValues is called at the end of this method

  fIsDelogging = deLog;
  ResetCalculatedValues(); 
}

void QScopeAnalysis::SetDePreamp(Bool_t dePreamp)
{
  fIsDePreamp = dePreamp;
  if(dePreamp){
    cout << "QScopeAnalysis::SetDePreamp: Beware - we don't normally turn the AntiPreamp method on." << endl;
    cout << "Make sure that the preampRC value is set properly.  This parameter is not normally checked." << endl;
  }
  ResetCalculatedValues();
}

void QScopeAnalysis::SetRmPostBaseline(Bool_t RmPostBaseline)
{
  fRmPostBaseline = RmPostBaseline;
  ResetCalculatedValues();
}

void QScopeAnalysis::SetUseMeasuredScopeOffset(Bool_t anOption)
{
  fIsUseMeasuredScopeOffset = anOption;
  ResetCalculatedValues();
}

void QScopeAnalysis::ResetCalculatedValues(void)
{
  //This sets the Charge, RiseTime and PulseStartTime values back to zero
  
  fCharge = 0;
  fRiseTime = 0;
  fPulseStartTime = 0;
}

void QScopeAnalysis::ClearAnalysisWaveforms(void)
{
  /*
  fAntiloggedValues.Scale(0);
  fIntegratedValues.Scale(0);
  fBaselineRemovedValues.Scale(0);
  fInverseAntiloggedValues.Scale(0);
  fOriginalNCDPulse.Scale(0);

  fAntiloggedValues.SetEntries(0);
  fIntegratedValues.SetEntries(0);
  fBaselineRemovedValues.SetEntries(0);
  fInverseAntiloggedValues.SetEntries(0);
  fOriginalNCDPulse.SetEntries(0);
*/
  fAntiloggedValues.Reset();
  fIntegratedValues.Reset();
  fBaselineRemovedValues.Reset();
  fInverseAntiloggedValues.Reset();
  fOriginalNCDPulse.Reset();
}

Bool_t QScopeAnalysis::ProcessWaveforms(void)
{
  //This processes the waveform and calculates the measureable quantities

  Bool_t theReturn = false;

  ClearAnalysisWaveforms();

  //AntiLog, BaselineRemoval, AntiPreamp, ZeroLevelFilter, StartPosition, and RunningIntegral must be done first and in this order!!!
  if(IsGoodLogAmpParameters() || ( !GetIsDeloggingOn() && GetIsUsingMeasuredScopeOffset()) ){
    theReturn = true;
    AntiLog();
    BaselineRemoval();   //this can effectively be turned off by setting the baselinePercent value to 0
    AntiPreamp();   //removes the effect of the preamps RC circuit - by default this is turned off though.
    ZeroLevelFilter();
    StartPosition();
    RunningIntegral();
    
    //Now can calculate measureable quantites
    RiseTimeAndCharge();
  }
  else{
    theReturn = false;
    if(fMessages) printf("Bad logamp parameters. Can't analyze waveform\n");
  }
  return theReturn;
}


void QScopeAnalysis::AntiLog(void)
{  
  //Antilog takes the parameters extracted from the logamp calibration and uses them to antilog the scope trace.
  //For now we are using a generic set of default values (from MUX 0, Ch 0).
  //In general we will want to read the parameters for each channel for each run.
  Float_t scopeOffset;
  if(GetIsUsingMeasuredScopeOffset()) scopeOffset = GetMeasuredScopeOffset();
  else scopeOffset = fScopeOffset; //use the value in the ncla bank
  
  //if deLog is true, then delog the pulse... 
  //otherwise, leave it be, but copy the non-delogged trace into the fAntiloggedValues
  //so the rest of the class will function normally, but with a pulse that hasn't been delogged
  //use this only for special circumstances
  if(GetIsDeloggingOn() == true){
    DelogWaveform(fRawValues,fAntiloggedValues,faParameter,fbParameter,fLogOffset,scopeOffset);
  }
  else {
    for(Int_t i = 0; i < fCurrentWaveformSize; i++)
      fAntiloggedValues.SetBinContent(i,fRawValues.GetBinContent(i)-scopeOffset);
  }
  
  return;
}

Bool_t QScopeAnalysis::DelogWaveform(TH1 &raw, TH1 &delog, Float_t a, Float_t b,
                     Float_t c,Float_t preTrigOffset,Bool_t delogFinalBin)
{
  //This is a stand-alone method for delogging a waveform of any number of bins using the 
  //logamp parameters provided by the user.  This method is called by the AntiLog method.
  //
  //Note that this method delogs each bin in the waveform, starting with bin 0 up to 
  //but excluding the last bin and the overflow bin.
  //This is because the NCD pulses are stored from bin zero up to NBinsX - 1.  
  //
  //However, the last argument of this method is a switch that allows the user
  //to change this behavior.  When the switch is true, then the underflow bin (bin 0)
  //is not delogged and the last bin (but 
  //not the overflow bin) is delogged.  This was a requested feature
  //but should not be used by default.  The switch is set to false in the 
  //declaration of the method and is thus off unless specified by the user
  //
  //The switch should be left to the default value of false when analyzing a 
  //standard NCD waveform (as stored in the MuxScope class).
  //
  //The user must take care to use the proper bins in 
  //their analysis since this delogging method, with the switch set to true, 
  //will change the value of those bins
  //to a non-zero value.  
  //
  //The rest of QScopeAnalysis uses this method with the switch off and should continue to do so.
  //Thus, this switch only affects direct users of this method.  If you load QScopeAnalysis with
  //a pulse, the switch will be off during the analysis of the pulse.
  
  
  if(raw.GetNbinsX() != delog.GetNbinsX()){
    cout << "QScopeAnalysis::DelogWaveform - Error: Waveforms do not have the same number of bins." << endl;
    cout << "Exiting DelogWaveform method with nothing done." << endl;
    return false;
  }
  
  //the "b" parameter in the pretrigger region
  Double_t b_prime = b * pow(10, -c/a);

  //add one so that the following for loop delogs the final bin
  for(Int_t i = (0x1 & delogFinalBin); i < raw.GetNbinsX() + (0x1 & delogFinalBin); i++){
    if(i < fScopeTriggerPosition * raw.GetNbinsX()) {  //fScopeTriggerPosition THIS SHOULD BE OBTAINED FROM NQXX BANK.  BUT HOW?
                                                            //fAntiloggedValues.SetBinContent(i,fRawValues.GetBinContent(i) - scopeOffset);
                                                            //fAntiloggedValues.SetBinContent(i,0);
      delog.SetBinContent(i,b_prime * (pow(10,(raw.GetBinContent(i) - preTrigOffset) / a) - 1.0));
    }
    else {
      delog.SetBinContent(i,b * (pow(10,(raw.GetBinContent(i) - c - preTrigOffset) / a) - 1.0));
    }
  }
  
  return true;
  
}

void QScopeAnalysis::InverseAntiLog(void)
{
  //This method logs the values found in fAntiloggedValues using the given logamp parameters
  
  Float_t scopeOffset;
  if(GetIsUsingMeasuredScopeOffset()) scopeOffset = GetMeasuredScopeOffset();
  else scopeOffset = fScopeOffset; //use the value in the ncla bank
  

  Int_t i = 0;
  //  fInverseAntiloggedValues.SetEntries(0);  //reset them to zero because SetBinContent adds an entry each time
  for (i = 0; i < fCurrentWaveformSize; i ++) {
  
    if(i < fScopeTriggerPosition * fCurrentWaveformSize) { //HARDCODED 10%!!!  THIS SHOULD BE OBTAINED FROM NQXX BANK.  BUT HOW?
      fInverseAntiloggedValues.SetBinContent(i,fRawValues.GetBinContent(i));
      //fInverseAntiloggedValues.SetBinContent(i,fAntiloggedValues.GetBinContent(i) + fScopeOffset);
    }
    else {
      fInverseAntiloggedValues.SetBinContent(i,faParameter*log10(1+fAntiloggedValues.GetBinContent(i)/fbParameter) + fLogOffset + scopeOffset);
    }
  
  }
  
  return;
}

void QScopeAnalysis::AntiPreamp(void)
{
  //anti preamp is turned off by default. This is the default option forncd pulse shape analysis since I am told that we will not attempt to deconvolve the preamp/logamp RC effects from the delogged pulse.
  if(GetIsDePreampOn()){
    for(Int_t i=0; i < fCurrentWaveformSize; i++){
      if(i < fScopeTriggerPosition * fCurrentWaveformSize) { //HARDCODED 10%!!!  THIS SHOULD BE OBTAINED FROM NQXX BANK.  BUT HOW?
	fOriginalNCDPulse.SetBinContent(i,fBaselineRemovedValues.GetBinContent(i));
      }
      else {
	fOriginalNCDPulse.SetBinContent(i,fBaselineRemovedValues.GetBinContent(i) + ((1/fpreampRCFactor)*fBaselineRemovedValues.Integral(fScopeTriggerPosition * fCurrentWaveformSize,i)) );
      }
    }
  }
  else {  //if dePreamp is off, then do nothing!  This is the default option forncd pulse shape analysis since I am told that we will not attempt to deconvolve the preamp/logamp RC effects from the delogged pulse.
    for(Int_t i=0; i < fCurrentWaveformSize; i++){
      fOriginalNCDPulse.SetBinContent(i,fBaselineRemovedValues.GetBinContent(i));
    }
  }
}

void QScopeAnalysis::BaselineRemoval(void)
{
  //BaselineRemoval determines the average baseline over the last 30% of the scope trace (by default) and then 
  //subtracts that average baseline from each point in the scope trace.  The percentage can be changed by giving this method a float arguement
  //BaselineRemoval shouldn't be necessary if the correct antilog parameters are being used, since AntiLog 
  //will automatically produce a pulse with a zeroed baseline.
  //By default this method has now effectively been turned off by setting the default percentage of waveform that is averaged to 0.

  Float_t sum = 0;
  Double_t baselineAverage = 0;
  Int_t i = 0;
  Int_t binStart = fCurrentWaveformSize - (Int_t)(GetBaselineRemovalPercent()*fCurrentWaveformSize); 
  for (i = binStart; i < fCurrentWaveformSize; i++) {    
    sum += fAntiloggedValues.GetBinContent(i);
  }                                    
  if(GetBaselineRemovalPercent() != 0 && sum != 0)   baselineAverage = sum / (GetBaselineRemovalPercent()*fCurrentWaveformSize);
  else baselineAverage = 0;

  for (i = 0; i < fCurrentWaveformSize; i++) {  
    fBaselineRemovedValues.SetBinContent(i,(fAntiloggedValues.GetBinContent(i) - baselineAverage));
  }

  return;
}


void QScopeAnalysis::RunningIntegral()
{
  //RunningIntegral outputs a new histogram that is the integral of the input histogram.  
  //The integration starts at StartPosition, since the pre-pulse baseline is different than the true 
  //post-pulse baseline. 
  //For now we do this to the fBaselineRemovedValues
  //We only do RunningIntegral if the event has passed ZeroLevelFilter.
  
  //MUST DO Antilog, BaselineRemoval, AntiPreamp, StartPosition and before calling this method
  Float_t sum = 0;
  Int_t i = 0;

  if (GetDidPassZeroLevelFilter()) {
    for (i = fPulseStartTime; i < fCurrentWaveformSize; i++) {
      sum += fOriginalNCDPulse.GetBinContent(i);  
      fIntegratedValues.SetBinContent(i, sum);  
    }           //this integrates starting with first negative bin through the end of the pulse
  }
}

Int_t QScopeAnalysis::StartPosition()
{
  //StartPosition finds the position of maximum (negative) deviation from zero in the scope trace, then 
  //steps backwards from there to where the trace crosses zero (if it never does then return 0) - that is the
  //StartPosition.
  //Different from Analyst STG since STG didn't require a zero crossing, just getting within some multiple 
  //of the baseline sigma of zero.  
  //Because of this, StartPosition will tend to give earlier start times than STG.  
  //StartPosition won't work right if the baseline is not zero, so we use fOriginalNCDPulse.
  //We only do StartPosition if the event has passed the ZeroLevelFilter, otherwise return -1.

  fPulseStartTime = 0;
  Double_t maximumDeviation = 0;  //maximum deviation from 0 in the scope trace
  Int_t maximumDeviationPosition;   //bin number of maximum deviation position
  Int_t i = (Int_t)(fScopeTriggerPosition * fCurrentWaveformSize);     
  //I think this is a bug here -> bin 0 of fOriginalNCDPulse seems to be a large number 
  //and is causing this method to fail unless it is started at bin 1.  
  //And this is because the bin 0 in the raw trace is not being filled as it should be.

  if (GetDidPassZeroLevelFilter()) {
    for (i; i < fCurrentWaveformSize; i++) {  
      if (fOriginalNCDPulse.GetBinContent(i) < maximumDeviation) {
	//assuming downward going pulse 
	maximumDeviation = fOriginalNCDPulse.GetBinContent(i);
	maximumDeviationPosition = i;  //i has maximumDeviation
      }
    }
    i = maximumDeviationPosition;  //now set i to MaximumDeviationPosition
    while ( (fOriginalNCDPulse.GetBinContent(i) < 0) && (i != 0) ) {
      //as long as pulse is negative
      i--;			//step backwards through pulse from i
    }               
    fPulseStartTime = i + 1;    //last bin > 0 if going forward so i + 1 is first bin < 0
                       //if reached start of pulse before zero crossing, then startPosition is 0    
    //printf("pulse start time: %d\n",fPulseStartTime);
    
  } 
  else {
    fPulseStartTime = 0; //if event fails ZeroLevelFilter, return 0 for StartPosition
  }
  
  return fPulseStartTime;
}


Int_t QScopeAnalysis::EndPosition()
{
  //EndPosition finds the position of maximum (negative) deviation from zero in the scope trace, then 
  //steps forward from there to where the trace crosses zero (if it never does then return 0)
  //  - that is the EndPosition.
  // see also QScopeAnalysis::StartPosition

  fPulseEndTime = 0;
  Double_t maximumDeviation = 0;  //maximum deviation from 0 in the scope trace
  Int_t maximumDeviationPosition;   //bin number of maximum deviation position
  Int_t i = (Int_t)(fScopeTriggerPosition * fCurrentWaveformSize);     //I think this is a bug here -> bin 0 of fOriginalNCDPulse seems to be a large number and is causing this method to fail unless it is started at bin 1.  And this is because the bin 0 in the raw trace is not being filled as it should be.

  if (GetDidPassZeroLevelFilter()) {
    for (i; i < fCurrentWaveformSize; i++) {  
      if (fOriginalNCDPulse.GetBinContent(i) < maximumDeviation) {
	//assuming downward going pulse 
	maximumDeviation = fOriginalNCDPulse.GetBinContent(i);
	maximumDeviationPosition = i;  //i has maximumDeviation
      }
    }
    i = maximumDeviationPosition;  //now set i to MaximumDeviationPosition
    while ( (fOriginalNCDPulse.GetBinContent(i) < 0) && (i != 0) ) {
      //as long as pulse is negative
      i++;			//step forward through pulse from i
    }               
    fPulseEndTime = i - 1;    //last bin > 0 if going forward so i - 1 is first bin > 0
                       //if reached end of pulse before zero crossing, then startPosition is 0    
//    printf("pulse end time: %d\n",fPulseEndTime);
    
  } 
  else {
    fPulseEndTime = fOriginalNCDPulse.GetNbinsX(); //if event fails ZeroLevelFilter, return largest bin for EndPosition
  }
  
  return fPulseEndTime;
}


Bool_t QScopeAnalysis::ZeroLevelFilter()
{
  //ZeroLevelFilter calculates the maximum delta between consecutive zero crossings.  
  //If MaximumDelta is <= 300 points, then 0 is returned to flag the event for cutting.  
  //ZeroLevelFilter is applied to data that has been antilogged (and, for the time being, that has had 
  //residual baseline removed).
  //This cut removes many types of noise events, such as baseline noise, oscillatory noise, and microphonic 
  //noise.  
  //ZeroLevelFilter is different from Analyst ZLF in that it does not calculate the maximum deviation from 
  //zero in the event - that is done in StartPosition.
  //It also does not calculate the number of zero crossings.
  //The value of the fMaximumDeltaCut was developed from tests on non-antilogged data, but we would like to 
  //apply the cut to antilogged data (because we will remove the baseline simultaneously with antilogging).
  //It won't matter much since the antilogging has minimal affect on the horizontal width of a pulse.

  //if the ZLF is turned off, just return true and don't cut any events
  if(GetIsZeroLevelFilter() == false) {
    fPassZLF = true;
    return fPassZLF;
  }

  Int_t fMaximumDeltaCut = 300;

  Double_t maximumDelta = 0;
  Int_t i = 0;
  Int_t zeroCrossing1 = 0;
  Int_t zeroCrossing2 = 0;

  while (i < fCurrentWaveformSize) {   //as long as i is < end of scope trace
    //if between i and i + 1, the value goes from negative (or zero) to 
    //positive (or zero) or from positive (or zero) to negative (or zero), 
    //then record i in ZeroCrossing1 and increment i 
    if ( ( (fOriginalNCDPulse.GetBinContent(i) <= 0) && 
	   (fOriginalNCDPulse.GetBinContent(i + 1) >= 0) ) 
	 || ( (fOriginalNCDPulse.GetBinContent(i) >= 0) && 
	      (fOriginalNCDPulse.GetBinContent(i + 1) <= 0) ) ) {   
      zeroCrossing1 = i;
      i++;
      while (i < fCurrentWaveformSize) { //as long as i is < end of scope trace
	//if find another zero crossing, break out of loop
	if ( ( (fOriginalNCDPulse.GetBinContent(i) <= 0) && 
	       (fOriginalNCDPulse.GetBinContent(i + 1) >= 0) ) 
	    || ( (fOriginalNCDPulse.GetBinContent(i) >= 0) && 
		 (fOriginalNCDPulse.GetBinContent(i + 1) <= 0) ) ) {
	  break;
	}
	//if don't find another zero crossing, increment i and look again
	//do this until you find second zero crossing or reach end of trace
	i++;
      }
      //record i of second zero crossing in ZeroCrossing2
      //if and only if it didn't find second zero crossing, then it will exit 
      //the while loop with i = end of scope trace
      //so if i = end of scope trace, don't want to record ZeroCrossing2
      if (i != fCurrentWaveformSize) {
	zeroCrossing2 = i;
      }
      //if difference between two zero crossings is greater than MaximumDelta 
      //then record difference as new MaximumDelta
      if ( (Int_t(zeroCrossing2 - zeroCrossing1) ) > maximumDelta) {
	maximumDelta = Double_t(zeroCrossing2 - zeroCrossing1);
      }
    } else {
      i++;
    }
    //if didn't find the first zero crossing, then increment i and look again
  }
  if (maximumDelta <= fMaximumDeltaCut) {
    fPassZLF = false;
  } else {
    fPassZLF = true;
  }
  
  return fPassZLF;
}


Float_t QScopeAnalysis::ChargeIntegral()
{
  //ChargeIntegral returns the value of the integral of the input histogram from the StartPosition up to 
  //the specified upper limit, or up to the end of the pulse (by default).
  //For now doing ChargeIntegral to fOriginalNCDPulse, since it is crucial that the baseline be 
  //properly zeroed in order to get a sensible integral.
  //We only do ChargeIntegral if the event has passed ZeroLevelFilter.

  if(fPulseStartTime == 0) StartPosition();  //in case StartPosition was called already!

  if(fRmPostBaseline)
    fPulseEndTime = EndPosition();
  else
    fPulseEndTime = fOriginalNCDPulse.GetNbinsX()-1;

  if (GetDidPassZeroLevelFilter() && !fCharge) {  //only calculate if not already calculated and if pass ZLF
    fCharge = fOriginalNCDPulse.Integral(fPulseStartTime,fPulseEndTime);
  } 
  else {
    fCharge = 0; //if event fails ZeroLevelFilter, return 0 for ChargeIntegral
  }
  
  return fCharge;
}

Int_t QScopeAnalysis::RiseTime(void)
{
  if(!fRiseTime) RiseTimeAndCharge();  //if fRiseTime has been calculated, then don't waste the CPU cycles
  return fRiseTime;
}

Bool_t QScopeAnalysis::RiseTimeAndCharge(void)
{
  if(fRiseTime && fCharge) return true; //the values have already been calculated.
  Bool_t noErr = true;
  fRiseTime = 0; fCharge = 0.0;
  Double_t fractionChargeIntegral;

  if(fPulseStartTime == 0) StartPosition();  //just in case StartPosition wasn't called already!
  Int_t i = fPulseStartTime;  //ensures that riseTime > 0 and eliminates about 2000 calls to GetBinContent()
  if(fRmPostBaseline)
    fPulseEndTime = EndPosition();
  else
    fPulseEndTime = fOriginalNCDPulse.GetNbinsX();
  if (GetDidPassZeroLevelFilter()) {
    fCharge = fOriginalNCDPulse.Integral(fPulseStartTime,fPulseEndTime);
    fractionChargeIntegral = fCharge * fRiseTimePercent;	
    while (fIntegratedValues.GetBinContent(i) > fractionChargeIntegral && i < fCurrentWaveformSize) {
      i++;
    }
    fRiseTime = i - fPulseStartTime;
  } 
  else {
    noErr = false; //if event fails ZeroLevelFilter, return false
  }

  return noErr;
}


void QScopeAnalysis::GetAltRiseTimeFWHM(Int_t &risetime, Int_t &FWHM){
  //Hamish Leslie's alternative definition of FWHM and risetime of a scope
  //trace.
  
  //I Fill sctrace[] with the contents of the antilogged scope trace

  Double_t *sctrace = new Double_t[fCurrentWaveformSize];

  for(int ialog=0;ialog<fCurrentWaveformSize;ialog++)
    sctrace[ialog] = fOriginalNCDPulse.GetBinContent(ialog);

  //Find minimum bin number (peak) and contentof the 4 channel sum (mino)

  int peak = 0, it11;
  float mino = 0.0;

  for(int it1 = 1500 ; it1 < 5000 ; it1 = it1 + 4){
    float sum4 = 0.0;
    for (it11 = 0 ; it11 < 4 ; it11++)
      sum4 = sum4 + sctrace[it1+it11];

    if(sum4<mino){
      mino = sum4;
      peak = it1+2;
    }

  }
  //Find the half minimum points width1 and width2
  //and the channels at which the leading edge drops to 25% and 75% of mino
  int width1,width2,rise1,rise2,w1ind=0,w2ind=0,r1ind=0,r2ind=0;
  float  hmino = mino / 2.0;
  float  qmino = mino / 4.0;
  float q3mino = mino * 0.75;
  for(int it1=1500;it1<5000;it1=it1+4){
    float sum4 = 0.0;
    float sum4d = 0.0;
    for (it11=0 ; it11<4 ; it11++){
      sum4=sum4+sctrace[it1+it11];
      sum4d=sum4d+sctrace[6500-it1+it11-4];
    }
    if(w1ind==0&&sum4<hmino){
      width1=it1+2;
      w1ind=1;
    }
    if(w2ind==0&&sum4d<hmino){
      width2=6500-it1-2;
      w2ind=1;
    }
    if(r1ind==0&&sum4<qmino){
      rise1=it1+2;
      r1ind=1;
    }
    if(r2ind==0&&sum4<q3mino){
      rise2=it1+2;
      r2ind=1;
    }
  }
  //subtract to give risetime and Full Width at Half Maximum.
  risetime = rise2-rise1;
  FWHM     = width2-width1;
  delete [] sctrace;
}

Int_t QScopeAnalysis::GetTimeBetweenAmplitudes(TH1 *delog,  Float_t startPer,
                                              Float_t endPer, Int_t minBin)
{
  //This finds the amount of time between the first bin that is startPer% of the maximum 
  //negative going pulse amplitude to the first bin that is endPer% of the maximum.  The
  //default values of startPer and endPer are 10% and 50%.  If the startPer or endPer bin is before 
  //the minBin, then a -1 is returned.
  
  Int_t maxSigBin = delog->GetMinimumBin();
  Double_t maxSigVal = delog->GetMinimum();
  Double_t startBinVal = startPer * maxSigVal;
  Double_t endBinVal = endPer * maxSigVal;
  Int_t startBin = 0;
  Int_t endBin = 0;
  Int_t timeBetweenBins = 0;
  
  if(maxSigBin <= minBin) timeBetweenBins = -1;
  else {
    for(Int_t i = minBin; (i < delog->GetNbinsX()) && (startBin==0 || endBin==0); i++) {
      if(delog->GetBinContent(i) < startBinVal && (startBin == 0) ) startBin = i;
      if(delog->GetBinContent(i) < endBinVal && (endBin == 0) ) endBin = i;
    }
    timeBetweenBins = endBin - startBin;
  }
    
  return timeBetweenBins;
}

Int_t QScopeAnalysis::GetTimeBetweenAmplitudes(Float_t startPer,Float_t endPer, Int_t minBin)
{
  //This finds the amount of time between the first bin that is startPer% of the maximum 
  //negative going pulse amplitude to the first bin that is endPer% of the maximum.  The
  //default values of startPer and endPer are 10% and 50%.  If the startPer or endPer bin is before 
  //the minBin, then a -1 is returned.  This method uses the fOriginalNCDPulse histogram,
  //which is the histogram pulse that is returned by the GetNCDPulse metho and is the fully
  //delogged signal.
  
  return GetTimeBetweenAmplitudes(GetNCDPulse(),startPer,endPer,minBin);
  
}

void QScopeAnalysis::MeasureScopeOffset(void)
{
  Float_t average;
  //Float_t chi2; 
  //Int_t ndof;
  //TF1 Localfunc("Localfunc","[0]",0,fScopeTriggerPosition * 0.75 * fCurrentWaveformSize);
  //fRawValues.Fit("Localfunc","R");
  //fMeasuredScopeOffset = Localfunc.GetParameter(0);
  //fMeasuredScopeOffsetUncer = Localfunc.GetParError(0);
  //chi2 = Localfunc.GetChisquare();
  //ndof = Localfunc.GetNDF();
  Int_t i;
  for(i=0;i<fScopeTriggerPosition*0.75*fCurrentWaveformSize;i++){
    average += fRawValues.GetBinContent(i);
  }
  if(i!=0) average = average/(Float_t)i;
  else average = 0;
  fMeasuredScopeOffset = average;
  if(fMessages) printf("MeasuredScopeOffset Results\n");
  //printf("MeasuredScopeOffset = %f +- %f\n",fMeasuredScopeOffset,fMeasuredScopeOffsetUncer);
  //printf("Chi^2/DOF = %f/%d\n",chi2,ndof);
  if(fMessages) printf("Computed Average of first %d points: %f\n",i,average);
  if(fMessages) printf("Compare to NCLA bank value of %f\n\n",fScopeOffset);

}

void QScopeAnalysis::PrintSettingsStatus(void)
{
      
  cout << "QScopeAnalysis Settins Status" << endl << endl;
  cout << "       DeLogging option is set to:  " << (GetIsDeloggingOn() ? "on" : "off") << endl;
  cout << "       Logamp Parameter staus is:  " << (IsGoodLogAmpParameters() ? "good" : "bad") << endl;
  cout << "              parameter 'a': " << faParameter << endl;
  cout << "              parameter 'b': " << fbParameter << endl;
  cout << "              parameter 'fLogOffset' or 'c' : " << fLogOffset << endl;
  cout << "              parameter 'fpreampRCFactor' (no longer used) : " << fpreampRCFactor << endl;
  cout << "              parameter 'felecDelayTime' or 'delta T' : " << felecDelayTime << endl;
  cout << "              parameter 'fScopeOffset' or 'pre-trigger offset' : " << fScopeOffset << endl;
  cout << "       UsedMeasuredScopeOffset option is set to:  " << (GetIsUsingMeasuredScopeOffset() ? "on" : "off") << endl;
  cout << "       RmPostBaseline option is set to:  " << (GetIsRmPostBaseline() ? "on" : "off") << endl;
  cout << "       DePreamp option is set to:  " << (GetIsDePreampOn() ? "on" : "off") << endl;
  cout << "       Zero Level Filter is set to:  " << (GetIsZeroLevelFilter() ? "on" : "off") << endl;
  cout << "       Scope Trigger Position (in fraction of total number of bins) is set to: " << GetScopeTriggerPosition() << endl;
  cout << "       Rise-time percentage level is set to: " << GetRiseTimePercent() << endl;
  cout << "       Baseline Removal Percent is set to:  " << GetBaselineRemovalPercent() << endl;
  
}
