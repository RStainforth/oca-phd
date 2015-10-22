#ifndef _QOCAReflect_
#define _QOCAReflect_

//*-- Author : Olivier Simard
//*-- Copyright (C) 2004 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCAReflect                                                          //
//                                                                      //
// Methods to study Optical Reflections:                                //
//                                                                      //
//  - AV  Reflection Cuts at high radius                                //
//  - NCD Reflection Corrections at any position                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TFile.h"
#include "TMath.h"
#include "TVector3.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "QOptics.h"

class QOCAReflect;
static QOCAReflect* gQOCAReflect;

// Special functions for TF1.
Double_t QOCAReflect_external_AlphaAv(Double_t*, Double_t*);
Double_t QOCAReflect_external_BetaAv(Double_t*, Double_t*);

class QOCAReflect : public QOptics {

 private:

  // 3-d vectors
  TVector3 fSource; 		// from the center to the source
  TVector3 fPmt; 		// from the center to a PMT
  TVector3 fAv; 		// from the center to an AV point
  TVector3 fNcd;		// from the center to a NCD point
  TVector3 fSource_Av; 		// from the source to an AV point
  TVector3 fAv_Pmt; 		// from an AV point to a PMT
  TVector3 fSource_Pmt; 	// from the source to a PMT
  TVector3 fSource_Ncd; 	// from the source to a NCD point
  TVector3 fNcd_Pmt;	 	// from a NCD point to a PMT
  TVector3 fXp, fYp, fZp;       // new coordinate system

  // magnitudes
  Double_t fRsource; 		// source radius from center
  Double_t fZsource; 		// source z-component
  Double_t fRav; 		// acrylic vessel inner radius from center
  Double_t fRpsup; 		// PSUP distance from center
  // from vectors (R)
  Double_t fRsource_pmt; 	// direct distance from the source to a PMT
  Double_t fRsource_av; 	// direct distance from the source to an AV point
  Double_t fRav_pmt; 		// direct distance from an AV point to a PMT
  Double_t fRsource_av_pmt; 	// reflected path
  Double_t fRsource_ncd;	// direct distance from the source to a NCD point
  Double_t fRncd_pmt;		// direct distance from a NCD point to a PMT
  Double_t fRsource_ncd_pmt; 	// reflected path
  Double_t fRdiff; 		// difference of paths
  // from QOptics (D)
  Double_t fDsource_pmt; 	// direct distance from the source to a PMT
  Double_t fDsource_av; 	// direct distance from the source to an AV point
  Double_t fDav_pmt; 		// direct distance from an AV point to a PMT
  Double_t fDsource_av_pmt; 	// reflected path
  Double_t fDsource_ncd;	// direct distance from the source to a NCD point
  Double_t fDncd_pmt;		// direct distance from a NCD point to a PMT
  Double_t fDsource_ncd_pmt; 	// reflected path
  Double_t fDdiff; 		// difference of paths

  // time of flight (T)
  Double_t fTsource_pmt; 	// from the source to a PMT
  Double_t fTsource_av; 	// from the source to an AV point
  Double_t fTav_pmt; 		// from an AV point to a PMT
  Double_t fTsource_av_pmt; 	// reflected path
  Double_t fTsource_ncd;	// from the source to a NCD point
  Double_t fTncd_pmt;		// from a NCD point to a PMT
  Double_t fTsource_ncd_pmt; 	// reflected path
  Double_t fTdiff; 		// difference of paths

  // -----------------

  // angles (radians)
  Double_t fTheta_lo, fTheta_hi;	// limits
  Double_t fPhi_lo, fPhi_hi; 		// limits
  Double_t fAlphaAv; 		// incident angle on the AV surface
  Double_t fBetaAv; 		// reflection angle from the AV surface

  // spherical coordinates
  Double_t fTheta_source, fPhi_source; 		// of the source
  Double_t fTheta_pmt, fPhi_pmt; 		// of a PMT
  Double_t fTheta_av, fPhi_av;			// of an AV point

