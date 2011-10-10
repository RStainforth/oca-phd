//___________________________________________________________________________
//
// Class description for the QDiag class
//
// Diagonal analysis method:
// -------------------------
// 		1 - A group of runs located along a straight line (a diagonal to
// avoid the belly plates or the neck region) is selected.
//		2 - Pairs of PMTs located close to the diagonal axis are selected
//		3 - The occupancy ratio for each pair is calculated, taking into account
// the solid angle correction, as well as other minor corrections, due to the
// PMT angular response and laserball asymmetry.
//		4 - The occupancy ratios are averaged for all selected pairs weighted by
// statistical error, calculated from the raw occupancy.
// 		5 - The average occupancy ratio are fitted to an exponential in function
// of source distance to the center. The exponential slope is twice the
// attenuation length.
//
// How to use this class:
// ----------------------
// 		Inputs:
// 	The data are taken from the OCATree in the qpath files, and the correction 
// factors from the qocafit files, so you need them both. However, no rdt, rch
// or dqxx files are needed.
// It also needs an ascii file containing the list of runs for each dataset,
// wavelength and diagonal direction. This is the diagonal_runlist.dat file in the
// qoca directory. 
//
// 		Outputs:
// 	As an intermediate output, root files containing histograms for each run are
// written to the data/histo directory (that you have to create). This is to avoid
// looping through the large qpath files everytime.
// 	The inverse attenuation length (plus stat error) is written to an ascii file
// fit_results_{dataset}.txt in your home directory. 
// 	The attenuations are also written as TVectors in a root file, since it can be  
// more convenient for subsequent use in macros.
// Postscript files with the occupancy ratios in function of distance are written 
// to {home}/eps
//
// Usage:
// ------
// After creating an instance of the class, you'll need to change the default 
// data directory with SetDataDir, unless you're on loon at Queen's
// You might also want to use SetHomeDir to redirect the output.
// The data can be retrieved in two ways:
// 		- the ProjectData() method extracts it from the OCATree, loads it to memory
// 		and writes out histofram files for subsequent faster loading.
//		You have to use this method if you're running for the first time.
//		- the ReadData() loads to memory the histograms saved with a previous call
// 		to ProjectData(). This is of course faster.
// The PMT pair selection and occupancy ratio calculation is done in Process().
// The actual fit to the ratios and output of results is done in FitRatio()
// The FitProduct() method plots the normalized occupancy product for the same PMT
// pairs (the product should be nearly independent of distance). 
//
// To reuse the same instance of the class, use Initialize() and then the Setters
// for the various parameters. 
//
//*-- Author : Jose' Maneira, based on macro by Aksel Hallin
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include "QDiag.h"
#include <string.h>

ClassImp(QDiag);

QDiag::QDiag()
{
    // Constructor for QDiag class. 
    // Sets default values.
    Initialize();
}

//______________________________________________________________________________________
//

QDiag::QDiag(char *dataset,int lambda,char *postype)
{
    // Constructor for QDiag class. 
    // Sets default values for scan and tolerance. 
    Initialize();
    SetDataSet(dataset);
    SetLambda(lambda);
    SetPosType(postype);
}

//______________________________________________________________________________________
//

QDiag::QDiag(char *dataset,int lambda,char *postype,char *scanc,Float_t tolerance,char *correction)
{
    // Constructor for QDiag class.
    // Sets all 6 user-defined values.
	// -As of March 2003, the datasets can be jan03, oct02, may02, sep01 and sep00
	// -Wavelength (lambda) can be 337,365,386,420,500 or 620
	// -The type of fit position to use (postype) is either fruns or fits 
	// -The direction of the diagonal scan (scanc) is either xzscan or xnzscan
	// -The tolerance is the maximum distance in cm between the PMT and the diagonal
	// axis. It can be a number between about 30 and 180. 
	// -Solid Angle correction is always applied. Additionaly, you can choose to
	// use no other correction (none), just PMT angular response correction (pmt),
	// just laserball asymmetry correction (laser) or both (all).
	// 
    Initialize();
    SetDataSet(dataset);
    SetLambda(lambda);
    SetPosType(postype);
    SetScan(scanc);
    SetTolerance(tolerance);
	SetCorrectionType(correction);
}

//______________________________________________________________________________________
//

