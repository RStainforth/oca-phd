#include "QOCATree.h"
#include "QPmtFit.h"
#include "QOptics.h"
#include "QPMTuvw.h"
#include "TEventList.h"
#include "TRandom.h"
#include "TMath.h"

//_____________________________________________________________________
//
// QPmtFit is a class that is made in order to analyze an entire laserball
// scan.  It contains routines that allow one to fit for each PMT position 
// and time offset, based on the laserball positions and the time data.
// 
// Reports based on this class are:
// http://sno.phy.queensu.ca/~hallin/private/PMT%20Cabling.htm
// http://sno.phy.queensu.ca/~hallin/private/Misplaced%20PMTs%20II.htm
// http://sno.phy.queensu.ca/~hallin/private/PCA%20Shifts%20and%20Timing.htm
//


ClassImp(QPmtFit)

QPmtFit::QPmtFit(QOCATree *tree):
  fDistance("Distance","Distance",1000,0,200),
  fChisq("Chisq","Chisquare",1000,0.0,1000.0),
  fPmts("Pmts","Pmt distances",10000,0,10000),
  fCutDistance("CutDistance","Fit-Nominal distance, for good chisquare",10000,0,10000),
  fAngle("Angles","Angle of rotation",10000,0,10000),
  fDevlb("Devlb","Deviation vs lb angle",200,-1,1,200,-0.5,0.5),
  fXdev("Xdeviation","Deviation in PMT X",2000,-200,200),
  fYdev("Ydeviation","Deviation in PMT Y",2000,-200,200),
  fZdev("Zdeviation","Deviation in PMT Z",2000,-200,200)

{
  // remove histograms from directories - this is necessary because these 
  //histograms are created and destroyed with the QPMTfit class- they need 
  //to belong to QPmtFit and not the TDirectory.
  fDistance.SetDirectory(0);  fChisq.SetDirectory(0); fPmts.SetDirectory(0);
  fCutDistance.SetDirectory(0); fAngle.SetDirectory(0); fDevlb.SetDirectory(0);
  fXdev.SetDirectory(0); fYdev.SetDirectory(0); fZdev.SetDirectory(0);

  TEventList *evl=tree->GetEventList(); // use the event list to impose run cuts
  // on the tree.  Runs not in the event list will be ignored.
  if(evl)fN=(int)evl->GetN(); else fN=(int)tree->GetEntries();
  fLBPositions=new TVector3[fN];
  fRunNumbers=new Int_t[fN];
  fX=new Float_t[fN];
  int i,j;
  for(i=0;i<10000;i++)fTimes[i]=new Float_t[fN];
  for(i=0;i<10000;i++)fSigma[i]=new Float_t[fN];
  for(i=0;i<10000;i++)fWidth[i]=new Float_t[fN];
  for(i=0;i<10000;i++)for(j=0;j<fN;j++){
    fTimes[i][j]=0; fWidth[i][j]=0;fSigma[i][j]=0;
  }


  // For testing QLBPositionFit with an incomplete qpath, I overrode
  // QFitxyz with manpos.  This needs to be changed for fitting PMT
  // positions.
  //  printf(" Version broken-- using manipulator positions! \n");

  QOCARun *run=tree->GetQOCARun();
  int ircount,ipcount,runindex;
  for(ircount=i=0;i<fN;i++){
    if(evl)runindex=evl->GetEntry(i); else runindex=i;
    tree->GetEntry(runindex);
    TVector3 zoffset(0,0,+2);//kludge to test devlb.  
    fLBPositions[ircount]=run->GetQFitxyz(); //+zoffset;
    fRunNumbers[ircount]=run->GetRun();
    TVector3 manpos=run->GetManipxyz();
    //    fLBPositions[ircount]=manpos; //this line needs to be removed for good qpath
    fX[ircount]=ircount;
    for(ipcount=j=0;j<run->GetNpmt();j++){
      QOCAPmt *pm=run->GetPMT(j);
      if(pm->GetBasicBad()||pm->GetOtherbad())continue;
      if(pm->GetNprompt()>100)ipcount++;
      float testTime;
      testTime=pm->GetTprompt()+pm->GetRchtof()-run->GetQt();
      if(isnan(testTime))testTime=0.0;
      fTimes[pm->GetPmtn()][ircount]=testTime; 
      //TVector3 pmtpos(pm->GetPmtx(),pm->GetPmty(),pm->GetPmtz());
      //float tflight=(pmtpos-manpos).Mag()/21.88;
      //fTimes[pm->GetPmtn()][ircount]=pm->GetTprompt()+tflight-run->GetQt();
      float w=pm->GetNprompt();
      if(w<100||pm->GetTwidth()<0.6){  //get rid of "nan's in fSigma & deweight low stat. runs
	fWidth[pm->GetPmtn()][ircount]=2.0;
	fSigma[pm->GetPmtn()][ircount]=1.0;
      }else{
	fWidth[pm->GetPmtn()][ircount]=pm->GetTwidth();
	fSigma[pm->GetPmtn()][ircount]=pm->GetTwidth()/sqrt(w);
      }
    }
    if(ipcount>5000)ircount++;
  }
  //  We need to deal with instances where PMTs don't appear in a subset of 
  //runs.  We do this through setting Sigma for those to a huge value
  //.  Thus, these events will never contribute to chisquare.

  for(i=0;i<10000;i++)for(j=0;j<ircount;j++)if(fTimes[i][j]==0)fSigma[i][j]=1e10;

  fN=ircount;
  fNparameters=4;
  fCovar = matrix(1,fNparameters,1,fNparameters);
  fAlpha = matrix(1,fNparameters,1,fNparameters);
  for(i=0;i<fNparameters;i++)fIa[i]=1;  //vary all parameters
  fIa[3]=0;
  fParameters[3]=0;
  QOptics optics;
  float vgroupd2o = optics.GetVgroupD2O(run->GetLambda());
  float vgroupacr = optics.GetVgroupAcrylic(run->GetLambda());
  float vgrouph2o = optics.GetVgroupH2O(run->GetLambda());
  float vgroupmean = (vgroupd2o*600 + vgroupacr*5.5 + vgrouph2o*245) / 850.5;
  fDtDx=1.0/vgroupmean;
  fChisq.GetXaxis()->SetTitle("Chisquare");
  fChisq.GetYaxis()->SetTitle("Number of PMTs");
  fDistance.GetXaxis()->SetTitle("Distance between fit and nominal, cm");
  fDistance.GetYaxis()->SetTitle("Number of PMTs");

}
    

