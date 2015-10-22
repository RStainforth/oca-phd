#include "QOCATree.h"
#include "QLBPositionFit.h"
#include "QOptics.h"
#include "QPMTuvw.h"
#include "TEventList.h"
#include "TRandom.h"
#include "QPmtFit.h"
#include "QPMTxyz.h"


ClassImp(QLBPositionFit)

  QLBPositionFit::QLBPositionFit(QPmtFit &aFit):
    fFit(aFit)
{
  fN=0;
  fCovar = matrix(1,fNparameters,1,fNparameters);
  fAlpha = matrix(1,fNparameters,1,fNparameters);
  int i;
  for(i=0;i<fNparameters;i++)fIa[i]=1;  //vary all parameters
  fDtDxD2O = 1.0/fOptics.GetVgroupD2O(500);
  fDtDxAcrylic = 1.0/fOptics.GetVgroupAcrylic(500);
  fDtDxH2O = 1.0/fOptics.GetVgroupH2O(500);
  fDtDx=fDtDx0 = (600*fDtDxD2O+5.5*fDtDxAcrylic+245*fDtDxH2O)/850;
  fMode=0;
  fFastDistances=0;
  fRun1=fRun2=0;
  for(i=0;i<10000;i++)fD2ODistances[i]=NULL;
  int nruns=fFit.GetNumberOfRuns();
  fSourcePositions=new TVector3[nruns];
  fOffset=new TVector3[nruns];
  fSourceInitialized=0;  
}
    

QLBPositionFit::~QLBPositionFit(){
  free_matrix(fCovar,1,fNparameters,1,fNparameters);
  free_matrix(fAlpha,1,fNparameters,1,fNparameters);
  int i;
  for(i=0;i<10000;i++)if(fD2ODistances[i]){
    delete[] fD2ODistances[i];
    delete[] fAcrylicDistances[i];
    delete[] fH2ODistances[i];
    delete[] fDirections[i];
  }
  delete[] fSourcePositions;
}


//void QLBPositionFit::InitializeFastDistances(){

  //  TVector3 *fSourcePositions; //record of source positions for calc. distances
  //  Float_t *fD2ODistances[10000];
  //Float_t *fH2ODistances[10000];
  //Float_t *fAcrylicDistances[10000];
  //TVector3 *fDirections[10000];
//}  

void QLBPositionFit::DoFit(int iRun1, int iRun2, TH1F &aPullHist, Float_t &aChiSquare){

  // find the best fit positions based on the time differences for two runs 
  // in a laser scan.  
  // the toffset array..  
  int i;
  fFit.GetData(iRun1,iRun2,fX,fTimes,fSigma,fN);
  fRun1=iRun1; fRun2=iRun2;
  fLBPosition1=fFit.GetLBPosition(iRun1);
  fLBPosition2=fFit.GetLBPosition(iRun2);
  if((fLBPosition1-fLBPosition2).Mag()<100){
    printf("%d %d are too close together.  Will not fit\n",iRun1,iRun2);
    aChiSquare=-1;
    return;
  }
  if(gPMTxyz==0)gPMTxyz=new QPMTxyz();
  for(i=0;i<fN;i++)fPmt[i]=gPMTxyz->GetXYZ((int)fX[i]);
  fParameters[0]=10;
  fParameters[1]=10;
  fParameters[2]=10;
  fParameters[3]=200;
  fParameters[4]=100;
  fParameters[5]=100;
  fParameters[6]=fDtDxD2O;
  fParameters[7]=fDtDxAcrylic;
  fParameters[8]=fDtDxH2O;
  fParameters[9]=0.0; //dt
  fIa[6]=0;
  fIa[7]=0;
  fIa[8]=0;
  fIa[9]=1;
  SetMode(0);
  MrqFit(fX-1,fTimes-1,fSigma-1,fN,fParameters-1,fIa-1,fNparameters,fCovar,fAlpha,&aChiSquare);
  printf("Laserball %d (%f,%f,%f),Laserball %d (%f,%f,%f), aChiSquare %f\n",
	 fRun1, fParameters[0],fParameters[1],fParameters[2],fRun2, fParameters[3],fParameters[4],fParameters[5],aChiSquare);
  printf("%f %f %f\n", fParameters[6],fParameters[7],fParameters[8]);
  SetMode(1);
  //  fIa[9]=0; 


  //The following loop is required if running in fast mode to get rid of PMT's
  //that are not initialized
  int j;
  for(j=i=0;i<fN;i++){
    fX[j]=fX[i];
    fTimes[j]=fTimes[i];
    fSigma[j]=fSigma[i];
    fPmt[j]=fPmt[i];
    float dummy[11];
    float yvalue;
    mrqfuncs(0.0,i+1,fParameters-1,&yvalue,dummy,0);
    float pull=(fTimes[i]-yvalue)/fSigma[i];
    if(pull*pull<25)j++;
  } 
  fN=j;

  MrqFit(fX-1,fTimes-1,fSigma-1,fN,fParameters-1,fIa-1,fNparameters,fCovar,fAlpha,&aChiSquare);
  printf("Laserball %d (%f,%f,%f),Laserball %d (%f,%f,%f), aChiSquare %f\n",
 	 fRun1, fParameters[0],fParameters[1],fParameters[2],fRun2, fParameters[3],fParameters[4],fParameters[5],aChiSquare);
  printf("%f %f %f\n", fParameters[6],fParameters[7],fParameters[8]);
  aPullHist.Reset();
  for(i=0;i<fN;i++){
    float dummy[11];
    float yvalue;
    mrqfuncs(0.0,i+1,fParameters-1,&yvalue,dummy,0);
    float pull=(fTimes[i]-yvalue)/fSigma[i];
    aPullHist.SetBinContent((int)fX[i],pull);
  } 
  return;
}

