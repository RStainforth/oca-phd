//////////////////////////////////////////////////////////////////
//QSNO                                                          //
//                                                              //
//  Base object containing                                      //
//  global definitions, standard message logging, etc.          //
//  Written by M. Boulay 21/05/98                               //
//////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay  21/05/98                        

#include "QSNO.h"
#include "QPMTxyz.h"
#include <string.h>
#include "TMath.h"

QBrowser *gQBrowser       = NULL;
QSnoed   *gSnoed          = NULL;
QCal     *gCal            = NULL;
QSnoCal  *gCalibrator     = NULL;
QPMTxyz  *gPMTxyz         = NULL;
QSNO     *gSNO            = new QSNO();

ClassImp (QSNO)

  //QParameterList *plist = new QParameterList;  //Global QParameterList
  
QSNO::QSNO ()
{
  //Constructor, create the global variables

}
QSNO::~QSNO ()
{
if (gSNO == this) gSNO = NULL;
}
QParameterList *QSNO::
GetGplist ()
{
  //Returns a pointer to the Global QParameterList plist.  One can then
  //access parameters in root, for example, with: 
  //Begin_html  
  // </l>root[0] gSystem.Load("QParameter.sl");<l>
  // </l>root[1] QSNO glsno;<l>
  // </l>root[2] QParameter *parlist = glsno.GetGplist();<l>
  // </l>root[3] QParameter *p1 = parlist->find("m_electron");<l>
  // </l>root[4] printf("m_e = %f +/- %f\n",p1->get_value(),p1->get_error());<l>
  //End_html

  return plist;
}

QPMTxyz * QSNO::GetPMTxyz()
{
  //Returns a pointer to the Global QPMTxyz gPMTxyz.  Creates one if it does not exist.
  if ( ! gPMTxyz ) gPMTxyz = new QPMTxyz("read");
  return gPMTxyz;
}

QBrowser *QSNO::GetgQBrowser()
{
  //Returns a pointer to the Global QBrowser gQBrowser.
  return gQBrowser;
}


Int_t QSNO::GetJulianDate(Int_t d, Int_t m, Int_t y )
{
  //Convert from day, month, year to Julian Date, defined as the 
  //number of days which have passed since Jan 1, 1975.

   Int_t jd;

   jd =     ( 1461 * ( y + 4800 + ( m - 14 ) / 12 ) ) / 4 +   ( 367 * ( m - 2 - 12 * ( ( m - 14 ) / 12 ) ) ) / 12 - 
             (3 * ( ( y + 4900 + ( m - 14 ) / 12 ) / 100 ) ) / 4 + d - 32075;
   //  printf("%i-%i-%i julian date is  %i\n",d,m,y,jd);
   //jd is now the number of days since jan 1, 4713 BC
   //in these units, Jan 1 1975 is  2442414
   return jd - 2442413; //the one is required for agreement with the snoman JDY.
}

void QSNO::GetDate( Int_t JulianDate, Int_t &d, Int_t &m, Int_t &y )
{
  //Convert the given Julian date to Gregorian.
  Int_t l,n,i,j;
  Int_t jd = JulianDate;
  jd += 2442413; // ie w.r.t. -4713 

         l = jd + 68569;
        n = ( 4 * l ) / 146097;
        l = l - ( 146097 * n + 3 ) / 4;
        i = ( 4000 * ( l + 1 ) ) / 1461001;
        l = l - ( 1461 * i ) / 4 + 31;
        j = ( 80 * l ) / 2447;
        d = l - ( 2447 * j ) / 80;
        l = j / 11;
        m = j + 2 - ( 12 * l );
        y = 100 * ( n - 49 ) + i + l;

}

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
void QSNO::GetFileName(const char *aDirectory, const int &aRunNumber, char *aFileName,const char aFormat[]){
  //Routine to find standard autosno filename in a directory.  This routine returns the filename with the largest pass number.
  //aDirectory and aRunNumber are inputs; aFileName is returned with the complete, directory included filename. 
  //aFormat (default "SNO_0%d_p%d.root") is a scanf format statement, that can be specified in order to use to routine to 
  //look for other pass-inclusive file names- for instance, rch files. 
  DIR *adir=opendir(aDirectory);
  dirent*anentry;
  int pass,pass0,run;
  aFileName[0]=0;
  char line[256];
  if(adir==NULL)return;  
  for(pass0=-1, anentry=readdir(adir);anentry>0;){
    //    if(sscanf(anentry->d_name,"SNO_0%d_p%d.root",&run, &pass)==2){
    if(sscanf(anentry->d_name,aFormat,&run, &pass)==2){  //default aFormat is SNO_0%d_p%d.root
      if(run==aRunNumber&&pass>pass0){
	pass0=pass;
	strcpy(line,anentry->d_name);
      }
    }
    anentry=readdir(adir);
    
  }
  closedir(adir);
  if(pass0==-1)return;  // no files found
  // file found- add directory name in front and return.
  strcpy(aFileName,aDirectory);
  char *strptr=aFileName+strlen(aFileName)-1;
  while (*strptr=='/')strptr--; // remove all trailing slashes 
  strptr++; *strptr='/';  // add a single trailing slash
  strptr++;
  strcpy(strptr,line);
  return;  
  
}


