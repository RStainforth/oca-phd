
#ifndef _qlog_h
#define _qlog_h

#include <TObject.h>
#include <stdio.h>
#include <time.h>

// QLog class definition
class QLog:public TObject
{

  public:
  QLog();
  QLog (char *cfile, Int_t mVerbose = 1, Int_t mLogLevel = 1);
   ~QLog ();
  void Log (char *message, int mVerbose = 1, int mLogLevel = 1);
  void SetLevel (int mLogLevel);
  void SetVerbose (int mVerbose);
    ClassDef (QLog, 0)		//Message logging utilities
    private:
  int tLogLevel;
  int tVerbose;
  FILE *LogFile;

public:  
  static QLog *sQLog;

};

void PLog (char *message, int mVerbose = 1, int mLogLevel = 1);

#endif
