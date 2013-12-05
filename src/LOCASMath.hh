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

#ifndef _LOCASMATH_
#define _LOCASMATH_

#include <TMath.h>
#include <TObject.h>


namespace LOCAS{

  class LOCASMath : public TObject
  {
  public:
    LOCASMath(){ }
    ~LOCASMath(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Float_t MPECorrectedNPrompt( Float_t nPrompt, Float_t nPulses );
    Float_t MPECorrectedNPromptErr( Float_t nPrompt, Float_t nPulses );

    
    ClassDef( LOCASMath, 0 );

  };

}

#endif
