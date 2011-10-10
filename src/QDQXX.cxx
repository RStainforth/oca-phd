//////////////////////////////////////////////////////////////////////////////
//QDQXX                                                                     //
//                                                                          //
//Utility to read and decode DQXX channel status banks.                     //
//                                                                          //
//Written by M. Boulay Oct 27/99. 
// 
// This class reads and decodes/recodes the DQXX banks. It is used by QPMTxyz, 
// which has a set of methods like IsTubeOn() that are often more convenient.
//
//  QDQXX remaps the DQCH and DQCR words, from the banks and
//  return the channel status for the given DQCH words.  The channel status 
//   has reflects a remapping of the bits in DQCH and DQCR, with bits defined
//   by the following enumeration (in QGlobals.h).
//   Note: The status is negative logic- bits that fail are cleared.
//
// Ignored bits are marked by **** in the list below, and 
//            do not enter in Tube on Line or Channel on Line
// If all not-ignored  bits 0-23 are set, the tube is on-line
// If all not-ignored bits are set, the channel is on-line
//
//enum ccc_info_status {
//    KCCC_B_CRATE           =  0,  dqcr B_Crate (bit 0)
//    KCCC_B_SLOT_OP         =  1,  dqcr B_SLOT_OP (bit 1) ****
//    KCCC_B_GT              =  2,  dqcr B_GT
//    KCCC_B_CR_ONLINE       =  3,  dqcr B_CR_ONLINE
//    KCCC_B_CR_HV           =  4,  dqcr B_HV
//    KCCC_B_MB              =  8,  dqcr B_MB
//    KCCC_B_DC              =  9,  dqcr B_DC + ith daughter card
//    KCCC_B_DAQ             = 10,  dqcr B_DAQ
//    KCCC_B_SEQUENCER       = 16,  dqch B_SEQUENCER
//    KCCC_B_100NS           = 17,  dqch B_100 NS ****
//    KCCC_B_20NS            = 18,  dqch B_20 NS ****
//    KCCC_B_VTHR_MAX        = 19,  dqch reset if threshold ==255
//    KCCC_B_QINJ            = 20,  dqch B_QINJ ****
//    KCCC_B_N100            = 21,  dqch B_N100 ****
//    KCCC_B_N20             = 22,  dqch B_N20 ****
//    KCCC_B_NOT_OP          = 23,  dqch B_NOT_OP
//    KCCC_B_PMTIC           = 24,  dqcr B_PMTIC
//    KCCC_B_RELAY           = 26,  dqcr B_RELAY + DAUGHTER CARD
//    KCCC_B_PMTIC_RESISTOR  = 27,  dqch B_RESISTOR
//    KCCC_B_PMT_CABLE       = 28,  dqch B_CABLE
//    KCCC_B_75OHM           = 29   dqch B_75OHM ****
//  };
//////////////////////////////////////////////////////////////////////////////

//*--Author : Mark Boulay 27/10/99

#include "QDQXX.h"
#include "QSNO.h"
#include "QPMTxyz.h"
#include "QPMT.h"

ClassImp(QDQXX)

QDQXX::QDQXX(const char *titles)
{
  //Default constructor.
  Initialize();
  if ( titles ) ReadTitles( titles );
}

QDQXX::~QDQXX()
{
  //Destructor.
  Clear();
  delete [] fDQCH;
  delete [] fDQCR;
}

void QDQXX::Initialize()
{
  //Initialize banks.
  Int_t i;
  fDQCH = new QBank * [19];
  fDQCR = new QBank * [19];
  for ( i = 0; i < 19; i++)
    {
      fDQCH[i] = NULL;
      fDQCR[i] = NULL;
    }
}

void QDQXX::Clear(const Option_t *)
{
  //Clear the DQXX banks.
  Int_t i;
  for ( i = 0; i < 19; i++)
    {
      if ( fDQCH[i] ) { delete fDQCH[i]; fDQCH[i] = NULL; }
      if ( fDQCR[i] ) { delete fDQCR[i]; fDQCR[i] = NULL; }
    }
}

void QDQXX::SetDQCH( Int_t i, QBank *bank )
{
  //Set DQCH bank i.
  if ( fDQCH[i] )     delete fDQCH[i]; fDQCH[i] = 0;
  if ( ! bank   )     return;
  fDQCH[i]  = new QBank( *bank );
}

void QDQXX::SetDQCR( Int_t i, QBank *bank )
{
  //Set DQCR bank i.
  if ( fDQCR[i] )     delete fDQCR[i]; fDQCR[i] = 0;
  if ( ! bank   )     return;
  fDQCR[i]  = new QBank( *bank );
}

