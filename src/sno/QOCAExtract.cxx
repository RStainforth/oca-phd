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

#include <QOCAExtract.h>
#include "TMath.h"

ClassImp(QOCAExtract)
;
//
//
// OCA Data Extracting class
//
// This class is used to extract data from enormous optics fit root
// files (qocafit) and to put the data into text files. The access of the
// data via text files makes easier the plotting and sharing/posting
// and reduces data transfer when processing.
//
// The extraction of text files is essential before the plots (QOCAPlots)
// or qoca titles files (QOCATitles) can be produced.
//
// While the data is accessed in the qocafit files, the functions usually
// opens up the systematics tree, provided a list of systematic tree names,
// and compute the errors (adding in quadrature). All the separate systematics
// are also written to the textfiles, as an indicator for its contribution to
// the total error.
//
//
//______________________________________________________________________________
QOCAExtract::QOCAExtract(Char_t* optionfile, Bool_t mode, Bool_t display, Char_t* rootdirectory, Char_t* exdirectory):QOCAAnalysis(optionfile, mode, display)
{
  // QOCAExtract constructor: basic assignments from arguments

  if(rootdirectory != NULL){
    SetIDir(rootdirectory);
  }

  if(exdirectory != NULL){
    SetODir(exdirectory);
  }

  // must open qocafit .root files
  SetILabel("qocafit");
  SetIFormat(".root");

  // must produce textfiles
  SetOFormat(".txt");

  SetExtractDisplay();

  // QOCALoad pointer
  fQOCALoad = new QOCALoad();

  // QOCAFit pointer to access qocafit.root files
  fQOCAFit = new QOCAFit();

  // set default number of bins for important histograms
  SetIncBins(); SetOccBins();
  SetEffBins(); SetPmtBins();

}

//______________________________________________________________________________
QOCAExtract::~QOCAExtract()
{
  // QOCAExtract destructor

  if(fQOCAFit) delete fQOCAFit;
  if(fQOCALoad) delete fQOCALoad;

}

//______________________________________________________________________________
TH1F* QOCAExtract::GetValues(TFile& fin, Char_t* tree, Int_t run, Char_t* identifier)
{
  // Detects the run number given in argument and fill the TH1F histogram
  // on a PMT-by-PMT basis so that it can be looked at with XSnoed with
  // the correct logical channel number.
  // The identifier is just the function name in QOCAPmt.

  if(identifier == NULL){
    fprintf(stderr, "In QOCAExtract::GetValues():\n");
    fprintf(stderr, "No QOCAPmt function provided.\nAborting.\n"); exit(-1);
  }

  QOCARun* qr = new QOCARun();
  QOCATree* qt = (QOCATree*) fin.Get(tree);
  qt->SetBranchAddress("Runs",&qr);
  Int_t nrun = (Int_t) qt->GetEntries();
  Int_t index = 0;

  // find the index corresponding to the given run number
  for(Int_t irun = 0 ; irun < nrun ; irun++){
    if(qt->GetEntry(irun)){
      if(qr->GetRun() == run){
	index = irun;
      }
    }
    else{
      fprintf(stderr, "In QOCAExtract::GetValues():\n");
      fprintf(stderr, "Bad index entry.\nAborting.\n"); exit(-1);
    }
  }
  
  // then assign the index correctly
  qt->GetEntry(index); 
  qr = (QOCARun*) qt->GetQOCARun();

  TH1F* h = new TH1F("h", "QOCAExtract PMT histogram", fNbinpmt, fMinpmt, fMaxpmt);

  for(Int_t ipmt = 0 ; ipmt < qr->GetNpmt() ; ipmt++){
    
    Float_t value = 0.0;

    // logical channel number of this pmt
    QOCAPmt* p = (QOCAPmt*) qr->GetPMT(ipmt);
    
    Int_t bin = p->GetPmtn();
    
    if(!strcmp(identifier, "Occupancy")) value = p->GetOccupancy();
    if(!strcmp(identifier, "Occratio")) value = p->GetOccratio();
    if(!strcmp(identifier, "Pmteffm")) value = p->GetPmteffm();
    if(!strcmp(identifier, "Cospmt")) value = p->GetCospmt();

    // set this logical channel PMT bin value
    h->SetBinContent(bin, value);

  }

  return h;
}