  // rotation angles about associated axes from -ve z-axis
  TRotation* fRot, *fUnit;

  // angle between source vector and pmt vector (scalar product)
  Double_t fCTheta_ps;
  Double_t fCTheta_ps_max, fCTheta_ps_min; // associated limits

  // -----------------

  // tolerances
  Double_t fStep_Theta; 	// Theta step size
  Double_t fStep_Phi; 		// Phi step size
  Double_t fStep_Z;		// Z step size
  Double_t fDelta_Theta; 	// tolerance on a Theta value
  Double_t fDelta_Phi; 		// tolerance on a Phi value
  Double_t fDelta_Plane; 	// tolerance for plane vectors
  Double_t fDelta_Refle;	// tolerance for reflection acceptance

  // counters
  Int_t fNAvsolution; 		// counts the number of AV reflections
  				// for a single PMT
  Int_t fNAvrefle; 		// counts valid AV reflections
  Int_t fNAvtrans; 		// counts valid AV transmissions
  Int_t fNAvnewcut; 		// number of PMT discarded due to AV reflections
  Int_t fNAvdiscard; 		// counts discarded solutions based on tolerance value
  Int_t fNNcdrefle; 		// counts valid NCD reflections

  // histograms and functions
  TF1* fAlphaAv_func;		// analytic model AV incident angle
  TF1* fBetaAv_func; 		// analytic model AV reflection angle

  // others
  Double_t fConvert;		// radians/degrees conversion factor
  Int_t fDisplay; 		// degree of message display
  Int_t fAvRefleType;		// AV reflection type
  				// 0: none, 1: low or high, 2:far, 3:unphysical, 4:unknown
  Bool_t fBadpmt; 		// flag for bad pmts
  Bool_t fOops; 		// flag for bad solutions
  Bool_t fSelect;		// flag for selected solutions
  				// set to kTRUE to speed up running time

  Double_t fTwindow; 		// timing window width

  Double_t fNcdZmin;            // Ncd minimal Z value allowed
  Double_t fNcdZmax;            // Ncd maximal Z value allowed

 public:

  // Constructor and Destructor
  QOCAReflect();
  ~QOCAReflect();

  Int_t fNpmts; 		// raw number of pmts (~10000)

  // ----------------------------------------
  // Setters
  // note: some functions' names are explicit
  // to differentiate from QOptics member functions

  Bool_t SetSourceVector(TVector3);
  void SetPmtVector(TVector3);
  void SetAvVector(TVector3);
  void SetNcdVector(TVector3);
  void SetSourcetoPmtVector(TVector3);
  void SetSourcetoAvVector(TVector3);
  void SetAvtoPmtVector(TVector3);
  void SetSourcetoNcdVector(TVector3);
  void SetNcdtoPmtVector(TVector3);

  void SetSourceR(Double_t rs = 0.0){fRsource = rs;}
  void SetSourceZ(Double_t z = 0.0){fZsource = z;}
  void SetPmtR(Double_t rp = 0.0){fRpsup = rp;}
  void SetAvR(Double_t ra = 0.0){fRav = ra;}
  void SetSourcetoPmtR(Double_t rsp = 0.0){fRsource_pmt = rsp;}
  void SetSourcetoAvR(Double_t rsa = 0.0){fRsource_av = rsa;}
  void SetAvtoPmtR(Double_t rap = 0.0){fRav_pmt = rap;}
  void SetSourcetoAvtoPmtR(Double_t rsap = 0.0){fRsource_av_pmt = rsap;}
  void SetSourcetoNcdR(Double_t rsn = 0.0){fRsource_ncd = rsn;}
  void SetNcdtoPmtR(Double_t rnp = 0.0){fRncd_pmt = rnp;}
  void SetSourcetoNcdtoPmtR(Double_t rsnp = 0.0){fRsource_ncd_pmt = rsnp;}
  void SetRDifference(Double_t rdiff = 0.0){fRdiff = rdiff;}
  void SetSourcetoPmtD(Double_t dsp = 0.0){fDsource_pmt = dsp;}
  void SetSourcetoAvD(Double_t dsa = 0.0){fDsource_av = dsa;}
  void SetAvtoPmtD(Double_t dap = 0.0){fDav_pmt = dap;}
  void SetSourcetoAvtoPmtD(Double_t dsap = 0.0){fDsource_av_pmt = dsap;}
  void SetSourcetoNcdD(Double_t dsn = 0.0){fDsource_ncd = dsn;}
  void SetNcdtoPmtD(Double_t dnp = 0.0){fDncd_pmt = dnp;}
  void SetSourcetoNcdtoPmtD(Double_t dsnp = 0.0){fDsource_ncd_pmt = dsnp;}
  void SetDDifference(Double_t ddiff = 0.0){fDdiff = ddiff;}

