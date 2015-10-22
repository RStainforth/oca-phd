#ifndef __QPMTuvw_h
#define __QPMTuvw_h

// Copyright (C) 1999 CodeSNO. All rights reserved.

// Updated 2000,  Q Technology Project.  

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QPMTuvw                                                              //
//                                                                      //
// SNO PMT directions and nearest neighbours.                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include<TVector3.h>

//______________________________________________________________________________
//  PMTuvw class definition
//______________________________________________________________________________

class QPMTuvw : public TObject {
 private:
  TVector3 fUVW[10000];
  TVector3 fBogusVector;

  Int_t *fPMTNeighbours[4][10000];   // List of PMT neighbours
  Int_t fPMTNumNeighbours[4][10000];

 public:
  QPMTuvw();
  ~QPMTuvw();
  
  TVector3 GetPMTDir(Int_t lcn);
  Int_t *GetPMTNeighbours(Int_t lcn,Int_t orientation=0);
  Int_t GetNumPMTNeighbours(Int_t lcn,Int_t orientation=0);

  ClassDef(QPMTuvw,2) //PMT panel directions
};
//______________________________________________________________________________
#endif

