void QLBPositionFit::CalculateChiSquare(Int_t iparam,TH1F &chis){
  // calculate chisquare as a function of a parameter
  // Uses the histogram to get the number of bins, lower and upper limits of 
  // the parameter;

  chis.Reset();
  int i;
  Float_t chisq;
  //  Float_t *beta=new Float_t[fNparameters+1];
  
  Float_t psave=fParameters[iparam];
  for(i=1;i<chis.GetNbinsX();i++){
    fParameters[iparam]=chis.GetBinCenter(i);
    //    mrqcof(fX-1,fTimes-1,fSigma-1,fN,fParameters-1,fIa-1,fNparameters,fAlpha, 
    //	   beta,&chisq);
    int p;
    for(chisq=0.0,p=0;p<fN;p++){
      float dummy[10];
      float yvalue;
      mrqfuncs(0.0,p+1,fParameters-1,&yvalue,dummy,0);
      float pull=(fTimes[p]-yvalue)/fSigma[p];
      chisq+=pull*pull;
      //      aPullHist.SetBinContent((int)fX[i],pull);
    } 
    chis.SetBinContent(i,chisq);
  }
  //delete[] beta;
  return;
}


void QLBPositionFit::mrqfuncs(float, Int_t ix, float a[], float *y, float dyda[], int ){
  if(ix==1){ // first call to mrqfuncs in mrqcof 
    fLBPosition1.SetXYZ(a[1],a[2],a[3]);
    fLBPosition2.SetXYZ(a[4]+a[1],a[5]+a[2],a[6]+a[3]);
    fDtDx=(600*a[7]+5.5*a[8]+245*a[9])/850;

    if(fMode){
      SetSource(fLBPosition1,fRun1);
      SetSource(fLBPosition2,fRun2);
    }
  }
  Double_t dd2o1,dd2o2,dacr1,dacr2,dh2o1,dh2o2;
  //,cp1,cp2,tp1,tp2;
  //Int_t p1,p2;
  fP=fPmt[ix-1]; //Directly accessed arrays need to have the offset -1
  
  fV1=fLBPosition1-fP;
  fV2=fLBPosition2-fP;
  float d2=fV2.Mag();
  float d1=fV1.Mag();
  fVN1=-a[7]/d1*fV1;
  fVN2=a[7]/d2*fV2;
  float dt=(d2-d1)*fDtDx+a[10];
  if(fMode){
    int ipmt=(int)fX[ix-1];// Directly accessed, has offset -1

    GetDistances(ipmt,fRun1,dd2o1,dacr1,dh2o1);
    GetDistances(ipmt,fRun2,dd2o2,dacr2,dh2o2);
    *y=(dd2o2-dd2o1)*a[7] +(dacr2-dacr1)*a[8]+(dh2o2-dh2o1)*a[9]+a[10];
    dyda[7]=dd2o2-dd2o1;
    dyda[8]=dacr2-dacr1;
    dyda[9]=dh2o2-dh2o1;
  }else{
    *y=dt;
    dyda[7]=(d2-d1)*600/850;
    dyda[8]=(d2-d1)*5.5/850;
    dyda[9]=(d2-d1)*245/850;
  }
  if(fabs(*y-dt)>0.5){
    //  printf("%f %f\n",*y,dt);
  }
  //*y=dt;
  // the following three lines use the approximation that 
  // sqrt((r+d)*(r+d))-sqrt(r*r)=(r*d +1/2*d*d)/(r*r),
  // where r and d are vectors.  We put in a 1 cm step in x,y,z to 

  // get the three derivatives;
  // dyda[1]=fVN1.X();
//   dyda[2]=fVN1.Y();
//   dyda[3]=fVN1.Z();
//   dyda[4]=fVN2.X();
//   dyda[5]=fVN2.Y();
//   dyda[6]=fVN2.Z();
  dyda[1]=fVN1.X()+fVN2.X();
  dyda[2]=fVN1.Y()+fVN2.Y();
  dyda[3]=fVN1.Z()+fVN2.Z();
  dyda[4]=fVN2.X();
  dyda[5]=fVN2.Y();
  dyda[6]=fVN2.Z();
  dyda[10]=1.0;
  return ;
}


