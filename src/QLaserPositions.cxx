#include "QLaserPositions.h"
#include "TFile.h"
#include "QLBPositionFit.h"

//*-- Author: A.L. Hallin
//

ClassImp(QLaserPositions)  //


  TVector3 QLaserPositions::Offset(0,0,0);

QLaserPositions::QLaserPositions():
fPmtPull("Pmtpull","PMT by PMT pulls",10000,0,10000){
//Normal constructor.  The default constructor shouldn't allocate 
// memory- we break that rule in many places in QSNO, but it is very
// cumbersome to always test that everything has been constructed.

  fRun1=fRun2=0;
  outputTree=NULL;
  fPmtPull.SetDirectory(0);

}

QLaserPositions::QLaserPositions(TFile &aFile):
fPmtPull("Pmtpull","PMT by PMT pulls",10000,0,10000){
//"Reading in" constructor.  Finds tree, sets this general record
// as the memory location to read and reads in first entry.
  outputTree=NULL;
  inputTree=(TTree *)aFile.Get("LaserPositions");
  ptr=this;
  inputTree->SetBranchAddress("LaserRecords",&ptr);
  inputTree->GetEntry(0);  
  fPmtPull.SetDirectory(0);
}

QLaserPositions::QLaserPositions(TTree &aTree):
fPmtPull("Pmtpull","PMT by PMT pulls",10000,0,10000){
//"Reading in" constructor.  Finds tree, sets this general record
// as the memory location to read and reads in first entry.
  outputTree=NULL;
  inputTree=&aTree;
  ptr=this;
  inputTree->SetBranchAddress("LaserRecords",&ptr);
  inputTree->GetEntry(0);  
  fPmtPull.SetDirectory(0);
}
  
  
QLaserPositions::~QLaserPositions(){
  if(outputTree)delete outputTree;
}

void QLaserPositions::GenerateTree(QPmtFit &aFit, char *aFileName,int aFastMode){
  QLBPositionFit lbpf(aFit);
  lbpf.SetFastDistances(aFastMode);
  TFile f(aFileName,"recreate");
  int i,j;
  for(i=0;i<aFit.GetNumberOfRuns();i++)for(j=i+1;j<aFit.GetNumberOfRuns();j++){
    // for(i=0;i<3;i++)for(j=i+1;j<3;j++){
    lbpf.DoFit(i,j,fPmtPull,fChiSquared);
    if(fChiSquared<0)continue; //Don't save runs that fail fit (too close!)
    fRun1=aFit.GetRunNumber(i);
    fRun2=aFit.GetRunNumber(j);
    fFit1=lbpf.GetLBPosition1();
    fFit2=lbpf.GetLBPosition2();
    fManipulator1=aFit.GetLBPosition(i); //Kludge!  Not necessarily manpos
    fManipulator2=aFit.GetLBPosition(j);
    fTriggerOffset=lbpf.GetTriggerOffset();
    fN=lbpf.GetN();
    int irow,icol;
    for(irow=0;irow<6;irow++)for(icol=0;icol<6;icol++){
      fCovariance[irow][icol]=lbpf.GetCovariance(irow,icol);
      fAlpha[irow][icol]=lbpf.GetAlpha(irow,icol);
    }
    FillTree();
  }
  outputTree->Write();
  f.Close();
  outputTree=NULL;   // f.Close() should delete outputTree;
    
}

void QLaserPositions::Print(const Option_t *)const{
  printf("QLaserPositions:  \tRun 1 %d  \t\t\tRun2 %d\n",fRun1,fRun2);
  printf("Fit Positions:  \t(%6.2f,%6.2f,%6.2f)\t(%6.2f,%6.2f,%6.2f)\n",fFit1.X(),fFit1.Y(),
	 fFit1.Z(),fFit2.X(),fFit2.Y(),fFit2.Z());
  printf("Manipulator Positions:\t(%6.2f,%6.2f,%6.2f)\t(%6.2f,%6.2f,%6.2f)\n",fManipulator1.X(),fManipulator1.Y(),
	 fManipulator1.Z(),fManipulator2.X(),fManipulator2.Y(),fManipulator2.Z());
  printf("Covariance Matrix:\n");
  int i,j;
  for(i=0;i<6;i++){for(j=0;j<6;j++)printf("%6.3f\t",fCovariance[i][j]);printf("\n'");
  }
}


void QLaserPositions::FillTree(){
  //Fill the tree.  Will create it if necessary
  if(outputTree==NULL)NewTree();
  outputTree->Fill();
}

TTree *QLaserPositions::NewTree(){
  //Create the tree. 
  if(outputTree!=NULL)delete outputTree;
  outputTree=new TTree("LaserPositions","Laserball positions from fit to time differences");
  ptr=this;
  outputTree->Branch("LaserRecords","QLaserPositions",&ptr,4000,99);
  outputTree->SetBranchAddress("LaserRecords",&ptr);;
  return outputTree;
}

