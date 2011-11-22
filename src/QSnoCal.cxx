/////////////////////////////////////////////////////////////////////
//QSnoCal                                                          //
//Performs ECA and PCA calibration with constants from SNODB, using//
//SNOMAN-style calibration algorithms.  The calibration constants' //
//validity will be checked against the event's 10 MHz clock.       //
//                                                                 //
//Written by M. Boulay Dec 10/98                                   //
/////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay 10/12/98

#include "QSnoCal.h"

ClassImp(QSnoCal)

  //extern "C" void blow_(Int_t *, Int_t *, Int_t &, Int_t & );
QSnoCal::QSnoCal( const char *options, Int_t JulianDate, Int_t UT1 )
{
  //Constructor.  Sets the validity time stamp according to 
  //the given date and time. If options = "read" [default], calibration
  //data is read in from the file cal.root, and the calibration mode is set
  //to kStaticConstants.
  
  Int_t date, time1;
  QSNO::ConvertDate( JulianDate, UT1, 0, date, time1 );
  fDate = date;
  fTime = time1;
  Initialize();
  if (!strcmp( options, "read"))  { ReadConstants("cal.root"); fMode = kStaticConstants; }
  else if (strcmp( options,"") ) { ReadConstants(options); }
}

QSnoCal::QSnoCal( Int_t date, Int_t time1 )
{
  //Constructor
  fDate = date;
  fTime = time1;
  Initialize();
  //For now, read constants
  ReadConstants("cal.root"); fMode = kStaticConstants;
}

void QSnoCal::Initialize()
{
  //Initialize banks.
 
  fDataType = 11;
  fMode = kDynamicConstants;
  fWalkMode = 6;
  fGainMode = -1;
  //Initialize pointers to zero.
  Int_t i;
  ftslp = new QBank *[39];           //time slopes and pedestals  
  fqslp = new QBank *[39];           //charge slopes and pedestals 
  fpdst = new QBank *[39];           //PDST banks
  for (i=0; i < 39; i++)
    {
      ftslp[i] = 0;
      fqslp[i] = 0;
      fpdst[i] = 0;
    }
  fwalk = 0;                         //discriminator walk constants
  fgain = 0;                         //PMT gains
  faltwalk = 0;                      //alternate discriminator walk constants
  ftzero   = 0;                      //zero offsets
  fchcs    = 0;                      //Chcs bank
  fClient = new QSnoDB("localhost");
}

QSnoCal::~QSnoCal()
{
  //Destructor.  Deletes all banks.
  Int_t i;
  for (i=0; i < 39; i++)
    {
      delete ftslp[i];
      delete fqslp[i];
      delete fpdst[i];
    }
  delete [] ftslp;
  delete [] fqslp;
  delete fwalk;
  delete fgain;
  delete faltwalk;
  delete ftzero;
  delete fchcs;
  delete fClient;
if (this==gCalibrator) gCalibrator = NULL;
}

void QSnoCal::SetServer( const char *server){ fServer = server; fClient->SetServer( server ); }
Int_t QSnoCal::PingServer()
{
  //Check if the QSnoDBServer is alive.
  if (fClient) return fClient->PingServer();
  return -2;
}

Int_t QSnoCal::ReadConstants()
{
  //Query the database, read in all relevant banks if the version we currently have 
  //is not valid.
 fWalkMode = 1;
 fGainMode = 1;
 if ( fClient->PingServer() != 0 ) return -1;
 //returns -1 if there is no connection to the server, or 0 otherwise
 Int_t nqslp = 0, ntslp = 0, npdst = 0;
 Int_t i;
 for (i=1; i < 39; i++)
   {
     if ( fqslp[i] )
       {
	 if ( ! fqslp[i]->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants)
	   { 
	     delete fqslp[i];
	     fqslp[i] = fClient->GetBank("QSLP",i,fDate,fTime);
	   }
       }
     else  fqslp[i] = fClient->GetBank("QSLP",i,fDate,fTime);

     if ( ftslp[i] )
       {
	 if ( ! ftslp[i]->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants)
	   {
	     delete ftslp[i];
             ftslp[i] = fClient->GetBank("TSLP",i,fDate,fTime);
	   }
       }
     else ftslp[i] = fClient->GetBank("TSLP",i,fDate,fTime);

      if ( fpdst[i] )
       {
	 if ( ! fpdst[i]->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants)
	   {
	     delete fpdst[i];
             fpdst[i] = fClient->GetBank("PDST",i,fDate,fTime);
	   }
       }
     else fpdst[i] = fClient->GetBank("PDST",i,fDate,fTime);
     


     if ( fqslp[i] && fqslp[i]->IsData() ) 
       {
	 nqslp++;
       }
     if ( ftslp[i] && ftslp[i]->IsData() ) 
       {
	 ntslp++;
       }
     if ( fpdst[i] && fpdst[i]->IsData() )
       {
	 npdst++;
       }
   }
 if ( fwalk )
   {
     if ( ! fwalk->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants ) 
       {
	 delete fwalk;
	 fwalk = fClient->GetBank("WALK",1,fDate,fTime);
       }
   }
 else fwalk = fClient->GetBank("WALK",1,fDate,fTime);

 if ( fgain )
   {
     if ( ! fgain->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants) 
       {
	 delete fgain;
	 fgain = fClient->GetBank("GAIN",1,fDate,fTime);
       }
   }
 else fgain = fClient->GetBank("GAIN",1,fDate,fTime);

 // if ( faltwalk )
 // {
 //   if ( ! faltwalk->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants ) 
 //     {
 //	 delete faltwalk;
 //	 faltwalk = fClient->GetBank("WALK",6,fDate,fTime);
 //     }
 // }
 //else fwalk = fClient->GetBank("WALK",6,fDate,fTime);
 if ( faltwalk ){;}
 else faltwalk = new QBank("WALK", 6 ); 

 if ( ftzero ) {;}
 else ftzero = new QBank("TZRO",1);
 
 if ( fchcs ) {;}
 else fchcs = new QBank("CHCS",1);

 //if ( ftzero )
 // {
 //   if ( ! ftzero->IsValid(fDate,fTime,fDataType) && fMode != kStaticConstants ) 
 //     {
 //	 delete ftzero;
 //	 ftzero = fClient->GetBank("TZRO",1,fDate,fTime);
 //     }
 // }
 //else ftzero = fClient->GetBank("TZRO",1,fDate,fTime);

 printf("QSnoCal: retrieved %i QSLP banks, %i TSLP banks\n",nqslp,ntslp);
 if ( fwalk && fwalk->IsData() ) printf("Received WALK bank 1, ");
 else if ( faltwalk && faltwalk->IsData() ) printf("Received WALK bank 6, ");
 else printf("No WALK bank available, ");
 if ( fgain && fwalk->IsData() ) printf("Received GAIN bank\n");
 else printf("No GAIN bank available\n");
 return (0); 
}

