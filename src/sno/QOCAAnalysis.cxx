//*-- Author :    Olivier Simard   2004 08
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

#include "QOCAAnalysis.h"

Int_t QOCAAnalysis::fNinstances = 0;
Bool_t QOCAAnalysis::fInit = kFALSE;
//______________________________________________________________________________
ClassImp(QOCAAnalysis)
;
//
// QOCAAnalysis
// ============
//
// Contains specific methods to analyze the optical calibration data
// from .rdt files. It serves as a main class for other sub-classes
// that perform specific analyses.
//
// This class provides functions that were previously written in macros
// to perform that analysis in root. The following format also allows the
// user to run the macros, now functions, in root, but also to compile
// executables that can run faster in automated job submitting script.
//
// The QOCALoad class is used to load options from a cardfile.
// The QOCAAnalysis constructor is setting most of the options needed
// assuming a cardfile readable by QOCALoad is provided (see QOCALoad.cxx).
// If no cardfile is provided, the options can be set manually using the
// setter functions.
//
// In addition, the QOCAAnalysis framework allows two different modes:
//  1) running mode: the functions are old macros or "runners" and are meant
//                   to process the data from .rdt files or QOCATree's saved
//                   in .root files.
//  2) analysis mode: when the data is processed and saved in QOCATree's or
//                    QOCAFit object in .root files, the data is extracted
//                    or plotted using other classes. In this case, the
//                    present class is mainly used to set options and store
//                    arrays.
//
// The cardfiles and flags can be set directly in the constructor.
// The functions are called through an object or pointer:
//
//  [0] Char_t* cardfile = "/home/user/cardfile.txt";
//  [0] TString cardfile = "/home/user/cardfile.txt";
//
//  [1] Bool_t mode = 0; // running mode
//  [1] Bool_t mode = 1; // analysis mode
//
//  [2] Bool_t display = 1; // for messages
//
//  [3] QOCAAnalysis* qoca = new QOCAAnalysis(cardfile, mode, display);
//
//  [4] qoca->Functions(arguments)
//
//______________________________________________________________________________
QOCAAnalysis::QOCAAnalysis(Char_t* optionfile, Bool_t mode, Bool_t display)
{
  // QOCAAnalysis constructor : initializes inputs found in "optionfile"
  // depending on the "mode" the user wants to run.

  // Load data at first time called: must be in same mode!
  if (!fInit){

  SetFast(); // default is to calculate all the systematics (running mode)
  SetMessages(display); // allow display

  if(fDisplay){
    printf("\n\n////////////////////////////////////////////////////////\n");
    printf("//                                                    //\n");
    printf("// QOCAAnalysis                                       //\n");
    printf("//                                                    //\n");
    printf("// Methods to process optical data in a sensible way  //\n");
    printf("//                                                    //\n");
    printf("////////////////////////////////////////////////////////\n\n");
  }

  // Initialize the QOCALoad instance for option loading methods
  fQOCALoad = new QOCALoad();

  // Systematics identifiers and names
  fSyst = 0;          // means not a systematic or nominal fit
  fSystnames = NULL;  // array is empty for now
  fFulloutput = kFALSE; // limit to nominal fit for now
  fCardVersion = 1.; // cardfile were not in CVS before version 2.0

  // Display warning if no cardfile is provided
  if(optionfile == NULL){
    printf("Warning in QOCAAnalysis::QOCAAnalysis(): No option file set.\n");
    printf("The user must set the options manually!\n");
  } else {
    // QOCALoad settings: the cardfile should be in the same directory (PWD)
    // where the class is initialized.
    fQOCALoad->SetFullname(
      fQOCALoad->AppendElement((Char_t*)getenv("PWD"), "/"), optionfile);
    // get the cardfile version
    fQOCALoad->SetKeyword("version");
    Char_t** ver = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    if(ver) fCardVersion = (Float_t) atof(ver[0]);
    else Warning("QOCAAnalysis","No cardfile version was found.\n");
  }

  // -----------------------------------------------------------
  // Check out the mode: default is kFALSE (0) or runner mode.
  if(mode){  // analysis mode
    
    printf("QOCAAnalysis: analysis mode.\n");
    printf("Reading option file:\n\t%s\n\tVersion %.1f\n",optionfile,fCardVersion);
    printf("\n////////////////////////////////////////////////////////\n");

    // -- Wavelengths: the wlens are provided in the cardfile. It can be one
    //    value or all of them up to 6 values in nm: 337,365,386,420,500,620.
    //    The keyword "wlen" must be found in the cardfile otherwise no values
    //    will be loaded. Do not change here.
    fQOCALoad->SetKeyword("wlen");
    Char_t** wlenarr = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    SetNwlen((Int_t)fQOCALoad->GetDimensionFast());
    SetWlens((Float_t*) fQOCALoad->ConverttoFloats(wlenarr, GetNwlen()));

    if(fDisplay){
      printf("-----------------------------\n");
      printf("List of wavelengths loaded: \n");
      fQOCALoad->DisplayMatrixContent(wlenarr);
      printf("-----------------------------\n");
    }

    // delete local wlen array
    for(Int_t i = 0 ; i < GetNwlen() ; i++){
      delete [] wlenarr[i]; // each element
    } delete [] wlenarr; // the array


    // -- Scans and associated SNO Julian Dates: the scan acronyms are found
    //    with time drift flags and dates.
    //    1st column : 1 (fit start/end) or 0 (normal use) for each scan
    //    2nd column : scan name (standard month+year)
    //    3rd column : associated julian date (in days)
    fQOCALoad->SetKeyword("scan");
    Char_t** tmps = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    // extract the number of lines as the number of scans
    SetNscan(fQOCALoad->GetNLines());
    // extract the columns and store scan data locally
    SetTimeLimits((Int_t*)fQOCALoad->ConverttoIntegers(fQOCALoad->GetColumn(tmps, 1), GetNscan()));
    SetScanArray((Char_t**)fQOCALoad->GetColumn(tmps, 2));
    SetJulianDates((Float_t*)fQOCALoad->ConverttoFloats(fQOCALoad->GetColumn(tmps, 3), GetNscan()));

    // set the time limits for time drift fits
    Int_t use_s = 0;
    for(Int_t i = 0 ; i < GetNscan() ; i++){
      if(fLimits[i] == 1){
        use_s++;
	if(use_s == 1){SetFitStart(fDates[i]);}
	else{
	  if(use_s == 2){SetFitEnd(fDates[i]);}
	  else{
	    if(use_s > 2 && fDisplay){
              printf("There is a minor mistake in the scan card file.\n");
              printf("Should only affect time dependencies analyses.\n");
	    }
	  }
	}
      }
    }

    if(fDisplay){
      printf("-----------------------\n");
      printf("List of scans loaded : \n");
      fQOCALoad->DisplayMatrixContent(tmps);
      printf("-----------------------\n");
    }

    // Note: do not delete local scan array because
    // its values are passed to local arrays (SetScanArray() functions)

    // Load systematic error factors and names
    fQOCALoad->SetKeyword("syst");
    Char_t** tmpe = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t ntotal = fQOCALoad->GetNLines();
    // usage (flag 0: don't use, 1: use, 2: main)
    // where 'main' is usually the nominal fit and 0,1 are
    // reserved for systematic errors
    Int_t* flags =
    fQOCALoad->ConverttoIntegers(fQOCALoad->GetColumn(tmpe, 1), ntotal);
    // multiplicative factors for each systematics
    // (determined by Bryce Moffat) 
    // the factor of the 'main' entry is read as the fFulloutput flag
    // and then set to zero.
    Float_t* fact =
    fQOCALoad->ConverttoFloats(fQOCALoad->GetColumn(tmpe, 2), ntotal);
    // the name of the qocafit objects (fit parameters) and 
    // systematics in the QOCAFit structure
    Char_t** systnames = fQOCALoad->GetColumn(tmpe, 3);

    // scan the error list to find which one to use (flag 0, 1, or 2)
    Int_t use = 0; // not used in total error calculation
    Int_t mainq = 0, mainqindex = 0; // nominal fit
    for(Int_t isyst = 0 ; isyst < ntotal ; isyst++){
      if(flags[isyst] == 1) use++;
      else {
	if(flags[isyst] == 2){
	  // use first 'main' if more than one
          if(mainq > 0){
            flags[isyst] = 1; // reset to systematic type
            use++; // use it
          } else {
	    fFulloutput = (Bool_t) fact[isyst]; // store this flag
            fact[isyst] = 0.; // set it to zero after
	    mainqindex = isyst; // index in array
	    mainq++;
          }
        }
      }
      fact[isyst] *= (Float_t) flags[isyst]; // multiply by 1 or 0
    }
    // the number of systematics includes the 'main'
    SetNsyst(use+mainq);
    // declare new arrays (smaller if some systematics are not used)
    Int_t* usedflag = new Int_t[fNsyst];
    Float_t* usedfact = new Float_t[fNsyst];
    Char_t** usedname = new Char_t*[fNsyst];
    // set 'main' to index 0 - mandatory for other classes
    usedflag[0] = flags[mainqindex];
    usedfact[0] = fact[mainqindex];
    usedname[0] = systnames[mainqindex];
    // compare explicitely the 'main' name to what is used for nominal
    if(!strcmp(usedname[0],"qocafit_nominal")) fIsSyst = kFALSE;
    else {
      printf("Systematic control: will treat systematic %s as nominal for now.\n",usedname[0]);
      fIsSyst = kTRUE;
    }
    // print statement about the output mode
    if(fFulloutput) printf("User chose to produce files for all systematics also.\n");
    
    Int_t iuse = 1; // index of used trees
    printf("---------------------------------------\n");
    printf("Systematic factors and names loaded : \n");
    for(Int_t isyst = 0 ; isyst < ntotal ; isyst++){
      if(isyst == mainqindex) continue;
      if(flags[isyst] > 0){
	// assign to shorter arrays
	usedflag[iuse] = flags[isyst];
	usedfact[iuse] = fact[isyst];
	usedname[iuse] = systnames[isyst];
	iuse++;
      }
    }
    for(Int_t isyst = 0 ; isyst < fNsyst ; isyst++){
      printf("%d %.2f %s\n",usedflag[isyst],usedfact[isyst],usedname[isyst]);
    }
    printf("Using %d tree and %d systematics out of %d total.\n",
           mainq,use,ntotal-mainq);
    printf("---------------------------------------\n");
    // set the array in class consistent with fNsyst
    SetSystFactors(usedfact);
    SetSystNames(usedname);
    delete [] flags;
    delete [] fact;
    delete [] systnames;
    delete [] usedflag;

  } //if(mode)
  else{ // runner mode

    printf("QOCAAnalysis: runner mode.\n");
    printf("Reading option file:\n\t%s\n\tVersion %.1f\n",optionfile,fCardVersion);
    printf("\n////////////////////////////////////////////////////////\n");
    
  }

  // Reset the input/output private fields to be set/get using this class'
  // functions. See QOCAAnalysis.h for a description of the fields.
  ResetIO();

  fInit = kTRUE;
  }

  fNinstances++;  // Increment number of instances
  printf("Number of QOCAAnalysis instances: %d\n",fNinstances);

  // new members
  falpha_acrylic = 0.0;
  fin = NULL;
  fout = NULL;
}

