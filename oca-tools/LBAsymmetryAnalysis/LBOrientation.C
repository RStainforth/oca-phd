////////////////////////////////////////////////////////////////////
///
/// FILENAME: LBOrientation.C
///
/// BRIEF: This class obtains the parameters that quantify
///        the laserball light distribution. It reads the run 
///        IDs from a text file. The final results are plotted  
///	   and saved as ".pdf" into the directory where this file is.
///        The parameters are saved into an ".ocadb" file.
///                  
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     10/2016 : A.S.Inacio - First Version.
///
/// DETAIL: To execute this file, first it is necessary to compile.
///         At ROOT command line:
///
///           .L LBOrientation.C+
///
///         Then use one of the functions declared to run the analysis.
///
////////////////////////////////////////////////////////////////////

#include "LBOrientation.h"

using namespace std;
using namespace RAT;
using namespace RAT::DU;

ClassImp(LBOrientation);

LBOrientation::LBOrientation( const Int_t lambda, const std::string& scan, const std::string& path ){

  Initialize();
  SetLambda( lambda );
  SetScan( scan );
  SetPath( path );
  if( !lambdaValidity || !scanValidity || !pathValidity ){ return; }
  ReadData();
  if( !orientationValidity ){ return; }
  Ratios();
  PlotResults();
  WriteToFile();

}

//______________________________________________________________________________________
//

LBOrientation::~LBOrientation(){

}

//______________________________________________________________________________________
//

void LBOrientation::Initialize(){
  
  // Default parameters: wavelength of 505 nm, and path to the oct15 SOC files directory 
  fLambda = 0;
  fPath = "";
  fScan = "";
  fPhase = "";

  lambdaValidity = true;
  scanValidity = true;
  pathValidity = true;
  orientationValidity = true;

  for( Int_t i = 0; i < NRUNS; i++ ){
    fNPMTs[i]        = 0;
    fRun[i]          = 0;
    fSourceWL[i]     = 0.0;
    fSourcePos[i]    = TVector3(0.0,0.0,0.0);
    fSourceDirVec[i] = TVector3(0.0,0.0,0.0);
    fOrientation[i]  = 0;

    for( Int_t j = 0; j < 9500; j++ ){
      fPMTPos[i][j]  = TVector3(0.0,0.0,0.0);
      fPMTOcc[i][j]  = 0.0;
      fPMTOccErr[i][j] = 0.0;
    }
  }

  Double_t fCosthetastep = 2./NTHETA;
  for( Int_t sth = 0; sth < NTHETA; sth++ ){
    fCosthetamin[sth] = -1. + sth*fCosthetastep;
    fCosthetamax[sth] = -1. + (sth + 1)*fCosthetastep;

    // Amplitude and phase for the N/S ratio
    fAmplitudeFIT13[sth] = 0.0;
    fPhaseFIT13[sth] = 0.0;
    fAmplitudeFIT13error[sth] = 0.0;
    fPhaseFIT13error[sth] = 0.0;

    // Amplitude and phase for the W/E ratio
    fAmplitudeFIT20[sth] = 0.0;
    fPhaseFIT20[sth] = 0.0;
    fAmplitudeFIT20error[sth] = 0.0;
    fPhaseFIT20error[sth] = 0.0;

    fAmplitudeFIT[sth] = 0.0;
    fPhaseFIT[sth] = 0.0;
    fAmplitudeFITerror[sth] = 0.0;
    fPhaseFITerror[sth] = 0.0;

    fRTheta[sth] = sth;

    cout << "costhetamin " << fCosthetamin[sth] << " costhetamax " << fCosthetamax[sth] << " step " << sth << endl;
  }    
  cout << "Number of costheta slices " << NTHETA << endl;

  Double_t phistep = 2*TMath::Pi()/NPHI;
  for( Int_t sphi = 0; sphi < NPHI; sphi++ ){
    fPhimin[sphi] = -TMath::Pi() + sphi*phistep;
    fPhimax[sphi] = -TMath::Pi() + (sphi+1)*phistep;
    fRPhi[sphi]   = -TMath::Pi() + (sphi+0.5)*phistep;

    cout << "phimin " << fPhimin[sphi] << " phimax " << fPhimax[sphi] << " step " << sphi << endl;
  }
  cout << "Number of phi slices " << NPHI << endl;

  for( Int_t m = 0; m < NTHETA; m++ ){
    for( Int_t n = 0; n < NPHI; n++ ){
			
      fRatio13[n] = 0.0;
      fRatio20[n] = 0.0;
      fRatio20_90[n] = 0.0;

      fEratio13[n] = 0.0;
      fEratio20[n] = 0.0;
      fEratio20_90[n] = 0.0;
    }
  }
}

