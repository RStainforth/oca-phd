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
	
  char *LOCASROOT = getenv("LOCAS_ROOT");
  if (LOCASROOT != NULL) {
	gSystem->Load("libLOCAS.so");
	cout << "LOCAS: Libraries loaded." << endl;
	gSystem->AddIncludePath(" -I$LOCAS_ROOT/include");
  }
}

