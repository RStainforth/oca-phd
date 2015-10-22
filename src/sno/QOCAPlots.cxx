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

#include "QOCAPlots.h"
#include "TMath.h"

ClassImp(QOCAPlots);
//
//
// OCA Plots Drawing class
//
//
// All plots are produced with default root settings
// defined in constructor using a TStyle. 
// User-defined styles can be set through SetStyle(TStyle*) below.
// None of these settings can be guaranteed : see your .rootrc for 
// further option settings.
//
// The functions usually produce a TCanvas which has to be saved
// in a .root file and then to disk. This class does not save any
// plot by itself. Use a macro or an executable to save the TCanvas.
//
// The canvas/histogram/graph names and titles can be set by the user
// by calling the right Set functions before each function call. Each
// function will check and set default names in case the titles are
// left NULL.
//
// In any case, the canvases are produced to be viewed and modified
// (for example using a TBrowser object).
// Hope it makes a difference in time-consuming macro editing.
//
//
//______________________________________________________________________________
QOCAPlots::QOCAPlots(Char_t* optionfile, Bool_t mode, Bool_t display, Char_t* edirectory, Char_t* pdirectory):QOCAAnalysis(optionfile, mode, display)
{
  // QOCAPlots constructor.
  //
  // initializes input/ouput directories and default options.
  //
  
  // display from constructor
  fDisplay = display;

  fQOCALoad = new QOCALoad();

  if(edirectory != NULL){
    SetIDir(edirectory);
  }

  if(pdirectory != NULL){
    SetODir(pdirectory);
  }

  // initialize all options to default values
  SetDefaultOptions();
  
  // plot style now uses TStyle
  fStyle = new TStyle("qocaplots","QOCAPlots-TStyle");
  // Apply SNO style
  if(fDisplay) printf("Using QOCAPlots-TStyle.\n");

  fStyle->SetCanvasBorderMode(0);
  fStyle->SetPadBorderMode(0);
  fStyle->SetPadColor(0);
  fStyle->SetCanvasColor(0);

  fStyle->SetPadGridX(1); 
  fStyle->SetPadGridY(1);
  fStyle->SetPadTickX(1);
  fStyle->SetPadTickY(1);
  fStyle->SetTextFont(132);
  fStyle->SetTitleOffset(1.2,"Y");
  fStyle->SetTitleFont(132, "XYZ");
  fStyle->SetLabelFont(132, "XYZ");
  fStyle->SetTitleFont(22,"title");
  fStyle->SetStatFont(132);
  fStyle->SetTextFont(132);
  fStyle->SetOptStat(0);
  fStyle->SetOptFit(1111);

  gROOT->SetStyle("qocaplots");
  gROOT->ForceStyle();

  // pre-determined colors (avoid yellow and light colors once for all)
  fColor[0] = 1; fColor[1] = 2; fColor[2] = 4; fColor[3] = 8;
  fColor[4] = 41; fColor[5] = 28; fColor[6] = 30; fColor[7] = 42;
  fColor[8] = 49; fColor[9] = 50;
  fColor[10] = 11; fColor[11] = 12; fColor[12] = 13; fColor[13] = 14;
  fColor[14] = 15; fColor[15] = 21; fColor[16] = 22; fColor[17] = 23;
  fColor[18] = 24; fColor[19] = 25;
  
}

//______________________________________________________________________________
QOCAPlots::~QOCAPlots()
{
  // QOCAPlots destructor

  if(fQOCALoad) delete fQOCALoad;
  if(fStyle) delete fStyle;

}

//______________________________________________________________________________
void QOCAPlots::SetDefaultOptions()
{
  // Sets the default options in the constructor
  // or whenever the user function is called.

  SetNpmtBins();
  SetWlen();

  // Default input/output file labels and file formats
  SetILabel(); SetOLabel();
  SetIFormat(); SetOFormat();
  
  // Legend related settings
  SetLegendPos(); // position
  SetLegendOpt(); // display options

  // Set titles to default
  SetCanvasTitle();
  SetPlotTitle(); 
  SetAxisTitles();

  // Fit Parameters
  SetFitFunction(); // for time dependence analysis

  // Plot titles and ranges
  SetXRange(); SetYRange(); SetOffset(); SetFactor();
  SetMarker();

  return;
}

//______________________________________________________________________________
void QOCAPlots::SetStyle(TStyle* aStyle)
{
  // Apply personal style
 
  if(fDisplay) printf("Using user-defined style %s.\n",aStyle->GetName());

  gROOT->SetStyle(aStyle->GetName());
  gROOT->ForceStyle();
  
  return;
}

