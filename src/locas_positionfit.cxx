#include <stdio.h>
#include <string.h>

#include "TString.h"
#include "TFile.h"
#include "TROOT.h"
#include "TObject.h"
#include "TVector.h"

#include "QOCAFit.h"
#include "QPath.h"
#include "QRdt.h"
//----------------------------------------------------------------
//	qoca_positionfit.cxx
//	
//*--Author: Jose' Maneira  14/01/03
//
//	This code does the source position fit to a single laserball
//	run, using the data from the rdt files. The rch file is needed
//	only to get the wavelength.
//	The code writes the source position fit as a TVector3 back into
//	the rdt file, so that the quality control can then compare it
//	to the manipulator position.
//     

TROOT theROOT("theROOT","Run QPath");
int main( Int_t argc, char *argv[])
{
#define M_PI   3.14159265358979323846
	
	// with the new optional argument we need to expect that the user will pass either 4 or 5 arguments
  if ((argc != 5) && (argc !=6 )){
    printf("Usage: qoca_fitposition [run number] [pass code]");
    printf(" [rdt dir] [dqxx dir] [[wlen]]\n");
    printf("Pass number usage is overridden with \"nopass\".\n");
    printf("To fit MC files use \"mc\".\nTo look for the last pass available use \"lastpass\"\n");
	printf("[wlen] is optional and allows to set the default wavelength in case it is missing in the rdt file.\n");
    exit(0);
  }
  
  Int_t run[1] = {atoi(argv[1])};
  TString *pass[1]; pass[0] = new TString(argv[2]);
  TString rdt_dir(argv[3]);
  TString dqxx_dir(argv[4]);
  
  Int_t cruns[1] = {0};
  Int_t fruns[1] = {0};
  Char_t rdtfile[128];
  printf("pass %s\n",pass[0]->Data());
  
  QPath *qp = new QPath();
  if (argc == 6) {
    Float_t wlen = atof(argv[5]);
    printf("qoca_positionfit: Assuming default lambda as %.0f\n",wlen);
    qp->SetDefaultLambda(wlen);
  }
  qp->SetFitLBPosition(3);  // QOptics
  qp->SetRunList(1,run,cruns);
  
  QRdt *qrdt = new QRdt();
  qrdt->SetRdtdir(rdt_dir.Data());

	// now we need to consider the new options
	// and set the parameters in Qrdt accordingly
  if (!strcmp(pass[0]->Data(),"lastpass")){
    qrdt->SetRdtstyle(1);
    qrdt->SetRdtpass(-1);
    qrdt->LoadRdtFile(run[0]);
    sprintf(rdtfile,"%s",qrdt->GetRdtfullname().Data());
  } else {
    if (!strcmp(pass[0]->Data(),"mc")){
      sprintf(rdtfile,"%s/snomc_%d.rdt",rdt_dir.Data(),run[0]);
    } else if (!strcmp(pass[0]->Data(),"nopass")) {		
      sprintf(rdtfile,"%s/sno_%d.rdt",rdt_dir.Data(),run[0]);
    } else {
      sprintf(rdtfile,"%s/sno_0%d_%s.rdt",rdt_dir.Data(),run[0],pass[0]->Data());
    }
    qrdt->LoadRdtFile(rdtfile);
  }
	//	QRdt *qrdt = new QRdt(rdtfile);
  qrdt->SetRdtRunnumber(run[0]);
  qp->SetDqxxdir((Char_t *) dqxx_dir.Data());
  
  QOCATree *rawtree = qp->FillFromRDT(qrdt,"optics");
  QOCATree *postree = qp->CalculatePositions(rawtree,"poptics");
	
  
  
  TFile fout(rdtfile,"UPDATE");
  TVector3 directfit_pos,qpathfit_pos;
  QOCARun *ocarun = new QOCARun();
  postree->SetBranchAddress("Runs",&ocarun);
  postree->GetEntry(0);
  if (ocarun->GetRun() == run[0]){
    directfit_pos 	= ocarun->GetFitxyz();
    qpathfit_pos	= ocarun->GetQFitxyz();
    directfit_pos.Write("directfit_pos");
    qpathfit_pos.Write("qpathfit_pos");
  }	
  fout.Close();
}