Int_t QSnoCal::ReadConstants( const char *file )
{
  //Read the calibration constants (in the form of QBanks ) from
  //file, generated with QSnoCal::WriteConstants.
  //The input file will be searched for in the following order:
  //Begin_html
  //<p>
  //<l> The current working directory
  //<l> The user's HOME directory
  //<l> The QSNO_ROOT/parameters directory.
  //</p>
  //End_html

   TFile f1;
   TFile *fconst = f1.Open(file);
   char fname[1024];
   if ( fconst->IsOpen() ) 
     {
       printf("QSnoCal::ReadConstants: Using input file %s\n",file);
     }
   else
     {
       delete fconst;
       char *home = getenv("HOME");
       sprintf(fname,"%s/%s",home,file);
       fconst = f1.Open(fname,"READ");
       if (fconst->IsOpen() ) Warning("ReadConstants","Used input file %s",fname);
     }
    
   if ( ! fconst->IsOpen() )
    {
       delete fconst; 
       char *qsno = getenv("QSNO_ROOT");
       sprintf(fname,"%s/parameters/%s",qsno,file);
       fconst = f1.Open(fname,"READ");
       if (fconst->IsOpen() )Warning("ReadConstants","Used input file %s",fname);
     }

    if ( ! fconst->IsOpen() )
    {
        delete fconst; 
        char *qsno = getenv("LOCAS_ROOT");
        sprintf(fname,"%s/data/%s",qsno,file);
        fconst = f1.Open(fname,"READ");
        if (fconst->IsOpen() )Warning("ReadConstants","Used input file %s",fname);
    }

   if ( ! fconst->IsOpen() )
     {
       Error("ReadConstants","cannot open iput file %s", file );
       return (-1);
       //returns -1 if the file cannot be found, or 0 otherwise 
    }
   TObjArray *banks = (TObjArray *)fconst->Get("banks");
   Int_t i;
   for (i=1; i < 39; i++)
    {
      if (fqslp[i])  delete fqslp[i];
      fqslp[i] = (QBank *)banks->At(i-1);
    }
  for ( i = 39; i < 77; i++)
    {
      if (ftslp[i-38]) delete ftslp[i-38];
      ftslp[i-38] = (QBank *)banks->At(i-1);
    }
   for ( i = 77; i < 115; i++)
    {
      if (fpdst[i-76]) delete fpdst[i-76];
      fpdst[i-76] = (QBank *)banks->At(i-1);
    }


  if (fwalk) delete fwalk; fwalk = (QBank *)banks->At(114);
  if (fgain) delete fgain; fgain = (QBank *)banks->At(115);
  if (faltwalk) delete faltwalk; faltwalk = (QBank *)banks->At(116);
  if (ftzero)   delete ftzero;   ftzero   = (QBank *)banks->At(117);
  if (fchcs)    delete fchcs;    fchcs    = (QBank *)banks->At(118);
  for (i=0; i < 118; i++){ banks->RemoveAt(i); }
  delete banks;
  fconst->Close();
  delete fconst;

  //Force these constants
  SetMode(kStaticConstants);
  return (0);
}

