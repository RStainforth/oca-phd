#include "QPMTxyz.h"
#include "QSnoDB.h"
#include "QBank.h"

#define PMTXYZ_MAX 10000
//*-- Author : Mark Boulay                          
//* Copyright (c) 1998 Mark Boulay, All Rights Reserved. 
//____________________________________________________________________________                       
////////////////////////////////////////////////////////////////
//                                                            //
//QPMTxyz: contains the PMT database information from the     //
//titles banks CCCT and GEDP.                                 //
//QPMT uses the global gPMTxyz to retrieve PMT coordinates.   //          
//Written by M. Boulay 06/98                                  //
////////////////////////////////////////////////////////////////

ClassImp(QPMTxyz)
  
QPMTxyz::QPMTxyz( const char *filename )
{

  //Default constructor.  To read constants from SNOMAN titles files, call this constructor
  //with a NULL filename, then call QPMTxyz::ReadConstants().
  //QPMTxyz( "read" ) will cause the file $QSNO_ROOT/parameters/pmt_database_ver2.root to be 
  //read in.
  //Specifiy a different filename to read that file.

 fccct         = NULL;
 fgedp         = NULL;
 fblown75s     = NULL;
 fClient       = new QSnoDB(); 
 fDQXX         = NULL;
 fCHCS         = NULL;
 fPmtCrate=fPmtCard=fPmtChannel=fPmtSnomanNo=fPmtQueensNo=fPanel=NULL;
 fPmtX=fPmtY=fPmtZ=NULL; fPmt75Ohm=NULL; fBatch3Panel=NULL;
 if ( filename ) ReadConstants( filename );

}

void QPMTxyz::Initialize()
{
  //Initialize the local arrays.  This method must be called after the titles banks 
  //have been loaded, and is called by ReadConstants.

  if ( (!fccct) || (!fgedp) || (!fblown75s) ) 
    {
      Error("Initialize","PMT database not present.\n");
      exit(-1);
    }

  if ( !fPmtCrate )    fPmtCrate     = new Int_t   [PMTXYZ_MAX];
  if ( !fPmtCard  )    fPmtCard      = new Int_t   [PMTXYZ_MAX];
  if ( !fPmtChannel )  fPmtChannel   = new Int_t   [PMTXYZ_MAX];
  if ( !fPmtSnomanNo ) fPmtSnomanNo  = new Int_t   [PMTXYZ_MAX];
  if ( !fPmtQueensNo ) fPmtQueensNo  = new Int_t   [PMTXYZ_MAX];
  if ( !fPmtX )        fPmtX         = new Float_t [PMTXYZ_MAX];
  if ( !fPmtY )        fPmtY         = new Float_t [PMTXYZ_MAX];
  if ( !fPmtZ )        fPmtZ         = new Float_t [PMTXYZ_MAX];
  if ( !fPmt75Ohm )    fPmt75Ohm     = new Bool_t  [PMTXYZ_MAX];
  if ( !fPanel )       fPanel        = new Int_t  [PMTXYZ_MAX];
  if ( !fBatch3Panel)  fBatch3Panel  = new Bool_t [800];  
  int i;
  Int_t icrate,icard,ichannel,lpmt, ccc, gedp_lpmt;
  const Int_t rge_pmt_xyz_offset     = 19705;
  const Int_t rge_pmt_panel_id_offset = 10005;  //address of first panel id
  const Int_t ccct_ccc_snoman_offset = 11;

  for (i=0; i < PMTXYZ_MAX; i++){ fPmtSnomanNo[i] = -9999; }
  
  for (icrate = 0; icrate < 19; icrate ++)
    {
      for (icard = 0; icard < 16; icard ++)
	{
	  for (ichannel = 0; ichannel < 32; ichannel++)
	    {
	      ccc  = 1024 * icard + 32*icrate + ichannel; 
	      lpmt = 512  * icrate +32*icard  + ichannel;
	      
	      fPmtSnomanNo[lpmt] = fccct->icons( ccc + ccct_ccc_snoman_offset );
	      fPmtCrate[lpmt]    = icrate;
	      fPmtCard[lpmt]     = icard;
	      fPmtChannel[lpmt]  = ichannel;
	    }
	}
    }

  for (i = 0; i < PMTXYZ_MAX; i++)
    { 
      gedp_lpmt = -1;
      if ( fPmtSnomanNo[i] > 0 ) gedp_lpmt = (fPmtSnomanNo[i] - 1)*3 + rge_pmt_xyz_offset;
      
      if ( gedp_lpmt > 0 && fPmtSnomanNo[i] > 0  )
	{
	  fPmtX[i] = fgedp->rcons(gedp_lpmt );
	  fPmtY[i] = fgedp->rcons(gedp_lpmt+1);
	  fPmtZ[i] = fgedp->rcons(gedp_lpmt+2);
	  fPanel[i]= fgedp->icons(fPmtSnomanNo[i]+rge_pmt_panel_id_offset);
	}
      else
	{
	  fPanel[i]=-9999;
	  fPmtX[i]=fPmtY[i]=fPmtZ[i]=-9999;  // invalid pmt number
	}
    }

  //Fill the blown 75Ohm list
  for (i=0; i < PMTXYZ_MAX; i++){ fPmt75Ohm[i] = kFALSE; }

  //for (i=0; i < fblown75s->icons(1); i++)
  //  {
  //    fPmt75Ohm[fblown75s->icons(i+2)] = kTRUE;
  //  }
}

