//////////////////////////////////////////////////////////////////////
//                                                                  //
//   QParameter class.  Retrieve and store parameters through a     // 
//   common interface.                                              //
//   Written by F. Duncan 98/02/15                                  //
//   Updated by M. Boulay 98/04/02                                  //
//////////////////////////////////////////////////////////////////////

#define debug_parameters 1
#define TRUE  1
#define FALSE 0
#define COMCHAR '#'
#define MAXPARAMETERS 1024
#define MAXPARAMETERSTRING 1024

#include "QParameter.h"
#include <string.h>
ClassImp (QParameter)
  QParameter::QParameter ()
{
  //Default constructor.  Calls init("null")
  init ("null");
}

QParameter::QParameter (char *namestring)
{
  //Constructor calls init(namestring).
  init (namestring);
}
QParameter::~QParameter ()
{
  //Destructor
  delete [] name;
};
void QParameter::
init (char *namestring)
{
  //Initialize parameter (?)
  name = new char[100];
  if ( !strcmp(namestring,"") ) strcpy(name,"null");
  else  strcpy (name, namestring);
  
  //Initialize flags for value_set and error_set
  value_set = FALSE;
  error_set = FALSE;
  error_type = undefined;
  state = state_fixed;
  
  //parameter_list[number_of_parameters] = this;
  //number_of_parameters++;
}
double QParameter::
get_value ()
{
  //Return the numerical value of the parameter
  return value;
};
double QParameter::
set_value (double val)
{
  //Set the numerical value of the parameter
  value = val;
  return val;
};
double QParameter::
get_error ()
{
  //Return the numerical value of the error associated with the parameter
  return error;
};
double QParameter::
set_error (double err)
{
  //Set the numerical value of the error associated with the parameter
  error = err;
  return err;
};

void QParameter::
print (int level)
{
  //Print details of the parameter to stdout if level is greater than zero
  char errorstr[132];
  printf ("parameter: %s\n", name);
  if (level > 0)
    {
      printf ("  %-10s = %10lg\n", "value", value);
      printf ("  %-10s = %s\n", "units", units);
      printf ("  %-10s = %10lg\n", "error", error);
      // error type
      if (error_type == undefined)
	strcpy (errorstr, "undefined");
      else if (error_type == one_sigma)
	strcpy (errorstr, "one_sigma");
      else if (error_type == two_sigma)
	strcpy (errorstr, "two_sigma");
      else if (error_type == three_sigma)
	strcpy (errorstr, "three_sigma");
      else if (error_type == fwhm)
	strcpy (errorstr, "fwhm");
      else
	strcpy (errorstr, "UNKNOWN");
      printf ("  %-10s = %s\n", "error type", errorstr);
      
      printf ("  %-10s = \"%s\"\n", "reference", reference);
    }
}


int QParameter::
update_value (char *str, char *errorstr)
{
  //Update the value of the parameter
  double val;
  sscanf (str, "%lf", &val);
  value = val;
  value_set = TRUE;
  
  return (1);
}

int QParameter::
update_error (char *str, char *errorstr)
{
  //Update the error of the parameter
  double val;
  sscanf (str, "%lf", &val);
  error = val;
  error_set = TRUE;
  
  return (1);
}


int QParameter::
update_units (char *str, char *errorstr)
{
  //Update the units in which  the parameter is given
  int i, j, k;
  char quotechar = '\"';
  char c;
  char string[MAXPARAMETERSTRING];
  
  // skip white space
  for (i = 0; ((c = str[i]) != quotechar) && (c != '\0'); i++)
    ;
  if (c == '\0')
    {
      strcpy (errorstr, "missing opening quotation mark");
      return (0);		// didn't find an opening quote
      
    }
  i++;
  
  // copy parameter string;
  for (j = i, k = 0; ((c = string[k] = str[j]) != quotechar)
	 && (c != '\0'); j++, k++)
    ;
  string[k] = '\0';
  if (str[j] == '\0')
    {
      strcpy (errorstr, "missing closing quotation mark");
      return (0);		// no closing quote
      
    }
  
  sscanf (string, "%s", units);
  units_set = TRUE;
  return (1);
}


int QParameter::
update_reference (char *str, char *errorstr)
{
  
  //Update the reference for the parameter
  int i, j, k;
  char quotechar = '\"';
  char c;
  char string[MAXPARAMETERSTRING];
  
  // skip white space
  for (i = 0; ((c = str[i]) != quotechar) && (c != '\0'); i++)
    ;
  if (c == '\0')
    {
      strcpy (errorstr, "missing opening quotation mark");
      return (0);		// didn't find an opening quote
      
    }
  i++;
  
  // copy parameter string;
  for (j = i, k = 0; ((c = string[k] = str[j]) != quotechar)
	 && (c != '\0'); j++, k++)
    ;
  string[k] = '\0';
  if (str[j] == '\0')
    {
      strcpy (errorstr, "missing closing quotation mark");
      return (0);		// no closing quote
      
    }
  
  strcpy (reference, string);
  //sscanf(string,"%s",reference);
  reference_set = TRUE;
  return (1);
}

int QParameter::
update_error_type (char *str, char *errorstr)
{
  //Update the error_type of the parameter (one_sigma, two_sigma, three_sigma)
  char errortype_string[MAXPARAMETERSTRING];
  
  sscanf (str, "%s", errortype_string);
  if (!strcmp (errortype_string, "one_sigma"))
    {
      error_type = one_sigma;
    }
  else if (!strcmp (errortype_string, "two_sigma"))
    {
      error_type = two_sigma;
    }
  else if (!strcmp (errortype_string, "three_sigma"))
    {
      error_type = three_sigma;
    }
  else if (!strcmp (errortype_string, "fwhm"))
    {
      error_type = fwhm;
    }
  else
    {
      sprintf (errorstr, "undefined error type: %s", errortype_string);
      return (0);
    }
  return (1);
}