Int_t QDQXX::ReadTitles( const char *titles )
{
  //Read all DQCH and DQCR banks from titles file.
  Clear();
  Int_t i;
  QBank *mBank = NULL;
  Int_t nCH = 0, nCR = 0;
  for ( i=0; i < 19; i++)
    {
      QTitles mCHTitles((char *)titles,(char *)"DQCH",i);
      mBank = mCHTitles.GetBank();
      if ( mBank ) nCH++;
      SetDQCH( i, mBank);
      delete mBank;

      QTitles mCRTitles((char *)titles,(char *)"DQCR",i);
      mBank = mCRTitles.GetBank();
      if ( mBank ) nCR++;
      SetDQCR( i, mBank);
      delete mBank;
    }
  printf("QDQXX: Retrieved %i of 19 DQCH banks.  Retrieved %i of 19 DQCR banks.\n",nCH, nCR);
  
  //Next piece of code produces usefull statistics on the the PMTs on-line
  
  Int_t nPMTsC = 0, nOWLsC = 0, nFECDsC = 0, nLGsC = 0, nSparesC = 0, nTotalC = 0;
  Int_t nPMTs = 0, nOWLs = 0, nFECDs = 0, nLGs = 0, nSpares = 0, nTotal = 0;
  QPMT pmt;
  for (i=KMIN_PMT_LCN; i < KMAX_PMT_LCN; i++)
    {
      pmt.Setn(i);
      if ( LcnInfo( i , KCCC_TUBE_ON_LINE ) == 1 )
	{
	  nTotal++;
	  if ( pmt.IsNormalPMT() ) nPMTs++;
	  if ( pmt.IsOWLPMT() ) nOWLs++;
	  if ( pmt.IsCalibrationPMT() ) nFECDs++;
	  if ( pmt.IsLGPMT() ) nLGs++;
	  if ( pmt.IsSparePMT() ) nSpares++;
	}
      if ( LcnInfo( i , KCCC_CHAN_ON_LINE ) == 1 )
	{
	  nTotalC++;
	  if ( pmt.IsNormalPMT() ) nPMTsC++;
	  if ( pmt.IsOWLPMT() ) nOWLsC++;
	  if ( pmt.IsCalibrationPMT() ) nFECDsC++;
	  if ( pmt.IsLGPMT() ) nLGsC++;
	  if ( pmt.IsSparePMT() ) nSparesC++;
	}
    }
   printf("QDQXX: Channel summary :  Total  PMTs  OWLs  FECDs  LGs  Spares\n");
   printf("       Channels online :  %5i  %4i %4i  %5i  %3i  %6i\n",nTotalC, nPMTsC,nOWLsC,nFECDsC,nLGsC,nSparesC);
   printf("       Tubes online    :  %5i  %4i %4i  %5i  %3i  %6i\n",nTotal, nPMTs,nOWLs,nFECDs,nLGs,nSpares);
  return 0;
}

Int_t QDQXX::GetNonline()
{

  //Return the number of inward looking tubes online.
  Int_t nPMTsC = 0, nOWLsC = 0, nFECDsC = 0, nLGsC = 0, nSparesC = 0, nTotalC = 0;
  Int_t nPMTs = 0, nOWLs = 0, nFECDs = 0, nLGs = 0, nSpares = 0, nTotal = 0, i;
  QPMT pmt;
  for (i=KMIN_PMT_LCN; i < KMAX_PMT_LCN; i++)
    {
      pmt.Setn(i);
      if ( LcnInfo( i , KCCC_TUBE_ON_LINE ) == 1 )
	{
	  nTotal++;
	  if ( pmt.IsNormalPMT() ) nPMTs++;
	  if ( pmt.IsOWLPMT() ) nOWLs++;
	  if ( pmt.IsCalibrationPMT() ) nFECDs++;
	  if ( pmt.IsLGPMT() ) nLGs++;
	  if ( pmt.IsSparePMT() ) nSpares++;
	}
      if ( LcnInfo( i , KCCC_CHAN_ON_LINE ) == 1 )
	{
	  nTotalC++;
	  if ( pmt.IsNormalPMT() ) nPMTsC++;
	  if ( pmt.IsOWLPMT() ) nOWLsC++;
	  if ( pmt.IsCalibrationPMT() ) nFECDsC++;
	  if ( pmt.IsLGPMT() ) nLGsC++;
	  if ( pmt.IsSparePMT() ) nSparesC++;
	}
    }
   
  return nPMTs;
}