void QLaserPositions::SetTree(TTree *aTree){
  // Allow us to read an existing tree into this Record
  ptr=this;
  aTree->SetBranchAddress("LaserRecords",&ptr);
  inputTree=aTree;
}

void QLaserPositions::GetEntry(Int_t i){
  if(inputTree !=NULL)inputTree->GetEntry(i);
  else printf("Input tree not defined in QLaserPostions\n");
}

Double_t QLaserPositions::GetEntries(){
  if( inputTree ) return( inputTree->GetEntries() );
  return( 0 );
}

void QLaserPositions::CalculateMeanPositions(TH1F &hX,TH1F &hY,TH1F &hZ, FILE *fout){

  int iRun[100];
  Float_t sumx[100],sumy[100],sumz[100];
  Float_t weightx[100],weighty[100],weightz[100];
  Float_t wt;
  TVector3 manip[100];
  int i,nRuns;  
  float chicut=100000;
  printf ("Cutting at chisquared= %f\n", chicut);
  for(i=0;i<100;i++)sumx[i]=sumy[i]=sumz[i]=weightx[i]=weighty[i]=weightz[i]=0.0;

  for(nRuns=i=0;i<GetEntries();i++){
    GetEntry(i);
    if(fChiSquared==0 ||fChiSquared>chicut)continue;
    int j;
    for(j=0;j<nRuns;j++){
      if(iRun[j]==fRun1)break;
    }
    if(j==nRuns){iRun[j]=fRun1;manip[j]=fManipulator1; nRuns++;}
    wt=fCovariance[0][0];
    sumx[j]+=fFit1.X()/wt;  weightx[j]+= 1.0/wt;
    sumy[j]+=fFit1.Y()/wt;  weighty[j]+= 1.0/wt;
    sumz[j]+=fFit1.Z()/wt;  weightz[j]+= 1.0/wt;
    for(j=0;j<nRuns;j++){
      if(iRun[j]==fRun2)break;
    }
    if(j==nRuns){iRun[j]=fRun2;manip[j]=fManipulator2; nRuns++;}
    wt=fCovariance[0][0];
    sumx[j]+=fFit2.X()/wt;  weightx[j]+= 1.0/wt;
    sumy[j]+=fFit2.Y()/wt;  weighty[j]+= 1.0/wt;
    sumz[j]+=fFit2.Z()/wt;  weightz[j]+= 1.0/wt;

  }
  for(i=0;i<nRuns;i++){
    printf(" %d (%5.2f +/- %5.2f,%5.2f +/- %5.2f,%5.2f +/- %5.2f); (%5.2f,%5.2f,%5.2f)\n",iRun[i],
	   sumx[i]/weightx[i],sqrt(1.0/weightx[i]),
	   sumy[i]/weighty[i],sqrt(1.0/weighty[i]),
	   sumz[i]/weightz[i],sqrt(1.0/weightz[i]),
	   manip[i].X(),manip[i].Y(),manip[i].Z());

    if(fout)fprintf(fout,"%d\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%5.2f\n",iRun[i],
	   sumx[i]/weightx[i],sqrt(1.0/weightx[i]),
	   sumy[i]/weighty[i],sqrt(1.0/weighty[i]),
	   sumz[i]/weightz[i],sqrt(1.0/weightz[i]),
	   manip[i].X(),manip[i].Y(),manip[i].Z());

    hX.Fill(sumx[i]/weightx[i]-manip[i].X());
    hY.Fill(sumy[i]/weighty[i]-manip[i].Y());
    hZ.Fill(sumz[i]/weightz[i]-manip[i].Z());

  }
}

Float_t QLaserPositions::CalculateChiSquared(const Int_t aRun, const TVector3 &aPosition){
  TVector3 a;
  if(aRun==fRun1)a=aPosition-fFit1;
  else if(aRun==fRun2)a=aPosition-fFit2;
  else return 0;
  int i,j;
  float sum=0;
  for(i=0;i<3;i++)for(j=0;j<3;j++)sum+=fAlpha[i][j]*a[i]*a[j];
  return sum;
}

Float_t QLaserPositions::ParallelFitError(){
  TVector3 off=(fFit1-fFit2).Unit();
  float sum;
  int i,j;
  int sign;
  for(sum=0,i=0;i<3;i++)for(j=0;j<3;j++){
    sum+= off[i]*off[j]*(fCovariance[i][j]+fCovariance[i][j+3]+fCovariance[i+3][j]+fCovariance[i+3][j+3]);
  }
  return sqrt(sum);
}

  
Float_t QLaserPositions::PerpendicularFitError(){
  TVector3 off=(fFit1-fFit2);
  off=off.Cross(fFit1); off=off.Unit();
  float sum;
  int i,j;
  for(sum=0,i=0;i<6;i++)for(j=0;j<6;j++){
    sum+= off[i]*off[j]*(fCovariance[i][j]+fCovariance[i][j+3]+fCovariance[i+3][j]+fCovariance[i+3][j+3]);
  }
  return sqrt(sum);
}



