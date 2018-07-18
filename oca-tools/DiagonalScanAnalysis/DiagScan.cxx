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
///
///         source env_oca_snoplus.sh
///
///         At ROOT command line:
///
///           .L DiagScan.cxx+
///
///         Then use the DiagScan function declared in this code or
///         the example code runDiagScan.cxx.
///
////////////////////////////////////////////////////////////////////

#include "DiagScan.h"

using namespace std;
using namespace RAT;
using namespace RAT::DU;

ClassImp(DiagScan);

DiagScan::DiagScan(){}

DiagScan::DiagScan( const Int_t lambda, const std::string& diagonal, const std::string& scan, const std::string& path ){

  Initialize();
  SetLambda( lambda );
  SetDiagonal( diagonal );
  SetScan( scan );
  SetPath( path );
  if ( !lambdaValidity || !diagValidity || !scanValidity || !pathValidity ){ return; }
  ReadData();
  Process();
  FitRatio();
  Product();

}

//______________________________________________________________________________________
//

DiagScan::~DiagScan(){

}

//______________________________________________________________________________________
//

void DiagScan::Initialize(){
 
  cout << "Initializing all variables!" << endl;

  fLambda     = 0;
  fPath       = "";
  fScan       = "";
  fPhase      = "";
  fDiagonal   = "";

  lambdaValidity = true;
  scanValidity   = true;
  pathValidity   = true;
  diagValidity   = true;

  fDistanceCut        = 1500.0;
  fShadowing          = 150.0;

  fNRuns              = 0;

  fNSigma             = 2;

  fAttCoef            = 0.0;
  fAttCoefErr         = 0.0;

  for ( Int_t i = 0; i < NRUNS; i++ ){
    fNPairs[i]        = 0;
    fNPulses[i]       = 0;
    fNPMTs[i]         = 0;
    fRunID[i]         = 0;
    fSourceWL[i]      = 0.0;
    fSourcePos[i]     = TVector3( 0.0, 0.0, 0.0 );
    fSumRatio[i]      = 0.0;
    fSumProduct[i]    = 0.0;
    fSumRerrors[i]    = 0.0;
    fSumPerrors[i]    = 0.0;
    fDistance[i]      = 0.0;

    for ( Int_t j = 0; j < NPMS; j++ ){
      fRatio[i][j]    = 0.0;    
      fProduct[i][j]  = 0.0;  
      fRerrors[i][j]  = 0.0; 
      fPerrors[i][j]  = 0.0;
      fPair1[i][j]    = 0;
      fPair2[i][j]    = 0;

      fPMTPos[i][j]         = TVector3( 0.0, 0.0, 0.0 );
      fPMTOcc[i][j]         = 0.0;
      fPMTCorrections[i][j] = 0.0;
      fPMTCorrOcc[i][j]     = 0.0;
      fPMTDistInAV[i][j]    = 0.0;
    }
  }
}

//______________________________________________________________________________________
//

void DiagScan::ReadData(){

  /// Opens the SOC files and all the relevant information for each run is saved. 

  string field;
  string line;

  ifstream file( "runlist.txt" );

  if ( file.is_open() ){
    while ( getline(file,line) ){

      istringstream IS(line);
      std::vector<std::string> list_of_fields;

      while( IS >> field){ list_of_fields.push_back( field ); }

      if ( fScan == list_of_fields[0] && fLambda == atoi(list_of_fields[2].c_str()) && fDiagonal == list_of_fields[3] ){
        fPhase = list_of_fields[1];
        fNRuns = atoi( list_of_fields[4].c_str() );
        for( Int_t j = 5; j < 5+fNRuns; j++ ){ 
          fRunID[j-5] = atoi( list_of_fields[j].c_str() );
        }
        break;
      }
    }
    file.close();
  }

  if ( fDiagonal == "xpz" )  fDiagonalVector = TVector3( 1.0, 0.0, 1.0 );
  if ( fDiagonal == "xnz" )  fDiagonalVector = TVector3( 1.0, 0.0, -1.0 );

  // Opening the SOC files
  RAT::DU::Utility::Get()->LoadDBAndBeginRun();
  for ( Int_t iRun = 0; iRun < fNRuns; iRun++ ){

    std::string fSocFilename;
    if( fScan == "oct15" ){
      fSocFilename = fPath + fScan + "/" + fPhase + "/" + ::to_string(fRunID[iRun]) + "_Run.root";
    }
    else{
      fSocFilename = fPath + fScan + "/" + fPhase + "/SOC_0000" + ::to_string(fRunID[iRun]) + ".root";
    }
    cout << "Opening file " << fSocFilename << endl;
    RAT::DU::SOCReader *socreader = new RAT::DU::SOCReader( fSocFilename );

    for ( size_t isoc = 0; isoc < socreader->GetSOCCount(); isoc++ ){

      const RAT::DS::SOC& rsoc = socreader->GetSOC( isoc );

      // Number of pulses triggered
      fNPulses[ iRun ] = rsoc.GetNPulsesTriggered();

      // Source wavelength
      fSourceWL[ iRun ] = rsoc.GetCalib().GetMode();

      // Source fitted position
      RAT::DS::FitResult fit = rsoc.GetFitResult("lbfit");
      RAT::DS::FitVertex vert = fit.GetVertex(0);
      fSourcePos[ iRun ] = vert.GetPosition();

      // Select PMTs
      SelectPMTs( iRun, rsoc );

    }
    delete socreader;
  }
}

