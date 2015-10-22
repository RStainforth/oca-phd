/* CAUTION: This is the ANSI C (only) version of the Numerical Recipes
   utility file nrutil.h.  Do not confuse this file with the same-named
   file nrutil.h that is supplied in the 'misc' subdirectory.
   *That* file is the one from the book, and contains both ANSI and
   traditional K&R versions, along with #ifdef macros to select the
   correct version.  *This* file contains only ANSI C.               */

#ifndef _NR_UTILS_H_
#define _NR_UTILS_H_

inline float SQR(float a)
{
	return (a * a);
}

inline double DSQR(double a)
{
	return (a * a);
}

inline double DMAX(double a,double b)
{
	return (a > b ? a : b);
}

inline double DMIN(double a,double b)
{
	return (a < b ? a : b);
}

inline float FMAX(float a,float b)
{
	return (a > b ? a : b);
}

inline float FMIN(float a,float b)
{
	return (a < b ? a : b);
}

inline long LMAX(long a,long b)
{
	return (a > b ? a : b);
}

inline long LMIN(long a,long b)
{
	return (a < b ? a : b);
}

inline int IMAX(int a,int b)
{
	return (a > b ? a : b);
}

inline int IMIN(int a,int b)
{
	return (a < b ? a : b);
}

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

/* we don't use these routines anyway, so comment this out
** (vector(), in particular, generates a conflicting name error with g++2.96)
** - PH 11/19/04
void nrerror(char error_text[]);
float *vector(long nl, long nh);
int *ivector(long nl, long nh);
unsigned char *cvector(long nl, long nh);
unsigned long *lvector(long nl, long nh);
//double *dvector(long nl, long nh);
float **matrix(long nrl, long nrh, long ncl, long nch);
double **dmatrix(long nrl, long nrh, long ncl, long nch);
int **imatrix(long nrl, long nrh, long ncl, long nch);
float **submatrix(float **a, long oldrl, long oldrh, long oldcl, long oldch,
	long newrl, long newcl);
float **convert_matrix(float *a, long nrl, long nrh, long ncl, long nch);
float ***f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
void free_vector(float *v, long nl, long nh);
void free_ivector(int *v, long nl, long nh);
void free_cvector(unsigned char *v, long nl, long nh);
void free_lvector(unsigned long *v, long nl, long nh);
//void free_dvector(double *v, long nl, long nh);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch);
void free_submatrix(float **b, long nrl, long nrh, long ncl, long nch);
void free_convert_matrix(float **b, long nrl, long nrh, long ncl, long nch);
void free_f3tensor(float ***t, long nrl, long nrh, long ncl, long nch,
	long ndl, long ndh);
*/

#endif /* _NR_UTILS_H_ */
