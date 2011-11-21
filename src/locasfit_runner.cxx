// Fast QOCAFit: iterates to convergence in only a few minutes.
//
// Several different fits can be attempted to ensure convergence with tube
// selection based on chisquared cuts of 10 sigma and 5 sigma.
//
// 12-Mar-2001 - Bryce Moffat
//
// Modified - 12-Sep-2003 - Olivier Simard
//   - executable form (.cxx) to extend the option loading method.
//   - breaks the processing in easy steps .
//     and allow automatization.
//   - code is also run by calling QOCAAnalysis::RunQOCAFit(Narg, arg)
//
//-------------------------------------------------------------------------

#include <QOCAAnalysis.h>

int main(Int_t, Char_t**);

int main(Int_t Narg, Char_t** arg){

// check arguments provided to the function
  Int_t narg = Narg - 1;
  if((narg < 5) || (narg > 5)){
    fprintf(stderr,"Usage: qocafit_runner [cardfile] [step] [scan] [fitmethod] [wlen]\n");
    if(narg < 5) fprintf(stderr,"Needs more arguments. Exiting.\n");
    if(narg > 5) fprintf(stderr,"Too many arguments. Exiting.\n");
    exit(-1);
  }

  // initialize class and option file
  QOCAAnalysis* qa = new QOCAAnalysis(arg[1]); // no messages

  // call the QOCAFit function
  qa->RunQOCAFit(Narg, arg);

  return 0;
}

