#ifndef __QRdt_h__
#define __QRdt_h__

//*-- Author : Ranpal Dosanjh
//*-- Copyright (C) 2003 CodeSNO. All rights reserved.

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QRdt                                                                 //
//                                                                      //
// Loads .rdt files for use in Qpath, etc.                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TVector3.h>
#include <TVector.h>
#include <TVectorF.h>

#include "QMath.h"
#include <fstream>

// ------------------------------------------------------------------------
//  Path class for optics definition
// ------------------------------------------------------------------------

// Global pointer to QPath object
class QRdt;

class QRdt: public TObject {

 private:

  TString   fDefaultTString;
  TVector3 *fDefaultTVector3;
  Int_t     fPassMaximum;

  // ----------------------------
  // To get the .rdt file
  // ----------------------------
  Int_t   fRdtRunnumber;	// run number for .rdt file
  TString fRdtdir;		// .rdt directory
  TString fRdtfilename;		// .rdt filename
  TString fRdtfullname;		// .rdt directory and filename

  Int_t fRdtstyle;		// .rdt file name style
  Int_t fRdtpass;		// .rdt pass number

  void    ConstructRdtfullname();
  TString BuildRdtfullname(TString dirname, TString filename);

  void    ConstructRdtfilename();
  TString FindRdtfileStyle();
  TString FindRdtfilePass(Int_t style);
  TString BuildRdtfilename(Int_t style, Int_t pass);

  Bool_t IsReallyFilled(TString varname = "");

  // ----------------------------
  // From the .rdt file
  // ----------------------------
  Bool_t  fRdtLoaded;		// file was successfully loaded

  TString *fLogFields;		// Fields from the log file

  Int_t   fJulianDayStart;	// SNO Julian day (start)
  Int_t   fJulianDayStop;	// SNO Julian day (stop)
  Int_t   fUT1Start;		// SNO (universal?) time (up to seconds) (start)
  Int_t   fUT1Stop;		// SNO (universal?) time (up to seconds) (stop)
  Int_t   fUT2Start;		// SNO (universal?) time (fraction of s in ns) (start)
  Int_t   fUT2Stop;		// SNO (universal?) time (fraction of s in ns) (stop)

  Int_t   fNpulses;		// Number of pulses (from occupancy hist)

  Int_t   fDyecell;		// Dye cell number
  Float_t fLambda;		// Wavelength
  Float_t fOrientation;		// Orientation 

  TVectorF *fLambdaTVector;	// Wavelength vector (from file)
  TVectorF *fOrientationTVector;	// Orientation vector (from file)
  TVector  *fRSPS; // Vector with rsps flags (from file) 

  TVector3 *fManipPos;		// Position from manipulator
  TVector3 *fReflectPos;	// Position from reflection technique
  TVector3 *fDirectFitPos;	// Position from QPath direct path fit
  TVector3 *fFullFitPos;	// Position from QPath full fit

  TH1F *fTimeResidual;		// Normalized time residual
  TH1F *fTimePeak;		// Time peak
  TH1F *fTimeWidth;		// Time Width
  TH1F *fRchToF;		// ToF used in .rch file
  TH1F *fToF;			// Time of flight
  TH2F *fTimeZ;			// Time vs source z
  TH2F *fTimeToF;		// Time vs time of flight
  TH2F *fTimeNbins;		// N of bins in time search
  
  TH1F *fOccupancy;		// Occupancy (counts)
  TH1F *fOccWide;		// Occupancy in enlarged window
  TH2F *fOccWindowPmt;		// Occupancy (window width vs pmt)
  TH2F *fOccWindowPmtz;		// Occupancy (window width vs pmt z)
  TH2F *fNormWindowPmtz;	// PMT norm (window width vs pmt z)
  TH2F *fOccZ;			// Occupancy vs source z
  TH2F *fOccZNopipe;		// Occupancy vs source z (no water pipe)

