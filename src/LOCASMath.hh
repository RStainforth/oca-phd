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
#include "LOCASDataPoint.hh"


namespace LOCAS{

  class LOCASMath
  {
  public:
    LOCASMath(){ }
    ~LOCASMath(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    static Double_t MPECorrectedNPrompt( const Double_t nPrompt, const Double_t nPulses );         // Calculate the MPE correction for the prompt counts
    static Double_t MPECorrectedNPromptErr( const Double_t nPrompt, const Double_t nPulses );      // Calculate the error on the MPE corrected prompt counts
    static Double_t MPECorrectedNPromptCorr( const Double_t mpeOcc, const Double_t nPrompt, const Double_t nPulses );      // Calculate the correction factor on the MPE calculation of the occupancy

    static Double_t OccRatioErr( const LOCASPMT* pmt );
    static void CalculateMPEOccRatio( const LOCASDataPoint& dPoint, Float_t& occRatio, Float_t& occRatioErr );

    static float* LOCASVector( const long nStart, const long nEnd );
    static int* LOCASIntVector( const long nStart, const long nEnd );
    static float** LOCASMatrix( const long nStarti, const long nEndi, const long nStartj, const long nEndj );

    static void LOCASFree_Matrix(float **m, const long nrl, const long nrh, const long ncl, const long nch);
    static void LOCASFree_Vector(float *v, const long nl, const long nh);
    static void LOCASFree_IntVector(int *v, const long nl, const long nh);
    
    ClassDef( LOCASMath, 0 );

  };

}

#endif
