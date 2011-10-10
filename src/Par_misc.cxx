#include <stdio.h>
#include <assert.h>
#include "QParameter.h"

#define debug_parameters 1
#define TRUE  1
#define FALSE 0
#define COMCHAR '#'
#define MAXPARAMETERS 1024
#define MAXPARAMETERSTRING 1024

QParameter *parameter (char *namestring);




static QParameter *parameter_list[MAXPARAMETERS];
static int number_of_parameters = 0;


int parameters_read (char *filename);

void parameters_summary (FILE * fp);

int par_read_line (FILE * fp, char *line, int *lnum);
int par_find_command (FILE * fp, char *line, int *lnum);
int par_find_parameter (FILE * fp, char *line, int *lnum, int *pline,
			char *parstr, char *fieldstr, char *valstr);
void par_get_parameter_string (char *line, int *lnum, char *parstr,
			       int *istart, int *istop);

void par_get_field_string (char *line, int *lnum, char *fieldstr,
			   int *iparstop, int *ifldstart, int *ifldstop);
void par_get_value_string (FILE * fp, char *line, int *lnum,
			   char *valstr, int i_equals);

void par_update (char *parstr, char *fieldstr, char *valuestr,
		 char *filename, int pline);
QParameter *par_find (char *parname);

int 
par_find_command (FILE * fp, char *line, int *lnum)
{
  return (0);			// do nothing right now

}

int 
par_find_parameter (FILE * fp, char *line, int *lnum, int *pline,
		    char *parstr, char *fieldstr,
		    char *valstr)
{
  int i, i_equals;
  int iparstart, iparstop;
  int ifldstart, ifldstop;
  int found_parameter;

  found_parameter = 0;

  for (i = 0; (line[i] != '\0') && (line[i] != COMCHAR) && (!found_parameter)
       ; i++)
    {
      if (line[i] == '=')
	{
	  i_equals = i;
	  found_parameter = 1;
	}
    }
  if (found_parameter)
    {
      //  if(debug_parameters > 0){
      //      printf("par_find_parameter: found parameter at line %d\n",
      //                 *lnum);
      //  }
      *pline = *lnum;		// set line that the parameter was found on.

      par_get_parameter_string (line, lnum, parstr, &iparstart, &iparstop);
      par_get_field_string (line, lnum, fieldstr,
			    &iparstop, &ifldstart, &ifldstop);
      par_get_value_string (fp, line, lnum, valstr, i_equals);
      //  if(debug_parameters > 0){
      //  //printf("par_find_parameter:\n");
      //    printf("  -- parstr  =<%s>\n",parstr);
      //    printf("  -- fieldstr=<%s>\n",fieldstr);
      //    printf("  -- valstr  =<%s>\n",valstr);
      // }
    }
  return (found_parameter);
}

void 
par_get_parameter_string (char *line, int *lnum, char *parstr,
			  int *istart, int *istop)
{
  int i, j, k;
  char c;

  //if(debug_parameters){
  //  printf("par_get_parameter_string:\n");
  //}

  // skip white space
  for (i = 0; (c = line[i] == ' ') || (c == '\t'); i++)
    ;

  if ((line[i] == '.') || (line[i] == '='))
    {
      printf ("line[%d]:%s\n", *lnum, line);
      printf ("parameter ERROR: found blank parameter name\n");
      exit (1);
    }
  *istart = i;

  //if(debug_parameters){
  //  printf("  -- istart = %d\n",*istart);
  //}

  // copy parameter string;
  for (j = i, k = 0; ((c = parstr[k] = line[j]) != '=')
       && (c != '.') && (c != ' '); j++, k++)
    ;
  parstr[k] = '\0';
  *istop = j - 1;

  //if(debug_parameters){
  //  printf("  -- istop=%d\n",*istop);
  //  printf("  -- parstr=<%s>\n",parstr);
  //}

}

void 
par_get_field_string (char *line, int *lnum, char *fieldstr,
		      int *iparstop, int *ifldstart, int *ifldstop)
{
  int i, j, k, ipstop;
  char c;

  ipstop = *iparstop;
  if ((line[ipstop + 1]) != '.')
    {
      fieldstr[0] = '\0';
      i = *ifldstart = *ifldstop = ipstop + 1;
    }
  else
    {
      i = ipstop + 2;
      *ifldstart = i;
      // copy parameter string;
      for (j = i, k = 0; ((c = fieldstr[k] = line[j]) != '=')
	   && (c != ' '); j++, k++)
	;
      fieldstr[k] = '\0';
      *ifldstop = j - 1;
    }

  // if(debug_parameters){
  //    printf("par_get_field_string:\n");
  //    printf("  -- ipstop   =%d\n",ipstop);
  //    printf("  -- ifldstart=%d\n",*ifldstart);
  //    printf("  -- ifldstop =%d\n",*ifldstop);
  //    printf("  -- fieldstr =<%s>\n",fieldstr);
  //}
}


void 
par_get_value_string (FILE * fp, char *line, int *lnum,
		      char *valstr, int i_equals)
{
  int i, j, k, parameter_line;
  char c;

  //if(debug_parameters){
  //    printf("par_get_value_string:\n");
  //}
  parameter_line = *lnum;
  i = i_equals + 1;
  j = 0;
  while (((c = line[i]) != ';') && (j < (MAXPARAMETERSTRING - 1)))
    {
      if (c == '\0')
	{
	  if (!par_read_line (fp, line, lnum))
	    {
	      printf ("parameter ERROR: end of file found while reading\n");
	      printf ("                 value field for parameter on line %d\n",
		      parameter_line);
	      exit (1);
	    }
	  valstr[j] = '\n';
	  i = -1;
	  //printf(" end of line found, reading new line, reseting i\n");
	}
      else
	{
	  valstr[j] = c;
	}
      i++;
      j++;
    }
  if (j >= (MAXPARAMETERSTRING - 1))
    {
      printf ("parameter ERROR: Maximum string length exceded while reading\n");
      printf ("                 value field for parameter on line %d\n",
	      parameter_line);
      exit (1);
    }
  valstr[j] = '\0';
  // if(debug_parameters){
  //    printf("  -- valstr length=%d\n",j);
  //    printf("  -- valstr=<%s>\n",valstr);
  //}
}



int 
par_read_line (FILE * fp, char *line, int *lnum)
{
  char c;
  int i, j, gotcomment;

  (*lnum)++;
  for (i = 0; ((c = getc (fp)) != '\n') && (c != EOF); i++)
    {
      line[i] = c;
    }
  if (c == '\\')
    while (((c = getc (fp)) != '\n') && (c != EOF))
      ;
  line[i] = '\0';

  // if(debug_parameters > 0){
  //     printf("par_read_line: line[%d]=<%s>\n",*lnum,line);
  // }

  if (c == EOF)
    return (FALSE);
  else
    return (TRUE);
}


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// parameters_summary
// prints summary of all defined parameters to file fp
//
void 
parameters_summary (FILE * fp)
{
  QParameter *p;
  int i;

  fprintf (fp, "\n  Parameter Summary\n");
  fprintf (fp, "  %d parameters defined\n", number_of_parameters);
  fprintf (fp, "--------------------------------------------------------\n");
  fprintf (fp, "%-20s %-15s %-10s %-10s",
	   "QParameter", "value", "error", "units");
  fprintf (fp, "\n");
  for (i = 0; i < number_of_parameters; i++)
    {
      p = parameter_list[i];
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
