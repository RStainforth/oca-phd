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
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"

#include <map>

#include "RAT/DS/SOCPMT.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/FitResult.hh"
#include "RAT/DS/FitVertex.hh"
#include "RAT/DU/SOCReader.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/DU/LightPathCalculator.hh"

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

  fRunID = locasRHS.fRunID;
  fCentralRunID = locasRHS.fCentralRunID;
  fWavelengthRunID = locasRHS.fWavelengthRunID;

  fSourceID = locasRHS.fSourceID;
  fCentralSourceID = locasRHS.fCentralSourceID;
  fWavelengthSourceID = locasRHS.fWavelengthSourceID;

  fIsMainRun = locasRHS.fIsMainRun;
  fIsCentralRun = locasRHS.fIsCentralRun;
  fIsWavelengthRun = locasRHS.fIsWavelengthRun;

  fLambda = locasRHS.fLambda;
  fNLBPulses = locasRHS.fNLBPulses;

  fCentralLambda = locasRHS.fCentralLambda;
  fCentralNLBPulses = locasRHS.fCentralNLBPulses;

  fWavelengthLambda = locasRHS.fWavelengthLambda;
  fWavelengthNLBPulses = locasRHS.fWavelengthNLBPulses;

  fLBIntensityNorm = locasRHS.fLBIntensityNorm;
  fCentralLBIntensityNorm = locasRHS.fCentralLBIntensityNorm;
  fWavelengthLBIntensityNorm = locasRHS.fWavelengthLBIntensityNorm;

  fLBPos = locasRHS.fLBPos;
  fCentralLBPos = locasRHS.fCentralLBPos;
  fWavelengthLBPos = locasRHS.fWavelengthLBPos;

  fLBXPosErr = locasRHS.fLBXPosErr;
  fLBYPosErr = locasRHS.fLBYPosErr;
  fLBZPosErr = locasRHS.fLBZPosErr;

  fCentralLBXPosErr = locasRHS.fCentralLBXPosErr;
  fCentralLBYPosErr = locasRHS.fCentralLBYPosErr;
  fCentralLBZPosErr = locasRHS.fCentralLBZPosErr;

  fWavelengthLBXPosErr = locasRHS.fWavelengthLBXPosErr;
  fWavelengthLBYPosErr = locasRHS.fWavelengthLBYPosErr;
  fWavelengthLBZPosErr = locasRHS.fWavelengthLBZPosErr;

  fLBPosChi2 = locasRHS.fLBPosChi2;
  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fCentralLBPosChi2 = locasRHS.fCentralLBPosChi2;
  fCentralLBTheta = locasRHS.fCentralLBTheta;
  fCentralLBPhi = locasRHS.fCentralLBPhi;

  fWavelengthLBPosChi2 = locasRHS.fWavelengthLBPosChi2;
  fWavelengthLBTheta = locasRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = locasRHS.fWavelengthLBPhi;

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun& LOCASRun::operator=( const LOCASRun& locasRHS )
{

  // Copies all Run information from locasRHS
  // EXCEPT for the fLOCASPMTs map

  fRunID = locasRHS.fRunID;
  fCentralRunID = locasRHS.fCentralRunID;
  fWavelengthRunID = locasRHS.fWavelengthRunID;

  fSourceID = locasRHS.fSourceID;
  fCentralSourceID = locasRHS.fCentralSourceID;
  fWavelengthSourceID = locasRHS.fWavelengthSourceID;

  fIsMainRun = locasRHS.fIsMainRun;
  fIsCentralRun = locasRHS.fIsCentralRun;
  fIsWavelengthRun = locasRHS.fIsWavelengthRun;

  fLambda = locasRHS.fLambda;
  fNLBPulses = locasRHS.fNLBPulses;

  fCentralLambda = locasRHS.fCentralLambda;
  fCentralNLBPulses = locasRHS.fCentralNLBPulses;

  fWavelengthLambda = locasRHS.fWavelengthLambda;
  fWavelengthNLBPulses = locasRHS.fWavelengthNLBPulses;

  fLBIntensityNorm = locasRHS.fLBIntensityNorm;
  fCentralLBIntensityNorm = locasRHS.fCentralLBIntensityNorm;
  fWavelengthLBIntensityNorm = locasRHS.fWavelengthLBIntensityNorm;

  fLBPos = locasRHS.fLBPos;
  fCentralLBPos = locasRHS.fCentralLBPos;
  fWavelengthLBPos = locasRHS.fWavelengthLBPos;

  fLBXPosErr = locasRHS.fLBXPosErr;
  fLBYPosErr = locasRHS.fLBYPosErr;
  fLBZPosErr = locasRHS.fLBZPosErr;

  fCentralLBXPosErr = locasRHS.fCentralLBXPosErr;
  fCentralLBYPosErr = locasRHS.fCentralLBYPosErr;
  fCentralLBZPosErr = locasRHS.fCentralLBZPosErr;

  fWavelengthLBXPosErr = locasRHS.fWavelengthLBXPosErr;
  fWavelengthLBYPosErr = locasRHS.fWavelengthLBYPosErr;
  fWavelengthLBZPosErr = locasRHS.fWavelengthLBZPosErr;

  fLBPosChi2 = locasRHS.fLBPosChi2;
  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fCentralLBPosChi2 = locasRHS.fCentralLBPosChi2;
  fCentralLBTheta = locasRHS.fCentralLBTheta;
  fCentralLBPhi = locasRHS.fCentralLBPhi;

  fWavelengthLBPosChi2 = locasRHS.fWavelengthLBPosChi2;
  fWavelengthLBTheta = locasRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = locasRHS.fWavelengthLBPhi;


  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Initialise()
{ 

  SetRunID( -1 );
  SetCentralRunID( -1 );
  SetWavelengthRunID( -1 );

  SetSourceID( "" );
  SetCentralSourceID( "" );
  SetWavelengthSourceID( "" );

  SetIsMainRun( false );
  SetIsCentralRun( false );
  SetIsWavelengthRun( false );

  SetLambda( -10.0 );
  SetNLBPulses( -10.0 );  
  SetCentralLambda( -10.0 );
  SetCentralNLBPulses( -10.0 );
  SetWavelengthLambda( -10.0 );
  SetWavelengthNLBPulses( -10.0 );

  SetLBIntensityNorm( -10.0 );
  SetCentralLBIntensityNorm( -10.0 );
  SetWavelengthLBIntensityNorm( -10.0 );

  SetLBPos( -9999.9, -9999.9, -9999.9 );
  SetCentralLBPos( -9999.9, -9999.9, -9999.9 );
  SetWavelengthLBPos( -9999.9, -9999.9, -9999.9 );

  SetLBXPosErr( -10.0 );
  SetLBYPosErr( -10.0 );
  SetLBZPosErr( -10.0 );

  SetCentralLBXPosErr( -10.0 );
  SetCentralLBYPosErr( -10.0 );
  SetCentralLBZPosErr( -10.0 );

  SetWavelengthLBXPosErr( -10.0 );
  SetWavelengthLBYPosErr( -10.0 );
  SetWavelengthLBZPosErr( -10.0 );

  SetLBPosChi2( -10.0 );
  SetLBTheta( -10.0 );
  SetLBPhi( -10.0 );

  SetCentralLBPosChi2( -10.0 );
  SetCentralLBTheta( -10.0 );
  SetCentralLBPhi( -10.0 );

  SetWavelengthLBPosChi2( -10.0 );
  SetWavelengthLBTheta( -10.0 );
  SetWavelengthLBPhi( -10.0 );

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Clear( Option_t* option )
{

  Initialise();
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Fill( RAT::DU::SOCReader& socR, 
                     RAT::DU::LightPathCalculator& lLP,
                     RAT::DU::PMTInfo& lDB,
                     Int_t runID )
{

  RAT::DS::SOC* socPtr = new RAT::DS::SOC;
  // First check that a SOC file with the specified runID exists in the SOCReader
  for ( Int_t iSOC = 0; iSOC < socR.GetSOCCount(); iSOC++ ){
    *socPtr = socR.GetSOC( iSOC );
    if ( socPtr->GetRunID() == runID ){ break; }
    else{ continue; }
    
    if ( iSOC == ( socR.GetSOCCount() - 1 ) ){
      cout << "LOCASRun::Fill: Error: No SOC file with specified run-ID found" << endl;
    }
  }

  // Now that a SOC file which matches the run ID specified has been
  // found. We can now begin to fill the LOCASRun object with all the
  // neccessary information.
  
  // The run information from the SOC file...
  CopySOCRunInfo( *socPtr );
  // ... and the PMT information from the SOC file.
  CopySOCPMTInfo( *socPtr );

  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  cout << "Laserball Position is: ( " 
       << GetLBPos().X() << ", "
       << GetLBPos().Y() << ", "
       << GetLBPos().Z() << " ) mm, R = " << GetLBPos().Mag() << " mm" << endl;

  // Create an iterator to loop over the PMTs...
  std::map< Int_t, LOCASPMT >::iterator iLP;

  // The PMT ID variable for each PMT in the loop
  Int_t pmtID;

  Double_t wavelengthMeV = lLP.WavelengthToEnergy( GetLambda() * 1.0e-6 );
  
  // Set the PMT positions and normals and then 'feed' the PMT a calculated light path.
  // The PMT 'eats' this light path and calculates it's member variables
  // e.g. distance through scintillator, acrylic and water and various flags based
  // on the quality of the light path.
  for ( iLP = GetLOCASPMTIterBegin(); iLP != GetLOCASPMTIterEnd(); iLP++ ){

    // Get the PMT ID and LOCASPMT object
    pmtID = iLP->first;
    // Set the run ID
    ( iLP->second ).SetRunID( runID );

    // Set the Normals and Positions
    ( iLP->second ).SetPos( lDB.GetPosition( pmtID ) );
    ( iLP->second ).SetNorm( lDB.GetDirection( pmtID ) );
    ( iLP->second ).SetType( lDB.GetType( pmtID ) );

    // Put some of the run specific information on the PMT data structure.
    // This is worth it when it comes to fitting, as only a single pointer to
    // the PMT object will be required.
    ( iLP->second ).SetNLBPulses( GetNLBPulses() );
    ( iLP->second ).SetLBIntensityNorm( GetLBIntensityNorm() );

    LOCASMath lMath;
    ( iLP->second ).SetMPECorrOccupancy( lMath.MPECorrectedNPrompt( ( iLP->second ).GetOccupancy(), GetNLBPulses() ) );
    ( iLP->second ).SetMPECorrOccupancyErr( lMath.MPECorrectedNPromptErr( ( iLP->second ).GetOccupancy(), GetNLBPulses() ) );
    ( iLP->second ).SetMPECorrOccupancyCorr( ( iLP->second ).GetMPECorrOccupancy() / ( iLP->second ).GetOccupancy() );
    
    // Calculate the light path for this source position and PMT
    lLP.CalcByPosition( GetLBPos(), GetPMT( iLP->first ).GetPos(), wavelengthMeV, 10.0 );
    
    // 'feed' the light path to the PMT
    ( iLP->second ).ProcessLightPath( lLP );

    ///////// Off-Axis Laserball Theta and Phi Angles //////////
    TVector3 lbAxis( 0.0, 0.0, 1.0 );
    lbAxis.SetPhi( GetLBPhi() );
    lbAxis.SetTheta( GetLBTheta() );
    
    ( iLP->second ).SetRelLBTheta( ( ( iLP->second ).GetInitialLBVec() ).Angle( lbAxis ) );
    Float_t laserPhi = ( ( iLP->second ).GetInitialLBVec() ).Phi();
    Float_t relLBPhi = fmod( (Float_t)( laserPhi + lbAxis.Phi() ), 2.0 * M_PI ); 
    ( iLP->second ).SetRelLBPhi( relLBPhi );
  
    // Reset the light path object
    lLP.Clear();

   
    ( iLP->second ).VerifyPMT();
    
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCRunInfo( RAT::DS::SOC& socRun )
{

  // Copies all the Run-level information from a SOC file
  // and puts it into (this) LOCASRun object

  SetRunID( socRun.GetRunID() );
  SetSourceID( socRun.GetSourceID() );
  SetLambda( (Double_t)socRun.GetCalib().GetMode() );

  RAT::DS::FitResult lbFit = socRun.GetFitResult( "lbfit" );
  RAT::DS::FitVertex lbVertex = lbFit.GetVertex( 0 );
   
  SetLBPos( lbVertex.GetPosition() );

  SetLBXPosErr( lbVertex.GetPositivePositionError().X() );
  SetLBXPosErr( lbVertex.GetPositivePositionError().Y() );
  SetLBXPosErr( lbVertex.GetPositivePositionError().Z() );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCPMTInfo( RAT::DS::SOC& socRun )
{

  // Copies all the SOCPMTs information from from a SOC
  // file and copies them into LOCASPMT objects

  std::vector< UInt_t > pmtIDs = socRun.GetSOCPMTIDs();

  for ( Int_t iPMT = 0; iPMT < pmtIDs.size(); iPMT++ ){
    AddSOCPMT( socRun.GetSOCPMT( pmtIDs[ iPMT ] ) );
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
    ( fLOCASPMTs[ pmtID ] ).AddSOCPMTData( socPMT );
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

  if( fLOCASPMTs.find( pmtID ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = locasPMT;
  }

  else{
    std::cout << "PMT Not Added - PMT with same ID already exists, PMT-ID: " << pmtID <<  std::endl;
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
  
  // if( fLOCASPMTs.find( iPMT ) == fLOCASPMTs.end() ){
  //   fLOCASPMTs[ iPMT ] = LOCASPMT( iPMT );
  // }

  return fLOCASPMTs[ iPMT ];
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CrossRunFill( LOCASRun* cRun, LOCASRun* wRun )
{

  if ( cRun == NULL && wRun == NULL ){
    cout << "LOCASRun::CrossRunFill: Error: No Central or Wavelength Run Information to fill from" << endl;
    cout << "--------------------------" << endl;
    return;
  }

  if ( cRun != NULL ){
    
    fCentralRunID = cRun->GetRunID();
    fCentralSourceID = cRun->GetSourceID();
    
    fCentralLambda = cRun->GetLambda();
    fCentralNLBPulses = cRun->GetNLBPulses();
    
    fCentralLBIntensityNorm = cRun->GetLBIntensityNorm();
    fCentralLBPos = cRun->GetLBPos();
    
    fCentralLBTheta = cRun->GetLBTheta();
    fCentralLBPhi = cRun->GetLBPhi();
        
  }
  
  if ( wRun != NULL ){
    
    fWavelengthRunID = wRun->GetRunID();
    fWavelengthSourceID = wRun->GetSourceID();
    
    fWavelengthLambda = wRun->GetLambda();
    fWavelengthNLBPulses = wRun->GetNLBPulses();
    
    fWavelengthLBIntensityNorm = wRun->GetLBIntensityNorm();
    fWavelengthLBPos = wRun->GetLBPos();
    
    fWavelengthLBTheta = wRun->GetLBTheta();
    fWavelengthLBPhi = wRun->GetLBPhi();
    
  }

  if ( cRun ){

    cout << "LOCASRun::CrossRunFill: Filling Central Run Information..." << endl;
    cout << "--------------------------" << endl;
    std::map< Int_t, LOCASPMT >::iterator iCPMT;
    for( iCPMT = cRun->GetLOCASPMTIterBegin(); iCPMT != cRun->GetLOCASPMTIterEnd(); iCPMT++ ){
      Int_t pmtID = ( iCPMT->first );
      ( fLOCASPMTs[ pmtID ] ).SetCentralRunID( cRun->GetRunID() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralIsVerified( ( iCPMT->second ).GetIsVerified() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralPromptPeakTime( ( iCPMT->second ).GetPromptPeakTime() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralPromptPeakWidth( ( iCPMT->second ).GetPromptPeakWidth() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralTimeOfFlight( ( iCPMT->second ).GetTimeOfFlight() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancy( ( iCPMT->second ).GetOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancyErr( ( iCPMT->second ).GetOccupancyErr() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancyCorr( ( iCPMT->second ).GetOccupancyCorr() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralLBIntensityNorm( ( iCPMT->second ).GetLBIntensityNorm() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralNLBPulses( ( iCPMT->second ).GetNLBPulses() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancy( ( iCPMT->second ).GetMPECorrOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancyErr( ( iCPMT->second ).GetMPECorrOccupancyErr() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancyCorr( ( iCPMT->second ).GetMPECorrOccupancyCorr() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralFresnelTCoeff( ( iCPMT->second ).GetFresnelTCoeff() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralDistInInnerAV( ( iCPMT->second ).GetDistInInnerAV() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralDistInAV( ( iCPMT->second ).GetDistInAV() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralDistInWater( ( iCPMT->second ).GetDistInWater() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralDistInNeck( ( iCPMT->second ).GetDistInNeck() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralTotalDist( ( iCPMT->second ).GetTotalDist() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralSolidAngle( ( iCPMT->second ).GetSolidAngle() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralCosTheta( ( iCPMT->second ).GetCosTheta() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralRelLBTheta( ( iCPMT->second ).GetRelLBTheta() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralRelLBPhi( ( iCPMT->second ).GetRelLBPhi() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralCHSFlag( ( iCPMT->second ).GetCHSFlag() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralCSSFlag( ( iCPMT->second ).GetCSSFlag() );

      ( fLOCASPMTs[ pmtID ] ).SetCentralDQXXFlag( ( iCPMT->second ).GetDQXXFlag() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralBadPath( ( iCPMT->second ).GetBadPath() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralNeckFlag( ( iCPMT->second ).GetNeckFlag() );
      
    }
  }
  
  if ( wRun ){

    cout << "LOCASRun::CrossRunFill: Filling Wavelength Run Information..." << endl;
    cout << "--------------------------" << endl;
    std::map< Int_t, LOCASPMT >::iterator iWPMT;
    for( iWPMT = wRun->GetLOCASPMTIterBegin(); iWPMT != wRun->GetLOCASPMTIterEnd(); iWPMT++ ){
      Int_t pmtID = ( iWPMT->first );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthRunID( wRun->GetRunID() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthIsVerified( ( iWPMT->second ).GetIsVerified() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthPromptPeakTime( ( iWPMT->second ).GetPromptPeakTime() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthPromptPeakWidth( ( iWPMT->second ).GetPromptPeakWidth() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthTimeOfFlight( ( iWPMT->second ).GetTimeOfFlight() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancy( ( iWPMT->second ).GetOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancyErr( ( iWPMT->second ).GetOccupancyErr() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancyCorr( ( iWPMT->second ).GetOccupancyCorr() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthLBIntensityNorm( ( iWPMT->second ).GetLBIntensityNorm() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthNLBPulses( ( iWPMT->second ).GetNLBPulses() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancy( ( iWPMT->second ).GetMPECorrOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancyErr( ( iWPMT->second ).GetMPECorrOccupancyErr() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancyCorr( ( iWPMT->second ).GetMPECorrOccupancyCorr() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthFresnelTCoeff( ( iWPMT->second ).GetFresnelTCoeff() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInInnerAV( ( iWPMT->second ).GetDistInInnerAV() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInAV( ( iWPMT->second ).GetDistInAV() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInWater( ( iWPMT->second ).GetDistInWater() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthDistInNeck( ( iWPMT->second ).GetDistInNeck() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthTotalDist( ( iWPMT->second ).GetTotalDist() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthSolidAngle( ( iWPMT->second ).GetSolidAngle() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthCosTheta( ( iWPMT->second ).GetCosTheta() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthRelLBTheta( ( iWPMT->second ).GetRelLBTheta() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthRelLBPhi( ( iWPMT->second ).GetRelLBPhi() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthCHSFlag( ( iWPMT->second ).GetCHSFlag() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthCSSFlag( ( iWPMT->second ).GetCSSFlag() );

      ( fLOCASPMTs[ pmtID ] ).SetWavelengthDQXXFlag( ( iWPMT->second ).GetDQXXFlag() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthBadPath( ( iWPMT->second ).GetBadPath() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthNeckFlag( ( iWPMT->second ).GetNeckFlag() );
      
    }
  }

  // Calculate the number of prompt counts over each PMT for the run
  CalculateLBIntensityNorm();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CalculateLBIntensityNorm()
{
  
  std::map< Int_t, LOCASPMT >::iterator iPMT;
  Float_t lbIntensityNorm = 0.0;
  Float_t centrallbIntensityNorm = 0.0;
  Float_t wavelengthlbIntensityNorm = 0.0;

  Int_t nPMTs = 0;
  Int_t nCentralPMTs = 0;
  Int_t nWavelengthPMTs = 0;

  for ( iPMT = GetLOCASPMTIterBegin(); iPMT != GetLOCASPMTIterEnd(); iPMT++ ){
    if ( ( iPMT->second ).GetDQXXFlag() == 1 ){
      lbIntensityNorm += ( iPMT->second ).GetMPECorrOccupancy();
      nPMTs++;
    }
    if ( ( iPMT->second ).GetCentralDQXXFlag() == 1 ){
      centrallbIntensityNorm += ( iPMT->second ).GetCentralMPECorrOccupancy();
      nCentralPMTs++;
    }
    if ( ( iPMT->second ).GetWavelengthDQXXFlag() == 1 ){
      wavelengthlbIntensityNorm += ( iPMT->second ).GetWavelengthMPECorrOccupancy();
      nWavelengthPMTs++;
    }
  }

  if ( nPMTs != 0 ){ lbIntensityNorm /= nPMTs; }
  else{ lbIntensityNorm = -10.0; }

  if ( nCentralPMTs != 0 ){ centrallbIntensityNorm /= nCentralPMTs; }
  else{ centrallbIntensityNorm = -10.0; }

  if ( nWavelengthPMTs != 0 ){ wavelengthlbIntensityNorm /= nWavelengthPMTs; }
  else{ wavelengthlbIntensityNorm = -10.0; }

  fLBIntensityNorm = lbIntensityNorm;
  fCentralLBIntensityNorm = centrallbIntensityNorm;
  fWavelengthLBIntensityNorm = wavelengthlbIntensityNorm;

}

 






