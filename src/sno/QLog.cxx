////////////////////////////////////////////////////////////////////////
//Class containing some message logging utilities.                    //
//Changing the data members tLogLevel and tVerbose through the        //
//member functions SetLevel and SetVerbose affect the verbosity and   //
//output.                                                             //
// Begin_html                                                         //
//</l> SetVerbose(0)    Do not log any messages     <l>               //
//</l> SetVerbose(1)    Default.  Log most messages <l>               //
//</l> SetVerbose(2..3) Log more messages           <l>               //
//</l> SetLevel(1)   Default.  Log to file and stdout <l>             //
//</l> SetLevel(2)   Log to file only                 <l>             //
//</l> SetLevel(3)   Log to stdout only               <l>             //
// End_html                                                           //
////////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay                          


#include "QLog.h"

ClassImp (QLog)

QLog *QLog::sQLog = NULL;

QLog::QLog(){
tLogLevel = 1;
tVerbose = 1;

};
QLog::QLog (char *cfile, Int_t mVerbose, Int_t mLogLevel)
{
  //Default constructor for QLog which sets the output file to cfile.
  
  char *cfile2 = new char[100];
  strcpy(cfile2,cfile);
  //  LogFile = fopen (cfile2, "a");
  LogFile = NULL;
  tLogLevel = mLogLevel;
  tVerbose = mVerbose;
  char *mesg = new char[1000];
  time_t ltime = time( (time_t *) NULL );
  tm *ltime2 = localtime( &ltime );
  sprintf(mesg,"Log file %s entry at %s",cfile2,asctime(ltime2));
  Log(mesg,2);
  delete[] cfile2;
  delete[] mesg;
}
QLog::~QLog ()
{
};
void QLog::
SetLevel (int mLogLevel)
{
  //Set the value of tLogLevel
  tLogLevel = mLogLevel;
}
void QLog::
SetVerbose (int mVerbose)
{
  //Set the value of tVerbose.  A higher value leads to more message logging.
  tVerbose = mVerbose;
}
void QLog::
Log (char *message, Int_t mVerbose, Int_t mLogLevel)
{
  //Log the message with given parameters.
  
  if (tVerbose != 0 && mVerbose <= tVerbose)
    {
      
      if (mLogLevel == 1 || mLogLevel == 2)
	{ if (LogFile) fprintf (LogFile, "%s\n", message); }
      if (mLogLevel == 1 || mLogLevel == 3)
	printf("%s\n",message);
    }
}
void
PLog (char *message, Int_t mVerbose, Int_t mLogLevel)
{
	if (!QLog::sQLog) {
		QLog::sQLog = new QLog ("qu_sno.log",1,2);
	}
	if (QLog::sQLog) {
		QLog::sQLog->Log(message,mVerbose,mLogLevel);
	}
}