//______________________________________________________________________________________
//

void DiagScan::Process(){

  /// Pairs of PMTs (one from each group) are formed and their occupancy ratio and product are calculated (including
  /// their corresponding uncertainities).The weighted mean of the ratio and product for each run is calculated.
	
  cout << "Calculating Occupancy Ratios!" << endl;

  for ( Int_t i = 0; i < fNRuns; i++ ){

    // Exclude PMTs with abnormally low or high occupancy. Identify them by comparing their occupancy with the average of the group where they are included.
    // This check can be disabled by commenting the following line.
    CheckLowOccupancy( i );
    
    // Selects the pairs of PMTs   
    TVector3 sum;
    Float_t min;
    Int_t j, l, m;

    for ( Int_t k = 0; k < ( fNPMTs[i] - 1 ); k++ ){
      // Find out the pair for pmt k
      min = 500.; 
      j   =  -1;
      for ( l=k+1; l < fNPMTs[i]; l++ ){
        Int_t count = 0;
        if ( fNPairs > 0 ) {
          for ( m = 0; m < fNPairs[i]; m++ ) {
            // Check for double counting
            if ( l == fPair2[i][m] || k == fPair2[i][m] || l == fPair1[i][m] || k == fPair1[i][m] ) count = 1;
          }
        }
        sum = fPMTPos[i][k] + fPMTPos[i][l];
        if ( count == 0 && sum.Mag() <  min ){ 
          min = sum.Mag(); 
          j = l;
        }
      }
      if ( j >= 0 ){
        if( fPMTPos[i][k].Z() > 0. ){
          fPair1[i][fNPairs[i]] = k;
          fPair2[i][fNPairs[i]] = j;
        }
        else {
          fPair1[i][fNPairs[i]] = j;
          fPair2[i][fNPairs[i]] = k;
        }
        fNPairs[i]++;
      }	
    }

    // Calculate Occ1/Occ2 and Occ1*Occ2
    Float_t sumR        = 0.;
    Float_t Rweight     = 0.;
    Float_t sumP        = 0.;
    Float_t Pweight     = 0.;
    Float_t distance    = 0.;

    for ( Int_t k = 0; k < fNPairs[i]; k++ ){
      Int_t l = fPair1[i][k];
      Int_t m = fPair2[i][k];

      fRatio[i][k]     = fPMTCorrOcc[i][l] / fPMTCorrOcc[i][m];
      fRerrors[i][k]   = TMath::Sqrt(fPMTCorrOcc[i][l] / (TMath::Power(fPMTCorrOcc[i][m],2)*fPMTCorrections[i][l]) + TMath::Power(fPMTCorrOcc[i][l],2) / (TMath::Power(fPMTCorrOcc[i][m],3)*fPMTCorrections[i][m]));

      // Product normalized by the square of the number of pulses emitted
      fProduct[i][k]   = (fPMTCorrOcc[i][l] * fPMTCorrOcc[i][m]) / TMath::Power(fNPulses[i],2);
      fPerrors[i][k]   = TMath::Sqrt((TMath::Power(fPMTCorrOcc[i][l],2) * fPMTCorrOcc[i][m]/(fPMTCorrections[i][m])) + (fPMTCorrOcc[i][l] * TMath::Power(fPMTCorrOcc[i][m],2) / (fPMTCorrections[i][l]))) / fNPulses[i];

      distance         = fPMTDistInAV[i][l] - fPMTDistInAV[i][m];

      sumR             = sumR + fRatio[i][k]/TMath::Power(fRerrors[i][k],2);
      Rweight          = Rweight + 1/TMath::Power(fRerrors[i][k],2);
   
      sumP             = sumP + fProduct[i][k]/TMath::Power(fPerrors[i][k],2);
      Pweight          = Pweight + 1/TMath::Power(fPerrors[i][k],2);

      fDistance[i] = fDistance[i] + distance;
    }

    fSumRatio[i]   = sumR/Rweight;               // Weighted arithmetic mean value of the occupancy ratio for run i
    fSumRerrors[i] = 1/TMath::Sqrt(Rweight);     // Standard deviation for the Weighted arithmetic mean value 
    fSumProduct[i] = sumP/Pweight;               // Weighted arithmetic mean value of the occupancy product for run i
    fSumPerrors[i] = 1/(TMath::Sqrt(Pweight));   // Standard deviation for the Weighted arithmetic mean value  

    fDistance[i] = fDistance[i]/fNPairs[i];
  }
}

