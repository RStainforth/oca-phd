///////////////////////////////////////////////////////////////
//QMCVX                                                      //
//                                                           //
//Structure for storing and manipulating MCO vertices and    //
//tracks.                                                    //
///////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay                   

#include "QMCVX.h"
#include <stdlib.h>

ClassImp(QMCVX)
  
  QMCVX::QMCVX(Double_t mctime,Float_t *xvars, Int_t aBits ) {
  //Normal QMCVX constructor. Fills MCVX data
  //as follows:
  //Begin_html
  
  //<l>fPosition[0,1,2]  = xvars[0,1,2];
  //<l>fDirection[0,1,2] = xvars[3,4,5];
  //<l>fEnergy           = xvars[6];
  //<l>fIDP              = (Int_t)xvars[7];
  //<l>fTime             = mctime;
  //<l>fINC      = (Int_t)xvars[8];
  //<l>fIndex            = aBits;
  //<l>fCLS      = (Int_t)xvars[10];
  //<l>fRGN      = (Int_t)xvars[11];
  //<l>fIDM      = (Int_t)xvars[12];
  //<l>fRG2      = (Int_t)xvars[13];
  //<l>fIM2      = (Int_t)xvars[14];
  //<l>fNcerD2O, H2O, ACR = (Int_t)xvars[15-17]
  //End_html
  Initialize();
  Set(mctime,xvars, aBits);
  
}

QMCVX::QMCVX() {
  Initialize();
}

QMCVX::~QMCVX() {
}

QMCVX::QMCVX( const QMCVX &mcvx ):TObject(mcvx)
{
  //Copy constructor for QMCVX objects. 
  
  fX = mcvx.fX;
  fY = mcvx.fY;
  fZ = mcvx.fZ;
  fU = mcvx.fU;
  fV = mcvx.fV;
  fW = mcvx.fW;
  fEnergy = mcvx.fEnergy;
  fTime = mcvx.fTime;
  fINC = mcvx.fINC;
  fIDP  = mcvx.fIDP;
  fIndex = mcvx.fIndex;
  fCLS   = mcvx.fCLS;
  fRGN   = mcvx.fRGN;
  fIDM   = mcvx.fIDM;
  fRG2   = mcvx.fRG2;
  fIM2   = mcvx.fIM2;
  fNCerD2O = mcvx.fNCerD2O;
  fNCerH2O = mcvx.fNCerH2O;
  fNCerACR = mcvx.fNCerACR;
  
  
}

QMCVX & QMCVX::operator=(const QMCVX &rhs )
{
  // QMCVX assignment operator.
  
  //--MB
  fX = rhs.fX;              
  fY = rhs.fY;                   
  fZ = rhs.fZ;              
  fU = rhs.fU;                  
  fV = rhs.fV;             
  fW = rhs.fW;             
  fEnergy = rhs.fEnergy;  
  fTime = rhs.fTime;        
  fINC = rhs.fINC;         
  fIDP   = rhs.fIDP;            
  fIndex = rhs.fIndex;
  fCLS   = rhs.fCLS;
  fRGN   = rhs.fRGN;
  fIDM   = rhs.fIDM;
  fRG2   = rhs.fRG2;
  fIM2   = rhs.fIM2;
  fNCerD2O = rhs.fNCerD2O;
  fNCerH2O = rhs.fNCerH2O;
  fNCerACR = rhs.fNCerACR;
  return *this;
}

void QMCVX::Set(Double_t mctime,Float_t* xvars, Int_t aBits) {
  //Set this QMCVX to the data given.
  fX  = xvars[0];                      //vertex x
  fY  = xvars[1];                      //vertex y
  fZ  = xvars[2];                      //vertex z
  fU  = xvars[3];                      //track x direction cosine
  fV  = xvars[4];                      //track y direction cosine
  fW  = xvars[5];                      //track z direction cosine
  fEnergy       = xvars[6];            //energy for outgoing track 
  fIDP          = (Int_t)xvars[7];     //outgoing track particle ID
  fTime         = mctime;              //time in nsec
  fINC  = (Int_t)xvars[8];             //vertex interaction code
  fIndex         = aBits;              //vertex index
  fCLS  = (Int_t)xvars[10];            //vertex class 
  fRGN  = (Int_t)xvars[11];            //region code
  fIDM  = (Int_t)xvars[12];            //media code
  fRG2  = (Int_t)xvars[13];            //second region code (if boundary) 
  fIM2  = (Int_t)xvars[14];            //second media code (if boundary) 
  fNCerD2O = (Int_t)xvars[15];         //# of Cerenkov photons in D2O 
  fNCerH2O = (Int_t)xvars[16];         //# of Cerenkov photons in H20
  fNCerACR = (Int_t)xvars[17];         //# of Cerenkov photons in AV       
}

  
 

void QMCVX::Set(Int_t* mcvx, Int_t *mctk, Int_t anIndex) {
  //Set this QMCVX to the data given- decode the SNOMAN memory here
  Float_t *fptr=(Float_t *)mcvx;
  fCLS = mcvx[0];
  fINC = mcvx[1];
  fX  = fptr[2];                      //vertex x
  fY  = fptr[3];                      //vertex y
  fZ  = fptr[4];                      //vertex z
  Double_t *dptr; dptr=(Double_t*)(fptr+5);
  fTime= *dptr;
  //Bug/feature:  fTime is identical to MCVX_TIM here; includes
  //  MC_GTR when called from SNOMAN.  This way is probably best for
  // MCEvent trees.
    
  fRGN  = mcvx[7];            //region code
  fIDM  = mcvx[8];            //media code
  fRG2  = mcvx[9];            //second region code (if boundary) 
  fIM2  = mcvx[10];            //second media code (if boundary) 
  fNCerD2O = 0;         //# of Cerenkov photons in D2O 
  fNCerH2O = 0;         //# of Cerenkov photons in H20
  fNCerACR = 0;         //# of Cerenkov photons in AV       

  // get info from tracks
  if(mctk){
    Float_t *ft=(Float_t *)mctk;
    fU  = ft[1];                      //track x direction cosine
    fV  = ft[2];                      //track y direction cosine
    fW  = ft[3];                      //track z direction cosine
    fEnergy       = ft[4];            //energy for outgoing track 
    fIDP          = mctk[0];           //outgoing track particle ID
  }else{
    fU=fV=fW=fEnergy=0;  fIDP=0;
  }
  fIndex         = anIndex;              //vertex index
  
}


Bool_t QMCVX::IsSame(Int_t anIndex, const QMCVX *aMcvx, Int_t aPrintMode)const{
  const Int_t *data=Data();
  const Int_t *data2=aMcvx->Data();
  int i,j;
  Int_t n=DataN();
  for(j=i=0;i<=n;i++)if(data[i]!=data2[i])j++;
  if(j>0 &&aPrintMode==1){
    printf("QMCVX:%d  Differences in words ",anIndex);
    for(i=0;i<=n;i++)printf(" %d",i);
    printf("\n");
  }
  return (j==0);
}

void QMCVX::Initialize() {
  
}