QDiag::~QDiag()
{
    // Default destructor.
    if(fDataDir)		delete  fDataDir;
    if(fHomeDir)		delete  fHomeDir;
    if(fDataSet)		delete  fDataSet;
    if(fPosType)		delete  fPosType;
    if(fScan)			delete  fScan;
    if(fCorrectionType) delete  fCorrectionType;
    if(fTreeFileName)	delete  fTreeFileName;
    if(fHistoFileName)  delete  fHistoFileName;
    if (fHistoFile) fHistoFile->Close();
    int i;
    for (i = 0; i < NRUNS; i++) {
       	if (fHocc[i]){
            fHocc[i]->Delete();
      	}
      	if (fHcounts[i]){
             fHcounts[i]->Delete();
        }  
    }
  
    printf("QDiag Object deleted \n");
}
//______________________________________________________________________________________
//

void QDiag::Initialize()
{
    // The default settings are the may02 dataset, wavelength 500 nm,
	// fruns position type, xz scan direction and 70 cm tolerance.
	// The default data directory is /net/vulture/optics/current/
	
	fDataDir             = new TString("/dsk2/optics/current/");
    fHomeDir             = new TString(getenv("HOME"));
    fDataSet             = new TString("may02");
    fPosType             = new TString("fruns");
    fScan                = new TString("xzscan");
    fCorrectionType 	 = new TString("all");
    fLambda              = 500;
    fTolerance           = 100;
	fInvAtt				 = 0;
	fInvAttErr			 = 0;
 
    fNpmts = NPMS;
    fNruns = NRUNS;
    for (int i = 0; i < NRUNS; i++) {
        fRun[i] =  0;
	//        fSource[i] = TVector3(0,0,0);
        fHocc[i]     = NULL; 
        fHcounts[i] = NULL; 
	    fSumDistance[i] = 0;
        fSumRatio[i]	= 0;
        fSumProduct[i]  = 0;
        fSumRerrors[i]	= 0;
        for (int j = 0; j < NPMS; j++) {
            fDistance[j][i]   = 0; 
            fRatio[j][i]      = 0;    
            fProduct[j][i]    = 0;  
            fRerrors[j][i]    = 0;    
        } 
    }
    UpdateFileNames();
    printf("QDiag Object initialized \n");
}

//______________________________________________________________________________________
//

void QDiag::ProjectData()
{
    // The occupancy data is retrieved from the OCA tree by projecting it into 1D 
    // histograms. Since this is time-consuming, the histograms are saved to file 
    // and the fit is done later.
	//
    
    
    Int_t i;
    char lCondition1[96], lHname1[20], lName1[30],lVname[30];  
    char lCondition2[96], lHname2[20], lName2[30];  
  
    // Get runlist, source position and OCATree from OCA file
    TFile fin(fTreeFileName->Data(),"READ");
    printf("File %s successfully open\n",fTreeFileName->Data());
//    QOCATree *ocatree = (QOCATree *) fin.Get("optica");
     QOCATree *ocatree = (QOCATree *) fin.Get("optix");
   fNruns = (int) ocatree->GetEntries();
    printf("Runs in this file: %d \n",fNruns);
    QOCARun *ocarun = new QOCARun();
    ocatree->SetBranchAddress("Runs",&ocarun);
    for (i=0; i<fNruns; i++){
        ocatree->GetEntry(i);
        fSource[i] = ocarun->GetLaserxyz();
        fRun[i] = ocarun->GetRun();
        printf("Run %d (%d of %d) at position %f %f %f\n",
            fRun[i],(i+1),fNruns,fSource[i].X(),fSource[i].Y(),fSource[i].Z());
    }
 
  
//   Project the ocatree to the histograms
    for(i = 0; i < fNruns; i++){ 
    
        sprintf(lCondition1,"fOccupancy/fSolidangle*(fRun==%d)*(!fBad)",fRun[i]);
        sprintf(lCondition2,"fNprompt*(fRun==%d)*(!fBad)",fRun[i]);
        sprintf(lHname1,"Run%d",fRun[i]);
        sprintf(lHname2,"C%d",fRun[i]);
        sprintf(lName1,"fPmtn>>%s",lHname1);
        sprintf(lName2,"fPmtn>>%s",lHname2);
 
        fHocc[i]     = new TH1F(lHname1,lHname1,10000,0,10000);
        fHcounts[i] = new TH1F(lHname2,lHname2,10000,0,10000);
   
        ocatree->Draw(lName1,lCondition1,"goff"); 
        ocatree->Draw(lName2,lCondition2,"goff"); 
    
        printf("Making Histograms for Run # %d of %d ...be patient\n",(i+1),fNruns);
    }

      
    // Save histograms and position vector
    fHistoFile = new TFile(fHistoFileName->Data(),"RECREATE");    
    if (fHistoFile->IsOpen()) {
        for(i=0;i<fNruns;i++){   	    
	    	fHocc[i]->GetEntries();   
	    	fHcounts[i]->GetEntries();
	    	fHocc[i]->Write();   
	    	fHcounts[i]->Write();	    
	    	fHocc[i]->SetDirectory(gROOT);
	    	fHcounts[i]->SetDirectory(gROOT);
			sprintf(lVname,"Position%d",fRun[i]);
			fSource[i].Write(lVname);
			//fSource[i]->SetDirectory(gROOT);
        }
        printf("Done saving files \n");
    } else {
        printf("File %s cannot be opened \n",fHistoFileName->Data());
    }
    fHistoFile->Purge();  
    fHistoFile->Close();
}

