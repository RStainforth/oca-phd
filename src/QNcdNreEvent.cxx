/*
 *  QNcdNreEvent.cxx
 *  Xqsno
 *
 *  Created by Adam Cox on 9/21/06.
 *  Copyright 2006 University of Washington. All rights reserved.
 *
 */

#include <iostream.h>
#include "QNcdNreEvent.h"
#include "QMuxScope.h"

ClassImp (QNcdNreEvent);

QNcdNreEvent::QNcdNreEvent(void)
{
  
  Initialize();
}

QNcdNreEvent::~QNcdNreEvent(void)
{
  
}

void QNcdNreEvent::Initialize(void)
{

  SetTheCrossingFactor(0.55);  //same as kNCDLogAmpCalibrationLevelCrossingFactor... for now...
  SetStepSize(10); //same as kDefaultCalibStepSize, for now
  SetSquareWidthAtHalfMax(1100);  //the square wave definition in orca is 1000, but the reflections make it slightly wider
  SetMinSize(60);  //this is the minimum amplitude from the baseline that the pulse must be in order to be a NRE pulse
  SetEndOfPrePulseTime(780); //this value is approximately equal to the electronic delay time, plus the width of 
                             //the prepulse plus the total reflection time (300 + 100 + 
  SetVerbose(0);
}


Bool_t QNcdNreEvent::IsNreEvent(QMuxScope *mMuxScope)
{  
  //returns true if it is, false if it ain't... 
  Bool_t theReturn = false;
  Bool_t theFinalReturn = false;
  Int_t peakError = 70;
  Int_t startPos = 1500 +  GetEndOfPrePulseTime();
  Float_t theCrossingLevel;
  Float_t average = 0;
  Int_t preTrigAveSize = 500;  //should be less than 1500... but 500 bins are enough to get an average value
  Short_t size = 14999; //this must be a 15000 bin waveform
  Int_t numLevelCrossingsAfterPrepulse = 2;
  Int_t totalNumLevelCrossings = 6;
    
  if(bVerbose)     cout << "NCD string : " << mMuxScope->GetNCDStringNumber() << endl;
  
  //bin 15000 is set to zero and it screws up some methods, like GetMinimum.
  mMuxScope->GetValues()->GetXaxis()->SetRange(0,14999);  

  if(mMuxScope->GetValues()->GetNbinsX() > size) {  //this must be a 15000 bin waveform
    
    for (Short_t i = 0;i<preTrigAveSize;i++) {
      average += mMuxScope->GetValues()->GetBinContent(i);
    }
    average = average/preTrigAveSize;
    if(bVerbose)  cout << "average : " << average  << endl;
    theCrossingLevel = CalculateCrossingLevel(mMuxScope, average, GetTheCrossingFactor());
    if(bVerbose)  cout << "crossing fraction : " << GetTheCrossingFactor()  << endl;
    if(bVerbose)  cout << "crossing level : " << theCrossingLevel  << endl;
    //if the peak-to-peak amplitude is too small, don't bother...assume that it's not a NRE pulse
    if (average - mMuxScope->GetValues()->GetMinimum() > GetMinSize()) { 
      Int_t numLevelX_startPos = NumberOfLevelCrossings(mMuxScope,theCrossingLevel,startPos);
      Int_t numLevelX = NumberOfLevelCrossings(mMuxScope,theCrossingLevel,0);
      Int_t numLevelX_startPosPlus = NumberOfLevelCrossings(mMuxScope,theCrossingLevel,startPos + 2000);
      if(bVerbose) cout << "number of level crossings " << startPos << " : " << numLevelX_startPos  << endl;
      if(bVerbose) cout << "number of level crossings 0 : " << numLevelX  << endl;
      if(bVerbose) cout << "number of level crossings " << startPos + 2000 << " : " << numLevelX_startPosPlus  << endl;
      
      if ( numLevelX_startPos == numLevelCrossingsAfterPrepulse &&
           numLevelX == totalNumLevelCrossings &&
           numLevelX_startPosPlus == 0) {  
        //there shouldn't be any more level crossings after teh pulse
        theReturn = true;
      }
      //else printf("wrong number of level crossings\n");
    }
  }
  else {
    cout << "QNcdNreEvent::IsNreEvent - Error. The raw waveform has too few bins" << endl;
    theReturn = false;
  }
  
  
  if (theReturn == true) {  
    Int_t crossWidth = CrossingWidth(mMuxScope,theCrossingLevel,4);
    if(bVerbose) cout << "crossing width : " << crossWidth  << endl;
    if(crossWidth >= (GetSquareWidthAtHalfMax() - peakError) && 
       crossWidth <= (GetSquareWidthAtHalfMax() + peakError)) {
      if(bVerbose) cout << "     NRE EVENT IS TRUE"  << endl;
      theFinalReturn = true;
    }
    
  }
  
  else theFinalReturn = theReturn;
  
  return theFinalReturn;
  
  }

