#include "TVector3.h"
#include "TVector.h"
#include "QPMTxyz.h"
#include "QOptics.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TROOT.h"
#include "TProfile.h"
#include "TF1.h"
#include "TVectorF.h"

TROOT theRoot("theRoot","Example application");
#define USAGE "Usage: reflecttime [run number]<[rch filename]><[dqxx dir]>\n"
//
//	reflecttime.cxx
//	Uses the difference between 180 reflection and prompt time to fit laserball position
//	Inputs:
//	1) Run number
//	2) <Optional> Rch filename. If not supplied, will use rch/sno_xxxxx.rch
//	3) <DQXX dir> DQQXX directory. If not supplied, will use env. variable DQXXDIR
//
//	Notes:
//	Results are unreliable if position is off-center (more than 50 cm) or
//	runlength is small (less than ~1:30 h)
//
//	Revision history:
//	??/??/??			?? ?? 		Created
//	10/06/03			J. Maneira	Revised	
//										- use a simple peak-find algorithm to get the 
//									prompt and reflection time, instead of the bin with 
//									maximum number of counts 
//										- use QOptics to get the exact speed of light
//									for each wavelength
//										- supply rch filename as argument, in order to
//									support mcprod-style filenames
//										- dqxxdir is passed can be passed as an argument to 
//									avoid environment variables
//										- fit position can be written as a TVector3 to the
//									rch file (if filename is supplied)
int main(int argc, char **argv)
{
	Float_t width_prompt = 4;
	Float_t width_ref 	 = 6;
	Float_t start_prompt =  0;
	Float_t delta_ref    = 74;
	Float_t r_psup		 = 840;
	
	Double_t peakfind(TH1F *tchan,Double_t xPeak = 0,Double_t xWidthNormal = 4);

	int		irun = 99999;
	int 	run;
	char 	fname[1024],dqxxdir[1024],dqxxfile[1024];
	
	if(argc < 2 || argc > 4){
  		printf(USAGE);
    	exit(-1);
	} else {		
		irun = atoi(argv[1]);
		if (argc == 2) {
			if (irun>0) sprintf(fname,"rch/sno_%d.rch",irun);  
  			else sprintf(fname,"rch/snomc_%d.rch",abs(irun));  // Negative run numbers for MC
		} else {
  			strcpy(fname,argv[2]);
		}
		if (argc == 4) sprintf(dqxxdir,"%s",argv[3]);
		else {
			Char_t* temp = getenv("DQXXDIR");
			if (temp==NULL) {
    			fprintf(stderr,"Needs DQXX Banks to work properly.\n");
    			fprintf(stderr,"Set environment variable DQXXDIR or suplly it as an argument.\n");
    			fprintf(stderr,"Aborting reflecttime.\n");
    			exit(-1);
			} else sprintf(dqxxdir,"%s",temp);
		}
	}
	
	
 	TFile *f=new TFile(fname,"UPDATE");
  	if (!f->IsOpen()) {
    	printf("Can't open file: %s\n",fname);
    	exit(-1);
  	}

	printf("REFLECTIME:\tRunning reflection time position fit on run %d\n",irun);
  	Double_t	lambda = (Double_t) (*(TVector*) f->Get("lambda"))(1);
  	QOptics qo;
  	Double_t vgroup  =  qo.GetVgroupD2O(lambda);
  	printf("REFLECTIME:\tWavelength %3.0f nm - Group Velocity %3.3f PSUP radius %3.0f\n",lambda,vgroup,r_psup);

  	TH2F *time;
  	if (irun>0) {
    	time = (TH2F *) f->Get("fRchTime");  // Use full RCH histogram for data (0.25ns bins)
		run = irun;
	} else {
    	time = (TH2F *) f->Get("fRchTimeW");  // Use wide timing window for MC (2ns bins)
		run = abs(irun);
    	sprintf(dqxxdir,"rch");
	}
  		
	sprintf(dqxxfile,"%s/dqxx_%.10d.dat",dqxxdir,run);
	FILE *test = fopen(dqxxfile,"r");
	if (!test){
      	sprintf(dqxxfile,"%s/DQXX_%.10d.dat",dqxxdir,run);
		FILE *test2 = fopen(dqxxfile,"r");
		if (!test2){
			printf("ERROR:\t dqxx file not found - %s\n",dqxxfile);
			exit(0);
		} else {
			fclose(test2);
		}
	} else {
		fclose(test);
	}	
	
 	if (!(gSNO->GetPMTxyz())) {  // Set up global pointer gPMTxyz
    	printf("REFLECTIME:\tUnable to locate PMT's from gPMTxyz in tdiff\n");
    	exit(-1);
  	}
	
    gPMTxyz->ReadDQXX(dqxxfile);


 //====================

  TAxis *axis;
  Char_t hname[1024];

  TH1D *channel;
  sprintf(hname,"rx%d",abs(irun));
  TProfile rx(hname,"X Profile", 170,-850, 850);

  sprintf(hname,"ry%d",abs(irun));
  TProfile ry(hname,"Y Profile", 170, -850,850);

  sprintf(hname,"rz%d",abs(irun));
  TProfile rz(hname, "Z Profile", 170, -850, 850); 
  
  sprintf(hname,"pmtn%d",abs(irun));
  TH1D pmtn(hname, "PMTN", 10000, 0, 10000);   
   
  float x,y,z;
  Double_t t_p, t_r;
  for(int i=0; i<10000; i++){
    if (gPMTxyz->IsNormalPMT(i) && gPMTxyz->IsTubeOn(i)) {
      channel = time->ProjectionX("pxx",i+1,i+1);
      axis = channel->GetXaxis();

	  x = (Axis_t)gPMTxyz->GetX(i);
	  y = (Axis_t)gPMTxyz->GetY(i);
	  z = (Axis_t)gPMTxyz->GetZ(i);

 	  //____________________________
 	  // Prompt time
	  t_p = peakfind((TH1F*)channel,start_prompt,width_prompt);
	  
	  //____________________________
 	  // Reflected time
	  t_r = peakfind((TH1F*)channel,t_p + delta_ref,width_ref);
	  
	  delete channel;
	  
	  // If reflected peak statistics are too low, due to asymmetric LB,
	  // for instance, peakfind will mistakenly identify other features 
	  // at lower times. For source positions well within 1 m from the
	  // center, a simple cut should be sufficient.
	  if ((t_r - t_p) < 64 ) continue; 
	  rx.Fill(x,t_r - t_p);
      ry.Fill(y,t_r - t_p);
      rz.Fill(z,t_r - t_p);
	  
	  pmtn.Fill(i,t_r - t_p);



    }
  }

  //--------------------
  // Print and save results
  TF1 f1("f1","pol1",-850,850);


  f1.SetRange(-850,850);
  rx.Fit("f1","nr");
  float dx = r_psup*vgroup/2.0*f1.GetParameter(1);

  f1.SetRange(-850,850);
  ry.Fit("f1","nr");
  float dy= r_psup*vgroup/2.0*f1.GetParameter(1);

  f1.SetRange(-600,850);
  rz.Fit("f1","nr");
  float dz= r_psup*vgroup/2.0*f1.GetParameter(1);
  
  	TVector3 position(dx,dy,dz);

	if (argc ==2) {
  		printf ("REFLECTIME:\tfit position %f  %f %f\n", dx, dy, dz);

  		FILE *fout=fopen("reflecttime.txt","a");
  		fprintf(fout,"%d %f %f %f \n",irun,dx,dy,dz);
  		fclose(fout);

  		TFile frefl("reflecttime.root","update"); 
  		rx.Write();
  		ry.Write();
  		rz.Write();
		pmtn.Write();
  		frefl.Close();
	} else {
		f->cd();
  		printf ("REFLECTIME:\tfit position %f  %f %f\n", dx, dy, dz);
		position.Write("reflection_pos");
	}
	f->Close();
}