QPmtFit::~QPmtFit(){
  delete[] fLBPositions; 
  int i;
  for(i=0;i<10000;i++)delete[] fTimes[i];
  for(i=0;i<10000;i++)delete[] fSigma[i];
  for(i=0;i<10000;i++)delete[] fWidth[i];
  delete[] fX;
  delete[] fRunNumbers;
  free_matrix(fCovar,1,fNparameters,1,fNparameters);
  free_matrix(fAlpha,1,fNparameters,1,fNparameters);

}

void QPmtFit::GetData(Int_t Run1, Int_t Run2,float *x, float *t, float *sigma, Int_t &n){
  if(Run1<0 || Run1>=fN || Run2<0 || Run2 >=fN){
    printf(" Invalid arguments to QPmtFit::GetData %d %d \n",Run1,Run2);
    return;
  }
  int i,j;
  for(j=i=0;i<10000;i++){
    if(fTimes[i][Run1]!=0.0 && fTimes[i][Run2]!=0){
      x[j]=i; t[j]=fTimes[i][Run2]-fTimes[i][Run1];
      sigma[j]=sqrt(fSigma[i][Run2]*fSigma[i][Run2]+fSigma[i][Run1]*fSigma[i][Run1]);
      if(fabs(sigma[j])<0.02){
	printf("sigma[j]=%f\n",sigma[j]);
      }
	j++;
    }
  }
  n=j-1;
  return;
}


