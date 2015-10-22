//____________________________________________________
//
// QGlobalShaper
//*-- Author : Peter Skensved

#include "QGlobalShaper.h"

ClassImp(QGlobalShaper);


QGlobalShaper::QGlobalShaper()
{
  return;
}


QGlobalShaper::QGlobalShaper(Int_t *ivars)
{
	fBoardInfo = ivars[0] & 0xf;
	fGlobalScaler = ivars[1] & 0xfffffff;
	return;
}


QGlobalShaper::QGlobalShaper(const QGlobalShaper &aGlobalShaper )
{
	fBoardInfo = aGlobalShaper.fBoardInfo & 0xf;
	fGlobalScaler = aGlobalShaper.fGlobalScaler & 0xfffffff;
	return;
}

Bool_t QGlobalShaper::IsSame(Int_t anIndex, const QGlobalShaper *aGlobalShaper, Int_t aPrintMode)const{
  
  Bool_t retval=(fBoardInfo==aGlobalShaper->fBoardInfo &&
	  fGlobalScaler == aGlobalShaper->fGlobalScaler);
  if(retval &&aPrintMode)printf("GlobalShaper: %d\n",anIndex);
  return retval;
}
