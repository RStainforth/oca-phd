////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASChiSquare.cc
///
/// CLASS: LOCAS::LOCASChiSquare
///
/// BRIEF: A simple class to compute the global chi-square
///        of an entire dataset given a LOCASModel object and
///        LOCASModelParameterStore object
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASChiSquare.hh"
#include "LOCASDataPoint.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASMath.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASChiSquare )

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateChiSquare( LOCASDataPoint& dPoint )
{

  Float_t modelVal = fModel->ModelPrediction( dPoint );
  Float_t dataVal = 0.0;
  Float_t error = 0.0;
  LOCASMath::CalculateMPEOccRatio( dPoint, dataVal, error );

  Float_t res = ( dataVal - modelVal );
  Float_t chiSq =  ( res * res ) / ( error * error );

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateGlobalChiSquare()
{
  
  // Calculate the total chisquare over all datapoints (PMTs) in the dataset
  Float_t chiSq = 0.0;
  std::vector< LOCASDataPoint >::iterator iD;

  for ( iD = fDataStore->GetLOCASDataPointsIterBegin();
        iD != fDataStore->GetLOCASDataPointsIterEnd();
        iD++ ){

      chiSq += EvaluateChiSquare( *(iD) );
  }

  return chiSq;

}
