#include "RAT/DS/SOCPMT.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/FitResult.hh"
#include "RAT/DS/FitVertex.hh"
#include "RAT/DU/SOCReader.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"
#include "RAT/DU/ChanHWStatus.hh"

#include "OCARun.hh"
#include "OCADB.hh"
#include "OCAPMT.hh"
#include "OCAMath.hh"

#include "TMath.h"

#include <map>
#include <iostream>

using namespace std;
using namespace OCA;

ClassImp( OCARun )

//////////////////////////////////////
//////////////////////////////////////

OCARun::OCARun( const OCARun& ocaRHS )
{
  
  // Copy constructor.
  // Copy all the run specific information from the 
  // OCARun object on the right hand side 'rhs' to 
  // a new instance of a OCARun object (this one).
  // However, PMT information is copied separately
  // using either OCARun::CopyOCAPMTInfo or
  // LOCARun::CopySOCPMTInfo.

  fRunID = ocaRHS.fRunID;
  fCentralRunID = ocaRHS.fCentralRunID;
  fWavelengthRunID = ocaRHS.fWavelengthRunID;

  fSourceID = ocaRHS.fSourceID;
  fCentralSourceID = ocaRHS.fCentralSourceID;
  fWavelengthSourceID = ocaRHS.fWavelengthSourceID;

  fIsMainRun = ocaRHS.fIsMainRun;
  fIsCentralRun = ocaRHS.fIsCentralRun;
  fIsWavelengthRun = ocaRHS.fIsWavelengthRun;

  fLambda = ocaRHS.fLambda;
  fNLBPulses = ocaRHS.fNLBPulses;
  fIntensity = ocaRHS.fIntensity;
  fGlobalTimeOffset = ocaRHS.fGlobalTimeOffset;

  fCentralLambda = ocaRHS.fCentralLambda;
  fCentralNLBPulses = ocaRHS.fCentralNLBPulses;
  fCentralIntensity = ocaRHS.fCentralIntensity;
  fCentralGlobalTimeOffset = ocaRHS.fCentralGlobalTimeOffset;

  fWavelengthLambda = ocaRHS.fWavelengthLambda;
  fWavelengthNLBPulses = ocaRHS.fWavelengthNLBPulses;
  fWavelengthIntensity = ocaRHS.fWavelengthIntensity;
  fWavelengthGlobalTimeOffset = ocaRHS.fWavelengthGlobalTimeOffset;

  fLBIntensityNorm = ocaRHS.fLBIntensityNorm;
  fCentralLBIntensityNorm = ocaRHS.fCentralLBIntensityNorm;

  fLBPos = ocaRHS.fLBPos;
  fCentralLBPos = ocaRHS.fCentralLBPos;
  fWavelengthLBPos = ocaRHS.fWavelengthLBPos;

  fLBXPosErr = ocaRHS.fLBXPosErr;
  fLBYPosErr = ocaRHS.fLBYPosErr;
  fLBZPosErr = ocaRHS.fLBZPosErr;

  fCentralLBXPosErr = ocaRHS.fCentralLBXPosErr;
  fCentralLBYPosErr = ocaRHS.fCentralLBYPosErr;
  fCentralLBZPosErr = ocaRHS.fCentralLBZPosErr;

  fWavelengthLBXPosErr = ocaRHS.fWavelengthLBXPosErr;
  fWavelengthLBYPosErr = ocaRHS.fWavelengthLBYPosErr;
  fWavelengthLBZPosErr = ocaRHS.fWavelengthLBZPosErr;

  fLBOrientation = ocaRHS.fLBOrientation;
  fCentralLBOrientation = ocaRHS.fCentralLBOrientation;
  fWavelengthLBOrientation = ocaRHS.fWavelengthLBOrientation;

  fLBTheta = ocaRHS.fLBTheta;
  fLBPhi = ocaRHS.fLBPhi;

  fCentralLBTheta = ocaRHS.fCentralLBTheta;
  fCentralLBPhi = ocaRHS.fCentralLBPhi;

  fWavelengthLBTheta = ocaRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = ocaRHS.fWavelengthLBPhi;

  if ( !fOCAPMTs.empty() ){ fOCAPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

OCARun& OCARun::operator=( const OCARun& ocaRHS )
{

  // Copies all the run specific information from the
  // OCARun object on the right hand side 'ocaRHS'
  // to this one, 'this'. 
  // However, PMT information is copied separately
  // using either OCARun::CopyOCAPMTInfo or
  // LOCARun::CopySOCPMTInfo.

  fRunID = ocaRHS.fRunID;
  fCentralRunID = ocaRHS.fCentralRunID;
  fWavelengthRunID = ocaRHS.fWavelengthRunID;

  fSourceID = ocaRHS.fSourceID;
  fCentralSourceID = ocaRHS.fCentralSourceID;
  fWavelengthSourceID = ocaRHS.fWavelengthSourceID;

  fIsMainRun = ocaRHS.fIsMainRun;
  fIsCentralRun = ocaRHS.fIsCentralRun;
  fIsWavelengthRun = ocaRHS.fIsWavelengthRun;

  fLambda = ocaRHS.fLambda;
  fNLBPulses = ocaRHS.fNLBPulses;
  fIntensity = ocaRHS.fIntensity;
  fGlobalTimeOffset = ocaRHS.fGlobalTimeOffset;

  fCentralLambda = ocaRHS.fCentralLambda;
  fCentralNLBPulses = ocaRHS.fCentralNLBPulses;
  fCentralIntensity = ocaRHS.fCentralIntensity;
  fCentralGlobalTimeOffset = ocaRHS.fCentralGlobalTimeOffset;

  fWavelengthLambda = ocaRHS.fWavelengthLambda;
  fWavelengthNLBPulses = ocaRHS.fWavelengthNLBPulses;
  fWavelengthIntensity = ocaRHS.fWavelengthIntensity;
  fWavelengthGlobalTimeOffset = ocaRHS.fWavelengthGlobalTimeOffset;

  fLBIntensityNorm = ocaRHS.fLBIntensityNorm;
  fCentralLBIntensityNorm = ocaRHS.fCentralLBIntensityNorm;

  fLBPos = ocaRHS.fLBPos;
  fCentralLBPos = ocaRHS.fCentralLBPos;
  fWavelengthLBPos = ocaRHS.fWavelengthLBPos;

  fLBXPosErr = ocaRHS.fLBXPosErr;
  fLBYPosErr = ocaRHS.fLBYPosErr;
  fLBZPosErr = ocaRHS.fLBZPosErr;

  fCentralLBXPosErr = ocaRHS.fCentralLBXPosErr;
  fCentralLBYPosErr = ocaRHS.fCentralLBYPosErr;
  fCentralLBZPosErr = ocaRHS.fCentralLBZPosErr;

  fWavelengthLBXPosErr = ocaRHS.fWavelengthLBXPosErr;
  fWavelengthLBYPosErr = ocaRHS.fWavelengthLBYPosErr;
  fWavelengthLBZPosErr = ocaRHS.fWavelengthLBZPosErr;

  fLBOrientation = ocaRHS.fLBOrientation;
  fCentralLBOrientation = ocaRHS.fCentralLBOrientation;
  fWavelengthLBOrientation = ocaRHS.fWavelengthLBOrientation;

  fLBTheta = ocaRHS.fLBTheta;
  fLBPhi = ocaRHS.fLBPhi;

  fCentralLBTheta = ocaRHS.fCentralLBTheta;
  fCentralLBPhi = ocaRHS.fCentralLBPhi;

  fWavelengthLBTheta = ocaRHS.fWavelengthLBTheta;
  fWavelengthLBPhi = ocaRHS.fWavelengthLBPhi;

  if ( !fOCAPMTs.empty() ){ fOCAPMTs.clear(); }

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::ClearRun()
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
  SetIntensity( -10.0 );
  SetGlobalTimeOffset ( -10.0 );
  SetCentralLambda( -10.0 );
  SetCentralNLBPulses( -10.0 );
  SetCentralIntensity( -10.0 );
  SetCentralGlobalTimeOffset ( -10.0 );
  SetWavelengthLambda( -10.0 );
  SetWavelengthNLBPulses( -10.0 );
  SetWavelengthIntensity( -10.0 );
  SetWavelengthGlobalTimeOffset( -10.0 );

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

  if ( !fOCAPMTs.empty() ){ fOCAPMTs.clear(); }

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::FillRunInfo( RAT::DS::SOC* socPtr,
                          UInt_t runID, Int_t lbPosMode,
                          Bool_t copyPMTInfo )
{

  // Create a new RAT::DS::SOC object to be used
  // in the loop below.
  //RAT::DS::SOC* socPtr = new RAT::DS::SOC;
  
  // First check that a SOC file with the 
  // specified runID exists in the SOCReader.
  // for ( Int_t iSOC = 0; iSOC < (Int_t)socR.GetSOCCount(); iSOC++ ){

  //   // Get the SOC entry.
  //   *socPtr = socR.GetSOC( iSOC );

  //   // Check for the run ID on the SOC file.
  //   if ( socPtr->GetRunID() == runID ){ break; }
  //   else{ continue; }
    
  //   // If the SOC object with the required run ID cannot be found
  //   // then return an error.
  //   if ( iSOC == ( (Int_t)socR.GetSOCCount() - 1 ) ){
  //     cout << "OCARun::Fill: Error: No SOC file with specified run-ID found" << endl;
  //     return;
  //   }
  // }

  // Now that a SOC file which matches the run ID specified has been
  // found. We can now begin to fill the OCARun object with all the
  // neccessary information.

  // The run information from the SOC file...
  CopySOCRunInfo( *socPtr );

  if ( lbPosMode == 1 ){ 
    SetLBPos( socPtr->GetCalib().GetPos() ); 
  }
  if ( lbPosMode == 2 ){ 
    cout << "OCARun::FillRunInfo: Camera Coordinates not currently available, setting to manipulator position\n";
    SetLBPos( socPtr->GetCalib().GetPos() );
  }
  if ( lbPosMode == 3 ){
    // The fitted laserball position.
    RAT::DS::FitResult lbFit = socPtr->GetFitResult( "lbfit" );
    RAT::DS::FitVertex lbVertex = lbFit.GetVertex( 0 );
    
    SetLBPos( lbVertex.GetPosition() );
    SetLBXPosErr( lbVertex.GetPositivePositionError().X() );
    SetLBXPosErr( lbVertex.GetPositivePositionError().Y() );
    SetLBXPosErr( lbVertex.GetPositivePositionError().Z() );
  }

  if ( copyPMTInfo ){
    // ... and the PMT information from the SOC file.
    CopySOCPMTInfo( *socPtr );
  }

  //delete socPtr;

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::FillPMTInfo( RAT::DU::LightPathCalculator& lLP,
                          RAT::DU::ShadowingCalculator& lSC,
                          RAT::DU::ChanHWStatus& lCHS,
                          RAT::DU::PMTInfo& lDB,
                          UInt_t runID )
{

  // Create a new RAT::DS::SOC object to be used
  // in the loop below.
  //RAT::DS::SOC* socPtr = new RAT::DS::SOC;
  
  // // First check that a SOC file with the 
  // // specified runID exists in the SOCReader.
  // for ( Int_t iSOC = 0; iSOC < (Int_t)socR.GetSOCCount(); iSOC++ ){
  //   // Get the SOC entry.
  //   *socPtr = socR.GetSOC( iSOC );

  //   // Check for the run ID on the SOC file.
  //   if ( socPtr->GetRunID() == runID ){ break; }
  //   else{ continue; }
    
  //   // If the SOC object with the required run ID cannot be found
  //   // then return an error.
  //   if ( iSOC == ( (Int_t)socR.GetSOCCount() - 1 ) ){
  //     cout << "OCARun::Fill: Error: No SOC file with specified run-ID found" << endl;
  //     return;
  //   }
  // }
  // Create an iterator to loop over the PMTs...
  map< Int_t, OCAPMT >::iterator iLP;

  // The PMT ID variable for each PMT in the loop
  Int_t pmtID = 0;

  // Get the wavelength of the laser light in units of MeV.
  Double_t wavelengthMeV = lLP.WavelengthToEnergy( GetLambda() * 1.0e-6 );
  
  // Set the PMT positions and normals and then 'feed' the PMT a calculated light path.
  // The PMT 'eats' this light path and calculates it's member variables
  // e.g. distance through scintillator, acrylic and water and various flags based
  // on the quality of the light path.
  for ( iLP = GetOCAPMTIterBegin(); iLP != GetOCAPMTIterEnd(); iLP++ ){
    // Get the PMT ID and OCAPMT object
    pmtID = iLP->first;
    // Set the run ID
    ( iLP->second ).SetRunID( runID );

    // Set the PMT positions, normals and types.
    ( iLP->second ).SetPos( lDB.GetPosition( pmtID ) );
    ( iLP->second ).SetLBPos( GetLBPos() );
    ( iLP->second ).SetLBOrientation( GetLBOrientation() );
    ( iLP->second ).SetNorm( lDB.GetDirection( pmtID ) );
    ( iLP->second ).SetType( lDB.GetType( pmtID ) );

    // Put some of the run specific information on the PMT data structure.
    // This is worth it when it comes to fitting, as only a single reference to
    // the PMT object will be required.
    ( iLP->second ).SetNLBPulses( GetNLBPulses() );
    ( iLP->second ).SetLBIntensityNorm( GetLBIntensityNorm() );

    
    ( iLP->second ).SetMPECorrOccupancy( OCAMath::MPECorrectedNPrompt( ( iLP->second ).GetPromptPeakCounts(), GetNLBPulses() ) );
    ( iLP->second ).SetMPECorrOccupancyErr( OCAMath::MPECorrectedNPromptErr( ( iLP->second ).GetPromptPeakCounts(), GetNLBPulses() ) );
    
    // Calculate the light path for this source position and PMT
    // Require a 10.0 mm precision for the light path to be
    // calculated to the PMT.
    lLP.CalcByPosition( GetLBPos(), GetPMT( iLP->first ).GetPos(), 
                        wavelengthMeV, 10.0 );

    // 'feed' the light path to the PMT
    ( iLP->second ).ProcessLightPath( lLP, lSC, lCHS );

    ///////// Off-Axis Laserball Theta and Phi Angles //////////
    TVector3 lbAxis( 0.0, 0.0, 1.0 );
    //lbAxis.SetPhi( GetLBPhi() );
    //lbAxis.SetTheta( GetLBTheta() );
    
    // Set the relative theta and phi coordinates of the light
    // in the local frame of the laserball as it left the source.
    ( iLP->second ).SetRelLBTheta( ( ( iLP->second ).GetInitialLBVec() ).Angle( lbAxis ) );

    TVector3 vec = ( ( iLP->second ).GetInitialLBVec() );
    vec.Rotate( -1*GetLBPhi(), lbAxis ); 
    ( iLP->second ).SetRelLBPhi( vec.Phi() );
  
    // Reset the light path object
    lLP.Clear();
   
    // Verify the values on the PMT.
    ( iLP->second ).VerifyPMT();
    
  }

  //delete socPtr;

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::CopySOCRunInfo( RAT::DS::SOC& socRun )
{

  // Copies all the Run-level information from a SOC file
  // and puts it into (this) OCARun object

  // The run ID, source ID and laser wavelength.
  SetRunID( socRun.GetRunID() );
  SetSourceID( socRun.GetSourceID() );
  SetLambda( (Double_t)socRun.GetCalib().GetMode() );
  SetIntensity( (Double_t)socRun.GetCalib().GetIntensity() );

  // The number of pulses.
  SetNLBPulses( socRun.GetNPulsesTriggered() );

  SetGlobalTimeOffset( socRun.GetGlobalTimeOffset() );
  
  SetLBTheta( 0.0 );
  SetLBPhi( socRun.GetCalib().GetDir().Phi() );

  SetLBOrientation( GetLBPhi() / ( TMath::Pi() / 2.0 ) );

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::CopySOCPMTInfo( RAT::DS::SOC& socRun )
{

  // Copies all the SOCPMTs information from from a SOC
  // file and copies them into OCAPMT objects

  // List of PMT IDs.
  vector< UInt_t > pmtIDs = socRun.GetSOCPMTIDs();

  // Add the PMTs.
  for ( Int_t iPMT = 0; iPMT < (Int_t)pmtIDs.size(); iPMT++ ){
    AddSOCPMT( socRun.GetSOCPMT( pmtIDs[ iPMT ] ) );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::CopyOCARunInfo( OCARun& ocaRun )
{

  // Make us of the equality operator as defined above.
  *this = ocaRun;

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::CopyOCAPMTInfo( OCARun& ocaRun )
{

  // Copies all the OCAPMTs information from from a
  // different OCARun object and copies them into 
  // new OCAPMT objects in this OCARun

  map<Int_t, OCAPMT>::iterator iOCAPMT;

  for ( iOCAPMT = ocaRun.GetOCAPMTIterBegin(); 
        iOCAPMT != ocaRun.GetOCAPMTIterEnd(); 
        ++iOCAPMT ){
    AddOCAPMT( iOCAPMT->second );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::AddSOCPMT( RAT::DS::SOCPMT& socPMT )
{


  // Add a SOC PMT to the OCAPMT objects held by this
  // OCARun object.
  Int_t pmtID = socPMT.GetLCN();

  // Check that a PMT of the same ID doesn't already exist.
  if( fOCAPMTs.find( pmtID ) == fOCAPMTs.end() ){
    fOCAPMTs[ pmtID ] = OCAPMT( pmtID );
    ( fOCAPMTs[ pmtID ] ).AddSOCPMTData( socPMT );
  }

  else{
    cout << "PMT Not Added - PMT with same ID already exists" << endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::AddOCAPMT( OCAPMT& ocaPMT )
{

  // Add a OCAPMT to the OCAPMT objects held by this
  // OCARun object.
  Int_t pmtID = ocaPMT.GetID();

  // Check that a PMT of the same ID doesn't already exist.
  if( fOCAPMTs.find( pmtID ) == fOCAPMTs.end() ){
    fOCAPMTs[ pmtID ] = ocaPMT;
  }

  else{
    cout << "PMT Not Added - PMT with same ID already exists, PMT-ID: " << pmtID <<  endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::RemovePMT( Int_t iPMT )
{

  // Remove a OCA PMT from the fOCAPMTs map
  // i.e. the collection of OCAPMT objects currently held.
  fOCAPMTs.erase( iPMT );

}

//////////////////////////////////////
//////////////////////////////////////

OCAPMT& OCARun::GetPMT( Int_t iPMT )
{

  // Return the OCAPMT with LCN 'iPMT'
  return fOCAPMTs[ iPMT ];
  
}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::CrossRunFill( OCARun* cRun )
{


  // Ensure that the pointers to the central and wavelength
  // runs are not 'NULL', otherwise return an error.
  if ( cRun == NULL ){
    cout << "OCARun::CrossRunFill: Error: No Central Run Information to fill from" << endl;
    cout << "--------------------------" << endl;
    return;
  }

  // If a pointer to a central run OCARun object
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

  // Start filling the PMT information from the central run
  // to all the 'central' values on the OCAPMTs stored in 
  // this one.
  if ( cRun != NULL ){

    map< Int_t, OCAPMT >::iterator iCPMT;
    for( iCPMT = cRun->GetOCAPMTIterBegin(); iCPMT != cRun->GetOCAPMTIterEnd(); iCPMT++ ){
      Int_t pmtID = ( iCPMT->first );
      ( fOCAPMTs[ pmtID ] ).SetCentralRunID( cRun->GetRunID() );
      ( fOCAPMTs[ pmtID ] ).SetCentralIsVerified( ( iCPMT->second ).GetIsVerified() );

      ( fOCAPMTs[ pmtID ] ).SetCentralLBPos( cRun->GetLBPos() );
      ( fOCAPMTs[ pmtID ] ).SetCentralLBOrientation( cRun->GetLBOrientation() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralPromptPeakTime( ( iCPMT->second ).GetPromptPeakTime() );
      ( fOCAPMTs[ pmtID ] ).SetCentralPromptPeakWidth( ( iCPMT->second ).GetPromptPeakWidth() );
      ( fOCAPMTs[ pmtID ] ).SetCentralPromptPeakCounts( ( iCPMT->second ).GetPromptPeakCounts() );
      ( fOCAPMTs[ pmtID ] ).SetCentralPromptPeakCountsErr( ( iCPMT->second ).GetPromptPeakCountsErr() );

      ( fOCAPMTs[ pmtID ] ).SetCentralTimeOfFlight( ( iCPMT->second ).GetTimeOfFlight() );

      ( fOCAPMTs[ pmtID ] ).SetCentralLBIntensityNorm( ( iCPMT->second ).GetLBIntensityNorm() );   
      ( fOCAPMTs[ pmtID ] ).SetCentralNLBPulses( ( iCPMT->second ).GetNLBPulses() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralMPECorrOccupancy( ( iCPMT->second ).GetMPECorrOccupancy() );
      ( fOCAPMTs[ pmtID ] ).SetCentralMPECorrOccupancyErr( ( iCPMT->second ).GetMPECorrOccupancyErr() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralFresnelTCoeff( ( iCPMT->second ).GetFresnelTCoeff() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralDistInInnerAV( ( iCPMT->second ).GetDistInInnerAV() );
      ( fOCAPMTs[ pmtID ] ).SetCentralDistInAV( ( iCPMT->second ).GetDistInAV() );
      ( fOCAPMTs[ pmtID ] ).SetCentralDistInWater( ( iCPMT->second ).GetDistInWater() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralSolidAngle( ( iCPMT->second ).GetSolidAngle() );
      ( fOCAPMTs[ pmtID ] ).SetCentralCosTheta( ( iCPMT->second ).GetCosTheta() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralRelLBTheta( ( iCPMT->second ).GetRelLBTheta() );
      ( fOCAPMTs[ pmtID ] ).SetCentralRelLBPhi( ( iCPMT->second ).GetRelLBPhi() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralCHSFlag( ( iCPMT->second ).GetCHSFlag() );
      ( fOCAPMTs[ pmtID ] ).SetCentralCSSFlag( ( iCPMT->second ).GetCSSFlag() );
      
      ( fOCAPMTs[ pmtID ] ).SetCentralBadPath( ( iCPMT->second ).GetBadPath() );
      ( fOCAPMTs[ pmtID ] ).SetCentralNeckFlag( ( iCPMT->second ).GetNeckFlag() );

      Float_t occRatio = 0.0;
      Float_t occRatioErr = 0.0;
      OCAMath::CalculateMPEOccRatio( ( fOCAPMTs[ pmtID ] ), occRatio, occRatioErr );
      ( fOCAPMTs[ pmtID ] ).SetOccupancyRatio( occRatio );
      ( fOCAPMTs[ pmtID ] ).SetOccupancyRatioErr( occRatioErr );
      
    }
  }
  // Calculate the number of prompt counts over each PMT for the run
  CalculateLBIntensityNorm();

}

//////////////////////////////////////
//////////////////////////////////////

void OCARun::CalculateLBIntensityNorm()
{
  
  // Initialise the values to hold the 
  // laserball intensity normalisation value for the
  // off-axis, central and wavelength runs.
  map< Int_t, OCAPMT >::iterator iPMT;
  Float_t lbIntensityNorm = 0.0;
  Float_t centrallbIntensityNorm = 0.0;

  Int_t nPMTs = 0;
  Int_t nCentralPMTs = 0;

  // Loop through all the PMTs and compute the total number of counts
  // in all the prompt peak timing windows (occupancy) for each PMT
  // in each off-axis and central run.

  // Note: A call to OCARun::CrossRunFill must have been made
  // if the central values are to calculated.
  for ( iPMT = GetOCAPMTIterBegin(); iPMT != GetOCAPMTIterEnd(); iPMT++ ){

    // Off-axis run: ensure the CHS flag is 1.
    if ( ( iPMT->second ).GetCHSFlag() == 1 
         && ( iPMT->second ).GetMPECorrOccupancy() >= 0.0 ){
      lbIntensityNorm += ( iPMT->second ).GetMPECorrOccupancy();
      nPMTs++;
    }

    // Central run: ensure the CHS flag is 1.
    if ( ( iPMT->second ).GetCentralCHSFlag() == 1
         && ( iPMT->second ).GetCentralMPECorrOccupancy() >= 0.0 ){
      centrallbIntensityNorm += ( iPMT->second ).GetCentralMPECorrOccupancy();   
      nCentralPMTs++;
    }

  }

  // Provided the total number of PMTs included in each run isn't zero
  // (very unlikely), divide through by the number of PMTs to 
  // calculate the average occupancy (Laserball normalisation).

  lbIntensityNorm *= (Float_t)( 10000.0 / nPMTs );
  centrallbIntensityNorm *= (Float_t)( 10000.0 / nCentralPMTs );

  // Define the private member variables which
  // hold the intensity normalisation value for each
  // of the off-axis, central and wavelength runs.
  if ( lbIntensityNorm > 0.0 ){ fLBIntensityNorm = lbIntensityNorm; }
  else{ fLBIntensityNorm = 0.0; }
  if ( centrallbIntensityNorm > 0.0 ){ fCentralLBIntensityNorm = centrallbIntensityNorm; }
  else{ fCentralLBIntensityNorm = 0.0; }

  // Also give these values to every PMT for easy access by
  // OCAOpticsMode::ModelPrediction.
  for ( iPMT = GetOCAPMTIterBegin(); iPMT != GetOCAPMTIterEnd(); iPMT++ ){

    ( iPMT->second ).SetLBIntensityNorm( fLBIntensityNorm );
    ( iPMT->second ).SetCentralLBIntensityNorm( fCentralLBIntensityNorm );

  }

}
