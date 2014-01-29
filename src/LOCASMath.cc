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

#include "LOCASMath.hh"

#define NR_END 1

using namespace LOCAS;
using namespace std;

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

Float_t* LOCASMath::LOCASVector( Float_t nStart, Float_t nEnd )
{

	float *v;

	v=(float *)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(float)));
	if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
	return v-nStart+NR_END;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t** LOCASMath::LOCASMatrix( Float_t nStarti, Float_t nEndi, Float_t nStartj, Float_t nEndj )
{

	long i, nrow=nEndi-nStarti+1,ncol=nEndj-nStartj+1;
	float **m;

	/* allocate pointers to rows */
	m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
	if (!m) cout << "LOCAS::LOCASMath: Allocation failure 1 in LOCASMatrix()" << endl;
	m += NR_END;
	m -= nStarti;

	/* allocate rows and set pointers to them */
	m[nStarti]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
	if (!m[nStarti]) cout << "LOCAS::LOCASMath: Allocation failure 2 in LOCASMatrix()" << endl;
	m[nStarti] += NR_END;
	m[nStarti] -= nStartj;

	for(i=nStarti+1;i<=nEndi;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;

}