Float_t QNcdNreEvent::CalculateCrossingLevel(QMuxScope *mMuxScope, Float_t average, Float_t crossFactor)
{
  mMuxScope->GetValues()->GetXaxis()->SetRange(0,14999);  //the bin 15000 is always zero and so you have to set the proper
                                                          //range in order for GetMinimum to return the value that you want it to.
  Float_t theLevel = average - crossFactor * (average - mMuxScope->GetValues()->GetMinimum());
  
  return theLevel;
}

Int_t QNcdNreEvent::NumberOfLevelCrossings(QMuxScope *mMuxScope, Float_t theLevel,Int_t startBin)
{
  Int_t count = 0;
  Float_t avg1=0;
  Float_t avg2=0;
  Int_t n=0;
  const Int_t k=10;
  
  if(mMuxScope->GetValues()->GetNbinsX() > 0){
    for (Int_t i = GetStepSize()+startBin; i < mMuxScope->GetValues()->GetNbinsX()-GetStepSize();
         i += GetStepSize()) {
      avg1 = avg2 = 0;
      for ( n=i-k ; n<=i+k ; n+=1 ) 
      {
        avg1 += mMuxScope->GetValues()->GetBinContent(n);
        avg2 += mMuxScope->GetValues()->GetBinContent(n+GetStepSize());
      }
      avg1 = avg1/(2*k+1);
      avg2 = avg2/(2*k+1);
      if ( (avg1 < theLevel && avg2 >= theLevel) ||
           (avg1 > theLevel && avg2 <= theLevel)) {
        count++;
      }
    }
  }
  else{
    cout << "QNcdNreEvent::NumberOfLevelCrossings - Error. size of mMuxScope->GetValues() is less than or equal to zero.\n" << endl;
  }
  
  return count;
}

Int_t QNcdNreEvent::CrossingWidth(QMuxScope *mMuxScope, Float_t theLevel, Int_t peakOption)
{
  // return the width spanned by the two crossing locations
  Int_t count=0;
  Int_t width=0;
  Int_t var1=0;
  Float_t avg1=0;
  Float_t avg2=0;
  Int_t n=0;
  const Int_t k=10;
  Bool_t leftEdge=false;
  Bool_t rightEdge=false;
  Int_t startBin = 0;
  
  if (mMuxScope->GetValues()->GetNbinsX() > 0) {
    if(peakOption == 4){
      startBin = 1500 + GetEndOfPrePulseTime();  //move the start position after the prepulse.
      peakOption = 1;
    }
    for (Int_t i = GetStepSize()+startBin; i < mMuxScope->GetValues()->GetNbinsX()-GetStepSize();
         i += GetStepSize()) {
      avg1 = avg2 = 0;
      for ( n=i-k ; n<=i+k ; n+=1 ) {
        avg1 += mMuxScope->GetValues()->GetBinContent(n);
        avg2 += mMuxScope->GetValues()->GetBinContent(n+GetStepSize());
      }
      avg1 = avg1/(2*k+1);
      avg2 = avg2/(2*k+1);
      if ( (avg1 < theLevel && avg2 >= theLevel) ||
           (avg1 > theLevel && avg2 <= theLevel)) {
        count++;
        if ((peakOption==1 && count==1 && leftEdge==false) || 
            (peakOption==2 && count==3 && leftEdge==false) || 
            (peakOption==3 && count==2 && leftEdge==false)) {
          var1 = i;
          leftEdge=true;
        }
        if ((peakOption==1 && count==2 && rightEdge==false) || 
            (peakOption==2 && count==4 && rightEdge==false) || 
            (peakOption==3 && count==3 && rightEdge==false)) {
          width = (i-var1);
          rightEdge=true;
        }
      }
    }
  }
  else{
    cout << "QNcdNreEvent::CrossingWidth.  The QMuxScope waveform has zero bins\n" << endl;
  }
  return width;
}