  void SetSourcetoPmtT(Double_t tsp = 0.0){fTsource_pmt = tsp;}
  void SetSourcetoAvT(Double_t tsa = 0.0){fTsource_av = tsa;}
  void SetAvtoPmtT(Double_t tap = 0.0){fTav_pmt = tap;}
  void SetSourcetoAvtoPmtT(Double_t tsap = 0.0){fTsource_av_pmt = tsap;}
  void SetSourcetoNcdT(Double_t tsn = 0.0){fTsource_ncd = tsn;}
  void SetNcdtoPmtT(Double_t tnp = 0.0){fTncd_pmt = tnp;}
  void SetSourcetoNcdtoPmtT(Double_t tsnp = 0.0){fTsource_ncd_pmt = tsnp;}
  void SetTDifference(Double_t tdiff = 0.0){fTdiff = tdiff;}

  void SetThetaLimits(Double_t theta_lo = 0.0, Double_t theta_hi = TMath::Pi())
    {fTheta_lo = theta_lo; fTheta_hi = theta_hi;}
  void SetPhiLimits(Double_t phi_lo = -TMath::Pi(), Double_t phi_hi = TMath::Pi())
    {fPhi_lo = phi_lo; fPhi_hi = phi_hi;}
  void SetThetaSource(Double_t theta_source = 0.0){fTheta_source = theta_source;}
  void SetThetaPmt(Double_t theta_pmt = 0.0){fTheta_pmt = theta_pmt;}
  void SetThetaAv(Double_t theta_av = 0.0){fTheta_av = theta_av;}
  void SetPhiSource(Double_t phi_source = 0.0){fPhi_source = phi_source;}
  void SetPhiPmt(Double_t phi_pmt = 0.0){fPhi_pmt = phi_pmt;}
  void SetPhiAv(Double_t phi_av = 0.0){fPhi_av = phi_av;}

  void SetAlphaAv(Double_t alpha = 0.0){fAlphaAv = alpha;}
  void SetBetaAv(Double_t beta = 0.0){fBetaAv = beta;}
  void SetCThetaps(Double_t thetaps = 0.0){fCTheta_ps = thetaps;}
  void SetCThetapsMax(Double_t thps_max = -999.){fCTheta_ps_max = thps_max;}
  void SetCThetapsMin(Double_t thps_min = 999.){fCTheta_ps_min = thps_min;}
  void SetCThetapsRange(Double_t thps_min = 999., Double_t thps_max = -999.)
    {fCTheta_ps_min = thps_min; fCTheta_ps_max = thps_max;}

  void SetStepSizes(Double_t step_th = 0.0, Double_t step_ph = 0.0)
    {fStep_Theta = step_th; fStep_Phi = step_ph;}
  void SetStepZ(Double_t stepz = 0.0){fStep_Z = stepz;}
  void SetThetaTol(Double_t thetatol = 0.0){fDelta_Theta = thetatol;}
  void SetPhiTol(Double_t phitol = 0.0){fDelta_Phi = phitol;}
  void SetPlaneTol(Double_t planetol = 0.0){fDelta_Plane = planetol;}
  void SetRefleTol(Double_t refletol = 0.0){fDelta_Refle = refletol;}