//______________________________________________________________________________
void QOCAExtract::ExtractEfficiencies(Float_t wlen, TH1F* mask)
{
  // Extract the PMT Efficiencies for the wlen and scans provided.
  // Produces a text file containing the PMT numbers
  // and the averaged efficiencies.
  //
  // No systematics here since one needs the model-applied tree (optica)
  // which is not reproduced for all the systematics as we speak.
  //
  // Adapted from Jose's pmtvar.cxx.:
  //
  // QOCAFit uses occratio, so we need to normalize the efficiencies,
  // using either the central run efficicency or the average efficiency.
  // This is what we'll do since QOCAFit doesn't calculate
  // the central run efficiency...
  //
  // os 2005.07
  // Checks that pmt number and histogram bins are the same. The histograms
  // currently have 10000 entries when the default SetEffBins() is called.
  // This could be changed but the important is that the first bin starts
  // at pmtn = 1, not 0.
  //
  // os 2005.09.16
  // New efficiency normalization using the run average. There is still
  // a total average normalization, but it has become a small correction.
  //
  // os 2005.09.19
  // The PMT variability as a function of incident angle is also extracted.
  // Produces a second text file: the PMT variability with incident angle.
  // These numbers are needed on a scan-by-scan basis to perform a 
  // precise QOCAFit using the occupancies.
  // While QOCAFit only needs the polynomial function fit parameters, the 
  // data is also written, for plotting.
  //
  // 2006.10 - O.Simard
  // Added masking histogram (see arguments) when comparing relative 
  // efficiencies with a similar PMT set from different scans.
  //
  // 2006.11 - O.Simard
  // Added estimate of relative error on the mean for individual PMTs.
  //

  // verifications header before extraction
  if(fDisplay){
    printf("Called QOCAExtract::ExtractEfficiencies(%d):\n",(Int_t)wlen);
  }
  SetIFormat(".root"); SetOFormat(".txt"); // force file formats
  Int_t nscan = GetNscan();
  Char_t** scans = GetScanArray();
  Char_t inputname[1024], outputname[1024];
  Char_t name[128], title[128];

  // masking histogram is initialzed if NULL
  sprintf(name,"pmt_mask_%d",(Int_t)wlen);
  sprintf(title,"PMT mask at %d nm",(Int_t)wlen);
  if(mask == NULL) {
    mask = new TH1F(name,title,fNbinpmt,fMinpmt,fMaxpmt);
    for(Int_t ibin = 1 ; ibin <= fNbinpmt ; ibin++){
      mask->SetBinContent(ibin,1.0); // no mask then all values are 1.0
    }
  } else printf("%d PMT efficiencies will be normalized.\n",(Int_t)mask->Integral());

  for(Int_t iscan = 0 ; iscan < nscan ; iscan++){

    Char_t* scan = scans[iscan]; // this specific scan based on index
    // set input/output single file names
    SetOLabel("eff"); // make sure it is reset
    sprintf(inputname, "%s%s_%s_%s_%d%s",
	    GetIDir(),GetILabel(),scan,GetKind(),(Int_t)wlen,GetIFormat());
    sprintf(outputname, "%s%s_%s_%s_%d%s",
	    GetODir(),GetOLabel(),scan,GetKind(),(Int_t)wlen,GetOFormat());
    if(fDisplay){
      printf("I/O files:\n\tI: %s\n\tO: %s\n",inputname,outputname);
    }

    // Tree, Run, and Pmt objects
    QOCARun* ocarun = new QOCARun(); // Run info
    QOCAPmt* ocapmt = new QOCAPmt(); // Pmt info
    TFile* f = new TFile(inputname,"READ"); // reading the rootfile
    fQOCAFit = (QOCAFit*) f->Get("qocafit_nominal");    // access fit results

    // -------------------
    // first find out about the PMT angular response fit limits
    // to get the PMT variability curve.
    Int_t npmtrbin  = fQOCAFit->GetNangresp();     // number of pmtr bins in qocafit
    Int_t paroffset = fQOCAFit->Getangresppari();  // qocafit parameter offset in array
    Int_t* vary_bin = fQOCAFit->Getfmrqvary();     // flags for varying bins
	
    Int_t jbin = 1; // get the rest of the bins	
    while(vary_bin[jbin+paroffset]){
      if(jbin == npmtrbin-1) break; // pmt response breaks after last varying bin
      jbin++;
    }
  
    npmtrbin = jbin; // save for histogram limits
    fQOCAFit->Clear();
    delete fQOCAFit;
    // -------------------

    QOCATree* ocatree = (QOCATree *) f->Get("optica"); // access model-applied tree
    ocatree->SetBranchAddress("Runs",&ocarun);
    Int_t nrun = (Int_t) ocatree->GetEntries();
    
    // histograms :
    //   h1sum: Contains the PMT efficiencies. 
    //          It gets modified along the function.
    //   h1num: Contains the number of runs that participate in a
    //          single PMT efficiency average calculation.
    //   h1run: Contains the run average efficiency for 
    //          normalization.
    //   h1err: Contains the PMT efficiency error estimate (stat only)
    //   h1var: Contains the PMT efficiency variation as a function of 
    //          incident angle.
    //   h2inc: Contains the PMT incident angle vs the efficiency
    //   h1occ: Contains the PMT incident angle vs occupancy
    //
    sprintf(name,"sum_eff_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"Summed Efficiency %s at %d nm",scan,(Int_t)wlen);
    TH1F* h1sum = new TH1F(name,title,fNbinpmt,fMinpmt,fMaxpmt);

    sprintf(name,"num_eff_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"Number of Valid Efficiencies per PMT %s at %d nm",scan,(Int_t)wlen);
    TH1F* h1num = new TH1F(name,title,fNbinpmt,fMinpmt,fMaxpmt);

    sprintf(name,"run_eff_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"Run Efficiencies %s at %d nm",scan,(Int_t)wlen);
    TH1F* h1run = new TH1F(name,title,nrun,1.,(Float_t)nrun);

    sprintf(name,"eff_err_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"Efficiencies Errors %s at %d nm",scan,(Int_t)wlen);
    TH1F* h1err = new TH1F(name,title,fNbinpmt,fMinpmt,fMaxpmt);

    sprintf(name,"var_eff_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"PMT Efficiency Variability %s at %d nm",scan,(Int_t)wlen);
    TH1F* h1var = new TH1F(name,title,npmtrbin,0.,(Float_t)npmtrbin);

    sprintf(name,"inc_eff_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"PMT Incident Angle vs Model Efficiencies %s at %d nm",scan,(Int_t)wlen);
    TH2F* h2inc = new TH2F(name,title,npmtrbin,0.,(Float_t)npmtrbin,100,0,5);

    // use a profile for occupancy since there are no indications a priori
    // of what the numbers could be.
    sprintf(name,"inc_occ_%s_%d",scan,(Int_t)wlen);
    sprintf(title,"PMT Incident Angle vs Model Occupancies %s at %d nm",scan,(Int_t)wlen);
    TProfile* h1occ = new TProfile(name,title,npmtrbin,0.,(Float_t)npmtrbin);
    

    // make quick basic check here on how the pmt numbers 
    // must coincide with bin numbers:
    if((h1sum->GetBinLowEdge(1) < 1) || (h1sum->GetBinLowEdge(1) > 2)){
      Error("ExtractEfficiencies","Histogram bin number and pmtn are shifted.");
      exit(-1);
    }

    // loop over run and pmts
    for(Int_t irun = 0; irun < nrun; irun++){
      ocatree->GetEntry(irun); // sets ocarun to irun entry
      Int_t npmt = ocarun->GetNpmt();
      Int_t run = ocarun->GetRun();
        
      // discard high radius runs
      TVector3 pos = (TVector3) ocarun->GetLaserxyz(); // fitted position
      if(pos.Mag() > 450.) continue;                // 450 cm and greater
      
      if(npmt == 0){
	Warning("ExtractEfficiencies","No PMTs found in QOCARun at index %d\n",run);
      }

      // counter for valid pmts per run
      Int_t npmt_run = 0;
      sprintf(name,"pmt_eff_%2d_%s_%d",irun,scan,(Int_t)wlen);
      sprintf(title,"Pmt Efficiency per run %s at %d nm",scan,(Int_t)wlen);
      TH1F* h1pmt = new TH1F(name,title,fNbinpmt,fMinpmt,fMaxpmt);

      // pmt loops ---------------------------

      // -------------------------------------
      // first loop fills the run and pmt info
      // -------------------------------------
      for(Int_t ipmt = 0 ; ipmt < npmt ; ipmt++){
	ocapmt = (QOCAPmt*) ocarun->GetPMT(ipmt);
	Int_t pmtn = ocapmt->GetPmtn();
	if(pmtn == 0) continue; // filled in underflow anyway
	// good pmts only, or those that are not masked
	if((ocapmt->GetBad() == 0) && (ocapmt->GetFitstatus() == 0) 
	   && (mask->GetBinContent(pmtn) == 1.0)){
	  // fill model efficiencies and others
	  Float_t eff = ocapmt->GetPmteffm();
	  Float_t inc = 180./TMath::Pi()*acos(ocapmt->GetCospmt());
	  Float_t occ = ocapmt->GetOccupancy();
	  							      
	  // request physical values
	  if((eff > 1e-09) && (eff < 1e+09)){
	    // store efficiency in corresponding bin
	    h1pmt->SetBinContent(pmtn,eff);
	    // add this pmt contribution for this run
	    h1run->AddBinContent(irun+1,eff);
	    npmt_run++;
	    // store incident angle and occupancy info
	    h2inc->Fill(inc,eff);
	    h1occ->Fill(inc,occ);

	  } else h1pmt->SetBinContent(pmtn,0.);
	} else continue;
      } // for(ipmt)

      // get the average efficiency for this run
      if(npmt_run > 0){
	h1run->SetBinContent(irun+1,
			     (h1run->GetBinContent(irun+1)/(Float_t)npmt_run));
      } else {
	Warning("ExtractEfficiencies","No valid PMT efficiencies for run %d\n",run);
      }

      // --------------------------------------------
      // second fills the efficiency summed histogram
      // --------------------------------------------
      for(Int_t pmtn = 1 ; pmtn <= h1pmt->GetNbinsX() ; pmtn++){
	// get the run-corrected efficiency 
	Float_t eff = h1pmt->GetBinContent(pmtn);
	if(eff > 0){
	  // add contribution per pmt
	  h1sum->AddBinContent(pmtn,h1pmt->GetBinContent(pmtn)/h1run->GetBinContent(irun+1));
	  // add to total number of run per pmt (+1)
	  h1num->AddBinContent(pmtn);
	}
      } // for(pmtn)

      // end of pmt loops -----------------------
      ocapmt->Clear();
      delete h1pmt;

    } // for(irun)
    ocarun->Clear();
    
    // -------------------------
    // Reorganize the efficiency data:
    //  - h1sum is the summed efficiencies per pmt per wlen
    //    It needs to be averaged : divide by number of PMTs in each bin.
    //  - h1num is the number of pmts used for the sum.
    //  - avg_tot is the sum of all the averaged efficiencies per wlen
    //    It is used to normalize the efficiencies (it scales h1sum).
    // -------------------------
    
    Int_t nbin = h1sum->GetNbinsX();
    Int_t nonzeropmts = 0;
    for(Int_t pmtn = 1 ; pmtn <= nbin ; pmtn++){
      if(h1num->GetBinContent(pmtn) > 0){
        // make sure the error entry is filled before h1sum is modified.
        // the error is relative.
        h1err->SetBinContent(pmtn,
			     h1sum->GetBinError(pmtn)/(h1sum->GetBinContent(pmtn)*sqrt(h1num->GetBinContent(pmtn))));
        // divide h1sum entry by number of pmts to give an average
	h1sum->SetBinContent(pmtn,
			     h1sum->GetBinContent(pmtn)/h1num->GetBinContent(pmtn));
	nonzeropmts++;
      }
      else {
	h1sum->SetBinContent(pmtn, 0.); // avoid in integral sum
        h1err->SetBinContent(pmtn, 0.); // avoid in integral sum
      }
    }
    
    // Calculate the total average by summing h1sum
    // and dividing by the number of valid PMTs.
    Float_t total_avg = h1sum->Integral()/(Float_t)nonzeropmts;
    if(fDisplay){
      printf("total_avg = %.2f/%d = %.4f\n",h1sum->Integral(),nonzeropmts,total_avg);
    }
    
    // scale h1sum and h1err
    h1sum->Scale(1./total_avg);
    h1err->Scale(1./total_avg);
  
    // from here, the average of h1sum should be ~ 1.
    if(fDisplay){
      printf("Averaged Efficiency = %f\n", h1sum->Integral()/(Float_t)nonzeropmts);
    }

    // final averaged efficiency are written to text files
    FILE* feff = fopen(outputname,"w");
    fprintf(feff,"# ptmn\teff\n");
    for(Int_t ibin = 1 ; ibin <= nbin ; ibin++){
      Float_t fineff = h1sum->GetBinContent(ibin);
      Float_t finerr = h1err->GetBinContent(ibin);
      if(fineff == 0.) {fineff = 1.; finerr = 0.;}
      fprintf(feff,"%.4d\t%10.3e\t%10.3e\n",ibin,fineff,finerr);
    } 
    fclose(feff);

    printf("Done with the extraction.\n");
    printf("Now proceeding to PMT variability calculation...\n");

    // -------------------------
    // PMT variability calculations:
    // Loop over the incident angle bins to get the efficiency
    // and occupancy spreads for each slice of theta_pmt.
    for(Int_t jbin = 1 ; jbin <= npmtrbin ; jbin++){
      
      Float_t variability; // to be stored

      // get the efficiency distribution in that slice
      TH1D* h1inc = (TH1D*) h2inc->ProjectionY("",jbin,jbin,"");
            
      if(h1occ->GetBinContent(jbin) > 0){
	
	// -- maneira
	// total variability is taken as the relative spread of efficiency
	// statistical error is taken as 1/sqrt of total occupancy in bin
	// final variability is the quadratical subtraction of stat from total
	
	Double_t total = (Double_t)h1inc->GetRMS()/(Double_t)h1inc->GetMean();
	Double_t stat  = 1./sqrt((Double_t)h1occ->GetBinContent(jbin)); 
	variability    = (Float_t)sqrt(total*total - stat*stat);
	
	h1var->SetBinContent(jbin,variability);
	if(fDisplay){
	  printf("Found variability for bin %2d: %.2f\n",jbin,variability);
	}
      } 
    }

    // fit the variability with a second order polynomial function
    TF1* polfunc = new TF1("polfunc","pol2(0)",0.,(Float_t)npmtrbin);
    h1var->Fit(polfunc,"RQN"); // fit without printing or drawing

    // print final variability : whatever the scan is
    // append "eff" to it.
    SetOLabel("var");
    sprintf(outputname, "%s%s_%seff_%s_%d%s",
	    GetODir(),GetOLabel(),scan,GetKind(),(Int_t)wlen,GetOFormat());

    FILE* fvar = fopen(outputname,"w");
    fprintf(fvar,"# variability parameters\n");
    for(Int_t ipar = 0 ; ipar < 3 ; ipar++){
      fprintf(fvar,"%+.4e\t",(Double_t)polfunc->GetParameter(ipar));
    } fprintf(fvar,"\n");
    fprintf(fvar,"# variability parameters errors\n");
    for(Int_t ipar = 0 ; ipar < 3 ; ipar++){
      fprintf(fvar,"%+.4e\t",(Double_t)polfunc->GetParError(ipar));
    } fprintf(fvar,"\n");
    fprintf(fvar,"# variability data\n");
    for(Int_t jbin = 1 ; jbin <= npmtrbin ; jbin++){
      fprintf(fvar,"%.2d\t%.5f\t%.5f\n",jbin,
	      (Double_t)h1var->GetBinContent(jbin),(Double_t)h1var->GetBinError(jbin));
    } 
    fclose(fvar);
    
    // delete basic histograms
    delete h1sum; delete h1num; delete h1run; delete h1err;
    delete h1var; delete h1occ; delete h2inc;
    delete polfunc;

    ocatree->Clear();
    f->Close(); delete f;

  } // for(iscan)
  
  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractEfficiencies(TH1F* mask)
{
  // Loop version of the previous function

  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractEfficiencies():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractEfficiencies(): \n");
  printf("* Extracting PMT Efficiencies from Model.\n");
  printf("--------------------------------------------------------\n");

  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Int_t nkind = GetFlagsSum();

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
      ExtractEfficiencies(wlens[iwlen],mask);
    }
  }

  return;
}

//______________________________________________________________________________
Float_t* QOCAExtract::LoadEfficiencies(Char_t* filename, Float_t lambda)
{
  // Useful function to load the efficiencies from a titles file format 
  // produced by QOCATitles.
  // Stolen from QPath::LoadPmteffTitles().
  // The filename to be read usually contains efficiencies for 6 
  // wavelengths, which is passed as the seonc argument.

  printf("---------------------------------------------\n");
  printf("* In QOCAExtract::LoadEfficiencies(): \n");
  printf("* Loading PMT Efficiencies from Titles file.\n");
  printf("---------------------------------------------\n");

  Int_t i, j;
  // a little too much storage for efficiencies
  Int_t* eff_titles = new Int_t[10000];
  Float_t* eff = new Float_t[10000];
  
  QPMTxyz* pmtxyz = new QPMTxyz("read");
  QTitles* file = new QTitles(filename,"QVAR",1);
  QBank* bank = (QBank*)file->GetBank();
  
  Int_t wlindex = 3;
  if (lambda < 340) wlindex = 0;
  if (lambda > 360 && lambda < 370) wlindex = 1;
  if (lambda > 380 && lambda < 390) wlindex = 2;
  if (lambda > 415 && lambda < 425) wlindex = 3;
  if (lambda > 495 && lambda < 505) wlindex = 4;
  if (lambda > 615) wlindex = 5; 
  
  // snoman pmt number loop
  for(i = 1; i <= 9522; i++) eff_titles[i] = bank->icons(i + wlindex*9522);
  // qsno pmt number loop
  for(i = 0; i < 10000; i++){
    if(!pmtxyz->IsNormalPMT(i) || pmtxyz->IsInvalidPMT(i)) continue;
    j = pmtxyz->GetSnomanNo(i);
    if (j < 1 || j > 10000) {
      printf("error: channel %d snoman %d \n",i,j);
      continue;
    }
    // bank integers need to be transformed to floats
    eff[i] = (Float_t) eff_titles[j] / 1000.;
    if(eff[i] == 0.) eff[i] = 1.;
  }
  
  // delete initialized objects
  delete pmtxyz; 
  delete file;
  delete eff_titles;
  
  return eff;
}

//______________________________________________________________________________
void QOCAExtract::UpdateEfficiencies(Char_t* filename, Char_t* treename)
{
  // Useful function to update the efficiencies in an existing
  // processed QOCATree.
  // Creates a clone of the input tree in a second file, containing
  // the updated efficiencies stored in QOCAPmt::SetPmteffc(x).
  // The name of the files are created automatically from the input
  // scan and wavelengths:
  // Example: oct03 at 500nm
  //    input:    qpath_oct03_fruns_500.root
  //    update:   qpath_oct03eff_fruns_500.root
  //

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::UpdateEfficiencies(): \n");
  printf("* Updating PMT Efficiencies from Model.\n");
  printf("--------------------------------------------------------\n");

  Int_t nfiles = 0; // number of files to be updated
  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Int_t nkind = GetFlagsSum();
  SetILabel("qpath");
  SetIFormat(".root"); SetOFormat(".root"); // force file formats
  Int_t nscan = GetNscan();
  Char_t** scans = GetScanArray();
  if(nscan > 1){
    Warning("UpdateEfficiencies",
	    "%d scans will be updated with the same efficiency file.",nscan);
  }
  
  Char_t originalname[1024], updatedname[1024];

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){

      Char_t* scan = scans[iscan]; // this specific scan based on index
      
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
	
	// since it is an update, force the filenames to be similar
	sprintf(originalname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scan,GetKind(),(Int_t)wlens[iwlen],GetIFormat());
	sprintf(updatedname, "%s%s_%s_eff_%d%s", // replace kind by "eff"
		GetIDir(),GetILabel(),scan,(Int_t)wlens[iwlen],GetOFormat());
	
	// read the input tree
	TFile* originalfile = new TFile(originalname,"READ");
	QOCATree* originaltree = (QOCATree*) originalfile->Get(treename);
	QOCARun* ocarun = originaltree->GetQOCARun();
	Int_t nruns = (Int_t) originaltree->GetEntries();
	
	// create the output tree
	TFile* updatedfile = new TFile(updatedname,"RECREATE");
	QOCATree* updatedtree = new QOCATree(originaltree->GetName(),originaltree->GetTitle());
	QOCARun* ocaruncalc = updatedtree->GetQOCARun();
	QOCAPmt* ocapmtcalc = new QOCAPmt();

	// get the array containing the efficiencies
	Float_t* eff = LoadEfficiencies(filename, wlens[iwlen]);
	
	// -------------------------------------
	// store new values by copying each pmt
	// and modifying the Pmteffc entry only.
	for(Int_t jrun = 0 ; jrun < nruns ; jrun++) {
	  originaltree->GetEntry(jrun);     // get run info
	  *ocaruncalc = *ocarun;            // copy run info, not the pmts
	  Int_t npmt = ocarun->GetNpmt();
	  
	  for(Int_t jpmt = 0 ; jpmt < npmt ; jpmt++) {
	    *ocapmtcalc = *(ocarun->GetPMT(jpmt)); // copy PMT data
	    Int_t pmtn = ocapmtcalc->GetPmtn();
	    ocapmtcalc->SetPmteffc(eff[pmtn]);     // update efficiency
	    ocaruncalc->AddPMT(ocapmtcalc);        // save updated PMT 
	  }
	  updatedtree->Fill();
	}

	ocarun->Clear(); originaltree->Clear();
	originalfile->Close(); delete originalfile;
	if(updatedtree){
	  updatedfile->cd();
	  updatedtree->Write(0, TObject::kOverwrite);
	}
	printf("Updated tree: %s\n",updatedname);
	updatedfile->Close(); delete updatedfile;
	delete ocapmtcalc;
	delete eff;
	nfiles++;

      } // iwlen
    } // iscan
  } // ikind

  printf("All %d files have been updated.\n",nfiles);

  return;
}

//______________________________________________________________________________
void QOCAExtract::ChooseKind(Int_t ikind)
{
  // Get the right kind set by the user with
  // either SetKind(Char_t*) or SetFlags(fits, fruns, eff, medium_flag)
  // according to the index ikind (usually set in a loop).

  Char_t* kind = NULL;
  Int_t nkind = GetFlagsSum();

  // default
  if(nkind <= 0) kind = "fruns";

  // trivial case for three kinds
  if(nkind == 3){
    if(ikind == 0) kind = "fits";
    if(ikind == 1) kind = "fruns";
    if(ikind == 2) kind = "eff";
  }
  
  // two kinds - mix them
  if(nkind == 2){
    if(ikind == 0){
      if(GetFitsFlag()) kind = "fits";
      else if(GetFrunsFlag()) kind = "fruns";
      else kind = "fruns";
    }
    if(ikind == 1){
      if(GetFrunsFlag() && !strstr(GetKind(),"fruns")) kind = "fruns";
      else if(GetEffFlag()) kind = "eff";
      else kind = "fruns";
    }
  }

  // one kind - find which one
  if(nkind == 1){
    if(GetFitsFlag()) kind = "fits";
    if(GetFrunsFlag()) kind = "fruns";
    if(GetEffFlag()) kind = "eff";
  }

  SetKind(kind);

  return;
}

//______________________________________________________________________________
void QOCAExtract::FindAttenuation(QOCAFit* qf)
{
  // Get the right attenuation value from a given medium
  // and the error associated (sqrt of diagonal element)

  // way to decide which media is plotted
  if(!strcmp(GetMedium(), "D2O") || GetMedium() == NULL){
    printf("*   Medium = %s (Heavy Water).\n", GetMedium());
    SetAttenuation(qf->Getd2o());
    SetAttenuationErr(qf->Getd2oerr());
    SetMediumFlag(0);
  }
    else{
      if(!strcmp(GetMedium(), "H2O")){
	printf("*   Medium = %s (Light Water).\n", GetMedium());
        SetAttenuation(qf->Geth2o());
	SetAttenuationErr(qf->Geth2oerr());
        SetMediumFlag(2);
      }
      else{
	if(!strcmp(GetMedium(), "ACR")){
	  printf("*   Medium = %s (Acrylic).\n", GetMedium());
          SetAttenuation(qf->Getacrylic());
	  SetAttenuationErr(qf->Getacrylicerr());
          SetMediumFlag(1);
	}
	else{
	  SetMedium("D2O");
	  printf("*   Setting Medium to default (%s).\n", GetMedium());
          SetAttenuation(qf->Getd2o());
	  SetAttenuationErr(qf->Getd2oerr());
          SetMediumFlag(0);
	}
      }
    }

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractAttenuation(Char_t* medium)
{
  SetMedium(medium);
  ExtractAttenuation();

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractAttenuation()
{
  // Extract the Inverse Attenuation Lengths from the qocafit rootfiles
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  //
  // -------
  // Inputs:
  //
  // - needs qocafit rootfiles by qocafit_runner
  // - rootfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetIDir(Char_t*)
  // - the function creates the rootfile names and load the
  //   data with a TFile object and a QOCAFit pointer
  // -------
  // Outputs:
  //
  // - produces one attenuation textfile per wavelength per scan per fit type
  // - each textfile also contains the total error and a detailed
  //   list of all the available systematics asked for in the cardfile
  // - textfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetODir(Char_t*)
  // -----------
  //

  // verifications header before extraction
  if(!(GetOLabel())){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAttenuation():\n");
      printf("Setting default output label to 'attenuation' \n");
    }
    SetOLabel("attenuation");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAttenuation():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractAttenuation(): \n");
  printf("* Extracting Inverse Attenuation Lengths.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen(), nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2
  Char_t* oldformat = GetOFormat();

  Char_t inputname[1024],outputname[1024],outputnamesyst[1024];

  Float_t* Thiserr = new Float_t[nsyst];
  Float_t* Thisrel = new Float_t[nsyst];

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

	// reading the rootfile
	TFile* f = new TFile(inputname,"READ");

	// Initialize local floats
	Float_t Atten = 0.0;
	Float_t SystAtten = 0.0;
	Float_t SystAttenErr = 0.0;
	//Float_t** covariance = NULL; // not used - see below
	Float_t Staterr = 0.0;
	Float_t Statrel = 1.0;
	Float_t Systsq = 0.0;
	Float_t* fact = GetSystFactors();

	// ------------------------------------------------------------
	// loop over systematics: 0 = tree with index 2 found in cardfile
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  Thiserr[isyst] = 0.0; Thisrel[isyst] = 0.0;
	  SetSystIndex(isyst);
	  fQOCAFit = (QOCAFit*) f->Get(GetSystName());
	  if(!fQOCAFit || !f){
	    Warning("ExtractAttenuation","Input file not found. Skipping."); continue;
	  }
	  FindAttenuation(fQOCAFit);

          if(isyst == 0){
	    // get the attenuation and the statistical error
            Atten = GetAttenuation();
	    Staterr = GetAttenuationErr();
	    // get the error from the covariance matrix
	    //covariance = fQOCAFit->Getfmrqcovar();
	    //Staterr = sqrt(covariance[GetMediumFlag()+1][GetMediumFlag()+1]);
	    if(Atten != 0) Statrel = Staterr/Atten;
	    if(Atten <= 0) Warning("ExtractAttenuation",
				   "Negative attenuation in %s.",GetSystName());
          }
	  else{
	    // for systematics, write shorter text files
	    SystAtten = GetAttenuation();
	    SystAttenErr = GetAttenuationErr();
	    Thiserr[isyst] = (fact[isyst])*(SystAtten-Atten);
	    Thisrel[isyst] = Thiserr[isyst]/Atten;
	    Systsq += pow(Thiserr[isyst],2);
	    
	    // if in full output mode, write separate (shorter) files 
	    if(GetFullOutput()){
	      // set up new text file name
	      SetOFormat(GetSystOFormat(isyst,oldformat));
	      sprintf(outputnamesyst, "%s%s%s_%s_%s_%d%s",
		      GetODir(),GetMedium(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());
	      // write only this systematic value and statistical error
	      FILE* systatt = fopen(outputnamesyst,"w");
	      
	      fprintf(systatt,"# Values - Relative Values - Description\n");
	      fprintf(systatt,"% 10.5e\t% 10.5e\t#. Attenuation.\n", SystAtten, 1.);
	      fprintf(systatt,"% 10.5e\t% 10.5e\t#. Statistical and Relative error.\n",
		      SystAttenErr, fabs(SystAttenErr/SystAtten));
	      
	      // close the output file
	      fclose(systatt);
            }
          }
	  fQOCAFit->Clear();
	  delete fQOCAFit;
        } // for(isyst)

	// ------------------------------------------------------------

	// assign total errors
	Float_t Systerr = sqrt(Systsq);
	Float_t Systrel = Systerr/Atten;
	Float_t Toterr = sqrt(pow(Staterr,2) + pow(Systerr,2));
	Float_t Totrel = Toterr/Atten;

	// reset file format for nominal
	SetOFormat(oldformat);
	sprintf(outputname, "%s%s%s_%s_%s_%d%s",
		GetODir(),GetMedium(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	// ------------------------------------------------------------
	// Output : two columns : Value - Relative Value
	FILE* fatt = fopen(outputname,"w");

	fprintf(fatt,"# Values - Relative Values - Description\n");
	fprintf(fatt,"% 10.5e\t% 10.5e\t#. Attenuation.\n", Atten, 1.);
	fprintf(fatt,"% 10.5e\t% 10.5e\t#. Total and Relative Total error.\n",
		Toterr, Totrel);
	fprintf(fatt,"% 10.5e\t% 10.5e\t#. Statistical and Relative error.\n",
		Staterr, Statrel);
	fprintf(fatt,"% 10.5e\t% 10.5e\t#. Systematic and Total Relative Systematic error.\n",
		Systerr, Systrel);

        for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  SetSystIndex(isyst);
	  fprintf(fatt,"% 10.5e\t% 10.5e\t#. Systematic %d (%s).\n",
	  	Thiserr[isyst],Thisrel[isyst],GetSystIndex(),GetSystName());
	}

        // close the output file
	fclose(fatt);
        f->Close(); delete f;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)
  delete [] Thiserr; delete [] Thisrel;

  return;
}


//______________________________________________________________________________
void QOCAExtract::ExtractAngularResponse()
{
  // Extract the PMT Relative Angular Response from the qocafit rootfiles
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  //
  // -------
  // Inputs:
  //
  // - needs qocafit rootfiles by qocafit_runner
  // - rootfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetIDir(Char_t*)
  // - the function creates the rootfile names and load the
  //   data with a TFile object and a QOCAFit pointer
  // -------
  // Outputs:
  // - produces one pmtr textfile per wavelength per scan per fit type
  // - each textfile also contains the total error and a detailed
  //   list of all the available systematics asked for in the cardfile
  // - textfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetODir(Char_t*)
  // -----------

  // verifications header before extraction
  if(!(GetOLabel()) || !(strcmp(GetOLabel(), "attenuation"))){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAngularResponse():\n");
      printf("Setting default output label to 'pmtAngResp' \n");
    }
    SetOLabel("pmtAngResp");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAngularResponse():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractAngularResponse(): \n");
  printf("* Extracting PMT Relative Angular Response.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen(), nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2
  Char_t* oldformat = GetOFormat();

  Char_t inputname[1024],outputname[1024],outputnamesyst[1024];

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());
	
	TFile* f = new TFile(inputname,"READ");

	// initialize the arrays before loop
	Int_t ibin = 0;
	Int_t nbin = 0, nbinnomi = 0;
	Int_t nbinmax = 90;
	Double_t binw = 0;
	Int_t paroffsetnomi = 0;
	Int_t* vary_binnomi = NULL;
	Double_t* pmtrnomi = new Double_t[nbinmax];
	Double_t* pmtrstat = new Double_t[nbinmax];
	for(ibin = 0 ; ibin < nbinmax ; ibin++){
	  pmtrnomi[ibin] = 1; pmtrstat[ibin] = 0;
	}
	Double_t** pmtrsyst = new Double_t*[nsyst];
	Double_t** pmtrsysterr = new Double_t*[nsyst];
	Double_t* pmtrErr = new Double_t[nsyst];
        for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  pmtrsyst[isyst] = new Double_t[nbinmax];
	  pmtrsysterr[isyst] = new Double_t[nbinmax];
	  pmtrErr[isyst] = 0;
	  // default values
	  for(ibin = 0 ; ibin < nbinmax ; ibin++){
	    pmtrsyst[isyst][ibin] = 1;
	    pmtrsysterr[isyst][ibin] = 0;
          }
	}
	Float_t pmtrnorm = 1.; // pmtr normalization (value at 0 deg.)
	Float_t* fact = GetSystFactors();
	
	// ------------------------------------------------------
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){

          // read in each qocafit 
	  SetSystIndex(isyst);
	  fQOCAFit = (QOCAFit*) f->Get(GetSystName());

	  if(!fQOCAFit || !f){
	    Warning("ExtractAngularResponse","Input file not found. Skipping."); continue;
	  }

	  nbin = fQOCAFit->GetNangresp(); // number of bins in qocafit
	  binw = (Double_t)nbinmax/nbin;  // bin width
	  Int_t paroffset = fQOCAFit->Getangresppari(); // qocafit parameter offset in array
	  Int_t* vary_bin = fQOCAFit->Getfmrqvary(); // flags for varying bins
	
	  if(isyst == 0){ // nominal
	    nbinnomi = nbin; // save for later
	    paroffsetnomi = paroffset;
	    vary_binnomi = vary_bin;
	    ibin = 0; // get info at normal incidence
	    pmtrnomi[ibin] = fQOCAFit->Getangresp(ibin); // at 0 deg.
	    pmtrnorm = pmtrnomi[ibin];
	    if(fDisplay) printf("* PMT Response Normalization = %.1f\n",pmtrnorm);
	
	    for(ibin = 1 ; ibin < nbinnomi ; ibin++){
	      // skip fixed bin
	      if(!vary_binnomi[ibin+paroffsetnomi]) continue;
	      // angular response and statistical errors
	      pmtrnomi[ibin] = fQOCAFit->Getangresp(ibin)/pmtrnorm;    // angular response
	      pmtrstat[ibin] = fQOCAFit->Getangresperr(ibin)/pmtrnorm; // statistical error
	      if(pmtrnomi[ibin] < 1.) Warning("ExtractAngularResponse",
					      "PMT Response bin %d smaller than 1.",ibin);
	    }
	  }
	  else { // systematics
	    ibin = 0;
	    pmtrsyst[isyst][ibin] = fQOCAFit->Getangresp(ibin);  

	    // same as for nominal but store in different arrays
	    for(ibin = 1 ; ibin < nbin ; ibin++){
	      // skip fixed bin
	      if(!vary_binnomi[ibin+paroffsetnomi]) continue;
	      // angular response and statistical errors
	      pmtrsyst[isyst][ibin] = fQOCAFit->Getangresp(ibin); // angular response
	      pmtrsyst[isyst][ibin] /= pmtrsyst[isyst][0];        // relative
	      pmtrsysterr[isyst][ibin] = fQOCAFit->Getangresperr(ibin)/pmtrsyst[isyst][0];
	    }

	    // if in full output mode, write separate (shorter) files 
	    if(GetFullOutput()){
	      // set up new text file name
	      SetOFormat(GetSystOFormat(isyst,oldformat));
	      sprintf(outputnamesyst, "%s%s_%s_%s_%d%s",
		      GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());
	      // write only this systematic value and statistical error
	      FILE* systpmtr = fopen(outputnamesyst,"w");
	      fprintf(systpmtr,"%s\t%10s\t%10s\n","# xpmtr", "pmtrsyst","pmtrsysterr");

	      for(ibin = 0 ; ibin < nbin ; ibin++){
		Double_t xpmtr = ibin*binw + 0.5;
	        // exponential notation
	        fprintf(systpmtr,"%.1f\t%+10.5e\t%+10.5e\n",
		        xpmtr,pmtrsyst[isyst][ibin],pmtrsysterr[isyst][ibin]);
              }
	      // close the output file
	      fclose(systpmtr);
	      if(fDisplay) printf("* %d bins in %s.\n", nbin, outputnamesyst);
            }
	  }

	  fQOCAFit->Clear();
	  delete fQOCAFit;
	} // for(isyst)

	// reset file format for nominal
	SetOFormat(oldformat);
	sprintf(outputname, "%s%s_%s_%s_%d%s",
		GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	// ------------------------------------------------------------
	// Output : multiple columns : Bins - Value - Errors
	FILE* fpmtr = fopen(outputname,"w");
 	fprintf(fpmtr,"%s\t%10s\t%10s\t%10s\t%10s\t%10s",
	"# xpmtr", "ypmt","pmtrTotErr","pmtrStatErr","pmtrSystErr","other systematics ...\n");

	// Bins that vary (original value + systematic error)
	for(ibin = 0 ; ibin < nbinnomi ; ibin++){

	  Double_t xpmtr = ibin*binw + 0.5;
	  Double_t ypmtr = pmtrnomi[ibin];
	  Double_t pmtrStatErr = pmtrstat[ibin];
	  Double_t pmtrSystsq = 0;

	  // inintialize errors
	  for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
	    Double_t Newypmtr = pmtrsyst[isyst][ibin];
	    Double_t Newypmtrerr = pmtrsysterr[isyst][ibin];
	    if((Newypmtr == 1.0) && (Newypmtrerr == 0)) continue; // skip defaults
	    pmtrErr[isyst] = (fact[isyst])*(Newypmtr-ypmtr);
	    pmtrSystsq += pow(pmtrErr[isyst],2);
	  }

	  Double_t pmtrSyst = sqrt(pmtrSystsq);
	  Double_t pmtrTotErr = sqrt(pmtrSystsq + pow(pmtrStatErr,2));

	  // exponential notation
	  fprintf(fpmtr,"%.1f\t%+10.5e\t%+10.5e\t%+10.5e\t%+10.5e\t",
	  	  xpmtr,ypmtr,pmtrTotErr,pmtrStatErr,pmtrSyst);
	  for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
		fprintf(fpmtr,"%+10.5e\t", pmtrErr[isyst]);
	  }
	  fprintf(fpmtr,"\n");
	  if(fDisplay) printf("* %d bins in %s.\n", nbinnomi, outputname);
	} // for(ibin)

        // close the output file
	fclose(fpmtr);
        f->Close(); delete f;

	delete [] pmtrnomi;
	delete [] pmtrstat;
	delete [] pmtrErr;
	for(Int_t i = 0 ; i < nsyst ; i++){
          delete [] pmtrsyst[i];
	  delete [] pmtrsysterr[i];
        }
	delete [] pmtrsyst;
	delete [] pmtrsysterr;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractAngularResponse2()
{
  // Same as previous but for QOCAFit::Getangresp2().

  // verifications header before extraction
  if(!(GetOLabel()) || !(strcmp(GetOLabel(), "pmtAngResp"))){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAngularResponse2():\n");
      printf("Setting default output label to 'pmtAngResp2' \n");
    }
    SetOLabel("pmtAngResp2");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAngularResponse2():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractAngularResponse2(): \n");
  printf("* Extracting PMT Relative Angular Response for Pmt Group2.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen(), nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2
  Char_t* oldformat = GetOFormat();

  Char_t inputname[1024],outputname[1024],outputnamesyst[1024];

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());
	
	TFile* f = new TFile(inputname,"READ");

	// initialize the arrays before loop
	Int_t ibin = 0;
	Int_t nbin = 0, nbinnomi = 0;
	Int_t nbinmax = 90;
	Double_t binw = 0;
	Int_t paroffsetnomi = 0;
	Int_t* vary_binnomi = NULL;
	Double_t* pmtrnomi = new Double_t[nbinmax];
	Double_t* pmtrstat = new Double_t[nbinmax];
	for(ibin = 0 ; ibin < nbinmax ; ibin++){
	  pmtrnomi[ibin] = 1; pmtrstat[ibin] = 0;
	}
	Double_t** pmtrsyst = new Double_t*[nsyst];
	Double_t** pmtrsysterr = new Double_t*[nsyst];
	Double_t* pmtrErr = new Double_t[nsyst];
        for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  pmtrsyst[isyst] = new Double_t[nbinmax];
	  pmtrsysterr[isyst] = new Double_t[nbinmax];
	  pmtrErr[isyst] = 0;
	  // default values
	  for(ibin = 0 ; ibin < nbinmax ; ibin++){
	    pmtrsyst[isyst][ibin] = 1;
	    pmtrsysterr[isyst][ibin] = 0;
          }
	}
	Float_t pmtrnorm = 1.; // pmtr normalization (value at 0 deg.)
	Float_t* fact = GetSystFactors();
	
	// ------------------------------------------------------
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){

          // read in each qocafit 
	  SetSystIndex(isyst);
	  fQOCAFit = (QOCAFit*) f->Get(GetSystName());

	  if(!fQOCAFit || !f){
	    Warning("ExtractAngularResponse2","Input file not found. Skipping."); continue;
	  }

	  nbin = fQOCAFit->GetNangresp(); // number of bins in qocafit
	  binw = (Double_t)nbinmax/nbin;  // bin width
	  Int_t paroffset = fQOCAFit->Getangresp2pari(); // qocafit parameter offset in array
	  Int_t* vary_bin = fQOCAFit->Getfmrqvary(); // flags for varying bins
	
	  if(isyst == 0){ // nominal
	    nbinnomi = nbin; // save for later
	    paroffsetnomi = paroffset;
	    vary_binnomi = vary_bin;
	    ibin = 0; // get info at normal incidence
	    pmtrnomi[ibin] = fQOCAFit->Getangresp2(ibin); // at 0 deg.
	    pmtrnorm = pmtrnomi[ibin];
	    if(fDisplay) printf("* PMT Response Normalization = %.1f\n",pmtrnorm);
	
	    for(ibin = 1 ; ibin < nbinnomi ; ibin++){
	      // skip fixed bin
	      if(!vary_binnomi[ibin+paroffsetnomi]) continue;
	      // angular response and statistical errors
	      pmtrnomi[ibin] = fQOCAFit->Getangresp2(ibin)/pmtrnorm;    // angular response
	      pmtrstat[ibin] = fQOCAFit->Getangresp2err(ibin)/pmtrnorm; // statistical error
	      if(pmtrnomi[ibin] < 1.) Warning("ExtractAngularResponse2",
					      "PMT Response bin %d smaller than 1.",ibin);
	    }
	  }
	  else { // systematics
	    ibin = 0;
	    pmtrsyst[isyst][ibin] = fQOCAFit->Getangresp2(ibin);  

	    // same as for nominal but store in different arrays
	    for(ibin = 1 ; ibin < nbin ; ibin++){
	      // skip fixed bin
	      if(!vary_binnomi[ibin+paroffsetnomi]) continue;
	      // angular response and statistical errors
	      pmtrsyst[isyst][ibin] = fQOCAFit->Getangresp2(ibin); // angular response
	      pmtrsyst[isyst][ibin] /= pmtrsyst[isyst][0];        // relative
	      pmtrsysterr[isyst][ibin] = fQOCAFit->Getangresp2err(ibin)/pmtrsyst[isyst][0];
	    }

	    // if in full output mode, write separate (shorter) files 
	    if(GetFullOutput()){
	      // set up new text file name
	      SetOFormat(GetSystOFormat(isyst,oldformat));
	      sprintf(outputnamesyst, "%s%s_%s_%s_%d%s",
		      GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());
	      // write only this systematic value and statistical error
	      FILE* systpmtr = fopen(outputnamesyst,"w");
	      fprintf(systpmtr,"%s\t%10s\t%10s\n","# xpmtr", "pmtrsyst","pmtrsysterr");

	      for(ibin = 0 ; ibin < nbin ; ibin++){
		Double_t xpmtr = ibin*binw + 0.5;
	        // exponential notation
	        fprintf(systpmtr,"%.1f\t%+10.5e\t%+10.5e\n",
		        xpmtr,pmtrsyst[isyst][ibin],pmtrsysterr[isyst][ibin]);
              }
	      // close the output file
	      fclose(systpmtr);
	      if(fDisplay) printf("* %d bins in %s.\n", nbin, outputnamesyst);
            }
	  }

	  fQOCAFit->Clear();
	  delete fQOCAFit;
	} // for(isyst)

	// reset file format for nominal
	SetOFormat(oldformat);
	sprintf(outputname, "%s%s_%s_%s_%d%s",
		GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	// ------------------------------------------------------------
	// Output : multiple columns : Bins - Value - Errors
	FILE* fpmtr = fopen(outputname,"w");
 	fprintf(fpmtr,"%s\t%10s\t%10s\t%10s\t%10s\t%10s",
	"# xpmtr", "ypmt","pmtrTotErr","pmtrStatErr","pmtrSystErr","other systematics ...\n");

	// Bins that vary (original value + systematic error)
	for(ibin = 0 ; ibin < nbinnomi ; ibin++){

	  Double_t xpmtr = ibin*binw + 0.5;
	  Double_t ypmtr = pmtrnomi[ibin];
	  Double_t pmtrStatErr = pmtrstat[ibin];
	  Double_t pmtrSystsq = 0;

	  // inintialize errors
	  for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
	    Double_t Newypmtr = pmtrsyst[isyst][ibin];
	    Double_t Newypmtrerr = pmtrsysterr[isyst][ibin];
	    if((Newypmtr == 1.0) && (Newypmtrerr == 0)) continue; // skip defaults
	    pmtrErr[isyst] = (fact[isyst])*(Newypmtr-ypmtr);
	    pmtrSystsq += pow(pmtrErr[isyst],2);
	  }

	  Double_t pmtrSyst = sqrt(pmtrSystsq);
	  Double_t pmtrTotErr = sqrt(pmtrSystsq + pow(pmtrStatErr,2));

	  // exponential notation
	  fprintf(fpmtr,"%.1f\t%+10.5e\t%+10.5e\t%+10.5e\t%+10.5e\t",
	  	  xpmtr,ypmtr,pmtrTotErr,pmtrStatErr,pmtrSyst);
	  for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
		fprintf(fpmtr,"%+10.5e\t", pmtrErr[isyst]);
	  }
	  fprintf(fpmtr,"\n");
	  if(fDisplay) printf("* %d bins in %s.\n", nbinnomi, outputname);
	} // for(ibin)

        // close the output file
	fclose(fpmtr);
        f->Close(); delete f;

	delete [] pmtrnomi;
	delete [] pmtrstat;
	delete [] pmtrErr;
	for(Int_t i = 0 ; i < nsyst ; i++){
          delete [] pmtrsyst[i];
	  delete [] pmtrsysterr[i];
        }
	delete [] pmtrsyst;
	delete [] pmtrsysterr;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractLBdist()
{
  // Extract the laserball (LB) distribution from the qocafit rootfiles
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  // using the Float_t* Getlbdist() array stored in QOCAFit.
  //
  // -------
  // Inputs:
  // - needs qocafit rootfiles produced by qocafit_runner
  // - rootfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetIDir(Char_t*)
  // - the function creates the rootfile names and load the
  //   data with a TFile object and a QOCAFit pointer
  // -------
  // Outputs:
  // - produces one lbdist textfile per wavelength per scan per fit type
  // - each textfile also contains the total error and a detailed
  //   list of all the available systematics asked for in the cardfile
  // - textfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetODir(Char_t*)
  // -----------

  // verifications header before extraction
  if(!GetOLabel()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractLBdist():\n");
      printf("Setting default output label to 'lbdist' \n");
    }
    SetOLabel("lbdist");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractLBdist():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractLBdist(): \n");
  printf("* Extracting Laserball distribution.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen(), nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2
  Char_t* oldformat = GetOFormat();

  Char_t inputname[1024],outputname[1024],outputnamesyst[1024];

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());
	sprintf(outputname, "%s%s_%s_%s_%d%s",
		GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	TFile* f = new TFile(inputname,"READ");

	Int_t ibin = 0, nbin = 0;
	Int_t nbinmax = 432; // 12 (theta) x 36 (phi)
	Double_t* lbdist = new Double_t[nbinmax];
	Double_t* lbstat = new Double_t[nbinmax];
	for(ibin = 0 ; ibin < nbinmax ; ibin++){
	  lbdist[ibin] = 1; lbstat[ibin] = 0;
	}
	Double_t** lbsyst = new Double_t*[nsyst];
	Double_t** lbsysterr = new Double_t*[nsyst];
	Double_t* lbErr = new Double_t[nsyst];
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  lbsyst[isyst] = new Double_t[nbinmax];
	  lbsysterr[isyst] = new Double_t[nbinmax];
	  for(Int_t ibin = 0 ; ibin < nbinmax ; ibin++){
	    lbsyst[isyst][ibin] = 1;
	    lbsysterr[isyst][ibin] = 0;
	  }
	}
	// laserball distribution specifics
	Int_t ntheta = 0;
	Int_t nphi = 0;
	Float_t binwtheta = 0;
	Float_t binwphi = 0;

	Float_t* fact = GetSystFactors();
        // systematics (starting at systindex = 1)
	// ------------------------------------------------------
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){

	  // read in each qocafit 
	  SetSystIndex(isyst);
	  fQOCAFit = (QOCAFit*) f->Get(GetSystName());

	  if(!fQOCAFit || !f){
	    Warning("ExtractLBdist","Input file not found. Skipping."); continue;
	  }

	  // get the number of parameters
	  ntheta = fQOCAFit->GetNbinstheta();
	  nphi = fQOCAFit->GetNbinsphi();
	  // get the 2d histogram
	  TH2F* laserdist = (TH2F*)fQOCAFit->GetLaserdist();
	  // get the limits
	  Float_t themin = laserdist->GetYaxis()->GetXmin();
	  Float_t themax = laserdist->GetYaxis()->GetXmax();
	  Float_t phimin = laserdist->GetXaxis()->GetXmin();
	  Float_t phimax = laserdist->GetXaxis()->GetXmax();

	  binwtheta = (themax-themin)/ntheta;
	  binwphi = (phimax-phimin)/nphi;
	  nbin = ntheta*nphi;
	  if(nbin != nbinmax){
	    Warning("ExtractLBdist","Number of bins does not match expected: %d vs %d.",
	            nbin,nbinmax);
	  }

	  if(isyst == 0){ // nominal
	    // don't use 'vary_bin' here since some higher parameters can exist
	    // not like pmtr where the cut is made at high theta
	    for(ibin = 0 ; ibin < nbin ; ibin++){
	      lbdist[ibin] = fQOCAFit->Getlbdist(ibin);    // laserball distribution
	      lbstat[ibin] = fQOCAFit->Getlbdisterr(ibin); // statistical error
	    }
	  }
	  else {
	    for(ibin = 0 ; ibin < nbin ; ibin++){
	      lbsyst[isyst][ibin] = fQOCAFit->Getlbdist(ibin);
	      lbsysterr[isyst][ibin] = fQOCAFit->Getlbdisterr(ibin);
	    }
	    // if in full output mode, write separate (shorter) files 
	    if(GetFullOutput()){
	      // set up new text file name
	      SetOFormat(GetSystOFormat(isyst,oldformat));
	      sprintf(outputnamesyst, "%s%s_%s_%s_%d%s",
		      GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());
	      // write only this systematic value and statistical error
	      FILE* systdist = fopen(outputnamesyst,"w");
	      fprintf(systdist,"%10s\t%10s\t%10s\t%10s\n",
	              "# lbtheta", "lbphi","lbdist","lbdistStatErr");

	      for(Int_t itheta = 0 ; itheta < ntheta ; itheta++){
	        Double_t lbtheta = (itheta+0.5)*binwtheta;
	        fprintf(systdist,"# cos(theta) bin %d\n",itheta);
	        for(Int_t iphi = 0 ; iphi < nphi; iphi++){
	          Double_t lbphi = (iphi+0.5)*binwphi;
	          ibin = (itheta*nphi)+iphi;

		  fprintf(systdist,"%+10.1e\t%+10.1e\t%+10.5e\t%+10.5e\n",
		          lbtheta,lbphi,lbsyst[isyst][ibin],lbsysterr[isyst][ibin]);
		} // for(iphi)
	      } // for(itheta)

	      // close the output file
	      fclose(systdist);
	      if(fDisplay) printf("* %d bins in %s.\n", nbin, outputnamesyst);
            }

	  }
	
	  fQOCAFit->Clear();
	  delete fQOCAFit;
	} // for(isyst)

	// reset file format for nominal
	SetOFormat(oldformat);
	sprintf(outputname, "%s%s_%s_%s_%d%s",
		GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	// ------------------------------------------------------------
	// Output : multiple columns : Bins - Value - Errors
	FILE* flbdist = fopen(outputname,"w");
 	fprintf(flbdist,"%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\n",
	"# lbtheta", "lbphi","lbdist","lbdistTotErr","lbdistStatErr","lbdistSystErr","other systematics ...");

	// Bins that vary (original value + systematic error)
	// using itheta and iphi explicitely used to calculate ibin
	for(Int_t itheta = 0 ; itheta < ntheta ; itheta++){
	  Double_t lbtheta = (itheta+0.5)*binwtheta;
	  fprintf(flbdist,"# cos(theta) bin %d\n",itheta);
	  for(Int_t iphi = 0 ; iphi < nphi; iphi++){
	    Double_t lbphi = (iphi+0.5)*binwphi;
	    ibin = (itheta*nphi)+iphi;
	    Double_t lb = lbdist[ibin];
	    Double_t lbStatErr = lbstat[ibin];
	    Double_t lbSystsq = 0;

	    // inintialize errors
	    for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
	      Double_t Newylb = lbsyst[isyst][ibin];
	      lbErr[isyst] = (fact[isyst])*(Newylb-lb);
	      lbSystsq += pow(lbErr[isyst],2);
	    }

	    Double_t lbSyst = sqrt(lbSystsq);
	    Double_t lbTotErr = sqrt(lbSystsq + pow(lbStatErr,2));

	    fprintf(flbdist,"%+10.1e\t%+10.1e\t%+10.5e\t%+10.5e\t%+10.5e\t%+10.5e\t",
		    lbtheta,lbphi,lb,lbTotErr,lbStatErr,lbSyst);
	    for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
	      fprintf(flbdist,"% 10.5e\t", lbErr[isyst]);
	    }
	    fprintf(flbdist,"\n");

	  } // for(iphi)
	} // for(itheta)

        // close the output file
	fclose(flbdist);
        f->Close(); delete f;

	delete [] lbdist;
	delete [] lbstat;
	for(Int_t i = 0 ; i < nsyst ; i++){
	  delete [] lbsyst[i];
	  delete [] lbsysterr[i];
	}
	delete [] lbsyst;
        delete [] lbErr;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)
  return;
}



