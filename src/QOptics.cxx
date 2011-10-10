//*-- Author :    Bryce Moffat   15/08/99
//*-- Author :    Bryce Moffat   10/03/00

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
#include <math.h>

#include <TROOT.h>
#include <QTitles.h>
#include <QPMT.h>
#include "QOptics.h"
#include "QPMTuvw.h"
#include "QNCDArray.h"
#include "TMath.h"

#define DEBUG_QOptics kFALSE

QNCDArray* QOptics::fNcdArray;

//______________________________________________________________________________
// Static class variables defined here, so only one instance has to be kept
// in memory regardless of how many class instances there are in total
//______________________________________________________________________________
Int_t QOptics::fNumberOfInstances = 0;        // no instances on startup!
Bool_t QOptics::fQOpticsInitialized = kFALSE; // no geometry read in yet at startup!
Double_t QOptics::fAVInnerRadius = 600.5;     // inner radius of av (SNOMAN::geometry.dat)
Double_t QOptics::fAVThickness = 5.5;         // mean av thickness (SNOMAN::geometry.dat)
//______________________________________________________________________________  

Double_t QOptics::fAVPanelTheta[11];   // boundaries of AV panel rows
Double_t QOptics::fAVPanelPhi[10][20]; // phi boundaries
Int_t QOptics::fAVPanelsInRow[10];     // number of panels in each row
Int_t QOptics::fAVPanelsTotal[10];      // cumulative sum of all panels up to row [i]
//______________________________________________________________________________
Double_t QOptics::fBellyTheta[11];  // Theta of centre of belly plates: IsBelly(th,phi)
Double_t QOptics::fBellyPhi[11];    // Phi of centre of belly plates: IsBelly(th,phi)
//______________________________________________________________________________
QPMTuvw* QOptics::fPMTuvw;     // pointer to PMT/PSUP panel normal vectors
//______________________________________________________________________________
TVector3 QOptics::fRopeX[20];  // ropes vertically above this point
TVector3 QOptics::fRopeU;      // vectors are filled in constructor

Double_t QOptics::fPMTReflectorRadius = 13.55; // pmt radius in cm
Double_t QOptics::fLaserballRadius = 5.45;     // laserball radius in cm
Double_t QOptics::fNCDRadius = 2.579;          // average ncd outer radius in cm
//______________________________________________________________________________
QMath* QOptics::fQM;  // pointer to QMath object for finding rope intersections
//______________________________________________________________________________

// The following data taken from Richard Ford's own OCA code, still in
// ~moffat/sno/snoman/3_0187/oca/ as of 13-Nov-1999.  The origin of this
// table before that is unknown for the moment - does it correspond exactly
// to something in SNOMAN?  Hmmmmm...

Double_t QOptics::fPMTAngRespCos[100] = {
  0.1346633,     0.1357469,     0.1367958,     0.1378035,
  0.1387619,     0.1396620,     0.1404937,     0.1412463,
  0.1419100,     0.1424767,     0.1429415,     0.1433037,
  0.1435669,     0.1437375,     0.1438242,     0.1438358,
  0.1437808,     0.1436665,     0.1434984,     0.1432808,
  0.1430160,     0.1427045,     0.1423446,     0.1419311,
  0.1414526,     0.1408844,     0.1401727,     0.1392214,
  0.1379638,     0.1364584,     0.1347924,     0.1330110,
  0.1311291,     0.1291476,     0.1270595,     0.1248516,
  0.1225036,     0.1199857,     0.1172537,     0.1142414,
  0.1108518,     0.1069486,     0.1023538,     0.9685797E-01,
  0.9024896E-01, 0.8235972E-01, 0.7313160E-01, 0.6269641E-01,
  0.5151138E-01, 0.4063156E-01, 0.3191504E-01, 0.2669742E-01,
  0.2404030E-01, 0.2244352E-01, 0.2113256E-01, 0.1982216E-01,
  0.1842962E-01, 0.1694937E-01, 0.1540383E-01, 0.1382381E-01,
  0.1224055E-01, 0.1068271E-01, 0.9175452E-02, 0.7740440E-02,
  0.6396350E-02, 0.5159342E-02, 0.4043359E-02, 0.3059987E-02,
  0.2217787E-02, 0.1521095E-02, 0.9687846E-03, 0.5534880E-03,
  0.2617833E-03, 0.7540999E-04, 0.0000000E+00, 0.0000000E+00,
  0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00,
  0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00,
  0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00,
  0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00,
  0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00,
  0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00   };

//______________________________________________________________________________
TVector3 QOptics::fAnchorsPos[100]; // Positions of the NCD anchors (already corrected to the PSUP reference)
Int_t    QOptics::fNumAnchors = 0;


