//*-- Author :    Olivier Simard   2006 01
/*************************************************************************
 * Copyright(c) 2006, The SNO Software Project, All rights reserved.     *
 * Authors:  Olivier Simard                                              *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include "QNCDPositionFit.h"

ClassImp(QNCDPositionFit)
;
//
//   ------------------------------------------------------------
// - QNCDPositionFit : NCD Position Fitter that uses Optical Data -
//   ------------------------------------------------------------
//
// NCD positions are initialized to their anchor point,
// where they are attached to the Acrylic Vessel (AV), through
// a QNCDArray from which QNCDPositionFit derives.
//
// Once a QNCD has been loaded from the QNCDArray, the functions
// of this class are useful to select the PMT occupancies that are
// used to fit the NCD position.
// The PMT occupancies are loaded through a QOCATree, the data
// structure used to store processed optical data. A QOCATree
// usually contains a set of QOCARun's and each run many QOCAPmt's.
// Each of these objects are identified in this class:
//      
//      QNCD: fCurrentNcd
//  QOCATree: fQOCATree
//   QOCAFit: fQOCAFit
//   QOCARun: fCurrentRun
//   QOCAPmt: fCurrentPmt
//
// It is useful that any QOCATree can be loaded, making easier the
// analysis of systematics in the fit procedure.
// A QOCAFit may also be loaded to correct the PMT occupancies
// with the laserball anisotropy in z.
//
// Another thing that can be done is the splitting of the Ncds
// into sections. By setting the number of sections explicitely
// with SetNZsections(Int_t), the class adds an additional PMT
// selection based on which z-section they belong too.
// This feature can be useful to test if the Ncds are tilted
// from the vertical (even if as of 12.2005 there are no ways in
// QOptics to get a shadow mask with tilted Ncds).
//
//   ---------------
//   Main functions:
//   ---------------
// 
// Fill***();
//  There are many functions used to fill data into histograms
//  that are described below.
//   - FillNcd() calls FillNcdRun() in a loop over all runs contained
//     in the QOCATree loaded.
//   - FillNcdRun() calls FillNcdPmt() for all QOCAPmt's contained
//     in the current QOCARun.
//   - FillNcdPmt() looks for 'clear shadows' based on the PMT position
//     and relative occupancy. There are other checks made to make sure
//     the PMTs kept are close or in the direct/partial shadowed region.
//
// FitNcdPosition();
//  Must be called after Fill functions.
//  The function passes the filled histograms and the fitting functions
//  to TMinuit which (tries to) finds the NCD coordinate (x,y).
//  After this function call, the fitted position and error are accessible
//  through the Get functions.
//
//   ---------------
//   Other Notes:
//   ---------------
//
// Histograms:
//   TProfile's object are used in this class. In either 1D or 2D, the 
//   TProfile are useful to keep adding data (TProfile::Fill) without
//   having to calculate explicitely the mean and error in each bin.
//    1D: the shadow can be evaluated using the distance of closest
//        approach of a light ray to a Ncd.
//    2D: the inverse relative occupancy is filled in surface defined
//        around the default position. The maximum of this histogram
//        is the point in (x,y) where the shadow is the most important,
//        averaged over all runs. It is particularly cool to visualize
//        the so-called 'occupancy map' to see the resolution of the 
//        method.
//
// Functions:
//   TF1 and TF2 are used to define functions of one or multiple parameters.
//   It is easy after the function is defined to pass the fitting function
//   through TH1::Fit(TF1*) since TF2 derives from TF1.
//   Gaussian functions in 1D and 2D are defined with this class but are not
//   classes members (see ROOT documentation to find out why).
//   These functions are called gauss1d and gauss2d.
//
//
//______________________________________________________________________________
QNCDPositionFit::QNCDPositionFit(QOCATree* aQOCATree, QOCAFit* aQOCAFit):QNCDArray()
{
  // QNCDPositionFit default constructor.
  // QNCDArray is public so it loads the array by default.

  printf("-----------------------------------------------------\n");
  printf("QNCDPositionFit: Loading default NCD array configuration\n");

  fCurrentRun = NULL; // current run
  fCurrentPmt = NULL; // current pmt

  // assigns manually the QOCATree containing the optical data.
  if(aQOCATree) SetQOCATree(aQOCATree);
  
  // Assign the QOCAFit containing the optical fit parameters.
  fQOCAFit = aQOCAFit;
  if(aQOCAFit){
    printf("QNCDPositionFit: Using corrections for laserball asymmetry.\n");
  } else printf("QNCDPositionFit: Using flat laserball distribution.\n");
  
  // assign QNCD working pointer to first found in array, by default.
  fCurrentNcd = (QNCD*) GetCounter(0); // inheritance from QNCDArray
  
  // shadow tolerance (similar to QOptics::fNcdTolerance)
  fShadowTolerance = 5.0;
  
  // display flag for debugging or to get info in each function
  fDisplay = 0;

  // TMinuit related variables
  fFitDisplay = -1; // minimal printing in TMinuit
  fErrorType = 1;   // use chisquare parabola to calculate errors
  
  // physical object size constants
  fRncd = 2.54; // radius of a typical ncd counter (cm)
  fRpmt = 13.45; // radius of a typical pmt with reflector (cm)
  fRlb = 5.45;  // radius of laserball (cm)
  
  // run and z-section indices
  fRunIndex = 0;
  fNZsection = 1; // default is one section (whole ncd)
  fZsectionIndex = 0;

  // acceptance values
  fMinOcc = 0.01;
  fMaxOcc = 10.0;
  fMaxPmtIncAng = 0.25*TMath::Pi();
  fMinPmtperRun = 30;
  fMinRunperNcd = 5;
  fMaxSourceRadius = 600.0;
  fMinSourceNcdDistance = 0;     // minimal
  fMaxSourceNcdDistance = 1e+10; // maximal

  // other parameters
  fNpmts = 0; // number of valid data points
  fNruns = 0; // number of valid runs
  
  fClosenessMin = 0;
  fClosenessMax = 1;
  fOccupancy1d = new TProfile("fOccupancy1d",
			      "Relative Occupancy vs Light ray distance",
			      40,-5,5); // 0.25 fRncd bin width

  // initialize varying parameters for 2D function
  fOccupancy2d = NULL;    // initialized in SetQNCD()
  fSurfaceTolerance = 10; // +/- 10 cm
  fSurfaceNbin = 100;

  // ncd position and error storage
  fNcdXnominal = 0;
  fNcdYnominal = 0;
  fNcdXfit = NULL;
  fNcdYfit = NULL;
  fNcdXerr = NULL;
  fNcdYerr = NULL;
  fNcdXsig = NULL;
  fNcdYsig = NULL;
  fNcdZmin = NULL;
  fNcdZmax = NULL;
  fNPmtperZ = NULL;
  
  // predefined contour calculation options
  fContours = kFALSE;
  fNContourPoints = 50;
  fContour99 = NULL;
  fContour95 = NULL;
  fContour90 = NULL;
  
  printf("-----------------------------------------------------\n");

}

//______________________________________________________________________________
QNCDPositionFit::~QNCDPositionFit()
{
  // QNCDPositionFit destructor.
  // Delete arrays and objects created using new in this class
  // and resets the according pointers to NULL.

  fCurrentRun = NULL;
  fCurrentPmt = NULL;

  if(fOccupancy1d){delete fOccupancy1d; fOccupancy1d = NULL;}
  if(fOccupancy2d){delete fOccupancy2d; fOccupancy2d = NULL;}

  if(fNcdXfit){ delete [] fNcdXfit; fNcdXfit = NULL;} 
  if(fNcdYfit){ delete [] fNcdYfit; fNcdYfit = NULL;}
  if(fNcdXerr){ delete [] fNcdXerr; fNcdXerr = NULL;}
  if(fNcdYerr){ delete [] fNcdYerr; fNcdYerr = NULL;}
  if(fNcdXsig){ delete [] fNcdXsig; fNcdXsig = NULL;}
  if(fNcdYsig){ delete [] fNcdYsig; fNcdYsig = NULL;}
  if(fNcdZmin){ delete [] fNcdZmin; fNcdZmin = NULL;}
  if(fNcdZmax){ delete [] fNcdZmax; fNcdZmax = NULL;}
  if(fNPmtperZ){delete [] fNPmtperZ; fNPmtperZ = NULL;}

  if(fContour99){delete fContour99; fContour99 = NULL;}
  if(fContour95){delete fContour95; fContour95 = NULL;}
  if(fContour90){delete fContour90; fContour90 = NULL;}

}

//______________________________________________________________________________
void QNCDPositionFit::ResetHistograms()
{
  // Reset the histogram bin values to their default using TH1::Reset().
  // Apparently, under/over flows are not reset. Who cares.

  if(fOccupancy1d) fOccupancy1d->Reset();
  if(fOccupancy2d) fOccupancy2d->Reset();

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetQOCATree(QOCATree* aQOCATree)
{
  // Sets the fQOCATree pointer and binds the fCurrentRun to it.
  // Assigning QOCARun's should be done with SetQOCARun(aRunIndex) 
  // afterwards.

  if(aQOCATree){
    fQOCATree = aQOCATree;
    printf("QNCDPositionFit::SetQOCATree(): Reading QOCATree %s.\n",fQOCATree->GetName());
  
    // assign run pointer
    fCurrentRun = fQOCATree->GetQOCARun();

    // Number of entries in QOCATree is usually the number of runs or positions. 
    fNpos = (Int_t) fQOCATree->GetEntries();
  
  } else Warning("SetQOCATree","Needs a valid QOCATree to work properly.\n");

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetQOCARun(Int_t aRunIndex)
{
  // Sets the fCurrentRun pointer.
  // Also sets the run index [0, fNpos-1] and the source position vector.

  if(!fQOCATree || aRunIndex < 0 || aRunIndex > fQOCATree->GetEntries()){
    Error("SetQOCARun","Run index supplied out of range.");
  }
  fQOCATree->GetEntry(aRunIndex);
  fRunIndex = aRunIndex;
  fPosition = (TVector3) fCurrentRun->GetLaserxyz();
  Int_t run = (Int_t) fCurrentRun->GetRun();

  if(fDisplay >= 1){
    printf("-----------------------------------------------------\n");
    printf("QNCDPositionFit has been set to Run %.2d (%d).\n",aRunIndex,run);
    if(fDisplay >= 2){
      printf(" with source position : %7.2f %7.2f %7.2f.\n",
	     fPosition.X(),fPosition.Y(),fPosition.Z());
    }
    printf("-----------------------------------------------------\n");
  }

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetQOCAPmt(Int_t aPmtIndex)
{
  // Sets the fCurrentPmt pointer.
  // Also sets Pmt related quantities, with respect to the source:
  // - direction of light path leaving the source: fDirection
  // - total distance between source and pmt centers : fDistance
  // - incident angle of light on pmt surface : fCostheta.
  // All these quantities depend on the geometry only.

  if(!fCurrentRun || (aPmtIndex < 0) || (aPmtIndex > fCurrentRun->GetNpmt())){
    Error("SetQOCAPmt","Pmt index supplied out of range.");
  }
  fCurrentPmt = (QOCAPmt*) fCurrentRun->GetPMT(aPmtIndex);
  fDirection.SetXYZ(fCurrentPmt->GetPMTrelvecX(),
		    fCurrentPmt->GetPMTrelvecY(),
		    fCurrentPmt->GetPMTrelvecZ());
  fDistance = 
    fCurrentPmt->GetDd2o() + fCurrentPmt->GetDacr() + fCurrentPmt->GetDh2o();
  fCostheta = fCurrentPmt->GetCospmt();
  
  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetQNCD(Int_t aNcdIndex)
{
  // Sets the fCurrentNcd pointer.
  // Initializes the profile histograms given the nominal
  // position of this Ncd.

  if(aNcdIndex < 0 || aNcdIndex > GetNCounters()){
    Error("SetQNCD","Ncd index supplied out of range.");
  }
  fCurrentNcd = (QNCD*) GetCounter(aNcdIndex);
  fNcdXnominal = (Float_t)fCurrentNcd->GetTop().X();
  fNcdYnominal = (Float_t)fCurrentNcd->GetTop().Y();

  printf("-----------------------------------------------------\n");
  printf("QNCDPositionFit has been set to Ncd %.2d.\n",aNcdIndex);
  if(fDisplay){
    printf(" with position : %10.2f %10.2f.\n",fNcdXnominal,fNcdYnominal);
  }
  printf("-----------------------------------------------------\n");

  // check on limits of inverse occupancy
  if(fMaxOcc == 0.){
    fMaxOcc = 10.0; // default value
    Warning("SetQNCD","Maximum Relative Occupancy brought back to %.2f",fMaxOcc);
  }
  if(fMinOcc == 0.){
    fMinOcc = 0.01; // default value
    Warning("SetQNCD","Minimum Relative Occupancy brought back to %.2f",fMinOcc);
  }

  // initialize occupancy 2D map centered on nominal position
  Char_t name[128], title[1024];
  sprintf(name,"fOccupancy2d_%.2d",aNcdIndex);
  sprintf(title,"Occupancy Map and Position for NCD %.2d",aNcdIndex);
  if(fOccupancy2d) delete fOccupancy2d;
  fOccupancy2d = new TProfile2D(name,title,
    fSurfaceNbin,(fNcdXnominal-fSurfaceTolerance),(fNcdXnominal+fSurfaceTolerance),
    fSurfaceNbin,(fNcdYnominal-fSurfaceTolerance),(fNcdYnominal+fSurfaceTolerance),
    1./fMaxOcc,1./fMinOcc); // limits in z sets a default cut on occupancy

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetNZsection(Int_t aNumber)
{
  // Sets the number of z-sections for the current Ncd.
  // An hardcoded maximum is set to 8, since more z-sections
  // would not make any sense.
  // Assigns the different boundaries for each Ncd.
  // In cases of difficulties, the top section of each Ncd is always 
  // the shortest in length. This would be the hardest to fit the 
  // position without the laserball correction anyway.
  // Also initializes the number of Pmts per z-section (fNPmtperZ).
  // fCurrentNcd needs to be set before calling this function.

  if(aNumber <= 0){
    Error("SetNZsection","Cannot initialize size-zero arrays.");
    exit(-1);
  }
  if(aNumber > 8){
    Warning("SetNZsection",
	    "Should reconsider the number of z-sections: %d is large.",aNumber);
  } 

  // set the number of z-sections to the input argument
  fNZsection = aNumber;

  // arrays with floats
  if(fNcdXfit){delete [] fNcdXfit; fNcdXfit = NULL;}
  if(fNcdYfit){delete [] fNcdYfit; fNcdYfit = NULL;}
  if(fNcdXerr){delete [] fNcdXerr; fNcdXerr = NULL;}
  if(fNcdYerr){delete [] fNcdYerr; fNcdYerr = NULL;}
  if(fNcdXsig){delete [] fNcdXsig; fNcdXsig = NULL;}
  if(fNcdYsig){delete [] fNcdYsig; fNcdYsig = NULL;}
  if(fNcdZmin){delete [] fNcdZmin; fNcdZmin = NULL;}
  if(fNcdZmax){delete [] fNcdZmax; fNcdZmax = NULL;}
  if(fNPmtperZ){delete [] fNPmtperZ; fNPmtperZ = NULL;}

  fNcdXfit = new Float_t[fNZsection];
  fNcdYfit = new Float_t[fNZsection];
  fNcdXerr = new Float_t[fNZsection];
  fNcdYerr = new Float_t[fNZsection];
  fNcdXsig = new Float_t[fNZsection];
  fNcdYsig = new Float_t[fNZsection];
  fNcdZmin = new Float_t[fNZsection];
  fNcdZmax = new Float_t[fNZsection];
  fNPmtperZ = new Int_t[fNZsection];
    
  // get boundaries for each section of Ncd
  TVector3 top = fCurrentNcd->GetTop();
  TVector3 bottom = fCurrentNcd->GetBottom();
  Float_t length = (Float_t)fCurrentNcd->GetLength();
  Float_t lengthpersection = length/(Float_t)fNZsection;
    
  // set first section (index 0) explicitly
  fNcdZmin[0] = (Float_t) bottom.Z(); // attachment point
  fNcdZmax[0] = (Float_t) bottom.Z() + lengthpersection;
    
  // loop to determine the rest 
  for(Int_t iz = 1 ; iz < fNZsection ; iz++){
    // set this minimum to previous maximum 
    fNcdZmin[iz] = fNcdZmax[iz-1];
    // set this maximum by incrementing
    fNcdZmax[iz] = fNcdZmin[iz] + lengthpersection;
  }
  // reset maximum of last section explicitly
  fNcdZmax[fNZsection-1] = (Float_t) top.Z(); // cable

  if(fDisplay >= 2){
    printf("Z-section limits:\n");
    for(Int_t iz = 0 ; iz < fNZsection ; iz++){
      printf("\t%.2d %+10.2f %+10.2f\n",iz,fNcdZmin[iz],fNcdZmax[iz]);
    }
  }

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetNZsectionAsym()
{
  // Redefine the z-sections using a weighting cos2(theta) function.
  // Produces longer sections at bottom/top where data is
  // missing sometimes. Use with fNZsection even only, can get
  // weird limits sometimes: use with display switch.
    
  // get boundaries for each section of Ncd
  TVector3 top = fCurrentNcd->GetTop();
  TVector3 bottom = fCurrentNcd->GetBottom();

  Float_t distancexy = sqrt(pow(top.X(),2) + pow(top.Y(),2));
  // reweight with respect to cos2 distribution
  for(Int_t iz = 1 ; iz < fNZsection ; iz++){
    fNcdZmin[iz] *= pow(cos(atan(fabs(fNcdZmin[iz]/distancexy))),2);
    fNcdZmax[iz] *= pow(cos(atan(fabs(fNcdZmax[iz]/distancexy))),2);
  }
  // reset maximum of last section explicitly
  fNcdZmax[0] *= pow(cos(atan(fabs(fNcdZmax[0]/distancexy))),2);
  fNcdZmax[fNZsection-1] = (Float_t) top.Z(); // cable

  if(fDisplay >= 2){
    printf("Z-section limits:\n");
    for(Int_t iz = 0 ; iz < fNZsection ; iz++){
      printf("\t%.2d %+10.2f %+10.2f\n",iz,fNcdZmin[iz],fNcdZmax[iz]);
    }
  }
  
  return;
}

//______________________________________________________________________________
void QNCDPositionFit::SetZLimits(Float_t aMin, Float_t aMax)
{
  // Set the z-limit on a single z-section explicitely.
  // Assumes the fNcdZmin and fNcdZmax arrays were created with
  // at least one z-section so that the [0] index can be filled.

  if(fNZsection != 1){
    Warning("SetZLimits","Cannot set limits for more than one section.");
    return;
  }
  // swap if limits don't make sense
  if(aMin > aMax){
    Float_t buf = aMin;
    aMin = aMax;
    aMax = buf;
  }
  // check if current ncd limits make sense
  Float_t ncdminz = (Float_t)fCurrentNcd->GetBottom().Z();
  Float_t ncdmaxz = (Float_t)fCurrentNcd->GetTop().Z();
  if(aMin < ncdminz) aMin = ncdminz;
  if(aMax > ncdmaxz) aMax = ncdmaxz;

  // then set them explicitely
  fNcdZmin[0] = aMin;
  fNcdZmax[0] = aMax;

  if(fDisplay >= 1){
    printf("User reset z-limits for this single z-section: %.2f %.2f\n",aMin,aMax);
  }
  return;
}

//______________________________________________________________________________
Float_t QNCDPositionFit::GetZsectionLength(Int_t aZsectionIndex)
{
  // Returns the length of a section for the splitted Ncd.

  Float_t length = 0.; // default

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Warning("GetZsectionLength","Section index not in range.");
  } else if(fNZsection == 1) return fCurrentNcd->GetLength();
    
  if(fNcdZmax && fNcdZmin){
    length = fNcdZmax[aZsectionIndex] - fNcdZmin[aZsectionIndex];
  } else Warning("GetZsectionLength","Arrays do not exist.");

  return length;
}

//______________________________________________________________________________
Float_t QNCDPositionFit::GetMiddleZ(Int_t aZsectionIndex)
{
  // Returns the middle z-value of a section for the splitted Ncd.

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Warning("GetMiddleZ","Section index not in range.");
    return 0;
  }
  
  Float_t middle = 0;
  Float_t length = GetZsectionLength(aZsectionIndex);

  // add half length from bottom
  middle = fNcdZmin[aZsectionIndex] + 0.5*length;

  return middle;
}

//______________________________________________________________________________
Int_t QNCDPositionFit::GetZsectionIndex(Float_t aZvalue)
{
  // Returns the ncd section index for this z-value.
  // Loops over the z-section limits and find the index.
  // It is possible given the geometry of the Pmt array
  // that aZvalue is outside the range and that the function
  // returns a negative section index.
  
  Int_t sectionindex = -1; // default value (same as histogram)
  Int_t counts = 0;

  // try to put outliers in the ends
  // if(aZvalue < fNcdZmin[0]) return 0;
  // if(aZvalue > fNcdZmin[fNZsection-1]) return fNZsection-1;

  for(Int_t iz = 0 ; iz < fNZsection ; iz++){
    if((fNcdZmin[iz] < aZvalue) && (aZvalue < fNcdZmax[iz])){
      sectionindex = iz;
      counts++;
    }
  }

  // messages in case of problems
  if(fDisplay >= 2){
    if(counts <= 0) Warning("GetZsectionIndex","No z-section index found for z = %.2f cm.",aZvalue);
    else if(counts > 1) Warning("GetZsectionIndex","%d z-section index found.",counts);
  }

  return sectionindex;
}

//______________________________________________________________________________
Int_t QNCDPositionFit::GetNPmtperZ(Int_t aZsectionIndex)
{
  // Returns the number of Pmts filled for this z-section.
  // Assumes fNPmtperZ has been created and filled.
  
  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Warning("GetNPmtperZ","Section index not in range.");
    return 0;
  }
  Int_t npmtz = 0;
  if(!fNPmtperZ) Warning("GetNPmtperZ","fNPmtperZ does not exist.");
  else npmtz = fNPmtperZ[aZsectionIndex];

  return npmtz;
}

//______________________________________________________________________________
Float_t QNCDPositionFit::GetNcdPositionX(Int_t aZsectionIndex)
{
  // Returns the x-coordinate of the Ncd.

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Error("GetNcdPositionsX","Ncd z-section index supplied out of range.");
  }
  
  return fNcdXfit[aZsectionIndex];
}

//______________________________________________________________________________
Float_t QNCDPositionFit::GetNcdPositionY(Int_t aZsectionIndex)
{
  // Returns the y-coordinate of the Ncd.
  
  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Error("GetNcdPositionsY","Ncd z-section index supplied out of range.");
  }

  return fNcdYfit[aZsectionIndex];
}

//______________________________________________________________________________
Float_t QNCDPositionFit::GetNcdPositionXerr(Int_t aZsectionIndex)
{
  // Returns the error on the x-coordinate of the Ncd.

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Error("GetNcdPositionsXerr","Ncd z-section index supplied out of range.");
  }
  
  return fNcdXerr[aZsectionIndex];
}
//______________________________________________________________________________
Float_t QNCDPositionFit::GetNcdPositionYerr(Int_t aZsectionIndex)
{
  // Returns the error on the y-coordinate of the Ncd.

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Error("GetNcdPositionsYerr","Ncd z-section index supplied out of range.");
  }

  return fNcdYerr[aZsectionIndex];
}
//______________________________________________________________________________
Float_t QNCDPositionFit::GetNcdPositionXsig(Int_t aZsectionIndex)
{
  // Returns the error on the x-coordinate of the Ncd.

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Error("GetNcdPositionsXerr","Ncd z-section index supplied out of range.");
  }
  
  return fNcdXsig[aZsectionIndex];
}
//______________________________________________________________________________
Float_t QNCDPositionFit::GetNcdPositionYsig(Int_t aZsectionIndex)
{
  // Returns the error on the y-coordinate of the Ncd.

  if((aZsectionIndex < 0) || (aZsectionIndex > fNZsection)){
    Error("GetNcdPositionsYerr","Ncd z-section index supplied out of range.");
  }

  return fNcdYsig[aZsectionIndex];
}

//______________________________________________________________________________
Bool_t QNCDPositionFit::IsOtherNcd()
{
  // Checks if the calculated path is also shadowed by a secondary Ncd.
  // This is possible since the Ncd index and minimal distance calculated 
  // by QOptics that are stored in QOCARun only keep the info for one Ncd.
  //
  // Loops over all other Ncds and checks the distances to the light
  // path against fShadowTolerance. The check is done only on the
  // distance, no further corrections applied.
  // Uses QNCD::GetDistance() which loads the nominal positions.
  // This should be fine since the fitted Ncd positions should not
  // be off by much.
  // Returns kTRUE if there is at least one other Ncd in the way.

  Int_t status;
  Bool_t answer = kFALSE;
  QNCD* qncd;
  Int_t nncd = GetNCounters(); // from QNCDArray
  // array of ncd counters
  Int_t* ncd = new Int_t[nncd];

  for(Int_t incd = 0 ; incd < nncd ; incd++){
    ncd[incd] = 0; // no multiple-shadowing coming from this ncd

    if(incd == fCurrentNcd->GetIndex()) continue; // skip self-check
    else {
      qncd = (QNCD*) GetCounter(incd);
      Double_t dist = qncd->GetDistance(fPosition,fDirection,status);

      // distance and status check
      if(((dist-fRncd) < fShadowTolerance) && (status < 10)){
	answer = kTRUE; // switch to true even if it happens only once
	ncd[incd] = 1;  // multiple-shadowing coming from this ncd
      }
    }
  }

  // display specific info 
  if(fDisplay >= 3 && answer){
    printf("In IsOtherNcd(): Other Ncds were in the way:\n");
    printf("\tIndices: ");
    for(Int_t incd = 0 ; incd < nncd ; incd++) if(ncd[incd]) printf("%5.2d",incd);
    printf("\n");
  }

  delete [] ncd;
  return answer;
}

//______________________________________________________________________________
Bool_t QNCDPositionFit::IsShadowed()
{
  // Get shadow flags from the data stored in fQOCATree.
  // Assumes the QOCATree has been completely filled by
  // QPath::CalculatePositions() which calls QOptics::IsNcd().
  // The function returns kTRUE if the pmt is shadowed, given the 
  // tolerance.
  //
  // This function should not be called by the fit function 
  // since it will only return the PMTs as determined as shadowed
  // using the Ncd default positions, stored in the QOCATree.

  if(
     fCurrentPmt->GetNcd() 
     && (fCurrentPmt->GetNcdMinDist() <= fShadowTolerance)
     && (fCurrentPmt->GetCounterIndex() == fCurrentNcd->GetIndex())
     ) return kTRUE;
  else return kFALSE;
}


//______________________________________________________________________________
Double_t* QNCDPositionFit::GetShadowLimits()
{
  // Provide another way (comparable to QOptics) to calculate the 
  // shadow region, given in terms of the phi spherical coordinate 
  // of the light ray direction.
  // Assumes the Ncd is perfectly vertical (no tilt) and 
  // point source.

  // array with two values to be returned
  Double_t* limit = new Double_t[2]; // phimin,phimax
  limit[0] = +999; limit[1] = -999;  // default

  // unit vectors parallel to ncd counter axis
  TVector3 unitz_p(0,0,1);
  TVector3 unitz_n(0,0,-1);

  // assign source to what is found in QOCARun
  TVector3 source = fCurrentRun->GetLaserxyz();

  // assign ncd vector
  TVector3 ncd = fCurrentNcd->GetCenter();

  // vector from the source to ncd center
  TVector3 sn = ncd - source;

  // vector from ncd center to surface perpendicular to sn,
  // or normal with magnitude corresponding to ncd radius.
  TVector3 d_p = unitz_p.Cross(sn.Unit());
  d_p.SetMag(fRncd);
  TVector3 d_n = unitz_n.Cross(sn.Unit());
  d_n.SetMag(fRncd);

  // add up d_ vectors to sn
  TVector3 sn_p = sn + d_p;
  TVector3 sn_n = sn + d_n;

  if(sn_n.Phi() < sn_p.Phi()){
    limit[0] = sn_n.Phi();
    limit[1] = sn_p.Phi();
  } else {
    limit[1] = sn_n.Phi();
    limit[0] = sn_p.Phi();
  }

  if(fDisplay){
    printf("QNCDPositionFit::GetShadowLimits(): ");
    printf("Limits in Phi calculated: %.5f < Phi < %.5f (rad.)\n",limit[0],limit[1]);
  }
  
  return limit;
}

//______________________________________________________________________________
Double_t QNCDPositionFit::CalculateCloseness()
{
  // The fCloseness (x) parameter corresponds to DeltaL/fRncd, 
  // the corrected distance of the light ray to the Ncd surface. 
  //   - x <= 0 is the shadowed zone.
  //   - x >  0 is the non-shadowed zone.
  //
  // fCurrentRun, fCurrentPmt, and fCurrentNcd need to be set before
  // calling this function.
  
  // set fClosest according to QNCD::fCenter
  fClosest = fCurrentNcd->GetClosestPoint(fPosition,fDirection,1);

  // get the ncd central point (at z = 0) and
  // set the z-point to the corresponding run-pmt light direction
  TVector3 ncd(fCurrentNcd->GetCenter().X(),fCurrentNcd->GetCenter().Y(),fClosest.Z());

  // how close the ray comes to the ncd
  TVector3 dist = fClosest - ncd; // from ncd to closest point
  Double_t mindist = dist.Mag();

  // similar to QOptics::IsNcd()
  Double_t parallel = (fClosest-fPosition).Mag();
  Double_t phi = acos(fDistance/sqrt(pow(fDistance,2) + pow((fCostheta*fRpmt - fRlb),2)));
  Double_t perpendicular = parallel*sin(phi);
  Double_t deltaL = mindist - fRncd - fRlb - perpendicular;

  // x is the distance remaining relative to the ncd radius.
  Double_t x = (Double_t)(deltaL/fRncd);

  // set up the minimal and maximal values seen for x:
  if(x < fClosenessMin) fClosenessMin = x;
  if(x > fClosenessMax) fClosenessMax = x;

  // store it temporarily for access via GetCloseness()
  fCloseness = x;
  return x;
}

//______________________________________________________________________________
Bool_t QNCDPositionFit::ValidatePmt()
{
  // Determine if the Pmt is a good candidate for the selected Ncd fit.
  // Assumes fCurrentRun, fCurrentPmt, fCurrentNcd, and fZsectionIndex
  // have been all set.
  // Returns kTRUE if the Pmt is fine for fit.

  // basic conditions to skip Pmts
  if(!fCurrentPmt || fCurrentPmt->GetBasicBad()) return kFALSE;
  
  //-----------------------------------------
  // Repeat what is in SetQOCAPmt explicitely
  //-----------------------------------------
  // light direction vector stored in QOCAPmt
  fDirection.SetXYZ(fCurrentPmt->GetPMTrelvecX(),
		    fCurrentPmt->GetPMTrelvecY(),
		    fCurrentPmt->GetPMTrelvecZ());
  
  // total distance from source to pmt
  fDistance = 
    fCurrentPmt->GetDd2o() + fCurrentPmt->GetDacr() + fCurrentPmt->GetDh2o();
  // incident angle
  fCostheta = fCurrentPmt->GetCospmt();

  // need 'clear' shadow ; hence discard other flags
  if(fCurrentPmt->GetBelly() || fCurrentPmt->GetNeck() || fCurrentPmt->GetPipe() 
     || fCurrentPmt->GetRope() || fCurrentPmt->GetOtherbad() || (acos(fCostheta) > fMaxPmtIncAng)){
    return kFALSE;
  }
  
  //--------------------------------------
  // Selection based on default positions.
  //--------------------------------------
  // check with current tolerance
  Double_t x = CalculateCloseness(); // this sets fClosest
  if(x > (fShadowTolerance/fRncd)) return kFALSE;

  // check on z-section index with fClosest
  if(GetZsectionIndex(fClosest.Z()) != fZsectionIndex) return kFALSE;
  
  // check for other ncds in the way (mutliple shadowing)
  if(IsOtherNcd()) return kFALSE;

  // mpe-corrected counts times run normalization
  Double_t norm = (Double_t)fCurrentRun->GetNorm();
  Double_t occ = (Double_t)fCurrentPmt->GetOccupancy()*norm;
  Double_t err = (Double_t)fCurrentPmt->GetOccupancyerr()*norm;
  // discard relative occupancies if not in user range
  if((occ < fMinOcc) || (fMaxOcc < occ) || (occ == 0.0) || (occ == -0.0)
    || (err/occ > 1)) return kFALSE;
  
  // fCurrentPmt satisfies all requirements
  return kTRUE;
}
//______________________________________________________________________________
void QNCDPositionFit::FillNcdPmt()
{
  // Fills the occupancy map in the region around fCurrentNcd.
  // Assumes fCurrentRun, fCurrentPmt, and fCurrentNcd have been all set.
  // Cannot be called without initializing histograms.
  // ValidatePmt() should be called before this function.

  // closest point from light path to ncd surface and its components.
  // these are starting values from default position.
  Double_t closex = fClosest.X();
  Double_t closey = fClosest.Y();
  Double_t closez = fClosest.Z();

  // mpe-corrected counts times run normalization
  Double_t norm = (Double_t)fCurrentRun->GetNorm();
  Double_t occ = (Double_t)fCurrentPmt->GetOccupancy()*norm;
  
  // optional : correct z-dependence in pmt occupancies with
  // fitted laserball distribution.
  Double_t lbdist = 1.0;
  if(fQOCAFit != NULL){
    lbdist = (Double_t) (fQOCAFit->ModelLBDist(fCurrentRun,fCurrentPmt))*(fQOCAFit->ModelLBMask(fCurrentPmt));
  }
  occ /= lbdist; // remove laserball effects from occupancy

  //----------------------------------------
  // Make final check on z-section and fill all histograms.
  //----------------------------------------  
  Int_t zsectionindex = GetZsectionIndex(closez);
  if((zsectionindex < 0) || (zsectionindex != fZsectionIndex)) return;
  else {
    (fNPmtperZ[zsectionindex])++; // add to this z-section counter
    fOccupancy1d->Fill(fCloseness,occ); // sum over all runs
    fOccupancy2d->Fill(closex,closey,1./occ); // initial point for ray trace
    RayTrace(1./occ); // all other points
  }

  return;
}

//______________________________________________________________________________
Bool_t QNCDPositionFit::ValidateRun()
{
  // Determine if the Run is a good candidate for the selected Ncd fit.
  // Assumes fCurrentRun and fCurrentNcd have been set.
  // Returns kTRUE if the Ncd is fine for fit, based on the 
  // number of valid Pmts in the run.

  //-----------------------------------
  // Load the source position and check
  // the magnitude of the vector.
  //-----------------------------------
  fPosition = (TVector3) fCurrentRun->GetLaserxyz();
  Float_t mag = (Float_t) fPosition.Mag();
  if(mag > fMaxSourceRadius){
    if(fDisplay >= 1){
      printf("Skipped run due to high radius: %.1f > %.1f\n",
	     mag,fMaxSourceRadius);
    }
    return kFALSE;
  }
  //-----------------------------------
  // Load the ncd position and check
  // with allowed distances.
  //-----------------------------------
  TVector3 ncd = fCurrentNcd->GetCenter();
  TVector3 sou = fPosition;
  sou.SetZ(0.); // only considers x,y
  TVector3 dis = ncd - sou;
  Double_t distance = (Double_t)dis.Mag();
  if((distance < fMinSourceNcdDistance) || (fMaxSourceNcdDistance < distance)){
    if(fDisplay >= 1) printf("Skipped run due to positioning restrictions.\n");
    return kFALSE;
  }

  //--------------------------------------------------------------
  // Loop over all the PMTs to calculate the averaged occupancies.
  // PMT quantities must be set before calling FillNcdPmt(ipmt).
  //--------------------------------------------------------------
  Int_t nvalid = 0; // PMTs that have all requirements
  for(Int_t ipmt = 0 ; ipmt < fCurrentRun->GetNpmt() ; ipmt++){
    // set the QOCAPmt pointer
    fCurrentPmt = (QOCAPmt*) fCurrentRun->GetPMT(ipmt);
    // then check basic validity and z-section
    if(ValidatePmt()) nvalid++; // keep count
  }
  
  if(nvalid < fMinPmtperRun) return kFALSE;
  
  return kTRUE;
}
//______________________________________________________________________________
void QNCDPositionFit::FillNcdRun()
{
  // Fills Occupancy and Shadow Expectations for the Ncd 
  // for a specified Run, which has been set before.
  // Also fills z-sections if Ncds are fitted using multiple z-sections.
  // Assumes fCurrentNcd and fCurrentRun have been both set.
  // Stores the number of Pmts according to user settings.

  Int_t incd = fCurrentNcd->GetIndex();
  Int_t run = fCurrentRun->GetRun(); 
  if(fDisplay >= 1){
    printf("-----------------------------------------------------\n");
    printf("In FillNcdRun() for ncd %.2d in run %.2d/%.2d (%.5d).\n",
	   incd,fRunIndex,fNpos,run);
    printf("Source is at (%.1f,%.1f,%.1f).\n",
	   fPosition.X(),fPosition.Y(),fPosition.Z());
    printf("Filling averaged occupancy histogram ...\n");
  }

  //--------------------------------------------------------------
  // Loop over all the PMTs to calculate the averaged occupancies.
  // PMT quantities must be set before calling FillNcdPmt(ipmt).
  //--------------------------------------------------------------
  Int_t nvalid = 0; // PMTs that have all requirements
  for(Int_t ipmt = 0 ; ipmt < fCurrentRun->GetNpmt() ; ipmt++){
    // set the QOCAPmt pointer
    fCurrentPmt = (QOCAPmt*) fCurrentRun->GetPMT(ipmt);
    // then fill
    if(ValidatePmt()) {FillNcdPmt(); nvalid++;} // fill
  }

  fNpmts += nvalid; // counts good PMTs

  return;
}

//______________________________________________________________________________
Bool_t QNCDPositionFit::ValidateNcd()
{
  // Determine if the Ncd position can be fitted given the data that was 
  // filled.
  // Assumes fCurrentNcd has been set.
  // Returns kTRUE if the Ncd is fine for fit.

  Int_t nrun = 0;
  for(Int_t irun = 0 ; irun < fNpos ; irun++){
    SetQOCARun(irun);
    if(ValidateRun()) nrun++;
  }

  // final check for ncd fit availability
  if(nrun < fMinRunperNcd) return kFALSE;

  return kTRUE;
}
//______________________________________________________________________________
void QNCDPositionFit::FillNcd()
{
  // Fill data looping over all the runs.
  // --
  // Note:
  //   Counters for good Pmts and Runs are reset here.
  //   In cases the fit is performed without calling this function,
  //   the user must reset the counters by hand to get the right number
  //   of Pmts/runs used by the fit.

  // reset counters
  fNpmts = 0;
  fNruns = 0;

  Int_t nrun = 0;
  for(Int_t irun = 0 ; irun < fNpos ; irun++){
    SetQOCARun(irun);
    if(ValidateRun()) {FillNcdRun(); nrun++;}
  }

  fNruns += nrun; // counts goos runs

  if(fDisplay >= 1){
    printf("-----------------------------------------------------\n");
    printf("%d valid runs available for the fit (target: %d).\n",fNruns,fMinRunperNcd);
    printf("%d valid PMTs were successfully filled up to now.\n",fNpmts);
    printf("-----------------------------------------------------\n");
  }

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::RayTrace(Double_t aValue)
{
  // Uses the direction of the photon leaving the source to draw
  // a line in the 2D occupancy plot, around the default Ncd position.
  // fClosest needs to be calculated before calling this
  // function since it is the starting point.
  // fCurrentRun, fCurrentNcd, and fCurrentPmt also need to be set.

  if(!fOccupancy2d) Warning("RayTrace","No 2D histogram available.");

  // Grid axes
  TAxis* x = fOccupancy2d->GetXaxis();
  TAxis* y = fOccupancy2d->GetYaxis();
  // limits and bin widths
  Float_t xmax = (Float_t)x->GetXmax();
  Float_t xmin = (Float_t)x->GetXmin();
  Float_t ymax = (Float_t)y->GetXmax();
  Float_t ymin = (Float_t)y->GetXmin();
  Float_t nbinsx = (Float_t)x->GetNbins();
  Float_t nbinsy = (Float_t)y->GetNbins();
  Float_t binwidthx = (xmax-xmin)/nbinsx;
  Float_t binwidthy = (ymax-ymin)/nbinsy;

  // get the direction vector for manipulation
  TVector3 closest_noz(fClosest.X(),fClosest.Y(),0.);
  TVector3 ncd(fCurrentNcd->GetCenter().X(),fCurrentNcd->GetCenter().Y(),0.);
  // how close the ray comes to the ncd
  TVector3 dist = closest_noz - ncd; // from ncd to closest point
  TVector3 unity(0,1,0);
  Double_t psi = unity.Angle(closest_noz);
  TVector3 init(closest_noz);
  // shift length
  Double_t shiftlength = sqrt( pow(binwidthx*sin(psi),2) + pow(binwidthy*cos(psi),2) );
  TVector3 shift(fDirection.X(),fDirection.Y(),0.);
  shift.SetMag(shiftlength);

  //-----------------------------------------
  // loop until the ray leaves the 2D surface
  //-----------------------------------------
  Bool_t inside = kTRUE;   // ray is still inside region
  Bool_t reverse = kFALSE; // because starting point is in the center
  while(inside){ 

    if(reverse) init -= shift; // opposite way
    else init += shift;        // towards pmt
    Double_t closex = init.X();
    Double_t closey = init.Y();
    
    // condition to be outside the surface
    if((xmin > closex) || (closex > xmax) || (ymin > closey) || (closey > ymax)){
      if(reverse) inside = kFALSE;
      else {init = closest_noz; reverse = kTRUE;}
    } else fOccupancy2d->Fill(closex,closey,aValue);
  }

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::PrintSelection()
{
  // Prints the user option for run/pmt data selection.

  printf("-----------------------------------------------------\n");
  printf("QNCDPositionFit::PrintSelection(): Selection Criteria:\n");
  printf("  Shadow Tolerance = %12.2f (cm)\n",fShadowTolerance);
  printf("  Minimum Relative Occupancy = %12.2f\n",fMinOcc);
  printf("  Maximum Relative Occupancy = %12.2f\n",fMaxOcc);
  printf("  Maximum PMT Incident Angle = %12.2f (deg.)\n",fMaxPmtIncAng*(180./TMath::Pi()));
  printf("  Smallest Source-Ncd distance allowed = %.2e cm\n",fMinSourceNcdDistance);
  printf("  Largest Source-Ncd distance allowed =  %.2e cm\n",fMaxSourceNcdDistance);
  printf("  Minimum Number of PMTs per Run per Ncd = %d\n",fMinPmtperRun);
  printf("  Minimum Number of Runs per Ncd = %d\n",fMinRunperNcd);
  printf("-----------------------------------------------------\n");

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::FitNcdPosition(Int_t aZsectionIndex)
{
  // Loops over all runs and performs fit.

  if(!fOccupancy2d) Warning("FitNcdPosition","No 2D histogram available.");

  fZsectionIndex = aZsectionIndex;
  //ResetHistograms();

  if(!ValidateNcd()) Warning("FitNcdPosition","Data was not filled properly.");
  else FillNcd();
  
  FitNcdPosition();

  return;
}

//______________________________________________________________________________
void QNCDPositionFit::FitNcdPosition()
{
  // This function calls the 2D gaussian function in the fit.
  
  if(fDisplay >= 0){
    printf("-----------------------------------------------------\n");
    printf("In QNCDPositionFit::FitNcdPosition()...\n");
    printf("using %d PMT Occupancies in %d runs.\n",fNpmts,fNruns);
    printf("Following output comes from TMinuit.\n");
    printf("-----------------------------------------------------\n");
  }
  
  if(!fOccupancy2d) Warning("FitNcdPosition()","No 2D histogram available.");

  //------------------------------------
  // Set up the function and the occupancy map limits.
  // Perform first fit with nominal positions.
  //------------------------------------

  // grid axes
  TAxis* x = fOccupancy2d->GetXaxis();
  TAxis* y = fOccupancy2d->GetYaxis();
  // limits and bin widths
  Float_t xmax = (Float_t)x->GetXmax();
  Float_t xmin = (Float_t)x->GetXmin();
  Float_t ymax = (Float_t)y->GetXmax();
  Float_t ymin = (Float_t)y->GetXmin();
  Float_t wx = xmax-xmin;
  Float_t wy = ymax-ymin;
  //Float_t zmax = (Float_t)fOccupancy2d->GetZmax();
  Float_t zmax = (Float_t)fOccupancy2d->GetMaximum(1./fMinOcc);
  Float_t zmin = (Float_t)fOccupancy2d->GetZmin();
  Float_t meanz = (Float_t)fOccupancy2d->GetMean(3); // z-axis mean
  //printf("zmin = %.2f , zmax = %.2f, meanz = %.2f\n",zmin,zmax,meanz);
  Int_t binx, biny, binz;
  fOccupancy2d->GetMaximumBin(binx,biny,binz);
  Float_t xstart = (Float_t)x->GetBinCenter(binx);
  Float_t ystart = (Float_t)y->GetBinCenter(biny);
  if(fDisplay >= 2){
    printf("QNCDPositionFit::FitNcdPosition():\n");
    printf("  Found maximum bin : %d %d , with (x,y) = %10.2f, %10.2f\n",
	   binx,biny,xstart,ystart);
  }
  
  // 2D function
  TF2* f2 = new TF2("gauss2d",gauss2d,xmin,xmax,ymin,ymax,9);
  // give some sensible values
  //-------
  f2->SetParName(0,"Sigma-x");
  f2->SetParameter(0,wx); // sigmax
  f2->SetParLimits(0,0,1e+05);   // limits on sigmax
  //-------
  f2->SetParName(1,"Sigma-y");
  f2->SetParameter(1,wy); // sigmay
  f2->SetParLimits(1,0,1e+05);   // limits on sigmax
  //-------
  f2->SetParName(2,"Mean-x");
  f2->SetParameter(2,fNcdXnominal); // meanx
  f2->SetParLimits(2,f2->GetXmin(),f2->GetXmax()); // limits on x
  //-------
  f2->SetParName(3,"Mean-y");
  f2->SetParameter(3,fNcdYnominal); // meany
  f2->SetParLimits(3,f2->GetYmin(),f2->GetYmax()); // limits on y
  //-------
  f2->SetParName(4,"Norm");
  f2->SetParameter(4,zmax); // norm
  //-------
  f2->SetParName(5,"Offset");
  f2->SetParameter(5,0.5*(meanz-zmin)); // inverse of the average relative occupancy
  f2->SetParLimits(5,zmin,meanz);
  //-------
  f2->SetParName(6,"Slope-x");
  f2->SetParameter(6,0.); // relative changes in occupancy with x
  f2->FixParameter(6,0.);
  //-------
  f2->SetParName(7,"Slope-y");
  f2->SetParameter(7,0.); // relative changes in occupancy with x
  f2->FixParameter(7,0.);
  //-------
  Double_t d = 0;
  memcpy(&d,&fOccupancy2d,sizeof(TProfile2D*));
  f2->SetParName(8,"Ignore");
  f2->FixParameter(8,d);
  
  //------------------------------------
  // fitting calls:
  //  1) simplest chisquare fit
  //  2) log-likelihood fit
  //  3) improved log-likelihood fit with error estimation
  fOccupancy2d->Fit(f2,"ONR");
  fOccupancy2d->Fit(f2,"LLONR");
  fOccupancy2d->Fit(f2,"LLMEONR");

  // get Minuit fit status
  Int_t ierflag = gMinuit->GetStatus();
  if(ierflag == 4) Warning("FitNcdPosition","No convergence: %d.",ierflag);

  //------------------------------------
  // Get the first position estimators.
  // Use seperate projections if the position is
  // not within user's acceptance.
  //------------------------------------

  // variables containing the results
  Double_t ncdx, ncdxerr, ncdy, ncdyerr, sigmax, sigmay;
  sigmax = (Double_t)f2->GetParameter(0);
  sigmay = (Double_t)f2->GetParameter(1);
  ncdx = (Double_t)f2->GetParameter(2);
  ncdxerr = (Double_t)f2->GetParError(2);
  ncdy = (Double_t)f2->GetParameter(3);
  ncdyerr = (Double_t)f2->GetParError(3);

  // fill the storage arrays
  fNcdXfit[fZsectionIndex] = ncdx;
  fNcdYfit[fZsectionIndex] = ncdy;
  fNcdXerr[fZsectionIndex] = ncdxerr;
  fNcdYerr[fZsectionIndex] = ncdyerr;
  fNcdXsig[fZsectionIndex] = sigmax;
  fNcdYsig[fZsectionIndex] = sigmay;

  // restore limits in the TF2
  xmax = (Float_t)x->GetXmax();
  xmin = (Float_t)x->GetXmin();
  ymax = (Float_t)y->GetXmax();
  ymin = (Float_t)y->GetXmin();
  f2->SetRange(xmin,ymin,xmax,ymax);
  
  if(fContours){
    //------------------------------------
    // Contour plots in 2D with confidence
    // levels (C.L.): Q = -2*ln(1-alpha)
    // alpha = 99% -> Q = 9.210
    // alpha = 95% -> Q = 5.991
    // alpha = 90% -> Q = 4.605
    //------------------------------------

    // Minuit options
    ierflag = 0;
    
    Double_t printlevel = (Double_t)fFitDisplay;
    Double_t fcnerror = (Double_t)fErrorType;
    
    // execute the print level command
    gMinuit->mnexcm("SET PRINT",&printlevel,1,ierflag);
    
    // execute the set error command
    gMinuit->mnexcm("SET ERRDEF",&fcnerror,1,ierflag);

    Int_t minoscalls = fNContourPoints;
    if(fDisplay >= 1){printf("-----------------------------------------------------\n");
    printf("QNCDPositionFit::FitNcdPosition(): asking Minuit to calculate contours ...\n");}

    // delete old contours if they exist
    if(fContour99){delete fContour99; fContour99 = NULL;}
    if(fContour95){delete fContour95; fContour95 = NULL;}
    if(fContour90){delete fContour90; fContour90 = NULL;}

    // contour for 99% C.L.
    if(fDisplay >= 1){
      printf("-----------------------------------------------------\n");
      printf("QNCDPositionFit::FitNcdPosition(): %s Confidence level with %d points ...\n",
	     "99%",minoscalls);
    }
    gMinuit->SetErrorDef(9.210);
    fContour99 = (TGraph*)gMinuit->Contour(minoscalls,2,3);
  
    // contour for 95% C.L.
    if(fDisplay >= 1){
      printf("-----------------------------------------------------\n");
      printf("QNCDPositionFit::FitNcdPosition(): %s Confidence level with %d points ...\n",
	     "95%",minoscalls);
    }
    gMinuit->SetErrorDef(5.991);
    fContour95 = (TGraph*)gMinuit->Contour(minoscalls,2,3);

    // contour for 90% C.L.
    if(fDisplay >= 1){
      printf("-----------------------------------------------------\n");
      printf("QNCDPositionFit::FitNcdPosition(): %s Confidence level with %d points ...\n",
	     "90%",minoscalls);
    }
    gMinuit->SetErrorDef(4.605);
    fContour90 = (TGraph*)gMinuit->Contour(minoscalls,2,3);

    if(fDisplay >= 1){printf("QNCDPositionFit::FitNcdPosition(): Done with contours.\n");
    printf("-----------------------------------------------------\n");}

  }

  delete f2;
  return;
}

//______________________________________________________________________________
void QNCDPositionFit::FitNcdPosition(TH2D* surface)
{
  // Wrapper for FitNcdPosition

  if(!surface) Warning("FitNcdPosition()","No input 2D histogram available.");

  if(fOccupancy2d) delete fOccupancy2d;
  fOccupancy2d = (TProfile2D*) surface->Clone();

  FitNcdPosition();
}

//______________________________________________________________________________
Double_t gauss1d(Double_t* coord, Double_t* par)
{
  // This function is a 1D gaussian function with mean and rms
  // in x with a normalization factor in front.

  Double_t x = coord[0]; // x-coordinate

  Double_t sigmax = par[0]; 
  Double_t meanx = par[1];
  Double_t norm = par[2];
 
  // Cowan p.32 Eq. (2.22) with a normalization factor
  
  Double_t f = (norm/TMath::Sqrt(2*TMath::Pi()*sigmax*sigmax))*
    TMath::Exp(-(x-meanx)*(x-meanx)/(2*sigmax*sigmax));

  return f;
}

//______________________________________________________________________________
Double_t gauss2d(Double_t* coord, Double_t* par)
{
  // This function is a 2D gaussian surface with mean and rms
  // in both x,y with an extra normalization factor in front.

  Double_t x = coord[0]; // x-coordinate
  Double_t y = coord[1]; // y-coordinate

  Double_t sigmax = par[0]; 
  Double_t sigmay = par[1];
  Double_t meanx = par[2];
  Double_t meany = par[3];
  Double_t norm = par[4];
  Double_t offset = par[5];
  Double_t slopex = par[6];
  Double_t slopey = par[7];
  TProfile2D* p;
  memcpy(&p,&par[8],sizeof(TProfile2D*));

  //Int_t binx = p->GetXaxis()->FindBin(x);
  //Int_t biny = p->GetYaxis()->FindBin(y);
  Int_t ndata = (Int_t) p->GetBinEntries(p->FindBin(x,y));
  if(ndata < 5) TF2::RejectPoint(); // default kTRUE
 
  // Cowan p.34 Eq. (2.30) with rho = 0 and a normalization factor
  
  Double_t f = (norm/(2*TMath::Pi()*sigmax*sigmay))*
    TMath::Exp(-(x-meanx)*(x-meanx)/(2*sigmax*sigmax))*
    TMath::Exp(-(y-meany)*(y-meany)/(2*sigmay*sigmay)) +
    offset + x*slopex + y*slopey;

  return f;
}

//______________________________________________________________________________
void QNCDPositionFit::SetFitPosition(TVector3 aVector)
{
  // Sets the NCD position based on the fit value.
  fCurrentNcd->SetCenter(aVector);
  return;
}
//______________________________________________________________________________
void QNCDPositionFit::ResetPosition()
{
  // Sets the NCD position back to the nominal value.
  fCurrentNcd->CancelShift();
  return;
}

//______________________________________________________________________________
void QNCDPositionFit::ReFillAfterFit()
{
  // Useful function that uses the fitted position of the Ncd
  // and refills the histograms to allow the user if the
  // data makes more sense with the new position.

  ResetHistograms(); // empties fOccupancy1d and fOccupancy2d
  
  Double_t ncdx = fNcdXfit[fZsectionIndex];
  Double_t ncdy = fNcdYfit[fZsectionIndex];
  TVector3 center(ncdx,ncdy,0.);

  fCurrentNcd->SetCenter(center);

  FillNcd(); // refill

  fCurrentNcd->CancelShift(); // go back to default

  return;
}

