////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModel.cc
///
/// CLASS: LOCAS::LOCASModel
///
/// BRIEF: Class used to process the LOCASRun files
///        and prepare the parameters and values for each PMT
///        required for the model of the optical response.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file.
///
////////////////////////////////////////////////////////////////////

#include "LOCASRunReader.hh"
#include "LOCASDB.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASModel.hh"

#include <map>

using namespace LOCAS;
using namespace std;

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModel::EvaluateChiSquare( Double_t* params )
{

  Float_t chiSquare = 0.0;
  Float_t dataROcc = GetDataROcc();
  Float_t modelROcc = CalcModelROcc( params );
  Float_t roccError = GetROccError();

  chiSquare = ( ( dataROcc - modelROcc ) * ( dataROcc - modelROcc ) ) / ( ( roccError ) * ( roccError ) );

  return chiSquare;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModel::EvaluateGlobalChiSquare( Double_t* params )
{

  Float_t globalChiSquare = 0.0;

  for ( Int_t iRun = 0; iRun < fListOfRunIDs.size(); iRun++ ){
    fCurrentRun = LOCASRunReader::GetLOCASRun( fListOfRunIDs[ iRun ] );
    
    std::map< Int_t, LOCASPMT >::iterator iPMT;
    for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin();
          iPMT != fCurrentRun->GetLOCASPMTIterEnd();
          iPMT++ ){
      
      *fCurrentPMT = ( iPMT->second );
      globalChiSquare += EvaluateChiSquare( params );
      
    }

  }
    
  return globalChiSquare;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModel::CalcModelROcc( Double_t* params )
{

  Float_t normRatio = 1.0;
  return normRatio;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModel::GetDataROcc()
{

  Float_t normRatio = 1.0;
  return normRatio;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASModel::GetROccError()
{

  Float_t normRatio = 1.0;
  return normRatio;

}
