// Using QOCATitles.cxx functions to produce SNOMAN titles files
//
// -----
// Author: Olivier Simard - November 2004
// ------------------------------------------
//
// os -- 08.2005
// Now loops over all systematics specified in the cardfile
// upon user decision. Produces the same titles banks and 
// command files; useful for systematics study of the detector
// response.
//


#include <QOCATitles.h>

int main(Int_t, Char_t**);

int main(Int_t Narg, Char_t** arg){

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
  Int_t* qtit_flag = new Int_t[Nfixed - 1];

  // fill the rest of the array if the flags
  // have been omitted by the user
  if(N < Nfixed){
    printf("Forcing default flag values for last %d arguments.\n",Nfixed-N);
  }
  for(Int_t iflag = 0 ; iflag < N ; iflag++){
    qtit_flag[iflag] = atoi(arg[iflag+2]);
  }
  for(Int_t iflag = N ; iflag < Nfixed ; iflag++){
    qtit_flag[iflag] = 0;
  }

  // print flag summary
  printf("------------------------------------------------------\n");
  printf("Argument Summary:\n");
  printf("Executable: %s\nCardfile: %s\n",arg[0],arg[1]);
  printf("Flags:\n");
  printf("\t%30s: \t%s\n","QOCAFit Reduced ChiSquare","N/A");
  printf("\t%30s: \t%d\n","Media Attenuations",qtit_flag[1]);
  printf("\t%30s: \t%d\n","PMT Angular Response",qtit_flag[2]);
  printf("\t%30s: \t%d\n","Laserball Distribution",qtit_flag[3]);
  printf("\t%30s: \t%d\n","Laserball Mask Function",qtit_flag[4]);
  printf("\t%30s: \t%d\n","PMT Model Efficiencies",qtit_flag[5]);
  printf("\t%30s: \t%d\n","Averages",qtit_flag[6]);
  printf("\t%30s: \t%d\n","Time Dependencies",qtit_flag[7]);
  printf("\t%30s: \t%s\n","Unused","N/A");
  printf("\t%30s: \t%s\n","Unused","N/A");
  printf("------------------------------------------------------\n");

  // loading class with specified cardfile location
  QOCALoad* ql = new QOCALoad();
  ql->SetFullname(
  	ql->AppendElement((Char_t*)getenv("PWD"), "/"), cardfile);

  // load input/output directories
  ql->SetKeyword("io");
  Char_t** dirs = ql->CreateArray(ql->GetFullname());

  // Titles files class (put directories in cardfile with keyword "io")
  // analysis mode + display
  QOCATitles* qt = new QOCATitles(cardfile, 1, 1, dirs[1], dirs[2]);

  // options to be loaded
  ql->SetKeyword("options");
  Char_t** opt = ql->CreateArray(ql->GetFullname());
  // os -- does not need to specify this anymore
  // Int_t syst = atoi(opt[0]);
  // qt->SetSystIndex(syst);
  Int_t extrapo_num = atoi(opt[1]);
  qt->SetNextrapbin(extrapo_num);
  Bool_t usefit = (Bool_t) atoi(opt[2]);
  qt->SetUseFit(usefit);
  delete [] opt;

  // option for "funny" attenuation at 337 nm for sep00
  ql->SetKeyword("f337");
  qt->SetFunny((Bool_t) atoi(ql->CreateArray(ql->GetFullname())[0]));

  // acrylic and rayleigh measurements
  ql->SetKeyword("ray");
  Char_t** mea = ql->CreateArray(ql->GetFullname());
  Int_t ray_dim = ql->GetDimensionFast();
  Int_t ray_lines = ql->GetNLines();
  Char_t** lam = ql->GetColumn(mea,1); // wavelengths
  Char_t** rmd = ql->GetColumn(mea,2); // measurements in d2o
  Char_t** rmh = ql->GetColumn(mea,3); // measurements in h2o
  Char_t** acr = ql->GetColumn(mea,4); // acrylic attenuations
  Char_t** rqd = ql->GetColumn(mea,5); // calculated value in d2o
  Char_t** rqa = ql->GetColumn(mea,6); // calculated value in acr
  Char_t** rqh = ql->GetColumn(mea,7); // calculated value in h2o

  // convert values first
  Int_t* wlen = ql->ConverttoIntegers(lam,ray_lines);
  Double_t* drmd = ql->ConverttoDoubles(rmd,ray_lines);
  Double_t* drmh = ql->ConverttoDoubles(rmh,ray_lines);
  Double_t* drqd = ql->ConverttoDoubles(rqd,ray_lines);
  Double_t* drqa = ql->ConverttoDoubles(rqa,ray_lines);
  Double_t* drqh = ql->ConverttoDoubles(rqh,ray_lines);
  Double_t* acratt = ql->ConverttoDoubles(acr,ray_lines);

  // then assign directly if there is 6 wavelengths
  if(qt->GetNwlen() == ray_lines){
    qt->SetRayleighd2om(drmd);
    qt->SetRayleighh2om(drmh);
    qt->SetRayleighd2oq(drqd);
    qt->SetRayleighacrq(drqa);
    qt->SetRayleighh2oq(drqh);
    qt->SetAttacrLoaded(acratt);
  } 
  else { // less wavelengths : rearrange arrays
    Double_t* lessdrmd = new Double_t[qt->GetNwlen()];
    Double_t* lessdrmh = new Double_t[qt->GetNwlen()];
    Double_t* lessdrqd = new Double_t[qt->GetNwlen()];
    Double_t* lessdrqa = new Double_t[qt->GetNwlen()];
    Double_t* lessdrqh = new Double_t[qt->GetNwlen()];
    Double_t* lessacratt = new Double_t[qt->GetNwlen()];

    // reorganize indices
    for(Int_t iwlen = 0 ; iwlen < qt->GetNwlen() ; iwlen++){
      for(Int_t jwlen = 0 ; jwlen < ray_lines ; jwlen++){
        if((Int_t)qt->GetWlens()[iwlen] == wlen[jwlen]){ // wlens are the same
          lessdrmd[iwlen] = drmd[jwlen];
          lessdrmh[iwlen] = drmh[jwlen];
          lessdrqd[iwlen] = drqd[jwlen];
          lessdrqa[iwlen] = drqa[jwlen];
          lessdrqh[iwlen] = drqh[jwlen];
          lessacratt[iwlen] = drqh[jwlen];
        }
      }
    }
    // set shorter arrays
    qt->SetRayleighd2om(lessdrmd);
    qt->SetRayleighh2om(lessdrmh);
    qt->SetRayleighd2oq(lessdrqd);
    qt->SetRayleighacrq(lessdrqa);
    qt->SetRayleighh2oq(lessdrqh);
    qt->SetAttacrLoaded(acratt);
  }

  // bunch of deletion loops to get rid of stored values

  delete [] rmd; delete [] rmh; delete [] acr;
  delete [] rqd; delete [] rqa; delete [] rqh;

  for(Int_t i = 0 ; i < ray_dim ; i++) delete [] mea[i];
  delete [] mea;

  // fractions of rayleigh scattering in each medium
  ql->SetKeyword("frac");
  Char_t** frac = ql->CreateArray(ql->GetFullname());
  qt->SetRayleighFracd2o((Double_t) atof(frac[0]));
  qt->SetRayleighFracacr((Double_t) atof(frac[1]));
  qt->SetRayleighFrach2o((Double_t) atof(frac[2]));
  delete [] frac;

  // SNOMAN scale of rayleigh scattering in each medium
  ql->SetKeyword("scale");
  Char_t** scal = ql->CreateArray(ql->GetFullname());
  qt->SetRayleighScaled2o((Double_t) atof(scal[0]));
  qt->SetRayleighScaleacr((Double_t) atof(scal[1]));
  qt->SetRayleighScaleh2o((Double_t) atof(scal[2]));
  delete [] scal; 
  
  // parameter space limits
  ql->SetKeyword("angpar");
  Char_t** par1 = ql->CreateArray(ql->GetFullname());
  qt->SetAngleParLimits((Int_t) atoi(par1[0]), (Int_t) atoi(par1[1]));
  delete [] par1;
  ql->SetKeyword("lampar");
  Char_t** par2 = ql->CreateArray(ql->GetFullname());
  qt->SetLambdaParLimits((Int_t) atoi(par2[0]), (Int_t) atoi(par2[1]));
  delete [] par2;

  // phases and media
  ql->SetKeyword("media");
  Char_t** med = ql->CreateArray(ql->GetFullname());
  Int_t med_lines = ql->GetNLines();
  qt->SetNPhase(med_lines);
  Char_t** phases = ql->GetColumn(med,1);
  Int_t* codes = ql->ConverttoIntegers(ql->GetColumn(med,2),med_lines);
  for(Int_t iphase = 0 ; iphase < qt->GetNPhase() ; iphase++){
    if(!strcmp(phases[iphase], "hist")){qt->SetMediumCodehist(codes[iphase]);}
    if(!strcmp(phases[iphase], "pure")){qt->SetMediumCoded2o(codes[iphase]);}
    if(!strcmp(phases[iphase], "salt")){qt->SetMediumCodesalt(codes[iphase]);}
  }

  printf("Done loading options.\n");

  //
  // all options needed are now loaded
  //
  // ========================================================
  // ========================================================

  // ========================================================
  // first produce banks and command files for nominal
  qt->SetSystIndex(0);

  // Media Attenuations (fruns)
  if(qtit_flag[1]){
    qt->SetKind("fruns");
    qt->SetILabel("attenuation");
    qt->SetOLabel("media_qoca");
    if(qt->GetIsSyst()) {
      Char_t* iformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".txt");
      Char_t* oformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".cmd");
      qt->SetIFormat(iformat);
      qt->SetOFormat(oformat);
    } else {
      qt->SetIFormat(".txt");
      qt->SetOFormat(".cmd");
    }
    qt->LoadAttenuations();
    qt->MediaTitles();
  }

  // -----------------------------
  // PMT Angular Response and Reflectivity (fruns)
  if(qtit_flag[2]){
    Char_t pmttitle[1024];
    sprintf(pmttitle,"%s/snoman/%s/prod/pmt_response.dat",
		    (Char_t*)getenv("SNO_ROOT"),(Char_t*)getenv("SNO_SNOMAN_VER"));
    qt->SetFnamPmtResp(pmttitle);
    qt->Diagnostic();
    qt->SetKind("fruns");
    qt->SetILabel("pmtAngResp");
    qt->SetOLabel("pmt_response_qoca");
    if(qt->GetIsSyst()) {
      Char_t* iformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".txt");
      Char_t* oformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".dat");
      qt->SetIFormat(iformat);
      qt->SetOFormat(oformat);
    } else {
      qt->SetIFormat(".txt");
      qt->SetOFormat(".dat");
    }
    qt->LoadPMTR("boulayreflv6");
    qt->PMTRespTitles();
  }

  // -----------------------------
  // Laserball distribution and mask function
  if(qtit_flag[3] || qtit_flag[4]){
    qt->SetKind("fruns");
    qt->SetILabel("lbdist"); // first to be done
    qt->SetOLabel("laserball_info");
    if(qt->GetIsSyst()) {
      Char_t* iformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".txt");
      Char_t* oformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".dat");
      qt->SetIFormat(iformat);
      qt->SetOFormat(oformat);
    } else {
      qt->SetIFormat(".txt");
      qt->SetOFormat(".dat");
    }
    qt->LoadLBdist();
    qt->LBdistTitles();
  }

  // -----------------------------
  // PMT Relative Efficiencies
  if(qtit_flag[5]){
    qt->SetKind("fruns");
    qt->SetILabel("eff");
    qt->SetOLabel("opt_variations");
    if(qt->GetIsSyst()) {
      Char_t* iformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".txt");
      Char_t* oformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".dat");
      qt->SetIFormat(iformat);
      qt->SetOFormat(oformat);
    } else {
      qt->SetIFormat(".txt");
      qt->SetOFormat(".dat");
    }
    qt->LoadEfficiencies();
    qt->PMTeffvarTitles();
  }

  // -----------------------------
  // Media Attenuations Drift Function (fruns)
  if(qtit_flag[7]){
    qt->SetKind("fruns");
    qt->SetILabel("attdrift");
    qt->SetOLabel("mc_drift_attenuation");
    if(qt->GetIsSyst()) {
      Char_t* iformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".txt");
      Char_t* oformat = ql->AppendElement(
			ql->AppendElement("_",qt->GetSystName(0)),".dat");
      qt->SetIFormat(iformat);
      qt->SetOFormat(oformat);
    } else {
      qt->SetIFormat(".txt");
      qt->SetOFormat(".dat");
    }
    Int_t nphase_default = 3; // number of maximum phases in that function
    qt->LoadPolyTerms(phases,nphase_default);
    qt->DriftTitles();
  }
  




  // ========================================================
  // Then check if the user decided to produce the banks and command files
  // for each systematics. 
  if(qt->GetFullOutput()){
    for(Int_t isyst = 1 ; isyst < qt->GetNsyst() ; isyst++){ // avoid nominal
      qt->SetSystIndex(isyst);
      printf("At systematic %.2d : %s\n",isyst,qt->GetSystName(isyst)); 

      // -----------------------------
      // Media Attenuations (fruns)
      if(qtit_flag[1]){
        qt->SetKind("fruns");
        qt->SetILabel("attenuation");
        qt->SetOLabel("media_qoca");
        Char_t* iformat = qt->GetSystOFormat(isyst,".txt");
        Char_t* oformat = qt->GetSystOFormat(isyst,".cmd");
        qt->SetIFormat(iformat);
        qt->SetOFormat(oformat);
        qt->LoadAttenuations();
        qt->MediaTitles();
      }

      // -----------------------------
      // PMT Angular Response and Reflectivity (fruns)
      if(qtit_flag[2]){
        Char_t pmttitle[1024];
        sprintf(pmttitle,"%s/snoman/%s/prod/pmt_response.dat",
	       (Char_t*)getenv("SNO_ROOT"),(Char_t*)getenv("SNO_SNOMAN_VER"));
        qt->SetFnamPmtResp(pmttitle);
        qt->Diagnostic();
        qt->SetKind("fruns");
        qt->SetILabel("pmtAngResp");
        qt->SetOLabel("pmt_response_qoca");
        Char_t* iformat = qt->GetSystOFormat(isyst,".txt");
        Char_t* oformat = qt->GetSystOFormat(isyst,".dat");
        qt->SetIFormat(iformat);
        qt->SetOFormat(oformat);
        qt->LoadPMTR("boulayreflv6");
        qt->PMTRespTitles();
      }

      // -----------------------------
      // Laserball distribution and mask function
      if(qtit_flag[3] || qtit_flag[4]){
        qt->SetKind("fruns");
        qt->SetILabel("lbdist"); // first to be done
        qt->SetOLabel("laserball_info");
        Char_t* iformat = qt->GetSystOFormat(isyst,".txt");
        Char_t* oformat = qt->GetSystOFormat(isyst,".dat");
        qt->SetIFormat(iformat);
        qt->SetOFormat(oformat);
        qt->LoadLBdist();
        qt->LBdistTitles();
      }

      // -----------------------------
      // PMT Relative Efficiencies
      if(qtit_flag[5]){
        qt->SetKind("fruns");
        qt->SetILabel("eff");
        qt->SetOLabel("opt_variations");
        Char_t* iformat = qt->GetSystOFormat(isyst,".txt");
        Char_t* oformat = qt->GetSystOFormat(isyst,".dat");
        qt->SetIFormat(iformat);
        qt->SetOFormat(oformat);
        qt->LoadEfficiencies();
        qt->PMTeffvarTitles();
      }

      // -----------------------------
      // Media Attenuations Drift Function (fruns)
      if(qtit_flag[7]){
        qt->SetKind("fruns");
        qt->SetILabel("attdrift");
        qt->SetOLabel("mc_drift_attenuation");
        Char_t* iformat = qt->GetSystOFormat(isyst,".txt");
        Char_t* oformat = qt->GetSystOFormat(isyst,".dat");
        qt->SetIFormat(iformat);
        qt->SetOFormat(oformat);
        Int_t nphase_default = 3; // number of maximum phases in that function
        qt->LoadPolyTerms(phases,nphase_default);
        qt->DriftTitles();
      }

    } // for(isyst)
  } // fulloutput bracket

  return 0;
}