void QSNO::GetTime(Int_t UT1, Int_t UT2, Int_t &hh, Int_t &mm, Int_t &ss )
{
  //Convert UT1 and UT2 to a time in hh mm ss (UTC time).
  //UT1 is time (in seconds) since the beginning of the day; ie midnight.
  //UT2 is time (in nanoseconds) since beginning of last second, reset every second.
  hh = UT1/3600;
  mm = (UT1%3600)/60;
  ss = (UT1%3600)%60;
}

void QSNO::ConvertDate( Int_t JulianDate, Int_t UT1, Int_t UT2, Int_t & date, Int_t & time)
{
  //Convert Julian Date, UT1, UT2 into date (yyyymmdd) and time (hhmmsscc). Returns UTC time.
  
  Int_t year,month,day,hh,mm,ss;
  GetDate( JulianDate, day,month,year);
  GetTime( UT1, UT2, hh, mm, ss);
  date = year*10000 + month*100 + day;
  time = hh*1000000 + mm *10000 + ss*100 + 00;
}

void QSNO::Unix2Julian( Double_t UnixTime, Int_t & JulianDate, Int_t & UT1, Int_t & UT2)
{
  // Convert Unix time to SNO's Julian date, UT1 and UT2.
  // Calibrated by hand based on Unix time in run_info.log, coupled with
  // QSNO::ConvertDate(), to get the validity dates in titles files. 
  // UT2 is filled, but has no impact in QSNO::ConvertDate().
  // -- rsd 2003.08.14

  Int_t UnixInt = (Int_t)floor(UnixTime); // Int_t is good until 2038 
  Double_t UnixFrac = UnixTime - (Double_t)UnixInt;

  // Unix time zero is Jan 1 1970.  SNO JD time zero is Jan 1 1975.
  const Int_t calibration = 157680000;
  Int_t UnixCalib = UnixInt - calibration;
  
  const Int_t secondsinday = 86400;
  JulianDate = UnixCalib / secondsinday;
  UT1 = UnixCalib % secondsinday;
  // UT2 is in nanoseconds...split 1E9 up to get accuracy to Int_t precision.
  UT2 = (Int_t)(floor(UnixFrac*1E7 + 0.5)*1E2); // in nanoseconds

//  cout << UnixTime << " " << UnixInt << " " << UnixFrac << " " << UnixCalib << endl;
//  cout << JulianDate << " " << UT1 << " " << UT2 << endl;
}

Double_t QSNO::Julian2Unix(Int_t JulianDate, Int_t UT1, Int_t UT2)
{
    // Unix time zero is Jan 1 1970.  SNO JD time zero is Jan 1 1975.
    // (This is 5 * 365 days worth of seconds.  Note that 1972 is a leap year
    //  so there would be an extra day except that Julian day 0 is actually
    //  Dec 31 1974 so there is one less day on the end - PH)
    const Double_t calibration = 157680000;
    const Double_t secondsinday = 86400;
    return(JulianDate * secondsinday + calibration + UT1 + UT2 * 1e-9);
}