//////////////////////////////////

Double_t peakfind(TH1F *tchan,Double_t xPeak = 0,Double_t xWidthNormal = 4){

	TAxis *axis=tchan->GetXaxis();													  
    Int_t ichan;  																	  
    Double_t w, x, sumw, sumw2, sumwx, sumwx2,width,time,counts;  										
    Double_t xWidth = xWidthNormal; 													
        																				
    Bool_t done = kFALSE;																

    // Find the prompt peak (when centroid in window doesn't change any more)			
    // and integrate the prompt peak for the Counts histogram							
    do {																				
    	for(sumw=sumw2=sumwx=sumwx2=0, ichan = axis->FindBin(xPeak-xWidth); 			  
    			ichan < axis->FindBin(xPeak+xWidth);											  
    			ichan++) {  																	  
      		x = axis->GetBinCenter(ichan);													  
     		w = tchan->GetBinContent(ichan);  												  
      		sumw += w;																		  
      		sumw2 += w*w; 																	  
      		sumwx +=w*x;  																	  
      		sumwx2 += w*x*x;  																  
    	}																				  
		if(sumw !=0) {  																  
      		time=sumwx/sumw;  																  
      		counts=sumw;  																	  
      		width=sqrt(fabs(sumwx2/sumw -time*time)); 										  
      		if(fabs(time-xPeak) < 0.05)	done = kTRUE;																	  
      		else {																			  
   				done=kFALSE;																	  
    			xPeak=time; 																	  
     		} 																				  
    	} else {																		  
      		counts=width=time=0; done=kTRUE;  												  
    	}																				  
	} while (!done); 
	return  time;		
}														  
