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
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: Utility class used to calculate mathematical formulae
///         required in the optics fit. This currently includes
///         calculations for the multiple photo-electron counts
///         at the PMTs.
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

    Float_t MPECorrectedNPrompt( Float_t nPrompt, Float_t nPulses );         // Calculate the MPE correction for the prompt counts
    Float_t MPECorrectedNPromptErr( Float_t nPrompt, Float_t nPulses );      // Calculate the error on the MPE corrected prompt counts

    Float_t OccRatioErr( LOCASPMT* pmt );

    Float_t* LOCASVector( Long_t nStart, Long_t nEnd );
    Int_t* LOCASIntVector( Long_t nStart, Long_t nEnd );
    Float_t** LOCASMatrix( Long_t nStarti, Long_t nEndi, Long_t nStartj, Long_t nEndj );

    void LOCASFree_Matrix(float **m, long nrl, long nrh, long ncl, long nch);
    void LOCASFree_Vector(float *v, long nl, long nh);
    void LOCASFree_IntVector(int *v, long nl, long nh);
    
    ClassDef( LOCASMath, 0 );

  };

}

#endif