//______________________________________________________________________________________
//


void QDiag::ReadData()
{

     // This method loads data into memory:
     //    - the list of  runs # to use in the scan from an ascii file
     //    - source positions for those runs from the OCATree file
     //    - the occupancy histograms previously saved in ProjectData()
     //
     // To use this method, you need to call ProjectData() first and to have
	 // an ascii file named diagonal_runlist.txt with the list of runs for 
	 // each diagonal. The format is:
	 // dataset wavelength scan nruns run[0] ...run[nruns] 0-filled up to 10
	 
     Int_t i,j,wl,nr,rs[10];
     Char_t ds[5],sc[6],name[128];
     sprintf(name,"%s/qoca/diagonal_runlist.dat",getenv("QSNO_ROOT"));
     ifstream fin(name);
     while (fin.peek() != EOF) {
    	 fin >> ds >> wl >> sc >> nr >> rs [0] >> rs[1] >> rs [2] >> rs [3] >> rs[4] >> rs [5]
    	    >> rs [6] >> rs[7] >> rs [8] >> rs[9];
    	 if (!strcmp(fDataSet->Data(),ds) && fLambda == wl && !strcmp(fScan->Data(),sc)) break;
     } 
     std::cout << "Runlist for " << ds << " " << wl << " " << sc << " " << nr << std::endl;
     fNruns = nr;
     for (i=0;i<fNruns;i++) fRun[i] = rs[i];
     fin.close();
     
     // Read the source positions from the tree
     TFile ftree(fTreeFileName->Data(),"READ");
     printf("File %s successfully open\n",fTreeFileName->Data());
     QOCATree *ocatree = (QOCATree *) ftree.Get("optix");
     QOCARun  *ocarun  = new QOCARun();
     ocatree->SetBranchAddress("Runs",&ocarun);
     Int_t lTotRuns = (Int_t) ocatree->GetEntries();
     for(i = 0; i < lTotRuns; i++){
	 	if (!strcmp(fDataSet->Data(),"sep01") && fLambda ==337 && i ==26) continue;
         ocatree->GetEntry(i);
         for(j = 0; j < fNruns; j++){
    	     if(ocarun->GetRun() == fRun[j]){
    	       fSource[j] = ocarun->GetLaserxyz();
     	       fNorm[j] = ocarun->GetNorm();
   	       	 	printf("Run %d (%d of %d) at position %f %f %f\n",
    	     	      fRun[j],(j+1),fNruns,fSource[j].X(),fSource[j].Y(),fSource[j].Z());
    	      break;
    	     }    
         }
     }
		
	 


     // Read histos from saved file
     char lHname1[20],lHname2[20];  
     TFile fhist(fHistoFileName->Data(),"READ");
     for(i=0; i<fNruns; i++){

    	  sprintf(lHname1,"Run%d",fRun[i]);
    	  sprintf(lHname2,"C%d",fRun[i]);
    	  
    	  fHocc[i]     = new TH1F();
    	  fHcounts[i] = new TH1F();

    	  fHocc[i]     = (TH1F *) fhist.Get(lHname1);
    	  fHcounts[i] = (TH1F *) fhist.Get(lHname2);

	  	  fHocc[i]->GetEntries();
	  	  fHcounts[i]->GetEntries();
	  
	  	  fHocc[i]->SetDirectory(gROOT);
	  	  fHcounts[i]->SetDirectory(gROOT);
	  
     }
   fhist.Close();   
   printf("Histos read and file closed \n");
}

//______________________________________________________________________________________
//


