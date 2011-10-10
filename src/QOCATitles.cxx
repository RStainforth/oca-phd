/*************************************************************************
 * Copyright(c) 2004, The SNO Software Project, All rights reserved.     *
 * Authors:  Olivier Simard, Ranpal Dosanjh, Bryce Moffat                *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include "QOCATitles.h"
#include "TMath.h"

ClassImp(QOCATitles)
;
//
//
// SNOMAN titles files generator from QOCAFit results.
//
// --------------------
// Major update: use PMTR from the fit, and extrapolate using the SNOMAN function
// beyond 36 or 40 degrees.  Separate the SNOMAN function into fit_titles_helper.C
// and load it separately.
// 16-Mar-2001 - Bryce Moffat
//
// --------------------
// Change the way the SNOMMAN PMTR extrapolation is done beyond the QOCAFit
// measurement: make it agree over the last 4 degrees.  The thesis production
// QOCAFit's are done with 90 bins 1 degree wide, central value (0-0.5, 0.5-1.5,
// 1.5-2.5, etc.) and with the 0 degree bin anchored at PMTR=1.
//
// These "final" fits also have Acrylic fixed at the smoothed Davis values
// (from ~/sno/oca/atten/sno_atten.C: expopol function) and H2O is fit as an
// independent model parameter.  No need to correct the H2O attenuation any more!
// 6-Apr-2001 - Bryce Moffat
//
// --------------------
// Modified loading of data to accommodate log file input, when the QOCAFit
// streamer for ROOT version 3.xx isn't working.  Requires qoca_summary.C as
// another "helper" macro.
// 28-Nov-2001 - Bryce Moffat
//
// --------------------
// Modified to QOCATitles class format to accomodate an automated titles files
// production using a simple macro in ROOT or an executable.
// 20-Aug-2004 - Olivier Simard, Ranpal S. Dosanjh
//
//
//______________________________________________________________________________
QOCATitles::QOCATitles(Char_t* optionfile, Bool_t mode, Bool_t display, Char_t* edirectory, Char_t* tdirectory):QOCAAnalysis(optionfile, mode, display)
{
  //
  // QOCATitles constructor
  //

  if(edirectory != NULL){
    SetIDir(edirectory); // extraction text files directory
  }

  if(tdirectory != NULL){
    SetODir(tdirectory); // titles files directory
  }

  // for most banks it is only one scan at the time
  fScanTitles = GetScanArray()[0];
  fFunny337 = kFALSE;
  fNextrapbin = 6;
  fNangle = 90;  // should be reset with (fAngleparmax - fAngleparmin + 1)
  fNlambda = 50; // should be reset with (fLambdaparmax - fLambdaparmin + 1)/10
  fNdlam = 10; // 10 max
  fNpol = 2; // 10 max
  fNpolsnoman = 10;
  fNormbin = 10; // normalization bin

  // media codes
  fNphase = 0;
  fMedium_code_hist = -1;
  fMedium_code_d2o = -1;
  fMedium_code_salt = 200;

  // time stamp at the end of initialization
  TDatime *datetime = new TDatime();
  fTimestamp = (Char_t*) datetime->AsString();
  printf("===================================================================\n");
  printf("QOCATitles: Generating titles for %s on %s\n",fScanTitles,fTimestamp);
  printf("===================================================================\n");
  delete datetime;

  fQOCALoad = new QOCALoad();
  fUsefit = kFALSE; // set default to use extracted textfiles
  fFnampmtresp = NULL; // set to nothing
  fOffset = 5;
  fRootfile = "temp";
  fNrootfile = 0;

  // warning for attempt to create banks for only few wavelengths
  if(GetNwlen() != 6){
    printf("Warning in QOCATitles:\n");
    printf("\tStandard banks must be created for all SNO wavelengths!\n");
  }
}

//______________________________________________________________________________
QOCATitles::~QOCATitles()
{
  // QOCATitles destructor

  if(fQOCALoad) delete fQOCALoad;
  printf("QOCATitles::~QOCATitles(): There were %d root files written as quality checks.\n",fNrootfile);

}

//______________________________________________________________________________
Float_t QOCATitles::ValueFromBank(Int_t i, Int_t j, QBank *bank, Int_t offset)
{

  Int_t link_access = i*GetNang() + j;
  return bank->rcons(link_access+offset);

}

//______________________________________________________________________________
Bool_t QOCATitles::FillFromBank(TH2F *hist, QBank *bank, Int_t offset)
{

  Int_t nlam = GetNlam();
  Int_t nang = GetNang();

  if(hist->GetNbinsX() != nlam) return 0;
  if(hist->GetNbinsY() != nang) return 0;

  for(Int_t ilam = 0 ; ilam < nlam ; ilam++){
	for(Int_t iang = 0 ; iang < nang ; iang++){
		Int_t xbin = ilam+1;
		Int_t ybin = iang+1;
		Float_t value = ValueFromBank(ilam,iang,bank,offset);
		hist->SetCellContent(xbin, ybin, value);
	}
  }

  return 1;
}

//______________________________________________________________________________
Bool_t QOCATitles::FillFromNormal(TH1F *des, TH2F *src)
{

  Int_t nlam = src->GetNbinsX();

  if(des->GetNbinsX() != nlam) return 0;

  printf("In QOCATitles::FillFromNormal(): Normalizing with bin %d.\n",fNormbin);

  for(Int_t ilam = 0 ; ilam < nlam ; ilam++){
	Int_t xbin = ilam+1;
	Float_t value = src->GetCellContent(xbin,fNormbin);
	des->SetBinContent(xbin,value);
  }

  return 1;
}

//______________________________________________________________________________
void QOCATitles::Diagnostic()
{
  // Ranpal's diagnostic function using the 3 previous functions. The snoman
  // name file (fFnampmtresp) has to be set prior to this call.

  printf("In QOCATitles::Diagnostic() : Producing basic histograms.\n");
  Int_t offset_pmtr = GetOffset();
  Int_t offset_refl = offset_pmtr + ((GetNang())*(GetNlam()));

  // Open titles file and put into bank
  // ----------------------------------
  QTitles tpmtr(GetFnamPmtResp(),"PMTR",1);
  QBank *pmtr = tpmtr.GetBank();

  // new naming scheme for systematics
  Char_t name[128];

  // Get PMT Response and put into histo
  // -----------------------------------
  if(GetSystIndex() == 0) sprintf(name,"resp_deg");
  else sprintf(name,"resp_deg_%.2d",GetSystIndex());
  TH2F *resp_deg = new TH2F(name,"SNOMAN PMT Response",
				GetNlam(),22,72,
				GetNang(),0,90);

  FillFromBank(resp_deg,pmtr,offset_pmtr);


  // Get PMT Reflectivity and put into histo
  // ---------------------------------------
  if(GetSystIndex() == 0) sprintf(name,"refl_deg");
  else sprintf(name,"refl_deg_%.2d",GetSystIndex());
  TH2F *refl_deg = new TH2F(name,"SNOMAN PMT Reflect",
				GetNlam(),22,72,
				GetNang(),0,90);

  FillFromBank(refl_deg,pmtr,offset_refl);


  // Get PMT quantum efficiency from response histo
  // ----------------------------------------------
  if(GetSystIndex() == 0) sprintf(name,"qesnoman");
  else sprintf(name,"qesnoman_%.2d",GetSystIndex());
  TH1F *qesnoman = new TH1F(name,"PMT QE versus Wavelength",
				GetNlam(), 22*10, 72*10);

  // default normalization bin is 10th bin (instead of 1 ??)
  FillFromNormal(qesnoman,resp_deg);

  // Output into file if nominal
  // ----------------
  if(GetSystIndex() == 0){
    Char_t rootfile[1024];
    sprintf(rootfile, "%s_%.2d.root",fRootfile,fNrootfile);
    TFile *fout = new TFile(rootfile,"RECREATE");
    resp_deg->Write();
    refl_deg->Write();
    qesnoman->Write();
    fout->Close();
    delete fout;
    fNrootfile++;
  }

  // Reflectivity histogram set from diagnostic function
  SetQESnoman(qesnoman); // quantum efficiency
  SetReflTH2F(refl_deg); // reflectivity

  return;
}

//______________________________________________________________________________
void QOCATitles::BinshiftTH1F(TH1F *hist, Float_t shift, Float_t keepval)
{
  // Shift the values in hist by a fraction (shift) of a bin.  This is done
  // with a linear interpolation between bins.  If either of the bins used in
  // the interpolation are equal to keepval, then the contents of the bin are
  // unchanged.  For the PMTR bin-shifting, his conditional forces the
  // normalization bin to be one, and protects edge bins (bins beside those
  // with no data) from spurious slopes.

  Int_t ibin;
  Int_t nbin = hist->GetNbinsX();
  TString hnam = hist->GetName();
  hnam += "_temp";
  TH1F *temp = (TH1F *)hist->Clone(hnam);

  if ( (shift > 1.0) || (shift < -1.0) ) {
	cout << "Error in QOCATitles::BinshiftTH1F: Desired shift outside range.  Ignoring." << endl;
	return;
  }

  for (ibin=0;ibin<nbin;ibin++) {
	Int_t xbin = ibin+1;
	Int_t Xbin = (shift>0) ? xbin+1 : xbin-1;

	Float_t thisval = temp->GetBinContent(xbin);
	Float_t nextval = temp->GetBinContent(Xbin);
	Float_t newval;

	if (thisval==keepval || nextval==keepval) newval = keepval;
	else newval = (1-shift)*nextval + shift*thisval;

	hist->SetBinContent(xbin,newval);
  }

  delete temp;

  Float_t xmin = hist->GetXaxis()->GetXmin() + shift;
  Float_t xmax = hist->GetXaxis()->GetXmax() + shift;
  hist->GetXaxis()->SetLimits(xmin,xmax);

  return;
}

//______________________________________________________________________________
Int_t QOCATitles::FindLastBin(TH1F *hist, Float_t keepval)
{

  // Find the last bin that contains data.  This is the last bin that does not
  // contain keepval.

  Int_t nbin = hist->GetNbinsX();
  Int_t xbin = nbin;
  while (hist->GetBinContent(xbin) == keepval) xbin--;

  return xbin;
}

//______________________________________________________________________________
Float_t QOCATitles::FindScale(TH1F *numer, TH1F *denom, Int_t nbadbin, Int_t lastbin)
{

  // Determine the value by which denom must be scaled so that the nbadbin
  // bins up to lastbin have the same average as in numer.

  Int_t ibin;
  Int_t nbin = numer->GetNbinsX();
  if (denom->GetNbinsX() != nbin) {
	cout << "Error in QOCATitles::FindScale(): Incompatible histograms.  Ignoring." << endl;
	return 0.;
  }

  Int_t edgebin = lastbin - nbadbin;
  Float_t avgnum = 0;
  Float_t avgden = 0;
  for (ibin=edgebin;ibin<lastbin-1;ibin++) { // lastbin-1 to throw out last bin
	Int_t xbin = ibin+1;

	avgnum += numer->GetBinContent(xbin);
	avgden += denom->GetBinContent(xbin);
  }

  if (!avgnum || !avgden) {
	cout << "Error in QOCATitles::FindScale(): Bad scale result: ";
	cout << "numer average = " << avgnum << " and ";
	cout << "denom average = " << avgden << ".  ";
	cout << "Returning 0." << endl;
	return 0.;
  }

  return avgnum/avgden;
}

//______________________________________________________________________________
Float_t QOCATitles::FindScale(TH1F *numer, TH1F* denom, Int_t nbins, Float_t lastx)
{
  // Overloaded so that user can provide the x-value of the last bin rather
  // than the bin number.

  Int_t lastbin = numer->GetXaxis()->FindBin(lastx);
  return FindScale(numer,denom,nbins,lastbin);
}

//______________________________________________________________________________
void QOCATitles::ExtrapolateTH1F(TH1F *start, TH1F* finish, Int_t edgebin)
{
  // Extrapolate the final bins of start using finish.  All bins in start up
  // to and including edgebin are kept.  The remaining bins come from finish.

  Int_t ibin;
  Int_t nbin = start->GetNbinsX();

  if (finish->GetNbinsX() != nbin) {
	cout << "Error in QOCATitles::ExtrapolateTH1F: Incompatible histograms.  Ignoring." << endl;
	return;
  }

  for (ibin=edgebin;ibin<nbin;ibin++) {
	// edgebin is kept.
	Int_t xbin = ibin+1;
	Float_t newval = finish->GetBinContent(xbin);
//	Float_t newval = finish->GetBinContent(xbin);
	start->SetBinContent(xbin,newval);
  }

  return;
}

//______________________________________________________________________________
void QOCATitles::ExtrapolateTH1F(TH1F *start, TH1F* finish, Float_t edgex)
{
  // Overloaded so that user can provide the x-value of the edge bin rather
  // than the bin number.

  Int_t edgebin = start->GetXaxis()->FindBin(edgex);
  ExtrapolateTH1F(start,finish,edgebin);
  return;
}

//______________________________________________________________________________
void QOCATitles::OmnipolateTH2F(TH2F *hist, Int_t nseed, Float_t *xseed, TH1F **hseed)
{
  // Interpolate and extrapolate TH1F's to fill a TH2F.  The nseed histograms
  // in the array hseed are the TH1F's.  They are slices of the TH2F; the
  // latter's y-axis is the same of the formers' x-axes.  The x-values of the
  // TH1F's are contained in the xseed array.
  //
  // A linear interpolation is made of two TH1F's for TH2F x-bins between
  // them.  For x-bins outside the TH1F's, a linear extrapolation is made
  // using the two edge TH1F's.

  Int_t ibinx, ibiny;
  Int_t nbinx = hist->GetNbinsX();
  Int_t nbiny = hist->GetNbinsY();

  Int_t iseed;

  for (iseed=0;iseed<nseed;iseed++) {
  	if (nbiny != hseed[iseed]->GetNbinsX()) {
		cout << "Error in QOCATitles::OmnipolateTH2F: inconsistent histograms.  Ignoring." << endl;
		return;
  	}
  }

  for (ibinx=0;ibinx<nbinx;ibinx++) {
	Int_t xbin = ibinx+1;
	Float_t x = hist->GetXaxis()->GetBinCenter(xbin);

	// Find the two TH1F's used in the inter/extrapolation.
	for (iseed=0;iseed<nseed;iseed++) {
		if (x < xseed[iseed]) break;
	}
	Int_t jseed = (iseed==0)     ? iseed   : // Extrapolate low bins
		      (iseed==nseed) ? iseed-2 : // Extrapolate high bins
				       iseed-1 ; // Interpolate all other bins
	Float_t xlo = xseed[jseed];
	Float_t xhi = xseed[jseed+1];

	// Do the inter/extrapolation
	Float_t dist = (x-xlo)/(xhi-xlo);
	for (ibiny=0;ibiny<nbiny;ibiny++) {
		Int_t ybin = ibiny+1;
		Float_t vallo = (hseed[jseed])->GetBinContent(ybin);
		Float_t valhi = (hseed[jseed+1])->GetBinContent(ybin);
		
		Float_t val = (1-dist)*vallo + dist*valhi;
		if (val<0) val = 0;
		hist->SetBinContent(xbin,ybin,val);
	}
  }

  return;
}

//______________________________________________________________________________
void QOCATitles::XScaleTH2F(TH2F *hist, TH1F *hscale)
{

  // Scale hist by hscale.  hist and hscale have the same x-axis.  All hist
  // bins in y for each hist bin in x is scaled by the same number: the
  // corresponding hscale bin in x.

  Int_t ibinx, ibiny;
  Int_t nbinx = hist->GetNbinsX();
  Int_t nbiny = hist->GetNbinsY();

  if (hscale->GetNbinsX() != nbinx) {
	cout << "Error in QOCATitles::XScaleTH2F: Incompatible histograms.  Ignoring." << endl;
	return;
  }

  for (ibinx=0;ibinx<nbinx;ibinx++) {
	Int_t xbin = ibinx+1;
	Float_t xscale = hscale->GetBinContent(xbin);
	Float_t norm = hist->GetBinContent(xbin,1);

	for (ibiny=0;ibiny<nbiny;ibiny++) {
		Int_t ybin = ibiny+1;
		Float_t val = xscale*hist->GetBinContent(xbin,ybin);
		if (norm!=1) val /= norm;
		hist->SetBinContent(xbin,ybin,val);
	}
  }

  return;
}

//______________________________________________________________________________




//______________________________________________________________________________
void QOCATitles::LoadAttenuations()
{
  //
  // Loads the attenuations from the text files for d2o and h2o
  // for a single scan only
  //

  printf("------------------------------------\n");
  printf("Loading Attenuation Coefficients ...\n");

  // load wavelengths
  Float_t* wlens = GetWlens();

  Double_t* attensd2o = new Double_t[GetNwlen()];
  Double_t* attensacr = new Double_t[GetNwlen()]; 
  Double_t* attensacr_loaded = GetAttacrLoaded(); // set in executable 
  Double_t* attensh2o = new Double_t[GetNwlen()];
  
  Char_t extractd2o[1024],extracth2o[1024];

  for(Int_t iwlen = 0 ; iwlen < GetNwlen() ; iwlen++){
    Int_t wlen = (Int_t) wlens[iwlen]; // reassignment as an Int_t from Float_t
    // build the full path textfile name
    sprintf(extractd2o,"D2O%s_%s_%s_%d%s",
      GetILabel(),fScanTitles,GetKind(),wlen,GetIFormat());
    sprintf(extracth2o,"H2O%s_%s_%s_%d%s",
      GetILabel(),fScanTitles,GetKind(),wlen,GetIFormat());

    // access the textfiles and keep the data to plot
    fQOCALoad->SetFullname(GetIDir(), extractd2o);
    Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t dim1 = fQOCALoad->GetDimensionFast();
    // convert the Char_t* format to Float_t
    attensd2o[iwlen] = (Double_t) atof(tmp1[0]); // first column first line

    fQOCALoad->SetFullname(GetIDir(), extracth2o);
    Char_t** tmp2 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());

    attensh2o[iwlen] = (Double_t) atof(tmp2[0]); // first column first line
    Int_t dim2 = fQOCALoad->GetDimensionFast();
    // remove Rayleigh scattering fraction
    attensd2o[iwlen] -= fRayfracd2o*(GetRayleighd2om()[iwlen]);
    attensh2o[iwlen] -= fRayfrach2o*(GetRayleighh2oq()[iwlen]);
    attensacr[iwlen] = attensacr_loaded[iwlen] - fRayfracacr*(GetRayleighacrq()[iwlen]);

    // clear some space
    for(Int_t i = 0 ; i < dim1 ; i++) delete [] tmp1[i];
    for(Int_t j = 0 ; j < dim2 ; j++) delete [] tmp2[j];
    delete [] tmp1; delete [] tmp2;

  } // for(iwlen)

  // keep in memory for command files
  SetAttd2o(attensd2o);
  SetAttacr(attensacr);
  SetAtth2o(attensh2o);

  // print to screen
  for(Int_t iwlen = 0 ; iwlen < GetNwlen() ; iwlen++){
    printf(" at %d nm: %10.3e %10.3e %10.3e\n", (Int_t) wlens[iwlen],
      GetAttd2o()[iwlen], GetAttacr()[iwlen], GetAtth2o()[iwlen]);
  }

  printf("------------------------------------\n");
  return;
}

//______________________________________________________________________________
void QOCATitles::LoadPMTR(Char_t* snomanpmtrfile)
{
  // Loads the PMT Angular Response from text files
  // for a single scan only
  //
  // pmtrsnoman is the scan equivalent of the textfiles produced from snoman
  //

  printf("-----------------------------------\n");
  printf("QOCATitles: Loading PMT Angular Response ...\n");

  // load dimensions
  Int_t nlam = GetNlam();
  Int_t nang = GetNang();
  Int_t nwlen = GetNwlen();

  // load wavelengths
  Float_t* wlens = GetWlens();

  TH1F** pmtr = new TH1F *[nwlen];
  TH1F** snoman = new TH1F *[nwlen];
  TH1F* qesnoman = GetQESnoman();

  // create filenames and histogram names
  Char_t extpmtrqoca[1024],hnameqoca[1024],extpmtrsnom[1024],hnamesnom[1024];
  Char_t name[128];

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    Int_t wlen = (Int_t) wlens[iwlen]; // reassignment as an Int_t from Float_t
    // build the full path textfile name
    sprintf(extpmtrqoca,"%s_%s_%s_%d%s",
      GetILabel(),fScanTitles,GetKind(),wlen,GetIFormat());
    sprintf(extpmtrsnom,"%s_%s_%s_%d%s",
      GetILabel(),snomanpmtrfile,GetKind(),wlen,".txt"); // hardcoded
    sprintf(hnameqoca,"pmtr_%d_%.2d",wlen,GetSystIndex());
    sprintf(hnamesnom,"snoman_%d_%.2d",wlen,GetSystIndex());

    // access the textfiles and keep the data to plot
    fQOCALoad->SetFullname(GetIDir(), extpmtrqoca);
    Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t nlines1 = fQOCALoad->GetNLines();
    Int_t dim = fQOCALoad->GetDimensionFast();

    // pmtr data is second column
    Float_t* pmtrqoca = fQOCALoad->ConverttoFloats(
			fQOCALoad->GetColumn(tmp1,2),nlines1);
    for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
    delete [] tmp1;

    // access the textfiles and keep the data to plot
    fQOCALoad->SetFullname(GetIDir(), extpmtrsnom);
    Char_t** tmp2 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t nlines2 = fQOCALoad->GetNLines();
    dim = fQOCALoad->GetDimensionFast();
    // pmtr data is second column
    Float_t* pmtrsnom = fQOCALoad->ConverttoFloats(
			fQOCALoad->GetColumn(tmp2,2),nlines2);
    for(Int_t i = 0 ; i < dim ; i++) delete [] tmp2[i];
    delete [] tmp2;

    // assign data to histograms
    pmtr[iwlen] = new TH1F(hnameqoca,"PMTR",90,-0.5,89.5);
    snoman[iwlen] = new TH1F(hnamesnom,"SNOMAN PMTR",90,0.,90.);

    for(Int_t i = 0 ; i < 90 ; i++){
      pmtr[iwlen]->SetBinContent(i+1,pmtrqoca[i]);
      snoman[iwlen]->SetBinContent(i+1,pmtrsnom[i]);
    }

    delete [] pmtrqoca;
    delete [] pmtrsnom;

  } // for(iwlen)

  // -----------------------------
  // actual operations below

  Int_t nbadbin = GetNextrapbin();

  // Shift qoca pmtr to match snoman bins
  for (Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    BinshiftTH1F(pmtr[iwlen],0.5);
  }

  // Extrapolate in angle
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    Int_t lastbin = FindLastBin(pmtr[iwlen]);
    Float_t scale = FindScale(pmtr[iwlen],snoman[iwlen],nbadbin,lastbin);
    snoman[iwlen]->Scale(scale);
    ExtrapolateTH1F(pmtr[iwlen],snoman[iwlen],lastbin-nbadbin);
  }

  // Omnipolate over all wavelengths
  if(GetSystIndex() == 0) sprintf(name,"pmtr_fit");
  else sprintf(name,"pmtr_fit_%.2d",GetSystIndex());
  TH2F *pmtr_fit = new TH2F(name,"PMTR from Fit",nlam,220,720,nang,0,90);
  OmnipolateTH2F(pmtr_fit, nwlen, wlens, pmtr);

  // Scale the x-axis by the quantum efficiency
  TH2F *pmtr_fitqe = (TH2F *)pmtr_fit->Clone("pmtr_fitqe");
  XScaleTH2F(pmtr_fitqe,qesnoman);

  if(GetSystIndex() == 0){
    Char_t rootfile[1024];
    sprintf(rootfile, "%s_%.2d.root",fRootfile,fNrootfile);
    TFile* fout = new TFile(rootfile,"RECREATE");
    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){pmtr[iwlen]->Write();}
    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){snoman[iwlen]->Write();}
    qesnoman->Write();
    pmtr_fit->Write();
    pmtr_fitqe->Write();
    fout->Close();
    delete fout;
    fNrootfile++;
  }

  // Set the pmtr histogram to be written in titles files
  SetPmtrTH2F(pmtr_fitqe);

  delete [] pmtr;
  delete [] snoman;
  printf("------------------------------------\n");
  return;
}

//______________________________________________________________________________
void QOCATitles::LoadPolyTerms(Char_t** files, Int_t nphase_def)
{
  //
  // Loads the polynomial terms from a single text file
  //

  // dimension in wavelength
  Int_t nwlen = GetNwlen();
  // dimension in polynomials
  Int_t npol = GetNpol();
  // number of different phases
  Int_t nphase = GetNPhase();

  // array of arrays of arrays
  Float_t*** poly = new Float_t**[nphase];

  // initialize the zero values, whatever the actual number of phases
  for(Int_t iphase = 0 ; iphase < nphase_def ; iphase++){
    poly[iphase] = new Float_t*[nwlen];
    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
      poly[iphase][iwlen] = new Float_t[npol];
      for(Int_t ipol = 0 ; ipol < npol ; ipol++){
        poly[iphase][iwlen][ipol] = 0.;
      }
    }
  }

  // create filename
  Char_t drift[1024];

  // loop over to fill the terms
  for(Int_t iphase = 0 ; iphase < nphase ; iphase++){

    sprintf(drift,"%s_%s_%s%s",GetILabel(),files[iphase],GetKind(),GetIFormat());

    // access the textfiles
    fQOCALoad->SetFullname(GetIDir(), drift);
    Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t ncol = fQOCALoad->GetNColumns();
    Int_t dim = fQOCALoad->GetDimensionFast();

    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
      // on one line, first column is wlen, other are poly
      Float_t* terms = fQOCALoad->ConverttoFloats(
				fQOCALoad->GetLine(tmp1,iwlen+1),ncol);

      for(Int_t ipol = 0 ; ipol < npol ; ipol++){
        // fill array with data
        poly[iphase][iwlen][ipol] = terms[ipol+1]; // avoid wlen (first element)

        // change salt attenuation zeroth polynomial
        if(ipol==0){
          poly[2][iwlen][ipol] -= fRayfracd2o * fRayleighd2om[iwlen];
        }
      }
    } // for(iwlen)
    for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
    delete [] tmp1;
  } // for(iphase)

  // set the polynomial terms for all phases
  SetPoly(poly);

  return;
}

//______________________________________________________________________________
void QOCATitles::LoadEfficiencies()
{
  // Load PMT efficiency files created by QOCAExtract
  // variability with incidence angle from qocafit model QOCATree (optica)
  // found in the cardfile using QOCAFit::GetPmteffm()
  // This function is an exception to the usual QOCAExtract::Extract
  // function since it extracts from a QOCATree (not the fit).

  printf("-----------------------------------\n");
  printf("QOCATitles: Loading PMT Relative Efficiencies ...\n");

  QPMTxyz* qpmt = new QPMTxyz("read"); // PMT array info

  if(GetFlagsSum() > 1){
    Error("LoadEfficiencies","Only one fit type is requested not %d.",GetFlagsSum());
    exit(-1);
  }
  // load dimensions
  Int_t nwlen = GetNwlen(); Float_t* wlens = GetWlens();
  Char_t* scan = fScanTitles;
  Int_t si = GetSystIndex();
  Char_t name[128], title[128];
  Char_t inputname[1024];

  // declare pmt position histograms (x,y,z)
  TH2F** hposx = new TH2F*[nwlen];
  TH2F** hposy = new TH2F*[nwlen];
  TH2F** hposz = new TH2F*[nwlen];

  if(GetSystIndex() == 0) sprintf(name,"eff_wlen");
  else sprintf(name,"eff_wlen_%.2d",GetSystIndex());
  TH2F* h2eff = new TH2F(name,"PMT Relative Efficiencies vs Wavelength",
    10000,1,10000,6,0,6);

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    // position histograms
    sprintf(name,"pmtx_%s_%d_%.2d",scan,(Int_t)wlens[iwlen],si);
    sprintf(title,"PMT Efficiencies vs X");
    hposx[iwlen] = new TH2F(name,title,100,-900,900,100,0,2);
    sprintf(name,"pmty_%s_%d_%.2d",scan,(Int_t)wlens[iwlen],si);
    sprintf(title,"PMT Efficiencies vs Y");
    hposy[iwlen] = new TH2F(name,title,100,-900,900,100,0,2);
    sprintf(name,"pmtz_%s_%d_%.2d",scan,(Int_t)wlens[iwlen],si);
    sprintf(title,"PMT Efficiencies vs Z");
    hposz[iwlen] = new TH2F(name,title,100,-900,900,100,0,2);

    sprintf(inputname, "%s%s_%s_%s_%d%s",
      GetIDir(),GetILabel(),scan,GetKind(),(Int_t)wlens[iwlen],GetIFormat());
    fQOCALoad->SetFullname(inputname);
    fQOCALoad->SetKeyword();
    Char_t** array = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t nlines = fQOCALoad->GetNLines();
    Int_t dim = fQOCALoad->GetDimensionFast();
    // pmt numbers
    Int_t* pmt = fQOCALoad->ConverttoIntegers(
      fQOCALoad->GetColumn(array,1),nlines);
    // averaged efficiencies
    Float_t* eff = fQOCALoad->ConverttoFloats(
      fQOCALoad->GetColumn(array,2),nlines);
    for(Int_t i = 0 ; i < dim ; i++) delete [] array[i];
    delete [] array;

    // deviation from mean (= 1.)
    Float_t sigma = 0.;
    Int_t Npmt = 0;
    for(Int_t j = 0 ; j < nlines  ; j++){
      if(eff[j] > 0 && eff[j] != 1.){ // exclude 1.0 from spread calculation
        sigma += pow((eff[j] - 1.0),2);
	Npmt++;
      }
      // initialize values in histo
      h2eff->SetCellContent(j+1, iwlen+1, 1.);
    }
    sigma = sqrt(sigma/(Float_t)Npmt);
    printf(" at %d nm: Efficiency distribution spread (sigma) = %f\n",
           (Int_t)wlens[iwlen],sigma);

    // save into histogram
    for(Int_t j = 0 ; j < nlines ; j++){

      // prepare for final efficiency average histogram
      if(!qpmt->IsNormalPMT(pmt[j])) continue;
      Int_t jsnoman = (Int_t) qpmt->GetSnomanNo(pmt[j]);
      if(jsnoman < 1 || jsnoman > 9522) continue;

      if((eff[j] > 0) && (eff[j] != 1.0) && (fabs(eff[j] - 1.0) < 3.0*sigma)){
        h2eff->SetCellContent(jsnoman, iwlen+1, eff[j]);
	// fill efficiencies vs pmt position
	hposx[iwlen]->Fill(qpmt->GetX(pmt[j]),eff[j]);
	hposy[iwlen]->Fill(qpmt->GetY(pmt[j]),eff[j]);
	hposz[iwlen]->Fill(qpmt->GetZ(pmt[j]),eff[j]);
      }
      else h2eff->SetCellContent(jsnoman, iwlen+1, 1.);
    }

    delete [] pmt; delete [] eff;

  }

  if(GetSystIndex() == 0){
    Char_t rootfile[1024];
    sprintf(rootfile, "%s_%.2d.root",fRootfile,fNrootfile);
    TFile* fout = new TFile(rootfile,"RECREATE");
    h2eff->Write();
    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
      hposx[iwlen]->Write();
      hposy[iwlen]->Write();
      hposz[iwlen]->Write();
    }
    fout->Close();
    delete fout;
    fNrootfile++;
  }
  // Set the efficiencies histogram to be written in titles files
  SetEfficiencies(h2eff);

  delete qpmt;
  printf("-----------------------------------\n");
  return;
}

//______________________________________________________________________________
void QOCATitles::LoadLBdist()
{
  // Loads the 12 x 36 = 432 parameters of the LB distribution
  // and saves it to an histogram that serves to produce
  // the titles files.
  // Does the same with the LB mask function.

  printf("-----------------------------------\n");
  printf("QOCATitles: Loading Laserball Distribution ...\n");

  // load dimensions
  Int_t nwlen = GetNwlen(); Float_t* wlens = GetWlens();
  Char_t* scan = fScanTitles;
  Int_t si = GetSystIndex();
  Int_t nphi = 36, ntheta = 12;

  // one distribution per wavelength
  TH2F** lbdist = new TH2F*[nwlen];
  Double_t** lbpoly = new Double_t*[nwlen];

  Char_t inputname[1024], histname[128];

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    sprintf(histname, "fLaserdist_%s_%d_%.2d",scan,(Int_t)wlens[iwlen],si);
    // as defined in QOCAFit
    lbdist[iwlen] = new TH2F(histname,"Laserball angular distribution",
			nphi,0,2.0*TMath::Pi(),ntheta,-1,1);

    sprintf(inputname, "%s%s_%s_%s_%d%s",
      GetIDir(),GetILabel(),scan,GetKind(),(Int_t)wlens[iwlen],GetIFormat());
    fQOCALoad->SetFullname(inputname);
    fQOCALoad->SetKeyword();
    Char_t** array = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t dim = fQOCALoad->GetDimensionFast();
    Int_t nlines = fQOCALoad->GetNLines();
    if(nlines != 432){
      Warning("LoadLBdist","Number of parameters don't match: %d vs %d\n",nlines,432);
    }

    Double_t* dist = fQOCALoad->ConverttoDoubles(
    	fQOCALoad->GetColumn(array,3),nlines);
    for(Int_t i = 0 ; i < dim ; i++) delete [] array[i];
    delete [] array;

    for(Int_t itheta = 0 ; itheta < ntheta ; itheta++){
      for(Int_t iphi = 0 ; iphi < nphi ; iphi++){
        lbdist[iwlen]->SetCellContent(iphi+1,itheta+1,dist[itheta*nphi+iphi]);
      }
    }

    // scale with the maximum value:
    // if values are > 1 SNOMAN is terribly slow ...
    lbdist[iwlen]->Scale(1./lbdist[iwlen]->GetMaximum());

    delete [] dist;
  }

  // now switch to parameters of the mask function
  SetILabel("lbpoly");

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    sprintf(inputname, "%s%s_%s_%s_%d%s",
      GetIDir(),GetILabel(),scan,GetKind(),(Int_t)wlens[iwlen],GetIFormat());
    fQOCALoad->SetFullname(inputname);
    fQOCALoad->SetKeyword();
    Char_t** array2 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t dim = fQOCALoad->GetDimensionFast();
    Int_t nlines2 = fQOCALoad->GetNLines();
    lbpoly[iwlen] = fQOCALoad->ConverttoDoubles(
		    fQOCALoad->GetColumn(array2,1),nlines2);
    for(Int_t i = 0 ; i < dim ; i++) delete [] array2[i];
    delete [] array2;
  }

  // set the histograms and arrays
  SetLBdist(lbdist);
  SetLBpoly(lbpoly);

  if(GetSystIndex() == 0){
    Char_t rootfile[1024];
    sprintf(rootfile, "%s_%.2d.root",fRootfile,fNrootfile);
    TFile* fout = new TFile(rootfile,"RECREATE");
    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){lbdist[iwlen]->Write();}
    fout->Close();
    delete fout;
    fNrootfile++;
  }
  printf("-----------------------------------\n");
  return;
}

//______________________________________________________________________________
// FUNCTIONS TO WRITE THE INFO IN SNOMAN TITLES AND COMMAND FILES
//______________________________________________________________________________
//______________________________________________________________________________

//______________________________________________________________________________
void QOCATitles::DatabaseHeader(FILE* file)
{
  // Writes this standard unspecified validity range to all banks
  // except those that need time specifications (ex: drifts).

  fprintf(file,"#.\n");
  fprintf(file,"#.    Standard Database Header\n");
  fprintf(file,"#.\n");
  fprintf(file,"19750101        0 20380517 03331900  #.  1..4   Intrinsic validity\n");
  fprintf(file,"       0        0        0           #.  5..7   Data type, Task type, Format no.\n");
  fprintf(file,"       0        0        0           #.  8..10  Creation Date, Time, Source Id.\n");
  fprintf(file,"19750101        0 20380517 03331900  #. 11..14  Effective validity\n");
  fprintf(file,"       0        0                    #. 15..16  Entry Date Time\n");
  fprintf(file,"4*0                                  #. 17..20  Spare\n");
  fprintf(file,"10*0                                 #. 21..30  Temporary data (not in database)\n");
  fprintf(file,"#.\n");
  fprintf(file,"#.    End of Standard Database Header\n");
  fprintf(file,"#.\n");

  return;
}
//______________________________________________________________________________
void QOCATitles::MediaTitles()
{
  //
  // Produces the Media Attenuation snoman command file
  //
  printf("------------------------------------\n");
  printf("QOCATitles: Producing Media Command File ...\n");

  // dimension in wavelength
  Int_t nwlen = GetNwlen();

  // (1/cm) from H2O values and slope of Boivin line <400nm
  Float_t alphad337 = 2.2e-4;

  // build and set the ouput file name
  Char_t filename[1024];
  sprintf(filename,"%s%s_%s%s",GetODir(),GetOLabel(),fScanTitles,GetOFormat());

  FILE *fmedia = fopen(filename,"w");
  if (!fmedia) {
    Error("MediaTitles","Couldn't get a file handle for media properties.");
    exit(-1);
  }

  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"*  Command file to set optical parameters\n");
  fprintf(fmedia,"*  Based on QOCAFit results\n");
  fprintf(fmedia,"*  This file must be read in before any version of media.dat\n");
  fprintf(fmedia,"*  File produced: %s\n",fTimestamp);
  fprintf(fmedia,"*  B.A. Moffat - Queen's University\n");
  fprintf(fmedia,"*  Contact: O. Simard (Carleton)\n");
  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"*  Rayleigh scale factor is change from 25degC (SNOMAN) to   \n");
  fprintf(fmedia,"*    11degC, the average SNO water temperature reported by   \n");
  fprintf(fmedia,"*    the water group (Tony Noble, 2000).");
  fprintf(fmedia,"*\n");
  fprintf(fmedia,"*  The absorption numbers were calculated from the QOCAFit   \n");
  fprintf(fmedia,"*    attenuation results by subtracting the contribution of  \n");
  fprintf(fmedia,"*    Rayleigh scattering to the prompt attenuation.          \n");
  fprintf(fmedia,"*------------------------------------------------------------\n");
  if(GetSystIndex()==0) {
    fprintf(fmedia,"*  Nominal fit results for 4ns time window.\n");
    if(fFunny337){
      fprintf(fmedia,"*  Nominal alpha_d2o(337nm) adjusted to %.2e (1/cm)\n",alphad337);
      fprintf(fmedia,"*    to compensate for contamination from fluorescence in\n");
      fprintf(fmedia,"*    the fibre optic (before subtracting Rayleigh scattering!)\n");
      fprintf(fmedia,"*  Add delta(alpha_d2o(337))_nominal = %.2e.\n",alphad337-fLd2o[0]);
    } else {
      fprintf(fmedia,"*  Using nominal alpha_d2o(337nm).\n");
    }
  } else {
    fprintf(fmedia,"*  Systematic %d (%s).\n",GetSystIndex(),GetSystName());
  }
  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"\n");

  //--------------------
  fprintf(fmedia,"$meda_heavy_water $attn_num_data %4.1f\n",(Double_t)nwlen);

  fprintf(fmedia,"$meda_heavy_water $attn_wavelengths  ");
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++) fprintf(fmedia," %10.1f ",GetWlens()[iwlen]);
  fprintf(fmedia,"%d*0.0\n",15-nwlen);

  fprintf(fmedia,"$meda_heavy_water $attn_coefficients ");
  for(Int_t iwlen = 0 ; iwlen<nwlen ; iwlen++) fprintf(fmedia," %10.3e ",fLd2o[iwlen]);
  fprintf(fmedia,"%d*0.0\n",15-nwlen);
  fprintf(fmedia,"$meda_heavy_water $attn_scl_fac 1.0\n");
  fprintf(fmedia,"$meda_heavy_water $rayleigh_scl_fac %.3f\n",fRayscaled2o);
  fprintf(fmedia,"$meda_heavy_water $isothermal_comp 4.92e-10\n");
  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"\n");

  //--------------------
  fprintf(fmedia,"$meda_acrylic_standard $attn_num_data %4.1f\n",(Double_t)nwlen);

  fprintf(fmedia,"$meda_acrylic_standard $attn_wavelengths  ");
  for(Int_t iwlen = 0 ; iwlen<nwlen ; iwlen++) fprintf(fmedia," %10.1f ",GetWlens()[iwlen]);
  fprintf(fmedia,"%d*0.0\n",15-nwlen);

  fprintf(fmedia,"$meda_acrylic_standard $attn_coefficients ");
  for(Int_t iwlen = 0 ; iwlen<nwlen ; iwlen++) fprintf(fmedia," %10.3e ",fLacr[iwlen]);
  fprintf(fmedia,"%d*0.0\n",15-nwlen);

  fprintf(fmedia,"$meda_acrylic_standard $attn_scl_fac 1.0\n");
  fprintf(fmedia,"$meda_acrylic_standard $rayleigh_scl_fac %.3f\n",fRayscaleacr);
  fprintf(fmedia,"$meda_acrylic_standard $isothermal_comp 3.55e-10\n");
  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"\n");

  //--------------------
  fprintf(fmedia,"* Acrylic Neck: Set to dark for UVA acrylic.\n");
  fprintf(fmedia,"$geom_media $ACRC_OVL $acrylic_dark\n");
  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"\n");

  //--------------------
  fprintf(fmedia,"$meda_light_water $attn_num_data %4.1f\n",(Double_t)nwlen);

  fprintf(fmedia,"$meda_light_water $attn_wavelengths  ");
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++) fprintf(fmedia," %10.1f ",GetWlens()[iwlen]);
  fprintf(fmedia,"%d*0.0\n",15-nwlen);

  fprintf(fmedia,"$meda_light_water $attn_coefficients ");
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++) fprintf(fmedia," %10.3e ",fLh2o[iwlen]);
  fprintf(fmedia,"%d*0.0\n",15-nwlen);

  fprintf(fmedia,"$meda_light_water $attn_scl_fac 1.0\n");
  fprintf(fmedia,"$meda_light_water $rayleigh_scl_fac %.3f\n",fRayscaleh2o);
  fprintf(fmedia,"$meda_light_water $isothermal_comp 4.78e-10\n");
  fprintf(fmedia,"*------------------------------------------------------------\n");
  fprintf(fmedia,"\n");

  //--------------------
  fclose(fmedia);

  printf("QOCATitles: Done.\n");
  printf("------------------------------------\n");
  return;
}

//______________________________________________________________________________
void QOCATitles::PMTRespTitles()
{
  //
  // Produces the PMT Response snoman titles file
  //

  printf("------------------------------------\n");
  printf("QOCATitles: Producing PMT Response Titles File ...\n");

  // build and set the ouput file name
  Char_t filename[1024];
  sprintf(filename,"%s%s_%s%s",GetODir(),GetOLabel(),fScanTitles,GetOFormat());

  FILE *fpmtr = fopen(filename,"w");
  if(!fpmtr){
    Error("PMTRespTitles","Couldn't get a file handle for pmt response.");
    exit(-1);
  }
  fprintf(fpmtr,"*DO  PMTR   1    -i(30I -F)   -n9034    #.  Bank for PMT data\n");
  fprintf(fpmtr,"#.         (This bank contains the PMT response in sno)\n");
  fprintf(fpmtr,"#.\n");
  fprintf(fpmtr,"#.    Contact: O. Simard (Carleton).\n");
  fprintf(fpmtr,"#.\n");
  fprintf(fpmtr,"#.    History:-\n");
  fprintf(fpmtr,"#.    =======\n");
  fprintf(fpmtr,"#.\n");
  fprintf(fpmtr,"#.    4.00/84  B.A. Moffat  Response from the SNO optical calibration.\n");
  fprintf(fpmtr,"#.    %s\n",fTimestamp);
  fprintf(fpmtr,"#.\n");
  if(GetSystIndex() == 0){
    fprintf(fpmtr,"#.  Nominal fit results for 4ns time window.\n");
  } else {
    fprintf(fpmtr,"#.  Systematic %d (%s).\n",GetSystIndex(),GetSystName());
  }

  // write the data base header
  DatabaseHeader(fpmtr);
  fprintf(fpmtr," \n");
  fprintf(fpmtr," \n");
  fprintf(fpmtr,"#.    User Data.\n");
  fprintf(fpmtr," \n");
  fprintf(fpmtr," 0.17     #. Shortest travel time from front of bucket to PMT.\n");
  fprintf(fpmtr," 0.17     #. Decay constant for parameterizing bucket travel times.\n");
  fprintf(fpmtr," 0.10     #. Angular spread in cosine on bouncing.\n");
  fprintf(fpmtr," 0.28     #. Limit on CERFAC (do not alter! - why not?)\n");
  fprintf(fpmtr," \n");
  fprintf(fpmtr,"#. Response data\n");
  fprintf(fpmtr,"\n");

  // clear up indices
  for(Int_t i=1; i<=50; i++){
    for(Int_t j=1; j<=90; j++){
      fprintf(fpmtr,"%15.4e ",fPmtresp->GetCellContent(i,j));
      if ((i*90+j) % 6 == 0) fprintf(fpmtr,"\n");
    }
  }
  fprintf(fpmtr,"\n");
  fprintf(fpmtr,"#. Reflectivity\n");
  fprintf(fpmtr,"\n");

  // clear up indices
  for(Int_t i=1; i<=50; i++){
    for(Int_t j=1; j<=90; j++){
      fprintf(fpmtr,"%15.4e ",fPmtrefl->GetCellContent(i,j));
      if((i*90+j) % 6 == 0) fprintf(fpmtr,"\n");
    }
  }

  fclose(fpmtr);
  printf("QOCATitles: Done.\n");
  printf("------------------------------------\n");

  return;
}

//______________________________________________________________________________
void QOCATitles::DriftTitles()
{
  //
  // Produces the Media Attenuation drift function snoman titles file
  //

  printf("Producing Media Attenuation Drift Titles File ...\n");

  // dimensions
  Int_t npol = GetNpol();
  Int_t ndlam = GetNdlam();
  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();

  // assign saved stuff
  Float_t*** poly = GetPoly();

  // build and set the ouput file name
  Char_t filename[1024];
  sprintf(filename,"%s%s_%s%s",GetODir(),GetOLabel(),fScanTitles,GetOFormat());

  FILE *fdrift = fopen(filename,"w");
  if(!fdrift){
    Error("DriftTitles","Couldn't get a file handle for attenuation drift.");
    exit(-1);
  }

  //--------------------
  fprintf(fdrift,"*LOG\n");
  fprintf(fdrift,"*---  Standard Titles File: mc_drift_attenuation.dat              %s\n",fTimestamp);
  fprintf(fdrift,"*---                \n");
  fprintf(fdrift,"*LOG OFF\n");
  //--------------------
  // pre-good data
  fprintf(fdrift,"*DO  ATDR  1  -i(30I 1I -F)                     #. MC Attenuation drift.\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    Contact: J. Formaggio (UW)\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    Standard Database Header\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"19750101        0 19991014        0  #.  1..4   Intrinsic validity\n");
  fprintf(fdrift,"       0        0        0           #.  5..7   Data type, Task type, Format no.\n");
  fprintf(fdrift,"       0        0        0           #.  8..10  Creation Date, Time, Source Id.\n");
  fprintf(fdrift,"19750101        0 19991014        0  #. 11..14  Effective validity\n");
  fprintf(fdrift,"       0        0                    #. 15..16  Entry Date Time\n");
  fprintf(fdrift,"4*0                                  #. 17..20  Spare\n");
  fprintf(fdrift,"10*0                                 #. 21..30  Temporary data (not in database)\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    End of Standard Database Header\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    User Data. \n");
  fprintf(fdrift,"#.    Drift response is stored as a 9th order polynomial in julian day.\n");
  fprintf(fdrift,"#.    First is media code (set to -1 for no drift)\n");
  fprintf(fdrift,"#.    Next are wavelengths and coefficients \n");
  fprintf(fdrift,"%d                       #. Media Code (D2O)\n",fMedium_code_hist);

  // write the polynomial terms (0 to ndlam)
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    fprintf(fdrift,"%d. ",(Int_t)wlens[iwlen]); // wlen
    for(Int_t ipol = 0 ; ipol < npol ; ipol++){
      fprintf(fdrift,"   %.4E",poly[0][iwlen][ipol]);
    }
    fprintf(fdrift,"   %d*0.\n",fNpolsnoman-npol); // rest of ipol
  }
  // write the rest
  for(Int_t iwlen = 0 ; iwlen < (ndlam-nwlen) ; iwlen++){
    fprintf(fdrift,"0.   10*0.\n");
  }

  //--------------------
  fprintf(fdrift,"\n*----------------------------------------------------------------------------\n\n");
  //--------------------
  // pure d2o data
  fprintf(fdrift,"*DO  ATDR  1  -i(30I 1I -F)                   #. MC Attenuation drift.\n");
  fprintf(fdrift,"#.                                               D2O Data first section.\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    Contact: J. Formaggio (UW)\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    Standard Database Header\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"19991014        0 20010528 19570208  #.  1..4   Intrinsic validity\n");
  fprintf(fdrift,"       0        0        0           #.  5..7   Data type, Task type, Format no.\n");
  fprintf(fdrift,"       0        0        0           #.  8..10  Creation Date, Time, Source Id.\n");
  fprintf(fdrift,"19991014        0 20010528 19570208  #. 11..14  Effective validity\n");
  fprintf(fdrift,"       0        0                    #. 15..16  Entry Date Time\n");
  fprintf(fdrift,"4*0                                  #. 17..20  Spare\n");
  fprintf(fdrift,"10*0                                 #. 21..30  Temporary data (not in database)\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    End of Standard Database Header\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    User Data. \n");
  fprintf(fdrift,"#.    Drift response is stored as a 9th order polynomial in julian day.\n");
  fprintf(fdrift,"#.    First is media code (set to -1 for no drift)\n");
  fprintf(fdrift,"#.    Next are wavelengths and coefficients \n");
  fprintf(fdrift,"%d                       #. Media Code (D2O)\n",fMedium_code_d2o);

  // write the polynomial terms (0 to ndlam)
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    fprintf(fdrift,"%d. ",(Int_t)wlens[iwlen]); // wlen
    for(Int_t ipol = 0 ; ipol < npol ; ipol++){
      fprintf(fdrift,"   %.4E",poly[1][iwlen][ipol]);
    }
    fprintf(fdrift,"   %d*0.\n",fNpolsnoman-npol); // rest of ipol
  }
  // write the rest
  for(Int_t iwlen = 0 ; iwlen < (ndlam-nwlen) ; iwlen++){
    fprintf(fdrift,"0.   10*0.\n");
  }
  //--------------------
  fprintf(fdrift,"\n*----------------------------------------------------------------------------\n\n");
  //--------------------
  // salt data
  fprintf(fdrift,"*DO  ATDR  1  -i(30I 1I -F)                    #. Attenuation Drift.\n");
  fprintf(fdrift,"#.                                                Salt.\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    Contact: J. Formaggio (UW)\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    Standard Database Header\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"20010528 19570208 20380517 03331900  #.  1..4   Intrinsic validity\n");
  fprintf(fdrift,"       0        0        0           #.  5..7   Data type, Task type, Format no.\n");
  fprintf(fdrift,"       0        0        0           #.  8..10  Creation Date, Time, Source Id.\n");
  fprintf(fdrift,"20010508 19570208 20380517 03331900  #. 11..14  Effective validity\n");
  fprintf(fdrift,"       0        0                    #. 15..16  Entry Date Time\n");
  fprintf(fdrift,"4*0                                  #. 17..20  Spare\n");
  fprintf(fdrift,"10*0                                 #. 21..30  Temporary data (not in database)\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    End of Standard Database Header\n");
  fprintf(fdrift,"#.\n");
  fprintf(fdrift,"#.    User Data. \n");
  fprintf(fdrift,"#.    First is media code\n");
  fprintf(fdrift,"#.    Next are wavelengths and coefficients \n");
  fprintf(fdrift,"%d                       #. Media Code (D2O)\n",fMedium_code_salt);

  // write the polynomial terms (0 to ndlam)
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    fprintf(fdrift,"%d. ",(Int_t)wlens[iwlen]); // wlen
    for(Int_t ipol = 0 ; ipol < npol ; ipol++){
      fprintf(fdrift,"   %.4E",poly[2][iwlen][ipol]);
    }
    fprintf(fdrift,"   %d*0.\n",fNpolsnoman-npol); // rest of ipol
  }
  // write the rest
  for(Int_t iwlen = 0 ; iwlen < (ndlam-nwlen) ; iwlen++){
    fprintf(fdrift,"0.   10*0.\n");
  }

  //--------------------
  fprintf(fdrift,"-1                      #. Set to media code less than/equal to zero to end \n");
  //--------------------
  fclose(fdrift);

  printf("QOCATitles: Done.\n");
  printf("------------------------------------\n");
  return;
}

//______________________________________________________________________________
void QOCATitles::PMTeffvarTitles()
{
  // Produces the PMT efficiencies variations titles file
  // and the averaged titles file.
  // Needs the TH2F* fPmteff 2D histogram in order to succeed.

  printf("------------------------------------\n");
  printf("Producing PMT efficiencies variations Titles Files ...\n");

  // dimensions
  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Char_t* scan = fScanTitles;

  // build and set the ouput file name
  Char_t filename[1024];

  sprintf(filename,"%s%s_%s%s",GetODir(),GetOLabel(),fScanTitles,GetOFormat());

  FILE *feff = fopen(filename,"w");
  if(!feff){
    Error("PMTeffvarTitles","Couldn't get a file handle for PMT efficiencies.");
    exit(-1);
  }

  fprintf(feff,"*LOG\n");
  fprintf(feff,"*---  Standard Titles File: opt_variations_%s.dat\t%s\n",
    scan,fTimestamp);
  fprintf(feff,"*---  Banks: QVAR,RVAR,AVAR,WAVL\n");
  fprintf(feff,"*LOG OFF\n");
  fprintf(feff,"*-------------------------------------------------------------------------------\n");
  fprintf(feff,"*---\n");
  fprintf(feff,"*---  PMT (PMT): Relative variations in PMT quantum efficiencies\n");
  fprintf(feff,"*---\n");
  fprintf(feff,"*---  Contact: O. Simard (Carleton), J. Maneira (Queen's)\n");
  fprintf(feff,"*---\n");
  fprintf(feff,"*-------------------------------------------------------------------------------\n");
  fprintf(feff,"*---\n");
  fprintf(feff,"*DO   QVAR  1  -i(30I -I) -n57162    #. Relative variation in optical properties\n");

  // write the data base header
  DatabaseHeader(feff);
  fprintf(feff,"#.\n");
  fprintf(feff,"#.    User Data.\n");
  fprintf(feff,"#.\n");
  fprintf(feff,"#.    The rest of this bank is non-standard.\n");
  fprintf(feff,"#.\n");
  fprintf(feff,"#.    Revision History:-\n");
  fprintf(feff,"#.    ================\n");
  fprintf(feff,"#.    04 Feb 1997 \tFirst version\n");
  fprintf(feff,"#.\n");
  fprintf(feff,"#.\n");
  fprintf(feff,"#.\n");

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    fprintf(feff,"#.         ********************************************\n");
    fprintf(feff,"#.         *** Values at Wavelength (%d) %d/%d. *****\n",
            (Int_t)wlens[iwlen],iwlen+1, nwlen);
    fprintf(feff,"#.         ********************************************\n");
    fprintf(feff,"#.\n#.\n");
    fprintf(feff,"#. Relative Overall Quantum Efficiency of Each PMT Expressed as\n");
    fprintf(feff,"#.           1000 x the Fraction of the Standard Input Value\n");
    fprintf(feff,"#.\n");

    for(Int_t jpmt = 1,k = 1; jpmt <= 9522; k++,jpmt++){
      if((jpmt-1)%200 == 0){
        fprintf(feff,"\n#. PMTs %d through %d\n",jpmt,jpmt+199);
      }
      fprintf(feff,"%4d  ",(Int_t)(1000.*(fPmteff->GetCellContent(jpmt,iwlen+1))));
      if(k == 201) k = 1;
      if(k%15 == 0) fprintf(feff,"\n");
    }
    fprintf(feff,"\n");

  } // for(iwlen)

  fclose(feff);

  printf("QOCATitles: Done.\n");
  printf("------------------------------------\n");
  return;
}


//______________________________________________________________________________
void QOCATitles::LBdistTitles()
{
  //
  // Produces the Laserball distribution titles file
  //

  printf("------------------------------------\n");
  printf("QOCATitles: Producing Laserball Distribution Titles File ...\n");

  // dimensions
  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();

  // build and set the ouput file name
  Char_t filename[1024];
  sprintf(filename,"%s%s_%s%s",GetODir(),GetOLabel(),fScanTitles,GetOFormat());

  FILE *flbdist = fopen(filename,"w");
  if(!flbdist){
    Error("LBdistTitles","Couldn't get a file handle for laserball distribution.");
    exit(-1);
  }

  fprintf(flbdist,"*LOG\n");
  fprintf(flbdist,"*---  Standard flbdist File: laserball_info.dat                %s\n",fTimestamp);
  fprintf(flbdist,"*---  Banks:  LASR\n");
  fprintf(flbdist,"*LOG OFF\n");
  fprintf(flbdist,"*DO   LASR   1   -i(30I -F) -n5000   #.  Laserball angular distribution\n");
  fprintf(flbdist,"#.\n"); 
  fprintf(flbdist,"#.    Contact:  O. Simard (Carleton)\n");

  // write the data base header
  DatabaseHeader(flbdist);
  fprintf(flbdist,"#.\n");
  fprintf(flbdist,"#. This title bank gives the Laplace series of spherical harmonics to\n");
  fprintf(flbdist,"#. 2nd order which defines the laserball angular distribution\n");
  fprintf(flbdist,"#.\n");
  fprintf(flbdist,"#. New implementation with parameters extracted from QOCAFit:\n");
  fprintf(flbdist,"#. Coefficients for order 6 polynomial giving the mask function in theta and\n");
  fprintf(flbdist,"#. Emission probabililty grid in 12 (costheta) * 36 (phi) bins\n");
  fprintf(flbdist,"#.\n");
  fprintf(flbdist,"3.14159                #. Laserball zenith orientation (0.0 = pointed upwards)\n");
  fprintf(flbdist,"0.00000                #. Laserball azimuthal orientation\n");
  fprintf(flbdist,"-0.001234 -0.001601 -0.002423 0.003404 0.007795  #. C01 C11 C02 C12 C22\n");
  fprintf(flbdist,"0.000961 0.001388 0.002150                       #. S11 S12 S22\n");


  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    fprintf(flbdist,"#.         ********************************************\n");
    fprintf(flbdist,"#.         *** Values at Wavelength (%d) %d/%d. *****\n",
           (Int_t)wlens[iwlen],iwlen+1, nwlen);
    fprintf(flbdist,"#.         ********************************************\n");
    fprintf(flbdist,"#. Coefficients for laserball mask (theta) order 0 is first, 6 is last\n");
    for(Int_t ipol = 0; ipol < 7; ipol++){
      fprintf(flbdist,"%1.6f  ",fLBpoly[iwlen][ipol]);
    }
    fprintf(flbdist,"\n#. Laserball distribution grid 0.5 deg bins in phi\n");
    for(Int_t itheta = 1 ; itheta <= 12 ; itheta++){
      fprintf(flbdist,"#. Coefficients for %1.2f < CosTheta < %1.2f\n",
	      -1. + (float)(itheta-1)*2./12.,-1 + (float)(itheta)*2./12.);
      for(Int_t iphi = 1 ; iphi <= 36 ; iphi++){
        fprintf(flbdist,"%1.6f  ",
	        fLBdist[iwlen]->GetCellContent(iphi,itheta));
	if(iphi%6 == 0) fprintf(flbdist,"\n");
      }
    }
  }

  fclose(flbdist);

  printf("QOCATitles: Done.\n");
  printf("------------------------------------\n");
  return;
}