  TH2F *fOccThetaPhi;		// Occupancy (theta vs phi)
  TH2F *fOccThetaPhi2;		// Occupancy (theta vs phi) (?)
  TH2F *fOccAThetaPhi;		// Occupancy (acrylic theta vs phi)
  TH2F *fOccLThetaPhi;		// Occupancy (laserball theta vs phi)
  TH2F *fNpmtLThetaPhi;		// Number of pmts (laserball theta vs phi)

  TH1F *fOccRayleigh;		// Occupancy from Rayleigh scattered photons (MC, prompt)
  TH1F *fOccDirect;		// Occupancy from direct photons (MC, prompt)
  TH1F *fOccReflected;		// Occupancy from reflected photons (MC, prompt)
  TH1F *fOccRayleighTotal;	// Occupancy from Rayleigh scattered photons (MC, total)
  TH1F *fOccDirectTotal;	// Occupancy from direct photons (MC, total)
  TH1F *fOccReflectedTotal;	// Occupancy from reflected photons (MC, total)

  void SetHisto(TH1** histoptr, TH1* argptr);
  TH1 *GetHisto(TH1 *histoptr);
  Stat_t GetContentByBin(TH1 *histoptr, Int_t  binx);
  Stat_t GetContentByBin(TH1 *histoptr, Int_t  binx, Int_t  biny);
  Stat_t GetContentByBin(TH1 *histoptr, Int_t  binx, Int_t  biny, Int_t  binz);
  Stat_t GetContentByVal(TH1 *histoptr, Axis_t valx);
  Stat_t GetContentByVal(TH1 *histoptr, Axis_t valx, Axis_t valy);
  Stat_t GetContentByVal(TH1 *histoptr, Axis_t valx, Axis_t valy, Axis_t valz);

  void SetRdtLoaded(Bool_t rdtloaded = kFALSE)			{fRdtLoaded = rdtloaded;}

  void SetJulianDayStart(Int_t juliandate=-1)			{fJulianDayStart = juliandate;}
  void SetJulianDayStop(Int_t juliandate=-1)			{fJulianDayStop = juliandate;}
  void SetUT1Start(Int_t ut1=-1)				{fUT1Start= ut1;}
  void SetUT1Stop(Int_t ut1=-1)					{fUT1Stop= ut1;}
  void SetUT2Start(Int_t ut2=-1)				{fUT2Start = ut2;}
  void SetUT2Stop(Int_t ut2=-1)					{fUT2Stop = ut2;}

  void SetNpulses(Int_t npulses=-1)				{fNpulses = npulses;}

  void SetDyecell(Int_t dyecell=-1)				{fDyecell = dyecell;}
  void SetLambda(Float_t lambda=-1)				{fLambda = lambda;}
  void SetOrientation(Float_t orientation=-1)			{fOrientation = orientation;}

  void SetLambdaTVector(TVectorF *lambdatvector=NULL)		{fLambdaTVector = lambdatvector;}
  void SetOrientationTVector(TVectorF *orientationtvector=NULL)	{fOrientationTVector = orientationtvector;} 

  void SetManipPos(TVector3 *manippos=NULL)			{fManipPos = manippos;}
  void SetReflectPos(TVector3 *reflectpos=NULL)			{fReflectPos = reflectpos;}
  void SetDirectFitPos(TVector3 *directfitpos=NULL)  		{fDirectFitPos = directfitpos;}
  void SetFullFitPos(TVector3 *fullfitpos=NULL)			{fFullFitPos = fullfitpos;}

  void SetTimeResidual(TH1F *timeresidual=NULL)	{SetHisto((TH1**)&fTimeResidual,timeresidual);}
  void SetTimePeak(TH1F *timepeak=NULL)		{SetHisto((TH1**)&fTimePeak,timepeak);}
  void SetTimeWidth(TH1F *timewidth=NULL)	{SetHisto((TH1**)&fTimeWidth,timewidth);}
  void SetRchToF(TH1F *rchtof=NULL)		{SetHisto((TH1**)&fRchToF,rchtof);}
  void SetToF(TH1F *tof=NULL)			{SetHisto((TH1**)&fToF,tof);}
  void SetTimeZ(TH2F *timez=NULL)		{SetHisto((TH1**)&fTimeZ,timez);}
  void SetTimeToF(TH2F *timetof=NULL)		{SetHisto((TH1**)&fTimeToF,timetof);}
  void SetTimeNbins(TH1F *timenbins=NULL)	{SetHisto((TH1**)&fTimeNbins,timenbins);}
  
