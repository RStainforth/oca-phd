////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRun.cc
///
/// CLASS: LOCAS::LOCASRun
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///        (Full description in LOCASRun.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASLightPath.hh"

#include <map>

#include "RAT/DS/SOCPMT.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/SOCReader.hh"

using namespace LOCAS;
using namespace std;

ClassImp( LOCASRun )

// Default Constructor
LOCASRun::LOCASRun()
{  
  // NULL-ify PMT private variables  
  fLOCASPMTs.clear();
  Initialise();
}

//////////////////////////////////////
//////////////////////////////////////

// Default Destructor
LOCASRun::~LOCASRun()
{
  // Delete LOCASPMT array
  
}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun::LOCASRun( const LOCASRun& locasRHS )
{

  // Copies all Run information from locasRHS
  // EXCEPT for the fLOCASPMTs map

  fRunID = locasRHS.fRunID;
  fSourceID = locasRHS.fSourceID;

  fIsMainRun = locasRHS.fIsMainRun;
  fIsCentralRun = locasRHS.fIsCentralRun;
  fIsWavelengthRun = locasRHS.fIsWavelengthRun;

  fLambda = locasRHS.fLambda;
  fNLBPulses = locasRHS.fNLBPulses;

  fMainLBIntensityNorm = locasRHS.fMainLBIntensityNorm;
  fCentralLBIntensityNorm = locasRHS.fCentralLBIntensityNorm;
  fWavelengthLBIntensityNorm = locasRHS.fWavelengthLBIntensityNorm;

  fTimeSigmaMean = locasRHS.fTimeSigmaMean;
  fTimeSigmaSigma = locasRHS.fTimeSigmaSigma;

  fLBPosType = locasRHS.fLBPosType;

  fLBPos = locasRHS.fLBPos;

  fLBXPosErr = locasRHS.fLBXPosErr;
  fLBYPosErr = locasRHS.fLBYPosErr;
  fLBZPosErr = locasRHS.fLBZPosErr;

  fLBPosChi2 = locasRHS.fLBPosChi2;
  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fDirFitLBPos = locasRHS.fDirFitLBPos;

  fDirFitLBXPosErr = locasRHS.fDirFitLBXPosErr;
  fDirFitLBYPosErr = locasRHS.fDirFitLBYPosErr;
  fDirFitLBZPosErr = locasRHS.fDirFitLBZPosErr;

  fDirFitLBPosChi2 = locasRHS.fDirFitLBPosChi2;

  fLPFitLBPos = locasRHS.fLPFitLBPos;

  fLPFitLBXPosErr = locasRHS.fLPFitLBXPosErr;
  fLPFitLBYPosErr = locasRHS.fLPFitLBYPosErr;
  fLPFitLBZPosErr = locasRHS.fLPFitLBZPosErr;

  fLPFitLBPosChi2 = locasRHS.fLPFitLBPosChi2;

  fManipLBPos = locasRHS.fManipLBPos;

  fManipLBXPosErr = locasRHS.fManipLBXPosErr;
  fManipLBYPosErr = locasRHS.fManipLBYPosErr;
  fManipLBZPosErr = locasRHS.fManipLBZPosErr;

  fManipLBPosChi2 = locasRHS.fManipLBPosChi2;

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun& LOCASRun::operator=( const LOCASRun& locasRHS )
{

  // Copies all Run information from locasRHS
  // EXCEPT for the fLOCASPMTs map

  fRunID = locasRHS.fRunID;
  fSourceID = locasRHS.fSourceID;

  fIsMainRun = locasRHS.fIsMainRun;
  fIsCentralRun = locasRHS.fIsCentralRun;
  fIsWavelengthRun = locasRHS.fIsWavelengthRun;

  fLambda = locasRHS.fLambda;
  fNLBPulses = locasRHS.fNLBPulses;

  fMainLBIntensityNorm = locasRHS.fMainLBIntensityNorm;
  fCentralLBIntensityNorm = locasRHS.fCentralLBIntensityNorm;
  fWavelengthLBIntensityNorm = locasRHS.fWavelengthLBIntensityNorm;

  fTimeSigmaMean = locasRHS.fTimeSigmaMean;
  fTimeSigmaSigma = locasRHS.fTimeSigmaSigma;

  fLBPosType = locasRHS.fLBPosType;

  fLBPos = locasRHS.fLBPos;

  fLBXPosErr = locasRHS.fLBXPosErr;
  fLBYPosErr = locasRHS.fLBYPosErr;
  fLBZPosErr = locasRHS.fLBZPosErr;

  fLBPosChi2 = locasRHS.fLBPosChi2;
  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fDirFitLBPos = locasRHS.fDirFitLBPos;

  fDirFitLBXPosErr = locasRHS.fDirFitLBXPosErr;
  fDirFitLBYPosErr = locasRHS.fDirFitLBYPosErr;
  fDirFitLBZPosErr = locasRHS.fDirFitLBZPosErr;

  fDirFitLBPosChi2 = locasRHS.fDirFitLBPosChi2;

  fLPFitLBPos = locasRHS.fLPFitLBPos;

  fLPFitLBXPosErr = locasRHS.fLPFitLBXPosErr;
  fLPFitLBYPosErr = locasRHS.fLPFitLBYPosErr;
  fLPFitLBZPosErr = locasRHS.fLPFitLBZPosErr;

  fLPFitLBPosChi2 = locasRHS.fLPFitLBPosChi2;

  fManipLBPos = locasRHS.fManipLBPos;

  fManipLBXPosErr = locasRHS.fManipLBXPosErr;
  fManipLBYPosErr = locasRHS.fManipLBYPosErr;
  fManipLBZPosErr = locasRHS.fManipLBZPosErr;

  fManipLBPosChi2 = locasRHS.fManipLBPosChi2;

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Initialise(){ }

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Clear( Option_t* option )
{
  // Reset all the values for this LOCASRun

  SetRunID( 0 );
  SetSourceID( 0 );

  SetIsMainRun( false );
  SetIsCentralRun( false );
  SetIsWavelengthRun( false );

  SetLambda( 0.0 );
  SetNLBPulses( 0.0 );

  SetMainLBIntensityNorm( 0.0 );
  SetCentralLBIntensityNorm( 0.0 );
  SetWavelengthLBIntensityNorm( 0.0 );

  SetTimeSigmaMean( 0.0 );
  SetTimeSigmaSigma( 0.0 );

  SetLBPosType( 0 );

  SetLBPos( -9999.9, -9999.9, -9999.9 );

  SetLBXPosErr( 0.0 );
  SetLBYPosErr( 0.0 );
  SetLBZPosErr( 0.0 );

  SetLBPosChi2( 0.0 );
  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetDirFitLBPos( -9999.9, -9999.9, -9999.9 );

  SetDirFitLBXPosErr( 0.0 );
  SetDirFitLBYPosErr( 0.0 );
  SetDirFitLBZPosErr( 0.0 );

  SetDirFitLBPosChi2( 0.0 );

  SetLPFitLBPos( -9999.9, -9999.9, -9999.9 );

  SetLPFitLBXPosErr( 0.0 );
  SetLPFitLBYPosErr( 0.0 );
  SetLPFitLBZPosErr( 0.0 );

  SetLPFitLBPosChi2( 0.0 );

  SetManipLBPos( -9999.9, -9999.9, -9999.9 );

  SetManipLBXPosErr( 0.0 );
  SetManipLBYPosErr( 0.0 );
  SetManipLBZPosErr( 0.0 );

  SetManipLBPosChi2( 0.0 );

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Fill( RAT::SOCReader& socR, Int_t runID )
{

  // First perform the neccessary checks
  RAT::DS::SOC* socPtr = NULL;
  Long64_t NSOCs = socR.GetNSOC();

  for ( Int_t iSOC = 0; iSOC < NSOCs; iSOC++ ){

    socPtr = socR.GetSOC( iSOC );

    if ( socPtr->GetRunID() == runID ){
      socPtr = socR.GetSOC( iSOC );
      break;
    }

    if ( ( iSOC == NSOCs - 1 ) && ( socPtr->GetRunID() != runID ) ){
      std::cout << "No SOC File with specified Run ID found in the SOCReader! Setting to Default" << std::endl;
      socPtr = socR.GetSOC( 0 );
    }

  }

  // Now that a SOC file which matches the run ID specified has been
  // found. We can now begin to fill the LOCASRun object with all the
  // neccessary information.
  CopySOCRunInfo( socPtr );
  CopySOCPMTInfo( socPtr );

  // Create the LOCAS Data base Object (LOCASDB) and
  // load PMT information and detector parameters;
  LOCASDB lDB;
  // Load the RAT PMT Positions, PMT Normals and Types
  lDB.LoadPMTPositions();
  lDB.LoadPMTNormals();
  lDB.LoadPMTTypes();

  // Create an iterator to loop over the PMTs...
  std::map<Int_t, LOCASPMT>::iterator iLP;

  // The PMT ID variable for each PMT in the loop
  Int_t pmtID;
  
  // Create a LOCASLightPath which will be calculated for each 
  // loop ( and inherently each PMT )
  LOCASLightPath lLP;
  
  // Set the PMT positions and normals and then 'feed' the PMT a calculated light path.
  // The PMT 'eats' this light path and calculates it's member variables
  // e.g. distance through scintillator, acrylic and water and various flags based
  // on the quality of the light path.
  for ( iLP = GetLOCASPMTIterBegin(); iLP != GetLOCASPMTIterEnd(); iLP++ ){

    // Get the PMT ID and LOCASPMT object
    pmtID = iLP->first;
    
    // Set the Normals and Positions
    ( iLP->second ).SetPos( lDB.GetPMTPosition( pmtID ) );
    ( iLP->second ).SetNorm( lDB.GetPMTNormal( pmtID ) );
    ( iLP->second ).SetType( lDB.GetPMTType( pmtID ) );

    ( iLP->second ).SetLBTheta( ( lDB.GetPMTPosition( pmtID ) - GetLBPos() ).Theta() );
    ( iLP->second ).SetLBPhi( ( lDB.GetPMTPosition( pmtID ) - GetLBPos() ).Phi() );
    
    // Calculate the light path for this source position and PMT
    lLP.CalculatePath( GetLBPos(), GetPMT( iLP->first ).GetPos(), 10.0, GetLambda() );
    
    // 'feed' the light path to the PMT
    ( iLP->second ).ProcessLightPath( lLP );
    
    // Reset the light path object
    lLP.Clear();
    
  }

  // Clear the LOCASDB object to free up memory
  lDB.Clear();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CalculateLBIntensityNorm()
{

  fMainLBIntensityNorm = 0.0;
  std::map< Int_t, LOCASPMT>::iterator iPMT;
  
  for ( iPMT = GetLOCASPMTIterBegin(); iPMT != GetLOCASPMTIterEnd(); iPMT++ ){   
    fMainLBIntensityNorm += ( iPMT->second ).GetOccupancy();
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCRunInfo( RAT::DS::SOC* socRun )
{

  // Copies all the Run-level information from a SOC file
  // and puts it into (this) LOCASRun object

  SetRunID( socRun->GetRunID() );
  SetSourceID( socRun->GetSourceID() );
  SetLambda( socRun->GetLaserWavelength() );
  SetLBPos( socRun->GetSourcePosManip() );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCPMTInfo( RAT::DS::SOC* socRun )
{

  // Copies all the SOCPMTs information from from a SOC
  // file and copies them into LOCASPMT objects

  std::map<Int_t, RAT::DS::SOCPMT>::iterator iSOCPMT;
  
  for ( iSOCPMT = socRun->GetSOCPMTIterBegin(); iSOCPMT != socRun->GetSOCPMTIterEnd(); ++iSOCPMT ){
    AddSOCPMT( iSOCPMT->second );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopyLOCASRunInfo( LOCASRun& locasRun )
{

  *this = locasRun;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopyLOCASPMTInfo( LOCASRun& locasRun )
{

  // Copies all the LOCASPMTs information from from a
  // different LOCASRun object and copies them into 
  // new LOCASPMT objects in this LOCASRun

  std::map<Int_t, LOCASPMT>::iterator iLOCASPMT;

  for ( iLOCASPMT = locasRun.GetLOCASPMTIterBegin(); iLOCASPMT != locasRun.GetLOCASPMTIterEnd(); ++iLOCASPMT ){
    AddLOCASPMT( iLOCASPMT->second );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::AddSOCPMT( RAT::DS::SOCPMT& socPMT )
{


  Int_t pmtID = socPMT.GetLCN();

  if( fLOCASPMTs.find( pmtID ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = LOCASPMT( pmtID );
  }

  else{
    std::cout << "PMT Not Added - PMT with same ID already exists" << std::endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::AddLOCASPMT( LOCASPMT& locasPMT )
{

  Int_t pmtID = locasPMT.GetID();

  if( fLOCASPMTs.find( locasPMT.GetID() ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = locasPMT;
  }

  else{
    std::cout << "PMT Not Added - PMT with same ID already exists" << std::endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::RemovePMT( Int_t iPMT )
{

  // Remove a LOCAS PMT from the fLOCASPMTs map
  fLOCASPMTs.erase( iPMT );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASRun::GetPMT( Int_t iPMT )
{

  // Return the LOCASPMT with LCN 'iPMT'
  
  if( fLOCASPMTs.find( iPMT ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ iPMT ] = LOCASPMT( iPMT );
  }

  return fLOCASPMTs[ iPMT ];
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::SetLBPosType( Int_t posType )
{
  // Set the fitted laserball type to be used in the LOCAS fit
  // 0 := Direct Straight Line laserball position
  // 1 := RAT::DS::LightPath fitted laserball position
  // 2 := Manipulator coordinates laserball position

  switch ( posType ) {

  case 0:
    SetLBPos( fManipLBPos );
    break;

  case 1:
    SetLBPos( fDirFitLBPos );
    break;

  case 2:
    SetLBPos( fLPFitLBPos );
    break;

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CrossRunFill( LOCASRun& cRun, LOCASRun& wRun )
{

  // Check that the specified Runs are what they say they are
  // i.e. that 'cRun' is a central run (and whether it has been filled)
  // and that 'wRun' is a wavelength-run (and whether is has been filled)
  
  Bool_t validMRun, validCRun, validWRun = false;

  if ( fIsMainRun != true ){ cout << "LOCAS::LOCASRun::CrossRunFill ERROR: The Run that is being filled is not the main run! Aborting." << endl; return; }
  else { validMRun = true; }

  if ( cRun.GetIsCentralRun() ){
    if ( cRun.GetRunID() < 0 ){ cout << "LOCAS::LOCASRun::CrossRunFill: No central-run information specified to be cross-filled into run file " << fRunID << "_LOCASRun.root" << endl; }
    else{ validCRun == true; cout << "LOCAS::LOCASRun::CrossRunFill: Central-run information has been specified and will be cross-filled into run file " << fRunID << "_LOCASRun.root" << endl; }
  }
  else{ cout << "LOCAS::LOCASRun::CrossRunFill: Specified 'central-run' isn't a central-run. Aborting." << endl; return; }
  
  if ( wRun.GetIsWavelengthRun() ){
    if ( wRun.GetRunID() < 0 ){ cout << "LOCAS::LOCASRun::CrossRunFill: No wavelength-run information specified to be cross-filled into run file " << fRunID << "_LOCASRun.root" << endl; }
    else{ validWRun == true; cout << "LOCAS::LOCASRun::CrossRunFill: wavelength-run information has been specified and will be cross-filled into run file " << fRunID << "_LOCASRun.root" << endl; }
  }
  else{ cout << "LOCAS::LOCASRun::CrossRunFill: Specified 'wavelength-run' isn't a wavelength-run. Aborting." << endl; return; }
  // As an example, now fill all the relative solid-angles
  
  std::map<Int_t, LOCASPMT>::iterator iPMT;
  
  for ( iPMT = GetLOCASPMTIterBegin(); iPMT != GetLOCASPMTIterEnd(); ++iPMT ){
    
    LOCASPMT rPMT ( fLOCASPMTs[ iPMT->first ] );

    Int_t pmtID = rPMT.GetID();
    LOCASPMT crPMT = cRun.GetPMT( pmtID );

    if ( crPMT.GetHasEntries() ){
      ( fLOCASPMTs[ pmtID ] ).SetCorrSolidAngle( rPMT.GetSolidAngle() / crPMT.GetSolidAngle() );
      ( fLOCASPMTs[ pmtID ] ).SetCorrFresnelTCoeff( rPMT.GetFresnelTCoeff() / crPMT.GetFresnelTCoeff() );
    }
    
  }  
  
  
}


 