//______________________________________________________________________________
void QOCAExtract::ExtractLBpoly()
{
  // Extract the laserball (LB) mask function from the qocafit rootfiles
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  // using the TF1* LBMaskTF1() stored in QOCAFit.
  //
  // -------
  // Inputs/Outputs: same as ExtractLBdist
  // -----------

  // verifications header before extraction
  if(!GetOLabel()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractLBpoly():\n");
      printf("Setting default output label to 'chi' \n");
    }
    SetOLabel("lbpoly");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractLBpoly():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractLBpoly(): \n");
  printf("* Extracting Laserball Mask function polynomials.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen(), nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2
  Char_t* oldformat = GetOFormat();

  Char_t inputname[1024],outputname[1024],outputnamesyst[1024];

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());
	
	TFile* f = new TFile(inputname,"READ");

	Int_t ipar = 0, npara = 5, nparq = 0;
	//Int_t npara = 7; // laserball mask
	Double_t* lbpoly = new Double_t[npara];
	Double_t* lbstat = new Double_t[npara];
	for(ipar = 0 ; ipar < npara ; ipar++){
	  lbpoly[ipar] = 1; lbstat[ipar] = 0;
	}
	Double_t** lbsyst = new Double_t*[nsyst];
	Double_t** lbsysterr = new Double_t*[nsyst];
	Double_t* lbErr = new Double_t[nsyst];
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  lbsyst[isyst] = new Double_t[npara];
	  lbsysterr[isyst] = new Double_t[npara];
	  for(Int_t ipar = 0 ; ipar < npara ; ipar++){
	    lbsyst[isyst][ipar] = 1;
	    lbsysterr[isyst][ipar] = 0;
	  }
	}

	Float_t* fact = GetSystFactors();
        // systematics (starting at systindex = 1)
	// ------------------------------------------------------
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){

	  // read in each qocafit 
	  SetSystIndex(isyst);
	  fQOCAFit = (QOCAFit*) f->Get(GetSystName());

	  if(!fQOCAFit || !f){
	    Warning("ExtractLBpoly","Input file not found. Skipping."); continue;
	  }

	  nparq = fQOCAFit->GetNlbmask();
	  TF1* mask = (TF1*) fQOCAFit->LBMaskTF1();
	  //npar = mask->GetNpar();
	  //if((npar != nparq) || (npar != npara)){
	  if(npara != nparq){
	    Warning("ExtractLBpoly","Number of bins does not match expected: %d vs %d.",
	            npara,nparq);
	  }
	  Double_t max = (Double_t) mask->GetMaximum(-1,1);

	  if(isyst == 0){
	    for(ipar = 0 ; ipar < nparq ; ipar++){
	      lbpoly[ipar] = fQOCAFit->Getlbmask(ipar)/max; 
	      lbstat[ipar] = fQOCAFit->Getlbmaskerr(ipar)/max;
	    }
	  }
	  else {
	    for(ipar = 0 ; ipar < nparq ; ipar++){
	      lbsyst[isyst][ipar] = fQOCAFit->Getlbmask(ipar)/max; 
	      lbsysterr[isyst][ipar] = fQOCAFit->Getlbmaskerr(ipar)/max;
	    }
	    // if in full output mode, write separate (shorter) files 
	    if(GetFullOutput()){
	      // set up new text file name
	      SetOFormat(GetSystOFormat(isyst,oldformat));
	      sprintf(outputnamesyst, "%s%s_%s_%s_%d%s",
		      GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());
	      // write only this systematic value and statistical error
	      FILE* systpoly = fopen(outputnamesyst,"w");
	      fprintf(systpoly,"# Laserball Mask Polynomial Parameters in (1+costheta)\n");
	      fprintf(systpoly,"%10s\t%10s\n","# lbpoly","lbsysterr");
	      for(ipar = 0 ; ipar < nparq ; ipar++){
	        fprintf(systpoly,"%+10.5e\t%+10.5e\n",
	                lbsyst[isyst][ipar], lbsysterr[isyst][ipar]);
	      }
	      fclose(systpoly);
	    }
	  }
	  fQOCAFit->Clear();
	  delete fQOCAFit;
	} // for(isyst)

	// reset file format for nominal
	SetOFormat(oldformat);
	    sprintf(outputname, "%s%s_%s_%s_%d%s",
		    GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	FILE* flbpoly = fopen(outputname,"w");
	fprintf(flbpoly,"# Laserball Mask Polynomial Parameters in (1+costheta)\n");
	fprintf(flbpoly,"%s\t%10s\t%10s\t%10s\t%10s\n",
	"# lbpoly", "lbpolyTotErr","lbpolyStatErr","lbpolySystErr","other systematics ...");

	for(ipar = 0 ; ipar < nparq ; ipar++){
	  Double_t lbpol = lbpoly[ipar];
	  Double_t lbStatErr = lbstat[ipar];
	  Double_t lbSystsq = 0;
	  // inintialize errors
	  for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
	    Double_t Newpoly = lbsyst[isyst][ipar];
	    lbErr[isyst] = (fact[isyst])*(Newpoly-lbpol);
	    lbSystsq += pow(lbErr[isyst],2);
	  }
	  Double_t lbSyst = sqrt(lbSystsq);
	  Double_t lbTotErr = sqrt(lbSystsq + pow(lbStatErr,2));

	  fprintf(flbpoly,"%+10.5e\t%+10.5e\t%+10.5e\t%+10.5e\t",
		  lbpol,lbTotErr,lbStatErr,lbSyst);
	  for(Int_t isyst = 1 ; isyst < nsyst ; isyst++){ // avoid repeating nominal
	    fprintf(flbpoly,"% 10.5e\t",lbErr[isyst]);
	  }
	  fprintf(flbpoly,"\n");
	}
	
	// close the output file
	fclose(flbpoly);
        f->Close(); delete f;

	delete [] lbpoly;
	delete [] lbstat;
	for(Int_t i = 0 ; i < nsyst ; i++){
	  delete [] lbsyst[i];
	  delete [] lbsysterr[i];
	}
	delete [] lbsyst;
        delete [] lbErr;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractChiSquare()
{
  // Extract the reduced ChiSquare from the qocafit rootfiles
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  // using fChisquare/(fNpmts-GetNparsVariable()) in QOCAFit
  //
  // -------
  // Inputs:
  // - needs qocafit rootfiles produced by qocafit_runner
  // - rootfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetIDir(Char_t*)
  // - the function creates the rootfile names and load the
  //   data with a TFile object and a QOCAFit pointer
  // -------
  // Outputs:
  // - produces one chisquare textfile per wavelength per scan per fit type
  // - each textfile also contains the total error and a detailed
  //   list of all the available systematics asked for in the cardfile
  // - textfiles directory can be specified with the QOCAExtract constructor
  //   or with QOCAAnalysis SetODir(Char_t*)
  // -----------

  // verifications header before extraction
  if(!(GetOLabel()) || !(strcmp(GetOLabel(), "pmtAngResp"))){
    if(fDisplay){
      printf("In QOCAExtract::ExtractChiSquare():\n");
      printf("Setting default output label to 'chi' \n");
    }
    SetOLabel("chi");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractChiSquare():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractChiSquare: \n");
  printf("* Extracting Reduced ChiSquare Results at the end of fit.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen(), nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2

  Char_t inputname[1024],outputname[1024];

  Float_t* Thiserr = new Float_t[nsyst];
  Float_t* Thisrel = new Float_t[nsyst];
	
  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());
	sprintf(outputname, "%s%s_%s_%s_%d%s",
		GetODir(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	// reading the rootfile
	TFile* f = new TFile(inputname,"READ");

	// Initialize local floats
	Float_t chisquare = 1.0;
	Float_t systchi = 1.0;
	Float_t Systsq = 0.0;
	Float_t* fact = GetSystFactors();

	// ------------------------------------------------------------
	// loop over systematics: 0 = qocafit_nominal
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  Thiserr[isyst] = 0.0; Thisrel[isyst] = 0.0;
	  SetSystIndex(isyst);
	  fQOCAFit = (QOCAFit*) f->Get(GetSystName());
	  if(!fQOCAFit || !f){
	    Warning("ExtractChiSquare","Input file not found. Skipping."); continue;
	  }

          if(isyst == 0){
	    chisquare = (fQOCAFit->GetChisquare())/(fQOCAFit->GetNpmts() - fQOCAFit->GetNparsVariable());
          }
	  else{
	    // not really an error here ... deviation based on systematics
            systchi = (fQOCAFit->GetChisquare())/(fQOCAFit->GetNpmts() - fQOCAFit->GetNparsVariable());
	    Thiserr[isyst] = (fact[isyst])*(systchi-chisquare);
	    Thisrel[isyst] = Thiserr[isyst]/chisquare;
	    Systsq += pow(Thiserr[isyst],2);
          }
	  fQOCAFit->Clear();
	  delete fQOCAFit;
        } // isyst

	// ------------------------------------------------------------

	// assign total errors
	Float_t Systerr = sqrt(Systsq);
	Float_t Systrel = Systerr/chisquare;

	// ------------------------------------------------------------
	// Output : two columns : Value - Relative Value
	FILE* fatt = fopen(outputname,"w");
	fprintf(fatt,"# Values - Relative Values - Description\n");
	fprintf(fatt,"% 10.5e\t% 10.5e\t#. Reduced ChiSquare.\n", chisquare, 1.);
	fprintf(fatt,"% 10.5e\t% 10.5e\t#. Systematic and Total Relative Systematic error.\n",
		Systerr, Systrel);

        for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  SetSystIndex(isyst);
	  fprintf(fatt,"% 10.5e\t% 10.5e\t#. Systematic %d (%s).\n",
	  	Thiserr[isyst],Thisrel[isyst],GetSystIndex(),GetSystName());
	}

        // close the output file
	fclose(fatt);
        f->Close(); delete f;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)

  delete [] Thiserr;
  delete [] Thisrel;
  return;
}


