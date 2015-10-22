//___________________________________________________________________________
//
// Class description for the QNCDArray class
//
// The input is in SNOMAN titles file format.
// Creates a set of QNCD objects, that store basic geometrical information 
// for a given NCD array configuration.
// Calculates minimum impact parameter for a straight line. (minimum distance
// to a counter axis).
// Calculates solid angle shadowing for pointlike or line sources.
// 
//*-- Author : Aksel Hallin, Jose' Maneira
//
// 10.2005 -- os
// Moved the number of counters (fNcounters) and the array of QNCD objects
// to the private part of class definition. Added appropriate Getters for 
// other function to access them. 
//
//
#include "QNCDArray.h"
ClassImp(QNCDArray)

#include "QNCD.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TFile.h"
#include "TRandom.h"
#include "TF1.h"
#include "TMath.h"

  //These functions are used to allow us to throw energies and positions including the effects of shadowing.
Double_t vFunc(Double_t *x, Double_t *par){
  Double_t answer=par[4]*(1.0-par[0])/(sqrt(6.283)*par[1])*exp(-0.5*pow((*x-par[2])/par[1],2))+par[4]*par[0]/(2*par[3])*exp(-fabs(*x-par[2])/par[3]);
  return answer;
};

Double_t statFunc(Double_t *x, Double_t *par){
  // Returns the *sigma* of for a given mean energy x.
  // par[0]=shadow
  // nhits=par[1]+par[2]*energy, if there were no shadow
  // par[3] is the "statistics subtracted" resolution offset (fractional)
  Float_t nhits=(par[1]+par[2]* *x)*par[0];  // "number of hit tubes"
  Float_t sigma=*x *sqrt(1.0/nhits+par[3]*par[3]); //width in units of energy
  return sigma;
  //  Float_t diff=(*x-par[0]*par[1])/sigma;
  //  return exp(-diff*diff/2);
}

Double_t eFunc(Double_t *x, Double_t *par){
  Float_t diff=(*x-par[1])/par[2];
  return par[0]*exp(-diff*diff/2);
}

const float lengths[14]={400,550,600,650,700,750,850,900,900,950,1000,1050,1050,1100};

QNCDArray::QNCDArray(char *aFile){
	Initialize(aFile);
}

QNCDArray::QNCDArray(){
	char aFile[100];
	gSNO->GetSnomanTitles("geometry.dat",aFile);

	//	sprintf(aFile,"%s/autosno/prod/optimize_ncd_50center.dat",getenv("QSNO_ROOT"));
	Initialize(aFile);
}


void QNCDArray::Initialize(char *aFile){

  QTitles *titles = new QTitles(aFile,"GEDP",111);
  QBank *bank = (QBank*) titles->GetBank();
  fNcounters = bank->icons(1);
 
  int i;
  Float_t x,y,length,vol;
  for (i = 0; i < fNcounters; i++){
 	x      = bank->rcons(2+ 4*i);
 	y      = bank->rcons(2+ 4*i + 1);
 	length = bank->rcons(2+ 4*i + 2);
 	vol    = bank->rcons(2+ 4*i + 3);
	// 13.06.2005 -- os
	// changed the initialization of QNCD object:
	// counters carry index 0 to 39.
	// fList[i] = new QNCD(x,y,length);
	fList[i] = new QNCD(x,y,length,i);
  }
//   char str[3];
//   for(i=0;i<14;i++)fscanf(inp," %f %f\n",x1+i,x2+i);
//   for(i=0;i<14;i++)diag[i]=(x1[i]==x2[i]);
//   int ii;
//   i=0;
//   int j=0;
//   for (ii=i=0;i<14;i++){
//     int jmax;
//     jmax = diag[i]?4:8;
//     for(j=0;j<jmax;j++){
//       str[0]='A'+i; str[1]='1'+j;str[2]=0;
//       fList[ii]=new QNCD(str,this,lengths[i]);
//       fIndex[i][j]=ii;
//       ii++;
//     }
//   }
// 
//   while(fscanf(inp, "%s", str)==1){
//     GetCounter(str)->Remove();
//     //printf("%s removed! Warning: kludge is rotating counters by 90 degrees.  Change GetCounter when numbering is fixed!\n",str);
//   }
//   fclose(inp);
  char line[128];
  sprintf(line,"%s Array Lengths",fName);
  fHist=new TH2F("NCDarray",line,12,-600,600,12,-600,600);
  fHist->GetXaxis()->SetTitle("x (m)");
  fHist->GetYaxis()->SetTitle("y (m)");
//  for(i=j=0;i<96;i++)if(fList[i]->IsPresent())j++;
  printf(" Array has %d active counters\n",fNcounters);
  fHist->SetDirectory(0);  // belongs to QNCDArray, not to TDirectory
  printf(" Gets this far at least!\n " );
  fEnergyF1=new TF1("NCDEnergyF1",eFunc,0,20,3);
  printf(" Test1 \n" );
  fPositionF1=new TF1("NCDPositionF1",vFunc,-200,200,5);
  printf(" Test2 \n" );
  fStatF1=new TF1("NCDStatisticsF1",statFunc,0,20,5);
  printf(" Test3 \n" );
}




