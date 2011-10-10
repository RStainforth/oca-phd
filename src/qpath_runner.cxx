// Load and run the new QPath from libqoca.so
// Bryce Moffat - 29-Apr-2000
// Modified - 4-Oct-2000 for new September OCA runs.
// Modified - 21-Mar-2001:
//   - to process all systematics for all wavelengths
//   - also include all central runs in a single tree for all lambda
//
// Modified - 12-Jul-2003 - Olivier Simard
//   - executable form (.cxx) to extend the option loading method.
//   - breaks the processing in easy steps .
//     and allow automatization.
//   - code is also run by calling QOCAAnalysis::RunQPath(Narg, arg)
//
//-------------------------------------------------------------------------

#include <QOCAAnalysis.h>

int main(Int_t, Char_t**);

int main(Int_t Narg, Char_t** arg){

  // check the arguments provided
  Int_t narg = Narg - 1;
  if((narg < 5) || (narg > 5)){
    fprintf(stderr,"Usage: qpath_runner [cardfile] [step] [scan] [fitmethod] [wlen]\n");
    if(narg < 5) fprintf(stderr,"Needs more arguments. Exiting.\n");
    if(narg > 5) fprintf(stderr,"Too many arguments. Exiting.\n");
    exit(-1);
  }

  // initialize class and option file
  QOCAAnalysis* qa = new QOCAAnalysis(arg[1]); // no messages

  // call the QPath function
  qa->RunQPath(Narg, arg);

  return 0;
}

