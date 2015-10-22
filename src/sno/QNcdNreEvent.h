/*
 *  QNcdNreEvent.h
 *  Xqsno
 *
 *  Created by Adam Cox on 9/21/06.
 *  Copyright 2006 University of Washington. All rights reserved.
 *
 */


#ifndef _QNcdNreEvent_h_
#define _QNcdNreEvent_h_

#include "Rtypes.h"
#include "TNamed.h"

class QMuxScope;

class QNcdNreEvent : public TNamed {
  
public:
  
  QNcdNreEvent(void);
  virtual ~QNcdNreEvent(void);
  
  Bool_t IsNreEvent(QMuxScope *qmx);
  
  //getters and setters for pulse recognition parameters
  //these are initialized in the constructor
  Float_t GetEndOfPrePulseTime(void){ return fEndOfPrePulseTime; }
  Float_t GetTheCrossingFactor(void){ return fCrossingFactor; }
  Int_t GetSquareWidthAtHalfMax(void){ return fSquareWidthAtHalfMax; }
  Int_t GetStepSize(void) {return fStepSize;}
  Int_t GetMinSize(void) {return fMinSize;}

  void SetEndOfPrePulseTime(Float_t val){ fEndOfPrePulseTime = val; }
  void SetTheCrossingFactor(Float_t val){ fCrossingFactor = val; }
  void SetSquareWidthAtHalfMax(Int_t val){ fSquareWidthAtHalfMax = val; }
  void SetStepSize(Int_t s) {fStepSize = s;}
  void SetMinSize(Int_t s) {fMinSize = s;}
  
  void SetVerbose(Bool_t s) {bVerbose = s;}
  Bool_t GetVerbose(void) {return bVerbose;}
  
  //used by the IsNreEvent method and left as public for easier diagnostics
  Int_t NumberOfLevelCrossings(QMuxScope *qmx, Float_t theLevel,Int_t startBin = 0);
  Int_t CrossingWidth(QMuxScope *qmx, Float_t theLevel, Int_t peakOption);
  Float_t CalculateCrossingLevel(QMuxScope *qmx, Float_t average, Float_t crossFactor);
  
private:
    
  void Initialize(void);
  
  Float_t fCrossingFactor;
  Float_t fEndOfPrePulseTime;
  Int_t fSquareWidthAtHalfMax;
  Int_t fStepSize;
  Int_t fMinSize;
  
  Bool_t bVerbose;
  ClassDef(QNcdNreEvent,1);
};

#endif