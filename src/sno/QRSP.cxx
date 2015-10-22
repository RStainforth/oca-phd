//////////////////////////////////////////////////////////////////
//QRSP                                                          //
//                                                              //
//The QRSP bank contains the output of the SNOMAN RSP processor //
//used for energy calibration.                                  //
//////////////////////////////////////////////////////////////////

//*-- Author : M. Boulay
                        
#include "QRSP.h"
#include <stdlib.h>

ClassImp(QRSP)

QRSP::QRSP(Float_t *rvars, Int_t ifitter) {
  //Normal QRSP constructor. 
  //ifitter is the fitter index (10 for QPDF)
  //Fills the QRSP bank as follows:
  //Begin_html
  //<l>fNeffMode     =  ivars[0] </l>
  //<l>fResponseMode =  ivars[1] </l>
  //<l>fDefMode      =  ivars[2] </l>
  //<l>fNeff         =  rvars[0] </l>
  //<l>fResponse     =  rvars[1] </l>
  //<l>fDef          =  rvars[2] </l>
  //<l>fNonline      =  rvars[3] </l>
  //<l>fNoise        =  rvars[4] </l>
  //<l>fEnergy       =  rvars[5] </l>
  //<l>fUncertainty  =  rvars[6] </l>
  //<l>fQuality      =  rvars[7] </l>
  //End_html
 
  Set(rvars, ifitter);
}

QRSP::QRSP() 
{
  //Default constructor.
}


QRSP::~QRSP() 
{
  //Destructor.
}

QRSP::QRSP( const QRSP &rsp )
{
  //Copy constructor for QRSP objects. 
  //Variable values are taken from the rvars array.
  fOpticalResponse = rsp.fOpticalResponse;
  fNwin            = rsp.fNwin;
  fNwin2           = rsp.fNwin2;
  fNdark           = rsp.fNdark;
  fNeff            = rsp.fNeff;
  fNcor            = rsp.fNcor;
  fNcorMC          = rsp.fNcorMC;
  fNonline         = rsp.fNonline;
  fNcal            = rsp.fNcal;
  fNefficient      = rsp.fNefficient;
  fNworking        = rsp.fNworking;
  fEnergy          = rsp.fEnergy;
  fUncertainty     = rsp.fUncertainty;
  fQuality         = rsp.fQuality;
  fRd2o            = rsp.fRd2o;
  fRacr            = rsp.fRacr;
  fRh2o            = rsp.fRh2o; 
  fRfresnel        = rsp.fRfresnel;
  fRmpe            = rsp.fRmpe;
  fRpmtr           = rsp.fRpmtr;
  fReff            = rsp.fReff;
  fDrift           = rsp.fDrift;
  fNhits           = rsp.fNhits;
  fFitIndex        = rsp.fFitIndex;
  fNwinAllQ        = rsp.fNwinAllQ;
  fNhitsAllQ       = rsp.fNhitsAllQ;
  fNhitsDQXX       = rsp.fNhitsDQXX;
  fNwinPt          = rsp.fNwinPt;
  fTshift          = rsp.fTshift;
  fPMTResponse     = rsp.fPMTResponse;
  fAltEnergy       = rsp.fAltEnergy;
  fNckv            = rsp.fNckv;
  fResolution      = rsp.fResolution;
  fFoM             = rsp.fFoM;
  fNCDShadCor      = rsp.fNCDShadCor;
  fRLambda         = rsp.fRLambda;
  fOmega           = rsp.fOmega;
  fCKVProb         = rsp.fCKVProb;
  fChanEff         = rsp.fChanEff;
  fPMTEff          = rsp.fPMTEff;
  fMPE             = rsp.fMPE;
}

