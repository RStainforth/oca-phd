#ifndef QNCD_h
#define QNCD_h
#include "TVector3.h"
class QNCDArray;

class QNCD{
public:
  // 13.06.2005 -- os
  // added ncd index in initialization
  QNCD(Float_t x,Float_t y,Float_t length,Int_t index); 
//  QNCD(char *aName, QNCDArray *anArray,float aLength); 
//  QNCD(){fPresent=0; return;}
  virtual ~QNCD(){return;}
//  void Remove(){fPresent=0;};
//  int IsPresent(){return fPresent;}
  const TVector3 &GetTop(){return fTop;}
  const TVector3 &GetBottom(){return fBottom;}
//  int IsName(char *aName){return ((fName[0]==aName[0]) && (fName[1]==aName[1]));}
//  int NCDCoords(const char *astr, float &ax, float &ay, QNCDArray *anArray);
  float GetLength(){fV=fTop-fBottom; return fV.Mag();}
//  char fName[3];  // Name of string (ie. A1)
  TVector3 GetClosestPoint(TVector3 &aPosition,TVector3 &aDirection,Bool_t shift = kFALSE);
  Double_t GetDistance(TVector3 &aPosition,TVector3 &aDirection,Int_t &status);

  // 13.06.2005 -- os
  // return functions for newly added stuff 
  Int_t GetIndex(){return fIndex;}
  const TVector3& GetCenter(){return fCenter;}
  void SetCenter(TVector3 aVector);

  // function to shift the fCenter vector
  void Shift(TVector3 aShift);
  void Shift(Double_t aShiftinX = 0., Double_t aShiftinY = 0.);
  void CancelShift();
  Bool_t GetShifted(){return fShifted;}

private:
  TVector3 fBottom; // Bottom of string
  TVector3 fTop;    // top of string
  // 13.06.2005 -- os
  // added the ncd index and center vector
  Int_t fIndex;		// counter index
  TVector3 fCenter;	// center of string (defined by z = 0)

  Bool_t fShifted;      // set to TRUE if this NCD has been shifted
  
  int fPresent;  // set to 1 if string is in array
  TVector3 fV;   // working vector
  ClassDef(QNCD,1) 	// NCD string geometrical object
};

#endif
