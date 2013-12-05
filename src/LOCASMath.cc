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
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: Utility class used to calculate mathematical formulae
///         required in the optics fit.
///
////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TObject.h>

#include "LOCASMath.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASMath );

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::MPECorrectedNPrompt( Float_t nPrompt, Float_t nPulses )
{

  Float_t mpePrompt = -1.0 * nPulses * TMath::Log( ( 1.0 - ( nPrompt / nPulses ) ) );
  return mpePrompt;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASMath::MPECorrectedNPromptErr( Float_t nPrompt, Float_t nPulses )
{

  Float_t mpePromptErr = ( TMath::Sqrt( nPrompt ) ) / ( 1.0 - ( nPrompt / nPulses ) );
  return mpePromptErr;

}