//______________________________________________________________________________
Float_t QOCAPlots::FindMaximum(Float_t* values, const Int_t& dim)
{
  // Find the maximum from an array of number.

  Float_t max = 0.;
  for(Int_t n = 0 ; n < dim ; n++){
    if((values[n] > max) && (values[n] > 0.)){max = values[n];}
    else {max = max;}
  }

  if(max < 0.){
    fprintf(stderr,"In QOCAPlots::FindMaximum(): No Max point found.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  else return max;
}

//______________________________________________________________________________
Float_t QOCAPlots::FindMaximum(TH1F** h, const Int_t dim)
{
  // Find the maximum from an array of histograms.

  Float_t *val = new Float_t[dim];
  Float_t max = 0;
  for(Int_t n = 0 ; n < dim ; n++){
    val[n] = (h[n])->GetMaximum();
  }
  for(Int_t n = 0 ; n < dim ; n++){
    if((val[n] > max) && (val[n] > 0)){max = val[n];}
    else {max = max;}
  }
  delete [] val;
  if(max == 0.){
    fprintf(stderr,"In QOCAPlots::FindMaximum(): No Max point found.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  else return max;
}

//______________________________________________________________________________
Float_t QOCAPlots::FindMinimum(Float_t* values, const Int_t& dim)
{
  // Find the minimum from an array of number.

  Float_t min = 1.;
  for(Int_t n = 0 ; n < dim ; n++){
    if((values[n] < min) && (values[n] > 0.)){min = values[n];}
    else {min = min;}
  }

  if(min == 0.){
    fprintf(stderr,"In QOCAPlots::FindMinimum(): No Min point found.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  else return min;
}

//______________________________________________________________________________
void QOCAPlots::VerifyFlags()
{
  // Verifies the flags and displays information

  // display cardfile info
  printf("*   %d scan(s) and \n*   %d wavelength(s) of \n",GetNscan(),GetNwlen());
  if(GetFitsFlag() && GetFrunsFlag() && GetEffFlag()){
    printf("*   all qocafit files\n");
    SetKind(); //resets
  }
  else{
    if(GetFitsFlag()){
      printf("*   fits qocafit file(s)\n");
      SetKind("fits");
    }
    else{
      if(GetFrunsFlag()){
	printf("*   fruns qocafit file(s)\n");
	SetKind("fruns");
      }
      else{
	if(GetEffFlag()){
	  printf("*   eff qocafit file(s)\n");
	  SetKind("eff");
	}	
	else{SetKind();}
      }
    }
  }

  return;
}

//______________________________________________________________________________
void QOCAPlots::ChooseMedium()
{
  // Get the right medium based on the medium flag
  // set by the user with
  // either SetMediumFlag(Int_t) or SetFlags(fits, fruns, medium_flag)

  // if the medium is explicitly set, set the flags to be sure
  if(GetMedium() != NULL){
    if(!(strcmp(GetMedium(), "D2O"))){SetMediumFlag(0);}
    else{
      if(!(strcmp(GetMedium(), "ACR"))){SetMediumFlag(1);}
      else{
	if(!(strcmp(GetMedium(), "H2O"))){SetMediumFlag(2);}
      }
    }
  } else {SetMedium("D2O"); SetMediumFlag(0);}

  // if no medium is explictly set, check the flags
  if(GetMediumFlag() == 0){SetMedium("D2O");}
  if(GetMediumFlag() == 1){SetMedium("ACR");}
  if(GetMediumFlag() == 2){SetMedium("H2O");}
    
  // display confirmation
  if(!(strcmp(GetMedium(), "D2O")) && (GetMediumFlag() == 0)){
    printf("*   Medium = D2O (Heavy Water)\n");
  }
  else{
    if(!(strcmp(GetMedium(), "ACR")) && (GetMediumFlag() == 1)){
      printf("*   Medium = ACR (Acrylic)\n");
    }
    else{
      if(!(strcmp(GetMedium(), "H2O")) && (GetMediumFlag() == 2)){
	printf("*   Medium = H2O (Light Water)\n");
      } 
      else{
	fprintf(stderr,"In QOCAPlots::ChooseMedium(): Medium and Flag mismatch.\n");
	fprintf(stderr,"Aborting.\n"); exit(-1);
      }
    }
  }

  return;
}

//______________________________________________________________________________
void QOCAPlots::ChooseKind(Int_t ikind)
{
  // Get the right kind set by the user with
  // either SetKind(Char_t*) or SetFlags(fits, fruns, medium_flag)
  // according to the index ikind (usually set in a loop).

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
TCanvas* QOCAPlots::PlotAttenuation(Char_t* medium)
{
  // PlotAttenuation() with explicit medium setting.
  //
  // Valid options:
  //   - NULL or "D2O" for heavy water
  //   - "ACR" for acrylic
  //   - "H2O" for light water

  SetMedium(medium);
  return PlotAttenuation();
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotAttenuation()
{
  // Plot the Inverse Attenuation Lengths vs Wavelength
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  // -------
  // Inputs:
  // - needs textfiles (.txt) produced by QOCAExtract
  // - textfiles directory can be specified with the QOCAPlots constructor
  //   or with QOCAAnalysis SetIDir(Char_t*)
  // - the function creates the textfile names and load them
  //   with QOCALoad (which fails if the file is not found)
  // -------
  // Outputs:
  // - produces a plot in TCanvas format
  // -----------

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotAttenuation(): Setting default input label to 'attenuation'.\n");
    SetILabel("attenuation");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotAttenuation(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAPlots::PlotAttenuation(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'.\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAttenuation(): \n");
  printf("* Plotting Inverse Attenuation Lengths for \n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nkind = GetFlagsSum(), nscan = GetNscan(), nwlen = GetNwlen();
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // create the list of extraction files to access
  Int_t dim = nkind*nwlen*nscan;
  Char_t extractfile[1024]; Char_t name[128]; Char_t title[128];

  // vectors to store the local data
  Float_t* attens = new Float_t[dim]; // attenuations
  Float_t* errors = new Float_t[dim]; // errors

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan

      // local index for array access
      Int_t i = iscan+(ikind*nscan);

      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	// local index for array access
        Int_t j = iwlen+(i*nwlen);

	// build the full path textfile name
	sprintf(extractfile,"%s%s_%s_%s_%d%s",
		GetMedium(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

        // access the textfiles and keep the data to plot
	fQOCALoad->SetFullname(GetIDir(), extractfile);
        Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
	Int_t dim = fQOCALoad->GetDimensionFast();

	// convert the Char_t* format to Float_t
        attens[j] = atof(tmp1[0]); // first column first line
        errors[j] = atof(tmp1[2]); // first column second line

	// print the numbers for debugging
	if(fDisplay) printf("Att(%d) = %.2f +/- %.2f\n",j,attens[j],errors[j]);
	for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
	delete [] tmp1;

      } //iwlen
    } //iscan
  } //ikind

  // ---------------
  // Plotting method
  // ---------------

  // usual plot and axes titles: do not change
  sprintf(title,"%s Inverse Attenuation Length vs Wavelength",GetMedium());
  if(fCtitle == NULL) SetCanvasTitle(title);
  if(fPtitle == NULL) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Wavelength (nm)",
		  "Inverse Attenuation Lengths (cm^{-1})");
  }

  // name for TCanvas object
  if(GetMediumFlag() == 0) sprintf(name,"d2oatt");
  if(GetMediumFlag() == 1) sprintf(name,"acratt");
  if(GetMediumFlag() == 2) sprintf(name,"h2oatt");

  // TCanvas object is returned
  TCanvas* canvas = new TCanvas(name,fCtitle);

  // dynamic range setting based on the data to plot
  SetYRange(FindMinimum(attens, dim), FindMaximum(attens, dim));
  SetOffset(FindMaximum(errors, dim));
  // allow a space between the real maximum and the plot maximum
  SetFactor(1.5);

  // plot histograms with points and error bars
  TGraphErrors** grph = new TGraphErrors *[nkind*nscan];

  // legend options
  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);


  // loop over : reorganizing data for each scan
  //
  // basically, each fit type of each scan has its own color
  // and TGraphErrors object. Each TGraphErrors instance has
  // nwlen data points with no errors on the wavelength values.
  //
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  // index for attens and errors values : outside the for loops
  Int_t index = 0;

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // new color for each scan

      // local index for array access
      Int_t i = iscan+(ikind*nscan);
      Float_t* atts = new Float_t[nwlen];
      Float_t* errs = new Float_t[nwlen];
      
      // reorganize the data in local arrays
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
	atts[iwlen] = attens[index];
	errs[iwlen] = errors[index];
	index++;
      }

      // declare each graph instance with the newly reorganized arrays
      grph[i] = new TGraphErrors(nwlen, wlens, atts, 0, errs);

      // set the color and marker options
      GraphSettings(grph[i], i, 1);

      // first time around
      if(i==0){ (grph[i])->Draw("AP");
	// set the maxima and minima
	GraphSettings(grph[i]);
      }
      else{(grph[i])->Draw("P");} //draw others on top

      // create legend entries
      Char_t legendentry[128]; sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      if(fDisplay) printf(" legend entry: %s\n",legendentry);
      leg->AddEntry(grph[i], legendentry,"p");

      leg->SetBorderSize(1);
      leg->Draw();

      delete [] atts; 
      delete [] errs;

    } // for(iscan)
  } // for(ikind)

  canvas->SetFillColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  delete [] grph;
  return canvas;
}

//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAttenuationDrift(Char_t* medium)
{
  // PlotAttenuationDrift() with explicit medium setting.
  //
  // Valid options:
  //   - NULL or "D2O" for heavy water
  //   - "ACR" for acrylic
  //   - "H2O" for light water

  SetMedium(medium);
  return PlotAttenuationDrift();
}

//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAttenuationDrift()
{
  // Plot the Inverse Attenuation Lengths vs SNO Julian Date
  // usually for available wavelengths, scans and fit type
  // found in the cardfile.
  //
  // A fit is done over the data of each wavelength
  // fitting options (polynomial fit order) 
  // can be specified in the cardfile with the fit limits.
  //
  // The SNO Julian Date of each scan must be known before use:
  // they can be extracted by the user from .rdt files.
  // 
  // -------
  // Inputs & Ouputs: same as PlotAttenuation()
  // - a small textfiles containing fit parameters is also produced
  //   (only exception to QOCAExtract since more than one scan is needed to
  //    extract the drift function polynomial terms)
  //
  // for comments see PlotAttenuation() since the structure is the same
  // -----------

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotAttenuationDrift(): Setting default input label to 'attenuation'.\n");
    SetILabel("attenuation");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotAttenuationDrift(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){   
      printf("In QOCAPlots::PlotAttenuationDrift(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAttenuationDrift(): \n");
  printf("* Plotting Inverse Attenuation Lengths Time Dependence for \n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nkind = GetFlagsSum();
  if(nkind == 0 || nkind >= 2){
    fprintf(stderr,"In QOCAPlots::PlotAttenuationDrift(): Can't plot more than one fit type here.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }

  Int_t nscan = GetNscan(), nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Char_t** scans = GetScanArray();

  // create the list of extraction files to access
  Int_t dim = nwlen*nscan;
  Char_t extractfile[1024]; Char_t name[128]; Char_t title[128];

  // vectors
  Float_t* attens = new Float_t[dim]; // attenuations
  Float_t* errors = new Float_t[dim]; // errors

  // correlation measurements
  Float_t* rcorr = GetCorr();

  // loop over : no fit type here
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
    for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

      // local index for array access
      Int_t j = iwlen+(iscan*nwlen);

      sprintf(extractfile,"%s%s_%s_%s_%d%s",
	      GetMedium(),GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

      fQOCALoad->SetFullname(GetIDir(), extractfile);
      Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
      Int_t dim = fQOCALoad->GetDimensionFast();
      if(tmp1[0]){attens[j] = atof(tmp1[0]);} // first column first line
      else{fprintf(stderr,"Nothing to convert. Aborting.\n"); exit(-1);}
      errors[j] = atof(tmp1[2]); // first column second line

      if(fDisplay) printf("Att(%d) = %.2f +/- %.2f\n",j,attens[j],errors[j]);
      for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
      delete [] tmp1;
      
    } //iwlen
  } //iscan
   
  sprintf(title,"%s Inverse Attenuation Length vs Time",GetMedium());
  if(fCtitle == NULL) SetCanvasTitle(title);
  if(fPtitle == NULL) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("SNO Julian Date (days)",
		  "Inverse Attenuation Length (cm^{-1})");
  }

  // array to be returned
  TCanvas** carray = new TCanvas*[1+nwlen];

  // single summary canvas
  TCanvas* canvas = new TCanvas("attdriftall",fCtitle);

  SetYRange(FindMinimum(attens, dim), FindMaximum(attens, dim));
  SetOffset(FindMaximum(errors, dim));
  SetFactor(1.5);

  TGraphErrors** gr = new TGraphErrors *[nwlen]; // one graph per wlen
  Int_t index = 0;

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Wavelength Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);

  // Put the fit (drift) information in a text file
  Char_t driftfnam[1024];
  sprintf(driftfnam,"%s%sattenuation_drift.txt",GetIDir(),GetMedium());
  FILE* driftfile = fopen(driftfnam,"w");

  // ------------------
  // Plotting method 1: one single canvas
  // with all stuff on it each wavelength has its own color
  // ------------------
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen
    
    Float_t* atts = new Float_t[nscan];
    Float_t* errs = new Float_t[nscan];
 
    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan
      
      // local index for array access
      index = iwlen+(iscan*nwlen);
      atts[iscan] = attens[index];
      errs[iscan] = errors[index];
    
    } //for(iscan)

    gr[iwlen] = new TGraphErrors(nscan, (Float_t*)GetJulianDates(), atts, 0, errs);
   
    GraphSettings(gr[iwlen], iwlen, 1);

    if(iwlen==0){(gr[iwlen])->Draw("AP");
      GraphSettings(gr[iwlen]);
    }
    else{(gr[iwlen])->Draw("P");}

    // define a fit function (2-d max for now)
    TF1 *f1 = new TF1("f1", GetFitFunc(), GetFitStart(), GetFitEnd());
    //Int_t i = iwlen+2;
    //if(i == 5){i *= 10;} //avoid yellow
    f1->SetLineColor(fColor[iwlen]);
    if(fDisplay){(gr[iwlen])->Fit("f1","R");}
    else{(gr[iwlen])->Fit("f1","RQ");}

    Float_t corrAtten = f1->GetParameter(0) - GetCorrFactor()*rcorr[iwlen];

    // display for debugging
    if((f1->GetNDF()) && fDisplay){
      printf("chi/ndf = %.2f / %d = %.2f\n",
        f1->GetChisquare(),f1->GetNDF(),(f1->GetChisquare()/f1->GetNDF()));
      printf("***************\n");
      printf("* %f %.2f %.2f %.2f %.2f %.2f\n",
        wlens[iwlen],f1->GetParameter(0),corrAtten,
	f1->GetParError(0),f1->GetParameter(1),f1->GetParError(1));
      printf("***************\n");
    }

    // create legend entries
    Char_t legendentry[128]; sprintf(legendentry,"%d nm)",(Int_t)wlens[iwlen]);
    if(fDisplay) printf(" legend entry: %s\n",legendentry);
    leg->AddEntry(gr[iwlen], legendentry,"p");
    leg->SetBorderSize(1);
    leg->Draw();

    // fill the drift information file as a 9th order polynomial
    // leaving the empty ones to 0.
    // errors can be added using : f1->GetParError(ipol)
    fprintf(driftfile, "%d.\t", (Int_t) wlens[iwlen]);
    for(Int_t ipol = 0 ; ipol < f1->GetNpar() ; ipol++){
      fprintf(driftfile, "% 10.4E\t", f1->GetParameter(ipol));
    }
    Int_t nzeros = 10 - f1->GetNpar(); // 9th order
    fprintf(driftfile, "%d*0.0\n", nzeros);

  } // for(iwlen)

  fclose(driftfile); // close textfile
  // save the global view plot with all the wavelengths
  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  carray[0] = canvas;

  // ------------------
  // Plotting method 2:
  // one Canvas per wavelength; each scan has its own color
  // ------------------

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

    // here each data point needs to be associated with a scan
    // and a wavelength (with a different color)
    // so the arrays are single data points all plotted
    // on top of each other
    Float_t* atts = new Float_t[nscan];
    Float_t* errs = new Float_t[nscan];
    Float_t* atts1 = new Float_t[1];
    Float_t* errs1 = new Float_t[1];
    Float_t* juld1 = new Float_t[1];

    // one canvas per wavelength
    sprintf(title,"%s Inverse Attenuation Length vs Time at %.3d nm",
	    GetMedium(),(Int_t)wlens[iwlen]);
    // name for TCanvas object
    if(GetMediumFlag() == 0) sprintf(name,"d2oattdrift_%.3d",(Int_t)wlens[iwlen]);
    if(GetMediumFlag() == 1) sprintf(name,"acrattdrift_%.3d",(Int_t)wlens[iwlen]);
    if(GetMediumFlag() == 2) sprintf(name,"h2oattdrift_%.3d",(Int_t)wlens[iwlen]);

    // TCanvas object is returned
    TCanvas* tdrift = new TCanvas(name,title);

    // one legend per wavelength
    TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			       fLegendpos[2],fLegendpos[3],"Scan Legend");
    leg->SetEntrySeparation(fLegendsep);
    leg->SetMargin(fLegendmar);

    // first loop around scans to fill tmp vectors
    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){
      index = iwlen+(iscan*nwlen);
      atts[iscan] = attens[index];
      errs[iscan] = errors[index];
    }

    // then fill a first graph over the full range
    TGraphErrors* tmp = new TGraphErrors(nscan,GetJulianDates(),(Float_t*)atts,0,(Float_t*)errs);
    GraphSettings(tmp);
    tmp->Draw("AP");

    TGraphErrors** gr = new TGraphErrors *[nscan];
    // then re-loop and fill each point as a seperate graph instance
    // one graph instance (different color) per scan per wavelength
    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){

      atts1[0] = atts[iscan];
      errs1[0] = errs[iscan];
      juld1[0] = (GetJulianDates())[iscan];

      gr[iscan] = new TGraphErrors(1,(Float_t*)juld1,(Float_t*)atts1,0,(Float_t*)errs1);
      
      // set color and draw on top of the tmp frame
      GraphSettings(gr[iscan], iscan, 1);
      (gr[iscan])->Draw("P");

      // one legend entry per scan per wavelength
      Char_t legendentry[128]; sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      if(fDisplay) printf(" legend entry: %s\n",legendentry);
      leg->AddEntry(gr[iscan], legendentry,"p");
      
    } //for(iscan)

    // define one fit function per wavelength
    TF1 *f1 = new TF1("f1", GetFitFunc(), GetFitStart(), GetFitEnd());
    f1->SetLineColor(kBlack);
    if(fDisplay){tmp->Fit("f1","R");} // should call Draw()
    else{tmp->Fit("f1","RQ");}
    if((f1->GetNDF()) && fDisplay){
      printf("chi/ndf = %.2f / %d = %.2f\n",
        f1->GetChisquare(),f1->GetNDF(),(f1->GetChisquare()/f1->GetNDF()));
    }
      
    // fit results must appear for each wavelength
    fStyle->SetOptFit(1);
    leg->SetBorderSize(1);
    leg->Draw();

    // save each canvas in the root file and delete
    tdrift->cd();
    tdrift->SetFillColor(kWhite);
    tdrift->SetHighLightColor(kWhite);
    tdrift->SetFrameFillColor(kWhite);
    carray[iwlen+1] = tdrift;
    delete [] gr;

  } // for(iwlen)
  return carray;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotPmtr(Char_t* wlen,Int_t pmtgroup)
{
  // PlotPmtr() with explicit wavelength value.
  //
  // The wavelength is read in Char_t* format so it must be written using quotes.
  // Example: at wlen = 500 nm : PlotPmtr("500");

  SetWlen(wlen);
  return PlotPmtr(pmtgroup);
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotPmtr(Int_t pmtgroup)
{
  // Plot the PMT Angular Response vs Incident Angle bins in degrees
  // usually for 1 specified wlen and all files (fits/fruns)
  // found in the cardfile.
  //
  // 09.2006 - O.Simard
  // Looks for Pmtr Group 2 if available when pmtgroup > 1 and plots it on top.
  // The resulting plot could be messy if multiple scans are plotted.
  //
  // -------
  // Inputs:
  // - needs textfiles (.txt) produced by QOCAExtract
  // - textfiles directory can be specified with the QOCAPlots constructor
  //   or with QOCAAnalysis SetIDir(Char_t*)
  // - the function creates the textfile names and load them
  //   with QOCALoad (which fails if the file is not found)
  // -------
  // Outputs:
  // - produces a TCanvas
  // -----------

  Int_t nkind = GetFlagsSum();

  // verifications header before plotting takes place
  if(!GetWlen()){
    fprintf(stderr,"In QOCAPlots::PlotPmtr(): No wavelength has been set.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotPmtr(): Setting default input label to 'pmtAngResp'.\n");
    SetILabel("pmtAngResp");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotPmtr(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!nkind){
    if(fDisplay){
      printf("In QOCAPlots::PlotPmtr(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotPmtr(): \n");
  printf("* Plotting PMT Relative Angular Response at %s nm.\n",GetWlen());

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan();
  Char_t** scans = GetScanArray();

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024];Char_t title[1024];
  Char_t legendentry[128]; Char_t name[128];

  SetYRange(1.0, 1.3); SetOffset(0.0); SetFactor(1.3);
  Int_t maxbin = 0;

  // ---------------
  // Plotting method
  // ---------------

  sprintf(title,"Relative PMT Angular Response at %s nm vs Incident Angle",GetWlen());
  if(fCtitle) SetCanvasTitle(title);
  if(fPtitle) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Incident Angle (degrees)",
		  "Relative PMT Angular Response (arb. units)");
  }

  // name for TCanvas object
  sprintf(name,"pmtr%s",GetWlen());
  TCanvas* canvas = new TCanvas(name,fCtitle);

  // Vectors
  Float_t* bins; // bins (0 to 90)
  Float_t* angresps; // angular responses
  Float_t* errors; // errors

  // TGrapherrors instances to be declared
  TGraphErrors** grph  = new TGraphErrors *[dim];
  TGraphErrors** grph2 = new TGraphErrors *[dim];

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);

  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

      // local index for array access
      Int_t j = iscan+(ikind*nscan);
   
      for(Int_t igroup = 1 ; igroup <= pmtgroup ; igroup++){
	  
	if(igroup == 1){

	  sprintf(extractfile,"%s_%s_%s_%s%s",
		  GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat()); // usual label

	  // Access the files and put all the content in one array
	  Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
	  Char_t** tmp1 = fQOCALoad->CreateArray();
	  Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
	  bins = fQOCALoad->GetColumn(tmp2, 1); //first columns
	  angresps = fQOCALoad->GetColumn(tmp2, 2); //second column
	  errors = fQOCALoad->GetColumn(tmp2, 3); //third column
	  for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
	  delete [] tmp1;
	  
	  // find out the last non-zero bin
	  Int_t lastbin = 90; // default
	  Bool_t last = kFALSE; // not found yet
	  for(Int_t ibin = 0 ; ibin < fQOCALoad->GetNLines()-1; ibin++){
	    if(bins[ibin] != ibin) bins[ibin] -= 0.5;
	    if(last) continue; // skip zeros
	    if(fDisplay){
	      printf("AngResp(%.2f) = %.2f +/- %.2f\n",
		     bins[ibin],angresps[ibin],errors[ibin]);
	    }
	    // ignore first bin
	    if((ibin > 0 )&&(angresps[ibin] == 1.)&&(angresps[ibin+1] == 1.)){
	      last = kTRUE; // found last non-zero bin
	      lastbin = ibin;
	    }
	  }
	  
	  grph[j] = new TGraphErrors(lastbin, bins, angresps, 0, errors);
	  if(lastbin > maxbin) maxbin = lastbin;
	  GraphSettings(grph[j], j, 1); //markersize
	  
	  if(j==0){ //first time around
	    SetYRange(1.0, FindMaximum(angresps, lastbin));
	    SetOffset(FindMaximum(errors, lastbin));
	    (grph[j])->Draw("AP");
	    GraphSettings(grph[j]);
	    (grph[j])->SetMinimum(fYmin); //more convenient for pmtr
	  } else{(grph[j])->Draw("P");}
	  
	  sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
	  if(fDisplay) printf(" legend entry: %s\n",legendentry);
	  leg->AddEntry(grph[j], legendentry,"p");
	  leg->SetBorderSize(1);
	  leg->Draw();
	  
	  delete bins; delete angresps; delete errors;
	} else {
	  if(igroup == 2){
	    Char_t* newlabel = new Char_t[512];
	    sprintf(newlabel,"%s2",GetILabel()); // "2" follows the label
	    sprintf(extractfile,"%s_%s_%s_%s%s",
		    newlabel,scans[iscan],GetKind(),GetWlen(),GetIFormat());
	    
	    // Access the files and put all the content in one array
	    Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
	    Char_t** tmp1 = fQOCALoad->CreateArray();
	    Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
	    bins = fQOCALoad->GetColumn(tmp2, 1); //first columns
	    angresps = fQOCALoad->GetColumn(tmp2, 2); //second column
	    errors = fQOCALoad->GetColumn(tmp2, 3); //third column
	    for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
	    delete [] tmp1;
	    
	    // find out the last non-zero bin
	    Int_t lastbin = 90; // default
	    Bool_t last = kFALSE; // not found yet
	    for(Int_t ibin = 0 ; ibin < fQOCALoad->GetNLines()-1; ibin++){
	      if(bins[ibin] != ibin) bins[ibin] -= 0.5;
	      if(last) continue; // skip zeros
	      if(fDisplay){
		printf("AngResp(%.2f) = %.2f +/- %.2f\n",
		       bins[ibin],angresps[ibin],errors[ibin]);
	      }
	      // ignore first bin
	      if((ibin > 0 )&&(angresps[ibin] == 1.)&&(angresps[ibin+1] == 1.)){
		last = kTRUE; // found last non-zero bin
		lastbin = ibin;
	      }
	    }
	    
	    grph2[j] = new TGraphErrors(lastbin, bins, angresps, 0, errors);
	    if(lastbin > maxbin) maxbin = lastbin;
	    GraphSettings(grph2[j], j+10, 1); //grey shade, same marker
	    
	    (grph2[j])->Draw("P");
	    
	    sprintf(legendentry,"%s (%s,group2)",scans[iscan],GetKind());
	    if(fDisplay) printf(" legend entry: %s\n",legendentry);
	    leg->AddEntry(grph2[j], legendentry,"p");
	    leg->SetBorderSize(1);
	    leg->Draw();

	    delete bins; delete angresps; delete errors;
	    delete newlabel;

	  } else continue;
	}
      } //for(igroup)
      
    } // for(iscan)
  } // for(ikind)

  // reset the x-axis of the first graph plotted
  // in case the range has changed.
  (grph[0])->GetHistogram()->GetXaxis()->SetLimits(0,maxbin);
  (grph[0])->GetHistogram()->GetXaxis()->SetRangeUser(0,maxbin);

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  delete [] grph;
  delete [] grph2;
  return canvas;
}

