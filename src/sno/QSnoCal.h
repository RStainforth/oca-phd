#ifndef __QSNOCAL_H
#define __QSNOCAL_H

//*--Author: Mark Boulay

class QSnoDB;

#include "QSNO.h"
#include "QCal.h"
#include "QBank.h"
#include "QPMT.h"
#include "QEvent.h"
#include <TFile.h>
#include <TObjArray.h>
#include "QSnoDB.h" 
#include "QTitles.h"

class QPMT;
class QEvent;

class QSnoCal : public QCal
{
 public:
  QSnoCal( const char *options, Int_t JulianDate = 0, Int_t UT1 = 0);
  QSnoCal( Int_t date = 19980815, Int_t time = 0);
  virtual ~QSnoCal();
  Int_t DoECA(QPMT *pmt);
  Int_t DoWalk(QPMT *pmt);
  Int_t DoGain(QPMT *pmt);
  Int_t ReadConstants();
  Int_t ReadConstants(const char *file);
  Int_t ReadTitles(const char*, const char*, const char*, const char*, const char*, const char *);
  Int_t WriteConstants(const char *file);
  Int_t Calibrate( QPMT *pmt );
  Int_t Calibrate( QEvent *event);
  void SetServer( const char *server );
  Int_t MakeValid( Int_t date, Int_t time );
  Int_t MakeValid( QEvent *event );
  Int_t MakeValidJulian(Int_t JulianDate, Int_t UT1 );
  Bool_t IsValid( Int_t date, Int_t time );
  Bool_t IsValid( QEvent *event );
  Bool_t IsValidJulian( Int_t JulianDate, Int_t UT1 );
  Int_t PingServer();
  void SetMode( Int_t mode ){ fMode = mode; }
  Int_t GetMode(){ return fMode; }
  Int_t Sett( QPMT *pmt ) { Calibrate( pmt ); return (0); }
  Int_t Sethl( QPMT *pmt ){ Calibrate( pmt ); return (0); }
  Int_t Seths( QPMT *pmt ){ Calibrate( pmt ); return (0); }
  Int_t Setlx( QPMT *pmt ){ Calibrate( pmt ); return (0); }

  QBank *GetTslp(Int_t i){ if ( i > 0 && i < 39 ) return ftslp[i]; else return 0; }
  QBank *GetQslp(Int_t i){ if ( i > 0 && i < 39 ) return fqslp[i]; else return 0; }
  QBank *GetPdst(Int_t i){ if ( i > 0 && i < 39 ) return fpdst[i]; else return 0; }
  
  QBank *GetWalk(){ return fwalk; }
  QBank *GetGain(){ return fgain; }
  QBank *GetAltWalk() { return faltwalk; }
  QBank *GetTzero() { return ftzero; }
  QBank *GetChcs() {  return fchcs; }
  void SetTzero( QBank *bank );
  void SetChcs( QBank *bank );
  void SetWalkMode(Int_t walk){ fWalkMode = walk; }
  void MakeSimpleConstants( const char *outfile );
  ClassDef(QSnoCal,0)//Standard ECA and PCA calibration

 protected:
  QBank **fqslp;   //Charge slopes and pedestals
  QBank **ftslp;   //Time slopes and pedestals
  QBank **fpdst;   //PDST banks
  QBank *fwalk;    //Discriminator walk constants
  QBank *fgain;    //PMT gains
  QBank *faltwalk; //Alternate discrimator walk constants
  QBank *ftzero;   //Zero offsets for each channel
  QBank *fchcs;    //Chcs bank
  Int_t fDate;     //Validity date (ddmmyyyy)
  Int_t fTime;     //Validity time (hhmmsscc)
  Int_t fDataType; //Date type ( = 11 for detector data )

  Int_t fMode;     //Update mode = static or dynamic
  Int_t fWalkMode; //Walk constants to use.
  Int_t fGainMode; //Gain constants to use.
  QSnoDB *fClient; //SNODB client
  TString fServer; //SNODB server address
  
  void Initialize();
  void blow(Int_t *source, Int_t *target, Int_t nbytes, Int_t nbits);
  Int_t eca_pattern(Int_t crate, Int_t channel ,Int_t card);
};

#endif