void QDiag::Process()
{	
    // This method is to be called after the histograms were loaded with
	// ReadData(). 
    // The PMT pairs to be used in the analysis are selected according to the 
	// scan direction and the tolerance.
	// For each pair, the ratio and product (for quality control) of corrected
	// occupancy are calculated.  
    // The sum of the ratios and products for all pairs is calculated.
	//
    QPMTxyz  pos("read");
    TVector3 sum, center, pmts[NPMS];
    Float_t  table[NPMS][4+NRUNS], rawtable[NPMS][4+NRUNS];
    Int_t    pair1[NPMS],pair2[NPMS]; 
    Float_t  weight, weight_prod, min;
    Int_t    i, j, k, m, idelete;
    Float_t  staterror = 0.03; // statistical uncertainty in single tube about 3%
    //Float_t  systerror = 4.5;    // systematical error on position
    //  if(fSource[j][2]>200) systerror=7; else if (fSource[j][2]>0)systerror=4.5;
  
    
    // Select scan direction
    if (!strcmp(fScan->Data(),"xzscan"))  center = TVector3(1,0,1).Unit();
    if (!strcmp(fScan->Data(),"xnzscan")) center = TVector3(1,0,-1).Unit();
    if (!strcmp(fScan->Data(),"yzscan"))  center = TVector3(0,1,1).Unit();
    if (!strcmp(fScan->Data(),"ynzscan")) center = TVector3(0,-1,1).Unit();    
    center *= 840;
    printf("Process: Scan direction/PSUP intersection: %f %f %f\n", center.X(), center.Y(),center.Z());  

  	// The correction factors are in the qocafit file
	QOCAFit *oca;
	if (strcmp(fCorrectionType->Data(),"none")){	
		Char_t ocafitfile[128];	
		sprintf(ocafitfile,"%sqocafit/qocafit_%s_%s_%d.root",
			fDataDir->Data(),fDataSet->Data(),fPosType->Data(),fLambda);			
		printf("Process: Getting correction %s factor from file %s \n",
			fCorrectionType->Data(),ocafitfile);
		TFile *focafit = new TFile(ocafitfile,"READ");
		oca =  (QOCAFit*) focafit->Get("qocafit_nominal");
	}
    

    // Select the PMTs that will be used for the analysis, choosing the ones closest
    // to the intersection of the scan direction with the PSUP and fill tables with 
    // their info and data.

    printf("Process: Select PMTs within %2.0f cm tolerance.\n",fTolerance);  
    for(idelete=k=i=0; i<10000; i++){
        pmts[k] = TVector3(pos.GetX(i),pos.GetY(i),pos.GetZ(i));    
        if((center-pmts[k]).Mag() < (Double_t) fTolerance || 
	   (center+pmts[k]).Mag() < (Double_t) fTolerance){             
            
	    // Fill tables with PMT position
            table[k][0] = i; 
            table[k][1] = pmts[k].X();
            table[k][2] = pmts[k].Y();
            table[k][3] = pmts[k].Z();
            
	    // Now fill the tables with the PM occupancy from the histos
            printf("Process: PMT # %d at %3.0f %3.0f %3.0f  - Solid Angle Corrected Occupancies: \n",
				i,pmts[k].X(),pmts[k].Y(),pmts[k].Z());
            for(j=0; j<fNruns; j++){
                table[k][4+j]	 = fHocc[j]->GetBinContent(i+1);
				
				// Calculate the correction factor
				// Laserball distribution
				Float_t costheta = (pmts[k] - fSource[j]).CosTheta();
				Float_t phi 	 = (pmts[k] - fSource[j]).Phi();
				if (costheta>1) costheta = 1;
				else if (costheta<-1) costheta = -1;
				if (phi > 2*M_PI) phi -= 2*M_PI;
				else if (phi < 0) phi += 2*M_PI;
  				Float_t laser = 1.0;
				
				// PMT angular distribution
				Float_t angle = (Float_t) pmts[k].Angle(pmts[k] - fSource[j]);
				if (angle > 2*M_PI) angle -= 2*M_PI;
				else if (angle < 0) angle += 2*M_PI;
				Float_t pmt = 1.0;
				//printf("%d %d %f %f %f %f %f ",i,j,costheta,phi,angle,laser,pmt);
				
				Float_t correction = 1.0;
				if (!strcmp(fCorrectionType->Data(),"laser") || !strcmp(fCorrectionType->Data(),"all")) laser = oca->ModelLBDist(costheta,phi);
				if (!strcmp(fCorrectionType->Data(),"pmt")   || !strcmp(fCorrectionType->Data(),"all"))   pmt = oca->ModelAngResp(angle);
				correction = laser * pmt;
				correction = 1/correction;
				
				table[k][4+j] *= correction; 
				
                rawtable[k][4+j] = fHcounts[j]->GetBinContent(i+1); // for error calculation 
                //printf(" | %f %f \n",table[k][4+j],rawtable[k][4+j]);
                if(table[k][4+j]==0 || rawtable[k][4+j]==0){
                    printf("\n Zero occupancy PMT %d Run %d k %d \n",i,fRun[j],k);
                    idelete=1;
                } else {
					printf("Process: PMT %d k %d Run %d Occ %4.1f RawOcc %4.0f Correction %1.3f\n",
						i, k, fRun[j],table[k][4+j],rawtable[k][4+j],correction);
				}
            }
            printf("\n");
            k++;
            if(idelete){idelete=0; k--;}
         }
    }
    fNpmts = k;  
    printf("Process: Total PMTs within tolerance = %d\n",fNpmts); 
  
    // Make pairs of diametrically opposite PMTs
	//initialize	
	Int_t npairs = 0;
	for(i = 0; i < fNpmts; i++){
		pair1[i] = 0;
		pair2[i] = 0;
	}
	
	for(i = 0; i < (fNpmts-1); i++){
		// Find out the pair for pmt i
        min = 500;
		k = -1;
        for (j=i+1; j<fNpmts;j++){
			Int_t count = 0;
			if(npairs > 0) {
				for (m = 0; m < npairs; m++) {
					//check for double counting
					if (j == pair2[m] || i == pair2[m] ||
						j == pair1[m] || i == pair1[m]) count = 1;
				}
			}
            sum = pmts[i] + pmts[j];
            if(count == 0 && sum.Mag() < (Double_t) min){ 
				//get closest to diameter
				min = sum.Mag(); 
				k = j;
			}
        }  
		if (k >= 0) {
			if (table[i][3] > 0){
				pair1[npairs] = i;
				pair2[npairs] = k;
			} else {
				pair1[npairs] = k;
				pair2[npairs] = i;
			}
			npairs++;
		}		
    }
	fNpairs = npairs;
	printf("Process: Number of PMT Pairs: %d\n",fNpairs);
	for (i = 0; i < fNpairs; i++){
		printf("Process: Pair %d of tube %d (%4.0f) at %3.0f %3.0f %3.0f is tube %d (%4.0f) at %3.0f %3.0f %3.0f\n",
			i,pair1[i],table[pair1[i]][0],table[pair1[i]][1],table[pair1[i]][2],table[pair1[i]][3],
			  pair2[i],table[pair2[i]][0],table[pair2[i]][1],table[pair2[i]][2],table[pair2[i]][3]);
	}
	
	
	for(i = 0; i < fNpairs; i++){		
            for(j=0; j< fNruns; j++){
                fRatio[i][j]    = table[pair1[i]][j+4] / table[pair2[i]][j+4];
                fProduct[i][j]  = sqrt(table[pair1[i]][j+4] * table[pair2[i]][j+4] * fNorm[j] * fNorm[j]);
                fDistance[i][j] = pmts[pair1[i]].Unit() * fSource[j];
                Float_t temp    = sqrt(pow(staterror + 1.0/sqrt(rawtable[pair1[i]][j+4]),2) + 
                                       pow(staterror + 1.0/sqrt(rawtable[pair2[i]][j+4]),2));
                fRerrors[i][j] = fRatio[i][j] * temp;
				fPerrors[i][j] = 0.5 * fProduct[i][j] * temp;
				weight          = fRerrors[i][j] *  fRerrors[i][j]; 
				weight_prod     = fPerrors[i][j] *  fPerrors[i][j]; 
                if(weight == 0 || weight_prod == 0) continue;
                weight           = 1.0/weight;
                weight_prod      = 1.0/weight_prod;
                fSumRatio[j]    += weight * fRatio[i][j];
                fSumDistance[j] += weight * fDistance[i][j];
                fSumRerrors[j]  += weight;
                fSumProduct[j]  += weight_prod * fProduct[i][j];
                fSumPerrors[j]  += weight_prod;
            }
    }
  
  
    for(j=0; j< fNruns; j++){
        weight     = fSumRerrors[j];
        weight_prod = fSumPerrors[j];
        fSumRatio[j]    /= weight;
        fSumProduct[j]  /= weight_prod;
        fSumDistance[j] /= weight;
        fSumRerrors[j]	= 1.0/sqrt(weight);
        fSumPerrors[j]	= 1.0/sqrt(weight_prod);
        printf("Process: %d Run %d fRatio %1.3f fDistance %3.1f \n",j,fRun[j],fSumRatio[j],fSumDistance[j]);
    }

}  
//______________________________________________________________________________________
//
void QDiag::FitRatio()
{
	// The PMT occupancy ratios are plotted in function of distance (gif files
	// produced) and fitted to exponentials.
	// The D20 inverse attenuation length is calcuated as 1/2 the slope of the
	// sum-of-ratios exponetials and the paramenters are saved in an ascii file.
	
	int i;
    char name[128], fnameall[128],fnamesum[128];									     
	TCanvas* canvas;
	TCanvas* sumcanvas;
	canvas = new TCanvas("canvas","Canvas",0,0,800,800);

	
    if (fNpairs <= 12) {
		canvas->Divide(4,3);
	} else if (fNpairs <= 20) {
	    canvas->Divide(5,4);
	} else {
		canvas->Divide(7,5);
    }
	
	TGraphErrors *gr[NPMS];
    for(i = 0; i < 35 && i < fNpairs; i++){
		canvas->cd(i+1);														     
		gr[i]=new TGraphErrors(fNruns,fDistance[i],fRatio[i],NULL,fRerrors[i]);      
    	sprintf(name,"Ratio PMT pair %d",i);  
		gr[i]->SetTitle(name);						     
    	gr[i]->SetMarkerStyle(20); 											     
    	gr[i]->Fit("expo","Q");													     
    	gr[i]->Draw("ap"); 													     
     }  																			     

    sumcanvas = new TCanvas("sumcanvas","Sumcanvas",0,0,500,500);					     

    TGraphErrors *sumgr;															     
    TF1 exp("exx","expo");  														     
    sumcanvas->cd(1);																     
    sumgr=new TGraphErrors(fNruns,fSumDistance,fSumRatio,NULL,fSumRerrors);  		     
    sumgr->SetMarkerStyle(20);  													     
    sprintf(name,"%s %d nm, %s %s %d cm tol %d pmt pairs %s Cor",  						     
	   				fDataSet->Data(),fLambda,fPosType->Data(),fScan->Data(),
					(int)fTolerance,fNpairs,fCorrectionType->Data());    
    sumgr->SetTitle(name);  														     
    sumgr->Fit("exx","Q");  															     
    sumgr->Draw("ap");  															     

    fInvAtt    = exp.GetParameter(1)/2;
	fInvAttErr = exp.GetParError(1)/2;
	
	sprintf(fnameall,"%seps/all_%s%d%s%s%d%s.eps",fHomeDir->Data(),fDataSet->Data(),       
				fLambda,fPosType->Data(),fScan->Data(),(int) fTolerance,fCorrectionType->Data());			     
    sprintf(fnamesum,"%seps/sum_%s%d%s%s%d%s.eps",fHomeDir->Data(),fDataSet->Data(),       
				fLambda,fPosType->Data(),fScan->Data(),(int) fTolerance,fCorrectionType->Data());			     
																					     
	canvas->SaveAs(fnameall);
	sumcanvas->SaveAs(fnamesum);

	char fname[128],sp1[3],sp2[3]; 
	strcpy(sp1," "); 
	strcpy(sp2," "); 
	if (fPosType->Length() == 4) strcpy(sp1,"  ");
	if (fScan->Length() == 6)  strcpy(sp2,"  ");
	
	
       sprintf(fname,"%sfit_results_%s.txt",fHomeDir->Data(),fDataSet->Data());
       ofstream fout(fname,std::ios::app);
        fout << fPosType->Data() << sp1  <<  fScan->Data() << sp2 << fLambda << " nm "      
             << fInvAtt << " " << fInvAttErr << " " << fTolerance <<" tol "<< fNpairs 
			 <<" pairs " << fCorrectionType->Data() << "Cor" << std::endl;	        	  
        fout.close();
		
	if (fLambda == 337) i = 0;
	if (fLambda == 365) i = 1;
	if (fLambda == 386) i = 2;
	if (fLambda == 420) i = 3;
	if (fLambda == 500) i = 4;
	if (fLambda == 620) i = 5;
	sprintf(fname,"%sfit_results_%d%s.root",fHomeDir->Data(),(int) fTolerance,fCorrectionType->Data());
	TFile *fres = new TFile(fname,"UPDATE");
	sprintf(name,"value_%s%s%s",fDataSet->Data(),fPosType->Data(),fScan->Data());
	TVectorF *value = (TVector*) fres->Get(name);
	if (!value) value = new TVector(6);
	(*value)(i) = fInvAtt;
	value->Write(name);
	sprintf(name,"error_%s%s%s",fDataSet->Data(),fPosType->Data(),fScan->Data());
	TVectorF *error = (TVector*) fres->Get(name);
	if (!error) error = new TVector(6);
	(*error)(i) = fInvAttErr;
	error->Write(name);
	fres->Close();
	
			
			
}  
//______________________________________________________________________________________
//
void QDiag::FitProduct()
{
	// The PMT occupancy products are plotted in function of distance (gif files
	// produced).
	// The optical model predicts that the product should be constant with 
	// distance, so these distributions provide quality control for the
	// attenuation length extraction.

    int i;
    char name[128], fnameall[128],fnamesum[128];									     
    TCanvas* canvas;
	TCanvas* sumcanvas;
	canvas = new TCanvas("canvas","Canvas",0,0,800,500);

    if (fNpairs <= 12) {
		canvas->Divide(4,3);
	} else if (fNpairs <= 20) {
	    canvas->Divide(5,4);
	} else {
		canvas->Divide(7,5);
    }
	
	
	TGraphErrors *gr[NPMS];
    for(i = 0; i < 35 && i < fNpairs; i++){
    	canvas->cd(i+1);														    	 
    	gr[i]=new TGraphErrors(fNruns,fDistance[i],fProduct[i],NULL,fPerrors[i]);   		
    	gr[i]->SetMarkerStyle(20);  											    	 
    	sprintf(name,"Product PMT pair %d",i);  
		gr[i]->SetTitle(name);						     
    	gr[i]->Draw("ap");  													    	 
     }  																			     

    sumcanvas = new TCanvas("sumcanvas","Sumcanvas",0,0,500,500);					     

    TGraphErrors *sumgr;															     
    sumcanvas->cd(1);																     
    sumgr=new TGraphErrors(fNruns,fSumDistance,fSumProduct,NULL,fSumPerrors);  		     
    sumgr->SetMarkerStyle(20);  													     
    sprintf(name,"%s %d nm, %s %s %d cm Tol %d Pmt pairs %s Cor",  						     
	   fDataSet->Data(),fLambda,fPosType->Data(),fScan->Data(),
	   (int)fTolerance, fNpairs,fCorrectionType->Data());    
    sumgr->SetTitle(name);  														     
    sumgr->Draw("ap");  															     

	
	sprintf(fnameall,"%seps/prodall_%s%d%s%s%d%s.eps",fHomeDir->Data(),fDataSet->Data(),       
				fLambda,fPosType->Data(),fScan->Data(),(int) fTolerance,fCorrectionType->Data());			     
    sprintf(fnamesum,"%seps/prodsum_%s%d%s%s%d%s.eps",fHomeDir->Data(),fDataSet->Data(),       
				fLambda,fPosType->Data(),fScan->Data(),(int) fTolerance,fCorrectionType->Data());			     
																					     
	canvas->SaveAs(fnameall);
	sumcanvas->SaveAs(fnamesum);
			
			
}  
//______________________________________________________________________________________
//
void QDiag::FitWholeScan()
{
	// This method calls reads, processes and fits the data in sequence for all the 
	// wavelengths, position types and scan directions.
	// The inverse attenuation lengths are saved in an ascii file.

	// Fixed parameters
	char *lDataDir 		  = GetDataDir();
	char *lDataSet 		  = GetDataSet();
	char *lCorrectionType = GetCorrectionType();
	Float_t lTolerance 	  = GetTolerance();

    // Variable Parameters range
	int lLambdaFeb[6] = {365,500,0,0,0,0};
	int lLambdaJan[6] = {337,365,500,0,0,0};
	int lLambdaNov[6] = {337,365,386,500,0,0};
	int lLambdaAll[6] = {337,365,386,420,500,620};  
	
	int nLambda,lLambda[6];
	if (!strcmp(lDataSet,"feb00")){
            nLambda = 2;
            for(int n = 0; n < 6; n++) lLambda[n] = lLambdaFeb[n];
        } else if (!strcmp(lDataSet,"jan01a") || !strcmp(lDataSet,"jan01b")){
            nLambda = 3;
            for(int n = 0; n < 6; n++) lLambda[n] = lLambdaJan[n];
        } else if (!strcmp(lDataSet,"nov01")){
            nLambda = 4;
            for(int n = 0; n < 6; n++) lLambda[n] = lLambdaNov[n];
        } else {
            nLambda = 6;
            for(int n = 0; n < 6; n++) lLambda[n] = lLambdaAll[n];
	}

	TString *lPosType[2];
	lPosType[0] = new TString("fruns");
	lPosType[1] = new TString("fits");

	TString *lScan[2];
	lScan[0] = new TString("xzscan");
	lScan[1] = new TString("xnzscan");
		
	
	
	int i,j,k;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			for (k = 0; k < nLambda; k++) {
                if (lLambda[k] == 500 && i == 1) continue;
				Initialize();
				SetDataDir(lDataDir);
				SetDataSet(lDataSet);
				SetLambda(lLambda[k]);
				SetCorrectionType(lCorrectionType);
				fPosType = lPosType[i];
				fScan    = lScan[j];
				SetTolerance(lTolerance);
				UpdateFileNames();
				
				ReadData();
				Process();
				FitRatio();
				FitProduct();

			}
		} 
	} 	
}