//______________________________________________________________________________
QOCAAnalysis::~QOCAAnalysis()
{
  // QOCAAnalysis destructor : deletes what was initialized with the new
  // operator. The Char_t** arrays created by QOCALoad are all created with
  // new so they can be deleted here if they were not in the constructor.
  // The arrays do not use much memory anyway.
  // --------
  // It is the user's responsability to delete the instances in the reverse order
  // they were initialized otherwise the delete operator will produce
  // segmentation faults, as in the example below:
  //
  // root [0] QOCAAnalysis* q1 = new QOCAAnalysis();
  // Number of QOCAAnalysis instances: 1
  // root [1] QOCAAnalysis* q2 = new QOCAAnalysis();
  // Number of QOCAAnalysis instances: 2
  // root [2] delete q1;
  // Number of QOCAAnalysis instances: 1
  // root [3] delete q2;
  // Number of QOCAAnalysis instances: 0
  // Segmentation fault
  //

  fNinstances--;  // Decrement number of instances
  printf("Number of QOCAAnalysis instances: %d\n",fNinstances);
  if(fNinstances == 0){
    if(fQOCALoad) delete fQOCALoad;
    ClearArrays();
    fInit = kFALSE;
    printf("QOCAAnalysis: Done.\n");
  }

  // deal with TFile's
  if(fin){
    if(fin->IsOpen()) fin->Close(); 
    delete fin; fin = NULL;
  }
  
  if(fout){
    if(fout->IsOpen()) fout->Close(); 
    delete fout; fout = NULL;
  }

}

//______________________________________________________________________________
void QOCAAnalysis::ResetIO()
{
  // Resets the Input/Ouput directories, labels and formats:
  // Directories: working directories where files are read(I) or written(O).
  // Labels: used to build file names.
  // Format: extension to the file (ex: .root, .txt, .dat, etc.)

  SetIDir(); SetODir();
  SetILabel(); SetOLabel();
  SetIFormat(); SetOFormat();

  return;
}

//______________________________________________________________________________
void QOCAAnalysis::ClearArrays()
{
  // Delete arrays allocated with QOCALoad and other character strings arrays

  if(GetNscan() != 0){
    for(Int_t i = 0 ; i < GetNscan() ; i++){
      delete [] fScans[i];
    }
    delete [] fScans;
  } 
  if(fWlens) delete [] fWlens;

  return;
}

//______________________________________________________________________________
Char_t* QOCAAnalysis::GetSystOFormat(Int_t isyst,Char_t* oldformat)
{
  // returns a modified output format in case of systematics
  
  Char_t* newoformat = fQOCALoad->AppendElement(
                       fQOCALoad->AppendElement("_",GetSystName(isyst)),oldformat);
  return newoformat;
}