Int_t QSnoCal::ReadTitles(const char *finpeca, const char *finqeca, const char *finteca, const char *fintzero, const char *finwalk, const char *finchcs)
{
  //Read all calibration banks from the titles files.
  //the inputs are the titles filenames

 fMode = kStaticConstants;
 Int_t nqslp = 0, ntslp = 0, npdst = 0;
 Int_t i;

 QTitles **tqeca = new QTitles *[40];
 QTitles **tteca = new QTitles *[40];
 QTitles **tpeca = new QTitles *[40];
 QTitles *twalk, *ttzero, *tchcs;

 for (i=1; i < 39; i++)
   {
     printf("READING ECA bank %i\n", i);
     if ( fqslp[i] ) delete fqslp[i];
     
     tqeca[i] = new QTitles((char *)finqeca,(char *)"QSLP",i);
     fqslp[i] = tqeca[i]->GetBank();

     if ( ftslp[i] ) delete ftslp[i];
       
     tteca[i] = new QTitles((char *)finteca,(char *)"TSLP",i);
     ftslp[i] = tteca[i]->GetBank();

     if ( fpdst[i] ) delete fpdst[i];
       
     tpeca[i] = new QTitles((char *)finpeca,(char *)"PDST",i);
     fpdst[i] = tpeca[i]->GetBank();
     
     if ( fqslp[i] && fqslp[i]->IsData() ) 
       {
	 nqslp++;
       }
     if ( ftslp[i] && ftslp[i]->IsData() ) 
       {
	 ntslp++;
       }
     if ( fpdst[i] && fpdst[i]->IsData() )
       {
	 npdst++;
       }
   }
 
     if ( faltwalk ) delete faltwalk;
     
     twalk = new QTitles((char *)finwalk,(char *)"WALK",6);
     faltwalk = twalk->GetBank();


     if ( ftzero ) delete ftzero;

     ttzero = new QTitles((char *)fintzero,(char *)"TZRO",1);
     ftzero = ttzero->GetBank();
       

     if ( fchcs ) delete fchcs;
     tchcs = new QTitles( (char *)finchcs, (char *)"CHCS",1);
     fchcs = tchcs->GetBank();

     //generate dummy fwalk and fgain banks
     if (fwalk) {;}
     else fwalk = new QBank("WALK",1);
     if (fgain) {;}
     else fgain = new QBank("GAIN",1); 

 printf("QSnoCal: retrieved %i QSLP banks, %i TSLP banks\n",nqslp,ntslp);
 if ( fwalk && fwalk->IsData() ) printf("Received WALK bank 1, ");
 else if ( faltwalk && faltwalk->IsData() ) printf("Received WALK bank 6, ");
 else printf("No WALK bank available, ");
 if ( fgain && fwalk->IsData() ) printf("Received GAIN bank\n");
 else printf("No GAIN bank available\n");
 return (0); 
}


Int_t QSnoCal::WriteConstants( const char *file )
{
  //Output the current set of calibration constants to file.
  TFile fconst(file,"RECREATE");
  TObjArray banks(119);
  Int_t i;
  for (i=1; i < 39; i++)
    {
      banks.AddAt( fqslp[i],i-1);
    }
  for ( i = 39; i < 77; i++)
    {
      banks.AddAt( ftslp[i-38], i-1);
    }
   for ( i = 77; i < 115; i++)
    {
      banks.AddAt( fpdst[i-76], i-1);
    }

  banks.AddAt( fwalk, 114);
  banks.AddAt( fgain, 115);
  banks.AddAt( faltwalk, 116);
  banks.AddAt( ftzero,   117);
  banks.AddAt( fchcs, 118);
  fconst.cd();
  banks.Write("banks",kSingleKey);
  fconst.Close();
  return (0);
}

Bool_t QSnoCal::IsValid( Int_t date, Int_t time1 )
{
  //Test if the current set of banks are valid for the given date and time.
  Bool_t valid = kTRUE;
  
  if (fMode == kStaticConstants) return valid;
  Int_t i;
  for ( i = 1; i < 39; i++)
    {
      if ( ! ftslp[i] || ! fqslp[i] ) return kFALSE;
      else
	{
	  if ( ! fqslp[i]->IsValid(date, time1, fDataType) ) valid = kFALSE;
	  if ( ! ftslp[i]->IsValid(date, time1, fDataType) ) valid = kFALSE;
	  if ( ! fpdst[i]->IsValid(date, time1, fDataType) ) valid = kFALSE;
	}
    }

  if ( (fWalkMode == 1 && !fwalk)  || ( fGainMode == 1 && !fgain) ) return kFALSE;
 
  //if ( ! fwalk->IsValid(date, time1, fDataType )) valid = kFALSE;
  //if ( ! fgain->IsValid(date, time1, fDataType )) valid = kFALSE;
  if ( fWalkMode >= 6 && !(ftzero) ) valid = kFALSE;
  return valid;
}

Bool_t QSnoCal::IsValidJulian( Int_t JulianDate, Int_t UT1 )
{
  //Test if the current set of banks are valid for the give Julian date.
  Int_t date, time1;
  QSNO::ConvertDate( JulianDate, UT1, 0, date, time1 );
  return IsValid( date, time1 );
}
Bool_t QSnoCal::IsValid( QEvent *event )
{
  //Test if the current set of banks are valid for the given event.
  return IsValidJulian( event->GetJulianDate(), event->GetUT1() );
}

