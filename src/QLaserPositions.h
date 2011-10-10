#ifndef __QLaserPositions_h__
#define __QLaserPositions_h__

#include "TTree.h"
#include "TVector3.h"
#include "QPmtFit.h"

class QLaserPositions:public TObject
{
public:
  QLaserPositions();
  QLaserPositions(class TFile &aFile);
  QLaserPositions(class TTree &aTree);
  ~QLaserPositions();
  
  void FillTree();  //add this record to tree
  TTree *NewTree();  //make a new tree, with this branch
  TTree *GetTree(){return outputTree;} //This will be NULL before NewTree is called
  void SetTree(TTree *aTree);
  TTree *GetInputTree(){return inputTree;}
  void GetEntry(Int_t i);
  Double_t GetEntries();
  void CalculateMeanPositions(TH1F &hX,TH1F &hY, TH1F &hZ,FILE *fout=NULL);
  void GenerateTree(QPmtFit &aFit, char *aFileName, int aFastMode=1);
  void Print(const Option_t * opt)const;
  TH1F *GetPull(){return &fPmtPull;}
  Float_t CalculateChiSquared(const Int_t aRun, const TVector3 &aPosition);
  TVector3 &GetFit1(){return fFit1;}
  TVector3 &GetFit2(){return fFit2;}
  Float_t GetSeparation(){return (fManipulator2-fManipulator1).Mag();}
  Float_t ProjectDifference(){return (fFit1-fManipulator1-Offset)*(fFit2-fFit1).Unit();}
  Float_t PerpendicularDifference(){return sqrt( (fFit1-fManipulator1-Offset).Mag2()-pow(ProjectDifference(),2));}
  Float_t Difference(){return (fFit1-fManipulator1-Offset).Mag();}
  Float_t ZSeparation(){return (fManipulator1-fManipulator2).Unit().Z();}
  Float_t RelativeSeparation(){return (fFit1-fManipulator1-fFit2+fManipulator2).Mag();}
  void SetOffset(TVector3 &anOffset){Offset=anOffset;}
  Float_t ParallelFitError();//Error from fit(chisquare)
  Float_t PerpendicularFitError();
  Float_t GetAlpha(Int_t i, Int_t j){return fAlpha[i][j];}
  Float_t GetCovariance(Int_t i, Int_t j){return fCovariance[i][j];}

private:
  Int_t fRun1; //Run number
  Int_t fRun2; //Second Run.  Guaranteed that fRun2>fRun1
  TVector3 fFit1; // Fit based on dt scan
  TVector3 fFit2; // Second Fit position
  Float_t fCovariance[6][6];
  Float_t fAlpha[6][6];  // curvature matrix 
  TVector3 fManipulator1;  // Manipulator Position
  TVector3 fManipulator2;  // Manipulator Position
  static TVector3 Offset;
  Float_t fChiSquared;  // from fit
  Int_t fN; //Number of data points in fit
  TH1F fPmtPull;   // Tracks PMT contribution to chisquar
  Float_t fTriggerOffset;  //trigger time difference

  TTree *outputTree;   //! Tree of records for writing(! means don't put this variable in i/o)
  QLaserPositions *ptr; //! ptr to this record-- needed for tree.
  TTree *inputTree; //! Tree of records for reading. 
  ClassDef (QLaserPositions, 1)	//Class for fitting and plotting laser positions based on timing differences in an optics scan.
};

#endif // __QLaserPositions_h__