//______________________________________________________________________________
void QOCAAnalysis::PathSystematics(QOCATree* pos, QPath* qp, 
				   Float_t rsyst, Int_t psyst, Char_t** names)
{

  // Function that performs the systematic calculations in QPath provided
  // the options in a cardfile. The options are passed:
  //  rsyst: factor provided
  //  psyst: code for operation (addition, multiplication, smear, etc.)
  //  names: systematic object names modified from original ones.
  //
  // Used in QOCAAnalysis::RunQPath()
  // Assumes qp->SetFitLBPosition(lb) has been set before
  //

  if(!fout->IsOpen()){
    Warning("PathSystematics","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::PathSystematics(): \n");
  
  // First reset all the systematics values to zero
  qp->SetAllsyst(0.);
 
  // Then decide in which direction to apply the systematics:
  //  R(radial), X(horizontal) and Z(vertical).
  // search for those letters in the titles (names[3])
  Char_t* title = names[3];
  if(strstr(title, "R")){
    printf("* This systematic error is a radial (R) "); // sentence to be continued
    qp->SetRsyst(rsyst);
  }
  else{
    if(strstr(title, "Z")){
      printf("* This systematic error is a vertical (Z) ");
      qp->SetZsyst(rsyst);
    }
    else{
      if(strstr(title, "X")){
	printf("* This systematic error is a horizontal (X) ");
	qp->SetXsyst(rsyst);
      }
      else{
	if(strstr(title, "D")){
	  printf("* This systematic error is a laserball surface ");
	  qp->SetBallsyst(rsyst);
	}
	else{
	  fprintf(stderr,"Systematics direction choice failed. (QOCAAnalysis::PathSystematics())\n"); exit(-1);
	}
      }
    }
  }

  // Finally decide which kind :
  //  psyst: 0=none, -1=smear, +1=shift, +2=multiply

  if(psyst == 1){printf("shift ");}
  else{if(psyst == 2){printf("factor ");}
    else{if(psyst == -1){printf("smear ");}
      else{if(psyst == 0){printf("copy (doing nothing) ");}
	else{
	  fprintf(stderr,"error because of a wrong input. (QOCAAnalysis::PathSystematics())\n"); exit(-1);
	}
      }
    }
  }
 
  printf("of the original position.\n");
  qp->SetPositionsyst(psyst);

  fout->cd();

  // QPath's main functions to produce QOCATree's
  QOCATree* path = (QOCATree*) qp->CalculatePaths(pos, names[1], names[3]);
  QOCATree* oca = (QOCATree*) qp->CalculateOccratio(path, names[2], names[3]);
  
  fout->RecursiveRemove(path); // gets rid of TFile ownership
  delete path; // remove from file and memory

  // write out objects and delete from TFile buffer
  fout->cd(); oca->Write(); 
  printf("* QOCATree %s has been written.\n",oca->GetName());
  fout->Delete(oca->GetName());

  return;
}

//______________________________________________________________________________
void QOCAAnalysis::ModelAppliedQOCATree(QOCAFit* ocafit, Char_t* name)
{
  // Produce a model-applied QOCATree and save it to the output TFile.
  // After being written the QOCATree object is removed from the
  // TFile object list, to spare the memory.

  if(!fout->IsOpen()){
    Warning("ModelAppliedQOCATree","Output TFile is not opened ... Aborting.");
    exit(-1);
  } 

  if(ocafit){
    QOCATree* modeltree = ocafit->Apply();
    fout->cd(); modeltree->Write(name);
    fout->Delete(modeltree->GetName()); // remove from memory
  }
  return;
}
//______________________________________________________________________________
void QOCAAnalysis::ModelAppliedLBDist(QOCAFit* ocafit)
{
  // Produce a model-applied LB distribution and save it to the output TFile.
  // After being written the TH2F object cannot be removed from the
  // TFile object list because it is a member of QOCAFit.

  if(!fout->IsOpen()){
    Warning("ModelAppliedLBDist","Output TFile is not opened ... Aborting.");
    exit(-1);
  }

  if(ocafit){
    TH2F* lbdist = ocafit->ApplyLaserdist();
    fout->cd(); lbdist->Write();
  }
  return;
}

//______________________________________________________________________________
Float_t* QOCAAnalysis::SaveParameters(QOCAFit* ocafit)
{
  // Function called to return (save) the fit parameters. The parameters
  // are stored in an array for further use.
  // Used in QOCAAnalysis::RunQOCAFit()

  
  Int_t npar = ocafit->GetNpars();
  printf("\n* In QOCAAnalysis::SaveParameters(): Saving %d parameters.\n",npar);
  Float_t* pars = new Float_t[npar];
  for(Int_t ipar = 0 ; ipar < npar ; ipar++){
    pars[ipar] = ocafit->GetParameter(ipar+1);
  }

  return pars;
}


//______________________________________________________________________________
Float_t* QOCAAnalysis::ChiSquares(QOCAFit* ocafit,Int_t nchi, 
				  Char_t** chi_values, Char_t** chi_titles)
{

  // Function to perform the nchi (usually 5) fits and to save the parameters
  // before the nominal fit is performed. The fit of the model to the data
  // is performed by calling DoQOCAFit(QOCAFit*).
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("ChiSquares","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::ChiSquares(): Fitting %d times.\n",nchi);

  Float_t* pars = NULL;

  for(Int_t ichi = 0 ; ichi < nchi ; ichi++){
    ocafit->SetChilimmin(atoi(chi_values[ichi]));
    DoQOCAFit(ocafit); // fit
    ocafit->SetNameTitle(chi_titles[ichi+nchi],chi_titles[ichi]);
    fout->cd(); ocafit->Write();

    // save parameters before the nominal fit
    if(ichi == nchi-2){
      pars = (Float_t*) SaveParameters(ocafit);
    }
  }
  // Write out the nominal model tree and laserball distributions:
  ModelAppliedQOCATree(ocafit);
  ModelAppliedLBDist(ocafit);

  if(pars == NULL) { Warning("ChiSquares","Parameters were not saved ... Aborting."); exit(-1); }

  return pars;
}

//______________________________________________________________________________
void QOCAAnalysis::H2O_Acr(QOCAFit* ocafit, Float_t* fitpars,
		Int_t n, Char_t** h2oacr_values, Char_t** h2oacr_titles)
{
  // Function that fits the data in the case the acrylic attenuation
  // varies, which is usually not the case since it has been measured
  // ex-situ. It takes the previously saved parameters.
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("H2O_Acr","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::H2O_Acr(): Fitting %d times.\n",n);

  // save the vary bit for acrylic, whatever it is
  Int_t vary = ocafit->GetAcrylicVary();

  for(Int_t i = 0 ; i < n ; i++){
    // Seed fit
    for(Int_t j = 0 ; j < ocafit->GetNpars() ; j++){
      ocafit->SetParameter(j+1, fitpars[j]);
    }
    ocafit->DataSetup();
    ocafit->SetAcrylicVary((Bool_t)atoi(h2oacr_values[i]));
    ocafit->DoFit();
    ocafit->SetNameTitle(h2oacr_titles[i+n],h2oacr_titles[i]);
    fout->cd(); ocafit->Write();

    if(fApply){ModelAppliedQOCATree(ocafit, h2oacr_titles[i+(2*n)]);}

    ocafit->SetAcrylicVary(vary); // set it back to what it was

  }
  return;
}

//______________________________________________________________________________
void QOCAAnalysis::TimeWindows(QOCAFit* ocafit, Float_t* fitpars,
			       Int_t n, Char_t** timew_titles)
{
  // Function that fits the data in the case the time windows are
  // varied. The user usually provides a series of window half-width
  // values, starting from 1 to 20 ns. 
  // The nominal fit value is 4ns/8ns half-/full- width.
  // It takes the previously saved parameters.
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("TimeWindows","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::TimeWindows(): Fitting %d times.\n",n);

  for(Int_t i = 0 ; i < n ; i++){
    // os -- supply index instead of window width
    ocafit->SetTimeWi(i);

    // Seed fit
    for(Int_t j = 0 ; j < ocafit->GetNpars(); j++){
      ocafit->SetParameter(j+1, fitpars[j]);
    }
    // allow Rayleigh attenuations to vary
    ocafit->SetD2OVary(kFALSE);     ocafit->SetRayleighD2OVary(kTRUE);
    ocafit->SetAcrylicVary(kFALSE); ocafit->SetRayleighAcrylicVary(kTRUE);
    ocafit->SetH2OVary(kFALSE);     ocafit->SetRayleighH2OVary(kTRUE);
   
    // set initial values for the Rayleigh scattering
    // to the values measured at 420 nm
    ocafit->Setrsd2o(5.13529e-05); // measured
    ocafit->Setrsacrylic(1.13332e-04); // QOptics::GetRayleighAcrylic()
    ocafit->Setrsh2o(6.61939e-05); // measured
 
    // Load fOccW[i] into fmrqxxx arrays
    DoQOCAFit(ocafit);
    ocafit->SetNameTitle(timew_titles[i+n],timew_titles[i]);
    fout->cd(); ocafit->Write();

    if(fApply){ModelAppliedQOCATree(ocafit,timew_titles[i+(2*n)]);}
  }
  return;
}

//______________________________________________________________________________
void QOCAAnalysis::ResChiSquares(QOCAFit* ocafit, Float_t* fitpars,
		Int_t n, Char_t** reschi_values, Char_t** reschi_titles)
{
  // Function that fits the data with specific chisquare limits.
  // The fit is done multiple times.
  // It takes the previously saved parameters.
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("ResChiSquares","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::ResChiSquares(): Fitting %d times.\n",n);

  for(Int_t i = 0 ; i < n ; i++){  //start the loop at index 1

    ocafit->SetChilimmin(atoi(reschi_values[i]));

    // seed fit
    for(Int_t j = 0 ; j < ocafit->GetNpars(); j++){
      ocafit->SetParameter(j+1, fitpars[j]);
    }
    DoQOCAFit(ocafit);
    DoQOCAFit(ocafit);
    DoQOCAFit(ocafit);
    ocafit->SetNameTitle(reschi_titles[i+n],reschi_titles[i]);
    fout->cd(); ocafit->Write();

    if(fApply){ModelAppliedQOCATree(ocafit,reschi_titles[i+(2*n)]);}
  }
  return;
}

//______________________________________________________________________________
void QOCAAnalysis::LBDistribution(QOCAFit* ocafit, Float_t* fitpars,
		Int_t n, Char_t** lbdist_values, Char_t** lbdist_titles,
		Int_t nruns, Int_t* runs)
{
  // Function that fits the data according to different laserball
  // distributions, namely, a flat and a squared functions are used.
  // It takes the previously saved parameters.
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("LBDistribution","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::LBDistribution(): Fitting %d times.\n",n);

  // TH2F* objects
  TH2F* hlbsquared = NULL;
  TH2F* hlbflat = NULL;

  // Save the previous configuration
  Int_t lbtype = ocafit->GetLBdistType();
  Bool_t pmtr2vary = ocafit->GetAngResp2Vary();

  for(Int_t i = 0 ; i < n ; i++){

    ocafit->InitParameters(nruns,runs); // resets the vary bits
    ocafit->SetLBdistType(lbtype);
    ocafit->SetAngResp2Vary(pmtr2vary);
    ocafit->Setacrylic(falpha_acrylic);
    // Fix the lbdist to the systematic.
    if(lbtype == 0) ocafit->SetLBDistVary(atoi(lbdist_values[i]));
    else ocafit->SetLBDistWaveVary(atoi(lbdist_values[i]));
    // Seed fit
    for(Int_t j = 0 ; j < ocafit->GetNpars(); j++){
      ocafit->SetParameter(j+1, fitpars[j]);
    }
    // Necessary to load fOccratio or fOccW into fmrqxxx arrays
    ocafit->DataSetup();

    if(i == 0){
      printf("\n----------------------------------------\n");
      printf("---   1) lbdist' = lbdist * lbdist   ---\n");
      printf("---   Squared flat lbdist systematic\n");
      
      if(lbtype == 0){
	hlbsquared = ocafit->ApplyLaserdist();
	hlbsquared->Multiply(hlbsquared);
	ocafit->FillLBDist(hlbsquared);
      } else {
	// Get the parameters
	Float_t* lbdist = (Float_t*) ocafit->Getlbdistwave();
	Float_t* newlbdist = new Float_t[ocafit->GetNbinsthetawave()*ocafit->GetNdistwave()];
	// first loop to copy the parameters
	for(Int_t k = 0 ; k < ocafit->GetNbinsthetawave()*ocafit->GetNdistwave() ; k++){
	  newlbdist[k] = lbdist[k];
	}
	// second loop to modify the parameters
	for(Int_t k = 0 ; k < ocafit->GetNbinsthetawave()*ocafit->GetNdistwave() ; k++){
	  newlbdist[k] *= 2.0; // scale
	}
	// set it in the array of parameters (amplitudes only)
	ocafit->Setlbamplitudes(newlbdist);
	delete newlbdist;
      }
    }
    else {
      printf("\n----------------------------------------\n");
      printf("---   2) lbdist' = 1.0 (flat)        ---\n");
      printf("---   Flat lbdist systematic\n");
      if(lbtype == 0){
	hlbflat = ocafit->ApplyLaserdist();
	hlbflat->Divide(hlbflat);
	ocafit->FillLBDist(hlbflat);
      } else {
	// Get the parameters
	Float_t* lbdist = (Float_t*) ocafit->Getlbdistwave();
	Float_t* newlbdist = new Float_t[ocafit->GetNbinsthetawave()*ocafit->GetNdistwave()];
	// first loop to copy the parameters
	for(Int_t k = 0 ; k < ocafit->GetNbinsthetawave()*ocafit->GetNdistwave() ; k++){
	  newlbdist[k] = lbdist[k];
	}
	// second loop to modify the parameters
	for(Int_t k = 0 ; k < ocafit->GetNbinsthetawave()*ocafit->GetNdistwave() ; k++){
	  newlbdist[k] = 0; // amplitude is zero - reproduces a height of one
	}
	// set it in the array of parameters (amplitudes only)
	ocafit->Setlbamplitudes(newlbdist);
	delete newlbdist;
      }
    }

    ocafit->DoFit();

    ocafit->SetNameTitle(lbdist_titles[i+n],lbdist_titles[n]);
    fout->cd(); ocafit->Write();

    if(fApply){ModelAppliedQOCATree(ocafit,lbdist_titles[i+(2*n)]);}
  }
  return;
}

//______________________________________________________________________________
void QOCAAnalysis::NoPmtpmtVar(QOCAFit* ocafit,
		Int_t n, Char_t** nopmt_values, Char_t** nopmt_titles,
		Int_t nruns,Int_t* runs)
{
  // Function that fits the data without considering the PMT-PMT
  // variations.
  // It takes the previously saved parameters.
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("NoPmtpmtVar","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else printf("\n* In QOCAAnalysis::NoPmtpmtVar(): Fitting %d times.\n",n);

  // Save the previous configuration
  Int_t lbtype = ocafit->GetLBdistType();
  Bool_t pmtr2vary = ocafit->GetAngResp2Vary();

  ocafit->InitParameters(nruns,runs); // resets the vary bits
  ocafit->Setacrylic(falpha_acrylic);
  ocafit->SetLBdistType(lbtype);
  ocafit->SetAngResp2Vary(pmtr2vary);

  ocafit->SetModelErr(0,0.0); // This turns off PMT-PMT variations.

  for(Int_t i = 0 ; i < n ; i++){

    ocafit->SetChilimmin(atoi(nopmt_values[i]));
    DoQOCAFit(ocafit);

    if(i == n-1){
      ocafit->SetNameTitle(nopmt_titles[n-i],nopmt_titles[n-1-i]);
      fout->cd(); ocafit->Write();

      if(fApply){ModelAppliedQOCATree(ocafit,nopmt_titles[n-i+1]);}
    }
  }
  ocafit->SetModelErr(1); // This turns on wavelength-dependent PMT-PMT variations.
  return;
}

//______________________________________________________________________________
void QOCAAnalysis::FitSystematics(QOCATree* ox, QOCAFit* ocafit,
		Int_t n, Char_t** syst_values, Char_t** syst_titles,
		Int_t nruns, Int_t* runs,Int_t nsyst)
{
  // Function that fits the systematics from QPath calculated systematic
  // QOCATree's.
  // It takes the previously saved parameters.
  // Used in QOCAAnalysis::RunQOCAFit()

  if(!fout->IsOpen()){
    Warning("FitSystematics","Output TFile is not opened ... Aborting.");
    exit(-1);
  } else {
    printf("\n* In QOCAAnalysis::FitSystematics(): "); 
    printf("Fitting %d times for %d systematics.\n",n,nsyst);
  }

  if(!fin->IsOpen()){
    Warning("FitSystematics","Input TFile is not opened ... Aborting.");
    exit(-1);
  }

  // Save the previous configuration
  Int_t lbtype = ocafit->GetLBdistType();
  Bool_t pmtr2vary = ocafit->GetAngResp2Vary();

  ocafit->SetTimeWi(-1);

  for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){

    printf("\n-------------------------------------\n");
    printf("Systematic %d proceeding, %s, %s.\n",
	   isyst+1,syst_titles[isyst],syst_titles[isyst+nsyst]);
    printf("-------------------------------------\n\n");

    QOCATree* oxs = (QOCATree*) fin->Get(syst_titles[isyst+nsyst]);  // Off-centre tree
    ocafit->SetTree(oxs,ox);
    ocafit->InitParameters(nruns,runs); // resets the vary bits
    ocafit->Setacrylic(falpha_acrylic);
    ocafit->SetLBdistType(lbtype);
    ocafit->SetAngResp2Vary(pmtr2vary);
    ocafit->SetNameTitle(syst_titles[isyst+(3*nsyst)],syst_titles[isyst+(2*nsyst)]);

    for(Int_t j = 0 ; j < n ; j++){
      ocafit->SetChilimmin(atoi(syst_values[j]));
      DoQOCAFit(ocafit);
    }
   
    fout->cd(); ocafit->Write();

    if(fApply){ModelAppliedQOCATree(ocafit,syst_titles[isyst+(4*nsyst)]);}
  }
  return;
}

//______________________________________________________________________________
void QOCAAnalysis::DoQOCAFit(QOCAFit* ocafit)
{
  // Wrapper for QOCAFit::DoFit() including the needed call to
  // QOCAFit::DataSetup().

  printf("\n* In QOCAAnalysis::DoQOCAFit().\n");

  // the fit method must be set before this call
  ocafit->DataSetup();
  ocafit->DoFit();

  return;
}

//______________________________________________________________________________
// RUNNER FUNCTIONS


//______________________________________________________________________________
void QOCAAnalysis::RunQPath(Int_t Narg, Char_t** arg)
{
  // Main function that replaces Bryce Moffat's original qpath_runner.C
  //
  // The keywords that fQOCALoad->SetKeyword() uses to find the data
  // in the cardfile must not be changed.
  //
  // 4 arguments to be read by the executable:
  //  - the step identifier (Char_t*)
  //    It tells QOCALoad how to load run lists and options depending
  //    on which fit (or step) is to be performed.
  //  - the scan identifier (Char_t* or TString)
  //  - the position fit method (Char_t* or TString)
  //  - the wavelength (Char_t* or TString)
  //
  // ----------------------------------
  // 05.2006 - O.Simard
  // Apparently things are getting a bit out of control. Here is some
  // documentation if you are totally lost in what follows.
  //
  // o) QOptics does all the optics: refraction, group velocities, vectors,
  // 	lengths, angles, and even more. It contains all the geometry we need,
  // 	loaded from geometry.dat.
  // 	Notice there aren't too many calls to QOptics below ... because:
  // 	
  // o) QPath is our interface to QOptics: it contains all the long and
  // 	painful functions that process the data and put them into large data
  // 	files. The main functions of QPath deals with QOCATree's.
  // 	Below, only one instance of QPath is used, and this is enough.
  // 	
  // o) QOCAAnalysis: this class is the user interface to QPath. If you don't
  // 	like what follows, think about what this code would be if the options
  // 	had to be entered by hand every time or even worse, if the code needed
  // 	to be recompiled. Once you know what it does, change the cardfile and 
  // 	smile. Optics does not run on autosno: it should be easy to run.
  //
  // o) qpath_runner: executable that runs QOCAAnalysis::RunQPath(). In this
  //    way, the executable is never changed. Only this class needs to be changed.
  //    
  // Summary: this is an attempt to make this code more object-oriented.
  //
  // 	Processing 	-> 	Storage 	-> Analysis
  // or
  // 	QOptics/QPath 	-> 	QPath/QOCATree 	-> QOCAAnalysis/qpath_runner
  //
  
  fQOCALoad->Presentator(arg[0]);

  // check arguments provided to the function
  if((Narg < 6) || (Narg > 6)){
    fprintf(stderr,"Arguments needed to function QOCAAnalysis::RunQPath: \n");
    fprintf(stderr,"\t%10s: \t%d\n","Narg",6);
    fprintf(stderr,"\t%10s: \t%s\n","arg[0]","[function name] (unused)");
    fprintf(stderr,"\t%10s: \t%s\n","arg[1]","[cardfile] (unused)");
    fprintf(stderr,"\t%10s: \t%s\n","arg[2]","[step]");
    fprintf(stderr,"\t%10s: \t%s\n","arg[3]","[scan]");
    fprintf(stderr,"\t%10s: \t%s\n","arg[4]","[fit method]");
    fprintf(stderr,"\t%10s: \t%s\n","arg[5]","[wlen]");
    if(Narg < 6) fprintf(stderr,"Needs more arguments. Exiting.\n");
    if(Narg > 6) fprintf(stderr,"Too many arguments. Exiting.\n");
    exit(-1);
  }
  else{
        fQOCALoad->SetStep(arg[2]); // set the step
        fQOCALoad->SetScan(arg[3]); // set the scan
        fQOCALoad->SetMeth(arg[4]); // set the scan
        fQOCALoad->SetWlen(arg[5]); // set the method
  }

  // Step identifier and flags (as in QOCALoad)
  Char_t* step = fQOCALoad->GetStep();
  Char_t* meth = fQOCALoad->GetMeth();
  Bool_t kP = (!strcmp(step,"pos")) || (!strcmp(step,"POS"));
  Bool_t kT = (!strcmp(step,"tree")) || (!strcmp(step,"TREE"));
  Bool_t kM = (!strcmp(meth,"fits")) || (!strcmp(meth,"FITS"));

  if(!kP && !kT){
    fprintf(stderr,"In QOCAAnalysis::RunQPath() : bad step.\n"); exit(-1);
  }

  // remind the function of what was possibly loaded in the constructor
  Char_t* dir = fQOCALoad->GetDirectory();
  Char_t* cardfile = fQOCALoad->GetFilename();
  // set the cardfile fullname in QOCALoad
  fQOCALoad->SetFullname(dir, cardfile);

  // now load directories
  fQOCALoad->SetKeyword("dir");
  Char_t** direc = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t dir_dim = fQOCALoad->GetDimensionFast();
  if(!direc) printf("* Did not find directories in cardfile. Running with defaults.\n");
  else printf("* Found %d directories in cardfile.\n",dir_dim);

  // Set up directories
  fQOCALoad->SetDirRunList(direc[0]); // run lists dir
  fQOCALoad->SetDirIn(direc[1]); // input .root files dir if needed
  fQOCALoad->SetDirOut(direc[1]); // output .root files dir (same)

  // create the file lists depending on what step
  // of the optical processing we're dealing with
  Char_t** files = fQOCALoad->LoadFileList();
  for(Int_t i = 0 ; i < 3 ; i++){
    printf("* %s\n",files[i]); // display
  } printf("\n");

  // copy the input/output files
  Char_t* posfile = files[1];
  Char_t* outfile = files[2];

  // file access using TFile class : overwrite the existing file if any
  if(fout) {
    if(fout->IsOpen()) fout->Close();
    delete fout;
  }
  fout = new TFile(outfile, "RECREATE");

  // get all run numbers and set them up in vectors
  fQOCALoad->LoadRunList(files[0]); // loads all run numbers into an array
  Int_t nruns = fQOCALoad->GetNrun(); // number of runs
  Int_t* runs = fQOCALoad->GetRuns(); // second column of the qrunlist : runs
  Int_t* cruns = fQOCALoad->GetCruns(); // third column of the qrunlist : central runs
  Int_t* fruns = fQOCALoad->GetFruns(); // fourth column of the qrunlist : 500 nm runs

  // ----------------------------------------------------------
  // Qpath calculations

  // QPath class pointer
  QPath* qp = new QPath();

  // look for the fit procedure
  // kM = 1: use own position found in position file (do not use fruns)
  // kM = 0: use 500 nm runs position in position file (use fruns)
  if(kM){
    //for(Int_t irun = 0 ; irun < nruns ; irun++){fruns[irun] = 0;}
    for(Int_t irun = 0 ; irun < nruns ; irun++){fruns[irun] = runs[irun];}
  }

  // get the rdt files style and pass options
  fQOCALoad->SetKeyword("p_rdtopt");
  fQOCALoad->SetFullname(dir, cardfile);
  Char_t** rdtoptarr = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t stylevalue = (Int_t) atoi((Char_t*) rdtoptarr[0]);
  qp->SetRdtFileStyle(stylevalue);
  Int_t passvalue = (Int_t) atoi((Char_t*) rdtoptarr[1]);
  Int_t* pass = new Int_t[nruns];
  
  // display the runlist with minimal info
  printf("\n*************************************************\n");
  printf("This is a qpath call over the following runs:\n");
  printf("runs  cruns fruns\n");
  for(Int_t irun = 0 ; irun < nruns ; irun++){
    // set rdt pass values
    pass[irun] = passvalue;
    // print run list
    printf("%d %d %d\n",runs[irun],cruns[irun],fruns[irun]);
  }
  printf("----------------------\n");
  printf("Total number of runs: %d\n",nruns);
  printf("User determined all runs are at wavelength %s nm.\n",fQOCALoad->GetWlen());
  printf("\n*************************************************\n");

  // get the position fit flag for qpath
  fQOCALoad->SetKeyword("p_posfit");
  fQOCALoad->SetFullname(dir, cardfile);
  Char_t** posfitarr = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t LBPosParam = (Int_t) atoi((Char_t*) posfitarr[0]);
  // set it in qpath
  qp->SetFitLBPosition(LBPosParam);
  // build runlist for FillFromRDT() and CalculateOccratio()
  qp->SetRunList(nruns,runs,cruns,fruns,pass);
  // set the wavelength
  qp->SetDefaultLambda((Int_t) atoi(fQOCALoad->GetWlen()));

  // get the force-dqxx flag for FillFromRDT
  fQOCALoad->SetKeyword("p_forcedqxx");
  fQOCALoad->SetFullname(dir, cardfile);
  Char_t** forcedqxxarr = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t forcedqxx = (Int_t) atoi((Char_t*) forcedqxxarr[0]);
  qp->SetForceDqxxFlag((Bool_t)forcedqxx);

  if(kT){
    // look in the position tree for fruns laserball positions
    TFile* ffit = new TFile(posfile, "READ");
    QOCATree* pos = (QOCATree*) ffit->Get("poptics");
    if(pos==NULL){
      printf("Warning in QOCAAnalysis::RunQPath(): QOCATree* is NULL,\n");
      printf(" ignore the previous TFile warning.\n");
    }
    else{
      Int_t nruns_ox = qp->GetLBPositions(pos);
      printf("\n* Missing %d runs to find positions out of %d total.\n",nruns_ox,nruns);
    }
    ffit->Close();
    delete ffit;
  }

  fout->cd();

  // store the runlist into the output file. Is it really necessary?
  TVector* rlist = new TVector(nruns);
  for(Int_t i = 0 ; i < nruns ; i++){
    rlist->operator()(i) = runs[i];
  }
  fout->cd(); rlist->Write("run list");

  // delete the run list after that since it has been loaded
  // in the QPath class
  delete [] runs; delete [] cruns; delete [] fruns; delete [] pass;

  // load directories into the QPath class (unfortunately not in order)
  // --------
  // direc[4] is a directory (DQXXDIR by default) 
  if(!strcmp(direc[4],"default")) qp->SetDqxxdir();
  else qp->SetDqxxdir((TString) direc[4]);
  // direc[5] is a directory (PWD + /rdt by default)
  if(!strcmp(direc[5],"default")) qp->SetRdtdir();
  else qp->SetRdtdir((TString) direc[5]);

  // direc[6] is a file: pmt efficiencies titles file (end of salt phase and later)
  if(!strcmp(direc[6],"default")) qp->SetPmtvarfile();
  else qp->SetPmtvarfile((TString) direc[6]);

  // load tree (QOCATree) names
  fQOCALoad->SetKeyword("p_treenames");
  fQOCALoad->SetFullname(dir, cardfile);
  Char_t** name = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t nnam = fQOCALoad->GetDimensionFast();

  // ----------------

  // QOCATree production:
  // - 4 main
  // - 6 x 2 = 12 for systematics

  // [0] rawtree
  // raw QOCATree filled from .rdt files
  QOCATree* rawtree = (QOCATree*) qp->FillFromRDT(name[0]);
  //WriteinFile(rawtree, fout);
  fout->cd(); rawtree->Write();

  if(!(qp->SetCentreRuns(rawtree))){
    fprintf(stderr,"Couldn't find all the central runs!\n"); exit(-1);
  }

  // load option integer for the ncd flag in the fBad mask
  fQOCALoad->SetKeyword("p_ncd");
  Char_t** ncd = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t flagncd = atoi(ncd[0]);
  qp->SetFlagNcd(flagncd); // to include or not ncd mask function
  Double_t ncdtol = atof(ncd[1]);
  qp->SetNcdTolerance(ncdtol); // sets the ncd distance tolerance
  // direc[3] is a file: 
  // set the Ncd position file after the flag is set
  if(!flagncd || !strcmp(direc[3],"default")){
    qp->SetNcdPositionfile(); // default
  } else qp->SetNcdPositionfile((TString) direc[3]);

  // load option integer for the pavr flag for high radius runs processing
  fQOCALoad->SetKeyword("p_pavr");
  Char_t** pavr = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  //Int_t flagpavr = atoi(pavr[0]);
  //qp->SetFlagPavr(flagpavr); // enable high radius live cut
  Float_t timewindow = atof(pavr[1]); // set the time window parameter needed
  qp->SetTimeWindowPavr(timewindow);  // via QPath
  
  // [1] postree
  // position QOCATree which contains fit for laserball positions
  // don't delete "postree" which will be used in the systematics
  QOCATree* postree = (QOCATree*) qp->CalculatePositions(rawtree, name[1]);
  //WriteinFile(postree, fout);
  fout->cd(); postree->Write();
  printf("* QOCATree %s has been written.\n",postree->GetName());
  //delete rawtree;
  fout->Delete(rawtree->GetName());
  rawtree = NULL;

  // stop HERE for position file production
  if(kP){
    printf("\n========================================\n");
    printf("* QOCAAnalysis::RunQPath() (qpath_runner)\n");
    printf("* successfully ended (positions).\n");
    printf("\n========================================\n");
    // close the rootfile
    fout->Close();
    delete fout;
    return;
  }

  // ----------------

  // [2] pathtree
  // paths QOCATree which contains all light paths calculations
  QOCATree* pathtree = (QOCATree*) qp->CalculatePaths(postree, name[2]);
  //WriteinFile(pathtree, fout);
  fout->cd(); pathtree->Write();
  printf("* QOCATree %s has been written.\n",pathtree->GetName());
  // use the correct paths and solid angles for occratio
  qp->SetCentreRuns(pathtree);

  // [3] ocatree
  // occupancy ratio method QOCATree
  QOCATree* ocatree = (QOCATree*) qp->CalculateOccratio(pathtree, name[3]);
  //WriteinFile(ocatree, fout);
  fout->cd(); ocatree->Write();
  printf("* QOCATree %s has been written.\n",ocatree->GetName());

  // remove latest entry in output file from memory
  fout->Delete(ocatree->GetName());

  // load option integer for fast processing
  fQOCALoad->SetKeyword("fast");
  Char_t** fast = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Bool_t flagfast = (Bool_t) atoi(fast[0]);
  SetFast(flagfast);

  // stop HERE for fast file production
  if(fFast){
    printf("\n========================================\n");
    printf("* QOCAAnalysis::RunQPath() (qpath_runner)\n");
    printf("* successfully ended (fast).\n");
    printf("\n========================================\n");
    // close the rootfile
    fout->Close(); delete fout; fout = NULL;
    return;
  }

  // ----------------

  printf("\n*******************************************************\n");
  printf("\n Systematics calculations :\n");
  printf("\n*******************************************************\n");

  // load systematics options
  fQOCALoad->SetKeyword("p_systematics");
  Char_t** syst = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t nsyst = fQOCALoad->GetNColumns();
  Int_t nopt = fQOCALoad->GetNLines();

  // from here no more QOCALoad variable is needed
  delete fQOCALoad;

  // Don't fit positions for systematics - use 500nm positions
  qp->SetFitLBPosition(0);

  // Don't redo Ncd reflection calculations if applicable
  qp->SetFlagNcdReflectionDone(kTRUE);

  // Ensures the same PMT set is used to calculate the systematics
  // when used with pathtree.
  qp->SetReMaskFlag(kFALSE); // keep original mask

  Float_t Rsyst; Int_t Psyst;
  // treename arguments passed to systematics function
  Char_t** newnames = new Char_t*[nnam];

  // loop over number systematics (nsyst)
  for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
    printf("\n=======================================\n");
    printf("* Systematic %d/%d proceeding.\n*\t%s.\n",
    	isyst+1,nsyst,syst[isyst+((nopt-1)*nsyst)]);
    printf("=======================================\n");

    // need to convert for specific variable type
    Rsyst = atof(syst[isyst]);		//convert to float
    Psyst = atoi(syst[isyst+nsyst]);	//convert to int

    // modify original names appending name extensions
    // newnames[0] = rawname: optics_ext
    // newnames[1] = pathname: poptix_ext
    // newnames[2] = ocaname: optix_ext
    // newnames[3] = title: strings. ex: X'=X+5cm
    newnames[0] = fQOCALoad->AppendElement(name[0], syst[isyst+(2*nsyst)]);
    newnames[1] = fQOCALoad->AppendElement(name[2], syst[isyst+(2*nsyst)]);
    newnames[2] = fQOCALoad->AppendElement(name[3], syst[isyst+(2*nsyst)]);
    newnames[3] = syst[isyst+((nopt-1)*nsyst)];

    // qpath systematics function: pass already-declared QOCATree pointers.
    // it should write the trees and clear memory by itself
    //PathSystematics(postree, qp, fout, Rsyst, Psyst, newnames);
    // use pathtree as input to be able to keep original Bad mask
    PathSystematics(pathtree, qp, Rsyst, Psyst, newnames);

    // delete newly created tree names
    delete [] syst[isyst];
    delete [] syst[isyst+(2*nsyst)];
    delete [] syst[isyst+(3*nsyst)];

  } //for(isyst)

  //
  // ---------------------------------------------
  //
  
  // 07.2006 - O.Simard
  // Add new systematics manually for NCD optics.
  // Here, we want to fill new QOCATree's with altered properties, 
  // therefore in some cases we don't need to go through
  // the whole path calculations again.
  // The systematics are:
  //
  //  o) Modified efficiencies     (readable from an external file)
  //  o) Narrower shadow tolerance (settable via QPath)
  //  o) Scaled NCD reflectivity   (settable via QPath)

  if(qp->GetFlagNcd()){
    
    // Reset all LB position systematics values to zero
    qp->SetAllsyst(0.);

    // ------------------------------
    // Modify Ncd reflectivity:
    // --
    printf("\n=======================================\n");
    printf("* Additional Systematic proceeding.\n");
    printf("*  Scaling Ncd Reflectivity by 0.0\n");
    printf("=======================================\n");
    // --
    qp->SetReMaskFlag(kFALSE);
    qp->SetFlagNcdReflectionDone(kTRUE); // do not repeat Ncd reflection
    qp->SetNcdReflectivityScale(0.); // none
    QOCATree* othertree = (QOCATree*) qp->CalculatePaths(pathtree,"poptix_ncdref00");
    ocatree = (QOCATree*) qp->CalculateOccratio(othertree,"optix_ncdref00");
    fout->RecursiveRemove(othertree);
    delete othertree;
    //WriteinFile(ocatree,fout);
    fout->cd(); ocatree->Write();
    printf("* QOCATree %s has been written.\n",ocatree->GetName());
    fout->Delete(ocatree->GetName());
    // --
    printf("\n=======================================\n");
    printf("* Additional Systematic proceeding.\n");
    printf("*  Scaling Ncd Reflectivity by 10.0.\n");
    printf("=======================================\n");
    // --
    qp->SetNcdReflectivityScale(10); // one order of magnitude
    othertree = (QOCATree*) qp->CalculatePaths(pathtree,"poptix_ncdref10");
    ocatree = (QOCATree*) qp->CalculateOccratio(othertree,"optix_ncdref10");
    fout->RecursiveRemove(othertree);
    delete othertree;
    //WriteinFile(ocatree,fout);
    fout->cd(); ocatree->Write();
    printf("* QOCATree %s has been written.\n",ocatree->GetName());
    fout->Delete(ocatree->GetName());
    
    /*
    // ------------------------------
    // Modify efficiencies: redo occratio only
    // --
    printf("\n=======================================\n");
    printf("* Additional Systematic proceeding.\n");
    printf("*  Same Efficiency of 0.99 for valid PMTs.\n");
    printf("=======================================\n");
    // --
    qp->SetReMaskFlag(kFALSE);
    qp->SetFlagNcdReflectionDone(kTRUE); // do not repeat Ncd reflection
    qp->SetSingleEfficiencyValue(0.9999); // all efficiencies set to this value
    ocatree = (QOCATree*) qp->CalculateOccratio(pathtree,"optix_flateff");
    //WriteinFile(ocatree,fout);
    fout->cd(); ocatree->Write();
    printf("* QOCATree %s has been written.\n",ocatree->GetName());
    fout->Delete(ocatree->GetName());
    fout->Delete(pathtree->GetName());
    */

    // ------------------------------
    // Modify efficiencies: redo occratio only
    // --
    printf("\n=======================================\n");
    printf("* Additional Systematic proceeding.\n");
    printf("*  Scaling Efficiencies by 10%% for valid PMTs.\n");
    printf("=======================================\n");
    // --
    qp->SetReMaskFlag(kFALSE);
    qp->SetFlagNcdReflectionDone(kTRUE); // do not repeat Ncd reflection
    qp->SetFlagEfficienciesLoaded(kFALSE); // need to reload (original) efficiencies
    qp->ScaleEfficiencies(1.10); // efficiencies affected by this value
    ocatree = (QOCATree*) qp->CalculateOccratio(pathtree,"optix_wideeff");
    //WriteinFile(ocatree,fout);
    fout->cd(); ocatree->Write();
    printf("* QOCATree %s has been written.\n",ocatree->GetName());
    fout->Delete(ocatree->GetName());
    fout->Delete(pathtree->GetName());
    
    // ------------------------------
    // Modify shadow tolerance: redo paths and occratio
    Double_t newtol = 0.5*ncdtol;
    // --
    printf("\n=======================================\n");
    printf("* Additional Systematic proceeding.\n");
    printf("*  Decreasing Shadow Tolerance from %.2f to %.2f cm.\n",ncdtol,newtol);
    printf("=======================================\n");
    // --
    qp->SetReMaskFlag(kTRUE); // update Bad mask with smaller shadow tolerance
    qp->SetFlagEfficienciesLoaded(kFALSE); // need to reload (original) efficiencies
    if(!strcmp(direc[6],"default")) qp->SetPmtvarfile();
    else qp->SetPmtvarfile((TString) direc[6]);
    qp->SetNcdTolerance(newtol); // half of what was used
    qp->SetFlagNcdReflectionDone(kTRUE); // do not repeat Ncd reflection
    pathtree = (QOCATree*) qp->CalculatePaths(postree,"poptix_sha");
    ocatree = (QOCATree*) qp->CalculateOccratio(pathtree,"optix_sha");
    fout->RecursiveRemove(pathtree);
    delete pathtree;
    //WriteinFile(ocatree,fout);
    fout->cd(); ocatree->Write();
    printf("* QOCATree %s has been written.\n",ocatree->GetName());
    fout->Delete(ocatree->GetName());
    qp->SetNcdTolerance(ncdtol); // reset before leaving
    
  }

  //
  // ---------------------------------------------
  //

  printf("\n* Systematics : Done.\n");

  // delete remaining tree from memory
  fout->Delete(postree->GetName());

  // close the rootfile
  fout->Close(); delete fout; fout = NULL;

  //------------------------------------------
  // Summary of central runs

  Int_t ncruns = qp->GetNumberOfCentreRuns();
  Int_t* crunindex = qp->GetCentreRunIndex();
  Int_t* crunsdistinct = qp->GetCentreRuns();

  printf("========================================\n");
  printf("Number of centre runs : %d\n",ncruns);
  printf("Distinct centre runs : " );
  for (Int_t i = 0 ; i < ncruns ; i++) {printf("%d ",crunsdistinct[i]);}
  printf("\nIndex of centre runs :\n");
  for (Int_t i = 0 ; i < nruns ; i++) {printf("%d ",crunindex[i]);}
  printf("\nHere are the pointers to central runs used :\n");
  QOCARun** crunptr = qp->GetCentreRunPtr();
  for (Int_t i = 0 ; i < ncruns ; i++){
    printf("Run %d\nIndex %d\nPointer %p\n",crunsdistinct[i],i,crunptr[i]);
  }

  delete qp;

  printf("\n========================================\n");
  printf("* QOCAAnalysis::RunQPath() (qpath_runner)\n");
  printf("* successfully ended (trees).\n");
  printf("\n========================================\n");

  return;
}

