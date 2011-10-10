// header file for QOCAPlots.cxx

#ifndef _QOCAPLOTS_
#define _QOCAPLOTS_

#include <QOCAAnalysis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TH1.h>
#include <TF1.h>
#include <TH2.h>
#include <TText.h>
#include <TPaveStats.h>

class QOCAPlots : public QOCAAnalysis {

 private:

  Char_t* fWlen;		// local class wavelength
  Int_t fPmtbins;		// number of PMT bins in TH1Fs

  Float_t fLegendpos[4];	// stores TLegend position coordinates
  Float_t fLegendsep; 
  Float_t fLegendmar;
  Char_t* fCtitle, * fPtitle;	// plot titles. C: Canvas, P: Plot,
  Char_t* fXtitle, * fYtitle;	// plot titles  X,Y: Axis
  Float_t fXmin, fXmax, fYmin, fYmax;   // for axis range
  Float_t fOffset;              // an offset to see error bars, use with care
  Float_t fFactor;              // a factor to multiply an offset by
  Int_t fMarker;                // to keep a marker style integer in memory
  Float_t fRcorr[6];            
  Float_t fCorrfactor;          
  Int_t fColor[20];             // array of pre-determined colors for plotting

  Bool_t fDisplay;		// message display flag

  QOCALoad* fQOCALoad;		// loading class pointer

  TStyle* fStyle;               // plotting style


 public:

  QOCAPlots(Char_t* optionfile = NULL, Bool_t mode = kTRUE, Bool_t display = kTRUE,
  	    Char_t* edirectory = NULL, Char_t* pdirectory = NULL);

  ~QOCAPlots();

  // ----------
  // Setters
  void SetWlen(Char_t* w = NULL){fWlen = w;}
  void SetLegendPos(Float_t x1 = 0.8, Float_t y1 = 0.8, Float_t x2 = 1.0, Float_t y2 = 1.0){
    fLegendpos[0] = x1; fLegendpos[1] = y1;
    fLegendpos[2] = x2; fLegendpos[3] = y2;
  }
  void SetLegendOpt(Float_t sep = 0.5, Float_t margin = 0.4){
    fLegendsep = sep; fLegendmar = margin;
  }
  void SetCanvasTitle(Char_t* tc = NULL){fCtitle = tc;}
  void SetPlotTitle(Char_t* tp = NULL){fPtitle = tp;}
  void SetAxisTitles(Char_t* tx = NULL, Char_t* ty = NULL){
    fXtitle = tx; fYtitle = ty;
  }
  void SetXRange(Float_t xmin = 0.0, Float_t xmax = 1.0){
    fXmin = xmin; fXmax = xmax;
  }
  void SetYRange(Float_t ymin = 0.0, Float_t ymax = 1.0){
    fYmin = ymin; fYmax = ymax;
  }
  void SetNpmtBins(Int_t npmtbins = 10000){fPmtbins = npmtbins;}
  void SetOffset(Float_t offset = 0.0){fOffset = offset;}
  void SetFactor(Float_t factor = 1.0){fFactor = factor;}
  void SetMarker(Int_t style = 22){fMarker = style;}
  void SetCorr(Float_t c337 = 0.0, Float_t c365 = 0.0, Float_t c386 = 0.0, 
	       Float_t c420 = 0.0, Float_t c500 = 0.0, Float_t c620 = 0.0){
    fRcorr[0] = c337; fRcorr[1] = c365; 
    fRcorr[2] = c386; fRcorr[3] = c420;
    fRcorr[4] = c500; fRcorr[5] = c620;
  }
  void SetCorrFactor(Float_t fac = 0.0){fCorrfactor = fac;}
  void SetPlotDisplay(Bool_t d = kFALSE){fDisplay = d;}
  void SetStyle(TStyle* aStyle);

  // ----------
  // Getters
  Char_t* GetWlen(){return fWlen;}
  Char_t* GetCanvasTitle(){return fCtitle;}
  Char_t* GetPlotTitle(){return fPtitle;}
  Char_t* GetAxisTitle(Char_t* name){
    if(!strcmp(name, "x")){return fXtitle;}
    else{if(!strcmp(name, "y")){return fYtitle;}
         else{cerr << "No axis title to return (QOCAPlots::GetAxisTitle)" << endl; exit(-1);}}
  }
  Float_t GetXMax(){return fXmax;}
  Float_t GetXMin(){return fXmin;}
  Float_t GetYMax(){return fYmax;}
  Float_t GetYMin(){return fYmin;}
  Int_t GetNpmtBins(){return fPmtbins;}
  Float_t GetOffset(){return fOffset;}
  Float_t GetFactor(){return fFactor;}
  Int_t GetMarker(){return fMarker;}
  Float_t* GetCorr(){return fRcorr;}
  Float_t GetCorrFactor(){return fCorrfactor;}
  Bool_t GetPlotDisplay(){return fDisplay;}

  // ----------
  // Other function prototypes
  Float_t FindMaximum(TH1F**, const Int_t);
  Float_t FindMaximum(Float_t*, const Int_t&);
  Float_t FindMinimum(Float_t*, const Int_t&);
  void VerifyFlags();
  void ChooseMedium();
  void ChooseKind(Int_t);

  // Plotting functions of extractable fit parameters
  TCanvas* PlotAttenuation(Char_t*);
  TCanvas* PlotAttenuation();
  TCanvas** PlotAttenuationDrift(Char_t*);
  TCanvas** PlotAttenuationDrift();
  TCanvas* PlotPmtr(Char_t*,Int_t pmtgroup = 1);
  TCanvas* PlotPmtr(Int_t pmtgroup = 1);
  TCanvas** PlotAllPmtr(Int_t pmtgroup = 1);
  TCanvas* PlotPmtrError(Char_t*, Char_t*);
  TCanvas* PlotLBdist1D(Char_t*, Int_t);
  TCanvas* PlotLBdist1D(Int_t);
  TCanvas** PlotAllLBdist1D();
  TCanvas* PlotLBdist2D(Char_t*);
  TCanvas* PlotLBdist2D();
  TCanvas** PlotAllLBdist2D();
  TCanvas* PlotLBmask(Char_t*);
  TCanvas* PlotLBmask();
  TCanvas* PlotAllLBmask();
  TCanvas* PlotChiSquare();
  TCanvas* PlotPmtEfficiencies(Char_t*);
  TCanvas* PlotPmtEfficiencies();
  TCanvas** PlotAllPmtEfficiencies();
  TCanvas* MakeBigLegend();
  TCanvas* PlotTimeWindows(Char_t*);
  TCanvas* PlotTimeWindows();
  TCanvas** PlotAllTimeWindows();
  
  // Various Settings Functions
  void SetDefaultOptions();
  void GraphSettings(TGraphErrors*);
  void GraphSettings(TGraphErrors*, Int_t, Float_t);

  ClassDef(QOCAPlots,0) // QOCA Automatic Plotting Class
};

#endif