//______________________________________________________________________________________
//

void DiagScan::FitRatio(){

  /// The weighted occupancy ratio of all runs is fitted and plotted with their respective uncertainities.
	
  cout << "Plotting the Ratios and Fitting them!"<< endl;

  TGraphErrors* gr;

  gROOT->SetStyle("Plain");
  gROOT->Reset();
  gStyle->SetOptFit();
  gStyle->SetOptStat(111111);

  TCanvas* c0;
  c0 = new TCanvas("c0","c0",200,10,700,500);
  c0->SetGrid();

  gr = new TGraphErrors(fNRuns,fDistance,fSumRatio,0,fSumRerrors);

  gr->SetMarkerColor(1);
  gr->SetMarkerSize(1.5);
  gr->SetMarkerStyle(21);

  std::string title = "Ratio, Diagonal " + fDiagonal + ", Wavelength " + ::to_string(fLambda) + " nm";
  gr->SetTitle(title.c_str());
  gr->GetXaxis()->SetTitle("2*Distance LB-CenterAV (mm)");
  gr->GetYaxis()->SetTitle("Occupancy Ratio");
  gr->Draw("AP");

  TF1 *fit = new TF1("fit","exp([0]+[1]*x)");
  gr->Fit("fit");
  // Value of the slope
  fAttCoef = fit->GetParameter(1);
  // Error of the slope
  fAttCoefErr = fit->GetParError(1);

  c0->Update();
  std::string saveName = "FittedRatio_" + fScan + "_" + fDiagonal + "_" + ::to_string(fLambda) + ".pdf";
  c0->SaveAs(saveName.c_str(),"pdf");
	
  cout << "#######################" << endl;
  cout << "Inner AV Attenuation Coeffiecient = " 
       << - fAttCoef 
       << " +/- " << fAttCoefErr << " (stat.)" 
       << " mm-1" << endl;
  cout << "#######################" << endl;

  delete fit;
  delete gr;
  delete c0;

}

//______________________________________________________________________________________
//

void DiagScan::Product(){

  /// The weighted occupancy product of all runs is plotted with their respective uncertainities.

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
  std::string title2 = "Product, Diagonal " + fDiagonal + ", Wavelength " + ::to_string(fLambda) + " nm"; 
  prod->SetTitle(title2.c_str());
  prod->SetMinimum(-1500000); 
  prod->SetMaximum(1500000);
  prod->GetXaxis()->SetTitle("2*Distance LB-CenterAV (mm)");
  prod->GetYaxis()->SetTitle("Occupancy Product");
  prod->GetYaxis()->SetTitleOffset(1.15);

  std::string saveName2 = "Product_" + fScan + "_" + fDiagonal + "_" + ::to_string(fLambda) + ".pdf";
  c1->SaveAs(saveName2.c_str(),"pdf");

  delete c1;
  delete prod;
}

//______________________________________________________________________________________
//