Int_t QSnoCal::MakeValid(Int_t date, Int_t time1 )
{
  //Update banks if necessary so that the current banks are valid for this date and time.
  fDate = date;
  fTime = time1;
  Int_t irc = 0;
  if ( !IsValid( date, time1 ) ) irc = ReadConstants();
  return (irc);
}

Int_t QSnoCal::MakeValid( QEvent *event )
{
  //Update banks if necessary so that the current banks are valid for this event.
  return MakeValidJulian( event->GetJulianDate(), event->GetUT1() );
}

Int_t QSnoCal::MakeValidJulian( Int_t JulianDate, Int_t UT1 )
{
  //Update banks if necessary so that the current banks are valid for this Julian date.
  Int_t date,time1;
  QSNO::ConvertDate( JulianDate, UT1, 0, date, time1 );
  return MakeValid( date, time1 );
}

Int_t QSnoCal::Calibrate( QPMT *pmt )
{
  //Calibrate this pmt.
  if ( pmt->Getn() < KMIN_PMT_LCN || pmt->Getn() >= KMAX_PMT_LCN ) return -1;
  return ( DoECA( pmt ) || DoWalk( pmt ) || DoGain( pmt ) );
}

Int_t QSnoCal::Calibrate( QEvent *event )
{
  //Calibrate this event.
  
  if ( fMode != kStaticConstants ){ if ( !IsValid( event ) ) MakeValid( event ); }
  Int_t i, retval = 0;
  QPMT *pmt = 0;
  for (i=0; i < event->GetnPMTs(); i++)
    {
      pmt = event->GetPMT(i);
      if ( Calibrate( pmt ) ) retval = -1;
    }
  event->SetBit(kCalibrated,kTRUE);
  return ( retval );
}