  void SetNAvReflection(Int_t nr = 0){fNAvrefle = nr;}
  void SetNAvTransmission(Int_t nt = 0){fNAvtrans = nt;}
  void SetNAvSolution(Int_t nsol = 0){fNAvsolution = nsol;}
  void SetNAvNewCut(Int_t nc = 0){fNAvnewcut = nc;}
  void SetNAvDiscarded(Int_t dis = 0){fNAvdiscard = dis;}
  void SetNNcdReflection(Int_t nr = 0){fNNcdrefle = nr;}
  void SetDisplay(Int_t d = 0){fDisplay = d;}
  void SetAvRefleType(Int_t type = 0){fAvRefleType = type;}
  void SetBadPmt(Bool_t bad = kFALSE){fBadpmt = bad;}
  void SetOops(Bool_t oops = kFALSE){fOops = oops;}
  void SetSelect(Bool_t sel = kFALSE){fSelect = sel;}
  void SetTimeWindow(Double_t tw = 10.0);

  void SetNcdZMin(Double_t z = -999.){fNcdZmin = z;}
  void SetNcdZMax(Double_t z =  999.){fNcdZmax = z;}

  // ------
  // some re-setters (resets to default values)
  void ResetAllRs();
  void ResetAllDs();
  void ResetAllTs();
  void ResetAllAngles();
  void ResetAllTols();
  void ResetCounters();

  // ----------------------------------------
  // Getters
  TVector3 GetSourceVector(){return fSource;}
  TVector3 GetPmtVector(){return fPmt;}
  TVector3 GetAvVector(){return fAv;}
  TVector3 GetNcdVector(){return fNcd;}
  TVector3 GetSourcetoPmtVector(){return fSource_Av;}
  TVector3 GetSourcetoAvVector(){return fAv_Pmt;}
  TVector3 GetAvtoPmtVector(){return fSource_Pmt;}
  TVector3 GetSourcetoNcdVector(){return fSource_Ncd;}
  TVector3 GetNcdtoPmtVector(){return fNcd_Pmt;}

  Double_t GetSourceR(){return fRsource;}
  Double_t GetSourceZ(){return fZsource;}
  Double_t GetPmtR(){return fRpsup;}
  Double_t GetAvR(){return fRav;}
  Double_t GetSourcetoPmtR(){return fRsource_av;}
  Double_t GetSourcetoAvR(){return fRav_pmt;}
  Double_t GetAvtoPmtR(){return fRsource_pmt;}
  Double_t GetSourcetoAvtoPmtR(){return fRsource_av_pmt;}
  Double_t GetSourcetoNcdR(){return fRsource_ncd;}
  Double_t GetNcdtoPmtR(){return fRncd_pmt;}
  Double_t GetSourcetoNcdtoPmtR(){return fRsource_ncd_pmt;}
  Double_t GetSourcetoPmtD(){return fDsource_pmt;}
  Double_t GetSourcetoAvD(){return fDsource_av;}
  Double_t GetAvtoPmtD(){return fDav_pmt;}
  Double_t GetSourcetoAvtoPmtD(){return fDsource_av_pmt;}
  Double_t GetSourcetoNcdD(){return fDsource_ncd;}
  Double_t GetNcdtoPmtD(){return fDncd_pmt;}
  Double_t GetSourcetoNcdtoPmtD(){return fDsource_ncd_pmt;}

  Double_t GetSourcetoPmtT(){return fTsource_pmt;}
  Double_t GetSourcetoAvT(){return fTsource_av;}
  Double_t GetAvtoPmtT(){return fTav_pmt;}
  Double_t GetSourcetoAvtoPmtT(){return fTsource_av_pmt;}
  Double_t GetSourcetoNcdT(){return fTsource_ncd;}
  Double_t GetNcdtoPmtT(){return fTncd_pmt;}
  Double_t GetSourcetoNcdtoPmtT(){return fTsource_ncd_pmt;}

