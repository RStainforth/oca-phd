#ifndef _QPMTxyz_h
#define _QPMTxyz_h

#include <stdio.h>
#include <Rtypes.h>
#include <TObject.h>
#include <stdlib.h>
#include <string.h>
#include <TFile.h>
#include "QGlobals.h"
#include "QDQXX.h"
#include "QCHCS.h"
#include "TVector3.h"

//*-- Author : Mark Boulay                          
//* Copyright (c) 1998 Mark Boulay      

class QSnoDB;
class QBank;

class QPMTxyz : public TObject
{
 public:
   QPMTxyz(const char *filename = NULL);
  ~QPMTxyz();
  Int_t GetPmtn( Int_t iCrate, Int_t iCard, Int_t iChannel)
  {
    //Return the logical Pmtn for a given Crate, Card, Channel.  Pmtn = 512*Crate + 32*Card +Channel.
    return 512*iCrate + 32*iCard + iChannel;
  }
  Float_t GetX( int lpmt );
  Float_t GetY( int lpmt );
  Float_t GetZ( int lpmt );
  TVector3 GetXYZ( int lpmt);  
  Int_t   GetCrate( Int_t lpmt );
  Int_t   GetCard ( Int_t lpmt );
  Int_t   GetChannel( Int_t lpmt );
  Int_t   GetSnomanNo( Int_t lpmt );
  Int_t   GetQueensNo( Int_t lpmt );
  Int_t   GetCCC( Int_t lpmt );
  Bool_t  Is75Blown( Int_t lpmt );
  Int_t   GetType( Int_t lpmt );
  Int_t GetPanel(Int_t lpmt);
  Bool_t  IsNormalPMT( Int_t lpmt );
  Bool_t  IsOWLPMT( Int_t lpmt );
  Bool_t  IsLGPMT(  Int_t lpmt );
  Bool_t  IsCalibrationPMT( Int_t lpmt );
  Bool_t  IsSparePMT( Int_t lpmt );
  Bool_t  IsInvalidPMT( Int_t lpmt );
  Bool_t  IsBatch3Concentrator(Int_t lpmt);
  Int_t   ReadConstants(  const char *filename = NULL, const int idate=20040923,const int itime=100222 );
  Int_t   WriteConstants( const char *filename = NULL );
  Int_t   ReadDQXX( const char *filename = NULL);
  QDQXX *  GetDQXX() { return fDQXX; }
  Bool_t  IsChannelOn(Int_t lcn);
  Bool_t  IsTubeOn(Int_t lcn);
  Int_t   GetThreshold( Int_t lcn );
  Int_t   GetHV(Int_t lcn );
  Int_t   ReadCHCS( const char *filename = NULL, Int_t date = 0, Int_t time = 0);
  Int_t   ReadCHCS( const char *filename, Int_t jd, Int_t ut1, Int_t ut2);
  QCHCS *  GetCHCS() { return fCHCS; }
  Int_t   IsTubeCalStatGood( Int_t lcn );
  void    Remap(const char *filename);
  ClassDef(QPMTxyz,0)//PMT coordinates
  private:

  void Initialize();
  
  Float_t  *fPmtX;
  Float_t  *fPmtY;
  Float_t  *fPmtZ;
  Int_t    *fPmtCrate;
  Int_t    *fPmtCard;
  Int_t    *fPmtChannel;
  Int_t    *fPmtSnomanNo;
  Int_t    *fPmtQueensNo;
  Bool_t   *fPmt75Ohm;
  Bool_t   *fBatch3Panel;
  Int_t    *fPanel;
  QSnoDB   *fClient;     //SNODB client
  QBank    *fccct;       //CCCT ccc->snoman map
  QBank    *fgedp;       //PMT positions
  QBank    *fblown75s;   //List of blown 75Ohm channels
  QDQXX    *fDQXX;       //DQXX titles banks
  QCHCS    *fCHCS;       //CHCS (RSPS) titles banks
};


#endif