TVector3 QSNO::SolarDirection( Int_t JulianDate, Int_t UT1, Int_t UT2, Float_t labtwist )
{
// This routine returns a vector from the detector origin to the sun, in local
// detector coordinates. 

    Float_t mJulianDay = (Float_t)JulianDate + (Float_t)UT1/86400. +
                        (Float_t)UT2/(1e9*86400.);  
    TRotation mRotator;
    Float_t mOmega=-2*TMath::Pi();
    Float_t mLongitude=(81+12./60+5./3600)*TMath::Pi()/180;
    Float_t mLatitude=(90-(46+28./60+31.0/3600))*TMath::Pi()/180;
    Float_t mLabtwist=labtwist*TMath::Pi()/180;

    Float_t stheta = sin (M_PI/180*23.5)*cos(2*M_PI/365.2425 * (mJulianDay+194));
    Float_t ctheta=sqrt(1-stheta*stheta);
    TVector3 mSun(0,ctheta,stheta);
    Float_t mTime=mJulianDay-floor(mJulianDay);   
    mRotator.RotateZ(mTime*mOmega+mLongitude);
    mRotator.RotateX(-mLatitude); // rotate up to Sudbury
    mRotator.RotateZ(mLabtwist);  // rotate to construction North
    TVector3 mDir=mRotator*mSun;
    return mDir; 
}

TVector3 QSNO::SolarDirectionPH( Int_t JulianDate, Int_t UT1, Int_t UT2, Float_t labtwist )
{
    // Calculates the direction to the sun in detector coordinates
    // for the given time in UTC with zero time of Jan 0,1975.
    // (ref: http://hotel04.ausys.se/pausch/comp/ppcomp.html)
    //
    // Verification: This method agrees with US Navy data to within
    // 0.6 degrees between 1990 and 2020, and agrees with the SNOMAN
    // calculation to within 0.03 degrees for the year 2000.
    // (US Navy data from http://aa.usno.navy.mil/AA/data/docs/AltAz.html)
    //
    // - P. Harvey 06/16/00

    // constant to convert to radians
    const Double_t to_rad = TMath::Pi() / 180;
    
    // offset in days from SNOMAN time zero (Jan 0,1975) to time zero
    // for the astronomical constants (Jan 0,2000)
    const Double_t days_offset = 9131;

    // the number of days since Jan 0, 2000 (time zero for astronomical constants)
    Double_t days = JulianDate + UT1/86400.0 + UT2/(1e9*86400.0) - days_offset;
    // the obliquity of the ecliptic
    Double_t ecl = (23.4393 - 3.563E-7 * days) * to_rad;
    // the argument of perihelion
    Double_t w = (282.9404 + 4.70935E-5 * days) * to_rad;
    // the eccentricity of earth's orbit
    Double_t e = 0.016709 - 1.151E-9 * days;
    // the mean anomaly
    Double_t ma = (356.0470 + 0.9856002585 * days) * to_rad;
    // the eccentric anomaly
    Double_t ea = ma + e * sin(ma) * (1.0 + e * cos(ma));
    
    // the direction to the sun relative to the major axis of the elliptical orbit
    Double_t xv = cos(ea) - e;
    Double_t yv = sqrt(1.0 - e*e) * sin(ea);

    // the true anomaly (angle between position and perihelion)
    Double_t v = atan2(yv, xv);

    // the Sun's true longitude in ecliptic rectangular geocentric coordinates
    Double_t sun_longitude = v + w;

    // calculate the direction to sun including earth tilt
    // (x is east, y is north, z is away from the earth)
    Double_t xs = cos(sun_longitude);
    Double_t ys = sin(sun_longitude);
    TVector3 vec1(ys*cos(ecl), ys*sin(ecl), xs);
    
    // calculate rotation of the earth about it's axis for this time of day
    // (k0 is empirically generated to agree with US Navy data - PH 06/16/00)
    Double_t k0 = 0.27499;               // rotational position at time zero
    Double_t k1 = 1.0 + 1.0 / 365.2425;  // rotational period of the earth in days
    Double_t spin = k0 + k1 * days;      // spin of the earth (number of revolutions)
    // compute the spin angle (reducing to the range -2*PI to 2*PI)
    Double_t spin_angle = (spin - (long)spin) * 2 * TMath::Pi();

    Double_t longitude = (81+12./60+5./3600) * to_rad;
    Double_t latitude  = (46+28./60+31.0/3600) * to_rad;

    TRotation rotator;
    rotator.RotateY(longitude - spin_angle); // rotate to longitude and time of day
    rotator.RotateX(latitude);               // rotate up to Sudbury
    rotator.RotateZ(labtwist * to_rad);      // rotate to construction North
    TVector3 sun_dir = rotator * vec1;

    return sun_dir; 
}

Double_t QSNO::GetSunriseTime(Int_t JulianDate, Int_t UT1, Int_t UT2)
{
    // Get next sunrise time after specified time
    // Returns Unix UTC time
    
    Double_t t = Julian2Unix(JulianDate, UT1, UT2);
    return GetSunTime(t, 0.0, 1);
}