  void SetOccupancy(TH1F *occupancy=NULL)	{SetHisto((TH1**)&fOccupancy,occupancy);}
  void SetOccWide(TH1F *occwide=NULL)		{SetHisto((TH1**)&fOccWide,occwide);}
  void SetOccWindowPmt(TH2F *occwindowpmt=NULL)	{SetHisto((TH1**)&fOccWindowPmt,occwindowpmt);}
  void SetOccWindowPmtz(TH2F *occwindowpmtz=NULL)   {SetHisto((TH1**)&fOccWindowPmtz,occwindowpmtz);}
  void SetNormWindowPmtz(TH2F *normwindowpmtz=NULL) {SetHisto((TH1**)&fNormWindowPmtz,normwindowpmtz);}
  void SetOccZ(TH2F *occz=NULL)			{SetHisto((TH1**)&fOccZ,occz);}
  void SetOccZNopipe(TH2F *occznopipe=NULL)	{SetHisto((TH1**)&fOccZNopipe,occznopipe);}
  
  void SetOccThetaPhi(TH2F *occthetaphi=NULL)	{SetHisto((TH1**)&fOccThetaPhi,occthetaphi);}
  void SetOccThetaPhi2(TH2F *occthetaphi2=NULL)	{SetHisto((TH1**)&fOccThetaPhi2,occthetaphi2);}
  void SetOccAThetaPhi(TH2F *occathetaphi=NULL)	{SetHisto((TH1**)&fOccAThetaPhi,occathetaphi);}
  void SetOccLThetaPhi(TH2F *occlthetaphi=NULL)	{SetHisto((TH1**)&fOccLThetaPhi,occlthetaphi);}
  void SetNpmtLThetaPhi(TH2F *npmtlthetaphi=NULL) {SetHisto((TH1**)&fNpmtLThetaPhi,npmtlthetaphi);}
  
  void SetOccRayleigh(TH1F *occrayleigh=NULL) 	{SetHisto((TH1**)&fOccRayleigh,occrayleigh);}
  void SetOccDirect(TH1F *occdirect=NULL) 	{SetHisto((TH1**)&fOccDirect,occdirect);}
  void SetOccReflected(TH1F *occreflected=NULL) {SetHisto((TH1**)&fOccReflected,occreflected);}
  void SetOccRayleighTotal(TH1F *occrayleightotal=NULL) {SetHisto((TH1**)&fOccRayleighTotal,occrayleightotal);}
  void SetOccDirectTotal(TH1F *occdirecttotal=NULL) {SetHisto((TH1**)&fOccDirectTotal,occdirecttotal);}
  void SetOccReflectedTotal(TH1F *occreflectedtotal=NULL) {SetHisto((TH1**)&fOccReflectedTotal,occreflectedtotal);}

  void LoadDate(TFile *rdtfile);
  void InputDateFromLog(Int_t &jdstart, Int_t &jdstop, 
			Int_t &ut1start, Int_t &ut1stop, 
			Int_t &ut2start, Int_t &ut2stop);
  Bool_t InputLog();
  Bool_t FillLogFields(TString fnam, Int_t rdtrunnumber);


  void LoadLambda(TFile *rdtfile);
  void LoadOrientation(TFile *rdtfile);
  void LoadRSPS(TFile *rdtfile);
  void LoadPosition(TFile *rdtfile);

  TVector3 *ReadPosition(TFile *rdtfile, TString name);
  TVector3 *ReadOldPosition(TFile *rdtfile, TString name);

 public:
  QRdt(TString fullname = "");
  ~QRdt();
  void Initialize();
  void Close();

  // ----------------------------
  // To get the .rdt file
  // ----------------------------