//______________________________________________________________________________
ClassImp(QOptics)
;
//______________________________________________________________________________
//
// Optics class.
//
// Returns optics parameters for the acrylic vessel, calculates media travel
// distances through d2o, acrylic and h2o.
//
//______________________________________________________________________________
QOptics::QOptics(Double_t sx, Double_t sy, Double_t sz)
  : source(sx,sy,sz), psup_centre(0,0,0), psup_centre_target(0,0,0), av_centre(0,0,0)
{
  // QOptics constructor.
  //
  // Reads panel data from SNOMAN file "geometry.dat", from bank GEDP 201.
  // Looks for this file in current working directory.
  // Eventually, should try $SNO_ROOT/snoman/$SNO_SNOMAN_VER/prod/geometry.dat.
  //
  // The default constructor is provided by setting the arguments to 0 in the
  // header file (QOptics.h).

  if (!(gSNO->GetPMTxyz())) {    // Ensure PMT geometry is loaded
    printf("Unable to locate PMT's from gPMTxyz in QOptics::QOptics()\n");
    exit(-1);
  }

  if (!fQOpticsInitialized) { // Load up the acrylic panels' layout from SNOMAN

    /*
    char* filenameString;
    char* qsnoroot = getenv("QSNO_ROOT");
    // the two +1 bellow are to emphasize that strlen does not take null 
    // terminator into account
    if( qsnoroot!=NULL ) {
      filenameString = 
	new char[strlen(qsnoroot)+1+strlen("/qoca/geometry.dat")+1];
    } else {
      printf("QOptics requires that you have the QSNO_ROOT environment variable set.\n");
      qsnoroot = "/usr/home/qusno/qsno_prod";
      printf("Trying a default: %s\n",qsnoroot);
      printf("...but think twice about what you are doing!\n");
      filenameString = 
	new char[strlen("/usr/home/qusno/qsno_prod/qoca/geometry.dat")];
    }
    sprintf(filenameString,"%s/qoca/geometry.dat",qsnoroot);
    */
 
    // 10.2006 -- N. Barros
    // Introduced hardcoded dimensions of the NCD anchors
    
    // The following values are in cm
    fAnchorRadius = 3.81;
    fAnchorHeight = 5.715; 
    
    // 05.2006 -- O.Simard
    // Commented the above.
    // Change the qoca/geometry.dat to the /prod version.
    Char_t* filenameString = new Char_t[1024];
    gSNO->GetSnomanTitles("geometry.dat",filenameString);

    Int_t i,j,kp;

    //--------------------
    // AV panels read in from GEDP 201
    QTitles* qpanels = new QTitles(filenameString,"GEDP",201);

    for(kp=i=0;i<10;i++)
      {
	fAVPanelsInRow[i]=(Int_t)qpanels->GetFWord(i*4 + 1);
	fAVPanelTheta[i]=atan(qpanels->GetFWord(i*4 +2));
	// fAVPanelPhi[i]=new Double_t[1+fAVPanelsInRow[i]];  // allocation now general to [][20]
	for(j=0;j<fAVPanelsInRow[i];j++)
	  {
	    fAVPanelPhi[i][j]=qpanels->GetFWord(41+kp*3); 
	    kp++;
	  }
	fAVPanelPhi[i][fAVPanelsInRow[i]]=qpanels->GetFWord(39+kp*3);  // last panel boundary
      }
    fAVPanelTheta[10]=M_PI;    // since atan(qpanels->GetFWord(39)); not quite pi...
    fAVPanelsTotal[0]=0;
    for(i=0;i<9;i++) fAVPanelsTotal[i+1] = fAVPanelsTotal[i]+fAVPanelsInRow[i];
    
    for (i=0; i<10; i++) {
      fAVPanelTheta[i] = fabs(fAVPanelTheta[i]-M_PI_2);      // Put theta on [0,pi]
      // Read in [-pi/2,pi/2], with 0 at equator, -pi/2 south pole, pi/2 north pole
    }

    delete qpanels;

    //--------------------
    // Belly plate centres, for use with IsBelly(th,phi) in fBelly cut on
    // optics liable to have problems.
    // Basically, fBellyTheta[i] = 0 and fBellyPhi[i] = 36*i degrees... But read
    // from titles file for "good luck" (NB. Change from SNOMAN 4_0083 -> 4_0084)
    QTitles* qbelly = new QTitles(filenameString,"GEDP",310);

    for (i=0;i<10;i++) {
      fBellyTheta[i] = 90.0 * M_PI/180.0;
      fBellyPhi[i] = atan2(qbelly->GetFWord(2+i*2), qbelly->GetFWord(1+i*2)); // [-pi:pi]
      if (fBellyPhi[i]<0) fBellyPhi[i] += M_2PI; // ensure range [0:2pi]
    }

    fBellyTheta[10] = fBellyTheta[0];
    fBellyPhi[10] = fBellyPhi[0] + M_2PI; // 0 deg and 360 deg bin overlap...

    // From GEDP 310, we get tan(phi_halfrange) = 0.133501259, so
    // phi_halfrange = 7.604 degrees.  Enlarge this 9.000 degrees, to include
    // more light rays near the edge of the belly plates.
    Setthbellyrange(); // Set to default (9 degrees?)
    Setphbellyrange(); // Set to default (9 degrees?)

    delete qbelly;

    //--------------------
    // Normalize angular response so that normal incidence has unit response
    for (i=1; i<100; i++) fPMTAngRespCos[i] /= fPMTAngRespCos[0];
    fPMTAngRespCos[0] = 1.0;

    if (DEBUG_QOptics) {
      for (i=0; i<10;i++) {
	printf("fAVPanelsInRow[i] = %5i[%5i] \n",fAVPanelsInRow[i],i);
	printf("fAVPanelsTotal[i]= %5i[%5i] \n",fAVPanelsTotal[i],i);
	printf("Theta %8.3g Phi ",fAVPanelTheta[i]*180/M_PI);
	for (j=0; j<fAVPanelsInRow[i]; j++) printf("%8.3g ",fAVPanelPhi[i][j]*180/M_PI);
	printf("\n");
	printf("Total number of panels %i\n",GetNumberOfPanels());
      }
      printf("\n");
      printf("Belly plates:\n");
      printf("Theta half-range: %12.8f Phi half-range: %12.8f\n",
	     thbellyrange*180/M_PI, phbellyrange*180/M_PI);
      for (i=0; i<11; i++) {
	printf("%d (%8.4g,%8.4g) ",i,fBellyTheta[i]*180/M_PI,fBellyPhi[i]*180/M_PI);
	if (i%5 == 1) printf("\n");
      }
      printf("\n");
    }

    printf("Attempting to create PMT/PSUP panel normals data structure.\n");
    fPMTuvw = new QPMTuvw();

    // Initialize ropes - ropes have a diameter of 1.35" -> 3.429cm
    Double_t roperadius = 605.03; // (239.7" - 0.75" - 0.75") * 2.54 cm/"
    Double_t ropetheta = 36.0;    // angle between pairs of ropes
    Double_t ropedelth = 4.782;   // angular offset of ropes (atan(19.925/238.2))
    Double_t anglerad = M_PI/180; // conversion factor
    for (i=0; i<10; i++) {
      fRopeX[2*i].SetX( roperadius * cos((i*ropetheta-ropedelth)*anglerad) );
      fRopeX[2*i].SetY( roperadius * sin((i*ropetheta-ropedelth)*anglerad) );
      fRopeX[2*i].SetZ( 0 );

      fRopeX[2*i+1].SetX( roperadius * cos((i*ropetheta+ropedelth)*anglerad) );
      fRopeX[2*i+1].SetY( roperadius * sin((i*ropetheta+ropedelth)*anglerad) );
      fRopeX[2*i+1].SetZ( 0 );
    }
    fRopeU.SetXYZ(0,0,1);
    Setropemindist();  // Set to default (15cm?)

    fQM = new QMath();
    
    printf("Loading default NCD array configuration\n");
    fNcdArray = new QNCDArray();
    if (!fNcdArray) printf("Couldn't open default NCD array configuration file.\n");

    delete filenameString;

    // 12.2006 N. Barros
    // The NCD Anchors positions are loaded from a text file in /qoca
    // Later this may be changed to a proper title file.
    // make sure the current directory is not changed
    Char_t* filenameString2 = new Char_t[1024];
    char* qsnoroot = getenv("QOCA_ROOT");

    if( qsnoroot!=NULL ) {
      filenameString2 = 
	new char[strlen(qsnoroot)+1+strlen("/data/ncd_anchors.dat")+1];
    } else {
      printf("QOptics requires that you have the QSNO_ROOT environment variable set.\n");
      qsnoroot = "/usr/home/qsno/qsno_prod";
      printf("Trying a default: %s\n",qsnoroot);
      printf("...but think twice about what you are doing!\n");
      filenameString2 = 
	new char[strlen("/usr/home/qsno/qsno_prod/qoca/ncd_anchors.dat")];
    }
    sprintf(filenameString2,"%s/data/ncd_anchors.dat",qsnoroot);

    Float_t anchorx = 999.9;
    Float_t anchory = 999.9;
    Float_t anchorz = 999.9;

    printf("----------------------------------------------\n");
    printf("Loading NCD Anchors from file %s.\n",filenameString2);
    printf("----------------------------------------------\n");
    
    FILE* infile = fopen(filenameString2,"r");
    if (infile == NULL){
      printf("Failed to open NCD Anchor positions file %s\n",filenameString2);
      exit(-1);
    }
    
    Int_t status = 0;
    Int_t p = 0;
    // let's proceed to loading the files
    while (((status = fscanf(infile,"%*s%f%f%f\n",&anchorx, &anchory, &anchorz)) != EOF) && (p != 100)){
      //     printf("-->Loaded anchor: (%f,%f,%f)\n",anchorx,anchory,(anchorz+5.));
      // Correction to the PSUP reference.
      // we know that the PSUP has a shift of 5cm downwards, compared with the center of the AV
      fAnchorsPos[p].SetXYZ(anchorx,anchory,(anchorz-5.));
      // The NCD anchor is a cylinder. So we also need to move the position of the anchor to the center
      // of the cylinder, instead of using the center in the AV.
      TVector3 pos(fAnchorsPos[p].Unit());
      Double_t distance = fAnchorsPos[p].Mag(); 
      fAnchorsPos[p] *= (1.-(fAnchorHeight/(2*distance)));
      p++;
    }
    fNumAnchors = p;
    fclose(infile);
    printf("----------------------------------------------\n");
    printf("Loaded %d NCD anchors from file\n",fNumAnchors);
    printf("----------------------------------------------\n");
    
    

    fQOpticsInitialized = kTRUE;
  }

  fNumberOfInstances++;  // Increment number of instances
  printf("Number of QOptics instances: %d\n",fNumberOfInstances);

  SetIndices(420.0);      // Sets fWavelength and nd, na, nh at lambda
  nd = GetIndexD2O();     // index of heavy water
  na = GetIndexAcrylic(); // index of acrylic
  nh = GetIndexH2O();     // index of light water
  
  rs = source.Mag();
  r1 = fAVInnerRadius;              // redundant
  r2 = fAVInnerRadius+fAVThickness;  // redundant
  rp = 850.0;           // must be set for each individual PMT for best results

  if (rs < r1) fSourceInside = kTRUE;
  else fSourceInside = kFALSE;

  fNumRefract = 0;
  fAcrylicVector.SetXYZ(0,0,0);
  fAcrylicVector2.SetXYZ(0,0,0);

  fPMTrelvec.SetXYZ(0,0,1);
  fPMTincident.SetXYZ(0,0,1);

  fTimeOfFlight = 0.0;

  fPanelNum = -1;  // Initialize these for each instance of the class
  fLayerNum = -1;
  bellyplate = kFALSE;
  fAnchor = kFALSE; // identifies if the last path calculate hits a NCD anchor

  acrylicneck = kFALSE;
  fNcdOptics  = kFALSE; // identifies if NCD geometry is present
  fNcd        = kFALSE;
  fRope = kFALSE;
  fPipe = kFALSE;
  fNcdRope = kFALSE;

  fNcdMinDist = 9999.;
  fNcdTolerance = 0.;

  fDd2o = 0;
  fDacrylic = 0;
  fDh2o = 0;
  fCtheta = 1.0;
  fTransPwr = 0;
  fPMTAngResp = 0;

  fCosThetapsMin = 9999.;  // default values
  fCosThetapsMax = -9999.;

  fCounterIndex = -1; // invalid counter index

  fFastMode = kFALSE; // tells if optical flag settings are skipped in GetDistances()

}
//______________________________________________________________________________
QOptics::~QOptics()
{
  // QOptics destructor.

  fNumberOfInstances--;  // Decrement number of instances
  printf("Number of QOptics instances: %d\n",fNumberOfInstances);
  if (fNumberOfInstances == 0) {
    printf("Doing final destructions of fPMTuvw, fQM and unsetting initialized flag.\n");
    delete fPMTuvw;
    delete fQM;
    delete fNcdArray;
    fQOpticsInitialized = kFALSE;
  }
}
//______________________________________________________________________________
void QOptics::SetFastMode(Bool_t aFlag, Bool_t kPrint)
{
  // Sets the fast mode flag (see GetDistances).
  // If set to true, the optical flags are no set, and only the distances,
  // angles, time of flights, etc. are found. 
  // The second argument is to get rid of the printing statements.

  if(kPrint) printf("QOptics::SetFastMode: ");
  if(aFlag){
    if(kPrint) printf("Enabling fast mode. No optical flags will be set in GetDistances.\n");
  } else {
    if(kPrint) printf("Optical flags will be set in GetDistances.\n");
  }
  fFastMode = aFlag;
  return;
}
//______________________________________________________________________________
void QOptics::ResetNcdPositions(Char_t* aFile)
{
  // Reset the Ncd positions according to aFile. No change is made 
  // if the fNcdOptics flag is not turned on.

  if(!fNcdOptics) printf("QOptics::ResetNcdPositions: Ncd Positions won't be updated.\n");
  else { // Update positions
    if(fNcdArray) delete fNcdArray;
    printf("Loading user NCD array configuration:\n\t%s\n",aFile);
    fNcdArray = new QNCDArray(aFile);
    // same message as in constructor
    if (!fNcdArray) printf("Couldn't open user NCD array configuration file.\n");
  }
  
  return;
}
//______________________________________________________________________________
Int_t QOptics::GetNumRefract()
{
  // Return the number of refractions in the latest path calculated.
  // 0 = DIRECT (source outside AV, direct line of sight to PMT)
  // 1 = inside (source inside AV, refraction at AV on way to PMT)
  // 2 = outside (source outside AV, refraction on entering and exiting AV)

  return fNumRefract;
}
//______________________________________________________________________________
TVector3 QOptics::GetAcrylic()
{
  // Return vector to refraction point in middle of acrylic, as stored by
  // latest call to GetDistances()

  return fAcrylicVector;
}
//______________________________________________________________________________
TVector3 QOptics::GetAcrylic2()
{
  // Return vector to refraction point in middle of acrylic, as stored by
  // latest call to GetDistances() - for second intersection, in the case
  // of the source outside the AV and light ray through the d2o.

  return fAcrylicVector2;
}
//______________________________________________________________________________
TVector3 QOptics::GetPMTrelvec()
{
  // Return vector from source towards PMT (NB. different in general from the
  // vector directly from the source to the PMT - this is the vector of the
  // light path leaving the source.

  return fPMTrelvec;
}
//______________________________________________________________________________
TVector3 QOptics::GetPMTincident()
{
  // Return vector of light incident on the PMT in the last calculated path.

  return fPMTincident;
}
//______________________________________________________________________________
Int_t QOptics::GetPanel()
{
  // Return the panel last found by GetDistances()

  return fPanelNum;
}
//______________________________________________________________________________
Int_t QOptics::GetPanel(Double_t th, Double_t ph)
{
  // Return the panel located at angular coordinates (th,ph).

  Int_t i,j;
  
  if (th < 0 || th > M_2PI) th = fmod(th,M_2PI);  // ensure [-2pi:2pi] range
  if (th < 0) th += M_2PI;                        // ensure [0:2pi] range
  if (th > M_PI) th = M_2PI - th;                 // ensure [0:pi] range

  if (ph < 0 || ph > M_2PI) ph = fmod(ph,M_2PI);  // ensure [-2pi:2pi] range
  if (ph < 0) ph += M_2PI;                        // ensure [0:2pi] range
  if (ph > M_PI) ph -= M_2PI;                     // ensure [-pi:pi] range

  for(i=1; i<11; i++)if(fAVPanelTheta[i] >= th)break;  // fAVPanelTheta[i] increasing on [0,pi]
  i--;
  for(j=0;j<fAVPanelsInRow[i];j++) if(fAVPanelPhi[i][j] <= ph) break;
  if(j==fAVPanelsInRow[i]) { // NB some panels start before phi=pi !
    if (fAVPanelPhi[i][0] - M_2PI <= ph) j = fAVPanelsInRow[i] - 1;
    else if (fAVPanelPhi[i][1] - M_2PI <= ph) j=0;
    else if (fAVPanelPhi[i][2] - M_2PI <= ph) j=1;
    else printf("Shouldn't get here in QOptics::GetPanel()! %f %f \n",th,ph);
  }
  else j--;
  i=fAVPanelsTotal[i]+j;
  return i;
}
//______________________________________________________________________________
Int_t QOptics::GetLayer()
{
  // Return the layer number last located by GetDistances()

  return fLayerNum;
}
//______________________________________________________________________________
Int_t QOptics::GetLayer(Int_t panelnumber)
{
  // Return the layer number for panel = panelnumber

  Int_t i;

  if(panelnumber<0 || panelnumber>121 ) return -1;
  for(i=1; i<10; i++) if(panelnumber<fAVPanelsTotal[i]) break;
  return i;
}
//______________________________________________________________________________
Int_t QOptics::GetLayer(Double_t th)
{
  // Return the layer number located at azimuthal angle th.
  // th=0 corresponds to the z-axis (straight up), th=pi is the south pole.

  Int_t i;

  if (th < 0 || th > M_2PI) th = fmod(th,M_2PI);  // ensure [0,2pi] range
  if (th > M_PI) th = M_2PI - th;                   // ensure [0,pi] range

  for(i=1; i<11; i++)if(fAVPanelTheta[i] >= th)break;  // fAVPanelTheta[i] increasing on [0,pi]
  i--;
  return i;
}
//______________________________________________________________________________
Int_t QOptics::GetNumberOfPanels()
{
  // Returns the total number of acrylic panels in the detector.
  // Range is [0 .. GetNumberOfPanels()-1]

  return fAVPanelsTotal[9]+fAVPanelsInRow[9];
}
//______________________________________________________________________________
Bool_t QOptics::IsBelly()
{
  // Returns the value of IsBelly(th,ph) for the latest path calculated.
  // See QOptics::GetDistances() for the actual call.

  return bellyplate;
}
//______________________________________________________________________________
Bool_t QOptics::IsBelly(Int_t panelnumber)
{
  // Returns kTRUE if panelnumber corresponds to a belly plate.

  if (panelnumber >= 50 && panelnumber < 70 && panelnumber % 2 ==0) return kTRUE;
  else return kFALSE;
}
//______________________________________________________________________________
Bool_t QOptics::IsBelly(Double_t th, Double_t ph)
{
  // Returns kTRUE if panel at (th,ph) corresponds to a belly plate.
  //
  // 26-Nov-2000 - Moffat
  // Increased the size of the belly plate criterion from 15.582 degrees to
  // 18 degrees to allow for misplaced belly plates or marginally sure optics
  // for light rays passing near the border of the plates.
  //
  // This replaces the old version which simply called IsBelly(GetPanel(th,ph));

  // Enlarge the belly plate region artificially.
  // Usual belly plates are from +/- 7.791 deg in theta, and distributed in phi.
  // This corresponds to 82.209 to 97.791 deg cut in theta.
  // The cut here will be 18 degrees, but sampled around in phi properly.
  // 24,26-Nov-2000 - Moffat

  Int_t i;

  if (th < 0 || th > M_2PI) th = fmod(th,M_2PI);  // ensure [-2pi:2pi] range
  if (th < 0) th += M_2PI;                        // ensure [0:2pi] range
  if (th > M_PI) th = M_2PI - th;                 // ensure [0:pi] range

  if (ph < 0 || ph > M_2PI) ph = fmod(ph,M_2PI);  // ensure [-2pi:2pi] range
  if (ph < 0) ph += M_2PI;                        // ensure [0:2pi] range

  for (i=0; i<11; i++) {
    if ((ph > fBellyPhi[i]-phbellyrange) && (ph < fBellyPhi[i]+phbellyrange)) {
      if ((th > fBellyTheta[i]-thbellyrange) && (th < fBellyTheta[i]+thbellyrange))
	return kTRUE;
      else break;  // if not in theta range for phi slice, forget it!
    }
  }

  return kFALSE;
}
//______________________________________________________________________________
Bool_t QOptics::IsNeckOptics()
{
  // Returns the value of IsNeckOptics(th) || IsChimney(a,p) for the latest path
  // calculated.  See QOptics::GetDistances() for the actual call which sets
  // acrylicneck.
  
  return acrylicneck;
}
//______________________________________________________________________________
Bool_t QOptics::IsNeckOptics(Double_t th)
{
  // Simple geometric cut: if the point at azimuthal angle th (theta) is within
  // 30cm of the neck ring boss, return kTRUE.
  // th specified in radians.

  // First, make sure th is in top half of detector.  Then, check that its sine
  // is less than the cylindrical radius/av radius corresponding to 30cm beyond
  // the neck boss.  NB. This is a little approximate; done with SNO graph paper.
  //
  // The last check is to ensure that th is not inside the rubbing ring.

  if ((th<M_PI_2) && (sin(th)<(133.0/fAVInnerRadius)) && (sin(th)>(48.0/fAVInnerRadius))) return kTRUE;
  else return kFALSE;
}
//______________________________________________________________________________
Bool_t QOptics::IsChimney(TVector3 &a, TVector3 &p, Double_t delrin,
			  Double_t delrout)
{
  // Check whether the light ray from a to p goes through the acrylic vessel
  // chimney.  The chimney is a set of two cylinders, radii rinner and router.
  // Intersection is based on a + alpha*(v_unit) giving a point of intersection
  // on either cylinder for positive alpha.  v_unit = (p-a).Unit() is the unit
  // vector between the source (a) and destination (p).
  //
  // Chimney ID 57.4" = 146cm
  // Chimney thickness 2.16" = 5.5cm (approx)
  // Neck boss is somewhat bigger, something like 14cm thick.
  // Rubbing ring ID 38" = 96.5cm
  // Height is from 4" = 10.2cm below boss on chimney/AV thick joint
  //           to 1286cm above the AV centre (elevation 98.823feet,
  //                                          AV centre nominally at 56.625feet)
  //
  // delrin - inner radius changed by this amount to allow for path innaccuracies
  // delrout - outer radius changed by this amount

  Bool_t intersect = kFALSE;

  Double_t rinner =  96.5 + delrin;
  if (rinner<0) rinner = 0;
  Double_t router = 146.0 + 5.5 + delrout;
  if (router<rinner) router = rinner;

  Double_t zlow  =  588.0;
  Double_t zhigh = 1286.0;

  Double_t alpha1,alpha2;
  TVector3 point1,point2;
  TVector3 v;
  v = (p - a).Unit();

  //--------------------
  // Check inner radius intersection.
  Double_t disc = 2*v(0)*a(0)*v(1)*a(1) - v(0)*v(0)*(a(1)*a(1) - rinner*rinner)
    - v(1)*v(1)*(a(0)*a(0) - rinner*rinner);
  Double_t denom = v(0)*v(0) + v(1)*v(1);
  if (disc>0) { // two real roots
    alpha1 = (-(v(0)*a(0)+v(1)*a(1)) + sqrt(disc)) / denom;
    alpha2 = (-(v(0)*a(0)+v(1)*a(1)) - sqrt(disc)) / denom;

    point1 = a + v*alpha1;
    point2 = a + v*alpha2;

    if (alpha1>0 && point1(2) > zlow && point1(2) < zhigh) intersect = kTRUE;
    if (alpha2>0 && point2(2) > zlow && point2(2) < zhigh) intersect = kTRUE;

  } else if (disc==0 && denom!=0) { // one real root

    alpha1 = -(v(0)*a(0)+v(1)*a(1)) / denom;
    point1 = a + v*alpha1;
    if (alpha1>0 && point1(2) > zlow && point1(2) < zhigh) intersect = kTRUE;
    
  } // otherwise, two imaginary roots...

  //--------------------
  // Check outer radius intersection.
  disc = 2*v(0)*a(0)*v(1)*a(1) - v(0)*v(0)*(a(1)*a(1) - router*router)
    - v(1)*v(1)*(a(0)*a(0) - router*router);
  denom = v(0)*v(0) + v(1)*v(1);
  if (disc>0) { // two real roots
    alpha1 = (-(v(0)*a(0)+v(1)*a(1)) + sqrt(disc)) / denom;
    alpha2 = (-(v(0)*a(0)+v(1)*a(1)) - sqrt(disc)) / denom;

    point1 = a + v*alpha1;
    point2 = a + v*alpha2;

    if (alpha1>0 && point1(2) > zlow && point1(2) < zhigh) intersect = kTRUE;
    if (alpha2>0 && point2(2) > zlow && point2(2) < zhigh) intersect = kTRUE;

  } else if (disc==0 && denom!=0) { // one real root

    alpha1 = -(v(0)*a(0)+v(1)*a(1)) / denom;
    point1 = a + v*alpha1;
    if (alpha1>0 && point1(2) > zlow && point1(2) < zhigh) intersect = kTRUE;
    
  } // otherwise, two imaginary roots...

  //--------------------
  return intersect;
}
//______________________________________________________________________________
Bool_t QOptics::IsNeckPanel(Int_t panelnumber)
{
  // Returns kTRUE if panelnumber is at the top of the detector.

  if (panelnumber < fAVPanelsInRow[0]) return kTRUE;
  else return kFALSE;
}
//______________________________________________________________________________
Bool_t QOptics::IsNeckPanel(Double_t th, Double_t ph)
{
  // Returns kTRUE if panel at (th,ph) is at the top of the detector.

  return IsNeckPanel(GetPanel(th,ph));
}
//______________________________________________________________________________
Bool_t QOptics::IsPipe()
{
  // Returns pipe variable from previos calculation.

  return fPipe;
}
//______________________________________________________________________________
Bool_t QOptics::IsPipe(TVector3 &a)
{
  // From position on acrylic, a, figures out if path is near a pipe.
  
  Double_t theta = 180*a.Theta()/M_PI;
  Double_t phi = 180*a.Phi()/M_PI;

  fPipe = kFALSE;
  if( phi<75 || phi>105 ) return kFALSE;
  
  if( theta<30 && (phi<75 || phi>105) ) return kFALSE;
  if( theta>=30 && theta<120 && (phi<80 || phi>100) ) return kFALSE;
  if( theta>=120 && (phi<85 || phi>95) ) return kFALSE;
  
  fPipe = kTRUE;
  return kTRUE;
}
//______________________________________________________________________________
Bool_t QOptics::IsSourceInside()
{
  // Returns whether source was inside AV in previous calculation

  return fSourceInside;
}
//______________________________________________________________________________
Bool_t QOptics::IsRope()
{
  // Returns the value of IsRope(a,p) for the latest path calculated.
  // See QOptics::GetDistances() for the actual call.

  // 10-Mar-2000 returns kFALSE because IsRope(a,p) routine needs debugging.

  return fRope;
}
//______________________________________________________________________________
Bool_t QOptics::IsRope(TVector3 &a, TVector3 &p)
{
  // Figures out the distance of closest approach between the path from the
  // outside of the AV to the PMT.
  //
  // Only considers vertical part of rope. Checks to ensure intersection is in
  // top half of rope.
  //
  // The radius of the rope is set wide to take into account that the pmt is not
  // really a point.
  //
  // This cut is designed to get rid of all the carp (in the light water -
  // specially acclimatized by Chris Jillings to breathe a pure nitrogen
  // solution in the water :) at the expense of losing good tubes.

  //  return kFALSE; // 21-7-00 Stopgap to prevent bogus results...?

  Int_t i,j;
  Double_t distance;
  Bool_t isRope = kFALSE;

  TVector3 u0 = p-a;  // from av (initial point) to pmt (final point)
  TVector3 u1;        // direction vector along closest approach
  TVector3 u2(0,0,1); // direction vector of the rope
  TVector3 t;         // coefficients in solution
  TVector3 R;         // resultant = fRope[i] - a  // in explicit equation sol'n

  u0 = u0.Unit();
  TVector3 point1,point2,point;  // some debugging variables

  //--------------------
  // Explicit solution for vertical ropes (0,0,1)
  u2 = u2.Unit();
  u1 = (u0.Cross(u2)).Unit();

  for (i=0; i<20 && !isRope; i++) {
    R = fRopeX[i] - a;
    t.SetX( (R(0)+u0(1)/u0(0)*R(1))/(u0(0)+u0(1)*u0(1)/u0(0)) );
    t.SetY( (R(0)*u0(1)*u0(0) - R(1)*u0(0)*u0(0)) / (u0(0)*u0(0)+u0(1)*u0(1)) );
    t.SetZ( (R(0)*u0(0)*u0(2) + R(1)*u0(1)*u0(2) - R(2)*(u0(0)*u0(0)+u0(1)*u0(1)))
	    / (u0(0)*u0(0)+u0(1)*u0(1)) );

    point1 = a + t(0)*u0;
    point2 = fRopeX[i] + t(2)*u2;
    point = a + t(0)*u0 + t(1)*u1;
    //    distance = fabs(t(1));
    distance = (point2-point1).Mag();

    if (distance<ropemindist && t(2)>0 && t(0)>0) {
      if (point2.Mag() < 850.0) isRope = kTRUE;  // Make sure point on rope inside PSUP!
    }
  }

  fRope = isRope;
  return isRope;

  //--------------------
  // Chris' original routine follows, with definitions for u0,u1 and u2 changed to
  // conform to usage in explicit solution.
  isRope = kFALSE;
  fRope = kFALSE;

  u0 = p-a;
  u0 = u0.Unit();
  Float_t** m = fQM->matrix(1,3,1,3);  // for QMath::gaussj
  Float_t** v = fQM->matrix(1,3,1,1);

  Int_t solutionOK;

  for (i=0; i<20 && !isRope; i++) { // note condition
    u2 = fRopeU.Unit();
    u1 = (u0.Cross(u2)).Unit();
    // Build Matrix
    for (j=1; j<=3; j++) {
      m[j][1] =  u0[j-1];  // shortest distance vector
      m[j][2] =  u1[j-1];  // light ray vector
      m[j][3] = -u2[j-1];  // rope vector
      v[j][1] = fRopeX[i][j-1] - a[j-1];
    }
    solutionOK = fQM->gaussj(m,3,v,1);
    //    t.SetXYZ( (Double_t)(v[1][1]) , (Double_t)(v[2][1]) , (Double_t)(v[3][1]) );
    t.SetXYZ( v[1][1] , v[2][1] , v[3][1] );

    // Check to see where on the rope vector the position of closest approach is.
    // If z>0 AND intersection is on "forward" part of light ray path, then rope
    // intersection is valid.
    point1 = a + u0 * t[0];
    point2 = fRopeX[i] + u2*t[2];
    point =  a + u0 * t[0] + u1*t[1];
    distance = (point2-point1).Mag();

    if (distance<ropemindist && t[2]>0 && t[0]>0) {
      isRope = kTRUE;
    }
  }
  
  fQM->free_matrix(m,1,3,1,3);
  fQM->free_matrix(v,1,3,1,1);

  fRope = isRope;
  return isRope;
}
//______________________________________________________________________________
Bool_t QOptics::IsPromptAVRefl()
{
  // Return the last estimated value (by GetDistances()) of fPromptAVRefl.

  return fPromptAVRefl;
}
//______________________________________________________________________________
Bool_t QOptics::IsPromptAVRefl(TVector3 &s, TVector3 &p)
{
  // Very simplistic criterion for possible contamination by prompt AV reflections.
  //
  // Inside AV only known to be a problem beyond 500cm radius.
  // Outside AV unknown...
  //
  // 05.2004 - O.Simard
  // Removed the "very simplistic criterion" to radius dependent criterion to use
  // with QOCAReflect and QPath::CalculatePaths().
  // The limit on the source radius is set for AV reflections only. Other
  // reflections should be taken care of with the addition of new functions.

  fPromptAVRefl = kFALSE;

  Float_t smag = s.Mag();
  Double_t cos_thetaps = p.Unit() * s.Unit(); // parameter -- os

  // New way of selecting high radius pmt data. Must verify if the values
  // have been set before proceeding. Else do it the old way.
  if((smag >= 450) && (smag <= 580)){
    if((fCosThetapsMin != 9999.) && (fCosThetapsMax != -9999.0)){
      if((cos_thetaps > fCosThetapsMin)
	 && (cos_thetaps < fCosThetapsMax)){fPromptAVRefl = kTRUE;} // new criterion
    }
    else{
      if(cos_thetaps < 0.1){fPromptAVRefl = kTRUE;} // previous criterion
    }
  }
  
  // For much higher radii: flag anyway: do not try source positions
  // outside the AV (light water optics) with this function.
  if((smag > 580) && (smag < 620)){fPromptAVRefl = kTRUE;}

  return fPromptAVRefl;
}
//______________________________________________________________________________
Bool_t QOptics::IsNcd()
{
  // Return the last estimated value (by GetDistances()) of fNcd

  return fNcd;
}
//______________________________________________________________________________
Bool_t QOptics::IsNcd(TVector3 &s, TVector3 &d)
{
	// Ncd mask function

	Int_t counter;
	fCounterIndex = -1; // reset counter index
	fNcdMinDist = fNcdArray->ImpactParameter(s,d,counter);
	// get counter information
	fCounterIndex = counter;
	if (counter >= fNcdArray->GetNCounters() || counter < 0 || !fNcdArray->GetCounter(counter)){
		fNcdMinDist = 999.;
	} else {
		// if a ray traverses this counter already, 
		// flag right away and skip lengthy calculations
		if(fNcdMinDist - fNCDRadius < 0) return kTRUE;
		else {
		  Double_t x1 = (fNcdArray->GetCounter(counter)->GetClosestPoint(s,d) - s).Mag();
  		  Double_t x2 = fDd2o + fDacrylic + fDh2o;
  		  // This is the radius of the "solid cone" defined by the laserball
  		  // and the PMT
  		  Double_t y1 = (fCtheta*fPMTReflectorRadius - fLaserballRadius)*x1/x2 + fLaserballRadius;
		  fNcdMinDist -= fNCDRadius;	//subtract counter radius from distance
		  fNcdMinDist -= y1;      //subtract light cone radius from distance
		  fNcdMinDist /= (1 - x1/x2); // correct it to the source position
		}
	}
  	if(fNcdMinDist < fNcdTolerance) fNcd = kTRUE;
	else fNcd = kFALSE;

  return fNcd;
  
}
//______________________________________________________________________________
TVector3 QOptics::GetPMTPosition(Int_t pmtn)
{
  // Returns a TVector3 containing the PMT's coordinates.

  TVector3 pmt(gPMTxyz->GetX(pmtn),gPMTxyz->GetY(pmtn),gPMTxyz->GetZ(pmtn));
  pmt += psup_centre;
  //pmt += 6.0*GetPMTNormal(pmtn);  //add offset-- ALH temporary change.
  return pmt;
}
//______________________________________________________________________________
TVector3 QOptics::GetPMTNormal(Int_t pmtn)
{
  // Returns a TVector3 containing the normal vector to the PMT specified.
  // Currently only a crude estimate, based on a radial vector approximation.
  // True geometry has each PMT as a member of a PSUP panel; the panels each
  // face the "centre" of the PSUP.
  //
  // Modified 3-Dec-99 to include reference to Chris Jillings' class QPMTuvw()
  // which will load the PSUP panel normals, and associate them with PMT's
  // using logical channel numbers (ie. pmtn as standard in QSNO).

  TVector3 pmtnormal;

  if (fPMTuvw) {
    pmtnormal = (fPMTuvw->GetPMTDir(pmtn)).Unit();
  } else {
    printf("No fPMTuvw pointer!\n");
    printf("Resorting to old radial vector for PMT normals.\n");
    pmtnormal.SetXYZ(gPMTxyz->GetX(pmtn),gPMTxyz->GetY(pmtn),gPMTxyz->GetZ(pmtn));
    pmtnormal -= psup_centre_target;
    pmtnormal = pmtnormal.Unit();
  }
  
  return pmtnormal;
}
//______________________________________________________________________________
QPMTuvw *QOptics::GetPMTuvw()
{
  // Returns a pointer to the QPMTuvw object created by QOptics.

  return fPMTuvw;
}
//______________________________________________________________________________
Bool_t QOptics::GetDistances(Int_t pmtn)
{
  // Calls GetDistances() and uses class variables as arguments.
  //
  // Use GetXXX() to retrieve distances, cos(theta_pmt), panel number or
  // transmitted power.

  return GetDistances(pmtn,fDd2o,fDacrylic,fDh2o,fCtheta,fPanelNum,fTransPwr);
}
//______________________________________________________________________________
Bool_t QOptics::GetDistances(Int_t pmtn,
			     Double_t &d2o, Double_t &acrylic, Double_t &h2o,
			     Double_t &ctheta,
			     Int_t &avpanel,
			     Double_t &transpwr)
{
  // Calls GetDistances() and fills passed variables with results if successful.

  // --------------------
  // Test whether target is a valid PMT on the PSUP, and check where the source is.
  if (gPMTxyz->IsInvalidPMT(pmtn) || !(gPMTxyz->IsNormalPMT(pmtn))) return kFALSE;

  TVector3 pmtvec = GetPMTPosition(pmtn);
  if (pmtvec(0) == -9999) return kFALSE;  // PMT without a position...?

  // Get normal vector to the pmt (points outwards, away from the centre of the
  // detector.)
  fPMTNormal = GetPMTNormal(pmtn);

  if (GetDistances(pmtvec,fPMTNormal)) {

    d2o = GetDd2o();
    acrylic = GetDacrylic();
    h2o = GetDh2o();
    ctheta = GetCtheta();
    avpanel = GetPanel();
    transpwr = GetTransPwr();
    return kTRUE;

  } else return kFALSE;
}
//______________________________________________________________________________
Bool_t QOptics::GetDistances(TVector3 p, TVector3 n)
{  
  // GetDistances calculates the direct distance from a source to a position,
  // assuming the source is inside the av and p (pmt vector coordinate) is
  // outside.  It returns the geometric distance through the d2o, the h2o, and
  // the acrylic, including the panel number.
  //
  // n is the PMT normal vector (points outwards!).
  //
  // Also returns the transmitted power (intensity) based on Fresnel light
  // transmission coefficients.
  //
  // Extracted from QPath routines by A. Hallin, Queen's University, July 1999.
  // Modified for spherical refraction 28-9-99 by Bryce Moffat.
  //
  // For now only works if we start inside d2o and go to PMT's.
  //
  // Calculates time of flight.  Chris Jillings 20-2-00
  // 
  // Added paths for sources outside AV:
  //   - direct line of sight to PMT - Chris Jillings 8-3-00
  //   - path through AV with refraction - Bryce Moffat 21-7-00
  //
  // Returns kTRUE if paths calculated correctly; kFALSE indicates a problem,
  // and none of the return values will be set correctly.

  //--------------------
  // Check for source outside the AV:
  if (!fSourceInside) return getDistancesOutside(p,n);

  fDd2o = fDacrylic = fDh2o = -9999;
  fTimeOfFlight = -9999; // set to error value.

  rp = p.Mag();  // PMT radius in class variable
  
  if (rp < r2) return kFALSE;  // PMT must be outside the AV...?
  if (rs > rp) return kFALSE;  // Source shouldn't be outside PSUP/PMT!

  // --------------------
  // Set up local coordinate system:
  // xunit - points along radial direction from origin to source position
  // zunit - perpendicular to plane containing origin, source, and pmt
  // yunit - perpendicular to source vector; in plane of source and pmt vectors
  
  // Set source() to something small to make sure vector algebra all works!
  if (rs == 0.0) SetSource(1e-5,0,0);
  TVector3 xunit = source.Unit();
  TVector3 zunit = (xunit.Cross(p)).Unit();
  TVector3 yunit = (zunit.Cross(xunit)).Unit();
  
  thPMTtarget = atan2((p.Cross(source)).Mag(),p*source);
  
  Double_t theta = rtsafe(0,M_PI,1e-3);
  Double_t th1 = theta1(theta);
  Double_t th2 = theta2(theta) + th1;
  //  Double_t th3 = theta3(theta) + th2;
  
  TVector3 a1 = cos(th1)*xunit + sin(th1)*yunit;
  a1.SetMag(r1);
  fAcrylicVector = a1; // set this vector here and reuse it
  
  TVector3 a2 = cos(th2)*xunit + sin(th2)*yunit;
  a2.SetMag(r2);

  // various vectors
  TVector3 direction(fAcrylicVector - source); // source to pmt direction
  TVector3 inter(a2 - fAcrylicVector); // intermediate (acrylic)
  TVector3 incident(p - a2); // incident on pmt surface
  
  fPMTrelvec = direction.Unit();
  fPMTincident = incident.Unit();
  
  // Cos(theta) of angle of incidence of light onto front face of PMT bucket
  fCtheta = (n * fPMTincident);

  fDd2o = direction.Mag();
  fDacrylic = inter.Mag();
  fDh2o = incident.Mag();
  // Does the light ray go through the acrylic vessel neck? or a bellyplate?
  if(fDacrylic > 0.0) fNumRefract = 1;
  else fNumRefract = 0;
  
  // Incident angles at the interfaces
  Double_t cosida = fAcrylicVector.Unit()*fPMTrelvec; // d2o/acrylic inc ang
  if (cosida > 1.0) cosida = 1.0;
  else if (cosida < -1.0) cosida = -1.0;
  Double_t incidentda = acos(cosida);
  
  Double_t cosiah = a2.Unit()*inter.Unit(); // acrylic/h2o incident ang
  if (cosiah > 1.0) cosiah = 1.0;
  else if (cosiah < -1.0) cosiah = -1.0;
  Double_t incidentah = acos(cosiah);
 
  // Transmission Power through interfaces 
  fTransPwr = 0.5 * (
		    GetTransPwrPara(nd,na,incidentda) *
		    GetTransPwrPara(na,nh,incidentah)
		    +
		    GetTransPwrPerp(nd,na,incidentda) *
		    GetTransPwrPerp(na,nh,incidentah) );
  
 
  // calculate the time of flight right away 
  fTimeOfFlight =
    fDd2o/GetVgroupD2O() + fDh2o/GetVgroupH2O() + fDacrylic/GetVgroupAcrylic();

  // Fast Mode: end this function here (no optical flag set)  
  if(fFastMode){
    if(fNumRefract) return kTRUE; // if(fDacrylic > 0.0)
    else return kFALSE;
  }

  // Below is a bunch of old definitions (still valid, just rewritten) 
  //fPMTrelvec = (a1 - source).Unit();
  //fPMTincident = (p - a2).Unit();
  //fDd2o = (a1-source).Mag();
  //fDacrylic = (a2-a1).Mag();
  //fDh2o = (p-a2).Mag();
  //fCtheta = (n * (p-a2).Unit());
  //fAcrylicVector = a1; // moved up
  //Double_t cosida = a1.Unit()*(a1-source).Unit(); // d2o/acrylic inc ang
  //Double_t cosiah = a2.Unit()*(a2-a1).Unit(); // acrylic/h2o incident ang
  //Double_t sinida = nd*sin(incidentda)/na;  // sine of refracted angle
  //Double_t siniah = nh*sin(incidentah)/na;  // sine of refracted angle
 

  // Below are optical flag settings (requires some more calculations)
  //

  // Neck Optics
  Double_t a1theta = a1.Theta();
  Double_t a1phi   = a1.Phi();
  acrylicneck = IsNeckOptics(a1theta) || IsChimney(a1,p);
 
  // Belly Plates 
  bellyplate = IsBelly(a1theta,a1phi);
  // if (bellyplate) fDacrylic *= 2.0;  // Is this all there is to it???  NO!
 
  // Acrylic Panels and Layers 
  fPanelNum = GetPanel(a1theta,a1phi);
  fLayerNum = GetLayer(a1theta);
 
  // Kevlar Ropes holding the AV 
  fRope = IsRope(a2,p);

  // Circulation Pipes
  fPipe = IsPipe(a1);

  // Prompt AV Reflections when inside the AV
  fPromptAVRefl = IsPromptAVRefl(source,p);
  
  // Check if this path hits a NCD anchor
  // must receive the source and the direction inside D2O
  fAnchor = IsAnchor(source,direction);


  //   if (  fNcdRope){
  //     printf("Hit on umbilical\n");
  //   }
  
  //jm 10/11/03 ncd mask function
  if(fNcdOptics){
    //TVector3 dir(a1-source);
    fNcd  = IsNcd(source,direction);
    fNcdRope = IsNcdRope(source,direction);
  } else {
    fNcd = kFALSE;
    fNcdRope = kFALSE;
  }
  
  if (fCtheta>0 && fCtheta<1) fPMTAngResp = GetPMTAngResp(fCtheta);
  else fPMTAngResp = 0;
  
  if (DEBUG_QOptics) {
    if (fDacrylic > 20.) {
      printf("%12.5g %12.5g %12.5g %12.5g %16.10g\n",
	     fDacrylic, fCtheta,
	     incidentda*180./M_PI, incidentah*180./M_PI,
	     fTransPwr);
    }
  }
  //fTimeOfFlight =
  //  fDd2o/GetVgroupD2O() + fDh2o/GetVgroupH2O() + fDacrylic/GetVgroupAcrylic();
  if (fDacrylic > 0.0) return kTRUE;
  else return kFALSE;
}
//______________________________________________________________________________
Double_t QOptics::GetPmtSolidAngle(Int_t pmtn)
{
  // 05.2006 - O.Simard
  // Calculate solid angle of pmtn as viewed from the source
  // using GetDistances() to four points on pmt surface so that 
  // the AV refraction is considered. See Bryce's thesis pp. 64-67.
  // It also sets the average incident angle fCthetaBar.
  //
  // IMPORTANT:
  //   This calculation needs to immediately follow a call
  //   to GetDistances() so that the vectors are set properly.
  //   Otherwise the solid angle is calculated for another pmt.
  //
  //   Ex:
  //     GetDistances(pmtn);     // check pmt and path validity
  //     GetPmtSolidAngle(pmtn); // calculates solid angle
  //
  // returns -1 if the calculation is unsuccessful.
  //

  // angles going accross pmt surface
  Double_t sinalpha = 0.0, sinbeta = 0.0;
  Double_t solidangle = -1;
  Double_t costhetabar = 0.0; // average incident angle 

  // middle point of solid angle surface
  TVector3 av = fAcrylicVector;
  TVector3 av1, av2, av3, av4;
  TVector3 pmtvec = GetPMTPosition(pmtn);

  TVector3 zprime = (GetPMTNormal(pmtn)).Unit();
  TVector3 yprime = (zprime.Cross(fPMTincident)).Unit();
  TVector3 xprime = (yprime.Cross(zprime)).Unit();

  // four points at PMT edges
  TVector3 pmtvec1 = pmtvec + fPMTReflectorRadius * xprime;
  TVector3 pmtvec2 = pmtvec - fPMTReflectorRadius * xprime;
  TVector3 pmtvec3 = pmtvec + fPMTReflectorRadius * yprime;
  TVector3 pmtvec4 = pmtvec - fPMTReflectorRadius * yprime;

  // Check for all four points. When GetDistances() is called 
  // a new av vector is found.
  // When not valid set it to the old value (av).
  
  if(fNumRefract > 0){ // Goes through the AV
    if(GetDistances(pmtvec1, zprime)){
      av1 = fAcrylicVector; costhetabar += fCtheta;
    } else av1 = av;
    
    if(GetDistances(pmtvec2, zprime)){
      av2 = fAcrylicVector; costhetabar += fCtheta;
    } else av2 = av;
    
    if(GetDistances(pmtvec3, zprime)){
      av3 = fAcrylicVector; costhetabar += fCtheta;
    } else av3 = av;
    
    if(GetDistances(pmtvec4, zprime)){
      av4 = fAcrylicVector; costhetabar += fCtheta;
    } else av4 = av;
  } else { // doesn't go through the AV
    av1 = pmtvec1;
    av2 = pmtvec2;
    av3 = pmtvec3;
    av4 = pmtvec4;
    costhetabar = (pmtvec1.Unit() * zprime + pmtvec2.Unit() * zprime +
		   pmtvec3.Unit() * zprime + pmtvec4.Unit() * zprime);
  }

  // Average cos(theta_pmt)
  costhetabar /= 4.0;
  
  sinalpha = TMath::Sqrt((1.0 - ((av1 - source).Unit() * (av2 - source).Unit()))/2.0);
  sinbeta  = TMath::Sqrt((1.0 - ((av3 - source).Unit() * (av4 - source).Unit()))/2.0);

  // -----------------
  // Notes from Bryce: 
  // (numbers are calculated with fPMTReflectorRadius = 15cm)
  // -----------------
  // 9.9e-4 is the nominal solid angle for a tube at the centre.
  // this is: pi * sin(0.678) * sin(0.678) = pi * (10/845)**2 = 4.40e-4
  // or:      pi * sin(1.017) * sin(1.017) = pi * (15/845)**2 = 9.90e-4
  // and should be used in occratio to get a _relative_ solidangle 
  // value near 1.0. One would get values above 1.0 if the source 
  // is at high radius since the PMT is closer to the source compared 
  // to the center.

  // here we want the absolute value:
  solidangle = M_PI*sinalpha*sinbeta;

  if((solidangle < 0) || (solidangle > 4*M_PI)){
    Warning("GetPmtSolidAngle","Solid angle calculated is outside range: %f",solidangle);
    solidangle = -1; // error
  }

  fCthetaBar = costhetabar; // set this before leaving

  return solidangle;
}
//________________________________________________________________________________
Int_t QOptics::GetPmtNcds2DProjection(Int_t pmtn, Double_t *a, Double_t *b, Double_t *theta, Double_t *ncdcx, Double_t *ncdcy, Int_t *ncdst)
{
  // 09.2006 - P.-L. Drouin
  // Project the PMT pmtn and NCD counters into a plane perpendicular
  // to a line between the source and the PMT and located 1 cm from
  // the source. The function fills the variables a and b with the
  // lenght of the major and minor semi-axes of the PMT ellipse. theta
  // is filled with the angle between the major semi-axis and the
  // x axis. ncdcx and ncdcy are filled with the x and y coordinates
  // of the NCD corners (4 points per counter). ncdst is non-zero
  // and set to 1 only if the NCD has been successfully projected
  // into the plane.
  //
  // Note: QOptics::GetDistances(pmtn) must be called before calling this
  // function.

  TVector3 punormal;
  TVector3 xaxis,yaxis;

  if(!GetPmtNormalProjection(pmtn, a, b, theta, &punormal, &xaxis, &yaxis)) {
    Int_t nncd=fNcdArray->GetNCounters(); //number of NCD counters
    Int_t i; //iteration integer
    Double_t ncdx[4],ncdy[4];
    Int_t ncdpr;

    memset(ncdst,0,nncd*sizeof(Int_t));

    //Loop over NCD counters. This assumes the NCDs are vertical
    for(i=0; i<nncd; i++) {
      printf("NCD %i:\n",i);

      if(!(ncdpr=GetNcd2DProjection(punormal,xaxis,yaxis,-*theta,i,ncdx,ncdy))) {
	ncdcx[i*4]=ncdx[0];
	ncdcy[i*4]=ncdy[0];
	ncdcx[i*4+1]=ncdx[1];
	ncdcy[i*4+1]=ncdy[1];
	ncdcx[i*4+2]=ncdx[2];
	ncdcy[i*4+2]=ncdy[2];
	ncdcx[i*4+3]=ncdx[3];
	ncdcy[i*4+3]=ncdy[3];
	ncdst[i]=1;

      } else if(ncdpr>0) {
	printf("ERROR: GetNcd2DProjection failed with NCD %i\n",i);
	return 1;
      }
    }

  } else {
    printf("ERROR: GetPmtNormalProjection failed with PMT %i\n",pmtn);
    return 1;
  }

  return 0;
}
//________________________________________________________________________________
Double_t QOptics::GetPmtShadow(Int_t pmtn, Bool_t ncdendapprox, Bool_t withpmtrefraction)
{
  // 09.2006 - P.-L. Drouin
  // Compute the total shadowing of a PMT by all NCDs and returns (shadowed PMT area)/(total PMT area).
  // The algorithm takes into account the refraction of the
  // PMT through the AV and does the shadowing calculations
  // analytically. The only source of error of the algorithm that
  // affects all PMTs is the calculation of the PMT ellipse parameters
  // that is done using the center of the PMT as the center of the
  // PMT ellipse. This has almost no effect on the parameters for a
  // source near the center of the detector and a very small effect
  // when going close to the AV. The 5 points version of the function
  // QOptics::GetPmtNormalProjection can be used if more accuracy is
  // needed and if slower calculations are acceptable. The algorithm
  // should not be trusted to compute the shadowing of a PMT from
  // the end of a NCD. An approximation is also made in the rare
  // cases where two NCD sides are crossing in the PMT ellipse.
  // If ncdendapprox is set to kFALSE, the function will return -1 if
  // the algorithm detects that the PMT is shadowed by the end of an NCD

  Double_t a, b, theta; //PMT ellipse major axis, minor axis and rotation angle relative to x-axis
  TVector3 punormal; //Vector pointing to the center of the PMT ellipse (after refraction through AV)
  TVector3 xaxis,yaxis; //Direction of x and y axes in the 2D plane normal to punormal

  Int_t nncd=fNcdArray->GetNCounters(); //number of NCD counters
  Bool_t incd[40]; //NCDs that intersect the PMT (from GetFastShadow 2D shadowing algorithm)

  //If GetFastShadow determines that at least one NCD may shadow the PMT
  if(GetFastShadow(pmtn,incd)) {

    //If the PMT ellipse is projected into a plane normal to punormal without error
    if(!GetPmtNormalProjection(pmtn, &a, &b, &theta, &punormal, &xaxis, &yaxis, withpmtrefraction)) {
      Int_t i,k; //iteration integers
      Double_t ncdx[4],ncdy[4]; //Coordinates of NCD corners in the plane normal to punormal
      Int_t ncdpr; //Status of NCD projection
      Int_t sideyx=-1; //Equations of NCD side segments are expressed as y(x) (not as x(y))
      Double_t *cura, *curb; //major and minor axes of the ellipse in the chosen coordinates system
      Int_t statusl,statusr,status1,status2; //status of GetLEIntersection
      Double_t incdxyl[5][4],incdxyr[5][4], //Coordinates of intersections between the NCD sides (left or right)
               incdcl[5],incdcr[5];         //and the PMT ellipse for all intersecting, NCDs
      Bool_t incdl[5],incdr[5]; //left and right NCD sides intersecting the PMT ellipse for all intersecting,
                                //NCDs
      Int_t nps=0; //Number of NCDs that partially shadow the pmt
      //Double_t *xj1,*xj2,*xk1,*xk2;
      Double_t m1, m2, c1, c2; //Slope and intercept of NCD segments
      Double_t xcross, ycross, xs1, ys1, xs2, ys2, ns1, ns2; //coordinates of the point where two projected
                                                             //parallel NCD segments would intersect if they were infinite
                                                             //Coordinates of two NCD corners relative to this point
      Double_t dbuf,dbuf4[4]; //Double_t buffers
      Int_t nil,nir,ni1,ni2; //Number of intersections of a NCD segment with PMT ellipse
      Bool_t ncdend=kFALSE; //An NCD end is shadowing the PMT                             

      //Loop over NCD counters. This assumes the NCDs are vertical
      for(i=0; i<nncd; i++) {
	//printf("NCD %i:\n",i);

	//If the NCD is identified as a potentially shadowing NCD by GetFastShadow
	if(incd[i]) {

	  //If the NCD is projected in the plane normal to punormal without error and if the source is not inside the NCD
	  if(!(ncdpr=GetNcd2DProjection(punormal,xaxis,yaxis,-theta,i,ncdx,ncdy))) {

	    //printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",ncdx[0],ncdy[0],ncdx[1],ncdy[1],ncdx[2],ncdy[2],ncdx[3],ncdy[3]);

	    //If sideyx has not been initialized
	    if(sideyx<0) {

	      if(TMath::Abs(ncdx[1]-ncdx[2]) < TMath::Abs(ncdy[1]-ncdy[2]) && TMath::Abs(ncdx[3]-ncdx[0]) < TMath::Abs(ncdy[3]-ncdy[0])){
		sideyx=kFALSE;
		cura=&a;
		curb=&b;

	      } else {
		sideyx=kTRUE;
		cura=&b;
		curb=&a;
	      }
	    }

	    //If side segments can be expressed by x(y) functions with a good accuracy
	    if(!sideyx){
	      //printf("x(y)\n");

	      //Get the intersection points between the NCD side segments and the PMT ellipse
	      GetLEIntersection(b, a, ncdy[1], ncdx[1], ncdy[2], ncdx[2], &m1, incdcl+nps, incdxyl[nps]+2, incdxyl[nps], incdxyl[nps]+3, incdxyl[nps]+1, &statusl);
	      GetLEIntersection(b, a, ncdy[3], ncdx[3], ncdy[0], ncdx[0], &m2, incdcr+nps, incdxyr[nps]+2, incdxyr[nps], incdxyr[nps]+3, incdxyr[nps]+1, &statusr);
	      //Get the position of the intersection of the two lines defined by the NCD side segments
	      ycross=(incdcr[nps]-incdcl[nps])/(m1-m2);
	      xcross=m1*ycross+incdcl[nps];

	    } else {
	      //printf("y(x)\n");

	      //Get the intersection points between the NCD side segments and the PMT ellipse
	      GetLEIntersection(a, b, ncdx[1], ncdy[1], ncdx[2], ncdy[2], &m1, incdcl+nps, incdxyl[nps]+2, incdxyl[nps], incdxyl[nps]+3, incdxyl[nps]+1, &statusl);
	      GetLEIntersection(a, b, ncdx[3], ncdy[3], ncdx[0], ncdy[0], &m2, incdcr+nps, incdxyr[nps]+2, incdxyr[nps], incdxyr[nps]+3, incdxyr[nps]+1, &statusr);
	      //Get the position of the intersection of the two lines defined by the NCD side segments
	      xcross=(incdcr[nps]-incdcl[nps])/(m1-m2);
	      ycross=m1*xcross+incdcl[nps];
	    }

	    //If the intercept of the first side segment is greater than the intercept of the second side segment
	    if(incdcl[nps]>incdcr[nps]) {
	      //The first side segment is the "left" segment. The variables of the two segments are swapped
	      status1=statusr;
	      statusr=statusl;
	      statusl=status1;

	      memcpy(dbuf4,incdxyr[nps],4*sizeof(Double_t));
	      memcpy(incdxyr[nps],incdxyl[nps],4*sizeof(Double_t));
	      memcpy(incdxyl[nps],dbuf4,4*sizeof(Double_t));

	      c1=incdcr[nps];
	      incdcr[nps]=incdcl[nps];
	      incdcl[nps]=c1;
	    }
	    
	    //printf("%e\t%e\n",incdcl[nps],incdcr[nps]);

	    xs1=ncdx[1]-xcross;
	    ys1=ncdy[1]-ycross;
	    xs2=ncdx[3]-xcross;
	    ys2=ncdy[3]-ycross;

	    //printf("%i\t%i\t%f\t%f\t%f\n",(statusl>>2),(statusr>>2),ycross*xs1-xcross*ys1,ycross*xs2-xcross*ys2,xcross*(xs1*(ns2=TMath::Sqrt(xs2*xs2+ys2*ys2))+xs2*(ns1=TMath::Sqrt(xs1*xs1+ys1*ys1)))+ycross*(ys1*ns2+ys2*ns1));
	    //If this is not the case where both segments are on the same side of the ellipse with no intersection
	    if((statusl>>2)==0 || (statusr>>2)==0 || (m1==m2 && !((statusl>>2)&(statusr>>2))) || (!((ycross*xs1-xcross*ys1>0)^(ycross*xs2-xcross*ys2<=0)) && xcross*(xs1*(ns2=TMath::Sqrt(xs2*xs2+ys2*ys2))+xs2*(ns1=TMath::Sqrt(xs1*xs1+ys1*ys1)))+ycross*(ys1*ns2+ys2*ns1)<0)){

	      //Store the number of intersections for the two side segments in nil and nir
	      nil=statusl&3;
	      nir=statusr&3;

	      //If the end of a NCD side segments is intersecting the PMT
	      if(nil==1 || nir==1){
		//printf("End of NCD %i is shadowing PMT %i (%i,%i,?,?)\n",i,pmtn,nil,nir);
		ncdend=kTRUE;
	      }

	      //If side segments have not been expressed as y(x) functions
	      if(!sideyx){
		//printf("y(x)\n");
		//Get the intersection points between the NCD top & bottom segments and the PMT ellipse
		GetLEIntersection(a, b, ncdx[0], ncdy[0], ncdx[1], ncdy[1], &m1, &c1, &dbuf, &dbuf, &dbuf, &dbuf, &status1);
		GetLEIntersection(a, b, ncdx[2], ncdy[2], ncdx[3], ncdy[3], &m2, &c2, &dbuf, &dbuf, &dbuf, &dbuf, &status2);
	        //Get the position of the intersection of the two lines defined by the NCD top & bottom segments
		xcross=(c2-c1)/(m1-m2);
		ycross=m1*xcross+c1;

	      } else {
		//printf("x(y)\n");
		//Get the intersection points between the NCD top & bottom segments and the PMT ellipse
		GetLEIntersection(b, a, ncdy[0], ncdx[0], ncdy[1], ncdx[1], &m1, &c1, &dbuf, &dbuf, &dbuf, &dbuf, &status1);
		GetLEIntersection(b, a, ncdy[2], ncdx[2], ncdy[3], ncdx[3], &m2, &c2, &dbuf, &dbuf, &dbuf, &dbuf, &status2);
	        //Get the position of the intersection of the two lines defined by the NCD top & bottom segments
		ycross=(c2-c1)/(m1-m2);
		xcross=m1*ycross+c1;
	      }

	      xs1=ncdx[0]-xcross;
	      ys1=ncdy[0]-ycross;
	      xs2=ncdx[2]-xcross;
	      ys2=ncdy[2]-ycross;

	      //printf("%i\t%i\t%f\t%f\t%f\n",(status1>>2),(status2>>2),ycross*xs1-xcross*ys1,ycross*xs2-xcross*ys2,xcross*(xs1*(ns2=TMath::Sqrt(xs2*xs2+ys2*ys2))+xs2*(ns1=TMath::Sqrt(xs1*xs1+ys1*ys1)))+ycross*(ys1*ns2+ys2*ns1));
	      //If this is not the case where both segments are on the same side of the ellipse with no intersection
	      if((statusl>>2)==0 || (statusr>>2)==0 || (status1>>2)==0 || (status2>>2)==0 || (m1==m2 && !((status1>>2)&(status2>>2))) || (!((ycross*xs1-xcross*ys1>0)^(ycross*xs2-xcross*ys2<=0)) && xcross*(xs1*(ns2=TMath::Sqrt(xs2*xs2+ys2*ys2))+xs2*(ns1=TMath::Sqrt(xs1*xs1+ys1*ys1)))+ycross*(ys1*ns2+ys2*ns1)<0)){
		ni1=status1&3;
		ni2=status2&3;

		//If one of the two segments intersects the PMT ellipse
		if(ni1!=0 || ni2!=0) {
		  //printf("End of NCD %i is shadowing PMT %i (%i,%i,%i,%i)\n",i,pmtn,nil,nir,ni1,ni2);
		  ncdend=kTRUE;
		}

		//If also the sides did not intersect the PMT ellipse
		if(nil==0 && nir==0) {

		  //If the four sides do not intersect even if one of the slope equations predicts intersection with the ellipse,
		  //then the NCD does not shadow the PMT
		  if (!(statusl>>2) || !(statusr>>2) || !(status1>>2) || !(status2>>2)){
		    //printf("No shadowing of PMT %i from NCD %i by condition 3 (%i,%i,%i,%i)\n",pmtn,i,nil,nir,ni1,ni2);
		    continue;

		    //Else the NCD is shadowing the PMT completely
		  } else {
		    //printf("NCD %i shadows PMT %i completely  (%i,%i,%i,%i)\n",i,pmtn,nil,nir,ni1,ni2);
		    return 1.;
		  }

		} else {
		  //printf("NCD %i shadows partially PMT %i (%i,%i,%i,%i)\n",i,pmtn,nil,nir,ni1,ni2);

		  incdl[nps]=nil;
		  incdr[nps]=nir;

		  for(k=0; k<nps; k++) {
		    //If the two selected NCDs are overlapping
		    if((!incdl[nps] || !incdr[nps] || !incdl[k] || !incdr[k] || ((incdcl[nps]>=incdcr[k] || incdcr[nps]>=incdcl[k]) && (incdcl[nps]<incdcr[k] || incdcr[nps]<incdcl[k]))) && (!incdr[nps] || !incdl[k] || (incdl[nps] && incdr[k]) || incdcr[nps]>=incdcl[k]) && (!incdl[nps] || !incdr[k] || (incdr[nps] && incdl[k]) || incdcl[nps]<incdcr[k])) {
		      //printf("Merging virtual NCD %i with last NCD\n",k);

		      //If the left side segment of the last NCD is at the left of the left side segment of the kth virtual NCD, than update the left side of the virtual NCD
		      if(incdl[nps] && incdl[k] && incdcl[nps]<incdcl[k]) {memcpy(incdxyl[k],incdxyl[nps],4*sizeof(Double_t)); incdcl[k]=incdcl[nps];}

		      //If the right side segment of the last NCD is at the right of the right side segment of the kth virtual NCD, than update the right side of the virtual NCD
		      if(incdr[nps] && incdr[k] && incdcr[nps]>=incdcr[k]) {memcpy(incdxyr[k],incdxyr[nps],4*sizeof(Double_t)); incdcr[k]=incdcr[nps];}

		      //Update incdl and incdr for the kth virtual NCD
		      incdl[k]=(incdl[nps]&&incdl[k]);
		      incdr[k]=(incdr[nps]&&incdr[k]);
		      nps--;
		      k--;
		    }
		  }

		  if(!incdl[0] && !incdr[0]) {
		    //printf("PMT %i is completely shadowed by multiple partially shadowing NCDs\n",pmtn);
		    return 1.;
		  }

		  //Increment nps by 1 such that its value corresponds to the number of virtual NCDs
		  nps++;

		  /*printf("%i|\t",nps);

		  for(k=0; k<nps; k++) {
		    printf("%f\t%f\t",incdl[k]?incdxyl[k][0]:-1,incdr[k]?incdxyr[k][0]:-1);
		  }
		  printf("\n");*/
		}

	      } else {
		//The NCD is not shadowing the PMT
		//printf("No shadowing of PMT %i from NCD %i by condition 2 (%i,%i,%i,%i)\n",pmtn,i,nil,nir,status1&3,status2&3);
		continue;
	      }

	    } else {
	      //printf("No shadowing of PMT %i from NCD %i by condition 1 (%i,%i,?,?)\n",pmtn,i,statusl&3,statusr&3);
	    }

	    //Else if source is in a NCD
	  } else if(ncdpr==-1) {
	    return 1.;

	  } else if(ncdpr>0) {
	    printf("ERROR: GetNcd2DProjection failed with NCD %i\n",i);
	    return 0.5;
	  }

	} else {
	  //printf("No shadowing of PMT %i from NCD %i (FastShadow)\n",pmtn,i);
	}
      }

      if(!ncdendapprox && ncdend) return -1.;

      if(ncdend && !nps) {
	return 0.5;
      }

      //if(nps) printf("PMT %i is partially shadowed\n",pmtn);
      dbuf=0;

      //Compute the total shadowing fraction by all virtual NCDs
      for(k=0; k<nps; k++) {
	//printf("%i, (%f, %f)\t(%f, %f)\t\t%i, (%f, %f)\t(%f, %f)\n",incdl[k],incdxyl[k][2],incdxyl[k][0],incdxyl[k][3],incdxyl[k][1],incdr[k],incdxyr[k][2],incdxyr[k][0],incdxyr[k][3],incdxyr[k][1]);
	//printf("%f\n",((!incdr[k]?TMath::Pi()*a*b:GetEllipseIntegral(*cura,*curb,incdxyr[k]))-(!incdl[k]?0:GetEllipseIntegral(*cura,*curb,incdxyl[k])))/(TMath::Pi()*a*b));
	dbuf+=(!incdr[k]?TMath::Pi()*a*b:GetEllipseIntegral(*cura,*curb,incdxyr[k]))-(!incdl[k]?0:GetEllipseIntegral(*cura,*curb,incdxyl[k]));
      }
      //return 1-shadowing fraction
      return dbuf/(TMath::Pi()*a*b);

    } else {
      printf("ERROR: GetPmtNormalProjection failed with PMT %i\n",pmtn);
      return 0.5;
    }

  } else {
    //printf("No shadowing of PMT %i (FastShadow)\n",pmtn);
    return 0.;
  }
}
//________________________________________________________________________________
Int_t QOptics::GetFastShadow(Int_t pmtn, Bool_t *incd)
{
  // 09.2006 - P.-L. Drouin
  // Loop over all NCDs and discard most of the counters that are
  // not shadowing the PMT pmtn. The algorithm returns the number
  // of NCDs that are not discarded and sets the elements of incd
  // corresponding to these NCDs to kTRUE.

  TVector2 pmtvec(GetPMTPosition(pmtn).X()-source.X(),GetPMTPosition(pmtn).Y()-source.Y()); //Projection of PMT position in the x-y plane
  TVector2 ncdvec; //Position of a NCD in the x-y plane, relative to the source. The magnitude of ncdvec is the distance
                   //between the source and the NCD surface
  TVector2 dpmtvec(-pmtvec.Y(),pmtvec.X()); //Direction perpendicular to PMT direction
  dpmtvec=dpmtvec.Unit()*fPMTReflectorRadius;
  TVector2 dncdvec; //Line perpendicular to NCD direction which end points are:
                    //1-the intersection between the NCD surface and the segment between the source and the NCD center
                    //2-the segment tangent to the NCD surface and going through the source
  TVector2 pmtvecp1=pmtvec+dpmtvec;
  TVector2 pmtvecp2=pmtvec-dpmtvec;
  TVector2 ncdvecp1, ncdvecp2;
  Int_t nincd=0;    //Number of shadowing NCDs
  Int_t nncd=fNcdArray->GetNCounters(); //number of NCD counters
  Double_t x0;      //Distance between the source and the center of a NCD

  //Initialize the incd array to 0
  memset(incd,0,40*sizeof(Bool_t));

  //Loop over the NCDs
  for (Int_t i=0; i<nncd; i++) {
    ncdvec.Set(fNcdArray->GetCounter(i)->GetCenter().X()-source.X(),fNcdArray->GetCounter(i)->GetCenter().Y()-source.Y());
    
    //If the source is in the NCD
    if(ncdvec.Mod2()<=fNCDRadius*fNCDRadius) {
      incd[i]=kTRUE;
      //Increment the number of shadowing NCDs
      nincd++;
      continue;
    }
    x0=ncdvec.Mod();
    ncdvec*=(x0-fNCDRadius)/x0;

    dncdvec.Set(-ncdvec.Y(),ncdvec.X());
    dncdvec=dncdvec.Unit()*fNCDRadius*TMath::Sqrt(x0*x0-fNCDRadius*fNCDRadius)/(x0+fNCDRadius);
    ncdvecp1=ncdvec+dncdvec;
    ncdvecp2=ncdvec-dncdvec;

    //printf("%i\t%f\t%f\t%f\t%f\n",i,TMath::ACos(ncdvecp1.Unit()*pmtvecp2.Unit())*180/TMath::Pi(),TMath::ACos(pmtvecp1.Unit()*ncdvecp2.Unit())*180/TMath::Pi(),ncdvecp1.X()*pmtvecp2.Y()-ncdvecp1.Y()*pmtvecp2.X(),pmtvecp1.X()*ncdvecp2.Y()-pmtvecp1.Y()*ncdvecp2.X());

    //If the NCD is shadowing the PMT
    if(ncdvecp1.X()*pmtvecp2.Y()-ncdvecp1.Y()*pmtvecp2.X()<=0 && pmtvecp1.X()*ncdvecp2.Y()-pmtvecp1.Y()*ncdvecp2.X()<=0){
      incd[i]=kTRUE;
      nincd++;
    }
  }

  //printf("%i\n",nincd);

  return nincd;
}
//________________________________________________________________________________
Int_t QOptics::GetPmtNormalProjection(Int_t pmtn, Double_t *a, Double_t *b, Double_t *theta, TVector3 *punormal, TVector3 *xaxis, TVector3 *yaxis, Bool_t withpmtrefraction)
{
  // 09.2006 - P.-L. Drouin
  // Project the PMT pmtn into a plane perpendicular to a line between
  // the source and the PMT and located 1 cm from the source. The
  // function fills the variables a and b with the lenght of the major
  // and minor semi-axes of the PMT ellipse. theta is filled with the
  // angle between the major semi-axis and the x axis. punormal is
  // set to a unit vector normal to the plane. xaxis and yaxis are
  // set to unit vectors that define the x and y axes in the plane.
  // The algorithm takes into account the refraction of the PMT
  // ellipse through the AV. It approximates that the center of the
  // refracted PMT ellipse is at the same position than the refracted
  // PMT center.
  // The functionn returns a non-zero value if the projection failed.

  TVector3 sav1, sav2, sav3;
  TVector3 pmtvec = GetPMTPosition(pmtn); //PMT position

  //Use PMT refraction if withpmtrefraction is set to kTRUE and if event radius<600
  if(withpmtrefraction && source.Mag2()<360000.) {
    GetDistances(pmtn);
    *punormal = fAcrylicVector-source; //Vector going from the source in the direction of the refracted PMT center

  } else {
    *punormal = pmtvec-source;
  }

  //printf("%f\n",TMath::ACos(punormal->Unit()*(pmtvec-source).Unit())*180/TMath::Pi());

  //GetPMTNormal points inside the PMT, not outside!
  TVector3 zprime = (GetPMTNormal(pmtn)).Unit();
  TVector3 xprime = (fPMTincident.Cross(zprime)).Unit();
  TVector3 yprime = (zprime.Cross(xprime)).Unit();

  //Get 3 points around the PMT reflector
  sav1 = pmtvec + fPMTReflectorRadius * xprime;
  sav2 = pmtvec + fPMTReflectorRadius * (yprime*0.866025403784438597-xprime*0.5);
  sav3 = pmtvec - fPMTReflectorRadius * (yprime*0.866025403784438597+xprime*0.5);

  TVector3 vbuf;
  Double_t x1,x2,x3,y2,y3;
  Double_t A,B,C,BCdet;

  // Check for all four points. When GetDistances() is called 
  // a new sav# vector is found.

  //Use PMT refraction if withpmtrefraction is set to kTRUE and if event radius<600
  if(withpmtrefraction && source.Mag2()<360000. && fNumRefract > 0){ // Goes through the AV
    if(GetDistances(sav1, zprime)){
      sav1 = fAcrylicVector-source;
    } else return 1;

    if(GetDistances(sav2, zprime)){
      sav2 = fAcrylicVector-source;
    } else return 1;

    if(GetDistances(sav3, zprime)){
      sav3 = fAcrylicVector-source;
    } else return 1;

  } else { // doesn't go through the AV
    sav1 -= source;
    sav2 -= source;
    sav3 -= source;
  }

  //Computes punormal using the 4 facing points of the ellipse
  *punormal=punormal->Unit();

  //printf("|sav1|=%f\t|sav2|=%f\t|sav3|=%f\n",sav1.Mag(),sav2.Mag(),sav3.Mag());

  //sav# are set to the points (relative to source) where existing sav# cross a plane normal to vectror av and
  //located 1 unit from the source
  sav1*=1./(sav1*(*punormal));
  //sav# projection in the plane are computed
  sav1-=*punormal;
  //a 2D coodinates system is defined in the plane. xaxis and yaxis are the axes of the coordinates system
  *xaxis=sav1.Unit();
  *yaxis=(sav1.Cross(*punormal)).Unit();

  x1=sav1.Mag(); //The x axis is chosen such that y1=0
  if(Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav2,&x2,&y2) || Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav3,&x3,&y3)) {
    printf("ERROR: PMT solid angle is too large\n");
    return 1;
  }

  /*x1=3/TMath::Sqrt(5);
  x2=2; y2=11./5;
  x3=0; y3=3/TMath::Sqrt(5);*/

  //printf("x1=%f\tx2=%f\ty2=%f\tx3=%f\ty3=%f\n",x1,x2,y2,x3,y3);

  BCdet=x1*x1*y2*y3*(x3*y2-x2*y3);

  //Solve for A, B and C in the ellipse equation A*x^2+B*y^2+C*x*y=1
  A=1./(x1*x1);
  B=(x1*x1*x3*y3-x2*x2*x3*y3-x1*x1*x2*y2+x2*x3*x3*y2)/BCdet;
  C=(x1*x1*y2*y2-x1*x1*y3*y3+x2*x2*y3*y3-x3*x3*y2*y2)/BCdet;
  //printf("A=%f\tB=%f\tC=%f\n",A,B,C);

  if(A<0 || B<0) {
    printf("PROBLEM: A or B is (are) negative\n");
    return 1;
  }

  //Compute the corresponding parameters a, b and theta of the ellipse, where theta is the angle between xaxis and the segment a
  *a=TMath::Sqrt(2./(A+B-TMath::Sqrt((A-B)*(A-B)+C*C)));
  *b=TMath::Sqrt(2./(A+B+TMath::Sqrt((A-B)*(A-B)+C*C)));
  *theta=0.5*TMath::ATan2(-C,B-A);

  //printf("a=%f\tb=%f\ttheta=%f\n",*a,*b,*theta);
  //printf("%15.10e, %15.10e, %15.10e, 0, 0\n",*a,*b,*theta);

  //printf("Solid Angle: %e\n",*a*(*b)*TMath::Pi());

  return 0;
}
//________________________________________________________________________________
Int_t QOptics::GetPmtNormalProjection(Int_t pmtn, Double_t *a, Double_t *b, Double_t *theta, Double_t *x0, Double_t *y0, TVector3 *punormal, TVector3 *xaxis, TVector3 *yaxis)
{
  // 09.2006 - P.-L. Drouin
  // Project the PMT pmtn into a plane perpendicular to a line
  // between the source and the PMT and located 1 cm from the
  // source. The function fills the variables a and b with the length
  // of the major and minor semi-axes of the PMT ellipse. theta is
  // filled with the angle between the major semi-axis and the x
  // axis. x0 and y0 are set to the x and y coordinates of the PMT
  // ellipse center. punormal is set to a unit vector normal to the
  // plane. xaxis and yaxis are set to unit vectors that define the x
  // and y axes in the plane.  The algorithm takes into account the
  // refraction of the PMT ellipse through the AV. This function is
  // more accurate than the other GetPmtNormalProjection function,
  // but is significantly slower.
  // The function returns a non-zero value if the projection failed.

  GetDistances(pmtn);
  *punormal = fAcrylicVector-source; //This vector could also be set using the points sav1,..., sav5, requiring one less call of GetDistances
  TVector3 sav1, sav2, sav3, sav4, sav5;
  TVector3 pmtvec = GetPMTPosition(pmtn); //PMT position
  //pmtvec.Print();

  //printf("%f\n",TMath::ACos(punormal->Unit()*(pmtvec-source).Unit())*180/TMath::Pi());

  //GetPMTNormal points inside the PMT, not outside!
  TVector3 zprime = (GetPMTNormal(pmtn)).Unit();
  TVector3 xprime = (fPMTincident.Cross(zprime)).Unit();
  TVector3 yprime = (zprime.Cross(xprime)).Unit();

  //Get 5 points around the PMT reflector
  sav1 = pmtvec + fPMTReflectorRadius * xprime;
  sav2 = pmtvec + fPMTReflectorRadius * (0.951056516295153531*xprime+0.309016994374947451*yprime);
  sav3 = pmtvec + fPMTReflectorRadius * (0.587785252292473248*xprime-0.809016994374947340*yprime);
  sav4 = pmtvec + fPMTReflectorRadius * (-0.587785252292473026*xprime-0.809016994374947340*yprime);
  sav5 = pmtvec + fPMTReflectorRadius * (-0.951056516295153531*xprime+0.309016994374947451*yprime);

  //sav2 = pmtvec + fPMTReflectorRadius * (0.951056516295153531*xprime+0.309016994374947451*yprime);
  //sav3 = pmtvec + fPMTReflectorRadius * (yprime*0.866025403784438597-xprime*0.5);
  //sav4 = pmtvec - fPMTReflectorRadius * (yprime*0.866025403784438597+xprime*0.5);
  //sav5 = pmtvec + fPMTReflectorRadius * (0.587785252292473248*xprime-0.809016994374947340*yprime);

  TVector3 vbuf;
  Double_t x1,x2,x3,x4,x5,y2,y3,y4,y5;
  Double_t x1p2, x2p2, x3p2, x4p2, x5p2, y2p2, y3p2, y4p2, y5p2;
  Double_t x2ty2, x3ty3, x4ty4, x5ty5;
  Double_t A, B, C, D, F, G;
  Double_t delta;
  Double_t b2ac, abnum, abdetpart;

  // Check for all four points. When GetDistances() is called 
  // a new sav# vector is found.

  if(fNumRefract > 0){ // Goes through the AV
    if(GetDistances(sav1, zprime)){
      sav1 = fAcrylicVector-source;
    } else return 1;

    if(GetDistances(sav2, zprime)){
      sav2 = fAcrylicVector-source;
    } else return 1;

    if(GetDistances(sav3, zprime)){
      sav3 = fAcrylicVector-source;
    } else return 1;

    if(GetDistances(sav4, zprime)){
      sav4 = fAcrylicVector-source;
    } else return 1;

    if(GetDistances(sav5, zprime)){
      sav5 = fAcrylicVector-source;
    } else return 1;

  } else { // doesn't go through the AV
    sav1 -= source;
    sav2 -= source;
    sav3 -= source;
    sav4 -= source;
    sav5 -= source;
  }

  //Computes punormal using the 4 facing points of the ellipse
  *punormal=punormal->Unit();

  //printf("|sav1|=%f\t|sav2|=%f\t|sav3|=%f\n",sav1.Mag(),sav2.Mag(),sav3.Mag());

  //sav# are set to the points (relative to source) where existing sav# cross a plane normal to vector av and
  //located 1 unit from the source
  sav1*=1./(sav1*(*punormal));
  //sav# projection in the plane are computed
  sav1-=*punormal;
  //a 2D coordinates system is defined in the plane. xaxis and yaxis are the axes of the coordinates system
  *xaxis=sav1.Unit();
  *yaxis=(sav1.Cross(*punormal)).Unit();

  x1=sav1.Mag(); //The x axis is chosen such that y1=0
  if(Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav2,&x2,&y2) || Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav3,&x3,&y3) || Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav4,&x4,&y4) || Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav5,&x5,&y5)) {
    printf("ERROR: PMT solid angle is too large\n");
  }

  /*x1=3/TMath::Sqrt(5)+0.5;
  x2=2+0.5; y2=11./5;
  x3=0+0.5; y3=3/TMath::Sqrt(5);
  x4=-2+0.5; y4=-1;
  x5=1+0.5; y5=-0.4;*/

  //printf("x1:=%15.10e;\tx2:=%15.10e;\ty2:=%15.10e;\tx3:=%15.10e;\ty3:=%15.10e;\tx4:=%15.10e;\ty4:=%15.10e;\tx5:=%15.10e;\ty5:=%15.10e;\n",x1,x2,y2,x3,y3,x4,y4,x5,y5);

  x1p2=x1*x1;
  x2p2=x2*x2;
  x3p2=x3*x3;
  x4p2=x4*x4;
  x5p2=x5*x5;
  y2p2=y2*y2;
  y3p2=y3*y3;
  y4p2=y4*y4;
  y5p2=y5*y5;
  x2ty2=x2*y2;
  x3ty3=x3*y3;
  x4ty4=x4*y4;
  x5ty5=x5*y5;

  //Compute the parameters in the general ellipse equation Ax^2+2Bxy+Cy^2+2Dx+2Fy+G=0
  A=y2p2*(x3ty3*(y4*x1-y5*x1-y4*x5+y5*x4)-x4ty4*(y5*x3+y3*x1-y3*x5-x1*y5)+x5ty5*(y4*x3+y3*x1-y3*x4-x1*y4))+y3p2*(-x2ty2*(y4*x1-y5*x1-y4*x5+y5*x4)+x4ty4*(y2*x1-y5*x1-y2*x5+y5*x2)-x5ty5*(-y4*x1+y2*x1+y4*x2-y2*x4))+y4p2*(x2ty2*(y5*x3+y3*x1-y3*x5-x1*y5)-x3ty3*(y5*x2+y2*x1-y2*x5-x1*y5)+x5ty5*(-y2*x3-y3*x1+y3*x2+x1*y2))+y5p2*(-x2ty2*(y4*x3+y3*x1-y3*x4-x1*y4)+x3ty3*(y4*x2+y2*x1-y2*x4-x1*y4)-x4ty4*(-y2*x3-y3*x1+y3*x2+x1*y2));

  C=x1p2*(-x2ty2*(-y5*x3+y4*x3+y3*x5-y3*x4-x5*y4+x4*y5)+x3ty3*(-y5*x2+y4*x2+y2*x5-y2*x4-x5*y4+x4*y5)-x4ty4*(y5*x3-y2*x3-y3*x5+y3*x2+x5*y2-x2*y5)+x5ty5*(-y2*x3+y4*x3+y3*x2-y3*x4-x2*y4+x4*y2))+x2p2*(-x3ty3*(y4*x1-y5*x1-y4*x5+y5*x4)+x4ty4*(y5*x3+y3*x1-y3*x5-x1*y5)-x5ty5*(y4*x3+y3*x1-y3*x4-x1*y4))+x3p2*(x2ty2*(y4*x1-y5*x1-y4*x5+y5*x4)-x4ty4*(y2*x1-y5*x1-y2*x5+y5*x2)+x5ty5*(-y4*x1+y2*x1+y4*x2-y2*x4))+x4p2*(-x2ty2*(y5*x3+y3*x1-y3*x5-x1*y5)+x3ty3*(y5*x2+y2*x1-y2*x5-x1*y5)-x5ty5*(-y2*x3-y3*x1+y3*x2+x1*y2))+x5p2*(x2ty2*(y4*x3+y3*x1-y3*x4-x1*y4)-x3ty3*(y4*x2+y2*x1-y2*x4-x1*y4)+x4ty4*(-y2*x3-y3*x1+y3*x2+x1*y2));

  B=0.5*(x1p2*(y2p2*(-y5*x3+y4*x3+y3*x5-y3*x4-x5*y4+x4*y5)-y3p2*(-y5*x2+y4*x2+y2*x5-y2*x4-x5*y4+x4*y5)+y4p2*(y5*x3-y2*x3-y3*x5+y3*x2+x5*y2-x2*y5)-y5p2*(-y2*x3+y4*x3+y3*x2-y3*x4-x2*y4+x4*y2))+x2p2*(y3p2*(y4*x1-y5*x1-y4*x5+y5*x4)-y4p2*(y5*x3+y3*x1-y3*x5-x1*y5)+y5p2*(y4*x3+y3*x1-y3*x4-x1*y4))+x3p2*(-y2p2*(y4*x1-y5*x1-y4*x5+y5*x4)+y4p2*(y5*x2+y2*x1-y2*x5-x1*y5)-y5p2*(y4*x2+y2*x1-y2*x4-x1*y4))+x4p2*(y2p2*(y5*x3+y3*x1-y3*x5-x1*y5)-y3p2*(y2*x1-y5*x1-y2*x5+y5*x2)+y5p2*(-y2*x3-y3*x1+y3*x2+x1*y2))+x5p2*(-y2p2*(y4*x3+y3*x1-y3*x4-x1*y4)+y3p2*(-y4*x1+y2*x1+y4*x2-y2*x4)-y4p2*(-y2*x3-y3*x1+y3*x2+x1*y2)));

  D=0.5*(x1p2*(-y2*(-x3ty3*y4p2+x3ty3*y5p2+x4ty4*y3p2-x4ty4*y5p2-x5ty5*y3p2+x5ty5*y4p2)+y3*(x4ty4*y2p2-x4ty4*y5p2-x2ty2*y4p2-x5ty5*y2p2+x5ty5*y4p2+x2ty2*y5p2)-y4*(x3ty3*y2p2-x3ty3*y5p2-x2ty2*y3p2-x5ty5*y2p2+x5ty5*y3p2+x2ty2*y5p2)+y5*(x3ty3*y2p2-x3ty3*y4p2-x2ty2*y3p2-x4ty4*y2p2+x4ty4*y3p2+x2ty2*y4p2))+x2p2*(-y3*(-x4ty4*y5p2+x5ty5*y4p2)+y4*(-x3ty3*y5p2+x5ty5*y3p2)-y5*(-x3ty3*y4p2+x4ty4*y3p2))+x3p2*(-y2*(+x4ty4*y5p2-x5ty5*y4p2)-y4*(-x2ty2*y5p2+x5ty5*y2p2)+y5*(-x2ty2*y4p2+x4ty4*y2p2))+x4p2*(-y2*(-x3ty3*y5p2+x5ty5*y3p2)+y3*(-x2ty2*y5p2+x5ty5*y2p2)-y5*(-x2ty2*y3p2+x3ty3*y2p2))+x5p2*(-y2*(x3ty3*y4p2-x4ty4*y3p2)-y3*(-x2ty2*y4p2+x4ty4*y2p2)+y4*(-x2ty2*y3p2+x3ty3*y2p2)));

  F=0.5*(x1p2*(x2*(-x3ty3*y4p2+x3ty3*y5p2+x4ty4*y3p2-x4ty4*y5p2-x5ty5*y3p2+x5ty5*y4p2)-x3*(x4ty4*y2p2-x4ty4*y5p2-x2ty2*y4p2-x5ty5*y2p2+x5ty5*y4p2+x2ty2*y5p2)+x4*(x3ty3*y2p2-x3ty3*y5p2-x2ty2*y3p2-x5ty5*y2p2+x5ty5*y3p2+x2ty2*y5p2)-x5*(x3ty3*y2p2-x3ty3*y4p2-x2ty2*y3p2-x4ty4*y2p2+x4ty4*y3p2+x2ty2*y4p2))+x2p2*(-x1*(-x3ty3*y4p2+x3ty3*y5p2+x4ty4*y3p2-x4ty4*y5p2-x5ty5*y3p2+x5ty5*y4p2)+x3*(-x4ty4*y5p2+x5ty5*y4p2)-x4*(-x3ty3*y5p2+x5ty5*y3p2)+x5*(-x3ty3*y4p2+x4ty4*y3p2))+x3p2*(x1*(-x2ty2*y4p2+x2ty2*y5p2+x4ty4*y2p2-x4ty4*y5p2-x5ty5*y2p2+x5ty5*y4p2)-x2*(-x4ty4*y5p2+x5ty5*y4p2)+x4*(-x2ty2*y5p2+x5ty5*y2p2)-x5*(-x2ty2*y4p2+x4ty4*y2p2))+x4p2*(-x1*(-x2ty2*y3p2+x2ty2*y5p2+x3ty3*y2p2-x3ty3*y5p2-x5ty5*y2p2+x5ty5*y3p2)-x2*(x3ty3*y5p2-x5ty5*y3p2)-x3*(-x2ty2*y5p2+x5ty5*y2p2)+x5*(-x2ty2*y3p2+x3ty3*y2p2))+x5p2*(x1*(-x2ty2*y3p2+x2ty2*y4p2+x3ty3*y2p2-x3ty3*y4p2-x4ty4*y2p2+x4ty4*y3p2)-x2*(-x3ty3*y4p2+x4ty4*y3p2)-x3*(x2ty2*y4p2-x4ty4*y2p2)-x4*(-x2ty2*y3p2+x3ty3*y2p2)));

  G=x1p2*(-y2p2*(x3ty3*x4*y5-x4*y3*x5ty5-x3*x4ty4*y5-x3ty3*x5*y4+x5*y3*x4ty4+x3*x5ty5*y4)-y3p2*(-x2ty2*x4*y5+x4*y2*x5ty5+x2*x4ty4*y5+x2ty2*x5*y4-x5*y2*x4ty4-x2*x5ty5*y4)-y4p2*(x2ty2*x3*y5-x3*y2*x5ty5-x2*x3ty3*y5-x2ty2*x5*y3+x5*y2*x3ty3+x2*x5ty5*y3)+y5p2*(x2ty2*x3*y4-x3*y2*x4ty4-x2*x3ty3*y4-x2ty2*x4*y3+x4*y2*x3ty3+x2*x4ty4*y3))+x2p2*(-y3p2*(-y5*x1*x4ty4+y4*x1*x5ty5)+y4p2*(-x3ty3*x1*y5+x1*y3*x5ty5)-y5p2*(-x3ty3*x1*y4+x1*y3*x4ty4))+x3p2*(y2p2*(-y5*x1*x4ty4+y4*x1*x5ty5)-y4p2*(-x2ty2*x1*y5+x1*y2*x5ty5)+y5p2*(-x2ty2*x1*y4+x1*y2*x4ty4))+x4p2*(-y2p2*(-y5*x1*x3ty3+y3*x1*x5ty5)+y3p2*(-x2ty2*x1*y5+x1*y2*x5ty5)-y5p2*(-x2ty2*x1*y3+x1*y2*x3ty3))+x5p2*(-y2p2*(y4*x1*x3ty3-y3*x1*x4ty4)-y3p2*(-x2ty2*x1*y4+x1*y2*x4ty4)+y4p2*(-x2ty2*x1*y3+x1*y2*x3ty3));

  if(A*C-B*B<=0 || (delta=A*(C*G-F*F)-B*(B*G-D*F)+D*(B*F-C*D))==0 || delta/(A+C)>=0) {
    printf("ERROR: Unable to compute PMT ellipse equation (%e, %e, %e)\n",A*C-B*B,A*(C*G-F*F)-B*(B*G-D*F)+D*(B*F-C*D),delta/(A+C));
    return 1;
  }

  b2ac=B*B-A*C;
  abnum=2*(A*F*F+C*D*D+G*B*B-2*B*D*F-A*C*G);
  abdetpart=TMath::Sqrt((A-C)*(A-C)+4*B*B);

  //printf("A:=%15.10e;B:=%15.10e;C:=%15.10e;DD:=%15.10e;F:=%15.10e;G:=%15.10e;b2ac:=%15.10e;abnum:=%15.10e;abdetpart:=%15.10e;\n",A,B,C,D,F,G,b2ac,abnum,abdetpart);

  *x0=(C*D-B*F)/b2ac;
  *y0=(A*F-B*D)/b2ac;
  *theta=0.5*TMath::ATan2(-2*B,C-A);
  *a=TMath::Sqrt(abnum/(b2ac*(abdetpart-(C+A))));
  *b=TMath::Sqrt(abnum/(b2ac*(-abdetpart-(C+A))));

  //printf("a=%15.10e\tb=%15.10e\ttheta=%15.10e\tx0=%15.10e\ty0=%15.10e\n",*a,*b,*theta,*x0,*y0);
  //printf("%15.10e, %15.10e, %15.10e, %15.10e, %15.10e\n",*a,*b,*theta,*x0,*y0);

  return 0;
}
//________________________________________________________________________________
Int_t QOptics::GetPmtPointsNormalProjection(Int_t pmtn, Bool_t refraction, Int_t numpoints, Double_t *xpoints, Double_t *ypoints, TVector3 *punormal, TVector3 *xaxis, TVector3 *yaxis)
{
  // 09.2006 - P.-L. Drouin
  // Project a set of points equally distributed around the reflector
  // of the PMT pmtn into a plane perpendicular to a line between
  // the source and the PMT and located 1 cm from the source. If
  // refraction is set to kTRUE, the calculations are performed
  // considering the refraction of the PMT ellipse through the
  // AV. numpoints is the number of points that should be projected
  // on the plane. xpoints and ypoints arrays are filled with the
  // coordinates of the different projected points. punormal is set
  // to a unit vector normal to the plane. xaxis and yaxis are set
  // to unit vectors that define the x and y axes in the plane.
  // The function returns a non-zero value if the projection failed.

  GetDistances(pmtn);
  *punormal = fAcrylicVector-source; //Vector going from the source in the direction
  //of middle point of solid angle
  TVector3 sav1, sav2;
  TVector3 pmtvec = GetPMTPosition(pmtn);
  //pmtvec.Print();

  //printf("%f\n",TMath::ACos(punormal->Unit()*(pmtvec-source).Unit())*180/TMath::Pi());

  //GetPMTNormal points inside the PMT, not outside!
  TVector3 zprime = (GetPMTNormal(pmtn)).Unit();
  TVector3 xprime = (fPMTincident.Cross(zprime)).Unit();
  TVector3 yprime = (zprime.Cross(xprime)).Unit();

  Int_t i;
  Double_t dbuf;

  // four points at PMT edges
  sav1 = pmtvec + fPMTReflectorRadius * xprime;

  // Check for all four points. When GetDistances() is called 
  // a new sav# vector is found.

  if(refraction && fNumRefract > 0){ // Goes through the AV
    if(GetDistances(sav1, zprime)){
      sav1 = fAcrylicVector-source;
    } else return 1;

  } else { // doesn't go through the AV
    sav1 -= source;
  }

  //Computes punormal using the 4 facing points of the ellipse
  *punormal=punormal->Unit();

  sav1*=1./(sav1*(*punormal));
  //sav# projection in the plane are computed
  sav1-=*punormal;
  //a 2D coordinates system is defined in the plane. xaxis and yaxis are the axes of the coordinates system
  *xaxis=sav1.Unit();
  *yaxis=(sav1.Cross(*punormal)).Unit();

  xpoints[0]=sav1.Mag(); //The x axis is chosen such that y1=0
  ypoints[0]=0;

  for(i=1; i<numpoints; i++) {
    dbuf=2*TMath::Pi()*i/numpoints;
    sav2 = pmtvec + fPMTReflectorRadius * (TMath::Cos(dbuf)*xprime + TMath::Sin(dbuf)*yprime);

    if(refraction && fNumRefract > 0){
      if(GetDistances(sav2, zprime)){
	sav2 = fAcrylicVector-source;
      } else return 1;

    } else { // doesn't go through the AV
      sav2 -= source;
    }

    if(Get2DRotProjection(*punormal,*xaxis,*yaxis,0,sav2,xpoints+i,ypoints+i)) {
      printf("ERROR: PMT solid angle is too large\n");
    }
  }

  return 0;
}
//______________________________________________________________________________
Bool_t QOptics::Get2DRotProjection(const TVector3 &punormal, const TVector3 &xaxis, const TVector3 &yaxis, const Double_t &theta, TVector3 point, Double_t *x, Double_t *y)
{
  // 09.2006 - P.-L. Drouin
  // Compute the projection of the 3D point point, as seen from the
  // origin, in a plane that is defined by the normal unit vector
  // pnormal and located 1 unit from the origin. xaxis and yaxis
  // are two unit 3D vectors in that plane that define the axes of
  // the coordinates system. theta allows to rotate the projected
  // point in the plane (no rotation when theta=0).
  // x and y are filled with the coordinates of the projected point.
  // The functions returns kTRUE if point is located between the
  // plane and the origin.

  Double_t sprod=point*punormal;

  if(sprod<0) return kTRUE;
  point*=1./sprod;

  point-=punormal;

  //Coordinates of sav#'s in the new coordinates system
  *x=point*xaxis;
  *y=point*yaxis;

  if(theta!=0) {
    Double_t nx,ny;
    nx=TMath::Cos(theta)*(*x)-TMath::Sin(theta)*(*y);
    ny=TMath::Sin(theta)*(*x)+TMath::Cos(theta)*(*y);
    *x=nx;
    *y=ny;
  }

  return kFALSE;
}
//______________________________________________________________________________
Int_t QOptics::GetNcd2DProjection(const TVector3 &punormal, const TVector3 &xaxis, const TVector3 &yaxis, const Double_t &theta, const Int_t ncd, Double_t *ncdx, Double_t *ncdy)
{
  // 09.2006 - P.-L. Drouin
  // Compute the projection of the NCD ncd, as seen from the source,
  // in a plane that is defined by the normal unit vector pnormal
  // and located 1 unit from the source. xaxis and yaxis are two
  // unit 3D vectors in that plane that define the axes of the
  // coordinates system. theta allow to rotate the projected NCD
  // in the plane (no rotation when theta=0). ncdx and ncdy are
  // filled with the coordinates of the projected NCD corners in
  // the plane. If a NCD crosses the plane, it is shorten in order
  // to allow the projection.
  // The function returns -2 if the NCD is out of the field of view,
  // -1 if the source is in the NCD and a value greater than 0 if
  // the algorithm produces an unexpected result. Otherwise, if the
  // projection succeeded, 0 is returned.

  Double_t x0; //Horizontal distance of source from NCD
  TVector3 vbuf=fNcdArray->GetCounter(ncd)->GetTop()-source;
  TVector3 dvbuf;
  Double_t bottomZ,dbuf;
  Int_t ncdpprt, ncdpprb; //Results of NCD points projection
  Int_t ncdopt1=0, ncdopt2=0; //Results of other NCD points calculation
  const Double_t corncdangle=89.99/180.*TMath::Pi(); //If this value is too far from 90 degrees, the projection becomes inaccurate when the source is close to an NCD
  TVector3 ncdc[4]; //NCD corners
  
  //fNcdArray->GetCounter(i)->GetTop().Print();
  x0=TMath::Sqrt(vbuf.X()*vbuf.X()+vbuf.Y()*vbuf.Y());
  
  //printf("%f\t%f\t%f\t%f\n",hdist,fNCDRadius,vbuf.Z(),fNcdArray->GetCounter(ncd)->GetBottom().Z()-source.Z());
  
  bottomZ=(fNcdArray->GetCounter(ncd)->GetBottom()-source).Z();
  
  //If the source position is outside the NCD
  if(x0>fNCDRadius || vbuf.Z()<0 || bottomZ>0) {
    //Compute the position of the 4 NCD corners, as seen from the source position
    
    dbuf=fNCDRadius*TMath::Sqrt(1-fNCDRadius*fNCDRadius/(x0*x0))/x0;
    dvbuf.SetXYZ(vbuf.Y()*dbuf,-vbuf.X()*dbuf,0);
    dbuf=(x0*x0-fNCDRadius*fNCDRadius)/(x0*x0);
    vbuf.SetXYZ(vbuf.X()*dbuf,vbuf.Y()*dbuf,vbuf.Z());
    
    ncdc[0]=vbuf-dvbuf;
    ncdc[1]=vbuf+dvbuf;
    ncdc[2].SetXYZ(ncdc[1].X(),ncdc[1].Y(),bottomZ);
    ncdc[3].SetXYZ(ncdc[0].X(),ncdc[0].Y(),bottomZ);
    //ncdc[0].Print();
    //ncdc[1].Print();
    //ncdc[2].Print();
    //ncdc[3].Print();
    //Try to compute the projection of these corners in the punormal plane (fail if corners are outside the field of view)
    //printf("Angles before correction\n");
    ncdpprt=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[0],ncdx,ncdy);
    ncdpprt+=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[1],ncdx+1,ncdy+1);
    ncdpprb=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[2],ncdx+2,ncdy+2);
    ncdpprb+=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[3],ncdx+3,ncdy+3);

    //If at least one corner is in the field of view
    if(ncdpprt<2 || ncdpprb<2) {

      //printf("Before:\n");
      //punormal.Print();
      //ncdc[0].Print();
      //ncdc[1].Print();
      //ncdc[2].Print();
      //ncdc[3].Print();

      //If one of the top corners is out the field of view, take 2 points closer to bottom corners
      if(ncdpprt) {
	ncdopt1=GetOtherNcdPoint(punormal,ncdc[0],1,corncdangle,ncdc);
	ncdopt2=GetOtherNcdPoint(punormal,ncdc[1],1,corncdangle,ncdc+1);

	if(ncdopt1>0 || ncdopt2>0) {
	  //Assertion: No NCD point with an angle < corncdangle (NCD out of field of view)

	  //printf("GetOtherNcdPoint failed with top corner\n");
	  //printf("Angle Top 1: %f\n",TMath::ACos((ncdc[0]*punormal)/ncdc[0].Mag())/TMath::Pi()*180.);
	  //printf("Angle Top 2: %f\n",TMath::ACos((ncdc[1]*punormal)/ncdc[1].Mag())/TMath::Pi()*180.);
	  //printf("Angle Bottom 1: %f\n",TMath::ACos((ncdc[2]*punormal)/ncdc[2].Mag())/TMath::Pi()*180.);
	  //printf("Angle Bottom 2: %f\n",TMath::ACos((ncdc[3]*punormal)/ncdc[3].Mag())/TMath::Pi()*180.);
	  return -2;
	}
      }

      //If one of the bottom corners is out the field of view, take 2 points closer to top corners
      if(ncdpprb) {

	if(ncdopt1!=-1 || ncdopt2!=-1) {

	  if(GetOtherNcdPoint(punormal,ncdc[2],0,corncdangle,ncdc+2)>0 || GetOtherNcdPoint(punormal,ncdc[3],0,corncdangle,ncdc+3)>0) {
	    //Assertion: No NCD point with an angle < corncdangle (NCD out of field of view)

	    //printf("GetOtherNcdPoint failed with bottom corner\n");
	    //printf("Angle Top 1: %f\n",TMath::ACos((ncdc[0]*punormal)/ncdc[0].Mag())/TMath::Pi()*180.);
	    //printf("Angle Top 2: %f\n",TMath::ACos((ncdc[1]*punormal)/ncdc[1].Mag())/TMath::Pi()*180.);
	    //printf("Angle Bottom 1: %f\n",TMath::ACos((ncdc[2]*punormal)/ncdc[2].Mag())/TMath::Pi()*180.);
	    //printf("Angle Bottom 2: %f\n",TMath::ACos((ncdc[3]*punormal)/ncdc[3].Mag())/TMath::Pi()*180.);
	    return -2;
	  }

	} else {
	  //printf("NCD out of the field of view\n");
	  return -2;
	}
      }

      //printf("After:\n");
      //punormal.Print();
      //ncdc[0].Print();
      //ncdc[1].Print();
      //ncdc[2].Print();
      //ncdc[3].Print();

      //2D projections of top corners are recomputed if they have been relocated
      if(ncdpprt) {
	//printf("Angles after top corners correction\n");
	//ncdc[0].Print();
	//ncdc[1].Print();
	ncdpprt=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[0],ncdx,ncdy);
	ncdpprt+=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[1],ncdx+1,ncdy+1);
      }

      //2D projections of bottom corners are recomputed if they have been relocated
      if(ncdpprb) {
	//printf("Angles after bottom corners correction\n");
	//ncdc[2].Print();
	//ncdc[3].Print();
	ncdpprb=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[2],ncdx+2,ncdy+2);
	ncdpprb+=Get2DRotProjection(punormal,xaxis,yaxis,theta,ncdc[3],ncdx+3,ncdy+3);
      }

      //printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",ncdx[0],ncdy[0],ncdx[1],ncdy[1],ncdx[2],ncdy[2],ncdx[3],ncdy[3]);

      if(ncdpprt || ncdpprb) {
	printf("ERROR: NCD corner(s) out of the field of view\n");
	printf("Angle Top 1: %f\n",TMath::ACos((ncdc[0]*punormal)/ncdc[0].Mag())/TMath::Pi()*180.);
	printf("Angle Top 2: %f\n",TMath::ACos((ncdc[1]*punormal)/ncdc[1].Mag())/TMath::Pi()*180.);
	printf("Angle Bottom 1: %f\n",TMath::ACos((ncdc[2]*punormal)/ncdc[2].Mag())/TMath::Pi()*180.);
	printf("Angle Bottom 2: %f\n",TMath::ACos((ncdc[3]*punormal)/ncdc[3].Mag())/TMath::Pi()*180.);
	return 3;
      }

    } else {
      //printf("NCD out of the field of view\n");
      return -2;
    }
  } else {
    //printf("Source is in NCD %i\n",ncd);
    return -1;
  }
  return 0;
}
//______________________________________________________________________________
Int_t QOptics::GetOtherNcdPoint(const TVector3 &punormal, const TVector3 &ancdpoint, const Bool_t findupper, const Double_t &theta, TVector3 *newpoint)
{
  // 09.2006 - P.-L. Drouin
  // Compute a vector newpoint that points to a NCD string axis and
  // that makes an angle theta with the unit vector punormal, given
  // an existing point ancdpoint along the NCD axis. If findupper
  // is set to kTRUE, a point with higher Z will be chosen if more
  // than one solution exists. The function returns 1 if no solution
  // is found. It returns -1 if one solution is found and -2 if two
  // solution exist.

  // Assertion Cos(theta)>0
  Double_t ct=TMath::Cos(theta);
  Double_t A=(punormal.X()*ancdpoint.X()+punormal.Y()*ancdpoint.Y());
  Double_t C=punormal.Z()*punormal.Z()-TMath::Power(ct,2);
  Double_t B=(ancdpoint.X()*ancdpoint.X()+ancdpoint.Y()*ancdpoint.Y())*C+A*A;

  if(B<0) return 1;

  B=TMath::Sqrt(B);
  Double_t z;
  Int_t snz=(2*(punormal.Z()>0)-1);
  Int_t snzct=(2*(punormal.Z()*punormal.Z()-ct*ct>0)-1);

  //If two solutions with ct>0
  if(-snzct*ct*A>TMath::Abs(punormal.Z())*B){
    //printf("Two solutions\n");
    z=(-punormal.Z()*A+(2*findupper-1)*(2*(C>0)-1)*B*ct)/C;
    newpoint->SetXYZ(ancdpoint.X(),ancdpoint.Y(),z);
    return -2;

  //Else if only one solution
  } else if(-snzct*ct*A>-TMath::Abs(punormal.Z())*B) {
    //printf("One solution\n");
    z=(-punormal.Z()*A+snzct*snz*B*ct)/C;
    newpoint->SetXYZ(ancdpoint.X(),ancdpoint.Y(),z);
    return -1;

  } else {
    //printf("No solution\n");
    return 1;
  }

}
//______________________________________________________________________________
void QOptics::GetLEIntersection(const Double_t &a, const Double_t &b, const Double_t &x1, const Double_t &y1, const Double_t &x2, const Double_t &y2, Double_t *m, Double_t *c, Double_t *ix1, Double_t *iy1, Double_t *ix2, Double_t *iy2, Int_t *status)
{
  // 09.2006 - P.-L. Drouin
  // Compute the intersection point(s) of a line defined by the end
  // points (x1,y1) and (x2,y2) with an ellipse centered at the origin
  // with semi-axes a and b and store their coordinates in ix1, iy1,
  // ix2, iy2. The values of x1 and x2 are assumed to be different. m
  // and c are set respectively to the slope and the y-intercept of
  // the line. status is set to the number of intersection points if
  // the line intersects the ellipse. If no intersection is possible
  // (even if the line had an infinite length), status is set to 4
  // if c<0 or 8 otherwise. ix1, iy1, ix2 and iy2 are set even the
  // line ends before intersecting the ellipse.

  Double_t isr; //Value of the expression in the square root when solving for intersection
  //Assertion: x2-x1!=0
  *m=(y2-y1)/(x2-x1);
  *c=y1-(*m)*x1;

  isr=(*c)*(*c)*(*m)*(*m)-((*m)*(*m)+b*b/(a*a))*((*c)*(*c)-b*b);

  *status=0;

  if(isr<=0) {
    //No intersection are possible (or the segment is tangent to the ellipse)
    //*status=4 if c<0 or 8 otherwise
    *status=4*(1+((*c)>=0));
    //printf("%f\t%f\t%f\t%f\t%f\n",isr,(*m),(*c),x1,x2);
    return;

  } else {
    //Possibility of two intersections
    Double_t sr=TMath::Sqrt(isr);
    Double_t den=(*m)*(*m)+b*b/(a*a);
    *ix1=(-(*c)*(*m)-sr)/den;
    *iy1=(*m)*(*ix1)+(*c);

    //if ix1 is in the possible interval
    if((*ix1-x1)*(*ix1-x2)<=0) {
      //printf("One, %f\n",(*ix1-x1)*(*ix1-x2));
      (*status)++;
    }

    *ix2=(-(*c)*(*m)+sr)/den;
    *iy2=(*m)*(*ix2)+(*c);

    //if ix2 is in the possible interval
    if((*ix2-x1)*(*ix2-x2)<=0) {
      //printf("Two, %f\n",(*ix2-x1)*(*ix2-x2));
      (*status)++;
    }

    //printf("%f\t%f\t%f\t%i\t%f\t%f\t%f\t%f\n",isr,(*m),(*c),*status,x1,x2,*ix1,*ix2);
  }
}
//______________________________________________________________________________
Double_t QOptics::GetEllipseIntegral(const Double_t &a, const Double_t &b, const Double_t *xypoints)
{
  // 09.2006 - P.-L. Drouin
  // Return the area of an ellipse from which a right part has been
  // removed by cutting the ellipse with a straight line. a and b
  // are respectively the major and minor semi-axis. xypoints[0]
  // and xypoints[1] must be set to the x coordinates of the
  // intersection points of the line with the ellipse. xypoints[2]
  // and xypoints[3] must be set to the corresponding y coordinates
  // of the intersection points.

  const Double_t *xmin, *xmax, *ymin, *ymax;
  Double_t aa=a*a;

  if(xypoints[0]<xypoints[1]) {
    xmin=xypoints;
    xmax=xypoints+1;
    ymin=xypoints+2;
    ymax=xypoints+3;

  } else if(xypoints[0]>xypoints[1]) {
    xmin=xypoints+1;
    xmax=xypoints; 
    ymin=xypoints+3;
    ymax=xypoints+2;

  } else {
    return b/a*(0.5*aa*TMath::Pi()+(*xmin)*TMath::Sqrt(aa-(*xmin)*(*xmin))+aa*TMath::ASin(*xmin/a));
  }

  return b/(2*a)*(aa*TMath::Pi()+(*xmin)*TMath::Sqrt(aa-(*xmin)*(*xmin))+aa*TMath::ASin(*xmin/a)+(*xmax)*TMath::Sqrt(aa-(*xmax)*(*xmax))+aa*TMath::ASin(*xmax/a))+(*ymax<*ymin?1:-1)*0.5*(*xmax-*xmin)*(*ymin+*ymax);

}
//______________________________________________________________________________
Bool_t QOptics::GetDistancesToNcd(Int_t aNcdIndex,Double_t aZ,Double_t anAngle)
{
  // 05.2006 - O.Simard : Introduced this function to enable 
  // distance calculations from the source to Ncds.
  // The arguments allow to calculate the distances to the Ncd surface.
  //  - aZ: z-value on the counter.
  //  - anAngle: angle from the normal to the Ncd surface (pointing
  //             to the source).
  // The ncd radius is fixed. Assumes Ncds are vertical.
  //
  // The distance in the d2o and time-of-flight are set. The fCtheta
  // variable is also set, but has a different meaning: 
  // cos(theta_NCD) instead of cos(theta_PMT).

  // --------------------
  if (rs > fAVInnerRadius) return kFALSE;  // Source shouldn't be outside AV

  // Test whether target is a valid Ncd inside the AV, and check where the source is.
  if(aNcdIndex >= fNcdArray->GetNCounters() || aNcdIndex < 0 || !fNcdArray->GetCounter(aNcdIndex)){
    Warning("GetDistancesToNcd","Bad Ncd index.");
    return kFALSE;
  }

  // Check boundaries on z
  if((aZ > (fNcdArray->GetCounter(aNcdIndex)->GetTop()).Z()) || 
     (aZ < (fNcdArray->GetCounter(aNcdIndex)->GetBottom()).Z())){
    Warning("GetDistancesToNcd","Z-coordinate given is not within the Ncd length.");
    return kFALSE;
  }

  // Check boundaries on the angle
  if((anAngle < -0.5*M_PI) || (anAngle > 0.5*M_PI)){
    Warning("GetDistancesToNcd","Angle given is behind the Ncd from this point of view.");
    return kFALSE;
  }
  // Get the Ncd vector at z = 0
  TVector3 ncdvec = fNcdArray->GetCounter(aNcdIndex)->GetCenter();

  // This vector should be added to ncdvec to point on a point
  // on the surface.
  TVector3 normal((source-ncdvec).Unit()); // opposite direction, unit length
  normal.SetPhi(normal.Phi() + anAngle);   // rotate according to anAngle
  
  // Calculate the length and theta of the shift
  Double_t length = sqrt(aZ*aZ + fNCDRadius*fNCDRadius);
  Double_t theta = 0.5*M_PI - atan(aZ/fNCDRadius); // theta in spherical coords.

  // Vector to be added to ncdvec
  TVector3 ncdadd(0,0,1);
  ncdadd.SetMagThetaPhi(length,theta,normal.Phi());
  
  // Vector to use to get distances
  TVector3 ncdsur(ncdvec + ncdadd);

  // Vector from source to Ncd surface
  TVector3 dir(ncdsur - source);

  // Distance, time-of-flight, and incident angle calculations
  fDd2o = dir.Mag();      // from source to surface
  fTimeOfFlight = fDd2o/GetVgroupD2O(); // time
  fCtheta = cos(((source - ncdsur).Unit()).Angle(ncdadd.Unit())); // incident on surface

  // Ncd shadowing: this path goes straight on the Ncd
  fNcd = kTRUE;
  fCounterIndex = aNcdIndex;
  
  // other relevant default values
  fDacrylic = 0.;
  fDh2o = 0.;
  fTransPwr = 0;
  fPanelNum = -1;
  fLayerNum = -1;
  fRope = kFALSE;
  fPipe = kFALSE;
  fPromptAVRefl = kFALSE;
  bellyplate = kFALSE;
  acrylicneck = kFALSE;

  return kTRUE;
}
//______________________________________________________________________________
Double_t QOptics::GetNcdSolidAngle(Int_t aNcdIndex,Double_t aZinitial,Double_t aZfinal)
{
  // 05.2006 - O.Simard, Pierre-Luc Drouin.
  // Calculates the solid angle of a Ncd from the current source position.
  // Assumes the Ncds are vertical.
  // The surface of the Ncds are modelled as a rectangle surface (effects
  // on the radial distance due to the cylindrical surface are neglected).
  //
  // The arguments aZinitial and aZfinal are supposed to be limits of
  // integration in z (manip. coords.) on the Ncd counter. If the limits
  // are left to default values or exceed the physical limits as read
  // in the GEDP 111 bank, then the real limits are used and the length is
  // the full length.
  // 
  // Warning: aZinitial must be less than aZfinal : aZinitial < aZfinal.
  // Otherwise the calculation will be unsuccessful.
  //
  // returns -1 if the calculation is unsuccessful.
  //

  if (rs > fAVInnerRadius) return -1;  // Source shouldn't be outside AV

  // --------------------
  // Test whether target is a valid Ncd inside the AV, and check where the source is.
  if(aNcdIndex >= fNcdArray->GetNCounters() || aNcdIndex < 0 || !fNcdArray->GetCounter(aNcdIndex)){
    Warning("GetNcdSolidAngle","Bad Ncd index.");
    return -1;
  }
  // initial must be smaller than final
  if(aZinitial > aZfinal) return -1;
  
  // Get the Ncd vector at z = 0
  TVector3 ncdvec = fNcdArray->GetCounter(aNcdIndex)->GetCenter();

  // Ncd coordinates from geometry
  Double_t ncd_x = ncdvec.X();
  Double_t ncd_y = ncdvec.Y();
  Double_t ncd_ztop = fNcdArray->GetCounter(aNcdIndex)->GetTop().Z();
  Double_t ncd_zbot = fNcdArray->GetCounter(aNcdIndex)->GetBottom().Z();

  // Check if the arguments are valid,
  // otherwise set them to the actual ncd limits in z.
  if((aZinitial == -999)||(aZinitial < ncd_zbot)) aZinitial = ncd_zbot;
  if((aZfinal == 999)||(aZfinal > ncd_ztop)) aZfinal = ncd_ztop;

  // Middle point of Ncd counter
  Double_t middle = 0.5*(ncd_zbot + ncd_ztop);

  // Source coordinates
  Double_t sou_x = source.X();
  Double_t sou_y = source.Y();
  Double_t sou_z = source.Z();

  // Distance perpendicular to Ncd-axis: only use x and y for both
  Double_t dist = TMath::Sqrt(TMath::Power((ncd_x-sou_x),2) + TMath::Power((ncd_y-sou_y),2));
  
  // Distance between the Ncd middle point and perpendicular to source
  Double_t deltaz = sou_z - middle;

  // Solid angle calculation: analytic expression from dA \dot dn / r^2
  // intgrated over Z between the specified limits
  Double_t solidangle;
  Double_t diff_hi = aZfinal - deltaz;
  Double_t diff_lo = aZinitial - deltaz;
  Double_t length_hi = TMath::Sqrt(TMath::Power(dist,2) + 
				   TMath::Power(diff_hi,2));
  Double_t length_lo = TMath::Sqrt(TMath::Power(dist,2) + 
				   TMath::Power(diff_lo,2));
  Double_t limit_hi = diff_hi/length_hi;
  Double_t limit_lo = diff_lo/length_lo;
  
  solidangle = (2*fNCDRadius/dist)*(limit_hi - limit_lo);
  
  if((solidangle < 0) || (solidangle > 4*M_PI)){
    Warning("GetNcdSolidAngle","Solid angle calculated is outside range: %f",solidangle);
    solidangle = -1; // error
  }

  return solidangle;
}
//________________________________________________________________________________
Double_t QOptics::GetTimeOfFlight()
{
  // Give the latest time of flight calculated in GetDistances()

  return fTimeOfFlight;
}
//______________________________________________________________________________
Bool_t QOptics::getDistancesOutside(Int_t pmtn, Double_t &d2o, Double_t &acrylic, 
				    Double_t &h2o, Double_t &ctheta, 
				    Int_t &avpanel, Double_t &transpwr)
{
  // Calls getDistancesOutside() and fills passed variables with results if
  // successful.

  //--------------------
  // Test whether target is a valid PMT on the PSUP, and check where the source is.
  if (gPMTxyz->IsInvalidPMT(pmtn) || !(gPMTxyz->IsNormalPMT(pmtn))) return kFALSE;
  
  TVector3 pmtvec = GetPMTPosition(pmtn);
  if (pmtvec(0) == -9999) return kFALSE;  // PMT without a position...?

  // Get normal vector to the pmt (points outwards, away from the centre of the
  // detector.)
  fPMTNormal = GetPMTNormal(pmtn);

  if (getDistancesOutside(pmtvec,fPMTNormal)) {

    d2o = GetDd2o();
    acrylic = GetDacrylic();
    h2o = GetDh2o();
    ctheta = GetCtheta();
    avpanel = GetPanel();
    transpwr = GetTransPwr();
    return kTRUE;

  } else return kFALSE;
}
//______________________________________________________________________________
Bool_t QOptics::getDistancesOutside(TVector3 p,TVector3 n)
{
  // Calculate the distance from a source outside the acrylic vessel to a pmt.
  //
  // Private function: called by QOptics::GetDistances() for sources outside the
  // acrylic vessel.
  //
  // n is the PMT normal vector (points outwards).
  //
  // Routine added by Chris Jillings, 8-3-00
  // Modified and completed (for paths through the AV) by Bryce Moffat, 21-7-00

  fDd2o = fDacrylic = fDh2o = -9999;
  fTimeOfFlight = -9999; // set to error value.
  
  if (p(0) == -9999) return kFALSE;  // PMT without a position...?
  rp = p.Mag();  // PMT radius in class variable
  
  if (rp < r2) return kFALSE;  // PMT must be outside the AV...?
  if (rs > rp) return kFALSE;  // Source shouldn't be outside PSUP/PMT!

  //--------------------
  // Check for direct path from source to PMT first
  // thetainit - angle between source vector and direct light path to PMT
  //             (for sources outside the AV, this is near 0 for direct tubes,
  //             and near pi for tubes across the AV).
  // thcutout - line of sight just grazes the outer surface of AV
  // thcutin  - line of sight just grazes the inner surface of AV
  Double_t psmag = (p-source).Mag();
  Double_t sint = 1/(psmag*rs)*((p.Cross(source)).Mag());
  Double_t cost = (rp*rp - psmag*psmag - rs*rs)/(2*psmag*rs);
  Double_t thetainit = atan2(sint,cost);

  Double_t thcutout = M_PI - asin((r2+5.0)/rs); // Add 5cm for border effects
  Double_t thcutin = M_PI - asin((r1-5.0)/rs);

  // Path is grazing the AV: reject this case as too error-prone
  if (thetainit >= thcutout && thetainit <= thcutin) return kFALSE;

  if (thetainit < thcutout) {
    // Path is exclusively outside the AV: direct line of sight
    // In this case, thinit is the angle of the path!
    // Take care of light-water paths.
    // At end of this if block, return kTRUE
    fDd2o = 0;
    fDacrylic = 0;
    fDh2o = (p-source).Mag();

    // Cos(theta) of angle of incidence of light onto front face of PMT bucket
    fCtheta = (n * (p-source).Unit());
 
    // ray can't intersect a NCD anchor
    fAnchor = kFALSE;

    bellyplate = kFALSE;   // ray can't intersect a belly plate
    fNumRefract = 0;
    fAcrylicVector.SetXYZ(0,0,0);  // No AV intersection

    fPMTrelvec = (p - source).Unit();
    fPMTincident = (p - source).Unit();

    acrylicneck = IsChimney(source,p); // check for ray through AV neck

    fPanelNum = -1;
    fLayerNum = -1;

    fRope = IsRope(source,p);
    fPipe = kFALSE;

    fPromptAVRefl = IsPromptAVRefl(source,p);

    fTransPwr = 1;

    if (fCtheta>0 && fCtheta<1) fPMTAngResp = GetPMTAngResp(fCtheta);
    else fPMTAngResp = 0;

    if (DEBUG_QOptics) {
      printf("DIRECT: %12.5g %12.5g %12.5g\n",fDh2o,fCtheta,fPMTAngResp);
    }

    fTimeOfFlight = fDh2o/GetVgroupH2O();

    return kTRUE;
  } else if (thetainit > thcutin) {
    // Path is h2o/av/d2o/av/h2o from source to PMT
    // If we are here, we have work to do.

    // --------------------
    // Set up local coordinate system:
    // xunit - points along radial direction from origin to source position
    // zunit - perpendicular to plane containing origin, source, and pmt
    // yunit - perpendicular to source vector; in plane of source and pmt vectors
    
    // Set source() to something small to make sure vector algebra all works!
    if (rs == 0.0) SetSource(1e-5,0,0);
    TVector3 xunit = source.Unit();
    TVector3 zunit = (xunit.Cross(p)).Unit();
    TVector3 yunit = (zunit.Cross(xunit)).Unit();
    
    thPMTtarget = atan2((p.Cross(source)).Mag(),p*source);

    Double_t theta = rtsafe(M_PI_2,M_PI,1e-3);
    Double_t th1 = theta1(theta);
    Double_t th2 = theta2(theta) + th1;
    Double_t th3 = theta3(theta) + th2;
    Double_t th4 = theta4(theta) + th3;
    //  Double_t th5 = thPMTtarget;

    TVector3 a1 = cos(th1)*xunit + sin(th1)*yunit;
    a1.SetMag(r2);

    TVector3 a2 = cos(th2)*xunit + sin(th2)*yunit;
    a2.SetMag(r1);

    TVector3 a3 = cos(th3)*xunit + sin(th3)*yunit;
    a3.SetMag(r1);

    TVector3 a4 = cos(th4)*xunit + sin(th4)*yunit;
    a4.SetMag(r2);
    
    fPMTrelvec = (a1 - source).Unit();
    fPMTincident = (p - a4).Unit();

    fDd2o = (a3-a2).Mag();
    fDacrylic = (a4-a3).Mag() + (a2-a1).Mag();
    fDh2o = (a1-source).Mag() + (p-a4).Mag();
    
    // Cos(theta) of angle of incidence of light onto front face of PMT bucket
    fCtheta = (n * (p-a4).Unit());

    // Does the light ray go through the acrylic vessel neck? or a bellyplate?
    fNumRefract = 2;
    fAcrylicVector = a1;
    Double_t a1theta = a1.Theta();
    Double_t a1phi = a1.Phi();
    fAcrylicVector2 = a4;
    Double_t a4theta = a4.Theta();
    Double_t a4phi = a4.Phi();
    acrylicneck = IsNeckOptics(a1theta) || IsNeckOptics(a4theta) ||
      IsChimney(source,a1) || IsChimney(a4,p);

    bellyplate = IsBelly(a1theta,a1phi) || IsBelly(a4theta,a4phi);
    // if (bellyplate) fDacrylic *= 2.0;  // Hmmmmm...


    // Check if the ray passes through a NCD anchor
    // we must consider the points and directions INSIDE the AV
    // a2 - Point inside the AV where the first ray-acrylic interaction occured
    // a3 - Point inside the AV where the second ray-acrylic interaction occured
    TVector3 d2odirection(a3-a2);
    TVector3 d2odirection2(a2-a3);
    fAnchor = IsAnchor(a3,d2odirection) || IsAnchor(a2,d2odirection2);
    
    fPanelNum = GetPanel(a1theta,a1phi) + 1000*GetPanel(a4theta,a4phi);
    fLayerNum = GetLayer(a1theta) + 1000*GetLayer(a4theta);

    fRope = IsRope(source,a1) || IsRope(a4,p);
    fPipe = IsPipe(a2) || IsPipe(a3);

    fPromptAVRefl = IsPromptAVRefl(source,p);

    Double_t cosiha1 = -a1.Unit()*(a1-source).Unit(); // h2o/acrylic inc ang
    if (cosiha1 > 1.0) cosiha1 = 1.0;
    else if (cosiha1 < -1.0) cosiha1 = -1.0;
    Double_t incidentha = acos(cosiha1);
    //    incidentha = asin(rs/r2*sin(theta));
    
    Double_t cosiad2 = -a2.Unit()*(a2-a1).Unit(); // acrylic/d2o incident ang
    if (cosiad2 > 1.0) cosiad2 = 1.0;
    else if (cosiad2 < -1.0) cosiad2 = -1.0;
    Double_t incidentad = acos(cosiad2);
    //    incidentad = asin(nh/na*rs/r1*sin(theta));
    
    Double_t cosida3 = a3.Unit()*(a3-a2).Unit(); // d2o/acrylic incident ang
    if (cosida3 > 1.0) cosida3 = 1.0;
    else if (cosida3 < -1.0) cosida3 = -1.0;
    Double_t incidentda = acos(cosida3);
    //    incidentda = asin(nh/nd*rs/r1*sin(theta));

    Double_t cosiah4 = a4.Unit()*(a4-a3).Unit(); // acrylic/h2o inc ang
    if (cosiah4 > 1.0) cosiah4 = 1.0;
    else if (cosiah4 < -1.0) cosiah4 = -1.0;
    Double_t incidentah = acos(cosiah4);
    //    incidentah = asin(rs/r2*nh/na*sin(theta));
    
    fTransPwr = 0.5 * (
		      GetTransPwrPara(nh,na,incidentha) *
		      GetTransPwrPara(na,nd,incidentad) *
		      GetTransPwrPara(nd,na,incidentda) *
		      GetTransPwrPara(na,nh,incidentah)
		      +
		      GetTransPwrPerp(nh,na,incidentha) *
		      GetTransPwrPerp(na,nd,incidentad) *
		      GetTransPwrPerp(nd,na,incidentda) *
		      GetTransPwrPerp(na,nh,incidentah) );

    if (fCtheta>0 && fCtheta<1) fPMTAngResp = GetPMTAngResp(fCtheta);
    else fPMTAngResp = 0;

    if (DEBUG_QOptics) {
      printf("REFRACT: %12.5g %12.5g %12.5g %12.5g %16.10g\n",
	     fDacrylic, fCtheta,
	     incidentda*180./M_PI, incidentah*180./M_PI,
	     fTransPwr);
    }

    fTimeOfFlight =
      fDd2o/GetVgroupD2O() + fDh2o/GetVgroupH2O() + fDacrylic/GetVgroupAcrylic();

    if (fDacrylic > 0.0) return kTRUE;
    else return kFALSE;
  }

  return kFALSE; // routine falls through to here if no path calculable...
}
//________________________________________________________________________________
void QOptics::ShootToPSUP(TVector3& x, TVector3& p, TVector3& psup)
{
  // Given an initial position and direction, x and p, calculate the point at 
  // which the PSUP is struck.

  // Routine added by Chris Jillings, 08-Mar-00

  // These are defined as #define's in QOptics.h...
  //  const Int_t kD2O = 0;
  //  const Int_t kAcrylic = 1;
  //  const Int_t kH2O = 2;
  const TVector3 zero(0,0,0);

  Double_t xmag = x.Mag();
  if( xmag>840 ) psup = zero;
  return;
  if( xmag<r1 ) {
    printf("Routine ShootToPSUP does not support a source in the D2O.\n");
    return;
  }
  Int_t whereAmI;
  Bool_t finished = kFALSE;
  TVector3 xc = x; // current values
  TVector3 pc = p;  
  TVector3 xn;  // next values
  TVector3 pn;
  Bool_t stLinePath;
  Double_t cutoffCosine;
  Double_t transpwr;
  Double_t cummPower = 1; // gets worn down at interfaces
  Bool_t tir;  // totalinternal reflection flag;

  if( xmag<r1 ) whereAmI = kD2O;
  else if (xmag<r2 ) {
    whereAmI = kAcrylic;
  } else {
    whereAmI = kH2O;
  }

  if( xmag==r1 ) {
    if( pc.Dot(xc)<0 ) whereAmI = kD2O;
    else whereAmI = kAcrylic;
  }
  if( xmag==r2 ) {
    if( pc.Dot(xc)<0 ) whereAmI = kAcrylic;
    else whereAmI = kH2O;
  }

  Int_t iterationCounter = 0;

  while ( !finished ) {
    xmag = xc.Mag();
    if( xmag<r1 ) whereAmI = kD2O;
    else if (xmag<r2 ) {
      whereAmI = kAcrylic;
    } else {
      whereAmI = kH2O;
    }

    switch( whereAmI ) {
    case kD2O:
      LineToSphere(xc,pc,xn,r1);
      Refract(whereAmI,kAcrylic,xn,pc,pn,transpwr,tir);
      cummPower *= transpwr;
      whereAmI = kAcrylic;
      break;
    case kH2O:
      stLinePath = kFALSE;
      if( xc.Dot(pc) >0.0 ) stLinePath = kTRUE;
      else {
	cutoffCosine = -sqrt((xc.Mag2() - r2*r2)/xc.Mag2());
	if( xc.Dot(pc)/xmag > cutoffCosine ) stLinePath = kTRUE;
      }
      if( stLinePath == kTRUE ) {
	finished = kTRUE;
	LineToSphere(xc,pc,psup,850); 
      }  else {
	LineToSphere(xc,pc,xn,r2);
	Refract(whereAmI,kAcrylic,xn,pc,pn,transpwr,tir); 
	cummPower *= transpwr;
	whereAmI = kAcrylic;
      }
      break;
    case kAcrylic:
      cutoffCosine = -sqrt((xc.Mag2() - r1*r1)/xc.Mag2());
      if( xc.Dot(pc)/xmag >cutoffCosine ) {
	LineToSphere(xc,pc,xn,r2);
	Refract(whereAmI,kH2O,xn,pc,pn,transpwr,tir);
	cummPower *= transpwr;
	whereAmI = kH2O;
      } else {
	LineToSphere(xc,pc,xn,r1);
	Refract(whereAmI,kD2O,xn,pc,pn,transpwr,tir);
	cummPower *= transpwr;
	whereAmI = kD2O;
      }
      if( tir ) { // will not escape
	finished = kTRUE;
	psup = zero;
      }

      break;
    default:
      printf("Unrecognized medium (%d) in QOPtics::ShootToPSUP() switch.\n",whereAmI);
      break;
    } // end of switch on media
    xc = xn; pc = pn;  // update current position/direction
    if( iterationCounter++>20 ) { 
      printf("Ran out of iterations (>20) in QOptics::ShootToPSUP().\n");
      finished = kTRUE;
      psup = zero;
    }
  }
}
//______________________________________________________________________________
void QOptics::LineToSphere(TVector3& x, TVector3& p, TVector3& xsp, Double_t radius)
{
  // Finds the intersection of a line from a point to a sphere of given 
  // radius along direction p.

  // Routine added by Chris Jillings, 08-Mar-00

  Double_t t1, t2; // parameter of line
  Double_t rsq = radius*radius;
  Double_t a = 1.0;
  Double_t b,c;
  const TVector3 zero(0,0,0);

  b = 2*(x.Dot(p));
  c = x.Mag2() - rsq;

  Double_t disc =  -b + sqrt( b*b - 4*a*c );
  if( disc<1.0E-12 ) {
    printf("Bad discriminant error: disc in QOptics::LineToSphere().\n");
    xsp = zero;
    return;
  } 
  disc = sqrt(disc);

  t1 =  ( -b + disc )/ (2*a);
  t2 =  ( -b - disc )/ (2*a);
  
  if( (t1>0 && t2>0) || (t1<=0 && t2<=0) ) {
    printf("Parameterization variables t1=%g,t2=%g out of range in "
	   "QOptics::LineToSphere().\n",t1,t2);
    xsp = zero;
    return;
  }
  if( t1>0 ) xsp = x + p*t1;
  else xsp = x + p*t2;
}
//______________________________________________________________________________
void QOptics::Refract(Int_t m1, Int_t m2, TVector3& n, TVector3& p1, TVector3& p2, 
		      Double_t& transpwr, Bool_t& tir)
{
  // Calculate the refraction of light from medium m1 to medium m2, given the
  // normal vector to the surface n and the incomming ray vector p1.
  // The refracted ray vector p2, transmitted power (Fresnel) or a total internal
  // reflection flag tir are returned.

  // Routine added by Chris Jillings, 08-Mar-00

  // m1, m2 are the media codes.
  // n is the normal vector 
  // p1 is input vector
  // p2 is output
  // tir is the total internal reflection flag

  // These are defined as #define's in QOptics.h...
  //  const Int_t kD2O = 0;
  //  const Int_t kAcrylic = 1;
  //  const Int_t kH2O = 2;

  TVector3 norm = n;
  if( norm.Dot(p1) >0 ) {
    norm *= (1.0/n.Mag());
  } else {
    norm *= (-1.0/n.Mag());
  }
  Double_t index1, index2;
  switch( m1 ) {
  case kD2O:
    index1 = GetIndexD2O();
    index2 = GetIndexAcrylic();
    break;
  case kAcrylic:
    index1 = GetIndexAcrylic();
    if( m2 == kD2O ) index2 = GetIndexD2O();
    else index2 = GetIndexH2O();
    break;
  case kH2O:
    index1 = GetIndexH2O();
    index2 = GetIndexAcrylic();
    break;
  }

  Double_t sth1, sth2;
  Double_t cth1 = norm.Dot(p1);
  sth1 = sqrt( 1- cth1*cth1 );

  tir = kFALSE;
  if( sth1>(index2/index1) ) tir = kTRUE;
  sth2 = index1* sth1/index2;
  Double_t cth2 = sqrt( 1-sth2*sth2 );

  Double_t coeff1, coeff2;

  if( !tir ) {
    // do the work, same algorithm as refraction in refraction.for in SNOMAN 3.0192
    coeff1 = index1/index2;
    coeff2 = cth2 - coeff1*cth1;
    p2 = p1*coeff1 + norm*coeff2;
    transpwr = GetTransPwr(index1,index2,acos(cth1));
  }
}
//______________________________________________________________________________
TVector3 QOptics::GetSource()
{
  // Return the current internal source vector.

  return source;
}
//______________________________________________________________________________
void QOptics::SetSource(Double_t sx, Double_t sy, Double_t sz)
{
  // Set the QOptics internal source vector.

  source.SetXYZ(sx,sy,sz);
  rs = source.Mag();
  if (rs < r1) fSourceInside = kTRUE;
  else fSourceInside = kFALSE;
}
//______________________________________________________________________________
void QOptics::SetSource(TVector3 s)
{
  // Set the QOptics internal source vector.

  source = s;
  rs = source.Mag();
  if (rs < r1) fSourceInside = kTRUE;
  else fSourceInside = kFALSE;
}
//______________________________________________________________________________
void QOptics::SetPSUPCentre(Double_t px, Double_t py, Double_t pz)
{
  // Set the QOptics internal PSUP centre offset vector.
  //
  // This vector will be _added_ to all PMT coordinates in
  // QOptics::GetPMTPosition() to simulate an offset of the PSUP coordinate
  // system with respect to the manipulator/AV coordinate system used to
  // position the laserball.

  psup_centre.SetXYZ(px,py,pz);
}
//______________________________________________________________________________
void QOptics::SetPSUPCentre(TVector3 pc)
{
  // Set the QOptics internal PSUP centre offset vector.

  psup_centre = pc;
}
//______________________________________________________________________________
TVector3 QOptics::GetPSUPCentre()
{
  // Return the QOptics internal PSUP centre offset vector.

  return psup_centre;
}
//______________________________________________________________________________
void QOptics::SetPSUPCentreTarget(Double_t px, Double_t py, Double_t pz)
{
  // Set the QOptics internal PSUP centre target offset vector.
  //
  // This vector will be _added_ to all PMT coordinates in
  // QOptics::GetPMTNormal() to simulate an offset of the PSUP target with
  // respect to the standard PSUP coordinate system.

  psup_centre_target.SetXYZ(px,py,pz);
}
//______________________________________________________________________________
void QOptics::SetPSUPCentreTarget(TVector3 pct)
{
  // Set the QOptics internal PSUP centre target offset vector.

  psup_centre_target = pct;
}
//______________________________________________________________________________
TVector3 QOptics::GetPSUPCentreTarget()
{
  // Return the QOptics internal PSUP centre target offset vector.

  return psup_centre_target;
}
//______________________________________________________________________________
void QOptics::SetAVCentre(Double_t px, Double_t py, Double_t pz)
{ 
  // Set the QOptics internal AV offset vector.
  //
  // Because the center of the AV is the coordinate center
  // for all TVector3's in this class, the AV shift with respect
  // to the manipulator system is simulated as the shift of the
  // PSUP and source positions in the same directions.

  SetPSUPCentre(-px,-py,-pz);
  SetPSUPCentreTarget(-px,-py,-pz);
  SetSource(source.X() - px, source.Y() - py, source.Z() - pz);
  av_centre.SetXYZ(px,py,pz);
  
  if(source.Mag() > fAVInnerRadius){
    Warning("SetAVCentre","Source is outside: %8.2f %8.2f %8.2f %8.2f\n",
	   source.X(),source.Y(),source.Z(),source.Mag());
  }
  return;
}
//______________________________________________________________________________
void QOptics::SetAVCentre(TVector3 aShift)
{  
  // Same as previous, but with a TVector3 as argument.

  SetPSUPCentre(-aShift);
  SetPSUPCentreTarget(-aShift);
  SetSource(GetSource() - aShift);
  av_centre = aShift;
  
  if(source.Mag() > fAVInnerRadius){
    Warning("SetAVCentre","Source is outside: %8.2f %8.2f %8.2f %8.2f\n",
	   source.X(),source.Y(),source.Z(),source.Mag());
  }
  return;
}
//______________________________________________________________________________
TVector3 QOptics::GetAVCentre()
{
  // Return the AV centre shift

  return av_centre;
}
//______________________________________________________________________________
Double_t QOptics::GetDd2o()
{
  // Return the d2o distance for last path calculated.

  return fDd2o;
}
//______________________________________________________________________________
Double_t QOptics::GetDacrylic()
{
  // Return the acrylic distance for last path calculated.

  return fDacrylic;
}
//______________________________________________________________________________
Double_t QOptics::GetDh2o()
{
  // Return the h2o distance for last path calculated.

  return fDh2o;
}
//______________________________________________________________________________
Double_t QOptics::GetCtheta()
{
  // Return cos(theta_PMT) for last path calculated.

  return fCtheta;
}
//______________________________________________________________________________
Double_t QOptics::GetCthetaBar()
{
  // Return average cos(theta_PMT) for last pmt solid angle calculated.

  return fCthetaBar;
}
//______________________________________________________________________________
Double_t QOptics::GetTransPwr()
{
  // Return the net transmitted light intensity for the last path calculated
  // in GetDistances()

  return fTransPwr;
}
//______________________________________________________________________________
Double_t QOptics::GetTransPwr(Double_t n,Double_t np,Double_t incident)
{
  // Return the net transmitted light intensity for light incident at angle
  // incident from medium n to medium np.

  Double_t sini   = sin(incident);
  Double_t cosi   = cos(incident);
  Double_t npcos2 = (np*np-n*n*sini*sini); // (np*cos(refracted angle))**2
  Double_t npcos;
  if (npcos2<0) return 0.0; // total internal reflection
  else npcos = sqrt(npcos2);

  //  Double_t rperp = (n*cosi-npcos)/(n*cosi+npcos);  // perpendicular pol.
  //  Double_t rpara = (np*np*cosi-n*npcos)/(np*np*cosi+n*npcos); // parallel pol.

  //  This also works: (included for completeness).  
  //  Double_t refl = 0.5*(rperp*rperp+rpara*rpara);
  //  return (1 - refl);
  
  Double_t prefactor = npcos/(n*cosi);  // for amplitute**2 -> power transf.

  Double_t tperp = 2*n*cosi/(n*cosi+npcos);   // perpendicular polarization
  Double_t tpara = 2*n*np*cosi/(np*np*cosi+n*npcos); // parallel polarization
  Double_t trans = 0.5 * prefactor * (tperp*tperp + tpara*tpara);

  return trans;
}
//______________________________________________________________________________
Double_t QOptics::GetTransPwrPara(Double_t n,Double_t np,Double_t incident)
{
  // Return the transmitted light intensity for light incident at angle incident
  // and polarized parallel to the plane from medium n to medium np.

  Double_t sini   = sin(incident);
  Double_t cosi   = cos(incident);
  Double_t npcos2 = (np*np-n*n*sini*sini); // (np*cos(refracted angle))**2
  Double_t npcos;
  if (npcos2<0) return 0.0; // total internal reflection
  else npcos = sqrt(npcos2);

  Double_t prefactor = npcos/(n*cosi);  // for amplitute**2 -> power transf.

  Double_t tpara = 2*n*np*cosi/(np*np*cosi+n*npcos); // parallel polarization
  Double_t trans = prefactor * tpara*tpara;
  return trans;
}
//______________________________________________________________________________
Double_t QOptics::GetTransPwrPerp(Double_t n,Double_t np,Double_t incident)
{
  // Return the transmitted light intensity for light incident at angle incident
  // and polarized perpendicular to the plane from medium n to medium np.

  Double_t sini   = sin(incident);
  Double_t cosi   = cos(incident);
  Double_t npcos2 = (np*np-n*n*sini*sini); // (np*cos(refracted angle))**2
  Double_t npcos;
  if (npcos2<0) return 0.0; // total internal reflection
  else npcos = sqrt(npcos2);

  Double_t prefactor = npcos/(n*cosi);  // for amplitute**2 -> power transf.

  Double_t tperp = 2*n*cosi/(n*cosi+npcos);   // perpendicular polarization
  Double_t trans = prefactor * tperp*tperp;
  return trans;
}
//______________________________________________________________________________
Double_t QOptics::GetPMTAngResp()
{
  return fPMTAngResp;
}
//______________________________________________________________________________
Double_t QOptics::GetPMTAngResp(Double_t costheta)
{
  // Calculate by interpolation in a table the relative angular response
  // at an incident angle corresponding to costheta.
  // Error messages added: CJJ, Feb, 2000.

  Int_t bin = (Int_t) ((1.0-costheta)*100.0);
  if (bin>=100) {
    bin=99;
    fprintf(stderr,"Warning: Out-of-range cosine<0 in QOptics::GetPMTAngResp()");
  }
  else if (bin<0) {
    bin=0;
    fprintf(stderr,"Warning: Out-of-range cosine>1 in QOptics::GetPMTAngResp()");
  }
  // No interpolation, same as in R. Ford's routine in SNOMAN: oca_extract.f.
  // This is reasonable given the bin spacing and the errors associated with
  // the cos(theta) calculation (probably about 5% ?) and the response error
  // (about 3%, from M. Lay thesis, Oxford).

  return fPMTAngRespCos[bin];
}
//______________________________________________________________________________
void QOptics::SetWavelength(Double_t lambda)
{
  // This function is a legacy function -- redundant with SetIndices()
  // since the indices of refraction always need to be recalculated when
  // specifying the wavelength.

  SetIndices(lambda);  // Must also set indices for new wavelength!
}
//______________________________________________________________________________
Double_t QOptics::GetWavelength()
{
  return fWavelength;
}
//______________________________________________________________________________
void QOptics::SetIndices(Double_t lambda)
{
  // Set the indices of refraction for the specified wavelength.
  // If no wavelength is specified, the currently stored value is used.

  if (lambda==0) lambda = fWavelength;
  else fWavelength = fabs(lambda);     // Negative for default wavelength in QPath!

  nd = GetIndexD2O(lambda);
  na = GetIndexAcrylic(lambda);
  nh = GetIndexH2O(lambda);

  printf("wl %g nd %g na %g nh %g\n",fWavelength,nd,na,nh);
}
//______________________________________________________________________________
Double_t QOptics::GetIndexD2O(Double_t lambda,Bool_t derivative)
{
  // Return index of refraction of heavy water; lambda in nanometers (!)
  // Function comes from SNOMAN 3_0187 (September 1999)

  if (lambda==0) lambda = fWavelength;
  else lambda = fabs(lambda);
  if (derivative)
    return 0.01333*(-0.32*eVnmfactor/(lambda*lambda))*exp(0.32*eVnmfactor/lambda);
  else
    return 1.302 + 0.01333*exp(0.32*eVnmfactor/lambda);
}
//______________________________________________________________________________
Double_t QOptics::GetIndexAcrylic(Double_t lambda,Bool_t derivative)
{
  // Return index of refraction of acrylic; lambda in nanometers (!)
  // Function comes from SNOMAN 3_0187 (September 1999)

  if (lambda==0) lambda = fWavelength;
  else lambda = fabs(lambda);
  if (derivative)
    return 0.02*(-0.32*eVnmfactor/(lambda*lambda))*exp(0.32*eVnmfactor/lambda);
  else
    return 1.452 + 0.02*exp(0.32*eVnmfactor/lambda);
}
//______________________________________________________________________________
Double_t QOptics::GetIndexH2O(Double_t lambda,Bool_t derivative)
{
  // Return index of refraction of light water; lambda in nanometers (!)
  // Function comes from SNOMAN 3_0187 (September 1999)

  if (lambda==0) lambda = fWavelength;
  else lambda = fabs(lambda);
  if (derivative)
    return 0.01562*(-0.32*eVnmfactor/(lambda*lambda))*exp(0.32*eVnmfactor/lambda);
  else
    return 1.302 + 0.01562*exp(0.32*eVnmfactor/lambda);
}
//______________________________________________________________________________
Double_t QOptics::GetRayleighD2O(Double_t lambda)
{
  // Calculate the Rayeleigh scattering length at the temperature of SNO,
  // approximately 11 degrees Celsius.
  // Uses the Einstein-Smoluchowski formula and parameter values as determined
  // by Martin Pickel in literature search during summer 2000.
  //
  // lambda in nm (eg. 500 not 500e-9!)

  if (lambda==0) lambda = fWavelength;
  else lambda = fabs(lambda);

  // Index of refraction:
  // SNOMAN at 25 degC is 1.302; difference is +0.001 to 8 degC for SNO:
  Double_t n = 1.303 + 0.0013333*exp(0.32*197.327*2*M_PI/lambda);

  Double_t kT = 1.38066 * 284.15 * 1.0e13; // For 11 degC, and SI units for lambda in nm
  Double_t betat = 5.041e-10;  // Isothermal compressibility (1/Pa)
  Double_t rho = 0.079;  // depolarization ratio 
  Double_t bprime = 0.935;  // Unknown wavelength dependence...
  Double_t dndp = (n - 1) * betat * bprime;
  Double_t alpha = 32 * pow(M_PI,3) * kT /
    ( 3 * pow(lambda,4) * betat ) *
    n*n * dndp*dndp *
    (6+3*rho)/(6-7*rho);  // Cabannes factor
  return alpha / 100.0;  // convert from 1/m to 1/cm for consistent cm units;
}
//______________________________________________________________________________
Double_t QOptics::GetRayleighH2O(Double_t lambda)
{
  // Calculate the Rayeleigh scattering length at the temperature of SNO,
  // approximately 11 degrees Celsius.
  // Uses the Einstein-Smoluchowski formula and parameter values as determined
  // by Martin Pickel in literature search during summer 2000.
  //
  // lambda in nm (eg. 500 not 500e-9!)

  if (lambda==0) lambda = fWavelength;
  else lambda = fabs(lambda);

  // Index of refraction:
  // SNOMAN at 25 degC is 1.302; difference is +0.001 to 8 degC for SNO:
  Double_t n = 1.303 + 0.001562*exp(0.32*197.327*2*M_PI/lambda);

  Double_t kT = 1.38066 * 284.15 * 1.0e13; // For 11 degC, and SI units for lambda in nm
  Double_t betat = 4.832e-10;  // Isothermal compressibility (1/Pa)
  Double_t rho = 0.051;  // depolarization ratio 
  Double_t bprime = 0.95771 + 2.75664e-5*lambda;
  Double_t dndp = (n - 1) * betat * bprime;
  Double_t alpha = 32 * pow(M_PI,3) * kT /
    ( 3 * pow(lambda,4) * betat ) *
    n*n * dndp*dndp *
    (6+3*rho)/(6-7*rho);  // Cabannes factor
  return alpha / 100.0;  // convert from 1/m to 1/cm for consistent cm units
}
//______________________________________________________________________________
Double_t QOptics::GetRayleighAcrylic(Double_t lambda)
{
  // Calculate the Rayeleigh scattering length at the temperature of SNO,
  // approximately 11 degrees Celsius.
  // Uses the SNOMAN function, scaled from 25degC to 11 degC; is this correct?
  //
  // lambda in nm (eg. 500 not 500e-9!)

  if (lambda==0) lambda = fWavelength;
  else lambda = fabs(lambda);

  Double_t betat = 3.55e-10;
  Double_t confac = 1.53e26 * 284.15/298.15;  // Change SNOMAN confac for 11 degC
  Double_t scale_fac = 1.0;
  Double_t energy = 1.239842e-3 / lambda;  // energy in MeV = hbar*c / lambda
  Double_t n = 1.452 + 0.02*exp(0.32*197.327*2*M_PI/lambda);

  Double_t disral = 100.0 * betat * confac * scale_fac * pow(energy,4) *
    pow(n*n-1,2) * pow(n*n+2,2);

  return disral / 100.0;  // convert from 1/m to 1/cm for consistent cm units
}
//______________________________________________________________________________
Double_t QOptics::VgroupD2O(Double_t aLambda)
{
  // Legacy function.

  return GetVgroupD2O(aLambda);
}
//______________________________________________________________________________
Double_t QOptics::GetVgroupD2O(Double_t aLambda)
{
  // Return the group velocity of light in D2O (cm/ns)
  // Depends completely on numerical results from GetIndexD2O().

  Double_t wavelength;
  if (aLambda == 0) wavelength = fWavelength;
  else wavelength = fabs(aLambda);
  
  Double_t derivative = GetIndexD2O(wavelength,kTRUE);
  return 29.979246 / (GetIndexD2O(wavelength) - wavelength*derivative);
}
//______________________________________________________________________________
Double_t QOptics::VgroupH2O(Double_t aLambda)
{
  // Legacy function.

  return GetVgroupH2O(aLambda);
}
//______________________________________________________________________________
Double_t QOptics::GetVgroupH2O(Double_t aLambda)
{
  // Return the group velocity of light in H2O (cm/ns)
  // Depends completely on numerical results from GetIndexH2O().
  
  Double_t wavelength;
  if (aLambda == 0) wavelength = fWavelength;
  else wavelength = fabs(aLambda);

  Double_t derivative = GetIndexH2O(wavelength,kTRUE);
  return 29.979246 / (GetIndexH2O(wavelength) - wavelength*derivative);
}
//______________________________________________________________________________
Double_t QOptics::VgroupAcrylic(Double_t aLambda)
{
  // Legacy function.

  return GetVgroupAcrylic(aLambda);
}
//______________________________________________________________________________
Double_t QOptics::GetVgroupAcrylic(Double_t aLambda)
{
  // Return the group velocity of light in Acrylic (cm/ns)
  // Depends completely on numerical results from GetIndexAcrylic().

  Double_t wavelength;
  if (aLambda == 0) wavelength = fWavelength;
  else wavelength = fabs(aLambda);

  Double_t derivative = GetIndexAcrylic(wavelength,kTRUE);
  return 29.979246 / (GetIndexAcrylic(wavelength) - wavelength*derivative);
}
//______________________________________________________________________________
Double_t  QOptics::LambdaToOmega(Double_t aLambda)
{
  // Convert from aLambda in nm to Omega in rad/sec
  // Conversion factor is 2*pi*c (with c = 29.97e16 nm/s) = 1.8883e18 nm/s

  Double_t wavelength;
  if( aLambda==0 ) wavelength = fWavelength;
  else wavelength = fabs(aLambda);

  return 1.888307E18/wavelength;
}
//______________________________________________________________________________
Double_t  QOptics::OmegaToLambda(Double_t aOmega)
{
  // Convert from aOmega in rad/sec to Lambda in nm
  // Conversion factor is 2*pi*c (with c = 29.97e16 nm/s) = 1.8883e18 nm/s

  if (aOmega!=0) return 1.888307E18/aOmega;
  else return -1;
}
//______________________________________________________________________________
Double_t  QOptics::LambdaToEnergy(Double_t aLambda)
{
  // Convert from aLambda in nm to Energy in eV
  // Conversion factor is h*c = hbar*c*2*pi = 197.327053 eV*nm * 2*pi = 1239.6 nm
  //
  // 07.2006 - O.Simard
  // Energy conversion updated to match SNOMAN (PDG) value of 1239.841.

  Double_t wavelength;
  if (aLambda==0) wavelength = fWavelength;
  else wavelength = fabs(aLambda);

  // return 1239.6/wavelength;
  return 1239.841/wavelength;
}
//______________________________________________________________________________
Double_t QOptics::GetNcdReflectivity(Double_t aLambda)
{
  // Return the Ncd nickel surface reflectivity in percent for 
  // the given wavelength (nm). The function is modeled as a second
  // order polynomial curve, each parameters coming from ex-situ 
  // measurements. Parameters values are the same as SNOMAN by default,
  // and should be updated if necessary.

  if(aLambda==0) aLambda = fWavelength;
  
  if((aLambda < 300) || (aLambda > 700)){
    Warning("GetNcdReflectivity","Ncd reflectivity not defined at %.2f nm.",aLambda);
    return 0;
  }

  // Number taken from NCD Unidoc or SNOMAN
  Double_t p0 = -0.01387;
  Double_t p1 = 4.5357e-04;
  Double_t p2 = -2.3154e-07;

  Double_t reflectivity = p0 + p1*aLambda + p2*TMath::Power(aLambda,2);
  
  return reflectivity;
}
//________________________________________________________________________________
// Utility Routines for refraction between heavy water/acrylic/light water
//
// For sources inside the AV they are (for fSourceInside = kTRUE)
//   - theta1() and d1dt()
//   - theta2() and d2dt()
//   - theta3() and d3dt()
//   - thResidual() and dthResidualdth()
//
// For sources outside the AV they are:
//   - 1 through 3 from above, with the use of fSourceInside = kFALSE
//   - theta4() and d4dt()
//   - theta5() and d5dt()
//   - thResidual() and dthResidualdth() from above, with fSourceInside = kFALSE
//________________________________________________________________________________
Double_t QOptics::theta1(const Double_t &thet)
{
  // For source inside the AV:
  //* theta1 - angle between source radial vector and heavy water/acrylic
  //*          interface point, from the origin
  //* rs - source radius
  //* r1 - inner acrylic vessel radius
  //* theta - angle between source radial vector and heavy water/acrylic
  //*         interface point to be reached, from the source point
  
  Double_t costheta1;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);
  if (fSourceInside)
    costheta1 = rs/r1*sint*sint + cost*sqrt(1-pow(rs/r1*sint,2));
  else
    costheta1 = rs/r2*sint*sint - cost*sqrt(1-pow(rs/r2*sint,2));
  if(costheta1>1.0){
    if(costheta1>1.0000001)printf("Error, costheta1 %f\n",costheta1);
    costheta1=1.0;
  }else if(costheta1<-1.0){
    if(costheta1< -1.0000001)printf("Error, costheta1 %f\n",costheta1);
    costheta1=-1.0;
  }

  return acos(costheta1);
}