QPMTxyz::~QPMTxyz(){
  //Destructor.
  delete [] fPmtCrate;
  delete [] fPmtCard;
  delete [] fPmtChannel;
  delete [] fPmtSnomanNo;
  delete [] fPmtQueensNo;
  delete [] fPmt75Ohm;
  delete [] fBatch3Panel;
  delete [] fPanel;
  
  delete fClient;
  delete fccct;
  delete fgedp;
  delete fblown75s;
  delete fDQXX;     fDQXX = NULL;
  delete fCHCS;     fCHCS = NULL;

  // avoid accessing deleted objects - PH 01/14/99
  if (gPMTxyz == this) gPMTxyz = NULL;
 
}

Int_t QPMTxyz::ReadConstants( const char * filename, const int idate, const int itime )
{
  //Read in PMT database file and initialize local arrays.  If filename = "read", read the file 
  //$QSNO_ROOT/parameters/pmt_database_ver2.root.  If filename = NULL, read in SNOMAN titles files, 
  //else read the named file.  By default, uses 20040923 bank; if another bank
  //is desired, change idate and itime (Note: pmt positions hardwired to 20040923_ver2).

  if ( !filename )
    {
        
        printf("WARNING:: Looking for files directly through GetSnomanTitles. This is (very) likely to fail.\n");
      //Read in SNOMAN titles files

      char ccct_fname[1024], gedp_fname[1024];
      Int_t irc1 = gSNO->GetSnomanTitles("map_ccc_tube.dat",ccct_fname);
      Int_t irc2 = gSNO->GetSnomanTitles("pmt_positions_20040923_ver2.dat",gedp_fname);

      if ( !irc1 || !irc2 )
	{
	  Error("ReadConstants","Can't find input files for CCCT and GEDP banks");
	  exit(-1);
	}

      QTitles t_ccct(ccct_fname,"CCCT",1,idate, itime);
      QTitles t_gedp(gedp_fname,"GEDP",422);

      // fccct = fClient->GetBank("CCCT",1,19990101,0,11);
      // fgedp = fClient->GetBank("GEDP",422,19990101,0,11);
      fccct = t_ccct.GetBank();
      fgedp = t_gedp.GetBank();
    }
  else
    {
      //Open pmt_database_ver2.root and retrieve banks.

      char *pmtf = getenv("OCA_SNO_ROOT");
      char pmtfile[100];
      if ( !strcmp(filename, "read") ) sprintf(pmtfile,"%s/data/pmt_database_ver2.root",pmtf);
      else sprintf(pmtfile,"%s/data/%s",pmtf,filename);
 
      TFile pmt_data(pmtfile,"READ");
      
      if (!pmt_data.IsOpen() )
	{
	  printf("Error in QPMTxyz: cannont open file %s\n",pmtfile);
	  exit(-1);
	}
      fccct = (QBank *)pmt_data.Get("CCCT");
      fgedp = (QBank *)pmt_data.Get("GEDP");
      //      pmt_data->Close();
      //      delete pmt_data;
    }


  // Open batch 3 concentrator file and fill fBatch3Concentrator
  // It is conjectured that this batch of concentrators might be poorer quality

  char *pmtf = getenv("OCA_SNO_ROOT");
  char panelfile[100];
  sprintf(panelfile,"%s/data/%s",pmtf,"batch3concentrator.dat");
  FILE *fdata = fopen(panelfile,"r");
  if ( !fdata )
      {
        printf("Error in QPMTxyz: cannot open file %s\n",panelfile);
        exit(-1);
      }
  Int_t ipanel;
  if ( !fBatch3Panel)  fBatch3Panel  = new Bool_t [800];  
  for(ipanel=0;ipanel<800;ipanel++)fBatch3Panel[ipanel]=false;
  for(;fscanf(fdata," %d",&ipanel)==1;){
    if(fBatch3Panel[ipanel])printf("duplicate Panel numbers in batch3concentrator.dat %d \n",ipanel);
    fBatch3Panel[ipanel]=true;
  }
  fclose(fdata);


  //Open 75Ohm database (this is nearly obsolete --26/4/2000).
  
  char pmtfile[100];
  sprintf(pmtfile,"%s/data/%s",pmtf,"pmt_75ohm_data.dat");
  fdata = fopen(pmtfile,"r");
  Int_t i, npoints;
  if ( !fdata )
      {
        printf("Error in QPMTxyz: cannot open file %s\n",pmtfile);
        exit(-1);
      }
  fblown75s = new QBank();
  fscanf(fdata, "%i",&npoints);
  Int_t crate, card,channel;
  fblown75s->GetData()->Set( npoints + 1 );
  fblown75s->GetData()->AddAt(npoints,0);
  
  for (i=0; i < npoints; i++)
  {
    fscanf(fdata,"%i %i %i",&crate,&card,&channel);
    fblown75s->GetData()->AddAt(GetPmtn(crate,card,channel), i+1);
  }
  
  fblown75s->SetBankName("fblown75s");
  
  fclose( fdata );
  Initialize();
  return 0; 
}

