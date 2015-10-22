#ifndef _QFIT_H
#define _QFIT_H

#include "TObject.h"
#include "Rtypes.h"
#include <stdio.h>
#include <math.h>
#include <TArrayI.h>
#include <TArrayF.h>
#include <TString.h>
#include <TMatrix.h>
#include <TClass.h>

// These have to be consistent with the classifier definitions

#define QFIT_MAX_FIT_OUTPUTS            10
#define QFIT_MAX_BETAS                  5
#define QFIT_MAX_RESIDUAL_HARMONICS     5
#define QFIT_MAX_RESIDUALS              5
#define QFIT_MAX_LEGENDRES              5
#define QFIT_MAX_ANG_CORRELATIONS       17


class QFit : public TObject {

public:

  // Constructors ...
  QFit(); 
  QFit( const QFit &fit );
  QFit &operator=(const QFit &rhs);
  QFit(Float_t *, Int_t , char *);
  QFit(Float_t* xvars, Int_t aBits);
  virtual ~QFit();
  void Initialize();
  void Set(Float_t* xvars, Int_t aBits);
  void SetNumIter(Int_t i){fNumIter = i;}
  Int_t GetNumIter(){return fNumIter; }

  void FromFTXC(Float_t *source, Float_t *destination, Int_t max, Float_t backfill);
  void FromFTXC(Float_t *source, Float_t *destination, Int_t max) {
		FromFTXC( source, destination, max, -999999.0);
	};

  enum FTGOut{GLike, GLikeGrid, GLikeRatio,GRfit, GRfit3, 
	      GFUsed, GChi2, GChi2Nfit, GDiv};
  enum FTPOut{PDStatAng2D, PDStatAngPhi, PDStatTim, PKSProbAng2D, 
	      PKSProbAngPhi, PKSProbTim};

  Float_t     GetX() { return fX; }
  Float_t     GetY() { return fY; }
  Float_t     GetZ() { return fZ; }
  Float_t     GetU() { return fU; }
  Float_t     GetV() { return fV; }
  Float_t     GetW() { return fW; }
  Float_t     GetR() { return sqrt( fX*fX + fY*fY + fZ*fZ ); }
  Float_t     GetTheta1();
  Float_t     GetEnergy() { return fEnergy; }
  Float_t     GetSigmaR();
  Float_t     GetTime() { return fTime; }
  Int_t       GetNumPMTsUsed() { return fNumPMTsUsed; }
  Float_t     GetQualityOfFit() { return fQualityOfFit; }
  Int_t       GetIndex() { return fIndex; }
  Int_t       GetIntOK() { return (Int_t)fOK; };
  Float_t     GetC11() { return C11; }
  Float_t     GetC12() { return C12; }
  Float_t     GetC13() { return C13; }
  Float_t     GetC14() { return C14; }
  Float_t     GetC22() { return C22; }
  Float_t     GetC23() { return C23; }
  Float_t     GetC24() { return C24; }
  Float_t     GetC33() { return C33; }
  Float_t     GetC34() { return C34; }
  Float_t     GetC44() { return C44; }
  Float_t     GetDstat_ang2d   ()   { return fIndex==9?fFitOutput[PDStatAng2D]:-9999.0; }
  Float_t     GetDstat_ang_phi ()   { return fIndex==9?fFitOutput[PDStatAngPhi]:-9999.0;}
  Float_t     GetDstat_tim     ()   { return fIndex==9?fFitOutput[PDStatTim]:-9999.0;}
  Float_t     GetKSprob_ang2d  ()   { return fIndex==9?fFitOutput[PKSProbAng2D]:-9999;}
  Float_t     GetKSprob_ang_phi()   { return fIndex==9?fFitOutput[PKSProbAngPhi]:-9999;}
  Float_t     GetKSprob_tim    ()   { return fIndex==9?fFitOutput[PKSProbTim]:-9999;}
  Float_t     GetGLike() {return fIndex==7?fFitOutput[GLike]:-9999.0;}
  Float_t     GetGLikeGrid() {return fIndex==7?fFitOutput[GLikeGrid]:-9999.0;}
  Float_t     GetGLikeRatio() {return fIndex==7?fFitOutput[GLikeRatio]:-9999.0;}
  Float_t     GetGRfit() {return fIndex==7?fFitOutput[GRfit]:-9999.0;}
  Float_t     GetGRfit3() {return fIndex==7?fFitOutput[GRfit3]:-9999.0;}
  Float_t     GetGFUsed() {return fIndex==7?fFitOutput[GFUsed]:-9999.0;}
  Float_t     GetGChi2() {return fIndex==7?fFitOutput[GChi2]:-9999.0;}
  Float_t     GetGChi2Nfit() {return fIndex==7?fFitOutput[GChi2Nfit]:-9999.0;}
  Float_t     GetGDiv() {return fIndex==7?fFitOutput[GDiv]:-9999.0;}
  Float_t     GetSol_Dir       ()   {return fSol_Dir; }
  Float_t     *GetBeta         ()   {return fBeta;}
  Float_t     *GetResidualHarmonics(){return fResidualHarmonics;}  
  Float_t     *GetResiduals    ()   {return fResiduals;}  
  Float_t     *GetLegendres    ()   {return fLegendres;}  
  Float_t     Beta14();
  Float_t     GetThetaIJ       ()   { return fThetaIJ; }
  Float_t     GetNeckLikelihood()   {return fNeckLikelihood;}  
  Float_t     GetITR           ()   {return fITR;}
  Float_t     *GetAngularCorrelation(){return fAngularCorrelation;}
  Float_t     *GetFitOutput()         {return fFitOutput;}
  Float_t     GetPhiKSAllHits()const {return fPhiKSAllHits;}
  Float_t     GetPhiKSPromptHits()const {return fPhiKSPromptHits;}
  Float_t     GetAngKSAllHits()const {return fAngKSAllHits;}
  Float_t     GetAngKSPromptHits()const {return fAngKSPromptHits;}
  Float_t     GetChargeThetaIJ()const {return fChargeThetaIJ;}
  Float_t     GetPDTChargeProb()const {return fPDTChargeProb;}
  Float_t     GetPDTMaxCharge()const {return fPDTMaxCharge;}
  Float_t     GetPDTnTubes()const {return fPDTnTubes;}
  Float_t     GetPDTnWindowTubes()const {return fPDTnWindowTubes;}
  Float_t     GetTimeKS()const {return fTimeKS;}
  const char *GetName() const; 
  Bool_t IsOK(){ return fOK;}
  
