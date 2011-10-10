



/////////////////////////////////////////////////////////////////
//                                                             //
//  List of parameters, with associated utilities to update,   //
//  and compare parameter lists.  The global object plist      //
//  contains the base parameter list                           //
//  Written by F. Duncan                                       //
//  Last Updated by M. Boulay 02/04/98                         //
/////////////////////////////////////////////////////////////////
#ifndef ParList_h
#define ParList_h
#define debug_parameters 1
#define TRUE  1
#define FALSE 0
#define COMCHAR '#'
#define MAXPARAMETERS 1024
#define MAXPARAMETERSTRING 1024


#include <Rtypes.h>
#include "QParameter.h"
#include <stdio.h>
#include <assert.h>
#include <TObject.h>
#include "snoplog.h"
#include "QLog.h"
#include <stdlib.h>

const int ListSize = 1024;

extern int par_read_line (FILE * fp, char *line, int *lnum);
extern int par_find_command (FILE * fp, char *line, int *lnum);
extern int par_find_parameter (FILE * fp, char *line, int *lnum, int *pline,
			       char *parstr, char *fieldstr,
			       char *valstr);
class QParameterList:public TObject
{
  public:

  QParameterList ();		// Default constructor creates a 16 parameter list, mgb

  QParameterList (char *namestr, int sz = ListSize);
    QParameterList (char *namestr, const QParameter *, int);
    QParameterList (char *namestr, const QParameterList &);
   ~QParameterList ();

  /* added to class by mgb: (from standalone)
     int par_read_line(FILE *fp,char *line,int *lnum);
     int par_find_command(FILE *fp,char *line,int *lnum);
     int par_find_parameter(FILE *fp, char *line,int *lnum,int *pline,
     char *parstr,char *fieldstr,
     char *valstr);
   */
  void read (char *fname);	// Read in parameters from file *fname
  void Reset();
  void update (char *pstr, char *fstr, char *vstr, char *fname, int pline);
  //void Add(QParameter *param, char *parname="",double value=0.0,double error=0.0);
  void Add (QParameter * newPar);
  QParameter *find (char *name);
  void summary (FILE * fp);
//  int Equals(QParameterList Master);  
  // not implemented yet
  //  QParameterList& operator=(const QParameterList&);
  //  QParameter& operator[] (int );
  virtual const char *GetName () const
  {
    return tName;
  }
  int getSize ()
  {
    return size;
  }
  int GetNPars ()
  {
    return num;
  }				// number of defined parameters? MGB

  int Has (QParameterList * mSubList);
    ClassDef (QParameterList, 0)	//Manipulate a list of parameters 
    protected:
  // ParUtils tUtils;

    private:
  void init (char *namestr, const QParameter *, int, int);

  char name[MAXPARAMETERSTRING];
  char filename[MAXPARAMETERSTRING];
  int read_in;
  int size, num;
  QParameter *list;
  char *tName;
};


#endif
