// Using QOCAPlots.cxx functions to plot
// Optics related extracted parameters
// -----
// Author: Olivier Simard - November 2004
// -----
//
// ------------------------------------------

#include <QOCAPlots.h>

int main(Int_t, Char_t**);

int main(Int_t Narg, Char_t** arg){

  // -----------------------------------------------------
  //
  // Produces plots by reading the extracted files
  //
  // Medium and fit type are selected through the function
  // QOCAPlots::SetFlags(fits, fruns, medium)
  // where the arguments are integers:
  //
  // fits, fruns = 0 : do not plot this data
  // fits, fruns = 1 : plot this data
  // medium: (0: D2O, 1:ACR, 2:H2O) 
  //   medium only needed for attenuation data, 
  //   pmt angular response will ignore it
  //
  // examples:
  // - (0,0,0) or () : default = plot fruns for D2O
  // - (1,0,0) or (1): plot only fits data for D2O
  // - (1,1,2) : plot both fits/fruns data for H2O
  //
  // note: some function will not plot both fits/fruns 
  // on the same plot. warnings exist in the code anyway. 
  //
  // Input/Ouput labels and format only need to be specified
  // if multiple function calls are made.
  //
  // -----------------------------------------------------

  // rearrange the number of arguments
  Int_t N = Narg - 1;
  Int_t Nfixed = 11; // 10 flags + cardfile name

  // quick check of the arguments
  if(N <= 0){
    fprintf(stderr,"Bad arguments. Usage:\n");
    fprintf(stderr," > %s [cardfile] [flags]\n",arg[0]);
    exit(-1);
  }

  // keep the cardfile name
  Char_t* cardfile = arg[1];
  N--;

  // create an array of integers to contain the flags
  Int_t* qplot_flag = new Int_t[Nfixed - 1];

  // fill the rest of the array if the flags
  // have been omitted by the user
  if(N < Nfixed){
    printf("Forcing default flag values for last %d arguments.\n",Nfixed-N);
  }
  for(Int_t iflag = 0 ; iflag < N ; iflag++){
    qplot_flag[iflag] = atoi(arg[iflag+2]);
  }
  for(Int_t iflag = N ; iflag < Nfixed ; iflag++){
    qplot_flag[iflag] = 0;
  }

  // print flag summary
  printf("------------------------------------------------------\n");
  printf("Argument Summary:\n");
  printf("Executable: %s\nCardfile: %s\n",arg[0],arg[1]);
  printf("Flags:\n");
  printf("\t%30s: \t%d\n","QOCAFit Reduced ChiSquare",qplot_flag[0]);
  printf("\t%30s: \t%d\n","Media Attenuations",qplot_flag[1]);
  printf("\t%30s: \t%d\n","PMT Angular Response",qplot_flag[2]);
  printf("\t%30s: \t%d\n","Laserball Distribution",qplot_flag[3]);
  printf("\t%30s: \t%d\n","Laserball Mask Function",qplot_flag[4]);
  printf("\t%30s: \t%d\n","PMT Model Efficiencies",qplot_flag[5]);
  printf("\t%30s: \t%s\n","Averages","N/A");
  printf("\t%30s: \t%d\n","Time Dependencies",qplot_flag[7]);
  printf("\t%30s: \t%d\n","Time Windows",qplot_flag[8]);
  printf("\t%30s: \t%s\n","Unused","N/A");
  printf("------------------------------------------------------\n");


  // loading class with specified cardfile location
  QOCALoad* ql = new QOCALoad();
  ql->SetFullname(
  	ql->AppendElement((Char_t*)getenv("PWD"), "/"), cardfile);

  // load input/output directories
  ql->SetKeyword("io");
  Char_t** dirs = ql->CreateArray(ql->GetFullname());

  // here is loaded fit specific data 
  // only needed by QOCAPlots::PlotAttenuationTimeDependence()
  ql->SetKeyword("fit");
  Char_t** fitoptions = ql->CreateArray(ql->GetFullname());
  Int_t ncol = ql->GetNColumns();
  Float_t* rcorr = ql->ConverttoFloats(ql->GetLine(fitoptions,1), ncol);
  Float_t factor = atof((Char_t*)((ql->GetLine(fitoptions,2))[0]));
  Char_t* fitfunc = (Char_t*) ((ql->GetLine(fitoptions,3))[0]);

  // load the data to plot (fits or fruns or both)
  ql->SetKeyword("data");
  Char_t** data_c = ql->CreateArray(ql->GetFullname());
  Int_t* data = ql->ConverttoIntegers(ql->GetLine(data_c,ql->GetNLines()), ql->GetNColumns());
  Int_t fits_flag = data[0];
  Int_t fruns_flag = data[1];
  Int_t eff_flag = data[2];

  // Specify input/output directories
  QOCAPlots* qplots = new QOCAPlots(cardfile, 1, 0, dirs[1], dirs[2]);
  
  // **********************************

  // TCanvas pointers
  TCanvas* a, ** b;

  // create the rootfile that will the canvas
  Char_t* file = ql->AppendElement(qplots->GetODir(), "plots_new.root");
  TFile* f = new TFile(file, "RECREATE");
  f->cd();

  // --------------------------------------
  // set up data to plot
  qplots->SetFlags(fits_flag,fruns_flag,eff_flag);

  // legend options
  qplots->SetLegendOpt(0.5, 0.4);
  Float_t legwx = 0.25;
  Float_t legwy;
  if(qplots->GetNscan() > 1) legwy = 0.25;
  else legwy = 0.15;

  // Legend plot
  a = (TCanvas*) qplots->MakeBigLegend();
  a->Write();
  f->cd();
  delete a;

  // set input textfile according to tree names
  if(qplots->GetIsSyst()){
    Char_t* iformat = ql->AppendElement(
		      ql->AppendElement("_",qplots->GetSystName(0)),".txt");
    qplots->SetIFormat(iformat);
  } else qplots->SetIFormat(".txt");

  // set the default medium
  qplots->SetMedium("D2O");
  
  // Reduced ChiSquare plots
  if(qplot_flag[0]){
    qplots->SetMedium("D2O");
    qplots->SetILabel("chi");
    qplots->SetOLabel("chi");
    qplots->SetAxisTitles("Wavelength (nm)",
			"Reduced #chi^{2} (arb. units)");
    qplots->SetLegendPos(0.565, 0.90-legwy, 0.565+legwx, 0.90);
    a = (TCanvas*) qplots->PlotChiSquare();
    a->Write();
    f->cd();
    delete a;
  }
  
  // --------------------------------------
  // Attenuation plots
  if(qplot_flag[1]){
    qplots->SetILabel("attenuation");
    qplots->SetOLabel("att");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetAxisTitles("Wavelength (nm)",
			"Inverse Attenuation Lengths (cm^{-1})");
    qplots->SetLegendPos(0.60, 0.90-legwy, 0.60+legwx, 0.90);
    a = (TCanvas*) qplots->PlotAttenuation("D2O");
    a->Write();
    delete a;
    f->cd();
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetLegendPos(0.15, 0.90-legwy, 0.15+legwx, 0.90);
    a = (TCanvas*) qplots->PlotAttenuation("H2O");
    a->Write();
    delete a;
    f->cd();
  }

  // --------------------------------------
  // PMT Angular Response plots
  // 09.2006 - O.Simard
  // Change to extract both pmt angular response if the bit is 2.
  if(qplot_flag[2] > 0){
    qplots->SetILabel("pmtAngResp");
    qplots->SetOLabel("pmtr");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetAxisTitles("Incident Angle (degrees)",
			"Relative PMT Angular Response (arb. units)");
    qplots->SetYRange(1.0, 1.35);
    qplots->SetLegendPos(0.15, 0.90-legwy, 0.15+legwx, 0.90);

    b = (TCanvas**) qplots->PlotAllPmtr((Int_t)qplot_flag[2]);
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen()+1 ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;

    /*
    // PMT Angular Response errors at 500 nm
    qplots->SetILabel("pmtAngResp");
    a = (TCanvas*) qplots->PlotPmtrError("500","Statistical");
    a->Write();
    f->cd();
    delete a;
    a = (TCanvas*) qplots->PlotPmtrError("500","Systematic");
    a->Write();
    f->cd();
    delete a;
    */
  }

  // --------------------------------------
  // Laserball distribution plots
  if(qplot_flag[3]){
    qplots->SetILabel("lbdist");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetAxisTitles("Azimuthal Angle #phi (radians)",
  			"Relative Laserball Intensity (arb. units)");
    qplots->SetYRange(0.95, 1.1);
    b = (TCanvas**) qplots->PlotAllLBdist1D();
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen() ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;

    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetAxisTitles("Azimuthal Angle #phi (radians)",
			  "Polar Angle cos#theta");
    b = (TCanvas**) qplots->PlotAllLBdist2D();
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen() ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b; 
  }

  // --------------------------------------
  // Laserball mask functions
  if(qplot_flag[4]){
    qplots->SetILabel("lbpoly");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetAxisTitles("Polar Angle cos#theta",
			  "Relative Laserball Intensity (arb. units)");
    a = (TCanvas*) qplots->PlotAllLBmask();
    a->Write();
    f->cd();
    delete a;
  }

  // --------------------------------------
  // Pmt relative efficiencies
  if(qplot_flag[5]){
    qplots->SetILabel("eff");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    b = (TCanvas**) qplots->PlotAllPmtEfficiencies();
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen() ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;
  }

  // --------------------------------------
  // Time Dependence (drift) plots
  // do only if there are more than one scan in the cardfile
  if(qplot_flag[7] && (qplots->GetNscan() > 1)){
    qplots->SetILabel("attenuation");
    qplots->SetOLabel("attdrift");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    qplots->SetAxisTitles("SNO Julian Date (days)",
			  "Inverse Attenuation Length (cm^{-1})");
    qplots->SetCorrFactor(factor);
    qplots->SetCorr(rcorr[0], rcorr[1], rcorr[2],
  		    rcorr[3], rcorr[4], rcorr[5]);
    qplots->SetFitFunction(fitfunc);
    qplots->SetLegendPos(0.75, 0.75, 1.0, 1.0);
    b = (TCanvas**) qplots->PlotAttenuationDrift("D2O");
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen()+1 ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;

    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    b = (TCanvas**) qplots->PlotAttenuationDrift("H2O");
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen()+1 ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;
  }

  // --------------------------------------
  // Media attenuation coefficients
  // as a function of prompt time window size
  if(qplot_flag[8]){
    qplots->SetILabel("timewindows");
    qplots->SetOLabel("tw");
    qplots->SetLegendPos(0.55, 0.85-legwy, 0.55+legwx, 0.85);

    // first for D2O
    qplots->SetMedium("D2O");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    b = (TCanvas**) qplots->PlotAllTimeWindows();
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen() ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;
    
    // then for H2O
    qplots->SetMedium("H2O");
    qplots->SetPlotTitle(); qplots->SetCanvasTitle();
    b = (TCanvas**) qplots->PlotAllTimeWindows();
    for(Int_t iwlen = 0 ; iwlen < qplots->GetNwlen() ; iwlen++){
      (b[iwlen])->Write();
      f->cd();
      delete b[iwlen];
    }
    delete b;
  }



  f->cd();
  f->Close();

  cout << "qocaplots_runner.cxx : Done plotting.\n";
  return 0;

}