// AVERAGES : Read textfiles and produce textfiles
//______________________________________________________________________________
void QOCAExtract::ExtractAttAverages(Char_t* medium)
{
  SetMedium(medium);
  ExtractAttAverages();

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractAttAverages()
{
  // Uses previously extracted textfiles (attenuation)
  // to compute the averages and put them in a similar textfile.
  // The function should only be called for multiple scans.
  //
  // -------
  // Inputs:
  // - previously extracted textfiles
  // -------
  // Outputs:
  // - same format textfiles with average values
  // -----------

  // verifications header before extraction
  if(!(GetOLabel())){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAttAverages():\n");
      printf("Setting default output label to 'attenuation' \n");
    }
    SetOLabel("attenuation");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractAttAverages():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractAttAverages(): \n");
  printf("* Extracting Inverse Attenuation Lengths Averages.\n");
  printf("--------------------------------------------------------\n");

  // check number of scans: quit if only one scan
  if(GetNscan() <= 1){
    Warning("ExtractAttAverages","Invalid number of scans for average calculations. Skipping.");
    return;
  }

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Int_t* limits = GetTimeLimits(); // start and end
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan();
  Int_t nwlen = GetNwlen();
  Int_t nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2

  Char_t inputname[1024],outputname[1024];

  // find out the limit scans (start and end)
  Int_t n = 0;
  Char_t* scan_start, * scan_end;
  for(Int_t iscan = 0 ; iscan < nscan ; iscan++){
    if((n == 0) && (limits[iscan] == 1)){
      scan_start = scans[iscan];
      n = 1;
    } else {
      if((n == 1) && (limits[iscan] == 1)){
        scan_end = scans[iscan];
	n = 2;
      }
    }
  }

  printf("* Starting scan: %s\n", scan_start);
  printf("* Ending scan:   %s\n", scan_end);
  if(n > 2) printf("* Make sure the limits are set properly.\n");
  printf("--------------------------------------------------------\n");

  // arrays with values to be filled and written
  Double_t* avgAtt = new Double_t[nwlen];
  Double_t* avgErr = new Double_t[nwlen];
  Double_t* avgRelErr = new Double_t[nwlen];
  Double_t* avgTotStat = new Double_t[nwlen];
  Double_t* avgRelTotStat = new Double_t[nwlen];
  Double_t* avgTotSyst = new Double_t[nwlen];
  Double_t* avgRelTotSyst = new Double_t[nwlen];
  
  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scans
  // iwlen: wavelengths
  // isyst: systematics
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    ChooseKind(ikind);

    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

      // initialize
      avgAtt[iwlen] = 0.; avgErr[iwlen] = 0.; avgRelErr[iwlen] = 0.;
      avgTotStat[iwlen] = 0.; avgRelTotStat[iwlen] = 0.;
      avgTotSyst[iwlen] = 0.; avgRelTotSyst[iwlen] = 0.;

      // only one average textfile per wavelength
      sprintf(outputname, "%s%s%s_%s_%s_%s_%d%s",
	      GetODir(),GetMedium(),GetOLabel(),scan_start,scan_end,
	      GetKind(),(Int_t)wlens[iwlen],GetOFormat());

      for(Int_t iscan = 0 ; iscan < nscan ; iscan++){

	// read from output directory (exception)
	sprintf(inputname, "%s%s%s_%s_%s_%d%s",
		GetODir(),GetMedium(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

	// read the attenuation and error
	fQOCALoad->SetFullname(inputname);
	fQOCALoad->SetKeyword(); // data files, no keyword
        Char_t** tmpc = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
	Int_t dim = fQOCALoad->GetDimensionFast();

	// convert to doubles
	Double_t att = (Double_t) atof(tmpc[0]);
	Double_t err = (Double_t) atof(tmpc[2]);
	Double_t totstat = (Double_t) atof(tmpc[4]);
	Double_t totsyst = (Double_t) atof(tmpc[6]);

	// add up squared values to avoid signs
	if(err > 0){
	  avgAtt[iwlen] += att/(pow(err,2));
	  avgErr[iwlen] += 1./(pow(err,2));
	  avgTotStat[iwlen] += 1./(pow(totstat,2));
	  avgTotSyst[iwlen] += 1./(pow(totsyst,2));
	}
	for(Int_t i = 0 ; i < dim ; i++) delete [] tmpc[i];
	delete tmpc;

      } // for(iscan)

      // calculate final values
      if(avgErr[iwlen] > 0){
	avgAtt[iwlen] *= 1/(avgErr[iwlen]);
	avgErr[iwlen]  = sqrt(1/(avgErr[iwlen]));
	avgTotStat[iwlen] = sqrt(1/(avgTotStat[iwlen]));
	avgTotSyst[iwlen] = sqrt(1/(avgTotSyst[iwlen]));
      }
      else{
        avgAtt[iwlen] = 0.; avgErr[iwlen] = 0.;
	avgTotStat[iwlen] = 0.; avgTotSyst[iwlen] = 0.;
      }

      // relative values
      avgRelErr[iwlen] = (avgErr[iwlen])/(avgAtt[iwlen]);
      avgRelTotStat[iwlen] = (avgTotStat[iwlen])/(avgAtt[iwlen]);
      avgRelTotSyst[iwlen] = (avgTotSyst[iwlen])/(avgAtt[iwlen]);

      // ------------------------------------------------------------
      // Output : two columns : Value - Relative Value
      FILE* fatt = fopen(outputname,"w");
      fprintf(fatt,"# Values - Relative Values - Description\n");
      fprintf(fatt,"% 10.5e\t% 10.5e\t#. Attenuation.\n", avgAtt[iwlen], 1.);
      fprintf(fatt,"% 10.5e\t% 10.5e\t#. Total and Relative Total error.\n",
		avgErr[iwlen], avgRelErr[iwlen]);
      fprintf(fatt,"% 10.5e\t% 10.5e\t#. Statistical and Relative error.\n",
		avgTotStat[iwlen], avgRelTotStat[iwlen]);
      fprintf(fatt,"% 10.5e\t% 10.5e\t#. Systematic and Total Relative Systematic error.\n",
		avgTotSyst[iwlen], avgRelTotSyst[iwlen]);
      // do not calculate the averages for single systematics
      for(Int_t isyst = 0 ; isyst < nsyst-1 ; isyst++){
	  SetSystIndex(isyst);
	  fprintf(fatt,"% 10.5e\t% 10.5e\t#. Systematic %d (%s).\n",
	  	0.0,0.0,GetSystIndex(),GetSystName());
      }
      // close the output file
      fclose(fatt);

    } // for(iwlen)
  } // for(ikind)

  delete avgAtt; delete avgErr; delete avgRelErr;
  delete avgTotStat; delete avgRelTotStat;
  delete avgTotSyst; delete avgRelTotSyst;
  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractPmtrAverages()
{
  // Uses previously extracted textfiles (pmtr)
  // to compute the averages and put them in a similar textfile.
  // The function should only be called for multiple scans.
  //
  // -------
  // Inputs:
  // - previously extracted textfiles
  // -------
  // Outputs:
  // - same format textfiles with average values
  // -----------

  // verifications before extraction
  if(!(GetOLabel()) || !(strcmp(GetOLabel(), "attenuation"))){
    if(fDisplay){
      printf("In QOCAExtract::ExtractPmtrAverages():\n");
      printf("Setting default output label to 'pmtAngResp' \n");
    }
    SetOLabel("pmtAngResp");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractPmtrAverages():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractPmtrAverages(): \n");
  printf("* Extracting Pmtr Averages\n* from previously extracted textfiles.\n");
  printf("--------------------------------------------------------\n");

  // check number of scans: quit if only one scan
  if(GetNscan() <= 1){
    Warning("ExtractPmtrAverages","Invalid number of scans for average calculations. Skipping.");
    return;
  }

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Int_t* limits = GetTimeLimits(); // start and end
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan();
  Int_t nwlen = GetNwlen();
  Int_t nsyst = GetNsyst();
  Int_t nkind = GetFlagsSum(); // 1 or 2

  Char_t inputname[1024],outputname[1024];

  // find out the limit scans (start and end)
  Int_t n = 0;
  Char_t* scan_start, * scan_end;
  for(Int_t iscan = 0 ; iscan < nscan ; iscan++){
    if((n == 0) && (limits[iscan] == 1)){
      scan_start = scans[iscan];
      n = 1;
    } else {
      if((n == 1) && (limits[iscan] == 1)){
        scan_end = scans[iscan];
	n = 2;
      }
    }
  }

  printf("* Starting scan: %s\n", scan_start);
  printf("* Ending scan:   %s\n", scan_end);
  if(n > 2) printf("* Make sure the limits are set properly.\n");
  printf("--------------------------------------------------------\n");

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){

   ChooseKind(ikind);

    // arrays with values to be filled and written
    Float_t** PmtrX = new Float_t*[nwlen];
    Double_t** avgPmtrY = new Double_t*[nwlen];
    Double_t** avgPmtrTotErr = new Double_t*[nwlen];
    Double_t** avgPmtrStatErr = new Double_t*[nwlen];
    Double_t** avgPmtrSystErr = new Double_t*[nwlen];

    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

      // only one average textfile per wavelength
      sprintf(outputname, "%s%s_%s_%s_%s_%d%s",
	      GetODir(),GetOLabel(),scan_start,scan_end,
	      GetKind(),(Int_t)wlens[iwlen],GetOFormat());

      Int_t Nbin = 90; // maximum size
      PmtrX[iwlen] = new Float_t[Nbin];
      avgPmtrY[iwlen] = new Double_t[Nbin];
      avgPmtrTotErr[iwlen] = new Double_t[Nbin];
      avgPmtrStatErr[iwlen] = new Double_t[Nbin];
      avgPmtrSystErr[iwlen] = new Double_t[Nbin];

      // initialize array values before scan loop
      for(Int_t ibin = 0 ; ibin < Nbin ; ibin++){
	  PmtrX[iwlen][ibin] = 0.;
	  avgPmtrY[iwlen][ibin] = 1.; avgPmtrTotErr[iwlen][ibin] = 0.;
	  avgPmtrStatErr[iwlen][ibin] = 0.; avgPmtrSystErr[iwlen][ibin] = 0.;
      }

      for(Int_t iscan = 0 ; iscan < nscan ; iscan++){

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetODir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

	fQOCALoad->SetFullname(inputname);
	fQOCALoad->SetKeyword(); // data files, no keyword
	Char_t** tmpc = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
	Int_t dim = fQOCALoad->GetDimensionFast();
	Int_t nbin = fQOCALoad->GetNLines();
	Nbin = nbin; // save

	Float_t* xpmtr = fQOCALoad->ConverttoFloats(fQOCALoad->GetColumn(tmpc,1),nbin);
	Double_t* ypmtr = fQOCALoad->ConverttoDoubles(fQOCALoad->GetColumn(tmpc,2),nbin);
	Double_t* pmtrTotErr = fQOCALoad->ConverttoDoubles(fQOCALoad->GetColumn(tmpc,3),nbin);
	Double_t* pmtrStatErr = fQOCALoad->ConverttoDoubles(
		fQOCALoad->GetColumn(tmpc,4),nbin);
	Double_t* pmtrSystErr = fQOCALoad->ConverttoDoubles(
		fQOCALoad->GetColumn(tmpc,5),nbin);

	for(Int_t i = 0 ; i < dim ; i++) delete [] tmpc[i];
	delete [] tmpc;

	// first pass : fill in arrays
	for(Int_t ibin = 0 ; ibin < nbin ; ibin++){
          if(pmtrTotErr[ibin] > 0){
	    PmtrX[iwlen][ibin] = xpmtr[ibin];
	    avgPmtrY[iwlen][ibin] += (ypmtr[ibin])/(pow(pmtrTotErr[ibin],2));
	    avgPmtrTotErr[iwlen][ibin] += 1./(pow(pmtrTotErr[ibin],2));
	    avgPmtrStatErr[iwlen][ibin] += 1./(pow(pmtrStatErr[ibin],2));
	    avgPmtrSystErr[iwlen][ibin] += 1./(pow(pmtrSystErr[ibin],2));
	  }
	} // for(ibin)

	delete xpmtr; delete ypmtr;
	delete pmtrTotErr; delete pmtrStatErr; delete pmtrSystErr;

      } // for(iscan)

      // ------------------------------------
      // prepare for output

      // output : multiple columns : Bins - Value - Errors
      FILE* fpmtr = fopen(outputname,"w");
      fprintf(fpmtr,"%s\t%10s\t%10s\t%10s\t%10s\t%10s",
      	"# xpmtr", "ypmt","pmtrTotErr","pmtrStatErr","pmtrSystErr","other systematics ...\n");

      // second pass : re-loop over bins to fill the output file
      for(Int_t ibin = 0 ; ibin < Nbin ; ibin++){
	if(avgPmtrTotErr[iwlen][ibin] > 0){
	  avgPmtrY[iwlen][ibin] *= 1./(avgPmtrTotErr[iwlen][ibin]);
	  avgPmtrTotErr[iwlen][ibin] = sqrt(1./(avgPmtrTotErr[iwlen][ibin]));
	  avgPmtrStatErr[iwlen][ibin] = sqrt(1./(avgPmtrStatErr[iwlen][ibin]));
	  avgPmtrSystErr[iwlen][ibin] = sqrt(1./(avgPmtrSystErr[iwlen][ibin]));
	}
	else{
	  avgPmtrY[iwlen][ibin] = 1.; avgPmtrTotErr[iwlen][ibin] = 0.;
	  avgPmtrStatErr[iwlen][ibin] = 0.; avgPmtrSystErr[iwlen][ibin] = 0.;
	}

	// fill averages in output
	fprintf(fpmtr,"%.1f\t%+10.5e\t%+10.5e\t%+10.5e\t%+10.5e\t",
		PmtrX[iwlen][ibin],avgPmtrY[iwlen][ibin],
		avgPmtrTotErr[iwlen][ibin],avgPmtrStatErr[iwlen][ibin],
		avgPmtrSystErr[iwlen][ibin]);
	for(Int_t isyst = 0 ; isyst < nsyst-1 ; isyst++){
	  fprintf(fpmtr,"% 10.5e\t", 0.0);
	}
	fprintf(fpmtr,"\n");
      } // for(ibin)

      // close the output file
      fclose(fpmtr);

      // release some memory
      delete PmtrX[iwlen];
      delete avgPmtrY[iwlen]; delete avgPmtrTotErr[iwlen];
      delete avgPmtrStatErr[iwlen]; delete avgPmtrSystErr[iwlen];

    } // for(iwlen)

    delete PmtrX; delete avgPmtrY; delete avgPmtrTotErr;
    delete avgPmtrStatErr; delete avgPmtrSystErr;
  } // for(ikind)

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractTimeWindows(Char_t* medium)
{
  SetMedium(medium);
  ExtractTimeWindows();

  return;
}

//______________________________________________________________________________
void QOCAExtract::ExtractTimeWindows()
{
  // Extract the Inverse Attenuation Lengths from the qocafit rootfiles
  // usually for available wavelengths, scans and fit type
  // found in the cardfile, as a function of the various time windows
  // fits that were done in QOCAFit.
  //

  // verifications header before extraction
  if(!(GetOLabel())){
    if(fDisplay){
      printf("In QOCAExtract::ExtractTimeWindows():\n");
      printf("Setting default output label to 'timewindows' \n");
    }
    SetOLabel("timewindows");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAExtract::ExtractTimeWindows():\n");
      printf("No choice of files have been made.\n");
      printf("Setting default files to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }

  printf("--------------------------------------------------------\n");
  printf("* In QOCAExtract::ExtractTimeWindows(): \n");
  printf("* Extracting Inverse Attenuation Lengths\n");
  printf("* as a function of the prompt time window width.\n");
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // set dimensions (fit type, number of wavelengths and scans)
  Int_t nscan = GetNscan(), nwlen = GetNwlen();
  Int_t nkind = GetFlagsSum(); // 1 or 2

  Char_t inputname[1024],outputname[1024];
  Char_t specialname[1024]; // special names for time windows
 
  // time window values (same as in QPath and QOCAFit)
  Int_t nsyst = 7; 
  Int_t tw[7] = {1,2,3,4,7,10,20};
  Float_t* Atten = new Float_t[nsyst];
  Float_t* Thiserr = new Float_t[nsyst];
  Float_t* Thisrel = new Float_t[nsyst];

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	sprintf(inputname, "%s%s_%s_%s_%d%s",
		GetIDir(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

	// reading the rootfile
	TFile* f = new TFile(inputname,"READ");

	// ------------------------------------------------------------
	// loop over time windows: hardcoded values!
	for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
	  Atten[isyst] = 0.0; Thiserr[isyst] = 0.0; Thisrel[isyst] = 0.0;
	  sprintf(specialname,"qocafit_tw%d",isyst);
	  fQOCAFit = (QOCAFit*) f->Get(specialname);
	  if(!fQOCAFit || !f){
	    Warning("ExtractTimeWindows","Input file not found. Skipping."); continue;
	  }
	  FindAttenuation(fQOCAFit);

	  // get the attenuation and the statistical error
          // for each time window.
          Atten[isyst] = GetAttenuation();
	  Thiserr[isyst] = GetAttenuationErr();
	  if(Atten[isyst] != 0) Thisrel[isyst] = (Thiserr[isyst])/(Atten[isyst]);
	  if(Atten[isyst] <= 0) Warning("ExtractTimeWindows",
				   "Negative attenuation in %s.",specialname);

	  fQOCAFit->Clear();
	  delete fQOCAFit;
        } // for(isyst)

	// ------------------------------------------------------------

	// output name
	sprintf(outputname, "%s%s%s_%s_%s_%d%s",
		GetODir(),GetMedium(),GetOLabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetOFormat());

	// ------------------------------------------------------------
	// Output : two columns : Value - Relative Value
	FILE* fatt = fopen(outputname,"w");

	fprintf(fatt,"# Time Window Half-Width (ns) - Values - Stat.Error - Relative.Err\n");
        for(Int_t isyst = 0 ; isyst < nsyst ; isyst++){
          sprintf(specialname,"qocafit_tw%d",isyst);
	  fprintf(fatt,"% 10.2d\t% 10.5e\t% 10.5e\t %10.5e\t#. Time Window %d (%s).\n",
	  	tw[isyst],Atten[isyst],Thiserr[isyst],Thisrel[isyst],
                isyst,specialname);
	}

        // close the output file
	fclose(fatt);
        f->Close(); delete f;

      } // for(iwlen)
    } // for(iscan)
  } // for(ikind)
  delete [] Atten; delete [] Thiserr; delete [] Thisrel;

  return;
}