  void SetX( Float_t x )                  { fX = x; }
  void SetY( Float_t y )                  { fY = y; }
  void SetZ( Float_t z )                  { fZ = z; }
  void SetU( Float_t u )                  { fU = u; }
  void SetV( Float_t v )                  { fV = v; }
  void SetW( Float_t w )                  { fW = w; }
  void SetEnergy(Float_t energy )         { fEnergy = energy;}
  void SetTime( Float_t t )               { fTime = t; }
  void SetThetaIJ(Float_t t)              { fThetaIJ = t; }
  void SetITR( Float_t t )                { fITR = t; }
  void SetNumPMTsUsed( Int_t num )        { fNumPMTsUsed = num; }
  void SetQualityOfFit(Float_t quality)   { fQualityOfFit = quality; }
  void SetName(const char *)          { ; }//fName=name; }
  void SetIndex( Int_t index )            { fIndex = index; }
  void SetOK(Bool_t aOK = kTRUE)          { fOK = aOK;}
  void SetIntOK( Int_t aOK = 1 )          { fOK = aOK; };
  void SetC11(Float_t c )                 { C11 = c; }
  void SetC12(Float_t c )                 { C12 = c; }
  void SetC13(Float_t c )                 { C13 = c; }
  void SetC14(Float_t c )                 { C14 = c; }
  void SetC22(Float_t c )                 { C22 = c; }
  void SetC23(Float_t c )                 { C23 = c; }
  void SetC24(Float_t c )                 { C24 = c; }
  void SetC33(Float_t c )                 { C33 = c; }
  void SetC34(Float_t c )                 { C34 = c; }
  void SetC44(Float_t c )                 { C44 = c; }

	// SNOMAN ( qevent_fill_qevent.for ) actually backfills fFitOutput with -9999 so do the same here :

