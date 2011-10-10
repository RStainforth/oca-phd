//__________________________________________________________
// QGlobarShaper.h
//*-- Author : Peter Skensved
//          

#ifndef _QGlobalShaper_h
#define _QGlobalShaper_h

#include <TObject.h>

class QGlobalShaper : public TObject
{
public:

	QGlobalShaper();
	QGlobalShaper(const QGlobalShaper &aGlobalShaper);
	QGlobalShaper(Int_t *ivars);
	virtual ~QGlobalShaper() {};

 	void Clear( Option_t *anOption) {}
	Int_t GetBoardInfo(void) {return fBoardInfo;}
  Int_t GetGlobalScaler(void) {return fGlobalScaler;}
  
	void SetBoardInfo(Int_t aBoardInfo) { fBoardInfo = aBoardInfo & 0xf;}
	void SetGlobalScaler(Int_t aGlobalScaler) { fGlobalScaler = aGlobalScaler & 0xfffffff; }

	Int_t *Data(){return (Int_t *)&fBoardInfo;}
	Int_t DataN(){int i= &fGlobalScaler-Data();return i;}
  	Bool_t IsSame(Int_t anIndex, const QGlobalShaper *aGlobalShaper, Int_t aPrintMode=0)const;
  ClassDef(QGlobalShaper,1);

 private:

	Short_t fBoardInfo;     // 4 bits of board info
	Int_t fGlobalScaler;    // 28 bits of scaler
    
};


#endif