//______________________________________________________________________________________
//

void LBOrientation::ReadData(){
  /// The SOC files are opened and all the relevant information for each run is saved: 
  /// source wavelength, source position vector, source orientations, PMT position, occupancy.

  Int_t wl,nr,rs[4];
  Char_t ds[5],phase[20],name[20];
  sprintf(name,"runlist.txt");
  ifstream file(name);

  while( file.peek() != EOF ){
    file >> ds >> phase >> wl >> nr >> rs[0] >> rs[1] >> rs[2] >> rs[3];
    if( fLambda == wl && fScan == ds) break;
  }

  fPhase = phase;
  for( Int_t i = 0; i < NRUNS; i++ ) fRun[i] = rs[i];
  file.close();

  // Opening the SOC files
  RAT::DU::Utility::Get()->LoadDBAndBeginRun();
  for( Int_t i = 0; i < NRUNS; i++ ){

    fNPMTs[i] = 0;

    std::string fSocFilename;
    if( fScan == "oct15" ){
      fSocFilename = fPath + fScan + "/" + fPhase + "/" + ::to_string(fRun[i]) + "_Run.root";
    }
    else{
      fSocFilename = fPath + fScan + "/" + fPhase + "/SOC_0000" + ::to_string(fRun[i]) + ".root";
    }
    cout << "Opening file " << fSocFilename << endl;
    RAT::DU::SOCReader *socreader = new RAT::DU::SOCReader(fSocFilename);

    const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();

    for( size_t isoc = 0; isoc < socreader->GetSOCCount(); isoc++ ){

      const RAT::DS::SOC& rsoc = socreader->GetSOC(isoc);

      // Laserball wavelength
      fSourceWL[i] = rsoc.GetCalib().GetMode();

      // Laserball coordinates
      fSourcePos[i] = rsoc.GetCalib().GetPos();

      // Laserball Orientation
      fOrientation[i] = rsoc.GetCalib().GetID();
      fSourceDirVec[i] = rsoc.GetCalib().GetDir();

      if( fOrientation[i] < 0 || fOrientation[i] > 3 ){
        cout << "Run " << fRun[i] << " has orientation ID " << fOrientation[i] << ", which is not valid!" << endl;
        orientationValidity = false;
        return;
      }
      else{
        // Verify that the direction vector agrees with the orientation ID
        if( fOrientation[i] == 0 ){ // East
          if( (int)fSourceDirVec[i].X() != 1 && (int)fSourceDirVec[i].Y() != 0 && (int)fSourceDirVec[i].Z() != 0 ){
            cout << "The direction vector for the East facing run " << fRun[i] << " is (" << fSourceDirVec[i].X() << ", " << fSourceDirVec[i].Y() << ", " << fSourceDirVec[i].Z() << "), which is not valid!" << endl;
            orientationValidity = false;
            return;
          }
        }
        if( fOrientation[i] == 1 ){ // North
          if( (int)fSourceDirVec[i].X() != 0 && (int)fSourceDirVec[i].Y() != 1 && (int)fSourceDirVec[i].Z() != 0 ){
            cout << "The direction vector for the North facing run " << fRun[i] << " is (" << fSourceDirVec[i].X() << ", " << fSourceDirVec[i].Y() << ", " << fSourceDirVec[i].Z() << "), which is not valid!" << endl;
            orientationValidity = false;
            return;
          }
        }
        if( fOrientation[i] == 2 ){ // West
          if( (int)fSourceDirVec[i].X() != -1 && (int)fSourceDirVec[i].Y() != 0 && (int)fSourceDirVec[i].Z() != 0 ){
            cout << "The direction vector for the West facing run " << fRun[i] << " is (" << fSourceDirVec[i].X() << ", " << fSourceDirVec[i].Y() << ", " << fSourceDirVec[i].Z() << "), which is not valid!" << endl;
            orientationValidity = false;
            return;
          }
        }
        if( fOrientation[i] == 3 ){ // South
          if( (int)fSourceDirVec[i].X() != 0 && (int)fSourceDirVec[i].Y() != -1 && (int)fSourceDirVec[i].Z() != 0 ){
            cout << "The direction vector for the South facing run " << fRun[i] << " is (" << fSourceDirVec[i].X() << ", " << fSourceDirVec[i].Y() << ", " << fSourceDirVec[i].Z() << "), which is not valid!" << endl;
            orientationValidity = false;
            return;
          }
        }
      }

      Double_t sumRunOcc = 0.0;
      Double_t sumOccErr = 0.0;

      // Loop over SOCPMTs
      vector<UInt_t> pmtids = rsoc.GetSOCPMTIDs();
			
      for( size_t ipmt = 0; ipmt < pmtids.size(); ipmt++ ){
        // PMT channel
        Int_t lcn = pmtids[ipmt];    // Logical channel number -> PMT ID number
        const RAT::DS::SOCPMT& pmt = rsoc.GetSOCPMT(lcn);

        if( pmtInfo.GetType(lcn) == RAT::DU::PMTInfo::NORMAL || pmtInfo.GetType(lcn) == RAT::DU::PMTInfo::HQE ){

          if( pmt.GetPeakFindOK() == 0 ){

              fPMTPos[i][fNPMTs[i]] = pmtInfo.GetPosition(lcn);

              fPMTOcc[i][fNPMTs[i]] = pmt.GetPromptOccupancy();

              sumRunOcc = sumRunOcc + pmt.GetPromptOccupancy();
              sumOccErr = sumOccErr + TMath::Sqrt(pmt.GetPromptOccupancy());

              fNPMTs[i]++;

          }
	}
      }

      // Normalize the Occupancy of each PMT in a run by the run Occupancy averaged by the number of normal and online PMTs for that run
      Double_t meanRunOcc = sumRunOcc / fNPMTs[i];
      Double_t meanOccErr = sumOccErr / fNPMTs[i];

      for( Int_t o = 0; o < fNPMTs[i]; o++ ){
        fPMTOcc[i][o] = fPMTOcc[i][o]/meanRunOcc;
        fPMTOccErr[i][o] = TMath::Sqrt( (fPMTOcc[i][o]/pow(meanRunOcc,2)) + (pow(fPMTOcc[i][o],2)*pow(meanOccErr,2)/pow(meanRunOcc,4)) );
      }

    }        
    delete socreader;
  }
}

