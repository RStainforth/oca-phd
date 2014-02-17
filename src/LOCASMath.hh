///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASMath.hh
///
/// CLASS: LOCAS::LOCASMath
///
/// BRIEF: Utility class which provides mathematical formulae
///        required in the optics fit.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: Utility class used to calculate mathematical formulae
///         required in the optics fit. This currently includes
///         calculations for the multiple photo-electron counts
///         at the PMTs, and the working arrays for the 
///         Levenberg-Marquardt algorithm. The functions:
///     
///                  LOCASVector
///                  LOCASIntVector
///                  LOCASMatrix
///                  LOCASFree_Vector
///                  LOCASFree_IntVector
///                  LOCASFree_Matrix
///
///        Are based on the Numerical Recipes code which provide
///        and allocate memory for the Levenberg-Marquardt working
///        arrays.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASMATH_
#define _LOCASMATH_

#include <TMath.h>
#include <TObject.h>

#include "LOCASPMT.hh"


namespace LOCAS{

  class LOCASMath : public TObject
  {
  public:
    LOCASMath(){ }
    ~LOCASMath(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Float_t MPECorrectedNPrompt( const Float_t nPrompt, const Float_t nPulses );         // Calculate the MPE correction for the prompt counts
    Float_t MPECorrectedNPromptErr( const Float_t nPrompt, const Float_t nPulses );      // Calculate the error on the MPE corrected prompt counts

    Float_t OccRatioErr( const LOCASPMT* pmt );

    float* LOCASVector( const long nStart, const long nEnd );
    int* LOCASIntVector( const long nStart, const long nEnd );
    float** LOCASMatrix( const long nStarti, const long nEndi, const long nStartj, const long nEndj );

    void LOCASFree_Matrix(float **m, const long nrl, const long nrh, const long ncl, const long nch);
    void LOCASFree_Vector(float *v, const long nl, const long nh);
    void LOCASFree_IntVector(int *v, const long nl, const long nh);
    
    ClassDef( LOCASMath, 0 );

  };

}

#endif