QNCDArray::~QNCDArray(){int i; 
 for(i=0;i<fNcounters;i++)delete fList[i];
 delete fEnergyF1; 
 delete fPositionF1;
 delete fStatF1;
}
TH2F *QNCDArray::GetHist(){
  fHist->Reset();
  int i;
  Stat_t xl;
  TVector3 d;
  for(i=0;i<fNcounters;i++){
    xl=0;
    d=fList[i]->GetTop();
    //if(fList[i]->IsPresent()){
      xl= fList[i]->GetLength();
      fHist->Fill(d.X(), d.Y(),xl);
   // }      
  }
  return fHist;
}

// QNCD* QNCDArray::GetCounter(char *aStr){
//   int i=aStr[0]-'A';
//   int j=aStr[1]-'1';
//   //kludge-- to accomodate differences between Mike Dragowsky and me.  Needs to be commented out 
//   //for final configurations!
//   if(diag[i]){j=j-1; if(j<0)j+=4;}
//   else {j=j-2; if(j<0)j+=8;}
//   //end of kludged
// 
//   return fList[fIndex[i][j]];
// }

float QNCDArray::Shadow(TVector3 &aPosition){ 
  float sa0=0;
  int i=0;
  int index[150];int j;
  float ctheta1[150], ctheta2[150], phi1[150], phi2[150];
  TVector3 diff;
  QNCD *counter;
  int ndets;
  for(ndets=i=0;i<fNcounters;i++){
    counter=fList[i];
  //  if(!counter->IsPresent())continue;
    diff= counter->GetBottom() - aPosition; ctheta1[ndets]=diff.CosTheta();
    diff= counter->GetTop() - aPosition; ctheta2[ndets]=diff.CosTheta();
    diff.SetZ(0);
    if(diff.Mag()>2.54){
      phi1[ndets]=diff.Phi()-asin(2.54/diff.Mag());
      phi2[ndets]=diff.Phi()+asin(2.54/diff.Mag());
      sa0+= (phi2[ndets]-phi1[ndets])*(ctheta2[ndets]-ctheta1[ndets]);
    }else{
      sa0+= TMath::Pi()*4;
    }
    ndets++;
  }
  //  int ndets=i;
  int k;
  for(index[0]=0, i=1; i<ndets; i++){
    for(j=0;j<i;j++){
      if(phi1[index[j]]>phi1[i])break;
    }
    for(k=i;k>j;k--)index[k]=index[k-1];
    index[j]=i;
      
  }
  float sa=0;
  int nactive;
  int active[25];
  int jnext;
  index[ndets]=ndets;
  phi1[ndets]=TMath::Pi();
  for(nactive=i=0;i<ndets;i++){
    j=index[i]; jnext=index[i+1];
    if(nactive==0){
      if(phi1[jnext]>phi2[j])sa += (phi2[j]-phi1[j])*(ctheta2[j]-ctheta1[j]);
      else{
	nactive=1;
	sa+=(phi1[jnext]-phi1[j])*(ctheta2[j]-ctheta1[j]);
	active[0]=j;
      }
    }else{
      active[nactive]=j;
      float phistart=phi1[j];
      nactive++;
      int loop;
      for(loop=1;loop;){
	float phimin=phi2[active[0]];
	float cmax=ctheta2[active[0]]; float cmin=ctheta1[active[0]];
	int kmin=0;
	for(k=1;k<nactive;k++){
	  if(phi2[active[k]]<phimin){
	    phimin=phi2[active[k]];
	    kmin=k;
	  }
	  if(ctheta2[active[k]]>cmax)cmax=ctheta2[active[k]];
	  if(ctheta1[active[k]]<cmin)cmin=ctheta1[active[k]];
	}
	if(phimin>phi1[jnext]){
	  loop=0;
	  sa += (phi1[jnext]-phistart)*(cmax-cmin);
	}else{
	  sa+=(phimin-phistart)*(cmax-cmin);
	  for(k=kmin; k<nactive;k++)active[k]=active[k+1];
	  nactive--;
	  phistart=phimin;
	}
	if(nactive==0)loop=0;
      }
    }
  }
  return sa;
}