  void    SetRdtRunnumber(Int_t runnumber = 0)			{fRdtRunnumber = runnumber;}
  Int_t   GetRdtRunnumber()					{return fRdtRunnumber;}
  void    SetRdtdir(TString dirname = "")			{fRdtdir = dirname;}
  TString GetRdtdir()  						{return fRdtdir;}
  void    SetRdtfilename(TString filename = "")			{fRdtfilename = filename;}
  TString GetRdtfilename()					{return fRdtfilename;}
  void    SetRdtfullname(TString fullname = "")			{fRdtfullname = fullname;}
  TString GetRdtfullname()					{return fRdtfullname;}
  
  void  SetRdtstyle(Int_t style = -1)				{fRdtstyle = style;}
  Int_t GetRdtstyle()						{return fRdtstyle;}
  void  SetRdtpass(Int_t pass = -1)				{fRdtpass = pass;}
  Int_t GetRdtpass()						{return fRdtpass;}

  void LoadRdtFile();
  void LoadRdtFile(Int_t runnumber);
  void LoadRdtFile(TString fullname);
 
  TH1F  *TH1D_to_TH1F(const TH1D *hin);
  Bool_t IsFilled(TString varname = "");
  Bool_t FileExists(TString fullname = "");

  // ----------------------------
  // From the .rdt file
  // ----------------------------

  Bool_t GetRdtLoaded()		{return fRdtLoaded;}

  Int_t GetJulianDayStart()	{return fJulianDayStart;}
  Int_t GetJulianDayStop()	{return fJulianDayStop;}
  Int_t GetUT1Start()		{return fUT1Start;}
  Int_t GetUT1Stop()		{return fUT1Stop;}
  Int_t GetUT2Start()		{return fUT2Start;}
  Int_t GetUT2Stop()		{return fUT2Stop;}

  Int_t GetNpulses()		{return fNpulses;}

  Int_t   GetDyecell()		{return fDyecell;}
  Float_t GetLambda()		{return fLambda;}
  Float_t GetOrientation()	{return fOrientation;}
  TVector *GetRSPS()	    {return fRSPS;}

  TVector3 *GetManipPos() 	{return fManipPos;}
  TVector3 *GetReflectPos()	{return fReflectPos;}
  TVector3 *GetDirectFitPos()	{return fDirectFitPos;}
  TVector3 *GetFullFitPos()	{return fFullFitPos;}

  TH1F *GetTimeResidual()	{return dynamic_cast<TH1F *>(GetHisto(fTimeResidual));}
  TH1F *GetTimePeak()		{return dynamic_cast<TH1F *>(GetHisto(fTimePeak));}
  TH1F *GetTimeWidth()		{return dynamic_cast<TH1F *>(GetHisto(fTimeWidth));}
  TH1F *GetRchToF()		{return dynamic_cast<TH1F *>(GetHisto(fRchToF));}
  TH1F *GetToF()		{return dynamic_cast<TH1F *>(GetHisto(fToF));}
  TH2F *GetTimeZ()		{return dynamic_cast<TH2F *>(GetHisto(fTimeZ));}
  TH2F *GetTimeToF()		{return dynamic_cast<TH2F *>(GetHisto(fTimeToF));}
  TH1F *GetTimeNbins()		{return dynamic_cast<TH1F *>(GetHisto(fTimeNbins));}
  
  TH1F *GetOccupancy()		{return dynamic_cast<TH1F *>(GetHisto(fOccupancy));}
  TH1F *GetOccWide()		{return dynamic_cast<TH1F *>(GetHisto(fOccWide));}
  TH2F *GetOccWindowPmt()	{return dynamic_cast<TH2F *>(GetHisto(fOccWindowPmt));}
  TH2F *GetOccWindowPmtz()	{return dynamic_cast<TH2F *>(GetHisto(fOccWindowPmtz));}
  TH2F *GetNormWindowPmtz()	{return dynamic_cast<TH2F *>(GetHisto(fNormWindowPmtz));}
  TH2F *GetOccZ()		{return dynamic_cast<TH2F *>(GetHisto(fOccZ));}
  TH2F *GetOccZNopipe()		{return dynamic_cast<TH2F *>(GetHisto(fOccZNopipe));}

