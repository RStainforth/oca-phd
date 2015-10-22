//* Author: M. Boulay


#ifndef _QRSP_H
#define _QRSP_H

#include "TObject.h"
#include "Rtypes.h"
#include <stdio.h>
#include <math.h>

class QRSP : public TObject {
public:

  QRSP(); 
  QRSP( const QRSP &rsp );
  QRSP &operator=(const QRSP &rhs);
  QRSP(Float_t *, Int_t );
  virtual ~QRSP();
  void Set(Float_t *, Int_t );
  void Set(Float_t *);
 
  Float_t   GetOpticalResponse() { return fOpticalResponse; }
  Float_t   GetNwin() { return fNwin; }
  Float_t   GetNwin2() { return fNwin2; }
  Float_t   GetNdark() { return fNdark; }
  Float_t     GetNeff()                 { return fNeff;          }
  Float_t   GetNhits() { return fNhits; }
  Float_t   GetNwinAllQ() { return fNwinAllQ; }
  Float_t   GetNhitsAllQ() { return fNhitsAllQ; }
  Float_t   GetNhitsDQXX() { return fNhitsDQXX; }
  Float_t   GetNcor() { return fNcor; }
  Float_t   GetNcorMC() { return fNcorMC; }
  
  Float_t     GetNonline()              { return fNonline;       }
  Float_t   GetNcal() { return fNcal; }
  Float_t   GetNefficient() { return fNefficient; }
  Float_t   GetNworking() { return fNworking; }
  Float_t     GetEnergy()               { return fEnergy;        }
  Float_t     GetUncertainty()          { return fUncertainty;   }
  Float_t     GetQuality()              { return fQuality;       }
  Float_t   GetRd2o() { return fRd2o; }
  Float_t   GetRacr() { return fRacr; }
  Float_t   GetRh2o() { return fRh2o; }
  Float_t   GetRfresnel() { return fRfresnel ; }
  Float_t   GetRmpe() { return fRmpe; }
  Float_t   GetRpmtr() { return fRpmtr; }
  Float_t   GetReff() { return fReff; }
  Float_t   GetDrift() { return fDrift; }
  Int_t     GetFitIndex() { return fFitIndex; }
  Float_t   GetNwinPt(){ return fNwinPt; }
  Float_t   GetTshift(){ return fTshift; }
  Float_t   GetPMTResponse() { return fPMTResponse; }
  Float_t   GetNckv()        { return fNckv; }
  Float_t   GetAltEnergy()   { return fAltEnergy; }
  Float_t   GetResolution()  { return fResolution; }
  Float_t   GetFoM()         { return fFoM; }
  Float_t   GetNCDShadCor()  { return fNCDShadCor; }
  Float_t   GetRLambda()     { return fRLambda; }
  Float_t   GetOmega()       { return fOmega; }
  Float_t   GetCKVProb()     { return fCKVProb; }
  Float_t   GetChanEff()     { return fChanEff; }
  Float_t   GetPMTEff()      { return fPMTEff; }
  Float_t   GetMPE()         { return fMPE; }

  void SetOpticalResponse( Float_t r )  { fOpticalResponse = r; }
  void SetNwin           ( Float_t r )  { fNwin = r;            }
  void SetNwin2          ( Float_t r )  { fNwin2 = r;           }
  void SetNdark          ( Float_t r )  { fNdark = r;           }
  void SetNeff           ( Float_t r )  { fNeff = r;            }
  void SetNhits          ( Float_t r )  { fNhits = r;           } 
  void SetNwinAllQ       ( Float_t r )  { fNwinAllQ = r;        } 
  void SetNhitsAllQ      ( Float_t r )  { fNhitsAllQ = r;       } 
  void SetNhitsDQXX      ( Float_t r )  { fNhitsDQXX = r;       } 


