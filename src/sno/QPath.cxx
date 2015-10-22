//*-- Author :    Bryce Moffat   25/04/00

/*************************************************************************
 * Copyright(c) 1999, The SNO Software Project, All rights reserved.     *
 * Authors: Bryce Moffat                                                 *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TF1.h>
#include <TFile.h>
#include "TRandom.h"

#include "QPath.h"
#include "QOCAReflect.h"

#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#endif

//______________________________________________________________________________
ClassImp(QPath)
;
//______________________________________________________________________________
//
// Path class for optics.
//
// SNO optics application algorithms: applies QOptics paths and
// laserball masks to data from .rdt (root delta-time) files or
// QOCATree data structures.
//
// 27-Oct-2000 - Moffat
// Routines to fit source position incorporated into QPath class.
//
// 17-Dec-2000 - Moffat
// Variables and code to stude position-related systematics
//
//______________________________________________________________________________
QPath::QPath()
{
  // QPath constructor.
  //
  // Takes a set of laserball runs and makes or fills the optical parameters
  // in a QOCATree.
  //
  // Input can be a QOCATree or a list or runs to be found in a directory of
  // .rdt (root delta-time) histogram files.
  //
  // See QTDiff for a method to convert from .rch (root calibration histograms)
  // to .rdt files.

  Int_t i;

  fNumberofruns = 0;
  fRunlist = NULL;
  fRunpasslist = NULL;
  fCentrerunlist = NULL;
  fCentrerunindex = NULL;

  fRunposlist = NULL;
  fRunpositions = NULL;

  fCentrerunptr = NULL;

  fNumberofcentre = 0;
  fCentreruns = NULL;

  fNCentrerunPmt = 0;
  fCentrerunPmtIndex = NULL;

  fDefaultlambda = 500;  // Set default to 500nm, and user can reset.
  fLambda = 500;         // Same as fDefaultlambda, for consistency.
  
  fRSPS = NULL;

  fRdtdir = "";
  fDqxxdir = "";
  fNcdPosfile = "";
  fPmtvarfile = "";

  fQOptics = new QOptics(0,0,0);  // Make QOptics class instances
  fQOpticsX = NULL;
  fQOpticsY = NULL;
  fQOpticsZ = NULL;
  //fQOpticsX = new QOptics(0,0,0);
  //fQOpticsY = new QOptics(0,0,0);
  //fQOpticsZ = new QOptics(0,0,0);

  fQLaserballUtil = new QLaserballUtil(); // Make QLaserballUtil class instance

  // PSUP-AV offset is about 0.0 in September 2000 manipulator system, apparently
  // due to a shift in the manipulator laserball zero.
  // 5-Dec-2000 - Bryce Moffat
  //  SetPSUPCentre(-0.7,-3.0,-6.4);  // QPath version: all instances of QOptics,X,Y,Z
  SetPSUPCentre( 0.0, 0.0, 0.0);  // QPath version: all instances of QOptics,X,Y,Z

  gSNO->GetPMTxyz();  // ensure PMT geometry is loaded

  // Laserball position systematics control
  fPositionsyst = 0;  // 0=no systematics added in positioning, -1=smear, +1=shift
  fXsyst = 0.0;
  fYsyst = 0.0;
  fZsyst = 0.0;
  fRsyst = 0.0;
  fBallsyst = 0.0;

  fFitLBPosition = 0;
  fFitLBPositionSetup = kFALSE;

  fCurrentrun = NULL;

  fVrch = 30.0 / fQOptics->GetIndexD2O();  // Erroneous rch speed of light in Sept 2000
  fVgroupd2o = fQOptics->GetVgroupD2O();
  fVgroupacr = fQOptics->GetVgroupAcrylic();
  fVgrouph2o = fQOptics->GetVgroupH2O();
  
  fTimeSigma = 1.6;
  fTimeSigmaNSigma = 3.0; // how many sigma's to cut pmt times
  fChisquare = 0;
  fdelpos = 1.0;  // Use 1 cm offsets to get derivatives by default ?

  fnpmts = 0;
  fmrqx = vector(1,10000);  // A little too much storage - but never needs reallocation
  fmrqy = vector(1,10000);
  fmrqsig = vector(1,10000);

  fmrqpars = vector(1,5);  // parameters: t0, x_LB, y_LB, z_LB, c_multiplier
  fmrqvary = ivector(1,5);
  for (i=1; i<=5; i++) fmrqvary[i] = 1;  // All parameters variable
  fmrqvary[5] = 0; // but speed of light not variable
  fmrqcovar = matrix(1,5,1,5);
  fmrqalpha = matrix(1,5,1,5);

  fPrint = 1;     // Print level: 0 no messages, 1 occasional info, 2 lots, 3 extreme
  fNcalls = 0;    // Number of calls to mrqfuncs() - used as a diagnostic
  fNmessage = 10; // Number of calls between message prints

  fNelements = 10000;
  fChiarray = new Float_t[10000];
  fResarray = new Float_t[10000];
  fPmteff = new Float_t[10000];

  fTimewinPavr = 8.0;    // default: set with SetTimeWindow()
  fRdtfilestyle = 0;     // default: set with SetRdtFileStyle()

  fEfficienciesLoaded = kFALSE; // flag to avoid loading efficiencies many times
  fNcdReflectionDone = kFALSE;  // flag to avoid calling QOCAReflect::NcdReflection() 
                                // more than once (takes way too long)
  fNcdReflectivityScale = 1.0;  // do not scale NCD reflectivity
  fReMask = kTRUE;              // flag to avoid masking PMTs from another QOCATree
                                // must be kept TRUE for data.

  fForceDqxx = kTRUE;           // flag to force checking dqxx files

}
//______________________________________________________________________________
QPath::~QPath()
{
  // QPath destructor.

  Int_t i;

  if (fRunlist) delete[] fRunlist;
  if (fCentrerunlist) delete[] fCentrerunlist;
  if (fCentrerunindex) delete[] fCentrerunindex;
  if (fRunposlist) delete[] fRunposlist;
  if (fRunpositions) delete[] fRunpositions;
  if (fCentreruns) delete[] fCentreruns;
  if (fCentrerunPmtIndex) delete[] fCentrerunPmtIndex;
  // if (fRSPS) delete fRSPS; // os -- not created with new in this class

  // Only delete the array of pointers, not the objects!
  // delete fCentrerunptr[i]; erases the run from the QOCATree! Not desired!
  // 27-Dec-2000 - Moffat
  // Reversed 29-Dec-2000 - Moffat - SetCentreRuns() allocates new QOCARun's for these!
  if (fCentrerunptr) {
    for (i=0; i<fNumberofcentre; i++) {
      if (fCentrerunptr[i]) delete fCentrerunptr[i];
    }
    delete[] fCentrerunptr;
  }

  delete fQOptics;
  if(fQOpticsX){delete fQOpticsX; fQOpticsX = NULL;}
  if(fQOpticsY){delete fQOpticsY; fQOpticsY = NULL;}
  if(fQOpticsZ){delete fQOpticsZ; fQOpticsZ = NULL;}
  delete fQLaserballUtil;

  free_vector(fmrqx,1,10000);
  free_vector(fmrqy,1,10000);
  free_vector(fmrqsig,1,10000);

  free_vector(fmrqpars,1,5);
  free_ivector(fmrqvary,1,5);
  free_matrix(fmrqcovar,1,5,1,5);
  free_matrix(fmrqalpha,1,5,1,5);

  delete[] fChiarray;
  delete[] fResarray;
  delete[] fPmteff;

}
//______________________________________________________________________________
QOCATree *QPath::FillFromRDT(const Char_t *rdtname,const Char_t *rdttitle)
{
  // This is the basic initialization routine for a QOCATree from "raw" data.
  // 
  // Read in files from "rdtdir"/sno_<run##>.rdt to fill in the raw
  // counts in the tree for runs in the runlist.  If 'rdtdir" is a
  // zero-length string, use a default directory (rdt/).
  //
  // For negative run numbers, the filenames change to "rdtdir"/snomc_<run##>.rdt,
  // where the absolute value for the run number is used.  Also, both .rch
  // and associated dqxx_00000xxxxx.dat are stored under "rchdir"/
  //
  // Similarly for .rch files (used to extract the wavelength of the run.)
  //
  // Also does a very crude normalization of the run: just putting the mean
  // occupancy into QOCARun::SetNorm().
  //
  // 22-Mar-2001 
  // When auto-saving, ROOT writes the new QOCATree with the default name.
  // This must be supplied by the user, and can't simply be given later with
  // a call to ocatree->Write("new_name").  So, for large QOCATree's (at least),
  // specify a unique name and title.
  //
  // 29-Jan-2003        Jose'
  // Wavelength and dye cell information can now be obtained from the rdt file,
  // so the call for the rch file was removed.
  // Rdt and Dqxx directories are now set in SetRdtdir and SetDqxxdir.
  // If existent, pass numbers are used to make up the file name as
  // sno_0xxxxx_px.rdt Otherwise, default format (sno_xxxxx.rdt) is used
  // 
  // 09-Aug-2003	rsd
  // Rewritten to use the QRdt class.  Reverted to using pass number (as opposed 
  // to using a string) when specifying the .rdt file.  The format of the file
  // name should be coded into QRdt.
  //
  // 07-Oct-2004	Olivier Simard
  // Now loads the LB fitted positions from the .rdt files when available.
  // The positions are found using qoca_positionfit.cxx during the nearline
  // processing of the optics scans.
  //
  // 11/05/2006       Nuno Barros
  // Now there is a list of the RDT files loaded. Helpful for debugging purposes

  // -------------------------------------------------
  // Stolen from previous version... clean up later...
  // -------------------------------------------------
  printf("Beginning FillFromRDT #2...\n");
  if (fNumberofruns<1) {
	fprintf(stderr,"Set the runs by SetRunList().\n");
	fprintf(stderr,"%d runs are currently defined - at least one needed!\n",
		fNumberofruns);
	fprintf(stderr,"Aborting FillFromRDT().\n");
	return NULL;
  }
  if (fRdtdir == "") {
	fprintf(stderr,"Set the rdt file directory with SetRdtdir().\n");
        fprintf(stderr,"Aborting FillFromRDT().\n");
	return NULL;
  }
  if (fDqxxdir == "") {
        fprintf(stderr,"Needs DQXX Banks to work properly.\n");
    	fprintf(stderr,"Set the dqxx file directory with SetDqxxdir().\n");
        fprintf(stderr,"Aborting FillFromRDT().\n");
	return NULL;
  }

  // ----------
  // Initialize
  // ----------
  QOCATree *ocatree = new QOCATree(rdtname,rdttitle);
  QOCARun *ocarun = ocatree->GetQOCARun();
  std::vector<TString> pRdtFiles;
  if (fNumberofruns == 0) {
        for(int itemp = 0; itemp < 10000; itemp++) fTimeNbins[itemp].resize(1);
        fTimeNbinsLoaded.resize(1);
  } else {
        for(int itemp = 0; itemp < 10000; itemp++) fTimeNbins[itemp].resize(fNumberofruns);
        fTimeNbinsLoaded.resize(fNumberofruns);
  }

  Int_t irun;
  for (irun=0; irun<fNumberofruns; irun++) {
	printf("----------------------------------------\n");
	printf("Processing run %d (%d out of %d)\n",fRunlist[irun],irun+1,fNumberofruns);

	// ---------------------------------------------------
	// Clear out run information from previous run, if any
	// ---------------------------------------------------
	ocarun->Clear();  

	// ------------------------
	// Load .rdt file into QRdt 
	// ------------------------
	QRdt *qrdt = new QRdt;
	qrdt->SetRdtRunnumber(fRunlist[irun]);
	qrdt->SetRdtdir(fRdtdir);
	qrdt->SetRdtstyle(fRdtfilestyle); // os -- enable user style
	if (fRunpasslist == NULL) qrdt->SetRdtstyle(0); // overwrite if no pass
	else {
	  // qrdt->SetRdtstyle(1); // now set by user
	  qrdt->SetRdtpass(fRunpasslist[irun]);
	}
	qrdt->LoadRdtFile();

	// ------------------------------------
	// Transfer info into run, if possible.
	// ------------------------------------
	if (FillRunFromRDT(qrdt,ocarun)) {
		ocatree->Fill();  // Fill the latest run (with pmt's) in the tree
		pRdtFiles.push_back(qrdt->GetRdtfilename());
	} else {
		printf("Warning from FillFromRDT():\n");
		printf("\tProblems with run %d (%d in list)\n",fRunlist[irun],irun);
		printf("\tNot including run %d in output QOCATree.\n",fRunlist[irun]);
		printf("\tThis QOCATree will otherwise be fine...\n");
	}

	delete qrdt;
	
	
  }
  // nb - Print out the list of loaded RDT files
  printf("----------------------------------------\n");
  printf("List of loaded RDT files.\n");
  printf("----------------------------------------\n");
  printf("\n--> Files loaded from %s\n\n",fRdtdir.Data());

  if (pRdtFiles.size() != (unsigned int) fNumberofruns){
    printf("WARNING: Some runs were not loaded from RDT files.\n\n");
  }

  std::vector<TString>::const_iterator it = pRdtFiles.begin();
  std::vector<TString>::const_iterator itEnd = pRdtFiles.end();
  while (it != itEnd) {
    printf("%s\n",(*it).Data());
    ++it;
  }

  return ocatree;

}
//______________________________________________________________________________
QOCATree *QPath::FillFromRDT(QRdt *qrdt, const Char_t *rdtname,const Char_t *rdttitle)
{
  // Overloaded to use QRdt.  Basically just a wrapper for FillRunFromRDT().
  // QOCATree will contain one run.

  QOCATree *ocatree = new QOCATree(rdtname,rdttitle);
  QOCARun *ocarun = ocatree->GetQOCARun();
  std::vector<TString> pRdtFiles;
  for(int itemp = 0; itemp < 10000; itemp++) fTimeNbins[itemp].resize(fNumberofruns);
  fTimeNbinsLoaded.resize(fNumberofruns);

  if (FillRunFromRDT(qrdt,ocarun)) ocatree->Fill();
  else {
	printf("Aborting FillFromRDT().\n");
  	return NULL;
  }
  return ocatree;

}
//______________________________________________________________________________
Bool_t QPath::FillRunFromRDT(QRdt* qrdt, QOCARun *ocarun)
{
  // Fills run information from QRdt instance.

  // --------------------------------------------------------
  // Exit if qrdt is invalid.  Otherwise, get the run number.
  // --------------------------------------------------------
  if (IsInvalidQRdt(qrdt)) {
	printf("\tAborting FillFromRDT().\n");
	fprintf(stderr,"Aborting FillFromRDT().\n");
        return kFALSE;
  }
  Int_t runnumber = qrdt->GetRdtRunnumber();
  printf("Calling QPath::FillRunFromRDT() for run %d\n",runnumber);


  // ----------------------
  // Read in the DQXX bank.
  // ----------------------
  TString fdqxx = MakeDqxxName(runnumber); // .dqxx filename with path
  if (fdqxx.IsNull()) {
    if(fForceDqxx){
      fprintf(stderr,"Aborting FillFromRDT().\n");
      return kFALSE;
    } 
    else Warning("FillRunFromRDT","Could not make this DQXX filename ... will proceed anyway.");
  } 
  if (qrdt->FileExists(fdqxx)) gPMTxyz->ReadDQXX(fdqxx.Data());

  // ----------------------------------------------------------------
  // Check if the time-of-flight used to correct the prompt time when 
  // producing the .rch file exists in a histogram.  
  // ----------------------------------------------------------------
  if (!qrdt->IsFilled("RchToF")) {
	printf("Warning in QPath::FillFromRdt():\n");
	printf("\tNo RchToF histogram in .rdt file.\n");
	printf("\tUsing %f for .rch speed of light\n",fVrch);
  } else printf("Using speed of light from the RchToF histogram.\n");
  
  Int_t runindex = 0;
  for(int itemp = 0; itemp < fNumberofruns; itemp++) {
    if (fRunlist[itemp] == runnumber) runindex = itemp;
  }

  if (!qrdt->IsFilled("TimeNbins")) {
        fTimeNbinsLoaded[runindex] = 0;
        printf ("Warning!: Time nbins histo not filled in rdt, will use 32-1.\n");
  } else {
        fTimeNbinsLoaded[runindex] = 1;
        printf ("Dynamically loading time Nbins from rdt.\n");
  }

  // ---------------------------------
  // Get some run info from .rdt file.
  // ---------------------------------
  TVector3 *manippos = (TVector3 *)qrdt->GetManipPos();
  if(qrdt->IsFilled("ManipPos")){
    if(!(manippos->X() > 0 || manippos->X() <= 0)) {
  	  manippos->SetX(-999);
	  Warning("FillRunFromRDT","Manip X position is nan in rdt file. Setting it to %f",manippos->X());
    }
    if(!(manippos->Y() > 0 || manippos->Y() <= 0)) {
  	  manippos->SetY(-999);
	  Warning("FillRunFromRDT","Manip Y position is nan in rdt file. Setting it to %f",manippos->Y());
    }
    if(!(manippos->Z() > 0 || manippos->Z() <= 0)) {
  	  manippos->SetZ(-999);
          Warning("FillRunFromRDT","Manip Z position is nan in rdt file. Setting it to %f",manippos->Z());
    }
    ocarun->SetManipxyz(manippos->X(),manippos->Y(),manippos->Z());
  }
  else ocarun->SetManipxyz(-999,-999,-999);

  // ---------
  // check for fit positions: if nan values, reset to default
  // such that the positions will be fitted later.

  // direct line fit
  if(qrdt->IsFilled("DirectFitPos")){
    TVector3 *dirfitpos = (TVector3 *)qrdt->GetDirectFitPos();
    if(!(dirfitpos->X() > 0 || dirfitpos->X() <= 0)) {
  	  dirfitpos->SetX(-999);
	  Warning("FillRunFromRDT","Direct line fit X position is nan in rdt file. Setting it to %f",dirfitpos->X());
    }
    if(!(dirfitpos->Y() > 0 || dirfitpos->Y() <= 0)) {
  	  dirfitpos->SetY(-999);
	  Warning("FillRunFromRDT","Direct line fit Y position is nan in rdt file. Setting it to %f",dirfitpos->Y());
    }
    if(!(dirfitpos->Z() > 0 || dirfitpos->Z() <= 0)) {
  	  dirfitpos->SetZ(-999);
	  Warning("FillRunFromRDT","Direct line fit Z position is nan in rdt file. Setting it to %f",dirfitpos->Z());
    }
    ocarun->SetFitxyz(dirfitpos->X(),dirfitpos->Y(),dirfitpos->Z());
  }
  else ocarun->SetFitxyz(-999,-999,-999);

  // full qoptics fit
  if(qrdt->IsFilled("FullFitPos")){
    TVector3 *fullfitpos = (TVector3 *)qrdt->GetFullFitPos();
    if(!(fullfitpos->X() > 0 || fullfitpos->X() <= 0)) {
  	  fullfitpos->SetX(-999);
	  Warning("FillRunFromRDT","Full fit X position is nan in rdt file. Setting it to %f",fullfitpos->X());
    }
    if(!(fullfitpos->Y() > 0 || fullfitpos->Y() <= 0)) {
  	  fullfitpos->SetY(-999);
	  Warning("FillRunFromRDT","Full fit Y position is nan in rdt file. Setting it to %f",fullfitpos->Y());
    }
    if(!(fullfitpos->Z() > 0 || fullfitpos->Z() <= 0)) {
  	  fullfitpos->SetZ(-999);
	  Warning("FillRunFromRDT","Full fit Z position is nan in rdt file. Setting it to %f",fullfitpos->Z());
    }
    ocarun->SetQFitxyz(fullfitpos->X(),fullfitpos->Y(),fullfitpos->Z());
  }
  else ocarun->SetQFitxyz(-999,-999,-999);
  // ---------

  // 11.2006 - O.Simard
  // Additional check on direct and full path fit positions:
  // The fit start point is the manipulator position. Therefore, if the fit fails 
  // (gaussj error or something like that) the fit position will be exactly the 
  // manipulator position. Even though there is a (very small) probability that the
  // fit finds the exact same position, we'll at least warn the user that the fitter
  // might have had issues.
  if(
    ((ocarun->GetQFitx() == ocarun->GetManipx()) && (ocarun->GetQFitx() != -999))
    && ((ocarun->GetQFity() == ocarun->GetManipy()) && (ocarun->GetQFity() != -999))
    && ((ocarun->GetQFitz() == ocarun->GetManipz()) && (ocarun->GetQFitz() != -999))
    ) Warning("FillRunFromRDT","Detected that Full fit and Manip coordinates are exactly the same.");

  // ---------
  Float_t lambda = fDefaultlambda;
  if (qrdt->IsFilled("Lambda")) lambda = qrdt->GetLambda();
  if (lambda == 0.0) {
  	Warning("FillRunFromRDT","Wavelength set in rdt file is 0, using default %d nm.",(int)fDefaultlambda);
	lambda = fDefaultlambda;
  }
  SetLambda(lambda);

  Float_t orientation = -1;
  if (qrdt->IsFilled("Orientation")) orientation = qrdt->GetOrientation();
  else orientation = (Float_t)fQLaserballUtil->GetRunLBOrientation(runnumber);
  
  fRSPS = (TVector*) qrdt->GetRSPS();

  // -------------------------------------------------------------
  // Set fVrch.  In more recent processing, this will not be used.
  // First condition is MC (uses group velocity in D2O).
  // Alternative is the default in Phil's tool's rootify.
  // -------------------------------------------------------------
  fVrch = (runnumber < 0) ? fQOptics->GetVgroupD2O(lambda) : fQOptics->GetVgroupD2O(500);
	
  // -----------------------------
  // Put .rdt run info into ocarun
  // -----------------------------
  ocarun->SetRun(runnumber);
  ocarun->SetLambda(lambda);
  ocarun->SetNpulses(qrdt->GetNpulses());

  // ----------------------------------------------------------
  // Set other variables to reasonable defaults/initializations
  // ----------------------------------------------------------
  ocarun->SetNorm(1.0);
  ocarun->SetLaserpos(0);  	// Use manipulator position as laserx,y,z position
  ocarun->SetBalltheta(0);  	// No theta rotations yet...
  ocarun->SetBallphi(fQLaserballUtil->GetRunLBPhi(orientation));


  // ----------------------------------------------
  // Loop over pmts.  Fill PMT info from .rdt file.
  // ----------------------------------------------
  QOCAPmt *ocapmt = new QOCAPmt();

  Int_t ngoodpmt = 0;  	// Number of good PMT's in a run
  Int_t pmtn;		// PMT number (0-10000), NOT index in run
  Double_t norm = 0.0; 	// For crude normalization calculation (mean of occupancies).
  			// Set to 0.0 for each run; sum of occupancies/npmt = crude norm
  Double_t timesigmamean; // arithmetic mean of twidth for this run
  Double_t timesigmasigma;// sigma of twidth for this run
  Double_t timesigmameansqd; // arithmetic mean of twidth squared for this run

  fCurrentrun = ocarun;	// Must set fCurrentrun to use run info in FillPmtFromRDT()
  fCurrentrunIndex = runindex;

  for (pmtn=0; pmtn<10000; pmtn++) {  // Loop over pmt's, adding them to the ocarun

	if (FillPmtFromRDT(qrdt,ocapmt,pmtn)) {
		norm += ocapmt->GetOccupancy();  // Add occupancy to norm (not raw counts...)
		ocarun->AddPMT(ocapmt);          // Increments ocarun->GetNpmt() by one
		timesigmamean += ocapmt->GetTwidth(); // Add time width
		timesigmameansqd += pow(ocapmt->GetTwidth(),2); // Add time width
		ngoodpmt++;
	}
  }
  
  fCurrentrun = NULL;	// Don't need it anymore.

  // ------------------------------------------------------------------------------
  // Make the normalization the average occupancy, then make the normalization
  // the inverse of that so it's a multiplicative factor.  eg. fNorm*fOccupancy = 1
  // ------------------------------------------------------------------------------
  if (ocarun->GetNpmt() > 0) norm /= ocarun->GetNpmt();
  else norm = 0.0;
  if (norm>0.0) ocarun->SetNorm(1.0/norm);
  else ocarun->SetNorm(0.0);
  
  // ------------------------------------------------------------
  // Complete calculation of time width mean and sigma estimators
  // by dividing by the appropriate factors
  // ------------------------------------------------------------
  if ((ocarun->GetNpmt() > 0) && (ocarun->GetNpmt() != 1)){
    Float_t n = (Float_t)ocarun->GetNpmt();
    timesigmamean /= n;
    timesigmameansqd /= n;
    // unbiased estimator for unknown true mean
    Double_t s_squared = (n/(n-1))*(timesigmameansqd - pow(timesigmamean,2)); 
    timesigmasigma = sqrt(s_squared);
  }
  else {
    timesigmamean = fTimeSigma; // hardcoded typical width
    timesigmameansqd = 0;
    // unknown standard deviation: 
    // set to large number so that all PMTs are in
    timesigmasigma = 1000; 
  }
  // set in arrays for later use
  ocarun->SetTwidthMean(timesigmamean);
  ocarun->SetTwidthSigma(timesigmasigma);
  
  // ------------------------
  // Output, clean-up, return
  // ------------------------
  printf("Run %d with %d PMT's (%d filled) with norm %10.4g\n",
          ocarun->GetRun(), ocarun->GetNpmt(), ngoodpmt, ocarun->GetNorm());
  printf("and time-width mean and deviation of %.4f and %.4f ns.\n",timesigmamean,timesigmasigma);

  delete ocapmt; // created with new in this function 

  return kTRUE;

}
//______________________________________________________________________________
Int_t QPath::FillPmtFromRDT(QRdt *qrdt, QOCAPmt *ocapmt, const Int_t pmtn)
{
  // Dump .rdt histo info into ocapmt structure for pmt number pmtn.
  // Should be called from FillRunFromRDT() only, since the latter checks 
  // if the necessary histos exist in the file.

  // -----------------
  // Initialize ocapmt
  // -----------------
  ocapmt->Clear();  // Reset all values for ocapmt
  ocapmt->SetBasicBad(kTRUE);

  // --------------------------------
  // Make sure run info is available.
  // --------------------------------
  if (!fCurrentrun) return 0;

  // ---------------------------------------
  // Check if pmt meets minimum requirements
  // ---------------------------------------
  if ( gPMTxyz->IsInvalidPMT(pmtn)     ) return 0;
  if (!gPMTxyz->IsNormalPMT(pmtn)      ) return 0;
  if (fForceDqxx && !gPMTxyz->IsTubeOn(pmtn)) return 0;
  if (!((*fRSPS)(pmtn))                ) return 0; // checking rsps/chcs from vector in rdt

  // --------------
  // Basic pmt info
  // --------------
  ocapmt->SetBasicBad(kFALSE);
  ocapmt->SetPmtn(pmtn);
  ocapmt->SetPmtx(gPMTxyz->GetX(pmtn));
  ocapmt->SetPmty(gPMTxyz->GetY(pmtn));
  ocapmt->SetPmtz(gPMTxyz->GetZ(pmtn));

  //ocapmt->SetPmteff(1.0);
  ocapmt->SetPmteffc(1.0); // reset in QPath::CalculateOccratio()

  // ------------------------------------------------------
  // Necessary info from .rdt file: (raw) occupancy counts, 
  // time-of-flight, prompt time peak and width.
  // ------------------------------------------------------
  ocapmt->SetNprompt(qrdt->GetOccupancy((Float_t)pmtn));
  ocapmt->SetTflight(qrdt->GetToF((Float_t)pmtn));
  ocapmt->SetTprompt(qrdt->GetTimePeak((Float_t)pmtn));
  ocapmt->SetTwidth(qrdt->GetTimeWidth((Float_t)pmtn));		

  /*
  // -----------------------------------------------------------------
  // Put in the Monte Carlo Direct, Reflected and Rayleigh information
  // for prompt and total light.
  // 3-Nov-2000 - Bryce Moffat
  // -----------------------------------------------------------------
  if (!qrdt->IsFilled("OccDirect")) {
	ocapmt->SetRayleigh(0.0);
	ocapmt->SetDirect(0.0);
	ocapmt->SetReflect(0.0);
	ocapmt->SetRayTotal(0.0);
	ocapmt->SetDirTotal(0.0);
	ocapmt->SetRefTotal(1.0);  // why is this = 1 ?
  } else {
	ocapmt->SetRayleigh(qrdt->GetOccRayleigh((Float_t)pmtn)); 
	ocapmt->SetDirect(qrdt->GetOccDirect((Float_t)pmtn));
	ocapmt->SetReflect(qrdt->GetOccReflected((Float_t)pmtn));
	ocapmt->SetRayTotal(qrdt->GetOccRayleighTotal((Float_t)pmtn));
	ocapmt->SetDirTotal(qrdt->GetOccDirectTotal((Float_t)pmtn));
	ocapmt->SetRefTotal(qrdt->GetOccReflectedTotal((Float_t)pmtn));
  }
  */

  // ----------------------------------------------------------------------
  // Put in the .rch time-of-flight used to get Tprompt, i.e.,  the 
  // time-of-flight used to correct the prompt time when producing the .rch 
  // file.  This will have to be re-added to the prompt time to get the raw 
  // time for the position fit.
  // ----------------------------------------------------------------------
  if (qrdt->IsFilled("RchToF")) ocapmt->SetRchtof(qrdt->GetRchToF((Float_t)pmtn));
  else { // have to compute it the old way
	TVector3 srcvec = fCurrentrun->GetManipxyz();
	TVector3 *pmtvec = new TVector3();
	pmtvec->SetXYZ(ocapmt->GetPmtx(),ocapmt->GetPmty(),ocapmt->GetPmtz());
	Float_t distance = (*pmtvec - srcvec).Mag();
	ocapmt->SetRchtof(distance/fVrch);
	delete pmtvec;
  }

  // ----------------------------------------------------------------------
  // Get the number of bins used in tdiff to search for the time peak.
  // This will be used in the position fit to rescale the time width, 
  // instead of the hard-coded sqrt(31) factor. For details on the checks
  // done on this change, see http://www.lip.pt/~maneira/sno/positions/sqrt31.html
  // jm, Jan 2007       
  // ----------------------------------------------------------------------
  if (fTimeNbinsLoaded[fCurrentrunIndex]){
    fTimeNbins[pmtn][fCurrentrunIndex] = (int) qrdt->GetTimeNbins((Float_t)pmtn);
  }

  // ------------------------------------
  // Variables for occupancy calculations
  // ------------------------------------
  Double_t npulses = (Double_t)fCurrentrun->GetNpulses();
  Double_t nprompt;
  Double_t occupancy;
  Double_t occupancyerr;


  // -----------------------------------------------------------------
  // Calculate MPE-corrected occupancy (NB. valid only for occ<100% !)
  // -----------------------------------------------------------------
  // 01.2007 - O.Simard
  // Changed the occupancy definition to be npulses-free.
  // This has two main advantages:
  //  o) The occupancy is now a fraction, not an absolute number of counts
  //  o) The fit time in QOCAFit is reduced since npulses is removed
  //     from all the normalization and occupancies. In doing this the
  //     calculations are always performed with numbers that are of the same
  //     order.
  // Note: if you'd like to revert this you can but make sure QOCAFit is
  //       compatible. In the future there should be some sort of flag to
  //       decide that but I think given the results are the same we should
  //       stick with this one for now. This is an Aksel-approved move.
  //
  nprompt = (Double_t)ocapmt->GetNprompt();
  // Comments below are old occupancies 
  // (vary a lot from run-to-run depending on npulses)
  //occupancy = - npulses * log(1.0 - nprompt/npulses);  // MPE corrected
  //occupancyerr = sqrt(nprompt)/(1.0 - nprompt/npulses); // counting statistics
  //ocapmt->SetOccCorrection((Float_t)(occupancy/nprompt));
  //
  // Below are new occupancies, always between [0,1].
  occupancy = -log(1.0 - nprompt/npulses);  // MPE corrected
  occupancyerr = sqrt(nprompt)/(npulses - nprompt); // counting statistics

  ocapmt->SetOccupancy(occupancy);
  ocapmt->SetOccupancyerr(occupancyerr);
  // save the MPE occupancy correction (new/old)
  ocapmt->SetOccCorrection((Float_t)(occupancy*npulses/nprompt));

  // ----------------------------------------------------------------------
  // New for Version 3 of QOCAPmt: array or occupancies over various time
  // windows.  The .rdt file already has a 2D histogram covering 1 to 25 ns
  // half-widths.  Store the following:
  // 1,2,3,4,5,6,7,8,9,10,15,20 ns half-widths in QOCAPmt (12 entries)
  // 9-Nov-2000 - Bryce Moffat
  // ----------------------------------------------------------------------
  Int_t ibin;
  Int_t timebin[7] = {1,2,3,4,7,10,20};
  ocapmt->SetNtimeW(7);
  for (ibin=0; ibin<7; ibin++) {
	nprompt = qrdt->GetOccWindowPmt((Float_t)pmtn,(Float_t)(timebin[ibin]));
	//occupancy = - npulses * log(1.0 - nprompt/npulses);  // MPE corrected
	occupancy = - log(1.0 - nprompt/npulses);  // MPE corrected
	ocapmt->SetOccupancyW(ibin,occupancy);
  }


  // ------------------------------------------------------------
  // Set Occratio & friends to sensible values before calculation
  // ------------------------------------------------------------
  ocapmt->SetOccratio(1.0);
  ocapmt->SetOccratioerr(1.0);
  ocapmt->SetGeomratio(1.0);
  ocapmt->SetGeomratioerr(1.0);
  ocapmt->SetModel(1.0);
  ocapmt->SetModelerr(1.0);
  //ocapmt->SetDd(0.0);
  //ocapmt->SetDh(0.0);
  //ocapmt->SetDa(0.0);
  ocapmt->SetDd2o(0.0);
  ocapmt->SetDh2o(0.0);
  ocapmt->SetDacr(0.0);

  //ocapmt->SetLbmask(fQLaserballUtil->LBMask(cos(ocapmt->GetLasertheta()),0));
  //ocapmt->SetLaserlight(1.0);

  return 1;
}
//______________________________________________________________________________
Bool_t QPath::IsInvalidQRdt(QRdt *qrdt)
{
  // Check if QRdt contains the minimum information for FillFromRdt().

  //
  // Is the pointer valid?
  // 
  if (!qrdt) {
	printf("Warning in QPath::IsInvalidQRdt():\n");
	printf("\tInvalid pointer to QRdt object\n");
	return kTRUE;
  }

  // 
  // Is a file even loaded?
  // 
  if (!qrdt->GetRdtLoaded()) {
	printf("Warning in QPath::IsInvalidQRdt():\n");
	printf("\tNo .rdt file loaded in qrdt.\n");
	return kTRUE;
  }

  // 
  // Contains the run number.
  //
  Int_t runnumber = qrdt->GetRdtRunnumber();
  if (!runnumber) {
	printf("Warning in QPath::IsInvalidQRdt():\n");
	printf("\tQRdt does not contain run number.\n");
	printf("\tUse QRdt::SetRdtRunnumber()\n");
	return kTRUE;
  }

  //
  // Contains the important histograms.
  //
  if (!qrdt->IsFilled("Occupancy")) {
	fprintf(stderr,"No Counts histogram in QRdt.\n");
	return kTRUE;
  }
  if (!qrdt->IsFilled("TimePeak")) {
	fprintf(stderr,"No Peak histogram in QRdt.\n");
	return kTRUE;
  }
  if (!qrdt->IsFilled("ToF")) {
	fprintf(stderr,"No Time-of-flight histogram in QRdt.\n");
	return kTRUE;
  }
  if (!qrdt->IsFilled("TimeWidth")) {
	fprintf(stderr,"No Width histogram in QRdt.\n");
	return kTRUE;
  }
  if (!qrdt->IsFilled("OccWindowPmt")) {
	fprintf(stderr,"No Variable-windowed histogram in QRdt.\n");
	return kTRUE;
  }

  // 
  // Contains the number of pulses.  This just comes from the number of
  // entries in the occupancy histogram.  So, if the occupancy histogram
  // exists (the condition is above) and npulses does not, then something
  // very strange is going on.
  //
  if (!qrdt->IsFilled("Npulses")) {
	fprintf(stderr,"No Npulses in QRdt.\n");
	fprintf(stderr,"\tSomething really funny is going on...\n");
	return kTRUE;
  }

  return kFALSE;
}
//______________________________________________________________________________
TString QPath::MakeDqxxName(const Int_t runnumber)
{
  // Wrapper for MakeDqxxName(runnumber,stringcase).  Looks for both cases.

  TString dqxxName = "";  // .dqxx filename with path
        
  // Check if the directory has been set. 
  if (fDqxxdir == "") {
        fprintf(stderr,"Needs DQXX Banks to work properly.\n");
        fprintf(stderr,"Set the dqxx file directory by SetDqxxdir().\n");
        return dqxxName;
  }
  
  QRdt* filecheck = new QRdt();
  
  dqxxName = MakeDqxxName(runnumber,"lowercase");
  if (filecheck->FileExists(dqxxName)) return dqxxName;
  printf("Warning in QPath::MakeDqxxName():\n");
  printf("\tDQXX file %s does not exist.\n",dqxxName.Data());
  printf("\tTrying uppercase.\n");

  dqxxName = MakeDqxxName(runnumber,"uppercase");
  if (filecheck->FileExists(dqxxName)) return dqxxName;
  printf("Warning in QPath::MakeDqxxName():\n");
  printf("\tDQXX file %s does not exist.\n",dqxxName.Data());
 
  delete filecheck;

  return "";
}
//______________________________________________________________________________
TString QPath::MakeDqxxName(const Int_t runnumber, const TString stringcase)
{

  TString dqxxName = "";  // .dqxx filename with path

  // Check if the directory has been set.
  if (fDqxxdir == "") {
        fprintf(stderr,"Needs DQXX Banks to work properly.\n");
        fprintf(stderr,"Set the dqxx file directory by SetDqxxdir().\n");
        return dqxxName;
  }

  TString scase = stringcase;
  scase.Remove(2);
  scase.ToLower();
  
  Char_t fdqxx[1024];
  if (scase == "lo") { // lowercase
  	sprintf(fdqxx,"dqxx_%.10d.dat",abs(runnumber));
  } else if (scase == "up") { // uppercase
	sprintf(fdqxx,"DQXX_%.10d.dat",abs(runnumber));
  } else {
	fprintf(stderr,"Warning from QPath::MakeDqxxName():\n");
	fprintf(stderr,"\tInvalid or no case specified.\n");
	fprintf(stderr,"\tUsing default.\n");
	sprintf(fdqxx,"dqxx_%.10d.dat",abs(runnumber));
  }
	
  dqxxName += fDqxxdir;
  dqxxName += "/";
  dqxxName += fdqxx;

  return dqxxName;

}
//______________________________________________________________________________
QOCATree *QPath::CalculatePositions(QOCATree *ocatree,
				    const Char_t *posname,const Char_t *postitle)
{
  // Fit the run positions and produce an output tree with fit positions stored
  // along with the rest of the run data in ocatree.
  //
  // 22-Nov-2000 - Bryce Moffat:
  // Monte Carlo runs have the D2O group velocity for light used to do the
  // time-of-flight correction in the rch histograms.
  // Data runs have the phase velocity = c/n used to correct for time-of-flight
  // in the rch histograms.
  //
  // 18-Dec-2000 - Bryce Moffat
  // Given laserball positions from somewhere else (eg. 500nm runs at same
  // positions), store it locally in TVector3 fRunpositions[] (see
  // QPath::GetLBPositions()), and reuse here as the laserball fLaserx,y,z
  // position instead of fManipxyz, fFitxyz or fQxyz positions.
  //
  // If fits are requested, the positions are nonetheless forced to be the ones
  // given in fRunpositions[] so that position comparisons can be made at various
  // wavelengths.  Does this make sense?
  //
  // 28-Dec-2000
  // Split from original FillFromRDT() which did everything: extract data from
  // .rdt files, fit positions and calculate paths and occratios.  The split is
  // to allow various systematics to be studied more efficiently.  That is, without
  // rereading and recalculating everything from scratch for each systematic.
  //
  // 22-Mar-2001
  // When auto-saving, ROOT writes the new QOCATree with positions with the default
  // name.  This must be supplied by the user, and can't simply be given later with
  // a call to ocatree->Write("new_name").  So, for large QOCATree's (at least),
  // specify a unique name and title for the position QOCATree.
  //
  // 07-Oct-2004 - Olivier Simard
  // Perform basic checks to avoid re-doing the position fits using QOptics.
  // The positions get repeated from FillFromRDT() when applicable but this is
  // necessary to keep a similar output.
  //
  // 05.2006 - O.Simard
  // fQOpticsX,Y,Z are now initialized here and used by the fitting function. 
  // If the positions are fitted without calling this function then the fitter
  // won't run because the QOpticsX,Y,Z instances are NULL.
  // These instances do not need to run in normal mode since only the distances
  // are used - the fast flag is set for all of them.

  if (!ocatree) {
    printf("Must specify a valid QOCATree pointer to QPath::CalculatePositions().\n");
    return NULL;
  }

  // Creates the X,Y,Z QOptics instances
  if(!fQOpticsX) { fQOpticsX = new QOptics(); fQOpticsX->SetFastMode(1,0); }
  if(!fQOpticsY) { fQOpticsY = new QOptics(); fQOpticsY->SetFastMode(1,0); }
  if(!fQOpticsZ) { fQOpticsZ = new QOptics(); fQOpticsZ->SetFastMode(1,0); }

  // set the PSUP centre to what fQOptics is set to
  SetPSUPCentre(GetPSUPCentre()); // set centre for fQOpticsX,Y,Z

  QOCARun *ocarun = ocatree->GetQOCARun();
  Int_t nruns = (Int_t) ocatree->GetEntries();
  // Copy header from existing OCA tree, the data members to be fleshed out below
  QOCATree *calctree = new QOCATree(posname,postitle);
  QOCARun *ocaruncalc = calctree->GetQOCARun(); // for storage when changing entries

  Int_t jrun;

  for (jrun=0; jrun<nruns; jrun++) {
    ocatree->GetEntry(jrun);
    *ocaruncalc = *ocarun;
    ocaruncalc->CopyPMTInfo(ocarun);

    // check first if there is a position tree available
    // if yes, load position and skip
    if(fRunposlist && fRunposlist[jrun]!=0 && fRunpositions[jrun].Mag()<1000.0){
      ocaruncalc->SetLaserxyz(fRunpositions[jrun]);
      printf("Forcing position: %10.3f %10.3f %10.3f for Run %d\n",
	       ocaruncalc->GetLaserx(),ocaruncalc->GetLasery(),ocaruncalc->GetLaserz(),
	       ocaruncalc->GetRun());
      calctree->Fill();
      continue;
    }

    // fFitLBPosition can possibly be:
    // 0 : gentle mode :
    //     check the rdt file and fit only if nothing is found.
    // 1 : force direct line fit
    // 2 : force full path fit
    // 3 : brutal mode :
    //     force both direct + full path fits

    // enable the option to force the fits even if
    // already written in the rdt file.
    Bool_t forcefits = kFALSE;
    Int_t printlbpos = fFitLBPosition;
    if(fFitLBPosition > 0) forcefits = kTRUE;
    if(fFitLBPosition == 3) printlbpos--;
    if(forcefits && fPrint>=1) printf("-------\nAttempting fit methods up to %d\n",printlbpos);

    // check if the rdt files are already filled with fits. -- os

    // direct line fit
    Bool_t dirfit = (Bool_t) ((ocaruncalc->GetFitx() == -999) ||
    				(ocaruncalc->GetFity() == -999) ||
				(ocaruncalc->GetFitz() == -999));

    if((forcefits && ((fFitLBPosition == 1) || (fFitLBPosition == 3)))|| dirfit){
      if(fPrint>=1) printf("-------\nAttempting direct fit method.\n");
      FitLBPosition(ocaruncalc,1); // index 1 for direct line fit
    }

    // full path fit
    Bool_t fullfit = (Bool_t) ((ocaruncalc->GetQFitx() == -999) ||
    				(ocaruncalc->GetQFity() == -999) ||
				(ocaruncalc->GetQFitz() == -999));

    if((forcefits && (fFitLBPosition >= 2))|| fullfit){
      if(fPrint>=1) printf("-------\nAttempting full path fit method.\n");
      FitLBPosition(ocaruncalc,2); // index 2 for full path fit
    }

    // we got here, everything has been found -- os
    ocaruncalc->SetLaserpos(2); // use FullFitPos position as laserx,y,z position
    if(!forcefits && !dirfit && !fullfit){
      if(fPrint>=1) printf("Using FullFitPos position: %10.3f %10.3f %10.3f for Run %d from .rdt file\n",
	ocaruncalc->GetLaserx(),ocaruncalc->GetLasery(),ocaruncalc->GetLaserz(),
	ocaruncalc->GetRun());
    }

    //--------------------
    // Are the run positions to be fixed to externally determined values?
    // This is meant to allow fits to be done at a single wavelength for all
    // laserball positions, and then use those positions for all wavelengths.
    // It is hoped that this will produce systematics involved with fits at only
    // a single wavelength, rather than several systematics at each wavelength...
    //
    // Some runs may be requested but not present, in which case we stick with
    // the run position found here for this ocaruncalc!  (This is the meaning of the
    // test on fRunpositions[jrun].Mag() < 1000.0, since this vector is way out
    // if the run couldn't be found.
    //
    // 18-Dec-2000 - Bryce Moffat

    calctree->Fill(); // Fill the latest run (with calculated position fits) in tree

  }

  // get rid of the X,Y,Z QOptics instances
  delete fQOpticsX; fQOpticsX = NULL;
  delete fQOpticsY; fQOpticsY = NULL;
  delete fQOpticsZ; fQOpticsZ = NULL;

  return calctree;
}

