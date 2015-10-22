//*-- Authors :    Bryce Moffat (25/04/00) - Olivier Simard (05.2006)

/*************************************************************************
 * Copyright(c) 1999, The SNO Software Project, All rights reserved.     *
 * Authors: Bryce Moffat, Olivier Simard                                 *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include "QLaserballUtil.h"
#include "TMath.h"

//______________________________________________________________________________
ClassImp(QLaserballUtil)
;

//______________________________________________________________________________
QLaserballUtil::QLaserballUtil()
{
  // QLaserballUtil constructor

  gQLaserballUtil = this;

  fLbtphist = NULL;
  fPmtanghist = NULL;
  fLBMaskTF1 = NULL;
  fLBMaskModelTF1 = NULL;
  fParnew = NULL;
  fParold = NULL;

  fNpar = 5; // five laserball parameters

  InitLBParameters(); // initialize fParnew and fParold

  fDebug = kFALSE; // no print statements by default


}
//______________________________________________________________________________
QLaserballUtil::~QLaserballUtil()
{
  // QLaserballUtil destructor

  if(gQLaserballUtil == this) gQLaserballUtil = NULL;

  if(fLBMaskTF1) {delete fLBMaskTF1; fLBMaskTF1 = NULL;}
  if(fLBMaskModelTF1) {delete fLBMaskModelTF1; fLBMaskModelTF1 = NULL;}
  if(fParnew) {delete fParnew; fParnew = NULL;}
  if(fParold) {delete fParold; fParold = NULL;}

}

//______________________________________________________________________________
void QLaserballUtil::InitLBParameters()
{
  // Sets the default LB parameters, new and old, using the 
  // fNpar dimension (usually 5).

  if(fParnew) {delete fParnew; fParnew = NULL;}
  if(fParold) {delete fParold; fParold = NULL;}

  if((fNpar < 5) || (fNpar > 10)) fNpar = 5; // reset

  fParnew = new Double_t[fNpar];
  fParold = new Double_t[fNpar];
  
  // -------------------------
  // These parameters were in QPath - let's assume they are 
  // still up-to-date.

  // new mask parameters in inches
  fParnew[0] = 1.0;
  fParnew[1] = 24.0;
  fParnew[2] = 2.030;
  fParnew[3] = 1.3500;
  fParnew[4] = 1.0;

  // old mask (naked ball) parameters in inches
  fParold[0] = 2.5;
  fParold[1] = 24.0;
  fParold[2] = 2.025;
  fParold[3] = 1.1875;
  fParold[4] = 1.0;
  // -------------------------

  return;
}
//______________________________________________________________________________
Float_t QLaserballUtil::GetRunLBOrientation(Int_t run)
{
  // Returns the orientation code: south=0, west=1, north=2, east=3
  // See GetRunLBPhi() for hints on conversion to phi angles.
  //
  // Only defined explicitly for runs in optical calibration data, after run
  // 10000 (start of production data).  For earlier runs, the orientation is
  // always returned as south; this includes Monte Carlo runs with negative
  // run numbers (see QPath::FillFromRDT()).
  //
  // Unrecognized runs return -1 (undefined orientation).

  //--------------------
  if (run<0) {  // Monte Carlo runs
    // "Artificially rotated" runs to break the laserball distribution symmetry
    // of a perfectly isotropic ball in the simulation.
    printf("Monte Carlo Run: ");
    Int_t runmod = run % 100;
    if ((runmod<=-30) && (runmod>-50)) {
      printf("Orientation 1\n");
      return 1;
    }
    if ((runmod<=-50) && (runmod>-70)) {
      printf("Orientation 2\n");
      return 2;
    }
    if ((runmod<=-70) && (runmod>-90)) {
      printf("Orientation 3\n");
      return 3;
    }
    printf("Orientation 0\n");
    return 0;
  }

  //--------------------
  if ((run>0) && (run<10000)) return 0;  // Early runs without a defined orientation

  //--------------------
  // February 2000 runs at 365nm, 500nm:
  if ((run>=10988) && (run<=11086)) return 0;
  if ((run>=11086) && (run<=11107)) return 2;
  if ((run>=11115) && (run<=11177)) return 3;
  if ((run>=11182) && (run<=11184)) return 1;
  if ((run>=11189) && (run<=11244)) return 3;
  if ((run>=11250) && (run<=11252)) return 0;
  if (run==11255) return 2;
  if ((run>=11259) && (run<=11260)) return 1;
  if ((run>=11263) && (run<=11265)) return 3;

  //--------------------
  // September 2000 runs at 337nm, 365nm, 386nm, 420nm, 500nm, 620nm
  if ((run>=12631) && (run<=12636)) return 3;  // Flipped runs, set 1
  if ((run>=12639) && (run<=12643)) return 1;
  if ((run>=12683) && (run<=12687)) return 2;
  if ((run>=12646) && (run<=12650)) return 0;

//  if ((run>=12652) && (run<=12790)) return 0;  // Runs at 337, 386, 500nm
  if ((run>=12652) && (run<=12682)) return 0;  // Runs at 337, 386, 500nm
  if ((run>=12688) && (run<=12790)) return 0;

  if ((run>=12797) && (run<=12803)) return 3;  // Flipped runs, set 2
  if ((run>=12806) && (run<=12812)) return 1;
  if ((run>=12814) && (run<=12821)) return 2;
  if ((run>=12823) && (run<=12830)) return 0;

  if ((run>=12832) && (run<=12975)) return 0;  // Runs at 365, 620, 420nm

//   if ((run>=12983) && (run<=13225)) return -2; // Guide tube runs must be fit!
  // *************************DG added these from LED studies
  // converted to orientations
  if (run >= 12983 && run <= 13004) return 1.893;
  if (run >= 13006 && run <= 13020) return 1.834;
  if (run >= 13022 && run <= 13024) return 1.834;
  if (run >= 13026 && run <= 13038) return 1.834;
  if (run >= 13042 && run <= 13052) return 2.011;
  if (run >= 13054 && run <= 13060) return 1.977;
  if (run >= 13061 && run <= 13078) return 1.877;
  if (run >= 13080 && run <= 13095) return 1.886;
  if (run >= 13096 && run <= 13102) return 1.807;
  if (run >= 13103 && run <= 13120) return 1.888;


  if ((run>=13232) && (run<=13290)) return 0;  // After guide tube runs (south?)

  //--------------------
  // January 2001 runs at 337nm, 365nm and 500nm (miniscan)
  if ((run>=14680) && (run<=14880)) return 0;  // All south facing

  //--------------------
  // Sept 2001 Lasermania
  if ((run>=21086) && (run<=21550)) return 0;  // All south facing
  if ((run>=21030) && (run<=21043)) return 1;  // West facing centre runs
  if ((run>=21046) && (run<=21056)) return 3;  // East facing centre runs
  if ((run>=21058) && (run<=21068)) return 2;  // North facing centre runs
  if ((run>=21070) && (run<=21085)) return 0;  // South facing centre runs

  //--------------------
  // Nov 2001 Miniscan
  if ((run>=22128) && (run<=22140)) return 1;  // West facing centre runs
  if ((run>=22141) && (run<=22320)) return 3;  // East facing centre  runs

  //--------------------
  // May 2002 big scan
  if ((run>=24903) && (run<=24913)) return 3;  // East facing centre runs
  if ((run>=24916) && (run<=24927)) return 1;  // West facing centre runs
  if ((run>=24935) && (run<=24958)) return 2;  // North facing centre runs
  if ((run>=24961) && (run<=24972)) return 0;  // South facing centre runs (main)

  if ((run>=24974) && (run<=25093)) return 0;  // South facing XZ diagonal 
  if ((run>=25096) && (run<=25106)) return -1; // Leslie-type events scan: ignore
  // if ((run>=25109) && (run<=25226)) return 3;  // East facing YZ diagonal (1)
  // if ((run>=25228) && (run<=25333)) return 3;  // East facing vertical (337-420) 
  // if ((run>=25335) && (run<=25400)) return 3;  // East facing YZ diagonal (2)
  // if ((run>=25402) && (run<=25443)) return 3;  // East facing vertical (620) 
  // if ((run>=25446) && (run<=25456)) return 3;  // East facing YZ diagonal (3)
  // above was wrong!
  if ((run>=25109) && (run<=25226)) return 1;  // East facing YZ diagonal (1)
  if ((run>=25228) && (run<=25333)) return 1;  // East facing vertical (337-420) 
  if ((run>=25335) && (run<=25400)) return 1;  // East facing YZ diagonal (2)
  if ((run>=25402) && (run<=25443)) return 1;  // East facing vertical (620) 
  if ((run>=25446) && (run<=25456)) return 1;  // East facing YZ diagonal (3)

  //--------------------
  // Oct 2002 Miniscan 
  if ((run>=27232) && (run<=27244)) return 1;  // West facing centre runs
  if ((run>=27246) && (run<=27256)) return 3;  // East facing centre runs
  if ((run>=27258) && (run<=27274)) return 2;  // North facing centre runs
  if ((run>=27276) && (run<=27286)) return 0;  // South facing centre runs (main)

  if ((run>=27288) && (run<=27483)) return 0;  // South facing (rest of scan)

  //--------------------
  // Jan 2003 Light & Heavy water scan 
  // Before H2O scan
  if ((run>=28543) && (run<=28557)) return 1;  // West facing centre runs
  if ((run>=28561) && (run<=28573)) return 3;  // East facing centre runs
  if ((run>=28576) && (run<=28586)) return 2;  // North facing centre runs
  if ((run>=28588) && (run<=28602)) return 0;  // South facing centre runs (main)
  //After H2O scan
  if ((run>=29044) && (run<=29055)) return 1;  // West facing centre runs
  if ((run>=29061) && (run<=29222)) return 3;  // East facing centre and YZ diagonal
  if ((run>=29225) && (run<=29236)) return 2;  // North facing centre runs
  if ((run>=29240) && (run<=29539)) return 0;  // South facing centre and XZ diagonal
  if ((run>=29541) && (run<=29621)) return 3;  // East facing YZ diagonal
 
  //--------------------
  // Apr 2003 second "end of salt" scan
  // 
  if ((run>=31190) && (run<=31210)) return 1;  // West facing centre runs
  if ((run>=31216) && (run<=31227)) return 3;  // East facing centre runs
  if ((run>=31232) && (run<=31242)) return 2;  // North facing centre runs
  if ((run>=31251) && (run<=31264)) return 0;  // South facing centre runs (main)

  if ((run>=31266) && (run<=31473)) return 0;  // South facing scan
  if ((run>=31476) && (run<=31658)) return 3;  // East facing scan

  //--------------------
  // Aug 2003 third "end of salt" scan
  //
  if ((run>=32998) && (run<=33076)) return 3;  // East facing scan (1)
  if ((run>=33080) && (run<=33089)) return 2;  // North facing centre runs
  if ((run>=33092) && (run<=33356)) return 0;  // South facing scan
  if ((run>=33092) && (run<=33356)) return 1;  // West facing centre runs (1)
  if ((run>=33373) && (run<=33470)) return 3;  // East facing scan (2)
  if ((run>=33484) && (run<=33486)) return 1;  // West facing centre runs (2)

  //--------------------

  return -1;  // Default value is -1: undefined
}
//______________________________________________________________________________
Float_t QLaserballUtil::GetRunLBPhi(Float_t orientation)
{
  // Returns the orientation angle of the laserball:
  // south=   0     orientation=0
  // west =  pi/2               1
  // north=  pi                 2
  // east =3*pi/2               3
  //
  // NB:
  // phi_laserball = phi_detector + GetRunLBPhi(run)
  // where phi_laserball is relative to runs with the manipulator slot
  // facing south.
  //
  // Unrecognized runs return -1.0 (undefined orientation).

//  Int_t orientation = GetRunLBOrientation(run);
//
//  if (orientation>=0) return (Float_t)orientation*TMath::Pi()/2.0;
//  else {
//    //**************************DG added these from LED studies
//    if (run >= 12983 && run <= 13004) return 2.973;
//    if (run >= 13006 && run <= 13020) return 2.881;
//    if (run >= 13022 && run <= 13024) return 2.881;
//    if (run >= 13026 && run <= 13038) return 2.881;
//    if (run >= 13042 && run <= 13052) return 3.159;
//    if (run >= 13054 && run <= 13060) return 3.106;
//    if (run >= 13061 && run <= 13078) return 2.948;
//    if (run >= 13080 && run <= 13095) return 2.962;
//    if (run >= 13096 && run <= 13102) return 2.839;
//    if (run >= 13103 && run <= 13120) return 2.965;
//    return -1.; // Added to eliminate compilation warning -- rsd 2002.06.03
//  }

  if (orientation>=0) return orientation*TMath::Pi()/2.0;
  return -1;

}

//______________________________________________________________________________
Float_t QLaserballUtil::LBAsymmetry(Float_t costheta,Float_t phi)
{
  // Laserball asymmetry

  Float_t asymmetry;

  // Extract the parameters from the histogram *tp, usually provided from Fraser
  // Duncan's fitting procedure of Ylm spherical harmonics to the phi-flipped
  // laserball runs.
  // 25-Jan-2000, updated by Moffat - 22-Apr-2000
  if (fLbtphist)
    asymmetry = fLbtphist->GetBinContent(fLbtphist->FindBin(phi,costheta));
  else asymmetry = 1.0;

  return asymmetry;
}

//______________________________________________________________________________
void QLaserballUtil::SetLBAsymmetryFile(TFile *lbasymfile)
{
  // Use the TFile given to fill in lbass_tp (laserball asymmetry) and
  // pmtanghist (PMT angular response) histograms.

  if(!lbasymfile){
    Warning("SetLBAsymmetryFile","No vaild TFile.");
    return;
  }

  if (lbasymfile->IsOpen()) {  // NB histograms may still be NULL if not in file!
    fLbtphist = (TH2 *) lbasymfile->Get("tp");
    fPmtanghist = (TH1 *) lbasymfile->Get("pmtang");
  } else {
    Error("SetLBAsymmetryFile","%s wasn't open.\n",lbasymfile->GetName());
  }
  return;
}
//______________________________________________________________________________
Double_t QLaserballUtil::LBMask(Float_t costheta, Int_t version)
{
  // Simple analytic model of laserball mask.
  // 
  // version == 0 : new mask with stainless steel shield (as of Feb-00)
  // version == 1 : naked ball (prior to Feb-00)
  //
  // As implemented, the parameters used correspond to adjusted values,
  // and not to measurements of the laserball hardware.  This will presumably
  // be improved in any future versions of the masking function...


  Double_t rs[1];
  rs[0] = costheta;

  switch (version) {
  case 1:
    // For old geometry (prior to mask of February 6-10, 2000)
    return lbmaskfunction(rs,fParold);
    break;
  case 0:
  default:
    // For new masked geometry (as of February 6-10, 2000)
    return lbmaskfunction(rs,fParnew);
    break;
  }

  Error("LBMask","Fell through! (version %d, costheta %10.7f\n",version,costheta);
  return -1;  // Fall through case (shouldn't be needed!)
}
//______________________________________________________________________________
void QLaserballUtil::SetLBMaskTF1(Int_t version, Bool_t cosine)
{
  // Returns a function to plot the laserball masking function
  //
  // If cosine is kTRUE, returns a function of cos(theta);
  // if cosine is kFALSE, returns a function of theta.

  TF1 *flbm;
  if(cosine) flbm = new TF1("flbm",QLaserballUtil_external_lbmaskfunction,-1,1,5);
  else flbm = new TF1("flbm",QLaserballUtil_external_lbmasktheta,0,180,5);

  flbm->SetLineWidth(1);
  flbm->SetParNames("Bottom Cylinder Height","Top Cylinder Height",
		    "Ball Radius","Cylinder Radius");
  flbm->SetNpx(180);
  flbm->SetLineColor(2);
  flbm->SetMinimum(0.0);
  flbm->SetMaximum(1.5);
  switch (version) {
  case 1:
    flbm->SetParameters(fParold);
    break;
  case 0:
  default:
    flbm->SetParameters(fParnew);
  }
  
  fLBMaskTF1 = flbm;
  return;
}



//______________________________________________________________________________
// Support routines for simple laserball masking function.
//
// 14-Dec-1999 - Bryce Moffat - Queen's University
// 25-Apr-2000 - Bryce Moffat - Updated for inclusion in QPath class
// 26-Mai-2006 - O.Simard     - Moved to QLaserballUtil class
//______________________________________________________________________________
Double_t QLaserballUtil::x0(Double_t d,Double_t a,Double_t b,Double_t r)
{
  // Returns the intersection point of the ellipse (from cylindrical mask)
  // with the ball (uniformly lit disk model).
  //
  // d is the x-offset for ellipse with respect to the circle
  // a,b are semi-major and -minor axes for the ellipse
  // r is the radius of the circle
  //
  // Returns 9999.0 if the intersection point isn't findable due to mathematical
  // error (eg. root of negative number).

  if (b>a) {  // ensure semi-major and -minor ellipses correspond to variables
    Double_t temp = a;
    a=b;
    b=temp;
  }

  Double_t x0result;

  if (a!=b) { // elliptical case
    Double_t aa = 1.0/(b*b) - 1.0/(a*a);
    Double_t bb = -2.0*d/(b*b);
    Double_t cc = (d/b)*(d/b) + (r/a)*(r/a) - 1.0;
    Double_t radical = bb*bb - 4.0*aa*cc;
    if (radical>=0) {
      x0result = (-bb-sqrt(radical))/(2.0*aa);
    } else {
      return 9999.0;
    }
  } else { // circular case
    if (d>0) x0result = (d*d+r*r-a*a)/(2*d);
    else return 9999.0;
  }

  return x0result;
}
//______________________________________________________________________________
Double_t QLaserballUtil::areaellipse(Double_t a,Double_t b,Double_t d,Double_t xlo,Double_t xhi)
{
  // Returns the area of an ellipse, offset from the centre by d (x-offset) in the
  // x-direction.
  // Bounds checking is performed.

  if (xlo>xhi) {
    Double_t temp = xlo;
    xlo = xhi;
    xhi = temp;
  }

  if (xlo<d-b) xlo = d-b;
  if (xhi>d+b) xhi = d+b;

  Double_t aell;
  Double_t asinpar;
  Double_t sqrtpar;

  if ((xlo<d+b) && (xhi>d-b)) {
    Double_t term1,term2;

    asinpar = (xhi-d)/b;
    if (asinpar>=1.0) asinpar = 1.0;
    if (asinpar<=-1.0) asinpar = -1.0;

    sqrtpar = 1-pow((xhi-d)/b,2);
    if (sqrtpar<0.0) sqrtpar = 0.0;

    term1 = (xhi-d)/b*sqrt(sqrtpar) + asin(asinpar);

    asinpar = (xlo-d)/b;
    if (asinpar>=1.0) asinpar = 1.0;
    if (asinpar<=-1.0) asinpar = -1.0;

    sqrtpar = 1-pow((xlo-d)/b,2);
    if (sqrtpar<0.0) sqrtpar = 0.0;

    term2 = (xlo-d)/b*sqrt(sqrtpar) + asin(asinpar);

    aell = a*b*(term1-term2);

  } else {
    aell = 0.0;
  }

  return aell;
}
//______________________________________________________________________________
Double_t QLaserballUtil::areacircle(Double_t r,Double_t xlo,Double_t xhi)
{
  // Returns the area of a circle centred on the origin.
  // Bounds checking is performed.

  if (xlo>xhi) {
    Double_t temp = xlo;
    xlo = xhi;
    xhi = temp;
  }

  if (xlo<-r) xlo = -r;
  if (xhi> r) xhi =  r;

  Double_t acir;
  Double_t asinpar;
  Double_t sqrtpar;
 
  if ((xlo<r) && (xhi>-r)) {

    asinpar = xhi/r;
    if (asinpar>1.0) asinpar = 1.0;
    if (asinpar<-1.0) asinpar = -1.0;

    sqrtpar = r*r-xhi*xhi;
    if (sqrtpar<0.0) sqrtpar = 0.0;

    Double_t term1 = xhi*sqrt(sqrtpar)+r*r*asin(asinpar);

    asinpar = xlo/r;
    if (asinpar>1.0) asinpar = 1.0;
    if (asinpar<-1.0) asinpar = -1.0;

    sqrtpar = r*r-xlo*xlo;
    if (sqrtpar<0.0) sqrtpar = 0.0;

    Double_t term2 = xlo*sqrt(sqrtpar)+r*r*asin(asinpar);
    acir = term1 - term2;
  } else {
    acir = 0.0;
  }

  return acir;
}
//______________________________________________________________________________
Double_t QLaserballUtil::lbmaskfunction(Double_t *rs, Double_t *par)
{
  // Returns the simple masking function for a cylinder above the laserball.
  //
  // rs[0] is cos(theta_laserball).
  // par[0..4] are hbot,htop,rball,rcyl,abscyl.
  //
  // The cylinder starts at hbot=0.5" above the top of the ball and continues to
  // htop=24" above the ball.
  // The laserball is modelled as a sphere of radius rball = 2.14"
  // The cylinder is modelled as a sphere of radius rcyl = 1.05"
  // The cylinder has relative absorption of abscyl = 1.0 (fully absorbing).
  //
  // NB. The theta parameter should only be used between [-pi,pi] - checks
  // incomplete on this!

  if (rs[0]>1.0) rs[0] = 1.0;
  if (rs[0]<-1.0) rs[0] = -1.0;
  Double_t theta = acos(rs[0]);  // Parameter is cos(theta) -> convert to radians

  Double_t hbot = par[0];   // Height of bottom ring above ball
  Double_t htop = par[1];   // Height of top ring above ball
  Double_t rball= par[2];   // Radius of ball
  Double_t rcyl = par[3];   // Radius of cylinder
  Double_t abscyl = par[4]; // Absorbance of cylinder (1=total abs, 0=transparent)

  if (theta<0) theta *= -1;  // Always put us in positive situation
  if (theta>TMath::Pi()/2.0) return 1.0;  // No masking of bottom angles

  Double_t dtop = (htop + rball)*sin(theta);
  Double_t btop = rcyl*cos(theta);
  Double_t atop = rcyl;
  Double_t x0top = x0(dtop,atop,btop,rball);

  Double_t dbot = (hbot + rball)*sin(theta);
  Double_t bbot = rcyl*cos(theta);
  Double_t abot = rcyl;
  Double_t x0bot = x0(dbot,abot,bbot,rball);

  // Circle area (to be used in denominator as normalization)
  Double_t aball = TMath::Pi()*rball*rball;

  // Mask area - a sum of various parts of the ellipses and box (preliminary)
  Double_t amask = TMath::Pi()*abot*bbot;

  // Mask ratio - returned value from this function
  Double_t lbmaskratio = amask/aball;

  // Ellipse areas (preliminary)
  Double_t aelltop = TMath::Pi()*atop*btop/2.0;
  Double_t acirtop = 0.0;
  Double_t aellbot = TMath::Pi()*abot*bbot/2.0;
  Double_t acirbot = 0.0;

  // Box area (preliminary)
  Double_t xbox = sqrt(rball*rball-atop*atop);  // Box intersection for most common case
  Double_t abox = 2.0*atop*(xbox-dbot);  // (when box is from bottom ellipse to circle)

  // Now, check if the top ellipse is inside, intersecting or outside circle
  // Second half of ellipse is the outer half.
  // First half of ellipse is the inner half, which isn't counted towards the
  // ellipse's area but rather to the box's.

  if (dtop-btop<rball) {  // top ellipse is inside or intersecting
    if (x0top<rball) { // top ellipse has a valid intersection
      if (x0top>dtop) { // top ellipse has intersection in second half
	aelltop = areaellipse(atop,btop,dtop,dtop,x0top);
	acirtop = areacircle(rball,x0top,rball);
	abox = 2.0*atop*(dtop-dbot);
      } else { // top ellipse has intersection in first half - area split box/circle
	xbox = sqrt(rball*rball-atop*atop);
	aelltop = 0.0;
	acirtop = areacircle(rball,xbox,rball);
	abox = 2.0*atop*(xbox-dbot);
      }
    } else { // top ellipse is entirely inside the circle
      aelltop = TMath::Pi()*atop*btop/2.0;
      acirtop = 0.0;
      abox = 2.0*atop*(dtop-dbot);
    }
  } else { // top ellipse is out of the picture; check the bottom ellipse
    xbox = sqrt(rball*rball-atop*atop);
    aelltop = 0.0;
    acirtop = areacircle(rball,xbox,rball);
    abox = 2.0*atop*(xbox-dbot);

    if (dbot-bbot<rball) { // bottom ellipse is inside or intersecting
      if (x0bot<rball) { // bottom ellipse has a valid intersection
	if (x0bot>dbot) { // bottom ellipse has intersection in second half
	  aellbot = TMath::Pi()*abot*bbot/2.0;
	  xbox = sqrt(rball*rball-atop*atop);
	  abox = 2.0*atop*(xbox-dbot);
	} else { // bottom ellipse has intersection in first half - split ellipse/circle
	  aellbot = areaellipse(abot,bbot,dbot,dbot-bbot,x0bot);
	  acirbot = areacircle(rball,x0bot,rball);
	  acirtop = 0.0;
	  abox = 0.0;
	}
      } else { // bottom ellipse is entirely inside the circle
	aellbot = TMath::Pi()*abot*bbot/2.0;
	xbox = sqrt(rball*rball-atop*atop);
	abox = 2.0*atop*(xbox-dbot);
      }
    } else {
      acirtop = 0.0;
      aellbot = 0.0;
      acirbot = 0.0;
      abox = 0.0;
    }
  }

  amask = aelltop + acirtop + abox + aellbot + acirbot;
  lbmaskratio = abscyl * (amask/aball);

  if(fDebug) {
    printf("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f "
	   "%10.4f %10.4f %10.4f %10.4f\n",
	   theta*180/TMath::Pi(),
	   dtop,btop,x0top,aelltop,acirtop,
	   acirbot,dbot,x0bot,aellbot,
	   abox,
	   amask,1.0-lbmaskratio);
  }

  return 1.0-lbmaskratio;
}
//______________________________________________________________________________
Double_t QLaserballUtil::lbmasktheta(Double_t *rs, Double_t *par)
{
  // This is the laserball mask function with rs[0] = theta.
  // Simply calls the usual lbmaskfunction() with conversion of theta->cos(theta)
  // NB. Theta must be specified in RADIANS.

  Double_t costheta[1];
  costheta[0] = cos(rs[0]*TMath::Pi()/180.0);
  return lbmaskfunction(costheta,par);
}

//______________________________________________________________________________
void QLaserballUtil::SetLBMaskModelTF1(Int_t nlbmask, Double_t* lbmask)
{
  // Returns a pointer to a function with no parameters, to plot the current
  // laserball mask function polynomial in (1+cos(theta_lb)).
  //
  // lbmask array must be of dimension nlbmask.

  if((nlbmask <= 0) || !lbmask){
    fLBMaskModelTF1 = NULL;
    return;
  }

  Double_t* par = new Double_t[1+nlbmask];
  par[0] = nlbmask;
  for(Int_t i=0; i<nlbmask; i++) par[1+i] = lbmask[i];

  TF1* f = new TF1("LB Mask",QLaserballUtil_external_LBMaskModel,-1,1,1+nlbmask);
  f->SetParameters(par);
  f->SetNpx(100);
  f->SetMarkerStyle(20);

  delete[] par;

  fLBMaskModelTF1 = f;

  return;
}
//______________________________________________________________________________
Double_t QLaserballUtil::LBMaskModel(Double_t* x, Double_t* par)
{
  // Utility function which returns the laserball mask function corresponding to
  // a[0] = cos(theta_lb)

  Int_t npars = (Int_t) par[0];

  Int_t i;
  Float_t lbm = 0;
  Float_t oneplus = 1.0 + x[0];

  for (i=npars-1; i>=0; i--)
    lbm = lbm*oneplus + par[1+i];
  
  return lbm;
}

//______________________________________________________________________________
Double_t QLaserballUtil_external_lbmaskfunction(Double_t *rs, Double_t *par)
{
  // Simply call the class-internal function; just a place-holder for LBMaskF1().

  if(gQLaserballUtil) return gQLaserballUtil->lbmaskfunction(rs,par);
  else return 1.0;
}
//______________________________________________________________________________
Double_t QLaserballUtil_external_lbmasktheta(Double_t *rs, Double_t *par)
{
  // Simply call the class-internal function; just a place-holder for LBMaskF1().

  if(gQLaserballUtil) return gQLaserballUtil->lbmasktheta(rs,par);
  else return 1.0;
}
//______________________________________________________________________________
Double_t QLaserballUtil_external_LBMaskModel(Double_t *rs, Double_t *par)
{
  // Simply call the class-internal function; just a place-holder for LBMaskF1().

  if(gQLaserballUtil) return gQLaserballUtil->LBMaskModel(rs,par);
  else return 1.0;
}
