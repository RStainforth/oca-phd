
/*************************************************************************
 * Copyright(c) 1999, The SNO Software Project, All rights reserved.     *
 * Authors: Bryce Moffat,R.Dosanjh,J.Maneira,O.Simard,N.Barros           *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include "QOCAFit.h"
#include "TMath.h"

#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#endif
//______________________________________________________________________________

ClassImp(QOCAFit)
;
//______________________________________________________________________________
//
// QOCAFit class.
//
// Mrqmin-based optical calibration fit routines, taking data from a QOCATree.
// Base class for doing simple fit with parameters as defined in May, 2000.
// The parameters are:
//    - run normalizations (1 per run)
//    - laserball angular distribution (polynomial in theta + 2d-histogram
//        for theta-phi residual correlations)
//    - PMT angular response (1 per 2 degree bin or per 0.01 interval in
//        cos(theta_pmt))
//    - attenuation length for d2o, acrylic, and h2o (3 parameters)
//
//______________________________________________________________________________
QOCAFit::QOCAFit(const char *name, const char *title) : TNamed(name,title)
{
  // Default constructor.

  fValidPars = kFALSE;
  fTreeSet = kFALSE;
  fDataSetup = kFALSE;
  SetTimeWi(-1); // Start with default time window

  fSingleNorm = kFALSE;

  fOccType = 1;  // Default is (still) OccRatio
  fTree = NULL;
  fCtree = NULL;

  fRuns = NULL;

  fCurrentrun = NULL;
  fCurrentpmt = NULL;

  fCurrentctrrun = NULL;
  fCurrentctrpmt = NULL;

  fNumberofruns = 0;
  fRunlist = NULL;
  fCentrerunlist = NULL;
  fCentrerunindex = NULL;

  fRunsToDo = NULL; // rsd
  fRunsToDoIndex = NULL; // rsd
  // fRunsInFile = NULL; // rsd
  fNumberofrunsinfile = 0; // rsd

  fCentrerunptr = NULL;

  fNumberofcentre = 0;
  fCentreruns = NULL;

  fmrqpars = NULL;  // These are initialized in InitParameters() below
  fmrqvary = NULL;
  fmrqcovar = NULL;
  fmrqalpha = NULL;

  fparscopy = NULL;
  fvarycopy = NULL;
  fcovarcopy = NULL;
  falphacopy = NULL;
  fnparscopy = 0;

  //--------------------
  // Set up the parameter counters
  // 21-Mar-2001: new defaults: 90bins, binned at center - Moffat
  //              why? easier to explain and defend than interpolation!
  fNangresp = 90;  // PMT angular response: 1deg steps, from 0 to 90 degrees
  fNlbmask = 5;    // (Degree+1) of polynomial representing laserball mask in theta
  fNbinstheta = 12;
  fNbinsphi = 36;
  fNbinsthetawave = 24;   // Allows to use a different ntheta depending on the LB model
  fNdistwave = 2;         // Amplitude and phase of sinusoidal LB model

  fLBdistType = 0;        // integer identifying the laserball model
                          // 0: histogram (default)
                          // 1: sinusoidal (few parameters)
  
  fAngrespFuncType = 2;   // integer identifying the pmt response function type
                          // 0: interpolation (requires +1 bin)
                          // 1,2: binned (edge, center)
  
  fLaserdist = NULL;      // 2D-histogram containing the laserball distribution

  InitParameters(0);      // Sets fNpars and allocates fmrqpars, vary, covar and alpha

  fmrqx = NULL;
  fmrqy = NULL;
  fmrqsig = NULL;

  fNdata = 0;
  fNpmts = 0;

  fChisquare = 0;
  fChilimit = 100.0;  // Changed from 1000.0 to 100.0 - 18-Feb-2001 - Moffat
  fChilimmin = 100.0; // New 3-Mar-2001 - Moffat
  fChimult = 10.0;
  fChicuttype = 0;

  fMinPMTperangle = 25;
  fMinPMTperLBbin = 25;
  fDynamicOccupancyCut = kFALSE; 
  fDynamicSigma = 3.;

  fNSkip = 1;
  fNStart = 0;

  fNelements = 0;
  fResarray = NULL;
  fChiarray = NULL;

  fModelErrType = 1; // By default, use angle- and wavelength-dependent errors
  fModelErr = 0.03;  // 3% error on model values (for type 2)

  fPmtVarPar0 = 0.;       // parameters for PMT variability function with theta_pmt
  fPmtVarPar1 = 0.;       // (for error type 3)
  fPmtVarPar2 = 0.;

  finorm = 1;
  fiang = 1;
  fciang = 1;
  filbdist = 1; 
  fcilbdist = 1;

  fSkipLT25 = 0;
  fSkipGT2 = 0;
  fSkipErrFrac = 0;
  fSkipBad = 0;
  fSkipBasicBad = 0;
  fSkipCentralBad = 0;

  fparmabase = 0;
  fparma = 0;
  fangindex = NULL;
  fparindex = NULL;
  fparvarmap = NULL;
  fRepeatoldmrqcof = kFALSE;

  fPrint = 1;
  fNcalls = 0;
  fNmessage = 50000;

  // fVersion is only used in GetXXXpari() to allow parameters to be propagated
  // from old versions to new versions.  The old versions should be read in, have
  // their parameter values copied to a new version instance of QOCAFit (via
  // GetParameters() or SetParameters()) - and NEVER attempt to simply redo the fit
  // using the old QOCAFit version...  I didn't check the results, and don't
  // guarantee compatibility...
  // 12-Mar-2001 - Bryce Moffat
  fVersion = QOCAFit::IsA()->GetClassVersion();
  printf("QOCAFit version %d.\n",fVersion);

  if (!(gSNO->GetPMTxyz())) {    // Ensure PMT geom loaded; sets gPMTxyz for CCC cut
    printf("Unable to locate PMT's from gPMTxyz in QOCAFit::QOCAFit()\n");
    exit(-1);
  }

}
//______________________________________________________________________________
QOCAFit::~QOCAFit()
{
  // Default destructor.

  DeAllocate();

  if (fLaserdist) {delete fLaserdist; fLaserdist = NULL;}
}
//____________________________________________________________________________
void QOCAFit::DeAllocate()
{
  // If the tree has been set up, deallocate all storage associated with the
  // tree/run arrays and with the model parameters and associated covariance
  // matrices.

  // fTreeSet is put to kTRUE by a successful (?) call to QOCAFit::SetTree().

  Int_t i;
  
  if (fTreeSet) {

    if (fRunlist) delete[] fRunlist;
    fRunlist = NULL;

    if (fCentrerunlist) delete[] fCentrerunlist;
    fCentrerunlist = NULL;

    if (fRuns) {  // Array of pointers to QOCARun's
//      for (i=0; i<fNumberofruns; i++)
      for (i=0; i<fNumberofrunsinfile; i++) // rsd
	if (fRuns[i]) delete fRuns[i];
      delete[] fRuns;
      fRuns = NULL;
    }

    if (fCentrerunindex) delete[] fCentrerunindex;
    fCentrerunindex = NULL;

    if (fCentreruns) delete[] fCentreruns;
    fCentreruns = NULL;

    if (fCentrerunptr) {  // Array of pointers to QOCARun's
      for (i=0; i<fNumberofcentre; i++)
	if (fCentrerunptr[i]) delete fCentrerunptr[i];
      delete[] fCentrerunptr;
      fCentrerunptr = NULL;
    }
  
    // rsd
    if (fRunsToDo) delete[] fRunsToDo;
    fRunsToDo = NULL; // rsd

    // rsd 
    if (fRunsToDoIndex) delete[] fRunsToDoIndex;
    fRunsToDoIndex = NULL; // rsd
 
    // Get rid of data points  
    if (fmrqx) free_vector(fmrqx,1,fNdata);
    fmrqx = NULL;
    if (fmrqy) free_vector(fmrqy,1,fNdata);
    fmrqy = NULL;
    if (fmrqsig) free_vector(fmrqsig,1,fNdata);
    fmrqsig = NULL;
  
    // Get rid of parameter storage
    DeAllocateParameters();

    if (fChiarray) delete[] fChiarray;
    if (fResarray) delete[] fResarray;
    fChiarray = NULL;
    fResarray = NULL;

    fTreeSet = kFALSE;
  }
}
//______________________________________________________________________________
void QOCAFit::AllocateParameters()
{
  // Allocate storage for parameters of the fit, destroying any previously
  // stored values.
  //
  // Updated for version 16: all parameters included independently of the
  // pmt response grouping and laserball models.

  //--------------------
  // Remove old parameter storage
  DeAllocateParameters();

  fNpars = 3                      // Attenuation lengths of d2o, acrylic, and h2o
    + 3                           // Rayleigh scattering lengths of d2o, acrylic, and h2o
    + 2*fNangresp                 // PMT Angular response(s)
    + fNlbmask                    // Laserball mask (function of theta or cos(theta))
    + fNbinstheta*fNbinsphi       // Laserball distribution (binned)
    + fNbinsthetawave*fNdistwave  // Laserball distribution (sine)
    + fNumberofruns;              // Run intensities (normalizations)
  
  fmrqpars = vector(1,fNpars);
  fmrqvary = ivector(1,fNpars);
  
  fmrqcovar = matrix(1,fNpars,1,fNpars);
  fmrqalpha = matrix(1,fNpars,1,fNpars);

  if (fPrint>=2) printf("Number of parameters %d.\n",fNpars);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::DeAllocateParameters()
{
  // De-allocate storage for parameters of the fit, destroying any previously
  // stored values.
  
  if (fmrqpars) free_vector(fmrqpars,1,fNpars);
  fmrqpars = NULL;
  if (fmrqvary) free_ivector(fmrqvary,1,fNpars);
  fmrqvary = NULL;
  
  if (fmrqcovar) free_matrix(fmrqcovar,1,fNpars,1,fNpars);
  fmrqcovar = NULL;
  if (fmrqalpha) free_matrix(fmrqalpha,1,fNpars,1,fNpars); 
  fmrqalpha = NULL;

  fValidPars = kFALSE;
}
//______________________________________________________________________________
void QOCAFit::ReAllocateNorms(Int_t nr)
{
  // Re-allocate storage for normalization parameters, at the end of all other
  // model parameters.  Preserve values of all parameters other than norms.
  //
  // Set fNumberofruns to nr when doing allocation of normalization constants.
  //
  // Preserve the value of old normalization parameters; this is used in
  // QOCAFit::SetTree() to allow a fit result to be applied to a tree using
  // the fit normalization constants without refitting, if desired.

  BackupPars();  // Make copy of parameters

  Int_t mid1;
  if (nr<fNumberofruns) mid1 = Getnormpari() + nr;
  else mid1 = Getnormpari() + fNumberofruns;

  Int_t oldmid2 = Getnormpari() + fNumberofruns;

  fNumberofruns = nr;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getnormpari() + fNumberofruns;

  RestorePars(mid1,newmid2,oldmid2,0.03);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::SetTree(QOCATree *datatree, QOCATree *centretree)
{
  // Set the OCA TREE to be used for fitting.  All runs in the datatree will
  // be stored locally in QOCAFit arrays.
  //
  // Cross-index the data runs from datatree with central runs from centretree.
  // This is essential for the occupancy ratio method of fitting: the numerator
  // and the denominator need to be calculated for the occratio prediction,
  // for centre runs with the laserball "slot" facing in different directions,
  // or other differences between central runs.
  //
  // For efficiency calibrated tubes, this just results in a series of null
  // pointers/indices which won't be taken into account when computing the model
  // predictions for the occupancy.

  Int_t i,j;
  Bool_t foundallruns = kTRUE;

  if (datatree != NULL) fTree = datatree;
  else {
    if (fPrint>=1) printf("Pointer to data tree is NULL in QOCAFit::SetTree().\n");
    return;
  }
  if (centretree != NULL) fCtree = centretree;
  else fCtree = datatree;  // Find centre runs in data if not separately specified

  if (fRunlist) delete[] fRunlist;
  if (fCentrerunlist) delete[] fCentrerunlist;
  if (fRuns) {  // Array of pointers to QOCARun's
  // why does this work?  
  // Shouldn't fNumberofruns be the number of runs selected by here?
  // Shouldn't we delete something like fNumberofrunsinfile?
//    for (i=0; i<fNumberofruns; i++)
    for (i=0; i<fNumberofrunsinfile; i++)
      if (fRuns[i]) delete fRuns[i];
    delete[] fRuns;
    fRuns = NULL;
  }

  //--------------------
  // Allocate parameters for run normalizations, keeping old normalizations
  // if possible.
  Int_t newnumberofruns = (Int_t) fTree->GetEntries();
  ReAllocateNorms(newnumberofruns);
  fNumberofrunsinfile = newnumberofruns; // rsd 2002.11

  //--------------------
  fRunlist = new Int_t[fNumberofruns];
  fCentrerunlist = new Int_t[fNumberofruns];
  fRuns = new QOCARun*[fNumberofruns];

  // Save fNdata in case it was right...
  Int_t oldfndata = fNdata;
  
  fNdata = 0;

  QOCARun *ocarun = fTree->GetQOCARun();

  for (j=0; j < fNumberofruns; j++) {
    fTree->GetEntry(j);  // Retrieve the run information

    fRuns[j] = new QOCARun();  // Copy the run information into a private array
    *fRuns[j] = *ocarun;
    fRuns[j]->CopyPMTInfo(ocarun);

    fNdata += fRuns[j]->GetNpmt();
    fRunlist[j] = fRuns[j]->GetRun();
    fCentrerunlist[j] = fRuns[j]->GetRuncentre();
  }

  // Get rid of any old storage (if required) - before re-allocating
  // This should give the possibility of reading in a QOCAFit from file and
  // preserving the exact contents for re-use and further computations!
  if (oldfndata != fNdata) {
    if (fmrqx) free_vector(fmrqx,1,oldfndata);
    if (fmrqy) free_vector(fmrqy,1,oldfndata);
    if (fmrqsig) free_vector(fmrqsig,1,oldfndata);
    if (fChiarray) delete[] fChiarray;
    if (fResarray) delete[] fResarray;
    fChiarray = NULL;
    fResarray = NULL;
  }

  //--------------------
  // Now, get the central runs into the cross-indexing arrays

  if (fCentrerunindex) delete[] fCentrerunindex;
  if (fCentreruns) delete[] fCentreruns;
  if (fCentrerunptr) {
    for (i=0; i<fNumberofcentre; i++)
      if (fCentrerunptr[i]) delete fCentrerunptr[i];
    delete[] fCentrerunptr;
    fCentrerunptr = NULL;
  }

  // Count number of distinct centre runs
  if (fPrint>=1) printf("Trying to figure out centre runs...\n");
  Bool_t distinct;

  fCentrerunindex = new Int_t[fNumberofruns]; // one per entry in fRunList
  fCentreruns = new Int_t[fNumberofruns]; // more or equal to the amount needed
  
  fNumberofcentre = 1;  // First run is distinct (by definition!)
  fCentreruns[fNumberofcentre-1] = fCentrerunlist[0];
  fCentrerunindex[fNumberofcentre-1] = 0;

  if (fPrint>=1) printf("Found first central run: %d\n",fCentrerunlist[0]);
  
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
      if (fPrint>=1) printf("Found another central run: %d (%d so far)\n",
			    fCentrerunlist[i],fNumberofcentre);
    }
  }

  QOCARun *ocarunc = fCtree->GetQOCARun(); // storage when retrieved from the tree

  fCentrerunptr = new QOCARun*[fNumberofcentre];  // ptr for each **distinct** run

  for (i=0; i<fNumberofcentre; i++) {
    fCentrerunptr[i] = new QOCARun();  // Actual QOCARun for centre runs
    fCentrerunptr[i]->SetRun(fCentreruns[i]);

    if (fCentreruns[i] == 0) {  // Check if no central run requested
      if (fPrint>=1)
	printf("No central run for some runs (Centre run offset %d).\n",i);
    } else {
      // Try to get it from the tree next - including all pmt-level contents!
      for (j=0; j<(Int_t)fCtree->GetEntries(); j++) {
	fCtree->GetEntry(j);
	if (ocarunc->GetRun() == fCentreruns[i]) {
	  if (fPrint>=1)
	    printf("Found central run %d in ocatree at position %d\n",fCentreruns[i],j);
	  *fCentrerunptr[i] = *ocarunc;
	  fCentrerunptr[i]->CopyPMTInfo(ocarunc);
	  break;  // get out of j loop searching for the run - it's been found!
	}
      }
      if (j == (Int_t)fCtree->GetEntries()) {
	delete fCentrerunptr[i];
	fCentrerunptr[i] = NULL; // Check if this is NULL before trying to use anyway!
	fprintf(stderr,"Can't find central run %d in QOCATree supplied to "
		"QOCAFit::SetTree() !\n",fCentreruns[i]);
	foundallruns = kFALSE;
      }
    }
  }

  if (!foundallruns) fprintf(stderr,"Can't find all centre runs!\n");

  //--------------------
  // fNdata is an over-estimate, as it includes all PMT's in the QOCATree and not
  // just those that are fittable (ie. not behind belly plates, ropes, pipes,
  // neck acrylic or otherwise bad).  fNpmts is the true estimate of all reasonable
  // PMT's which could be included in the fit.
  //
  // Only re-allocate if the pointers truly are NULL, and not read in from a file
  // Where one would want to preserve the contents!
  if (!fmrqx) fmrqx = vector(1,fNdata);
  if (!fmrqy) fmrqy = vector(1,fNdata);
  if (!fmrqsig) fmrqsig = vector(1,fNdata);

  //--------------------
  if (!fChiarray) fChiarray = new float[fNdata];
  if (!fResarray) fResarray = new float[fNdata];

  SetMrqArrays(fNdata,fResarray,fChiarray);

  fTreeSet = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::InitParameters(Int_t numberofruns, Int_t *runstobedone)
{
  // Overloaded to allow more flexibility in choosing runs from input file.
  // Calls InitParameters(numberofruns).

  SetRunsToDo(numberofruns,runstobedone);
  InitParameters(fNumberofruns);

}  
//______________________________________________________________________________
void QOCAFit::InitParameters(Int_t numberofruns)
{
  // Setup the parameter arrays and initialize to reasonable values (hopefully...)
  // This routine called by QOCAFit() constructor, and thereafter by the user
  // routine.  To set parameters explicitly, see the Get'ers and Set'ers for
  // individual parameters.
  //
  // Doesn't allocate space for run normalizations in the parameter sets.
  // The number of runs won't be known until SetTree() finds the off-centre runs
  // to be included in the fit.
  //
  // numberofruns <  0 : take current value for fNumberofruns and calculate everything
  // numberofruns == 0 : put fNumberofruns=0 and calculate everything
  // numberofruns >  0 : set value of fNumberofruns=numberofruns, and calc everything
  //
  // This allows the user to call InitParameters(4) for a 37-run data set, only init
  // and fit the first four runs, then InitParameters(37) to fit the whole data set
  // instead.
  //
  // 17-Feb-2001 - Bryce Moffat
  // Changed to h2o being the combined "h2o+acrylic" attenuation (from acrylic =
  // "acrylic+h2o") as the default initialization.  This brings this initialization
  // in line with current practice.  It is assumed that the h2o distances are more
  // accurate for attenuation minimizations than the acrylic path lengths.
  //
  // Default is LBmask and LBdist not variable in the fit, which will give faster
  // fits if DoFit() is called (inadvertently?) immediately after a call to
  // InitParameters().
  //
  // 08.2005 - Olivier Simard - Jose' Maneira
  // All parameters vary by default, except the acrylic attenuation and the 
  // following relative parameter normalizations:
  //   - PMT angular response at 0 deg. of incidence,
  //   - LB distribution at cos(theta) = 0, phi = pi,
  // that are fixed at 1.0.
  //
  // 06.2006 - O.Simard
  // Initial values for media attenuations are set to 0 explicitely.
  // The user is responsible for changing them after calling this function.
  // Added Rayleigh scattering lengths that are zero be default.

  Int_t i,j;

  if (numberofruns>=0) fNumberofruns = numberofruns;
  if (fPrint>=1) {
    printf("------------------------------------------------------------\n");
    printf("Initializing parameters including %d run normalizations.\n",fNumberofruns);
  }

  AllocateParameters();

  if (fPrint>=1) {
    //  printf("In QOCAFit::InitParameters(%d)\n",numberofruns);
    //  printf("And fNpars=%d\n",fNpars);
    printf("------------------------------------------------------------\n");
  }

  //--------------------
  // Set up initial parameter values.  Not all are reasonable at 1.0 -> see below.
  // NB. This list goes from 1..fNpars in Numerical Recipes, not 0..fNpars-1
  // as it would in C.

  // ------------
  // Default for all parameters is 1.0
  for (i=1; i <= fNpars; i++) fmrqpars[i] = 1.0;
  SetAllVary();

  // ------------
  // d2o attenuation length (1/cm)
  // Setd2o(1.0/10000.0);    // 17-Feb-2001 - Moffat
  Setd2o(0.0);
  SetD2OVary();           // default = kTRUE

  // ------------
  // acrylic attenuation length (1/cm)
  // Setacrylic(1.0e-10);    // 17-Feb-2001 - Moffat
  Setacrylic(0.0);
  SetAcrylicVary(kFALSE);

  // ------------
  // h2o attenuation length (1/cm) 
  // Seth2o(1.0/10000.0);    // 17-Feb-2001 - Moffat
  Seth2o(0.0);
  SetH2OVary();           // default = kTRUE

  // ------------
  // PMT relative angular response wrt normal incidence.
  // Approximate angular response by a linear increase with angle up to 36 degrees.
  // Sufficient for laserball positions inside the AV
  Float_t angle;
  for (i=1; i<fNangresp; i++) {
    angle = (0.5+i)*90.0/fNangresp; // Centre of each bin...
    if (angle<36) {
      fmrqpars[Getangresppari()+i] = 1.0 + 0.002222*angle;
      fmrqpars[Getangresp2pari()+i] = 1.0 + 0.002222*angle;
    }
    else {
      fmrqpars[Getangresppari()+i] = 1.0;
      fmrqpars[Getangresp2pari()+i] = 1.0;
    }
  }

  // SetAngRespVary() keeps the normal incidence bin fixed
  SetAngRespVary();       // default = kTRUE
  SetAngResp2Vary(kFALSE);

  // ------------
  // Laserball Mask polynomial function.
  // Standard flat LB mask starting point:
  if(fNlbmask > 0) fmrqpars[Getlbmaskpari()+0] = 1;
  for(Int_t imask = 1 ; imask < fNlbmask ; imask++) fmrqpars[Getlbmaskpari()+imask] = 0.0;
  SetMaskVary();          // default = kTRUE
  
  // ------------
  // Laserball distribution in (cos(theta), phi).
  // Standard flat LB dist starting point:
  for(Int_t i = 0 ; i < fNbinstheta*fNbinsphi ; i++) fmrqpars[Getlbdistpari()+i] = 1.0;
  // ------------
  // Laserball distribution in (cos(theta), sine(phi)).
  Float_t init;
  for(Int_t i = 0 ; i < fNbinsthetawave; i++) {
    for(Int_t j = 0 ; j < fNdistwave; j++) {
      if(j == 0) init = 0.01;    // amplitue_ac (about 1%)
      if(j == 1) init = 0.0;     // phase is zero
      fmrqpars[Getlbdistwavepari()+i*fNdistwave+j] = init;
    }
  }
    
  // SetLBDistVary() keeps the central bin fixed by default now -- jm 08.2005
  SetLBDistVary();             // default = kTRUE
  SetLBDistWaveVary(kFALSE);
  
  // ------------
  // Run normaliations.
  // Initialize Normalizations to sensible values.
  // Expected value is the "average occupancy", which should be about 0.03 = 3%
  // for typical OCA runs.  This should be equivalent to fNorm/fNpulses, prior to 
  // 21-Nov-2000, and equivalent to 1.0/(fNorm*fNpulses) afterwards
  //
  // 01.2007 - O.Simard
  // To be consistent with QPath, the normalizations are the inverse of the quantity
  // calculated in QPath, without the contribution from npulses. It is npulses-free.
  // All occurences of npulses should be commented out from here. If you want to revert
  // to the previous definition, do not forget to also change QPath.
  
  Float_t norm, npulses;

  for(i=0; i<fNumberofruns; i++) {
    j = GetRunsToDoIndex(i);       // next in list of runs to do; i<->j in fRuns below
    Getnorm()[i] = 0.03;           // Nominal is 3% occupancy
    if (fRuns) {
      // compiler suggested () around assignment used as truth value -- rsd 2002.06.03
      if ((fCurrentrun = fRuns[j])) { 
	norm = fCurrentrun->GetNorm();
	npulses = fCurrentrun->GetNpulses();
	//if (norm>0 && npulses>0) Getnorm()[i] = 1.0/(norm*npulses);
	if (norm>0) Getnorm()[i] = 1.0/norm;
      }
    }
  }
  SetNormsVary();       // default = kTRUE

  // 06.2006 - O.Simard
  // Add the Rayleigh partial attenuation lengths to the media attenuation
  // lengths if necessary. This default setting puts the three lengths to 
  // zero such that the total extinction remains the same as before. The user
  // must set them explicitely and their vary bits to enable the parameters to
  // float in the fit.
  Setrsd2o(0.0);
  Setrsacrylic(0.0);
  Setrsh2o(0.0);
  SetRayleighVary(kFALSE); // sets the three parameters

  return;
}
//______________________________________________________________________________
void QOCAFit::DoFit()
{
  // Iterate with MRQMIN until convergence of "Global Fit" is achieved... :)
  // Model parameters have previously been initialized by SetTree() or by the user.

  if (!fDataSetup) {
    fprintf(stderr,"Must set up data before QOCAFit::DoFit().\n");
    return;
  }

  if (!fTreeSet) {
    fprintf(stderr,"Must set up QOCATree before QOCAFit::DoFit().\n");
    return;
  }

  Int_t i,j;

  //--------------------
  // Fill arrays which keep track of the unique parameters to be computed
  FillParmabase();
  FillAngIndex();

  //--------------------
  if (fPrint>=1) {
    printf("----------------------------------------\n");
    printf("About to call QOCAFit::MrqFit()...using");
    if (fOccType == 1) printf(" OccRatio ");
    else printf(" Occupancy ");
    printf("as the Occupancy variable\n");
  }

  MrqFit(fmrqx, fmrqy, fmrqsig, fNpmts, fmrqpars, fmrqvary, fNpars, fmrqcovar,
	 fmrqalpha, &fChisquare);

  // Include copying single normalization to other runs, for eventual reuse
  // in Apply() or elsewhere.
  // 24-Nov-2000 - Moffat
  if (fSingleNorm) for (i=1; i<fNumberofruns; i++) Getnorm()[i] = Getnorm()[0];

  if (fPrint>=1) {
    printf("----------------------------------------\n");
    printf("Number of PMT's in fit %d, parameters %d (variable %d, fixed %d).\n",
	   fNpmts, fNpars, GetNparsVariable(), GetNparsFixed());

    // Also print the fit probability -- os
    printf("Fit complete, chisquare = %.4f, reduced = %.4f, prob = %f\n",fChisquare,
	   fChisquare/(fNpmts-GetNparsVariable()),
	   (Float_t)TMath::Prob((Double_t)fChisquare,fNpmts-GetNparsVariable()));

    if (!GetVary()) {
      printf("No variable parameters in fit.\n");
    } else {
      if (GetD2OVary()) printf("Variable D2O attenuation.\n");
      else printf("Fixed D2O Attenuation.\n");

      if (GetAcrylicVary()) printf("Variable acrylic attenuation.\n");
      else printf("Fixed acrylic attenuation.\n");

      if (GetH2OVary()) printf("Variable H2O attenuation.\n");
      else printf("Fixed H2O attenuation.\n");

      if (GetMaskVary()) printf("Variable laserball mask.\n");
      else printf("Fixed laserball mask.\n");

      if (GetAngRespVary() && GetAngResp2Vary()) printf("Variable PMT angular response, splitted in two Groups.\n");
      else if(GetAngRespVary() || GetAngResp2Vary()) printf("Variable PMT angular response.\n");
      else printf("Fixed PMT angular response.\n");

      if (GetLBDistVary()) printf("Variable laserball histogram distribution.\n");
      else printf("Fixed laserball histogram distribution.\n");
      
      if (GetLBDistWaveVary()) printf("Variable laserball sinusoidal distribution.\n");
      else printf("Fixed laserball sinusoidal distribution.\n");

      if (GetNormsVary()) {
	if (fSingleNorm) printf("Single normalization for all runs.\n");
	else printf("Variable normalizations for all runs.\n");
      } else printf("Fixed normalizations.\n");

      if (GetRayleighVary()) {
        if(GetRayleighD2OVary()) printf("Variable Rayleigh scattering in D2O.\n");
        else printf("Fixed Rayleigh scattering in D2O.\n");
        if(GetRayleighAcrylicVary()) printf("Variable Rayleigh scattering in acrylic.\n");
        else printf("Fixed Rayleigh scattering in acrylic.\n");
        if(GetRayleighH2OVary()) printf("Variable Rayleigh scattering in H2O.\n");
        else printf("Fixed Rayleigh scattering in H2O.\n");
      } else printf("Fixed Rayleigh scattering in all media.\n");
    }
  }

  if (fPrint>=1) {
    printf("----------------------------------------\n");
    printf("D2O     Attenuation: %12.8f cm^-1 (%12.8f cm) +/- %12.8f\n",
	   Getd2o(),1.0/Getd2o(),sqrt(fabs(fmrqcovar[1][1])));
    printf("Acrylic Attenuation: %12.8f cm^-1 (%12.8f cm) +/- %12.8f\n",
	   Getacrylic(),1.0/Getacrylic(),sqrt(fabs(fmrqcovar[2][2])));
    printf("H2O     Attenuation: %12.8f cm^-1 (%12.8f cm) +/- %12.8f\n",
	   Geth2o(),1.0/Geth2o(),sqrt(fabs(fmrqcovar[3][3])));
    if(GetAngRespVary()){
      printf("----------------------------------------\n");
      printf("PMT Angular Response:\n");
      for (i=0; i<fNangresp; i++) {
        printf("%12.6f ",Getangresp()[i]);
        if (i%6 == 0) printf("\n");
      }
      printf("\n");
    }
    if(GetAngResp2Vary()){
      printf("----------------------------------------\n");
      printf("PMT Angular Response Group2:\n");
      for (i=0; i<fNangresp; i++) {
        printf("%12.6f ",Getangresp2()[i]);
        if (i%6 == 0) printf("\n");
      }
      printf("\n");
    }
    printf("----------------------------------------\n");
    printf("LB Mask:\n");
    for (i=0; i<fNlbmask; i++) {
      printf("%12.8f ",Getlbmask()[i]);
      if (i%6 == 0) printf("\n");
    }
    printf("\n");
    
    printf("----------------------------------------\n");
    printf("LB Distribution:\n");
    if(fLBdistType){
      for (i=0; i<fNbinsthetawave; i++) {
	 for (j=0; j<fNdistwave; j++) {
	   printf("%12.8f",Getlbdistwave()[i*fNdistwave+j]);
	 }
	 printf("\n");
      }
    } else {
      for (i=0; i<fNbinstheta; i++) {
	for (j=0; j<fNbinsphi; j++) {
	  printf("%12.8f ",Getlbdist()[i*fNbinsphi+j]);
	  if (j%6 == 0) printf("\n");
	}
	printf("\n");
      }
    }
    printf("\n");
    printf("----------------------------------------\n");
    printf("Run Normalizations: \n");
    for (i=0; i<fNumberofruns; i++ ) {
      printf("%12.8f ",Getnorm()[i]);
      if (i%6 == 0) printf("\n");
    }
    printf("\n");

    printf("----------------------------------------\n");
    printf("D2O     Rayleigh Scattering: %12.8f cm^-1 (%12.8f cm)\n",
	   Getrsd2o(),1./Getrsd2o());
    printf("Acrylic Rayleigh Scattering: %12.8f cm^-1 (%12.8f cm)\n",
	   Getrsacrylic(),1./Getrsacrylic());
    printf("H2O     Rayleigh Scattering: %12.8f cm^-1 (%12.8f cm)\n",
	   Getrsh2o(),1./Getrsh2o());
    printf("\n");
  }
}
//________________________________________________________________________________
void QOCAFit::DataSetup()
{

  fSkipLT25 = 0;
  fSkipGT2 = 0;
  fSkipErrFrac = 0;
  fSkipBad = 0;
  fSkipBasicBad = 0;
  fSkipCentralBad = 0;
  // Setup up the Mrqmin arrays to contain only reasonable tubes for further
  // calculations of chi-squared (eg. during minimization).
  //
  // Moffat - 6-Mar-2001
  // Set status bits for PMT's which are cut from the QOCATree for the fit.
  // The meaning of the bits is:
  //   bit 0 (1<<0 = 1)  : Cut by Occratio or Occratioerr of Bad light path
  //   bit 1 (1<<1 = 2)  : Cut by Chisquared minimum limit
  //   bit 2 (1<<2 = 4)  : Cut by bad crate/card/channel (NB. usually none after chisq!)
  //   bit 3 (1<<3 = 8)  : Cut because PMTR bin with too few PMT's
  // These cuts are all sequential, and some (eg. chisq and CCC) would have cut the
  // same tubes, if given the chance...
  //
  // And finally, for compatibility in Apply() the following bit is set if the
  // PMT was _not_ used in the fit:
  //   bit 8 (1<<8 = 256): Not present in fmrqx array
  // This allows old QOCAFit results to be Apply()'ed and still flag PMT's which
  // weren't in the fit.

  // This routine should really only be called after a QOCAFit::SetTree().
  if (!fTree || !fCtree || !fCentrerunptr) {
    fprintf(stderr,"Must set up data tree and central run tree before "
	    "QOCAFit::DataSetup().\n");
    return;
  }

  Int_t jpmt,run, nSkip;
  Int_t i,j;
  Int_t ix;
  Float_t residual, chisquared;
  Float_t modelpred;
  Float_t sigma, wsigma;
  Float_t occratio, woccratio;
  Float_t occupancy, occupancyerr, woccupancy, woccupancyerr;
  Float_t wnprompt;
  Float_t npulses;

  fNpmts = 0;

  if (fPrint>=1) {
    printf("About to initialize data point arrays...\n");
  }

  // Count PMT's with given incident angles; some run sets may not have all angles,
  // which leads to a singular matrix in the fit...
  Int_t *pmtanglevalid = new Int_t[fNangresp];
  Int_t *pmtangle2valid = new Int_t[fNangresp];
  Int_t ianglevalid;
  Float_t angle;
  for (i=0; i<fNangresp; i++) {
    pmtanglevalid[i] = 0;
    pmtangle2valid[i] = 0;
  }
  // Count and index for mrqmin all the PMT's which look reasonable, in any or
  // all runs.

  fChisquare = 0.0;
  Int_t nPMTs = 0;
  for (run=0; run < fNumberofruns; run++) {
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    nPMTs+=fCurrentrun->GetNpmt();
    printf( "run %i has %i PMTs\n", run, fCurrentrun->GetNpmt() );
  }
  printf("TOTAL NUMBER OF PMTS IS: %i\n", nPMTs );
  
     

  for (run=0; run < fNumberofruns; run++) {
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    npulses = fCurrentrun->GetNpulses();  // For reconstruction windowed occupancy
    // --------------------
    // os -- 08.2005
    // flag fDynamicOccupancyCut must be turned on in order to apply
    // the occupancy cut based on mean occupancy and sigma.
    // The occupancy variable needs to be the same as in PmtSkip()
    Float_t occ, dcmeanocc = 1., dcsigma = 1.;
    if(fDynamicOccupancyCut){
      // first loop for mean
      for(Int_t ipmt = 0 ; ipmt < fCurrentrun->GetNpmt() ; ipmt++){
        fCurrentpmt = fCurrentrun->GetPMT(ipmt);
        if(fOccType == 0){ // occupancy
          occ = fCurrentpmt->GetOccupancy() * fCurrentrun->GetNorm();
        } else { // occratio
          occ = fCurrentpmt->GetOccratio() * fCurrentrun->GetNorm();
        }
        dcmeanocc += occ;
      }
      if(fCurrentrun->GetNpmt() != 0){
        dcmeanocc /= (Float_t)fCurrentrun->GetNpmt();
      }
      // second loop for sigma
      for(Int_t ipmt = 0 ; ipmt < fCurrentrun->GetNpmt() ; ipmt++){
        fCurrentpmt = fCurrentrun->GetPMT(ipmt);
        if(fOccType == 0){ // occupancy
          occ = fCurrentpmt->GetOccupancy() * fCurrentrun->GetNorm();
        } else { // occratio
          occ = fCurrentpmt->GetOccratio() * fCurrentrun->GetNorm();
        }
	dcsigma += pow((occ - dcmeanocc),2);
      }
      dcsigma = sqrt(dcsigma/(Float_t)fCurrentrun->GetNpmt());
    }
    // --------------------

    for (jpmt=0; jpmt < fCurrentrun->GetNpmt(); jpmt++) {
      // Make sure this PMT is OK.
      fCurrentpmt = fCurrentrun->GetPMT(jpmt);
      fCurrentpmt->SetFitstatus(0); // Reset Fitstatus completely

      // apply PmtSkip() based on the fDynamicOccupancyCut flag
      Bool_t pmtskip;
      if(fDynamicOccupancyCut) pmtskip = PmtSkip(dcmeanocc,dcsigma);
      else pmtskip = PmtSkip();

      if(pmtskip){ // settable condition
	// PMT isn't ok and should be skipped
	fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() | (1<<0) );
    nSkip++;
      } else {
	// PMT is reasonable and should be in the fit - at least for now
	fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() & ~(1<<0) );

        modelpred = Model(run,jpmt);
	residual = GetOccVariable() - modelpred;
	sigma = sqrt(pow(GetOccVariableErr(),2) +
		     pow(ModelErr(run,jpmt,GetOccVariable()),2));
	chisquared = residual*residual/(sigma*sigma);
    // if ( run / 10000 == 0 && jpmt % 50 == 0 ){
    //   printf("run %i, jpmt %i\n", run, jpmt );
    //     printf("modelPred is: %.5f\n", modelpred );
    //     printf("data val is: %.5f\n", GetOccVariable() );
    //     printf("sigma is: %.5f\n", sigma );
    //     printf("chisquare is: %.5f\n", chisquared);
    //     printf("--------------------\n");
    // }
      
	if(fPrint>=2) printf("%.4d %10.8g %10.8g %10.8g %10.8g\n",jpmt,modelpred,residual,sigma,chisquared);
	if(!(chisquared > 0 || chisquared <= 0)){
	  Warning("DataSetup","nan at PMT %d run %d\t",jpmt,fCurrentrun->GetRun());	
	  printf("%.2f %.2f %.2f\n",GetOccVariableErr(),modelpred,ModelErr(run,jpmt,GetOccVariable()));
	  continue;
	}
	fChisquare += chisquared;
	if (fNpmts>=0 && fNpmts<fNelements-1) {
	  fChiarray[fNpmts+1] = chisquared;
	  fResarray[fNpmts+1] = residual;
	}

	fmrqx[fNpmts+1] = (Float_t) (jpmt + run*10000);  // Special indexing scheme
	
	if (fTimeWi<0) {
	  fmrqy[fNpmts+1] = GetOccVariable();
	  fmrqsig[fNpmts+1] = sigma;
	} else {
	  // Fudge a little the error calculation for other time windows.
	  // Relies on the occratio in fOccW[i] corresponding to the statistics to
	  // be used in the error calculation...  Hmmm...
	  // 12-Nov-2000 - Bryce Moffat
	  // fmrqsig[fNpmts+1] = fmrqy[fNpmts+1] *
	  //     sqrt(pow(sigma/fCurrentpmt->GetOccratio(),2)
	  //     - pow(fCurrentpmt->GetOccupancyerr()/fCurrentpmt->GetOccupancy(),2)
	  //     + 1.0/fmrqy[fNpmts+1]);
	  //....................
	  // Actually, this is not the correct calculation to estimate the error
	  // on the fOccW[i] _occratio_.  Using 1.0/fmrqy[fNpmts+1] = 1.0/fOccW[i]
	  // gives a bogus occupancy error estimate because fOccW[i] already includes
	  // the corrections for: solid angle (fGeomratio) and central run occupancy
	  // (fPmteff - stored the central run occupancy as the efficiency in
	  // QPath::CalculateOccratio(), which is stored in the "optix" QOCATree).
	  //
	  // To reconstruct the window occupancy and approximate error (statistical):
	  //   fOccW[i] : window occratio = occupancy[i]/coccupancy[nominal]*geomratio
	  //   woccupancy : window occupancy = fOccW[i] * coccupancy[nominal]/geomratio
	  //                                 = fOccW[i] * fPmteff/fGeomratio
	  //   woccupancyerr : window occupancy error = sqrt(woccupancy) / woccupancy
	  //                                          = 1.0 / sqrt(woccupancy)
	  //
	  // The actual error on the occratio derived for a given window width is
	  // reconstructed from the nominal error sigma (see above) by subtracting
	  // the contribution from the nominal time window occupancy, and adding the
	  // error contribution from the i'th window occupancy.
	  //
	  // fmrqsig[fNpmts+1] = woccratio *
	  //   sqrt(pow(sigma/fCurrentpmt->GetOccratio(),2)
	  //   - pow(fCurrentpmt->GetOccupancyerr()/fCurrentpmt->GetOccupancy(),2)
	  //   + 1.0/woccupancy);
	  // 15-Feb-2001 - Bryce Moffat
	  //....................
	  // Another correction!  It seems the MPE correction has an effect on
	  // the occratio error per pmt which systematically may shift arount the
	  // QOCAFit result, particularly for alpha_(h2o+acrylic).
	  //
	  // woccupancyerr is improperly estimated above: it depends on the MPE
	  // correction.  The goal of these calculations is to obtain the correct
	  // errors on the windowed occratios.  The most obvious diagnostic is for
	  // the nominal 4-ns and windowed 4-ns windows to agree in all respects!
	  //
	  // Luckily, it turns out we can reconstruct the correct errors from the
	  // quantities stored in the tree.  The main new quantities are:
	  //   wnprompt = npulses * ( 1 - exp(-woccupancy / npulses) )
	  //   woccupancyerr = sqrt(wnprompt) / ( 1 - wnprompt / npulses)
	  //                 = sqrt(wnprompt) / exp( - woccupancy / npulses )
	  //
	  // NB.: woccupancy = - npulses * log(1 - wnprompt/npulses)
	  //      -woccupancy/npulses = log(1 - wnprompt/npulsees)
	  //      exp(-woccupancy/npulses) = 1 - wnprompt/npulses
	  // (useful for equations below).
	  //
	  // 6-Mar-2001 - Bryce Moffat

	  //printf("DataSetup() called for a different time window: %d ns.\n",(Int_t)fTimeWi);
	  
	  woccratio = fCurrentpmt->GetOccratioW(fTimeWi);
	  woccupancy = fCurrentpmt->GetOccupancyW(fTimeWi);
	  wnprompt = npulses * ( 1 - exp(-woccupancy/npulses) );
	  woccupancyerr = sqrt(wnprompt) / exp(-woccupancy/npulses);
	  occratio = fCurrentpmt->GetOccratio();
	  occupancy = fCurrentpmt->GetOccupancy();
	  occupancyerr = fCurrentpmt->GetOccupancyerr();
	  
	  wsigma = sqrt(pow(fCurrentpmt->GetOccratioerr(),2) -
			pow(occratio * occupancyerr/occupancy,2) +
			pow(woccratio * woccupancyerr/woccupancy,2) +
			pow(ModelErr(run,jpmt,woccratio),2));
	  
	  if(fOccType == 1){ // occratio  
	    fmrqy[fNpmts+1] = woccratio;
	    fmrqsig[fNpmts+1] = wsigma;
          } else {           // occupancy
	    fmrqy[fNpmts+1] = woccupancy;
	    fmrqsig[fNpmts+1] = sqrt(pow(woccupancyerr,2) + 
				     pow(ModelErr(run,jpmt,woccupancy),2));
          }
          
	}
	fNpmts++;
      }
    }
  }
  
  if (fPrint>=1)
    printf("Done initializing arrays to be fit with %d tubes total.\n",fNpmts);
    printf("Total Skipped %i tubes total.\n",nSkip);
    printf("Total Skipped: occvar > 2.0: %i\n", fSkipGT2);
    printf("Total Skipped: ErrFrac : %i\n", fSkipErrFrac);
    printf("Total Skipped: Bad PMTs: %i\n", fSkipBad);
    printf("Total Skipped: Basic Bad PMTs: %i\n", fSkipBasicBad);
    printf("Total Skipped: Central Bad PMTs: %i\n", fSkipCentralBad);
    printf("Total: %i = %i\n", nSkip, ( fSkipGT2 + fSkipErrFrac + fSkipBad + fSkipBasicBad + fSkipCentralBad ) );
	
  //--------------------
  // First remove the high-contributions to chisquared
  if (fPrint>=1)
    printf("Chilimit is %g, while Chisquare is %g for %d degrees of freedom.\n",
	   fChilimit,fChisquare,fNpmts-GetNparsVariable());

  fChilimit = fChisquare/(fNpmts-GetNparsVariable());
  if (fPrint>=1) printf("Reduced Chi squared: %.2f\n",fChilimit);

  // Modifications: 3-Mar-2001 - Moffat
  // Allow various types of cutting limits on PMT chisquared values.
  // Original version: impose 10*chisq/dof, no bound checks.
  // 18-Feb-2001: impose 10*chisq/dof _or_ fixed minimum of 100
  // 3-Mar-2001: impose a fixed minimum, or use one of the two previous cut versions
  //             with a variable multiplier
  fChilimit *= fChimult;

  if (fChicuttype==1) { // force the cut value to fChilimmin
    fChilimit = fChilimmin;
    if (fPrint>=1) printf("Forcing Chilimit to %g (fixed value!)\n",fChilimit);
  } else if (fChicuttype==2) { // use 10*chisq/dof
    if (fPrint>=1) printf("Leaving Chilimit at %g\n",fChilimit);
  } else {
    // fChicuttype==0 (default): use 10*chisq/dof _or_ fChilimmin
    if (fChilimit < fChilimmin) {
      fChilimit = fChilimmin;
      if (fPrint>=1) printf("Resetting Chilimit to %g (minimum value!)\n",fChilimit);
    } else {
      if (fPrint>=1) printf("Resetting Chilimit to %g\n",fChilimit);
    }
  }

  // Eliminate tubes with high chi-squared from the fit.
  j = 0;
  for (i=1; i<=fNpmts; i++) {
    ix = (Int_t) fmrqx[i];
    run = ix/10000;
    jpmt = ix%10000;
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    fCurrentpmt = fCurrentrun->GetPMT(jpmt);

    if (fChiarray[i] < fChilimit) {
      // PMT passes the chisquared cut, so clear bit 1:
      fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() & ~(1<<1) );

      fmrqx[j+1] = fmrqx[i];
      fmrqy[j+1] = fmrqy[i];
      fmrqsig[j+1] = fmrqsig[i];
      j++;
    } else {
      // PMT fails the chisquared cut, so set bit 1:
      fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() | (1<<1) );
    }
  }

  //--------------------
  if (fPrint>=1) {
    printf("Chilimit is %10.5f\n",fChilimit);
    printf("%d PMT's total, of which %d below and %d above Chilimit\n",
	   fNpmts,j,fNpmts-j);
  }
  fNpmts = j;

  //--------------------
  // Next, remove tubes from identified bad crates and cards.
  // Count number of PMT's in each PMT response bin.
  //
  // There was a bug in this section - found 6-Mar-2001 - Moffat
  // The run index "run" and pmt index "pmtn" were being checked against run numbers
  // and crate/card/channels.  This is corrected to refer to the actual QOCARun and
  // QOCAPmt data structures, which should produce the correct behaviour.
  // NB. To date, this cut never cut any channels - this was probably the biggest
  // reason why!

  
  // os -- do not print the following if unrelevant
  if (fPrint>=1 && (fCurrentrun->GetRun() > 12600 && fCurrentrun->GetRun() < 13300)) {  
    printf("Removing Crate 8, Card 10 and Crate 1, Card 2 for September 2000 data.\n");
    printf("fRun > 12600 && fRun < 13300 are affected by this CCC cut.\n");
  }
  
  Int_t crate, card;
  Int_t runnumber, pmtnumber;
  
  Float_t lbdist, pmtr, interpolfrac;
  Int_t nbinslbdist = fNbinstheta*fNbinsphi;
  Int_t lbpari;
  Int_t* lbvalid = new Int_t[nbinslbdist];
  for(i=0;i<nbinslbdist;i++) lbvalid[i]=0;

  j = 0;
  for (i=1; i<=fNpmts; i++) {
    ix = (Int_t) fmrqx[i];
    run = ix/10000;
    jpmt = ix%10000;
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    fCurrentpmt = fCurrentrun->GetPMT(jpmt);

    runnumber = fCurrentrun->GetRun();
    pmtnumber = fCurrentpmt->GetPmtn();

    if ((runnumber>12600) && (runnumber<13300)) { // Check that it's September 2000 data
      crate = gPMTxyz->GetCrate(pmtnumber);
      if (crate==8 || crate==1) {
	card = gPMTxyz->GetCard(pmtnumber);
	if ((crate==8 && card==10) || (crate==1 && card==2)) {
	  // PMT not in reasonable CCC - set status bit 2 and skip it.
	  fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() | (1<<2) );
	  continue;
	}
      }
    }
    // PMT in reasonable CCC - clear status bit 2 and store it:
    fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() & ~(1<<2) );

    // Count number of PMT's in each angular bin
    pmtr = ModelAngResp(fCurrentpmt,ianglevalid,interpolfrac); // called to set ianglevalid only
    if(PmtGroup(fCurrentpmt) == 2) pmtangle2valid[ianglevalid]++;
    else pmtanglevalid[ianglevalid]++;
	
    // Count number of PMT's in each lbdist bin
    lbdist = ModelLBDist(fCurrentrun,fCurrentpmt,lbpari); // called to set lbpari only
    lbvalid[lbpari]++;
    
    fmrqx[j+1] = fmrqx[i];
    fmrqy[j+1] = fmrqy[i];
    fmrqsig[j+1] = fmrqsig[i];
    j++;
  }

  //--------------------
  
  // os -- do not print if unrelevant
  if (fPrint>=1 && (fCurrentrun->GetRun() > 12600 && fCurrentrun->GetRun() < 13300)) {
    printf("%d PMT's total, of which %d cut in identified bad channels\n",
	   fNpmts,fNpmts-j);
  }
  fNpmts = j;


  //--------------------
  // Only allow bins in laserball angular distribution with more than 10 PMT's 
  // and  bins in PMT angular response with more than 25 PMT's in them to vary
  // NB. Must eliminate PMT's from the data set if they're in a bin with less
  // than 10 others - see below!
  //
  // 09.2006 - O.Simard
  // And just as a safety net the parameters that are fixed by the following
  // inspection are explicitely set to their default value (1.0).
  //
  // First check the LB
  //
  
  Int_t lbbinsinvalid = 0; // All angles valid a priori
  for (i=0; i<nbinslbdist; i++) {
    if (lbvalid[i] < fMinPMTperLBbin) {
      fmrqpars[Getlbdistpari()+i] = 1.; // set to default
      fmrqvary[Getlbdistpari()+i] = 0;
      if (lbvalid[i] > 0) {
	lbbinsinvalid++;  // only count those with some PMT's!
	if(!fLBdistType){
	  printf("Invalid LB distribution bin %d, itheta %d iphi %d: only %d PMT's\n",
	       i,i/fNbinsphi,i%fNbinsphi,lbvalid[i]);
	}
      }
    }
  }
  
  // Now the angular response
  //
  Int_t anglesinvalid = 0;  // All angles valid a priori
  Int_t angles2invalid = 0; // All angles valid a priori
  Int_t aux_anglevalid = 0;
  for (i=1; i<fNangresp; i++) {
    if (pmtanglevalid[i] < fMinPMTperangle) {
      fmrqpars[Getangresppari()+i] = 1.; // set to default
      fmrqvary[Getangresppari()+i] = 0;
      if (pmtanglevalid[i] > 0) {
	anglesinvalid++;  // only count those with some PMT's!
	printf("Invalid angular response bin %d, angle %6.2f: only %d PMT's\n",
	       i,i*90.0/fNangresp,pmtanglevalid[i]);
      }
    }
    if (pmtangle2valid[i] < fMinPMTperangle) {
      fmrqpars[Getangresp2pari()+i] = 1.; // set to default
      fmrqvary[Getangresp2pari()+i] = 0;
      if (pmtangle2valid[i] > 0) {
	angles2invalid++;  // only count those with some PMT's!
	printf("Invalid group2 angular response bin %d, angle %6.2f: only %d PMT's\n",
	       i,i*90.0/fNangresp,pmtangle2valid[i]);
      }
    }
  }

  for ( Int_t iBin = 1; iBin < fNangresp; iBin++ ){ 
    if ( pmtanglevalid[ iBin ] > 0 ){  
      printf("Angular response bin %d, Angle %6.2f: has %d PMT's\n",
             iBin,
             iBin * (90.0 / fNangresp ),
             pmtanglevalid[ iBin ] );
    }
  }
  
  
  if (lbbinsinvalid == 0 && anglesinvalid == 0 && angles2invalid){
    printf("No LB dist bins have less than %d PMT's and no PMT Response bins have less than %d PMT's\n",
	   fMinPMTperLBbin,fMinPMTperangle);
  } else {  // If any bin has too few PMT's in it, get rid of them
    if (fPrint>=1){
      if(!fLBdistType){
        if(lbbinsinvalid > 0) printf("%d LB dist bins have less than %d PMT's in them.\n",
				   lbbinsinvalid,fMinPMTperLBbin);
      }
      if(anglesinvalid > 0) printf("%d PMT Response bins have less than %d PMT's in them.\n",
				   anglesinvalid,fMinPMTperangle);
      if(angles2invalid > 0) printf("%d Group2 PMT Response bins have less than %d PMT's in them.\n",
				    angles2invalid,fMinPMTperangle);
    }
    
    j=0;
    for (i=1; i<=fNpmts; i++) {
      ix = (Int_t) fmrqx[i];
      run = ix/10000;
      jpmt = ix%10000;
      fCurrentrun = fRuns[GetRunsToDoIndex(run)];
      fCurrentpmt = fCurrentrun->GetPMT(jpmt);
      
      lbdist = ModelLBDist(fCurrentrun,fCurrentpmt,lbpari);
      pmtr = ModelAngResp(fCurrentpmt,ianglevalid,interpolfrac);
      if(PmtGroup(fCurrentpmt) == 2) aux_anglevalid = pmtangle2valid[ianglevalid];
      else aux_anglevalid = pmtanglevalid[ianglevalid];
      
      if(!fLBdistType){ // for histogram lbdist
        if (lbvalid[lbpari] >= fMinPMTperLBbin && aux_anglevalid >= fMinPMTperangle) {
          // keep a tube only if it is in a good lbdist AND a good pmtr bin
          // PMT in reasonable lb dist bin - clear status bit 3
          fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() & ~(1<<3) );
          
          fmrqx[j+1] = fmrqx[i];
          fmrqy[j+1] = fmrqy[i];
          fmrqsig[j+1] = fmrqsig[i];
          j++;
        } else {
	  // PMT not in reasonable LB dist bin - set status bit 3
	  fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() | (1<<3) );
	  if (fPrint>=2) {
	    if(lbbinsinvalid > 0) printf("Eliminated a tube at lb bin %d\n",lbpari);
	    if(anglesinvalid > 0) printf("Eliminated a tube at angle %8.3f and %d\n",
					 angle*180/M_PI,ianglevalid);
	    if(angles2invalid > 0) printf("Eliminated a group2 tube at angle %8.3f and %d\n",
					  angle*180/M_PI,ianglevalid);
	  }
        }
      } else { // for sinusoidal lbdist there is no such rejection
	if (aux_anglevalid >= fMinPMTperangle) {
	  // keep a tube only if it is in a good pmtr bin
	  // PMT in reasonable pmtr bin - clear status bit 3
	  fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() & ~(1<<3) );
	
	  fmrqx[j+1] = fmrqx[i];
	  fmrqy[j+1] = fmrqy[i];
	  fmrqsig[j+1] = fmrqsig[i];
	  j++;
	} else {
	  // PMT not in reasonable LB dist bin - set status bit 3
	  fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() | (1<<3) );
	  if (fPrint>=2) {
	    if(anglesinvalid > 0) printf("Eliminated a tube at angle %8.3f and %d\n",
					 angle*180/M_PI,ianglevalid);
	    if(angles2invalid > 0) printf("Eliminated a group2 tube at angle %8.3f and %d\n",
					  angle*180/M_PI,ianglevalid);
	  }
	}
      }
    }
    
    if (fPrint>=1)
      printf("%d PMT's total, of which %d in pmtr bins with < %d others or lb dist bins with < %d\n",
	     fNpmts,fNpmts-j,fMinPMTperangle,fMinPMTperLBbin);
    fNpmts = j;
  }

  //--------------------
  // For interpolated PMTR (fAngrespFuncType == 0), set the PMT Response to be
  // variable one bin past the end of data, for correct interpolation using pari
  // and pari+1.
  if (fAngrespFuncType == 0) {
    for (i=1; i<fNangresp; i++) {
      if ((fmrqvary[Getangresppari()+i]==1) && (fmrqvary[Getangresppari()+i+1]==0)) {
	fmrqvary[Getangresppari()+i+1] = 1;
	printf("Setting PMTR bin %d to variable as well\n",i+1);
	break;
      }
    }
    for (i=1; i<fNangresp; i++) {
      if ((fmrqvary[Getangresp2pari()+i]==1) && (fmrqvary[Getangresp2pari()+i+1]==0)) {
	fmrqvary[Getangresp2pari()+i+1] = 1;
	printf("Setting group2 PMTR bin %d to variable as well\n",i+1);
	break;
      }
    }
  }

  //--------------------
  // Set the message printout interval for QOCAFit::mrqfuncs()
  Int_t power = (Int_t) (log10((double)fNpmts) - 0.5);
  fNmessage = (Int_t) pow(10.0,power);

  delete [] pmtanglevalid;
  delete [] pmtangle2valid;
  delete [] lbvalid;

  PmtGroupCovar();

  //--------------------
  if (fNpmts > 0) fDataSetup = kTRUE;
  else {
    printf("No PMT's left to be fit - check the input QOCATree.\n");
    fDataSetup = kFALSE;
  }
  Int_t nParVary = 1;
  for ( Int_t iPar = 1; iPar <= fNpars; iPar++ ){
    if ( fmrqvary[ iPar ] ){
      printf("Parameter: %i has value %.10f which is the %i parameter that varies\n", iPar, fmrqpars[ iPar ], nParVary );
      nParVary++;
    }
  }

  return;
}

//______________________________________________________________________________
Bool_t QOCAFit::PmtSkip()
{

  Float_t occvar;
  Bool_t pmtskip = false;

  if (fOccType == 0) {
    // Occupancy method : reduce occupancy to a number close to 1
    // skip if too large or too low or too low
    //      if low statistics
    //      if dqxx/chcs flagged or bad path
    //      if we have no efficiency measurement (==1.0)
    // but don't skip if central path is bad
    printf("IS THIS EVER CALLED?\n");

    occvar = fCurrentpmt->GetOccupancy() * fCurrentrun->GetNorm();

    pmtskip = (occvar <= 0.05) || (occvar > 5.0) ||
      (GetOccVariableErr()/GetOccVariable() >0.25) ||
      fCurrentpmt->GetBad() || fCurrentpmt->IsBasicBad() || 
      (fCurrentpmt->GetPmteffc() == 1.0);
  } else {
    // OccRatio method : already close to 1
    // skip if too large or too low or too low
    //      if low statistics
    //      if dqxx/chcs flagged or bad path
    //      if central path is bad

    // remove geometry corrections
    occvar = fCurrentpmt->GetOccratio() * fCurrentrun->GetNorm();
    // pmtskip = (occvar <= 0.25) || (occvar > 2.0) ||
    //   (GetOccVariableErr()/GetOccVariable() >0.25) ||
    //   fCurrentpmt->GetBad() || fCurrentpmt->IsBasicBad() ||
    //   fCurrentpmt->GetCentralBad();
    // occvar = fCurrentpmt->GetOccratio() * fCurrentrun->GetNorm();
    if ( fCurrentpmt->IsBasicBad() ){ 
      if ( !pmtskip ){
        pmtskip = true;
        fSkipBasicBad++;
      }
    }
    else if ( fCurrentpmt->GetCentralBad() ){ 
      if ( !pmtskip ){
        pmtskip = true;
        fSkipCentralBad++;
      }
    }
    else if ( fCurrentpmt->GetBad() || occvar < 0.0 ){ 
      if ( !pmtskip ){
        pmtskip = true;
        fSkipBad++;
      }
    }
    else if ( (GetOccVariableErr()/GetOccVariable() > 0.25) ){
      if ( !pmtskip ){
        pmtskip = true;
        fSkipErrFrac++;
      }
    }
    else if ( occvar > 2.0 || occvar <= 0.25 ){ 
      if ( !pmtskip ){
        pmtskip = true;
        fSkipGT2++;
      }
    }
    else{ pmtskip = false; }
     
  }	

  // excessive info for debugging
  if(fPrint >= 2){
    printf("occ %5.0f occvar %5.0f err %2.4f eff %1.3f bad %1.0d\n",
	   GetOccVariable(),occvar,GetOccVariableErr()/GetOccVariable(),
	   fCurrentpmt->GetPmteffc(),fCurrentpmt->GetBad());
    printf("occ %5.0f occvar %f norm %1.5f fbad %d run %d pmt %d pmtz %3.0f \n",
	   GetOccVariable(),occvar,fCurrentrun->GetNorm(),fCurrentpmt->GetBad(),fCurrentrun->GetRun(),
	   fCurrentpmt->GetPmtn(),fCurrentpmt->GetPmtz());
  }
  return pmtskip;
  
}

//______________________________________________________________________________
Bool_t QOCAFit::PmtSkip(Float_t meanocc, Float_t sigma)
{
  // os -- 08.2005
  // Similar to PmtSkip() but uses the sigma factor input by the user
  // and previously calculated meanocc and sigma.
  if(meanocc < 0 || sigma <= 0){
    fprintf(stderr,"In QOCAFit::PmtSkip(meanocc,sigma) : Bad inputs.\n");
    return kFALSE; // no PMTs to fit if bad inputs
  }

  Float_t occvar;
  Bool_t pmtskip;
  if (fOccType == 0) {
    // Occupancy method : reduce occupancy to a number close to 1
    // skip if too large or too low or too low if low statistics
    //      if dqxx/chcs flagged or bad path 
    //      if we have no efficiency measurement (==1.0)
    // but don't skip if central path is bad
    
    occvar = fCurrentpmt->GetOccupancy() * fCurrentrun->GetNorm();

    pmtskip = (Bool_t)((occvar <= 0) || 
       (fabs(occvar - meanocc) > fDynamicSigma*sigma) || // outside user's limits
       (GetOccVariableErr()/GetOccVariable() > 0.25) ||
       fCurrentpmt->GetBad() || fCurrentpmt->IsBasicBad() ||
       (fCurrentpmt->GetPmteffc() == 1.0));
  } else {
    // OccRatio method : already close to 1
    // skip if too large or too low or too low if low statistics
    //      if dqxx/chcs flagged or bad path
    //      if central path is bad
    occvar = fCurrentpmt->GetOccratio() * fCurrentrun->GetNorm();
    pmtskip = (Bool_t)((occvar <= 0) || 
       (fabs(occvar - meanocc) > fDynamicSigma*sigma) || // outside user's limits
       (GetOccVariableErr()/GetOccVariable() > 0.25) ||
       fCurrentpmt->GetBad() || fCurrentpmt->IsBasicBad() ||
       fCurrentpmt->GetCentralBad());
  }	

  return pmtskip;
}
//______________________________________________________________________________
Int_t QOCAFit::PmtGroup(QOCAPmt *pmt)
{
  // jm -- 06.2006
  // PMTs are divided in two groups with different angular responses
  // Returns the group of this one
  // implemented for Batch3 reflectors selection

  Int_t pmtn = pmt->GetPmtn();
  return PmtGroup(pmtn);
}
//______________________________________________________________________________
Int_t QOCAFit::PmtGroup(Int_t pmtn)
{
  // jm -- 06.2006
  // PMTs are divided in two groups with different angular responses
  // Returns the group of this one
  // implemented for Batch3 reflectors selection
  //
  // 08.2006 - O.Simard
  // Added features with coordinate code and lo,hi limits.
  // Whatever the limits are, the pmts that are within the limits are
  // in the GROUP 1.
  // Flag must be on otherwise group1 is returned.

  // By default it is an invalid pmt
  Int_t group = 1;

  if(!GetAngResp2Vary()) return group; // group1 if not turned on 
  
  if((fPmtCoordCode <= 0) || (fPmtCoordCode > 4)) {
    Warning("PmtGroup","Invalide code %d, setting pmt coordinate to z",fPmtCoordCode);
    fPmtCoordCode = 3;
  }

  if(gPMTxyz->IsNormalPMT(pmtn)){

    if((fPmtCoordCode == 4) && (gPMTxyz->IsBatch3Concentrator(pmtn))) group = 2;
    else {
      
      Float_t pmtcoord; 
      if(fPmtCoordCode == 1) pmtcoord = gPMTxyz->GetX(pmtn); // selection based on x
      else if(fPmtCoordCode == 2) pmtcoord = gPMTxyz->GetY(pmtn); // selection based on y
      else if(fPmtCoordCode == 3) pmtcoord = gPMTxyz->GetZ(pmtn); // selection based on z
      else pmtcoord = gPMTxyz->GetZ(pmtn); // selection based on z

      // whatever is outside the range is in group 2
      if((pmtcoord < fPmtCoordCutLo) || (pmtcoord > fPmtCoordCutHi)) group = 2;
    }
  }
 
  return group;
}
//______________________________________________________________________________
void QOCAFit::PmtGroupCovar()
{
  // 08.2006 - O.Simard
  // Ensures there is no correlation between the pmt groups.

  if(!GetAngResp2Vary()) return;
  
  for(Int_t ipar = 0 ; ipar < fNangresp ; ipar++){
    for(Int_t jpar = 0 ; jpar < fNangresp ; jpar++){
      if(ipar == jpar) continue;
      fmrqcovar[Getangresppari()+ipar][Getangresp2pari()+jpar] = 0.;
      fmrqcovar[Getangresp2pari()+ipar][Getangresppari()+jpar] = 0.;
    }
  }
 
  return;
}
//______________________________________________________________________________
Float_t QOCAFit::CalcChisquare()
{
  // Calculate the total chi-squared value for all tubes using the current
  // set of parameters.

  if (!fDataSetup) {
    fprintf(stderr,"Must set up data before QOCAFit::CalcChisquare().\n");
    return 0.0;
  }
  
  if (fPrint>=2) {
    printf("Starting PMT is %d (0 is beginning of data).\n",fNStart);
    printf("Step is %d PMT's between evaluations (1 is all PMT's).\n",fNSkip);
  }

  Int_t i;
  Int_t ix,run,jpmt;
  Float_t modelpred;
  Float_t residual,sigma,chisquared;

  fChisquare = 0.0;
  for (i=fNStart+1; i<=fNpmts; i+= fNSkip) {
    ix = (Int_t) fmrqx[i];
   run = ix/10000;
    jpmt = ix%10000;

    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    fCurrentpmt = fCurrentrun->GetPMT(jpmt);

    modelpred = Model(run,jpmt);
    residual = GetOccVariable() - modelpred;
    sigma = sqrt(pow(GetOccVariableErr(),2) +
		 pow(ModelErr(run,jpmt,GetOccVariable()),2));
    //		 pow(ModelErr(run,jpmt,modelpred),2)); // Moffat - 6-Mar-2001
    chisquared = residual*residual/(sigma*sigma);

    fChisquare += chisquared;
  }

  return fChisquare;
}
//______________________________________________________________________________
void QOCAFit::FitNormalizations()
{
  // Preserve current state of variable parameter flags.
  // Then, allow only normalizations to vary and fit for normalizations only.
  // Restore previous state of variable parameter flags.

  Int_t i;
  fvarycopy = ivector(1,fNpars);
  for (i=1; i<=fNpars; i++) fvarycopy[i] = fmrqvary[i];  // Save current state

  SetAllVary(kFALSE);  // No variable parameters
  SetNormsVary(kTRUE); // Variable normalizations

  if (fPrint>=1) {
    printf("------------------------------------------------------------\n");
    printf("Fitting only normalizations.\n");
  }
  DoFit();

  for (i=1; i<=fNpars; i++) fmrqvary[i] = fvarycopy[i];  // Restore previous state
  free_ivector(fvarycopy,1,fNpars);

  return;
}
//______________________________________________________________________________
void QOCAFit::GetParameters(QOCAFit *seed)
{
  // Copy the parameter values and ranges from the seed QOCAFit structure to
  // this QOCAFit.

  Setd2o(seed->Getd2o());
  Setacrylic(seed->Getacrylic());
  Seth2o(seed->Geth2o());

  SetNlbmask(seed->GetNlbmask());
  Setlbmask(seed->Getlbmask());

  SetNangresp(seed->GetNangresp());
  Setangresp(seed->Getangresp());
  SetAngrespFuncType(seed->GetAngrespFuncType());

  SetNbinstheta(seed->GetNbinstheta());
  SetNdistwave(seed->GetNdistwave());
  SetNbinsphi(seed->GetNbinsphi());
  Setlbdist(seed->Getlbdist());
  Setlbdistwave(seed->Getlbdistwave());

  SetNumberofruns(seed->GetNumberofruns());
  Setnorm(seed->Getnorm());

  Setrsd2o(seed->Getrsd2o());
  Setrsacrylic(seed->Getrsacrylic());
  Setrsh2o(seed->Getrsh2o());

  return;
}
//______________________________________________________________________________
void QOCAFit::SetParameters(QOCAFit *target)
{
  // Copy the parameter values and ranges from this QOCAFit structure to the
  // target QOCAFit.

  target->Setd2o(Getd2o());
  target->Setacrylic(Getacrylic());
  target->Seth2o(Geth2o());

  target->SetNlbmask(GetNlbmask());
  target->Setlbmask(Getlbmask());

  target->SetNangresp(GetNangresp());
  target->Setangresp(Getangresp());
  target->SetAngrespFuncType(GetAngrespFuncType());

  target->SetNbinstheta(GetNbinstheta());
  target->SetNdistwave(GetNdistwave());
  target->SetNbinsphi(GetNbinsphi());
  target->Setlbdist(Getlbdist());
  target->Setlbdistwave(Getlbdistwave());

  target->SetNumberofruns(GetNumberofruns());
  target->Setnorm(Getnorm());

  target->Setrsd2o(Getrsd2o());
  target->Setrsacrylic(Getrsacrylic());
  target->Setrsh2o(Getrsh2o());

  return;
}
//______________________________________________________________________________
void QOCAFit::SetTimeWi(Int_t i)
{
  // Set the index for the time window to be used in the fit.
  //   <0   = use fOccratio (default time window, +/- 4ns as of 10-Nov-2000)
  //   0-11 = use fOccW[i] as the occupancy ratio.
  //
  // No checking for valid data is done here.  Assumption is that if the tube
  // is valid for the standard time window, it will certainly be valid for the
  // other windows.  tdiff currently uses the +/- 4ns window to find the centroid
  // and all other time windows are referenced to the same place.
  //
  // Applies to Version 3 of QOCAPmt.
  //
  // Changed indexing to <0 (default) and [0..11] for time windows.
  // 12-Mar-2001 - Bryce Moffat

  // timebin[] array copied from QPath::FillFromRDT() for use in printing out
  // information to the user/log file.
  Int_t timebin[7] = {1,2,3,4,7,10,20}; // should be read from what QPath contains!

  if (i<0) fTimeWi = -1;
  else if(i>=7) fTimeWi = 6;
  else fTimeWi = i;

  if (fPrint>=1) {
    if (fTimeWi<0) printf("Using default time window (4ns half-width).\n");
    else printf("Using time window index %d for %dns half-width.\n",
		fTimeWi,timebin[fTimeWi]);
  }
  return;
}
//______________________________________________________________________________
void QOCAFit::SetAllVary(Bool_t vary)
{
  // Set all model parameters to variable (or not).

  SetD2OVary(vary); 
  SetAcrylicVary(vary);
  SetH2OVary(vary);
  SetRayleighVary(!vary);
  SetMaskVary(vary);
  SetAngRespVary(vary);
  SetLBDistVary(vary); SetLBDistWaveVary(!vary);
  SetNormsVary(vary);

}
//______________________________________________________________________________
void QOCAFit::SetD2OVary(Bool_t vary)
{
  fmrqvary[Getd2opari()] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetAcrylicVary(Bool_t vary)
{
  fmrqvary[Getacrylicpari()] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetH2OVary(Bool_t vary)
{
  fmrqvary[Geth2opari()] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetMaskVary(Bool_t vary)
{
  Int_t i;
  for (i=1; i<fNlbmask; i++) fmrqvary[Getlbmaskpari()+i] = vary;
  // Ensure no duplicate normalizations from laserball mask normalization
  // Zero-order term in (1+cos(theta_lb)) is normalization
  fmrqvary[Getlbmaskpari()] = kFALSE;
}
//______________________________________________________________________________
void QOCAFit::SetAngRespVary(Bool_t vary)
{
  Int_t i;
  for (i=1; i<fNangresp; i++) fmrqvary[Getangresppari()+i] = vary;
  // Ensure no duplicate normalizations from PMT ang resp
  // First ang resp is normalization (normal incidence)
  fmrqvary[Getangresppari()] = kFALSE;
  // for occupancy fits, there are less data at normal incidence -- jm
  if(fOccType == 0) fmrqvary[Getangresppari()+1] = kFALSE;
}
//______________________________________________________________________________
void QOCAFit::SetAngResp2Vary(Bool_t vary)
{
  Int_t i;
  for (i=1; i<fNangresp; i++) fmrqvary[Getangresp2pari()+i] = vary;
  // Ensure no duplicate normalizations from PMT ang resp
  // First ang resp is normalization (normal incidence)
  fmrqvary[Getangresp2pari()] = kFALSE;
  // for occupancy fits, there are less data at normal incidence -- jm
  if(fOccType == 0) fmrqvary[Getangresp2pari()+1] = kFALSE;
}
//______________________________________________________________________________
void QOCAFit::SetLBDistVary(Bool_t vary)
{
  for (Int_t i=0; i<fNbinstheta*fNbinsphi; i++) fmrqvary[Getlbdistpari()+i] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetLBDistWaveVary(Bool_t vary)
{
  for (Int_t i=0; i<fNbinsthetawave*fNdistwave; i++) fmrqvary[Getlbdistwavepari()+i] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetNormsVary(Bool_t vary)
{
  Int_t i;
  for (i=0; i<fNumberofruns; i++) fmrqvary[Getnormpari()+i] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetRayleighD2OVary(Bool_t vary)
{
  fmrqvary[Getrsd2opari()] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetRayleighAcrylicVary(Bool_t vary)
{
  fmrqvary[Getrsacrylicpari()] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetRayleighH2OVary(Bool_t vary)
{
  fmrqvary[Getrsh2opari()] = vary;
}
//______________________________________________________________________________
void QOCAFit::SetRayleighVary(Bool_t vary)
{
  SetRayleighD2OVary(vary);
  SetRayleighAcrylicVary(vary);
  SetRayleighH2OVary(vary);
}
//______________________________________________________________________________
void QOCAFit::SetSingleNorm(Bool_t singlenorm)
{
  // For MC runs, the intensity is usually identical for all runs, and this allows
  // a single normalization to be fit.

  fSingleNorm = singlenorm;

  if (singlenorm) {
    printf("Using single normalization for all runs.\n");
    SetNormsVary(kFALSE);
    fmrqvary[Getnormpari()] = kTRUE;
  } else {
    printf("Using independent normalizations for each off-centre run.\n");
    SetNormsVary(kTRUE);
  }
}
// ***
// rsd
// ***
//______________________________________________________________________________
void QOCAFit::SetRunsToDo(Int_t n, Int_t runs[])
{
  // Allow user to select runs from qtree, rather than taking the first n.
  // Note it is the user's responsibility to put the runs in run[] in order,
  // so that there is no performance penality (if any) from jumping around
  // the qtree unnecessarily.  
  //
  // Fixes number of runs, corrected if some runs are not in the current qtree.
  //
  // Also fills an array of run indices.

  Int_t i,j,k;
  Int_t intree;
  Int_t *goodruns = new Int_t[n];

  if (fRunlist == NULL) Error("SetRunsToDo","fRunlist not set.");
  if (fNumberofrunsinfile == 0) Error("SetRunsToDo","fNumberofrunsinfile is zero.");
  if (n <= 0) Error("SetRunsToDo","Number of runs selected less than 1.");

  // Make sure selected runs are in tree.  Revise selection.
  k = 0;
  for (i=0; i<n; i++) {
    intree = 0;
    goodruns[k++] = runs[i];
    for (j=0; j<fNumberofrunsinfile; j++) if (runs[i] == fRunlist[j]) intree++;
    if (intree == 0) { // Not in tree.
      printf("Warning in QOCAFit::SetRunsToDo: ");
      printf("run %d selected but not in tree.\n",runs[i]);
      k--;
    } else if (intree > 1) { // In tree more than once.
      printf("Warning in QOCAFit::SetRunsToDo: ");
      printf("run %d occurs %d times in tree...why?\n",runs[i],intree);
    }
  }

  // Put revised selection in global variable
  fNumberofruns = k;
  if (fRunsToDo) delete[] fRunsToDo;
  fRunsToDo = new Int_t[fNumberofruns];
  for (i=0; i<fNumberofruns; i++) fRunsToDo[i] = goodruns[i];

  delete[] goodruns;

  // Set the indices of these runs
  SetRunsToDoIndex();
  return;
}    
//______________________________________________________________________________
void QOCAFit::SetRunsToDoIndex()
{
  // Fills an array of indices mapping fRunsToDo to fRuns (via fRunlist).
  
  Int_t i,j;
  
  if (fRunlist == NULL) Error("SetRunsToDoIndex","fRunlist not set.");
  if (fNumberofrunsinfile == 0) Error("SetRunsToDoIndex","fNumberofrunsinfile is zero.");
  if (fRunsToDo == NULL) Error("SetRunsToDoIndex","fRunsToDo not set.");
  if (fNumberofruns == 0) Error("SetRunsToDoIndex","fNumberofruns is zero.");

  if (fRunsToDoIndex) delete[] fRunsToDoIndex;
  fRunsToDoIndex = new Int_t[fNumberofruns];
  for (i=0; i<fNumberofruns; i++) fRunsToDoIndex[i] = -1;

  for (i=0; i<fNumberofruns; i++) {
    for (j=0; j<fNumberofrunsinfile; j++) {
      if ( (fRunsToDo[i] == fRunlist[j]) && (fRunsToDoIndex[i] == -1) ) {
        fRunsToDoIndex[i] = j;
      }
    }
  }
  return;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetVary()
{
  // Returns kTRUE if any parameters are variable.

  Bool_t retval = kFALSE;
  Int_t i;
  for (i=1; i<=fNpars; i++)
    if (fmrqvary[i]) { retval = kTRUE; break; }

  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetMaskVary()
{
  // Returns kTRUE if any of the laserball mask parameters are variable.

  Bool_t retval = kFALSE;
  Int_t i;
  for (i=0; i<fNlbmask; i++)
    if (fmrqvary[Getlbmaskpari()+i]) { retval = kTRUE; break; }

  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetAngRespVary()
{
  // Returns kTRUE if any of the PMT angular response parametes are variable.

  Bool_t retval = kFALSE;
  Int_t i;
  for (i=1; i<fNangresp; i++)
    if (fmrqvary[Getangresppari()+i]) { retval = kTRUE; break; }
  
  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetAngResp2Vary()
{
  // Returns kTRUE if any of the group2 PMT angular response parametes are variable.

  Bool_t retval = kFALSE;
  Int_t i;
  for (i=1; i<fNangresp; i++)
    if (fmrqvary[Getangresp2pari()+i]) { retval = kTRUE; break; }
  
  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetLBDistVary()
{
  // Returns kTRUE if any of the laserball distribution parameters are variable.
  
  Bool_t retval = kFALSE;
  Int_t i;
  for (i=0; i<fNbinstheta*fNbinsphi; i++)
    if (fmrqvary[Getlbdistpari()+i]) { retval = kTRUE; break; }
  
  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetLBDistWaveVary()
{
  Bool_t retval = kFALSE;
  Int_t i;
  for (i=0; i<fNbinsthetawave*fNdistwave; i++)
    if (fmrqvary[Getlbdistwavepari()+i]) { retval = kTRUE; break; }

  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetNormsVary()
{
  // Returns kTRUE if any of the normalizations are variable.
  // For single normalization versus multiple normalizations, check GetSingleNorm().

  Bool_t retval = kFALSE;
  Int_t i;
  for (i=0; i<fNumberofruns; i++)
    if (fmrqvary[Getnormpari()+i]) { retval = kTRUE; break; }

  return retval;
}
//______________________________________________________________________________
Bool_t QOCAFit::GetRayleighVary()
{
  // Returns kTRUE if any of the Rayleigh scattering length are variable.

  Bool_t retval = kFALSE;
  Int_t i;
  for (i=0; i<3; i++)
    if (fmrqvary[Getrsd2opari()+i]) { retval = kTRUE; break; }

  return retval;
}
//______________________________________________________________________________
Int_t QOCAFit::GetNparsVariable()
{
  // Count the number of variable parameters as given by fmrqvary[] = 1

  Int_t i;
  Int_t num = 0;
  for (i=1; i<=fNpars; i++) if (fmrqvary[i] != 0) num++;
  return num;
}
//______________________________________________________________________________
Int_t QOCAFit::GetNparsFixed()
{
  // Count the number of fixed parameters as given by fmrqvary[] = 0

  Int_t i;
  Int_t num = 0;
  for (i=1; i<=fNpars; i++) if (fmrqvary[i] == 0) num++;
  return num;
}
// ***
// rsd
// ***
//______________________________________________________________________________
Int_t QOCAFit::GetRunsToDoIndex(Int_t i)
{
  // Return index to run in QOCARun array (fRuns), given index of run to do 
  // (fRunsToDo).

  return GetRunsToDoIndex()[i];

}
//______________________________________________________________________________
Int_t* QOCAFit::GetRunsToDoIndex()
{
  // Return pointer to fRunsToDoIndex.
  return fRunsToDoIndex;
}

//______________________________________________________________________________
QOCATree *QOCAFit::Apply()
{
  // Apply the current model to the current data tree and return a new tree with
  // the model applied.  This means that occratio can be "fully corrected" when
  // plotting...
  //
  // Chi-squareds, residuals and sigmas for each QOCAPmt are calculated and
  // stored in the tree.
  //
  // Moffat - 6-Mar-2001
  // Set status bits for PMT's which are cut from the QOCATree for the fit.
  // The meaning of the bits is: see DataSetup()
  //
  // For compatibility in Apply() the following bit is set if the PMT was _not_
  // used in the fit:
  //   bit 8 (1<<8 = 256): Not present in fmrqx array
  // This allows old QOCAFit results to be Apply()'ed and still flag PMT's which
  // weren't in the fit.
  //
  // Moffat - 19-Mar-2001
  // Add the possibility to use different time windows to Apply().
  // This means, getting the occratio from the time windows, not from
  // QOCAPmt::GetOccratio() directly.

  // Make sure central runs are loaded for occratio; if necessary, try to load
  // them from this tree itself.
  if (!fTree || !fCtree || !fCentrerunptr) {
    fprintf(stderr,"Must set up data tree and central run tree before "
	    "QOCAFit::Apply().\n");
    return NULL;
  }
  
  QOCATree *applytree = new QOCATree("optica","Model-applied QOCATree");
  QOCARun *applyrun = applytree->GetQOCARun();
  QOCAPmt *applypmt = new QOCAPmt();

  Int_t i;

  //--------------------
  // Put the laserball distribution parameters into the histogram fLaserdist
  ApplyLaserdist();

  //--------------------
  Int_t ix;
  Int_t pmtn, run;
  Float_t modelpred, modelprederr, lbmask, laserlight, pmtang, solidangle;
  Float_t chisquared, residual, sigma;
  Float_t occupancy, occupancyerr;
  Float_t occratio, occratioerr;
  Float_t occvar, occvarerr;
  Float_t woccratio, wsigma;
  Float_t woccupancy, woccupancyerr;
  Float_t wnprompt;
  Float_t npulses;

  //--------------------
  // Set the bits for inclusion or exclusion from the fit
  // Loop over all PMT's in the locally stored trees: set status bit to cut (1)
  // Loop over all PMT's in the fmrqx[] array, and set those PMT's to included (0)
  for (run=0; run<fNumberofruns; run++) {
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    for (pmtn=0; pmtn<fCurrentrun->GetNpmt(); pmtn++) {
      fCurrentpmt = fCurrentrun->GetPMT(pmtn);
      // Set all PMT's status bit as not included in the QOCAFit:
      fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() | (1<<8) );
    }
  }

  for (i=1; i<=fNpmts; i++) {
    ix = (Int_t) fmrqx[i];
    run = ix/10000;
    pmtn = ix%10000;
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    fCurrentpmt = fCurrentrun->GetPMT(pmtn);
    // PMT was in the fit - unset status bit 8:
    fCurrentpmt->SetFitstatus( fCurrentpmt->GetFitstatus() & ~(1<<8) );
  }

  //--------------------
  if (fPrint>=1)
    printf("Step is %d PMT's between evaluations (1 is all PMT's).\n",fNSkip);

  for (run=0; run<fNumberofruns; run++) {
    fCurrentrun = fRuns[GetRunsToDoIndex(run)];
    *applyrun = *fCurrentrun; // Copy the main data entries, not the PMTs' array

    if (fPrint>=2) {
      printf("----------------------------------------\n");
      printf("Applying the model to run %d (run %d out of %d).\n",
	     fCurrentrun->GetRun(),run+1,fNumberofruns);
    }

    npulses = fCurrentrun->GetNpulses();  // For reconstruction windowed occupancy

    // Run normalization
    // 09.2005 - O.Simard
    // Be careful here: Getnorm()[run] and qocarun->GetNorm() are different!
    // applyrun->SetNorm(Getnorm()[run]);
    if(Getnorm()[run] > 0) applyrun->SetNorm(1./Getnorm()[run]); // for consistency with QOCATree
    //applyrun->SetNorm(1./(Getnorm()[run] * npulses)); // for consistency with QOCATree

    for (pmtn=fNStart; pmtn < fCurrentrun->GetNpmt(); pmtn+=fNSkip) {
      fCurrentpmt = fRuns[GetRunsToDoIndex(run)]->GetPMT(pmtn);
      *applypmt = *fCurrentpmt;

      // Additional geometry and intensity corrections to Occratio.
      // The fully corrected version is then:
      //    1 = Occratio' = fOccupancy * fGeomratio / modelpred
      // while:
      //        Occratio  = fOccupancy * fGeomratio
      // only.
      //
      // 11-Nov-2000 - Bryce Moffat
      // Change to make model be the occupancy, not the occupancy ratio.  For
      // individual pmt efficiencies
      // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      // 25-04-2004 - Jose' Maneira
      // Keeping with Bryce's method, use occupancy no matter what variable was used 
      // in the fit

      // save current occupancy variable
      Int_t currentocctype = fOccType;
      
      if(fOccType == 1){ // occratio
	// ====================================
	// Occratio method: revert to Occupancy
	// ====================================
	SetOccType(0); // revert to occupancy 
	modelpred = Model(run,pmtn);   // Sets fCurrentpmt as a side effect
	applypmt->SetModel(modelpred); // scaled by QOCAPmt:fPmteffc (input efficiency)
      
	// efficiency is occupancy * input efficiency / model prediction
	// this effectively removes the input efficiency, as wanted in the 
	// occratio method.
	applypmt->SetPmteffm(
			     (applypmt->GetOccupancy() * (applypmt->GetPmteffc())) /
			     (applypmt->GetModel())
			    );
      
	modelprederr = ModelErr(run,pmtn,modelpred); // force model occupancy
	applypmt->SetModelerr(modelprederr);  // Is this ok for occupancy?
      } else {
	// ===============================================
	// Occupancy method: keep input efficiency
	// ===============================================
	modelpred = Model(run,pmtn);   // Sets fCurrentpmt as a side effect
	applypmt->SetModel(modelpred); // scaled by QOCAPmt:fPmteffc (input efficiency)

	// efficiency is occupancy / model prediction
	applypmt->SetPmteffm(
			     applypmt->GetOccupancy() / applypmt->GetModel()
			    );
      
	modelprederr = ModelErr(run,pmtn,modelpred); // force model occupancy
	applypmt->SetModelerr(modelprederr);  // Is this ok for occupancy?
      }
      
      SetOccType(currentocctype); // Set occ variable back to previous value
      // reset the model based on the occupancy variable
      modelpred = Model(run,pmtn);  // Sets fCurrentpmt as a side effect
      modelprederr = ModelErr(run,pmtn);
      // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

      // explore time windows if applicable
      if (fTimeWi==-1) {
	occvar = GetOccVariable();
	occvarerr = GetOccVariableErr();
      } else {
	woccratio = fCurrentpmt->GetOccratioW(fTimeWi);
	woccupancy = fCurrentpmt->GetOccupancyW(fTimeWi);
	
	wnprompt = npulses * ( 1 - exp(-woccupancy/npulses) );
	
	woccupancyerr = sqrt(wnprompt) / exp(-woccupancy/npulses);
	
	occratio = fCurrentpmt->GetOccratio();
	occupancy = fCurrentpmt->GetOccupancy();
	occupancyerr = fCurrentpmt->GetOccupancyerr();
	
	wsigma = sqrt(pow(fCurrentpmt->GetOccratioerr(),2) -
		      pow(occratio * occupancyerr/occupancy,2) +
		      pow(woccratio * woccupancyerr/woccupancy,2) +
		      pow(ModelErr(run,pmtn,woccratio),2));
	
	if(fOccType){
	  occvar = woccratio;
	  occvarerr = wsigma;
        } else {
	  occvar = woccupancy;
	  occvarerr = sqrt(pow(woccupancyerr,2) + pow(ModelErr(run,pmtn,woccupancy),2));
        }
      }

      // Calculate chisquare, residual, and sigma
      residual = occvar - modelpred;
      sigma = sqrt(pow(occvarerr,2) + pow(modelprederr,2));
      chisquared = residual*residual/(sigma*sigma);

      applypmt->SetChisq(chisquared);
      applypmt->SetResidual(residual);
      applypmt->SetSigma(sigma);

      // 10.2006 - O.Simard
      // There was weird settings for occupancy and occratio for "ease of plotting".
      // Those were removed to avoid confusion: the definition of a variable
      // should never change in QOCATree.
      // This means: occupancy = occupancy, and occratio = occratio.
      
      // The following model quantities are for the particular PMT and run in
      // question. So, these are corrections to occupancy.
      //
      // Laserball mask:
      lbmask = ModelLBMask(fCurrentpmt);  // fCurrentpmt set by Model() above --^
      //applypmt->SetLbmask(lbmask);
      
      // Laserball asymmetry:
      laserlight = ModelLBDist(fCurrentrun,fCurrentpmt);
      //applypmt->SetLaserlight(laserlight);
     
      // PMT angular response:
      pmtang = ModelAngResp(fCurrentpmt);
      //applypmt->SetPmtang(pmtang);
      
      // Solid angle of PMT:
      solidangle = applypmt->GetSolidangle();

      // 08.2006 - O.Simard
      // Based on the comments above, remove from occupancy and call that
      // the correction (new/old). To remove these effects just divide the
      // occupancy: occupancy /= applypmt->GetOccCorrection().
      applypmt->SetOccCorrection(lbmask*laserlight*pmtang*solidangle);
      
      applyrun->AddPMT(applypmt);
    }

    applytree->Fill();
  }

  delete applypmt;

  return applytree;
}
//______________________________________________________________________________
void QOCAFit::ApplyRun(QOCARun *ocarun)
{
  // Use the model to fill in model values for the ocarun.
  // The normalization comes from the ocarun.
  // The run pointed to by ocarun will be modified directly, rather than
  // returning a separate QOCARun pointer.
  // 
  // The ocarun should already contain all the distances to each PMT and the
  // desired orientation of the laserball distribution in balltheta and ballphi.
  //
  // This routine is intended to obtain the SNO response to electrons and gamma
  // rays independently from SNOMAN / RSP.
  //
  // It is an adaptation from Model() and Apply() in QOCAFit which calculates the
  // light as an MPE-corrected occupancy.  The MPE correction should be undone in
  // order to get the effective number of hits.  Also, the occupancy is calculated
  // without reference to a central run, so distances are used directly rather
  // than using distance differences as for occratio.
  //
  // 24-Jan-2001 - Moffat
  // Third Euler angle needed to fully specify the direction of the laserball
  // axis and its orientation.  In the interim, with only two angles, divide the
  // cases into two; see ModelLBDist() for more details.

  Int_t pmtn;
  Float_t norm;
  Float_t laserlight, lbmask, npulses, intensity;
  Float_t dd2o, dacrylic, dh2o;
  Float_t angresp;
  Float_t pmtresponse;

  fCurrentrun = ocarun;

  for (pmtn=0; pmtn<fCurrentrun->GetNpmt(); pmtn++) {
    fCurrentpmt = ocarun->GetPMT(pmtn);

    norm = fCurrentrun->GetNorm();
    laserlight = ModelLBDist(fCurrentrun, fCurrentpmt);
    lbmask = ModelLBMask(fCurrentpmt);
    npulses = fCurrentrun->GetNpulses();
    intensity = npulses * laserlight * lbmask;  // # of photons in PMT over run

    dd2o = fCurrentpmt->GetDd2o();  // d2o distance
    dacrylic = fCurrentpmt->GetDacr(); // acrylic distance
    dh2o = fCurrentpmt->GetDh2o();  // h2o distance

    angresp = ModelAngResp(fCurrentpmt);

    pmtresponse = norm * intensity * angresp *
      exp( - dd2o*Getd2o() - dh2o*Geth2o() - dacrylic*Getacrylic());
	  
    if (fOccType == 0) pmtresponse *= fCurrentpmt->GetPmteffc();

    fCurrentpmt->SetModel(pmtresponse);
    //fCurrentpmt->SetLbmask(lbmask);
    //fCurrentpmt->SetLaserlight(laserlight);
    //fCurrentpmt->SetPmtang(angresp);
  }
  return;
}
//______________________________________________________________________________
TH2F *QOCAFit::ApplyLaserdist()
{
  // Put the current fit parameters into a histogram in cos(theta_lb) vs phi_lb,
  // stored in fLaserdist and returned as a pointer.
  
  if(fLaserdist) delete fLaserdist;
  
  Int_t itheta,j;

  if(!fLBdistType){
    fLaserdist = new TH2F("fLaserdist","Laserball Distribution (Histogram)",
			  fNbinsphi,0,2.0*M_PI,fNbinstheta,-1,1);
    for (itheta=0; itheta<fNbinstheta; itheta++)
      for (j=0; j<fNbinsphi; j++)
	fLaserdist->SetCellContent(j+1,itheta+1,Getlbdist()[itheta*fNbinsphi+j]);
  } else {
    fLaserdist = new TH2F("fLaserdist","Laserball Distribution (Sinusoidal)",
			  fNbinsphi,0,2.0*M_PI,fNbinsthetawave,-1,1);
    for(itheta=0; itheta<fNbinsthetawave; itheta++){
      TF1* f = LBDistWaveTF1(itheta);
      for(j=0; j<fNbinsphi; j++){
	Float_t phi = 2*j*M_PI/fNbinsphi;
	// bins starts with cos(theta) = -1 (itheta = 0) in TH2F
	// with index itheta in LBDistWaveTF1(itheta);
	fLaserdist->SetCellContent(j+1,itheta+1,f->Eval(phi));
      }
      delete f;
    }
  }
  return fLaserdist;
}
//______________________________________________________________________________
void QOCAFit::FillLBDist(TH2F *laserdist)
{
  // Fill the fit parameters corresponding to the laserball distribution from
  // the 2D histogram laserdist.
  //
  // The x-axis is phi_lb on [0:2pi] and the y-axis is cos(theta_lb) on [-1:1]

  Int_t i,j;
  Int_t pari;

  Float_t costheta,phi;

  Float_t *lbdist = (Float_t *) Getlbdist();

  TAxis *phiaxis = laserdist->GetXaxis();
  TAxis *cthaxis = laserdist->GetYaxis();
  Int_t phibin,cthbin;

  for (i=0; i<fNbinstheta; i++) {
    costheta = 2.0 * (i+0.5)/fNbinstheta - 1.0;  // Find cos(theta) of bin centre
    cthbin = cthaxis->FindBin(costheta);

    for (j=0; j<fNbinsphi; j++) {
      phi = 2*M_PI * (j+0.5)/fNbinsphi;  // Find phi of bin centre
      phibin = phiaxis->FindBin(phi);

      pari = i*fNbinsphi + j;

      lbdist[pari] = (Float_t) laserdist->GetCellContent(phibin,cthbin);
    }
  }
  return;
}
//______________________________________________________________________________
TH1F *QOCAFit::AngRespTH1F()
{
  // Returns a pointer to a histogram which contains the binned angular response
  // and error bars (from the diagonal matrix elements via Getangresperr()).

  Int_t i;

  Float_t xhalfbin = 0.5 * (90.0 / fNangresp);
  TH1F *h;
  Char_t htitle[256];
  sprintf(htitle,"PMT Angular Response (Type %d)",fAngrespFuncType);

  if (fAngrespFuncType == 1) {  // type 1: use bin "edge"
    h = new TH1F("AngRespH",htitle,fNangresp,0,90);
  } else {  // type 0 or 2: use bin centres
    h  = new TH1F("AngRespH",htitle,fNangresp,-xhalfbin,89+xhalfbin);
  }

  for (i=0; i<fNangresp; i++) {
    h->SetBinContent(i+1,Getangresp(i));
    h->SetBinError(i+1,Getangresperr(i));
  }

  return h;
}
//______________________________________________________________________________
TH1F *QOCAFit::AngResp2TH1F()
{
  // Returns a pointer to a histogram which contains the binned angular response
  // and error bars (from the diagonal matrix elements via Getangresperr()).

  Int_t i;

  Float_t xhalfbin = 0.5 * (90.0 / fNangresp);
  TH1F *h;
  Char_t htitle[256];
  sprintf(htitle,"Group2 PMT Angular Response (Type %d)",fAngrespFuncType);

  if (fAngrespFuncType == 1) {  // type 1: use bin "edge"
    h = new TH1F("AngResp2H",htitle,fNangresp,0,90);
  } else {  // type 0 or 2: use bin centres
    h = new TH1F("AngResp2H",htitle,fNangresp,-xhalfbin,89+xhalfbin);
  }

  for (i=0; i<fNangresp; i++) {
    h->SetBinContent(i+1,Getangresp2(i));
    h->SetBinError(i+1,Getangresp2err(i));
  }

  return h;
}
//______________________________________________________________________________
TF1 *QOCAFit::AngRespTF1()
{
  // Returns a pointer to a function with parameters equal to angular response
  // parameters from QOCAFit, to plot the current PMT angular response function.

  Int_t i;
  Double_t *par = new Double_t[1+fNangresp+1];
  Float_t *angresp = Getangresp();
  par[0] = fNangresp;
  for (i=0; i<fNangresp; i++) par[1+i] = angresp[i];
  par[fNangresp+1] = fAngrespFuncType;

  TF1 *f = new TF1("AngResp",sPMTResp,0,90,1+fNangresp+1);
  f->SetParameters(par);
  f->SetNpx(fNangresp);
  f->SetMarkerStyle(20);

  delete[] par;

  return f;
}
//______________________________________________________________________________
TF1 *QOCAFit::AngResp2TF1()
{
  // Returns a pointer to a function with parameters equal to angular response
  // parameters from QOCAFit, to plot the current PMT angular response function.

  Int_t i;
  Double_t *par = new Double_t[1+fNangresp+1];
  Float_t *angresp = Getangresp2();
  par[0] = fNangresp;
  for (i=0; i<fNangresp; i++) par[1+i] = angresp[i];
  par[fNangresp+1] = fAngrespFuncType;

  TF1 *f = new TF1("AngResp2",sPMTResp,0,90,1+fNangresp+1);
  f->SetParameters(par);
  f->SetNpx(fNangresp);
  f->SetMarkerStyle(20);

  delete[] par;

  return f;
}
//______________________________________________________________________________
Double_t QOCAFit::sPMTResp(Double_t *a,Double_t *par)
{
  // Utility function which returns the PMT angular response corresponding to 
  // angle a[0] in degrees, bounded by 0 and 90 degrees.
  //
  // par[0] specifies number of parameters; par[1] through par[par[0]] are
  // the parameters themselves.  Can't use fNangresp inside static class function!

  // see QOCAFit::ModelAngResp() when modifying...

  Float_t theta = a[0];
  if (theta<0.0 || theta>=90.0) return 0.0;  // out of range if not in [0:90]
  
  Int_t npars = (Int_t) par[0];
  Int_t pari = (Int_t) (theta*npars/90.0);  // bins from 0 to 90 degrees

  Int_t angrespfunctype = (Int_t) par[npars+1];

  if (angrespfunctype == 1) return par[pari+1];  // binned function?
  if (angrespfunctype == 2) {
    // 17-Jan-2001 - Moffat/Hallin - use bin centres, not left edges of bins
    // by subtracting half a bin width from theta before doing the conversion to
    // parameter number.
    // This is reasonable for binned fits to the data 0-0.5, 0.5-1.5, 1.5-2.5 binning
    // For SNOMAN, the simulations use binned data following 0-1, 1-2, 2-3 binning
    if (theta>=(90.0-0.5*90.0/npars)) return 0.0; // Last half bin 89.5-90.0 deg = 0.0
//    pari = (Int_t) ((theta + 0.5*90.0/npars) * (npars/90.0));
    pari = (Int_t) ((theta - 0.5*90.0/npars) * (npars/90.0));
    return par[pari+1];  // bin centres
  }

  // Otherwise, do linear interpolation (fAngrespFuncType == 0):
  Float_t theta1 = pari       * 90.0/npars;
//  Float_t theta2 = (pari + 1) * 90.0/npars; // variable not used -- rsd 2002.06.03

  if (pari<0) pari = 0;
  else if (pari >= npars) pari = npars-1;

  // Don't interpolate over the last bin with data: this just produces a
  // spurious slope, which was never used in the fit anyway.  The logic here
  // could be improved...
  // 13-Mar-2001 - Bryce Moffat
//  if (par[pari+2] == 1.0) return 1.0;
  if (angrespfunctype == 0) if (par[pari+1] == 1.0) return 1.0;

  // Extra offset of +1 in par[pari+1,2] is because par[0] is npars and array
  // really starts at index of 1...
  Float_t slope = (par[pari+2] - par[pari+1]) / (90.0 / npars);
  if ( (pari+2) > 90 ) slope = (0 - par[pari+1]) / (90.0 / npars);
  Float_t deltheta = theta - theta1;

  return par[pari+1] + slope * deltheta;
}
//______________________________________________________________________________
TF1 *QOCAFit::LBMaskTF1()
{
  // Returns a pointer to a function with no parameters, to plot the current
  // laserball mask function polynomial in (1+cos(theta_lb)).
  //
  // To change the parameters, use QOCAFit::Setlbmask(Float_t *lbm) and
  // QOCAFit::GetNlbmask() for the number of parameters.

  Int_t i;
  Double_t *par = new Double_t[1+fNlbmask];
  par[0] = fNlbmask;
  Double_t *err = new Double_t[1+fNlbmask];
  err[0] = 0.0;
  for (i=0; i<fNlbmask; i++) {par[1+i] = Getlbmask()[i]; err[1+i] = Getlbmaskerr(i);}

  TF1 *f = new TF1("LB Mask",sLBMask,-1,1,1+fNlbmask);
  f->SetParameters(par);
  f->SetParErrors(err);
  f->SetNpx(100);
  f->SetMarkerStyle(20);
  f->GetXaxis()->SetTitle("Polar Angle cos#theta_{LB}");

  delete[] par; delete[] err;

  return f;
}
//______________________________________________________________________________
Double_t QOCAFit::sLBMask(Double_t *a,Double_t *par)
{
  // Utility function which returns the laserball mask function corresponding to
  // a[0] = cos(theta_lb)
  //
  // par[0] specifies number of parameters; par[1] through par[par[0]] are
  // the parameters themselves.  Can't use fNlbmask inside static class function!

  // see QOCAFit::ModelLBDist() when modifying...

  Int_t npars = (Int_t) par[0];

  Int_t i;
  Float_t lbm = 0;
  Float_t oneplus = 1.0 + a[0];

  for (i=npars-1; i>=0; i--)
    lbm = lbm*oneplus + par[1+i];
  
  return lbm;
}
//______________________________________________________________________________
Double_t QOCAFit::dLBMask(Double_t *a,Double_t *par)
{
  // Partial derivatives for the laserball mask polynomial corresponding to
  // a[0] = cos(theta_lb) with respect to the 
  // parameter identified by the index in par[0].
  
  // see QOCAFit::ModelLBDist() when modifying...

  Int_t ipar = (Int_t) par[0];
  Double_t costheta = a[0];
  Double_t oneplus = 1.0 + costheta;
  
  Double_t dlbm = TMath::Power(oneplus,ipar); // power ipar;

  return dlbm;
}
//______________________________________________________________________________
TF1 *QOCAFit::LBDistWaveTF1(Int_t itheta)
{
  // Returns a pointer to a function with no parameters, to plot the current
  // laserball distribution sinusoidal function in this slice of
  // cos(theta_lb).

  if(itheta < 0 || itheta >= fNbinsthetawave) {
    Error("LBDistWaveTF1","The theta slice %d does not exist.",itheta);
  }

  Int_t i;
  Double_t *par = new Double_t[1+fNdistwave];
  par[0] = fNdistwave;
  Double_t *err = new Double_t[1+fNdistwave];
  err[0] = 0.0;
  for (i=0; i<fNdistwave; i++) {
    par[1+i] = Getlbdistwave()[itheta*fNdistwave+i];
    err[1+i] = Getlbdisterr(itheta*fNdistwave+i);
  }

  TF1 *f = new TF1("LB Dist",sLBDistWave,0,2*M_PI,1+fNdistwave);
  f->SetParameters(par);
  f->SetParErrors(err);
  f->SetNpx(10*fNbinsphi);
  f->SetMarkerStyle(20);
  f->GetXaxis()->SetTitle("Azimuthal Angle #phi (radians)");

  delete[] par; delete[] err;

  return f;
}
//______________________________________________________________________________
Double_t QOCAFit::sLBDistWave(Double_t *a,Double_t *par)
{
  // Utility function which returns the laserball distribution sinusoidal function 
  // corresponding to a[0] = phi.
  //
  // par[0] specifies number of parameters; par[1] through par[par[0]] are
  // the parameters themselves.  Can't use fNdistwave inside static class function!

  // see QOCAFit::ModelLBDist() when modifying...

  Double_t phi          = a[0];
  Double_t amplitude_ac = par[1];
  Double_t frequency    = 1.;
  Double_t phase        = par[2];
  Double_t amplitude_dc = 1.;
  Double_t lbd = amplitude_ac*sin(frequency*phi + phase) + amplitude_dc;

  return lbd;
}
//______________________________________________________________________________
Double_t QOCAFit::dLBDistWave(Double_t *a,Double_t *par)
{
  // Utility function which returns the partial derivative of the 
  // laserball distribution sinusoidal function with respect to the 
  // parameter identified by the index in par[0], and evaluated at
  // phi = a[0].

  // see QOCAFit::ModelLBDist() when modifying...

  Int_t ipar = (Int_t) par[0];
  Double_t phi          = a[0];
  Double_t amplitude_ac = par[1];
  Double_t frequency    = 1.;
  Double_t phase        = par[2];
  Double_t dlbd;

  // actual derivatives
  if(ipar == 0) dlbd = sin(frequency*phi + phase);
  else if(ipar == 1) dlbd = amplitude_ac*cos(frequency*phi + phase);
  else dlbd = 0.0;
  
  return dlbd;
}
//______________________________________________________________________________
Float_t QOCAFit::Model(Int_t irun, Int_t jpmt, Int_t na,Float_t *dyda)
{
  // Returns the model-predicted "intensity" (occupancy ratio) for a pmt for a
  // given run.  The parameters are:
  //              d2o, acrylic, h2o attenuation lengths,
  //              PMT angular response,
  //              laserball mask,
  //              laserball distribution,
  //              run normalizations,
  //              d2o, acrylic, h2o Rayleigh scattering lengths.
  //
  // Calculate the derivatives for all parameters in the model for a pmt in a
  // given run, provided na>0 and dyda isn't a NULL pointer.  Default parameters
  // allow the model value to be evaluated without calculating the derivatives
  // too.
  //
  // Boolean flag occtype=1 for occupancy ratio with a normalizing central run,
  // or occtype=0 for occupancy.
  // 11-Nov-2000 - Bryce Moffat
  // 
  // 25-April 2004 - Model now supplies the predicted Occupancy or OccRatio,
  // 				 according to the value of fOccType
  //				J. Maneira

  Int_t i;
  Int_t arpari;

  //--------------------
  // Calculate derivatives, if appropriate, for:
  //   attenuation lengths (i=1,2,3),
  //   angular response (i=4,...,4+fNangresp-1),
  //   laserball mask (etc.),
  //   laserball distribution, and
  //   normalizations.
  //
  // Extra logic is to keep track of which entries are changed; this is so sparse
  // that only those which were previously changed should be re-zeroed.
  Bool_t derivatives = kFALSE;
  if (na>0 && dyda) {
    derivatives = kTRUE;
    // Loop takes too long - only re-zero certain parameters
    //for (i=1; i<=na; i++) dyda[i] = 0;
    // All other parameters are re-calculated anyway - so no need to zero!
    
    dyda[Getnormpari()+finorm] = 0;
    dyda[Getangresppari()+fiang   ] = 0;
    dyda[Getangresppari()+fciang  ] = 0;
    dyda[Getangresp2pari()+fiang   ] = 0;
    dyda[Getangresp2pari()+fciang  ] = 0;
    if(fAngrespFuncType == 0){ // interpolation
      dyda[Getangresppari()+fiang+1 ] = 0;
      dyda[Getangresppari()+fciang+1] = 0;
      dyda[Getangresp2pari()+fiang+1 ] = 0;
      dyda[Getangresp2pari()+fciang+1] = 0;
    }
    dyda[Getlbmaskpari()+filbdist]  = 0;
    dyda[Getlbmaskpari()+fcilbdist] = 0;
    if(!fLBdistType){
      dyda[Getlbdistpari()+filbdist ] = 0;
      dyda[Getlbdistpari()+fcilbdist] = 0;
    } else {
      for(i=0; i<fNdistwave; i++){
	dyda[Getlbdistwavepari()+filbdist*fNdistwave+i]  = 0;
	dyda[Getlbdistwavepari()+fcilbdist*fNdistwave+i] = 0;
      }
    }
  } else derivatives = kFALSE;

  //--------------------
  // Set up run and central-run pointers:
  fCurrentrun = fRuns[GetRunsToDoIndex(irun)];
  fCurrentpmt = fCurrentrun->GetPMT(jpmt);
  Int_t icpmt = fCurrentpmt->GetCrunPmtIndex();

  if (PmtGroup(fCurrentpmt) == 2 ) arpari = Getangresp2pari();
  else arpari = Getangresppari();

  fCurrentctrrun = fCentrerunptr[fCentrerunindex[GetRunsToDoIndex(irun)]];
  fCurrentctrpmt = fCurrentctrrun->GetPMT(icpmt);

  if ( !fCurrentpmt  || !fCurrentrun ) return 0;
  if ( !fCurrentctrpmt  || !fCurrentctrrun ) return 0;

  //--------------------
  // Off-centre run:
  if (fSingleNorm) finorm = 0;  // 24-Nov-2000 - Moffat
  else finorm = irun;
  Float_t norm = Getnorm()[finorm];  // Off-centre run normalization

  //--------------------
  // For off-centre run's pmt:
  //
  // ALH note - 30-June-2000:
  //
  // To calculate the derivatives correctly, we need to be sure that the model
  // used is consistent.  So for instance, the derivatives we calculate below
  // are in terms of attenuations, not attenuation lengths.
  //
  // It is also not really correct to have both a LBDist(theta, phi) and
  // a LBMask(theta) and fit them independently.  We should make sure that
  // if LBDist is being fit for, LBMask is fixed.
  //
  // Finally, we cannot have multiple overall normalizations - so the PMT
  // angular response, laserball angular response, mask function, should all
  // have a defined normalization (say 1 in a particular bin) before the fit.  

  filbdist = 0; // stores itheta

  fiang = 0;  // Index of parameter used by model
  Float_t interpolfrac = 0;  // 22-Nov-2000 - Moffat - Interpol. frac. btw bins
  Float_t angresp    = ModelAngResp(fCurrentpmt,fiang,interpolfrac);
  Float_t laserlight = ModelLBDist(fCurrentrun, fCurrentpmt,filbdist);
  Float_t lbmask     = ModelLBMask(fCurrentpmt);
  Float_t npulses    = fCurrentrun->GetNpulses();
  //Float_t intensity  = npulses * laserlight * lbmask; // # photons in PMT over run
  Float_t intensity  = laserlight * lbmask; // relative intensity
  Float_t dd2o, dacrylic, dh2o, efficiency, solidangle, transpwr;

  // Assign variables differently for both occupancies/ratios.
  // -----------------
  if (fOccType == 0) { // Occupancy method
    
    dd2o       = fCurrentpmt->GetDd2o();       // d2o distance from src to pmt
    dacrylic   = fCurrentpmt->GetDacr();       // acrylic distance
    dh2o       = fCurrentpmt->GetDh2o();       // h2o distance
    efficiency = fCurrentpmt->GetPmteffc();    // calibrated efficiency
    solidangle = fCurrentpmt->GetSolidangle();
    transpwr   = fCurrentpmt->GetTranspwr();
    
  } else { // Occratio method
    
    dd2o       = fCurrentpmt->GetDd2o() - fCurrentctrpmt->GetDd2o();	   // Delta d2o distance
    dacrylic   = fCurrentpmt->GetDacr() - fCurrentctrpmt->GetDacr();       // Delta acrylic distance
    dh2o       = fCurrentpmt->GetDh2o() - fCurrentctrpmt->GetDh2o();       // Delta h2o distance
    //dd2o       = fCurrentpmt->GetDd();	 // Delta d2o distance
    //dacrylic   = fCurrentpmt->GetDa();         // Delta acrylic distance
    //dh2o       = fCurrentpmt->GetDh();         // Delta h2o distance
    efficiency = 1.0;                          // removed with central run ratio
    solidangle = 1.0;                          // already included in occratio
    transpwr   = 1.0;                          // already included in occratio
  }
 	
  // model response (updated for Rayleigh scattering)
  Float_t pmtresponse = norm * intensity * angresp * 
                        efficiency * solidangle * transpwr *
                        exp( 
                             - dd2o*(Getd2o() + Getrsd2o())
                             - dh2o*(Geth2o() + Getrsh2o())
                             - dacrylic*(Getacrylic() + Getrsacrylic())
                           );
  
  //--------------------
  // Off-centre run derivative contributions:
  // For derivative in LB Mask:
  Float_t coslb, phi;

  TVector3 ballaxis(0,0,1);   // ball axis; vertical for current laserball
  Float_t balltheta = fCurrentrun->GetBalltheta();
  Float_t ballphi = fCurrentrun->GetBallphi();

  TVector3 pmtrelvec(0,0,1);  // PMT vector relative to laserball position
  Float_t lasertheta = fCurrentpmt->GetLasertheta();
  Float_t laserphi = fCurrentpmt->GetLaserphi();

  // Make this look like ModelLBDist.
  if (fCurrentrun->GetBalltheta()==0) {  // no re-orientation of laserball axis
    coslb = cos(fCurrentpmt->GetLasertheta());
    phi = fmod((double) (laserphi + ballphi), 2.0*M_PI);
  } else {  // re-orientation of laserball axis; need third Euler angle to complete!
    ballaxis.SetXYZ(0,0,1);
    ballaxis.SetTheta(balltheta);
    ballaxis.SetPhi(ballphi);

    pmtrelvec.SetXYZ(0,0,1);
    pmtrelvec.SetTheta(lasertheta);
    pmtrelvec.SetPhi(laserphi);

    coslb = pmtrelvec * ballaxis;
    phi = 0.0;
  }

  // check bounds
  if (coslb>1) coslb = 1;
  else if (coslb<-1) coslb = -1;

  if (phi > 2*M_PI) phi -= 2*M_PI;
  else if (phi < 0) phi += 2*M_PI;

  if (derivatives) {
    dyda[Getd2opari()] = -dd2o;
    dyda[Getacrylicpari()] = -dacrylic;
    dyda[Geth2opari()] = -dh2o;

    // Check whether binned or interpolated model used:
    if (fAngrespFuncType == 0) {
      dyda[arpari+fiang  ] = +(1.0 - interpolfrac)/angresp;
      dyda[arpari+fiang+1] = +interpolfrac/angresp;
    } else if (fAngrespFuncType == 1 || fAngrespFuncType == 2) {
      dyda[arpari+fiang] = +1.0/angresp;
      // printf("dyda[ GetAngularResponseParIndex() + fiAng ] = %.5f\n", dyda[arpari+fiang]);
      // printf( "fiAng: %i, angResp: %.5f, 1.0/angResp: %.5f\n", fiang, angresp, 1.0 / angresp );
      // printf("---------------\n");
    }

    // LB parametrization require derivatives for each parameter
    if(!fLBdistType){
      dyda[Getlbdistpari()+filbdist] = +1.0/laserlight;
      //printf( "Laserlight DYDA: %.5f\n", dyda[Getlbdistpari()+filbdist] );
      //printf( "laserlight: %.5f\n", laserlight );
    }
    else {
      Double_t* parlb = new Double_t[1+fNdistwave];
      Double_t* lbphi = new Double_t(phi);
      for(i=0; i<fNdistwave; i++) {parlb[1+i] = Getlbdistwave()[filbdist*fNdistwave+i];}
      for(i=0; i<fNdistwave; i++){
        parlb[0] = (Double_t)i;
	dyda[Getlbdistwavepari()+filbdist*fNdistwave+i] = dLBDistWave(lbphi,parlb)/laserlight;
      }
      delete [] parlb;
      delete lbphi;
    }
    dyda[Getnormpari()+finorm] = +1.0/norm;

    Double_t* parmask = new Double_t[1+fNlbmask];
    Double_t* lbctheta = new Double_t(coslb);
    for(i=0; i<fNlbmask; i++) parmask[1+i] = Getlbmask()[i];
    for(i=0; i<fNlbmask; i++) {
      parmask[0] = (Double_t)i;
      dyda[Getlbmaskpari()+i] = dLBMask(lbctheta,parmask)/lbmask;
    }
    delete [] parmask;
    delete lbctheta;
    
    dyda[Getrsd2opari()] = -dd2o;
    dyda[Getrsacrylicpari()] = -dacrylic;
    dyda[Getrsh2opari()] = -dh2o;

  }

  //--------------------
  // Centre run:
  Float_t cnorm, cpmtresponse;
  if(fOccType == 1) {
    
    if (fCurrentctrrun && fCurrentctrpmt) { // Check validity - otherwise, use 1.0 !
  	  if ( fCurrentpmt->GetPmtn() != fCurrentctrpmt->GetPmtn()) {
		printf("Warning in QOCAFit::Model():\n");
		printf("\tPmt number mismatch:\n");
		printf("\tRun %d ipmt %d num %d  Crun %d ipmt %d num %d\n",
			   fCurrentrun->GetRun(), jpmt,   fCurrentpmt->GetPmtn(),
			fCurrentctrrun->GetRun(),icpmt,fCurrentctrpmt->GetPmtn());
		printf("\tThis should not happen here!\n");
		return 0;
  	  }
      cnorm = 1.0; // Central runs are pre-normalized in occratio in the OCATree

      fcilbdist = 0;
      laserlight = ModelLBDist(fCurrentctrrun, fCurrentctrpmt, fcilbdist);
      lbmask = ModelLBMask(fCurrentctrpmt);
      intensity = 1.0 * laserlight * lbmask;

      fciang = 0;
      interpolfrac = 0;  // 22-Nov-2000 - Moffat - Interpolation fraction btw bins
      angresp = ModelAngResp(fCurrentctrpmt,fciang,interpolfrac);

      //--------------------
      // For central run's pmt:
      cpmtresponse = cnorm * intensity * angresp;

      // For derivative in LB Mask
      balltheta = fCurrentctrrun->GetBalltheta();
      ballphi = fCurrentctrrun->GetBallphi();
      
      lasertheta = fCurrentctrpmt->GetLasertheta();
      laserphi = fCurrentctrpmt->GetLaserphi();
      
      if (fCurrentctrrun->GetBalltheta()==0) {  // no re-orientation of laserball axis
	coslb = cos(fCurrentctrpmt->GetLasertheta());
	phi = fmod((double) (laserphi + ballphi), 2.0*M_PI);
      } else {  // re-orientation of laserball axis; need third Euler angle to complete!
	ballaxis.SetXYZ(0,0,1);
	ballaxis.SetTheta(balltheta);
	ballaxis.SetPhi(ballphi);
	
	pmtrelvec.SetXYZ(0,0,1);
	pmtrelvec.SetTheta(lasertheta);
	pmtrelvec.SetPhi(laserphi);
	
	coslb = pmtrelvec * ballaxis;
	phi = 0.0;
	    
      }

      if (coslb>1) coslb = 1;
      else if (coslb<-1) coslb = -1;

      if (phi > 2*M_PI) phi -= 2*M_PI;
      else if (phi < 0) phi += 2*M_PI;

      //--------------------
      // Central run derivative contributions:
      if (derivatives) {
	// Check whether binned or interpolated model used:
	if (fAngrespFuncType == 0) {
	  dyda[arpari+fciang  ] -=  (1.0 - interpolfrac)/angresp;
	  dyda[arpari+fciang+1] -=  interpolfrac/angresp;
	} else if (fAngrespFuncType == 1 || fAngrespFuncType == 2) {
	  dyda[arpari+fciang] -= 1.0/angresp;
      // printf("dyda[ GetAngularResponseParIndex() + fCiAng ] = %.5f\n", dyda[arpari+fciang]);
      // printf( "fCiAng: %i, angRespCtr: %.5f, 1.0/angRespCtr: %.5f\n", fciang, angresp, 1.0 / angresp );
      // printf("---------------\n");
	}

	
	// LB parametrization require derivatives for each parameter, adapted
	// for the occupancy ratio.
	if(!fLBdistType){
      dyda[Getlbdistpari()+fcilbdist] -= 1.0/laserlight;
      //printf( "Central Laserlight DYDA: %.5f\n", dyda[Getlbdistpari()+fcilbdist] );
      //printf( "Central laserlight: %.5f\n", laserlight );
    }
	else {
	  Double_t* parlb = new Double_t[1+fNdistwave];
	  Double_t* lbphi = new Double_t(phi);
	  for(i=0; i<fNdistwave; i++) parlb[1+i] = Getlbdistwave()[fcilbdist*fNdistwave+i];
	
	  for(i=0; i<fNdistwave; i++){
	    parlb[0] = (Double_t)i;
	    dyda[Getlbdistwavepari()+fcilbdist*fNdistwave+i] -= dLBDistWave(lbphi,parlb)/laserlight;
	  }
	  delete [] parlb;
	  delete lbphi;
	}
	
	Double_t* parmask = new Double_t[1+fNlbmask];
	Double_t* lbctheta = new Double_t(coslb);
	for(i=0; i<fNlbmask; i++) parmask[1+i] = Getlbmask()[i];
	for(i=0; i<fNlbmask; i++) {
	  parmask[0] = (Double_t)i;
	  dyda[Getlbmaskpari()+i] -= dLBMask(lbctheta,parmask)/lbmask;
	}
	delete [] parmask;
	delete lbctheta;
	
      }
    } else {
      cpmtresponse = 1.0;  // And all derivatives are zero (no central run)
    }
  } else {
    cpmtresponse = 1.0;  // And all derivatives are zero - only the occupancy matters!
  }

  //--------------------
  // Model prediction is for the "corrected" occupancy or occupancy ratio
  Float_t modelvalue = pmtresponse / cpmtresponse;
  
  if (derivatives) {
    // Again, loop is too slow, so multiply only those elements which need it
    // for (i=1; i<=na; i++) dyda[i] *= modelvalue;
    //
    // NB: the following code multiplies by modelvalue TWICE for equal values of
    // fiang==fciang or filbdist==fcilbdist!!  Avoid this by using a list of unique
    // parameters from FillParsPoint() (see below).
    // 9-Mar-2001 - Bryce Moffat
    //
    //      dyda[Getd2opari()] *= modelvalue;
    //      dyda[Getacrylicpari()] *= modelvalue;
    //      dyda[Geth2opari()] *= modelvalue;
    
    //      dyda[Getangresppari()+fiang  ] *= modelvalue;
    //      if (fAngrespFuncType == 0) dyda[Getangresppari()+fiang+1] *= modelvalue;
    //      for (i=0; i<fNlbmask; i++) dyda[Getlbmaskpari()+i] *= modelvalue;
    //      dyda[Getlbdistpari()+filbdist] *= modelvalue;
    //      dyda[Getnormpari()+finorm] *= modelvalue;
    
    //      dyda[Getangresppari()+fciang  ] *= modelvalue;
    //      if (fAngrespFuncType == 0) dyda[Getangresppari()+fciang+1] *= modelvalue;
    //      dyda[Getlbdistpari()+fcilbdist] *= modelvalue;

    // Make use of the list facitilities for unique parameters
    // 9-Mar-2001 -Bryce Moffat
    FillParsPoint();
    for (i=1; i<=fparma; i++) {
      //printf("fparam is: %i\n", fparma);
		dyda[fparindex[i]] *= modelvalue; // Multiply unique pars
    }

    // Zero non-varying parameter derivatives - not strictly necessary!
    //    for (i=1; i<=fNpars; i++) if (!fmrqvary[i]) dyda[i] = 0.0;
  }

  return modelvalue;
}
//______________________________________________________________________________
void QOCAFit::SetModelErr(Int_t type, Float_t err)
{
  // Enable or disable the ModelErr contribution to the QOCAFit calculation.
  //
  // type = 0 : no Model errors = 0.0
  //        1 : wavelength dependent increase of systematic error with theta_PMT
  //        2 : constant fractional error
  //        3 : same as (1) but set by the user at run time 
  //            using SetModelErrFunction(par0,par1,par2)

  if(type < 0 || type > 3){
    Warning("SetModelErr","Unrecognized error type value %d.",type);
    fModelErrType = 1; // default
    fModelErr = err;   // default
  } else {
    fModelErrType = type;
    fModelErr = err;
  }
  
  return;
}
//______________________________________________________________________________
Float_t QOCAFit::ModelErr(Int_t irun, Int_t jpmt, Float_t occvariable)
{
  // Estimate the additional systematic error for PMT variability.
  // These functions are derived from previous QOCAFit's.
  //
  // 12-Jan-2001 - Moffat
  // February and September 2000 data runs have different PMT-PMT variability.
  // Take this into account by checking the run number.
  //
  // 23-Oct-2003 - Maneira
  // Added variability function calculated from as the salt scans (see memo for details)
  // They parameters looked pretty stable for all scans, so we just take the average.
  // The default is now to use these values for all runs,except the pure d2o scans, where 
  // we keep Bryce's original functions.
  // However, for each new scan, they should be recalculated and checked for stability.


  if (fModelErrType == 0) return 0.0;

  if (occvariable<0) occvariable = Model(irun,jpmt);

  if (fModelErrType == 2) return fModelErr * occvariable;

  if (fModelErrType == 1) {
    fCurrentrun = fRuns[GetRunsToDoIndex(irun)];
    fCurrentpmt = fCurrentrun->GetPMT(jpmt);
    
    Float_t wavelength = fCurrentrun->GetLambda();
    Float_t sigmapmt   = fModelErr;               // Fractional error - conv. to abs. below
    Float_t theta      = acos(fCurrentpmt->GetCospmt())*180.0/M_PI;
    
    Int_t runnumber = fCurrentrun->GetRun();

    if ((runnumber>=12600) && (runnumber<=13300)) {
	 
	// Sept. 2000 runs	
      if (wavelength>333 && wavelength<340) {
	sigmapmt = sqrt(pow(0.04633121 + 0.00070235*theta + 0.00000590*theta*theta,2) -
			pow(0.03189452 - 0.00014651*theta,2));
      } else if (wavelength>355 && wavelength<375) {
	sigmapmt = sqrt(pow(0.04468870 + 0.00055198*theta - 0.00000151*theta*theta,2) -
			pow(0.03410363 - 0.00011346*theta,2));
      } else if (wavelength>375 && wavelength<395) {
	sigmapmt = sqrt(pow(0.04602904 + 0.00005894*theta + 0.00001490*theta*theta,2) -
			pow(0.03244604 - 0.00010307*theta,2));
      } else if (wavelength>410 && wavelength<430) {
	sigmapmt = sqrt(pow(0.05457454 + 0.00055249*theta - 0.00000599*theta*theta,2) -
			pow(0.05111634 - 0.00007532*theta,2));
      } else if (wavelength>490 && wavelength<510) {
	sigmapmt = sqrt(pow(0.05195506 - 0.00138147*theta + 0.00004945*theta*theta,2) -
			pow(0.03018792 - 0.00017373*theta,2));
      } else if (wavelength>610 && wavelength<630) {
	sigmapmt = sqrt(pow(0.03829374 + 0.00005065*theta + 0.00001558*theta*theta,2) -
			pow(0.02766801 - 0.00012642*theta,2));
      } else {
	sigmapmt = 0.05;
      }

    } else if ((runnumber>=10900) && (runnumber<=11300)) { 
	
	// Feb. 2000 runs
      if (wavelength>355 && wavelength<375) {
	sigmapmt = sqrt(pow(0.03287963 + 0.00057363*theta + 0.00000312*theta*theta,2) -
			pow(0.02541142 - 0.00004858*theta,2));
	// For the fit after the PMTR interpolation fix: No visible change!
//  	sigmapmt = sqrt(pow(0.03286006 + 0.00057439*theta + 0.00000311*theta*theta,2) - 
//  			pow(0.02541142 - 0.00004858*theta,2));
      } else if (wavelength>490 && wavelength<510) {
  	sigmapmt = sqrt(pow(0.03563484 - 0.00009589*theta + 0.00001851*theta*theta,2) -
  			pow(0.02646099 - 0.00013946*theta,2));
	// For the fit after the PMTR interpolation fix: No visible change!
//  	sigmapmt = sqrt(pow(0.03566868 - 0.00010476*theta + 0.00001880*theta*theta,2) - 
//  			pow(0.02646099 - 0.00013946*theta,2));
      } else {
	sigmapmt = 0.05;
      }
    } else {
	  
	  // sigmapmt = fModelErr;  // Default for unrecognized run subset
	  // Default now is to use the average values from the salt phase
	  
	   if (wavelength>333 && wavelength<340) {
	sigmapmt = 0.05118 - 0.00002160*theta + 0.00003802*theta*theta;
      } else if (wavelength>355 && wavelength<375) {
	sigmapmt = 0.04613 - 0.0004448*theta + 0.00004059*theta*theta;
      } else if (wavelength>375 && wavelength<395) {
	sigmapmt = 0.03996 - 0.0003817*theta + 0.00004004*theta*theta;
      } else if (wavelength>410 && wavelength<430) {
	sigmapmt = 0.03973 - 0.0003951*theta + 0.000034063*theta*theta;
      } else if (wavelength>490 && wavelength<510) {
	sigmapmt = 0.03938 - 0.0005744*theta + 0.00004372*theta*theta;
      } else if (wavelength>610 && wavelength<630) {
	sigmapmt = 0.03940 - 0.0007118*theta + 0.00005118*theta*theta;
      } else {
	sigmapmt = 0.05;
      }
	  
    }
    return sigmapmt * occvariable;
  }

  // ---------------------------------------
  // os 09.2005
  // Now the user can set the error at run-time by calling
  // SetModelErr(3) and SetModelErrFunction(par0,par1,par2).
  // Therefore, there is no need to check for wavelength and 
  // hard-coded values as above.
  if (fModelErrType == 3) {
    
    fCurrentrun = fRuns[GetRunsToDoIndex(irun)];
    fCurrentpmt = fCurrentrun->GetPMT(jpmt);
    
    Float_t sigmapmt   = fModelErr;
    Float_t theta      = acos(fCurrentpmt->GetCospmt())*180.0/M_PI;

    sigmapmt = fPmtVarPar0 + fPmtVarPar1*theta + fPmtVarPar2*theta*theta;

    return sigmapmt * occvariable;
  }


  return 0.0;  // Fall-through condition (unrecognized fModelErr type)
}
//______________________________________________________________________________
void QOCAFit::SetModelErrFunction(Float_t par0, Float_t par1, Float_t par2)
{
  // 09.2005 - Olivier Simard
  // Allow run-time input of the PMT variability polynomial function with theta_PMT.
  // The function takes 2 or 3 parameters:
  //   - 2nd order: SetModelErrFunction(par0, par1, par2) (polynomial)
  //   - 1st order: SetModelErrFunction(par0, par1)       (linear)
  //
  // The user must ensure the parameters carry the right signs
  // (see ModelErr(irun, jpmt, occvariable)) for details.
  //
  //

  if((par0 == 0.0) && (par1 == 0.0) && (par2 == 0.0)){
    Warning("SetModelErrFunction","All parameters set to 0.");
  } else {
    // store the parameters in private fields
    fPmtVarPar0 = par0;  // offset
    fPmtVarPar1 = par1;  // multiplies theta
    fPmtVarPar2 = par2;  // multiplies theta^2
  }
  
  return;
}
//______________________________________________________________________________
void QOCAFit::GetModelErrFunction()
{
  // Simply prints the parameters set by the user using
  // SetModelErrFunction(par0, par1, par2).

  printf("QOCAFit::GetModelErrFunction(): fPmtVarParX = %+.4e %+.4e %+.4e\n",
	 fPmtVarPar0,fPmtVarPar1,fPmtVarPar2);

  return;
}
//______________________________________________________________________________
Float_t QOCAFit::GetModelErrFunctionPar(Int_t parnumber)
{
  // Simply prints and returns the parameter set by the user using
  // SetModelErrFunction(par0, par1, par2) for the given parnumber.

  if(parnumber < 0 || parnumber > 2){
    Warning("GetModelErrFunctionPar","Invalid parameter number %d.",parnumber);
    return 0.0;
  }

  Float_t par;
  if(parnumber == 0) par = fPmtVarPar0;
  if(parnumber == 1) par = fPmtVarPar1;
  if(parnumber == 2) par = fPmtVarPar2;

  printf("QOCAFit::GetModelErrFunction(): fPmtVarPar%d = %+.4f\n",
	 parnumber,par);

  return par;
}

//______________________________________________________________________________
Float_t QOCAFit::ModelAngResp(QOCAPmt *pmt)
{
  // Return the value of the angular response for the current model parameters
  // for the pmt.
  
  Int_t pari;
  Float_t interpolfrac;
  return ModelAngResp(pmt,pari,interpolfrac);
}
//______________________________________________________________________________
Float_t QOCAFit::ModelAngResp(QOCAPmt *pmt, Int_t &pari, Float_t &interpolfrac)
{
  // Return the value of the angular response for the current model parameters
  // for the pmt, as well as the parameter index used in pari.
  
  Float_t cosangle = pmt->GetCospmt();
  Float_t angle = acos(cosangle);
  Int_t pmtgroup = PmtGroup(pmt);
  return ModelAngResp(angle,pari,interpolfrac,pmtgroup);
}
//____________________________________________________________________________
Float_t QOCAFit::ModelAngResp(Float_t angle)
{
  // Return the value of the angular response for the current model parameters
  // for a pmt at incident angle given.

  Int_t pari;
  Float_t interpolfrac;
  return ModelAngResp(angle,pari,interpolfrac);
}
//____________________________________________________________________________
Float_t QOCAFit::ModelAngResp(Float_t angle, Int_t &pari, Float_t &interpolfrac, Int_t pmtgroup)
{
  // Return the value of the angular response for the current model parameters
  // for a pmt at incident angle given, as well as the parameter index used
  // in pari.
  //
  // angle is in radians; conversion factor is:
  // (angle * 180/pi) * (fNangresp/90 degrees) = angle*2*fNangresp/pi
  //
  // 31-Aug-2000 - Moffat/Hallin
  // Add interpolation for regions where angular response varies rapidly.
  // Needed in particular for studying runs from the guide tube, where the
  // PMT angular response is in the falloff region.
  //
  // NB. *** The calculation of the slope to do the interpolation actually
  // means that this value depends on two parameters: pari and pari+1 !  ***
  //
  // 23-Nov-2000 - Moffat
  // Hey: use of interpolation means there has to be a data point "beyond" the
  // last bin with data; ie. at the far edge of the last bin!  Also, the
  // derivatives need to know about the interpolation, hence the new returned
  // variable "interpolfrac", which gives the evaluation point's fractional
  // distance along the bin.

  Float_t *angresp;
  if (pmtgroup == 2 ) angresp = Getangresp2();
  else angresp = Getangresp();

  Float_t theta = angle * 180.0/M_PI;
  pari = (Int_t) (theta * fNangresp/90.0);  // 2 degree bins for fNangresp = 45

  // Maybe this logic should return 0 as the response instead of bounding
  // the parameter to the upper and lower limits of the angular response
  // range?
  if (pari <0) pari = 0;
  else if (pari >= fNangresp) pari = fNangresp - 1;  // kludge: last bin always 0...

  // Use binned values (binned fits to SNOMAN grey disk):
  if (fAngrespFuncType == 1) {
    interpolfrac = 0;
    return angresp[pari];
  }

  // Use binned values for bin centres (binned fits to data):
  if (fAngrespFuncType == 2) {
    // 17-Jan-2001 - Moffat/Hallin - use bin centres, not left edges of bins
    // by subtracting half a bin width from theta before doing the conversion to
    // parameter number.
    // This is reasonable for binned fits to the data 0-0.5, 0.5-1.5, 1.5-2.5 binning
    // For SNOMAN, the simulations use binned data following 0-1, 1-2, 2-3 binning
    pari = (Int_t) ((theta + 0.5*90.0/fNangresp) * (fNangresp/90.0));
    if (pari <0) pari = 0;
    else if (pari >= fNangresp) pari = fNangresp - 1;  // kludge: last bin always 0...
    interpolfrac = 0;
    return angresp[pari];
  }

  // Otherwise use linear interpolation between bins:
  Float_t theta1 = pari       * 90.0/fNangresp;  // Angle of low edge of bin
//  Float_t theta2 = (pari + 1) * 90.0/fNangresp;  // Angle of high edge of bin 
                                                   // variable not used -- rsd 2002.06.03

  Float_t slope = (angresp[pari+1] - angresp[pari]) / (90.0 / fNangresp);
  Float_t deltheta = theta - theta1;

  // 17-Jan-2001: interpolfrac bug?  Should be fractional distance along the bin
  // between theta1 and theta2.
  //  interpolfrac = 1.0 - deltheta / (90.0/fNangresp);  // 22-Nov-2000 - Moffat
  interpolfrac = deltheta / (90.0/fNangresp);  // 17-Jan-2001 - Moffat

  return angresp[pari] + slope * deltheta;
}

//______________________________________________________________________________
Float_t QOCAFit::ModelLBMask(QOCAPmt *pmt)
{
  // Models the laserball variation with theta by a polynomial of degree
  // (fNlbmask - 1) in cos(theta).

  Float_t lbtheta = pmt->GetLasertheta();
  Float_t coslbtheta = cos(lbtheta);

  return ModelLBMask(coslbtheta);
}
//______________________________________________________________________________
Float_t QOCAFit::ModelLBMask(Float_t costheta)
{
  // Models the laserball variation with theta by a polynomial of degree
  // (fNlbmask - 1) in cos(theta).

  Int_t i;
  Float_t *lbmask = Getlbmask();

  Float_t lbm = 0;
  Float_t oneplus = 1.0 + costheta;
  for (i=fNlbmask-1; i>=0; i--)
    lbm = lbm*oneplus + lbmask[i];
  
  return lbm;
}
//______________________________________________________________________________
Float_t QOCAFit::ModelLBDist(QOCARun *run, QOCAPmt *pmt)
{
  // Return the model's laserball distribution.
  // Takes into account re-orientation of the laserball in phi/theta for runs
  // with the slot facing south, east, west, north.

  Int_t pari;
  return ModelLBDist(run,pmt,pari);
}
//______________________________________________________________________________
Float_t QOCAFit::ModelLBDist(QOCARun *run, QOCAPmt *pmt, Int_t &pari)
{
  // Return the model's laserball distribution.
  // Takes into account re-orientation of the laserball in phi/theta for runs
  // with the slot facing south, east, west, north.
  // Also returns the parameter index in pari.
  //
  // 24-Jan-2001 - Moffat
  // Actually need three Euler angles (or direction cosines) to fully specify
  // the orientation of the laserball: the direction of the laserball axis,
  // and the rotation of the ball about that axis.
  //
  // Currently, we don't need to consider changes in the laserball axis because
  // the hardware isn't built to allow this.  However, for QOCA RSP I need to
  // be able to orient the light distribution for the "event" along any direction.
  // In this case, the rotation of the ball is irrelevant because the "event" is
  // rotationally symmetric about its axis.
  //
  // Split the rotations into two cases:
  //   1) fBalltheta  = 0 : laserball axis is (0,0,1) (vertical in global coords).
  //                        PMT cos(theta)-position is cos(fLasertheta)
  //                        laserball rotation is fBallphi
  //                        PMT phi-position is fLaserphi + fBallphi
  //
  //   2) fBalltheta != 0 : laserball axis is tilted by fBalltheta from vertical
  //                        in the direction specified by fBallphi
  //                        PMT cos(theta)-position is a dot product (see below)
  //                        PMT phi-position is 0 (rotationally symmetric)
  //
  // In future, a full specification of the ball axis direction and rotational
  // orientation could be added with fBallalpha (other Euler angle) in QOCARun
  // and corresponding rotation matrices added in QOCAFit.  But for now, this
  // is sufficient.  Enjoy!

  Float_t costheta;
  Float_t phi;

  TVector3 ballaxis(0,0,1);   // ball axis; vertical for current laserball
  Float_t balltheta = run->GetBalltheta();
  Float_t ballphi = run->GetBallphi();

  TVector3 pmtrelvec(0,0,1);  // PMT vector relative to laserball position
  Float_t lasertheta = pmt->GetLasertheta();
  Float_t laserphi = pmt->GetLaserphi();

  //--------------------
  // First treat the case for "events" in QOCA RSP: laserball axis pointing in
  // direction specified by Balltheta and Ballphi:
  if (balltheta != 0) {  // this shouldn't be dangerous even for a float value...
    ballaxis.SetXYZ(0,0,1);
    ballaxis.SetTheta(balltheta);
    ballaxis.SetPhi(ballphi);

    pmtrelvec.SetXYZ(0,0,1);
    pmtrelvec.SetTheta(lasertheta);
    pmtrelvec.SetPhi(laserphi);

    costheta = pmtrelvec * ballaxis;
    phi = 0.0;  // orientation undefinable without third Euler angle

  } else {
    //--------------------
    // Otherwise, the ball orientation is only in phi in global coord system, and
    // there is no theta rotation of the ball axis.  Of course, the PMT still makes
    // an angle Lasertheta with respsect to the laserball!
    costheta = cos(lasertheta);
    phi = fmod((double) (laserphi + ballphi), 2.0*M_PI);
  }

  //--------------------
  // Check bounds
  if (costheta>1) costheta = 1;
  else if (costheta<-1) costheta = -1;

  if (phi > 2*M_PI) phi -= 2*M_PI;
  else if (phi < 0) phi += 2*M_PI;

  return ModelLBDist(costheta,phi,pari);
}
//______________________________________________________________________________
Float_t QOCAFit::ModelLBDist(Float_t costheta, Float_t phi)
{
  // Return the model's laserball distribution.
  // No offset added to costheta or phi to take into account ball's orientation.
  // Use ModelLBDist(QOCARun *run, QOCAPmt *pmt) to compute the laserball
  // distribution taking into account the laserball orientation.

  Int_t pari;
  return ModelLBDist(costheta,phi,pari);
}
//______________________________________________________________________________
Float_t QOCAFit::ModelLBDist(Float_t costheta, Float_t phi, Int_t &pari)
{
  // Return the model's laserball distribution.
  // No offset added to costheta or phi to take into account ball's orientation.
  // Use ModelLBDist(QOCARun *run, QOCAPmt *pmt) to compute the laserball
  // distribution taking into account the laserball orientation.
  // Also returns the parameter index in pari.
  //
  // 08.2006 - O.Simard
  // This function takes into account if the LB function type before
  // setting the pari integer and returning laserlight.
  // Beware: the pari index is not the same depending on the LB model chosen.

  Int_t itheta;

  phi = fmod((double)phi,2.0*M_PI);
  if (phi > 2*M_PI) phi -= 2*M_PI;
  else if (phi < 0) phi += 2*M_PI;

  if(!fLBdistType){ // histogram
    itheta = (Int_t) ((1+costheta)/2 * fNbinstheta);
    if (itheta < 0) itheta = 0;
    if (itheta >= fNbinstheta) itheta = fNbinstheta-1;
    Int_t iphi = (Int_t) (phi/(2*M_PI) * fNbinsphi);
    if (iphi < 0) iphi = 0;
    if (iphi >= fNbinsphi) iphi = fNbinsphi-1;
    pari = itheta*fNbinsphi + iphi;
    Float_t laserlight = Getlbdist()[pari];
    return laserlight;
  } else { // sinusoidal
    itheta = (Int_t) ((1+costheta)/2 * fNbinsthetawave);
    if (itheta < 0) itheta = 0;
    if (itheta >= fNbinsthetawave) itheta = fNbinsthetawave-1;
    // use sLBDist function
    Double_t* aphi = new Double_t((Double_t)phi);
    Double_t* par = new Double_t[1+fNdistwave];
    par[0] = fNdistwave;
    for(Int_t ipar = 0; ipar < fNdistwave; ipar++){
      // get the parameters of the siusoidal function
      par[ipar+1] = Getlbdistwave()[itheta*fNdistwave+ipar];
    }
    Float_t laserlight = (Float_t)sLBDistWave(aphi,par); // value
    delete par;
    delete aphi;
    pari = itheta; // save the theta slice
    if(fPrint >= 2) printf("itheta: %d, laserlight = %10.8g\n",itheta,laserlight);
    return laserlight;
  }
}
//______________________________________________________________________________
Int_t QOCAFit::MrqFit(float x[], float y[], float sig[], int ndata, float a[],
		    int ia[], int ma, float **covar, float **alpha, float *chisq )
{
  //Fit these data using mrqmin() repeatedly until convergence is achieved.
  
  Int_t maxiter = 1000;
  Int_t numiter = 0;
  Int_t gooditer = 0;
  Int_t retval = 0;

  Float_t oldchisq = 0;
  Float_t lamda = -1.0;
  Float_t tol = 1.0;    // Chisquared must change by tol to warrant another iteration

  *chisq = 0;

  // First, call mrqmin with lambda = -1 for initialization
  if (fPrint>=1) printf("Calling mrqmin for initialization...\n");
  printf( "MA VAL IS: %i\n", ma );
  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda);
  if (fPrint>=1) printf("Done.  Chisq = %.2f\n",*chisq);
  oldchisq = *chisq;
  if (fPrint>=1) printf("CHISQ at origin = %8.2f\n",*chisq);
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lamda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
	 && retval == 0 && lamda != 0.0) {
    oldchisq = *chisq;
    if (fPrint>=1) printf("Iteration %d with lambda %g...\n",numiter,lamda);
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda );
    if (fPrint>=1) printf("New chisq = %12.2f with lambda %g \n",*chisq,lamda);
    numiter++;

    if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lamda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  
  lamda = 0;
  mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lamda );
  return retval;
}
//______________________________________________________________________________
Int_t QOCAFit::mrqmin(float x[], float y[], float sig[], int ndata, float a[],
		      int ia[], int ma, float **covar, float **alpha, float *chisq,
		      float *alamda)
{
  // Minimization routine for a single iteration over the data points.

  // Required helper routines:
  // void covsrt(float **covar, int ma, int ia[], int mfit);
  // void gaussj(float **a, int n, float **b, int m);
  // void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
  //	int ia[], int ma, float **alpha, float beta[], float *chisq,
  //	void (*funcs)(float, float [], float *, float [], int));
  printf("QOCAFIT::mrqmin called\n");
  int j,k,l,m, retval = 0;
  static int mfit;
  static float ochisq,*atry,*beta,*da,**oneda;

  //--------------------
  // Initialization  
  if (*alamda < 0.0) {
    atry=vector(1,ma);
    beta=vector(1,ma);
    da=vector(1,ma);
    if(fPrint>=2) printf("Varying parameter indices:\n");
    for (mfit=0,j=1;j<=ma;j++){
      if (ia[j]) {
	mfit++; 
	if(fPrint>=2) printf("\tpar %.3d varies\n",j);
      }
    }
    if(fPrint>=2) printf("Number of varying parameters: %d vs %d\n",mfit,GetNparsVariable());
    oneda=matrix(1,mfit,1,1);
    *alamda=0.001;
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
      covar[j][j]=alpha[j][j]*(1.0+(*alamda));
      //printf( "alpha[ %i ][ %i ] = %.5f\n", j, j, alpha[j][j] );
      oneda[j][1]=beta[j];
	  if(covar[j][j] <= 0.0) {
	    if(covar[j][j] == 0.0) {
	      printf("*** Zero covariance diagonal element at j %d (l %d)\n",j,l);
	      PrintParameter(j);
	      printf("*** Bad parameter %d\n",l);
	      PrintParameter(l);
	    } else {
	      printf("*** Negative covariance diagonal element at j %d (l %d)\n",j,l);
	      PrintParameter(j);
	    }
	  }
    }
  }

  if (fPrint>=1) printf("Inverting the solution matrix in QOCAFit::mrqmin()...\n");
  retval = gaussj(covar,mfit,oneda,1);
  if (fPrint>=1) if (retval<0) printf("error %d...",retval);
  if (fPrint>=1) printf("done.\n");

  for (j=1;j<=mfit;j++) da[j]=oneda[j][1];

  //--------------------
  // Final call to prepare covariance matrix and deallocate memory.
  if (*alamda == 0.0 ) {
    //printf("PRE_COVSRT\n");
    //PrintCovarianceMatrix();
    //printf("ma is: %i, mfit is: %i\n", ma, mfit );
    covsrt(covar,ma,ia,mfit);
    //printf("POST_COVSRT\n");
    //PrintCovarianceMatrix();
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
    *alamda *= 0.1;
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
    *alamda *= 10.0;
    *chisq=ochisq;
  }
  return retval;
}
//______________________________________________________________________________
void QOCAFit::covsrt(float **covar, int ma, int ia[], int mfit)
{
  // Covariance matrix sorting.  Helper routine for mrqmin()
  printf("QOCAFIT::covsrt called\n");
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
Int_t QOCAFit::gaussj(float **a, int n, float **b, int m)
{
  // Gauss-Jordan matrix solution helper routine for mrqmin.
  printf("QOCAFIT::gaussj called\n");
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
void QOCAFit::SetMrqArrays(Int_t nelements,Float_t *chiarray,Float_t *resarray)
{
  // Set the array pointers for storing residuals and chisq results on
  // an entry by entry basis in mrqcof()
  
  fNelements = nelements;
  fChiarray = chiarray;
  fResarray = resarray;
}
//________________________________________________________________________________
void QOCAFit::FillParmabase()
{
  // Calculate the base number of parameters for every function evaluation.
  // These are the parameters which are globally variable over all PMT's in all
  // runs.
  //
  // Fills:  fparmabase = number of base parameters which are variable
  //         fparindex[] = array of parameter indices for variable parameters
  //
  // Does not include the number of laserball distribution, PMT response
  // parameters or run normalization since these depend on the specific PMT/run
  // being evaluated.
  //
  // To get the total number of variable parameters for a particular evaluation,
  // call GetParma().

  Int_t i;

  if (fparindex) delete[] fparindex;
  fparindex = new Int_t[fNpars];  // #unique pars <= fNpars is guaranteed.

  fparmabase = 0;
  if (GetD2OVary()) fparindex[++fparmabase] = Getd2opari();
  if (GetAcrylicVary()) fparindex[++fparmabase] = Getacrylicpari();
  if (GetH2OVary()) fparindex[++fparmabase] = Geth2opari();

  // 06.2006 - O.Simard
  // following number of parameters were added to the fixed number
  // from version 15
  if (GetRayleighD2OVary()) fparindex[++fparmabase] = Getrsd2opari();
  if (GetRayleighAcrylicVary()) fparindex[++fparmabase] = Getrsacrylicpari();
  if (GetRayleighH2OVary()) fparindex[++fparmabase] = Getrsh2opari();
  
  for (i=0; i<fNlbmask; i++)
    if (fmrqvary[Getlbmaskpari()+i])
      fparindex[++fparmabase] = Getlbmaskpari() + i;

  if(fLBdistType){
    for (i=0; i<fNbinsthetawave*fNdistwave; i++)
      if (fmrqvary[Getlbdistwavepari()+i]) 
        fparindex[++fparmabase] = Getlbdistwavepari() + i;
  }
  
  if (fPrint>=2){
    printf("fparmabase = %d == ",fparmabase);
    for (i=1; i<=fparmabase; i++) printf("%d ",fparindex[i]);
    printf("\n");
  }
}
//________________________________________________________________________________
void QOCAFit::FillAngIndex()
{
  // Calculate and fill the array which correlates fiang and fciang with unique
  // variable parameters.
  //
  // For example: fiang  fciang  interpolated[]  binned[]
  //                0      0       0,1,x,x (2)   0,x,x,x  (1)
  //                0      1       0,1,2,x (3)   0,1,x,x  (2)
  //                0      2       0,1,2,3 (4)   0,2,x,x  (2)
  //                0      3       0,1,3,4 (4)   0,3,x,x  (2)
  //                ...
  //                1      0       0,1,2,x (3)   0,1,x,x  (2)
  //                1      1       1,2,x,x (2)   1,0,x,x  (1)
  //                1      2       1,2,3,x (3)   1,2,x,x  (2)
  //                1      3       1,2,3,4 (4)   1,3,x,x  (2)
  //                1      4       1,2,4,5 (4)   1,4,x,x  (2)
  //
  // The first element of the array fangindex[][][0] contains the number of
  // unique parameters for that combination of fiang and fciang.
  //
  // Also make a correspondence array between the parameters and the _variable_
  // parameters (globally!)

  Int_t i,j;

  if (fparvarmap) delete[] fparvarmap;
  fparvarmap = new Int_t[fNpars+1];

  j = 0;
  for (i=1; i<=fNpars; i++) if (fmrqvary[i]) fparvarmap[i] = ++j;

  // Check if necessary, then de-allocate the fangindex[][][] lookup table
  // NB. Always gets fully allocated here, so we only have to check the base
  // ***fangindex against NULL to verify whether the structure needs deleting.
  if (fangindex) {
    for (i=0; i<fNangresp+1; i++) {
      for (j=0; j<fNangresp+1; j++) {
	delete[] fangindex[i][j];
      }
      delete[] fangindex[i];
    }
    delete[] fangindex;
  }
  fangindex = NULL;

  // Allocate the three-dimensional lookup table
  fangindex = new Int_t**[fNangresp+1];
  for (i=0; i<fNangresp+1; i++) {
    fangindex[i] = new Int_t*[fNangresp+1];
    for (j=0; j<fNangresp+1; j++) fangindex[i][j] = new Int_t[4+1];
  }

  // Correlate fiang with fciang and unique PMTR parameters.
  // It's still up to the caller to check for 
  Int_t first, second;

  if(fOccType){    // occratio, old style
  for (i=0; i<=fNangresp; i++) {
	  for (j=0; j<=fNangresp; j++) {
		if (i<=j) { first = i; second = j; }
		else { first = j; second =i; }
  
		if (fAngrespFuncType == 0) {  // Interpolated
		  fangindex[i][j][1] = first;
		  fangindex[i][j][2] = first+1;
		  if (first==second) {
			fangindex[i][j][0] = 2;
			fangindex[i][j][3] = -1;
			fangindex[i][j][4] = -1;
		  } else if (second-first == 1) {
			fangindex[i][j][0] = 3;
			fangindex[i][j][3] = second+1;
			fangindex[i][j][4] = -1;
		  } else {
			fangindex[i][j][0] = 4;
			fangindex[i][j][3] = second;
			fangindex[i][j][4] = second+1;
		  }
		} else { // Binned
		  if (first==second) {
			fangindex[i][j][0] = 1;
			fangindex[i][j][1] = first;
			fangindex[i][j][2] = -1;
			fangindex[i][j][3] = -1;
			fangindex[i][j][4] = -1;
		  } else {
			fangindex[i][j][0] = 2;
			fangindex[i][j][1] = first;
			fangindex[i][j][2] = second;
			fangindex[i][j][3] = -1;
			fangindex[i][j][4] = -1;
		  }
		}
		if (fPrint>=2) {
		  printf("%d %d :: %d %d %d %d %d\n",i,j,fangindex[i][j][0],fangindex[i][j][1],
			  fangindex[i][j][2],fangindex[i][j][3],fangindex[i][j][4]);
		}
	  }
		}
  } else {
		for (i=0; i<=fNangresp; i++) {
	  for (j=0; j<=fNangresp; j++) {
		if (fAngrespFuncType == 0) {  // Interpolated
				fangindex[i][j][0] = 2;
				fangindex[i][j][1] = i;
				fangindex[i][j][2] = i+1;
				fangindex[i][j][3] = -1;
				fangindex[i][j][4] = -1;
			} else { // Binned
				fangindex[i][j][0] = 1;
				fangindex[i][j][1] = i;
				fangindex[i][j][2] = -1;
				fangindex[i][j][3] = -1;
				fangindex[i][j][4] = -1;
			}		     
		  }
		}
	}
  return;
}
//________________________________________________________________________________
Int_t QOCAFit::FillParsPoint()
{
  // Figure out how many variable and unique parameters there are for the current
  // fit model point.  This is the base number (attenuations and lbmask) plus the
  // ones that can change on a per PMT basis (PMTR, lbdist, run norm).
  //
  // Returns the number of variable parameters for a given PMT evaluation.

  Int_t i;

  //--------------------
  // Base unique variable parameters
  fparma = fparmabase;

  //--------------------
  // PMTR
  Int_t parnum;
  if (fPrint>=3)
    printf("group1 pmtr: %d (%d, %d) ==||== ",fangindex[fiang][fciang][0],fiang,fciang);
  for (i=1; i<=fangindex[fiang][fciang][0]; i++) {
    parnum = Getangresppari() + fangindex[fiang][fciang][i];
    if (fmrqvary[parnum]) {
      fparindex[++fparma] = parnum;
      if (fPrint>=3) printf("%d (%d), ",fangindex[fiang][fciang][i],parnum);
    }
  }
  if (fPrint>=3) printf("\n");
  if (fPrint>=3)
    printf("group2 pmtr: %d (%d, %d) ==||== ",fangindex[fiang][fciang][0],fiang,fciang);
  for (i=1; i<=fangindex[fiang][fciang][0]; i++) {
    parnum = Getangresp2pari() + fangindex[fiang][fciang][i];
    if (fmrqvary[parnum]) {
      fparindex[++fparma] = parnum;
      if (fPrint>=3) printf("%d (%d), ",fangindex[fiang][fciang][i],parnum);
    }
  }
  if (fPrint>=3) printf("\n");

  //--------------------
  // Laserball distribution
//   Int_t first, second;
//   if (filbdist<=fcilbdist) { first = filbdist; second = fcilbdist; }
//   else { first = fcilbdist; second = filbdist; }
// 
//   parnum = Getlbdistpari() + first;
//   if (fmrqvary[parnum]) fparindex[++fparma] = parnum;
// 
//   if (first != second) {
//     parnum = Getlbdistpari() + second;
//     if (fmrqvary[parnum]) fparindex[++fparma] = parnum;
//   }

  if(!fLBdistType){
    if(fOccType){  //occratio, old style
      Int_t first, second;
      if (filbdist<=fcilbdist) { first = filbdist; second = fcilbdist; }
      else { first = fcilbdist; second = filbdist; }
      
      if (first != second) {
        parnum = Getlbdistpari() + first;
        if (fmrqvary[parnum]) fparindex[++fparma] = parnum;	
        
    	parnum = Getlbdistpari() + second;
    	if (fmrqvary[parnum]) fparindex[++fparma] = parnum;
      }
    } else {
      parnum = Getlbdistpari() + filbdist;
      if (fmrqvary[parnum]) fparindex[++fparma] = parnum; 
    }
  }
  
  //--------------------
  // Normalization
  parnum = Getnormpari() + finorm;
  if (fmrqvary[parnum]) fparindex[++fparma] = parnum;

  //--------------------
  if (fPrint>=2) {
    printf("fparma %d : ",fparma);
    for (i=1; i<=fparma; i++) printf("%d ",fparindex[i]);
    printf("\n");
  }
  
  return fparma;
}
//________________________________________________________________________________
void QOCAFit::mrqcof(float x[], float y[], float sig[], int ndata, float a[],
		     int ia[], int ma, float **alpha, float beta[], float *chisq)
{
  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  printf("QOCAFIT::mrqcof called\n");
  float chisqentry;  // chi-squared for single entry in list
  
  float *beta2 = vector(1,ma);        // 10-Mar-2001 - Debugging checks - Moffat
  float **alpha2 = matrix(1,ma,1,ma);

  dyda=vector(1,ma);
  for (j=1;j<=ma;j++) {
    if (ia[j]) mfit++;
    dyda[j] = 0.0;
  }
  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) {
      alpha[j][k]=0.0;
      alpha2[j][k]=0.0; // Debugging - 10-Mar-2001 - Moffat
    }
    beta[j]=0.0;
    beta2[j]=0.0; // Debugging - 10-Mar-2001 - Moffat
  }
  *chisq=0.0;
  for (i=fNStart+1;i<=ndata;i+=fNSkip) {  // Skip some tubes to increase speed...
    if ( !dyda ){ std::cout << "dyda invalid!" << std::endl; }
    mrqfuncs( x[i],i,a,&ymod,dyda,ma);
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;

    //........................................
    // Loop over all variable parameters is unnecessary in QOCAFit.
    // We only calculate the derivative for 3 attenuations, the LB distribution
    // (<=2 pars), LB mask (<=2 pars) and normalization (1 par) so the loop
    // should be replaced with explicit summations over these derivatives only.
    // 8-Mar-2001 - Bryce Moffat
    //........................................
    FillParsPoint();
    for (l=1; l<=fparma; l++) {
//		if(dyda[fparindex[l]]==0.0) printf("mrqcofNull l %d index %d\n",l,fparindex[l]);	
	
      wt = dyda[fparindex[l]] * sig2i;
      for (m=1; m<=l; m++) {
	j = fparindex[l];
	k = fparindex[m];
	if (k<=j) alpha[fparvarmap[j]][fparvarmap[k]] += wt * dyda[k];
    // if ( fparvarmap[j] == 53 && fparvarmap[k] == 53 && wt != 0.0 ){ 
    //   std::cout << "wt is: " << wt <<  std::endl;
    //   std::cout << "dyda[k] is: " << dyda[k] <<  std::endl;
    //   std::cout << "sig2i is: " << sig2i <<  std::endl;
    //   std::cout << "1/sigi is: " << TMath::Sqrt( 1.0 / sig2i ) <<  std::endl;
    //   std::cout << "dyda[fParamIndex[l]] is: " << dyda[fparindex[l]] <<  std::endl;
    //   std::cout << "alpha[fParamVarMap[j]][fParamVarMap[k]] is: " << alpha[fparvarmap[j]][fparvarmap[k]] <<  std::endl;
    //   std::cout << "-----------" << std::endl;
    // }
      }
      beta[fparvarmap[fparindex[l]]] += dy * wt;
    }

    //........................................
    if (fRepeatoldmrqcof) {
      std::cout << "IS this called?" << std::endl;
      for (j=0,l=1;l<=ma;l++) {  // Original Numerical recipes code
	if (ia[l]) {
	  wt=dyda[l]*sig2i;
	  for (j++,k=0,m=1;m<=l;m++)
	    if (ia[m]) alpha2[j][++k] += wt*dyda[m];
	  beta2[j] += dy*wt;
	}
      }
    }
    //........................................
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    if (fPrint>=3) if (i%1==2000) printf(" %d %f %f\n",i,x[i],chisqentry);
    if (fChiarray!=NULL && i>=0 && i<fNelements) fChiarray[i] = chisqentry;
    if (fResarray!=NULL && i>=0 && i<fNelements) fResarray[i] = dy;
  }
  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) {
      alpha[k][j]=alpha[j][k];
      if (fRepeatoldmrqcof) {
	alpha2[k][j]=alpha2[j][k];
	if (alpha[k][j] != alpha2[k][j]) {
	  printf("bogus: %d %d :: %g <--> %g\n",k,j,alpha[k][j],alpha2[k][j]);
	}
      }
    }
  
  free_vector(dyda,1,ma);

  free_matrix(alpha2,1,ma,1,ma);  // 10-Mar-2001 - Debugging checks - Moffat
  free_vector(beta2,1,ma);
}
//______________________________________________________________________________
void QOCAFit::mrqfuncs(Float_t x, Int_t i, Float_t a[], Float_t *y,
			Float_t dyda[], Int_t na)
{
  //Function used by mrqcof()
  //   a[]    = parameters to be fit:
  //              d2o, acrylic, h2o attenuation lengths,
  //              PMT angular response,
  //              laserball mask,
  //              laserball distribution,
  //              run normalizations
  //   y      = modelled value based on input parameters = Nij expected for given PMT
  //   dyda[] = derivative of y w.r.t. input parameters
  //   na     = number of parameters
  //
  // For central runs, use the QOCARun::GetNorm() stored by QPath::Calculate(),
  // which is just the simple average.
  //

  if ( !fTree || !fRuns || !fCentrerunptr) {
    Error("mrqfuncs","OCA data structure unitialized!");
  }
  
  Int_t ix = (Int_t)x;
  Int_t irun  = ix/10000;
  Int_t jpmt = ix%10000;

  Float_t *mrqparsave = fmrqpars; // Save parameters and use the ones just passed
  fmrqpars = a;

  *y = Model(irun,jpmt,na,dyda);  // Derivatives also calculated!
  
  fmrqpars = mrqparsave; // Restore parameters

  fNcalls++;
  if (fNcalls%fNmessage == 0 && fPrint>=2)
    printf("In QOCAFit::mrqfuncs() at call %d\n",fNcalls);
  
}
//______________________________________________________________________________
void QOCAFit::SetNumberofruns(Int_t nr)
{
  // Set fNumberofruns to nr, but give warning message if increase beyond current
  // value of fNumberofruns.
  //
  // For negative values, try to get the value from fTree; this should be the
  // total number of runs available.
  //
  // Also allocate parameter storage for the run parameters; since these are at
  // the end of all other parameters of the physical model, this should be the
  // easiest one to de-allocate/re-allocate!

  if (nr<0) {
    if (fTreeSet && fTree) fNumberofruns = (Int_t) fTree->GetEntries();
    else {
      if (fPrint>=1) {
	printf("Warning: use QOCAFit::SetTree() before setting the number of runs.\n");
	printf("Leaving number of runs at %d.\n",fNumberofruns);
      }
    }
  } else {
    if (nr<=fNumberofruns) fNumberofruns = nr;
    else {
      if (fPrint>=1)
	printf("Warning: increasing number of runs to %d from %d.\n",
	       nr,fNumberofruns);
      fNumberofruns = nr;
    }
  }
}
//______________________________________________________________________________
void QOCAFit::BackupPars()
{
  // Make a backup copy of the current parameters

  Int_t i,j;

  fparscopy = vector(1,fNpars);
  fvarycopy = ivector(1,fNpars);
  fcovarcopy = matrix(1,fNpars,1,fNpars);
  falphacopy = matrix(1,fNpars,1,fNpars);
  fnparscopy = fNpars;

  for (i=1; i<=fNpars; i++) {
    fparscopy[i] = fmrqpars[i];
    fvarycopy[i] = fmrqvary[i];
    for (j=1; j<=fNpars; j++) {
      fcovarcopy[i][j] = fmrqcovar[i][j];
      falphacopy[i][j] = fmrqalpha[i][j];
    }
  }
}
//______________________________________________________________________________
void QOCAFit::RestorePars(Int_t mid1,Int_t newmid2,Int_t oldmid2,
			  Float_t newval, Int_t newvary)
{
  // Restore parameters from backup copies to fmrqpars[] and friends, over the
  // range 1..mid1-1 and then mid2..fNpars.
  // Puts the value to newval and the variable flag to newvary for parameters
  // in the middle range (mid1..mid2-1).

  Int_t i,j,m,n;

  for (i=1; i<mid1; i++) {
    fmrqpars[i] = fparscopy[i];
    fmrqvary[i] = fvarycopy[i];
    for (j=1; j<mid1; j++) {
      fmrqcovar[i][j] = fcovarcopy[i][j];
      fmrqalpha[i][j] = falphacopy[i][j];
    }
  }

  for (i=mid1; i<newmid2; i++) {
    fmrqpars[i] = newval;
    fmrqvary[i] = newvary;
    for (j=mid1; j<newmid2; j++) {
      fmrqcovar[i][j] = (i==j);
      fmrqalpha[i][j] = (i==j);
    }
  }
  
  for (i=newmid2, m=oldmid2; i<=fNpars; i++, m++) {
    fmrqpars[i] = fparscopy[m];
    fmrqvary[i] = fvarycopy[m];
    for (j=newmid2, n=oldmid2; i<=fNpars; i++, n++) {
      fmrqcovar[i][j] = fcovarcopy[m][n];
      fmrqalpha[i][j] = falphacopy[m][n];
    }
  }

  // De-allocate memory used in copying
  free_vector(fparscopy,1,fnparscopy);
  free_ivector(fvarycopy,1,fnparscopy);
  free_matrix(fcovarcopy,1,fnparscopy,1,fnparscopy);
  free_matrix(falphacopy,1,fnparscopy,1,fnparscopy);
}
//______________________________________________________________________________
void QOCAFit::SetNbinstheta(Int_t nbt)
{
  // Set fNbinstheta.
  // Laserball parameters are all preserved except laserball distribution.
  // Laserball distribution is NOT reset to initialized values.
  //
  // Is the covariance matrix really worth preserving?

  BackupPars();  // Make copy of parameters

  Int_t mid1  = Getlbdistpari();
  Int_t oldmid2  = Getlbdistpari() + fNbinstheta*fNbinsphi;

  fNbinstheta = nbt;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getlbdistpari() + fNbinstheta*fNbinsphi;

  RestorePars(mid1,newmid2,oldmid2,1.0);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::SetNbinsthetawave(Int_t nbt)
{
  // Set fNbinsthetawave to make sure the sinusoidal parameters are also
  // reallocated.
  // Laserball parameters are all preserved except laserball distribution.
  // Laserball distribution is NOT reset to initialized values.

  BackupPars();  // Make copy of parameters

  Int_t mid1 = Getlbdistwavepari();
  Int_t oldmid2 = Getlbdistwavepari() + fNbinsthetawave*fNdistwave;

  fNbinsthetawave = nbt;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getlbdistwavepari() + fNbinsthetawave*fNdistwave;

  RestorePars(mid1,newmid2,oldmid2,0.01);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::SetNbinsphi(Int_t nbp)
{
  // Set fNbinsphi.
  // Laserball parameters are all preserved except laserball distribution.
  // Laserball distribution is NOT reset to initialized values.
  //
  // Is the covariance matrix really worth preserving?

  BackupPars();  // Make copy of parameters

  Int_t mid1  = Getlbdistpari();
  Int_t oldmid2  = Getlbdistpari() + fNbinstheta*fNbinsphi;

  fNbinsphi = nbp;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getlbdistpari() + fNbinstheta*fNbinsphi;

  RestorePars(mid1,newmid2,oldmid2,1.0);

  fValidPars = kTRUE;
}

//______________________________________________________________________________
void QOCAFit::SetNlbmask(Int_t nlbm)
{
  // Set fNlbmask.
  // Laserball parameters are all preserved except laserball mask coefficients.
  // Laserball mask coefficients are NOT reset to initialized values.
  //
  // Is the covariance matrix really worth preserving?

  BackupPars();  // Make copy of parameters

  Int_t mid1  = Getlbmaskpari();
  Int_t oldmid2  = Getlbmaskpari() + fNlbmask;

  fNlbmask = nlbm;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getlbmaskpari() + fNlbmask;

  RestorePars(mid1,newmid2,oldmid2,0.0);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::SetNdistwave(Int_t nbp)
{
  // Set fNdistwave.
  // Laserball parameters are all preserved except laserball distribution.
  // Laserball distribution is NOT reset to initialized values.
  //
  // Is the covariance matrix really worth preserving?

  BackupPars();  // Make copy of parameters

  Int_t mid1  = Getlbdistwavepari();
  Int_t oldmid2  = Getlbdistwavepari() + fNbinsthetawave*fNdistwave;

  fNdistwave = nbp;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getlbdistwavepari() + fNbinsthetawave*fNdistwave;

  RestorePars(mid1,newmid2,oldmid2,0.01);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::SetLBdistType(Int_t disttype)
{
  // Set the laserball distribution type.
  // 0: binned (default) - fNbinstheta*fNbinsphi
  // 1: sine - fNbinsthetawave*fNdistwave

  if ((disttype<0) || (disttype>1)) {
    printf("Invalid LB dist type %d - leaving it as %d\n",disttype,fLBdistType);
  } else {
    fLBdistType = disttype;
    printf("fLBdistType %d - ",disttype);
    if (disttype == 0) {
      printf("Histogram.\n");
      SetLBDistVary(kTRUE); SetLBDistWaveVary(kFALSE);
    }
    else if (disttype == 1) {
      printf("Sinusoidal.\n");
      SetLBDistVary(kFALSE); SetLBDistWaveVary(kTRUE);
    }
    else printf("Unknown.\n");
  }
}
//______________________________________________________________________________
void QOCAFit::SetNangresp(Int_t nar)
{
  // Set fNangresp.
  // Laserball parameters are all preserved except PMT angular response.
  // PMTR parameters are NOT reset to initialized values.
  //
  // Is the covariance matrix really worth preserving?

  BackupPars();  // Make copy of parameters

  Int_t mid1  = Getangresppari();
  Int_t oldmid2  = Getangresppari() + fNangresp;

  fNangresp = nar;  // Make new parameters with new constants fNxxx
  AllocateParameters();

  Int_t newmid2 = Getangresppari() + fNangresp;

  RestorePars(mid1,newmid2,oldmid2,1.0);

  fValidPars = kTRUE;
}
//______________________________________________________________________________
void QOCAFit::SetAngrespFuncType(Int_t functype)
{
  // Set the PMT Angular response function type.
  // 0 = interpolated (default): value is interpolated between fNangresp bins
  // 1 = binned: value is for bin low edge
  // 2 = binned: value is for bin centre

  if ((functype<0) || (functype>2)) {
    printf("Invalid AngrespFuncType %d - leaving it as %d\n",functype,
	   fAngrespFuncType);
  } else {
    fAngrespFuncType = functype;
    printf("AngrespFuncType %d - ",functype);
    if (functype == 0) printf("Interpolated.\n");
    else if (functype == 1) printf("Binned, low edge.\n");
    else if (functype == 2) printf("Binned, centre.\n");
    else printf("Unknown.\n");
  }
}
//______________________________________________________________________________
void QOCAFit::SetNpmts(Int_t n)
{
  // Set the number of pmts manually.

  if (n>0 && n<fNdata) fNpmts = n;
  else fprintf(stderr,"Invalid number of PMT's %d for %d data - leaving at %d.\n",
	       n,fNdata,fNpmts);
}
//______________________________________________________________________________
Int_t QOCAFit::Getlbmaskpari()
{
  // Switch lbmask and angresp order in version 6.
  // Earlier versions had the attenuations (1,2,3), then PMTR(4 to 4+fNangresp),
  // then LBmask (4+fNangresp to 4+fNangresp+fNlbmask).
  // In version 6, the LBmask and PMTR are interchanged
  // 9-Mar-2001 - Moffat
  //
  // 07.2006 - O.Simard
  // Version 16: Added Rayleigh Scattering attenuations at position 4,5,6.

  if (fVersion < 6) return 4+fNangresp;
  else if(fVersion < 16) return 4;
  else return 7;
}
//______________________________________________________________________________
Int_t QOCAFit::Getlbdistwavepari()
{
  // 07.2006 - O.Simard
  // Switch lbdistwave and angresp order in version 16 because the sinusoidal
  // lbdist has a fixed number of parameters and must preceed the parameters 
  // with variable dimensions.

  return 7+fNlbmask;
}
//______________________________________________________________________________
Int_t QOCAFit::Getangresppari()
{
  // Switch lbmask and angresp order in version 6.
  // Earlier versions had the attenuations (1,2,3) then PMTR(4 to 4+fNangresp).
  // In version 6, the LBmask and PMTR are interchaned.
  // 9-Mar-2001 - Moffat
  //
  // 07.2006 - O.Simard
  // Version 16: Added Rayleigh Scattering attenuations at position 4,5,6.
  //             and interchanged the LB dist with PMTR.

  if (fVersion < 6) return 4;
  else if(fVersion < 16) return 4+fNlbmask;
  else return 7+fNlbmask+fNbinsthetawave*fNdistwave;
}
//______________________________________________________________________________
Int_t QOCAFit::Getangresp2pari()
{
  // Added in version 16. Returns Getangresppari() if old versions.

  if(fVersion < 16) return Getangresppari();
  else return 7+fNlbmask+fNbinsthetawave*fNdistwave+fNangresp;
}
//______________________________________________________________________________
Int_t QOCAFit::Getlbdistpari()
{
  // 07.2006 - O.Simard
  // Follows the pmt angular response(s).

  if (fVersion < 16) return 4+fNangresp+fNlbmask;
  else return 7+fNlbmask+fNbinsthetawave*fNdistwave+2*fNangresp;
}
//______________________________________________________________________________
Int_t QOCAFit::Getnormpari()
{
  // 07.2006 - O.Simard
  // Last in structure.

  if (fVersion < 16) return 4+fNangresp+fNlbmask+fNbinstheta*fNbinsphi;
  else return 7+fNlbmask+fNbinsthetawave*fNdistwave+2*fNangresp+fNbinstheta*fNbinsphi;
}
//______________________________________________________________________________
Float_t QOCAFit::GetParError(Int_t n)
{
  // Return the square root of the diagonal element corresponding to parameter
  // n.  If n is outside the parameter number range, return 0.0.

  Float_t covar;
  if (n>0 && n<fNpars) {
    covar = fmrqcovar[n][n];
    if (covar >= 0) return sqrt(covar);
    else {
      printf("Covariance element %d is <0!\n",n);
      return 0;
    }
  } else {
    printf("QOCAFit::GetParError(%d) is out of range [1:%d]\n",n,fNpars);
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getd2oerr()
{
  // Return the square root of the diagonal element corresponding to alpha_d2o.

  Float_t covar = fmrqcovar[Getd2opari()][Getd2opari()];
  if (covar >= 0) return sqrt(covar);
  else {
    printf("Covariance element for D2O is <0!\n");
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Geth2oerr()
{
  // Return the square root of the diagonal element corresponding to alpha_h2o.

  Float_t covar = fmrqcovar[Geth2opari()][Geth2opari()];
  if (covar >= 0) return sqrt(covar);
  else {
    printf("Covariance element for H2O is <0!\n");
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getacrylicerr()
{
  // Return the square root of the diagonal element corresponding to alpha_acrylic.

  Float_t covar = fmrqcovar[Getacrylicpari()][Getacrylicpari()];
  if (covar >= 0) return sqrt(covar);
  else {
    printf("Covariance element for acrylic is <0!\n");
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getangresperr(Int_t n)
{
  // Return the square root of the diagonal element corresponding to the PMT
  // response bin parameter n.
  
  if (n>=0 && n<fNangresp) {
    Float_t covar = fmrqcovar[Getangresppari()+n][Getangresppari()+n];
    if (covar >= 0) return sqrt(covar);
    else {
      printf("Covariance element for group1 angresp <0!\n");
      return 0;
    }
  } else {
    printf("QOCAFit::Getangresperr(%d) is out of range [0:%d]\n",n,fNangresp-1);
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getangresp2err(Int_t n)
{
  // Return the square root of the diagonal element corresponding to the PMT
  // response bin parameter n.
  
  if (n>=0 && n<fNangresp) {
    Float_t covar = fmrqcovar[Getangresp2pari()+n][Getangresp2pari()+n];
    if (covar >= 0) return sqrt(covar);
    else {
      printf("Covariance element for group2 angresp <0!\n");
      return 0;
    }
  } else {
    printf("QOCAFit::Getangresp2err(%d) is out of range [0:%d]\n",n,fNangresp-1);
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getlbmaskerr(Int_t n)
{
  // Return the square root of the diagonal element corresponding to the
  // laserball mask parameter n.
  
  if (n>=0 && n<fNlbmask) {
    Float_t covar = fmrqcovar[Getlbmaskpari()+n][Getlbmaskpari()+n];
    if (covar >= 0) return sqrt(covar);
    else {
      printf("Covariance element for lbmask <0!\n");
      return 0;
    }
  } else {
    printf("QOCAFit::Getlbmaskerr(%d) is out of range [0:%d]\n",n,fNlbmask-1);
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getlbdisterr(Int_t n)
{
  // Return the square root of the diagonal element corresponding to the
  // laserball distribution element n.
  
  if(!fLBdistType){
    if (n>=0 && n<fNbinstheta*fNbinsphi) {
      Float_t covar = fmrqcovar[Getlbdistpari()+n][Getlbdistpari()+n];
      if (covar >= 0) return sqrt(covar);
      else {
	printf("Covariance element for lbdist <0!\n");
	return 0;
      }
    } else {
      printf("QOCAFit::Getlbdisterr(%d) is out of range [0:%d]\n",n,
	     fNbinstheta*fNbinsphi-1);
      return 0;
    }
  } else { // sinusoidal
    if (n>=0 && n<fNbinsthetawave*fNdistwave) {
      Float_t covar = fmrqcovar[Getlbdistwavepari()+n][Getlbdistwavepari()+n];
      if (covar >= 0) return sqrt(covar);
      else {
	printf("Covariance element for lbdistwave <0!\n");
	return 0;
      }
    } else {
      printf("QOCAFit::Getlbdisterr(%d) is out of range [0:%d]\n",n,
	     fNbinsthetawave*fNdistwave-1);
      return 0;
    }
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getnormerr(Int_t n)
{
  // Return the square root of the diagonal element corresponding to the
  // run normalization parameter n.
  
  if (n>=0 && n<fNumberofruns) {
    Float_t covar = fmrqcovar[Getnormpari()+n][Getnormpari()+n];
    if (covar >= 0) return sqrt(covar);
    else {
      printf("Covariance element for norm <0!\n");
      return 0;
    }
  } else {
    printf("QOCAFit::Getnormerr(%d) is out of range [0:%d]\n",n,fNumberofruns-1);
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getrsd2oerr()
{
  // Return the square root of the diagonal element corresponding to alpha_rsd2o.

  Float_t covar = fmrqcovar[Getrsd2opari()][Getrsd2opari()];
  if (covar >= 0) return sqrt(covar);
  else {
    printf("Covariance element for Rayleigh scattering D2O is <0!\n");
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getrsh2oerr()
{
  // Return the square root of the diagonal element corresponding to alpha_rsh2o.

  Float_t covar = fmrqcovar[Getrsh2opari()][Getrsh2opari()];
  if (covar >= 0) return sqrt(covar);
  else {
    printf("Covariance element for Rayleigh scattering H2O is <0!\n");
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::Getrsacrylicerr()
{
  // Return the square root of the diagonal element corresponding to alpha_rsacrylic.

  Float_t covar = fmrqcovar[Getrsacrylicpari()][Getrsacrylicpari()];
  if (covar >= 0) return sqrt(covar);
  else {
    printf("Covariance element for Rayleigh scattering acrylic is <0!\n");
    return 0;
  }
}
//______________________________________________________________________________
Float_t QOCAFit::GetOccVariable()
{
  // Returns occupancy or occratio for the current pmt
  // according to fOccType
  
  if(fOccType == 1) return fCurrentpmt->GetOccratio();
  else return fCurrentpmt->GetOccupancy();
  
}
//______________________________________________________________________________
Float_t QOCAFit::GetOccVariableErr()
{
  // Returns the occupancy or occratio error for the current pmt
  // according to fOccType
  
  if(fOccType == 1) return fCurrentpmt->GetOccratioerr();
  else return fCurrentpmt->GetOccupancyerr();
  
}
//______________________________________________________________________________
void QOCAFit::SetParameter(Int_t n,Float_t value)
{
  // Set parameter number n to value.
  // n is specified in zero-offset array index, and converted internally to
  // 1..n format used by numerical recipes.

  if ((n>=1) && (n<=fNpars)) fmrqpars[n] = value;
}
//______________________________________________________________________________
void QOCAFit::Setangresp(Float_t *ar)
{
  // Set the array of angular response parameters.
  // ar[0..fNangresp-1] are stored in fmrqpars[4..4+fNangresp-1].

  Int_t i;
  Float_t *angresp = Getangresp();
  for (i=0; i<fNangresp; i++) angresp[i] = ar[i];
}
//______________________________________________________________________________
void QOCAFit::Setangresp2(Float_t *ar)
{
  // Set the array of group2 angular response parameters.
  // ar[0..fNangresp-1] are stored in fmrqpars[4..4+fNangresp-1].

  Int_t i;
  Float_t *angresp = Getangresp2();
  for (i=0; i<fNangresp; i++) angresp[i] = ar[i];
}
//______________________________________________________________________________
void QOCAFit::Setlbmask(Float_t *lbm)
{
  // Set the array of laserball mask parameters.
  // lbm[0..fNlbmask-1] are stored in fmrqpars[4+fNangresp..4+fNangresp+fNlbmask-1]

  Int_t i;
  Float_t *lbmask = Getlbmask();
  for (i=0; i<fNlbmask; i++) lbmask[i] = lbm[i];
}
//______________________________________________________________________________
void QOCAFit::Setlbdist(Float_t *lbd)
{
  // Set the array of laserball distribution parameters.
  // lbd[0..fNlbdist-1] are stored in fmrqpars[4+fNangresp+fNlbmask.....]

  Int_t i;
  Float_t *lbdist = Getlbdist();
  for (i=0; i<fNbinstheta*fNbinsphi; i++) lbdist[i] = lbd[i];
}
//______________________________________________________________________________
void QOCAFit::Setlbdistwave(Float_t *lbd)
{
  // Set the array of laserball distribution parameters.
  // lbd[0..fNlbdist-1] are stored in fmrqpars[4+fNangresp+fNlbmask.....]

  Int_t i;
  Float_t *lbdist = Getlbdistwave();
  for (i=0; i<fNbinsthetawave*fNdistwave; i++) lbdist[i] = lbd[i];
}
//______________________________________________________________________________
void QOCAFit::Setlbamplitudes(Float_t *lbd)
{
  // Set the array of laserball distribution amplitudes. Only need the theta bin.
  // The ordering is:
  //
  // lbdist[0] = first amplitude
  // lbdist[1] = first phase;
  // ...
  // lbdist[itheta*fNbinsthetawave+i]
  // ...
  // lbdist[fNbinsthetawave*fNdistwave-2] = last aplitude
  // lbdist[fNbinsthetawave*fNdistwave-1] = last offset

  Float_t *lbdist = Getlbdistwave();
  for(Int_t i=0; i<fNbinsthetawave*fNdistwave; i+=fNdistwave) lbdist[i] = lbd[i];
}
//______________________________________________________________________________
void QOCAFit::Setlbphases(Float_t *lbd)
{
  // Set the array of laserball distribution amplitudes. Only need the theta bin.
  // The ordering is:
  //
  // lbdist[0] = first amplitude
  // lbdist[1] = first phase;
  // ...
  // lbdist[itheta*fNbinsthetawave+i]
  // ...
  // lbdist[fNbinsthetawave*fNdistwave-2] = last aplitude
  // lbdist[fNbinsthetawave*fNdistwave-1] = last offset

  Float_t *lbdist = Getlbdistwave();
  for(Int_t i=1; i<fNbinsthetawave*fNdistwave; i+=fNdistwave) lbdist[i] = lbd[i];
}
//______________________________________________________________________________
void QOCAFit::Setnorm(Float_t *n)
{
  // Set the array of run normalization parameters.
  // n[0..fNumberofruns-1] are stored in fmrqpars[4+fNangresp+fNlbmask+
  // fNbinstheta*fNbinsphi....]

  Int_t i;
  Float_t *norm = Getnorm();
  for (i=0; i<fNumberofruns; i++) norm[i] = n[i];
}
//______________________________________________________________________________
void QOCAFit::PrintParameter(Int_t parindex)
{
  // Internal function to limit debugging problems.
  // --
  // Prints the parameter according to the index parindex.
  // It prints the value with its covariance diagonal element (error)
  // and tries to output the parameter class (att, pmtr, lbdist, etc.).
  
  if(fVersion < 16) Warning("PrintParameter","Function optimized for version 16 and higher."); 

  if((parindex < 1) || (parindex > fNpars+1)) {
    Warning("PrintParameter","Invalid parameter index %d",parindex);
    return;
  }

  Float_t par  = fmrqpars[parindex];
  Float_t err2 = fmrqcovar[parindex][parindex];
  Bool_t quit = kFALSE;

  if(!(par > 0 || par <= 0)){
    Warning("PrintParameter","Parameter %d is nan ... data corrupted.",parindex);
    quit = kTRUE;
  }

  if(!(err2 > 0 || err2 <= 0)){
    Warning("PrintParameter","Covariance[%d][%d] is nan ... data corrupted.",parindex,parindex);
    quit = kTRUE;
  }

  if(quit) return; // we have seen enough

  if(err2 >= 0){
    printf("  Parameter %d has value: %.8f +/- %.8f\n", parindex,
	   fmrqpars[parindex], sqrt(fmrqcovar[parindex][parindex]));
  } else {
    printf("  Parameter %d has value: %.8f\n",parindex,fmrqpars[parindex]);
    printf("  Parameter's %d error could not be printed.\n",parindex);
  }

  printf("  Parameter %d was determined to be: ",parindex);
  if(parindex < Getlbmaskpari()) printf("an extinction coefficient.\n");
  if((parindex >= Getlbmaskpari()) && (parindex < Getlbdistwavepari()))
    printf("a laserball mask polynomial coefficient, precisely the %d th coefficient.\n",
	   parindex-Getlbmaskpari());
  if((parindex >= Getlbdistwavepari()) && (parindex < Getangresppari())){
    printf("a laserball distribution (sinusoidal) parameter, precisely \n");
    if(parindex%2 == 0) printf("the %d th amplitude coefficient.\n",parindex-Getlbdistpari());
    else printf("the %d th phase.\n",parindex-Getlbdistpari());
  }
  if((parindex >= Getangresppari()) && (parindex < Getangresp2pari())){
    printf("a group1 angular response parameter, precisely the %d th bin.\n",
	   parindex-Getangresppari()); 
  }
  if((parindex >= Getangresp2pari()) && (parindex < Getlbdistpari())){
    printf("a group2 angular response parameter, precisely the %d th bin.\n",
	   parindex-Getangresp2pari()); 
  }
  if((parindex >= Getlbdistpari()) && (parindex < Getnormpari())){
    printf("a laserball distribution (histogram) parameter, precisely the %d th coefficient.\n",
	   parindex-Getlbdistpari());
  }
  if(parindex >= Getnormpari()){
    printf("a normalization factor, precisely the %d th run.\n",
	   parindex-Getnormpari());
  }

  return;
}
//______________________________________________________________________________
void QOCAFit::WriteFitToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );;

  TTree* runTree = new TTree( "QOCAFitT", "QOCAFit" );

  TH2F* lbDistribution = ApplyLaserdist();
  file->WriteTObject( lbDistribution );

  TH1F* angularResponse = AngRespTH1F();
  file->WriteTObject( angularResponse );

  TF1* angularResponseTF1 = AngRespTF1();
  file->WriteTObject( angularResponseTF1 );

  TF1* lbDistributionTF1 = LBMaskTF1();
  file->WriteTObject( lbDistributionTF1 );

  file->Close();
  delete file;

}
//______________________________________________________________________________
void QOCAFit::PrintCovarianceMatrix()
{

  // Print only the parameters which are variable
  for ( Int_t iPar = 1; iPar <= fNpars; iPar++ ){
    if ( fmrqvary[ iPar ] ){
      printf("Covariance matrix element [ %i ][ %i ]: %.20f\n", iPar, iPar, fmrqcovar[ iPar ][ iPar ] );
    }
  }

}
//______________________________________________________________________________
void QOCAFit::Streamer(TBuffer &R__b)
{
   // Stream an object of class QOCAFit.
  //
  // Updated 29-Jun-2001 for ROOT 3.00 "Automatic Schema Evolution" - BA Moffat

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if (R__v > 7) {
      QOCAFit::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      if (R__v > 8) {
      	AllocateParameters();
	fmrqx = vector(1,fNdata); // Allocate storage here
	fmrqy = vector(1,fNdata);
	fmrqsig = vector(1,fNdata);
        R__b.ReadStaticArray(fmrqx+1);
        R__b.ReadStaticArray(fmrqy+1);
        R__b.ReadStaticArray(fmrqsig+1);
        R__b.ReadStaticArray(fmrqpars+1);
        R__b.ReadStaticArray(fmrqvary+1);
        R__b.ReadStaticArray(fmrqcovar[1]+1);
        R__b.ReadStaticArray(fmrqalpha[1]+1);
      }
      return;
    }
    //====process old versions before automatic schema evolution
    fVersion = R__v;  // Remember fVersion read for GetXXXpari() calls - CAREFUL!
    printf("QOCAFit version %d read.\n",fVersion); // or R__v);
    TNamed::Streamer(R__b);
    QMath::Streamer(R__b);

    R__b >> fValidPars;

    R__b >> fNumberofruns;
    R__b >> fNbinstheta;
    R__b >> fNbinsphi;
    R__b >> fNlbmask;
    R__b >> fNangresp;
    R__b >> fNpars;

    AllocateParameters();  // Allocate parameter storage

    fLaserdist->Streamer(R__b);
    
    R__b.ReadStaticArray((Float_t *) fmrqpars+1);  // +1 offset for Numerical Recipes offset
    R__b.ReadStaticArray((Float_t *) fmrqvary+1);
    R__b.ReadStaticArray((Float_t *) fmrqcovar[1]+1);
    R__b.ReadStaticArray((Float_t *) fmrqalpha[1]+1);

    R__b >> fChisquare;
    R__b >> fChilimit;
    if (R__v >= 5) { // After version 5, includes chisq limit min and mult for cut
      R__b >> fChilimmin;
      R__b >> fChimult;
      R__b >> fChicuttype;
    }

    if (R__v >=7) { // After version 7, includes setable minimum number of PMT's
      R__b >> fMinPMTperangle;
    } else fMinPMTperangle = 25;

    R__b >> fNcalls;
    R__b >> fNmessage;

    if (R__v >= 2) { // After version 2, includes all fmrq arrays, fChiarray, etc.
      R__b >> fNdata;
      R__b >> fNpmts;

      R__b >> fDataSetup;

      fmrqx = vector(1,fNdata); // Allocate storage here
      fmrqy = vector(1,fNdata);
      fmrqsig = vector(1,fNdata);
      R__b.ReadStaticArray((Float_t *) fmrqx+1);
      R__b.ReadStaticArray((Float_t *) fmrqy+1);
      R__b.ReadStaticArray((Float_t *) fmrqsig+1);

      R__b >> fNelements;
      fResarray = new Float_t[fNelements];
      fChiarray = new Float_t[fNelements];
      R__b.ReadStaticArray(fResarray);
      R__b.ReadStaticArray(fChiarray);
    }

    if (R__v >= 3) { // After version 3, includes binned/interpolated PMT response
      R__b >> fAngrespFuncType;
    }

    if (R__v >= 4) { // After version 4, includes provision for single normalization
      R__b >> fSingleNorm;
    }

    if (R__v >= 6) { // After version 6, include the time window used, nskip, nstart
      R__b >> fTimeWi;
      R__b >> fNSkip;
      R__b >> fNStart;

      R__b >> fModelErrType;
      R__b >> fModelErr;
    }

    if (R__v >= 12) { // After version 12, flag choosing occupancy or occratio
      R__b >> fOccType;
    }
    
    // After version 13, includes setable minimum number of PMT's in LB bins
    // and user occupancy cut settings
    if (R__v >=13) { 
      R__b >> fMinPMTperLBbin;
      R__b >> fDynamicOccupancyCut;
      R__b >> fDynamicSigma;
    } else {
      fMinPMTperLBbin = 10;
      fDynamicOccupancyCut = kFALSE;
      fDynamicSigma = 3.;
    }

    // After version 14, include PMT variability parameters
    if (R__v >=14) { 
      R__b >> fPmtVarPar0;
      R__b >> fPmtVarPar1;
      R__b >> fPmtVarPar2;
    } else {
      fPmtVarPar0 = 0.;
      fPmtVarPar1 = 1.;
      fPmtVarPar2 = 2.;
    }
	
    R__b.CheckByteCount(R__s, R__c, QOCAFit::IsA());
    //====end of old versions
  } else {
    QOCAFit::Class()->WriteBuffer(R__b,this);
    R__b.WriteArray(fmrqx+1, fNdata);
    R__b.WriteArray(fmrqy+1, fNdata);
    R__b.WriteArray(fmrqsig+1, fNdata);
    R__b.WriteArray(fmrqpars+1, fNpars);
    R__b.WriteArray(fmrqvary+1, fNpars);
    R__b.WriteArray(fmrqcovar[1]+1, fNpars * fNpars);
    R__b.WriteArray(fmrqalpha[1]+1, fNpars * fNpars);
  }
}
//______________________________________________________________________________