//______________________________________________________________________________________
//

void QDiag::SetDataDir(char *aDirectory)
{
  if (fDataDir) {
    delete fDataDir;
    fDataDir = new TString(aDirectory);
  } else {
    fDataDir = new TString(aDirectory);
  }
  UpdateFileNames(); 
}

//______________________________________________________________________________________
//

void QDiag::SetHomeDir(char *aDirectory)
{
  if (fHomeDir) {
    delete fHomeDir;
    fHomeDir = new TString(aDirectory);
  } else {
    fHomeDir = new TString(aDirectory);
  }
}

//______________________________________________________________________________________
//
void QDiag::SetDataSet(char *aString)
{
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
  UpdateFileNames();
}
//______________________________________________________________________________________
//

void QDiag::SetLambda(int aNumber)
{
  if (aNumber != 337 && aNumber != 365 && aNumber != 386 &&  aNumber != 420 &&
  aNumber != 500 && aNumber != 620 ) {
    printf("Wavelength %d not available. Choose 337, 365, 386, 420, 500 or 620\n", aNumber);
    printf("Setting Wavelength to 500 nm by default.\n");
	aNumber = 500;
  }
  fLambda = aNumber;
  UpdateFileNames();
}
//______________________________________________________________________________________
//

void QDiag::SetPosType(char *aString)
{
  if (strcmp(aString,"fruns") && strcmp(aString,"fits")) {
    printf("Source position type %s not available. Choose fruns or fits.\n",aString);
	printf("Setting Source position type to fruns by default \n");
	aString = "fruns";
  }
  if (fPosType) {
    delete fPosType;
    fPosType = new TString(aString);
  } else {
    fPosType = new TString(aString);
  } 
  UpdateFileNames();
}
//______________________________________________________________________________________
//