  TH2F *GetOccThetaPhi()	{return dynamic_cast<TH2F *>(GetHisto(fOccThetaPhi));}
  TH2F *GetOccThetaPhi2()	{return dynamic_cast<TH2F *>(GetHisto(fOccThetaPhi2));}
  TH2F *GetOccAThetaPhi()	{return dynamic_cast<TH2F *>(GetHisto(fOccAThetaPhi));}
  TH2F *GetOccLThetaPhi()	{return dynamic_cast<TH2F *>(GetHisto(fOccLThetaPhi));}
  TH2F *GetNpmtLThetaPhi()	{return dynamic_cast<TH2F *>(GetHisto(fNpmtLThetaPhi));}

  TH1F *GetOccRayleigh()	{return dynamic_cast<TH1F *>(GetHisto(fOccRayleigh));}
  TH1F *GetOccDirect()		{return dynamic_cast<TH1F *>(GetHisto(fOccDirect));}
  TH1F *GetOccReflected()	{return dynamic_cast<TH1F *>(GetHisto(fOccReflected));}
  TH1F *GetOccRayleighTotal()	{return dynamic_cast<TH1F *>(GetHisto(fOccRayleighTotal));}
  TH1F *GetOccDirectTotal()	{return dynamic_cast<TH1F *>(GetHisto(fOccDirectTotal));}
  TH1F *GetOccReflectedTotal()	{return dynamic_cast<TH1F *>(GetHisto(fOccReflectedTotal));}

  Stat_t GetTimeResidual(Int_t bx)	{return GetContentByBin(fTimeResidual,bx);}
  Stat_t GetTimePeak(Int_t bx)		{return GetContentByBin(fTimePeak,bx);}
  Stat_t GetTimeWidth(Int_t bx)		{return GetContentByBin(fTimeWidth,bx);}
  Stat_t GetRchToF(Int_t bx)		{return GetContentByBin(fRchToF,bx);}
  Stat_t GetToF(Int_t bx)		{return GetContentByBin(fToF,bx);}
  Stat_t GetTimeZ(Int_t bx, Int_t by)	{return GetContentByBin(fTimeZ,bx,by);}
  Stat_t GetTimeToF(Int_t bx, Int_t by)	{return GetContentByBin(fTimeToF,bx,by);}
  Stat_t GetTimeNbins(Int_t bx)		{return GetContentByBin(fTimeNbins,bx);}
  
  Stat_t GetOccupancy(Int_t bx)			{return GetContentByBin(fOccupancy,bx);}
  Stat_t GetOccWide(Int_t bx)			{return GetContentByBin(fOccWide,bx);}
  Stat_t GetOccWindowPmt(Int_t bx, Int_t by)	{return GetContentByBin(fOccWindowPmt,bx,by);}
  Stat_t GetOccWindowPmtz(Int_t bx, Int_t by)	{return GetContentByBin(fOccWindowPmtz,bx,by);}
  Stat_t GetNormWindowPmtz(Int_t bx, Int_t by)	{return GetContentByBin(fNormWindowPmtz,bx,by);}
  Stat_t GetOccZ(Int_t bx, Int_t by)		{return GetContentByBin(fOccZ,bx,by);}
  Stat_t GetOccZNopipe(Int_t bx, Int_t by)	{return GetContentByBin(fOccZNopipe,bx,by);}

  Stat_t GetOccThetaPhi(Int_t bx, Int_t by)	{return GetContentByBin(fOccThetaPhi,bx,by);}
  Stat_t GetOccThetaPhi2(Int_t bx, Int_t by)	{return GetContentByBin(fOccThetaPhi2,bx,by);}
  Stat_t GetOccAThetaPhi(Int_t bx, Int_t by)	{return GetContentByBin(fOccAThetaPhi,bx,by);}
  Stat_t GetOccLThetaPhi(Int_t bx, Int_t by)	{return GetContentByBin(fOccLThetaPhi,bx,by);}
  Stat_t GetNpmtLThetaPhi(Int_t bx, Int_t by)	{return GetContentByBin(fNpmtLThetaPhi,bx,by);}