Int_t QSnoCal::DoECA( QPMT *pmt )
{
  //Apply the ECA calibration to this PMT, ie subtract pedestals and apply slopes for charges,
  //interpolate for T.

  const Int_t kMaxQTSteps = 31;
  Float_t qhs,qhl,qlx, charge, pedestal;
  Float_t time_offset = 500;
  Int_t icharge;
  Int_t ncrates = 19, ncards = 16, nchannels = 32, ncells = 16;
  Int_t icr = pmt->GetCrate()+1, ica = pmt->GetCard()+1, ich = pmt->GetChannel()+1,
              ice = pmt->GetCell()+1;
  Int_t jca = ica;
  if ( jca > ncards/2 ) jca -= (ncards/2);
  Float_t slope, intercept;
  Int_t HalfCrate = eca_pattern( icr, ich, ica);
  Int_t Offset;
  Int_t i, retval = 0;
  

  //Apply ECA calibration to charges.
  //  if ( fqslp[HalfCrate] != 0 && fqslp[HalfCrate]->IsData() )
  if ( fWalkMode == 1 ) //Use standard SNOMAN routines
  {
      for (i=1; i <= 3; i++)
	{ //Loop over charges.  1=qhs, 2=qhl, 3=qlx

	  if ( i == 1 ) icharge = (Int_t)pmt->Getihs();
	  if ( i == 2 ) icharge = (Int_t)pmt->Getihl();
	  if ( i == 3 ) icharge = (Int_t)pmt->Getilx();
	  Offset    = 6 * ((ice-1)*(nchannels*ncards/2)+(ich-1)*(ncards/2)+(jca-1))+
                    2*(i-1);
	  slope     = (Float_t)fqslp[HalfCrate]->icons(Offset + 1)/100.;
	  intercept = (Float_t)fqslp[HalfCrate]->icons(Offset + 2)/10.;
	  if ( slope > 0 ) charge = 1.*((Float_t)icharge-intercept)/slope;
	  else charge = -9999.;

	  if ( i == 1 ) pmt->Seths( charge );
	  if ( i == 2 ) pmt->Sethl( charge );
	  if ( i == 3 ) pmt->Setlx( charge );
	}
  }

  //For now, we'll subtract pedestals:
  
  else if ( fpdst[HalfCrate] != 0 && fpdst[HalfCrate]->IsData() )
    {
      for (i=1; i <= 3; i++)
	{//Loop over charges  1=qhs, 2=qhl, 3=qlx

	  if ( i == 1 ) icharge = (Int_t)pmt->Getihs();
	  if ( i == 2 ) icharge = (Int_t)pmt->Getihl();
	  if ( i == 3 ) icharge = (Int_t)pmt->Getilx();
	  Offset    = 5 * ((ice-1)*(nchannels*ncards/2)+(ich-1)*(ncards/2)+(jca-1)) +1; //Pedestal offset
	  pedestal  = (Float_t)fpdst[HalfCrate]->icons(Offset + i );
	  if ( pedestal > 0 ) charge = (Float_t)icharge - pedestal;
	  else charge = -9999.;

	  if ( i == 1 ) pmt->Seths( charge );
	  if ( i == 2 ) pmt->Sethl( charge );
	  if ( i == 3 ) pmt->Setlx( charge );
	}
    }

  else
    {
      pmt->Seths( -9999 );
      pmt->Sethl( -9999 );
      pmt->Setlx( -9999 );
      retval = -1;
    }


  //Apply ECA calibration to time.
  
  if ( ftslp[HalfCrate] == 0 || ftslp[HalfCrate]->IsGap() )
    {
      //We don't have any calibration banks for this pmt.
      pmt->Sett( -9999);
      return (-1);
    }

  Int_t   itime = pmt->Getit();
  Float_t time1  = time_offset - (Float_t)itime;
  Float_t XX;
  Int_t NSteps, NWords;
  Int_t IMin, IMax, IDelMax, IDelMin,IDel,nsum,M,IGood,ILastGood;
  IMin = IMax = IDelMax = IDelMin = IDel = nsum = M = IGood = ILastGood = 0;
  Int_t IPak[kMaxQTSteps+1], IX[kMaxQTSteps+2];
  Float_t Y[kMaxQTSteps+2],YY,del,xsum,x2sum,ysum,xysum,YDel,YDelMax,YDelMin;
  Int_t IFlg[33], IQFlg[33]; //or Bool_t
  YY = del = xsum = x2sum = ysum = xysum = YDel = YDelMax = YDelMin = 0;
  NSteps = ftslp[HalfCrate]->icons(1); //Number of T samplings used
  NWords = ftslp[HalfCrate]->icons(2); //Number of packed words per cccc

  for (i=1; i <= NSteps; i++)
    {
      Y[i] = (Float_t)((ftslp[HalfCrate])->icons(2+i)) / 100.0; //Known values of T
      YDel = Y[i] - time1;
      if ( YDel >= 0 && ( IMax == 0 || YDel < YDelMax ) ) 
	{
	  YDelMax = YDel;
	  IMax = i;
	}
      else if ( YDel < 0 && (IMin == 0 || -YDel < YDelMin ) )
	{
	  YDelMin = -YDel;
	  IMin = i;
	}
    }

  Offset = 2 + NSteps +NWords*( (ice-1)*(nchannels*ncards/2) +
	 (ich - 1)*(ncards/2) + (jca - 1) );  //Offset for T-slope info for this PMT.
  
  //GetArray()[Offset+0] <-> icons(1)
  //the following should be cleaned up.
  Int_t *test1 = new Int_t[NWords];
  for (i=0; i < NWords; i++){ test1[i] = ftslp[HalfCrate]->icons(Offset+1+i); }
  blow(test1, &IQFlg[1], 32, 1); //Unpack the T-slope flags
  delete test1;
  for ( i = 2; i <= NWords; i++)
    {
      IPak[i-1] = ftslp[HalfCrate]->icons(Offset + i );
    }
  for ( i = 1; i <=NSteps; i++)
    {
      IFlg[i] = IQFlg[i];
    }

  blow(&IPak[1],&IX[1],NSteps,12); //Unpack the T-slope info (12 bit words -> Full words)
  //We've now unpacked all constants required for t-calibration.

  
  IDelMax = 5000;
  IDelMin = -5000;
  IMin = IMax = nsum = 0;
  xsum = x2sum = ysum = xysum = 0;

  IGood = 1;
  while ( IFlg[IGood] && IGood < NSteps ) IGood++;
  ILastGood = NSteps;
  while ( IFlg[ILastGood] && ILastGood > 1 ) ILastGood--;
  //IGood and ILastGood now represent the first and last 'good' sampling point


  //Bracket the data for interpolation
  if ( itime >= IX[IGood] && itime <= IX[ILastGood] )
    {
      for (i=IGood; i <=ILastGood; i++)
	{
	  if ( !IFlg[i] )
	    {
	      //Status Flag OK, carry on.
	      xsum  = xsum + (Float_t)IX[i];
	      x2sum = x2sum +(Float_t)IX[i]*IX[i];
	      ysum  = ysum + Y[i];
	      xysum = xysum + Y[i]*(Float_t)IX[i];
	      nsum++;
	      IDel = itime - IX[i];
	      if ( IDel >= 0 && IDel < IDelMax )
		{
		  IDelMax = IDel;
		  IMin = i;
		}
	      else if (IDel < 0 && IDel > IDelMin )
		{
		  IDelMin = IDel;
		  IMax = i;
		}
	    }
	}
    }
  
  //Interpolate (linearly) to get the calibrated time.
  del = (Float_t)nsum*x2sum-xsum*xsum;
  if ( IMax - IMin <= 2 && IMax >= 2 && IMin > 0 )
    {
      //Our raw time value lies between IX[IMin] and IX[IMax],
      XX = ((Float_t)IDelMax*Y[IMax] - (Float_t)IDelMin*Y[IMin] ) / 
	     ((Float_t)IDelMax - (Float_t)IDelMin);
    }
  
  //If outside of range, extrapolate
  else if ( (IMax < 2 || (IMax < 0 && IMin > 0 ) || 
	    (IMax > 0 && IMin <= 0)) && del != 0 )
    {
      intercept = (x2sum*ysum-xsum*xysum)/del;
      slope     = ((Float_t)nsum*xysum-xsum*ysum)/del;
      XX        = intercept + slope*(Float_t)itime;
    }
  else { XX= -9999; retval = -1; }
  time1 = time_offset - XX;
  if ( XX == -9999 ) time1 = XX;
  pmt->Sett ( time1 );
  return (retval);
}

