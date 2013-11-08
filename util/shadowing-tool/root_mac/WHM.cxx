
// Function which loads the ROOT file
void
LoadRootFile( const char* lpFile,
              TTree **tree,
              RAT::DS::Root **rDS,
              RAT::DS::Run **rRun);

// Function which reads in all the root files and collects the number of MCPhotons
// collect at each one.
void WHM_${JobID}_${ConfigName}(){

  TMatrix pmtInfoTMP(10001,6);
  TMatrix* pmtInfo = &pmtInfoTMP;
  char buffer[32];

  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree *tree;

  for ( int j = 0; j < ${NumberOfJobs}; j++ ){

    snprintf( buffer, sizeof(char) * 32, "${JobID}_${ConfigName}_%i.root", j);
    cout << "Now analysing " << buffer << endl;

    LoadRootFile( buffer, &tree, &rDS, &rRun );
    
    // Collect event entries
    for( int iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){
      tree->GetEntry( iEvent );
      
      // Get the MC-PMT properties
      RAT::DS::MC* rMC = rDS->GetMC();
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();
      
      // Get the list of his MC-PMTs in the event
      for (int iPMT = 0; iPMT < rMC->GetMCPMTCount(); iPMT++){
                
        RAT::DS::MCPMT* rMCPMT = rMC->GetMCPMT( iPMT );      
        int pmtID = rMCPMT->GetPMTID();
        TVector3 pmtPos = rPMTProp->GetPos( pmtID );
	if ( pmtInfo(pmtID,0) == 0 ){
	  (*pmtInfo)(pmtID,0) = 1;
	  (*pmtInfo)(pmtID,1) = pmtPos.Mag();
	  (*pmtInfo)(pmtID,2) = pmtPos.CosTheta();
	  (*pmtInfo)(pmtID,3) = pmtPos.Phi();
	}
        
        for ( int iPhoton = 0; iPhoton < rMCPMT->GetMCPhotonCount(); iPhoton++ ){
	  double numPhotons = rMCPMT->GetMCPhotonCount();
	  (*pmtInfo)(pmtID,4) += numPhotons;

	  RAT::DS::MCPhoton* rMCPhoton = rMCPMT->GetMCPhoton( iPhoton );
	  double hitTime = rMCPhoton->GetHitTime();
	  (*pmtInfo)(pmtID,5) += hitTime;

        }
      }
    }
  }


  TFile *newFile = new TFile("${JobID}_${ConfigName}_TMP.root", "RECREATE");
  pmtInfo->Write("${JobID}_${ConfigName}_Matrix");
  newFile->Close();

}







// Load Root File Definition
void
LoadRootFile( const char* lpFile,
              TTree **tree,
              RAT::DS::Root **rDS,
              RAT::DS::Run **rRun)
{
  TFile *file = new TFile( lpFile );
  (*tree) = (TTree*)file->Get( "T" );
  TTree *runTree = (TTree*)file->Get( "runT" );

  *rDS = new RAT::DS::Root();

  (*tree)->SetBranchAddress( "ds", &(*rDS) );

  *rRun = new RAT::DS::Run();

  runTree->SetBranchAddress( "run", &(*rRun) );
  runTree->GetEntry();
}
