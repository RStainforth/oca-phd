//*-- Author :    Olivier Simard   2006 03
/*************************************************************************
 * Copyright(c) 2006, The SNO Software Project, All rights reserved.     *
 * Authors:  Olivier Simard                                              *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include "QLaserballMC.h"
#include "TMath.h"

ClassImp(QLaserballMC)
;
//
//  ------------------------------------------------------------
// - QLaserballMC: Methods for Easy Laserball MC Production     -
//  ------------------------------------------------------------
//
//  This class contains basic functions to set various options
//  and produce batch command files needed to produce laserball
//  (or similar mono-energetic sources of photons) Monte Carlo
//  data using SNOMAN.
//
//  The constructor and destructor are very basic; they set some
//  values that the user must set relative to its own computing
//  facility.
//
//  The main functions are:
//  
//  ProduceMCScan(args): produces a file name according to the arguments,
//  opens it, and accesses the QOCATree structure. The information is 
//  accessed on a run-by-run basis: run number, number of laser pulses (events),
//  source position, wavelength, etc. to keep the MC as similar as the data
//  as possible.
//  LoadRunInfo(QOCARun*) is the function that reads the necessary 
//  information from a QOCARun object.
//
//  WriteBatFile() and WriteCommandFile() are functions producing the 
//  necessary files to run SNOMAN. Options needs to be carefully set
//  before calling these functions.
//  ** Any change in the MC settings must be made in WriteCommandFile()
//  which contains all the SNOMAN commands.
//
//  WriteSplitCommandFiles() and ReconstructRch() are complementary
//  functions that allows to user to split a long MC job in many 
//  shorter separate jobs. This allows parallel processing. The files
//  produced are then recombined in a single one RCH file.
//
//
//______________________________________________________________________________
QLaserballMC::QLaserballMC()
{
  // Default contructor:
  // Give some default values to most of the variables, except for filenames.

  fSetupFile = "~/.cshrc"; // source user's settings
  fWdir = (Char_t*)getenv("PWD"); // set by default under linux
  fIdir = (Char_t*)getenv("PWD"); // set by default under linux
  fDQXXdir = (Char_t*)getenv("DQXXDIR"); // can be added to settings
  fPhase = "d2o_2"; // default is d2o_2 (great October 2003)
  // some titles available in /prod or /mcprod
  fLaserDistFile = "laserball_info_oct03.dat";
  fLaserDyesFile = "laserball_dye_default.dat"; // should check this one
  fPmtResponseFile = "pmt_response.dat";
  fMediaFile = "media_qoca_d2o_20060110.cmd";
  fPmtVariationFile = "pmt_variations_salt.dat";
  fBatFile = "laser_default.bat";
  fCmdFile = "laser_default.cmd";
  fRchFile = "laser_default.rch";
  fRdtFile = "snomc_default.rdt";
  fLogFile = "laser_default.log";
  fSeedFile = "seed_default.dat";
  
  fRun = 0; // no run number to start with
  fSourceModel = 170; // can put any source geometry code here (point, sphere, disk, etc.)
  fSourceRadius = 5.45; // laserball size in cm
  fNevents = 50000; // default number of events
  fIntensity = 2500; // default number of photons generated per event
  fSourceSlot = 0; // south
  fSourcePhi = 0;   // x-axis
  fSourceTheta = 0; // z-axis
  fEnergy = NULL;
  //fPosition = 0;
  fWlen = 420;
  fSeed1 = 1; // mc input seed1
  fSeed2 = 2; // mc input seed2
  fFactor = 1; // multiplicative factor
  fNsubruns = 2; // number of subruns if too many events
  fNeventlimit = 25000; // number of events to generate per process

  fDQXX = kTRUE; // use DQXX info by default
  fNcd = kFALSE; // default is no ncds
  fPmtvar = kFALSE; // pmt variations turned off
  fPmtbounce = kFALSE; // grey disk pmt model bounce off
  fPmt3dmodel = kFALSE; // turn off 3dPMT model (use grey disk)
  fRayleigh = kFALSE; // turn off Rayleigh scattering model
  fTdiff = kFALSE; // do not run tdiff
  fPosFit = kFALSE; // do not run qoca_positionfit
  fRsp = kTRUE; // run rsp by default
  fFresnel = kTRUE;
  fPhotodis = kTRUE;
  
  fRandom = new TRandom(0);
}

//______________________________________________________________________________
QLaserballMC::~QLaserballMC()
{
  // Default destructor

  delete fRandom;
}

//______________________________________________________________________________
void QLaserballMC::ProduceMCScan(Char_t* aFileType, Char_t* aScan, Char_t* aKind, Int_t aWlen, Char_t* aTree)
{
  // This functions takes all the arguments, build a file name, opens it,
  // and goes down the tree through all entries.
  // All applicable options should be set before calling this function. 

  // build the rootfile name
  Char_t file[1024]; 
  sprintf(file,"%s/%s_%s_%s_%d.root",fIdir,aFileType,aScan,aKind,aWlen);

  printf("=======================================\n");
  printf("In QLaserballMC::ProduceMCScan():\n");
  printf("Producing MC scan from file: \n  %s\n",file);

  // read the QOCATree
  TFile* fin = new TFile(file,"READ");
  QOCATree* ocatree = (QOCATree*) fin->Get(aTree);
  QOCARun* ocarun = ocatree->GetQOCARun();
  Int_t nrun = (Int_t) ocatree->GetEntries();
  Int_t nsplit = 1;
    
  // run loop
  for(Int_t irun = 0 ; irun < nrun ; irun++){

    ocatree->GetEntry(irun); // select run
    LoadRunInfo(ocarun);

    if((fNsubruns > 1) && (fNevents > fNeventlimit)){
      if(fNeventlimit == 0) fNeventlimit = 1;
      nsplit = (Int_t)((Float_t)fNevents/(Float_t)fNeventlimit);
       // set nsplit to 2 when the fraction is rounded to 1
      if(nsplit == 1) nsplit = 2;
      WriteSplitCommandFiles(nsplit); // subruns
    } else {
      // produce filenames
      Char_t rchname[1024],rdtname[1024],cmdname[1024];
      Char_t batname[1024],logname[1024],seedname[1024];
      sprintf(rchname,"%s/laser_%d.rch",fWdir,fRun); fRchFile = rchname;
      sprintf(rdtname,"%s/snomc_%d.rdt",fWdir,fRun); fRdtFile = rdtname;
      sprintf(cmdname,"%s/laser_%d.cmd",fWdir,fRun); fCmdFile = cmdname;
      sprintf(batname,"%s/laser_%d.bat",fWdir,fRun); fBatFile = batname;
      sprintf(logname,"%s/laser_%d.log",fWdir,fRun); fLogFile = logname;
      sprintf(seedname,"%s/seed_%d.dat",fWdir,fRun); fSeedFile = seedname;
      // set the seeds
      SetSeeds(0);
      // call the file-writing functions
      WriteCommandFile();
      WriteBatFile();
    }
  }

  // close file - done
  ocarun->Clear();
  ocatree->Clear();
  fin->Close(); delete fin;

  printf("Done.\n");
  printf("=======================================\n");

  return;
}

//______________________________________________________________________________
void QLaserballMC::WriteBatFile()
{
  // Writes the typical bat file to run SNOMAN.

  FILE* fbat = fopen(fBatFile,"w");
  if(!fbat){ Error("WriteBatFile","Problem with FILE pointer location.");
    exit(-1);
  }
  fprintf(fbat,"#!/bin/csh\n");
  fprintf(fbat,"source %s\n",fSetupFile);
  fprintf(fbat,"snomanpp %s < %s\n",fLogFile,fCmdFile);
  if(fTdiff){
    // run tdiff according to rsps flag
    if(fRsp) fprintf(fbat,"tdiff -f %d %s %s\n",fRun,fRchFile,fRdtFile);
    else fprintf(fbat,"tdiff 1 %s %s\n",fRchFile,fRdtFile);
    // run qoca_positionfit only if tdiff was run
    if(fPosFit){
      fprintf(fbat,"qoca_positionfit %d mc %s %s %d\n",
	      fRun,fWdir,fDQXXdir,(Int_t)fWlen);
    }
  }
  fclose(fbat);
  
  return;
}

//______________________________________________________________________________
void QLaserballMC::WriteCommandFile()
{
  // Writes the typical command file to run SNOMAN's MC.
  // All the changes to the MC should be made here.
  // The input/output and physical information that SNOMAN needs
  // must be set before using the appropriate Set functions.
  
  FILE* fcmd = fopen(fCmdFile,"w");
  if(!fcmd){ Error("WriteCommandFile","Problem with FILE pointer location.");
    exit(-1);
  }
      
  // header
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Run Specific Settings for Optics ************\n");
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"**** load_%s_settings.cmd is called at the end ****\n\n",fPhase);
  fprintf(fcmd,"***************************************************\n");

  // mc seed    
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Random Seed *********************************\n");
  fprintf(fcmd,"$starting_seed %.5d %.5d 1 \n",fSeed1,fSeed2);
  fprintf(fcmd,"$seed_file '%s'\n",fSeedFile);
      
  // run info
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Run Information *****************************\n");
  fprintf(fcmd,"set bank MCMA 1 word 1 to %.5d \n",fRun);
  fprintf(fcmd,"$initial_date 20020522 15371700 \n");
  if(fDQXX){
    fprintf(fcmd,"$dqxx_info $on\n");
    fprintf(fcmd,"titles %s/dqxx_00000%.5d.dat\n",fDQXXdir,fRun);
  } else fprintf(fcmd,"$dqxx_info $off\n");
  fprintf(fcmd,"$mc_energy $en_mono %s\n",fEnergy);
  
  // laserball info
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Laser simulation settings *******************\n");

  // position type settings: MCPI 9 and following
  fprintf(fcmd,"set bank MCPI 1 word 9 to %d * source type\n",fSourceModel);
  fprintf(fcmd,"set bank MCPI 1 word 10 to %.2f * x of source position\n",fPosition.X());
  fprintf(fcmd,"set bank MCPI 1 word 11 to %.2f * y of source position\n",fPosition.Y());
  fprintf(fcmd,"set bank MCPI 1 word 12 to %.2f * z of source position\n",fPosition.Z());

  // laserball and disk need radius info
  if(fSourceModel >= 130){ // old laserball
    fprintf(fcmd,"set bank MCPI 1 word 13 to %.2f * radius of simulated source\n",fSourceRadius);
  }
  // some source types needs this directionality info
  if(fSourceModel == 140){ // laserdisk
    fprintf(fcmd,"set bank MCPI 1 word 14 to %.2f * theta orientation of source\n",fSourceTheta);
    fprintf(fcmd,"set bank MCPI 1 word 15 to %.2f * phi orientation of source\n",fSourcePhi);
  } 
  if(fSourceModel >= 170){ // laserball with bank
    fprintf(fcmd,"set bank MCPI 1 word 14 to %d. * azimuthal orientation of laserball SWNE=>0123\n",fSourceSlot);
  }

  // direction type settings: MCPI 20 and following
  fprintf(fcmd,"set bank MCPI 1 word 20 to %d\n",fSourceModel);
  if(fSourceModel == 130){ // old laserball
    fprintf(fcmd,"set bank MCPI 1 word 21 to %.2f * x of source position\n",fPosition.X());
    fprintf(fcmd,"set bank MCPI 1 word 22 to %.2f * y of source position\n",fPosition.Y());
    fprintf(fcmd,"set bank MCPI 1 word 23 to %.2f * z of source position\n",fPosition.Z());
  }
  if(fSourceModel == 140){ // laserdisk
    fprintf(fcmd,"set bank MCPI 1 word 21 to %.2f * theta orientation of source\n",fSourceTheta);
    fprintf(fcmd,"set bank MCPI 1 word 22 to %.2f * phi orientation of source\n",fSourcePhi);
  }
  // note: new laserball with bank requires no further settings
  
  // settings for QIO
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** QIO Processor settings **********************\n");
  fprintf(fcmd,"set bank TQIO 3 word 9 to 1 * use tqio pos in tof subtraction\n");
  fprintf(fcmd,"set bank TQIO 3 word 10 to %.2f * x of source position\n",fPosition.X());
  fprintf(fcmd,"set bank TQIO 3 word 11 to %.2f * y of source position\n",fPosition.Y());
  fprintf(fcmd,"set bank TQIO 3 word 12 to %.2f * z of source position\n",fPosition.Z());
  fprintf(fcmd,"FILE QIO 2 %s * output file\n",fRchFile);
  
  // number of events and photons per event
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Event generation settings *******************\n");
  fprintf(fcmd,"$num_events %d\n",fNevents);
  fprintf(fcmd,"$mc_misc_num_photons %d\n",fIntensity);
  fprintf(fcmd,"$max_cer_ge_errors %d * ignore photon tracking errors\n",fIntensity);
  
  // banks, titles and commands
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Optical Response settings *******************\n");
  if(fPmtvar){ // pmt variations
    fprintf(fcmd,"$pmt_var $on\n");
    fprintf(fcmd,"titles %s\n",fPmtVariationFile);
  } else {
    fprintf(fcmd,"$pmt_var $off\n");
  }
  fprintf(fcmd,"titles %s\n",fLaserDistFile);
  fprintf(fcmd,"titles %s\n",fLaserDyesFile); // do not need this for MC
  fprintf(fcmd,"titles %s\n",fPmtResponseFile);
  
  if(fPmt3dmodel){ // 3dpmt
    fprintf(fcmd,"* turn 3d pmt model on\n");
    fprintf(fcmd,"$3d_pmt * see further settings below\n");
  } else {
    fprintf(fcmd,"* turn grey disk pmt model on\n");
    fprintf(fcmd,"$grey_disk_pmt\n");
    if(fPmtbounce) fprintf(fcmd,"$grey_disk_pmt_bounce $on\n");
    else fprintf(fcmd,"$grey_disk_pmt_bounce $off\n");
  }    
  fprintf(fcmd,"set bank PMTR 1 word 2 to 1.5 * PMT time resolution is 1.5 ns\n");
  fprintf(fcmd,"* i/o file settings slightly different for laserball simulation\n");
  if(fRayleigh) fprintf(fcmd,"$rayleigh_scat $on\n");
  else fprintf(fcmd,"$rayleigh_scat $off\n");
  fprintf(fcmd,"@%s\n",fMediaFile);
  // others
  if(fFresnel) fprintf(fcmd,"$fresnel_scat $on\n");
  else fprintf(fcmd,"$fresnel_scat $off\n");
  if(fPhotodis) fprintf(fcmd,"$photodisintegration $on\n");
  else fprintf(fcmd,"$photodisintegration $off\n");
  
  // general settings
  fprintf(fcmd,"\n");
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** General Default Settings ********************\n\n");
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Event Loop **********************************\n");
  fprintf(fcmd,"define event_loop\n");
  fprintf(fcmd,"call mco\n");
  // Note on using rsp for optics MC:
  // turning on rsp flags more than 1000 tubes systematically
  // and takes some time to load.
  // do not turn on unless the MC needs to have the exact same
  // run conditions as data.
  if(fRsp) fprintf(fcmd,"call rsp\n");
  fprintf(fcmd,"call qio(2)\n");
  fprintf(fcmd,"end_def\n");
  fprintf(fcmd,"\n");
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Monte Carlo settings ************************\n");
  fprintf(fcmd,"$pmt_nois_rate 0.0\n");
  fprintf(fcmd,"$mc_event_rate 45. $per_sec\n"); 
  fprintf(fcmd,"$mc_interaction_type   $start_photon_bomb\n");
  fprintf(fcmd,"$mc_time               $tim_gaussian 98.5 110. 0.7 100.\n");
  fprintf(fcmd,"$mc_miscellaneous      $misc_random_pol\n");
  fprintf(fcmd,"$mc_misc_find_region   $unknown\n");
  fprintf(fcmd,"$mc_num_seed_vx 1\n");
  fprintf(fcmd,"$mc_drift_attenuation $off * avoid d2o attenuation overwrite\n");
  fprintf(fcmd,"\n");
  
  fprintf(fcmd,"***** MC DAQ settings *****************************\n");
  fprintf(fcmd,"$ext_flag $asynch\n");	
  fprintf(fcmd,"set bank MCEL 1 word 2 to 175.0 * trigger time fix (M. Dunford)\n");
  fprintf(fcmd,"set bank MCDQ 1 word 23 to 2 * sets external source to LASER\n");
  fprintf(fcmd,"set bank MCDQ 1 word 29 to 32768\n");
  fprintf(fcmd,"set bank MCDQ 1 word 30 to 32768\n");
  fprintf(fcmd,"\n");
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** MC QIO settings *****************************\n");
  fprintf(fcmd,"set bank TQIO 3 word 1 to 1 * output is an rch file\n");
  fprintf(fcmd,"set bank TQIO 3 word 17 to 1.3549 * group refractive index for time residuals (hardcoded to 500 nm)\n");
  fprintf(fcmd,"set bank TQIO 3 word 19 to 0 * don't store DAMN/DARN words\n");
  fprintf(fcmd,"set bank TQIO 3 word 27 to 2 * number of vertex types\n");
  fprintf(fcmd,"set bank TQIO 3 word 28 to 100\n");
  fprintf(fcmd,"set bank TQIO 3 word 29 to 199\n\n");

  // can force ncd in the geometry for any phase
  if(fNcd){
    fprintf(fcmd,"***************************************************\n");
    fprintf(fcmd,"***** Force NCDs **********************************\n");
    fprintf(fcmd,"$enable_geom $NCD_OVL\n");
    fprintf(fcmd,"$enable_geom $NCD_LIVL\n");
    fprintf(fcmd,"$enable_geom $NCD_DIVL\n");
    fprintf(fcmd,"$enable_geom $NCD_WIRE\n");
    fprintf(fcmd,"$enable_geom $NCD_EC\n");
    fprintf(fcmd,"$enable_geom $NCD_LIVL_END\n");
    fprintf(fcmd,"$enable_geom $NCC\n\n");
  }

  // disable cerfac (does weird things sometimes)
  fprintf(fcmd,"***************************************************\n");
  fprintf(fcmd,"***** Disable Cerfac ******************************\n");
  fprintf(fcmd,"$mode_cerfac 1\n");
  fprintf(fcmd,"$cerfac 1.0\n");
  fprintf(fcmd,"$cerfac_ii 1.0\n");
  fprintf(fcmd,"$np_external_cerfac 0\n\n");

  // other default settings
  fprintf(fcmd,"@load_%s_settings.cmd\n",fPhase); // from mcprod
  fprintf(fcmd,"@run_snodb\n"); // anything else snoman needs
  
  fclose(fcmd);

  printf("QLaserballMC::WriteCommandFile:\nCommand File %s was produced successfully.\n",
	 fCmdFile);

  return;
}

//______________________________________________________________________________
void QLaserballMC::LoadRunInfo(QOCARun* ocarun)
{
  // Loads the run-specific info from a QOCARun object.
  
  printf("QLaserballMC::LoadRunInfo: Loading info for run %d...\n",ocarun->GetRun());

  // get the run number
  fRun = ocarun->GetRun();

  // wavelength to single photon energy
  Float_t lambda = ocarun->GetLambda(); // wavelength in nm
  fWlen = lambda;
  Char_t energy[512];
  // need to convert the energy in MeV
  Double_t energy_d = LambdaToEnergy(lambda);
  sprintf(energy,"%.4e",energy_d); // convert to character type
  fEnergy = energy;
  
  // source center coordinates
  TVector3 position = ocarun->GetManipxyz(); // source center
  fPosition = position;

  // number of events
  Int_t npulses = ocarun->GetNpulses(); // number of events
  fNevents = (Int_t)fFactor*npulses; // scale it by some factor
  
  // source orientation code:
  // the following applies for the MCPI bank convention (same as QOCA)
  Float_t lbphi = ocarun->GetBallphi(); // source orientation
  Float_t pi = (Float_t) TMath::Pi();
  if((1.0*pi - 0.1 < lbphi) && (lbphi < 1.0*pi + 0.1)) fSourceSlot = 2; // north
  if((1.5*pi - 0.1 < lbphi) && (lbphi < 1.5*pi + 0.1)) fSourceSlot = 3; // east 
  if((0.0*pi - 0.1 < lbphi) && (lbphi < 0.0*pi + 0.1)) fSourceSlot = 0; // south
  if((0.5*pi - 0.1 < lbphi) && (lbphi < 0.5*pi + 0.1)) fSourceSlot = 1; // west

  return;
}

//______________________________________________________________________________
void QLaserballMC::WriteSplitCommandFiles(Int_t aNumber)
{
  // Produces multiple command files for long jobs (to be run in parallel).
  // LoadRunInfo needs to be called before.

  Char_t rchname[1024],rdtname[1024],cmdname[1024];
  Char_t batname[1024],logname[1024],seedname[1024]; 

  // number of events needs to be reduced for each sub-run
  Int_t nevents = (Int_t) fNevents/aNumber;
  fNevents = nevents;

  // save the tdiff flag (disabled when subruns)
  Bool_t tdiff_flag = fTdiff;
  fTdiff = kFALSE; // disable

  for(Int_t isubrun = 0 ; isubrun < aNumber ; isubrun++){
    
    // rename original files
    sprintf(rchname,"%s/laser_%d.part%.2d.rch",fWdir,fRun,isubrun);
    fRchFile = rchname;
    sprintf(rdtname,"%s/snomc_%d.part%.2d.rdt",fWdir,fRun,isubrun);
    fRdtFile = rdtname;
    sprintf(cmdname,"%s/laser_%d.part%.2d.cmd",fWdir,fRun,isubrun);
    fCmdFile = cmdname;
    sprintf(batname,"%s/laser_%d.part%.2d.bat",fWdir,fRun,isubrun);
    fBatFile = batname;
    sprintf(logname,"%s/laser_%d.part%.2d.log",fWdir,fRun,isubrun);
    fLogFile = logname;
    sprintf(seedname,"%s/seed_%d.part%.2d.dat",fWdir,fRun,isubrun);
    fSeedFile = seedname;

    // define seeds
    SetSeeds(fRun + nevents*isubrun); // only needs to be different

    // write files for each subrun
    WriteCommandFile();
    WriteBatFile();
  }

  // reset fTdiff
  fTdiff = tdiff_flag;

  return;
}

//______________________________________________________________________________
void QLaserballMC::ReconstructRch(Int_t aRunNumber)
{
  // Takes a list of rch files, produced from splitted command
  // files and sum them into one file. fNsubruns needs to be set.
  //
  // This function really merge the (TH2F*)fRchTime histogram.
  // All other histograms are copied (not summed) because
  // they contain the same information for all subruns.
  // Note: charge histograms are not used for optics anyway.

  // modify fRun ; otherwise keep current
  if(aRunNumber > 0) fRun = aRunNumber;

  if(fNsubruns <= 1){
    Warning("QLaserballMC::ReconstructRch","No need to merge one rch file.");
    exit(-1);
  }

  Char_t outputrch[1024]; Char_t rchfile[1024];
  // single file that is the sum of all
  sprintf(outputrch,"%s/laser_%d.rch",fWdir,fRun);
  TFile* target = new TFile(outputrch,"RECREATE");

  // read the first subfile
  Int_t ifile = 0;
  sprintf(rchfile,"%s/laser_%d.part%.2d.rch",fWdir,fRun,ifile);
  TFile* subfile = new TFile(rchfile,"READ");

  // the list that contains the keys to be saved
  TList* list = new TList();
  list->Add(subfile);

  // point to the first entry in the list
  TFile* first_source = (TFile*)list->First();
  first_source->cd();
  TDirectory *current_sourcedir = gDirectory;

  // loop over the entries of the file
  TIter nextkey(current_sourcedir->GetListOfKeys());
  TKey *key;
  while((key = (TKey*)nextkey())){
    first_source->cd();
    TObject *obj1 = key->ReadObj();
    if(obj1) printf("Copying object: %s\n",key->GetName());
   
    if(!strcmp(obj1->GetName(),"fRchTime")){
      TH2F* h1 = (TH2F*)obj1; // primary fRchTime
      // loop over subrun files
      for(ifile = 1 ; ifile < fNsubruns; ifile++){
        sprintf(rchfile,"%s/laser_%d.part%.2d.rch",fWdir,fRun,ifile);
        TFile* subfileloop = new TFile(rchfile,"READ");
        // get the same histogram from this file
        TH2F* h2 = (TH2F*)subfileloop->Get("fRchTime");
        if(h2){
          printf("\tAdding object: %s\n",h2->GetName());
          h1->Add(h2);
          delete h2;
        }
        subfileloop->Close();
      }
    } 

    // write histogram to single file
    target->cd();
    obj1->Write(key->GetName());

  }

  // close opened files and get out
  subfile->Close();
  delete list;
  target->Close(); delete target;
  return;
}
//______________________________________________________________________________
void QLaserballMC::ReconstructRch(Char_t* aString)
{
  // Overloaded version to take a string as argument instead of a run number.

  if(aString == NULL){
    Warning("QLaserballMC::ReconstructRch","No rch string provided.");
    exit(-1);
  }

  if(fNsubruns <= 1){
    Warning("QLaserballMC::ReconstructRch","No need to merge one rch file.");
    exit(-1);
  }

  Char_t outputrch[1024]; Char_t rchfile[1024];
  // single file that is the sum of all
  sprintf(outputrch,"%s/laser_%s.rch",fWdir,aString);
  TFile* target = new TFile(outputrch,"RECREATE");

  // read the first subfile
  Int_t ifile = 0;
  sprintf(rchfile,"%s/laser_%s.part%.2d.rch",fWdir,aString,ifile);
  TFile* subfile = new TFile(rchfile,"READ");

  // the list that contains the keys to be saved
  TList* list = new TList();
  list->Add(subfile);

  // point to the first entry in the list
  TFile* first_source = (TFile*)list->First();
  first_source->cd();
  TDirectory *current_sourcedir = gDirectory;

  // loop over the entries of the file
  TIter nextkey(current_sourcedir->GetListOfKeys());
  TKey *key;
  while((key = (TKey*)nextkey())){
    first_source->cd();
    TObject *obj1 = key->ReadObj();
    if(obj1) printf("Copying object: %s\n",key->GetName());
   
    if(!strcmp(obj1->GetName(),"fRchTime")){
      TH2F* h1 = (TH2F*)obj1; // primary fRchTime
      // loop over subrun files
      for(ifile = 1 ; ifile < fNsubruns; ifile++){
        sprintf(rchfile,"%s/laser_%s.part%.2d.rch",fWdir,aString,ifile);
        TFile* subfileloop = new TFile(rchfile,"READ");
        // get the same histogram from this file
        TH2F* h2 = (TH2F*)subfileloop->Get("fRchTime");
        if(h2){
          printf("\tAdding object: %s\n",h2->GetName());
          h1->Add(h2);
          delete h2;
        }
        subfileloop->Close();
      }
    } 

    // write histogram to single file
    target->cd();
    obj1->Write(key->GetName());

  }

  // close opened files and get out
  subfile->Close();
  delete list;
  target->Close(); delete target;
  return;
}

//______________________________________________________________________________
void QLaserballMC::SetSeeds(Int_t aRunNumber)
{
  // Provide needed seeds based on the run number and internal
  // computer clock.

  Int_t inputseed;
  if(aRunNumber == 0) inputseed = fRun;
  else inputseed = aRunNumber;
  
  fRandom->SetSeed((UInt_t)inputseed);
  fSeed1 = 1 + (Int_t)fRandom->Integer(31328); // SNOMAN limit for seed1
  fRandom->SetSeed(0); // next seed is based on internal clock
  fSeed2 = 1 + (Int_t)fRandom->Integer(30081); // SNOMAN limit for seed2

  return;
}
//______________________________________________________________________________
Double_t QLaserballMC::LambdaToEnergy(Float_t aLambda)
{
  // Copied from QOptics.cxx:
  // Convert from aLambda in nm to Energy in MeV
  // Conversion factor is h*c = hbar*c*2*pi = 197.327053 eV*nm * 2*pi = 1239.6 nm
  // and 1e-06 is to get MeV instead of eV.

  Double_t wavelength;
  if(aLambda == 0) wavelength = fWlen;
  else wavelength = fabs(aLambda);

  return (1239.6*1e-06)/wavelength;
}

