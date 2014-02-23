////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModel.cc
///
/// CLASS: LOCAS::LOCASModel
///
/// BRIEF: The base class from which all models used in LOCAS
///        as part of a fitting routine inherit from
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASModel.hh"
#include "LOCASModelParameter.hh"
#include "LOCASModelParameterStore.hh"

#include <string>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASModel )

//////////////////////////////////////
//////////////////////////////////////

LOCASModel::LOCASModel( const LOCASModelParameterStore& locasParams, 
                        const std::string modelName )
{

  SetModelParameterStore( locasParams );
  SetModelName( modelName );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModel::SetModelParameterStore( const LOCASModelParameterStore& locasParams )
{

  fModelParameterStore = locasParams;
  if ( fParameters != NULL ){ delete[] fParameters; }
  else{ fParameters = new Double_t[ locasParams.GetNParameters() ]; }

}