Int_t QDQXX::GetNFECs()
{
  //return the number of front-end cards enabled.
  
  Int_t nfecs = 0;
  Int_t i_crate, i_card;
  for (i_crate = 0; i_crate < 19; i_crate++)
    {
      for ( i_card = 0; i_card < 16; i_card++)
	{
	  if ( fDQCR[i_crate] )
	    {
	      Int_t mOffsetDQCR = 11 + i_card;
	      Int_t mWordDQCR = fDQCR[i_crate]->icons(mOffsetDQCR);
	      //       Update status bits from DQCR.
	      if ( ! ((mWordDQCR>>KDQCR_B_SLOT_OP )&1) ){ nfecs++; }  
	    }
	}
    }

  return nfecs;
}


Int_t QDQXX::GetDQXXByLcn(Int_t lcn, Int_t &dqch, Int_t &dqcr)
{
  //return the DQCH and DQCR words for this logical circuit number.
  Int_t i_crate, i_card, i_chan;
  QPMT pmt;
  pmt.Setn(lcn);
  i_crate = gSNO->GetPMTxyz()->GetCrate(lcn); 
  i_card  = gSNO->GetPMTxyz()->GetCard(lcn);
  i_chan  = gSNO->GetPMTxyz()->GetChannel(lcn);

  Int_t mOffsetDQCH = 11 + 32*i_card + i_chan;
  Int_t mOffsetDQCR = 11 + i_card;
  Int_t status = 0;
  if ( !fDQCH[i_crate] || !fDQCR[i_crate] )
    {
      Warning("GetDQCHByLcn","DQXX bank missing");
      status = -1;
      dqch = -9999;
      dqcr = -9999;
      return status;
    }

  //Retrieve the appropriate words and return the channel status.
  dqch = fDQCH[i_crate]->icons(mOffsetDQCH);
  dqcr = fDQCR[i_crate]->icons(mOffsetDQCR);

  return status;

}

Int_t QDQXX::LcnInfo( Int_t lcn, Int_t i_mode )
{
  //Return the requested information from the DQXX banks for the given channel.
  //Requests are made by setting i_mode to one of:
  //
  //<Begin_html>
  //<l>KCCC_THRESH       Discriminator threshold.</l>
  //<l>KCCC_THRESH_ZERO  Threshold zero.</l>
  //<l>KCCC_STATUS       Status.  See SNOMAN Specification.</l>
  //<l>KCCC_HV           High voltage.</l>
  //<l>KCCC_TUBE_ON_LINE = 0 no, = 1 yes  See SNOMAN Specification.</l>
  //<l>KCCC_CHAN_ON_LINE = 0 no, = 1 yes  See SNOMAN Specification.</l>
  //<End_html>

  if ( lcn < KMIN_PMT_LCN || lcn >= KMAX_PMT_LCN )
    {
      Warning("LcnInfo","PMT number out of range.");
      return KCCC_UNDEFINED;
    }
  
  Int_t ccc_info = KCCC_UNDEFINED;

  //Retrieve the appropriate words and return the channel status.
  Int_t mWordDQCH; 
  Int_t mWordDQCR;
  if(GetDQXXByLcn(lcn,mWordDQCH,mWordDQCR) != 0)return ccc_info; //error return
  ccc_info = LcnInfo( mWordDQCH, mWordDQCR, i_mode, lcn );
  return ccc_info;

}

