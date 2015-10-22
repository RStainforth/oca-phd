//*-- Author :    Ranpal Dosanjh   2003 06
/*************************************************************************
 * Copyright(c) 2003, The SNO Software Project, All rights reserved.     *
 * Authors: Ranpal Dosanjh                                               *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include <string.h>

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>

//#include "QSnoed.h"
#include "QRdt.h"

ClassImp(QRdt) ;
//______________________________________________________________________________
//
// QRdt
// ====
// Loads .rdt files into a data structure.  For use with Qpath, etc.  
// Also provides a facility to construct .rdt file names based on a
// given "style" and pass number.  The latter can be specified so that
// the highest pass number is taken, while the former can be specified
// to look for the file corresponding to the run number in a variety of 
// styles.
//
// Loading examples:
// 
// [0] qrdt = new QRdt("/data/rdt/sno_12345.rdt");
// 
// [0] qrdt = new QRdt();
// [1] qrdt->LoadRdtFile("/data/rdt/sno_12345.rdt");
// 
// [0] qrdt = new QRdt();
// [1] qrdt->SetRdtdir("/data/rdt");
// [2] qrdt->SetRdtstyle(1);  // file name has form sno_012345_pX.rdt
// [3] qrdt->SetRdtpass(-1);  // look for highest pass number in dir
// [4] qrdt->LoadRdtFile(12345)
//
// Getting info examples:
//
// [0] Float_t lambda = qrdt->GetLambda();
// [1] if (qrdt->IsFilled("Lambda")) cout << "ok" << endl;
//
// Getting histograms (GetOccupancy(), GetTimeToF(), etc.) amounts to 
// passing pointers to clones.  Thus, the local histograms survive if 
// the QRdt instance is closed.  The trade-off is that these local 
// histograms have to be deleted to avoid a memory leak.
//
// Values in the histograms can be obtained directly, without having
// to create a local histogram.  So:
//
// [0] qrdt->GetOccupancy() 		// returns TH1F* clone
// 
// [0] qrdt->GetOccupancy((Int_t)x) 	// returns contents of bin x
//
// [0] qrdt->GetOccupancy((Float_t)x)	// returns contents of bin 
//					// associated with the value
// 					// of x on the x-axis.
//
//______________________________________________________________________________
QRdt::QRdt(TString fullname)
{
  // QRdt constructor.

  fDefaultTString = "";
  fDefaultTVector3 = new TVector3(-999.,-999.,-999.); 
  fPassMaximum = 9;

  Initialize();

  if (fullname != "") LoadRdtFile(fullname);

  return;
}
//______________________________________________________________________________
QRdt::~QRdt()
{
  // QRdt destructor.

  Close();
  delete fDefaultTVector3;

}
//______________________________________________________________________________
void QRdt::Initialize()
{
  // Initialize all vaiables

  // File and run info
  // -----------------
  fRdtRunnumber = 0;
  fRdtdir = fDefaultTString;
  fRdtfilename = fDefaultTString;
  fRdtfullname = fDefaultTString;

  fRdtstyle = -1;
  fRdtpass = -1;
 
  // Info from .rdt file
  // -------------------
  fRdtLoaded = kFALSE;

  fLogFields = NULL;

  fJulianDayStart = -1;
  fJulianDayStop = -1;
  fUT1Start = -1;
  fUT1Stop = -1;
  fUT2Start = -1;
  fUT2Stop = -1;

  fNpulses = -1;

  fDyecell = -1;
  fLambda = -1;
  fOrientation = -1;

  fLambdaTVector = NULL;
  fOrientationTVector = NULL;
  fOrientationTVector = NULL;
  fRSPS = NULL;

  fManipPos = NULL;
  fReflectPos = NULL;
  fDirectFitPos = NULL;
  fFullFitPos = NULL;

  fTimeResidual = NULL;
  fTimePeak = NULL;
  fTimeWidth = NULL;
  fRchToF = NULL;
  fToF = NULL;
  fTimeZ = NULL;
  fTimeToF = NULL;
  fTimeNbins = NULL;

  fOccupancy = NULL;
  fOccWide = NULL;
  fOccWindowPmt = NULL;
  fOccWindowPmtz = NULL;
  fNormWindowPmtz = NULL;
  fOccZ = NULL;
  fOccZNopipe = NULL;

  fOccThetaPhi = NULL;
  fOccThetaPhi2 = NULL;
  fOccAThetaPhi = NULL;
  fOccLThetaPhi = NULL;
  fNpmtLThetaPhi = NULL;

  fOccRayleigh = NULL;
  fOccDirect = NULL;
  fOccReflected = NULL;
  fOccRayleighTotal = NULL;
  fOccDirectTotal = NULL;
  fOccReflectedTotal = NULL;

  return;
}
//______________________________________________________________________________
void QRdt::Close()
{
  // De-allocate memory

  if (fLogFields) delete[] fLogFields;

  if (fLambdaTVector) delete fLambdaTVector;
  if (fOrientationTVector) delete fOrientationTVector;
  if (fRSPS) delete fRSPS;

  if (fManipPos) delete fManipPos;
  if (fReflectPos) delete fReflectPos;
  if (fDirectFitPos) delete fDirectFitPos;
  if (fFullFitPos) delete fFullFitPos;


  if (fTimeResidual) delete fTimeResidual;
  if (fTimePeak) delete fTimePeak;
  if (fTimeWidth) delete fTimeWidth;
  if (fRchToF) delete fRchToF;
  if (fToF) delete fToF;
  if (fTimeZ) delete fTimeZ;
  if (fTimeToF) delete fTimeToF;
  if (fTimeNbins) delete fTimeNbins;


  if (fOccupancy) delete fOccupancy;
  if (fOccWide) delete fOccWide;
  if (fOccWindowPmt) delete fOccWindowPmt;
  if (fOccWindowPmtz) delete fOccWindowPmtz;
  if (fNormWindowPmtz) delete fNormWindowPmtz;
  if (fOccZ) delete fOccZ;
  if (fOccZNopipe) delete fOccZNopipe;

  if (fOccThetaPhi) delete fOccThetaPhi;
  if (fOccThetaPhi2) delete fOccThetaPhi2;
  if (fOccAThetaPhi) delete fOccAThetaPhi;
  if (fOccLThetaPhi) delete fOccLThetaPhi;
  if (fNpmtLThetaPhi) delete fNpmtLThetaPhi;

  if (fOccRayleigh) delete fOccRayleigh;
  if (fOccDirect) delete fOccDirect;
  if (fOccReflected) delete fOccReflected;
  if (fOccRayleighTotal) delete fOccRayleighTotal;
  if (fOccDirectTotal) delete fOccDirectTotal;
  if (fOccReflectedTotal) delete fOccReflectedTotal;

  fRdtLoaded = kFALSE;

  return;
}
//______________________________________________________________________________
void QRdt::LoadRdtFile()
{
  // Wrapper for LoadRdtFile(fullname).
  // Assumes:
  // 	run number is set
  // 	.rdt directory is set

  ConstructRdtfilename();
  ConstructRdtfullname();
  LoadRdtFile(GetRdtfullname());

  return;
}
//______________________________________________________________________________
void QRdt::LoadRdtFile(Int_t runnumber)
{
  // Wrapper for LoadRdtFile(fullname).
  // Assumes:
  //    .rdt directory is set
  
  SetRdtRunnumber(runnumber);
  ConstructRdtfilename();
  ConstructRdtfullname();
  LoadRdtFile(GetRdtfullname());

  return;
}
//______________________________________________________________________________
void QRdt::LoadRdtFile(TString fullname)
{
  // Dump information from the .rdt file ("fullname") into the class.

  if (GetRdtLoaded()) {
	printf("Warning from QRdt::LoadRdtFile():\n");
	printf("\t.rdt file is currently active.\n");
	printf("\tClosing...\n");
	Close();
	Initialize();
  }
  SetRdtLoaded(kFALSE);

  Bool_t filefound = FileExists(fullname);

  if (!filefound) { // File does not exist
	printf("Warning from QRdt::LoadRdtFile():\n");
	printf("\tFile %s does not exist.\n",fullname.Data());
	printf("\tExiting.\n");
	return;
  }

  printf("QRdt::LoadRdtFile() found file:\n");
  printf("\t%s\n",fullname.Data());
  TFile *rdtfile = new TFile(fullname);

//  if (!InputLog()) {
//	printf("Warning from QRdt::LoadRdtFile():\n");
//	printf("\tProblem inputting run logs.\n");
//  }
//  Jose commented these 24/02/05  
//  now we check rsps from the vector in rdt


  LoadDate(rdtfile);

  LoadLambda(rdtfile);
  LoadOrientation(rdtfile);
  LoadRSPS(rdtfile);
  LoadPosition(rdtfile);

  SetTimePeak((TH1F *)rdtfile->Get("Peak"));
  SetTimeWidth((TH1F *)rdtfile->Get("Width"));
  SetRchToF((TH1F *)rdtfile->Get("fRchToF"));
  SetToF((TH1F *)rdtfile->Get("tof"));
  SetTimeZ((TH2F *)rdtfile->Get("tz"));
  SetTimeToF((TH2F *)rdtfile->Get("ttof"));
  SetTimeNbins((TH1F *)rdtfile->Get("nbins"));
  
  if (IsReallyFilled("TimeToF")) {
	TH1D *tresD = (TH1D *)GetTimeToF()->ProjectionX();
	TH1F *tresF = TH1D_to_TH1F(tresD);
	delete tresD;

	// Double_t norm = (Double_t)tresF->GetEntries();
	Double_t norm = (Double_t)tresF->Integral();
	// Double_t norm = (Double_t)tresF->Integral("width");
	tresF->Sumw2();
	tresF->Scale(1/norm);
	tresF->SetName("TRes (ttof projx)");
	SetTimeResidual(tresF);
  }

  SetOccupancy((TH1F *)rdtfile->Get("Counts"));
  SetOccWide((TH1F *)rdtfile->Get("CountsWide"));
  SetOccWindowPmt((TH2F *)rdtfile->Get("CountsWindow"));
  SetOccWindowPmtz((TH2F *)rdtfile->Get("CountsWindowZ"));
  SetNormWindowPmtz((TH2F *)rdtfile->Get("CountsWindowZnorm"));
  SetOccZ((TH2F *)rdtfile->Get("cz"));
  SetOccZNopipe((TH2F *)rdtfile->Get("cznopipe"));

  SetOccThetaPhi((TH2F *)rdtfile->Get("pt"));
  SetOccThetaPhi2((TH2F *)rdtfile->Get("pt2"));
  SetOccAThetaPhi((TH2F *)rdtfile->Get("apt"));
  SetOccLThetaPhi((TH2F *)rdtfile->Get("lbpt"));
  SetNpmtLThetaPhi((TH2F *)rdtfile->Get("lbnp"));

  SetOccRayleigh((TH1F *)rdtfile->Get("CountsRay"));
  SetOccDirect((TH1F *)rdtfile->Get("CountsDir"));
  SetOccReflected((TH1F *)rdtfile->Get("CountsRef"));
  SetOccRayleighTotal((TH1F *)rdtfile->Get("CountsRayTot"));
  SetOccDirectTotal((TH1F *)rdtfile->Get("CountsDirTot"));
  SetOccReflectedTotal((TH1F *)rdtfile->Get("CountsRefTot"));

  if (IsReallyFilled("Occupancy")) 
	SetNpulses((Int_t)(GetOccupancy()->GetEntries()));

  SetRdtLoaded(kTRUE);
  
  rdtfile->Close();
  return;
}
//______________________________________________________________________________  
void QRdt::LoadDate(TFile *rdtfile)
{ 
  // Get Julian day and times.
  // Use the rdtfile if you can.  Otherwise, get it from a runlog.

    // -- N. Barros <12/07/2011>
    //
    // as far as I know no RDT files have the dates any more
    // Definitely not the ones I have and therefore the method below doesn't even compile
    // in the most recent versions of compilers and ROOT.
    // Go straight for the InputDateFromLog
/*
    Int_t jdstart = (Int_t*)rdtfile->Get("jdstart");
    Int_t jdstop = (Int_t*)rdtfile->Get("jdstop");
    Int_t ut1start = (Int_t*)rdtfile->Get("ut1start");
    Int_t ut1stop = (Int_t*)rdtfile->Get("ut1stop");
    Int_t ut2start = (Int_t*)rdtfile->Get("ut2start");
    Int_t ut2stop = (Int_t*)rdtfile->Get("ut2stop");
*/
    Int_t jdstart = 0;
    Int_t jdstop = 0;
    Int_t ut1start = 0;
    Int_t ut1stop = 0;
    Int_t ut2start = 0;
    Int_t ut2stop = 0;

    
  if (!jdstart) {
  	jdstart = -1;
  	jdstop = -1;
  	ut1start = -1;
  	ut1stop = -1;
  	ut2start = -1;
  	ut2stop = -1;
	
	InputDateFromLog(jdstart,jdstop,ut1start,ut1stop,ut2start,ut2stop);
  }

  SetJulianDayStart(jdstart);
  SetJulianDayStop(jdstop);
  SetUT1Start(ut1start);
  SetUT1Stop(ut1stop);
  SetUT2Start(ut2start);	
  SetUT2Stop(ut2stop);	

  return;
}
//______________________________________________________________________________  
void QRdt::InputDateFromLog(Int_t &jdstart, Int_t &jdstop, 
			   Int_t &ut1start, Int_t &ut1stop, 
			   Int_t &ut2start, Int_t &ut2stop) 
{ 
  // Old style date/time getting.

  // Re-initialize variables
  jdstart = -1;
  jdstop = -1;
  ut1start = -1;
  ut1stop = -1;
  ut2start = -1;
  ut2stop = -1;

  // Get unix times from log.  Compute Julian day, UT1, and UT2.
  if (fLogFields) {
	Double_t unixtime = atof(fLogFields[2]);
	Double_t delta_unixtime = atof(fLogFields[3]);
  	gSNO->Unix2Julian(unixtime,jdstart,ut1start,ut2start);
  	gSNO->Unix2Julian(unixtime+delta_unixtime,jdstop,ut1stop,ut2stop);
    printf("QRdt::InputDateFromLog unixtime %f delta_unixtime %f jdstart %d ut1start %d ut2start %d\n",
		unixtime,delta_unixtime,jdstart,ut1start,ut2start);
  }


  return;

}
//______________________________________________________________________________  
Bool_t QRdt::InputLog()
{ 
  // Input the fields from the run log files corresponding to the present run
  // number.   Result is in fLogFields. Returns kTRUE if successful.
  // 
  // Note, there are lots of hard-coded bits here.  If the run log file format 
  // changes, this should be changed as well.

  // Make sure run number is set.
  Int_t rdtrunnumber = GetRdtRunnumber();
  if (!rdtrunnumber) {
	printf("Warning in QRdt::InputLog():\n");
	printf("\tRun number not set.\n");
	return kFALSE;
  }

  // Get the directory of the log files.
  TString fdir = getenv("OCA_SNO_ROOT");
  fdir += "/autosno/log/";
  TString fnam;

  // Get the override log file.
  fnam = fdir + "run_info_override.log";
  if (!FileExists(fnam)) {
	printf("Warning in QRdt::InputLog():\n");
	printf("\tOverride log file %s not found\n",fnam.Data());
  }
  if (FillLogFields(fnam,rdtrunnumber)) return kTRUE;

  // Get the regular log file.
  fnam = fdir + "run_info.log";
  if (!FileExists(fnam)) {
	printf("Warning in QRdt::InputLog():\n");
	printf("\tLog file %s not found\n",fnam.Data());
  }
  if (FillLogFields(fnam,rdtrunnumber)) return kTRUE;

  return kFALSE;

}
//______________________________________________________________________________  
Bool_t QRdt::FillLogFields(TString fnam, Int_t rdtrunnumber)
{ 
  // Loop over log file named "fnam" until "rdtrunnumber" is found.  Then fill 
  // the fields of fLogFields.  Returns kTRUE if run was found.
  // 
  // Note that the number of fields in a line is hard-coded here.  If the run 
  // log file format changes, this should be changed as well.

  // Get file.
  ifstream fin(fnam);
  if (!fin.is_open()) return kFALSE;
//  printf("QRdt:FillLogFields  file %s is open\n",fnam.Data());
//  printf("QRdt:FillLogFields  looking for run %d\n",rdtrunnumber);

  // Create new fLogFields
  const Int_t nfield = 21;
  if (fLogFields) delete [] fLogFields;
  fLogFields = new TString[nfield];

  // Loop over lines of file until line is found.  Also fill fields.
  Bool_t runfound = kFALSE;
  Int_t previous;
  while (!fin.eof() && !runfound) {
	for (Int_t ifield=0;ifield<nfield;ifield++) fin >> fLogFields[ifield];
	Int_t runnumber = atoi(fLogFields[0]);
//	if (runnumber > 50000 || runnumber < 10000) printf("runnumber %d previous %d\n",runnumber,previous);
	if (rdtrunnumber==runnumber)  { 
		runfound = kTRUE;
  		printf("QRdt:FillLogFields found runnumber %d in run log\n",runnumber);
	}
	previous = runnumber;
  }
  
  return runfound;

}
//______________________________________________________________________________  
void QRdt::LoadLambda(TFile *rdtfile)
{ 
  // Get wavelength and dye cell information.
  
  TVectorF *lambdaTVector = (TVectorF *)rdtfile->Get("lambda");
  if (lambdaTVector) {
  	SetDyecell((Int_t)(*lambdaTVector)(0));
  	SetLambda((*lambdaTVector)(1));
  }

  return;
}
//______________________________________________________________________________
void QRdt::LoadOrientation(TFile *rdtfile)
{ 
  // Get orientation information.
  // In .rdt file, we have NESW = {1,2,3,4}.  This must be transformed into the
  // standard {2,3,0,1}.
  //
  // os -- 2005.07
  // The cast bank information gets written to the rch files even when it is
  // empty (see tdiff.cxx). Hence, the value 0. should not pass the following
  // conditions.

  TVectorF *orientationTVector = (TVectorF *)rdtfile->Get("orientation");
  if (orientationTVector) {
	Float_t orientation = (*orientationTVector)(0);
	if(orientation > 0){
	  orientation += (orientation<3) ? 1 : -3;
	  SetOrientation(orientation);
	} else {
	  SetOrientation(); // default
	  printf("Warning in QRdt::LoadOrientation : Laserball orientation was set to %d.\n",
	          (Int_t)fOrientation);
	} 
  }

  return;
}
//______________________________________________________________________________
void QRdt::LoadRSPS(TFile *rdtfile)
{ 
  // Get RSPS information.
  
  TVector *temp = (TVector *)rdtfile->Get("RSPS");
  if (temp) {fRSPS = temp;}
  else {
  	printf("Warning! in QRdt::LoadRSPS. Couldn't load RSPS vector from rdt file.\n");  
  	printf("Will proceed, but these banks won't be checked at this point.\n");
	fRSPS = new TVector(9728);
	for(int i=0; i < 9728; i++) (*fRSPS)(i) = 1;  
  }

  return;
}
//______________________________________________________________________________
void QRdt::LoadPosition(TFile *rdtfile)
{ 
  // Get position information.
  
  //
  // Manipulator position
  //
//  TVector3 *sourcepos = NULL;
//  sourcepos = (TVector3 *) rdtfile->Get("manip_pos");
//  if (!sourcepos) { // manip_pos doesn't exist: try old system
//	TVectorF *sourcepos_old = NULL;
//	sourcepos_old = (TVectorF *) rdtfile->Get("source_pos");
//	if (sourcepos_old) { // backward compatibility
//		sourcepos = new TVector3((*sourcepos_old)(0),(*sourcepos_old)(1),(*sourcepos_old)(2));
//	}
//  }
//  if (!sourcepos || (sourcepos->Mag() <= 0)) { // does not exist or is zero
//	sourcepos = (TVector3 *)fDefaultTVector3->Clone("manip_pos"); // use default values
//  }
//  SetManipPos(sourcepos);
  SetManipPos(ReadPosition(rdtfile,"manip_pos"));
  if (!IsReallyFilled("ManipPos")) { // try old system
	SetManipPos(ReadOldPosition(rdtfile,"source_pos"));
  }

  //
  // Position from reflection method
  //
  SetReflectPos(ReadPosition(rdtfile,"reflection_pos"));

  //
  // Direct line fit position
  // 
  SetDirectFitPos(ReadPosition(rdtfile,"directfit_pos"));

  // 
  // QPath full-fit position
  //
  SetFullFitPos(ReadPosition(rdtfile,"qpathfit_pos"));

  return;
}
//______________________________________________________________________________
TVector3 *QRdt::ReadPosition(TFile *rdtfile,TString name)
{ 
  // Return standard-style TVector3 position from .rdt file.
  
  TVector3 *sourcepos = NULL;
  sourcepos = (TVector3 *) rdtfile->Get(name);
  if (!sourcepos || (sourcepos->Mag() < 0)) { // does not exist 
//  if (!sourcepos || (sourcepos->Mag() <= 0)) { // does not exist or is zero
// JM Nov 08 2003 What's wrong with zero? It's a perfectly valid position...
	sourcepos = (TVector3 *)fDefaultTVector3->Clone(name); // use default values
  }
  return sourcepos;
  
}
//______________________________________________________________________________
TVector3 *QRdt::ReadOldPosition(TFile *rdtfile,TString name)
{ 
  // Return standard-style TVector3 position from a TVector in .rdt file.
  
  TVector3 *sourcepos = NULL;
  sourcepos = (TVector3 *)fDefaultTVector3->Clone(name); // initialize to default

  TVectorF  *sourcepos_old = NULL;
  sourcepos_old = (TVectorF *) rdtfile->Get(name);
  if (sourcepos_old) { 
		sourcepos->SetX((*sourcepos_old)(0));
		sourcepos->SetY((*sourcepos_old)(1));
		sourcepos->SetZ((*sourcepos_old)(2));
//		sourcepos = new TVector3((*sourcepos_old)(0),
//					 (*sourcepos_old)(1),
//					 (*sourcepos_old)(2));
  }
  if (!sourcepos || (sourcepos->Mag() <= 0)) { // does not exist or is zero
	sourcepos = (TVector3 *)fDefaultTVector3->Clone(name); // use default values
  }
  return sourcepos;
  
}
//______________________________________________________________________________
void QRdt::ConstructRdtfullname()
{
  // Set the .rdt file and path based on fRdtfilename and fRdtdir

  TString dirname = GetRdtdir();
  TString filename = GetRdtfilename();
  TString fullname;

  if (filename==fDefaultTString) fullname = fDefaultTString;
  else fullname = BuildRdtfullname(dirname,filename);

  printf("QRdt::ConstructRdtfullname() setting .rdt file and path to:\n");
  printf("\t[%s]\n",fullname.Data());

  SetRdtfullname(fullname);
  return;
}
//______________________________________________________________________________
TString QRdt::BuildRdtfullname(TString dirname, TString filename)
{
  // Return the full file and path based given the directory and file name.
  TString fullname = dirname + "/" + filename;
  return fullname;
}
//______________________________________________________________________________
void QRdt::ConstructRdtfilename()
{
  // Set the .rdt filename based on fRdtstyle and fRdtpass

  Int_t style = GetRdtstyle();
  Int_t pass = GetRdtpass();
  TString filename = (style>= 0 && pass>= 0) ? BuildRdtfilename(style,pass) :
		     (style>= 0 && pass==-1) ? FindRdtfilePass(style)	    :
		     (style==-1 && pass==-1) ? FindRdtfileStyle()           : 
					       fDefaultTString              ;

  printf("QRdt::ConstructRdtfilename() setting .rdt filename to [%s]\n",filename.Data());
  SetRdtfilename(filename);
  return;
}
//______________________________________________________________________________
TString QRdt::FindRdtfileStyle()
{
  // Find the .rdt file with the highest pass number for styles in a
  // hardcoded priority order 

  TString filename; 

  Int_t style;
  Int_t istyle = 0;
  Int_t nstyle = 3;
  Int_t *priority;
  priority = new Int_t[nstyle];
  
  priority[0] = 2;
  priority[1] = 1;
  priority[2] = 0;

  Bool_t foundfile = kFALSE;

  while ( (!foundfile) && (istyle < nstyle) ) {
  	style = priority[istyle];
	filename = FindRdtfilePass(style);
	if (filename!=fDefaultTString) foundfile = kTRUE;
	istyle++;
  }
  
  delete[] priority;
  if (foundfile) return filename;
  else return fDefaultTString;

}
//______________________________________________________________________________
TString QRdt::FindRdtfilePass(Int_t style)
{
  // Find the .rdt file with the highest pass number for a given style

  TString dirname = GetRdtdir();
  TString filename;
  
  Bool_t foundfile = kFALSE;
  Int_t pass = fPassMaximum;

  while ( (!foundfile) && (pass >= 0) ) { 
	TString fullname;
	filename = BuildRdtfilename(style,pass);
	fullname = BuildRdtfullname(dirname,filename);
	foundfile = FileExists(fullname);
	pass--;
  }

  if (foundfile) return filename;
  else return fDefaultTString;

}
//______________________________________________________________________________
TString QRdt::BuildRdtfilename(Int_t style, Int_t pass)
{ 
  // Compose the .rdt file name based on the style and pass number
  // requested.  Fails on style < 0 or pass < 0.

  if ( style<0 || pass<0 ) return fDefaultTString;

  Int_t runnumber = GetRdtRunnumber();

  // Monte Carlo run numbers are less than zero
  // os -- unless style 9 is specified (see below)
  //TString rdttop = (runnumber< 0) ? "snomc_" : "sno_";
  TString rdttop = ((runnumber< 0) || (style == 9)) ? "snomc_" : "sno_";
  runnumber = abs(runnumber);

  TString rdthead;
  TString rdttail;

  if (style == 0) {		// sno_XXXXX.rdt
	rdthead = rdttop;
	rdttail = ".rdt";
  } else if (style == 1) {	// sno_0XXXXX_pX.rdt
	rdthead = rdttop + "0";
	rdttail = "_p";
	rdttail += pass;
	rdttail += ".rdt";
  } else if (style == 2) {	// sno_0XXXXX_X.rdt
	rdthead = rdttop + "0";
	rdttail = "_";
	rdttail += pass;
	rdttail += ".rdt";
  } else if (style == 9) {	// snomc_XXXXX.rdt for MC without runnumber<0
	rdthead = rdttop;
	rdttail += ".rdt";
  } else {			// no such style
	return fDefaultTString;
  }


  TString filename;
  filename = rdthead;
  filename += runnumber;
  filename += rdttail;

  return filename;
}
//______________________________________________________________________________
void QRdt::SetHisto(TH1** histoptr, TH1* argptr)
{

  *histoptr = argptr;
  if (*histoptr) (*histoptr)->SetDirectory(0);
  return;

}
//______________________________________________________________________________
TH1 *QRdt::GetHisto(TH1* histoptr)
{

  if (!histoptr) return NULL;

  TH1 *cloneptr = dynamic_cast<TH1  *>(histoptr->Clone());
  if (cloneptr) cloneptr->SetDirectory(0);
  return cloneptr;

}
//______________________________________________________________________________
Stat_t QRdt::GetContentByBin(TH1* histoptr, Int_t binx)
{
  if (histoptr) return histoptr->GetBinContent(binx);
  return 0;
}
//______________________________________________________________________________
Stat_t QRdt::GetContentByBin(TH1* histoptr, Int_t binx, Int_t biny)
{
  if (histoptr) return histoptr->GetBinContent(binx,biny);
  return 0;
}
//______________________________________________________________________________
Stat_t QRdt::GetContentByBin(TH1* histoptr, Int_t binx, Int_t biny, Int_t binz)
{
  if (histoptr) return histoptr->GetBinContent(binx,biny,binz);
  return 0;
}
//______________________________________________________________________________
Stat_t QRdt::GetContentByVal(TH1* histoptr, Axis_t valx)
{
  if (histoptr) 
	return histoptr->GetBinContent(
					histoptr->GetXaxis()->FindBin(valx)
				      );
  return 0;
}
//______________________________________________________________________________
Stat_t QRdt::GetContentByVal(TH1* histoptr, Axis_t valx, Axis_t valy)
{
  if (histoptr) 
	return histoptr->GetBinContent(
					histoptr->GetXaxis()->FindBin(valx),
					histoptr->GetYaxis()->FindBin(valy)
				      );
  return 0;
}
//______________________________________________________________________________
Stat_t QRdt::GetContentByVal(TH1* histoptr, Axis_t valx, Axis_t valy, Axis_t valz)
{
  if (histoptr) 
	return histoptr->GetBinContent(
					histoptr->GetXaxis()->FindBin(valx),
					histoptr->GetYaxis()->FindBin(valy),
					histoptr->GetZaxis()->FindBin(valz)
				      );
  return 0;
}
//______________________________________________________________________________
TH1F *QRdt::TH1D_to_TH1F(const TH1D *hin)
{
  TString houtname = hin->GetName();
  houtname += "_TH1F";

  TString houttitle = hin->GetTitle();

  Int_t nbin = hin->GetNbinsX();
  Axis_t xlo = hin->GetXaxis()->GetXmin();
  Axis_t xhi = hin->GetXaxis()->GetXmax();

  TH1F *hout = new TH1F(houtname,houttitle,nbin,xlo,xhi);
  for (Int_t ibin=0;ibin<nbin; ibin++) hout->SetBinContent(ibin,hin->GetBinContent(ibin));
  hout->SetEntries(hin->GetEntries());

  return hout;

}
//______________________________________________________________________________
Bool_t QRdt::IsFilled(TString varname)
{
  // Checks variable against default. 

  Bool_t isfilled = kFALSE;

  if (!GetRdtLoaded()) return isfilled;
  isfilled = IsReallyFilled(varname);

  return isfilled;

}
//______________________________________________________________________________
Bool_t QRdt::IsReallyFilled(TString varname)
{
  // Checks variable against default -- Private.  Used within class before 
  // fRdtLoaded is set.

  Bool_t isfilled = kFALSE;

       if (varname == "JulianDayStart")	isfilled = (fJulianDayStart != -1) ;
  else if (varname == "JulianDayStop")	isfilled = (fJulianDayStop != -1)  ;
  else if (varname == "UT1Start")	isfilled = (fUT1Start != -1)    ;
  else if (varname == "UT1Stop")	isfilled = (fUT1Stop != -1)     ;
  else if (varname == "UT2Start")	isfilled = (fUT2Start != -1)    ;
  else if (varname == "UT2Stop")	isfilled = (fUT2Stop != -1)     ;

  else if (varname == "Npulses")	isfilled = (fNpulses != -1)     ;

  else if (varname == "Dyecell") 	isfilled = (fDyecell != -1)	; 
  else if (varname == "Lambda") 	isfilled = (fLambda != -1) 	;
  else if (varname == "Orientation") 	isfilled = (fOrientation != -1)	;

  else if (varname == "ManipPos") 
	isfilled = ((Bool_t)(fManipPos)) ? 
                   (GetManipPos()->Mag() != fDefaultTVector3->Mag()) :
                   kFALSE;
  else if (varname == "ReflectPos")  	
	isfilled = ((Bool_t)(fReflectPos)) ?
                   (GetReflectPos()->Mag() != fDefaultTVector3->Mag()) :
                   kFALSE;

  else if (varname == "DirectFitPos")  	
	isfilled = ((Bool_t)(fDirectFitPos)) ?
                   (GetDirectFitPos()->Mag() != fDefaultTVector3->Mag()) :
                   kFALSE;
  else if (varname == "FullFitPos")  	
	isfilled = ((Bool_t)(fFullFitPos)) ?
                   (GetFullFitPos()->Mag() != fDefaultTVector3->Mag()) :
                   kFALSE;

  else if (varname == "TimeResidual")	isfilled = (Bool_t)(fTimeResidual);
  else if (varname == "TimePeak")	isfilled = (Bool_t)(fTimePeak)	;
  else if (varname == "TimeWidth")	isfilled = (Bool_t)(fTimeWidth)	;
  else if (varname == "RchToF")		isfilled = (Bool_t)(fRchToF)	;
  else if (varname == "ToF")		isfilled = (Bool_t)(fToF)	;
  else if (varname == "TimeZ")		isfilled = (Bool_t)(fTimeZ)	;
  else if (varname == "TimeToF")	isfilled = (Bool_t)(fTimeToF)	;
  else if (varname == "TimeNbins")	isfilled = (Bool_t)(fTimeNbins)	;

  else if (varname == "Occupancy")	isfilled = (Bool_t)(fOccupancy)		;
  else if (varname == "OccWide")	isfilled = (Bool_t)(fOccWide)		;
  else if (varname == "OccWindowPmt")	isfilled = (Bool_t)(fOccWindowPmt)	;
  else if (varname == "OccWindowPmtz")  isfilled = (Bool_t)(fOccWindowPmtz)	;
  else if (varname == "NormWindowPmtz") isfilled = (Bool_t)(fNormWindowPmtz)	;
  else if (varname == "OccZ")		isfilled = (Bool_t)(fOccZ)		;
  else if (varname == "OccZNopipe")	isfilled = (Bool_t)(fOccZNopipe)	;

  else if (varname == "OccThetaPhi")	isfilled = (Bool_t)(fOccThetaPhi)	;
  else if (varname == "OccThetaPhi2")	isfilled = (Bool_t)(fOccThetaPhi2)	;
  else if (varname == "OccAThetaPhi")	isfilled = (Bool_t)(fOccAThetaPhi)	;
  else if (varname == "OccLThetaPhi")	isfilled = (Bool_t)(fOccLThetaPhi)	;
  else if (varname == "NpmtLThetaPhi")  isfilled = (Bool_t)(fNpmtLThetaPhi)	;

  else if (varname == "OccRayleigh")	isfilled = (Bool_t)(fOccRayleigh)	;
  else if (varname == "OccDirect")	isfilled = (Bool_t)(fOccDirect)		;
  else if (varname == "OccReflected")	isfilled = (Bool_t)(fOccReflected)	;
  else if (varname == "OccRayleighTotal")	isfilled = (Bool_t)(fOccRayleighTotal)	;
  else if (varname == "OccDirectTotal")		isfilled = (Bool_t)(fOccDirectTotal)	;
  else if (varname == "OccReflectedTotal")	isfilled = (Bool_t)(fOccReflectedTotal)	;

  else printf("Warning in QRdt::IsFilled(): %s does not exist\n",varname.Data());

  return isfilled;

}
//______________________________________________________________________________
Bool_t QRdt::FileExists(TString fullname)
{
  // Returns kTRUE if file (with path) given by fullname exists.
  
  Bool_t exists = kFALSE;
//  Long_t id,size,flags,modtime;
//  use the next 2 lines instead when root version 4.00.xx becomes official
  Long_t id,flags,modtime;
  Long64_t size;

  exists = !(gSystem->GetPathInfo(fullname,&id,&size,&flags,&modtime));
  exists &= !(flags>>1); // make sure it isn't a directory or special file
  return exists;

}

/**
//______________________________________________________________________________
void QRdt::View(TH1* hist1, TH1* hist2, TH1* hist3, TH1* hist4)
{
  // 04.2006 -- O.Simard
  // Useful function to visualize histograms of PMT properties using
  // QSnoed, in interactive mode. 
  // N.B. QSnoed can read four histograms at the time.
  // void QSnoed::View(TH1 *hist_qhl, TH1 *hist_tac, TH1 *hist_qhs, TH1 *hist_qlx)

  QSnoed* qsnoed = new QSnoed();
  qsnoed->View(hist1,hist2,hist3,hist4);

  printf("----------------------------------------------\n");
  printf("QRdt::View():\n");
  printf("  Legend:\n");
  printf("\t%s = Qhl\n\t%s = Tac\n\t%s = Qhs\n\t%s = Qlx\n",
	 hist1->GetTitle(),hist2->GetTitle(),
	 hist3->GetTitle(),hist4->GetTitle());

  return;
}

//______________________________________________________________________________
void QRdt::ViewSummary()
{
  // Same as QRdt::View() but with predefined histograms.

  View(GetOccupancy(),GetTimePeak(),GetOccWide(),GetToF());
  return;
}
 
 **/
