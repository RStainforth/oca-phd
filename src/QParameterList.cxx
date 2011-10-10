#include "QParameterList.h"
#include <string.h>

ClassImp (QParameterList)
     QParameterList *plist = new QParameterList ();
//--------------------------------------------------------------------
// 
// QParameter List; list of parameters with utilities.
// Written by F. Duncan circa (1998), Updated by M. Boulay 98/05/05

QParameterList::QParameterList ()
{
  // Default constructor, creates a QParList with name SNO parameters
  // and reads the file parameter.list.

 
  init ("SNO parameters", 0, 100, 0);
  //read("{$QSNOROOT}/parameters/parameter.list");
  
  char *pardat = getenv("QOCA_ROOT");
  
  char pardatf[256];
  strcpy(pardatf,pardat);
  strcat(pardatf,"/data/parameter.list");
 
  read (pardatf);
}

QParameterList::QParameterList (char *namestr, int sz)
{
  //Constructor for a list of size sz with name namestr
  init (namestr, 0, sz, 0);
}

QParameterList::QParameterList (char *namestr, const QParameter * array, int sz)
{
  //Constructor for a list of size sz to be created from
  //a QParameter array.
  init (namestr, array, sz, 0);
}

QParameterList::QParameterList (char *namestr, const QParameterList & pl)
{
  //Constructor for a list from list pl.
  init (namestr, pl.list, pl.size, pl.num);
}

QParameterList::~QParameterList ()
{
  //Destructor 
  delete [] tName;
  delete [] list;
};

void QParameterList::init (char *namestr, const QParameter * array, int sz, int n)
{
  //Initialize the list with a QParameter array.
  tName = new char[100];
  strcpy (tName, namestr);
  //strcpy(name,namestr);
  num = n;
  list = new QParameter[size = sz];
  assert (list != 0);

  for (int i = 0; i < size; ++i)
    {
      if (array != 0)
	list[i] = array[i];
    }

  read_in = 0;

}

void QParameterList::
read (char *fname)
{

 
  //Read in a QParameterList from file fname
  FILE *fp;
  char line[MAXPARAMETERSTRING], parstr[MAXPARAMETERSTRING], fieldstr[MAXPARAMETERSTRING],
    valuestr[MAXPARAMETERSTRING];
  int line_number = 0;
  int *lnum, pline;
  int found_line = 0;
  
  if (debug_parameters > 0)
    {
      char mesg[100];
      sprintf (mesg, "QParameterList::read(%s)\n", fname);
      
      PLog (mesg, 3);
      //printf("plog:%i\n",plog);
    }
 
  if ((fp = fopen (fname, "r")) == NULL)
    {
      char mesg[100];
      sprintf (mesg, "QParameter ERROR: can't open file %s\n", fname);
      PLog (mesg, 1);
      exit (1);
    }
  
  strcpy(filename, fname);
  
  read_in = 1;

  lnum = &line_number;
 
  while (found_line = par_read_line (fp, line, lnum))
    {
     
      if (par_find_command (fp, line, lnum))
	{

	}
      else if (par_find_parameter (fp, line, lnum, &pline,
				   parstr, fieldstr, valuestr))
	{
	  update (parstr, fieldstr, valuestr, fname, pline);
	}
    }

  fclose (fp);
 
}


void QParameterList::
update (char *pstr, char *fstr, char *vstr, char *file,
	int pline)
{
  //Update the list (extensive logging)
  QParameter *par;
  char errorstr[MAXPARAMETERSTRING];

  if (debug_parameters)
    {
      char mesg[100];
      PLog ("QParameterList::update:", 3);
      sprintf (mesg, "  -- pstr  =<%s>\n", pstr);
      PLog (mesg, 3);
      sprintf (mesg, "  -- fstr=<%s>\n", fstr);
      PLog (mesg, 3);
      sprintf (mesg, "  -- vstr=<%s>\n", vstr);
      PLog (mesg, 3);
    }
  if ((par = find (pstr)) == 0)
    {
      char mesg[100];
      sprintf (mesg, "  -- parameter %s does not exist, definining new parameter\n",
	       pstr);
      PLog (mesg, 3);
      par = &(list[num]);
      par->init (pstr);
      num++;
    }
  if ((fstr[0] == '\0') || (!strcmp (fstr, "value")))
    {
      if (!par->update_value (vstr, errorstr))
	{
	  char mesg[100];
	  sprintf (mesg,
	    "QParameter ERROR: while parsing value string for parameter\n");
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s in file %s at line %d\n",
		   pstr, file, pline);
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s\n", errorstr);
	  PLog (mesg, 1);
	  exit (1);
	}
    }
  else if (!strcmp (fstr, "error"))
    {
      if (!par->update_error (vstr, errorstr))
	{
	  char mesg[100];
	  sprintf (mesg,
	    "QParameter ERROR: while parsing error string for parameter\n");
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s in file %s at line %d\n",
		   pstr, file, pline);
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s\n", errorstr);
	  PLog (mesg, 1);
	  exit (1);
	}
      int par_read_line (FILE * fp, char *line, int *lnum);
      int par_find_command (FILE * fp, char *line, int *lnum);
      int par_find_parameter (FILE * fp, char *line, int *lnum, int *pline,
			      char *parstr, char *fieldstr,
			      char *valstr);
    }
  else if (!strcmp (fstr, "error_type"))
    {
      if (!par->update_error_type (vstr, errorstr))
	{
	  char mesg[100];
	  sprintf (mesg,
		   "parameter ERROR: while parsing error_type string for parameter\n");
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s in file %s at line %d\n",
		   pstr, file, pline);
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s\n", errorstr);
	  PLog (mesg, 1);
	  exit (1);
	}
    }
  else if (!strcmp (fstr, "units"))
    {
      if (!par->update_units (vstr, errorstr))
	{
	  char mesg[100];
	  sprintf (mesg,
	    "QParameter ERROR: while parsing units string for parameter\n");
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s in file %s at line %d\n",
		   pstr, file, pline);
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s\n", errorstr);
	  PLog (mesg, 1);
	  exit (1);
	}
    }
  else if (!strcmp (fstr, "reference"))
    {
      if (!par->update_reference (vstr, errorstr))
	{
	  char mesg[100];
	  sprintf (mesg,
		   "QParameter ERROR: while parsing reference string for parameter\n");
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s in file %s at line %d\n",
		   pstr, file, pline);
	  PLog (mesg, 1);
	  sprintf (mesg, "                 %s\n", errorstr);
	  PLog (mesg, 1);
	  exit (1);
	}
    }
  else
    {
      char mesg[100];
      sprintf (mesg, "QParameter ERROR: field %s is not defined for parameter\n",
	       fstr);
      PLog (mesg, 1);
      sprintf (mesg, "                 %s in file %s at line %d\n",
	       pstr, file, pline);
      PLog (mesg, 1);
      exit (1);
    }
  //printf("QParameterList::update: par->value=%lg\n",par->get_value());
  //printf("            par->error=%lg\n",par->get_error());
}