//______________________________________________________________________________
Double_t QOptics::d1dt(const Double_t &thet)
{
  // For source inside the AV:
  //* calculates the derivative of theta1 with respect to theta
  //* rs - source radius
  //* r1 - inner acrylic vessel radius
  //* theta - angle between source radial vector and heavy water/acrylic
  //*         interface point to be reached, from the source point

  Double_t costheta1,d1d;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside) {
    costheta1 = rs/r1*sint*sint + cost*sqrt(1-pow(rs/r1*sint,2));
    d1d = -1/sqrt(1-costheta1*costheta1)
      *( rs/r1*2*sint*cost
	 -sint*sqrt(1-pow(rs/r1*sint,2))
	 -pow((rs/r1*cost),2)*sint/sqrt(1-pow((rs/r1*sint),2)));
  } else {
    costheta1 = rs/r2*sint*sint - cost*sqrt(1-pow(rs/r2*sint,2));
    d1d = -1/sqrt(1-costheta1*costheta1)
      *( rs/r2*2*sint*cost
	 +sint*sqrt(1-pow(rs/r2*sint,2))
	 +pow(rs/r2*cost,2)*sint/sqrt(1-pow(rs/r2*sint,2)));
  }

  return d1d;
}      
//______________________________________________________________________________
Double_t QOptics::theta2(const Double_t &thet)
{
  // For source inside the AV:
  //* theta2 - angle between radial vectors from origin to heavy water/acrylic
  //*          and acrylic/light water interface points
  //* rs - source radius/
  //* r1 - inner acrylic vessel radius
  //* r2 - outer acrylic vessel radius
  //* theta - angle between source radial vector and heavy water/acrylic
  //*         interface point to be reached (this is the same theta as
  //*         for theta1() above! - comes from taking Snell's law of refraction
  //*         into account directly in the formulas)
  //* nd - heavy water index of refraction
  //* na - acrylic index of refraction
  
  Double_t costheta2;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside) 
    costheta2 = pow((nd/na*rs*sint),2)/(r1*r2)
      + sqrt(1-pow((nd/na*rs/r1*sint),2))
      * sqrt(1-pow((nd/na*rs/r2*sint),2));
  else
    costheta2 = pow(nh/na*rs*sint,2)/(r1*r2)
      + sqrt(1-pow(nh/na*rs/r1*sint,2))
      * sqrt(1-pow(nh/na*rs/r2*sint,2));
  if(costheta2>1.0){
    if(costheta2>1.0000001)printf("Error, costheta2 %f\n",costheta2);
    costheta2=1.0;
  }else if(costheta2<-1.0){
    if(costheta2< -1.0000001)printf("Error, costheta2 %f\n",costheta2);
    costheta2=-1.0;
  }
  return acos(costheta2);
}
//______________________________________________________________________________
Double_t QOptics::d2dt(const Double_t &thet)
{
  // For source inside the AV:
  //* calculates the derivative of theta2 with respect to theta
  //* rs - source radius
  //* r1 - inner acrylic vessel radius
  //* r2 - outer acrylic vessel radius
  //* theta - angle between source radial vector and heavy water/acrylic
  //*         interface point to be reached (this is the same theta as
  //*         for theta1() above! - comes from taking Snell's law of refraction
  //*         into account directly in the formulas)
  //* nd - heavy water index of refraction
  //* na - acrylic index of refraction

  Double_t costheta2,d2d;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside) {
    costheta2 = pow(nd/na*rs*sint,2)/(r1*r2)
      + sqrt(1-pow(nd/na*rs/r1*sint,2))
      * sqrt(1-pow(nd/na*rs/r2*sint,2));
    if (1-costheta2 > 1e-6) {
      d2d = -1/sqrt(1-costheta2*costheta2)
	*( pow(nd/na*rs,2)*2*sint*cost/(r1*r2)
	   -pow(nd/na*rs,2)*sint*cost*(sqrt(1-pow(nd/na*rs/r2*sint,2))/
				       (r1*r1*sqrt(1-pow(nd/na*rs/r1*sint,2)))
				       +sqrt(1-pow(nd/na*rs/r1*sint,2))/
				       (r2*r2*sqrt(1-pow(nd/na*rs/r2*sint,2)))));
    } else d2d= 0.0;
  } else {
    costheta2 = pow(nh/na*rs*sint,2)/(r1*r2)
      + sqrt(1-pow(nh/na*rs/r1*sint,2))
      * sqrt(1-pow(nh/na*rs/r2*sint,2));
    if (1-costheta2 > 1e-6) {
      d2d = -1/sqrt(1-costheta2*costheta2)
	*( pow(nh/na*rs,2)*2*sint*cost/(r1*r2)
	   -pow(nh/na*rs,2)*sint*cost*(sqrt(1-pow(nh/na*rs/r2*sint,2))/
				       (r1*r1*sqrt(1-pow(nh/na*rs/r1*sint,2)))
				       +sqrt(1-pow(nh/na*rs/r1*sint,2))/
				       (r2*r2*sqrt(1-pow(nh/na*rs/r2*sint,2)))));
    } else d2d = 0.0;
  }
  return d2d;
}
//______________________________________________________________________________
Double_t QOptics::theta3(const Double_t &thet)
{
  // For source inside AV:
  //* theta3 - angle between radial vectors from origin to acrylic/light water
  //*          interface and pmt position
  //* rs - source radius/
  //* r2 - outer acrylic vessel radius
  //* rp - PSUP radius (ie. where the tube is - a bit of a fuzzy (+/- 10cm) concept)
  //* theta - angle between source radial vector and heavy water/acrylic
  //*         interface point to be reached (this is the same theta as
  //*         for theta1() above! - comes from taking Snell's law of refraction
  //*         into account directly in the formulas)
  //* nd - heavy water index of refraction
  //* nh - light water index of refraction
  //
  // For source outside AV:
  // theta3 - angle between radial vectors from origin to acrylic/heavy water
  //          interfaces
  // r1 - inner acrylic vessel radius
  
  Double_t costheta3;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside)
    costheta3 = pow(nd/nh*rs*sint,2)/(r2*rp)
      + sqrt(1-pow(nd/nh*rs/r2*sint,2))
      * sqrt(1-pow(nd/nh*rs/rp*sint,2));
  else
    costheta3 = 2*pow(nh/nd*rs/r1*sint,2) - 1;
  if(costheta3>1.0){
    if(costheta3>1.0000001)printf("Error, costheta3 %f\n",costheta3);
    costheta3=1.0;
  }else if(costheta3<-1.0){
    if(costheta3< -1.0000001)printf("Error, costheta3 %f\n",costheta3);
    costheta3=-1.0;
  }
  return acos(costheta3);
}
//______________________________________________________________________________
Double_t QOptics::d3dt(const Double_t &thet)
{
  // For source inside the AV:
  //* calculates the derivative of theta3 with respect to theta
  //* rs - source radius
  //* r2 - outer acrylic vessel radius
  //* rp - PSUP radius (ie. where the tube is - a bit of a fuzzy (+/- 10cm) concept)
  //* theta - angle between source radial vector and heavy water/acrylic
  //*         interface point to be reached (this is the same theta as
  //*         for theta1() above! - comes from taking Snell's law of refraction
  //*         into account directly in the formulas)
  //* nd - heavy water index of refraction
  //* nh - light water index of refraction

  Double_t costheta3,d3d;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside) {
    costheta3 = pow(nd/nh*rs*sint,2)/(r2*rp)
      + sqrt(1-pow(nd/nh*rs/r2*sint,2))
      * sqrt(1-pow(nd/nh*rs/rp*sint,2));
    if (1-costheta3 > 1e-6)
      d3d = -1/sqrt(1-costheta3*costheta3)
	*( pow(nd/nh*rs,2)*2*sint*cost/(r2*rp)
	   -pow(nd/nh*rs,2)*sint*cost*(sqrt(1-pow(nd/nh*rs/rp*sint,2))/
				       (r2*r2*sqrt(1-pow(nd/nh*rs/r2*sint,2)))
				       +sqrt(1-pow(nd/nh*rs/r2*sint,2))/
				       (rp*rp*sqrt(1-pow(nd/nh*rs/rp*sint,2)))));
    else d3d = 0.;
  } else {
    costheta3 = 2*pow(nh/nd*rs/r1*sint,2) - 1;
    if (1-costheta3 > 1e-6)
      d3d = -1/sqrt(1-costheta3*costheta3) * (pow(nh/nd*rs/r1,2)*4*sint*cost);
    else d3d = 0.;
  }
  return d3d;
}
//______________________________________________________________________________
Double_t QOptics::theta4(const Double_t &thet)
{
  // For source outside the AV only:
  // theta4 - angle between radial vectors from centre to inner and outer AV
  //          radii for second refraction; identical to theta2

  if (fSourceInside)
    return 0;
  else
    return theta2(thet);
}
//______________________________________________________________________________
Double_t QOptics::d4dt(const Double_t &thet)
{
  // For source outside the AV only:
  // Calculates the derivative of theta4 with respect to theta.
  // Identical with d2dt.

  if (fSourceInside)
    return 0;
  else
    return d2dt(thet);
}
//______________________________________________________________________________
Double_t QOptics::theta5(const Double_t &thet)
{
  // For source outside the AV only:
  // theta5 - angle between the radial vectors from origin to acrylic/light water
  //          interface and pmt position

  Double_t costheta5;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside)
    return 0;
  else
    costheta5 = pow(rs*sint,2)/(r2*rp)
      + sqrt(1-pow(rs/r2*sint,2))*sqrt(1-pow(rs/rp*sint,2));
  if(costheta5>1.0){
    if(costheta5>1.0000001)printf("Error, costheta5 %f\n",costheta5);
    costheta5=1.0;
  }else if(costheta5<-1.0){
    if(costheta5< -1.0000001)printf("Error, costheta5 %f\n",costheta5);
    costheta5=-1.0;
  }
  return acos(costheta5);
}
//______________________________________________________________________________
Double_t QOptics::d5dt(const Double_t &thet)
{
  // For source outside the AV only:
  // Calculates the derivative of theta5 with respect to theta.

  Double_t costheta5,d5d;
  Double_t sint,cost;
  sint = sin(thet);
  cost = cos(thet);

  if (fSourceInside) {
    return 0;
  } else {
    costheta5 =  pow(rs*sint,2)/(r2*rp)
      + sqrt(1-pow(rs/r2*sint,2))*sqrt(1-pow(rs/rp*sint,2));
    if (1-costheta5 > 1e-6)
      d5d = -1/sqrt(1-costheta5*costheta5)
	*( rs*rs/(r2*rp)*2*sint*cost
	   -rs*rs*sint*cost*(sqrt(1-pow(rs/rp*sint,2))/
			     (r2*r2*sqrt(1-pow(rs/r2*sint,2)))
			     +sqrt(1-pow(rs/r2*sint,2))/
			     (rp*rp*sqrt(1-pow(rs/rp*sint,2)))));
    else d5d = 0.;
  }
  return d5d;
}
//______________________________________________________________________________
Double_t QOptics::thResidual(const Double_t &thet)
{
  //* compute the difference between the PMT's thtarget and theta3(theta) for the
  //* current estimate of starting angle from the source theta
  //
  // thPMTtarget is the angle between the source vector and the PMT vector, with
  // the centre of the acrylic vessel as origin
  Double_t retvalue;   
  if (fSourceInside) {
    retvalue = thPMTtarget - (theta3(thet)+theta2(thet)+theta1(thet));
  }else
    retvalue= thPMTtarget -
      (theta5(thet)+theta4(thet)+theta3(thet)+theta2(thet)+theta1(thet));
  return retvalue;
}
//______________________________________________________________________________
Double_t QOptics::dthResidualdth(const Double_t &thet)
{
  //* compute the derivative in the difference between ... (see above)
  
  if (fSourceInside)
    return - (d3dt(thet)+d2dt(thet)+d1dt(thet));
  else
    return - (d5dt(thet)+d4dt(thet)+d3dt(thet)+d2dt(thet)+d1dt(thet));
}
//______________________________________________________________________________
//* The following routines are from Numerical recipes and are used to minimize
//* a function with calculable derivative (Newton-Raphson using the derivative
//* with bisection thrown to make it more stable).
//* See Numerical Recipes (Ch 9-4 in the first edition FORTRAN book).
//* Available online at http://www.nr.com/
//______________________________________________________________________________
void QOptics::funcd(Double_t thet,Double_t *f,Double_t *df)
{
  // Calculates the residual and derivative in the residual for rtsafe.
  //
  // Looks for the flag fSourceInside for direction which sets of angles to use:
  //   kTRUE : inside the AV, there are three angles
  //   kFALSE: outside the AV, there are five angles
  // Taken care of inside utility routines thResidual() and dthResidualdth()

  // rs=source.Mag();  // Pre-calculated and stored by routine calling rtsafe!
  // thPMTtarget = atan2((p.Cross(source)).Mag(),p*source); // Pre-calculated
  *f = thResidual(thet);
  *df = dthResidualdth(thet);
}
//______________________________________________________________________________
#define RTSAFE_MAXIT 100
//______________________________________________________________________________
Double_t QOptics::rtsafe(Double_t x1, Double_t x2, Double_t xacc)
{
  //* Routine from Numerical Recipes in FORTRAN, 2 ed.
  //* Press et al., 1992
  //*
  //  Basic modifications:
  //   - rtsafe knows about funcd, since it's explicitly part of this class
  //   - rtsafe reports errors by returning -10000 instead of calling
  //      nrerror() - to be replaced in the future ... ?
  //
  //* Input:
  //*   x1 - lower limit on x
  //*   x2 - upper limit on x
  //*   xacc - accuracy tolerated on x determined by this routine

  int j;
  Double_t df,dx,dxold,f,fh,fl;
  Double_t temp,xh,xl,rts;
  
  funcd(x1,&fl,&df);
  funcd(x2,&fh,&df);
  if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0)) {
    printf("Root must be bracketed in QOptics::rtsafe\n");
    return -10000;
  }
  if (fl == 0.0) return x1;
  if (fh == 0.0) return x2;
  if (fl < 0.0) {
    xl=x1;
    xh=x2;
  } else {
    xh=x1;
    xl=x2;
  }
  rts=0.5*(x1+x2);
  dxold=fabs(x2-x1);
  dx=dxold;
  funcd(rts,&f,&df);
  for (j=1;j<=RTSAFE_MAXIT;j++) {
    if ((((rts-xh)*df-f)*((rts-xl)*df-f) >= 0.0)
	|| (fabs(2.0*f) > fabs(dxold*df))) {
      dxold=dx;
      dx=0.5*(xh-xl);
      rts=xl+dx;
      if (xl == rts) return rts;
    } else {
      dxold=dx;
      dx=f/df;
      temp=rts;
      rts -= dx;
      if (temp == rts) return rts;
    }
    if (fabs(dx) < xacc) return rts;
    funcd(rts,&f,&df);
    if (f < 0.0)
      xl=rts;
    else
      xh=rts;
  }
  printf("Maximum number of iterations exceeded in QOptics::rtsafe\n");
  return -rts;
}
//______________________________________________________________________________

