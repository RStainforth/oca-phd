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
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TObject.h>

#include <iostream>
#include <cmath>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "LOCASMath.hh"
#include "LOCASPMT.hh"

#define NR_END 1

using namespace LOCAS;
using namespace std;

#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) { swapVal = (a); (a) = (b); (b) = swapVal; }
#endif

#define FREE_ARG char*

using namespace LOCAS;

ClassImp( LOCASMath )

LOCASMath::LOCASMath(){ }

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASMath::MPECorrectedNPrompt( const Double_t nPrompt, const Double_t nPulses )
{

  // This is the equation as featured in eqn 4.13, page 77 of B.Moffat's PhD thesis
  Double_t mpePrompt = -log( ( 1.0 - ( nPrompt / nPulses ) ) );
  return mpePrompt;

}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASMath::MPECorrectedNPromptErr( const Double_t nPrompt, const Double_t nPulses )
{

  // This is the equation as featured in eqn 4.14, page 77 of B.Moffat's PhD thesis
  Double_t mpePromptErr = sqrt( nPrompt ) / ( nPulses - nPrompt );
  return mpePromptErr;

}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASMath::MPECorrectedNPromptCorr( const Double_t mpeOcc, const Double_t nPrompt, const Double_t nPulses )
{

  // This is the equation as featured in eqn 4.14, page 77 of B.Moffat's PhD thesis
  Double_t mpePromptCorr = ( (Double_t)( mpeOcc * nPulses / nPrompt ) );
  return mpePromptCorr;

}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASMath::OccRatioErr( const LOCASPMT* pmt ){

  Double_t errResult = 1.0;
  Double_t run2 = pow( ( ( pmt->GetMPECorrOccupancyErr() ) / ( pmt->GetMPECorrOccupancy() ) ), 2 );
  Double_t centralRun2 = pow( ( ( pmt->GetCentralMPECorrOccupancyErr() ) / ( pmt->GetCentralMPECorrOccupancy() ) ), 2 );
  errResult = ( pmt->GetMPECorrOccupancy() / pmt->GetCentralMPECorrOccupancy() ) * sqrt( run2 + centralRun2 );
  return errResult;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::CalculateMPEOccRatio( const LOCASDataPoint& dPoint, Float_t& occRatio, Float_t& occRatioErr ){

  occRatio = dPoint.GetMPECorrOccupancy() / dPoint.GetCentralMPECorrOccupancy();
  occRatio *= ( dPoint.GetCentralFresnelTCoeff() * dPoint.GetCentralSolidAngle() ) / ( dPoint.GetFresnelTCoeff() * dPoint.GetSolidAngle() );
  occRatio *= dPoint.GetCentralLBIntensityNorm();
  
  Double_t offAxisRun2 = TMath::Power( dPoint.GetMPECorrOccupancyErr() / dPoint.GetMPECorrOccupancy(), 2 );
  Double_t centralRun2 = TMath::Power( dPoint.GetCentralMPECorrOccupancyErr() / dPoint.GetCentralMPECorrOccupancy(), 2 );
  occRatioErr = occRatio * TMath::Sqrt( offAxisRun2 + centralRun2 );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t* LOCASMath::LOCASVector( const long nStart, const long nEnd )
{

  Float_t *v;
  
  v=(Float_t*)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(Float_t)));
  if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
  return v-nStart+NR_END;
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t* LOCASMath::LOCASIntVector( const long nStart, const long nEnd )
{
  
  Int_t *v;
  
  v=(Int_t*)malloc((size_t) ((nEnd-nStart+1+NR_END)*sizeof(Int_t)));
  if (!v) cout << "LOCAS::LOCASMath: Allocation failure in LOCASVector()" << endl;
  return v-nStart+NR_END;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t** LOCASMath::LOCASMatrix( const long nStarti, const long nEndi, const long nStartj, const long nEndj )
{
  
  long i; 
  long nrow=nEndi-nStarti+1;
  long ncol=nEndj-nStartj+1;
  Float_t **m;
  
  /* allocate pointers to rows */
  m=(Float_t**) malloc((size_t)((nrow+NR_END)*sizeof(Float_t*)));
  if (!m) cout << "LOCAS::LOCASMath: Allocation failure 1 in LOCASMatrix()" << endl;
  m += NR_END;
  m -= nStarti;
  
  /* allocate rows and set pointers to them */
  m[nStarti]=(Float_t*) malloc((size_t)((nrow*ncol+NR_END)*sizeof(Float_t)));
  if (!m[nStarti]) cout << "LOCAS::LOCASMath: Allocation failure 2 in LOCASMatrix()" << endl;
  m[nStarti] += NR_END;
  m[nStarti] -= nStartj;
  
  for(i=nStarti+1;i<=nEndi;i++) m[i]=m[i-1]+ncol;
  
  /* return pointer to array of pointers to rows */
  return m;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_Vector( Float_t *v, const long nl )
/* free a Float_t vector allocated with vector() */
{

  free((FREE_ARG) (v+nl-NR_END));

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_Matrix(Float_t **m, const long nrl, const long ncl )
/* free a Float_t vector allocated with vector() */
{

  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::LOCASFree_IntVector(Int_t *v, const long nl )
/* free a Float_t vector allocated with vector() */
{

  free((FREE_ARG) (v+nl-NR_END));

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASMath::GaussJordanElimination( Float_t** lhsMatrix, Int_t nParameters,
                                        Float_t** rhsMatrix, Int_t mVectors )
{

  // This routine is inspired by the original 
  // numerical recipes routine 'gaussj' as featured on page 44 of...
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // --- 'Numerical Recipes: The Art of Scientific Computing' 3rd Edition
  // --- W.H.Press, S.A. Teukolsky, W.T. Vetterling, B.P. Flannery
  // --- Cambridge University Press, 2007
  // --- ISBN-10 0-521-88068-8
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // It has been annotated and rewritten to be consistent with the 
  // programming style of RAT.

  
  //
  // Linear equation solution by Gauss-Jordan elimination. lhsMatrix[ 1...nParameters ][ 1...nParameters ] is the input matrix.
  // rhsMatrix[ 1...nParameters ][ 1...mVectors ] is input containing the m right-hand side vectors. On output, a is
  // replaced by its matrix inverse, and b is replaced by the corresponding set of solution
  // vectors.
  
  // 'retVal', the final return value
  // retVal = -1 : Error, singular matrix
  // retVal = -2 : Error, singular matrix due to insufficient statistics
  // retVal = 0  : Everything's fine.
  Int_t retVal = 0;

  // Int_t arrays used in the book-keeping for the row and column operations
  Int_t *indXC,*indXR;

  // Int_t array for the index locations of the pivot element in the matrix
  Int_t *iPiv;

  // Current value for said pivot element inverse.
  Float_t pivInv = 0.0;

  // Dummy variables for the row operations in the second part of the method
  Float_t dumVal = 0.0;
  Float_t swapVal = 0.0;
  
  // Declare a vector of 'Int_t' objects which are
  // used for book-keeping on the pivot method for the elimination
  indXC = LOCAS::LOCASMath::LOCASIntVector( 1, nParameters );
  indXR = LOCAS::LOCASMath::LOCASIntVector( 1, nParameters );
  iPiv = LOCAS::LOCASMath::LOCASIntVector( 1, nParameters );
  
  // Variables for the first part of the method: searching for a pivot element
  // about which to perform the Gauss Jordan Elimination method
  Int_t iVar, jVar, kVar = 0;

  // Variable for the second part of the method: indices about which to perform
  // row/column operations on the said pivot element
  Int_t lVal, llVal = 0;

  // Variables for indices of the rows and columns of the matrix ( upon which
  // a search is made for the pivot element )
  Int_t iCol, iRow = 0;
  
  // First set all the indices for the pivot values to zero.
  for ( jVar = 1; jVar <= nParameters; jVar++ ){ iPiv[ jVar ] = 0; }
  
  Float_t bigVal = 0.0;
  
  // Loop over all the columns in the matrix (lhsMatrix) which are to be reduced by
  // Guass Jordan Elimination
  for ( iVar = 1; iVar <= nParameters; iVar++ ){
    
    // Loop search for a pivot element in the matrix (lhsMatrix). 
    // The pivot element is the element in the matrix upon which
    // reduction methods (column operations) are applied to the matrix 
    // around that element. Typically the pivot element has a value 1. 
    // It is about that element that row operations are performed.
    
    bigVal = 0.0;
    for ( jVar = 1; jVar <= nParameters; jVar++ ){
      if ( iPiv[ jVar ] != 1 ){
        for ( kVar = 1; kVar <= nParameters; kVar++ ) {
          if ( iPiv[ kVar ] == 0 ) {
            if ( fabs( lhsMatrix[ jVar ][ kVar ] ) >= bigVal ) {
              
              bigVal = fabs( lhsMatrix[ jVar ][ kVar ] );
              iRow = jVar;
              iCol = kVar;
              
            }
          } 
          
          // Initially all values of iPiv[ kVar ] = 0, but these
          // become rearranged and reassigned as the loop continues through the elements
          // in the all the rows and columns. If then, on a subsequent loop
          // through is it found that iPiv[ kVar ] > 1, then a singular matrix value
          // will be obtained, causing an error. Therefore return the error message and set
          // the return value, 'retVal' to -1.
          
          else if ( iPiv[ kVar ] > 1){ 
            cout << "LOCASMath::GaussJordanElimination: Error! Singular matrix element encountered!" << endl;
            retVal = -1;
          }
        }
      }
    }
      
    // Right here ( line below ) we have found the pivot element, so now, if needed we can
    // interchange the row in order to put the pivot element along the diagonal. On paper this
    // would mean rewriting the matrix with the new columns etc but computationally we can
    // just relabel the column indices: indXC[ lVal ], the column of the lVal-th pivot element is
    // lVal-th column that is reduced, whilse indXR[ lVal ] is the row in which that pivot element
    // was originally located. If indXR[ lVal ] is not equal to indXC[ lVal ], there is an implied
    // column interchange. With this form of book-keeping, the solutions; rhsMatrix will end
    // up in the correct order, and the inverse matrix will be scrambled by columns.
    ++( iPiv[ iCol ] );
    if ( iRow != iCol ) {
      for ( lVal = 1; lVal <= nParameters; lVal++ ){ SWAP( lhsMatrix[ iRow ][ lVal ],lhsMatrix[ iCol ][ lVal ]) }
      for ( lVal = 1; lVal <= mVectors; lVal++ ){ SWAP( rhsMatrix[ iRow ][ lVal ],rhsMatrix[ iCol ][ lVal ]) }
    }

    // We can now divide the pivot row by the element, located at (iRow, iCol)
    indXR[ iVar ] = iRow;
    indXC[ iVar ] = iCol;

    if ( lhsMatrix[ iCol ][ iCol ] == 0.0 ){
      cout << "LOCASMath::GaussJordanElimination: Error! Singular matrix element encountered!" << endl;
      cout << "Tip: This is likely due to insufficient statistics. Increase number of data points!" << endl;
      retVal = -2;
    }

    // The inverse value of the pivot element
    pivInv = 1.0 / lhsMatrix[ iCol ][ iCol ];
    
    // Set the diagonal value corresponding from this pivot element to 1.0
    lhsMatrix[ iCol ][ iCol ] = 1.0;
    
    for ( lVal = 1; lVal <= nParameters; lVal++ ){ lhsMatrix[ iCol ][ lVal ] *= pivInv; }
    for ( lVal = 1; lVal <= mVectors; lVal++ ){ rhsMatrix[ iCol ][ lVal ] *= pivInv; }

    // We now reduce the rows except for the pivot element.
    for ( llVal = 1; llVal <= nParameters; llVal++ ){
      if ( llVal != iCol ) {

        dumVal = lhsMatrix[ llVal ][ iCol ];
        lhsMatrix[ llVal ][ iCol ] = 0.0;

        for ( lVal = 1; lVal <= nParameters; lVal++ ){ 
          lhsMatrix[ llVal ][ lVal ] -= lhsMatrix[ iCol ][ lVal ] * dumVal;
        }
        for ( lVal = 1; lVal <= mVectors; lVal++ ){
          rhsMatrix[ llVal ][ lVal ] -= rhsMatrix[ iCol ][ lVal ] * dumVal;
        }
      }
	}

  }

  // That now marks the of the main loop over over the columns for the reduction. The only
  // thing which remains is to unscramle the solution in view of the column interchanges.
  // This is done by interchanging the pairs of columns in the reverse order that the
  // permutations were originally setup.
  
  for ( lVal = nParameters; lVal >= 1; lVal-- ){
    if ( indXR[ lVal ] != indXC[ lVal ] ){
      for ( kVar = 1; kVar <= nParameters; kVar++){ SWAP( lhsMatrix[ kVar ][ indXR[ lVal ] ],lhsMatrix[ kVar ][ indXC[ lVal ] ] ); }
    }
  }
  
  // Now free up the memory which was setup for the book-keeping vectors
  LOCAS::LOCASMath::LOCASFree_IntVector( iPiv, 1 );
  LOCAS::LOCASMath::LOCASFree_IntVector( indXR, 1 );
  LOCAS::LOCASMath::LOCASFree_IntVector( indXC, 1 );

  return retVal;
  
 }

//////////////////////////////////////
//////////////////////////////////////

void LOCASMath::CovarianceSorting( Float_t** covarianceMatrix, Int_t nParameters, 
                                   Int_t varyParameters[], Int_t nVaryParameters )
{

  // This routine is inspired by the original 
  // numerical recipes routine 'covsrt' as featured on page 805 of...
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // --- 'Numerical Recipes: The Art of Scientific Computing' 3rd Edition
  // --- W.H.Press, S.A. Teukolsky, W.T. Vetterling, B.P. Flannery
  // --- Cambridge University Press, 2007
  // --- ISBN-10 0-521-88068-8
  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  // It has been annotated and rewritten to be consistent with the 
  // programming style of RAT.

  // Covariance matrix sorting routine. This expands in storage the covariance matrix 'covarianceMatrix' so as to takeinto account parameters that are being help fixed (for later, this will return zero covariances)

  // Loop variables declaration
  Int_t iVar = 0;
  Int_t jVar = 0;
  Int_t kVar = 0;

  // Swapping variable
  Float_t swapVal;
  
  for ( iVar = nVaryParameters + 1; iVar <= nParameters; iVar++ ){
    for ( jVar = 1; jVar <= iVar; jVar++ ){ 
      covarianceMatrix[ iVar ][ jVar ] = covarianceMatrix[ jVar ][ iVar ] = 0.0; 
    }
  }

  kVar = nVaryParameters;

  for ( jVar = nParameters; jVar >= 1; jVar-- ){
    if ( varyParameters[ jVar ] ){
      for ( iVar = 1; iVar <= nParameters; iVar++ ){ 
        SWAP( covarianceMatrix[ iVar ][ kVar ], covarianceMatrix[ iVar ][ jVar ] ); 
      } 
      for ( iVar = 1; iVar <= nParameters; iVar++ ){ 
        SWAP( covarianceMatrix[ kVar ][ iVar ], covarianceMatrix[ jVar ][ iVar ] ); 
      }

	  kVar--;
    }
  }

}
