////////////////////////////////////////////////////////////////////
///
/// FILENAME: DiagScan.cxx
///
/// BRIEF: This class obtains the attenuation coefficient of the
///        medium inside the AV using the laserball in diagonal
///        positions. It reads the run IDs from a text file.
///        The final results are plotted and saved as ".pdf" into 
///				 the directory where this file is.
///                
/// AUTHOR: Ana Sofia Inacio <ainacio@lip.pt>
///
/// REVISION HISTORY:
///     07/2016 : A.S.Inacio - First Version.
///
/// DETAIL: To execute this file, first it is necessary to compile.
///         At ROOT command line:
///
///           .L DiagScan.cxx+
///
///         Then:
///
///           DiagScan()
///
///         Or:
///		
///           DiagScan(<wavelength>, "<scan>")
///
////////////////////////////////////////////////////////////////////

#include "DiagScan.h"

using namespace std;
using namespace RAT;
using namespace RAT::DU;

ClassImp(DiagScan);

DiagScan::DiagScan(){

  Initialize();

}
//______________________________________________________________________________________
//
DiagScan::DiagScan(Int_t lambda, /*Float_t tolerance, Float_t shadowing,*/ Char_t *scan){

  Initialize();
  SetLambda(lambda);
  SetScan(scan);
  //    SetTolerance(tolerance);
  //    SetShadowing(shadowing);
  //    SetAngResponseCoefficient(lambda);
  ReadData();
  Process();
  FitRatio();
  Product();

}
//______________________________________________________________________________________
//
DiagScan::~DiagScan(){

  if(fScan){
    delete fScan;
    fScan = NULL;		
  }

  printf("DiagScan Object deleted \n");

}
//______________________________________________________________________________________
//
void DiagScan::Initialize(){
 
  cout << "Initializing all variables!" << endl;

  fScan               = new TString("xpz");
  fLambda             = 420;
  fTolerance          = 1500.0;
  fShadowing          = 150.0;
  fNRuns              = 0;
  fAngRespCoefficient = 0.0;
  fMaskDistribution   = 0.0;

  for( Int_t i = 0; i < NRUNS; i++ ){
    fNPairs[i]        = 0;
    fNPulses[i]       = 0;
    fNPMTs[i]         = 0;
    fRunID[i]         = 0;
    fSourceWL[i]      = 0.0;
    fSourcePos[i]     = TVector3(0.0,0.0,0.0);
    fSumRatio[i]      = 0;
    fSumProduct[i]    = 0;
    fSumRerrors[i]    = 0;
    fSumPerrors[i]    = 0;
    fDistance[i]      = 0;

    for( Int_t j = 0; j < NPMS; j++ ){
      fPMTs[i][j][0]  = 0.0;
      fPMTs[i][j][1]  = 0.0;
      fPMTs[i][j][2]  = 0.0;
      fPMTs[i][j][3]  = 0.0;
      fTolPMTs[i][j]  = TVector3(0.0,0.0,0.0);
      fRatio[i][j]    = 0;    
      fProduct[i][j]  = 0;  
      fRerrors[i][j]  = 0; 
      fPerrors[i][j]  = 0;
      fPair1[i][j]    = 0;
      fPair2[i][j]    = 0;
    }

    for( Int_t h = 0; h < 9100; h++ ){
      fPMTNorm[i][h]  = TVector3(0.,0.,0.);
      fPMTPos[i][h]   = TVector3(0.,0.,0.);
      fPMTOcc[i][h]   = 0.;
      fPMTLCN[i][h]   = 0;
    }
  }
}
//______________________________________________________________________________________
//
void DiagScan::ReadData(){

  /// The SOC files are opened and all the relevant information for each run is saved: 
  /// number of pulses emitted by the source, source wavelength, source position, PMT 
  /// position, occupancy (and its error) and the vector normal to the PMT, facing 
  /// inwards the detector

  cout << "Reading Data from the SOC Files!" << endl;

  Int_t wl;
  Int_t nr;
  Int_t rs[6];
  Char_t ds[5];
  Char_t name[30];
  Char_t sc[3];
  sprintf(name,"diagonal_runlist.txt");

  ifstream file(name);

  while( file.peek() != EOF ){
    file >> ds >> wl >> sc >> nr >> rs[0] >> rs[1] >> rs[2] >> rs[3] >> rs[4] >> rs[5];
    if( fLambda == wl && !strcmp(fScan->Data(),sc) ) break;
  } 
  fNRuns = nr;
  for( Int_t i = 0; i < fNRuns; i++ ){ fRunID[i] = rs[i]; }
  file.close();
 
                       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Opening the SocFiles
  RAT::DU::Utility::Get()->LoadDBAndBeginRun();
  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  ChanHWStatus pmtStatus = Utility::Get()->GetChanHWStatus();

  for( Int_t i = 0; i < fNRuns; i++ ){
    Int_t run = fRunID[i];
    char fSocFilename[65];			// SocFile name
    sprintf(fSocFilename,"/lstore/sno/maneira/snoplus/mc/optics/water/%d_Run.root",run);

    RAT::DU::SOCReader *socreader = new RAT::DU::SOCReader(fSocFilename);

    for( size_t isoc = 0; isoc < socreader->GetSOCCount(); isoc++ ){

      const RAT::DS::SOC& rsoc = socreader->GetSOC(isoc);

      // Number of pulses triggered
      fNPulses[i] = rsoc.GetNPulsesTriggered();

      // Source wavelength
      fSourceWL[i] = rsoc.GetCalib().GetMode();

      // Source coordinates
      fSourcePos[i] = rsoc.GetCalib().GetPos();

      // Loop over SOCPMTs
      vector<UInt_t> pmtids = rsoc.GetSOCPMTIDs();
      for( size_t ipmt = 0; ipmt < pmtids.size(); ipmt++ ){

        // PMT channel
        Int_t lcn = pmtids[ipmt];    // Logical channel number -> PMT ID number
        const RAT::DS::SOCPMT& pmt = rsoc.GetSOCPMT(lcn);  
				
        // PMT type: NORMAL = 1, OWL = 2, LOWGAIN = 3, BUTT = 4, NECK = 5, CALIB = 6, SPARE = 10, INVALID = 11, BLOWN75 = 12 
        RAT::DU::PMTInfo::EPMTType type = pmtInfo.GetType(lcn);

        // Get these objects from SOCPeakFind
        Int_t peakok = pmt.GetPeakFindOK();

        if( type == RAT::DU::PMTInfo::NORMAL && peakok == 0 && pmtStatus.IsChannelOnline( lcn ) && pmtStatus.IsTubeOnline( lcn ) ){
          fPMTPos[i][fNPMTs[i]] = pmtInfo.GetPosition(lcn);
          fPMTLCN[i][fNPMTs[i]] = lcn;
          fPMTOcc[i][fNPMTs[i]] = pmt.GetPromptOccupancy();
          fPMTNorm[i][fNPMTs[i]] = -1.*pmtInfo.GetDirection(lcn);
          fNPMTs[i]++;
        }
      }
    }
    delete socreader;
  }
}
//______________________________________________________________________________________
//
void DiagScan::Process(){

/// The PMTs that are within the tolerance distance to the diagonal will be selected and the
/// all the necessary corrections to the occupancy are calculated. Pairs of PMTs (one from
/// each group) are formed and their occupancy ratio and product are calculated (and the 
/// corresponding uncertainities).The weighted mean value of the ratio and product for the run
/// is calculated.
	
cout << "Processing the Data!" << endl;

// Diagonal selection
TVector3 fDiagonalVec; // Vector representing the diagonal
if(!strcmp(fScan->Data(),"xpz"))  fDiagonalVec = TVector3(1,0,1);
if(!strcmp(fScan->Data(),"xnz"))  fDiagonalVec = TVector3(1,0,-1);

LightPathCalculator lightPath = Utility::Get()->GetLightPathCalculator();
ShadowingCalculator shadowCalc = Utility::Get()->GetShadowingCalculator();

for( Int_t i = 0; i < fNRuns; i++ ){

fNOKPMTs   = 0;

// Equivalent energy (MeV) of the wavelength (nm) emmited by the source
Float_t fEnergy = RAT::util::WavelengthToEnergy( fSourceWL[i] * 1.0e-6 );

// Set all of the tolerances for shadowing by the detector geometry to 150 mm equivalent.
shadowCalc.SetAllGeometryTolerances( fShadowing );

Float_t Group1SumOcc = 0;
Float_t Group2SumOcc = 0;

Float_t Group1SumOcc2 = 0;
Float_t Group2SumOcc2 = 0;

Int_t nPMTg1 = 0;
Int_t nPMTg2 = 0;

Float_t  lOccupancy[ fNPMTs[i] ];
Float_t  lCorrOccupancy[ fNPMTs[i] ];
Float_t  lCorrections[ fNPMTs[i] ];
Float_t  lDistance[ fNPMTs[i] ];
TVector3 lPositions[ fNPMTs[i] ];
Int_t    lLCN[ fNPMTs[i] ];

for( Int_t p = 0; p < fNPMTs[i]; p++ ){

// Distance from PMT to diagonal  
Float_t fDiagDist; // Distance from PMT to diagonal (mm)
fDiagDist = TMath::Sqrt(((fPMTPos[i][p].Dot(fPMTPos[i][p]))*(fDiagonalVec.Dot(fDiagonalVec))-(-fPMTPos[i][p].Dot(fDiagonalVec))*(-fPMTPos[i][p].Dot(fDiagonalVec)))/((fDiagonalVec).Dot(fDiagonalVec)));

if( fDiagDist < fTolerance ){
Float_t localityVal = 10; // The accepted tolerance (mm) for how close the light path is calculated to the 'pmtPos'
lightPath.CalcByPosition( fSourcePos[i], fPMTPos[i][p], fEnergy, localityVal );

// Check for shadowing
if(shadowCalc.CheckForShadowing(lightPath) == false){ 
Int_t nVal = 0; // nVal-sided polygon superimposed onto the PMT bucket for the calculation

// Solid Angle calculation
lightPath.CalculateSolidAngle( fPMTNorm[i][p], nVal );
fSolidAngle = lightPath.GetSolidAngle();   
															
// Total Fresnel transmission coefficient
fFresnel = lightPath.GetFresnelTCoeff();
											 
// PMT Angular Response calculation
lightPath.CalculateCosThetaPMT( fPMTLCN[i][p] );
Double_t cos = lightPath.GetCosThetaAvg();
Double_t theta = TMath::ACos(cos)*180/TMath::Pi();

// Laserball Light Distribution calculation
TVector3 lightVector = lightPath.GetInitialLightVec();
Double_t lbTheta = 1.0 + lightVector.CosTheta();
fMaskDistribution = 1; //GetLBMaskCorrection( fSourceWL[i], lbTheta );


fPMTResponse = 1 + fAngRespCoefficient*theta;

Float_t fCorrectedOcc = fPMTOcc[i][p]/(fSolidAngle*fFresnel*fPMTResponse*fMaskDistribution);
																
lOccupancy[fNOKPMTs]      = fPMTOcc[i][p];
lCorrOccupancy[fNOKPMTs]  = fCorrectedOcc;
lCorrections[fNOKPMTs]    = fSolidAngle*fFresnel*fPMTResponse*fMaskDistribution;
lDistance[fNOKPMTs]       = (lightPath.GetStartPos()-lightPath.GetPointOnAV1st()).Mag();
lPositions[fNOKPMTs]      = fPMTPos[i][p];
lLCN[fNOKPMTs]            = fPMTLCN[i][p];

if( fPMTPos[i][p].Z() > 0 ){
Group1SumOcc = Group1SumOcc + fCorrectedOcc/fNPulses[i];
Group1SumOcc2 = Group1SumOcc2 + TMath::Power(fCorrectedOcc,2)/TMath::Power(fNPulses[i],2);
nPMTg1++;
}
if( fPMTPos[i][p].Z() < 0 ){
Group2SumOcc = Group2SumOcc + fCorrectedOcc/fNPulses[i];
Group2SumOcc2 = Group2SumOcc2 + TMath::Power(fCorrectedOcc,2)/TMath::Power(fNPulses[i],2);
nPMTg2++;
}

fNOKPMTs++;

}
lightPath.ResetValues();
}
}

cout << "Finished PMT Loop" << endl;

Float_t Group1Occ = Group1SumOcc/nPMTg1; 
Float_t Group2Occ = Group2SumOcc/nPMTg2;

Float_t Group1Occ2 = Group1SumOcc2 / nPMTg1; 
Float_t Group2Occ2 = Group2SumOcc2 / nPMTg2;

Float_t sigma1 = TMath::Sqrt(Group1Occ2-Group1Occ*Group1Occ);
Float_t sigma2 = TMath::Sqrt(Group2Occ2-Group2Occ*Group2Occ);

Int_t number = fNOKPMTs;
Int_t counter = 0;

/*        ofstream outfile;
char outfilename[180];
sprintf(outfilename,"RemovedPMTs%s%dn%d.txt",fScan->Data(),(int)fLambda,(int)i);
outfile.open(outfilename);
outfile << "Low Occupancy PMTs removed from the analysis! " << endl;
outfile << "Scan " << fScan->Data() << " Wl " << fLambda << " Run " << i << endl;
outfile << "Shadowing Cuts: " << fShadowing << " mm; Distance to diagonal: " << fTolerance << " mm" << endl;*/

cout << "Hello" << endl;

for( Int_t p = 0; p < number; p++ ){
if( lPositions[p].Z() > 0 ){ // Group 1
if( lCorrOccupancy[p]/fNPulses[i] < Group1Occ - 2.*sigma1 ){
fNOKPMTs--;
//                    outfile << "Group 1, PMT ID: " << lLCN[p] << endl;
}
else{
fPMTs[i][counter][0] = lOccupancy[p];
fPMTs[i][counter][1] = lCorrOccupancy[p];
fPMTs[i][counter][2] = lCorrections[p];
fPMTs[i][counter][3] = lDistance[p];

fTolPMTs[i][counter]  = lPositions[p];

counter++;
}
}
if( lPositions[p].Z() < 0 ){ // Group 2
if( lCorrOccupancy[p]/fNPulses[i] < Group2Occ - 2.*sigma2 ){
fNOKPMTs--;
//                    outfile << "Group 2, PMT ID: " << lLCN[p] << endl;
}
else{
fPMTs[i][counter][0] = lOccupancy[p];
fPMTs[i][counter][1] = lCorrOccupancy[p];
fPMTs[i][counter][2] = lCorrections[p];
fPMTs[i][counter][3] = lDistance[p];

fTolPMTs[i][counter]  = lPositions[p];

counter++;
}
}
}

//        outfile.close();

cout << "Before making pairs of PMTs" << endl;

// Selects the pairs of PMTs   
TVector3 sum;
Float_t min;
Int_t j, l, m;

for( Int_t k = 0; k < ( fNOKPMTs - 1 ); k++ ){
// Find out the pair for pmt k
min = 500; 
j   =  -1;
for( l=k+1; l < fNOKPMTs; l++ ){
Int_t count = 0;
if( fNPairs > 0 ) {
for( m = 0; m < fNPairs[i]; m++ ) {
//check for double counting
if( l == fPair2[i][m] || k == fPair2[i][m] || l == fPair1[i][m] || k == fPair1[i][m] ) count = 1;
}
}
sum = fTolPMTs[i][k] + fTolPMTs[i][l];
if( count == 0 && sum.Mag() < (Float_t) min ){ 
min = sum.Mag(); 
j = l;
}
}     
if( j >= 0 ){
if( fTolPMTs[i][k].Z() > 0 ){
fPair1[i][fNPairs[i]] = k;
fPair2[i][fNPairs[i]] = j;
} else{
fPair1[i][fNPairs[i]] = j;
fPair2[i][fNPairs[i]] = k;
}
fNPairs[i]++;
}		
}

cout << "Before calculating the ratios" << endl;

// Calculate Occ1/Occ2 and Occ1*Occ2
Double_t sumR        = 0;
Double_t Rweight     = 0;
Double_t sumP        = 0;
Double_t Pweight     = 0;
Double_t distance    = 0;

for( Int_t k = 0; k < fNPairs[i]; k++ ){	
Int_t l = fPair1[i][k];
Int_t m = fPair2[i][k];
fRatio[i][k]     = fPMTs[i][l][1] / fPMTs[i][m][1];
fRerrors[i][k]   = TMath::Sqrt(fPMTs[i][l][1] / (TMath::Power(fPMTs[i][m][1],2)*fPMTs[i][l][2]) + TMath::Power(fPMTs[i][l][1],2) / (TMath::Power(fPMTs[i][m][1],3)*fPMTs[i][m][2]));
fProduct[i][k]   = (fPMTs[i][l][1] * fPMTs[i][m][1]) / TMath::Power(fNPulses[i],2);
fPerrors[i][k]   = TMath::Sqrt((TMath::Power(fPMTs[i][l][1],2) * fPMTs[i][m][1]/(fPMTs[i][m][2])) + (fPMTs[i][l][1] * TMath::Power(fPMTs[i][m][1],2) / (fPMTs[i][l][2]))) / fNPulses[i];
distance         = fPMTs[i][l][3] - fPMTs[i][m][3];
sumR             = sumR + fRatio[i][k]/TMath::Power(fRerrors[i][k],2);
Rweight          = Rweight + 1/TMath::Power(fRerrors[i][k],2);     
sumP             = sumP + fProduct[i][k]/TMath::Power(fPerrors[i][k],2);
Pweight          = Pweight + 1/TMath::Power(fPerrors[i][k],2);

fDistance[i] = fDistance[i] + distance;
}        

fSumRatio[i]   = sumR/Rweight;               // Weighted arithmetic mean value of the occupancy ratio for run i
fSumRerrors[i] = 1/TMath::Sqrt(Rweight);     // Standard deviation for the Weighted arithmetic mean value 
fSumProduct[i] = sumP/Pweight;             // Weighted arithmetic mean value of the occupancy product for run i
fSumPerrors[i] = 1/(TMath::Sqrt(Pweight));   // Standard deviation for the Weighted arithmetic mean value  

fDistance[i] = fDistance[i]/fNPairs[i];
}

cout << "Finished Run Loop" << endl;
}
//______________________________________________________________________________________
//
void DiagScan::FitRatio(){

	/// The weighted occupancy ratio of all runs is fitted and plotted with their respective uncertainities.
	
	cout << "Plotting the Ratios and Fitting them!"<< endl;

	Char_t name1[128], savename1[128];
	TGraphErrors* gr;

	gROOT->SetStyle("Plain");
	gROOT->Reset();
	gStyle->SetOptFit();
	gStyle->SetOptStat(111111);

	TCanvas* c0;
	c0 = new TCanvas("c0","c0",200,10,700,500);
	c0->SetGrid();
	
	cout << "after declaring tcanvas" << endl;

	gr = new TGraphErrors(fNRuns,fDistance,fSumRatio,0,fSumRerrors);
	gr->SetMarkerColor(1);
	gr->SetMarkerSize(1.5);
	gr->SetMarkerStyle(21);

	
	sprintf(name1,"Ratio, Diagonal %s, wavelength %d nm",fScan->Data(),fLambda); 
	gr->SetTitle(name1);
	gr->GetXaxis()->SetTitle("Distance (mm)");
	gr->GetYaxis()->SetTitle("Occupancy Ratio");
	gr->Draw("AP");
	
	TF1 *fit = new TF1("fit","exp([0]+[1]*x)");
	
	gr->Fit("fit");
//	gr->Fit("expo");
//	cout << "after fit" << endl;
	//gr->GetFunction("expo");
	// value of the slope
	fAttCoef = fit->GetParameter(1);
	// error of the slope
	fAttCoefEr = fit->GetParError(1);
	
//	cout << "getting coefficients" << endl;

	c0->Update();
	sprintf(savename1,"FittedRatio%s%dTol%dSh%d.pdf",fScan->Data(),fLambda,(int)fTolerance,(int)fShadowing);
	c0->SaveAs(savename1,"pdf");
	
//	cout << "saved plot" << endl;

	//delete c0;
//	delete gr;
//	delete fit;
	
  cout << "#######################" << endl;
  cout << "Inner AV Attenuation Coeffiecient = " 
       << - fAttCoef 
       << " +/- " << fAttCoefEr << " (stat.)" 
       << " mm-1" << endl;
  cout << "#######################" << endl;

}
//______________________________________________________________________________________
//
void DiagScan::Product(){

	/// The weighted occupancy product of all runs is plotted with their respective uncertainities.

	Char_t name2[128], savename2[128];
	TGraphErrors* prod;

	gROOT->SetStyle("Plain");
	gROOT->Reset();
	gStyle->SetOptFit();
	gStyle->SetOptStat(111111);
	gStyle->SetTitleW(0.7);
	gStyle->SetTitleH(0.05);

	TCanvas* c1;
	c1 = new TCanvas("c0","c0",200,10,700,500);
	c1->SetGrid();

	prod = new TGraphErrors(fNRuns,fDistance,fSumProduct,0,fSumPerrors);
	prod->SetMarkerColor(1);
	prod->SetMarkerSize(0.5);
	prod->SetMarkerStyle(21);

	prod->Draw("AP");
	sprintf(name2,"Product, Diagonal %s, wavelength %d nm",fScan->Data(),fLambda); 
	prod->SetTitle(name2);
	prod->SetMinimum(-1500000); 
	prod->SetMaximum(1500000);
	prod->GetXaxis()->SetTitle("Distance (mm)");
	prod->GetYaxis()->SetTitle("Occupancy Product");
	prod->GetYaxis()->SetTitleOffset(1.15);

	sprintf(savename2,"Product%s%dTol%dSh%d.pdf",fScan->Data(),fLambda,(int)fTolerance,(int)fShadowing);
	c1->SaveAs(savename2,"pdf");

//	delete c1;
//	delete prod;

}
//______________________________________________________________________________________
//
/*void DiagScan::SetDataSet(char *aString){

	if (strcmp(aString,"oct04") && strcmp(aString,"jul04") && strcmp(aString,"aug03") && strcmp(aString,"apr03") &&
	    strcmp(aString,"jan03") && strcmp(aString,"oct02") && strcmp(aString,"may02") &&
	    strcmp(aString,"nov01") && strcmp(aString,"sep01") && strcmp(aString,"jan01a") &&
	    strcmp(aString,"jan01b") && strcmp(aString,"sep00") && strcmp(aString,"feb00")) {
		printf("DataSet %s not available. ",aString); 
		printf("Choose oct04 jul04 aug03,apr03,jan03, oct02, may02,nov01,sep01,jan01a,jan01b,sep00,feb00 \n");
		printf("Setting DataSet to may02 by default. \n");
		aString = "may02";
	}
	if (fDataSet) {
		delete fDataSet;
		fDataSet = new TString(aString);
	} else {
		fDataSet = new TString(aString);
	} 

}
//______________________________________________________________________________________
//
*/
void DiagScan::SetAngResponseCoefficient(Int_t aNumber){

  /// Sets the coefficient necessary for the PMT angular response calculation, according to the
  /// wavelength of the runs that are being analyzed.
  /// This assumes that, at low incidence angles, the PMT angular response has a linear behaviour

  if(aNumber == 337){fAngRespCoefficient = 0.002;}
  if(aNumber == 369){fAngRespCoefficient = 0.003;}
  if(aNumber == 385){fAngRespCoefficient = 0.0028;}
  if(aNumber == 420){fAngRespCoefficient = 0.0032;}
  if(aNumber == 505){fAngRespCoefficient = 0.0036;}

}
//______________________________________________________________________________________
//
Float_t DiagScan::GetLBMaskCorrection( Int_t aLambda , Double_t aTheta ){

  /// Sets the 5 coefficents of the laserball mask intensity distribution polynomial, according
  /// to the wavelength of the runs that are being analyzed

  DB* db = DB::Get();

  std::vector<double> maskcoefs;

  if( aLambda == 337 ){				
    DBLinkPtr lbDB = db->GetLink( "LBDYEANGLE", "N2:AUG06" );
    maskcoefs = lbDB->GetDArray( "lb_mask_coef" );
  }
  if( aLambda == 369 ){
    DBLinkPtr lbDB = db->GetLink( "LBDYEANGLE", "PBD:AUG06" );
    maskcoefs = lbDB->GetDArray( "lb_mask_coef" );
  }
  if( aLambda == 385 ){
    DBLinkPtr lbDB = db->GetLink( "LBDYEANGLE", "BBQ:AUG06" );
    maskcoefs = lbDB->GetDArray( "lb_mask_coef" );
  }
  if( aLambda == 420 ){
    DBLinkPtr lbDB = db->GetLink( "LBDYEANGLE", "BIS-MSB:AUG06" );
    maskcoefs = lbDB->GetDArray( "lb_mask_coef" );
  }
  if( aLambda == 505 ){
    DBLinkPtr lbDB = db->GetLink( "LBDYEANGLE", "COUMARIN-500:AUG06" );
    maskcoefs = lbDB->GetDArray( "lb_mask_coef" );
  }

  Float_t mask = maskcoefs[0];
  for ( Int_t iPar = 1; iPar < 5; iPar++ ){
    fMaskDistribution += maskcoefs[iPar] * pow(aTheta,iPar);
  }

  return mask;
}

//______________________________________________________________________________________
//
void DiagScan::SetLambda(Int_t aNumber){

	if(aNumber != 337 && aNumber != 369 && aNumber != 385 &&  aNumber != 420 && aNumber != 505){
		printf("Wavelength %d not available. Choose 337, 365, 385, 420 or 505\n", aNumber);
		printf("Setting Wavelength to 505 nm by default.\n");
		aNumber = 505;
	}
	fLambda = aNumber;

}
//______________________________________________________________________________________
//
void DiagScan::SetScan(const Char_t *aString){

	if(strcmp(aString,"xpz") && strcmp(aString,"xnz")){
		printf("Scan direction %s not available. Choose xpz or xnz.\n",aString);
		printf("Setting scan direction to xpz by default\n");
		aString = "xpz";
	}
	if(fScan){
		delete fScan;
		fScan = new TString(aString);
	} 
	else{
		fScan = new TString(aString);
	} 

}
//______________________________________________________________________________________
//
void DiagScan::SetTolerance(Float_t aNumber){

	fTolerance = aNumber;

}
//______________________________________________________________________________________
//
void DiagScan::SetShadowing(Float_t aNumber){

	fShadowing = aNumber;

}
//______________________________________________________________________________________
//
