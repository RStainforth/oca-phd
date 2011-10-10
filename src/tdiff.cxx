#include <stdio.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TVector3.h>
#include <TVectorF.h>
#include <TVector.h>
#include <TAxis.h>
#include <string.h>
#include <QPMTxyz.h>
#include "QOptics.h"

////////////////////////////////////////////////////////////////
//                                                            //
// tdiff                                                      //
//                                                            //
// A legendary executable that does everything we need.       //
//                                                            //
// - Produces a simplified version of the RCH file            //
//   called RDT files (smaller in size).                      //
// - Finds peaks, counts, widths.                             //
// - See USAGE for arguments.                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#define USAGE "Usage: Either\ntdiff [MC=1] [rch filename] [rdt filename]\n or\ntdiff [DATA=0] [run number]\n or\ntdiff -f [run] [rch filename] [rdt filename]\n\n"

int main(int argc,char **argv)
{

	Char_t infile[1024];
	Char_t outfile[1024];
	Int_t run;
	Int_t runMonteCarlo = kFALSE;  // Set to kTRUE when dealing with MC run files

	if(argc<3) {
		printf(USAGE);
		exit(-1);
	} else {
		if (argv[1][0]=='-') {
			if (argv[1][1]!='f') { printf(USAGE); exit(-1); }
			if (strlen(argv[1])!=2) { printf(USAGE); exit(-1); }
			run = atoi(argv[2]);
			if(run <= 0) printf("Warning in tdiff: Invalid run number: %d\n",run);
			strcpy(infile,argv[3]);
			strcpy(outfile,argv[4]);
			if(strlen(infile)<4) printf("Warning in tdiff: Invalid rch filename.\n");
			if(strlen(outfile)<4) printf("Warning in tdiff: Invalid rdt filename.\n");
			printf("Using filename arguments.\n"); 
			printf("Run %d from %s to %s\n",run,infile,outfile);
    		} else {
      			if (argv[1][0]!='0' && argv[1][0]!='1') { printf(USAGE); exit(-1); }
      			sscanf(argv[1],"%d",&runMonteCarlo);
    
      			if (runMonteCarlo!=0) runMonteCarlo = kTRUE;
      			else runMonteCarlo = kFALSE;
    
// if (runMonteCarlo) {
//	sscanf(argv[2],"%d",&run);
//	sprintf(infile,"rch/snomc_%d.rch",run);
//	sprintf(outfile,"rdt/snomc_%d.rdt",run);
//	printf("Using Monte Carlo, from %s to %s\n",infile,outfile);
// } else {

			// 2006.04 - O.Simard
			// Allow the MC rch and rdt files to have any name schemes
			// that may or may not contain the run number in it.
      			if (runMonteCarlo) {
				sscanf(argv[2],"%s",infile);
				sscanf(argv[3],"%s",outfile);
				if(strlen(infile)<4) printf("Warning in tdiff: Invalid rch filename.\n");
				if(strlen(outfile)<4) printf("Warning in tdiff: Invalid rdt filename.\n");
				printf("Using Monte Carlo, from %s to %s\n",infile,outfile);
      			} else {
				sscanf(argv[2],"%d",&run);
				sprintf(infile,"rch/sno_%d.rch",run);
				sprintf(outfile,"rdt/sno_%d.rdt",run);
				printf("Using Data, from %s to %s\n",infile,outfile);
     			}
		}
	}

	//
	// If passed this point, tdiff should run properly. Enjoy.
	//
	//
	//

    Double_t lx,ly,lz; // this will be the source position
    
    TFile f1(infile,"READ");															  
    if (!f1.IsOpen()) { 																  
      printf("Couldn't open input file %s\n",infile);									  
      exit(-1); 																		  
    }																					  
    TH2F* tac2      = (TH2F *) f1.Get("fRchTime");  											  
    TH1F *rchtof    = (TH1F *) f1.Get("fRchToF");									  

    TH2F *tac2dir   = (TH2F *) f1.Get("fRchTDirect"); 									  
    TH2F *tac2noref = (TH2F *) f1.Get("fRchTNoNCDRef");									  
    TH2F *tac2ref   = (TH2F *) f1.Get("fRchTNCDRef");									  
    TH2F *tac2ray   = (TH2F *) f1.Get("fRchTRayleigh");									  

// Bool_t fMCtac2;  // Flag whether MC data read in or not 							  
// if (tac2dir && tac2ref && tac2ray && tac2noref) fMCtac2 = kTRUE;								  
// else fMCtac2 = kFALSE;  															  
    //--------------------  															  
    																					  
	TVector wavelength(2);
	if  ((TVector *)f1.Get("lambda")) {
		wavelength(0) = (*((TVector*)f1.Get("lambda")))(0);										  
 		wavelength(1) = (*((TVector*)f1.Get("lambda")))(1);										  
	} else {

		// I don't know who made this change but it should have
		// been explained why.

//		wavelength(0) = 4;
//		wavelength(1) = 500;
		wavelength(0) = 3;
		wavelength(1) = 420;
	}
	TVector *sp=(TVector *)f1.Get("source_pos");										  
 	TVector3 *ref=(TVector3 *)f1.Get("reflection_pos");										  
   	TVector *cast=(TVector *)f1.Get("cast");								

	// 2006.04 - O.Simard
	// Some older versions of the rch files do not even have 11 elements
	// stored in the cast vector. Check dimension before loading the
	// orientation.	  
	TVector orientation(1);
	if(cast->GetNoElements() > 11){
		if ((*cast)(11) > 0 && (*cast)(11) <= 4){
			orientation(0) = (*cast)(11);
		} else {
			orientation(0) = 0;
		}
	} else {
		printf("Warning: old rch files don't store the slot orientation.\n");
		orientation(0) = 0;
	}

	TVector *fRSPS=(TVector *)f1.Get("RSPS");
	
	// 2006.04 - O.Simard
	// Make basic check on the RSPS vector.
	// Ignore the rsps when the MC flag is on: that enables to
	// run MC without rsp and/or identical run conditions.
	printf("----------------------------------\n");
	printf("RSPS Checks:\n");
	Bool_t fIgnore_rsps = kFALSE; // use rsps by default
	Int_t nbad_rsps = 0;
	for(Int_t i = 0 ; i < fRSPS->GetNoElements(); i++){
	  if((*fRSPS)(i) == 0) nbad_rsps++;
	}
	printf("\t%d PMTs flagged by RSPS.\n",nbad_rsps);

	if(nbad_rsps >= 9728){ // hardcoded number of channels
	  printf("\t-> All PMTs flagged by RSPS.\n");
	  if(runMonteCarlo){
		  printf("\t-> Ignoring RSPS for Monte Carlo: Fine.\n");
		  fIgnore_rsps = kTRUE;
	  } else { // tdiff won't produce empty rdt files for data
		  printf("\t-> Not ignoring RSPS for Data:\n");
		  printf("\t-> tdiff will stop now ...\n");
		  exit(-1);
	  }
	} else {
		if(nbad_rsps < 2500) printf("\t-> This pass will be great.\n");
		else if(nbad_rsps < 5000) printf("\t-> This pass will be ok.\n");
		else printf("\t-> This pass will be garbage.\n");
	}
	// Then fill the RSPS vector with fake stuff if and only if
	// both the MC and ignore bits were turned on.
	if(runMonteCarlo && fIgnore_rsps){
	  for(Int_t i = 0 ; i < fRSPS->GetNoElements(); i++) (*fRSPS)(i) = 1;
	}
	printf("----------------------------------\n");

									  
    lx=(*sp)(0); ly=(*sp)(1); lz=(*sp)(2);  											  
    printf("Source position: %f %f %f\n", lx,ly,lz);									  
    TVector3 source(lx,ly,lz);  														  
    Int_t i,j; 
	
    Double_t  lt, pmtx, pmty, pmtz, tflight;											  
    Double_t zcoord;  // 'z' coordinate: distance along source radial vector			  
    Double_t ctheta,phi;																  
    Double_t actheta, aphi;   // acrylic coordinates									  
    Double_t lbctheta, lbphi; // laserball coordinates  								  
    Double_t c_d2o = 22.45;   // corresponds to n=1.33630								  
    																					  
    TFile fout(outfile,"RECREATE"); 													  
    if (!fout.IsOpen()) {																  
      printf("Couldn't open output file %s\n",outfile); 								  
      exit(-1); 																		  
    }																					  
    TH1D *tchan;																		  
    TH1F ttt;																			  
    TH1F tt("Peak","Time Peak",10000,0,10000);  										  
    TH1F tw("Width","Time Width",10000,0,10000);										  

    // Counts in each channel, integrated over the prompt peak  						  
    TH1F tc("Counts","Occupancy",10000,0,10000);										  
    // Counts in each channel, integrated over prompt peak and following 50ns			  
    TH1F tcwide("CountsWide","Occupancy in Wide Window",10000,0,10000); 				  
    // Counts in each channel, as a function of window width in ns  					  
    TH2F tcwindow("CountsWindow","Counts and Window Width",10000,0,10000,26,0,26);  	  
    TH2F tcwindowz("CountsWindowZ","Counts and Window Width vs PMTZ",					  
    	   170,-850,850,26,0,26);														  
    TH2F tcwindowznorm("CountsWindowZnorm","Number of PMTs normalization vs PMTZ",  	  
    	   170,-850,850,26,0,26);														  

    //--------------------  															  
    TH1F ttdir("PeakDir","Time Peak - Direct",10000,0,10000);							  
    TH1F twdir("WidthDir","Time Width - Direct",10000,0,10000); 						  
    TH1F tcdir("CountsDir","Occupancy - Direct",10000,0,10000); 						  
    TH1F tcdirtot("CountsDirTot","Occupancy - Direct - Total",10000,0,10000);			  

    TH1F ttnoref("PeakNoNCDRef","Time Peak - NoNCDRef",10000,0,10000);							  
    TH1F twnoref("WidthNoNCDRef","Time Width - NoNCDRef",10000,0,10000); 						  
    TH1F tcnoref("CountsNoNCDRef","Occupancy - NoNCDRef",10000,0,10000); 						  
    TH1F tcnoreftot("CountsNoNCDRefTot","Occupancy - NoNCDRef - Total",10000,0,10000);			  

    TH1F ttref("PeakRef","Time Peak - NCD Reflect",10000,0,10000);  						  
    TH1F twref("WidthRef","Time Width - NCD Reflect",10000,0,10000);						  
    TH1F tcref("CountsRef","Occupancy - NCD Reflect",10000,0,10000);						  
    TH1F tcreftot("CountsRefTot","Occupancy - NCD Reflect - Total",10000,0,10000);  		  

    TH1F ttray("PeakRay","Time Peak - Rayleigh",10000,0,10000); 						  
    TH1F twray("WidthRay","Time Width - Rayleigh",10000,0,10000);						  
    TH1F tcray("CountsRay","Occupancy - Rayleigh",10000,0,10000);						  
    TH1F tcraytot("CountsRayTot","Occupancy - Rayleigh - Total",10000,0,10000); 		  
    //--------------------  															  

	TH1F nbins("nbins","Number of Filled bins in peak search",10000,0,10000);
    TH1F tof("tof","Time of Flight",10000,0,10000); 									  
    TH2F ttof("ttof","Time vs Tof",800,-100,300,55,10,65);  							  
    // Someone changed the number of bins and ranges of the three
    // following histograms.

    TH2F tz("tz", "Time vs 'z' (source radial axis coordinate)",						  
    	800,-100,300,150,-900,900);													  
    TH2F cz("cz", "Occupancy vs 'z' (source radial axis coordinate",					  
    	10000,0,10000,150,-900,900);													  
    TH2F cznopipe("cznopipe","Occupancy vs 'z', No water pipe", 						  
    	  10000,0,10000,150,-900,900); 												  
    TH2F pt("pt","Theta vs Phi",20,-180,180,20,-1,1);									  
    TH2F pt2("pt2","Theta vs Phi",20,-180,180,20,-1,1); 								  
    TH2F apt("apt","Acrylic Theta vs Phi",20,-180,180,20,-1,1); 						  
    TH2F lbpt("lbpt","Laserball Theta vs Phi",20,-180,180,20,-1,1); 					  
    TH2F lbnp("lbnp","Npmts Theta vs Phi",20,-180,180,20,-1,1); 						  
    																	
	TH3F nbinswid("nbinswid","Filled bins vs Width vs peak",100,-5,5,100,0,4,32,1,33);
					  
    TAxis *channelaxis=tac2->GetYaxis();												  

    //  QMath QMath;																	  

    if (!(gSNO->GetPMTxyz())) {  // Set up global pointer gPMTxyz						  
      printf("Unable to locate PMT's from gPMTxyz in tdiff\n"); 						  
      exit(-1); 																		  
    }																					  

    QOptics *path = new QOptics(lx,ly,lz);  											  
    path->SetIndices(500);

    printf("----------------------------------\n"); 															  

    Double_t time, counts, width;														  
    lt=0;																				  
    // 2006.04 - O.Simard
    // Changed the if() arguments at the beginning of the loop
    // to consider running with rsps off for MC data.								  
    for (i=0; i < 10000; i++) { 														  
      if (!gPMTxyz->IsInvalidPMT(i) && gPMTxyz->IsNormalPMT(i) && ((*fRSPS)(i) || fIgnore_rsps)) {	

      } else continue;  																  
      pmtx = gPMTxyz->GetX(i);  														  
      pmty = gPMTxyz->GetY(i);  														  
      pmtz = gPMTxyz->GetZ(i);  														  
      TVector3 pmt(pmtx,pmty,pmtz); 													  

      // zcoord lets runs off the z-axis still be histogrammed along an axis			  
      // of spherical symmetry (the 'z'-axis...)						
      // zcoord = pmt * source.Unit();  // 'z' distance along source radial vector.
      zcoord = pmt.Z();// * source.Unit();  // 'z' distance along source radial vector.  

      ctheta=pmt.CosTheta();															  
      phi=pmt.Phi()*180/M_PI;															  

      Double_t d,a,h,c,trans;															  
      Int_t nn; 																		  

      path->GetDistances(i,d,a,h,c,nn,trans);											  
      TVector3 avs=path->GetAcrylic();  												  

      actheta=avs.CosTheta();															  
      aphi=avs.Phi()*180/M_PI;  														  
      TVector3 lb=pmt-source;															  
      lbctheta=lb.CosTheta();															  
      lbphi=lb.Phi()*180/M_PI;  														  
    																					  
      tflight = (sqrt((pmtx-lx)*(pmtx-lx)+  											  
    		  (pmty-ly)*(pmty-ly)+  													  
    		  (pmtz-lz)*(pmtz-lz))-lt) / c_d2o; 										  
    																					  
      j=channelaxis->FindBin(i);														  
      tchan=tac2->ProjectionX("tac2_px",j,j);											  
      Int_t n;  Double_t t; 															  
      for(j=0; j<tchan->GetNbinsX();j++){												  
    	n=(Int_t)tchan->GetBinContent(j);												  
    	t=tchan->GetBinCenter(j);														  
    	ttof.Fill(t,tflight,n); 														  
    	tz.Fill(t,zcoord,n);															  
      } 																				  
    																					  
      TAxis *axis=tchan->GetXaxis();													  
      Int_t ichan;  																	  
      Double_t w, x, sumw, sumw2, sumwx, sumwx2;										  
      Double_t xPeak = 0;																  
      Double_t xWidthNormal = 4; // Half width of prompt peak timing integral (ns)  	  
      Double_t xWidth = xWidthNormal;													  
      Double_t xWidthWide = 50;  // Late side of wide timing integral (ns)  			  
    																					  
      Bool_t done = kFALSE; 															  

      // Find the prompt peak (when centroid in window doesn't change any more) 		  
      // and integrate the prompt peak for the Counts histogram 
	  
	  //jm nov06 // find the number of filled: is it always 32?
	  Int_t nfilledbins;						  
      do {  																			  
    	for(nfilledbins=sumw=sumw2=sumwx=sumwx2=0, ichan = axis->FindBin(xPeak-xWidth); 			  
    	ichan < axis->FindBin(xPeak+xWidth);											  
    	ichan++) {  																	  
      x = axis->GetBinCenter(ichan);													  
      w = tchan->GetBinContent(ichan);  												  
      sumw += w;																		  
      sumw2 += w*w; 																	  
      sumwx +=w*x;  																	  
      sumwx2 += w*x*x;  
	  //jm nov06 // find the number of filled: is it always 32?
	  if(w>0) nfilledbins++;																  
    	}																				  

    	if(sumw !=0) {  																  
      time=sumwx/sumw;  																  
      counts=sumw;  																	  
      width=sqrt(fabs(sumwx2/sumw -time*time)); 										  
      if(fabs(time-xPeak) < 0.05)														  
    	done = kTRUE;																	  
      else {																			  
    	done=kFALSE;																	  
    	xPeak=time; 																	  
      } 																				  
    	} else {																		  
      counts=width=time=0; done=kTRUE;  												  
    	}																				  
      } while (!done);  																  
      tc.Fill(i,counts);																  
	  //jm nov06 // find the number of filled: is it always 32?
	  //if(nfilledbins != 32) printf("nfilledbins %d\n",nfilledbins);
	  nbins.Fill(i,nfilledbins);
	  nbinswid.Fill(time,width,nfilledbins);

      // 2006.04 - O.Simard
      // Changed the print statements so that the half-width
      // and integral counts are printed with less digits.
      if (i%500 == 0) { 																  
    	printf("Channel %d Start time %f Peak %f Stop time %f Half width %.2f\n", 		  
    	   i,xPeak-xWidth,xPeak,xPeak+xWidth,xWidth);									  
      } 																				  

      if ( i %500 == 0 )																  
    	printf("Channel %i Time %f Width %f Counts %.2f\n",i,time,width,counts);  		  
    																					  
      tt.Fill(i,time);  																  
      tw.Fill(i,width); 																  
      tof.Fill(i,tflight);  															  
      pt.Fill(phi,ctheta,counts);														  
      apt.Fill(aphi, actheta,counts);													  
      lbpt.Fill(lbphi,lbctheta,counts); 												  
    																					  
      if (counts>500) lbnp.Fill((Axis_t)lbphi,(Axis_t)lbctheta);						  
    																					  
      phi= phi<0?180+phi:phi-180;														  
      pt2.Fill(phi,ctheta,counts);  													  
      cz.Fill(i,zcoord,counts); 														  

      // Fill if not on line down +x great circle, nor if tube is right at the bottom.    
      if (fabs(pmtx)>50 && sqrt(pmtx*pmtx+pmty*pmty)>200)								  
    	cznopipe.Fill(i,zcoord,counts); 												  
    																					  
      //--------------------															  
      // Sum entries on the wide window for CountsWide histogram						  
      for(sumw=0, ichan=axis->FindBin(xPeak-xWidth);									  
      ichan<axis->FindBin(xPeak+xWidthWide);											  
      ichan++) {																		  
    	w= tchan->GetBinContent(ichan); 												  
    	sumw += w;  																	  
      } 																				  
      tcwide.Fill(i,sumw);  															  

      // Sum up entries for varying window width histogram CountsWindow 				  
      for (xWidth = 1; xWidth < 25; xWidth++) { 										  
    	for(sumw=sumw2=sumwx=sumwx2=0, ichan=axis->FindBin(xPeak-xWidth);				  
    	ichan<axis->FindBin(xPeak+xWidth);  											  
    	ichan++) {  																	  
      x=axis->GetBinCenter(ichan);  													  
      w= tchan->GetBinContent(ichan);													  
      sumw += w;																		  
      //	  sumw2 += w*w; 															  
      //	  sumwx +=w*x;  															  
      //	  sumwx2 += w*x*x;  														  
    	}																				  
    	tcwindow.Fill(i,xWidth,sumw);													  
    	tcwindowz.Fill(zcoord,xWidth,sumw); 											  
    	tcwindowznorm.Fill(zcoord,xWidth,1);											  
      } 																				  

      delete tchan; 																	  

      //--------------------															  
      // New 25-Oct-2000 - Bryce Moffat 												  
      // Add support for Direct, Reflect and Rayleigh information from SNOMAN			  
      //																				  
      // Sum entries on Direct, Reflected and Rayleigh Scattered over same window:  	  

      xWidth = xWidthNormal;															  
      if (tac2dir) {																	  
    	// Direct																		  
    	j=channelaxis->FindBin(i);  													  
    	tchan = tac2dir->ProjectionX("tac2dir_px",j,j); 								  
    	axis = tchan->GetXaxis();														  
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
    	} else {																		  
      		counts=width=time=0;  															  
    	}																				  
    	ttdir.Fill(i,time); 															  
    	twdir.Fill(i,width);															  
    	tcdir.Fill(i,counts);															  

    	tcdirtot.Fill(i,tchan->Integral(0,tchan->GetNbinsX()+1));						  
    	if ( i %500 == 0 ) printf("Channel %i Time %f Width %f Counts %f - Direct\n",	  
    			  i,time,width,counts); 												  
    	delete tchan;
      }

      if (tac2noref) {																	  
    	// No NCD Reflection																		  
    	j=channelaxis->FindBin(i);  													  
    	tchan = tac2noref->ProjectionX("tac2dir_px",j,j); 								  
    	axis = tchan->GetXaxis();														  
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
    	} else {																		  
      		counts=width=time=0;  															  
    	}																				  
    	ttnoref.Fill(i,time); 															  
    	twnoref.Fill(i,width);															  
    	tcnoref.Fill(i,counts);															  

    	tcnoreftot.Fill(i,tchan->Integral(0,tchan->GetNbinsX()+1));						  
    	if ( i %500 == 0 ) printf("Channel %i Time %f Width %f Counts %f - No NCD Reflection\n",	  
    			  i,time,width,counts); 												  
    	delete tchan;																	  
      }
      
      if (tac2ref) {																				  
    	// Reflected																	  
    	j=channelaxis->FindBin(i);  													  
    	tchan = tac2ref->ProjectionX("tac2ref_px",j,j); 								  
    	axis = tchan->GetXaxis();														  
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
    	} else {																		  
      		counts=width=time=0;  															  
    	}																				  
    	ttref.Fill(i,time); 															  
    	twref.Fill(i,width);															  
    	tcref.Fill(i,counts);															  

    	tcreftot.Fill(i,tchan->Integral(0,tchan->GetNbinsX()+1));						  
    	if ( i %500 == 0 ) printf("Channel %i Time %f Width %f Counts %f - Reflected\n",  
    			  i,time,width,counts); 												  
    	delete tchan;															  
      }
      
      if (tac2ray) {
    	// Rayleigh scattered															  
    	j=channelaxis->FindBin(i);  													  
    	tchan = tac2ray->ProjectionX("tac2ray_px",j,j); 								  
    	axis = tchan->GetXaxis();														  
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
    	} else {																		  
      		counts=width=time=0;  															  
    	}																				  
    	ttray.Fill(i,time); 															  
    	twray.Fill(i,width);															  
    	tcray.Fill(i,counts);															  

    	tcraytot.Fill(i,tchan->Integral(0,tchan->GetNbinsX()+1));						  
    	if ( i %500 == 0 ) printf("Channel %i Time %f Width %f Counts %f - Rayleigh\n",   
    			  i,time,width,counts); 	
    	delete tchan;						
      } 									
    	//--------------------  			
    }										
    										
    tc.SetEntries(tac2->GetEntries());    // This is the number of laser pulses.

    fout.cd();  										
    source.Write("manip_pos");							
    if (ref) ref->Write("reflection_pos");				
    wavelength.Write("lambda");							
    orientation.Write("orientation");
	fRSPS->Write("RSPS");								
    tt.Write(); 										
    tw.Write(); 										
    tc.Write(); 
	if (rchtof) rchtof->Write("fRchToF");				

    //--------------------  							
    // 25-Oct-2000 - Bryce Moffat						
    // New for Direct, Reflected and Rayleigh from SNOMAN
    if (tac2dir) {										
      tcdir.SetEntries(tac2->GetEntries());				
      tcdirtot.SetEntries(tac2->GetEntries());			
      ttdir.Write();		
      twdir.Write();		
      tcdir.Write();		
      tcdirtot.Write(); 	
    }
	if (tac2noref) {		
      tcnoref.SetEntries(tac2->GetEntries());				
      tcnoreftot.SetEntries(tac2->GetEntries());			
      ttnoref.Write();		
      twnoref.Write();		
      tcnoref.Write();		
      tcnoreftot.Write(); 	
    }
	if (tac2ref) {																				  
      tcref.SetEntries(tac2->GetEntries());				
      tcreftot.SetEntries(tac2->GetEntries());			
      ttref.Write();		
      twref.Write();		
      tcref.Write();		
      tcref.Write();		
      tcreftot.Write(); 	
    }
      
    if (tac2ray) {
      tcray.SetEntries(tac2->GetEntries());				
      tcraytot.SetEntries(tac2->GetEntries());			
      ttray.Write();		
      twray.Write();		
      tcray.Write();		
      tcraytot.Write(); 	
    }						
    //--------------------  

    tcwide.Write(); 		
    tcwindow.Write();		
    tcwindowz.Write();  	
    tcwindowznorm.Write();  
    tof.Write();			
    ttof.Write();			
    tz.Write(); 			
    cz.Write(); 			

    cznopipe.Write();		

    pt.Write(); 			
    pt2.Write();			
    apt.Write();			
    lbpt.Write();			
    lbnp.Write();		
	
	nbins.Write();
	nbinswid.Write();
		
    fout.Close();			
}
