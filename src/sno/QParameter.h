


#ifndef __par_h
#define __par_h
//extern "C" {
//#include <stdio.h>
//};
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <Rtypes.h>
#include <TObject.h>
//#include "qusno.h"
//#include "QLog.h"
#define debug_parameters 1
#define TRUE  1
#define FALSE 0
#define COMCHAR '#'
#define MAXPARAMETERS 1024
#define MAXPARAMETERSTRING 1024

enum Error_type
  {
    undefined, one_sigma, two_sigma, three_sigma, fwhm
  };
enum State
  {
    state_fixed, variable
  };

class QParameter:public TObject
  {				//QParameter class

    public:
    QParameter ();		//Default constructor with no arguments.

    QParameter (char *namestring);	//QParameter constructor with name.

     ~QParameter ();		//Destructor.

    void set_name (char *mName)
    {
      strcpy (name, mName);
    }
    char *get_name ()
    {
      return name;
    }

    double value;		//Numerical value of QParameter      

    int value_set;		//Flag describing value_set (?)

    double error;		//Numerical value of error in QParameter

    int error_set;		//Flag describing error_set (?)

    char units[132];		//Character string describing units 

    int units_set;		//Flag describing units_set (?)

    Error_type error_type;	//Type of error

    State state;		//State of QParameter

    char reference[MAXPARAMETERSTRING];		//Reference for value of parameter

    int reference_set;		//Flag describing reference_set



    void init (char *namestring);
    void print (int level);
    double get_value ();
    double set_value (double val);
    double get_error ();
    double set_error (double err);
    //char *get_name();

    int update_value (char *str, char *errorstr);
    int update_error (char *str, char *errorstr);
    int update_error_type (char *str, char *errorstr);
    int update_units (char *str, char *errorstr);
    int update_reference (char *valstr, char *errorstr);

      ClassDef (QParameter, 0)	//Parameter object
      protected:
      private:
    char *name;			//QParameter name

  };



//---------------------------------------------------------
/* orphans 

   QParameter *parameter(char *namestring);
   static QParameter *parameter_list[MAXPARAMETERS];
   static int       number_of_parameters=0;
   int parameters_read(char *filename);
   void parameters_summary(FILE *fp);
   int par_read_line(FILE *fp,char *line,int *lnum);
   int par_find_command(FILE *fp,char *line,int *lnum);
   int par_find_parameter(FILE *fp,char *line,int *lnum,int *pline,
   char *parstr,char *fieldstr,char *valstr);
   void par_get_parameter_string(char *line,int *lnum,char *parstr,
   int *istart,int *istop);
   void par_get_field_string(char *line,int *lnum,char *fieldstr,
   int *iparstop, int *ifldstart, int *ifldstop);
   void par_get_value_string(FILE *fp,char *line,int *lnum,
   char *valstr,int i_equals);
   void par_update(char *parstr,char *fieldstr,char *valuestr,
   char *filename,int pline);
   QParameter *par_find(char *parname);

   // *************************************
   // Define global QParameterList plist


   // QParameterList plist("SNO parameters", 16);
 */
#endif
