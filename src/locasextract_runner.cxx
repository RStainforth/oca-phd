// Using QOCAExtract.cxx functions to extract
// Optics related parameters from generated
// qocafit files
// -----
// Author: Olivier Simard - November 2004
// -----
//
// ------------------------------------------

#include <QOCAExtract.h>

int main(Int_t, Char_t**);

int main(Int_t Narg, Char_t** arg){

  // -----------------------------------------------------
  //
  // Produces extracted textfiles by reading the qocafit rootfiles
  //
  // Medium and fit type are selected through the function
  // QOCAPlots::SetFlags(fits, fruns, medium)
  // where the arguments are integers:
  //
  // fits, fruns = 0 : do not plot this data
  // fits, fruns = 1 : plot this data
  // medium: (0: D2O, 1:ACR, 2:H2O)
  //   medium only needed for attenuation data,
  //   pmt angular response will ignore it
  //
  // examples:
  // - (0,0,0) or () : default = plot fruns for D2O
  // - (1,0,0) or (1): plot only fits data for D2O
  // - (1,1,2) : plot both fits/fruns data for H2O
  //
  // note: some function will not plot both fits/fruns
  // on the same plot. warnings exist in the code anyway.
  //
  // Input/Ouput labels and format only need to be specified
  // if multiple function calls are made.
  //
  // -----------------------------------------------------

  // rearrange the number of arguments
  Int_t N = Narg - 1;
  Int_t Nfixed = 11; // 10 flags + cardfile name

  // quick check of the arguments
  if(N <= 0){
    fprintf(stderr,"Bad arguments. Usage:\n");
    fprintf(stderr," > %s [cardfile] [flags]\n",arg[0]);
    exit(-1);
  }

  // keep the cardfile name
  Char_t* cardfile = arg[1];
  N--;

  // create an array of integers to contain the flags
  Int_t* qex_flag = new Int_t[Nfixed - 1];

  // fill the rest of the array if the flags
  // have been omitted by the user
  if(N < Nfixed){
    printf("Forcing default flag values for last %d arguments.\n",Nfixed-N);
  }
  for(Int_t iflag = 0 ; iflag < N ; iflag++){
    qex_flag[iflag] = atoi(arg[iflag+2]);
  }
  for(Int_t iflag = N ; iflag < Nfixed ; iflag++){
    qex_flag[iflag] = 0;
  }

  // print flag summary
  printf("------------------------------------------------------\n");
  printf("Argument Summary:\n");
  printf("Executable: %s\nCardfile: %s\n",arg[0],arg[1]);
  printf("Flags:\n");
  printf("\t%30s: \t%d\n","QOCAFit Reduced ChiSquare",qex_flag[0]);
  printf("\t%30s: \t%d\n","Media Attenuations",qex_flag[1]);
  printf("\t%30s: \t%d\n","PMT Angular Response",qex_flag[2]);
  printf("\t%30s: \t%d\n","Laserball Distribution",qex_flag[3]);
  printf("\t%30s: \t%d\n","Laserball Mask Function",qex_flag[4]);
  printf("\t%30s: \t%d\n","PMT Model Efficiencies",qex_flag[5]);
  printf("\t%30s: \t%d\n","Averages",qex_flag[6]);
  printf("\t%30s: \t%s\n","Time Dependencies","N/A");
  printf("\t%30s: \t%d\n","Time Windows",qex_flag[8]);
  printf("\t%30s: \t%s\n","Unused","N/A");
  printf("------------------------------------------------------\n");


  // loading class with specified cardfile location
  QOCALoad* ql = new QOCALoad();
  ql->SetFullname(
  	ql->AppendElement((Char_t*)getenv("PWD"), "/"), cardfile);

  // load input/output directories
  ql->SetKeyword("io");
  Char_t** dirs = ql->CreateArray(ql->GetFullname());

  // load the data to plot (fits or fruns or both)
  ql->SetKeyword("data");
  Char_t** data_c = ql->CreateArray(ql->GetFullname());
  Int_t* data = ql->ConverttoIntegers(ql->GetLine(data_c,1), ql->GetNColumns());
  Int_t fits_flag = data[0];
  Int_t fruns_flag = data[1];
  Int_t eff_flag = data[2];

  // Extraction class (put directories in cardfile with keyword "io")
  // analysis mode + display
  QOCAExtract* qex = new QOCAExtract(cardfile,1,1,dirs[0],dirs[1]);

  // set up data to extract
  qex->SetFlags(fits_flag,fruns_flag,eff_flag);

  // ----------------------------------
  // extract according to list of flags

  // reduced fit chisquare
  if(qex_flag[0]){
    qex->SetOLabel("chi");
    qex->SetOFormat(".txt");
    qex->ExtractChiSquare();
  }

  // media attenuation coefficients
  if(qex_flag[1]){
    qex->SetOLabel("attenuation");
    qex->SetOFormat(".txt");
    qex->ExtractAttenuation("D2O");
    qex->ExtractAttenuation("H2O");
  }

  // pmt angular response
  // 09.2006 - O.Simard
  // Change to extract both pmt angular response if the bit is 2.
  if(qex_flag[2] > 0){
    qex->SetOLabel("pmtAngResp");
    qex->SetOFormat(".txt");
    qex->ExtractAngularResponse();
    if(qex_flag[2] == 2){
      qex->SetOLabel("pmtAngResp2");
      qex->SetOFormat(".txt");
      qex->ExtractAngularResponse2();
    }
  }

  // laserball distribution
  if(qex_flag[3]){
    qex->SetOLabel("lbdist");
    qex->SetOFormat(".txt");
    qex->ExtractLBdist();
  }

  // laserball mask function polynomials
  if(qex_flag[4]){
    qex->SetOLabel("lbpoly");
    qex->SetOFormat(".txt");
    qex->ExtractLBpoly();
  }

  // efficiencies, incident angles, occupancies
  if(qex_flag[5]){
    qex->SetOLabel("eff");
    qex->SetOFormat(".txt");
    qex->ExtractEfficiencies();
  }

  // averages (need two scans or more)
  if(qex_flag[6] && (qex->GetNscan() > 1)){
    // attenuations
    qex->SetILabel("attenuation"); qex->SetOLabel("attenuation");
    qex->SetIFormat(".txt"); qex->SetOFormat(".txt");
    qex->ExtractAttAverages("D2O");
    qex->ExtractAttAverages("H2O");

    // pmtr
    qex->SetILabel("pmtAngResp"); qex->SetOLabel("pmtAngResp");
    qex->SetIFormat(".txt"); qex->SetOFormat(".txt");
    qex->ExtractPmtrAverages();
  }

  // media attenuation coefficients
  // as a function of prompt time window size
  if(qex_flag[8]){
    qex->SetOLabel("timewindows");
    qex->SetOFormat(".txt");
    qex->ExtractTimeWindows("D2O");
    qex->ExtractTimeWindows("H2O");
  }

  cout << "qocaextract_runner.cxx: Done extracting.\n";
  return 0;
}