void QDiag::SetScan(char *aString)
{
  if (strcmp(aString,"xzscan") && strcmp(aString,"xnzscan")) {
    printf("Scan direction %s not available. Choose xzscan or xnzscan.\n",aString);
    printf("Setting scan direction to xzscan by default\n");
	aString = "xzscan";
  }
  if (fScan) {
    delete fScan;
    fScan = new TString(aString);
  } else {
    fScan = new TString(aString);
  } 
}
//______________________________________________________________________________________
//

void QDiag::SetCorrectionType(char *aString)
{
  if (strcmp(aString,"none") && strcmp(aString,"laser") && 
  		strcmp(aString,"pmt") && strcmp(aString,"all")) {
    printf("Occupancy Correction Type %s not available. Choose none, laser, pmt or all\n",aString);
    printf("Setting Occupancy Correction Type to none by default.\n");
	aString = "none";
  }
  if (fCorrectionType) {
    delete fCorrectionType;
    fCorrectionType = new TString(aString);
  } else {
    fCorrectionType = new TString(aString);
  } 
}
//______________________________________________________________________________________
//

void QDiag::SetTolerance(Float_t aNumber)
{
  fTolerance = aNumber;
}
//______________________________________________________________________________________
//

void QDiag::UpdateFileNames()
{
	// Filenames depend on the parameters, so thay should be updated everytime
	// they change, i.e., everytime a Set method for those parametters is called.
  if (fTreeFileName) {
    delete fTreeFileName;
    fTreeFileName = new TString(*fDataDir);
    ((((*fTreeFileName += "qpath/qpath_") += *fDataSet) += "_") += *fPosType);
    ((*fTreeFileName += "_") += fLambda) += ".root";
  } else {
    fTreeFileName = new TString(*fDataDir);
    ((((*fTreeFileName += "qpath/qpath_") += *fDataSet) += "_") += *fPosType);
    ((*fTreeFileName += "_") += fLambda) += ".root";
  } 
  if (fHistoFileName) {
    delete fHistoFileName;
    fHistoFileName = new TString(*fDataDir);
    ((((*fHistoFileName += "histo/histo_") += *fDataSet)+="_") += *fPosType);
    ((*fHistoFileName += "_") += fLambda) += ".root";
  } else {
    fHistoFileName = new TString(*fDataDir);
    ((((*fHistoFileName += "histo/histo_") += *fDataSet)+="_") += *fPosType);
    ((*fHistoFileName += "_") += fLambda) += ".root";
  } 
}
