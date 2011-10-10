#include "QMuonFit.h"
//
// QMuonFit.h
//
  
ClassImp(QMuonFit)

QMuonFit::QMuonFit(Int_t * aFit, Int_t *aVertex, Int_t *aTrack, Int_t *aMes,
		   Int_t &aFitter){
  //Simply copy over the Snoman banks to the fit structure. 
  Set(aFit,aVertex,aTrack,aMes,aFitter);
}


void QMuonFit::Set(Int_t * aFit, Int_t *aVertex, Int_t *aTrack, Int_t *aMes,
		   Int_t &aFitter){

  int i;
  Float_t *fptr;
  Int_t *ptr=(Int_t *)&fX;
  
  fIndex=aFitter;

  if(fIndex==-1){ //ftrm data
    for(fptr=&fX;fptr<=&fRawQRMS;fptr++)*fptr=-999999; //initialize data
    fReturnCode=fPMTAvailable=fPMTUsed=fEarlyHits=fLateHits=fIterations=fProbability=fMcMuMask=fMcPartMask=0;
    fIndex==-1;
    fptr=(float *)aFit;
    fU=fptr[1];
    fV=fptr[2];
    fW=fptr[3];
    fX=fptr[4];
    fY=fptr[5];
    fZ=fptr[6];
    fDeltaX=fptr[7];
    fDeltaY=fptr[8];
    fDeltaZ=fptr[9];
    fExitX=fptr[10];
    fExitY=fptr[11];
    fExitZ=fptr[12];
    fMcXStop=fptr[13];
    fMcYStop=fptr[14];
    fMcZStop=fptr[15];
    fMcEnergy=fptr[16];
    fMcP=fptr[17];
    fMcMuMask=aFit[18];
    fMcPartMask=aFit[19];
    fdEdX=fptr[20];
    fImpact=fptr[21];
    
  }else{
    for(i=0;i<8;i++)ptr[i]=aVertex[i];

    ptr=(Int_t *)&fU;
    for(i=0;i<6;i++)ptr[i]=aTrack[i];

    if(aFitter==8){
      ptr=&fReturnCode-1;
      for(i=1;i<56;i++)ptr[i]=aFit[i];
    }else{
      fptr=(Float_t *)&fReturnCode-1;
      for(i=1;i<56;i++)fptr[i]=-999999.0;
    }

    if(aFitter==13 || aFitter== 15 || aFitter==16){ //fTI, FTY,fTZ
      fptr=(Float_t *)aFit;
      fNegativeLogLikelyhood=fptr[14];
      fSingleToMultiphotonRatio=fptr[21];
    }else{
      fNegativeLogLikelyhood=-999999;
      fSingleToMultiphotonRatio=-999999;
    }

    //Fill in MES variables
    ptr=(Int_t *)&fDetectedNPE-1;
    if(aMes[2]<1){
      for(i=1;i<22;i++)ptr[i]=-999999;
    }else{
      for(i=1;i<22;i++)ptr[i]=aMes[i];
    }    
  }  
}