QParameter *QParameterList::
find (char *parname)
{
  //Returns a pointer to the first QParameter with name parname
  //in the list.  If no such parameter can be found, returns NULL.
  int i;

  for (i = 0; i < num; i++)
    {
      if (!strcmp (list[i].get_name (), parname))
	return (&(list[i]));
    }
  return (0);
}


// summary
// prints summary of all defined parameters in list to file fp
//
void QParameterList::
summary (FILE * fp)
{
  //Write summary of list to file *fp.
  QParameter *p;
  int i;

  fprintf (fp, "\n  QParameter List %s\n", name);
  if (read_in)
    {
      fprintf (fp, "  read from file:%s\n", filename);
    }
  fprintf (fp, "  %d parameters defined\n", num);
  fprintf (fp, "--------------------------------------------------------\n");
  fprintf (fp, "%-20s %-15s %-10s %-10s",
	   "QParameter", "value", "error", "units");
  fprintf (fp, "\n");
  for (i = 0; i < num; i++)
    {
      p = &(list[i]);
      fprintf (fp, "%-20s", p->get_name ());
      // value
      if (p->value_set == FALSE)
	{
	  fprintf (fp, " %-15s", "undefined");
	}
      else
	{
	  fprintf (fp, " %-15.8lg", p->value);
	}

      // error
      if (p->error_set == FALSE)
	{
	  fprintf (fp, " %-10s", "undefined");
	}
      else
	{
	  fprintf (fp, " %-10lg", p->error);
	}

      // units
      if (p->units_set == FALSE)
	{
	  fprintf (fp, " %-10s", "undefined");
	}
      else
	{
	  fprintf (fp, " %-10s", p->units);
	}

      fprintf (fp, "\n");
    }
  fprintf (fp, "--------------------------------------------------------\n");
}

int QParameterList::
Has (QParameterList * mSubList)
{
  //Determine if mSubList is contained in the list, with
  //all parameters equal.  Currently only checks the numerical value
  //of the parameters.

  int i, mHas = 1;
  char *mName;
  QParameter *mPar;
  for (i = 0; i < mSubList->GetNPars (); i++)
    {
      char mesg[100];
      //sprintf(mesg,"%i pars in sublist\n",mSubList->GetNPars());
      // PLog(mesg,3);
      mPar = &mSubList->list[i];
      //sprintf(mesg,"searching list for name, val=%f\n",mPar->get_value());
      //PLog(mesg,3);
      mName = mPar->get_name ();
      //sprintf(mesg,"name:%s\n",mName);
      //PLog(mesg,3);
      //
      if (find (mName) == (QParameter *) NULL)
	mHas = 0;		//no such parameter

      else if (find (mName)->get_value () != mPar->get_value ())
	{
	  strcpy (mesg, "value not equal.\n");
	  PLog (mesg, 3);
	  mHas = 0;
	}

      else
	{			//sprintf(mesg,"val1 %f, val2 %f\n",this->find(mName)->get_value(),mPar->get_value());
	  //PLog(mesg,3);
	  //sprintf(mesg,"name1: %s, name2 %s\n",this->GetName(),mSubList->GetName());
	  //PLog(mesg,3);

	}
    }
  //PLog("exiting",3);
  return mHas;
}

void QParameterList::Add (QParameter * newPar)
{
  //Add a new parameter to the end of the QParameterList
  list[num] = *newPar;
  list[num].set_name( "null2" ); //should define proper c.c.
  //if (parname != (char*)NULL) newPar->set_name(parname);
  //if (value != 0.0)           newPar->set_value(value);
  //if (error != 0.0)           newPar->set_error(error);
  num++;
}

void QParameterList::Reset()
{
  //Reset this parameter list. 
  num = 0;
}