Int_t QDQXX::LcnInfo( Int_t i_dqch, Int_t i_dqcr, Int_t imode, Int_t lcn )
{
  // This routine does the bit manipulations on dqch and dqcr to return 
  //thresholds, hv, or status.
 
  Int_t ccc_info = KCCC_UNDEFINED;
  Int_t i_chan   = gSNO->GetPMTxyz()->GetChannel( lcn );
  Int_t i_dc     = i_chan>>3;

  Int_t MASK8    = (Int_t)pow(2.0,8)  -1;
  Int_t MASK12   = (Int_t)pow(2.0,12) -1;
  Int_t MASK24   = (Int_t)pow(2.0,24) -1;

  if      ( imode == KCCC_THRESH ) 
    { ccc_info = (i_dqch>>KDQCH_B_VTHR)&MASK8; }
  else if ( imode == KCCC_THRESH_ZERO ) 
    { ccc_info = (i_dqch>>KDQCH_B_VTHR_ZERO)&MASK8; }
  else if ( imode == KCCC_HV ) 
    {ccc_info = (i_dqcr>>KDQCR_B_HV)&MASK12; }
  else if ( imode ==  KCCC_STATUS || imode == KCCC_TUBE_ON_LINE || imode == KCCC_CHAN_ON_LINE ) 
    {

//       Update status bits from DQCR.
        ccc_info = -1;
 
 if ( (i_dqcr>>KDQCR_B_CRATE)&1)  ccc_info = (~(1<<KCCC_B_CRATE))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_GT )&1)  ccc_info = (~(1<<KCCC_B_GT))&ccc_info;
 
 //the following ! sign is a temporary bug fix.  (This bit is always false).
 if ( !( i_dqcr>>KDQCR_B_CR_ONLINE )&1)  ccc_info = (~(1<<KCCC_B_CR_ONLINE))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_CR_HV )&1)  ccc_info = (~(1<<KCCC_B_CR_HV))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_MB )&1)  ccc_info = (~(1<<KCCC_B_MB))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_PMTIC )&1)  ccc_info = (~(1<<KCCC_B_PMTIC))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_DAQ )&1)  ccc_info = (~(1<<KCCC_B_DAQ))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_DC+i_dc)&1)  ccc_info = (~(1<<KCCC_B_DC))&ccc_info;
 
 if ( ( i_dqcr>>KDQCR_B_RELAY+i_dc)&1)  ccc_info = (~(1<<KCCC_B_RELAY))&ccc_info;
 
//       Update status bits from DQCH.
 
 if ( ( i_dqch>>KDQCH_B_PMT_CABLE )&1)  ccc_info = (~(1<<KCCC_B_PMT_CABLE))&ccc_info;
 
 if ( ( i_dqch>>KDQCH_B_PMTIC_RESISTOR )&1) ccc_info = (~(1<<KCCC_B_PMTIC_RESISTOR))&ccc_info;
 
 if ( ( i_dqch>>KDQCH_B_SEQUENCER )&1)  ccc_info = (~(1<<KCCC_B_SEQUENCER))&ccc_info;
 
 if ( imode != KCCC_TUBE_ON_LINE && imode != KCCC_CHAN_ON_LINE )
   {
     if ( ( i_dqch>>KDQCH_B_100NS )&1)  ccc_info = (~(1<<KCCC_B_100NS))&ccc_info;
     
     if ( ( i_dqch>>KDQCH_B_20NS )&1)  ccc_info = (~(1<<KCCC_B_20NS))&ccc_info;
   }

 if ( ( i_dqch>>KDQCH_B_NOT_OP )&1)  ccc_info = (~(1<<KCCC_B_NOT_OP))&ccc_info;
 
//       Ignore 75OHM , QINJ, N100, SLOT_OP and N20 bits for TUBE_ON_LINE
//       and CHAN_ON_LINE queries.
 
        if (imode != KCCC_TUBE_ON_LINE && imode != KCCC_CHAN_ON_LINE ) 
	  {
 
	    if ( ( i_dqch>>KDQCH_B_75OHM )&1) ccc_info = (~(1<<KCCC_B_75OHM))&ccc_info;
 
	    if ( ( i_dqch>>KDQCH_B_QINJ )&1)  ccc_info = (~(1<<KCCC_B_QINJ))&ccc_info;
 
	    if ( ( i_dqch>>KDQCH_B_N100 )&1)  ccc_info = (~(1<<KCCC_B_N100))&ccc_info;
 
	    if ( ( i_dqch>>KDQCH_B_N20 )&1)   ccc_info = (~(1<<KCCC_B_N20))&ccc_info;
 
	    if ( ( i_dqcr>>KDQCR_B_SLOT_OP )&1)  ccc_info = (~(1<<KCCC_B_SLOT_OP))&ccc_info;
 	  }
        if ( ((i_dqch>>KDQCH_B_VTHR)&MASK8)  == 255)  //ie threshold = MAX 
	  {
	    ccc_info = (~(1<<KCCC_B_VTHR_MAX))&ccc_info;
	  }
 
	if ( imode == KCCC_TUBE_ON_LINE ) 
	  {
	    if ( ccc_info == -1 ) 
	      {
		ccc_info = 1;
	      }
	    else
	      {
		ccc_info = 0;
	      }
	  }
	else if ( imode == KCCC_CHAN_ON_LINE ) 
	  {
	    if ( (ccc_info&MASK24)  == MASK24 ) 
	      {
		ccc_info = 1; 
	      }
	    else 
	      {
		ccc_info = 0;
	      }
	  }
 
    }
 
  return ccc_info;
   
}