Int_t QSnoCal::DoWalk( QPMT *pmt )
{
  //Apply Discriminator walk correction to this PMT.
  //pmt must already be ECA calibrated, with hs and t equal
  //to the pedestal corrected, slope adjusted values.
  //If ECA calibrated QHS is less than 0, 0.2 is used (flasher hack.)

  Int_t retval = 0;
  Float_t time1 = pmt->Gett();
  Float_t qhs = pmt->Geths();

  if ( fWalkMode == 1 ){
    //Standard PCA walk calibration
  if ( ! fwalk ) { printf("Error in QSnoCal::DoWalk. walk bank not available.\n"); pmt->Sett( -9999 ); return -1; }
  
  Int_t loc_lcn = pmt->Getn() + 1; //Pmtn shifted by 1. 
  Int_t loc_walk = 8*( loc_lcn - 1); //location of pmt in walk bank
  Float_t exp_factor; //exponential factor from walk bank.
  Float_t distance_pmt;  //correction factor for PCA calibration
  Float_t LaserX = fwalk->rcons(1), LaserY = fwalk->rcons(2), LaserZ = fwalk->rcons(3);
  
  Float_t qhl = pmt->Gethl();
  if ( fwalk->rcons( loc_walk + 16) == 0 && fwalk->rcons( loc_walk + 9 ) != 0)
    {
      //ie quality word = 0 (OK), walk parameter non-zero (OK)

      if ( pmt->Geths() < 0 ) qhs = 0.2; //flasher HACK
      exp_factor = fwalk->rcons(loc_walk+11)*((qhl/2.)-fwalk->rcons(loc_walk+10));

      //The first three words of the walk bank are the laser coordinates
      //for the PCA run.
      distance_pmt = sqrt(( pmt->GetX() - LaserX )*( pmt->GetX() - LaserX )
	           +      ( pmt->GetY() - LaserY )*( pmt->GetY() - LaserY )
                   +      ( pmt->GetZ() - LaserZ )*( pmt->GetZ() - LaserZ ));

      time1 = time1 + fwalk->rcons(loc_walk + 15 ) - fwalk->rcons(loc_walk + 9 )
	    -exp(-exp_factor) + distance_pmt/29.80 + 100.0;
    }
  else { time1 = -9999; retval = -1; }
  }

  else if (fWalkMode == 6 || fWalkMode == 8 ){
  //Apply the alternate walk-correction.
  
  Float_t TZERO, TZERO_AVG,dt;
  Int_t LCN = pmt->Getn();
  TZERO_AVG = ftzero->rcons( 1 );
  TZERO = ftzero->rcons(1 + LCN*3 + 2 );
  TZERO = TZERO - TZERO_AVG;
  Float_t pwalk_lo[6], pwalk_hi[6], Q_TMP;
  
  //retrieve the QHS pedestal
  Int_t ncrates = 19, ncards = 16, nchannels = 32, ncells = 16;
  Int_t icr = pmt->GetCrate()+1, ica = pmt->GetCard()+1, ich = pmt->GetChannel()+1,
              ice = pmt->GetCell()+1;
  Int_t jca = ica;
  if ( jca > ncards/2 ) jca -= (ncards/2);
  Int_t HalfCrate = eca_pattern( icr, ich, ica);
  Int_t Offset         = 5 * ((ice-1)*(nchannels*ncards/2)+(ich-1)*(ncards/2)+(jca-1)) +1; //Pedestal offset
  Float_t hs_pedestal  = (Float_t)fpdst[HalfCrate]->icons(Offset + 1 );
  Int_t i;


  if ( faltwalk->icons( 1 != 2 ) )
       {
	 //Use Korpach's walk correction
  Int_t walk_offset = 13 * pmt->GetCrate() + 1;
  for( i = 1; i <= 5; i++){
             pwalk_lo[i] = faltwalk->rcons(walk_offset + i);
	       }

  for( i = 1; i <=6; i++){
              pwalk_hi[i] = faltwalk->rcons(walk_offset + 6 + i );
		}

            Q_TMP = pmt->Geths();
            if (Q_TMP < -50. ) Q_TMP = 4096.0 - hs_pedestal; 
            if (Q_TMP < 0. )   Q_TMP = 0.0;
	    if (Q_TMP <= 25. ){

               dt = pwalk_lo[1] + exp(pwalk_lo[2] + pwalk_lo[3]*Q_TMP)
                   + exp( pwalk_lo[4] + pwalk_lo[5]*Q_TMP);
		 }
            else if ( Q_TMP > 25. ){
               dt = pwalk_hi[1] + exp(pwalk_hi[2] + pwalk_hi[3]*Q_TMP)
		                + exp(pwalk_hi[4] + pwalk_hi[5]*Q_TMP)
                   + pwalk_hi[6]/Q_TMP;

		 }
       }
  else
  //Use the mode 2 lookup walk correction
  {
   Int_t walk_offset = 403*pmt->GetCrate() +2;
   for (i=1; i <= 2; i++)
     {
       pwalk_hi[i] = faltwalk->rcons(walk_offset + 400 + i );
     }

   if (Q_TMP < -50. ) Q_TMP = 4096. - hs_pedestal;
   if (Q_TMP < 0. ) Q_TMP = 0.0;
   if (Q_TMP < 400. )  dt = faltwalk->rcons( walk_offset + 1 + (Int_t)Q_TMP );
   else                dt = pwalk_hi[1] + pwalk_hi[2]*(Q_TMP-400.);
  }

  if ( time1 != -9999. ) 
    {
      if (fWalkMode == 6 ) time1 = time1 -dt -TZERO;
      else                 time1 = time1 -TZERO;
    }
  }
  else
    {
      Error("DoWalk","Unknown walk mode %i",fWalkMode);
    }

  pmt->Sett( time1 );
  if ( fchcs )
    {
      Bool_t calstat = kFALSE;
      Int_t k;
      for (k=1; k <= fchcs->icons(1); k++)
	{
	  if ( fchcs->icons(k+1) == pmt->Getn() ) calstat = kTRUE;
	}
      if ( calstat ) pmt->Sett( -9999. );
    }
  return retval;
}

