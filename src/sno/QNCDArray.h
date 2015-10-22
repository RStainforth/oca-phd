#ifndef QNCDArray_h
#define QNCDArray_h
class QNCD;
class TH2F;
class TF1;
#include "TH1.h"
#include "TH2.h"
#include "TVector3.h"
#include "QTitles.h"

class QNCDArray:public TObject{
 public:
  QNCDArray(char *aFile);
  QNCDArray();
  ~QNCDArray();
  void Initialize(char *aFile);
  float x1[14], x2[14];
  int diag[14];
  char fName[120]; // name of array configuration
  float Shadow(TVector3 &aPosition);
  void LineShadow(TVector3 &aPosition, TVector3 &aDirection);
  void RandomShadow();
  void Spectrum(Int_t aLimit, TH1F &hout, TH1F &hsyst);
//  QNCD *GetCounter(char *aStr);
  TH2F *GetHist();
  TF1 *GetEnergyF1(){return fEnergyF1;}
  TF1 *GetPositionF1(){return fPositionF1;}
  TF1 *GetStatF1(){return fStatF1;}
  Double_t ImpactParameter(TVector3 &aPosition,TVector3 &aDirection,Int_t &counter_idx);

  // Getters
  QNCD* GetCounter(Int_t anIndex){return fList[anIndex];}
  Int_t GetNCounters(){return fNcounters;}
  
private:
  int fIndex[14][8];
  TH2F *fHist;
  TF1 *fEnergyF1; 
  TF1 *fPositionF1;
  TF1 *fStatF1;
  Int_t fNcounters;             // Number of QNCD counters found in geometry
  QNCD* fList[96];              // List of QNCD objects found in geometry

  ClassDef(QNCDArray,1)		// NCD Array loaded from geometry  
};

#endif
