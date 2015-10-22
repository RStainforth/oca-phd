#ifndef _QSNO_h
#define _QSNO_h

#include <Rtypes.h>
#include <TObject.h>
#include "QGlobals.h"
#include "snoplog.h"
#include "TVector3.h"
#include "TRotation.h"

class QSNO :  public TObject
{
  public:
  QSNO ();
  virtual ~QSNO();
  QBrowser *GetgQBrowser();
  QPMTxyz *GetPMTxyz();
  Int_t ReadDQXX( const char *filename );
  Int_t GetJulianDate( Int_t day, Int_t month, Int_t year );
  Int_t GetSnomanTitles(const char *aFilename, char *aFullPath);
  void GetDate( Int_t julian, Int_t &d, Int_t &m, Int_t &y );
  void GetFileName(const char *aDirectory, const int &aRunNumber, char *aFileName,const char *aFormat="SNO_0%d_p%d.root");
  void GetTime( Int_t UT1, Int_t UT2, Int_t &hh, Int_t &mm, Int_t &ss );
  void ConvertDate( Int_t JulianDate, Int_t UT1, Int_t UT2, Int_t &date, Int_t &time);
  void Unix2Julian( Double_t UnixTime, Int_t &JulianDate, Int_t &UT1, Int_t &UT2);
  Double_t Julian2Unix(Int_t JulianDate, Int_t UT1, Int_t UT2);
  TVector3 SolarDirection( Int_t JulianDate, Int_t UT1, Int_t UT2, Float_t labtwist=-49.58 );
  TVector3 SolarDirectionPH( Int_t JulianDate, Int_t UT1, Int_t UT2, Float_t labtwist=-49.58 );
  Double_t GetSunriseTime(Int_t JulianDate, Int_t UT1, Int_t UT2);
  Double_t GetSunsetTime(Int_t JulianDate, Int_t UT1, Int_t UT2);
  Double_t GetSunTime(Double_t start, Double_t z, Int_t dir);
  ClassDef (QSNO, 1)		//Base with global parameter and logging utils

};

#endif

