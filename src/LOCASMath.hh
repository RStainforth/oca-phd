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

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    static Double_t MPECorrectedNPrompt( const Double_t nPrompt, const Double_t nPulses );         // Calculate the MPE correction for the prompt counts
    static Double_t MPECorrectedNPromptErr( const Double_t nPrompt, const Double_t nPulses );      // Calculate the error on the MPE corrected prompt counts
    static Double_t MPECorrectedNPromptCorr( const Double_t mpeOcc, const Double_t nPrompt, const Double_t nPulses );      // Calculate the correction factor on the MPE calculation of the occupancy

    static Double_t OccRatioErr( const LOCASPMT* pmt );
    static void CalculateMPEOccRatio( const LOCASDataPoint& dPoint, Float_t& occRatio, Float_t& occRatioErr );

    static Float_t* LOCASVector( const long nStart, const long nEnd );
    static int* LOCASIntVector( const long nStart, const long nEnd );
    static Float_t** LOCASMatrix( const long nStarti, const long nEndi, const long nStartj, const long nEndj );

    static void LOCASFree_Matrix(Float_t **m, const long nrl, const long ncl);
    static void LOCASFree_Vector(Float_t *v, const long nl);
    static void LOCASFree_IntVector(int *v, const long nl);

    static void CovarianceSorting( Float_t** covarianceMatrix, Int_t nParameters, 
                                   Int_t varyParameters[], Int_t nVaryParameters );

    static Int_t GaussJordanElimination( Float_t** lhsMatrix, Int_t nParameters,
                                        Float_t** rhsMatrix, Int_t mVectors );


  };

}

#endif
