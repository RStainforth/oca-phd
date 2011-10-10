//* Author: A. Hallin


#ifndef _QFTK_H
#define _QFTK_H
#include "TObject.h"

class QFTK : public TObject {
 public:

  QFTK(); 
  QFTK( const QFTK &anFTK );
  QFTK &operator=(const QFTK &rhs);
  QFTK(Float_t *, Int_t );
  virtual ~QFTK();
  void Set(Float_t *, Int_t );
  Bool_t IsSame(Int_t anIndex, const QFTK *aRsp, Int_t aPrintMode=0)const;

  Int_t GetFitIndex()const {return fFitIndex;}
  Float_t GetProb()const {return fProb;}
  Float_t GetEnergy()const {return fEnergy;}
  Float_t GetNegativeUncertainty()const {return fNegativeUncertainty;}
  Float_t GetPositiveUncertainty()const {return fPositiveUncertainty;}
  Float_t GetNEff()const {return fNEff;}
  Float_t GetDirectHitProbability()const {return fDirectHitProbability;}
  Float_t GetMeanMPE()const {return fMeanMPE;}
  Float_t GetScatteredHitProbability()const {return fScatteredHitProbability;}
  Float_t GetPMTReflectedHitProbability()const {return fPMTReflectedHitProbability;}
  Float_t GetAV1HitProbability()const {return fAV1HitProbability;}
  Float_t GetAV2HitProbability()const {return fAV2HitProbability;}

  void SetFitIndex(Int_t aFitIndex){fFitIndex=aFitIndex;}
  void SetProb(Float_t aProb){fProb=aProb;}
  void SetEnergy(Float_t aEnergy){fEnergy=aEnergy;}
  void SetNegativeUncertainty(Float_t aNegativeUncertainty){fNegativeUncertainty=aNegativeUncertainty;}
  void SetPositiveUncertainty(Float_t aPositiveUncertainty){fPositiveUncertainty=aPositiveUncertainty;}
  void SetNEff(Float_t aNEff){fNEff=aNEff;}
  void SetDirectHitProbability(Float_t aDirectHitProbability){fDirectHitProbability=aDirectHitProbability;}
  void SetMeanMPE(Float_t aMeanMPE){fMeanMPE=aMeanMPE;}
  void SetScatteredHitProbability(Float_t aScatteredHitProbability){fScatteredHitProbability=aScatteredHitProbability;}
  void SetPMTReflectedHitProbability(Float_t aPMTReflectedHitProbability){fPMTReflectedHitProbability=aPMTReflectedHitProbability;}
  void SetAV1HitProbability(Float_t aAV1HitProbability){fAV1HitProbability=aAV1HitProbability;}
  void SetAV2HitProbability(Float_t aAV2HitProbability){fAV2HitProbability=aAV2HitProbability;}


 private:
  const Int_t *Data()const {return (Int_t *)&fFitIndex;}
  const Int_t DataN()const{int i= ((Int_t *)&fAV2HitProbability)-Data();return i;}
 public:

  ClassDef(QFTK,1) // Energy calibrator output

 private:
  Int_t fFitIndex;              // Fitter index used for FTK
  Float_t fProb;                // LH value at the best fit point
  Float_t fEnergy;              // Total Energy of the event
  Float_t fNegativeUncertainty; // Lower uncertainty on the Energy (in MeV)
  Float_t fPositiveUncertainty; // Upper uncertainty on the Energy (in MeV)
  Float_t fNEff;                // Number of hits inside the FTK time cut 
  Float_t fDirectHitProbability; // Probability of a direct photon being detected
  Float_t fMeanMPE;              // MPE correction term at the mean of the cerenkov photons distribution
    Float_t fScatteredHitProbability;    // Probability of a scattered photon being detected
  Float_t fPMTReflectedHitProbability;   // Probability of detecting a PMT reflection photon
  Float_t fAV1HitProbability;            // Probability of detecting a AV reflection photon which reflects off the first AV boundary
  Float_t fAV2HitProbability;            //   Probability of detecting a AV reflection photon which reflects off the second AV   
};

#endif