Bool_t QOptics::IsAnchor(){
  // Returns the value of IsAnchor() for the latest path calculated
  return fAnchor;
}

//______________________________________________________________________________

Double_t QOptics::GetClosestAnchor(TVector3 &s, TVector3 &d, Int_t &index, TVector3 &closest){
  // This function goes through the list of anchors and checks which one is the closest
  // to the light ray, the distance from the light ray to its center and the point in the light ray
  // that stays closest to the anchor

  // First let's get the normalised vector of the direction
  TVector3 normdir(d.Unit());
  
  // Start the index to -1 for control
  index = -1;

  // Set the distance to something anormaly big
  Double_t distance, min_distance = 9999.0; 
  Double_t tmpdist;
  Int_t tmpindex;
  
  // Set closest point to somewhere outside the AV
  closest.SetXYZ(999.,999.,999.);

  for (Int_t i = 0; i < fNumAnchors; i++){
    TVector3 ps(fAnchorsPos[i] - s);   
    
    // minimum distance from point to line
    distance = normdir.Cross(ps).Mag()/normdir.Mag();
    
    if (distance < min_distance){
      tmpdist = min_distance;
      min_distance = distance;
      tmpindex = index;
      index = i;
      
      // distance from source to closest point
      Double_t length = normdir.Dot(ps);
      

      // if the closest point is in the opposite direction
      // relative to the ray direction then the closest point is the source
      if (length < 0){
	length = 0.;
	min_distance = tmpdist;
	index = tmpindex;
      }
      else{
	TVector3 pClosest(s+length*normdir.Unit());
	closest = pClosest;
      }
    }
  }
  return min_distance;
}