// This routine is an attempt to speed up the fit by reducing the number
//of calls to GetDistances.  We call it occasionally, and for events
//close to the old call, find the vector between the "calculated" source
//position and the current source position and decompose it into a radial 
//vector" and a transverse vector.  Then subsequent calls to GetDistances 
//will take (source-pos-pmt).transverse, and see if it is less than some offset

/*  What I want to do here is to calculate all the distances for a given position*/  
void QLBPositionFit::SetSource(TVector3 &aSourcePosition, Int_t aRun){
  //  if(aSourcePosition.Mag()>600)aSourcePosition=590*aSourcePosition.Unit();
 fOffset[aRun]=aSourcePosition-fSourcePositions[aRun];
  float dtot=fOffset[aRun].Mag();
  if(!fSourceInitialized || !fFastDistances || dtot>10 ||fD2ODistances[(int)fX[0]][aRun]==0){  // recalculate the distances
    fSourceInitialized=1;
    fSourcePositions[aRun]=aSourcePosition;
    fOptics.SetSource(aSourcePosition);
    int i;
    for(i=0;i<10000;i++)if(fD2ODistances[i])fD2ODistances[i][aRun]=0;
    for(i=0;i<fN;i++){
      int ipmt=(int)fX[i];
      if(fD2ODistances[ipmt]==NULL){
	int iii=fFit.GetNumberOfRuns();
	fD2ODistances[ipmt]=new Float_t[iii];
	fAcrylicDistances[ipmt]=new Float_t[iii];
	fH2ODistances[ipmt]=new Float_t[iii];
	fDirections[ipmt]=new TVector3[iii];
        int kkk;
	for(kkk=0;kkk<iii;kkk++)fD2ODistances[ipmt][kkk]=10000;
      }
      Double_t  cp1,tp1,dd,da,dh;
      Int_t p1;
      fOptics.GetDistances(ipmt,dd,da,dh,cp1,p1,tp1);
      fD2ODistances[ipmt][aRun]=dd;
      fAcrylicDistances[ipmt][aRun]=da;
      fH2ODistances[ipmt][aRun]=dh;
      fDirections[ipmt][aRun]=fOptics.GetPMTrelvec();
    }
    fOffset[aRun].SetXYZ(0.0,0.0,0.0);
  }else if(fFastDistances){
    fOptics.SetSource(fSourcePositions[aRun]);
    int i;
    for(i=0;i<fN;i++){
      int ipmt=(int)fX[i];
      if(fD2ODistances[ipmt]==NULL){
	int iii=fFit.GetNumberOfRuns();
	fD2ODistances[ipmt]=new Float_t[iii];
	fAcrylicDistances[ipmt]=new Float_t[iii];
	fH2ODistances[ipmt]=new Float_t[iii];
	fDirections[ipmt]=new TVector3[iii];
      }
      if(fD2ODistances[ipmt][aRun]==0){
	Double_t  cp1,tp1,dd,da,dh;
	Int_t p1;
	fOptics.GetDistances(ipmt,dd,da,dh,cp1,p1,tp1);
	fD2ODistances[ipmt][aRun]=dd;
	fAcrylicDistances[ipmt][aRun]=da;
	fH2ODistances[ipmt][aRun]=dh;
	fDirections[ipmt][aRun]=fOptics.GetPMTrelvec();
      }
    }
  }
  return;
}
    
void QLBPositionFit::GetDistances(Int_t aPmt, Int_t aRun, Double_t &aDd2o,
  Double_t &aDacrylic, Double_t &aDh2o){
  float dlong=0;
  if(fFastDistances)dlong=fOffset[aRun]*fDirections[aPmt][aRun];
  aDd2o=fD2ODistances[aPmt][aRun]-dlong;
  aDacrylic=fAcrylicDistances[aPmt][aRun];
  aDh2o=fH2ODistances[aPmt][aRun];
  if(aDd2o>2000 ||aDacrylic>200 || aDh2o>1000){
    printf("bad distances\n");
  }
  return;
}