//______________________________________________________________________________________
//

void LBOrientation::Ratios(){
 
  /// Calculates the ratio of occupancies

  // SNO+ convention
  // Orientation 0 - East
  // Orientation 1 - North
  // Orientation 2 - West
  // Orientation 3 - South

  // For each orientation, for each theta slice and for each phi bin, sum the occupancies 
  // and uncertainties of the PMTs that fall in that bin, and count the number of PMTs
  Double_t sumOcc[NORIENTATIONS][NTHETA][NPHI];
  Double_t sumOcc2[NORIENTATIONS][NTHETA][NPHI];
  Double_t nPMTs[NORIENTATIONS][NTHETA][NPHI];

  for( Int_t l = 0; l < NORIENTATIONS; l++ ){
    for( Int_t m = 0; m < NTHETA; m++ ){
      for( Int_t n = 0; n < NPHI; n++ ){

        sumOcc[l][m][n] = 0.0;
        sumOcc2[l][m][n] = 0.0;
        nPMTs[l][m][n] = 0.0;

      }
    }
  }

  for(Int_t i = 0; i < NRUNS; i++){
    Int_t iorientation = fOrientation[i];
    for(Int_t j = 0; j < fNPMTs[i]; j++){
      Float_t costheta = fPMTPos[i][j].CosTheta();
      Float_t phi      = fPMTPos[i][j].Phi();

      for(Int_t itheta = 0; itheta < NTHETA; itheta++){
        for(Int_t iphi = 0; iphi < NPHI; iphi++){

          if(costheta >= fCosthetamin[itheta] && costheta < fCosthetamax[itheta]){
            if(phi >= fPhimin[iphi] && phi < fPhimax[iphi]){

              sumOcc[iorientation][itheta][iphi]  = sumOcc[iorientation][itheta][iphi] + fPMTOcc[i][j];
              nPMTs[iorientation][itheta][iphi]   = nPMTs[iorientation][itheta][iphi] + 1;
              sumOcc2[iorientation][itheta][iphi] = sumOcc2[iorientation][itheta][iphi] + pow(fPMTOcc[i][j],2);

            }
          }
        }
      }
    }
  }

  // Calculate the occupancy ratios for each phi bin of each theta slice
  Double_t h[NPHI];

  for(Int_t ptheta = 0; ptheta < NTHETA; ptheta++){
    for(Int_t pphi = 0; pphi < NPHI; pphi++){

      if( sumOcc[3][ptheta][pphi] != 0 ){

        fRatio13[pphi] = (sumOcc[1][ptheta][pphi]/nPMTs[1][ptheta][pphi]) / (sumOcc[3][ptheta][pphi]/nPMTs[3][ptheta][pphi]);

        // The uncertainty for each phi bin comes from the variance of the distribution of data points in that bin.
        // The statistical uncertainty is ignored here because it is less than 10% of the variance, thus not having
        // a big contribution to the ratio uncertainty.
        Float_t ef1 = (sumOcc2[1][ptheta][pphi]/nPMTs[1][ptheta][pphi] - pow(sumOcc[1][ptheta][pphi]/nPMTs[1][ptheta][pphi],2));
        Float_t ef3 = (sumOcc2[3][ptheta][pphi]/nPMTs[3][ptheta][pphi] - pow(sumOcc[3][ptheta][pphi]/nPMTs[3][ptheta][pphi],2));

        fEratio13[pphi] = TMath::Sqrt(ef1 + pow(sumOcc[1][ptheta][pphi]/sumOcc[3][ptheta][pphi],2)*ef3)/sumOcc[3][ptheta][pphi];

      }
      else{
        fRatio13[pphi] = 0.;
        fEratio13[pphi] = 0.;
      }

      if( sumOcc[0][ptheta][pphi] != 0 ){

        fRatio20[pphi] = (sumOcc[2][ptheta][pphi]/nPMTs[2][ptheta][pphi]) / (sumOcc[0][ptheta][pphi]/nPMTs[0][ptheta][pphi]);

        // The uncertainty for each phi bin comes from the variance of the distribution of data points in that bin.
        // The statistical uncertainty is ignored here because it is less than 10% of the variance, thus not having
        // a big contribution to the ratio uncertainty. 
        Float_t ef2 = (sumOcc2[2][ptheta][pphi]/nPMTs[2][ptheta][pphi] - pow(sumOcc[2][ptheta][pphi]/nPMTs[2][ptheta][pphi],2));
        Float_t ef0 = (sumOcc2[0][ptheta][pphi]/nPMTs[0][ptheta][pphi] - pow(sumOcc[0][ptheta][pphi]/nPMTs[0][ptheta][pphi],2));

        fEratio20[pphi] = TMath::Sqrt(ef2 + pow(sumOcc[2][ptheta][pphi]/sumOcc[0][ptheta][pphi],2)*ef0)/sumOcc[0][ptheta][pphi];

      }
      else{
        fRatio20[pphi] = 0.;
        fEratio20[pphi] = 0.;
      }
    }

    for(Int_t pphi = 0; pphi < NPHI; pphi++){
      if(pphi <= 27){
        fRatio20_90[pphi] = fRatio20[pphi+9];
        fEratio20_90[pphi] = fEratio20[pphi+9];
      }
      if(pphi > 27){
        fRatio20_90[pphi] = fRatio20[pphi-27];
        fEratio20_90[pphi] = fEratio20[pphi-27];
      }
    }

    GR13[ptheta] = new TGraphErrors(NPHI,fRPhi,fRatio13,0,fEratio13);
    GR20[ptheta] = new TGraphErrors(NPHI,fRPhi,fRatio20,0,fEratio20);
    GR20_90[ptheta] = new TGraphErrors(NPHI,fRPhi,fRatio20_90,0,fEratio20_90);

  }
}