//______________________________________________________________________________
Bool_t QOptics::IsAnchor(TVector3 &s, TVector3 &d) {
  // Returns kTRUE if the light ray passes near an NCD anchor 
  // The tolerance is calculated according to the source and direction of the ray
  
  // First we must get the closest anchor, 
  // the distance to it 
  // and the point in the light ray that stays closest to the anchor
  
  // The anchors positions must be already corrected from 
  // the AV reference to PSUP reference

  // s stands for source
  // d for direction
  
  Int_t index = -1;
  // let's hardcode the tolerance to 5 cm
  fAnchorTolerance = 5.;
  
  // Closest point in the light ray relative to the anchor
  TVector3 closest;

  // get the index of the closest anchor as well as the distance to it
  Double_t fAnchorMinDist = GetClosestAnchor(s,d,index,closest);
  
  if (index < 0) {
    // Something went wrong. 
    fAnchorMinDist = 999.;
  }
  else{
    if (fAnchorMinDist - fAnchorRadius < 0) {
      // if the ray passes *through* an anchor
      fAnchor = kTRUE;
    }
    else {
      // Let's make the full calculation of interaction distance
      // we will assume 5cm as threshold from the interaction point
      
      // we already have the closest point to the center of the anchor
      
      // the distance from the source to the closest point in the light ray
      Double_t x1 = (closest - s).Mag();
      // total distance travelled
      Double_t x2 = fDd2o + fDacrylic + fDh2o;
      // This is the radius of the "solid cone" defined by the laserball
      // and the PMT
      // part between parentesis --> normalisation
      // fCtheta*fPMTReflectorRadius : Projection of the reflector perpendicular to
      //                               incident light ray
      // fCtheta*fPMTReflectorRadius - fLaserballRadius:  Side of a triangle formed by the 
      //                                                  light ray source, PMT center and PMT reflector radius
      // first term: scale factor to a triangle with side x1
      // the sum of LB radius increases the triangle side by the uncertainty in the LB
      Double_t y1 = (fCtheta*fPMTReflectorRadius - fLaserballRadius)*x1/x2 + fLaserballRadius;
      fAnchorMinDist -= fAnchorRadius;	//subtract the NCD anchor  radius from distance
      fAnchorMinDist -= y1;             //subtract light cone radius from distance
      // scale the result again to the light cone until the PMT
      fAnchorMinDist /= (1 - x1/x2);    // correct it to the source position
    }
  }
  
  if(fAnchorMinDist < fAnchorTolerance) 
    fAnchor = kTRUE;
  else fAnchor = kFALSE;
  
  
  return fAnchor;
  
}
//______________________________________________________________________________