Int_t QSnoCal::DoGain( QPMT *pmt )
{
  //Apply PMT gain correction to this PMT.

  Int_t retval = 0;
  Float_t qhs, qhl, qlx, gain;

  if (fGainMode == 1){
    //Standard PCA gain calibration
  if ( !fgain ) 
    { 
      printf("Error in QSnoCal::DoGain: gain bank not available.\n"); 
      pmt->Seths( -9999 );
      pmt->Sethl( -9999 );
      pmt->Setlx( -9999 );
      return (-1); 
    }
 
  Int_t loc_lcn = pmt->Getn() + 1;
  Int_t loc_gain = 2*( loc_lcn - 1); //location of pmt in gain bank
  qhs = pmt->Geths(); qhl = pmt->Gethl(); qlx = pmt->Getlx(); 
  
  if ( fgain->rcons( loc_gain + 1 ) > 0 && fgain->rcons( loc_gain + 2 ) == 0 )
    {
      //Everything's OK.  Apply gain correction.
      gain = fgain->rcons( loc_gain + 1 );

      //gain is the same for the three charges.
      qhs = qhs/gain;
      qhl = qhl/gain;
      qlx = qlx/gain;
    }
  else
    {
      //Something's wrong. Flag these charges.
      qhs = -9999;
      qhl = -9999;
      qlx = -9999;
      retval = -1;
    }
  pmt->Seths( qhs );
  pmt->Sethl( qhl );
  pmt->Setlx( qlx );
  }
  else if ( fGainMode == -1 ) {;}//Do nothing
  else
  {
    Error("DoGain","Unknown gain mode %i",fGainMode); retval = -1;
  }
  return (retval);
}

void QSnoCal::blow(Int_t *source, Int_t *target, Int_t nbytes, Int_t nbits)
{
  //CERN's data unpacker routine (circa 1972!), used to unpack the TSLP and QSLP 
  //bank.  Recoded in C++ to avoid
  //at all cost linking qtree with CERNLIB.

  //source is a contiguous string of bytes, where the words of nbits are to be unpacked
  //into standard ( 32 bit on PC ) words in the array target.

  Int_t i,loc1=0,loc2=nbits-1;
  Int_t word = 0, word1 = 0, word2 = 0;
  Int_t isource = 0;
  Int_t factor = (Int_t)pow(2.0,nbits) - 1;
  for(i=0; i < nbytes; i++)
    {
      //copy bits from loc1 to loc2 in source to target[i]
      if ( loc2 < 32 )
	{
	  //all nbits are in this source word 
	  word = source[isource];
	  target[i] = (word >> ( 32 - (loc2 + 1) ) ) & factor;
	  loc1 = loc2+1;
	  loc2 += nbits;
	  if ( loc1 == 32 ) { isource++; loc1 = 0; loc2 = nbits - 1; }
	}
      else
	{
	  //our target word is split over two source words.
	  Int_t nbits1 = 32 - loc1;
	  Int_t nbits2 = nbits - nbits1;
	  word = source[isource++];
	  word1 =  word << ( nbits - nbits1 );//part of target word from source[1]
	  word = source[isource];
	  word2 =  word >> ( 32 - nbits2 );//part of target word from source[2]
	  target[i] = (word1 | ( word2& ((Int_t)pow(2.0,nbits2)-1) ) )  & factor;
	  loc1  = loc2 +1-32;
	  loc2 += nbits - 32;

	}
    }

}

