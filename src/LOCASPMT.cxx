#include "LOCASPMT.h"
// ROOT includes
#include "TSystem.h"
// RAT includes
#include "RAT/DB.hh"


//*-- Author : Simon JM Peeters
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// LOCASPMT                                                             //
//                                                                      //
// Singleton class to access information about individual PMTs          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//
// Revision History:
// 13 Oct 2011 - Created
//
ClassImp(LOCASPMT)

LOCASPMT * LOCASPMT::fLOCASPMT = NULL;
  
LOCASPMT * LOCASPMT::Get(void)
{
  if ( fLOCASPMT == NULL ) fLOCASPMT = new LOCASPMT();
  return fLOCASPMT;
}

LOCASPMT::LOCASPMT(void) 
{
  if ( !InitGeoPMT() ) {
    cerr << "LOCASPMT c'tor: could not access PMT geometry info" << endl;
  }
}

LOCASPMT::~LOCASPMT()
{ 
}

LOCASPMT::LOCASPMT( const LOCASPMT &pmt )
{
}

LOCASPMT & LOCASPMT::operator=(const LOCASPMT &rhs)
{
  //LOCASPMT assignment operator.
  return *this;
}

bool LOCASPMT::InitGeoPMT(void)
{
  TString dir = gSystem->Getenv("RATROOT");
  if ( dir.Length() == 0 ) {
    cerr << "LOCASPMT::Error : environment variable RATROOT not set" << endl;
    return false;
  }
  if  ( dir(dir.Length()-1) != '/') dir += '/';
  dir += "data";
  TString dbname = "PMTINFO.ratdb";
  TString PMT_pos_file = gSystem->FindFile(dir.Data(),dbname);
  if ( PMT_pos_file.Length() == 0 ) {
    cerr << "LOCASPMT::Error : Could not find database file PMTINFO.ratdb" << endl;
    return false;
  }
  RAT::DB * db = RAT::DB::Get();
  db->Load(PMT_pos_file.Data());  
  RAT::DBLinkPtr link = db->GetLink("PMTINFO");
  fX =link->GetDArray("x");
  fY =link->GetDArray("y");
  fZ =link->GetDArray("z");
  fdirX =link->GetDArray("u");
  fdirY =link->GetDArray("v");
  fdirZ =link->GetDArray("w");
  return true;
}

TVector * LOCASPMT::Position(UInt_t lcn)
{
  return new TVector(fX[lcn],fY[lcn],fZ[lcn]);
}

TVector * LOCASPMT::Direction(UInt_t lcn)
{
  return new TVector(fdirX[lcn],fdirY[lcn],fdirZ[lcn]);
  
}