Int_t QPMTxyz::WriteConstants( const char *filename )
{
  //Output the PMT database to root file.

  TFile *pmt_data = new TFile(filename,"RECREATE");
  if ( !pmt_data )
    {
      Error("WriteConstants","cannot open file %s for writing\n",filename);
      return (-1);
    }
  pmt_data->cd();
  if ( fccct ) fccct->Write("CCCT");
  if ( fgedp ) fgedp->Write("GEDP");
  
  pmt_data->Close();
  delete pmt_data;
  return 0;
}

// The following are the Get methods to retrieve information for a single PMT.
// The PMT numbering follows a crate, card, channel organization, according to
// lpmt = 512*icrate + 32*icard  + ichannel; (19 crates, 16 cards, 32 channels).

Float_t QPMTxyz::GetX( int lpmt ) { if ( lpmt < KMAX_PMT_LCN && lpmt >= 0) return fPmtX[lpmt]; else return -9999;}
Float_t QPMTxyz::GetY( int lpmt ) { if ( lpmt < KMAX_PMT_LCN && lpmt >= 0) return fPmtY[lpmt]; else return -9999;}
Float_t QPMTxyz::GetZ( int lpmt ) { if ( lpmt < KMAX_PMT_LCN && lpmt >= 0) return fPmtZ[lpmt]; else return -9999;}
TVector3 QPMTxyz::GetXYZ( int lpmt ) { 
  if ( lpmt < KMAX_PMT_LCN && lpmt >= 0) {
    return TVector3(fPmtX[lpmt],fPmtY[lpmt],fPmtZ[lpmt]); 
 }else return TVector3(-9999,-9999,-9999);
}

Int_t QPMTxyz::GetCrate( Int_t lpmt )
{
   //Return the Crate associated with this PMT. 
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return -9999;
  return fPmtCrate[lpmt];
}
Int_t QPMTxyz::GetCard( Int_t lpmt )
{
 //Return the Card associated with this PMT.
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return -9999;
  return fPmtCard[lpmt];
}
Int_t QPMTxyz::GetChannel( Int_t lpmt )
{
  //Return the Channel associated with this PMT.
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return -9999;
  return fPmtChannel[lpmt];
}