Int_t QSnoCal::eca_pattern(Int_t crate, Int_t channel, Int_t card )
{
  //Routine duplicated from snoman, generates link in QSLP or TSLP bank for 
  //a given channel.
  return 2*(crate-1) + 1 + (card -1 )/8;
}


void QSnoCal::SetTzero( QBank *bank )
{
  //Set the zero offset bank.
  if (ftzero) delete ftzero;
  ftzero = bank;
}

void QSnoCal::SetChcs( QBank *bank )
{
  //Set the CHCS banks.
  if ( fchcs ) delete fchcs;
  fchcs = bank;
}

void QSnoCal::MakeSimpleConstants( const char *fout )
{
  //Generate a simple set of calibration constants compatible with Xsnoed.
  //The format of the output file is as follows:
  //
  //Pmtn(I) CCC(I) Status_Word(I) (one entry per PMT)
  //cell time_offset time_slope hl_offset hl_slope hs_offset hs_slope lx_offset lx_slope (16 entries per PMT)
  //...
 
  
  FILE *fp = fopen(fout,"w");
  QPMT pmt;
  Int_t i, j, ipmt, icell;
  Float_t t1, t2;
  Float_t x1 = 1200, x2 = 2200;

  Float_t tim_ped, tim_slope;
  Int_t    hl_ped, hs_ped, lx_ped;
  Float_t  hl_slope = 1.0, hs_slope = 1.0, lx_slope = 1.0;
  fWalkMode = 8;


  Int_t walk_offset = 1;
  if ( faltwalk->icons(1) != 2 )
    {
      //Use Korpach-style correction 
      fprintf(fp,"1 19990101 0 20500101 0\n");
      for (i=1; i <= 5; i++) { fprintf(fp,"%g ", faltwalk->rcons(walk_offset+i)); }
      fprintf(fp,"0. 0. 0. 0. 0.\n");
      for (i=1; i <=6; i++) { fprintf(fp,"%g ", faltwalk->rcons(walk_offset + 6 + i)); }
      fprintf(fp,"0. 0. 0. 0.\n");
    }
  else
    {
      walk_offset = 2;
      //Use lookup-table correction
      fprintf(fp,"2 19990101 0 20500101 0\n");
      for (j=0; j < 40; j++)
	{
	  for (i=0; i<10; i++)
	    {
	      fprintf(fp, "%.2f ",faltwalk->rcons(walk_offset+(10*j +i+1) ) );
	    }
	  fprintf(fp, "\n");
	}
      fprintf(fp,"%g %g\n",faltwalk->rcons(walk_offset+400+1), faltwalk->rcons(walk_offset+400+2));
    
    }
   for (ipmt=0; ipmt < KMAX_PMT_LCN; ipmt++)
    {
      pmt.Setn(ipmt);
      fprintf(fp,"%i %i %i\n",pmt.Getn(), pmt.GetCCC(),0);
      //fprintf(fp,"%i %i %i %i %i %i %.2f %.2f %.2f %.2f %.2f\n",pmt.Getn(), pmt.GetCCC(), 1, pmt.GetCrate(), pmt.GetCard(), pmt.GetChannel(), pmt.GetX(), pmt.GetY(), pmt.GetZ(), 0.0, 0.0, 0.0);
      for ( icell = 0; icell < 16; icell++)
	{
	  pmt.SetCell(icell);
	  pmt.Setit((Int_t)x1 );
	  Calibrate( &pmt );
	  t1 = pmt.Gett();
	  
	  pmt.Setit( (Int_t)x2 );
	  Calibrate( &pmt );
	  t2 = pmt.Gett();
	  
	  if ( t2 > -9997. && t1 > -9997. && t1 != t2 )
	    {
	      tim_slope  = (x1-x2)/(t2-t1);
	      tim_ped    = tim_slope*t1 - x1;
	    }
	  else
	    {
	      tim_slope = 0.;
	      tim_ped = 0.;
	    }
	  //retrieve the pedestals
	  Int_t ncrates = 19, ncards = 16, nchannels = 32, ncells = 16;
	  Int_t icr = pmt.GetCrate()+1, ica = pmt.GetCard()+1, ich = pmt.GetChannel()+1,
	    ice = pmt.GetCell()+1;
	  Int_t jca = ica;
	  if ( jca > ncards/2 ) jca -= (ncards/2);
	  Int_t HalfCrate = eca_pattern( icr, ich, ica);
	  Int_t Offset         = 5 * ((ice-1)*(nchannels*ncards/2)+(ich-1)*(ncards/2)+(jca-1)) +1; //Pedestal offset
	  hs_ped               = fpdst[HalfCrate]->icons(Offset + 1 );
	  hl_ped               = fpdst[HalfCrate]->icons(Offset + 2 );
	  lx_ped               = fpdst[HalfCrate]->icons(Offset + 3 );

	  fprintf(fp,"%i %.0f %.3f %i %.1f %i %.1f %i %.1f\n", pmt.GetCell(), tim_ped, tim_slope, hl_ped,hl_slope, hs_ped, hs_slope, lx_ped, lx_slope);
	}
    }

  fclose(fp);
  printf("Done.\n");

}