void DiagScan::SelectPMTs( const Int_t nRun, const RAT::DS::SOC& soc ){

  const RAT::DU::PMTInfo& pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  RAT::DU::LightPathCalculator lightPath = Utility::Get()->GetLightPathCalculator();
  RAT::DU::ShadowingCalculator shadowCalc = Utility::Get()->GetShadowingCalculator();

  // Set all of the tolerances for shadowing by the detector geometry to 150 mm equivalent.
  shadowCalc.SetAllGeometryTolerances( fShadowing );

  vector<UInt_t> pmtids = soc.GetSOCPMTIDs();
  for ( size_t ipmt = 0; ipmt < pmtids.size(); ipmt++ ){

    Int_t lcn = pmtids[ipmt];    // Logical channel number -> PMT ID number
    const RAT::DS::SOCPMT& pmt = soc.GetSOCPMT(lcn);  

    // Choose only NORMAL PMTs with a calculated peak
    if ( pmtInfo.GetType( lcn ) == RAT::DU::PMTInfo::NORMAL && pmt.GetPeakFindOK() == 0 ){

      TVector3 pmtPos = pmtInfo.GetPosition( lcn );

      // Equivalent energy (MeV) of the wavelength (nm) emmited by the source
      Double_t energy = RAT::util::WavelengthToEnergy( fSourceWL[nRun] * 1.0e-6 );

      // Distance from PMT to diagonal  
      Float_t diagDist; // Distance from PMT to diagonal (mm)
      diagDist = TMath::Sqrt(((pmtPos.Dot(pmtPos))*(fDiagonalVector.Dot(fDiagonalVector))-(-pmtPos.Dot(fDiagonalVector))*(-pmtPos.Dot(fDiagonalVector)))/((fDiagonalVector).Dot(fDiagonalVector)));

      if ( diagDist < fDistanceCut ){
        Double_t localityVal = 10.; // The accepted tolerance (mm) for how close the light path is calculated to the pmtPos

        lightPath.CalcByPosition( fSourcePos[nRun], pmtPos, energy, localityVal );

        // Check for shadowing
        if ( shadowCalc.CheckForShadowing( lightPath ) == false ){
          Int_t nVal = 0; // nVal-sided polygon superimposed onto the PMT bucket for the calculation

          // Solid Angle calculation
          TVector3 pmtNormal = -1.0 * pmtInfo.GetDirection( lcn );
          lightPath.CalculateSolidAngle( pmtNormal, nVal );
          Double_t fSolidAngle = lightPath.GetSolidAngle(); 

          // Total Fresnel transmission coefficient
          Double_t fFresnel = lightPath.GetFresnelTCoeff();

          fPMTPos[ nRun ][ fNPMTs[nRun] ] = pmtInfo.GetPosition( lcn );
          fPMTOcc[ nRun ][ fNPMTs[nRun] ] = pmt.GetPromptOccupancy();
          fPMTCorrections[ nRun ][ fNPMTs[nRun] ] = fSolidAngle * fFresnel;
          fPMTCorrOcc[ nRun ][ fNPMTs[nRun] ] = pmt.GetPromptOccupancy() / ( fSolidAngle * fFresnel );
          fPMTDistInAV[ nRun ][ fNPMTs[nRun] ] = ( lightPath.GetStartPos() - lightPath.GetPointOnAV1st() ).Mag();

          fNPMTs[ nRun ]++;

        }
        lightPath.ResetValues();
      }
    }
  }
}

//______________________________________________________________________________________
//

