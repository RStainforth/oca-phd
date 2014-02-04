///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASMath.hh
///
/// CLASS: LOCAS::LOCASMath
///
/// BRIEF: Utility class used to calculate mathematical formulae
///        required in the optics fit.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: Utility class used to calculate mathematical formulae
///         required in the optics fit. This currently includes
///         calculations for the multiple photo-electron counts
///         at the PMTs.
///
////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TObject.h>

#include <iostream>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "LOCASMath.hh"

#define NR_END 1

using namespace LOCAS;
using namespace std;
#define FREE_ARG char*

ClassImp( LOCASMath );

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::MPECorrectedNPrompt( Float_t nPrompt, Float_t nPulses )
{

  // This is the equation as featured in eqn 4.13, page 77 of B.Moffat's PhD thesis
  Float_t mpePrompt = -1.0 * nPulses * TMath::Log( ( 1.0 - ( nPrompt / nPulses ) ) );
  return mpePrompt;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::MPECorrectedNPromptErr( Float_t nPrompt, Float_t nPulses )
{

  // This is the equation as featured in eqn 4.14, page 77 of B.Moffat's PhD thesis
  Float_t mpePromptErr = ( TMath::Sqrt( nPrompt ) ) / ( 1.0 - ( nPrompt / nPulses ) );
  return mpePromptErr;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t* LOCASMath::LOCASVector( Long_t nStart, Long_t nEnd )
{

	Float_t *v;

	v=(Float_t *)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(Float_t)));
	if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
	return v-nStart+NR_END;

}

//////////////////////////////////////
//////////////////////////////////////

Int_t* LOCASMath::LOCASIntVector( Long_t nStart, Long_t nEnd )
{

	Int_t *v;

	v=(Int_t *)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(Int_t)));
	if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
	return v-nStart+NR_END;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t** LOCASMath::LOCASMatrix( Long_t nStarti, Long_t nEndi, Long_t nStartj, Long_t nEndj )
{

	long i, nrow=nEndi-nStarti+1,ncol=nEndj-nStartj+1;
	Float_t **m;

	/* allocate pointers to rows */
	m=(Float_t **) malloc((size_t)((nrow+NR_END)*sizeof(Float_t*)));
	if (!m) cout << "LOCAS::LOCASMath: Allocation failure 1 in LOCASMatrix()" << endl;
	m += NR_END;
	m -= nStarti;

	/* allocate rows and set pointers to them */
	m[nStarti]=(Float_t *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(Float_t)));
	if (!m[nStarti]) cout << "LOCAS::LOCASMath: Allocation failure 2 in LOCASMatrix()" << endl;
	m[nStarti] += NR_END;
	m[nStarti] -= nStartj;

	for(i=nStarti+1;i<=nEndi;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_Vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_Matrix(float **m, long nrl, long nrh, long ncl, long nch)
/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_IntVector(int *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