void QNCDArray::LineShadow(TVector3 &aPosition, TVector3 &aDirection){
  //TGraph * ncdshadow(TVector3 &origin, TVector3 &direction, char *aFile){
  //NCDArray array(aFile);
  char grName[128];
  sprintf(grName,"%s_%d_%d_%d__%d_%d_%d",fName,
	  (int)(aPosition.X()+0.5),
	  (int)(aPosition.Y()+0.5),
	  (int)(aPosition.Z()+0.5),
	  (int)aDirection.X(),
	  (int)aDirection.Y(),
	  (int)aDirection.Z());
  printf(" Calculating graph %s\n",grName);
  float frac[300];
  float xfrac[300];
  TVector3 r0;
  int ix;
  float solidAngle;

  for(ix=0;ix<295;ix++){
    r0=aPosition+ix*2.0*aDirection;
    solidAngle=Shadow(r0);
    frac[ix]=solidAngle/4.0/TMath::Pi();
    xfrac[ix]=r0.Mag();
  }
  TGraph gr(295,xfrac,frac);
  TFile fout("ncd_shad.root","update");
  gr.Write(grName);
  fout.Close();
  return;
}


void QNCDArray::RandomShadow(){
char line[128];
  //sprintf(aFile,"%s","ncd_opt3.txt");
TH2F dist("Dist","Dist",100,0,600,100,0,0.25);
//NCDArray array(aFile);
TRandom rndm;
TVector3 r0(0,0,1);
int ix;
float solidAngle;
dist.Reset();
int aLimit=100000;
TH2F *h=GetHist();

for(ix=0;ix<aLimit;ix++){
  if(ix%100==0)printf("%d\n",ix);
  r0.SetMag(600*pow(rndm.Uniform(0,1.00),0.33333333));
  r0.SetTheta(acos(rndm.Uniform(-1,1)));
  r0.SetPhi(rndm.Uniform(0,TMath::Pi()*2));
  solidAngle=Shadow(r0);
  dist.Fill(r0.Mag(),solidAngle/4.0/TMath::Pi());
}

TFile f("ncd_shad.root","update");
 sprintf(line,"Dist_%s",fName);
dist.Write(line);
 sprintf(line,"Array_%s",fName);
h->Write(line);
f.Close();
}




void QNCDArray::Spectrum(Int_t aLimit, TH1F &hout, TH1F &hsyst){
  // Throws aLimit "electrons" according to the input distributions and generates an output measured energy histogram. 
  // Output energy distribution goes into hout.
  // Output hsyst contains a histogram of systematics-only corrected energy

  //  int Limit=100000;
 float fourpi=4*TMath::Pi();
 float twopi=2*TMath::Pi();
 TVector3 r0(1,1,1);
 TVector3 r,rp;
 TRandom rndm;
 Float_t e0,energy;
 Float_t solidAngle;
 Float_t sa2,ecorr;
 int ix;
 for(ix=0;ix<aLimit;ix++){
   if(ix%1000==0)printf("%d\n",ix);
   // throw an initial position r0;
   r0.SetMag(600*pow(rndm.Uniform(0,1.00),0.33333333));
   r0.SetTheta(acos(rndm.Uniform(-1,1)));
   r0.SetPhi(rndm.Uniform(0,twopi));
   // throw initial energy e0;
   e0=fEnergyF1->GetRandom(); 
   solidAngle=Shadow(r0);

   Float_t sf=1-solidAngle/fourpi;
   //   Float_t nhits=(e0-0.5624)/(0.1096)*sf; //+par[3]* par[0])*par[1];  // "number of hit tubes"
   //Float_t sigma=e0 *sqrt(1.0/nhits+0.01); //width in units of energy
 
  // fStatF1->SetParameter(0,e0);
  fStatF1->SetParameter(1,1-solidAngle/fourpi);
  energy=rndm.Gaus(e0*sf,fStatF1->Eval(e0));  
  //energy=e0*sf;
 
   // throw a reconstructed position, r
   r=r0+TVector3(fPositionF1->GetRandom(), fPositionF1->GetRandom(), fPositionF1->GetRandom());
   // calculate the energy correction
   sa2=0;
   int j;
   for(j=0;j<12;j++){
     if(j==0){
       rp=r;
     }else{
       rp=r + TVector3(fPositionF1->GetRandom(),fPositionF1->GetRandom(),fPositionF1->GetRandom());
     }
     sa2+=Shadow(rp);
   }
    sa2 /= 12;
   
    ecorr=energy/(1-sa2/fourpi);  // the corrected energy;
    hout.Fill(ecorr);
    hsyst.Fill(e0*sf/(1-sa2/fourpi));
 }
}


//------------------------------------------------------------------------------------------
Double_t QNCDArray::ImpactParameter(TVector3 &aPosition,TVector3 &aDirection,Int_t &counter_idx){

	Int_t status;
	Double_t dist,min_dist = 9999.;
  	Int_t i_min = 999;

  	for(int i=0;i<fNcounters;i++){
    if(!fList) printf("warning no fList!!! \n");

    dist = fList[i]->GetDistance(aPosition,aDirection,status);
	
	if (dist < min_dist && status < 10) {
		min_dist = dist;
		i_min = i;
	}
	
  }
  counter_idx = i_min;
  
  
  return min_dist;
}