QRSP & QRSP::operator=(const QRSP &rhs )
{
  // QRSP assignment operator.

  fOpticalResponse = rhs.fOpticalResponse;
  fNwin            = rhs.fNwin;
  fNwin2           = rhs.fNwin2;
  fNdark           = rhs.fNdark;
  fNeff            = rhs.fNeff;
  fNcor            = rhs.fNcor;
  fNcorMC          = rhs.fNcorMC;
  fNonline         = rhs.fNonline;
  fNcal            = rhs.fNcal;
  fNefficient      = rhs.fNefficient;
  fNworking        = rhs.fNworking;
  fEnergy          = rhs.fEnergy;
  fUncertainty     = rhs.fUncertainty;
  fQuality         = rhs.fQuality;
  fRd2o            = rhs.fRd2o;
  fRacr            = rhs.fRacr;
  fRh2o            = rhs.fRh2o; 
  fRfresnel        = rhs.fRfresnel;
  fRmpe            = rhs.fRmpe;
  fRpmtr           = rhs.fRpmtr;
  fReff            = rhs.fReff;
  fDrift           = rhs.fDrift;
  fNhits           = rhs.fNhits;
  fFitIndex        = rhs.fFitIndex;
  fNwinAllQ        = rhs.fNwinAllQ;
  fNhitsAllQ       = rhs.fNhitsAllQ;
  fNhitsDQXX       = rhs.fNhitsDQXX;
  fNwinPt          = rhs.fNwinPt;
  fTshift          = rhs.fTshift;
  fPMTResponse     = rhs.fPMTResponse;
  fAltEnergy       = rhs.fAltEnergy;
  fNckv            = rhs.fNckv;
  fResolution      = rhs.fResolution;
  fFoM             = rhs.fFoM;
  fNCDShadCor      = rhs.fNCDShadCor;
  fRLambda         = rhs.fRLambda;
  fOmega           = rhs.fOmega;
  fCKVProb         = rhs.fCKVProb;
  fChanEff         = rhs.fChanEff;
  fPMTEff          = rhs.fPMTEff;
  fMPE             = rhs.fMPE;

  return *this;
}


void QRSP::Set(Float_t *rvars, Int_t findex) 
{
  //Fill the data members of this QRSP.
   
  fOpticalResponse = rvars[0];
  fNwin            = rvars[1];
  fNwin2           = rvars[2];
  fNdark           = rvars[3];
  fNeff            = rvars[4];
  fNcor            = rvars[5];
  fNcorMC          = rvars[6];
  fNonline         = rvars[7];
  fNcal            = rvars[8];
  fNefficient      = rvars[9];
  fNworking        = rvars[10];
  fEnergy          = rvars[11];
  fUncertainty     = rvars[12];
  fQuality         = rvars[13]; 
  fRd2o            = rvars[14]; 
  fRacr            = rvars[15]; 
  fRh2o            = rvars[16]; 
  fRfresnel        = rvars[17]; 
  fRmpe            = rvars[18]; 
  fRpmtr           = rvars[19]; 
  fReff            = rvars[20]; 
  fDrift           = rvars[21];
  fNhits           = rvars[22];
  fFitIndex        = findex;
  fNwinAllQ        = rvars[23];
  fNhitsAllQ       = rvars[24];
  fNhitsDQXX       = rvars[25];
  fNwinPt          = rvars[26];
  fTshift          = rvars[27];
  fPMTResponse     = rvars[28];
  fAltEnergy       = rvars[29];
  fNckv            = rvars[30];
  fResolution      = rvars[31];
  fFoM             = rvars[32];
  fNCDShadCor      = rvars[33];
  fRLambda         = rvars[34];
  fOmega           = rvars[35];
  fCKVProb         = rvars[36];
  fChanEff         = rvars[37];
  fPMTEff          = rvars[38];
  fMPE             = rvars[39];
}
void QRSP::Set(Float_t *rvars) 
{
  //Fill the data members of this QRSP.  This version
  // assumes that rvars are copied directly from the RSP bank in SNOMAN.
  fOpticalResponse = rvars[0];
  fNwin            = rvars[1];
  fNwin2           = rvars[2];
  fNdark           = rvars[3];
  fNeff            = rvars[4];
  fNcor            = rvars[5];
  fNcorMC          = rvars[6];
  fNonline         = rvars[7];
  fNcal            = rvars[8];
  fNefficient      = rvars[9];
  fNworking        = rvars[10];
  fEnergy          = rvars[11];
  fUncertainty     = rvars[12];
  fQuality         = rvars[13]; 
  fRd2o            = rvars[14]; 
  fRacr            = rvars[15]; 
  fRh2o            = rvars[16]; 
  fRfresnel        = rvars[17]; 
  fRmpe            = rvars[18]; 
  fRpmtr           = rvars[19]; 
  fReff            = rvars[20]; 
  fDrift           = rvars[21];
  fNhits           = rvars[22];
  fFitIndex        = *(int *)(rvars+23);
  fNwinAllQ        = rvars[24];
  fNhitsAllQ       = rvars[25];
  fNhitsDQXX       = rvars[26];
  fNwinPt          = rvars[27];
  fTshift          = rvars[28];
  fPMTResponse     = rvars[29];
  fAltEnergy       = rvars[30];
  fNckv            = rvars[31];
  fResolution      = rvars[32];
  fFoM             = rvars[33];
  fNCDShadCor      = rvars[34];
  fRLambda         = rvars[35];
  fOmega           = rvars[36];
  fCKVProb         = rvars[37];
  fChanEff         = rvars[38];
  fPMTEff          = rvars[39];
  fMPE             = rvars[40];
}