Int_t QPMTxyz::GetSnomanNo( Int_t lpmt )
{
  //Return the Snoman number of this PMT.
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return -9999;
  return fPmtSnomanNo[lpmt];
}
Int_t QPMTxyz::GetQueensNo( Int_t lpmt )
{
  //Return the Queens number of this PMT.
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return -9999;
  return fPmtQueensNo[lpmt];
}
Int_t QPMTxyz::GetCCC( Int_t lpmt )
{
  //Return the CCC (DAQ circuit) number for this PMT.
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return -9999;
  return 1024*GetCard(lpmt)+32*GetCrate(lpmt)+ GetChannel(lpmt);
}
Bool_t QPMTxyz::Is75Blown( Int_t lpmt )
{
  //Determine wheter this pmt has a blown 75 Ohm back terminator.
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return kFALSE;
  return fPmt75Ohm[lpmt];
}

Int_t QPMTxyz::GetPanel(Int_t lpmt){
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return kFALSE;
  return fPanel[lpmt];
}

Bool_t QPMTxyz::IsBatch3Concentrator(Int_t lpmt){
  if ( lpmt < KMIN_PMT_LCN || lpmt >= KMAX_PMT_LCN ) return kFALSE;
  return fBatch3Panel[GetPanel(lpmt)];
}


Int_t QPMTxyz::GetType( Int_t lpmt )
{
  //Return this PMT's type (Normal PMT, LG pmt, etc. as defined in the enum ccc_type.
  Int_t ccc_type = -1;
  Int_t crate   = GetCrate( lpmt );
  Int_t card    = GetCard(  lpmt );
  Int_t tubeno;
  if ( crate > 18 || card > 15 )            ccc_type = KUTL_CCC_INVALID;
  else if ( crate == 17 && card == 15 )     ccc_type = KUTL_CCC_CALIBRATION;
  else
    {
      tubeno = GetSnomanNo( lpmt );
      if ( tubeno < 0 )                     ccc_type = KUTL_CCC_SPARE;
      else
	{
	                                    ccc_type = KUTL_CCC_PMT_NORMAL;
	  if ( tubeno >= KGE_PMT_MN_OWL )   ccc_type = KUTL_CCC_PMT_OWL; 
	  if ( tubeno >= KGE_PMT_MN_LG  )   ccc_type = KUTL_CCC_PMT_LG;
	  if ( tubeno >= KGE_PMT_MN_BUTT)   ccc_type = KUTL_CCC_PMT_BUTT;
	  if ( tubeno >= KGE_PMT_MN_NECK)   ccc_type = KUTL_CCC_PMT_NECK;
	}
    }
  return ccc_type;
}

Bool_t QPMTxyz::IsNormalPMT( Int_t lpmt )
{
  //Return true if this is a normal (inward facing) PMT.
  if ( GetType( lpmt ) == KUTL_CCC_PMT_NORMAL ) return kTRUE;
  else return kFALSE;
}

Bool_t QPMTxyz::IsOWLPMT( Int_t lpmt )
{
  //Return true if this is an OWL PMT.
  if ( GetType( lpmt ) == KUTL_CCC_PMT_OWL ) return kTRUE;
  else return kFALSE;
}

Bool_t QPMTxyz::IsLGPMT( Int_t lpmt )
{
  //Return true if this is a low gain PMT.
  if ( GetType( lpmt ) == KUTL_CCC_PMT_LG ) return kTRUE;
  else return kFALSE;
}

Bool_t QPMTxyz::IsSparePMT( Int_t lpmt )
{
  //Return true if this is a spare PMT.
  if ( GetType( lpmt ) == KUTL_CCC_SPARE ) return kTRUE;
  else return kFALSE;
}

Bool_t QPMTxyz::IsInvalidPMT( Int_t lpmt )
{
  //Return true if this is an invalid  PMT number.
  if ( GetType( lpmt ) == KUTL_CCC_INVALID || lpmt < 0 || lpmt >= KMAX_PMT_LCN ) return kTRUE;
  else return kFALSE;
}

Bool_t QPMTxyz::IsCalibrationPMT( Int_t lpmt )
{
  //Return true if this is a calibration channel.
  if ( GetType( lpmt ) == KUTL_CCC_CALIBRATION ) return kTRUE;
  else return kFALSE;
}

