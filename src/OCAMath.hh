///////////////////////////////////////////////////////////////////
///
/// FILENAME: OCAMath.hh
///
/// CLASS: OCA::OCAMath
///
/// BRIEF: Utility class which provides mathematical formulae
///        required in the optics fit.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: Utility class used to calculate mathematical formulae
///         required in the optics fit. This currently includes
///         calculations for the multiple photoelectron counts correction
///         at the PMTs, and the working arrays for the 
///         Levenberg-Marquardt algorithm. The functions:
///     
///                  OCAVector
///                  OCAIntVector
///                  OCAMatrix
///                  OCAFree_Vector
///                  OCAFree_IntVector
///                  OCAFree_Matrix
///
///        Are based on the Numerical Recipes code which provide
///        and allocate memory for the Levenberg-Marquardt working
///        arrays. These arrays are used by OCAChiSquare.
///
////////////////////////////////////////////////////////////////////

#ifndef _OCAMATH_
#define _OCAMATH_

#include <TMath.h>
#include <TObject.h>

#include "OCAPMT.hh"
#include "OCAPMT.hh"

#include "TVector.h"


namespace OCA{

  class OCAMath
  {
  public:

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    // Calculate the MPE correction for the prompt counts.
    static Double_t MPECorrectedNPrompt( const Double_t nPrompt, 
                                         const Double_t nPulses );

    // Calculate the error on the MPE corrected prompt counts.
    static Double_t MPECorrectedNPromptErr( const Double_t nPrompt, 
                                            const Double_t nPulses );

    // Calculate the correction factor on the MPE calculation of the occupancy.
    static Double_t MPECorrectedNPromptCorr( const Double_t mpeOcc, 
                                             const Double_t nPrompt, 
                                             const Double_t nPulses );

    // Calculate the error due to the PMT variability on a single
    // PMT as represented by a OCAPMT object.
    static Float_t CalculatePMTVariabilityError( const OCAPMT& dPoint,
                                                 TVector& polPars );

    // Given a OCAPMT object, this calculates both the
    // MPE corrected occupancy ratio and its error. The ratio
    // and the error are assigned to 'occRatio' and 'occRatioErr'
    // respectively.
    static void CalculateMPEOccRatio( const OCAPMT& dPoint, 
                                      Float_t& occRatio, 
                                      Float_t& occRatioErr );

    // Allocate memory for an array of type Float_t with indices
    // starting from 'nStart' and ending at 'nEnd'.
    static Float_t* OCAVector( const Long_t nStart, const Long_t nEnd );

    // Allocate memory for an array of type Int_t with indices
    // starting from 'nStart' and ending at 'nEnd'.
    static Int_t* OCAIntVector( const Long_t nStart, const Long_t nEnd );

    // Allocate memory for an array of arrays of type Float_t with indices
    // starting from 'nStart' and ending at 'nEnd' for both nests.
    static Float_t** OCAMatrix( const Long_t nStartI, const Long_t nEndI, 
                                  const Long_t nStartJ, const Long_t nEndJ );

    // Free up memory for an array of type Float_t, 'vArray'
    // with length 'nLength'.
    static void OCAFree_Vector( Float_t *vArray, const Long_t nLength );

    // Free up memory for an array of arrays of type Float_t, 'mArray'
    // with rows and columns of length 'nRowsLength' and 'nColsLength'
    // respectively.
    static void OCAFree_Matrix( Float_t **mArray, 
                                  const Long_t nRowsLength, 
                                  const Long_t nColsLength );

    // Free up memory for an array of type Int_t, 'vArray'
    // with length 'nLength'.    
    static void OCAFree_IntVector( Int_t *vArray, const Long_t nLength );

    // Helper routine used in the fit. This sorts the covariance matrix for the
    // current set of parameters. See the method definition 
    // in OCAMath.cc for more details.
    static void CovarianceSorting( Float_t** covarianceMatrix, Int_t nParameters, 
                                   Int_t varyParameters[], Int_t nVaryParameters );

    // Helper routine used in the fit. The fit is essentially modelled as
    // a problem of many equations for which a solution matrix is sought.
    // The solution matrix is put into 'row-echelon' form by Gauss-Jordan
    // elimination. This routine performs the row and column operations
    // to put the solution matrix into this 'row-echelon' form. See the 
    // method definition in OCAMath.cc for more details.
    static Int_t GaussJordanElimination( Float_t** lhsMatrix, Int_t nParameters,
                                         Float_t** rhsMatrix, Int_t mVectors );


  };

}

#endif