//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAllPmtr(Int_t pmtgroup)
{
  // Plot the PMT Angular Response vs Incident Angle bins in degrees
  // usually for all wlen and all files (fits/fruns)
  // found in the cardfile.
  //
  // 09.2006 - O.Simard
  // Looks for Pmtr Group 2 if available when pmtgroup > 1 and plots it on top.
  // The resulting plot could be messy if multiple scans are plotted.
  //
  // -------
  // Inputs & Outputs: same as PlotAttenuation()
  // - repeats PlotAttenuation() function and stores all
  //   seperate wlen plots along with the global plots
  //   with all the wavelengths
  // -----------

  Int_t nkind = GetFlagsSum();

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotAllPmtr(): Setting default input label to 'pmtAngResp'.\n");
    SetILabel("pmtAngResp");
  }
  if(!GetIFormat()){
    if(fDisplay)
      printf("In QOCAPlots::PlotAllPmtr(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!nkind){
    if(fDisplay){    
      printf("In QOCAPlots::PlotAllPmtr(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAllPmtr(): \n");
  printf("* Plotting PMT Relative Angular Response for \n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan(),nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Char_t** scans = GetScanArray();

  Int_t maxbin = 0;
  Float_t div_per = 0.01; // relative division between each pads

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024]; Char_t newtitle[1024]; Char_t name[128];
  Char_t wl[128]; Char_t tx[128]; Char_t legendentry[128];

  // ---------------
  // Plotting method
  // ---------------

  if(fCtitle == NULL) SetCanvasTitle("Relative PMT Angular Responses vs Incident Angle");
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Incident Angle (degrees)",
		  "Relative PMT Angular Response (arb. units)");
  }

  // array
  TCanvas** carray = new TCanvas*[nwlen+1];

  // name for TCanvas object
  sprintf(name,"pmtrall");
  // single canvas for summary
  TCanvas* canvas = new TCanvas(name,fCtitle,0,0,1200,900);
  canvas->Divide(2, 3, div_per, div_per, 0); // contains 2x3=6 graphs

  // vectors
  Float_t* bins;      // bins (0 to 90)
  Float_t* angresps;  // angular responses
  Float_t* errors;    // errors

  // create twice as many pmtr as there are pmt groups
  TGraphErrors*** grph  = new TGraphErrors **[nwlen];
  TGraphErrors*** grph2 = new TGraphErrors **[nwlen];
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    grph[iwlen]  = new TGraphErrors *[dim];
    grph2[iwlen] = new TGraphErrors *[dim];
  }

  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  // a pad for each wlen (all 6 wlens in a single canvas)

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    // set the wlen each time
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    SetWlen((Char_t*)wl);

    TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			       fLegendpos[2],fLegendpos[3],"Scan Legend");
    leg->SetEntrySeparation(fLegendsep);
    leg->SetMargin(fLegendmar);

    sprintf(newtitle,"Relative PMT Angular Response at %.3d nm vs Incident Angle",
      (Int_t)wlens[iwlen]);
    SetPlotTitle(newtitle);

    canvas->cd(iwlen+1); //pad selection

    for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

      // set the fit type internally
      ChooseKind(ikind);

      for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

	Int_t j = iscan+(ikind*nscan);

	  for(Int_t igroup = 1 ; igroup <= pmtgroup ; igroup++){
	  
	    if(igroup == 1){
	      sprintf(extractfile,"%s_%s_%s_%s%s",
		      GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat()); // usual label

	      // Access the files and put all the content in one array
	      Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
	      Char_t** tmp1 = fQOCALoad->CreateArray();
	      Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
	      bins = fQOCALoad->GetColumn(tmp2, 1); //first columns
	      angresps = fQOCALoad->GetColumn(tmp2, 2); //second column
	      errors = fQOCALoad->GetColumn(tmp2, 3); //third column
	      for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
	      delete [] tmp1;
	      
	      // find out the last non-zero bin
	      Int_t lastbin = 90; // default
	      Bool_t last = kFALSE; // not found yet
	      for(Int_t ibin = 0 ; ibin < fQOCALoad->GetNLines()-1; ibin++){
		if(bins[ibin] != ibin) bins[ibin] -= 0.5; // shift
		if(last) continue; // skip zeros
		if(fDisplay){
		  printf("AngResp(%.2f) = %.2f +/- %.2f\n",
			 bins[ibin],angresps[ibin],errors[ibin]);
		}
		// ignore first bin
		if((ibin > 0 )&&(angresps[ibin] == 1.)&&(angresps[ibin+1] == 1.)){
		  last = kTRUE; // found last non-zero bin
		  lastbin = ibin;
		}
	      }

	      grph[iwlen][j] = new TGraphErrors(lastbin, bins, angresps, 0, errors);
	      if(lastbin > maxbin) maxbin = lastbin; // find the max of all
	      GraphSettings(grph[iwlen][j], j, 0.5); // markersize
	      
	      if(j==0){ //first time around
		(grph[iwlen][j])->Draw("AP");
		GraphSettings(grph[iwlen][j]);
	      } else {(grph[iwlen][j])->Draw("P");}
	      
	      sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
	      if(fDisplay) printf(" legend entry: %s\n",legendentry);
	      leg->AddEntry(grph[iwlen][j], legendentry,"p");
	      leg->SetBorderSize(1);
	      leg->Draw();

	      delete bins; delete angresps; delete errors;

	    } else {
	      if(igroup == 2){
		Char_t* newlabel = new Char_t[512];
		sprintf(newlabel,"%s2",GetILabel()); // "2" follows the label
		sprintf(extractfile,"%s_%s_%s_%s%s",
			newlabel,scans[iscan],GetKind(),GetWlen(),GetIFormat());

		// Access the files and put all the content in one array
		Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
		Char_t** tmp1 = fQOCALoad->CreateArray();
		Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
		bins = fQOCALoad->GetColumn(tmp2, 1); //first columns
		angresps = fQOCALoad->GetColumn(tmp2, 2); //second column
		errors = fQOCALoad->GetColumn(tmp2, 3); //third column
		for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
		delete [] tmp1;
		
		// find out the last non-zero bin
		Int_t lastbin = 90; // default
		Bool_t last = kFALSE; // not found yet
		for(Int_t ibin = 0 ; ibin < fQOCALoad->GetNLines()-1; ibin++){
		  if(bins[ibin] != ibin) bins[ibin] -= 0.5; // shift
		  if(last) continue; // skip zeros
		  if(fDisplay){
		    printf("AngResp(%.2f) = %.2f +/- %.2f\n",
			   bins[ibin],angresps[ibin],errors[ibin]);
		  }
		  // ignore first bin
		  if((ibin > 0 )&&(angresps[ibin] == 1.)&&(angresps[ibin+1] == 1.)){
		    last = kTRUE; // found last non-zero bin
		    lastbin = ibin;
		  }
		}
		
		grph2[iwlen][j] = new TGraphErrors(lastbin, bins, angresps, 0, errors);
		if(lastbin > maxbin) maxbin = lastbin; // find the max of all
		GraphSettings(grph2[iwlen][j], j+10, 0.5); // grey shades same marker
		(grph2[iwlen][j])->Draw("P");
		
		sprintf(legendentry,"%s (%s,group2)",scans[iscan],GetKind());
		if(fDisplay) printf(" legend entry: %s\n",legendentry);
		leg->AddEntry(grph2[iwlen][j], legendentry,"p");
		leg->SetBorderSize(1);
		leg->Draw();

		delete bins; delete angresps; delete errors;
		delete newlabel;
		
	      } else continue;
	    }
	} //for(igroup)
      } // for(iscan)
    } // for(ikind)

    // reset the x-axis of the first graph plotted
    // in case the range has changed.
    (grph[iwlen][0])->GetHistogram()->GetXaxis()->SetLimits(0,maxbin);
    (grph[iwlen][0])->GetHistogram()->GetXaxis()->SetRangeUser(0,maxbin);

    // draw text after
    TText *t = new TText();
    t->SetTextSize(0.10); t->SetTextAlign(12);
    sprintf(tx,"%.3d nm",(Int_t)wlens[iwlen]);
    t->DrawTextNDC(0.15, 0.50, tx); // NDC : [0,1] system
    delete t;

  } //for(iwlen)

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  carray[0] = canvas;

  
  // then one canvas per wlen, for convenience
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    // set the wlen each time
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    SetWlen((Char_t*)wl);

    TCanvas* cw = PlotPmtr(pmtgroup);
    carray[iwlen+1] = cw;
  } //for(iwlen)
  printf("--------------------------------------------------------\n");

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    delete [] grph[iwlen];
    if(grph2[iwlen]) delete [] grph2[iwlen];
  }
  delete [] grph;
  delete [] grph2;
  return carray;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotPmtrError(Char_t* wlen, Char_t* option)
{
  // Plot the PMT Angular Response chosen Uncertainty
  // (Statistical or Systematic)
  // vs Incident Angle bins in degrees
  // usually for all wlen and all files (fits/fruns)
  // found in the cardfile
  // -------
  // Inputs & Outputs: read data from textfiles
  //                   return a Canvas with the graphs
  // -----------

  // verifications header before plotting takes place
  SetWlen(wlen);

  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotPmtrError(): Setting default input label to 'pmtAngResp'.\n");
    SetILabel("pmtAngResp");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotPmtrError(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){    
      printf("In QOCAPlots::PlotPmtrError(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotPmtrError(): \n");
  printf("* Plotting PMT Relative Angular Response Uncertainty for \n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  SetYRange(1.0, 1.3); SetOffset(0.0); SetFactor(1.3);

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nkind = GetFlagsSum(),nscan = GetNscan();
  Char_t** scans = GetScanArray();
  const Int_t Nbin_max = 90; //maximum number of bins

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024],histname[1024],name[128],title[128];

  // ---------------
  // Plotting method
  // ---------------

  sprintf(title,"Relative PMT Angular Response %s Uncertainty at %s nm vs Incident Angle",
	  option,GetWlen());
  if(fCtitle) SetCanvasTitle(title);
  if(fPtitle) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL));
  if(!strcmp(option, "Statistical")){
    SetAxisTitles("Incident Angle (degrees)", "Relative Statistical Uncertainty (arb. units)");
    sprintf(name,"pmtrstaterr%s",GetWlen());
  }
  if(!strcmp(option, "Systematic")){
    SetAxisTitles("Incident Angle (degrees)", "Relative Systematic Uncertainty (arb. units)");
    sprintf(name,"pmtrsysterr%s",GetWlen());
  }

  TCanvas* canvas = new TCanvas(name,fCtitle);

  // Vectors
  Float_t* errors, * pmtr;

  // TH1F instances to be declared
  TH1F** grph = new TH1F *[dim];

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);

  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

      // local index for array access
      Int_t j = iscan+(ikind*nscan);

      sprintf(extractfile,"%s_%s_%s_%s%s",
				GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());

      // Access the files and put all the content in one array
      fQOCALoad->SetFullname(GetIDir(), extractfile);
      Char_t** tmp = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
      Int_t nlines = fQOCALoad->GetNLines();
      Int_t dim = fQOCALoad->GetDimensionFast();

      pmtr = fQOCALoad->ConverttoFloats(fQOCALoad->GetColumn(tmp, 2), nlines); // fourth column

      // choice of the error to plot
      if(!strcmp(option, "Statistical")){
        sprintf(histname,"%s_pmtrstat_%s",scans[iscan],GetWlen());
	errors = fQOCALoad->ConverttoFloats(fQOCALoad->GetColumn(tmp, 4), nlines); // fourth column
      }
      if(!strcmp(option, "Systematic")){
        sprintf(histname,"%s_pmtrsyst_%s",scans[iscan],GetWlen());
	errors = fQOCALoad->ConverttoFloats(fQOCALoad->GetColumn(tmp, 5), nlines); // fifth column
      }
      for(Int_t i = 0 ; i < dim ; i++) delete [] tmp[i];
      delete [] tmp;

      // histogram new instance
      grph[j] = new TH1F(histname, fPtitle, nlines, 0, nlines);

      (grph[j])->SetLineColor(j+2);

      // fill histograms with errors/pmtr (relative not absolute)
      for(Int_t ibin = 0 ; ibin < nlines ; ibin++){
	(grph[j])->SetBinContent(ibin+1, ((errors[ibin])/(pmtr[ibin])));
      }

    if(j==0){ //first time around
      SetYRange(0.0, FindMaximum(errors, Nbin_max));
      (grph[j])->SetMaximum(fYmax+0.1*fYmax);
      (grph[j])->SetXTitle(fXtitle);
      (grph[j])->SetYTitle(fYtitle);
      (grph[j])->Draw();
    } else {(grph[j])->Draw("same");}

    Char_t legendentry[128]; sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
    if(fDisplay) printf(" legend entry: %s\n",legendentry);
    leg->AddEntry(grph[j], legendentry,"l");
    leg->SetBorderSize(1);
    leg->Draw();

    } // for(iscan)
  } // for(ikind)

  delete errors;
  delete pmtr;
  delete [] grph;

  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);

  return canvas;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotLBdist1D(Char_t* wlen, Int_t bintheta)
{
  // PlotLBdist1D() with explicit wavelength value
  // and cos(theta) bin number (0 to 11).
  //
  // The wavelength is read in Char_t* format so it must be written using quotes.
  // Example: at wlen = 500 nm and bin 2: PlotLBdist1D("500",2);

  SetWlen(wlen);
  return PlotLBdist1D(bintheta);
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotLBdist1D(Int_t bintheta)
{
  // Plot the Laserball distribution as a function of phi [0,2pi]
  // for a specified bin number in cos(theta)
  // usually for one specified wlen and all files (fits/fruns)
  // found in the cardfile.
  // -------
  // Inputs/Outputs: similar to PlotPmtr()
  // -----------

  // verifications header before plotting takes place
  if(bintheta < 0 || bintheta > 11){
    fprintf(stderr,"In QOCAPlots::PlotLBdist1D(): No matching theta bin number.\n");
    fprintf(stderr,"Valid range: [0,11].\n"); exit(-1);
  }
  if(!GetWlen()){
    fprintf(stderr,"In QOCAPlots::PlotLBdist1D(): No wavelength has been set.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotLBdist1D(): Setting default input label to 'lbdist'.\n");
    SetILabel("lbdist");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotLBdist1D(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAPlots::PlotLBdist1D(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'.\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotLBdist1D(): \n");
  printf("* Plotting Laserball Distribution at %s nm for bin %d.\n",GetWlen(),bintheta);

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan(),nkind = GetFlagsSum();
  Int_t nphi = 36;    // maximum number of bins in phi
  Char_t** scans = GetScanArray();

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024], bin[128], name[128], title[128];
  Char_t legendentry[128];

  // ---------------
  // Plotting method
  // ---------------

  sprintf(bin,"%d",bintheta);
  sprintf(title,"Relative Laserball Intensity at %s nm for bin %s vs Azimuthal Angle #phi (1D)",
    GetWlen(),bin);
  if(fCtitle == NULL) SetCanvasTitle(title);
  if(fPtitle == NULL) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Azimuthal Angle #phi (radians)",
    		  "Relative Laserball Intensity (arb. units)");
  }

  // name for TCanvas object
  sprintf(name,"lbdist1d%s",GetWlen());
  TCanvas* canvas = new TCanvas(name,fCtitle);

  // TGrapherrors instances to be declared
  TGraphErrors** grph = new TGraphErrors *[dim];

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);

  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

      // local index for array access
      Int_t j = iscan+(ikind*nscan);

      sprintf(extractfile,"%s_%s_%s_%s%s",
				GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());

      // Access the files and put all the content in one array
      Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
      Char_t** tmp1 = fQOCALoad->CreateArray();
      Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
      Float_t* bins = fQOCALoad->GetColumn(tmp2, 2); //second columns
      Float_t* lbdist = fQOCALoad->GetColumn(tmp2, 3); //third column
      //Float_t* errors = fQOCALoad->GetColumn(tmp2, 4); //fourth column
      Float_t* errors = fQOCALoad->GetColumn(tmp2, 6); // use systematics only
      for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
      delete [] tmp1;

      // reorganize the floats into smaller arrays
      Float_t* bin_th = new Float_t[nphi];
      Float_t* bwi_th = new Float_t[nphi];
      Float_t* lbd_th = new Float_t[nphi];
      Float_t* err_th = new Float_t[nphi];

      for(Int_t iphi = 0 ; iphi < nphi ; iphi++){
	Int_t offset = bintheta*nphi;
	bin_th[iphi] = bins[offset+iphi];
	bwi_th[iphi] = (2*TMath::Pi())/nphi;
        lbd_th[iphi] = lbdist[offset+iphi];
        err_th[iphi] = errors[offset+iphi];
      }

      // forget the y-axis error bars
      grph[j] = new TGraphErrors(nphi, bin_th, lbd_th, bwi_th, err_th);
      //grph[j] = new TGraphErrors(nphi, bin_th, lbd_th, bwi_th, 0);
      GraphSettings(grph[j], j, 1); //markersize

      if(j==0){ //first time around
        (grph[j])->Draw("AP");
        GraphSettings(grph[j]);
        //(grph[j])->SetMinimum(fYmin);
      } else {(grph[j])->Draw("P");}

      sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      if(fDisplay) printf(" legend entry: %s\n",legendentry);
      leg->AddEntry(grph[j], legendentry,"p");
      leg->SetBorderSize(1);
      leg->Draw();

      delete bin_th; delete bwi_th;
      delete lbd_th; delete err_th;
      delete bins; delete lbdist; delete errors;
    } // for(iscan)
  } // for(ikind)

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  delete [] grph;
  return canvas;
}
//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAllLBdist1D()
{
  // Plot the Laserball distribution as a function of phi [0,2pi]
  // for all bin numbers in cos(theta), all wlen and all files (fits/fruns)
  // found in the cardfile.
  // -------
  // Inputs/Outputs: similar to PlotAllPmtr()
  // -----------

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotAllLBdist1D(): Setting default input label to 'lbdist'.\n");
    SetILabel("lbdist");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotAllLBdist1D(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAPlots::PlotAllLBdist1D(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'.\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAllLBdist1D(): \n");
  printf("* Plotting Laserball Distribution at for \n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

// copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nwlen = GetNwlen(),nscan = GetNscan(),nkind = GetFlagsSum();
  Float_t* wlens = GetWlens();
  Char_t** scans = GetScanArray();

  Int_t nphi = 36;    // maximum number of bins in phi
  Int_t ntheta = 12;  // maximum number of bins in cos

  Float_t div_per = 0.01; // relative division between each pads

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024]; Char_t wl[128]; Char_t title[128];
  Char_t legendentry[128]; Char_t tx[128]; Char_t name[128];

  // ---------------
  // Plotting method
  // ---------------

  // array
  TCanvas** carray = new TCanvas*[nwlen];
  TGraphErrors*** grph = new TGraphErrors **[nwlen];
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    grph[iwlen] = new TGraphErrors *[dim];
  }

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    // set the wlen each time
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    SetWlen(wl);

    sprintf(title,"Relative Laserball Intensity at %s nm vs Azimuthal Angle #phi (1D)",
      GetWlen());
    SetCanvasTitle(title);

    // single canvas for summary
    sprintf(name,"lbdist1d%s",GetWlen());
    TCanvas* canvas = new TCanvas(name,fCtitle,1200,900);
    canvas->Divide(3, 4, div_per, div_per, 0); // contains 3x4=12 graphs

    if((fXtitle == NULL) && (fYtitle == NULL)){
      SetAxisTitles("Azimuthal Angle #phi (radians)",
    		"Relative Laserball Intensity (arb. units)");
    }

    // loop over :
    // itheta : bins in theta
    // ikind: fit type (fits, fruns or both)
    // iscan: scan (from cardfile)
    // a pad for each itheta
    for(Int_t itheta = 0 ; itheta < ntheta ; itheta++){

      TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
                                 fLegendpos[2],fLegendpos[3],"Scan Legend");
      leg->SetEntrySeparation(fLegendsep);
      leg->SetMargin(fLegendmar);

      sprintf(title,"Relative Laserball Intensity at %.3d nm vs Azimuthal Angle #phi",
        (Int_t)wlens[iwlen]);
      SetPlotTitle(title);

      canvas->cd(itheta+1); //pad selection

      for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

        // set the fit type internally
        ChooseKind(ikind);

        for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

          Int_t j = iscan+(ikind*nscan);

          sprintf(extractfile,"%s_%s_%s_%s%s",
                                GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());

          // Access the files and put all the content in one array
          Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
          Char_t** tmp1 = fQOCALoad->CreateArray();
          Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
          Float_t* bins = fQOCALoad->GetColumn(tmp2, 2); // second columns
          Float_t* lbdist = fQOCALoad->GetColumn(tmp2, 3); // third column
          //Float_t* errors = fQOCALoad->GetColumn(tmp2, 4); // fourth column
          Float_t* errors = fQOCALoad->GetColumn(tmp2, 6); // use systematics only
	  for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
	  delete [] tmp1;

	  // reorganize the floats into smaller arrays
          Float_t* bin_th = new Float_t[nphi];
          Float_t* bwi_th = new Float_t[nphi];
          Float_t* lbd_th = new Float_t[nphi];
          Float_t* err_th = new Float_t[nphi];

          for(Int_t iphi = 0 ; iphi < nphi ; iphi++){
            Int_t offset = itheta*nphi;
            bin_th[iphi] = bins[offset+iphi];
            bwi_th[iphi] = (2*TMath::Pi())/nphi;
            lbd_th[iphi] = lbdist[offset+iphi];
            err_th[iphi] = errors[offset+iphi];
          }

          // display
          if(fDisplay){
            for(Int_t iphi = 0 ; iphi < nphi ; iphi++){
              printf("LBdist(%.1f) = %.2f +/- %.2f\n",
		     bin_th[iphi],lbd_th[iphi],err_th[iphi]);
            }
          }
          // forget the y-axis error bars
          grph[iwlen][j] = new TGraphErrors(nphi, bin_th, lbd_th, bwi_th, err_th);
          //grph[iwlen][j] = new TGraphErrors(nphi, bin_th, lbd_th, bwi_th, 0);
          GraphSettings(grph[iwlen][j], j, 0.5); // markersize
	  
          if(j==0){ //first time around
            (grph[iwlen][j])->Draw("AP");
            GraphSettings(grph[iwlen][j]);
            //(grph[iwlen][j])->SetMinimum(fYmin);
          } else {(grph[iwlen][j])->Draw("P");}

          sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
          if(fDisplay) printf(" legend entry: %s\n",legendentry);
          leg->AddEntry(grph[iwlen][j], legendentry,"p");
          leg->SetBorderSize(1);
          leg->Draw();
	  
          // draw text after
          TText *t = new TText();
          t->SetTextSize(0.10); t->SetTextAlign(12);
          sprintf(tx,"theta bin %d",itheta);
          t->DrawTextNDC(0.12, 0.15, tx); // NDC : [0,1] system
          delete t;

          delete bin_th; delete bwi_th;
          delete lbd_th; delete err_th;
          delete bins; delete lbdist; delete errors;
	} // for(iscan)
      } // for(ikind)
    } // for(itheta)
    
    canvas->SetFillColor(kWhite);
    canvas->SetHighLightColor(kWhite);
    canvas->SetFrameFillColor(kWhite);
    carray[iwlen] = canvas;

  } // for(iwlen)

  for (Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    delete [] grph[iwlen];
  }
  delete [] grph;
  return carray;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotLBdist2D(Char_t* wlen)
{
  // PlotLBdist2D() with explicit wavelength value.
  //
  // The wavelength is read in Char_t* format so it must be written using quotes.
  // Example: at wlen = 500 nm : PlotLBdist2D("500");

  SetWlen(wlen);
  return PlotLBdist2D();
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotLBdist2D()
{
  // Plot the Laserball distribution as a function of phi [0,2pi]
  // and cos(theta) using a TH2F binned surface
  // usually for one specified wlen.

  // verifications header before plotting takes place
  if(!GetWlen()){
    fprintf(stderr,"In QOCAPlots::PlotLBdist2D(): No wavelength has been set.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotLBdist2D(): Setting default input label to 'lbdist'.\n");
    SetILabel("lbdist");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotLBdist2D(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAPlots::PlotLBdist2D(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'.\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotLBdist2D(): \n");
  printf("* Plotting Laserball Distribution at %s nm.\n",GetWlen());
  
  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan();
  Int_t nphi = 36, ntheta = 12;    // maximum number of bins in phi/theta
  Char_t** scans = GetScanArray();

  Float_t div_per = 0.01;
  
  // Create the list of extraction files to access
  Char_t extractfile[1024]; Char_t title[128]; Char_t name[128]; 
    
  // title
  sprintf(title,"Relative Laserball Intensity at %s nm (2D)",GetWlen());
  if(fCtitle == NULL) SetCanvasTitle(title);
  // set axis titles if not already defined
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Azimuthal Angle #phi (radians)",
    		  "Polar Angle cos#theta");
  }

  // single canvas for summary
  sprintf(name,"lbdist2d%s",GetWlen());
  TCanvas* canvas = new TCanvas(name,fCtitle,0,0,600,1200);
  canvas->Divide(1, nscan, div_per, div_per, 0); // contains nscan in one column
  
  TH2F** hist = new TH2F*[nscan];
  
  for(Int_t iscan = 0 ; iscan < nscan ; iscan++){
    
    canvas->cd(iscan+1);
    sprintf(title,"Relative Laserball Intensity for %s at %s nm (2D)",
	    scans[iscan],GetWlen());
    if(fPtitle == NULL) SetPlotTitle(title);
    sprintf(name,"lbdist2d%s%s",scans[iscan],GetWlen());
    hist[iscan] = new TH2F(name,title,nphi,0,2.0*TMath::Pi(),ntheta,-1,1);

    // extract file
    sprintf(extractfile,"%s_%s_%s_%s%s",
	    GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());
    
    // Access the files and put all the content in one array
    Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
    Char_t** tmp1 = fQOCALoad->CreateArray();
    Int_t dim = fQOCALoad->GetDimensionFast();
    Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
    for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
    delete [] tmp1;
    Float_t* lbdist = fQOCALoad->GetColumn(tmp2, 3); // third column
    delete [] tmp2;
    
    // fill TH2F according as in QOCAFit 
    for(Int_t itheta = 0 ; itheta < ntheta ; itheta++){
      Int_t offset = itheta*nphi;
      for(Int_t iphi = 0 ; iphi < nphi ; iphi++){
	hist[iscan]->SetCellContent(iphi+1,itheta+1,lbdist[iphi+offset]);
      }
    }
    delete [] lbdist;
    
    hist[iscan]->SetXTitle(fXtitle);
    hist[iscan]->SetYTitle(fYtitle);
    hist[iscan]->Draw("colz"); // color code only
    hist[iscan]->SetStats(0); // don't plot stats
    
  } // for(iscan)
 
  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  return canvas;
}

//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAllLBdist2D()
{
  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Char_t wl[128];

  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAllLBdist2D(): \n");
  printf("* Plotting Laserball Distribution for\n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");
  
  TCanvas** carray = new TCanvas*[nwlen];
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    carray[iwlen] = (TCanvas*) PlotLBdist2D((Char_t*)wl);
  }
  printf("--------------------------------------------------------\n");
  return carray;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotLBmask(Char_t* wlen)
{
  // PlotLBmask() with explicit wavelength value.
  //
  // The wavelength is read in Char_t* format so it must be written using quotes.
  // Example: at wlen = 500 nm : PlotLBmask("500");

  SetWlen(wlen);
  return PlotLBmask();
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotLBmask()
{
  // Plots the LB mask TF1 at previously set wavelength.

  // verifications header before plotting takes place
  if(!GetWlen()){
    fprintf(stderr,"In QOCAPlots::PlotLBmask(): No wavelength has been set.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotLBmask(): Setting default input label to 'lbpoly'.\n");
    SetILabel("lbpoly");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotLBmask(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAPlots::PlotLBmask(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotLBmask(): \n");
  printf("* Plotting Laserball Mask function for\n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nkind = GetFlagsSum(), nscan = GetNscan();
  Char_t** scans = GetScanArray();

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024];
  Char_t legendentry[128]; Char_t title[128]; Char_t name[128];

  //SetYRange(1.0, 1.3); SetOffset(0.0); SetFactor(1.3);

  // ---------------
  // Plotting method
  // ---------------

  sprintf(title,"Laserball Mask function at %s nm",GetWlen());
  if(fCtitle == NULL) SetCanvasTitle(title);
  if(fPtitle == NULL) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Polar Angle cos#theta",
		  "Relative Laserball Intensity (arb. units)");
  }

  // name for TCanvas object
  sprintf(name,"lbmask%s",GetWlen());
  TCanvas* canvas = new TCanvas(name,fCtitle);
  
  // TF1 instances to be declared
  TF1** func = new TF1*[dim];

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);

  // dummy histogram for axes purposes
  TH1F* h = new TH1F("h",fPtitle,100,-1,1);
  h->SetXTitle(fXtitle);
  h->SetYTitle(fYtitle);
  h->SetStats(0); // no stats box
  h->Draw();

  Float_t ymax = 1.; // maximum on y-axis (default)
  
  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

      // local index for array access
      Int_t j = iscan+(ikind*nscan);

      sprintf(extractfile,"%s_%s_%s_%s%s",
	      GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());

      fQOCALoad->SetFullname(GetIDir(),extractfile);
      Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
      Int_t dim = fQOCALoad->GetDimensionFast();
      Int_t npars = fQOCALoad->GetNLines();
      Float_t* lbmask = fQOCALoad->ConverttoFloats(
			fQOCALoad->GetColumn(tmp1,1),npars);
      for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
      delete [] tmp1;

      // set TF1 parameters
      Double_t* par = new Double_t[1+npars];
      par[0] = npars;
      for(Int_t ipar = 0 ; ipar < npars ; ipar++){
	par[1+ipar] = (Double_t)lbmask[ipar];
      }
      delete [] lbmask;

      sprintf(name,"lbmask%s%s%s",scans[iscan],GetKind(),GetWlen());
      // use the same function as in QOCAFit to plot
      func[j] = new TF1(name,QOCAFit::sLBMask,(Double_t)-1,(Double_t)1,1+npars);
      // SetParameters(par) deletes the histograms by calling TF1::Update() !!
      func[j]->SetParameters(par); // set all parameters
      
      // update y-maximum
      if(func[j]->GetMaximum() >= ymax){
	ymax = func[j]->GetMaximum() + 0.1; // add small value
      }

      // plotting settings,
      func[j]->SetLineColor(j+1);
      func[j]->SetLineWidth(2);
      func[j]->Draw("Csame"); // smooth curve, same canvas
      
      sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      if(fDisplay) printf(" legend entry: %s\n",legendentry);
      leg->AddEntry(func[j], legendentry,"l");

    } // for(iscan)
  } // for(ikind)

  h->SetMaximum(ymax); // set maximum
  leg->SetBorderSize(1);
  leg->Draw();

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  delete [] func;
  return canvas;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotAllLBmask()
{
  // Plots the LB mask TF1 for all wavelengths.

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotAllLBmask(): Setting default input label to 'lbpoly'.\n");
    SetILabel("lbpoly");
  }
  if(!GetIFormat()){
    if(fDisplay)
      printf("In QOCAPlots::PlotAllLBmask(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){    
      printf("In QOCAPlots::PlotAllLBmask(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAllLBmask(): \n");
  printf("* Plotting Laserball Mask function for\n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan(),nwlen = GetNwlen(), nkind = GetFlagsSum();
  Float_t* wlens = GetWlens();
  Char_t** scans = GetScanArray();

  Float_t div_per = 0.01; // relative division between each pads

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024]; Char_t title[128]; Char_t name[128];
  Char_t wl[128]; Char_t tx[128]; Char_t legendentry[128];

  // ---------------
  // Plotting method
  // ---------------

  sprintf(title,"Laserball Mask Functions");
  if(fCtitle == NULL) SetCanvasTitle(title);
  //if(fPtitle == NULL) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Polar Angle cos#theta",
		  "Relative Laserball Intensity (arb. units)");
  }

  // name for TCanvas object
  sprintf(name,"lbmaskall");
  // single canvas for summary
  TCanvas* canvas = new TCanvas(name,fCtitle,0,0,1200,900);
  canvas->Divide(2, 3, div_per, div_per, 0); // contains 2x3=6 graphs

  TH1F** hist = new TH1F *[nwlen];
  
  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  // a pad for each wlen (all 6 wlens in a single canvas)

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    canvas->cd(iwlen+1);
    // set the wlen each time
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    SetWlen((Char_t*)wl);
    sprintf(name,"lbmask%s",GetWlen());
    sprintf(title,"Laserball Mask function at %s nm",GetWlen());
    // dummy histogram for axes purposes
    hist[iwlen] = new TH1F(name,title,100,-1,1);
    hist[iwlen]->SetXTitle(fXtitle);
    hist[iwlen]->SetYTitle(fYtitle);
    hist[iwlen]->SetStats(0); // no stats box
    hist[iwlen]->Draw();
  
    Float_t ymax = 1.; // maximum on y-axis (default)

    // TF1 instances to be declared
    TF1** func = new TF1*[dim];

    TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			       fLegendpos[2],fLegendpos[3],"Scan Legend");
    leg->SetEntrySeparation(fLegendsep);
    leg->SetMargin(fLegendmar);

    // then do the loop (same as in PlotLBmask())
    for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

      // set the fit type internally
      ChooseKind(ikind);
      
      for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

	// local index for array access
	Int_t j = iscan+(ikind*nscan);
	
	sprintf(extractfile,"%s_%s_%s_%s%s",
		GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());

	fQOCALoad->SetFullname(GetIDir(),extractfile);
	Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
	Int_t npars = fQOCALoad->GetNLines();
	Int_t dim = fQOCALoad->GetDimensionFast();
	Float_t* lbmask = fQOCALoad->ConverttoFloats(
			  fQOCALoad->GetColumn(tmp1,1),npars);
	for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
	delete [] tmp1;

	// set TF1 parameters
	Double_t* par = new Double_t[1+npars];
	par[0] = npars;
	for(Int_t ipar = 0 ; ipar < npars ; ipar++){
	  par[1+ipar] = (Double_t)lbmask[ipar];
	}
	delete [] lbmask;

	sprintf(name,"lbmask%s%s%s",scans[iscan],GetKind(),GetWlen());
	// use the same function as in QOCAFit to plot
	func[j] = new TF1(name,QOCAFit::sLBMask,(Double_t)-1,(Double_t)1,1+npars);
	// SetParameters(par) deletes the histograms by calling TF1::Update() !!
	func[j]->SetParameters(par); // set all parameters
	
	// update y-maximum
	if(func[j]->GetMaximum() >= ymax){
	  ymax = func[j]->GetMaximum() + 0.1; // add small value
	}

	// plotting settings,
	func[j]->SetLineColor(j+1);
	func[j]->SetLineWidth(2);
	func[j]->Draw("Csame"); // smooth curve, same canvas
      
	sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
	if(fDisplay) printf(" legend entry: %s\n",legendentry);
	leg->AddEntry(func[j], legendentry,"l");
	
      } // for(iscan)
    } // for(ikind)
    
    // draw text after
    TText *t = new TText();
    t->SetTextSize(0.10); t->SetTextAlign(12);
    sprintf(tx,"%s nm",GetWlen());
    t->DrawTextNDC(0.15, 0.30, tx); // NDC : [0,1] system
    delete t;
    
    hist[iwlen]->SetMaximum(ymax); // set maximum
    leg->SetBorderSize(1);
    leg->Draw();
    
    //for(Int_t j = 0 ; j < dim ; j++) delete func[j];
    delete [] func;

  } //for(iwlen)

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  return canvas;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotPmtEfficiencies(Char_t* wlen)
{
  // same structure as previous functions

  SetWlen(wlen);
  return PlotPmtEfficiencies();
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotPmtEfficiencies()
{
  // Plot the Pmt relative efficiencies as extracted
  // by QOCAExtract in a TH2F with two scans on each axis
  // as a scatter plot. 
  // In case there is only one scan per wlen, the comparison between 
  // fits and fruns is made, otherwise it exits.
  // The bad efficiencies are set to the default (1).

  // verifications header before plotting takes place
  if(!GetWlen()){
    fprintf(stderr,"In QOCAPlots::PlotPmtEfficiencies(): No wavelength has been set.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotPmtEfficiencies(): Setting default input label to 'eff'.\n");
    SetILabel("eff");
  }
  if(!GetIFormat()){
    if(fDisplay)
      printf("In QOCAPlots::PlotPmtEfficiencies(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){    
      printf("In QOCAPlots::PlotPmtEfficiencies(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotPmtEfficiencies(): \n");
  printf("* Plotting Pmt relative efficiencies at %s nm.\n",GetWlen());

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan(),nkind = GetFlagsSum();
  Char_t** scans = GetScanArray();

  Float_t div_per = 0.01;

  Int_t dim = nkind*nscan;
  Char_t extractfile[1024]; Char_t title[128]; Char_t name[128];

  // only plot efficiencies for different scans
  if(dim <= 1){
    fprintf(stderr,"Only %d scan found in cardfile.\nAborting.\n",dim);
    exit(-1);
  }

  sprintf(name,"pmteff%s",GetWlen());
  TCanvas* canvas;

  // case where ther is only one scan but different kinds
  if((nscan == 1) && (nkind > 1)){
   
    sprintf(title,"PMT Efficiencies Comparison (fits/fruns) at %s nm",GetWlen());
    if(fCtitle == NULL) SetCanvasTitle(title); 
    canvas = new TCanvas(name,fCtitle,600,600);

    // histogram to be filled
    sprintf(title,"PMT Efficiencies Comparison (fits/fruns) for at %s nm %s",scans[0],GetWlen());
    TH2F* heff = new TH2F(name,title,100,0,2.5,100,0,2.5);

    ChooseKind(0); // first kind
    
    sprintf(extractfile,"%s_%s_%s_%s%s",
		GetILabel(),scans[0],GetKind(),GetWlen(),GetIFormat());
	
    fQOCALoad->SetFullname(GetIDir(),extractfile);
    Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t nlines1 = fQOCALoad->GetNLines();
    Int_t ncols1 = fQOCALoad->GetNColumns();
    Float_t* eff1 = fQOCALoad->ConverttoFloats(tmp1,nlines1);
    for(Int_t i = 0 ; i < nlines1*ncols1 ; i++){
      delete [] tmp1[i];
    }
    delete [] tmp1;
    
    // then set histogram axis title
    sprintf(title,"Relative Efficiency for %s (arb. units)",GetKind());
    heff->SetXTitle(title);
    
    // ---------
    ChooseKind(1); // second kind

    sprintf(extractfile,"%s_%s_%s_%s%s",
		GetILabel(),scans[0],GetKind(),GetWlen(),GetIFormat());
	
    fQOCALoad->SetFullname(GetIDir(),extractfile);
    Char_t** tmp2 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
    Int_t nlines2 = fQOCALoad->GetNLines();
    Int_t ncols2 = fQOCALoad->GetNColumns();
    Float_t* eff2 = fQOCALoad->ConverttoFloats(tmp2,nlines2);
    for(Int_t i = 0 ; i < nlines2*ncols2 ; i++){
      delete [] tmp2[i];
    }
    delete [] tmp2;

    // then set histogram axis title
    sprintf(title,"Relative Efficiency for %s (arb. units)",GetKind());
    heff->SetYTitle(title);
    
    // fill
    if(nlines1 != nlines2) printf("Warning: dimension mismatch: %d vs %d",
				  nlines1, nlines2);
    for(Int_t j = 0 ; j < nlines1 ; j++){
      if(eff1[j] == 1.0 || eff2[j] == 1.0) continue; // don't reduce scale
      else heff->Fill(eff1[j],eff2[j]);
    } 
    
    // draw with scale and stats (give mean and rms on both sides)
    heff->Draw("colz"); // color code only
    heff->SetStats(0);  // plot stats
    // put the stats in left corner
    //TPaveStats *st = (TPaveStats*) heff->FindObject("stats");
    //st->SetX1NDC(0.15); //new x start position
    //st->SetY1NDC(0.75); //new x end position

  }
  
  else {

    // there is more than one scan.
    // reduce the amount of plots by plotting only fruns.
    if(nkind > 1 && fDisplay){    
      printf("In QOCAPlots::PlotPmtEfficiencies(): Setting default fit type to 'fruns'\n");
      SetFrunsFlag(1);
    }

    // get the number of plots
    Int_t nplot = 0; // default
    for(Int_t iplot = nscan ; iplot > 1 ; iplot--) nplot += (iplot-1);
   
    // contains nplot in one column
    sprintf(title,"PMT Efficiencies Comparison at %s nm",GetWlen());
    if(fCtitle == NULL) SetCanvasTitle(title);
    canvas = new TCanvas(name,fCtitle,600,1200);
    canvas->Divide(1,nplot, div_per, div_per, 0); 

    // array
    Float_t** eff = new Float_t*[nscan];
    TH2F** heff = new TH2F*[nplot];

    Int_t npmt = 0;
    // loop first time to fill arrays
    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan
      
      sprintf(extractfile,"%s_%s_%s_%s%s",
	      GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());
      
      fQOCALoad->SetFullname(GetIDir(),extractfile);
      Char_t** tmp = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
      Int_t nlines = fQOCALoad->GetNLines();
      Int_t ncols = fQOCALoad->GetNColumns();
      Float_t* effscan = fQOCALoad->ConverttoFloats(tmp,nlines);
      for(Int_t i = 0 ; i < nlines*ncols ; i++){
	delete [] tmp[i];
      } delete [] tmp;
      eff[iscan] = effscan; // assign pointer to array
      npmt = nlines;
    } // for(iscan)

    Int_t counter = 0;
    // then double-loop until enough plots
    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){
      for(Int_t jscan = iscan+1 ; jscan < nscan ; jscan++){
	if(iscan == jscan) continue;
	else {
	  if(counter > nplot){
	    fprintf(stderr,"In PlotPmtEfficiencies(): Counter exceeded the number of plots: %d vs %d\n",
		    counter,nplot);
	  }
	  canvas->cd(counter+1);
	  sprintf(name,"pmteff%s_%s%s",GetWlen(),scans[iscan],scans[jscan]);
	  sprintf(title,"PMT Efficiencies Comparison at %s nm (%s vs %s) ",
		  GetWlen(),scans[iscan],scans[jscan]);
	  heff[counter] = new TH2F(name,title,100,0,2.5,100,0,2.5);
	  sprintf(title,"Relative Efficiency for %s (%s) (arb. units)",
		  scans[iscan],GetKind());
	  heff[counter]->SetYTitle(title);
	  sprintf(title,"Relative Efficiency for %s (%s) (arb. units)",
		  scans[jscan],GetKind());
	  heff[counter]->SetXTitle(title);

	  // fill
	  for(Int_t ipmt = 0 ; ipmt < npmt ; ipmt++){
	    // do not plot 1.0 (too many: blow up scale)
	    if(eff[iscan][ipmt] == 1.0 || eff[jscan][ipmt] == 1.0) continue;
	    else heff[counter]->Fill(eff[iscan][ipmt],eff[jscan][ipmt]);
	  }

	  // draw
	  heff[counter]->Draw("colz");
	  heff[counter]->SetStats(0);
	  counter++;
	  
	} // else

      } // for(jscan)
    } // for(iscan)

    // deleting loop
    for(Int_t iscan = 0 ; iscan < nscan ; iscan++) delete [] eff[iscan];
    delete [] eff;

  } // else 

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  return canvas;
}

//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAllPmtEfficiencies()
{
  // Same as PlotPmtEfficiencies() but for all wlen.

  Int_t nwlen = GetNwlen();
  Float_t* wlens = GetWlens();
  Char_t wl[128];

  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAllPmtEfficiencies(): \n");
  printf("* Plotting Pmt relative efficiencies for\n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");
  
  TCanvas** carray = new TCanvas*[nwlen];
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    carray[iwlen] = (TCanvas*) PlotPmtEfficiencies((Char_t*)wl);
  }
  printf("--------------------------------------------------------\n");
  return carray;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotChiSquare()
{
  // Plot the reduced ChiSquare vs Wavelength
  // usually for available wavelengths, scans and fit type
  // found in the cardfile
  // -------
  // Inputs and Outputs: similar to ExtractAttenuation()
  // -----------

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotChiSquare(): Setting default input label to 'chi'.\n");
    SetILabel("chi");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotChiSquare(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){
      printf("In QOCAPlots::PlotChiSquare(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'.\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotChiSquare(): \n");
  printf("* Plotting Reduced ChiSquare Results at the end of fit for \n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nkind = GetFlagsSum(),nscan = GetNscan(),nwlen = GetNwlen();
  Char_t** scans = GetScanArray();
  Float_t* wlens = GetWlens();

  // create the list of extraction files to access
  Int_t dim = nkind*(nwlen)*(nscan);
  Char_t extractfile[1024], name[128], title[128];

  // vectors to store the local data
  Float_t* chis = new Float_t[dim]; // chisquares
  Float_t* errors = new Float_t[dim]; // errors

  // loop over : loading data from textfiles
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ // each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ // each scan

      // local index for array access
      Int_t i = iscan+(ikind*nscan);

      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){ // each wlen

	// local index for array access
        Int_t j = iwlen+(i*nwlen);

	sprintf(extractfile,"%s_%s_%s_%d%s",
		GetILabel(),scans[iscan],GetKind(),(Int_t)wlens[iwlen],GetIFormat());

        // access the textfiles and keep the data to plot
	fQOCALoad->SetFullname(GetIDir(), extractfile);
        Char_t** tmp1 = fQOCALoad->CreateArray(fQOCALoad->GetFullname());
	Int_t dim = fQOCALoad->GetDimensionFast();

	// convert the Char_t* format to Float_t
        chis[j] = atof(tmp1[0]); // first column first line
        errors[j] = atof(tmp1[2]); // first column second line

	// print the numbers for debugging
	if(fDisplay) printf("Chi(%d) = %.2f +/- %.2f\n",j,chis[j],errors[j]);
	for(Int_t i = 0 ; i < dim ; i++) delete [] tmp1[i];
	delete [] tmp1;

      } //iwlen
    } //iscan
  } //ikind

  // ---------------
  // Plotting method
  // ---------------

  // usual plot and axes titles
  // do not change; no need to put them into a cardfile
  sprintf(title,"Reduced #chi^{2} from QOCAFit Results");
  if(fCtitle == NULL) SetCanvasTitle(title);
  if(fPtitle == NULL) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Wavelength (nm)",
		  "Reduced #chi^{2} (arb. units)");
  }

  // name for TCanvas object
  sprintf(name,"chi2");
  TCanvas* canvas = new TCanvas(name,fCtitle);

  // dynamic range setting based on the data to plot
  SetYRange(FindMinimum(chis, dim), FindMaximum(chis, dim));
  SetOffset(FindMaximum(errors, dim));
  // allow a space between the real maximum and the plot maximum
  SetFactor(1.5);

  // class to plot histograms with points and error bars
  TGraphErrors** grph = new TGraphErrors *[nkind*nscan];

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);


  // loop over : reorganizing data for each scan
  //
  // basically, each fit type of each scan
  // has its own color and TGraphErrors object.
  // each TGraphErrors instance has nwlen data points
  // with no errors on the wavelength values
  //
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  // index for chis and errors values : outside the for loops
  Int_t index = 0;

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //new color for each scan

      // local index for array access
      Int_t i = iscan+(ikind*nscan);
      Float_t* chiss = new Float_t[nwlen];
      Float_t* errs = new Float_t[nwlen];

      // reorganize the data in local arrays
      for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
	chiss[iwlen] = chis[index];
	errs[iwlen] = errors[index];
	index++;
      }

      // declare each graph instance with the newly reorganized arrays
      grph[i] = new TGraphErrors(nwlen, wlens, chiss, 0, errs);

      // set the color and marker options
      GraphSettings(grph[i], i, 1);

      // first time around
      if(i==0){ (grph[i])->Draw("AP");
	// set the maxima and minima
	GraphSettings(grph[i]); 
      }
      else{(grph[i])->Draw("P");} //draw others on top

      // create legend entries
      Char_t legendentry[128]; sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      if(fDisplay) printf(" legend entry: %s\n",legendentry);
      leg->AddEntry(grph[i], legendentry,"p");
      leg->SetBorderSize(1);
      leg->Draw();

      delete [] chiss;
      delete [] errs;

    } // for(iscan)
  } // for(ikind)

  canvas->SetFillColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  delete [] grph;
  return canvas;
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotTimeWindows(Char_t* medium)
{
  // PlotAttenuationDrift() with explicit medium setting.
  //
  // Valid options:
  //   - NULL or "D2O" for heavy water
  //   - "ACR" for acrylic
  //   - "H2O" for light water

  SetMedium(medium);
  return PlotTimeWindows();
}

