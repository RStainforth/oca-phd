//*-- Author :    Olivier Simard   2004 02
/*************************************************************************
 * Copyright(c) 2004, The SNO Software Project, All rights reserved.     *
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

#include "QOCAReflect.h"

ClassImp(QOCAReflect)
;
//
// QOCAReflect: SNO OCA Reflections Class
//
// Natural extension of QOptics methods to study and follow
// reflected light paths mainly at high radius source
// positions since it is where the effect is the most important.
//
// 05.2006 - O.Simard
// Updated for NCD reflections, although this kind of reflection
// does not require high radii source positions. The reflections
// are assumed to be diffuse.
//
// ----------------------------------
//
// When this class is called, it initializes automatically its own
// QOptics instance and uses many QOptics functions.
//
// [0] QOCAReflect* refl = new QOCAReflect();
//
//   * PMT directions calculated.
//   * PMT nearest neighbour list loaded.
//   * Number of QOptics instances: 1
//   * wl 420 nd 1.33628 na 1.50344 nh 1.34217
//
// ----------------------------------
// Main algorithm and functions usage:
//
// [ ] AV Reflections
//
// [0] QOCAReflect* refl = new QOCAReflect();
//
//  - Set a source:
// [1] TVector3 source(x,y,z);
// [2] refl->SetSourceVector(source);
//
// [1] TVector3* source = new TVector3(x,y,z);
// [2] refl->SetSourceVector(*source);
//
//  - Set a timing window (in ns) for the prompt light selection
// [3] Double_t timew = 10.;
// [4] refl->SetTimeWindow(timew);
//
//  - Ask for a valid PMT position:
// [5] Int_t pmtn;
//
// [6] refl->GetDistances(pmtn); // QOptics method
// [6] refl->SelectedPMT(pmtn); // implicit call of GetDistances(pmtn)
//
//  - Find if there are physical AV reflections
// [6] refl->TrackPMT(pmtn);
//
//  - Find all physical AV reflections and print summary
// [6] refl->TrackAllPMTs();
//
//  - Find the geometrical PMT selection based
//    on the timing window for prompt AV reflection contamination
// [6] Double_t* thps = refl->GetCThetapsRange();
//
// ----------------------------------
//
// [ ] NCD Reflections
//
// [0] QOCAReflect* refl = new QOCAReflect();
//
// [1] to [4] are the same as for AV reflections (above)
//
// - Set the step size in z for Ncd length scanning
// [5] refl->SetStepZ(5); // cm
//
// - Call main function that loops over Pmts/Ncds
// [6] TH1D* h = NCDReflection();
//
// h contains one value per pmtn, essentially a correction to the prompt 
// peak occupancy, since flagging would get rid of too many tubes.
//
// ----------------------------------
//
// NOTES:
// - Many functions call other functions.
//   Variables are set between calls so that each function
//   does not have to return a value or take that value as an
//   argument.
//
//   ---------
//   example 1: the main algorithm in GetCThetapsRange() is the following.
//   1) set tolerances to acceptable values
//      (order of 0.5 degree precision for instance)
//   2) loop over all TrackPMT(Int_t pmtn)
//   3) each TrackPMT(Int_t pmtn) sets its own vectors
//      verifies if the paths are valid:
//      - SelectedPMT(pmtn) is called,
//      - GetAvSolutions(arguments) is called:
//         o) needs alpha (incident angle on av) and
//            beta (reflection angle to the pmt) analytic functions
//            provided as TF1*,
//         o) solves for physical relflections (alpha = beta),
//         o) sets a number of solutions nsol,
//      - ScanAv(pmtn, nsol) looks for the best av vector
//        asking the function Minimize():
//         o) Minimize() loops over a small region in Theta
//            and sets the av vector based on the minimal
//            difference (alpha - beta) in case the analytic solution
//            was not precise enough: it performs a second check
//            based on vector algebra.
//      - A "fake" source is set in QOptics to that av vector
//        so that QOptics can be used to find the reflection path
//        distances and other basic quantities.
//      - If GetDistances(pmtn) returned kTRUE for that fake source
//        record the data and flag that pmtn if the time difference
//        between the direct and the reflected path falls below
//        the pre-set time window.
//   4) from all the values, get the range in thetaps:
//      it returns a Double_t[2]: first value is the minimum and
//      the second is the maximum seen over all the pmtn's.
//
//   ---------
//   example 2: the main algorithm in NCDReflection() is the following.
//   1) NCDReflection() contains a loop over all pmtn's
//   2) for each pmtn, GetNCDReflectionFactor(pmtn) calculates
//      the correction factor. This function loops over all Ncds.
//   3) for each QNCD, IsNCDReflection(QNCD*,pmtn) is called. This
//      function determines if there are reflections from that
//      pmtn-ncd couple.
//   4) IsNCDReflection(QNCD*,pmtn) calls ScanNCD(QNCD*). This
//      function scans the Ncd length and calculates the time
//      difference between the reflected and prompt path for each
//      point. The step size in z will make a difference on the results
//      and on the speed.
//
//______________________________________________________________________________
QOCAReflect::QOCAReflect()
{
  // QOCAReflect constructor: calls automatically the QOptics constructor.
  // 
  // It also initializes instances of TRotation class and sets
  // default values for all distances and angles.

  printf("----------------------------------------------------\n");
  printf("* QOCAReflect initialized its own QOptics instance.\n");

  // set current global pointer to latest created 
  // to access the external functions
  gQOCAReflect = this;
  fUnit = new TRotation();
  fRot = new TRotation(*fUnit);

  ResetAllRs(); ResetAllDs(); ResetAllTs();
  ResetAllAngles();
  ResetAllTols();
  ResetCounters();
  SetDisplay(); // message display disabled

  // inner radius of AV must be set and stay constant
  SetAvR(GetAVInnerRadius());

  fConvert = TMath::Pi()/180.0;
  fNpmts = 10000;

  // z-limits for Ncd reflections
  fNcdZmin = -999.;
  fNcdZmax =  999.;
}
//______________________________________________________________________________
QOCAReflect::~QOCAReflect()
{
  // QOCAReflect destructor. Deletes TRotation instances.

  if(fUnit) { delete fUnit; fUnit = NULL; }
  if(fRot)  { delete fRot;  fRot = NULL;  }
}
//______________________________________________________________________________
void QOCAReflect::ResetAllRs()
{
  // Reset all magnitudes associated with QOCAReflect vectors

  SetSourceR(); SetSourceZ();
  SetPmtR(); SetAvR();
  SetSourcetoPmtR(); SetSourcetoAvR(); SetSourcetoNcdR();
  SetAvtoPmtR(); SetNcdtoPmtR();
  SetSourcetoAvtoPmtR(); SetSourcetoNcdtoPmtR();
  SetRDifference();

  return;
}
//______________________________________________________________________________
void QOCAReflect::ResetAllDs()
{
  // Reset all magnitudes associated with QOptics vectors

  SetSourcetoPmtD(); SetSourcetoAvD(); SetSourcetoNcdD();
  SetAvtoPmtD(); SetNcdtoPmtD();
  SetSourcetoAvtoPmtD(); SetSourcetoNcdtoPmtD();

  SetDDifference();

  return;
}
//______________________________________________________________________________
void QOCAReflect::ResetAllTs()
{
  // Reset all time of flight associated with QOptics

  SetSourcetoPmtT(); SetSourcetoAvT(); SetSourcetoNcdT();
  SetAvtoPmtT(); SetNcdtoPmtT();
  SetSourcetoAvtoPmtT(); SetSourcetoNcdtoPmtT();
  SetTDifference();

  return;
}
//______________________________________________________________________________
void QOCAReflect::ResetAllAngles()
{
  // Reset angles and others

  SetThetaLimits(); SetPhiLimits();
  SetThetaSource(); SetPhiSource();
  SetThetaPmt(); SetPhiPmt();
  SetThetaAv(); SetPhiAv();
  SetAlphaAv(); SetBetaAv();
  SetCThetaps(); SetCThetapsMax(); SetCThetapsMin();
  
  return;
}
//______________________________________________________________________________
void QOCAReflect::ResetAllTols()
{
  // Reset tolerances and various step sizes

  SetStepSizes(); SetStepZ();
  SetThetaTol(); SetPhiTol();
  SetPlaneTol(); SetRefleTol();

  return;
}
//______________________________________________________________________________
void QOCAReflect::ResetCounters()
{
  // Reset integers that counts reflections or solutions

  SetNAvSolution();
  SetNAvReflection(); SetNNcdReflection();
  SetNAvTransmission();
  SetNAvNewCut(); SetNAvDiscarded();
  SetOops(); SetSelect();

  return;
}
//______________________________________________________________________________
void QOCAReflect::SetTimeWindow(Double_t tw)
{
  // Set the time window (in ns) used to identify prompt reflections.
  //
  // From Olivier's Masters thesis, MC data and QOCAFit results, 
  // only values above 8 ns should be used. It is also useless to
  // use QOCAReflect with a superwide time window, as all PMTs will
  // be flagged for detecting reflections.
  //

  if(tw < 8.){
    Warning("SetTimeWindow","Time Window below 8 ns are dangerous.");
    tw = 8.; // reset
  }

  fTwindow = tw;
  if(fDisplay){
    printf("------------------------------------\n");
    printf("* In QOCAReflect::SetTimeWindow():\n");
    printf("* Setting timing window to %.2f ns.\n",fTwindow);
    printf("------------------------------------\n");
  }

  return;
}
//______________________________________________________________________________
Bool_t QOCAReflect::SetSourceVector(TVector3 source)
{
  // Sets the source vector and the rotation angles
  // for off-z-axis source positions.
  //
  // Note that sources outside the AV (radius > 600 cm) will not
  // give sensible results until QOptics is fixed to work 
  // in this region.

  fSource = source;
  SetThetaSource(fSource.Theta()); SetPhiSource(fSource.Phi());
  SetSourceR((Double_t)fSource.Mag());
  SetSourceZ((Double_t)fSource.Z());

  // check for sources outside the AV
  if(GetSourceR() > 600.){
    Warning("SetSourceVector","QOptics will not work for sources outside the AV.");
    return kFALSE;
  }

  // check for sources with low radii
  if(GetSourceR() < 450.){
    printf("* In QOCAReflect::SetSourceVector():\n");
    printf("* Not a high radius run: %.2f < 450.0 cm.\n",GetSourceR());
    printf("* Skipping AV Reflection Calculations.\n");
    return kFALSE;
  }

  // set the transformated z-axis to the opposite source vector
  fZp = - fSource.Unit();

  // print radius and z-coordinate if necessary
  if(fDisplay){
    printf("* In QOCAReflect::SetSourceVector():\n");
    printf("* Source position:\n\twith R = %.2f and Z = %.2f cm.\n",
	   (Float_t)fRsource,(Float_t)fZsource);
  }

  return kTRUE;
}
//______________________________________________________________________________
void QOCAReflect::SetPmtVector(TVector3 pmt)
{
  // Sets the Pmt vector and other attached quantities.

  // assigns this vector
  fPmt = pmt;
  // find the spherical coordinates
  SetThetaPmt(fPmt.Theta()); SetPhiPmt(fPmt.Phi());
  // the radius of the PSUP is different for each single PMT
  SetPmtR((Double_t)fPmt.Mag());
  // set the angle between source and pmt
  // as it was originally done in QOptics using scalar product.
  SetCThetaps(fPmt.Unit() * fSource.Unit());

  // set the transformated x and y axis according to pmt position
  // with respect to the source position
  fXp = -(fSource.Unit()).Cross(fPmt.Unit());
  fYp = fZp.Cross(fXp);

  return;
}
//______________________________________________________________________________
void QOCAReflect::SetAvVector(TVector3 av)
{
  // Sets the Av vector and other attached quantities.

  fAv = av;
  SetThetaAv(fAv.Theta()); SetPhiAv(fAv.Phi());
  //do not reset the radius of the AV since it is assumed to be constant
  return;
}
//______________________________________________________________________________
void QOCAReflect::SetNcdVector(TVector3 ncd)
{
  // Sets the Ncd vector.

  fNcd = ncd;
  return;
}
//______________________________________________________________________________
void QOCAReflect::SetSourcetoPmtVector(TVector3 source_pmt)
{
  // Sets the vector between the source and the current pmt.

  fSource_Pmt = source_pmt;
  SetSourcetoPmtR((Double_t)fSource_Pmt.Mag());
  return;
}
//______________________________________________________________________________
void QOCAReflect::SetSourcetoAvVector(TVector3 source_av)
{
  // Sets the vector between the source and the current av point.

  fSource_Av = source_av;
  // incident angle on AV inner surface
  SetAlphaAv(fSource_Av.Angle(fAv));
  SetSourcetoAvR((Double_t)fSource_Av.Mag());
  return;
}
//______________________________________________________________________________
void QOCAReflect::SetAvtoPmtVector(TVector3 av_pmt)
{
  // Sets the vector between the current av point and the current pmt.

  fAv_Pmt = av_pmt;
  // reflection angle from AV inner surface
  // the vector av_pmt will be in the opposite direction
  // of the av vector if it is a reflection
  // caution : beta is 180 - angle between av and av_pmt vectors
  SetBetaAv(TMath::Pi() - fAv_Pmt.Angle(fAv));
  SetAvtoPmtR((Double_t)fAv_Pmt.Mag());
  return;
}
//______________________________________________________________________________
void QOCAReflect::SetSourcetoNcdVector(TVector3 source_ncd)
{
  // Sets the vector between the source and the current ncd.

  fSource_Ncd = source_ncd;
  // set distance to counter axis
  SetSourcetoNcdR((Double_t)fSource_Ncd.Mag());
  return;
}
//______________________________________________________________________________
void QOCAReflect::SetNcdtoPmtVector(TVector3 ncd_pmt)
{
  // Sets the vector between the current ncd and the current pmt.

  fNcd_Pmt = ncd_pmt;
  // set distance to counter axis
  SetNcdtoPmtR((Double_t)fNcd_Pmt.Mag());
  return;
}

//___________________________________________________________________
Double_t QOCAReflect_external_AlphaAv(Double_t* x, Double_t* par)
{
  // Simply call the class-internal function

  if(gQOCAReflect){return gQOCAReflect->AlphaAv(x,par);}
  else{
    Warning("QOCAReflect_external_AlphaAv()","No static pointer.\n");
    return 1.0;
  }
}
//___________________________________________________________________
TF1* QOCAReflect::GetAlphaAvTF1()
{
  // Returns a function to plot the incident angle function
  //
  // Calling hierachy : GetAlphaAvTF1->QOCAReflect_external_AlphaAv->AlphaAv
  //

  TF1* f = new TF1("alpha_av", QOCAReflect_external_AlphaAv, 0, 2*TMath::Pi(), 1);

  if(GetSourceR() != 0){
    f->SetParameter(0, (Double_t)fRsource);
  }
  else{
    f->SetParameter(0, 0.0);
    if(fDisplay){
      Warning("GetAlphaAvTF1()","Setting source radius to 0.0.\n");
    }
  }
  return f;
}
//_______________________________________________________________
Double_t QOCAReflect::AlphaAv(Double_t* x, Double_t* par)
{
  // Incident angle on inner AV surface
  // Depends only on fRsource, the source position radius
  // x[] is given in radians
  //
  // The following expressions are only valid for a z-axis source
  // Off-z-axis positions must be transformed before using this
  //

  Double_t phi_av = x[0];
  Double_t r_source = par[0];
  Double_t num = fRav - (r_source)*cos(phi_av);
  Double_t den = sqrt(r_source*r_source + fRav*fRav - 2*r_source*fRav*cos(phi_av));
  //returns alpha in radians
  Double_t alpha = acos((num)/(den));
  //if(IsInRange(alpha)){return alpha;}
  //else{return 0;}
  return alpha;
}
//___________________________________________________________________
Double_t QOCAReflect_external_BetaAv(Double_t* x, Double_t* par)
{
  // Simply call the class-internal function

  if(gQOCAReflect){return gQOCAReflect->BetaAv(x,par);}
  else{
    Warning("QOCAReflect_external_BetaAv()","No static pointer.\n");
    return 1.0;
  }
}
//___________________________________________________________________
TF1* QOCAReflect::GetBetaAvTF1()
{
  // Returns a function to plot the incident angle function
  //
  // Calling hierachy : GetBetaAvTF1->QOCAReflect_external_BetaAv->BetaAv
  //

  TF1* f = new TF1("beta_av", QOCAReflect_external_BetaAv, 0, 2*TMath::Pi(), 1);

  if(GetThetaPmt() != 0){
    f->SetParameter(0, acos(fCTheta_ps));
  }
  else{
    f->SetParameter(0, 0.0);
    if(fDisplay){
      Warning("GetBetaAvTF1()", "Setting PMT Angular Position to 0.0.\n");
    }
  }

  return f;
}

//_______________________________________________________________
Double_t QOCAReflect::BetaAv(Double_t* x, Double_t* par)
{
  // Reflection angle from inner AV surface for reflection
  // Independent of the source position radius
  // Depends only on the PMT position (phi_pmt)
  // theta_pmt = par[0] in radians
  //
  // The following expressions are only valid for a z-axis source
  // Off-z-axis positions must be transformed before using this
  //

  //Double_t delta_phi = (180-par[0]) - x[0]; //phi_pmt - phi_av
  Double_t delta_phi = par[0] - x[0]; //phi_pmt - phi_av
  Double_t num = fRav - fRpsup*cos(delta_phi);
  Double_t den = sqrt(fRpsup*fRpsup + fRav*fRav - 2*fRpsup*fRav*cos(delta_phi));
  //returns beta in radians
  Double_t beta = acos((num)/(den));
  //if(IsInRange(beta)){return beta;}
  //else{return 0;}
  return beta;
}

//_______________________________________________________________
Double_t QOCAReflect::FindSolution(TF1* f1, TF1* f2, Double_t x1, Double_t x2, Double_t step)
{
  // Algorithm to find numerical solution of two functions crossing each other
  // the solution is x, the common variable both functions depend on.
  //
  // Uses the difference between two functions evaluated at the same point.
  // When the diff changes sign, it means the solution point
  // has been passed : reuse to go back.
  //

  Double_t diff = 0;
  if(x1 > x2){
    //goes back assuming a positive step
    for(Double_t x = x1 ; x > x2+(0.5*step) ; x-=step){
      Double_t v1 = f1->Eval(x,0,0);
      Double_t v2 = f2->Eval(x,0,0);
      Double_t diff_tmp = v1 - v2;
      if(diff_tmp*diff < 0){
	return x;
      }
      diff = diff_tmp;
    }
  }
  else{
    //goes forward assuming a positive step
    for(Double_t x = x1 ; x < x2+(0.5*step) ; x+=step){
      Double_t v1 = f1->Eval(x,0,0);
      Double_t v2 = f2->Eval(x,0,0);
      Double_t diff_tmp = v1 - v2;
      if(diff_tmp*diff < 0){
	return x;
      }
      diff = diff_tmp;
    }
  }
  //default is to return 0 ; can be used as a boolean argument
  return 0;
}

//_______________________________________________________________
Int_t QOCAReflect::GetNAvSolutions(TF1* f1, TF1* f2, Double_t xi, Double_t xf, Double_t step)
{
  //
  // This function acts as a double-check for the number of solutions.
  // Uses the FindSolution algorithm to find the number of actual solutions.
  // Used only if the fSelect flag is set to kFALSE which means that there is
  // no selection in the reflection types a priori.
  //
  // No need to set the number of solution here
  // since this function must be called by GetAvSolutions() which also sets it
  //

  Int_t Nsol = 0; // default value
  Double_t y = xi;

  for(Double_t x = xi+step ; x <= xf ; x+=step){
    //search for solution in 10 points inside the step size
    //Double_t sol = FindSolution(f1, f2, y, x, 0.1*step);
    Double_t sol = FindSolution(f1, f2, y, x, 0.5*step);
    //solution found
    if(sol){Nsol++;}
    y = x;
  }

  if(fDisplay > 1){
    printf("In QOCAReflect::GetNAvSolutions():\n");
    printf("\tThere were %d different solution(s).\n",Nsol);
  }

  return Nsol;
}

//_______________________________________________________________
Double_t* QOCAReflect::GetAvSolutions(TF1* f1, TF1* f2, Double_t xi, Double_t xf, Double_t step)
{
  //
  // Creates a vector with the solutions in it for 2 different modes.
  //
  // All possible solutions:
  //   1: low incidence (some tubes)
  //   2: high incidence (some tubes)
  //   3: far reflection (all tubes)
  //   4: unphysical (some tubes, not used)
  //
  // fSelect = kFALSE : expects at least 1/3 solutions per PMT
  //
  // fSelect = kTRUE : expect at maximum 2/3 solutions per PMT
  //

  SetNAvSolution(); // reset the previous value of fNAvsolution
  Int_t Nsol = 0; // default value

  // look at the fSelect to find out if there is user selection
  if(!fSelect){ // means no selection, will find all solutions

    // Need to go through GetNAvSolutions() to reset Nsol
    Nsol = GetNAvSolutions(f1, f2, xi, xf, step);
    if(Nsol <= 0) return NULL;
  }
  else Nsol = 2; // means selection, force to look only for the two first solutions
    

  // set the number of solutions
  SetNAvSolution(Nsol);
  Double_t* sol_vec = new Double_t[Nsol];
  for(Int_t i = 0 ; i < Nsol ; i++){sol_vec[i] = 0.;} // set zeros

  Double_t y = xi;
  Int_t how_many = 0; // counter for solutions

  for(Double_t x = xi+step ; x <= xf ; x+=step){

    //Double_t sol = FindSolution(f1, f2, y, x, 0.1*step);
    Double_t sol = FindSolution(f1, f2, y, x, 0.5*step);
    if(sol && how_many < Nsol){ // we want all physical reflections
      //Double_t sol2 = FindSolution(f1, f2, y, sol, 0.01*step);
      Double_t sol2 = FindSolution(f1, f2, y, sol, 0.05*step);
      sol_vec[how_many] = sol2;
      how_many++; // increase after filling the array bin
    }
    y = x;
  }
  if(fDisplay){
    printf("In QOCAReflect::GetAvSolutions():\n");
    printf("\tThere were %d different solution(s).\n",Nsol);
    for(Int_t i = 0 ; i < Nsol ; i++){
      printf("\t phiav[%d] = %.1f (deg.)\n",i,ConverttoDeg(sol_vec[i]));
    }
    printf("\tSolution vector created.\n");
  }
  return sol_vec;
}

//____________________________________________________________________
Bool_t QOCAReflect::IsInRange(Double_t testvalue)
{
  // Checks if the testvalue is in the range [0, pi]
  
  Double_t pi = TMath::Pi();
  // if negative signs, add 2pi
  if(testvalue < 0) testvalue += 2*pi;

  if(0 <= testvalue && testvalue <= pi) return kTRUE;
  else return kFALSE;
}

//____________________________________________________________________
Bool_t QOCAReflect::IsInRange(Double_t testvalue, Double_t centralvalue, Double_t tol)
{
  // Checks if the testvalue is in the range
  // [centralvalue - tol, centralvalue + tol]

  Double_t xmin = centralvalue - tol;
  Double_t xmax = centralvalue + tol;
  //if negative signs, reverse the order
  if((xmax < 0 && xmin < 0) && xmax < xmin){
    Double_t tmp = xmax;
    xmax = xmin;
    xmin = tmp;
  }

  if(xmin <= testvalue && testvalue <= xmax) return kTRUE;
  else return kFALSE;
}

//____________________________________________________________________
void QOCAReflect::Transmission()
{
  //
  // Transmission:
  // - set distances and time of flight
  // - source inside the acrylic going through the AV and reaching PMTs
  // - calculations come from QOptics functions
  // - called once per PMT
  //

  // use QOptics to fill direct path distance (cm)
  // and time of flight (ns)
  SetSourcetoPmtD(GetDd2o() + GetDacrylic() + GetDh2o());
  SetSourcetoPmtT(GetTimeOfFlight());

  return;
}
//____________________________________________________________________
TH1D* QOCAReflect::NCDReflection()
{
  // Calls GetNCDReflectionFactor(pmtn) in a Pmt loop.
  //
  // The histogram returned contains a double per bin, being
  // the estimated correction to be applied on the pmt occupancy.

  // histogram to be returned
  TH1D* h = new TH1D("ncdrefcorr","NCD Reflection Occupancy Correction",
		     fNpmts,1,(Float_t)fNpmts);
  
  printf("------------------------------------- \n");
  printf("* In QOCAReflect::NCDReflection() ... \n");

  // pmt loop
  for(Int_t pmtn = 1 ; pmtn < fNpmts ; pmtn++){
    if(pmtn%1000 == 0) printf("\tat PMT %d\n",pmtn); // print update
    Double_t value = GetNCDReflectionFactor(pmtn);
    h->SetBinContent(pmtn,value);
  }
 
  printf("* Total number of Ncd reflections: %d\n",fNNcdrefle);
  printf("------------------------------------- \n");

  return h;
}

//____________________________________________________________________
Double_t QOCAReflect::GetNCDReflectionFactor(Int_t pmtn)
{
  // Calls IsNCDReflection() in a Ncd loop for this pmt.
  // Must consider all Ncds of the array.
  //
  // For each Ncd that returns kTRUE when calling IsNCDReflection(),
  // the PMT solid angle is calculated from the average z-position
  // of the Ncd counter: zavg = 0.5*(fNcdZmin+fNcdZmax).
  //
  // The final factor returned by this function is:
  //
  //   factor = weight*ncdsolidangle*pmtsolidangle/8pi^2
  //
  // Therefore this correction is uniquely based on timing and 
  // geometry.
  //
  // 01.2007 - O.Simard
  // Added weight relative to the source_ncd vector normal such
  // that Pmts at the back of the Ncds do not get as much reflections
  // due to the fraction of the reflective area they see. This 
  // does not affect the value itself by much, but the relative values
  // between Pmts will vary more.

  Double_t factor = 0.; // to be returned
  Double_t pmtsolidangle;
  Double_t ncdsolidangle;

  // get array of NCDs from QOptics
  QNCDArray* NcdArray = GetQNCDArray();

  if(fDisplay > 1){
    printf("------------------------------------\n");
    printf("* From QOCAReflect::GetNCDReflectionFactor(%.4d) ... \n",pmtn);
  }

  // loop over all ncd counter for this pmt
  for(Int_t incd = 0 ; incd < NcdArray->GetNCounters() ; incd++){

    // get this ncd counter info
    QNCD* qncd = (QNCD*)NcdArray->GetCounter(incd);

    // display info if needed
    if(fDisplay > 1){
      printf("\tCounter position: (x,y) = (%.2f, %.2f)\n",
    	     (qncd->GetCenter()).X(),(qncd->GetCenter()).Y());
    }

    // verify if there are valid reflections
    if(!IsNCDReflection(qncd,pmtn)) continue;

    // Ncd solid angle requires the limits only (normal incident angle)
    ncdsolidangle = GetNcdSolidAngle(qncd->GetIndex(),fNcdZmin,fNcdZmax);
    
    // QOptics needs to know where the point is on the Ncd
    // before calculating the solidangle:
    // if reflections were found z limits have been set
    Double_t zavg = 0.5*(fNcdZmin+fNcdZmax); // average
 
    // QOptics source: point on Ncd
    SetSource(qncd->GetCenter().X(),qncd->GetCenter().Y(),zavg); 
    if(!GetDistances(pmtn)) continue; // path could not be calculated
    pmtsolidangle = GetPmtSolidAngle(pmtn);
    
    // print for too much info
    if(fDisplay > 2){
      if(pmtn%1000 == 0){
	printf("%.4d pmtsolidangle = %.6f \t %.2d ncdsolidangle = %.6f\n",
	       pmtn,pmtsolidangle,incd,ncdsolidangle);
      }
    }

    SetSource(fSource); // reset QOptics source

    // calculate the angle between source_ncd and ncd_pmt
    // to weight the reflection
    TVector3 source_ncd(qncd->GetCenter() - fSource);
    TVector3 sn_xy(source_ncd.X(),source_ncd.Y(),0);
    TVector3 ncd_pmt(fPmt - qncd->GetCenter());
    TVector3 np_xy(ncd_pmt.X(),ncd_pmt.Y(),0);
    Double_t phi = TMath::Pi() - sn_xy.Angle(np_xy);
    Double_t weight = 0.5*(1+cos(phi));
    
    // add to the sum
    factor += weight*pmtsolidangle*ncdsolidangle;
  }

  // then normalize by solid angle maxima (2pi*4pi)
  factor /= 8*TMath::Power(TMath::Pi(),2); // normalize solid angles

  return factor; 
}

//____________________________________________________________________
Bool_t QOCAReflect::IsNCDReflection(QNCD* qncd,Int_t pmtn)
{
  // Check if a NCD reflection can affect the prompt
  // peak of this PMT.
  //
  // - assumes the source position has been set.
  // - gets the ncd counter info by using the QNCD object.
  // - calls ScanNCD(QNCD*) to get the reflection info.
  //
  // Note: shadowing effects should not be included here.
  // They can be removed further with the IsNcd() mask.

  // make sure the pmtn is fine for this kind of calculation:
  // start with the distances from the source (calls GetDistances())
  if(!SelectedPMT(pmtn)) return kFALSE; // sets fPmt
  
  // the pmt is fine therefore check for reflections:
  // quick checks on source-ncd distance
  
  // this one is for long distances and backward reflections
  TVector3 ncd(qncd->GetCenter().X(),qncd->GetCenter().Y(),fSource.Z());
  TVector3 source_ncd(ncd - fSource);
  if((source_ncd.Mag() > 150)){ // greater than 150 cm
    TVector3 ncd_pmt(fPmt - ncd);
    Double_t vd = GetVgroupD2O();
    Double_t diff = source_ncd.Mag() + ncd_pmt.Mag() - fRsource_pmt;
    // greater than 20 ns with backward angle
    if((diff/vd > 20) && (source_ncd.Angle(fSource_Pmt) > 0.5*TMath::Pi())){
      if(fDisplay > 1) 
	printf("QOCAReflect::IsNCDReflection: Pre-cut got rid of this pmt-ncd couple.\n");
      return kFALSE;
    }
  } 

  ScanNCD(qncd);

  // check if ScanNCD set the limits on z properly
  if((fNcdZmin == 999) && (fNcdZmax == -999)) return kFALSE;
  else AddaNcdReflection(); // reflection contaminates prompt peak 
  
  return kTRUE; // if here, PMT has been contaminated
}
//____________________________________________________________________
void QOCAReflect::ScanNCD(QNCD* qncd)
{
  // Scans the paths from the source to multiple points on
  // the Ncd counter, and for each point, calculates the total
  // path distance to the Pmt.
  // This function does not return anything but it sets the
  // z limits on the Ncd (fNcdZmin,fNcdZmax) after the scan
  // is completed.
  // --
  // This function assumes the AV refraction effects are
  // small. To get the total paths in all three media
  // (through QOptics::GetDistances()) see ScanNCDLong().
  //
  // fSource and fStep_Z need to be set before to get
  // the right paths and time differences.

  Double_t ncdz; // working z
  Double_t ncdz_top = (qncd->GetTop()).Z();
  Double_t ncdz_bot = (qncd->GetBottom()).Z();

  // inverted extreme limits
  fNcdZmin = 999;
  fNcdZmax = -999;

  // various variables
  Int_t found_min = 0, found_max = 0; // counters for z-points
  Double_t initialz, increment;       // starting point and increment size

  // redefine starting point and increment sign 
  // depending on pmt z-coordinate 
  if(fPmt.Z() > 0){
    initialz = ncdz_top - 0.5*fStep_Z;
    increment = -fStep_Z;
  } else {
    initialz = ncdz_bot + 0.5*fStep_Z;
    increment = fStep_Z;
  }
  
  // loop over Ncd z-points
  for(ncdz = initialz ; (ncdz > ncdz_bot)&&(ncdz < ncdz_top) ; ncdz+=increment){

    // ---------------------------------
    // Get time differences between reflected and direct paths
    
    // call GetDistancesToNcd to get the source-ncd path
    if(GetDistancesToNcd(qncd->GetIndex(),ncdz)){ // normal incidence assumed
      // if calculation successful, set distance and time
      fDsource_ncd = GetDd2o(); // distance in d2o
      fTsource_ncd = GetTimeOfFlight(); // time
    } else {
      fDsource_ncd = 0; fTsource_ncd = 0;
      continue;
    }
    
    // set Ncd vector
    TVector3 ncd(qncd->GetCenter().X(),qncd->GetCenter().Y(),ncdz);
    fNcd = ncd;
    // distances with straight lines
    fRsource_ncd = (fNcd - fSource).Mag(); // distance in d2o
    fTsource_ncd = fRsource_ncd/GetVgroupD2O();
    fRncd_pmt = (fPmt - fNcd).Mag();
    // associated time differences
    fTncd_pmt = (fRncd_pmt + GetNCDRadius())/GetVgroupD2O(); // approximation
    fTsource_ncd_pmt = fTsource_ncd + fTncd_pmt;
    fTdiff = fTsource_ncd_pmt - fTsource_pmt;
    // ---------------------------------
 
    if(fTdiff < fTwindow){
      if(ncdz < fNcdZmin) {fNcdZmin = ncdz; found_min++;}
      if(ncdz > fNcdZmax) {fNcdZmax = ncdz; found_max++;}
      
      // speed bump
      if((found_min > 1) && (found_max == 0)) ncdz+=(10-found_min)*increment; 
      if((found_max > 1) && (found_min == 0)) ncdz+=(10-found_max)*increment;
      
    } else if((found_min > 0)&&(found_max > 0)) break;
  }
  
  return;
}

//____________________________________________________________________
void QOCAReflect::ScanNCDLong(QNCD* qncd,Int_t pmtn)
{
  // Same as ScanNCD() but with better path calculations.

  Double_t ncdz; // working z
  Double_t ncdz_top = (qncd->GetTop()).Z();
  Double_t ncdz_bot = (qncd->GetBottom()).Z();

  // inverted extreme limits
  fNcdZmin = 999;
  fNcdZmax = -999;

  // various variables
  Int_t found_min = 0, found_max = 0; // counters for z-points
  Double_t initialz, increment;       // starting point and increment size

  // redefine starting point and increment sign 
  // depending on pmt z-coordinate 
  if(fPmt.Z() > 0){
    initialz = ncdz_top - 0.5*fStep_Z;
    increment = -fStep_Z;
  } else {
    initialz = ncdz_bot + 0.5*fStep_Z;
    increment = fStep_Z;
  }
  
  // loop over Ncd z-points
  for(ncdz = initialz ; (ncdz > ncdz_bot)&&(ncdz < ncdz_top) ; ncdz+=increment){

    // ---------------------------------
    // Get time differences between reflected and direct paths
    
    // call GetDistancesToNcd to get the source-ncd path
    if(GetDistancesToNcd(qncd->GetIndex(),ncdz)){ // normal incidence assumed
      // if calculation successful, set distance and time
      fDsource_ncd = GetDd2o(); // distance in d2o
      fTsource_ncd = GetTimeOfFlight(); // time
    } else {
      fDsource_ncd = 0; fTsource_ncd = 0;
      continue;
    }
    
    // then set the QOptics source there to calculate the additional path
    SetSource(qncd->GetCenter().X(),qncd->GetCenter().Y(),ncdz); // QOptics
    if(GetDistances(pmtn)){ // reset distances
      fDncd_pmt = GetDd2o() + GetDacrylic() + GetDh2o(); // distance in all media
      fTncd_pmt = GetTimeOfFlight() + GetNCDRadius()/GetVgroupD2O(); // time
    } else {
      fDncd_pmt = 0; fTncd_pmt = 0; fTdiff = 9999;
      continue;
    }
    
    // set total distances and time differences
    fDsource_ncd_pmt = fDsource_ncd + fDncd_pmt;
    fTsource_ncd_pmt = fTsource_ncd + fTncd_pmt;
    fDdiff = fDsource_ncd_pmt - fDsource_pmt;
    fTdiff = fTsource_ncd_pmt - fTsource_pmt;
    SetSource(fSource); // reset source
    // ---------------------------------
 
    if(fTdiff < fTwindow){
      if(ncdz < fNcdZmin) {fNcdZmin = ncdz; found_min++;}
      if(ncdz > fNcdZmax) {fNcdZmax = ncdz; found_max++;}
      
      // speed bump
      if((found_min > 1) && (found_max == 0)) ncdz+=(10-found_min)*increment; 
      if((found_max > 1) && (found_min == 0)) ncdz+=(10-found_max)*increment;
      
    } else if((found_min > 0)&&(found_max > 0)) break;
  }
  
  return;
}

//____________________________________________________________________
void QOCAReflect::Reflection(Int_t solution)
{
  //
  // Reflection:
  // - set distances and time of flight
  // - source inside the acrylic: the light bounces over the AV,
  //   travels additional distance in d2o, goes through AV
  //   and finally reaches the PMTs
  // - calculations come from both vectors defined in this class
  //   and QOptics functions
  // - called fNAvsolution time(s) per PMT (max = 2)
  //

  // reset bad pmt flag for reflection if first solution encountered
  if(solution == 0){SetBadPmt();}

  // find partial path distances and times for light going
  // from the source to the AV with vectors of this class
  // and given QOptics group velocity in d2o
  Double_t vd = GetVgroupD2O();
  SetSourcetoAvD(fRsource_av); // overwrite
  SetSourcetoAvT(fDsource_av/vd);

  // since GetDistances() have been called before
  // use the fake AV source to set distances and times
  // from the AV to the PMT
  SetAvtoPmtD(GetDd2o() + GetDacrylic() + GetDh2o());
  SetAvtoPmtT(GetTimeOfFlight());

  // compute total/differences values from what was set above
  // and from direct light path values set by Reflection()
  SetSourcetoAvtoPmtD(fDsource_av + fDav_pmt);
  SetSourcetoAvtoPmtT(fTsource_av + fTav_pmt);
  SetDDifference(fDsource_av_pmt - fDsource_pmt);
  SetTDifference(fTsource_av_pmt - fTsource_pmt); // sets fTdiff

  if(fDisplay){
    printf("Time difference = Direct - Reflected: fTdiff = %.4f (ns).\n",fTdiff);
  }

  // select data inside the time window (set before)
  // which potentially means prompt peak contamination
  if(fTdiff <= fTwindow){
    // if at least one reflection per PMT is below the window
    // activate the bad pmt flag
    SetBadPmt(kTRUE);

    // look for the limits of fCTheta_ps (new geometric cut region)
    // fCTheta_ps is set automatically when all vectors are known
    // before this call hopefully
    if(fCTheta_ps > fCTheta_ps_max){SetCThetapsMax(fCTheta_ps);}
    if(fCTheta_ps < fCTheta_ps_min){SetCThetapsMin(fCTheta_ps);}
  }

  // if the solution is the last one over all the possible ones
  // for that PMT (it could either 1 or 2)
  // just make sure to add this bad PMT to the count
  // if the time difference was below the full width window
  // for at least one of the possible reflections
  if(solution == fNAvsolution - 1){
    // adds a bad PMT to the total
    if(fBadpmt){AddaBadAvPmt();}
  }

  return;
}

//____________________________________________________________________
Bool_t QOCAReflect::SelectedPMT(Int_t pmtn)
{
  //
  // Verifying a single PMT status.
  // fSource must be set using SetSourceVector(TVector3 s)
  //
  // - calls GetDistances(pmtn) given a source position
  //   this function can be used as GetDistances(pmtn)
  //   to check if a selected PMT is valid
  // - sets the vector between the source and the pmt
  // - sets both pmt and source vectors
  // - fills transmission data
  //

  if(fDisplay > 1){
    printf("----------------------------------\n");
    printf("In QOCAReflect::SelectedPMT(Int_t)\n");
    printf("\tChecking PMT %.4d\n",pmtn);
  }

  // set (or reset) the z-axis source
  SetSource(fSource); // QOptics' source

  // verify if the light path is valid from source
  if(!GetDistances(pmtn)){return kFALSE;} 

  // find the vector between the source and the pmt position
  // the GetPMTPosition(pmtn) function returns a vector
  // from the center to the selected pmt
  SetPmtVector(GetPMTPosition(pmtn)); // sets fCTheta_ps
  SetSourcetoPmtVector(fPmt - fSource);

  // passed GetDistances(pmtn) : there is obviously transmission
  // fill this transmission info right away
  AddanAvTransmission(); // counter for valid transmissions
  Transmission();

  // at this point, coordinates have been rotated
  // on the z-axis, ready to look for AV reflections.

  if(fDisplay > 1){
    printf("\tSelectedPMT(%.4d) returned successfully.\n",pmtn);
  }

  return kTRUE;
}


//____________________________________________________________________
Bool_t QOCAReflect::TrackPMT(Int_t pmtn)
{
  // Verifying av reflection effects for a single PMT
  // for a single source position
  //
  // The couple (fRsource, fTheta_pmt) must be set
  // to find the numerical solution(s) alpha = beta,
  // the av point(s) where physical reflection will happen
  //
  // Calling GetAvSolutions() will :
  //   - call HowManySolutions() to set the fNAvsolution
  //   - create a vector with dimension fNAvsolution
  //   - get the analytic solution(s) of fAlphaAv and fTheta_av
  //
  // Calling ScanAv(pmtn) will :
  //   - look over a small interval around the av point (fTheta_av)
  //     and look if the pmtn is affected by av reflections
  //   - call Minimize()
  //
  // Calling Minimize() will :
  //   - minimize the difference alpha - beta
  //   - set the appropriate solution Av vector
  //

  // make sure the alpha function has been set before.
  // alpha only depends on the source position radius from the
  // center assuming it's oriented on the -ve z-axis
  fAlphaAv_func = (TF1*) GetAlphaAvTF1();

  // the source is reset to the actual position here.
  // the pmt vector is reset to the vector given
  // by GetPMTPosition(pmtn) and Transmission() is called.
  if(!SelectedPMT(pmtn)){return kFALSE;}

  // beta depends on the pmt vector if valid
  fBetaAv_func = (TF1*) GetBetaAvTF1(); // takes acos(fCTheta_ps)

  // set the transformation with both source and pmt vectors known
  *fRot = *fUnit; // reset first
  fRot->RotateAxes(fXp.Unit(), fYp.Unit(), fZp.Unit()); // set new rotation

  // set the upper limit on the av based on the thetaps value
  // the lower limit is always zero (source on the -ve z-axis)
  Double_t phi_av_hi = 2*TMath::Pi(); // default

  // fSelect = kTRUE: for low and high incidence reflections only (faster)
  // which means only reflections happening on the same side of the AV
  // the limit then is the pmt angular position
  if(fSelect){
    phi_av_hi = (Double_t) fBetaAv_func->GetParameter(0); // selected range
  }

  // find solution(s) and set the number of solution(s) in a general way
  // all the phiav[i] are given in radians
  Double_t step = ConverttoRad(1.0); // one degree step
  Double_t* phiav = GetAvSolutions(fAlphaAv_func, fBetaAv_func, 0.0, phi_av_hi, step);

  // no solution found: numerical error
  if(!fNAvsolution){
    Warning("TrackPMT","Lost PMT %.4d with cos(thetaps) of %.4f.",pmtn,fCTheta_ps);
    return kFALSE;
  }

  // in case all solutions are found
  if(fNAvsolution == 4){
      SetNAvSolution(3);
      fOops = kTRUE; // activate discarded solution flag
      AddanAvDiscarded(); // add one discarded solution
  }
  // in case Far and Unphysical solutions are found
  if(!fSelect && (fNAvsolution == 2)){
      SetNAvSolution(1);
      fOops = kTRUE; // activate discarded solution flag
      AddanAvDiscarded(); // add one discarded solution
  }

  // loop over the number of reflection solutions
  for(Int_t s = 0 ; s < fNAvsolution ; s++){

    SetAvRefleType(); // reset the reflection type

    if(phiav[s] != 0){ //avoid detector virtual bottom or empty solution

      // low and high incidence reflections
      if((phiav[s] > 0) && (phiav[s] <= TMath::Pi())){
        SetAvRefleType(1);
      }
      // others
      else{
        if(fSelect && (phiav[s] > phiav[s-1])){
	  SetAvRefleType(3); // unphysical reflection
	}
	else{
          SetAvRefleType(2); // far reflection
        }
      }

      // tolerance around solution
      SetPhiLimits(phiav[s] - fDelta_Phi, phiav[s] + fDelta_Phi);

      // scan the av to optimize the av point where the reflection
      // happens and fill the appropriate data for that solution s.
      if(!ScanAv(pmtn, s)){return kFALSE;}
    }
  }

  // delete newly created instances
  delete [] phiav;
  delete fBetaAv_func;
  delete fAlphaAv_func;

  return kTRUE;
}

//____________________________________________________________________
Bool_t QOCAReflect::ScanAv(Int_t pmtn, Int_t solution)
{
  //
  // Scanning an AV region for possible reflection solutions
  // (has to be set with SetThetaLimits(min, max) of
  // SetPhiLimits(min, max) before)
  //
  // should find an Av vector provided the values of phiav[s]
  //
  // - no limits on phi are needed since the phi components have been
  //   set equal to each other before (fPhi_av = fPhi_pmt) so
  //   that all vectors are in the same plane.
  //
  // Calling Minimize() will :
  //   - minimize the difference alpha - beta
  //   - set the appropriate solution Av vector
  //

  if(fDisplay){
    printf("----------------------------------\n");
    printf("From QOCAReflect::ScanAv(Int_t):\n");
    printf("About to start looping over AV points (solution %d/%d).\n",
	   (solution + 1),fNAvsolution);
  }

  if(!Minimize()){return kFALSE;}

  // Minimize() sets the solution av vector.
  // set other vectors that can be found here before filling.

  SetSourcetoAvVector(fAv - fSource); // sets solution alpha
  SetAvtoPmtVector(fPmt - fAv); // sets solution beta

  // display the angles found
  if(fDisplay){
    printf("Vector Summary: (R (cm), Theta (deg.), Phi (deg.))\n");
    printf("\t(TVector3) fPmt = (%5.1f,%10.2f,%10.2f)\n",
	   fPmt.Mag(),ConverttoDeg(fPmt.Theta()),ConverttoDeg(fPmt.Phi()));
    printf("\t(TVector3) fAv = (%5.1f,%10.2f,%10.2f)\n",
	   fAv.Mag(),ConverttoDeg(fAv.Theta()),ConverttoDeg(fAv.Phi()));
    printf("\t(TVector3) fSource_Av = (%5.1f,%10.2f,%10.2f)\n",
	   fSource_Av.Mag(),ConverttoDeg(fSource_Av.Theta()),
	   ConverttoDeg(fSource_Av.Phi()));
    printf("\t(TVector3) fAv_Pmt = (%5.1f,%10.2f,%10.2f)\n",
	   fAv_Pmt.Mag(),ConverttoDeg(fAv_Pmt.Theta()),
	   ConverttoDeg(fAv_Pmt.Phi()));
    printf("Incident and Reflection Angles:\n");
    printf("\tfAlphaAv = %5.1f (deg.) ",ConverttoDeg(fAlphaAv));
    if(fAvRefleType == 1){
      if(fAlphaAv > 0.7){printf("(High Incidence)\n");} // 40 deg.
      else{printf("(Low Incidence)\n");}
    }
    else{
      if(fAvRefleType == 2){printf("(Far)\n");}
      if(fAvRefleType == 3){printf("(Unphysical)\n");}
    }
    printf("\tfBetaAv = %5.1f (deg.)\n",ConverttoDeg(fBetaAv));
  }

  // set the fake source to that AV point
  SetSource(fAv); // QOptics' source

  // extra light path from reflected light:
  // from the AV point through d2o through AV to PMT.
  // if this extra light path is valid, fill histograms
  if(!GetDistances(pmtn)){return kFALSE;}

  AddanAvReflection(); // counter for valid reflections
  Reflection(solution);

  return kTRUE;
}

//____________________________________________________________________
Bool_t QOCAReflect::Minimize()
{
  // Function that finds the right Av vector that minimize
  // the (alpha - beta) difference given a PMT and a source vector
  //
  // - does not alter the pmt and source vectors
  // - sets and transforms the av vector
  //

  Double_t diff = 5.0; // difference = alpha - beta
  TVector2 bestav2vector(1,0); // to be asigned locally in 2-d
  TVector3 bestav3vector(1,0,0); // to be assigned at the end of that function
  Double_t sol1, sol2, bestsol;

  // reduce the steps progressively around the central phi value
  // to find a more precise solution given the tolerance fDelta_Refle
  sol1 = FindSolution(fAlphaAv_func, fBetaAv_func, fPhi_lo, fPhi_hi, fStep_Phi);
  if(sol1){
    diff = (fAlphaAv_func->Eval(sol1,0,0)) - (fBetaAv_func->Eval(sol1,0,0));
    if(diff < fDelta_Refle){bestsol = sol1;}
    else{
      sol2 = FindSolution(fAlphaAv_func, fBetaAv_func, sol1, sol1-fStep_Phi, 0.01*fStep_Phi);
      if(sol2){
        diff = (fAlphaAv_func->Eval(sol2,0,0)) - (fBetaAv_func->Eval(sol2,0,0));
      }
      if(diff < fDelta_Refle){bestsol = sol2;}
    }
  }
  else{bestsol = fPhi_lo + 0.5*(fPhi_hi-fPhi_lo);}

  // set the Av vector for the minimum found
  bestav2vector.SetMagPhi(fRav, bestsol);

  // check if the minimal difference found is greater
  // than the tolerance
  if(fDisplay){
    if(diff > fDelta_Refle){
      printf("----------------------------\n");
      printf("From QOCAReflect::Minimize()\n");
      printf("\tMinimal value out of precision!\n");
      printf("\t%.4f > %.4f\n",ConverttoDeg(diff),ConverttoDeg(fDelta_Refle));
    }
    else{
      printf("----------------------------\n");
      printf("From QOCAReflect::Minimize()\n\tSolution found!\n");
    }
  }

  // at this point the solution Av rotated vector was found
  // within the given tolerance (which must be small).
  // rotate the Av and pmt vectors back before leaving the function

  Double_t avtheta = 0.0, avphi = 0.0;

  if(fAvRefleType == 0){
    if(fDisplay){
      printf("----------------------------\n");
      printf("From QOCAReflect::Minimize()\n");
      printf("Bad reflection type: should not happen here.\n");
    }
    return kFALSE;
  }

  // set 3-d vectors coordinates defaults
  // low and high incidence reflections
  avphi = -0.5*TMath::Pi();
  avtheta = TMath::Pi() - bestav2vector.Phi();

  if(fAvRefleType == 2){ // far reflection
      // phi on opposite side of the pmt
      avphi += TMath::Pi();
      avtheta = bestav2vector.Phi() - TMath::Pi();
  }

  // set 3-d vector raw coordinates
  bestav3vector.SetMagThetaPhi(fRav-0.5, avtheta, avphi);

  // transformation
  bestav3vector *= *fRot;

  // set the solution
  SetAvVector(bestav3vector);

  return kTRUE;
}

//____________________________________________________________________
void QOCAReflect::TrackAllPMTs()
{
  // Verifying all possible av reflection effects over all PMTs
  // for a single source position like ScanAllPMTs
  // but uses only one single weight per PMT which
  // is more accurate and less time consuming
  // Source must be set using SetSourceVector(TVector3 s)
  //

  if(fDisplay){
    printf("------------------------------------\n");
    printf("* In QOCAReflect::TrackAllPMTs() ... \n");
  }
  // pmt loop
  for(Int_t pmtn = 1 ; pmtn < fNpmts ; pmtn++){TrackPMT(pmtn);}

  // Prints Summary
  printf("------------------------------------\n");
  printf("* From QOCAReflect::TrackAllPMTs():\n");
  printf("\tNumber of valid reflections: %d\n",fNAvrefle);
  printf("\tNumber of valid transmissions: %d\n",fNAvtrans);
  if(fNAvtrans != 9453){
    printf("\t\twas supposed to be 9453 (call SetNAvTransmission() to reset).\n");
  }
  Float_t perc = 100.0*((Float_t)fNAvnewcut/(Float_t)fNAvtrans);
  printf("\tNumber of bad PMTs (%.1f ns): %d (%.1f %%)\n",
	 fTwindow,fNAvnewcut,perc);
  printf("\tLimits on cos(thetaps): [%.4f, %.4f]\n",
	 fCTheta_ps_min,fCTheta_ps_max);
  if(fOops) printf("\tThere were %d discarded reflection solutions.\n",fNAvdiscard);
  printf("------------------------------------\n");

  return;

}

//____________________________________________________________________
Double_t* QOCAReflect::GetCThetapsRange()
{
  //
  // Main function to use in QOptics or QPath.
  // Wrapper for TrackAllPMTs().
  // Needs to be called once per source position per wavelength
  // which means it has to be called once per high radius run in a data set.
  //
  // It performs all the same tasks as TrackAllPMTs()
  // without all the histogram filling.
  //
  // Some calls are required prior to this function call:
  //   - Put an "if" statement to ensure R >= 450 cm.
  //   - QOCAReflect* qav = new QOCAReflect(); // constructor without histograms
  //   - qav->SetWavelength(wlen); // set the same wavelength
  //   - qav->SetSourceVector(source); // actual QOptics source
  //   - qav->SetTimeWindow(t); // timing window
  //   - Double_t* thps = qav->GetCThetapsRange(); // this function call
  //   - delete thps; // after usage
  //   - delete qav; // after usage
  //
  // It returns the limits on theta_ps wanted to make an appropriate
  // cut on high radius data with QOptics::IsPromptAVRefl().
  //
  // The actual values are cos(theta_ps) in the range [-1.0, 1.0]
  // as it is usually seen in the QOptics code (p.Unit() * s.Unit())
  //
  // thps[0] = theta_ps_min
  // thps[1] = theta_ps_max
  //

  Double_t* thps = new Double_t[2];

  if(fDisplay){
    printf("* In QOCAReflect::GetCThetapsRange().\n");
    printf("* Calculating Theta_ps limits for QOptics::IsPromptAVRefl().\n");
  }

  // looping step sizes can be controlled (theta, phi)
  SetStepSizes(ConverttoRad(0.1), ConverttoRad(0.1));
  // defines a range on phi/theta when ScanAv() is called
  SetPhiTol(ConverttoRad(0.5)); // 1 deg. interval
  SetThetaTol(ConverttoRad(0.5)); // 1 deg. interval
  // reflection tolerance has to be set for Minimize()
  SetRefleTol(ConverttoRad(0.05));


  TrackAllPMTs(); // summary is displayed after loop completion

  //
  // default (unchanged) values:
  // fCTheta_ps_min = 999.0
  // fCTheta_ps_max = -999.0
  // Note: QOptics has 9999.0 and -9999.0 as default values
  // which are NOT the same.
  //

  // check the lower limit and reset it to avoid rouding errors
  if(GetCThetapsMin() < -0.99){SetCThetapsMin(-1.0);}

  // assign the results to be returned
  thps[0] = (Double_t) GetCThetapsMin();
  thps[1] = (Double_t) GetCThetapsMax();

  return thps;

}