void DiagScan::CheckLowOccupancy( const Int_t iRun ){

  Float_t SumOccG1 = 0; // Sum of the corrected occupancies in group 1 (z > 0)
  Float_t SumOccG2 = 0; // Sum of the corrected occupancies in group 2 (z < 0)

  Float_t SumOcc2G1 = 0; // Sum of the squared corrected occupancies in group 1 (z > 0)
  Float_t SumOcc2G2 = 0; // Sum of the squared corrected occupancies in group 2 (z < 0)

  Int_t nPMTg1 = 0; // Number of PMTs in group 1 (z > 0)
  Int_t nPMTg2 = 0; // Number of PMTs in group 2 (z < 0)

  for ( Int_t pm = 0; pm < fNPMTs[iRun]; pm++ ){

    if ( fPMTPos[iRun][pm].Z() > 0 ){
      SumOccG1 = SumOccG1 + fPMTCorrOcc[iRun][pm];
      SumOcc2G1 = SumOcc2G1 + TMath::Power(fPMTCorrOcc[iRun][pm],2);
      nPMTg1++;
    }
    if ( fPMTPos[iRun][pm].Z() < 0 ){
      SumOccG2 = SumOccG2 + fPMTCorrOcc[iRun][pm];
      SumOcc2G2 = SumOcc2G2 + TMath::Power(fPMTCorrOcc[iRun][pm],2);
      nPMTg2++;
    }
  }

  Float_t AvOccG1 = SumOccG1 / nPMTg1; 
  Float_t AvOccG2 = SumOccG2 / nPMTg2;

  Float_t AvOcc2G1 = SumOcc2G1 / nPMTg1; 
  Float_t AvOcc2G2 = SumOcc2G2 / nPMTg2;

  // Standard deviation for each group, as defined in  http://mathworld.wolfram.com/StandardDeviation.html
  Float_t sigma1 = TMath::Sqrt(AvOcc2G1 - AvOccG1*AvOccG1);
  Float_t sigma2 = TMath::Sqrt(AvOcc2G2 - AvOccG2*AvOccG2);

  // Follows a loop over all PMTs. If their occupancy is bellow AvOcc-fNSigma*sigma or above AvOcc+fNSigma*sigma of their corresponding group, they are not used in the analysis.
  // Updates the arrays with PMT info.
  Int_t counter = 0;
  for ( Int_t pm = 0; pm < fNPMTs[iRun]; pm++ ){
    if ( fPMTPos[iRun][pm].Z() > 0 ){
      if ( fPMTCorrOcc[iRun][pm] > (AvOccG1 - fNSigma*sigma1) && fPMTCorrOcc[iRun][pm] < (AvOccG1 + fNSigma*sigma1) ){

        fPMTPos[iRun][counter] = fPMTPos[iRun][pm];
        fPMTOcc[iRun][counter] = fPMTOcc[iRun][pm];
        fPMTCorrections[iRun][counter] = fPMTCorrections[iRun][pm];
        fPMTCorrOcc[iRun][counter] = fPMTCorrOcc[iRun][pm];
        fPMTDistInAV[iRun][counter] = fPMTDistInAV[iRun][pm];

        counter++;
      }
    }
    if ( fPMTPos[iRun][pm].Z() < 0 ){
      if ( fPMTCorrOcc[iRun][pm] > (AvOccG2 - fNSigma*sigma2) && fPMTCorrOcc[iRun][pm] < (AvOccG2 + fNSigma*sigma2) ){

        fPMTPos[iRun][counter] = fPMTPos[iRun][pm];
        fPMTOcc[iRun][counter] = fPMTOcc[iRun][pm];
        fPMTCorrections[iRun][counter] = fPMTCorrections[iRun][pm];
        fPMTCorrOcc[iRun][counter] = fPMTCorrOcc[iRun][pm];
        fPMTDistInAV[iRun][counter] = fPMTDistInAV[iRun][pm];

        counter++;
      }
    }
  }
  fNPMTs[iRun] = counter;
}

//______________________________________________________________________________________
//

void DiagScan::SetLambda( const Int_t aNumber ){

  if (aNumber != 337 && aNumber != 365 && aNumber != 369 && aNumber != 385 &&  aNumber != 420 && aNumber != 450 && aNumber != 500 && aNumber != 505){
    printf("The wavelength %d nm does not belong to the list of wavelengths emitted by the laserball!\n", aNumber);
    lambdaValidity = false;
    return;
  }
  fLambda = aNumber;
}

//______________________________________________________________________________________
//

void DiagScan::SetScan( const std::string& aString ){

  if ( aString != "oct15" && aString != "dec17" && aString != "jul18" ){
    cout << aString << " is not a valid laserball scan!" << endl;
    scanValidity = false;
    return;
  }
  fScan = aString;
}

//______________________________________________________________________________________
//

void DiagScan::SetPath( const std::string& aString ){

  struct stat s;
  if ( stat( aString.c_str(), &s ) != 0 ){
    cout << "The path inserted does not exist!" << endl;
    pathValidity = false;
    return;
  }
  fPath = aString;
}

//______________________________________________________________________________________
//

void DiagScan::SetDiagonal( const std::string& aString ){

  if ( aString == "xpz" || aString == "xnz" || aString == "ynz" || aString == "ypz" ){
    fDiagonal = aString;
  }
  else {
    cout << aString << " is not a valid diagonal! Options are xpz, xnz, ypz or ynz." << endl;
    diagValidity = false;
    return;
  }
}

//______________________________________________________________________________________
//

void DiagScan::SetDistanceCut( const Float_t aNumber ){

  fDistanceCut = aNumber;
}

//______________________________________________________________________________________
//

void DiagScan::SetShadowingTolerance( const Float_t aNumber ){

  fShadowing = aNumber;
}

//______________________________________________________________________________________
//

void DiagScan::SetNSigmas( const Int_t aNumber ){

  fNSigma = aNumber;
}