//______________________________________________________________________________________
//

void LBOrientation::PlotResults(){
 
  /// Plots the results and fits them to extract the parameters.

  gROOT->SetStyle("Plain");
  gROOT->Reset();
  gStyle->SetOptFit();
  gStyle->SetOptStat(111111);

  TCanvas* c0;
  c0 = new TCanvas("c0","c0",200,100,700,500);

  TLegend *leg[NTHETA];

  std::string savename;
  Char_t hname[180];

  TMultiGraph *R;
  TMultiGraph *F;
  TMultiGraph *C[2];

  for(Int_t q = 0; q < 2; q++){
    Char_t hgraph[4];
    Char_t htgr[50];
    sprintf(hgraph,"C[%d]",q);
    sprintf(htgr,"Fit Results, %d nm",(int)fLambda);

    C[q] = new TMultiGraph(hgraph,htgr);
  }

  TGraphErrors *Amp13,*Amp20,*AmpFit;
  TGraphErrors *Phase13,*Phase20,*PhaseFit;

  Char_t hgraph[4];
  Char_t htgr[50];

  for(Int_t s = 0; s < NTHETA; s++){

    sprintf(htgr,"Occupancy ratio versus #phi %.3f < cos(#theta) < %.3f",fCosthetamin[s],fCosthetamax[s]);
    R = new TMultiGraph("R",htgr);
    sprintf(htgr,"Fit #phi %.3f < cos(#theta) < %.3f",fCosthetamin[s],fCosthetamax[s]);
    F = new TMultiGraph("F",htgr);

    GR13[s]	->SetMarkerColor(46);
    GR13[s]	->SetLineColor(46);
    GR13[s]	->SetLineStyle(1);
    GR13[s]	->SetMarkerStyle(21);
    R->Add(GR13[s],"p");
    GR20[s]	->SetMarkerColor(38);
    GR20[s]	->SetLineColor(38);
    GR20[s]	->SetLineStyle(1);
    GR20[s]	->SetMarkerStyle(21);
    R->Add(GR20[s],"p");
    R->Draw("A");

    R	->GetXaxis()->SetTitle("#phi (rad)");
    R	->GetYaxis()->SetTitleOffset(1.3);
    R	->GetYaxis()->SetTitle("Occ. ratio");

    leg[s] = new TLegend(0.6,0.65,0.75,0.9);
    leg[s] ->AddEntry(GR13[s],"N/S","p");
    leg[s] ->AddEntry(GR20[s],"W/E","p");

    leg[s] ->Draw();
    c0     ->Update();
    savename = "Ratios_" + fScan + "_" + ::to_string(fLambda) + "_" + ::to_string(s) + ".png";
    c0->SaveAs(savename.c_str(),"png");

    TF1 *fit1 = new TF1("fit1","(2/(1+[0]*cos(x+[1])))-1",-TMath::Pi(),TMath::Pi());
    fit1->SetParLimits(0, 0.0, 10.);
    fit1->SetParLimits(1, -TMath::Pi(), TMath::Pi());
    sprintf(hname,"N/S fitted, %.3f < cos(#theta) < %.3f",fCosthetamin[s],fCosthetamax[s]);

    // Create and save the plots for each of the slices and fit them
    c0->Clear();
    GR13[s]->SetTitle(hname);
    GR13[s]->GetXaxis()->SetTitle("#phi (rad)");
    GR13[s]->GetYaxis()->SetTitleOffset(1.3);
    GR13[s]->GetYaxis()->SetTitle("Occ. ratio");
    GR13[s]->Draw("");
    GR13[s]->Fit(fit1);
    c0     ->Update();
    savename = "RatioNS_" + fScan + "_" + ::to_string(fLambda) + "_" + ::to_string(s) + ".png";
    c0->SaveAs(savename.c_str(),"png");

    fAmplitudeFIT13[s] = fit1->GetParameter(0);
    fPhaseFIT13[s] = fit1->GetParameter(1);
    fAmplitudeFIT13error[s] = fit1->GetParError(0);
    fPhaseFIT13error[s] = fit1->GetParError(1);

    TF1 *fit2 = new TF1("fit2","(2/(1+[0]*sin(x+[1])))-1",-TMath::Pi(),TMath::Pi());
    sprintf(hname,"W/E fitted, %.3f < cos(#theta) < %.3f",fCosthetamin[s],fCosthetamax[s]);
    fit2->SetParLimits(0, 0.0, 10.);
    fit2->SetParLimits(1, -TMath::Pi(), TMath::Pi());
    c0->Clear();
    GR20[s]->SetTitle(hname);
    GR20[s]->GetXaxis()->SetTitle("#phi (rad)");
    GR20[s]->GetYaxis()->SetTitleOffset(1.3);
    GR20[s]->GetYaxis()->SetTitle("Occ. ratio");
    GR20[s]->Draw("");
    GR20[s]->Fit(fit2);
    c0     ->Update();
    savename = "RatioWE_" + fScan + "_" + ::to_string(fLambda) + "_" + ::to_string(s) + ".png";
    c0->SaveAs(savename.c_str(),"png");

    fAmplitudeFIT20[s] = fit2->GetParameter(0);
    fPhaseFIT20[s] = fit2->GetParameter(1);
    fAmplitudeFIT20error[s] = fit2->GetParError(0);
    fPhaseFIT20error[s] = fit2->GetParError(1);

    c0->Clear();

    sprintf(htgr,"Fit %.3f < cos(#theta) < %.3f",fCosthetamin[s],fCosthetamax[s]);
    F = new TMultiGraph("F",htgr);

    GR13[s]	->SetMarkerColor(46);
    GR13[s]	->SetLineColor(46);
    GR13[s]	->SetLineStyle(1);
    GR13[s]	->SetMarkerStyle(21);
    F->Add(GR13[s],"p");

    GR20_90[s]->SetMarkerColor(38);
    GR20_90[s]->SetLineColor(38);
    GR20_90[s]->SetLineStyle(1);
    GR20_90[s]->SetMarkerStyle(21);
    F->Add(GR20_90[s],"p");
    F->Draw("A");
    F->GetXaxis()->SetTitle("#phi (rad)");
    F->GetYaxis()->SetTitleOffset(1.3);
    F->GetYaxis()->SetTitle("Occ. ratio");
    F->GetHistogram()->SetMinimum(0.9);
    F->GetHistogram()->SetMaximum(1.1);

    leg[s] = new TLegend(0.79,0.1,0.89,0.29);
    leg[s] ->AddEntry(GR13[s],"N/S","p");
    leg[s] ->AddEntry(GR20_90[s],"W/E - 90","p");
    leg[s] ->Draw();
    c0->Update();
    savename = "Fits_" + fScan + "_" + ::to_string(fLambda) + "_" + ::to_string(s) + ".png";
    c0->SaveAs(savename.c_str(),"png");

    F->Fit(fit1);

    fAmplitudeFIT[s] = fit1->GetParameter(0);
    fPhaseFIT[s] = fit1->GetParameter(1);
    fAmplitudeFITerror[s] = fit1->GetParError(0);
    fPhaseFITerror[s] = fit1->GetParError(1);

    delete fit1;
    delete fit2;
  }

  Amp13 = new TGraphErrors(NTHETA,fRTheta,fAmplitudeFIT13,0,fAmplitudeFIT13error);
  Amp20 = new TGraphErrors(NTHETA,fRTheta,fAmplitudeFIT20,0,fAmplitudeFIT20error);

  Phase13 = new TGraphErrors(NTHETA,fRTheta,fPhaseFIT13,0,fPhaseFIT13error);
  Phase20 = new TGraphErrors(NTHETA,fRTheta,fPhaseFIT20,0,fPhaseFIT20error);

  c0->Clear();

  Amp13->SetMarkerColor(46);
  Amp13->SetLineColor(46);
  Amp13->SetLineStyle(1);
  Amp13->SetMarkerStyle(21);

  C[0] ->Add(Amp13,"p");

  Amp20->SetMarkerColor(38);
  Amp20->SetLineColor(38);
  Amp20->SetLineStyle(1);
  Amp20->SetMarkerStyle(21);

  C[0] ->Add(Amp20,"p");

  C[0] ->Draw("A");
  C[0] ->GetXaxis()->SetTitle("# slice of cos(#theta)");
  C[0] ->GetYaxis()->SetTitleOffset(1.3);
  C[0] ->GetYaxis()->SetTitle("Amplitude");

  TLegend *legend = new TLegend(0.12,0.7,0.37,0.9);
  legend ->AddEntry(Amp13,"Amplitude N/S","p");
  legend ->AddEntry(Amp20,"Amplitude W/E","p");
  legend ->Draw();
  c0     ->Update();
  savename = "Amplitudes_" + fScan + "_" + ::to_string(fLambda) + ".png";
  c0->SaveAs(savename.c_str(),"png");

  c0->Clear();

  Phase13->SetMarkerColor(46);
  Phase13->SetLineColor(46);
  Phase13->SetLineStyle(1);
  Phase13->SetMarkerStyle(21);
  C[1] ->Add(Phase13,"p");

  Phase20->SetMarkerColor(38);
  Phase20->SetLineColor(38);
  Phase20->SetLineStyle(1);
  Phase20->SetMarkerStyle(21);
  C[1] ->Add(Phase20,"p");

  C[1] ->Draw("A");
  C[1] ->GetXaxis()->SetTitle("# slice of cos(#theta)");
  C[1] ->GetYaxis()->SetTitleOffset(1.3);
  C[1] ->GetYaxis()->SetTitle("Phase (rad)");
  legend = new TLegend(0.15,0.7,0.39,0.89);
  legend ->AddEntry(Phase13,"Phase N/S","p");
  legend ->AddEntry(Phase20,"Phase W/E","p");
  legend ->Draw();
  c0     ->Update();
  savename = "Phases_" + fScan + "_" + ::to_string(fLambda) + ".png";
  c0->SaveAs(savename.c_str(),"png");

}