void QPmtFit::CheckPositions(){
  // fit X,Y,Z of each pmt in the array.  Check to see if 
  // it is within 25 cm of the nominal position.  Print all tubes 
  // that are not.  Print summary of statistics
  QPMTuvw uvw; 
  TVector3 fitpos[1000],pmtpos[1000];
  int status[10000];
  FILE *fout=fopen("qpmtfit.txt","w");
  int index[1000];
  float chisq;
  int i;
  for(i=0;i<fNparameters;i++)fIa[i]=1;  //vary all parameters
  fIa[3]=0;
  fParameters[3]=0;  // don't vary time offset for this test.
  int ttested=0;  // number of tubes checked
  int tgood=0;
  int tbad=0; 
  for(i=0;i<10000;i++){
    int izero;
    int j;
    status[i]=0;
    for(izero=j=0;j<fN;j++)if(fTimes[i][j]>1&&fWidth[i][j]>0)izero++;
    TRandom trand;
    if(izero>fN*0.75){
      status[i]=1;
      ttested++;
      int goodfits;
      float minchi;
      TVector3 bestpos(gPMTxyz->GetX(i),gPMTxyz->GetY(i), gPMTxyz->GetZ(i));
      fParameters[0]=gPMTxyz->GetX(i);
      fParameters[1]=gPMTxyz->GetY(i);
      fParameters[2]=gPMTxyz->GetZ(i);
      fParameters[3]=0;  
      for(goodfits=0,minchi=1e31;goodfits<5;){
	MrqFit(fX-1,fTimes[i]-1,fSigma[i]-1,fN,fParameters-1,fIa-1,fNparameters,fCovar,fAlpha,&chisq);
	TVector3 pos(fParameters[0],fParameters[1],fParameters[2]);
	if((pos-bestpos).Mag()<40){
	  goodfits++;
	  if(chisq<minchi){
	    minchi=chisq;
	    bestpos=pos;
	  }
	}else{
	  if(chisq<minchi){
	    goodfits=0;
	    minchi=chisq;
	    bestpos=pos;
	  }
	}
	float ctheta=trand.Uniform(-1,1);
	float stheta=sqrt(1-ctheta*ctheta);
	float phi=trand.Uniform(0,TMath::Pi()*2);
	fParameters[0]=840*stheta*cos(phi);
	fParameters[1]=840*stheta*sin(phi);
	fParameters[2]=840*ctheta;
	fParameters[3]=0;
      }

      fChisq.Fill(minchi);
      bestpos.GetXYZ(fParameters);
      //kludge for Sept00
      //if(fabs(gPMTxyz->GetY(i))<200)fParameters[1]=gPMTxyz->GetY(i);
      float dx=fParameters[0]-gPMTxyz->GetX(i);
      float dy=fParameters[1]-gPMTxyz->GetY(i);
      float dz=fParameters[2]-gPMTxyz->GetZ(i);
      
      float distance=sqrt(dx*dx+dy*dy+dz*dz);
      float dist2, angle;
      fDistance.Fill(distance);
      fPmts.Fill(i,distance);
      if(minchi<1000){
	fCutDistance.Fill(i,distance);
	fXdev.Fill(dx); fYdev.Fill(dy); fZdev.Fill(dz);
      }
      if(distance>20){
	if(tbad<1000){
	  index[tbad]=i;  
	  fitpos[tbad]=TVector3(fParameters[0],fParameters[1],fParameters[2]);
	  pmtpos[tbad]=fitpos[tbad]-TVector3(dx,dy,dz);
	  dist2=(pmtpos[tbad].Mag()/fitpos[tbad].Mag()*fitpos[tbad]-pmtpos[tbad]).Mag();
	  //calculate rotation angle
	  TVector3 norm=uvw.GetPMTDir(i);
	  TVector3 a=pmtpos[tbad]-(pmtpos[tbad]*norm)*norm.Unit();
	  TVector3 b=fitpos[tbad]-(fitpos[tbad]*norm)*norm.Unit();
	  angle=acos(a*b/a.Mag()/b.Mag())*180.0/TMath::Pi();
	  if ( (norm*a.Cross(b)) < 0)angle=360-angle;

          if(minchi<1000){
	    if(dist2<20){
	      fCutDistance.Fill(i,dist2-distance);
	    }else{
	      fAngle.Fill(i,angle);
	      fprintf(fout," %d bad,  chisq %f Position:%f %f %f d:%f  %f %f\n",i, chisq, fParameters[0],fParameters[1], fParameters[2],distance, dist2, angle);
	      printf(" %d bad,  chisq %f Position:%f %f %f d:%f  %f %f\n",i, chisq, fParameters[0],fParameters[1], fParameters[2],distance,dist2,angle);
	      tbad++;
	    }
	  }
	}
	status[i]=2;


      }else{
	tgood++;
      }
    }
  }

  fprintf(fout," ttested =%d, tgood= %d, tbad=%d \n", ttested,tgood,tbad);

  fprintf(fout,"\n\n  Mapping:\n");
 
  for(i=0;i<tbad&&i<1000;i++){
    int j,k;
    float distance=1000;
    float delta;
    for(j=0;j<tbad&&j<1000;j++){
      delta=(fitpos[i]-pmtpos[j]).Mag();
      if(delta<distance){
	k=j;distance=delta;
      }
    }
    j=index[i];

    fprintf(fout, "Pmt %d (%d/%d/%d): closest to bad pmt %d %f",j,
	    gPMTxyz->GetCrate(j), gPMTxyz->GetCard(j), gPMTxyz->GetChannel(j),
	    index[k],distance);
    printf( "Pmt %d (%d/%d/%d): closest to bad pmt %d %f",j,
	    gPMTxyz->GetCrate(j), gPMTxyz->GetCard(j), gPMTxyz->GetChannel(j),
	    index[k],distance);
    distance=1000;
    for(k=j=0;j<uvw.GetNumPMTNeighbours(index[i]);j++){
      int lcn=uvw.GetPMTNeighbours(index[i])[j];
      TVector3 pmt(gPMTxyz->GetX(lcn),gPMTxyz->GetY(lcn),gPMTxyz->GetZ(lcn));
      delta=(fitpos[i]-pmt).Mag();
      if(delta<distance&&status[lcn]!=1){
	k=lcn;distance=delta;
      }
    }
    fprintf(fout, "  Closest Neighbour pmt: %d %f %d\n",k,distance,status[k]);
    printf( "  Closest Neighbour pmt: %d %f %d\n",k,distance,status[k]);
    
  }
  fclose(fout);
	
  return;
}

      
void QPmtFit::CheckPmtt(){
  // fix x,y,z of each pmt in the array and fit for the time.  Fill 
  // the toffset array..  
  float chisq;
  int i;
  for(i=0;i<fNparameters;i++)fIa[i]=0;  //fix all positions
  fIa[3]=1;
  fParameters[3]=0;  // don't vary time offset for this test.
  int ttested=0;  // number of tubes checked
  fChisq.Reset(); fPmts.Reset();
  float tcalc;
  float derivs[6];
  TVector3 shiftlb(0,0,0);  //kludge- if this is an offset, it calculated
  // how devlb would appear if we didn't "fit out" a shift in the PCA
  // laserball position.
  for(i=0;i<10000;i++){
    int izero;
    int j;
    for(izero=j=0;j<fN;j++)if(fTimes[i][j]>1)izero++;
    if(izero>fN*0.75){
      
      ttested++;
      fParameters[0]=gPMTxyz->GetX(i);
      fParameters[1]=gPMTxyz->GetY(i);
      fParameters[2]=gPMTxyz->GetZ(i);
      MrqFit(fX-1,fTimes[i]-1,fSigma[i]-1,fN,fParameters-1,fIa-1,fNparameters,fCovar,fAlpha,&chisq);
      fChisq.Fill(chisq);
      fPmts.Fill(i,fParameters[3]);
      TVector3 pmt(fParameters[0],fParameters[1],fParameters[2]);
      for(j=0;j<fN;j++){
	TVector3 r=pmt-fLBPositions[j]-shiftlb;
	float distance=r.Mag();
	tcalc=-(distance*fDtDx-fParameters[3]-fTimes[i][j]);
	float cthlb=r.Z()/r.Mag();
	fDevlb.Fill(cthlb,tcalc);
      }
    }
  }

  printf(" ttested =%d, \n",ttested);
	
  return;
}

 
void QPmtFit::mrqfuncs(float, Int_t ix, float a[], float *y, float dyda[], int ){
  TVector3 pmt(a[1],a[2],a[3]);
  TVector3 r=pmt-fLBPositions[ix-1];
  float distance=r.Mag();
  //  float d2=distance*distance;
  *y=distance*fDtDx-a[4];
  // the following three lines use the approximation that 
  // sqrt((r+d)*(r+d))-sqrt(r*r)=(r*d +1/2*d*d)/(r*r),
  // where r and d are vectors.  We put in a 1 cm step in x,y,z to 
  // get the three derivatives;
  dyda[1]=(r.X()+0.5)/distance*fDtDx;
  dyda[2]=(r.Y()+0.5)/distance*fDtDx;
  dyda[3]=(r.Z()+0.5)/distance*fDtDx;
  dyda[4]=-1;
  return ;}
  
