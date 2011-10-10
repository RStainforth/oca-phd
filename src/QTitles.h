/////////////////////////////////////////////////////////
//
// Titles file reader
//
// Chris Jillings, September 1998
// J. Wendland, June 2005: added GetCWord
//
/////////////////////////////////////////////////////////

#ifndef __CINT__
#include "Rtypes.h"
#include "TObject.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "QBank.h"
#include "TArrayI.h"
#endif

#ifndef _Q_TITLES
#define _Q_TITLES 

  // The constants below are used to obtain validity date and time 
  // from titles files.  Currently, these are set to the intrinsic 
  // validity in the standard header.
  const Int_t kQTitles_header_validDate = 1;
  const Int_t kQTitles_header_validTime = 2;

class QTitles : public TObject {

 protected:
  char* fFilename; // full path
  char  fBank[5];   // bank name
  Int_t fBankNum;
  FILE* fFin;
  Float_t *fDataF;
  Int_t *fDataI;
  Int_t *fHeader;
  Int_t fNumData;
  Int_t fNumHeader;
  char  fFileline[100];
  Int_t fSize;
  Int_t fStartSize;
  Int_t fHeaderSize;

  Int_t fBankIndices;	// number of banks in file
  Int_t *fValidDate;	// array of validity dates
  Int_t *fValidTime;	// array of validity times

 public:
  QTitles();
  QTitles(char* filename, char* aBank, Int_t aBankNum, Int_t aSize=20);
  QTitles(char* filename, char* aBank, Int_t aBankNum, Int_t date, Int_t time, Int_t aSize=20);
  ~QTitles();
  Int_t GetHeaderWord(Int_t word);
  Float_t GetFWord(Int_t word);
  Int_t GetIWord(Int_t word);
  void GetHWord(Int_t num, char* charString); 
  void GetCWord(Int_t word, Int_t n1, Int_t nchar, Char_t* charString);
  Int_t GetN(void) { return fNumData; }
  QBank *GetBank();

  // rsd
  void Initialize(Int_t aSize=20);
  void SetBankName(char *aBank);
  void SetBankNum(Int_t aBankNum);
  void SetFilename(char *aFilename);
  void LoadBank(Int_t bankIndex=0);
  void LoadBankInfo();
  Int_t GetValidDate(Int_t ibank);
  Int_t GetValidTime(Int_t ibank);

 protected:
  Bool_t openFile();
//  void findBank(void);
  void readBank();
  void readHeader();
  void readData();
  void readLine();
  void readHeaderLine();
  Int_t dataType(char*);

  Bool_t getNextNonCommentLine();
  Bool_t isWS(char a);
  void fatalError(char* aMessage);
  char* elimWS();
  void increaseArray();

  Bool_t nextBank();
  void findBank(Int_t bankIndex=0);
  Int_t findValidBank(Int_t date, Int_t time);
  Int_t countBanks();

  ClassDef(QTitles,0) //ZEBRA titles bank reader


};


#endif