Bool_t QRSP::IsSame(Int_t anIndex, const QRSP *aRsp, Int_t aPrintMode)const{
  const Int_t *data=Data();
  const Int_t *data2=aRsp->Data();
  const Float_t *fdata=(Float_t *)data;
  const Float_t *fdata2=(Float_t *)data2;

  int i,j;
  Int_t n=DataN();
  for(j=i=0;i<n;i++)if(data[i]!=data2[i])j++;
  if(j>0 &&aPrintMode==1){
    printf("QRSP:%d  Differences in words ", anIndex);
    for(i=0;i<=n;i++)if(data[i]!=data2[i])printf("Entry %d Values: 0x%x 0x%x, as floating point %f %f\n",i,data[i],data2[i],fdata[i],fdata2[i]);
    printf("\n");
  }

  return (j==0);
}
 

void QRSP::Streamer(TBuffer &R__b)
{
   // Stream an object of class QRSP.
   // Reads from and to TBuffer object. 
   // TBuffer class used to serialize objects, see  http://root.cern.ch/root/html/TBuffer.html.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      if ( R__v == 1 )
	{
	  //old RSP version
	  Int_t ionline;
	  Float_t fd;
	  Int_t   id;
      	  R__b >> id;
      	  R__b >> ionline;
	  fNonline = Float_t(ionline);
      	  R__b >> fd;
      	  R__b >> fd;
      	  R__b >> fd;
      	  R__b >> fd;
      	  R__b >> fd;
      	  R__b >> fd;
 	}
      else if ( R__v == 2 )
	{
	  Int_t id;
	  Float_t fd;
	  R__b >> id;
	  R__b >> id;
	  R__b >> id;
          R__b >> fd;
          R__b >> fd;
	  R__b >> fd;
	  R__b >> fd;
	  R__b >> fd;
	  R__b >> fd;
	  R__b >> fd;
	  R__b >> fd;
	}
      else
	{   
	  R__b >> fOpticalResponse;
	  R__b >> fNwin;           
	  R__b >> fNwin2;          
	  R__b >> fNdark;          
	  R__b >> fNeff;           
	  R__b >> fNcor;           
	  R__b >> fNcorMC;         
	  R__b >> fNonline;        
	  R__b >> fNcal;           
	  R__b >> fNefficient;     
	  R__b >> fNworking;       
	  R__b >> fEnergy;         
	  R__b >> fUncertainty;    
	  R__b >> fQuality;        
	  R__b >> fRd2o;           
	  R__b >> fRacr;           
	  R__b >> fRh2o;           
	  R__b >> fRfresnel;       
	  R__b >> fRmpe;           
	  R__b >> fRpmtr;          
	  R__b >> fReff;           
	}
      if ( R__v > 3 )
	{
	  R__b >> fDrift;
	}
      else { fDrift = 0;}
      if ( R__v > 4  )
	{
	  R__b >> fNhits;
	}
      else { fNhits = 0; } 
      if ( R__v > 5 ) 
	{
	  R__b >> fFitIndex;
	}
      else
	{
	  fFitIndex = 0;
	}
      if ( R__v > 6 ) 
	{
	  R__b >> fNwinAllQ;
	  R__b >> fNhitsAllQ;
	  R__b >> fNhitsDQXX;
	}
      else
	{
	  fNwinAllQ = 0;
	  fNhitsAllQ = 0;
	  fNhitsDQXX=0;
	}
      if ( R__v > 7 )
	{
	  R__b >> fNwinPt;
	  R__b >> fTshift;
	}
      else
	{
	  fNwinPt = 0.;
	  fTshift = 0.;
	}
      if ( R__v > 8 )
        {
          R__b >> fPMTResponse;
          R__b >> fAltEnergy;
        }
      else
        {
          fPMTResponse = 0.0;
          fAltEnergy = 0.0;
        }
      if ( R__v > 9 )
        {
          R__b >> fNckv;
          R__b >> fResolution;
          R__b >> fFoM;
          R__b >> fNCDShadCor;
        }
      else
        {
          fNckv = 0.0;
          fResolution = 0.0;
          fFoM = 0.0;
          fNCDShadCor = 0.0;
        }
      if ( R__v > 10 )
        {
          R__b >> fRLambda;
          R__b >> fOmega;
          R__b >> fCKVProb;
          R__b >> fChanEff;
          R__b >> fPMTEff;
          R__b >> fMPE;
        }
      else
        {
          fRLambda=0.;
          fOmega=0.;
          fCKVProb=0.;
          fChanEff=0.;
          fPMTEff=0.;
          fMPE=0.;
        }

   } else {
      R__b.WriteVersion(QRSP::IsA());
      TObject::Streamer(R__b);  
	  R__b << fOpticalResponse;
	  R__b << fNwin;           
	  R__b << fNwin2;          
	  R__b << fNdark;          
	  R__b << fNeff;           
	  R__b << fNcor;           
	  R__b << fNcorMC;         
	  R__b << fNonline;        
	  R__b << fNcal;           
	  R__b << fNefficient;     
	  R__b << fNworking;       
	  R__b << fEnergy;         
	  R__b << fUncertainty;    
	  R__b << fQuality;        
	  R__b << fRd2o;           
	  R__b << fRacr;           
	  R__b << fRh2o;           
	  R__b << fRfresnel;       
	  R__b << fRmpe;           
	  R__b << fRpmtr;          
	  R__b << fReff;           
	  R__b << fDrift;
	  R__b << fNhits;
	  R__b << fFitIndex;
	  R__b << fNwinAllQ;
	  R__b << fNhitsAllQ;
	  R__b << fNhitsDQXX;
	  R__b << fNwinPt;
	  R__b << fTshift;
	  R__b << fPMTResponse;
	  R__b << fAltEnergy;
	  R__b << fNckv;
	  R__b << fResolution;
	  R__b << fFoM;
	  R__b << fNCDShadCor;
          R__b << fRLambda;
          R__b << fOmega;
          R__b << fCKVProb;
          R__b << fChanEff;
          R__b << fPMTEff;
          R__b << fMPE;
   }
}
