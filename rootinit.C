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
	
  char *OCAROOT = getenv("OCA_ROOT");
  if (OCAROOT != NULL) {
	gSystem->Load("libOCA.so");
	cout << "OCA: Libraries loaded." << endl;
	gSystem->AddIncludePath(" -I$OCA_ROOT/lib");
  }

  char *QOCAROOT = getenv("QOCA_ROOT");
  if (QOCAROOT != NULL) {
	gSystem->Load("libQOCA.so");
	cout << "QOCA: Libraries loaded." << endl;
	gSystem->AddIncludePath(" -I$QOCA_ROOT/include");
  }
}

