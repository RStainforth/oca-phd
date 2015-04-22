#include "RAT/DS/SOCPMT.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/FitResult.hh"
#include "RAT/DS/FitVertex.hh"
#include "RAT/DU/SOCReader.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/DU/LightPathCalculator.hh"


#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"

#include "TMath.h"

#include <map>
#include <iostream>

using namespace std;
using namespace LOCAS;

ClassImp( LOCASRun )

//////////////////////////////////////
//////////////////////////////////////

LOCASRun::LOCASRun( const LOCASRun& locasRHS )
{
  
  // Copy constructor.
  // Copy all the run specific information from the 
  // LOCASRun object on the right hand side 'rhs' to 
  // a new instance of a LOCASRun object (this one).
  // However, PMT information is copied separately
  // using either LOCASRun::CopyLOCASPMTInfo or
  // LOCARun::CopySOCPMTInfo.

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

  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fCentralLBTheta = locasRHS.fCentralLBTheta;
  fCentralLBPhi = locasRHS.fCentralLBPhi;

  fWavelengthLBTheta = locasRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = locasRHS.fWavelengthLBPhi;

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

LOCASRun& LOCASRun::operator=( const LOCASRun& locasRHS )
{

  // Copies all the run specific information from the
  // LOCASRun object on the right hand side 'locasRHS'
  // to this one, 'this'. 
  // However, PMT information is copied separately
  // using either LOCASRun::CopyLOCASPMTInfo or
  // LOCARun::CopySOCPMTInfo.

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

  fLBTheta = locasRHS.fLBTheta;
  fLBPhi = locasRHS.fLBPhi;

  fCentralLBTheta = locasRHS.fCentralLBTheta;
  fCentralLBPhi = locasRHS.fCentralLBPhi;

  fWavelengthLBTheta = locasRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = locasRHS.fWavelengthLBPhi;

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::ClearRun()
{ 

  // Set all the private member variables
  // to non-interpretive/physical values.

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

  SetLBTheta( -10.0 );
  SetLBPhi( -10.0 );

  SetCentralLBTheta( -10.0 );
  SetCentralLBPhi( -10.0 );

  SetWavelengthLBTheta( -10.0 );
  SetWavelengthLBPhi( -10.0 );

  if ( !fLOCASPMTs.empty() ){ fLOCASPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::Fill( RAT::DU::SOCReader& socR, 
                     RAT::DU::LightPathCalculator& lLP,
                     RAT::DU::PMTInfo& lDB,
                     UInt_t runID )
{

  // Create a new RAT::DS::SOC object to be used
  // in the loop below.
  RAT::DS::SOC* socPtr = new RAT::DS::SOC;
  
  // First check that a SOC file with the 
  // specified runID exists in the SOCReader.
  for ( Int_t iSOC = 0; iSOC < (Int_t)socR.GetSOCCount(); iSOC++ ){

    // Get the SOC entry.
    *socPtr = socR.GetSOC( iSOC );

    // Check for the run ID on the SOC file.
    if ( socPtr->GetRunID() == runID ){ break; }
    else{ continue; }
    
    // If the SOC object with the required run ID cannot be found
    // then return an error.
    if ( iSOC == ( (Int_t)socR.GetSOCCount() - 1 ) ){
      cout << "LOCASRun::Fill: Error: No SOC file with specified run-ID found" << endl;
      return;
    }
  }

  // Now that a SOC file which matches the run ID specified has been
  // found. We can now begin to fill the LOCASRun object with all the
  // neccessary information.
  
  // The run information from the SOC file...
  CopySOCRunInfo( *socPtr );
  // ... and the PMT information from the SOC file.
  CopySOCPMTInfo( *socPtr );

  // Note: The orientation of the laserball isn't held
  // on the SOC file currently. So will need to implement
  // this in the future.
  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  // Create an iterator to loop over the PMTs...
  map< Int_t, LOCASPMT >::iterator iLP;

  // The PMT ID variable for each PMT in the loop
  Int_t pmtID = 0;

  // Get the wavelength of the laser light in units of MeV.
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

    // Set the PMT positions, normals and types.
    ( iLP->second ).SetPos( lDB.GetPosition( pmtID ) );
    ( iLP->second ).SetNorm( lDB.GetDirection( pmtID ) );
    ( iLP->second ).SetType( lDB.GetType( pmtID ) );

    // Put some of the run specific information on the PMT data structure.
    // This is worth it when it comes to fitting, as only a single reference to
    // the PMT object will be required.
    ( iLP->second ).SetNLBPulses( GetNLBPulses() );
    ( iLP->second ).SetLBIntensityNorm( GetLBIntensityNorm() );

    
    ( iLP->second ).SetMPECorrOccupancy( LOCASMath::MPECorrectedNPrompt( ( iLP->second ).GetOccupancy(), GetNLBPulses() ) );
    ( iLP->second ).SetMPECorrOccupancyErr( LOCASMath::MPECorrectedNPromptErr( ( iLP->second ).GetOccupancy(), GetNLBPulses() ) );
    
    // Calculate the light path for this source position and PMT
    // Require a 10.0 mm precision for the light path to be
    // calculated to the PMT.
    lLP.CalcByPosition( GetLBPos(), GetPMT( iLP->first ).GetPos(), 
                        wavelengthMeV, 10.0 );
    
    // 'feed' the light path to the PMT
    ( iLP->second ).ProcessLightPath( lLP );

    ///////// Off-Axis Laserball Theta and Phi Angles //////////
    TVector3 lbAxis( 0.0, 0.0, 1.0 );
    lbAxis.SetPhi( GetLBPhi() );
    lbAxis.SetTheta( GetLBTheta() );
    
    // Set the relative theta and phi coordinates of the light
    // in the local frame of the laserball as it left the source.
    ( iLP->second ).SetRelLBTheta( ( ( iLP->second ).GetInitialLBVec() ).Angle( lbAxis ) );
    Float_t laserPhi = ( ( iLP->second ).GetInitialLBVec() ).Phi();
    Float_t relLBPhi = fmod( (Float_t)( laserPhi + lbAxis.Phi() ), 2.0 * TMath::Pi() ); 
    ( iLP->second ).SetRelLBPhi( relLBPhi );
  
    // Reset the light path object
    lLP.Clear();
   
    // Verify the values on the PMT.
    ( iLP->second ).VerifyPMT();
    
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopySOCRunInfo( RAT::DS::SOC& socRun )
{

  // Copies all the Run-level information from a SOC file
  // and puts it into (this) LOCASRun object

  // The run ID, source ID and laser wavelength.
  SetRunID( socRun.GetRunID() );
  SetSourceID( socRun.GetSourceID() );
  SetLambda( (Double_t)socRun.GetCalib().GetMode() );

  // The fitted laserball position.
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

  // List of PMT IDs.
  vector< UInt_t > pmtIDs = socRun.GetSOCPMTIDs();

  // Add the PMTs.
  for ( Int_t iPMT = 0; iPMT < (Int_t)pmtIDs.size(); iPMT++ ){
    AddSOCPMT( socRun.GetSOCPMT( pmtIDs[ iPMT ] ) );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopyLOCASRunInfo( LOCASRun& locasRun )
{

  // Make us of the equality operator as defined above.
  *this = locasRun;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CopyLOCASPMTInfo( LOCASRun& locasRun )
{

  // Copies all the LOCASPMTs information from from a
  // different LOCASRun object and copies them into 
  // new LOCASPMT objects in this LOCASRun

  map<Int_t, LOCASPMT>::iterator iLOCASPMT;

  for ( iLOCASPMT = locasRun.GetLOCASPMTIterBegin(); 
        iLOCASPMT != locasRun.GetLOCASPMTIterEnd(); 
        ++iLOCASPMT ){
    AddLOCASPMT( iLOCASPMT->second );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::AddSOCPMT( RAT::DS::SOCPMT& socPMT )
{


  // Add a SOC PMT to the LOCASPMT objects held by this
  // LOCASRun object.
  Int_t pmtID = socPMT.GetLCN();

  // Check that a PMT of the same ID doesn't already exist.
  if( fLOCASPMTs.find( pmtID ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = LOCASPMT( pmtID );
    ( fLOCASPMTs[ pmtID ] ).AddSOCPMTData( socPMT );
  }

  else{
    cout << "PMT Not Added - PMT with same ID already exists" << endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::AddLOCASPMT( LOCASPMT& locasPMT )
{

  // Add a LOCASPMT to the LOCASPMT objects held by this
  // LOCASRun object.
  Int_t pmtID = locasPMT.GetID();

  // Check that a PMT of the same ID doesn't already exist.
  if( fLOCASPMTs.find( pmtID ) == fLOCASPMTs.end() ){
    fLOCASPMTs[ pmtID ] = locasPMT;
  }

  else{
    cout << "PMT Not Added - PMT with same ID already exists, PMT-ID: " << pmtID <<  endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::RemovePMT( Int_t iPMT )
{

  // Remove a LOCAS PMT from the fLOCASPMTs map
  // i.e. the collection of LOCASPMT objects currently held.
  fLOCASPMTs.erase( iPMT );

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASRun::GetPMT( Int_t iPMT )
{

  // Return the LOCASPMT with LCN 'iPMT'
  return fLOCASPMTs[ iPMT ];
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASRun::CrossRunFill( LOCASRun* cRun, LOCASRun* wRun )
{


  // Ensure that the pointers to the central and wavelength
  // runs are not 'NULL', otherwise return an error.
  if ( cRun == NULL && wRun == NULL ){
    cout << "LOCASRun::CrossRunFill: Error: No Central or Wavelength Run Information to fill from" << endl;
    cout << "--------------------------" << endl;
    return;
  }

  // If a pointer to a central run LOCASRun object
  // exist, then fill the 'central' values on this object
  // with the regular values from that one.
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
  
  // If a pointer to a wavelength run LOCASRun object
  // exist, then fill the 'wavelength' values on this object
  // with the regular values from that one.  
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

  // Start filling the PMT information from the central run
  // to all the 'central' values on the LOCASPMTs stored in 
  // this one.
  if ( cRun != NULL ){

    cout << "LOCASRun::CrossRunFill: Filling Central Run Information..." << endl;
    cout << "--------------------------" << endl;
    map< Int_t, LOCASPMT >::iterator iCPMT;
    for( iCPMT = cRun->GetLOCASPMTIterBegin(); iCPMT != cRun->GetLOCASPMTIterEnd(); iCPMT++ ){
      Int_t pmtID = ( iCPMT->first );
      ( fLOCASPMTs[ pmtID ] ).SetCentralRunID( cRun->GetRunID() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralIsVerified( ( iCPMT->second ).GetIsVerified() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralPromptPeakTime( ( iCPMT->second ).GetPromptPeakTime() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralPromptPeakWidth( ( iCPMT->second ).GetPromptPeakWidth() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralTimeOfFlight( ( iCPMT->second ).GetTimeOfFlight() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancy( ( iCPMT->second ).GetOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralOccupancyErr( ( iCPMT->second ).GetOccupancyErr() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralLBIntensityNorm( ( iCPMT->second ).GetLBIntensityNorm() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralNLBPulses( ( iCPMT->second ).GetNLBPulses() );
      
      ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancy( ( iCPMT->second ).GetMPECorrOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetCentralMPECorrOccupancyErr( ( iCPMT->second ).GetMPECorrOccupancyErr() );
      
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

  // Start filling the PMT information from the central run
  // to all the 'central' values on the LOCASPMTs stored in 
  // this one.  
  if ( wRun != NULL ){

    cout << "LOCASRun::CrossRunFill: Filling Wavelength Run Information..." << endl;
    cout << "--------------------------" << endl;
    map< Int_t, LOCASPMT >::iterator iWPMT;
    for( iWPMT = wRun->GetLOCASPMTIterBegin(); iWPMT != wRun->GetLOCASPMTIterEnd(); iWPMT++ ){
      Int_t pmtID = ( iWPMT->first );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthRunID( wRun->GetRunID() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthIsVerified( ( iWPMT->second ).GetIsVerified() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthPromptPeakTime( ( iWPMT->second ).GetPromptPeakTime() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthPromptPeakWidth( ( iWPMT->second ).GetPromptPeakWidth() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthTimeOfFlight( ( iWPMT->second ).GetTimeOfFlight() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancy( ( iWPMT->second ).GetOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthOccupancyErr( ( iWPMT->second ).GetOccupancyErr() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthLBIntensityNorm( ( iWPMT->second ).GetLBIntensityNorm() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthNLBPulses( ( iWPMT->second ).GetNLBPulses() );
      
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancy( ( iWPMT->second ).GetMPECorrOccupancy() );
      ( fLOCASPMTs[ pmtID ] ).SetWavelengthMPECorrOccupancyErr( ( iWPMT->second ).GetMPECorrOccupancyErr() );
      
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
  
  // Initialise the values to hold the 
  // laserball intensity normalisation value for the
  // off-axis, central and wavelength runs.
  map< Int_t, LOCASPMT >::iterator iPMT;
  Float_t lbIntensityNorm = 0.0;
  Float_t centrallbIntensityNorm = 0.0;
  Float_t wavelengthlbIntensityNorm = 0.0;

  Int_t nPMTs = 0;
  Int_t nCentralPMTs = 0;
  Int_t nWavelengthPMTs = 0;

  // Loop through all the PMTs and compute the total number of counts
  // in all the prompt peak timing windows (occupancy) for each PMT
  // in each off-axis, central and wavelength run.

  // Note: A call to LOCASRun::CrossRunFill must have been made
  // if the central and wavelength values are to calculated.
  for ( iPMT = GetLOCASPMTIterBegin(); iPMT != GetLOCASPMTIterEnd(); iPMT++ ){

    // Off-axis run: ensure the DQXX flag is 1.
    if ( ( iPMT->second ).GetDQXXFlag() == 1 ){
      lbIntensityNorm += ( iPMT->second ).GetMPECorrOccupancy();
      nPMTs++;
    }

    // Central run: ensure the DQXX flag is 1.
    if ( ( iPMT->second ).GetCentralDQXXFlag() == 1 ){
      centrallbIntensityNorm += ( iPMT->second ).GetCentralMPECorrOccupancy();
      nCentralPMTs++;
    }

    // Wavelength run: ensure the DQXX flag is 1.
    if ( ( iPMT->second ).GetWavelengthDQXXFlag() == 1 ){
      wavelengthlbIntensityNorm += ( iPMT->second ).GetWavelengthMPECorrOccupancy();
      nWavelengthPMTs++;
    }
  }

  // Provided the total number of PMTs included in each run isn't zero
  // (very unlikely), divide through by the number of PMTs to 
  // calculate the average occupancy (Laserball normalisation).

  // Off-axis runs.
  if ( nPMTs != 0 ){ lbIntensityNorm /= nPMTs; }
  else{ lbIntensityNorm = -10.0; }

  // Central runs.
  if ( nCentralPMTs != 0 ){ centrallbIntensityNorm /= nCentralPMTs; }
  else{ centrallbIntensityNorm = -10.0; }

  // Wavelength runs.
  if ( nWavelengthPMTs != 0 ){ wavelengthlbIntensityNorm /= nWavelengthPMTs; }
  else{ wavelengthlbIntensityNorm = -10.0; }

  // Define the private member variables which
  // hold the intensity normalisation value for each
  // of the off-axis, central and wavelength runs.
  fLBIntensityNorm = lbIntensityNorm;
  fCentralLBIntensityNorm = centrallbIntensityNorm;
  fWavelengthLBIntensityNorm = wavelengthlbIntensityNorm;

  // Also give these values to every PMT for easy access by
  // LOCASOpticsMode::ModelPrediction.
  for ( iPMT = GetLOCASPMTIterBegin(); iPMT != GetLOCASPMTIterEnd(); iPMT++ ){

    ( iPMT->second ).SetLBIntensityNorm( fLBIntensityNorm );
    ( iPMT->second ).SetTotalNRunPromptCounts( fLBIntensityNorm * nPMTs );

    ( iPMT->second ).SetCentralLBIntensityNorm( fCentralLBIntensityNorm );
    ( iPMT->second ).SetWavelengthLBIntensityNorm( fWavelengthLBIntensityNorm );

  }

}
