//////////////////////////////////////////////////////////////////
//QFTK                                                          //
//                                                              //
//The QFTK bank contains the output of the SNOMAN FTK processor //
//used for energy calibration.  See Snoman companion/Monica     //
//Dunford for details.                                          //
//////////////////////////////////////////////////////////////////

//*-- Author : A. Hallin
                        
#include "QFTK.h"
#include <stdlib.h>

ClassImp(QFTK)

QFTK::QFTK(Float_t *rvars, Int_t ifitter) {
  //Normal QFTK constructor. 
  //ifitter is the fitter index (10 for QPDF)
  //Fills the QFTK bank from the SNOMAN FTXK structure.  
 
  Set(rvars, ifitter);
}

QFTK::QFTK() 
{
  //Default constructor.
}


QFTK::~QFTK() 
{
  //Destructor.
}

QFTK::QFTK( const QFTK &ftk ):TObject(ftk)
{
  //Copy constructor for QFTK objects. 
  *this=ftk;
}

QFTK & QFTK::operator=(const QFTK &rhs )
{
  // QFTK assignment operator.
  fFitIndex            = rhs.fFitIndex;
  fProb                = rhs.fProb;            
  fEnergy              = rhs.fEnergy;
  fNegativeUncertainty            = rhs.fNegativeUncertainty;
  fPositiveUncertainty            = rhs.fPositiveUncertainty;
  fNEff                = rhs.fNEff;
  fDirectHitProbability         = rhs.fDirectHitProbability;
  fMeanMPE       = rhs.fMeanMPE;
  fScatteredHitProbability      = rhs.fScatteredHitProbability;
  fPMTReflectedHitProbability      = rhs.fPMTReflectedHitProbability;
  fAV1HitProbability   = rhs.fAV1HitProbability;
  fAV2HitProbability   = rhs.fAV2HitProbability;
  return *this;
}


void QFTK::Set(Float_t *rvars, Int_t findex) 
{
  //Fill the data members of this QFTK.
  fFitIndex        = findex;
  fProb                = rvars[0];            
  fEnergy              = rvars[1];
  fNegativeUncertainty            = rvars[2];
  fPositiveUncertainty            = rvars[3];
  fNEff                = rvars[4];
  fDirectHitProbability         = rvars[5];
  fMeanMPE       = rvars[6];
  fScatteredHitProbability      = rvars[7];
  fPMTReflectedHitProbability      = rvars[8];
  fAV1HitProbability   = rvars[9];
  fAV2HitProbability   = rvars[10];
}

Bool_t QFTK::IsSame(Int_t anIndex, const QFTK *anFTK, Int_t aPrintMode)const{
  const Int_t *data=Data();
  const Int_t *data2=anFTK->Data();
  const Float_t *fdata=(Float_t *)data;
  const Float_t *fdata2=(Float_t *)data2;

  int i,j;
  Int_t n=DataN();
  for(j=i=0;i<n;i++)if(data[i]!=data2[i])j++;
  if(j>0 &&aPrintMode==1){
    printf("QFTK:%d  Differences in words ", anIndex);
    for(i=0;i<=n;i++)if(data[i]!=data2[i])printf("Entry %d Values: %x %x, as floating point %f %f\n",i,data[i],data2[i],fdata[i],fdata2[i]);
    printf("\n");
  }

  return (j==0);
}
 