Int_t QPMTxyz::ReadDQXX( const char * filename )
{
  //Read DQXX banks from given titles file.
  if ( fDQXX ) { delete fDQXX; fDQXX = NULL; }
  fDQXX = new QDQXX( filename );
  return 0;
}

Bool_t QPMTxyz::IsChannelOn( Int_t lcn )
{
  //Is this channel on-line as define in the DQXX banks?
  if ( !fDQXX ) return kFALSE;
  if ( fDQXX->LcnInfo(lcn, KCCC_CHAN_ON_LINE) == 1 ) return kTRUE;
  else return kFALSE;
}

Bool_t QPMTxyz::IsTubeOn( Int_t lcn )
{
  //Is this tube on-line as define in the DQXX banks?
  //This routine will only return true if the channel is on, the HV resistor is OK, 
  //the threshold is not at max, etc.

  if ( !fDQXX ) return kFALSE;
  if ( fDQXX->LcnInfo(lcn, KCCC_TUBE_ON_LINE) == 1 ) return kTRUE;
  else return kFALSE;
 
}

Int_t QPMTxyz::GetThreshold( Int_t lcn )
{
  //Return this channel's thresold as defined in the DQXX banks.
  //The returned value is the threshold in DAC counts above zero.

  if ( !fDQXX ) return -1;
  return fDQXX->LcnInfo(lcn, KCCC_THRESH) - fDQXX->LcnInfo(lcn,KCCC_THRESH_ZERO);
}

Int_t QPMTxyz::GetHV( Int_t lcn )
{
  //Return this channel's HV as defined in the DQXX banks.
  if ( !fDQXX ) return -1;
  return fDQXX->LcnInfo( lcn, KCCC_HV );
}

Int_t QPMTxyz::ReadCHCS(const char *filename, Int_t date, Int_t time)
{
  //Read CHCS banks from given titles file, for the calendar date and time.
  //QCHCS currently uses "RSPS" banks.

  if ( fCHCS ) { delete fCHCS; fCHCS = NULL; }
  fCHCS = new QCHCS(filename,date,time);
  return 0;
}

Int_t QPMTxyz::ReadCHCS(const char *filename, Int_t jd, Int_t ut1, Int_t ut2)
{
  //Read CHCS banks from given titles file by julian day.
  //QCHCS currently uses "RSPS" banks.

  if ( fCHCS ) { delete fCHCS; fCHCS = NULL; }
  fCHCS = new QCHCS(filename,jd,ut1,ut2);
  return 0;
}

Int_t QPMTxyz::IsTubeCalStatGood( Int_t lcn )
{
  // Returns kFALSE if the tube is vetoed by the CHCS banks, kTRUE otherwise.
  // If no CHCS banks are loaded, all pmts are accepted (!).

  if ( !fCHCS ) return kTRUE;
  if ( fCHCS->IsBadPmt(lcn) ) return kFALSE;
  return kTRUE;
}

void QPMTxyz::Remap(const char *filename){
  // This routine reads in a map, which is a file in which the first entry in
  //line is a PMT number (LCN) which refers to the crate/card/channel.  The
  //second entry is the tube whose current location is the desired psup 
  // location for the channel.  So for instance
  //865 870 sets XYZ[865]=XYZ[870].  This is used to temporarily remap 
  // the psup when considering effects of map changes.  

  FILE * inp=fopen(filename,"r");
  if(inp==NULL){
    printf("%s does not exist in QPMTxyz::Remap()\n", filename);
    return;
  }
  int i;
  int elect[10000],psup[10000];
  for(i=0;fscanf(inp," %d %d",elect+i,psup+i)==2;i++);
  int npmts=i;
  printf("Remap is moving %d pmts\n",npmts);
  float *xlist = new float[npmts];
  float *ylist = new float[npmts];
  float *zlist = new float[npmts];
  for(i=0;i<npmts;i++){
    xlist[i]=fPmtX[psup[i]];
    ylist[i]=fPmtY[psup[i]];
    zlist[i]=fPmtZ[psup[i]];
  }
  for(i=0;i<npmts;i++){
    fPmtX[elect[i]]=xlist[i];
    fPmtY[elect[i]]=ylist[i];
    fPmtZ[elect[i]]=zlist[i];
  }
  delete [] xlist;
  delete [] ylist;
  delete [] zlist;
}