//______________________________________________________________________________
void QOCAAnalysis::RunQOCAFit(Int_t Narg, Char_t** arg)
{
  // Main function that replaces Bryce Moffat's original qocafit_runner.C
  //
  // The keywords that fQOCALoad->SetKeyword() uses to find the data
  // in the cardfile must not be changed.
  //
  // 4 arguments to be read by the executable:
  //  - the step identifier (Char_t*)
  //    It tells QOCALoad how to load run lists and options depending
  //    on which fit (or step) is to be performed.
  //  - the scan identifier (Char_t* or TString)
  //  - the position fit method (Char_t* or TString)
  //  - the wavelength (Char_t* or TString)
  //

  fQOCALoad->Presentator(arg[0]);

  // check arguments provided to the function
  if((Narg < 6) || (Narg > 6)){
    fprintf(stderr,"Arguments needed to function QOCAAnalysis::RunQOCAFit: \n");
    fprintf(stderr,"\t%10s: \t%d\n","Narg",6);
    fprintf(stderr,"\t%10s: \t%s\n","arg[0]","[function name] (unused)");
    fprintf(stderr,"\t%10s: \t%s\n","arg[1]","[cardfile] (unused)");
    fprintf(stderr,"\t%10s: \t%s\n","arg[2]","[step]");
    fprintf(stderr,"\t%10s: \t%s\n","arg[3]","[scan]");
    fprintf(stderr,"\t%10s: \t%s\n","arg[4]","[fit method]");
    fprintf(stderr,"\t%10s: \t%s\n","arg[5]","[wlen]");
    if(Narg < 6) fprintf(stderr,"Needs more arguments. Exiting.\n");
    if(Narg > 6) fprintf(stderr,"Too many arguments. Exiting.\n");
    exit(-1);
  }
  else {
    fQOCALoad->SetStep(arg[2]); // set the step
    fQOCALoad->SetScan(arg[3]); // set the scan
    fQOCALoad->SetMeth(arg[4]); // set the scan
    fQOCALoad->SetWlen(arg[5]); // set the method
  }

  // Step identifier and flags (as in QOCALoad)
  Char_t* step = fQOCALoad->GetStep();
  Bool_t kP = (!strcmp(step,"pos")) || (!strcmp(step,"POS"));
  Bool_t kT = (!strcmp(step,"tree")) || (!strcmp(step,"TREE"));
  if(kP || kT){
    fprintf(stderr,"qocafit_runner : bad step value.\n"); exit(-1);
  }

  // remind the function of what was possibly loaded in the constructor
  Char_t* dir = fQOCALoad->GetDirectory();
  Char_t* cardfile = fQOCALoad->GetFilename();

  // Prepare for loading titles : Create array from card file
  fQOCALoad->SetFullname(dir, cardfile);
  Char_t*** titles_array = fQOCALoad->LoadOptions(0); // no display

  // load directories (same as RunQPath with different step integer)
  fQOCALoad->SetKeyword("dir");
  Char_t** direc = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  fQOCALoad->SetDirRunList(direc[0]);
  fQOCALoad->SetDirIn(direc[1]);
  fQOCALoad->SetDirOut(direc[2]);
  Char_t* textfiledir = direc[7]; // for pmtvar files

  // create the file names according to step
  Char_t** files = fQOCALoad->LoadFileList();
  for(Int_t i = 0 ; i < 3 ; i++){
    printf("* %s\n",files[i]);
  } printf("\n");

  // from the list new out/fits files are created
  Char_t* ffile = files[1]; // fits/fruns file name
  Char_t* outfile = files[2]; // output file name

  // File access using TFile class
  if(fout){
    if(fout->IsOpen()) fout->Close();
    delete fout;
  }
  fout = new TFile(outfile, "RECREATE");  //overwrite the existing file

  // Preliminaries : get all quantities and set up the vectors
  fQOCALoad->LoadRunList(files[0]);
  Int_t nruns = fQOCALoad->GetNrun(); // will be modified
  Int_t new_nruns = nruns;

  Int_t* flags = fQOCALoad->GetFitFlags(); // array of flags (run to be done or not)
  Int_t* allruns = fQOCALoad->GetRuns(); // array of all run numbers

  for(Int_t k = 0 ; k < nruns ; k++){
    if(flags[k] == 0){new_nruns--;} // substract
  }

  // allocate new memory space for the shorter run list
  Int_t* runs = new Int_t[new_nruns];

  Int_t sub = 0; // variable used to substract
  for(Int_t k = 0 ; k < nruns ; k++){
    // each skipped run is one more space to substract in the new array
    if(flags[k] == 0){sub++;}
    else{
      // vector values assignment
      runs[k-sub] = allruns[k];
    }
  }

  // -----
  // reset nruns
  nruns = new_nruns;

  // display the runlist and acylic information
  printf("\n*************************************************\n");
  printf("This is a qocafit call over %d runs of file\n%s\n",nruns,ffile);
  printf("The runs are : \n");
  for(Int_t i = 0 ; i < nruns ; i++){printf("%d \n",runs[i]);}
  printf("\n*************************************************\n");


  // *********************************************************
  // 06.2006 - O.Simard
  // load user attenuations here but set them later after
  // QOCAFit::InitParameters(nruns, runs).

  fQOCALoad->SetFullname(dir, cardfile);

  Float_t falpha_acrylic_ini = 0.0, falpha_d2o_ini = 0.0, falpha_h2o_ini = 0.0;
  Float_t falpha_d2o = 0.0, falpha_h2o = 0.0;
  Float_t fscale_acrylic = 0.0, fscale_d2o = 0.0, fscale_h2o = 0.0;
  Int_t nwlen = 0;
  Int_t* iwlen;
  Float_t* att, *scale;
  Bool_t found = kFALSE; // confirms values are set

  // start with the acrylic attenuation value
  fQOCALoad->SetKeyword("f_acrylic");
  Char_t** acrylic = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  nwlen = fQOCALoad->GetNLines();
  iwlen = fQOCALoad->ConverttoIntegers(
		fQOCALoad->GetColumn(acrylic, 1), nwlen);
  att = fQOCALoad->ConverttoFloats(
		 fQOCALoad->GetColumn(acrylic, 2), nwlen);
  scale = fQOCALoad->ConverttoFloats(
		   fQOCALoad->GetColumn(acrylic, 3), nwlen);

  // compare the wavelengths to set the acrylic attenuation
  for(Int_t i = 0 ; i < nwlen ; i++){
    if(iwlen[i] == (atoi(fQOCALoad->GetWlen()))){
      falpha_acrylic_ini = att[i];
      fscale_acrylic = scale[i];
      falpha_acrylic = falpha_acrylic_ini*fscale_acrylic; // save the value
      found = kTRUE;
    }
    delete [] acrylic[i];
  }
  delete [] acrylic; delete [] att;
  delete [] iwlen; delete [] scale;
  if(!found) falpha_acrylic = 0.0;

  // then the d2o attenuation value
  fQOCALoad->SetKeyword("f_d2o");
  Char_t** d2o = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  nwlen = fQOCALoad->GetNLines();
  iwlen = fQOCALoad->ConverttoIntegers(
		fQOCALoad->GetColumn(d2o, 1), nwlen);
  att = fQOCALoad->ConverttoFloats(
		 fQOCALoad->GetColumn(d2o, 2), nwlen);
  scale = fQOCALoad->ConverttoFloats(
		   fQOCALoad->GetColumn(d2o, 3), nwlen);

  // compare the wavelengths to set the d2o attenuation
  found = kFALSE;
  for(Int_t i = 0 ; i < nwlen ; i++){
    if(iwlen[i] == (atoi(fQOCALoad->GetWlen()))){
      falpha_d2o_ini = att[i];
      fscale_d2o = scale[i];
      falpha_d2o = falpha_d2o_ini*fscale_d2o; // save the value
      found = kTRUE;
    }
    delete [] d2o[i];
  }
  delete [] d2o; delete [] att;
  delete [] iwlen; delete [] scale;
  if(!found) falpha_d2o = 0.0;

  // finally the h2o attenuation value
  fQOCALoad->SetKeyword("f_h2o");
  Char_t** h2o = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  nwlen = fQOCALoad->GetNLines();
  iwlen = fQOCALoad->ConverttoIntegers(
		fQOCALoad->GetColumn(h2o, 1), nwlen);
  att = fQOCALoad->ConverttoFloats(
		 fQOCALoad->GetColumn(h2o, 2), nwlen);
  scale = fQOCALoad->ConverttoFloats(
		   fQOCALoad->GetColumn(h2o, 3), nwlen);

  // compare the wavelengths to set the h2o attenuation
  found = kFALSE;
  for(Int_t i = 0 ; i < nwlen ; i++){
    if(iwlen[i] == (atoi(fQOCALoad->GetWlen()))){
      falpha_h2o_ini = att[i];
      fscale_h2o = scale[i];
      falpha_h2o = falpha_h2o_ini*fscale_h2o; // save the value
      found = kTRUE;
    }
    delete [] h2o[i];
  }
  delete [] h2o; delete [] att;
  delete [] iwlen; delete [] scale;
  if(!found) falpha_h2o = 0.0;

  // ***  Centre runs in the main QOCATree for 500nm
  // ***

  if(fin){
    if(fin->IsOpen()) fin->Close();
    delete fin;
  }
  fin = new TFile(ffile,"READ");   // Centre + Off-centre tree
  QOCATree* ox = (QOCATree*) fin->Get("optix"); // optix tree always loaded

  // ----------------------------
  // QOCAFit class pointer
  QOCAFit* ocafit = new QOCAFit();

  // basic setup
  ocafit->SetTree(ox, ox);
  ocafit->InitParameters(nruns, runs);
  
  // set attenuations (vary bits are set later)
  ocafit->Setd2o(falpha_d2o);
  ocafit->Setacrylic(falpha_acrylic);
  ocafit->Seth2o(falpha_h2o);
  printf("-----------------------------------------\n");
  printf("Media attenuations user initial values:\n");
  printf(" alpha_d2o = %.5e X %.2f = %.5e\n",
    falpha_d2o_ini,fscale_d2o,falpha_d2o);
  printf(" alpha_acrylic = %.5e X %.2f = %.5e\n",
    falpha_acrylic_ini,fscale_acrylic,falpha_acrylic);
  printf(" alpha_h2o = %.5e X %.2f = %.5e\n",
    falpha_h2o_ini,fscale_h2o,falpha_h2o);
  printf("-----------------------------------------\n");

  // ----------------------------
  // start loading fit options
  fQOCALoad->SetKeyword("f_options");
  Char_t** options = fQOCALoad->CreateArray(fQOCALoad->GetFullname());

  ocafit->SetChicuttype(atoi(options[0]));  // Force fixed chisq cut (1)
  Int_t doApplyTree = atoi(options[7]);
  SetApply((Bool_t) doApplyTree);
  //--------------------------------------------------------------------------------

  
  // ----------------------------
  // General settings for QOCAFit

  // SetOccType() should be called before Set***Vary() calls. 
  ocafit->SetOccType((Bool_t)atoi(options[5])); // fit method : occupancies/ratios
  ocafit->SetD2OVary((Bool_t)atoi(options[1])); // d2o att.
  ocafit->SetAcrylicVary((Bool_t)atoi(options[11])); // non-varying acrylic att.
  ocafit->SetH2OVary((Bool_t)atoi(options[2])); // h2o att.

  // N.Barros
  // Changed to accomodate a new option to override the default action 
  // concerning the LB variability and Mask variability.
  //
  // 08.2006 - O.Simard
  // Removed doOldfit option since never used and allow choice between laserball models.
  //
  printf("* Attempting to model the laserball light distribution with flipped runs.\n");
  Int_t lbdisttype = atoi(options[6]);
  ocafit->SetLBdistType(lbdisttype);
  // override allows to fix the laserball distribution after a model was picked
  Int_t override = atoi(options[10]);
  if (override != -1){
    ocafit->SetMaskVary((Bool_t)override);
    if(lbdisttype==1){
      ocafit->SetLBDistWaveVary((Bool_t)override);
    } else ocafit->SetLBDistVary((Bool_t)override);
  }

  ocafit->SetAngRespVary((Bool_t)atoi(options[3])); // pmt response: depends on SetOccType()
  ocafit->SetNormsVary((Bool_t)atoi(options[4])); // normalizations
  ocafit->SetAngrespFuncType((Int_t)atoi(options[8])); // angresp function type
  ocafit->SetModelErr((Int_t)atoi(options[9])); // this turns on/off PMT-PMT variations
  ocafit->SetTimeWi(-1); // Default time window: 8ns full width

  // 08.2006 - O.Simard
  // Pmt grouping options are now available in QOCAFit
  fQOCALoad->SetFullname(dir,cardfile);
  fQOCALoad->SetKeyword("f_pmtgroup");
  Char_t** pmtgroups = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  ocafit->SetAngResp2Vary((Bool_t)atoi(pmtgroups[0]));     // vary bit
  ocafit->SetPmtCoordinateCode((Int_t)atoi(pmtgroups[1])); // selection
  ocafit->SetPmtCoordinateLo((Int_t)atoi(pmtgroups[2]));   // lower limit
  ocafit->SetPmtCoordinateHi((Int_t)atoi(pmtgroups[3]));   // upper limit

  // 08.2005 - O.Simard
  // Load minimum number of PMT per bin in both PMT response and laserball distribution.
  // Load occupancy cut settings.
  fQOCALoad->SetFullname(dir,cardfile);
  fQOCALoad->SetKeyword("f_minbin");
  Char_t** minbin = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t mininc = (Int_t) atoi(minbin[0]);
  Int_t minlbd = (Int_t) atoi(minbin[1]);
  if(mininc > 0) ocafit->SetMinPMTperangle(mininc);
  else ocafit->SetMinPMTperangle(); // default
  if(minlbd > 0) ocafit->SetMinPMTperLBbin(minlbd);
  else ocafit->SetMinPMTperLBbin(); // default
  fQOCALoad->SetFullname(dir,cardfile);
  fQOCALoad->SetKeyword("f_occcut");
  Char_t** cut = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Bool_t usecut = (Bool_t) atoi(cut[0]);
  Float_t sigmafactor = (Float_t) atof(cut[1]);
  ocafit->SetDynamicOccupancyCut(usecut);
  ocafit->SetDynamicSigma(sigmafactor);

  // if the PMT variations were turned on by the user,
  // we must look for a file to read them using
  // the textfile directory.
  if(ocafit->GetModelErrType() == 3){
    Char_t textfile[1024];
    sprintf(textfile,"%svar_%s_%s_%s.txt",textfiledir,
	    fQOCALoad->GetScan(),fQOCALoad->GetMeth(),fQOCALoad->GetWlen());
    printf("* In QOCAAnalysis::RunQOCAFit(): Loading PMT variability data from %s.\n",textfile);
    fQOCALoad->SetFullname(textfile);
    fQOCALoad->SetKeyword();
    Char_t** pmtvar_arr = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t n = fQOCALoad->GetNColumns();
    Float_t* pmtvardata = fQOCALoad->ConverttoFloats(
						     fQOCALoad->GetLine(pmtvar_arr, 1), n);
    ocafit->SetModelErrFunction(pmtvardata[0],pmtvardata[1],pmtvardata[2]);
    ocafit->GetModelErrFunction();
  }

  // ============== Fits methods ===================


  // fits 1 to nominal : it returns the parameters before the nominal fit
  // do not put fApply since we want this part to be written

  // loading additional stuff: redefine fullname and keywords
  fQOCALoad->SetFullname(dir, cardfile);
  fQOCALoad->SetKeyword("f_chi");
  Char_t** chi = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t n = fQOCALoad->GetNColumns();

  Float_t* fitpars = ChiSquares(ocafit, n, chi, titles_array[0]);

  delete [] chi;


  // load option integer for fast processing
  fQOCALoad->SetKeyword("fast");
  Char_t** fast = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Bool_t flagfast = (Bool_t) atoi(fast[0]);
  SetFast(flagfast);
  // stop HERE for fast file production
  if(fFast){
    printf("\n===========================================\n");
    printf("* QOCAAnalysis::RunQOCAFit() (qocafit_runner)\n");
    printf("* successfully ended (fast).\n");
    printf("\n===========================================\n");
    // close the rootfile
    fin->Close(); delete fin; fin = NULL;
    ocafit->Clear();
    fout->Delete(ocafit->GetName());
    fout->Close(); delete fout; fout = NULL;
    return;
  }

  // ----------------

  //--------------------
  // Fit with acrylic and h2o both variable - to see if the fit moves in parameter
  // space, and more importantly to get the covariance matrix elements for
  // h2o-acrylic: covar[2][2], covar[2][3] and covar[3][3].

  fQOCALoad->SetKeyword("f_h2oacr");
  Char_t** ha = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  n = fQOCALoad->GetNColumns();
  printf("\n*************************************\n");
  printf("\nFits with Acrylic Attenuation Varying\n");
  printf("\n*************************************\n\n");
  H2O_Acr(ocafit, fitpars, n, ha, titles_array[1]);

  delete [] ha;

  //--------------------
  // Time windows:
  // Re-use the result from above to do tube selection and initial fit guess.
  // Leave out 15 and 20ns time windows ([10] and [11]).
  // --
  // New: fix nominal attenuations (8ns) and use QOCAFit Rayleigh attenuations
  // instead. This is an attempt to fit for the fraction of attenuations
  // is due to RS using the variable-sized time windows.

  fQOCALoad->SetKeyword("f_timew");
  Char_t** tw = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  n = fQOCALoad->GetNColumns();
  printf("\n*************************************\n");
  printf("\nFits with Variable-Sized Time Windows\n");
  printf("\n*************************************\n\n");
  // the following functions fixes the parameters
  TimeWindows(ocafit, fitpars, n, titles_array[2]);

  // set back vary flags
  ocafit->SetD2OVary(kTRUE); ocafit->SetRayleighD2OVary(kFALSE);
  ocafit->SetH2OVary(kTRUE); ocafit->SetRayleighH2OVary(kFALSE);
  ocafit->SetRayleighAcrylicVary(kFALSE);
  // reset the values
  ocafit->Setrsd2o(0.);
  ocafit->Setrsacrylic(0.);
  ocafit->Setrsh2o(0.);
  
  ocafit->SetTimeWi(-1);  // Put back to nominal time window

  delete [] tw;

  //--------------------------------------------------------------------------------
  // Restrictive chi-squared cut:
  // Cut until 16 (4 sigma) then 9 (3 sigma) to check effect of chi2 on tube
  // selection and end result.

  fQOCALoad->SetKeyword("f_reschi");
  Char_t** rchi = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  n = fQOCALoad->GetNColumns();
  printf("\n*************************************\n");
  printf("\nFits with Smaller ChiSquare Limits\n");
  printf("\n*************************************\n\n");
  ResChiSquares(ocafit, fitpars, n, rchi, titles_array[3]);

  ocafit->SetChilimmin(25); // Reset chisquare for next fits

  delete [] rchi;


  //--------------------------------------------------------------------------------
  // Use artificial laserball distributions in the fit:
  //  1) lbdist' = lbdist * lbdist
  //  2) lbdist' = 1.0

  fQOCALoad->SetKeyword("f_lbdist");
  Char_t** lbd = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  n = fQOCALoad->GetNColumns();
  printf("\n*************************************\n");
  printf("\nFits with Altered LB Distribution\n");
  printf("\n*************************************\n\n");
  LBDistribution(ocafit, fitpars, n, lbd, titles_array[4], nruns, runs);

  delete [] lbd;

  //--------------------------------------------------------------------------------
  // Default time window: 8ns full width
  ocafit->SetTimeWi(-1);

  //--------------------------------------------------------------------------------
  // No Pmt-Pmt variations

  fQOCALoad->SetKeyword("f_pmtvar");
  Char_t** pv = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  n = fQOCALoad->GetNColumns();
  printf("\n*************************************\n");
  printf("\nFits with No PMT-PMT Variations\n");
  printf("\n*************************************\n\n");
  NoPmtpmtVar(ocafit, n, pv, titles_array[5], nruns, runs);

  delete [] pv;

  //--------------------------------------------------------------------------------
  // Fit systematics:
  //   - position variations
  //   - lbdist variations
  //   - updated for new NCD systematics

  printf("\n*************************************\n");
  printf("\nFits for Systematics calculations\n");
  printf("\n*************************************\n\n");

  // load ncd flag
  fQOCALoad->SetKeyword("p_ncd");
  Char_t** ncd = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t flagncd = atoi(ncd[0]);
  // load the number of actual systematics in qpath options
  fQOCALoad->SetKeyword("p_systematics");
  fQOCALoad->GetDimension(fQOCALoad->GetFullname());
  Int_t nsyst = fQOCALoad->GetNColumns();
  if(flagncd) nsyst += 4; // added ncd systematics
  fQOCALoad->SetKeyword("f_systval");
  Char_t** sv = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  n = fQOCALoad->GetNColumns();

  FitSystematics(ox, ocafit, n, sv,titles_array[6], nruns, runs,nsyst);

  delete [] sv;

  fin->Close(); delete fin; fin = NULL;
  fout->Close(); delete fout; fout = NULL;

  printf("\n===========================================\n");
  printf("* QOCAAnalysis::RunQOCAFit() (qocafit_runner)\n");
  printf("* successfully ended.");
  printf("\n===========================================\n");

  delete fQOCALoad;

  return;

}