// Returns the result of the last calculation of the interaction with the NCD umbilicals

Bool_t QOptics::IsNcdRope( ){
  return fNcdRope;
}


//______________________________________________________________________________

Bool_t QOptics::IsNcdRope(TVector3 &s, TVector3 &d){
  // Routine to determine if the light ray coming from source 's' with direction 'p' (pulling out from source)
  // Crosses any NCD umbilical

  // The umbilicals are roughly estimated as straight lines from the tip of the NCD
  // to the closest point in the neck

  // If the ray passes closest than a defined threshold it will be flagged

  // THis calculation assumes that the neck is perfectly aligned with the Z axis
  // which is correct with the whole QOptics implementation

  // let's consider a static threshold of 5cm
  Float_t threshold = 5.0;
  
  // Neck radius
  Float_t neckIR = 73.0;

  // Lowest point in neck
  // The bottom of the neck is located 10.2cm below the junction of the chimney and the AV
  Float_t neckZlow = 588.0;

  // Initialize to false
  fNcdRope = kFALSE;

  QNCD *ncd = 0; 
  
  // let's go through all NCDs
  // we have 40 NCDs
  Int_t nCounters = fNcdArray->GetNCounters();

  Float_t lowestNcdTop = 999.0;

  for (Int_t i = 0; i < nCounters; i++){
    // Get the counter
    ncd = fNcdArray->GetCounter(i);
    
    // Get the top position of the counter
    TVector3 ncdpos = ncd->GetTop();
    
    // If source is below NCD top and direction points downwards
    // it won't hit a NCD Rope...proceed to next counter
    if ((ncdpos.Z() > s.Z() ) && (d.Z() < 0.0))  continue;

    // Set the neck lowest position with the same azimuthal angle from the center
    Float_t x = neckIR * cos(ncdpos.Phi());
    Float_t y = neckIR * sin(ncdpos.Phi());
    
    TVector3 neckpos(x,y,neckZlow);
     
    // Now that we have two points we can consider the line 
    // the connects them and calculate the shortest distance 
    // to the light ray
    
    // s + fDd2O*d.Unit() results in the interaction point of the light ray with the AV
    TVector3 p2 = s+fDd2o*d.Unit();
    
    Float_t dist = distSegments(s, p2, ncdpos, neckpos);
    
    if (dist < threshold){
      fNcdRope = kTRUE;
      return kTRUE;
    }
  }

  // The ray isn't close enough to any NCD rope;
  return kFALSE;
}

