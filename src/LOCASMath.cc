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
#include "LOCASPMT.hh"

#define NR_END 1

using namespace LOCAS;
using namespace std;
#define FREE_ARG char*

ClassImp( LOCASMath );

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::MPECorrectedNPrompt( const Float_t nPrompt, const Float_t nPulses )
{

  // This is the equation as featured in eqn 4.13, page 77 of B.Moffat's PhD thesis
  Float_t mpePrompt = -1.0 * nPulses * TMath::Log( ( 1.0 - ( nPrompt / nPulses ) ) );
  return mpePrompt;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::MPECorrectedNPromptErr( const Float_t nPrompt, const Float_t nPulses )
{

  // This is the equation as featured in eqn 4.14, page 77 of B.Moffat's PhD thesis
  Float_t mpePromptErr = ( TMath::Sqrt( nPrompt ) ) / ( 1.0 - ( nPrompt / nPulses ) );
  return mpePromptErr;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::OccRatioErr( const LOCASPMT* pmt ){

  Float_t errResult = 1.0;
  Float_t run2 = TMath::Power( ( ( pmt->GetMPECorrOccupancyErr() ) / ( pmt->GetMPECorrOccupancy() ) ), 2 );
  Float_t centralRun2 = TMath::Power( ( ( pmt->GetCentralMPECorrOccupancyErr() ) / ( pmt->GetCentralMPECorrOccupancy() ) ), 2 );
  errResult = ( pmt->GetMPECorrOccupancy() / pmt->GetCentralMPECorrOccupancy() ) * TMath::Sqrt( run2 + centralRun2 );
  return errResult;
  
}

//////////////////////////////////////
//////////////////////////////////////

float* LOCASMath::LOCASVector( const long nStart, const long nEnd )
{

  float *v;
  
  v=(float*)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(float)));
  if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
  return v-nStart+NR_END;
  
}

//////////////////////////////////////
//////////////////////////////////////

int* LOCASMath::LOCASIntVector( const long nStart, const long nEnd )
{
  
  int *v;
  
  v=(int*)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(int)));
  if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
  return v-nStart+NR_END;
  
}

//////////////////////////////////////
//////////////////////////////////////

float** LOCASMath::LOCASMatrix( const long nStarti, const long nEndi, const long nStartj, const long nEndj )
{
  
  long i; 
  long nrow=nEndi-nStarti+1;
  long ncol=nEndj-nStartj+1;
  float **m;
  
  /* allocate pointers to rows */
  m=(float**) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
  if (!m) cout << "LOCAS::LOCASMath: Allocation failure 1 in LOCASMatrix()" << endl;
  m += NR_END;
  m -= nStarti;
  
  /* allocate rows and set pointers to them */
  m[nStarti]=(float*) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
  if (!m[nStarti]) cout << "LOCAS::LOCASMath: Allocation failure 2 in LOCASMatrix()" << endl;
  m[nStarti] += NR_END;
  m[nStarti] -= nStartj;
  
  for(i=nStarti+1;i<=nEndi;i++) m[i]=m[i-1]+ncol;
  
  /* return pointer to array of pointers to rows */
  return m;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_Vector(float *v, const long nl, const long nh)
/* free a float vector allocated with vector() */
{
  free((FREE_ARG) (v+nl-NR_END));
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_Matrix(float **m, const long nrl, const long nrh, const long ncl, const long nch)
/* free a float vector allocated with vector() */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_IntVector(int *v, const long nl, const long nh)
/* free a float vector allocated with vector() */
{
  free((FREE_ARG) (v+nl-NR_END));
}