//______________________________________________________________________________
QOCATree *QPath::CalculatePaths(QOCATree *ocatree,
				const Char_t *pathname,const Char_t *pathtitle)
{
  // Use QOptics to extract the paths from fLaserx,y,z in each run, and use
  // central runs for path differences.
  //
  // Calculate all the paths from the laserball to the PMT's for the runs
  // in the fRunlist[].
  //
  // 28-Dec-2000
  // Split from original FillFromRDT() which did everything: extract data from
  // .rdt files, fit positions and calculate paths and occratios.  The split is
  // to allow various systematics to be studied more efficiently.  That is, without
  // rereading and recalculating everything from scratch for each systematic.
  //
  // 22-Mar-2001
  // When auto-saving, ROOT writes the new QOCATree with paths with the default
  // name.  This must be supplied by the user, and can't simply be given later with
  // a call to ocatree->Write("new_name").  So, for large QOCATree's (at least),
  // specify a unique name and title for the path QOCATree.
  //
  // 05.2006 - O.Simard
  // Introduced the calculation of the Pmt occupancy correction due to both MPE
  // and NCD reflections.
  // NCD reflection probabilities are obtained from QOCAReflect.
  // Moved Pmt solid angle calculation to QOptics, which is better equipped.
  //
  // 07.2006 - O.Simard
  // Added Ncd Reflectivity scale factor that can bet set by the user to alter
  // the value returned by QOptics.
  // Added extra logic to avoid the "re-masking" of the PMTs. The way it works
  // is the following: 
  // If the input QOCATree ocatree contains Bad flags that we want to preserve,
  // then fReMask = kFALSE will copy them from the original tree (ocatree). 
  // Otherwise, when fReMask = kTRUE, the new path calculations update the Bad
  // flags as well. Not used for data, but really useful for systematics.
  //
  // 01.2007 - O.Simard
  // Modified slightly the NCD reflection correction part to make it run faster:
  // now it only corrects occupancies that will make it to the fit. Making
  // corrections to the rest of the PMTs is useless anyway.

  if (!ocatree) {
    printf("Must specify a valid QOCATree pointer to QPath::CalculatePaths().\n");
    return NULL;
  }

  QOCARun *ocarun = ocatree->GetQOCARun();  // for storage when retrieved from the tree
  Int_t nruns = (Int_t) ocatree->GetEntries();
  QOCATree *calctree = new QOCATree(pathname,pathtitle);
  QOCAPmt *ocapmtcalc = new QOCAPmt();
  QOCARun *ocaruncalc = calctree->GetQOCARun(); // for storage when calculating tree

  Int_t jrun,ipmt;
  Int_t pmtn,panel;
  Double_t dd2o,dh2o,dacr,cospmt,transpwr;

  TVector3 zunit(0,0,1);          // z-axis (constant)
  TVector3 pmtrelvec(0,0,850);    // vector direction of light from laserball towards PMT
  TVector3 pmtincidentvec(0,0,0); // vector direction of light incident on PMT

  TVector3 srcvec(0,0,0);         // Unmodified source coordinates
  TVector3 srcvecmod(0,0,0);      // Modified source coordinates - systematics test!

  TVector3 av(0,0,0);             // AV intersection of main ray to PMT
  TVector3 avn;                   // AV normal (pointing outwards)
  TVector3 pmtvec;                // PMT central coordinates
  Double_t costhetabar;           // Average cos(theta_PMT) from four offset points
  Double_t cosav;                 // Cos(theta_AV) of intersection of main ray to PMT
  Double_t solidangle;            // Relative solid angle of PMT from four offset points

  QOCAReflect* hr = NULL;         // Declare this QOCAReflect to find reflection paths

  for (jrun=0; jrun<nruns; jrun++) {
    ocatree->GetEntry(jrun);
    *ocaruncalc = *ocarun;  // Copy the main data entries, not the PMTs' array

    fQOptics->SetSource(ocarun->GetLaserx(),ocarun->GetLasery(),ocarun->GetLaserz());

    printf("----------------------------------------\n");
    fQOptics->SetIndices(ocarun->GetLambda());

    //--------------------
    // Calculate the optical paths, including position systematics.
    //
    // These systematics can be probed:
    //  + laserball position shifts (constant over all runs, or a smear for each run)
    //      - in x,y,z coordinates (or combinations)
    //      - in radius
    //  + laserball effective light emission centre (for solid angle calculation,
    //      this moves the average position of light emission along the vector
    //      joining the ball to the PMT)
    //
    // Originally, done by hand:
    // R' = 1.025 R systematic trial
    // 30-Oct-2000 - Bryce Moffat
    
    srcvec = fQOptics->GetSource();
    srcvecmod = srcvec;
    
    if (fPositionsyst == -1) {  // Smear
      if (fXsyst!=0) srcvecmod.SetX(srcvec.X() + gRandom->Gaus(0,fXsyst));
      if (fYsyst!=0) srcvecmod.SetY(srcvec.Y() + gRandom->Gaus(0,fYsyst));
      if (fZsyst!=0) srcvecmod.SetZ(srcvec.Z() + gRandom->Gaus(0,fZsyst));
      if (fRsyst!=0) srcvecmod.SetMag(srcvec.Mag() + gRandom->Gaus(0,fRsyst));
    } else if (fPositionsyst == 1) { // Shift
      if (fXsyst!=0) srcvecmod.SetX(srcvec.X() + fXsyst);
      if (fYsyst!=0) srcvecmod.SetY(srcvec.Y() + fYsyst);
      if (fZsyst!=0) srcvecmod.SetZ(srcvec.Z() + fZsyst);
      if (fRsyst!=0) srcvecmod.SetMag(srcvec.Mag() + fRsyst);
    } else if (fPositionsyst == 2) { // Multiply
      if (fXsyst!=0) srcvecmod.SetX(srcvec.X() * fXsyst);
      if (fYsyst!=0) srcvecmod.SetY(srcvec.Y() * fYsyst);
      if (fZsyst!=0) srcvecmod.SetZ(srcvec.Z() * fZsyst);
      if (fRsyst!=0) srcvecmod.SetMag(srcvec.Mag() * fRsyst);
    }
    
    if (fPositionsyst != 0) {
      ocaruncalc->SetLaserxyz(srcvecmod);
      fQOptics->SetSource(srcvecmod);
      srcvec = srcvecmod;
    }

    printf("Calculating paths for run %d (run %d out of %d).\n",
	   ocaruncalc->GetRun(),
	   jrun+1,nruns);         // offset +1 for [1..nruns] not [0..nruns-1]
    printf("Source at %6.1f %6.1f %6.1f\n",ocaruncalc->GetLaserx(),
	   ocaruncalc->GetLasery(), ocaruncalc->GetLaserz());

    if (fBallsyst != 0)
      printf("Ball systematic of %g will be added to each PMT.\n",fBallsyst);

    // ------------------------------------------------------------------
    // -- os 27.05.2004
    //
    // QOCAReflect calculations are done before the pmt loop. It needs to
    // be done once per high radius run (runs with radius < 450 cm will
    // be ignored for now).
    // The main function QOCAReflect::GetCThetapsRange() returns the two
    // extreme values of the CosThetaps parameter
    // (see QOptics::IsPromptAVRefl()).
    // Avoid creating QOCAReflect instances if not used.
    //
    // AV reflections are automatically checked for systematics, except
    // for the ball systematic.
    //
    // -- os 24.02.2005
    // Modified to enable storage of the parameters in a QOCARun object.

    if(
       ((fQOptics->GetSource()).Mag() >= 450.) // high radius runs only
       && (fBallsyst == 0) // don't need it for ball systematics
       ){
      // check in the current ocarun if the information is stored
      if((ocarun->GetCosThetapsMin() != 999.) &&
         (ocarun->GetCosThetapsMax() != -999.)
        ){
        fQOptics->SetCosThetapsMin((Double_t) ocarun->GetCosThetapsMin());
        fQOptics->SetCosThetapsMax((Double_t) ocarun->GetCosThetapsMax());
        ocaruncalc->SetCosThetapsMin((Float_t) ocarun->GetCosThetapsMin());
        ocaruncalc->SetCosThetapsMax((Float_t) ocarun->GetCosThetapsMax());
      }
      else{
        hr = new QOCAReflect(); // initialize the object: creates a qoptics instance
	hr->SetFastMode(1,1); // use internal QOptics instance in fast mode
        hr->SetSelect(1); // only consider "dangerous" reflections
        hr->SetWavelength(fQOptics->GetWavelength());
        hr->SetTimeWindow((Double_t) fTimewinPavr); // must be set by the user
        if(hr->SetSourceVector(fQOptics->GetSource())){
	  Double_t* thps = hr->GetCThetapsRange();
	  // set it in QOptics
	  fQOptics->SetCosThetapsMin((Double_t) thps[0]);
	  fQOptics->SetCosThetapsMax((Double_t) thps[1]);
	  // save it in QOCARun
	  ocaruncalc->SetCosThetapsMin((Float_t) thps[0]);
	  ocaruncalc->SetCosThetapsMax((Float_t) thps[1]);
        }
        delete hr; hr = NULL; // clear memory and get rid of a qoptics instance
      }
    }

    // 05.2006 - O.Simard
    // Check for Ncd reflections if the Ncd flag has been enabled.
    // The calculations takes some time even though QOptics runs in fast mode. 
    //   - call GetDistances(pmtn) a lot 
    //   - only do it once : check for the flag fNcdReflectionDone
    
    // This part only does preliminary setup for individual pmt corrections
    if(!fNcdReflectionDone && GetFlagNcd()){
      hr = new QOCAReflect(); // initialize the object: creates a qoptics instance
      hr->SetFastMode(1,1); // use internal QOptics instance in fast mode
      hr->SetWavelength(fQOptics->GetWavelength());
      hr->SetTimeWindow((Double_t) fTimewinPavr); // must be set by the user
      hr->SetSourceVector(fQOptics->GetSource());
      hr->SetStepZ(10); // 10 cm stepz for scanning Ncds
    }

    // redefine these variable for combined Pmt occupancy correction:
    //  - use both MPE and Ncd reflections
    Double_t npulses = (Double_t)ocarun->GetNpulses();
    Double_t nprompt = 0;
    Double_t occupancy = 0, oldoccupancy = 1;
    Char_t* functionequation = new Char_t[1024];

    // ------------------------------------------------------------------

    // Calculate the paths for each PMT in the run.
    for (ipmt=0; ipmt<ocarun->GetNpmt(); ipmt++) {
      *ocapmtcalc = *(ocarun->GetPMT(ipmt));  // Make a copy to modify
      fCurrentpmt = ocapmtcalc;

      pmtn = fCurrentpmt->GetPmtn();

      // Ball systematic: where is the "effective optical centre" of the
      // laserball?  Move the optical centre a distance fBallsyst along the
      // vector from the source to the PMT.
      if (fBallsyst != 0) {
	srcvec = (ocaruncalc->GetLaserxyz());
	fQOptics->SetSource(srcvec);  // This must be reset on each iteration

	if (fQOptics->GetDistances(pmtn,dd2o,dacr,dh2o,cospmt,panel,transpwr)) {
	  pmtrelvec = fQOptics->GetPMTrelvec();
	  srcvecmod = srcvec + pmtrelvec * fBallsyst;
	  
	  fQOptics->SetSource(srcvecmod);
	  srcvec = srcvecmod;
	}
      }

      // Get optical path information from QOptics
      if (fQOptics->GetDistances(pmtn,dd2o,dacr,dh2o,cospmt,panel,transpwr)) {

	pmtrelvec = fQOptics->GetPMTrelvec();
	// 11.2005 - O.Simard
	// Save the "pmtrelvec" vector information in QOCAPmt for QNCDPositionFit
	fCurrentpmt->SetPMTrelvecX(pmtrelvec.X());
	fCurrentpmt->SetPMTrelvecY(pmtrelvec.Y());
	fCurrentpmt->SetPMTrelvecZ(pmtrelvec.Z());
	pmtincidentvec = fQOptics->GetPMTincident();
	
	fCurrentpmt->SetLasertheta(pmtrelvec.Theta());
	if (fCurrentpmt->GetLasertheta() < 0) { 
        // Line below was a bug (inconsequential) -- rsd 2002.06.03
        //  fCurrentpmt->SetLaserphi(fCurrentpmt->GetLasertheta() + 2*TMath::Pi());
	  fCurrentpmt->SetLasertheta(fCurrentpmt->GetLasertheta() + 2*TMath::Pi());
	}
	
	fCurrentpmt->SetLaserphi(pmtrelvec.Phi());
	if (fCurrentpmt->GetLaserphi() < 0)
	  fCurrentpmt->SetLaserphi(fCurrentpmt->GetLaserphi() + 2*TMath::Pi());
	
	fCurrentpmt->SetCospmt(cospmt);
	//fCurrentpmt->SetPmtang(fQOptics->GetPMTAngResp());
	fCurrentpmt->SetTranspwr(transpwr);
	
	fCurrentpmt->SetDd2o(dd2o);
	fCurrentpmt->SetDacr(dacr);
	fCurrentpmt->SetDh2o(dh2o);
	
	// Modified total distance to take into account extra distance
	// through PMT bucket.
	// 31-Oct-2000 - Bryce Moffat
	// Taken out - 1-Nov-2000 - Bryce Moffat - invalid for Monte Carlo!
	//	    fCurrentpmt->SetDpmt(dd2o + dacr + dh2o + 8.0/cospmt);
	fCurrentpmt->SetDpmt(dd2o + dacr + dh2o);
	fCurrentpmt->SetNcdMinDist(fQOptics->GetNcdMinDist());	
	fCurrentpmt->SetCounterIndex(fQOptics->GetCounterIndex());	
	fCurrentpmt->SetPanel(panel);
	fCurrentpmt->SetBelly(fQOptics->IsBelly());
	fCurrentpmt->SetNeck(fQOptics->IsNeckOptics());
	fCurrentpmt->SetRope(fQOptics->IsRope());
	fCurrentpmt->SetPipe(fQOptics->IsPipe());
	fCurrentpmt->SetNcd(fQOptics->IsNcd());
	
	// New cuts - NCD anchors and NCD ropes
	fCurrentpmt->SetNcdRope(fQOptics->IsNcdRope());
	fCurrentpmt->SetAnchor(fQOptics->IsAnchor());
	

	fCurrentpmt->SetOtherbad(fQOptics->IsPromptAVRefl() || fQOptics->IsAnchor());

	// incident angle on the AV
	av = fQOptics->GetAcrylic();
	srcvec = fQOptics->GetSource();
	cosav = (av - srcvec).Unit() * av.Unit();
	fCurrentpmt->SetCosav(cosav);

	// Calculate solid angle of PMT as viewed from the laserball:
	// absolute solid angle from new QOptics
	fQOptics->SetFastMode(1,0); // do not reset the optical flags
	solidangle = fQOptics->GetPmtSolidAngle(pmtn);
	fQOptics->SetFastMode(0,0); // reset for next PMT

	// divide this value by the "average" value at the center.
	// this is only to get a number close to one (not really efficient though)
	fCurrentpmt->SetSolidangle(solidangle/(Double_t)8.029e-04);

	// costhetabar is set internally in previous call
	costhetabar = fQOptics->GetCthetaBar();
	fCurrentpmt->SetCospmtavg(costhetabar);

	// =================================
	//      Bad Flag is set here
	// =================================
	if(GetFlagNcd()) {
	  
	  // check if we need to remask with the updated Bad
	  if(fReMask){
	    // include ncd mask in the Bad mask
	    fCurrentpmt->SetBad(fQOptics->IsBelly() || fQOptics->IsNeckOptics() ||
				fQOptics->IsRope() || fQOptics->IsPipe() ||
				fQOptics->IsPromptAVRefl() || fQOptics->IsNcd() ||
				fQOptics->IsNcdRope() || fQOptics->IsAnchor());
	  }

	  // 01.2007 - O.Simard
	  // Due to the calculation length of the NCD reflection factor, only
	  // calculate those that were not determined to be shadowed using fQOptics->IsNcd().
	  Double_t ncdref_coef = 0.;
	  if(fNcdReflectionDone) ncdref_coef = fCurrentpmt->GetNcdReflCoef();
	  else {
	    if(!fQOptics->IsNcd()){
	    
	      // -------------------
	      // 05.2006 - O.Simard
	      // Create a TF1 using for occupancy correction using both 
	      // individual pmt and run info.
	      // Note: the correction is made for all Pmts, whether they are
	      // shadowed or not. The fBad flag will remove them anyway from the fit.
	      //
	      // The scale factor fNcdReflectivityScale is used to control the Ncd
	      // reflectivity provided by QOptics. It should be changed only when
	      // evaluating the systematics associated to the Ncd reflections.
	      //
	      ncdref_coef = hr->GetNCDReflectionFactor(pmtn);
	      
	      // 01.2007 - O.Simard
	      // The Ncd reflection coefficient needs to be normalized by the solid angle
	      // of the PMT viewed from the source.
	      ncdref_coef *= (4*TMath::Pi()*fQOptics->GetNcdReflectivity()/solidangle);
	      
	    } // if(!fQOptics->IsNcd()) 
	  } // else
	  
	  // save the Ncd reflection coefficient
	  fCurrentpmt->SetNcdReflCoef(ncdref_coef);

	  // then proceed with the combined occupancy correction (MPE + NCDR) at first order
	  nprompt = (Double_t)fCurrentpmt->GetNprompt();
	  sprintf(functionequation,"1+exp(-x)*((%.6e)*x-1)",fNcdReflectivityScale*ncdref_coef); // string
	  // define the TF1 between [0,1] (0 to 100% occupancy)
	  TF1* OccCorrectionTF1 = new TF1("OccupancyCorrection",(Char_t*)functionequation,0,1);
	  // correction of the occupancy
	  //occupancy = npulses*((Double_t)OccCorrectionTF1->GetX((Double_t)(nprompt/npulses),0,1));
	  occupancy = (Double_t)OccCorrectionTF1->GetX((Double_t)(nprompt/npulses),0,1);
	  // overwrite occupancy in fCurrentpmt
	  //oldoccupancy = fCurrentpmt->GetOccupancy();
	  if(fCurrentpmt->GetOccCorrection() > 0) {
	    oldoccupancy = fCurrentpmt->GetOccupancy()/fCurrentpmt->GetOccCorrection();
	  }
	  if(occupancy > 0) fCurrentpmt->SetOccupancy(occupancy);
	  // save this additional correction into the cumulative correction (new/old)
	  if(oldoccupancy > 0) fCurrentpmt->SetOccCorrection((Float_t)(occupancy/oldoccupancy));
	  delete OccCorrectionTF1;

	} else {
	  // check if we need to remask with the updated Bad
	  if(fReMask){
	    // don't include the ncd mask. this is the default (for now) jm 11/11/03
	    fCurrentpmt->SetBad(fQOptics->IsBelly() || fQOptics->IsNeckOptics() ||
				fQOptics->IsRope() || fQOptics->IsPipe() ||
				fQOptics->IsPromptAVRefl() || fQOptics->IsAnchor());	
	  }
	}
	
	
	
      } else {
	fCurrentpmt->SetOtherbad(kTRUE); // Path could not be calculated!
	fCurrentpmt->SetBad(kTRUE);
      }
      ocaruncalc->AddPMT(fCurrentpmt);
    }
    printf("Number of pmt in ocarun    : %d\n",ocarun->GetNpmt());
    printf("Number of pmt in ocaruncalc: %d\n",ocaruncalc->GetNpmt());

    calctree->Fill();

    delete functionequation;
    if(hr) {delete hr; hr = NULL;}

  }
     
  // set this here after the run loop 
  if(!fNcdReflectionDone) fNcdReflectionDone = kTRUE;

  delete ocapmtcalc; // Avoid clogging memory with obsolete objects

  return calctree;
}
//______________________________________________________________________________
Bool_t QPath::SetCentreRuns(QOCATree *ocatree)
{
  // Get and fill an array with central runs for the fRunlist/fRuncentrelist
  // from the ocatree.
  //
  // An entry of fCentrerunlist[i] = 0 implies that no central run should be
  // used for that fRunlist[i].  The pointer to the central run will be
  // stored as NULL for that run.
  //
  // If any central runs can't be found, return kFALSE.

  Int_t i,j;
  Bool_t foundallruns = kTRUE;

  if (!ocatree) {
    printf("Must specify a valid QOCATree pointer to QPath::SetCentreRuns().\n");
    return kFALSE;
  }

  // Only delete the array of pointers, not the objects!
  // delete fCentrerunptr[i]; erases the run from the QOCATree! Not desired!
  // 27-Dec-Moffat
  // Reversed 29-Dec-2000 - Moffat - SetCentreRuns() allocates new QOCARun's for these!
  if (fCentrerunptr) {
    printf("Deleting %d centre run pointers\n",fNumberofcentre);
    for (i=0; i<fNumberofcentre; i++) {
      if (fCentrerunptr[i]) {
	delete fCentrerunptr[i];
      	fCentrerunptr[i] = NULL;
      }
    }
    delete[] fCentrerunptr;
    fCentrerunptr = NULL;
  }

  QOCARun *ocarun = ocatree->GetQOCARun(); // storage when retrieved from the tree

  Int_t nruns = (Int_t)ocatree->GetEntries();

  fCentrerunptr = new QOCARun*[fNumberofcentre];  // ptr for each **distinct** run

  printf("----------------------------------------\n");

  for (i=0; i<fNumberofcentre; i++) {
    fCentrerunptr[i] = new QOCARun();  // Actual QOCARun for centre runs
    fCentrerunptr[i]->SetRun(fCentreruns[i]);

    if (fCentreruns[i] == 0) {  // Check if no central run requested
      printf("No central run for some runs (Centre run offset %d).\n",i);
    } else {
      // Try to get it from the tree next - including all pmt-level contents!
      ocatree->SetBranchStatus("*",0);
      ocatree->SetBranchStatus("fRun",1);

      for (j=0; j<nruns; j++) {
	ocatree->GetEntry(j);
	if (ocarun->GetRun() == fCentreruns[i]) {
	  printf("Found central run %d in ocatree at position %d\n",fCentreruns[i],j);
	  ocatree->SetBranchStatus("*",1);
	  ocatree->GetEntry(j);
	  *fCentrerunptr[i] = *ocarun;
	  fCentrerunptr[i]->CopyPMTInfo(ocarun);
	  break;  // get out of j loop searching for the run - it's been found!
	}
      }
      if (j == nruns) {
	// Reversed 29-Dec-2000 - Moffat - SetCentreRuns() allocates new QOCARun's
	delete fCentrerunptr[i];  // Don't erase from central QOCATree! 27-Dec-2000
	fCentrerunptr[i] = NULL; // Check if this is NULL before trying to use anyway!
	fprintf(stderr,"Can't find central run %d in QOCATree supplied to "
		"QPath::SetCentreRuns() !\n",fCentreruns[i]);
	foundallruns = kFALSE;
      }
    }
  }

  ocatree->SetBranchStatus("*",1);  // Restore full access

  // delete ocarun;  // Don't delete ocarun!  Kills entry in ocatree! 27-Dec-2000

  if (!foundallruns) fprintf(stderr,"Can't find all centre runs!\n");

  return foundallruns;
}
//______________________________________________________________________________
Int_t QPath::GetLBPositions(QOCATree *postree)
{
  // Get the run positions from another tree.
  // This is implemented to allow fitting of positions at one wavelength (500nm)
  // and using these fit positions for all other wavelengths done without
  // moving the manipulator.
  //
  // This routine attempts to extract fNumberofruns positions from the postree
  // and stores the results in TVector3 fRunpositions[] for use in
  // QPath::FillFromRDT().
  //
  // The return value is the number of runs which couldn't be located in postree,
  // and should be equal to 0 (zero) if all runs are found.
  //
  // 18-Dec-2000 - Bryce Moffat
  //
  // 02-06-2004 - Olivier Simard
  // Makes sure the postree is available before looping over entries. If not, it
  // goes automatically on "fits" mode (no entry needed).
  //


  if (!fRunposlist) return fNumberofruns; // No fit run numbers to look for? Quit.

  printf("----------------------------------------\n");
  printf("In QPath::GetLBPositions(): Looking for run positions from another QOCATree:\n");

  Int_t nmissing = fNumberofruns;
  Int_t i,j;

  QOCARun *ocarun;

  if(postree){
    ocarun = postree->GetQOCARun();
    printf("Position QOCATree %s opened.\n", postree->GetName());

    postree->SetBranchStatus("*",0);       // Only the run numbers and positions needed
    postree->SetBranchStatus("fRun",1);
    postree->SetBranchStatus("fLaserx",1);
    postree->SetBranchStatus("fLasery",1);
    postree->SetBranchStatus("fLaserz",1);

    for (i=0; i<fNumberofruns; i++) {
      fRunpositions[i].SetXYZ(-999,-999,-999);  // Default bogus value.
      if (fRunposlist[i] == 0) { // Some runs don't have 500nm runs at same positions.
	printf("No entry needed for run %d (%d): %d\n", i, fRunlist[i], fRunposlist[i]);
	nmissing--;
	continue;  // Skip to the next iteration in i
      }
      for (j=0; j<postree->GetEntries(); j++) {
	postree->GetEntry(j);
	
	if (ocarun->GetRun() == fRunposlist[i]) {
	  fRunpositions[i] = (ocarun->GetLaserxyz());
	  printf("For run %d (%d) found position run %d (%d)\n", i, fRunlist[i],
		 j, ocarun->GetRun());
	  nmissing--;

	  break; // Skip out of the iterations on j, back to i loop...
	}
      }
      if (j==postree->GetEntries()) {  // Bottomed out of the loop -> no run found!
	printf("For run %d (%d) there's no position run %d\n", i, fRunlist[i],
	       fRunposlist[i]);
      }
    }
    
    postree->SetBranchStatus("*",1);  // Restore full access
  }

  else{
    printf("Position QOCATree not found.\n");
    for (i=0; i<fNumberofruns; i++) {
      fRunpositions[i].SetXYZ(-999,-999,-999);  // Default bogus value.
      // Some runs don't have 500nm runs at same positions.
      if (fRunposlist[i] == 0) { 
	printf("No entry needed for run %d (%d): %d\n", 
	       i, fRunlist[i], fRunposlist[i]);
	nmissing--;
	continue;  // Skip to the next iteration in i
      }
    }
  }
  
  return nmissing;
}
//______________________________________________________________________________
QOCATree *QPath::CalculateOccratio(QOCATree *ocatree,
				   const Char_t *orname,const Char_t *ortitle)
{
  // Calculate the occupancy ratio compared to a set of centre runs and return
  // a new tree with the Occratio suite of variables filled.
  //
  // Each run may have an associated centre run, which is used to compute
  // the occupancy ratio ("occratio").  If no central run is associated with
  // a given run, occratio is left alone (put to a weird/default value).
  //
  // The norm of the central run is used as a quotient in the calculations,
  // to ensure that occratio is the occupancy divided by a number close to 1
  // from the central run.
  //
  // 9-Nov-2000 - Bryce Moffat
  // New for Version 3 of QOCAPmt: multiple time windows from the .rdt file
  // are stored.  This means occratio should be calculated for each case.
  //
  // 28-Dec-2000
  // Split from original FillFromRDT() which did everything: extract data from
  // .rdt files, fit positions and calculate paths and occratios.  The split is
  // to allow various systematics to be studied more efficiently.  That is, without
  // rereading and recalculating everything from scratch for each systematic.
  //
  // 22-Mar-2001
  // When auto-saving, ROOT writes the new QOCATree with calculated occupancy
  // ratios with the default name.
  // This must be supplied by the user, and can't simply be given later with
  // a call to ocatree->Write("new_name").  So, for large QOCATree's (at least),
  // specify a unique name and title.
  // 
  // 24-Sep-2003 Ranpal
  // Split routine up into sensible parts.
  //
  // 09.2005 - Olivier Simard
  // Moved the LoadPmteffTitles() call here so that the wavelength is known
  // and the right set of efficiencies loaded.
  // The occupancy fit uses this ("optix") tree anyway.
  //
  // 01.2007 - O.Simard
  // Recalculate the run normalization using fBad to make it match the PMTs
  // that are passed to QOCAFit.
   
  // -----------------------------------
  // Make sure ocatree pointer is valid.
  // -----------------------------------
  if (!ocatree) {
	printf("Warning in QPath::CalculateOccratio():\n");
	printf("\tInvalid QOCATree pointer\n");
	return NULL;
  }

  // -----------------------------------------------------
  // Make sure central runs are loaded for occratio. 
  // If necessary, try to load them from this tree itself.
  // -----------------------------------------------------
  if (!fCentrerunptr) {
	printf("Warning in QPath::CalculateOccratio():\n");
	printf("\tNo centre runs set.\n");
	printf("\tUsing this tree (%s: %s) to get central runs.\n",
		ocatree->GetName(),ocatree->GetTitle());
	SetCentreRuns(ocatree);
  }

  // -------------------------------------------------------
  // Load the efficiencies from the titles file before the
  // run loop. This is a single number per PMT for all runs,
  // to be stored in QOCAPmt::SetPmteffc().
  // Avoid re-loading for systematics trees.
  // -------------------------------------------------------
  if(!fEfficienciesLoaded) LoadPmteffTitles();

  // -------------------------------------------------------
  // Create new tree to hold runs with occratios calculated.
  // Copy header from existing OCA tree, (?)
  // the data members to be fleshed out below
  // -------------------------------------------------------
  QOCATree *calctree = new QOCATree(orname,ortitle);


  // ------------------------------------------
  // Set pointer for runs from off-centre tree.
  // ------------------------------------------
  QOCARun *orun = ocatree->GetQOCARun();  // for storage when retrieved from the tree


  // ------------------------------------------
  // Recalculate all normalizations using fBad
  // before sending the runs for occratio's.
  // ------------------------------------------
  printf("----------------------------------------\n");
  printf("Renormalization proceeding.\n");
  Int_t npmt_norm = 0;
  Float_t norm = 0;
  Int_t nrun = (Int_t) ocatree->GetEntries();
  for (Int_t irun=0; irun<nrun; irun++) {
	ocatree->GetEntry(irun);
	Int_t npmt = orun->GetNpmt();
	Float_t oldnorm = orun->GetNorm();
	npmt_norm = 0;
	norm = 0;
	// pmt loop
        for(Int_t ipmt = 0 ; ipmt < npmt ; ipmt++){
		QOCAPmt* pmt = orun->GetPMT(ipmt);
		if(!pmt || pmt->GetBasicBad() || pmt->GetBad() || (pmt->GetOccupancy() <= 0.)) continue;
		else {
		  npmt_norm++;
		  norm += pmt->GetOccupancy();
		}
	}
        // recalculate the normalization in the same way as in FillFromRDT
	if(npmt_norm > 0) {
	  norm /= npmt_norm;
	  if((1./norm) < 1e10) orun->SetNorm(1./norm);
	}
	printf("  Run %.2d (%d): New = %.2f (%d pmts), Old was %.2f (%d pmts), Difference of %.2f%%\n",
		irun,orun->GetRun(),1./norm,npmt_norm,oldnorm,npmt,100.*fabs(1./norm-oldnorm)*norm);
  }

  // -----------------------------------------------------
  // Loop over runs, computing the occratios for each run.  
  // Store in calctree if valid.
  // -----------------------------------------------------
  printf("----------------------------------------\n");
  printf("Occupancy ratio calculation proceeding.\n");
  nrun = (Int_t) ocatree->GetEntries();
  for (Int_t irun=0; irun<nrun; irun++) {
	
	// ----------------------------------------
	// Get the next off-centre and centre runs.
	// ----------------------------------------
	ocatree->GetEntry(irun);
	QOCARun *crun = fCentrerunptr[fCentrerunindex[irun]];

	// ---------------------------------
	// Calculate occratios for this run.
	// ---------------------------------
	printf("-------------- Run %d of %d --------------\n",irun+1,nrun);
	QOCARun *filledrun = (QOCARun *)CalculateOccratioRun(orun,crun);

	// ------------------------------------------------------------
	// Fill the latest run (with calculated pmt optics) in the tree
	// ------------------------------------------------------------
	if (filledrun) {
		calctree->SetQOCARun(filledrun);
		calctree->Fill();  
	} else printf("Run %d was not added to the tree.\n",irun+1);
  }

  return calctree; 
}
//______________________________________________________________________________
QOCARun *QPath::CalculateOccratioRun(QOCARun *orun, QOCARun *crun)
{
  // Calculate occupancy ratios, etc., for off-centre QOCARun orun and centre 
  // QOCARun crun.  Return filled QOCARun.  Returns NULL if the method was 
  // invoked invalidly.

  // ----------------------------
  // Check if pointers are valid.
  // ----------------------------
  if (!orun) {
	printf("Warning in QPath::CalculateOccratioRun():\n");
	printf("\tInvalid off-centre QOCARun pointer.\n");
	printf("\tSkipping.\n");
	return NULL;
  }
  if (!crun) {
	printf("Warning in QPath::CalculateOccratioRun():\n");
	printf("\tInvalid centre QOCARun pointer.\n");
	printf("\tSkipping.\n");
	return NULL;
  }


  // ------------------------
  // Check if runs are valid.
  // ------------------------
  Int_t orunnumber = orun->GetRun();
  if (orunnumber == 0) {
	printf("Warning in QPath::CalculateOccratioRun():\n");
	printf("\tInvalid QOCARun run number: %d\n",orunnumber);
	printf("\tSkipping.\n");
	return NULL;
  }
  Int_t crunnumber = crun->GetRun();
  if (crunnumber == 0) {
	printf("Warning in QPath::CalculateOccratioRun():\n");
	printf("\tInvalid central QOCARun run number: %d\n",orunnumber);
	printf("\tSkipping.\n");
	return NULL;
  }


  // ------------------------
  // Output start-of-routine.
  // ------------------------
  printf("Calculating occratios for run %d (%d/%d).\n", orunnumber, orunnumber, crunnumber);


  // ---------------------------------
  // Make QOptics object for this run.
  // ---------------------------------
  Double_t laserx = orun->GetLaserx();
  Double_t lasery = orun->GetLasery();
  Double_t laserz = orun->GetLaserz();
  fQOptics->SetSource(laserx,lasery,laserz);
  printf("Source at %6.1f %6.1f %6.1f\n",laserx,lasery,laserz);
  fQOptics->SetIndices(orun->GetLambda());

    
  // -----------------------------------
  // Get normalization for central run 
  // (part of CalculateOccratioPMT call)
  // -----------------------------------
  Double_t cnorm = crun->GetNorm(); 

  // ----------------------------------------------------
  // Produce an array of pmt numbers for the central run.
  // ----------------------------------------------------
  SetCentrerunPmtIndex(crun);


  // ------------------------------------------------
  // Copy the main data entries, not the PMTs' array,
  // into the new to-be-calculated QOCARun.
  // ------------------------------------------------
  QOCARun *run = new QOCARun();
  *run = *orun;  
  run->SetRuncentre(crunnumber);
  
  // -----------------------------------------------------------------
  // Loop over pmts, and calculating and filling the occupancy ratios.
  // -----------------------------------------------------------------
  Int_t pmtnsync = 0;
  Int_t nwarning = 0;
  Int_t kpmt = 0; // saves last pmt index returned 
  Int_t npmt = orun->GetNpmt();
  for (Int_t ipmt=0; ipmt<npmt; ipmt++) {
	QOCAPmt *opmt = (QOCAPmt *)orun->GetPMT(ipmt);  
	if (!opmt) continue; // invalid opmt pointer
	Int_t pmtn = opmt->GetPmtn();

	// -------------------
	// Find centre run pmt
	// -------------------
	Int_t jpmt = GetCentrerunPmtIndex(pmtn,kpmt);
	if(jpmt < 0) nwarning++; // if (jpmt<0) continue;

	QOCAPmt *cpmt = (QOCAPmt *)crun->GetPMT(jpmt);
		
	// -----------------------------------------
	// Output warning in case of misaligned pmts
	// -----------------------------------------
	Int_t newpmtnsync = (jpmt>=0) ? jpmt - ipmt : pmtnsync;
	if (newpmtnsync!=pmtnsync) {
	  if(fPrint>=2) printf("Warning: misaligned pmts at pmtn %d; %d and %d\n",pmtn,ipmt,jpmt);
	  pmtnsync = newpmtnsync;
	}

	// -------------------------------
	// Calculate occratio for this pmt
	// -------------------------------
	QOCAPmt *filledpmt = CalculateOccratioPMT(opmt,cpmt,cnorm);
	filledpmt->SetCrunPmtIndex(jpmt);

	// ----------------------------
	// Add it to run if it is good.
	// ----------------------------
	// os 11.2005 -- fixed memory leak by adding delete statement
	if (filledpmt) {run->AddPMT(filledpmt); delete filledpmt;}
	// os 12.2005 -- save returned index to speed up search in GetCentrerunPmtIndex()
	if(jpmt > 0) kpmt = jpmt+1;

  } // pmt loop
 
  // ------------------
  // Output statistics.
  // ------------------
  if(nwarning > 0) printf("Number of warnings: %d (this run has a difference of %d pmts)\n",
		   nwarning, abs(crun->GetNpmt() - orun->GetNpmt()));
  printf("Number of pmts in  input QOCARun: %d\n",orun->GetNpmt());
  printf("Number of pmts in output QOCARun: %d\n", run->GetNpmt());

  return run;
}
//______________________________________________________________________________
QOCAPmt *QPath::CalculateOccratioPMT(QOCAPmt *opmt, QOCAPmt *cpmt, Double_t cnorm)
{
  // Calculate occupancy ratio, etc., for off-centre QOCAPmt opmt and centre 
  // QOCAPmt cpmt.  Return filled QOCAPmt.  Returns NULL if the method was 
  // invoked invalidly.

  // --------------------------
  // Check if pointer is valid.
  // --------------------------
  if (!opmt) {
	printf("Warning in QPath::CalculateOccratioPMT():\n");
	printf("\tInvalid off-centre QOCAPmt pointer.\n");
	printf("\tSkipping.\n");
	return NULL;
  }


  // -----------------------------------------------
  // Copy data into the new to-be-calculated QOCAPmt
  // -----------------------------------------------
  QOCAPmt *pmt = new QOCAPmt();
  *pmt = *opmt;  


  // ------------------------------------
  // Make sure pmt met basic requirements
  // ------------------------------------
  if (pmt->IsBasicBad()) return pmt;


  // -------------------------------
  // Initialize variables to be set.
  // -------------------------------
  //pmt->SetPmteff(-1);
  pmt->SetPmteffc(-1);    // -- os 09.2005
  pmt->SetOccratio(-1);
  pmt->SetOccratioerr(0); // Slight change (used to be -1 in some cases)
  pmt->SetGeomratio(0);   // Slight change (used to be +1 in some cases)
  pmt->SetGeomratioerr(0);
  //pmt->SetDd(0);
  //pmt->SetDh(0);
  //pmt->SetDa(0);

  // --------------
  // Get pmt number
  // --------------
  Int_t pmtn = pmt->GetPmtn();
  
	
//   // -----------------------------
//   // Make sure centre run is okay.
//   // -----------------------------
//   if (!cpmt) {
// 	printf("Warning in QPath::CalculateOccratioPMT():\n");
// 	printf("\tInvalid centre run QOCAPmt pointer.\n");
// 	printf("\tSkipping pmt number %d.\n",pmtn);
// 	pmt->SetOtherbad(pmt->GetOtherbad() + 10);
// 	pmt->SetBad(pmt->GetOtherbad() + 10);
// 	return pmt;
//   }
// 
//   Int_t cpmtn = cpmt->GetPmtn();
//   if (cpmtn!=pmtn) {
// 	printf("Warning in QPath::CalculateOccratioPMT():\n");
// 	printf("\tPmt number mismatch: %d (off-centre) vs %d (centre)\n",pmtn,cpmtn);
// 	printf("\tSkipping pmt number %d.\n",pmtn);
// 	pmt->SetOtherbad(pmt->GetOtherbad() + 10);
// 	pmt->SetBad(pmt->GetOtherbad() + 10);
// 	return pmt;
//   }
// 
//   if (cpmt->IsBasicBad()) {
// 	pmt->SetBasicBad(kTRUE);
// 	return pmt;
//   }
// 
// 
//   // -----------------------------------------------------------
//   // Set belly and bad path flags for both this and central run.
//   // -----------------------------------------------------------
//   pmt->SetBelly(pmt->GetBelly() + 10*cpmt->GetBelly());
//   pmt->SetNeck(pmt->GetNeck() + 10*cpmt->GetNeck());
//   pmt->SetRope(pmt->GetRope() + 10*cpmt->GetRope());
//   pmt->SetPipe(pmt->GetPipe() + 10*cpmt->GetPipe());
//   pmt->SetOtherbad(pmt->GetOtherbad() + 10*cpmt->GetOtherbad());
//   pmt->SetBad(pmt->GetBad() + 10*cpmt->GetBad());
  
  // jose march 04  put all central run flags in fCentralBad: make fBad independent
  
  // -----------------------------
  // Make sure centre run is okay.
  // -----------------------------
  if (!cpmt) {
        printf("Warning in QPath::CalculateOccratioPMT():\n");
        printf("\tInvalid centre run QOCAPmt pointer.\n");
        printf("\tSkipping pmt number %d.\n",pmtn);
	pmt->SetCentralBad(1);
	return pmt;
  }

  Int_t cpmtn = cpmt->GetPmtn();
  if (cpmtn!=pmtn) {
        printf("Warning in QPath::CalculateOccratioPMT():\n");
        printf("\tPmt number mismatch: %d (off-centre) vs %d (centre)\n",pmtn,cpmtn);
        printf("\tSkipping pmt number %d.\n",pmtn);
	pmt->SetCentralBad(2);
	return pmt;
  }

  if (cpmt->IsBasicBad()) {
	pmt->SetCentralBad(3);
	return pmt;
  }


  // -----------------------------------------------------------
  // Set belly and bad path flags for both this and central run.
  // -----------------------------------------------------------
  pmt->SetCentralBad(pmt->GetCentralBad() + 10*((Int_t)cpmt->GetBad()));
  pmt->SetCentralBad(pmt->GetCentralBad() + 20*((Int_t)cpmt->GetBelly()));
  pmt->SetCentralBad(pmt->GetCentralBad() + 30*((Int_t)cpmt->GetNeck()));
  pmt->SetCentralBad(pmt->GetCentralBad() + 40*((Int_t)cpmt->GetRope()));
  pmt->SetCentralBad(pmt->GetCentralBad() + 50*((Int_t)cpmt->GetPipe()));
  if(fQOptics->IsNcdOptics()){
    pmt->SetCentralBad(pmt->GetCentralBad() + ((Int_t)60*cpmt->GetNcd()));
    pmt->SetCentralBad(pmt->GetCentralBad() + ((Int_t)80*cpmt->GetNcdRope()));
  }
  pmt->SetCentralBad(pmt->GetCentralBad() + 90*((Int_t)cpmt->GetAnchor()));
  pmt->SetCentralBad(pmt->GetCentralBad() + 70*((Int_t)cpmt->GetOtherbad()));

  // -------------
  // Set distances
  // -------------
  //pmt->SetDd(opmt->GetDd2o() - cpmt->GetDd2o());
  //pmt->SetDh(opmt->GetDh2o() - cpmt->GetDh2o());
  //pmt->SetDa(opmt->GetDacr() - cpmt->GetDacr());


  // ---------------------------------------
  // Basic variables in occratio calculation
  // ---------------------------------------
  Double_t occupancy = opmt->GetOccupancy();
  Double_t occupancyerr = opmt->GetOccupancyerr();
  Double_t solidangle = opmt->GetSolidangle();
  Double_t transpwr = opmt->GetTranspwr();

  Double_t coccupancy = cpmt->GetOccupancy() * cnorm;
  Double_t coccupancyerr = cpmt->GetOccupancyerr() * cnorm;
  Double_t csolidangle = cpmt->GetSolidangle();
  Double_t ctranspwr = cpmt->GetTranspwr();


  // ------------------------------------------------------------
  // PMT efficiency is proportional to occupancy for a centre run
  // ------------------------------------------------------------
  //pmt->SetPmteff(coccupancy);
  pmt->SetPmteffc(fPmteff[pmtn]); // -- os 09.2005


  // -----------------
  // Initialize ratios
  // -----------------
  Double_t geomratio = 1;
  Double_t geomratioerr = 0;
  Double_t occratio = -1;
  Double_t occratioerr = -1;


  // --------------------------------------------
  // Check if the occupancy is good for the ratio
  // --------------------------------------------
  if ( (occupancy<=0) || (coccupancy<=0) ) return pmt;


  // ----------------------------------------------
  // Compute geometry ratio.  Make sure it is good.
  // ----------------------------------------------
  geomratio = csolidangle/solidangle * ctranspwr/transpwr;
  if (geomratio<=0 && geomratio>=1e20) return pmt;
		

  // -------------------------
  // Calculate Occupancy Ratio
  // -------------------------
  occratio = (occupancy / coccupancy) * geomratio;
  occratioerr  = occratio;
  occratioerr *= sqrt(
			pow(occupancyerr/occupancy,2) +
			pow(coccupancyerr/coccupancy,2) +
			pow(geomratioerr/geomratio,2)
		     );

  // ----------------------------------------
  // Calculate occratio for each time window.
  // ----------------------------------------
  for(Int_t iwin=0; iwin<pmt->GetNtimeW(); iwin++) {
    Double_t occwindow = pmt->GetOccupancyW(iwin);
    Double_t coccwindow = cpmt->GetOccupancyW(iwin) * cnorm; // isn't norm different?
    if (occwindow>0 && coccwindow>0) {
      Double_t occratiowindow = (occwindow/coccwindow) * geomratio;
      pmt->SetOccratioW(iwin,occratiowindow);
    } else pmt->SetOccratioW(iwin,-1.0);
  } 
  

  // --------------------------------------
  // Set Occupancy Ratio and Geometry Ratio
  // --------------------------------------
  pmt->SetOccratio(occratio);
  pmt->SetOccratioerr(occratioerr);
  pmt->SetGeomratio(geomratio);
  pmt->SetGeomratioerr(geomratioerr);

  // Asymmetry and mask: change to ratio for plotting along with occratio?
  // To be implemented...

  return pmt;
}
//______________________________________________________________________________
void QPath::SetPSUPCentre(Double_t px, Double_t py, Double_t pz)
{
  // Set the QPath internal fQOptics PSUP centre offset vector.
  //
  // This vector will be _added_ to all PMT coordinates in
  // QOptics::GetPMTPosition() to simulate an offset of the PSUP coordinate
  // system with respect to the manipulator/AV coordinate system used to
  // position the laserball.

  fQOptics->SetPSUPCentre(px,py,pz);
  if(fQOpticsX) fQOpticsX->SetPSUPCentre(px,py,pz);
  if(fQOpticsY) fQOpticsY->SetPSUPCentre(px,py,pz);
  if(fQOpticsZ) fQOpticsZ->SetPSUPCentre(px,py,pz);
}
//______________________________________________________________________________
void QPath::SetPSUPCentre(TVector3 pc)
{
  // Set the QPath internal fQOptics PSUP centre offset vector.

  fQOptics->SetPSUPCentre(pc);
  if(fQOpticsX) fQOpticsX->SetPSUPCentre(pc);
  if(fQOpticsY) fQOpticsY->SetPSUPCentre(pc);
  if(fQOpticsZ) fQOpticsZ->SetPSUPCentre(pc);
}
//______________________________________________________________________________
void QPath::FitLBPosition(QOCARun *ocaruncalc, Int_t fitmethod)
{
  // Wrapper for QPath::FitLBPosition().
  // Calls FitLBPositionSetup() and then FitLBPosition() according
  // to the desired method.  Sets the appropriate fit variables in
  // ocaruncalc.

  if ( (fitmethod != 1) && (fitmethod != 2) ) {
	printf("Warning in QPath::FitLBPosition(...):\n");
	printf("\tBad fit method number.  Returning.\n");
	return;
  }

  TString description = (fitmethod == 1) ? "Direct Line fit" : "QOptics path fit";


  // Temporarily change this internal variable.  Change back before returning.
  // Long term: change the code so this will not be necessary.
  Int_t old_fFitLBPosition = fFitLBPosition;
  fFitLBPosition = fitmethod;

  FitLBPositionSetup(ocaruncalc);
  if (FitLBPosition()) {
	if (fPrint>=1) {
  		Float_t velocity = (fitmethod == 1) ? fVgroupmean : 1.;

		printf("----------------------------------------\n");
		printf("FitLBPosition %d: %s, Run %d\n",
			fitmethod,description.Data(),ocaruncalc->GetRun());
		printf("Mean group velocity %12.5f\n",fVgroupmean);
		printf("Found the position: %10.2f %10.2f %10.2f %10.2f %12.6g\n",
			GetFitX(), GetFitY(), GetFitZ(), GetFitT(), velocity*GetFitC());
		printf("Manipulator coords: %10.3f %10.3f %10.3f %10.3f %12.6g\n",
			ocaruncalc->GetManipx(), ocaruncalc->GetManipy(),
			ocaruncalc->GetManipz(), 0.0, 1.0);
		if(fPrint>=2){
		  // 01.2006 -- os
		  // Added to display the error on the fitted position
		  Float_t errx = sqrt(fmrqcovar[1][1]);
		  Float_t erry = sqrt(fmrqcovar[2][2]);
		  Float_t errz = sqrt(fmrqcovar[3][3]);
		  Float_t dx = (GetFitX() - ocaruncalc->GetManipx())/errx; 
		  Float_t dy = (GetFitY() - ocaruncalc->GetManipy())/erry;
		  Float_t dz = (GetFitZ() - ocaruncalc->GetManipz())/errz;
		  printf("Fit errors: %10.4f %10.4f %10.4f\n",errx,erry,errz);
		  printf("Relative to manip: %10.2f %10.2f %10.2f\n",dx,dy,dz);
		}
	}
	
	if (fitmethod == 1) {
		ocaruncalc->SetFitxyz(GetFitX(), GetFitY(), GetFitZ());
		ocaruncalc->SetFitt(GetFitT());
		ocaruncalc->SetFitchi2(fChisquare/(fnpmts-4));
	} else if (fitmethod == 2) {
		ocaruncalc->SetQFitxyz(GetFitX(), GetFitY(), GetFitZ());
		ocaruncalc->SetQFitt(GetFitT());
		ocaruncalc->SetQFitchi2(fChisquare/(fnpmts-4));
		// 01.2006 -- os : Added to store the error on the fitted position
		ocaruncalc->SetQFitxyzerr(sqrt(fmrqcovar[1][1]), 
					  sqrt(fmrqcovar[2][2]),
					  sqrt(fmrqcovar[3][3]));
		int i;
		for(i=1;i<=fnpmts;i++){
		  QOCAPmt *pm = (QOCAPmt*)ocaruncalc->GetPMT((Int_t)fmrqx[i]);
		  pm->SetPoschi2(fChiarray[i]);
		  pm->SetPosres(fResarray[i]);
		}
	}	

	ocaruncalc->SetLaserpos(fitmethod); // Use fit position as laserx,y,z
  } else {
	fprintf(stderr,"Couldn't do %s on run %d!\n",
		description.Data(),ocaruncalc->GetRun());
  }


  // Change back.
  fFitLBPosition = old_fFitLBPosition;

  return;
}
//______________________________________________________________________________
Int_t QPath::FitLBPosition()
{
  // Fit the laserball position for a given run.
  // The mrqmin arrays fmrqXXX must be set up previously by FitLBPositionSetup()
  
  if (!fFitLBPositionSetup) return 0;
  
  if (fPrint>=1) printf("About to call QPath::MrqFit()...\n");

  // set Qoptics' fast mode for position fits
  fQOptics->SetFastMode(1); // with printing message 
 
  MrqFit(fmrqx, fmrqy, fmrqsig, fnpmts, fmrqpars, fmrqvary, 5,  // 5 parameters
	 fmrqcovar, fmrqalpha, &fChisquare);
  
  // unset Qoptics' fast mode
  fQOptics->SetFastMode(0); // with printing message 
  
  if (fPrint>=1) {
    printf("----------------------------------------\n");
    printf("Number of PMT's in fit %d (%.1f%%), for 4 parameters.\n",
	   fnpmts,100.*((Float_t)fnpmts/(Float_t)fCurrentrun->GetNpmt()));
    printf("Position fit complete.  Chisquare %f, reduced %f\n",fChisquare,
	   fChisquare/(fnpmts-4));
  }

  return 1;
}
//______________________________________________________________________________
Int_t QPath::FitLBPositionSetup(QOCARun *run)
{
  // Set up the fmrqXXX arrays in preparation for a fit to the laserball position
  //
  // fCurrentrun must point to the run to be fit!
  // -----------------
  //
  // 11.2006 - O.Simard
  // Cleaned up, added safety nets, and added cuts on timing in pmt loop
  // of all PMTs stored in QOCARun. This prevents PMTs with (bad) widths
  // that have most likely bad timing to be considered as good data in the
  // position fit.

  if (!run) return 0;
  else fCurrentrun = run; // use fCurrentrun from here
  fCurrentrunIndex = 0;
  for(int itemp = 0; itemp < fNumberofruns; itemp++) {
        //printf("*******  run index itemp %d fRunlist[itemp] %d , fCurrentrun\n",
        //      itemp,fRunlist[itemp],fCurrentrun->GetRun());
        if (fRunlist[itemp] == fCurrentrun->GetRun()) fCurrentrunIndex = itemp;
  }
  printf("QPath: fCurrentrunindex %d\n",fCurrentrunIndex);

  
  TVector3 srcvec = fCurrentrun->GetManipxyz();
  // variable not used
  // TVector3 *pmtvec = new TVector3();

  QOCAPmt *pmt;

  fnpmts = 0;

  if (fPrint>=1) {
    printf("----------------------------------------\n");
    printf("About to initialize data point arrays for LB fit...\n");
  }

  // set above
  //srcvec.SetXYZ(fCurrentrun->GetManipx(), fCurrentrun->GetManipy(),
  //		 fCurrentrun->GetManipz());

  // Copy wavelengths to fQOpticsX,Y,Z
  Float_t lambda = fCurrentrun->GetLambda();
  fQOptics->SetIndices(lambda);
  if(fQOpticsX) fQOpticsX->SetIndices(lambda);
  if(fQOpticsY) fQOpticsY->SetIndices(lambda);
  if(fQOpticsZ) fQOpticsZ->SetIndices(lambda);

  // Set up useful speeds of light
  fVgroupd2o = fQOptics->GetVgroupD2O();
  fVgroupacr = fQOptics->GetVgroupAcrylic();
  fVgrouph2o = fQOptics->GetVgroupH2O();
  fVgroupmean = (fVgroupd2o*600.5 + fVgroupacr*5.5 + fVgrouph2o*245) / 850.5;
  // if (fPrint>=1) printf("Using %f for RCH speed of light\n",fVrch);

  // counters for info
  Int_t Nbad = 0, Ngood = 0;
  // |---------
  // | pmt loop
  for (Int_t ipmt = 0; ipmt < fCurrentrun->GetNpmt(); ipmt++) {
    pmt = fCurrentrun->GetPMT(ipmt);
    TVector3 pmtpos(fQOptics->GetPMTPosition(pmt->GetPmtn()));

    //if (!pmt->GetBasicBad() && pmt->GetOccupancy() >= 100) {
    // Change condition: cut tubes that
    //   - have basic bad flag set (offline, or whatever)
    //   - occupancy error greater than 10% (not enough stats)
    //   - time width outside of 3sigma
    //   - occupancy correction of 50% or greater (MPE at this stage)
    //
    // the following counts and skips pmts as they fail the above conditions
    if(pmt->GetBasicBad()){ Nbad++; continue; }
    if(pmt->GetOccupancyerr() > .1*pmt->GetOccupancy()){ Nbad++; continue; }
    if((pmt->GetOccCorrection() < 0.) || (pmt->GetOccCorrection() > 1.5)){ Nbad++; continue; }
    if(
       (pmt->GetTwidth() < fCurrentrun->GetTwidthMean() - fTimeSigmaNSigma*fCurrentrun->GetTwidthSigma()) ||
       (pmt->GetTwidth() > fCurrentrun->GetTwidthMean() + fTimeSigmaNSigma*fCurrentrun->GetTwidthSigma())
       ) { Nbad++; continue; }
    // made it here, it must be good
    Ngood++;

    // variable not used
    // pmtvec->SetXYZ( pmt->GetPmtx(), pmt->GetPmty(), pmt->GetPmtz());

    // Use Rchtof value: equivalent to fmrqy[fnpmts+1] = pmt->GetTprompt() + distance / fVrch;
    fmrqx[fnpmts+1]   = ipmt;
    fmrqy[fnpmts+1]   = pmt->GetTprompt() + pmt->GetRchtof();

    // jm- jn 2007  now using dynamic setting of number of bins, read from the rdt 
    //fmrqsig[fnpmts+1] = pmt->GetTwidth()/sqrt(31.0); // (Nbins-1) prompt window fRchTime (0.25ns wide)
    int dynamicnbins = 32;
    if (fTimeNbinsLoaded[fCurrentrunIndex]) dynamicnbins = fTimeNbins[pmt->GetPmtn()][fCurrentrunIndex];
    if (dynamicnbins <= 1) { // also avoids (dynamicnbins-1) = 0 
      dynamicnbins = 32;
      printf("Warning!!:Run %d (%d) pmt %d nbins 0!\n",fRunlist[fCurrentrunIndex],fCurrentrunIndex,pmt->GetPmtn());
    }
    //if(dynamicnbins !=32) printf("Time Nbins: %d Run %d pmt %d\n",dynamicnbins,fRunlist[fCurrentrunIndex],pmt->GetPmtn());
    //fmrqsig[fnpmts+1] = pmt->GetTwidth()/float(dynamicnbins-1); // (Nbins-1) prompt window fRchTime (0.25ns wide)
    fmrqsig[fnpmts+1] = pmt->GetTwidth()/sqrt(float(dynamicnbins-1)); // (Nbins-1) prompt window fRchTime (0.25ns wide)
    
    fnpmts++;
  }

  if((fCurrentrun->GetNpmt() - Nbad) != Ngood) {
    Warning("FitLBPositionSetup","Mismatch in number of PMTs: (total-bad vs good) = %d vs %d.\n",
	    (fCurrentrun->GetNpmt() - Nbad),Ngood);
  }

  // Always use manipulator position to seed the fits.  It's never very far off,
  // and can't get side-tracked the way previous fit results might in fLaserx,y,z.
  // (Used to use fCurrentrun->GetLaserx() , ... to set position).
  // 30-Dec-2000 - Moffat
  fQOptics->SetSource(srcvec);

  fmrqpars[1] = fCurrentrun->GetManipx();  // Seed the fit at a reasonable position
  fmrqpars[2] = fCurrentrun->GetManipy();
  fmrqpars[3] = fCurrentrun->GetManipz();
  fmrqpars[4] = 0.0;
  if (fFitLBPosition == 1) fmrqpars[5] = 1.0/fVgroupmean; // approx. light speed
  else if (fFitLBPosition >= 2) fmrqpars[5] = 1.0;  // multiplier for light speed
  else fmrqpars[5] = 1.0;

  fNcalls = 0;
  fNmessage = fnpmts;

  fFitLBPositionSetup = kTRUE;

  return 1;
}
//______________________________________________________________________________
void QPath::SetRunList(Int_t n, Int_t runs[], Int_t cruns[], Int_t fruns[],
						Int_t runpass[])
{
  // Set up the list of runs and central runs for processing by FillFromRDT()
  // or CalculateOccratio().
  // Also sets the pass numbers to be used for the chosen runs. The default is to
  // use files with no pass number.

  Int_t i,j;

  if (n>0) {
    if (fRunlist) delete[] fRunlist;
    if (fRunpasslist) delete[] fRunpasslist;
    if (fCentrerunlist) delete[] fCentrerunlist;
    if (fCentrerunindex) delete[] fCentrerunindex;
    if (fRunposlist) delete[] fRunposlist;
    if (fRunpositions) delete[] fRunpositions;
    if (fCentreruns) delete[] fCentreruns;

    // Only delete the array of pointers, not the objects!
    // delete fCentrerunptr[i]; erases the run from the QOCATree! Not desired!
    // 27-Dec-2000 -Moffat
    // Reversed 29-Dec-2000 - Moffat - SetCentreRuns() allocates new QOCARun's for these!
    if (fCentrerunptr) {
      printf("Deleting %d centre run pointers\n",fNumberofcentre);
      for (i=0; i<fNumberofcentre; i++) {
	if (fCentrerunptr[i]){delete fCentrerunptr[i]; fCentrerunptr[i] = NULL;}
      }
      delete[] fCentrerunptr;
      fCentrerunptr = NULL;
    }

    fNumberofruns = n;
    fRunlist = new Int_t[n];
    fCentrerunlist = new Int_t[n];
    if (fruns) {
      	fRunposlist = new Int_t[n];
      	fRunpositions = new TVector3[n];

    }
    if (runpass) fRunpasslist = new Int_t[n];

    for (i=0; i<n; i++) {
      	fRunlist[i] = runs[i];
      	fCentrerunlist[i] = cruns[i];  // Specify each run's centre run
      	if (fruns) {
			fRunposlist[i] = fruns[i];
			fRunpositions[i].SetXYZ(-999,-999,-999);
      	}
	if (runpass) fRunpasslist[i] = runpass[i];
   }

    printf("----------------------------------------\n");
    printf("Trying to figure out centre runs...\n");
    // Check how many **distinct** central runs are in list
    Bool_t distinct;

    fCentrerunindex = new Int_t[n]; // one per entry in fRunList
    fCentreruns = new Int_t[n]; // more or equal to the amount needed

    fNumberofcentre = 1;  // First run is distinct (by definition!)
    fCentreruns[0] = fCentrerunlist[0];
    fCentrerunindex[0] = 0;
    printf("Found first central run: %d\n",fCentrerunlist[0]);

    for (i=1; i<fNumberofruns; i++) {
      distinct = kTRUE;
      j = 0;
      do {
	if (fCentrerunlist[i] == fCentreruns[j]) {
	  distinct = kFALSE;
	  break;
	}
	j++;
      } while (j<fNumberofcentre);
      
      fCentrerunindex[i] = j;
      if (distinct) {
	fCentreruns[fNumberofcentre] = fCentrerunlist[i];
	fNumberofcentre++;
	printf("Found another central run: %d (%d so far)\n",
	       fCentrerunlist[i],fNumberofcentre);
      }
    }
  } else {
    fprintf(stderr,"Must specify at least 1 run in QPath::SetRunList()\n");
  }
}
//______________________________________________________________________________
void QPath::SetRunList(Int_t n, Int_t runs[], Int_t crun, Int_t fruns[], 
						Int_t runpass[])
{
  // Set up the list of runs and a single central run for processing by
  // CalculateOccratio().
  //
  // crun==0 has special meaning in FillFromRDT() and CalculateOccratio(), as
  // well as the QOCAFit routines: no central normalization, but a fPmteff from
  // elsewhere...?

  Int_t i;

  if (n>0) {
    Int_t *cruns = new Int_t[n];
    for (i=0; i<n; i++) cruns[i] = crun;
    SetRunList(n,runs,cruns,fruns,runpass);  // use the separated listing routine
    delete[] cruns;

    fNumberofcentre = 1;
  } else {
    fprintf(stderr,"Must specify at least 1 run in QPath::SetRunList()\n");
  }
}

