#ifndef _QMCVX_H
#define _QMCVX_H

#include "TObject.h"
#include "Rtypes.h"
#include <stdio.h>
#include <math.h>

class QMCVX : public TObject {
public:

  QMCVX(); 
  QMCVX( const QMCVX &mcvx );
  QMCVX &operator=(const QMCVX &rhs);
  QMCVX(Double_t, Float_t* xvars, Int_t aBits);
  virtual ~QMCVX();
  void Initialize();
  void Set(Double_t mctime,Float_t* xvars, Int_t aBits);
  void Set(Int_t *mcvxptr, Int_t *mctkptr, Int_t anIndex);
  
  Float_t     GetX() { return fX; }
  Float_t     GetY() { return fY; }
  Float_t     GetZ() { return fZ; }
  Float_t     GetU() { return fU; }
  Float_t     GetV() { return fV; }
  Float_t     GetW() { return fW; }
  Float_t     GetR() { return sqrt( fX*fX + fY*fY + fZ*fZ ); }
  Float_t     GetEnergy() { return fEnergy; }
  Double_t    GetTime() { return fTime; }
  Int_t       GetINC(){   return fINC; }
  Int_t       GetIDP(){ return fIDP; }
  Int_t       GetCLS(){ return fCLS; }
  Int_t       GetRGN(){ return fRGN; }
  Int_t       GetIDM(){ return fIDM; }
  Int_t       GetRG2(){ return fRG2; }
  Int_t       GetIM2(){ return fIM2; }
  Int_t       GetIndex() { return fIndex; }
  Int_t       GetNCerD2O() { return fNCerD2O; }
  Int_t       GetNCerH2O() { return fNCerH2O; }
  Int_t       GetNCerACR() { return fNCerACR; }
    
  void SetX( Float_t x )                  { fX = x; }
  void SetY( Float_t y )                  { fY = y; }
  void SetZ( Float_t z )                  { fZ = z; }
  void SetU( Float_t u )                  { fU = u; }
  void SetV( Float_t v )                  { fV = v; }
  void SetW( Float_t w )                  { fW = w; }
  void SetEnergy(Float_t energy )         { fEnergy = energy; }
  void SetTime( Double_t t )              { fTime = t; }
  void SetINC( Int_t num )                { fINC = num; }
  void SetIndex( Int_t index )            { fIndex = index; }
  void SetIDP( Int_t pid )                { fIDP = pid;}
  void SetCLS( Int_t cls )                { fCLS = cls;}
  void SetRGN( Int_t rgn )                { fRGN = rgn;}
  void SetIDM( Int_t idm )                { fIDM = idm;}
  void SetRG2( Int_t rg2 )                { fRG2 = rg2;}
  void SetIM2( Int_t im2 )                { fIM2 = im2;}
  void SetNCerD2O(Int_t ncer )          { fNCerD2O = ncer; }
  void SetNCerH2O(Int_t ncer )          { fNCerH2O = ncer; }
  void SetNCerACR(Int_t ncer )          { fNCerACR = ncer; }

  Bool_t IsSame(Int_t anIndex, const QMCVX *aQMCVX, Int_t aPrintMode=0)const;
private:
  const Int_t *Data()const{return (Int_t *)&fX;}
  Int_t DataN()const{int i= &fNCerACR-Data();return i;}
public:
  ClassDef(QMCVX,2) //Monte-Carlo vertex


private:
  Float_t fX;     //Vertex x
  Float_t fY;     //Vertex y
  Float_t fZ;     //Vertex z
  Float_t fU;     //Track x direction cosine
  Float_t fV;     //Track y direction cosine
  Float_t fW;     //Track z direction cosine
  Float_t fEnergy;//Energy for outgoing track
  Double_t fTime; //Time in nsec (Gtr_time at vertex)
  Int_t   fINC;   //Vertex interaction code
  Int_t   fIDP;   //Outgoing track particle ID
  Int_t   fIndex; //Vertex Index
  Int_t   fCLS;   //Vertex class
  Int_t   fRGN;   //Region code
  Int_t   fIDM;   //Media code
  Int_t   fRG2;   //2nd region code if boundary
  Int_t   fIM2;   //2nd media code if boundary
  Int_t   fNCerD2O; //Number of Cerenkov photons in D2O
  Int_t fNCerH2O;   //Number of Cerenkov photons in H2O
  Int_t fNCerACR;   //Number of Cerenkov photons in Acrylic vessel
};

#endif