//______________________________________________________________________________
Int_t QOCAAnalysis::FindRun(QOCATree* ocatree, Int_t aRunNumber)
{
  // Finds a run based on the run number in a given QOCATree
  // and returns the index associated with the QOCARun.

  if(aRunNumber <= 0) Warning("FindRun","Invalid run number."); 
  if(ocatree == NULL) Warning("FindRun","Invalid input QOCATree.");

  QOCARun* ocarun = ocatree->GetQOCARun();   // gets the run pointer
  Int_t nrun = (Int_t)ocatree->GetEntries(); // number of runs

  // loop until the run number is found
  for(Int_t irun ; irun < nrun ; irun++){
    ocatree->GetEntry(irun);
    if(ocarun->GetRun() == aRunNumber) return irun;
  }

  // got here: means the run was not found
  Warning("FindRun","Run Number %d was not found.",aRunNumber);
  return -1; // default
}

//______________________________________________________________________________
QOCATree* QOCAAnalysis::ReplacePositions(QOCATree* oldtree, Char_t* AnInputFile, Int_t aDefaultOption)
{
  // Replaces the positions in a "position" QOCATree and saves them
  // into Laserxyz (using QOCARun::SetLaserxyz(z,y,z)).
  //
  // The aDefaultOption integer is used to tell what kind of 
  // position to keep in case there are no replacement found:
  // the convention is the same as in QOCARun::SetLaserpos(Int_t):
  //
  //  aDefaultOption == 0: manipulator position
  //                    1: fit position - direct line fit
  //                    2: fit position - QOptics fit
  //  Any other value results in no change to LaserX,Y,Z.

  printf("In QOCAAnalysis::ReplacePositions...\n");

  // Get old position tree info
  QOCARun* oldrun = oldtree->GetQOCARun();

  // Initiate new tree and other related objects.
  Char_t title[1024];
  sprintf(title,"Position Modified QOCATree (%s)",oldtree->GetName());
  QOCATree* newtree = new QOCATree(oldtree->GetName(),title);
 
  Int_t nrun = (Int_t)oldtree->GetEntries();
  QOCARun* newrun = newtree->GetQOCARun();

  // Use QOCALoad to read input file: four columns
  // Accepted format: run number - x - y - z
  Char_t* key = fQOCALoad->GetKeyword(); // save surrent keyword
  fQOCALoad->SetKeyword();
  fQOCALoad->SetFullname(AnInputFile);
  Char_t** array = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
  Int_t nlines = fQOCALoad->GetNLines(); // important for later
  Float_t* arrayf = fQOCALoad->ConverttoFloats(array,fQOCALoad->GetDimensionFast());
  Float_t* run = fQOCALoad->GetColumn(arrayf,1); // first  col: run
  Float_t* px = fQOCALoad->GetColumn(arrayf,2);  // second col: x
  Float_t* py = fQOCALoad->GetColumn(arrayf,3);  // third  col: y
  Float_t* pz = fQOCALoad->GetColumn(arrayf,4);  // fourth col: z
  delete arrayf;

  // fill the new tree with available positions
  Int_t nreplaced = 0;

  for(Int_t irun = 0 ; irun < nrun ; irun++){
   oldtree->GetEntry(irun);
   *newrun = *oldrun; // copy QOCARun entries, not the PMTs
   for(Int_t idim = 0 ; idim < nlines ; idim++){
     if(newrun->GetRun() == (Int_t)run[idim]){
       newrun->SetLaserxyz(px[idim],py[idim],pz[idim]);
       nreplaced++;
     } else {
       // keep default depending on the option in argument
       if(aDefaultOption <= 0){
	 newrun->SetLaserxyz(newrun->GetManipx(),newrun->GetManipy(),newrun->GetManipz());
       } else if(aDefaultOption == 1){
	 newrun->SetLaserxyz(newrun->GetFitx(),newrun->GetFity(),newrun->GetFitz());
       } else if(aDefaultOption >= 2){
	 newrun->SetLaserxyz(newrun->GetQFitx(),newrun->GetQFity(),newrun->GetQFitz());
       }
     }
   }
   newtree->Fill();
  }

  // delete QOCALoad products
  for(Int_t i = 0 ; i < fQOCALoad->GetDimensionFast() ; i++){
    delete array[i];
  } delete array;
  fQOCALoad->SetKeyword(key); // reset the original keyword
  
  printf("In QOCAAnalysis::ReplacePositions:\n");
  printf("\tReplaced %d, kept %d positions.\n",nreplaced,nrun-nreplaced);

  return newtree;
} 