//______________________________________________________________________________
void QPath::SetAllsyst(Float_t s)
{
  // Set all systematics to the same value; usually, this will be zero to
  // eliminate all systematics.
  
  fXsyst = s;
  fYsyst = s;
  fZsyst = s;
  fRsyst = s;
  fBallsyst = s;
}

//______________________________________________________________________________
void QPath::SetFitLBPosition(Int_t f)
{
  // Check for valid fit type:
  //   0 = don't do fit
  //   1 = use QOptics to calculate paths and derivatives
  //   2 = use direct path for fit (ie. single medium, speed of light is free par)

  if (f<0 || f>3) fprintf(stderr,"Invalid fit type %d - using %d\n",f,fFitLBPosition);
  else fFitLBPosition = f;
  if((f == 3) && (fPrint>=1)) printf("User chose to force the position fits.\n");

}
//______________________________________________________________________________
Int_t QPath::MrqFit(float x[], float y[], float sig[], int ndata, float a[],
		    int ia[], int ma, float **covar, float **alpha, float *chisq )
{
  //Fit these data using mrqmin() repeatedly until convergence is achieved.

  // Check on QOptics instances
  if(!fQOpticsX || !fQOpticsY || !fQOpticsZ){
    Warning("MrqFit","QOptics instances must be initialized.");
    exit(-1);
  }
  
  Int_t maxiter = 1000;
  Int_t numiter = 0;
  Int_t gooditer = 0;
  Int_t retval = 0;

  Float_t oldchisq = 0;
  Float_t lambda = -1.0;
  Float_t tol = 1.0;    // Chisquared must change by tol to warrant another iteration

  *chisq = 0;

  // First, call mrqmin with lambda = -1 for initialization
  if (fPrint>=1) printf("Calling mrqmin for initialization...\n");
  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lambda);
  if (fPrint>=1) printf("Done.  Chisq = %f\n",*chisq);
  oldchisq = *chisq;
  if (fPrint>=1) printf("CHISQ at origin = %12.5g\n",*chisq);
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lambda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
	 && retval == 0 && lambda != 0.0) {
    oldchisq = *chisq;
    if (fPrint>=1) printf("Iterating with lambda %g...\n",lambda);
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lambda );
    if (fPrint>=1) printf("New chisq = %12.8g with lambda %g \n",*chisq,lambda);
    numiter++;

    if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lambda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  
  lambda = 0;
  retval = mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lambda );
  return retval;
}
//______________________________________________________________________________
Int_t QPath::mrqmin(float x[], float y[], float sig[], int ndata, float a[],
		      int ia[], int ma, float **covar, float **alpha, float *chisq,
		      float *alambda)
{
  // Minimization routine for a single iteration over the data points.

  // Required helper routines:
  // void covsrt(float **covar, int ma, int ia[], int mfit);
  // void gaussj(float **a, int n, float **b, int m);
  // void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
  //	int ia[], int ma, float **alpha, float beta[], float *chisq,
  //	void (*funcs)(float, float [], float *, float [], int));

  int j,k,l,m, retval = 0;
  int mfit;
  float ochisq,*atry,*beta,*da,**oneda;

  //--------------------
  // Initialization  
  if (*alambda < 0.0) {
    atry=vector(1,ma);
    beta=vector(1,ma);
    da=vector(1,ma);
    for (mfit=0,j=1;j<=ma;j++)
      if (ia[j]) mfit++;
    oneda=matrix(1,mfit,1,1);
    *alambda=0.001;
    mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq);
    ochisq=(*chisq);
    for (j=1;j<=ma;j++) atry[j]=a[j];
  }

  //--------------------
  // Bookkeeping on covariance and derivatives to prepare next parameter set.
  for (j=0,l=1;l<=ma;l++) {
    if (ia[l]) {
      for (j++,k=0,m=1;m<=ma;m++) {
	if (ia[m]) {
	  k++;
	  covar[j][k]=alpha[j][k];
	}
      }
      covar[j][j]=alpha[j][j]*(1.0+(*alambda));
      oneda[j][1]=beta[j];
    }
  }
  retval = gaussj(covar,mfit,oneda,1);
  for (j=1;j<=mfit;j++) da[j]=oneda[j][1];

  //--------------------
  // Final call to prepare covariance matrix and deallocate memory.
  if (*alambda == 0.0 ) {
    covsrt(covar,ma,ia,mfit);
    free_matrix(oneda,1,mfit,1,1);
    free_vector(da,1,ma);
    free_vector(beta,1,ma);
    free_vector(atry,1,ma);
    return retval;
  }

  //--------------------
  // Set up the trial parameters and try them
  for (j=0,l=1;l<=ma;l++)
    if (ia[l]) atry[l]=a[l]+da[++j];

  mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq);

  if (*chisq < ochisq) {
    *alambda *= 0.1;
    ochisq=(*chisq);
    for (j=0,l=1;l<=ma;l++) {
      if (ia[l]) {
	for (j++,k=0,m=1;m<=ma;m++) {
	  if (ia[m]) {
	    k++;
	    alpha[j][k]=covar[j][k];
	  }
	}
	beta[j]=da[j];
	a[l]=atry[l];
      }
    }
  } else {
    *alambda *= 10.0;
    *chisq=ochisq;
  }
  return retval;
}
//______________________________________________________________________________
void QPath::covsrt(float **covar, int ma, int ia[], int mfit)
{
  // Covariance matrix sorting.  Helper routine for mrqmin()

  int i,j,k;
  float swap;

  for (i=mfit+1;i<=ma;i++)
    for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
  k=mfit;
  for (j=ma;j>=1;j--) {
    if (ia[j]) {
      for (i=1;i<=ma;i++) { SWAP(covar[i][k],covar[i][j]) }
      for (i=1;i<=ma;i++) { SWAP(covar[k][i],covar[j][i]) }
      k--;
    }
  }
}
//______________________________________________________________________________
Int_t QPath::gaussj(float **a, int n, float **b, int m)
{
  // Gauss-Jordan matrix solution helper routine for mrqmin.

  int *indxc,*indxr,*ipiv;
  int i,icol,irow,j,k,l,ll;
  float big,dum,pivinv,swap;
  Int_t retval = 0;
  indxc=ivector(1,n);
  indxr=ivector(1,n);
  ipiv=ivector(1,n);
  for (j=1;j<=n;j++) ipiv[j]=0;
  for (i=1;i<=n;i++) {
    big=0.0;
    for (j=1;j<=n;j++)
      if (ipiv[j] != 1)
	for (k=1;k<=n;k++) {
	  if (ipiv[k] == 0) {
	    if (fabs(a[j][k]) >= big) {
	      big=fabs(a[j][k]);
	      irow=j;
	      icol=k;
	    }
	  } else if (ipiv[k] > 1) 
	    {
	      //nrerror("gaussj: Singular Matrix-1");
	      gSNO->Warning("gaussj","Singular Matrix-1");
	      retval = -1;
	    }
	}
    ++(ipiv[icol]);
    if (irow != icol) {
      for (l=1;l<=n;l++) { SWAP(a[irow][l],a[icol][l]) }
      for (l=1;l<=m;l++) { SWAP(b[irow][l],b[icol][l]) }
    }
    indxr[i]=irow;
    indxc[i]=icol;
    if (a[icol][icol] == 0.0) 
      {
	//nrerror("gaussj: Singular Matrix-2");
	gSNO->Warning("gaussj","Singular Matrix-2");
	retval = -2;
      }
    pivinv=1.0/a[icol][icol];
    a[icol][icol]=1.0;
    for (l=1;l<=n;l++) a[icol][l] *= pivinv;
    for (l=1;l<=m;l++) b[icol][l] *= pivinv;
    for (ll=1;ll<=n;ll++)
      if (ll != icol) {
	dum=a[ll][icol];
	a[ll][icol]=0.0;
	for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
	for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
      }
  }
  for (l=n;l>=1;l--) {
    if (indxr[l] != indxc[l])
      for (k=1;k<=n;k++)
	SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  }
  free_ivector(ipiv,1,n);
  free_ivector(indxr,1,n);
  free_ivector(indxc,1,n);
  return retval;
}
//______________________________________________________________________________
void QPath::mrqcof(float x[], float y[], float sig[], int ndata, float a[],
		     int ia[], int ma, float **alpha, float beta[], float *chisq)
{
  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  float chisqentry;  // chi-squared for single entry in list
  
  dyda=vector(1,ma);
  for (j=1;j<=ma;j++) {
    if (ia[j]) mfit++;
    dyda[j] = 0.0;
  }
  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) alpha[j][k]=0.0;
    beta[j]=0.0;
  }
  *chisq=0.0;

  fQOptics->SetSource(a[1], a[2], a[3]);
  fQOpticsX->SetSource(a[1] + fdelpos, a[2], a[3]); 
  fQOpticsY->SetSource(a[1], a[2] + fdelpos, a[3]);
  fQOpticsZ->SetSource(a[1], a[2], a[3] + fdelpos);

  for (i=1;i<=ndata;i++) {
    mrqfuncs( x[i],i,a,&ymod,dyda,ma);
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;
    for (j=0,l=1;l<=ma;l++) {
      if (ia[l]) {
	wt=dyda[l]*sig2i;
	for (j++,k=0,m=1;m<=l;m++)
	  if (ia[m]) alpha[j][++k] += wt*dyda[m];
	beta[j] += dy*wt;
      }
    }
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    if (fPrint>=3) if (i%1==2000) printf(" %d %f %f\n",i,x[i],chisqentry);
    if (fChiarray!=NULL && i>=0 && i<fNelements) fChiarray[i] = chisqentry;
    if (fResarray!=NULL && i>=0 && i<fNelements) fResarray[i] = dy;
  }
  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
  free_vector(dyda,1,ma);
}
//______________________________________________________________________________
void QPath::mrqfuncs(Float_t x, Int_t i, Float_t a[], Float_t *y,
			Float_t dyda[], Int_t )
{
  //Function used by mrqcof()
  //   a[]    = parameters to be fit:
  //              x, y, z of the laserball
  //              t of the laserball light pulse
  //   x[]    = indices of pmts in fCurrentrun
  //   y      = model value = D_path / V_group + t0
  //   dyda[] = derivative of y w.r.t. input parameters
  //   na     = number of parameters
  //


  if ( !fCurrentrun ) {
    Error("mrqfuncs","Need a current run to fit!");
  }
   
  Int_t ix = (Int_t)x;
  QOCAPmt *pmt = fCurrentrun->GetPMT(ix);
  Int_t pmtn = pmt->GetPmtn();
  TVector3 srcvec(fQOptics->GetSource());          // just in case GetDistances
  TVector3 pmtvec(fQOptics->GetPMTPosition(pmtn)); // falls through...

  Int_t panel;
  Double_t dd2o,dh2o,dacr,cospmt,transpwr; // For the model value
  Double_t dd2od,dh2od,dacrd;              // For derivatives.

  // NB. fQOptics source position set in mrqcof above, once for the whole loop!

  Double_t psmag; // Magnitude of source->pmt vector

  if (fFitLBPosition == 1) {  // Use direct line paths
    psmag = (pmtvec - srcvec).Mag();
    *y = psmag * a[5] +a[4];
    dyda[1] = - (pmtvec(0) - srcvec(0)) * a[5] / psmag;
    dyda[2] = - (pmtvec(1) - srcvec(1)) * a[5] / psmag;
    dyda[3] = - (pmtvec(2) - srcvec(2)) * a[5] / psmag;
    dyda[4] = 1.0;
    dyda[5] = psmag;
  }

  //--------------------
  Double_t tflight; // Time of flight along QOptics path
  Double_t lightspeed; // Distance weighted speed of light...

  if (fFitLBPosition >= 2) {
    if (fQOptics->GetDistances(pmtn,dd2o,dacr,dh2o,cospmt,panel,transpwr)) {
      
      tflight = (dd2o/fVgroupd2o + dacr/fVgroupacr + dh2o/fVgrouph2o);
      lightspeed =
	(fVgroupd2o*dd2o + fVgroupacr*dacr + fVgrouph2o*dh2o) / (dd2o+dacr+dh2o);
      *y = tflight * a[5] + a[4];

      //ALH: 2/19/2005--- i commented out the following lines, which are to implement the cos-theta timing correction for large angle hits in the light water.
      // we do want this, but I dislike calculating this every time-- so when we get to 
      // light water optics again we should think about efficiency.  We also 
      //probably want to correct the time calibration, and not the model, but 
      // we can discuss that.  

          
//       Float_t mymanx = fCurrentrun->GetManipx();
//       Float_t mymany = fCurrentrun->GetManipy();
//       Float_t mymanz = fCurrentrun->GetManipz();
//       Float_t mypmx = pmt->GetPmtx();
//       Float_t mypmy = pmt->GetPmty();
//       Float_t mypmz = pmt->GetPmtz();

//       Float_t mydpmt = sqrt((mymanx-mypmx)*(mymanx-mypmx) +(mymany-mypmy)*(mymany-mypmy) +(mymanz-mypmz)*(mymanz-mypmz));
     

//      if (mydpmt >700 && mydpmt < 1100) {
//	if (cospmt >=0.45 && cospmt<0.75) {
//	  *y = tflight* a[5] +a[4] + (-2.6045+3.47265*cospmt);}

	
// 	Double_t xvar = -21.9651*cospmt+7.61297;
	
// 	if (cospmt < 0.45 && cospmt>0.25) {
// 	  *y = tflight * a[5] + a[4] - exp(xvar);}
//       }

//       if (mydpmt<300) {
// 	if (cospmt<=0.45 && cospmt>0.125){
// 	  *y = tflight*a[5] +a[4] +(22.767-300.197*cospmt+1590.37*cospmt*cospmt-3638.38*cospmt*cospmt*cospmt+2970.86*cospmt*cospmt*cospmt*cospmt);}
//       }


      
      if (i%1000==0 && fPrint>=2)
	printf("%10d %10d %10.3f %10.3f %10.3f %10.3f\n",i,pmtn,dd2o/fVgroupd2o,
	       dacr/fVgroupacr, dh2o/fVgrouph2o, *y);
      
      //--------------------

      if (fQOpticsX->GetDistances(pmtn,dd2od,dacrd,dh2od,cospmt,panel,transpwr)) {
	dyda[1] =
	  ((dd2od-dd2o)/fVgroupd2o + (dacrd-dacr)/fVgroupacr + (dh2od-dh2o)/fVgrouph2o)
	  / fdelpos * a[5];
      } else {
	dyda[1] = - (pmtvec(0) - srcvec(0))*(a[5]/lightspeed)/(pmtvec - srcvec).Mag();
      }
      
      //--------------------
      if (fQOpticsY->GetDistances(pmtn,dd2od,dacrd,dh2od,cospmt,panel,transpwr)) {
	dyda[2] =
	  ((dd2od-dd2o)/fVgroupd2o + (dacrd-dacr)/fVgroupacr + (dh2od-dh2o)/fVgrouph2o)
	  / fdelpos * a[5];
      } else {
	dyda[2] = - (pmtvec(1) - srcvec(1))*(a[5]/lightspeed)/(pmtvec - srcvec).Mag();
      }
      
      //--------------------
      if (fQOpticsZ->GetDistances(pmtn,dd2od,dacrd,dh2od,cospmt,panel,transpwr)) {
	dyda[3] =
	  ((dd2od-dd2o)/fVgroupd2o + (dacrd-dacr)/fVgroupacr + (dh2od-dh2o)/fVgrouph2o)
	  / fdelpos * a[5];
      } else {
	dyda[3] = -(pmtvec(2) - srcvec(2))*(a[5]/lightspeed)/(pmtvec - srcvec).Mag();
      }
      
      //--------------------
      dyda[4] = 1.0;
      dyda[5] = tflight;

    } else {
      if (fPrint>=2) printf("QOptics failure %d !\n",pmtn);
      *y = fmrqy[i];  // If we can't calculate the path, set model = data - Hmmmm...
      dyda[1] = 0.0;
      dyda[2] = 0.0;
      dyda[3] = 0.0;
      dyda[4] = 0.0;
      dyda[5] = 0.0;
    }
  }
  
  //--------------------
  if (i%1000==0 && fPrint>=3)
    printf("%12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g\n",
	   a[1], dyda[1], a[2], dyda[2], a[3], dyda[3], a[4], dyda[4], a[5], dyda[5]);
  
  fNcalls++;
  if (fNcalls%fNmessage == 0 && fPrint>=1)
    printf("In QPath::mrqfuncs() at call %d\n",fNcalls);
}
//______________________________________________________________________________
void QPath::SetParameter(Int_t n,Float_t value)
{
  // Set parameter number n to value.
  // n is specified in zero-offset array index, and converted internally to
  // 1..n format used by numerical recipes.

  if ((n>=1) && (n<=5)) fmrqpars[n] = value;
}
//______________________________________________________________________________
TVector3 QPath::GetFitXYZ()
{
  // Return the laserball fit coordinate.

  TVector3 tv(fmrqpars[1], fmrqpars[2], fmrqpars[3]);
  return tv;
}
//______________________________________________________________________________
void QPath::SetMrqArrays(Int_t nelements,Float_t *chiarray,Float_t *resarray)
{
  // Set the array pointers for storing residuals and chisq results on
  // an entry by entry basis in mrqcof()
  
  fNelements = nelements;
  fChiarray = chiarray;
  fResarray = resarray;
}
//______________________________________________________________________________
void QPath::SetRdtdir(TString aString)
{
  // Set the directory where the rdt files can be found. The default is "rdt/".

  if (aString=="") {
    fRdtdir = getenv("PWD");
    fRdtdir += "/rdt";
  }
  else fRdtdir = aString;

  return;
}
//______________________________________________________________________________
void QPath::SetDqxxdir(TString aString)
{
  // Set the directory where the dqxx files can be found. The default is to get
  // the environment variable DQXXDIR.

  if (aString=="") {
    if(getenv("DQXXDIR") == NULL){
      Warning("SetDqxxdir","You should set your DQXXDIR variable.");
      fDqxxdir = "/data/data001/hepdb/titles";
    } else fDqxxdir = getenv("DQXXDIR");
  } else fDqxxdir = aString;

  return;
}
//______________________________________________________________________________
void QPath::SetNcdPositionfile(TString aString)
{
  // Set the file name (with full path) with the ncd position file
  if (aString=="") {
	fNcdPosfile = getenv("OCA_SNO_ROOT");
	fNcdPosfile += "/data/";
//	fNcdPosfile += getenv("SNO_SNOMAN_VER");
	fNcdPosfile += "/geometry.dat";
  } else fNcdPosfile = aString;

  // then call QOptics::ResetNcdPosition()
  fQOptics->ResetNcdPositions((Char_t*)fNcdPosfile.Data());

  return;
}
//______________________________________________________________________________
void QPath::SetPmtvarfile(TString aString)
{
  // Set the file name (with full path) with the pmt efficiency titles banks.

  if (aString=="") {
	fPmtvarfile = getenv("OCA_SNO_ROOT");
	fPmtvarfile += "/data/opt_variations_salt.dat";
  } else fPmtvarfile = aString;

  return;
}
//______________________________________________________________________________
void QPath::SetCentrerunPmtIndex(QOCARun *crun)
{
  // Make an array of the pmt numbers of the given central run.  This will allow 
  // the pmts of the centre run to be mapped into those of the off-centre run.
  // For use with GetCentrerunPmtIndex().

  if (!crun) {
	printf("Warning in QPath::SetCentrerunPmtIndex():\n");
	printf("\tInvalid pointer to QOCARun object.\n");
	printf("\tExiting.\n");
  }

  Int_t ncpmt = crun->GetNpmt();
  SetNCentrerunPmt(ncpmt);

  if (fCentrerunPmtIndex) delete[] fCentrerunPmtIndex;
  fCentrerunPmtIndex = new Int_t[ncpmt];

  for (Int_t icpmt=0; icpmt<ncpmt; icpmt++) {
	QOCAPmt *cpmt = (QOCAPmt *)crun->GetPMT(icpmt);
	if(!cpmt) { 
	  printf("Warning in QPath::SetCentrerunPmtIndex():\n");
	  printf("Invalid pointer to central run QOCAPmt object.\n");
	}
	fCentrerunPmtIndex[icpmt] = (cpmt) ? cpmt->GetPmtn() : -1;
  }

  return;
}
//______________________________________________________________________________
Int_t QPath::GetCentrerunPmtIndex(Int_t pmtn, Int_t lastindex)
{
  // Retrun the index of the centre run pmt with the same pmt number as pmtn.
  // This allows the user to find the QOCAPmt object in the centre QOCARun that
  // is associated with the given QOCAPmt of the off-centre run.
  // SetCentrerunPmtIndex() must have been called earler for this to work.
  //
  // lastindex is the last pmt index filled: speeds up the retrieval
  // of pmtn in the for loop.

  if (!fCentrerunPmtIndex) {
	printf("Warning in QPath::GetCentrerunPmtIndex():\n");
	printf("\tfCentrerunPmtIndex not set.  Use SetCentrerunPmtIndex().\n");
	printf("\tExiting.\n");
	return -2;
  }

  Int_t ncpmt = GetNCentrerunPmt();
  //for (Int_t icpmt=0; icpmt<ncpmt; icpmt++) {
  for (Int_t icpmt=lastindex; icpmt<ncpmt; icpmt++) {
	if (fCentrerunPmtIndex[icpmt] == pmtn) return icpmt;
	if (fCentrerunPmtIndex[icpmt] > pmtn) break; // passed it, get out
  }
  
  printf("Warning in QPath::GetCentrerunPmtIndex():\n");
  printf("\tOff-centre pmt number %d not found.\n",pmtn);

  return -1;
}
//______________________________________________________________________________
void QPath::LoadPmteffTitles()
{
  // Load PMT relative efficiencies found in file fPmtvarfile.
  // fLambda needs to be set properly so that the function
  // knows which set of efficiencies to get in the file.

  // make sure the current directory is not changed
  TDirectory* currentdir = gDirectory;

  printf("----------------------------------------------\n");
  printf("Loading PMT efficiencies from file %s.\n",fPmtvarfile.Data());
  printf("----------------------------------------------\n");

  Int_t i,j;
  Int_t* eff_titles = new Int_t[10000];
  
  QPMTxyz* pmtxyz = new QPMTxyz("read");
  QTitles* file = new QTitles((char*)fPmtvarfile.Data(),"QVAR",1);
  QBank* bank = (QBank*)file->GetBank();
  
  Int_t wlindex = -1; // os -- put a default value
  Float_t lambda = GetLambda(); // lambda in nm
  
  // assign the index based on lambda 
  if(lambda > 300 && lambda < 340) wlindex = 0;   // 337
  if(lambda > 360 && lambda < 370) wlindex = 1;   // 365
  if(lambda > 380 && lambda < 390) wlindex = 2;   // 386
  if(lambda > 415 && lambda < 425) wlindex = 3;   // 420
  if(lambda > 495 && lambda < 505) wlindex = 4;   // 500
  if(lambda > 615 && lambda < 700) wlindex = 5;   // 620
    
  // check if the wavelength is in our physical range
  if((lambda <= 300) || (lambda >= 700) || (wlindex == -1)){
    Warning("LoadPmteffTitles","Check wavelength %.1f. Will use 420 nm for now.",fLambda);
    wlindex = 3;
  }

  // snoman pmt number loop
  for(i = 1; i <= 9522; i++) eff_titles[i] = bank->icons(i + wlindex*9522);
  // qsno pmt number loop
  for(i = 0; i < 10000; i++){
    if(!pmtxyz->IsNormalPMT(i) || pmtxyz->IsInvalidPMT(i)) continue;
    j = pmtxyz->GetSnomanNo(i);
    if (j < 1 || j > 10000) {
      printf("QPath::LoadPmteffTitles(): Index Mismatch: channel %d snoman %d \n",i,j);
      continue;
    }
    // bank integers need to be transformed to floats
    fPmteff[i] = (Float_t) eff_titles[j] / 1000.;
    if (fPmteff[i] == 0.) fPmteff[i] = 1.;
  }
  
  // delete initialized objects
  delete pmtxyz;
  delete file;
  delete eff_titles;

  currentdir->cd(); // stay in the current directory

  // set this bit before leaving
  fEfficienciesLoaded = kTRUE;

  return; 
}
//______________________________________________________________________________
void QPath::SetSingleEfficiencyValue(Double_t aValue)
{
  // Fill the array of Pmt efficiencies (fPmteff) with a single value supplied 
  // in argument. Should be used to study systematic effects related to the 
  // input efficiencies only. The old efficiencies are checked to keep the same 
  // PMTs with valid efficiencies. Therefore, this function should always be 
  // called after LoadPmteffTitles().

  // check if the scaling factor is valid
  if((aValue < 0) || (aValue > 100)){
    Warning("SetSingleEfficiencyValue",
	    "Efficiency of %.2f is invalid. Using default value of 0.99 instead.",aValue);
    aValue = 0.99;
    printf("------------------------------------------------------------\n");
    printf("Setting all PMT efficiencies to %.2f.\n",aValue);
  }

  // loop goes up to 10000 (size in constructor)
  for(Int_t pmtn = 0 ; pmtn < 10000 ; pmtn++){
    if((fPmteff[pmtn] != 1.)&&(fPmteff[pmtn] > 0)) fPmteff[pmtn] = (Float_t) aValue;
  }

  // set this bit before leaving to make sure 
  // the value will not be overwritten by LoadPmteffTitles()
  // in CalculateOccratio().
  fEfficienciesLoaded = kTRUE;

  return;
}
//______________________________________________________________________________
void QPath::ScaleEfficiencies(Float_t aValue)
{
  // Fill the array of Pmt efficiencies (fPmteff) with the original values
  // multiplied or divided by a scaling factor (aValue), which is forced
  // to be a value greater than one.
  // The mean of the distribution being fixed to one by construction,
  // the values below one are divided by aValue, and inversely, the values
  // above one are multiplied by aValue.
  // This effectively increases the width of the distribution, and does not
  // shift the mean by much (eliminates potential factors that the 
  // normalization could absorb in QOCAFit).
  // The old efficiencies are reloaded and checked to keep the same 
  // PMTs with valid efficiencies.
  // This function calls LoadPmteffTitles().

  // check if the scaling factor is valid
  if((aValue < 1.0) || (aValue > 100)){
    Warning("ScaleEfficiencies",
	    "Scaling factor of %.2f is invalid. Efficiencies won't be changed.",aValue);
    aValue = 1.0;
    printf("------------------------------------------------------------\n");
    printf("Scaling PMT efficiencies with a factor of %.2f.\n",aValue);
  }
  
  // forced reload efficiencies from titles
  LoadPmteffTitles();

  // loop goes up to 10000 (size in constructor)
  for(Int_t pmtn = 0 ; pmtn < 10000 ; pmtn++){
    if((fPmteff[pmtn] != 1.)&&(fPmteff[pmtn] > 0)){
      // efficiencies greater than one
      if(fPmteff[pmtn] > 1) (fPmteff[pmtn]) *= aValue;
      else (fPmteff[pmtn]) /= aValue;
    }
  }

  // set this bit before leaving to make sure 
  // the value will not be overwritten by LoadPmteffTitles()
  // in CalculateOccratio().
  fEfficienciesLoaded = kTRUE;

  return;
}
