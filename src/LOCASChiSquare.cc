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

LOCASChiSquare::LOCASChiSquare( const LOCASOpticsModel& locasModel, 
                                const LOCASDataStore& locasData )
{

  // Set the model and the dataset for the chisquare to be calculated
  fModel = locasModel;
  fDataStore = locasData;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateChiSquare( LOCASDataPoint& dPoint )
{

  // Calculate the chisquare for a single datapoint (PMT) in the dataset
  Float_t dataVal = dPoint.GetMPEOccRatio();
  Float_t modelVal = fModel.ModelPrediction( dPoint );
  dPoint.SetModelOccRatio( modelVal );

  Float_t res = ( dataVal - modelVal );
  Float_t error = dPoint.GetMPEOccRatioErr();

  Float_t chiSq =  ( res * res ) / ( error * error );
  cout << "ModelOcc: " << modelVal << " | DataOcc: " << dataVal << " | ChiSquare: " << chiSq << endl;
  dPoint.SetChiSq( chiSq );

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASChiSquare::EvaluateGlobalChiSquare()
{
  
  // Calculate the total chisquare over all datapoints (PMTs) in the dataset
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

Float_t LOCASChiSquare::EvaluateGlobalChiSquare( LOCASDataStore& lStore )
{

  // Calculate the total chisquare over all datapoints (PMTs) in the dataset  
  Float_t chiSq = 0.0;
  std::vector< LOCASDataPoint >::iterator iD;

  for ( iD = lStore.GetLOCASDataPointsIterBegin();
        iD != lStore.GetLOCASDataPointsIterEnd();
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