  Double_t GetThetaSource(){return fTheta_source;}
  Double_t GetThetaPmt(){return fTheta_pmt;}
  Double_t GetThetaAv(){return fTheta_av;}
  Double_t GetPhiSource(){return fPhi_source;}
  Double_t GetPhiPmt(){return fPhi_pmt;}
  Double_t GetPhiAv(){return fPhi_av;}

  Double_t GetAlphaAv(){return fAlphaAv;}
  Double_t GetBetaAv(){return fBetaAv;}
  Double_t GetCThetaps(){return fCTheta_ps;}
  Double_t GetCThetapsMax(){return fCTheta_ps_max;}
  Double_t GetCThetapsMin(){return fCTheta_ps_min;}

  Double_t GetStepSizeTheta(){return fStep_Theta;}
  Double_t GetStepSizePhi(){return fStep_Phi;}
  Double_t GetStepSizeZ(){return fStep_Z;}
  Double_t GetThetaTol(){return fDelta_Theta;}
  Double_t GetPhiTol(){return fDelta_Phi;}
  Double_t GetPlaneTol(){return fDelta_Plane;}
  Double_t GetRefleTol(){return fDelta_Refle;}

  Int_t GetNAvReflection(){return fNAvrefle;}
  Int_t GetNAvTransmission(){return fNAvtrans;}
  Int_t GetNAvSolution(){return fNAvsolution;}
  Int_t GetNAvNewCut(){return fNAvnewcut;}
  Int_t GetNAvDiscarded(){return fNAvdiscard;}
  Int_t GetNNcdReflection(){return fNNcdrefle;}
  Double_t GetTimeWindow(){return fTwindow;}
  Int_t GetAvRefleType(){return fAvRefleType;}

  Double_t GetNcdZMin(){return fNcdZmin;}
  Double_t GetNcdZMax(){return fNcdZmax;}

  // ----------------------------------------
  // Other functions

  // ------
  // conversion and range checking
  Double_t ConverttoDeg(Double_t rad = 0.0){return rad/fConvert;}
  Double_t ConverttoRad(Double_t deg = 0.0){return fConvert*deg;}
  Bool_t IsInRange(Double_t);
  Bool_t IsInRange(Double_t, Double_t, Double_t);

  // ------
  // analytic functions and solutions for AV reflections
  TF1* GetAlphaAvTF1();
  TF1* GetBetaAvTF1();
  Double_t AlphaAv(Double_t*, Double_t*);
  Double_t BetaAv(Double_t*, Double_t*);
  Double_t FindSolution(TF1*, TF1*, Double_t, Double_t, Double_t);
  Int_t GetNAvSolutions(TF1*, TF1*, Double_t, Double_t, Double_t);
  Double_t* GetAvSolutions(TF1*, TF1*, Double_t, Double_t, Double_t);

  // ------
  // adders for counters
  void AddanAvReflection(){fNAvrefle++;}
  void AddanAvTransmission(){fNAvtrans++;}
  void AddaBadAvPmt(){fNAvnewcut++;}
  void AddanAvDiscarded(){fNAvdiscard++;}
  void AddaNcdReflection(){fNNcdrefle++;}

  // ------
  // checkers for AV transmission and reflection
  void Transmission();
  void Reflection(Int_t);
  Bool_t TrackPMT(Int_t pmtn);
  Bool_t SelectedPMT(Int_t pmtn);
  Bool_t ScanAv(Int_t, Int_t);
  void TrackAllPMTs();
  Bool_t Minimize();
  Double_t* GetCThetapsRange();

  // ------
  // checkers for NCD reflections
  TH1D* NCDReflection();
  Double_t GetNCDReflectionFactor(Int_t pmtn);
  Bool_t IsNCDReflection(QNCD*,Int_t pmtn);
  void ScanNCD(QNCD*);
  void ScanNCDLong(QNCD*,Int_t pmtn);

  ClassDef(QOCAReflect,0)    // QOCA Optics Reflection Class
};

#endif