//______________________________________________________________________________
TCanvas* QOCAPlots::PlotTimeWindows()
{
  // Plot the Inverse Attenuation Lengths vs Prompt time window half-width
  // usually for 1 specified wlen, for all scans and fit types
  // found in the cardfile.
  //
  // A fit is done over the data of each wavelength
  // fitting options (polynomial fit order) 
  // can be specified in the cardfile with the fit limits.
  // 

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotTimeWindows(): Setting default input label to 'timewindows'.\n");
    SetILabel("timewindows");
  }
  if(!GetIFormat()){
    if(fDisplay) printf("In QOCAPlots::PlotTimeWindows(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){   
      printf("In QOCAPlots::PlotTimeWindows(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotTimeWindows(): \n");
  printf("* Plotting Inverse Attenuation Lengths\n");
  printf("* as a function of the prompt time window half-width for\n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nkind = GetFlagsSum();
  if(nkind == 0 || nkind >= 2){
    fprintf(stderr,"In QOCAPlots::PlotTimeWindows(): Can't plot more than one fit type here.\n");
    fprintf(stderr,"Aborting.\n"); exit(-1);
  }

  Int_t nscan = GetNscan();
  Char_t** scans = GetScanArray();

  // create the list of extraction files to access
  Int_t dim = nkind*nscan;
  Char_t extractfile[1024];
  Char_t legendentry[128]; Char_t name[128]; Char_t title[128];

  // ---------------
  // Plotting method
  // ---------------

  sprintf(title,"%s Inverse Attenuation Lengths vs Prompt Window Half-Width at %s nm",
	  GetMedium(),GetWlen());
  if(fCtitle) SetCanvasTitle(title);
  if(fPtitle) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Time Window Half-Width (ns)",
		  "Inverse Attenuation Length (cm^{-1})");
  }

  // name for TCanvas object
  if(GetMediumFlag() == 0) sprintf(name,"twd2o%s",GetWlen());
  if(GetMediumFlag() == 1) sprintf(name,"twacr%s",GetWlen());
  if(GetMediumFlag() == 2) sprintf(name,"twh2o%s",GetWlen());
  TCanvas* canvas = new TCanvas(name,fCtitle);

  // Vectors
  Float_t* bins; // bins (0 to 90)
  Float_t* attens; // attenuations
  Float_t* errors; // errors

  // TGrapherrors instances to be declared
  TGraphErrors** grph = new TGraphErrors *[dim];

  TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			     fLegendpos[2],fLegendpos[3],"Scan Legend");
  leg->SetEntrySeparation(fLegendsep);
  leg->SetMargin(fLegendmar);

  // loop over :
  // ikind: fit type (fits, fruns or both)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

      // local index for array access
      Int_t j = iscan+(ikind*nscan);

      sprintf(extractfile,"%s%s_%s_%s_%s%s",
	      GetMedium(),GetILabel(),scans[iscan],
	      GetKind(),GetWlen(),GetIFormat());

      // Access the files and put all the content in one array
      Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
      Char_t** tmp1 = fQOCALoad->CreateArray();
      Int_t nlines = fQOCALoad->GetNLines();
      Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
      bins = fQOCALoad->GetColumn(tmp2, 1); //first columns
      attens = fQOCALoad->GetColumn(tmp2, 2); //second column
      errors = fQOCALoad->GetColumn(tmp2, 3); //third column
      for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
      delete [] tmp1;

      grph[j] = new TGraphErrors(nlines, bins, attens, 0, errors);
      GraphSettings(grph[j], j, 1); //markersize

      if(j==0){ //first time around
	// dynamic range setting based on the data to plot
	SetYRange(FindMinimum(attens, nlines), FindMaximum(attens, nlines));
	SetOffset(FindMaximum(errors, nlines));
        (grph[j])->Draw("AP");
	GraphSettings(grph[j]);
      } else{(grph[j])->Draw("P");}

      sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      if(fDisplay) printf(" legend entry: %s\n",legendentry);
      leg->AddEntry(grph[j], legendentry,"p");
      leg->SetBorderSize(1);
      leg->Draw();

      delete bins; delete attens; delete errors;

    } // for(iscan)
  } // for(ikind)

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  delete [] grph;
  return canvas;

}