Double_t QSNO::GetSunsetTime(Int_t JulianDate, Int_t UT1, Int_t UT2)
{
    // Get next sunset time after specified time
    // Returns Unix UTC time
    
    Double_t t = Julian2Unix(JulianDate, UT1, UT2);
    return GetSunTime(t, 0.0, -1);
}

Double_t QSNO::GetSunTime(Double_t start, Double_t z, Int_t dir)
{
    // Get Unix UTC time when sun z component equals specified z,
    // and direction of sun motion is upwards (dir=1) or downwards(dir=-1)
    // (this simple crossing algorithm doesn't work if z is close to +1 or -1)
    
    Double_t t0 = start;
    Double_t error = 1e-9;  // want nanosecond accuracy
    
    // do a coarse scan (3 hour intervals) to get a time before the specified z
    Int_t jday, ut1, ut2;
    Double_t z0, z1, t1;
    TVector3 vec;
    for (;;) {
        Unix2Julian(t0, jday, ut1, ut2);
        vec = SolarDirectionPH(jday, ut1, ut2);
        z0 = vec[2];
        if (dir * (z - z0) > 0) break;
        t0 += 10800;
    }
    // do a coarse scan (3 hour intervals) to get a time after the specified z
    for (;;) {
        t1 = t0 + 10800;
        Unix2Julian(t1, jday, ut1, ut2);
        vec = SolarDirectionPH(jday, ut1, ut2);
        z1 = vec[2];
        if (dir * (z - z1) < 0) break;
        t0 = t1;
        z0 = z1;
    }
    // do linear interpolation to find the crossing time
    Double_t tt;
    for (;;) {
        tt = t0 + (t1 - t0) * (z - z0) / (z1 - z0);
        Unix2Julian(tt, jday, ut1, ut2);
        vec = SolarDirectionPH(jday, ut1, ut2);
        Double_t zt = vec[2];
        if (dir * (z - zt) > 0) {
            if (fabs(t0-tt) < error) break;
            t0 = tt;    // we are before the crossing
            z0 = zt;
        } else {
            if (fabs(t1-tt) < error) break;
            t1 = tt;    // we are after the crossing
            z1 = zt;
        }
    }
    return tt;          // return our best estimate of the time
}


Int_t QSNO::ReadDQXX( const char *filename )
{
  //Read the DQXX titles bank from file and set them as the default for QPMTxyz.
  return GetPMTxyz()->ReadDQXX( filename );
}

Int_t QSNO::GetSnomanTitles( const char *aFilename, char *aFullPath )
{
  //Given a titles file name (aFilename), returns the full path
  //qualified file name by searching the following directories:
  //(i)   Current working directory
  //(ii)  SNO_ROOT/snoman/SNO_SNOMAN_VER/prod
  //(iii) QSNO_ROOT/parameters

  if ( !aFilename || !aFullPath ) 
    {
      Warning("GetSnomanTitles","Testing empty filename!!");
      return 0;
    }

  FILE *aFileExists = NULL;
  aFileExists = fopen(aFilename,"r");
  Int_t retval = 0;
  if ( aFileExists )
    {
      strcpy(aFullPath,aFilename);
    }
  else
    {
      //Try SNO_ROOT/snoman/SNO_SNOMAN_VER/prod
      char *sno_root = getenv("SNO_ROOT");
      char *sno_ver  = getenv("SNO_SNOMAN_VER");
      if ( sno_root && sno_ver )
	{
	  sprintf(aFullPath,"%s/snoman/%s/prod/%s",sno_root,sno_ver,aFilename);
	  aFileExists = fopen(aFullPath,"r");
	}
      
    }

  if ( !aFileExists )
    {
      //Try QSNO_ROOT/parameters
      char *qsno_root = getenv("QSNO_ROOT");
      if ( qsno_root )
	{
	  sprintf(aFullPath,"%s/parameters/%s",qsno_root,aFilename);
	  aFileExists = fopen(aFullPath,"r");
	}
    }
    
    if ( !aFileExists )
    {
        //Try QSNO_ROOT/parameters
        char *qsno_root = getenv("LOCAS_ROOT");
        if ( qsno_root )
        {
            sprintf(aFullPath,"%s/data/%s",qsno_root,aFilename);
            aFileExists = fopen(aFullPath,"r");
        }
    }

    
  if ( aFileExists ) 
    {
      retval = 1;
      fclose(aFileExists);
    }

  return retval;
}