  Stat_t GetOccRayleigh(Int_t bx)	{return GetContentByBin(fOccRayleigh,bx);}
  Stat_t GetOccDirect(Int_t bx)		{return GetContentByBin(fOccDirect,bx);}
  Stat_t GetOccReflected(Int_t bx)	{return GetContentByBin(fOccReflected,bx);}
  Stat_t GetOccRayleighTotal(Int_t bx)  {return GetContentByBin(fOccRayleighTotal,bx);}
  Stat_t GetOccDirectTotal(Int_t bx)	{return GetContentByBin(fOccDirectTotal,bx);}
  Stat_t GetOccReflectedTotal(Int_t bx) {return GetContentByBin(fOccReflectedTotal,bx);}

  Stat_t GetTimeResidual(Axis_t vx)	{return GetContentByVal(fTimeResidual,vx);}
  Stat_t GetTimePeak(Axis_t vx)		{return GetContentByVal(fTimePeak,vx);}
  Stat_t GetTimeWidth(Axis_t vx)	{return GetContentByVal(fTimeWidth,vx);}
  Stat_t GetRchToF(Axis_t vx)		{return GetContentByVal(fRchToF,vx);}
  Stat_t GetToF(Axis_t vx)		{return GetContentByVal(fToF,vx);}
  Stat_t GetTimeZ(Axis_t vx, Axis_t vy)	{return GetContentByVal(fTimeZ,vx,vy);}
  Stat_t GetTimeToF(Axis_t vx, Axis_t vy)	{return GetContentByVal(fTimeToF,vx,vy);}
  Stat_t GetTimeNbins(Axis_t vx)	{return GetContentByVal(fTimeNbins,vx);}
  
  Stat_t GetOccupancy(Axis_t vx)		{return GetContentByVal(fOccupancy,vx);}
  Stat_t GetOccWide(Axis_t vx)			{return GetContentByVal(fOccWide,vx);}
  Stat_t GetOccWindowPmt(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccWindowPmt,vx,vy);}
  Stat_t GetOccWindowPmtz(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccWindowPmtz,vx,vy);}
  Stat_t GetNormWindowPmtz(Axis_t vx, Axis_t vy) {return GetContentByVal(fNormWindowPmtz,vx,vy);}
  Stat_t GetOccZ(Axis_t vx, Axis_t vy)		{return GetContentByVal(fOccZ,vx,vy);}
  Stat_t GetOccZNopipe(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccZNopipe,vx,vy);}

  Stat_t GetOccThetaPhi(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccThetaPhi,vx,vy);}
  Stat_t GetOccThetaPhi2(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccThetaPhi2,vx,vy);}
  Stat_t GetOccAThetaPhi(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccAThetaPhi,vx,vy);}
  Stat_t GetOccLThetaPhi(Axis_t vx, Axis_t vy)	{return GetContentByVal(fOccLThetaPhi,vx,vy);}
  Stat_t GetNpmtLThetaPhi(Axis_t vx, Axis_t vy)	{return GetContentByVal(fNpmtLThetaPhi,vx,vy);}

  Stat_t GetOccRayleigh(Axis_t vx)	{return GetContentByVal(fOccRayleigh,vx);}
  Stat_t GetOccDirect(Axis_t vx)	{return GetContentByVal(fOccDirect,vx);}
  Stat_t GetOccReflected(Axis_t vx)	{return GetContentByVal(fOccReflected,vx);}
  Stat_t GetOccRayleighTotal(Axis_t vx)  {return GetContentByVal(fOccRayleighTotal,vx);}
  Stat_t GetOccDirectTotal(Axis_t vx)	{return GetContentByVal(fOccDirectTotal,vx);}
  Stat_t GetOccReflectedTotal(Axis_t vx) {return GetContentByVal(fOccReflectedTotal,vx);}

    /** Removed by N. Barros
  void View(TH1* hist1, TH1* hist2 = NULL, TH1* hist3 = NULL, TH1* hist4 = NULL);
  void ViewSummary();
     **/
  ClassDef(QRdt,0)    // Rdt file Reader
};

#endif // not __QRdt_h__
