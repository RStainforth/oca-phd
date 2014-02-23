////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASChiSquare.cc
///
/// CLASS: LOCAS::LOCASChiSquare
///
/// BRIEF: A simple class to compute the global chi-square
///        of an entire data set given a LOCASModel object and
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

LOCASChiSquare::LOCASChiSquare( const LOCASModel& locasModel, 
                                const LOCASDataStore& locasData )
{

  fModel = locasModel;
  fDataStore = locasData;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateChiSquare( const LOCASDataPoint& dPoint )
{

  Float_t dataVal = dPoint.GetOccRatio();
  Float_t modelVal = fModel.ModelPrediction( dPoint );
  Float_t res = ( dataVal - modelVal ) * ( dataVal - modelVal );

  Float_t error = dPoint.GetOccRatioErr();

  return ( res / ( error * error ) );
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateGlobalChiSquare()
{
  
  Float_t chiSq = 0.0;
  std::vector< LOCASDataPoint >::iterator iD;

  for ( iD = fDataStore.GetLOCASDataPointsIterBegin();
        iD != fDataStore.GetLOCASDataPointsIterEnd();
        iD++ ){

    chiSq += EvaluateChiSquare( *(iD) );

  }

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateGlobalChiSquare( const Double_t* params )
{

  fModel.SetParameters( params );
  Float_t chiSq = EvaluateGlobalChiSquare();
  
  return chiSq;

}
