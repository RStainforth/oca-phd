//*-- Author :    Chris Jillings  01/12/00
//*-- Author :    Bryce Moffat    03/12/00

/*************************************************************************
 * Copyright(c) 1999, The SNO Software Project, All rights reserved.     *
 * Authors: Bryce Moffat                                                 *
 *          Chris Jillings                                               *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <QTitles.h>
#include <QPMTxyz.h>

#include "QPMTuvw.h"

ClassImp(QPMTuvw)
;
//______________________________________________________________________________
//
// QPMTuvw class.
//
// Returns PMT normals and nearest neighbour information.
//
//______________________________________________________________________________
QPMTuvw::QPMTuvw() {
  
  // Load in PMT normal vectors from SNOMAN data file (pmt_positions.dat),
  // and lists of PMT nearest neighbours from QSNO_ROOT/qoca/pmtneighbours.dat.
  //
  // The nearest neighbours are all tubes within 6 degrees, about 25 per PMT.
  // This gives the user a very reduced selection of tubes to search when looking
  // for _the_ nearest neighbour, for instance.  For averaging the intensity
  // over regions of the detector, 6 degrees/25 tubes was considered reasonable.
  // Is it??
  //
  // 2006-03-21 O.Simard
  // Changed pmt_positions.dat to pmt_positions_ver2.dat
  // to match the PMT positions loaded by QPMTxyz.
  //

  //--------------------
  if (!gPMTxyz) gSNO->GetPMTxyz();
  fBogusVector.SetXYZ(-9999,-9999,-9999);

  Char_t prodDirectory[1024];
  Char_t filename[1024];
  Char_t* snoRoot = getenv("OCA_SNO_ROOT");
  //Char_t* snoVer = getenv("SNO_SNOMAN_VER");
  //sprintf(prodDirectory,"%s/data/",snoRoot,snoVer);
    sprintf(prodDirectory,"%s/data/",snoRoot);
  sprintf(filename,"%s/pmt_positions_ver2.dat",prodDirectory);

  QTitles* pmts = new QTitles(filename,"GEDP",422,20000); 
  QTitles* panels = new QTitles(filename,"GEDP",400,80000);
  
  TVector3 panelDir[760];
  Int_t i,j,k;

  // j starts the loop at the part of the panels titles where 
  // the panel normals are defined. i loops ofver the number of panels
  Double_t u,v,w;

  // In pmt_positions.dat, look for: "Panel normal (rge_pan_local_zdir)"
  // to find the correct location to start reading words
  for( i=1,j=(2+98*759) ; i <= 759 ; i++, j+=3 ) {
    u = (Double_t)panels->GetFWord(j);
    v = (Double_t)panels->GetFWord(j+1);
    w = (Double_t)panels->GetFWord(j+2);
    panelDir[i].SetXYZ(u,v,w);
  }

  // We now know where the panels point. 
  // Now must find in which panels the PMTs are sitting 
  Int_t pmtPanels[9700];

  // again, put j in the right place to read the titles. Fill aray with i
  // this panels array is filled with snoman number.
  //  for (i=0, j=5+9799; i<9699; i++,j++) {  // offset of 5 in 3_0187
  for (i=0, j=7+9999; i<9699; i++,j++) {  // offset of 7 in 3_0190
    pmtPanels[i+1] = pmts->GetIWord(j);
  }
  
  //  sprintf(filename,"%s/map_ccc_tube.dat",prodDirectory);

  //  Int_t tube2LCN[9799+1];

  //  Int_t daqcn,cr,ca,ch;
  //  QTitles *ccct = new QTitles(filename,"CCCT",1,30000);

  //    for( i=0, j=11+16384 ; i < 9799 ; i++,j++ ) {
  //      daqcn =  ccct->GetIWord(j);
  //      if( daqcn==-999999 ) {
  //        tube2LCN[i+1] = -999999;
  //      } else {
  //        ca = (daqcn & 0xfc00) >> 10;
  //        cr = (daqcn & 0x3e0) >> 5;
  //        ch = (daqcn & 0x1f);
  //        tube2LCN[i+1] = ch + 32*ca + 512*cr;
  //        printf("%d %d %d %d %d\n",daqcn,ca,cr,ch,tube2LCN[i+1]);
  //      }
  //    }

  Int_t tube2LCN[10000];
  Int_t snoman_no;
  Int_t panel_no;
  Int_t lcn;

  // Make an inverse mapping of SNOMAN Tube number -> logical channel number
  // without consulting the ccct database ourselves...
  for (i=0; i<10000; i++) {
    tube2LCN[i] = -999999;
    if (!gPMTxyz->IsInvalidPMT(i) && gPMTxyz->IsNormalPMT(i)) {
      snoman_no = gPMTxyz->GetSnomanNo(i);
      //      printf("Logical number %d => SNOMAN number %d\n",i,snoman_no);
      if (snoman_no>0) tube2LCN[snoman_no] = i;
    }
  }

  // now have all the requisite data to go from LCN to normal vector
  //    for( i=0 ; i<9699 ; i++ ) {
  //      snoman_no = i+1;
  //      lcn = tube2LCN[snoman_no];
  //      if( lcn==-999999 ) { 
  //        ;
  //      } else {
  //        panel_no = pmtPanels[snoman_no];
  //        fUVW[lcn] = panelDir[panel_no];
  //        //      fUVW[lcn] *= -1*(fUVW[lcn].Mag());  // in titles they point out
  //        fUVW[lcn] *= -1;
  //        fUVW[lcn] = fUVW[lcn].Unit();
  //        printf("%d %d %d %g %g %g\n",snoman_no,lcn,panel_no,
  //  	     fUVW[lcn].X(),fUVW[lcn].Y(),fUVW[lcn].Z());
  
  //      }
  //    }

  for (i=0; i<10000; i++) {
    fUVW[i] = fBogusVector;
    if (!gPMTxyz->IsInvalidPMT(i) && gPMTxyz->IsNormalPMT(i)) {
      lcn = i;
      snoman_no = gPMTxyz->GetSnomanNo(i);
      if (snoman_no>0 && snoman_no<9700) {
	panel_no  = pmtPanels[snoman_no];
	fUVW[lcn] = panelDir[panel_no];
	fUVW[lcn] *= -1;
	fUVW[lcn] = fUVW[lcn].Unit();
	//	printf("%d %d %d %g %g %g\n",snoman_no,lcn,panel_no,
	//	       fUVW[lcn].X(),fUVW[lcn].Y(),fUVW[lcn].Z());
      }
    }
  }
  
  printf("PMT directions calculated.\n");
  
  //--------------------
  // Now, load up the PMT nearest neighbours tables form the QSNO_ROOT/qoca/
  // pmtneighbours.dat file

  Char_t *qsnoroot = getenv("OCA_SNO_ROOT");
  // the two +1 bellow are to emphasize that strlen does not take null 
  // terminator into account
  if( qsnoroot==NULL ) {
    qsnoroot = "/usr/home/qusno/qsno_prod"; // Use a default...
    
    printf("QPMTuvw uses QSNO_ROOT to get pmtneighbours.dat.\n");
    printf("QSNO_ROOT isn't set - trying: %s\n",qsnoroot);
  }
  
  sprintf(filename,"%s/data/pmtneighbours.dat",qsnoroot);
  FILE *fpmtneighbours = fopen(filename,"r");
  
  // Read in the data and store for the south orientation (no PMT rotation),
  // as well as for artificial orientations, where the neighbours are for a 
  // "rotated" detector (used for data smoothing.)
  // south:  the neighbours for tube i for detector rotated by +  0 deg
  // east :  the neighbours for tube i for detector rotated by + 90 deg
  // north:  the neighbours for tube i for detector rotated by +180 deg
  // west :  the neighbours for tube i for detector rotated by +270 deg
  
  Int_t pmt;            // Number of pmt currently being processed
  Int_t orientation;    // 0=south, 1=east, 2=north, 3=west
  Int_t number,numberread; // Number of neighbours, and as read from file
  Int_t dummy[100];        // Temporary storage for PMT neighbours
  Char_t dummystr[256]=""; // For reading in (west), (east), etc. as strings
  
  for(i=0;i<4;i++)for(j=0;j<10000;j++)fPMTNeighbours[i][j]=0;

  for (i=0; i<4; i++) {
    fscanf(fpmtneighbours,"%s",dummystr);
    //    printf("%s ",dummystr);
    fscanf(fpmtneighbours,"%d:",&orientation);
    //    printf("%d: ",orientation);
    fscanf(fpmtneighbours,"%s",dummystr);
    //    printf("%s ",dummystr);
    for (j=0; j<10000; j++) {
      if (!gPMTxyz->IsInvalidPMT(j) && gPMTxyz->IsNormalPMT(j) && j!=9728) {
	fscanf(fpmtneighbours,"%d :",&pmt);
	//	printf("%d (%d) : ",pmt,j);
	numberread = 0;
	while (kTRUE) { // Look out for terminating colon on line
	  fscanf(fpmtneighbours,"%s",dummystr);
	  //	  printf("%s ",dummystr);
	  if (strcmp(dummystr,":")) {
	    sscanf(dummystr,"%d",&dummy[numberread]);
	    numberread++;
	  } else break;
	}
	fscanf(fpmtneighbours,"%d\n",&number);
	//	printf(" %d\n",number);
	if (number != numberread) {
	  fprintf(stderr,"Warning in QPMTuvw::QPMTuvw():\n");
	  fprintf(stderr,"Incorrect number of neighbours read "
		  "for tube %d: %d read, %d expected.\n",j,numberread,number);
	}
	fPMTNumNeighbours[orientation][j] = numberread;
	fPMTNeighbours[orientation][j] = new Int_t[numberread];
	for (k=0; k<numberread; k++) fPMTNeighbours[orientation][j][k] = dummy[k];
      }
    } 
  }

  printf("PMT nearest neighbour list loaded.\n");
}
//______________________________________________________________________________
QPMTuvw::~QPMTuvw() {
  // QPMTuvw destructor

  // Destruct the elements of ***fPMTNeighbours allocated in the constructor.
  Int_t i,j;
  for (i=0; i<4; i++) {
    for (j=0; j<10000; j++) {
      if (fPMTNeighbours[i][j]) delete[] fPMTNeighbours[i][j];
    }
  }
}
//______________________________________________________________________________
TVector3 QPMTuvw::GetPMTDir(Int_t lcn)
{
  // Check that lcn is indeed a valid PMT channel number; if so, return the
  // normal vector to that PMT.

  if (!gPMTxyz->IsInvalidPMT(lcn) && gPMTxyz->IsNormalPMT(lcn)) return fUVW[lcn];
  else return fBogusVector;
}
//______________________________________________________________________________
Int_t *QPMTuvw::GetPMTNeighbours(Int_t lcn,Int_t orientation)
{
  // Return a list of PMT numbers which are the neighbours of PMT lcn.
  // The number of valid elements in the list is returned by 
  // GetNumPMTNeighbours().  A NULL pointer is returned for lcn out of range.
  
  orientation = orientation%4;  // Make sure orientation is in [0:3]

  if (!gPMTxyz->IsInvalidPMT(lcn) && gPMTxyz->IsNormalPMT(lcn))
    return fPMTNeighbours[orientation][lcn];
  else return NULL;
}
//______________________________________________________________________________
Int_t QPMTuvw::GetNumPMTNeighbours(Int_t lcn,Int_t orientation)
{
  // Return the number of neighbours for PMT lcn.
  // For lcn not within range, returns -1.

  orientation = orientation%4;  // Make sure orientation is in [0:3]

  if (!gPMTxyz->IsInvalidPMT(lcn) && gPMTxyz->IsNormalPMT(lcn))
    return fPMTNumNeighbours[orientation][lcn];
  else return -1;
}
//______________________________________________________________________________
void QPMTuvw::Streamer(TBuffer &R__b)
{
  // Stream an object of class QPMTuvw.
  // This class does its own initializing from data files, so no real need to
  // stream it in or out, really... 
  //
  // Updated 29-Jun-2001 for ROOT 3.00 "Automatic Schema Evolution" - BA Moffat

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if (R__v > 1) {
      QPMTuvw:Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      return;
    }
    //====process old versions before automatic schema evolution
    TObject::Streamer(R__b);
    //        int R__i;
    //        for (R__i = 0; R__i < 10000; R__i++)
    //           fUVW[R__i].Streamer(R__b);
    //        fBogusVector.Streamer(R__b);
    //        for (R__i = 0; R__i < 40000; R__i++)
    //           ;//R__b.ReadArray(fPMTNeighbours);
    //        R__b.ReadStaticArray((int*)fPMTNumNeighbours);
    R__b.CheckByteCount(R__s, R__c, QPMTuvw::IsA());
    //====end of old versions
  } else {
    QPMTuvw::Class()->WriteBuffer(R__b,this);
  }
}
//______________________________________________________________________________