//    Input:  two line segments P (p1+p.p2) and S (s1+s.s2)  
//    Return: the shortest distance between P and S
//    p1, p2, s1 & s2 are the limits of the segments P and S, respectively
Float_t QOptics::distSegments( TVector3 &p1, TVector3 &p2, TVector3 &s1, TVector3 &s2)
{

  /**
   * Equations for each segment
   * P = p1 + r.(p2-p1)
   * S = s1 + t.(s2-s1)
   *
   * Square distance between two points in these segments
   * Q(r,t) = a*r^2 + 2*b*r*t + c*t^2 + 2*d*s + 2*e*t + f
   *
   * Definitions:
   *
   * u = p2-p1
   * v = s2-s1
   * w = p1-s1
   *
   * a = u.u
   * b = u.v
   * c = v.v
   * d = u.w
   * e = v.w
   * f = w.w
   *
   */

  // Vector along P
  TVector3   u = p2 - p1;
  // Vector along S
  TVector3   v = s2 - s1;
  // Vector connecting P and S
  TVector3   w = p1 - s1;


  // nbarros algorithm

  Float_t    a = u * u;        // always >= 0
  Float_t    b = u * v;
  Float_t    c = v * v;        // always >= 0
  Float_t    d = u * w;
  Float_t    e = v * w;
  //  Float_t    f = w * w;

  // Let's determine if the segments are paralel
  Float_t    D = a*c - b*b;       // always >= 0
  Float_t distance = 0.0;

  Float_t t, tc;
  Float_t s, sc;

  if (D == 0 ){
    // The lines are paralel
    // we just need to get the distance between them
    
    distance = w * (u.Cross(v)).Unit();
    
  }
  else{
    tc = (b*e - d*c) / D ;
    sc = (a*e - b*d) / D ;
 
    // Now let's check all cases
    
    if (tc < 0){
      t = 0;
    }
    else {
      if (tc > 1){
	t = 1;
      }
      else{
	t = tc;
      }
    }
    
    if (sc < 0){
      s = 0;
    }
    else {
      if (sc > 1){
	s = 1;
      }
      else{
	s = sc;
      }
    }
    
    // We have the minimum
    // Find the corresponding points in each segment

    TVector3 P = p1 + t * (p2-p1);
    TVector3 S = s1 + s * (s2-s1);
    
    distance = (P-S).Mag();
  }


  return distance;

}

#undef RTSAFE_MAXIT