//______________________________________________________________________________________
//

void LBOrientation::WriteToFile(){
 
  /// Saves the parameters to an ".ocadb" file.

  string outputFile = getenv( "OCA_SNOPLUS_DATA" ) + (string)"/lbdistparameters/lb_" + ::to_string(fScan) + "_" + ::to_string(fPhase) + "_" + ::to_string(fLambda) + (string)".ocadb";

  ofstream output;
  output.precision( 6 );
  output.open ( outputFile.c_str() );

  output << "{\n";
  output << "type : \"LBFITRESULTS\",\n";
  output << "index : \"" << "LBparameter_setup" << "\",\n";
  output << "version: 1," << endl;
  output << "run_range: [0, 0]," << endl;
  output << "pass: 0," << endl;
  output << "comment: \"\"," << endl;
  output << "timestamp: \"\"," << endl;

  output << "\n" ;

  output << "parameters_list: [ \"laserball_distribution\" ],\n";

  output << "\n" ;

  output << "// The laserball distribution sin-wave parameters.\n";
  output << "laserball_distribution_number_of_cos_theta_slices : " << NTHETA << ",\n";
  output << "laserball_distribution_number_of_parameters_per_cos_theta_slice : " << "2" << ",\n";
  output << "laserball_distribution_sin_wave : [ ";

  for ( Int_t i = 0; i < NTHETA; i++ ){
    if( i < NTHETA-1 ){
      output << fAmplitudeFIT[i] << ",\n";
      output << fPhaseFIT[i] << ",\n"; 
      output << "\n";
    }
    if( i == NTHETA-1 ){
      output << fAmplitudeFIT[i] << ",\n";
      output << fPhaseFIT[i] << "],\n";
      output << "\n";
    }
  }

  output << "laserball_distribution_errors : [ ";
  for ( Int_t i = 0; i < NTHETA; i++ ){
    if( i < NTHETA-1 ){
      output << fAmplitudeFITerror[i] << ",\n";
      output << fPhaseFITerror[i] << ",\n"; 
      output << "\n";
    }
    if( i == NTHETA-1 ){
      output << fAmplitudeFITerror[i] << ",\n";
      output << fPhaseFITerror[i] << "],\n"; 
      output << "\n";
    }
  }

  output << "\n}";
  output.close();

  cout << "LBOrientation: Fit parameters saved to OCADB file:\n";
  cout << outputFile << "\n"; 
}

//______________________________________________________________________________________
//

void LBOrientation::SetLambda( const Int_t aNumber ){

  if(aNumber != 337 && aNumber != 365 && aNumber != 369 && aNumber != 385 &&  aNumber != 420 && aNumber != 450 && aNumber != 500 && aNumber != 505){
    printf("The wavelength %d nm does not belong to the list of wavelengths emitted by the laserball!\n", aNumber);
    lambdaValidity = false;
    return;
  }
  fLambda = aNumber;
}

//______________________________________________________________________________________
//

void LBOrientation::SetScan( const std::string& aString ){

  if ( aString != "oct15" && aString != "dec17" ){
    cout << aString << " is not a valid laserball scan!" << endl;
    scanValidity = false;
    return;
  }
  fScan = aString;
}

//______________________________________________________________________________________
//

void LBOrientation::SetPath( const std::string& aString ){

  struct stat s;
  if ( stat( aString.c_str(), &s ) != 0 ){
    cout << "The path inserted does not exist!" << endl;
    pathValidity = false;
    return;
  }
  fPath = aString;
}