  void SetNcor           ( Float_t r )  { fNcor = r;            }
  void SetNcorMC         ( Float_t r)   { fNcorMC = r;          }
  void SetNonline        ( Float_t r)   { fNonline = r;         }
  void SetNcal           ( Float_t r)   { fNcal = r;            }
  void SetNefficient     ( Float_t r)   { fNefficient = r;      }
  void SetNworking       ( Float_t r)   { fNworking = r;        }
  void SetEnergy         ( Float_t r)   { fEnergy = r;          }
  void SetUncertainty    ( Float_t r)   { fUncertainty = r;     }
  void SetQuality        ( Float_t r)   { fQuality = r;         }
  void SetRd2o           ( Float_t r)   { fRd2o = r;            }
  void SetRacr           ( Float_t r)   { fRacr = r;            }
  void SetRh2o           ( Float_t r)   { fRh2o = r;            }
  void SetRfresnel       ( Float_t r)   { fRfresnel = r;        }
  void SetRmpe           ( Float_t r)   { fRmpe = r;            }
  void SetRpmtr          ( Float_t r)   { fRpmtr = r;           }
  void SetReff           ( Float_t r)   { fReff = r;            }
  void SetDrift          ( Float_t r)   { fDrift = r;           }
  void SetFitIndex       ( Int_t i )    { fFitIndex = i;        }
  void SetNwinPt         ( Float_t n)   { fNwinPt   = n;        }
  void SetTshift         ( Float_t t)   { fTshift = t;          }
  void SetPMTResponse    ( Float_t r)   { fPMTResponse = r;     }
  void SetNckv           ( Float_t n)   { fNckv = n;            }
  void SetAltEnergy      ( Float_t e)   { fAltEnergy = e;       }
  void SetResolution     ( Float_t r)   { fResolution = r;      }
  void SetFoM            ( Float_t f)   { fFoM = f;             }
  void SetNCDShadCor     ( Float_t c)   { fNCDShadCor = c;      }
  void SetRLambda        ( Float_t f)   { fRLambda = f;         }
  void SetOmega          ( Float_t f)   { fOmega = f;           }
  void SetCKVProb        ( Float_t f)   { fCKVProb = f;         }
  void SetChanEff        ( Float_t f)   { fChanEff = f;         }
  void SetPMTEff         ( Float_t f)   { fPMTEff = f;          }
  void SetMPE            ( Float_t f)   { fMPE = f;             }

  Bool_t IsSame(Int_t anIndex, const QRSP *aRsp, Int_t aPrintMode=0)const;
 private:
  const Int_t *Data()const {return (Int_t *)&fOpticalResponse;}
  const Int_t DataN()const{int i= ((Int_t *)&fNCDShadCor)-Data();return i;}
 public:

  ClassDef(QRSP,11) // Energy calibrator output

 private:
  Float_t fOpticalResponse; //Optical response
  Float_t fNwin;            //Number of hits in time window.
  Float_t fNwin2;           //Number of hits in time window (2).
  Float_t fNdark;           //Number of dark noise hits.
  Float_t fNeff;            //Effective Nhits. Neff = Nwin-Ndark
  Float_t fNcor;            //Number of corrected hits. Ncor = Neff/response
  Float_t fNcorMC;          //MC scaled corrected hits. NcorMC = Ncor x MCScale
  Float_t fNonline;         //Number of PMTs online for this event.
  Float_t fNcal;            //Number of calibrateable tubes for this event.
  Float_t fNefficient;      //Number of Quantumly efficient tubes for this event.
  Float_t fNworking;        //Number of working tubes = online & cal & efficient
  Float_t fEnergy;          //Equivalent electron energy.
  Float_t fUncertainty;     //Uncertainty in electron energy..
  Float_t fQuality;         //Quality word for this energy calibration.
  Float_t fRd2o;            //D2O contribution to response
  Float_t fRacr;            //Acrylic contribution to response
  Float_t fRh2o;            //H2O contribution to response
  Float_t fRfresnel;        //Fresnel contribution to response
  Float_t fRmpe;            //Multiphoton contribution to response
  Float_t fRpmtr;           //PMT angular response contribution to response
  Float_t fReff;            //Relative PMT efficiency contribution to response
  Float_t fDrift;           //Energy scale drift
  Float_t fNhits;           //Number of in-time hits from working tubes.
  Int_t   fFitIndex;        //Fitter index used for vertex.
  Float_t fNwinAllQ;        //In-time hits w/o charge threshold.
  Float_t fNhitsAllQ;       //All hits w/o charge threshold for working tubes.
  Float_t fNhitsDQXX;       //All hits for online channels.
  Float_t fNwinPt;          //Nwin for KPMT_PT
  Float_t fTshift;          //Time offset due to walk correction.
  Float_t fPMTResponse;     //Predicted number of initially generated photons.
  Float_t fNckv;            //Estimated number of Cerenkov photons.
  Float_t fAltEnergy;       //Alternative energy.
  Float_t fResolution;      //Dummy for now.
  Float_t fFoM;             //Dummy for now.
  Float_t fNCDShadCor;      //Dummy for now.
  Float_t fRLambda;
  Float_t fOmega;
  Float_t fCKVProb;
  Float_t fChanEff;
  Float_t fPMTEff;
  Float_t fMPE;
};

#endif