  void SetFitOutput(Float_t *array)          {FromFTXC(array,fFitOutput,QFIT_MAX_FIT_OUTPUTS,-9999.);}
  void SetSolDir(Float_t c)                  {fSol_Dir=(Float_t)c;}
  void SetBeta(Float_t *array)               {FromFTXC(array, fBeta,QFIT_MAX_BETAS);}
  void SetResidualHarmonics(Float_t *array)  {FromFTXC(array,fResidualHarmonics,QFIT_MAX_RESIDUAL_HARMONICS);}
  void SetResiduals(Float_t *array)          {FromFTXC(array,fResiduals,QFIT_MAX_RESIDUALS);}
  void SetLegendres(Float_t *array)          {FromFTXC(array,fLegendres,QFIT_MAX_LEGENDRES);}
  void SetNeckLikelihood(Float_t c)          {fNeckLikelihood=c;}
  void SetAngularCorrelation(Float_t *array) {FromFTXC(array,fAngularCorrelation,QFIT_MAX_ANG_CORRELATIONS);}
  void SetPhiKSAllHits(Float_t aPhiKSAllHits){fPhiKSAllHits=aPhiKSAllHits;}
  void SetPhiKSPromptHits(Float_t aPhiKSPromptHits){fPhiKSPromptHits=aPhiKSPromptHits;}
 void SetAngKSAllHits(Float_t aAngKSAllHits){fAngKSAllHits=aAngKSAllHits;}
 void SetAngKSPromptHits(Float_t aAngKSPromptHits){fAngKSPromptHits=aAngKSPromptHits;}
 void SetChargeThetaIJ(Float_t aChargeThetaIJ){fChargeThetaIJ=aChargeThetaIJ;}
 void SetPDTChargeProb(Float_t aPDTChargeProb){fPDTChargeProb=aPDTChargeProb;}
 void SetPDTMaxCharge(Float_t aPDTMaxCharge){fPDTMaxCharge=aPDTMaxCharge;}
 void SetPDTnTubes(Float_t aPDTnTubes){fPDTnTubes=aPDTnTubes;}
 void SetPDTnWindowTubes(Float_t aPDTnWindowTubes){fPDTnWindowTubes=aPDTnWindowTubes;}
 void SetTimeKS(Float_t aTimeKS){fTimeKS=aTimeKS;}

 void SetCovar(Int_t ndata, Float_t *data);
 Bool_t IsSame(Int_t anIndex, const QFit *aFit, Int_t aPrintMode=0)const;

private:
  const Int_t *Data()const{return (Int_t *)&fX;}
  Int_t DataN()const{
    Int_t *ptr = (Int_t *)&fTimeKS;
    int i= ptr-Data();
    return i;
  }
  
public:
  ClassDef(QFit,13) // Reconstruced vertex
private:
  Float_t fX;
  Float_t fY;
  Float_t fZ;
  Float_t fU;
  Float_t fV;
  Float_t fW;
  Float_t fEnergy;
  Float_t fTime;
  Int_t   fNumPMTsUsed;
  Float_t fQualityOfFit;
  Int_t   fIndex;
  Bool_t  fOK;
  Int_t fNumIter;
  Float_t C11,C12,C13,C14,C22,C23,C24,C33,C34,C44;  //covariance matrix

  Float_t fFitOutput[QFIT_MAX_FIT_OUTPUTS];  //Fitter outputs that are specific to different fitters

  //    fDstat_ang2d,fDstat_ang_phi, fDstat_tim, fKSprob_ang2d,
  //        fKSprob_ang_phi, fKSprob_tim;

  Float_t fThetaIJ;
	Float_t fITR;
  Float_t fSol_Dir;
  Float_t fNeckLikelihood;
  Float_t fBeta[QFIT_MAX_BETAS];                            // Harmonic parameters in SNOMAN
  Float_t fResidualHarmonics[QFIT_MAX_RESIDUAL_HARMONICS];
  Float_t fResiduals[QFIT_MAX_RESIDUALS];
  Float_t fLegendres[QFIT_MAX_LEGENDRES];
  Float_t fAngularCorrelation[QFIT_MAX_ANG_CORRELATIONS];
  Float_t fPhiKSAllHits; 
  Float_t fPhiKSPromptHits; 
  Float_t fAngKSAllHits; 
  Float_t fAngKSPromptHits;
  Float_t fChargeThetaIJ;  
  Float_t fPDTChargeProb;
  Float_t fPDTMaxCharge;
  Float_t fPDTnTubes;
  Float_t fPDTnWindowTubes;
  Float_t fTimeKS; 
};

#endif