//______________________________________________________________________________
TCanvas** QOCAPlots::PlotAllTimeWindows()
{
  // Similar to PlotTimeWindows() but produces one more canvas
  // with the summary.

  // verifications header before plotting takes place
  if(!GetILabel()){
    if(fDisplay) printf("In QOCAPlots::PlotAllTimeWindows(): Setting default input label to 'timewindows'.\n");
    SetILabel("timewindows");
  }
  if(!GetIFormat()){
    if(fDisplay)
      printf("In QOCAPlots::PlotAllTimeWindows(): Setting default input format to '.txt'.\n");
    SetIFormat(".txt");
  }
  if(!GetFlagsSum()){
    if(fDisplay){    
      printf("In QOCAPlots::PlotAllTimeWindows(): No choice of files have been made.\n");
      printf("Setting default fit type to 'fruns'\n");
    }
    SetFrunsFlag(1);
  }
  printf("--------------------------------------------------------\n");
  printf("* In QOCAPlots::PlotAllTimeWindows(): \n");
  printf("* Plotting Inverse Attenuation Lengths\n");
  printf("* as a function of the prompt time window half-width for\n");
  VerifyFlags(); ChooseMedium();
  printf("--------------------------------------------------------\n");

  // copy cardfile stuff loaded by QOCAAnalysis for local use
  Int_t nscan = GetNscan(),nwlen = GetNwlen(),nkind = GetFlagsSum();
  Float_t* wlens = GetWlens();
  Char_t** scans = GetScanArray();

  Float_t div_per = 0.01; // relative division between each pads

  // Create the list of extraction files to access
  Int_t dim = nkind*nscan*nwlen;
  Char_t extractfile[1024]; Char_t title[1024]; Char_t name[128];
  Char_t wl[128]; Char_t tx[128]; Char_t legendentry[128];

  // ---------------
  // Plotting method
  // ---------------

  sprintf(title,"%s Inverse Attenuation Lengths vs Prompt Window Half-Width at %s nm",
	  GetMedium(),GetWlen());
  if(fCtitle) SetCanvasTitle(title);
  if(fPtitle) SetPlotTitle(title);
  if((fXtitle == NULL) && (fYtitle == NULL)){
    SetAxisTitles("Time Window Half-Width (ns)",
		  "Inverse Attenuation Length (cm^{-1})");
  }

  // array
  TCanvas** carray = new TCanvas*[nwlen+1];

  // name for TCanvas object
  if(GetMediumFlag() == 0) sprintf(name,"twalld2o");
  if(GetMediumFlag() == 1) sprintf(name,"twallacr");
  if(GetMediumFlag() == 2) sprintf(name,"twallh2o");
  // single canvas for summary
  TCanvas* canvas = new TCanvas(name,fCtitle,0,0,1200,900);
  canvas->Divide(2, 3, div_per, div_per, 0); // contains 2x3=6 graphs

  // vectors
  Float_t* bins;      // bins (0 to 90)
  Float_t* attens;  // angular responses
  Float_t* errors;    // errors

  TGraphErrors*** grph = new TGraphErrors **[nwlen];
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){
    grph[iwlen] = new TGraphErrors *[dim];
  }

  // loop over :
  // ikind: fit type (fits, fruns, or eff)
  // iscan: scan (from cardfile)
  // iwlen: wavelengths (from cardfile)
  // a pad for each wlen (all 6 wlens in a single canvas)

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    // set the wlen each time
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    SetWlen((Char_t*)wl);

    TLegend* leg = new TLegend(fLegendpos[0],fLegendpos[1],
			       fLegendpos[2],fLegendpos[3],"Scan Legend");
    leg->SetEntrySeparation(fLegendsep);
    leg->SetMargin(fLegendmar);

    sprintf(title,"%s Inverse Attenuation Lengths vs Prompt Window Half-Width at %.3d nm",
      GetMedium(),(Int_t)wlens[iwlen]);
    SetPlotTitle(title);

    canvas->cd(iwlen+1); //pad selection

    for(Int_t ikind = 0 ; ikind < nkind ; ikind++){ //each fit

      // set the fit type internally
      ChooseKind(ikind);

      for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //each scan

	Int_t j = iscan+(ikind*nscan);

	sprintf(extractfile,"%s%s_%s_%s_%s%s",
	  GetMedium(),GetILabel(),scans[iscan],GetKind(),GetWlen(),GetIFormat());

	// Access the files and put all the content in one array
	Int_t adim = fQOCALoad->GetDimension(GetIDir(), extractfile);
	Char_t** tmp1 = fQOCALoad->CreateArray();
	Int_t nlines = fQOCALoad->GetNLines();
	Float_t* tmp2 = fQOCALoad->ConverttoFloats(tmp1, adim);
	bins = fQOCALoad->GetColumn(tmp2, 1); //first columns
	attens = fQOCALoad->GetColumn(tmp2, 2); //second column
	errors = fQOCALoad->GetColumn(tmp2, 3); //third column
	for(Int_t i = 0 ; i < adim ; i++) delete [] tmp1[i];
	delete [] tmp1;

	grph[iwlen][j] = new TGraphErrors(nlines, bins, attens, 0, errors);
	GraphSettings(grph[iwlen][j], j, 0.5); // markersize

	if(j==0){ //first time around
	  // dynamic range setting based on the data to plot
	  SetYRange(FindMinimum(attens, nlines), FindMaximum(attens, nlines));
	  SetOffset(FindMaximum(errors, nlines));
	  // allow a space between the real maximum and the plot maximum
	  SetFactor(1.5);
	  (grph[iwlen][j])->Draw("AP");
	  GraphSettings(grph[iwlen][j]);
	} else {(grph[iwlen][j])->Draw("P");}

	sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
        if(fDisplay) printf(" legend entry: %s\n",legendentry);
        leg->AddEntry(grph[iwlen][j], legendentry,"p");
	leg->SetBorderSize(1);
	leg->Draw();

	// draw text after
	TText *t = new TText();
	t->SetTextSize(0.10); t->SetTextAlign(12);
	sprintf(tx,"%.3d nm",(Int_t)wlens[iwlen]);
	t->DrawTextNDC(0.15, 0.50, tx); // NDC : [0,1] system
	delete t;

	delete bins; delete attens; delete errors;

      } // for(iscan)
    } // for(ikind)
  } //for(iwlen)

  canvas->cd();
  canvas->SetFillColor(kWhite);
  canvas->SetHighLightColor(kWhite);
  canvas->SetFrameFillColor(kWhite);
  carray[0] = canvas;

  
  // then one canvas per wlen, for convenience
  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++){

    // set the wlen each time
    sprintf(wl,"%.3d",(Int_t)wlens[iwlen]);
    SetWlen((Char_t*)wl);

    TCanvas* cw = PlotTimeWindows();
    carray[iwlen+1] = cw;
  } //for(iwlen)
  printf("--------------------------------------------------------\n");

  for(Int_t iwlen = 0 ; iwlen < nwlen ; iwlen++) delete [] grph[iwlen];
  delete [] grph;
  return carray;
}

