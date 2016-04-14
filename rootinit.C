{
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPalette(1);
  
  string RATSYSTEM = getenv("RATSYSTEM");
  string libname = string("libRATEvent_") + RATSYSTEM;
  gSystem->Load(libname.c_str());
  cout << "RAT: Libraries loaded." << endl;
  gSystem->AddIncludePath(" -I$RATROOT/include");
	
  string OCASNOPLUSROOT = getenv("OCA_SNOPLUS_ROOT");
  if ( OCASNOPLUSROOT != "" ){
	gSystem->Load("libOCA_SNOPLUS.so");
	cout << "OCA-SNOPLUS: Libraries loaded." << endl;
	gSystem->Load("libOCA_SNO.so");
	cout << "OCA-SNO: Libraries loaded." << endl;
    gSystem->AddIncludePath(" -I$OCA_SNOPLUS_ROOT/include");
  }

}