//______________________________________________________________________________
void QOCAPlots::GraphSettings(TGraphErrors* g)
{
  // Small function to set graph options:
  // - axes limits
  // - titles

  g->SetMaximum(fYmax+(fFactor*fOffset));
  g->SetMinimum(fYmin-(fFactor*fOffset)); //can be cancelled
  g->GetHistogram()->SetXTitle(fXtitle);
  g->GetHistogram()->SetYTitle(fYtitle);
  g->SetTitle(fPtitle);

  return;
}

//______________________________________________________________________________
void QOCAPlots::GraphSettings(TGraphErrors* g, Int_t index, Float_t markersize)
{
  // Small function to set graph options:
  // - line colors and width 
  // - marker styles, colors and size

  Int_t j = index + 20; // starts at 20
  if(j >= 30) j -= 10;
  if(index >= 19) index = 0; // fColor[10]
  g->SetMarkerColor(fColor[index]);
  g->SetLineColor(fColor[index]);
  g->SetMarkerStyle(j);
  g->SetMarkerSize(markersize);
  g->SetLineWidth(1);

  // display for debugging
  if(index == 0 && fDisplay) printf("* TGraphErrors instances accessed: \n");
  if(fDisplay) printf("%d ",index);

  return;

}

//______________________________________________________________________________
TCanvas* QOCAPlots::MakeBigLegend()
{

  // Plots a big legend as a TCanvas to distinguish
  // small characters.

  Int_t nkind = GetFlagsSum(),nscan = GetNscan();
  Char_t** scans = GetScanArray();

  Float_t* fakex = new Float_t[2];
  fakex[0] = 0; fakex[1] = 1;
  Float_t* fakey = new Float_t[2];
  fakey[0] = 0; fakey[1] = 1;
	
  TLegend* bigleg = new TLegend(0.1,0.1,0.9,0.9);
  TCanvas* legend = new TCanvas("leg","Legend");

  // fake graphs 
  TGraphErrors** g = new TGraphErrors *[nkind*nscan];

  for(Int_t ikind = 0 ; ikind < nkind ; ikind++){

    // set the fit type internally
    ChooseKind(ikind);

    for(Int_t iscan = 0 ; iscan < nscan ; iscan++){ //new color for each scan

      Int_t i = iscan+(ikind*nscan);

      g[i] = new TGraphErrors(1,fakex,fakey,0,0);

      // set the color and marker options
      // do the same as GraphSettings
      Int_t k = i+1;
      if(k >= 8) k = 10*k + 1;
      Int_t j = i + 20; // starts at 20
      if(j >= 30) j -= 10;
      //g[i]->SetMarkerColor(k);
      //g[i]->SetLineColor(k);
      g[i]->SetMarkerColor(i);
      g[i]->SetLineColor(i);
      g[i]->SetMarkerStyle(j);
      g[i]->SetMarkerSize(3.0);
   
      Char_t legendentry[128]; sprintf(legendentry,"%s (%s)",scans[iscan],GetKind());
      bigleg->AddEntry(g[i], legendentry,"p");
      bigleg->SetBorderSize(5);
      bigleg->Draw();

    } // for(iscan)
  } // for(ikind)

  legend->cd();
  bigleg->Draw(); 
  delete [] g;
  return legend; 
}
