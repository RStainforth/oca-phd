///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASLightPath.hh
///
/// CLASS: LOCAS::LOCASLightPath
///
/// BRIEF: LOCAS implementation of RAT's 
///        LightPath Class
///        (Full description in LOCASLightPath.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file.
///
////////////////////////////////////////////////////////////////////

#include "LOCASLightPath.hh"
#include "LOCASDB.hh"

#include <algorithm>
#include <complex>
#include <cmath>
#include <TVector3.h>
#include <TMath.h>

using namespace LOCAS;
using namespace std;


ClassImp( LOCASLightPath );

LOCASLightPath& LOCASLightPath::operator=( const LOCASLightPath& rhs )
{

  fTIR = rhs.fTIR;
  fResvHit = rhs.fResvHit;
  fXAVNeck = rhs.fXAVNeck;

  fLoopCeiling = rhs.fLoopCeiling;
  fFinalLoopValue = rhs.fFinalLoopValue;

  fFillFraction = rhs.fFillFraction;
  fPathPrecision = rhs.fPathPrecision;
  fLambda = rhs.fLambda;

  fDistInNeck = rhs.fDistInNeck;
  fDistInUpperTarget = rhs.fDistInUpperTarget;
  fDistInLowerTarget = rhs.fDistInLowerTarget;
  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;

  fSolidAngle = rhs.fSolidAngle;
  fCosThetaAvg = rhs.fCosThetaAvg;

  fFresnelTCoeff = rhs.fFresnelTCoeff;
  fFresnelRCoeff = rhs.fFresnelRCoeff;

  fStartPos = rhs.fStartPos;
  fEndPos = rhs.fEndPos;
  fLightPathEndPos = rhs.fLightPathEndPos;

  fIncidentVecOnPMT = rhs.fIncidentVecOnPMT;
  fInitialLightVec = rhs.fInitialLightVec;

  fPointOnAV1st = rhs.fPointOnAV1st;
  fPointOnAV2nd = rhs.fPointOnAV2nd;
  fPointOnAV3rd = rhs.fPointOnAV3rd;
  fPointOnAV4th = rhs.fPointOnAV4th;

  fLightPathType = rhs.fLightPathType;

  fAVInnerRadius = rhs.fAVInnerRadius;
  fAVOuterRadius = rhs.fAVOuterRadius;
  fAVNeckInnerRadius = rhs.fAVNeckInnerRadius;
  fAVNeckOuterRadius = rhs.fAVNeckOuterRadius;
  fPMTRadius = rhs.fPMTRadius;

  fScintRI = rhs.fScintRI;
  fUpperTargetRI = rhs.fUpperTargetRI;
  fLowerTargetRI = rhs.fLowerTargetRI;
  fAVRI = rhs.fAVRI;
  fWaterRI = rhs.fWaterRI;

  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::Initialise()
{

  fTIR = false;
  fResvHit = false;
  fXAVNeck = false;

  fLoopCeiling = -1;
  fFinalLoopValue = -1;

  fFillFraction = -10.0;
  fPathPrecision = -10.0;
  fLambda = -10.0;

  fDistInNeck = -10.0;
  fDistInUpperTarget = -10.0;
  fDistInLowerTarget = -10.0;
  fDistInScint = -10.0;
  fDistInAV = -10.0;
  fDistInWater = -10.0;

  fSolidAngle = -10.0;
  fCosThetaAvg = -10.0;
  fFresnelTCoeff = -10.0;
  fFresnelRCoeff = -10.0;

  fStartPos.SetX( -9999.9 );
  fStartPos.SetY( -9999.9 );
  fStartPos.SetZ( -9999.9 );

  fEndPos.SetX( -9999.9 );
  fEndPos.SetY( -9999.9 );
  fEndPos.SetZ( -9999.9 );

  fLightPathEndPos.SetX( -9999.9 );
  fLightPathEndPos.SetY( -9999.9 );
  fLightPathEndPos.SetZ( -9999.9 );

  fIncidentVecOnPMT.SetX( -9999.9 ); 
  fIncidentVecOnPMT.SetY( -9999.9 );
  fIncidentVecOnPMT.SetZ( -9999.9 );

  fInitialLightVec.SetX( -9999.9 ); 
  fInitialLightVec.SetY( -9999.9 );
  fInitialLightVec.SetZ( -9999.9 );

  fPointOnAV1st.SetX( -9999.9 );
  fPointOnAV1st.SetY( -9999.9 );
  fPointOnAV1st.SetZ( -9999.9 );
  
  fPointOnAV2nd.SetX( -9999.9 );
  fPointOnAV2nd.SetY( -9999.9 );
  fPointOnAV2nd.SetZ( -9999.9 );
  
  fPointOnAV3rd.SetX( -9999.9 );
  fPointOnAV3rd.SetY( -9999.9 );
  fPointOnAV3rd.SetZ( -9999.9 );
  
  fPointOnAV4th.SetX( -9999.9 );
  fPointOnAV4th.SetY( -9999.9 );
  fPointOnAV4th.SetZ( -9999.9 );

  fLightPathType = -1;

  LOCASDB lDB;

  lDB.LoadDetectorGeoParameters();
  
  fAVInnerRadius = lDB.GetAVInnerRadius();
  fAVOuterRadius = lDB.GetAVOuterRadius(); 
  fAVNeckInnerRadius = lDB.GetAVNeckInnerRadius();
  fAVNeckOuterRadius = lDB.GetAVNeckOuterRadius();

  lDB.Clear();
  lDB.LoadRefractiveIndices( "lightwater_sno",
			     "acrylic_sno",
			     "lightwater_sno" ); 
 
  fScintRI = lDB.GetScintRI();
  fLowerTargetRI = fScintRI;
  fLowerTargetRI = fScintRI;
  fAVRI = lDB.GetAVRI();
  fWaterRI = lDB.GetWaterRI();

  lDB.Clear();
  lDB.LoadPMTGeoParameters();

  fPMTRadius = lDB.GetPMTRadius();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::Clear()
{

  fTIR = false;
  fResvHit = false;
  fXAVNeck = false;

  fLoopCeiling = -1;
  fFinalLoopValue = -1;

  fFillFraction = -10.0;
  fPathPrecision = -10.0;
  fLambda = -10.0;

  fDistInNeck = -10.0;
  fDistInUpperTarget = -10.0;
  fDistInLowerTarget = -10.0;
  fDistInScint = -10.0;
  fDistInAV = -10.0;
  fDistInWater = -10.0;

  fSolidAngle = -10.0;
  fCosThetaAvg = -10.0;
  fFresnelTCoeff = -10.0;
  fFresnelRCoeff = -10.0;

  fStartPos.SetX( -9999.9 );
  fStartPos.SetY( -9999.9 );
  fStartPos.SetZ( -9999.9 );

  fEndPos.SetX( -9999.9 );
  fEndPos.SetY( -9999.9 );
  fEndPos.SetZ( -9999.9 );

  fLightPathEndPos.SetX( -9999.9 );
  fLightPathEndPos.SetY( -9999.9 );
  fLightPathEndPos.SetZ( -9999.9 );

  fIncidentVecOnPMT.SetX( -9999.9 ); 
  fIncidentVecOnPMT.SetY( -9999.9 );
  fIncidentVecOnPMT.SetZ( -9999.9 );

  fInitialLightVec.SetX( -9999.9 ); 
  fInitialLightVec.SetY( -9999.9 );
  fInitialLightVec.SetZ( -9999.9 );

  fPointOnAV1st.SetX( -9999.9 );
  fPointOnAV1st.SetY( -9999.9 );
  fPointOnAV1st.SetZ( -9999.9 );
  
  fPointOnAV2nd.SetX( -9999.9 );
  fPointOnAV2nd.SetY( -9999.9 );
  fPointOnAV2nd.SetZ( -9999.9 );
  
  fPointOnAV3rd.SetX( -9999.9 );
  fPointOnAV3rd.SetY( -9999.9 );
  fPointOnAV3rd.SetZ( -9999.9 );
  
  fPointOnAV4th.SetX( -9999.9 );
  fPointOnAV4th.SetY( -9999.9 );
  fPointOnAV4th.SetZ( -9999.9 );

  fLightPathType = -1;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculatePath( const TVector3& eventPos,
                                    const TVector3& pmtPos )
  
{
  Double_t localityVal = 0.0;
  Double_t wavelength = 400.0;
  CalculatePath( eventPos, pmtPos, localityVal, wavelength );
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculatePath( const TVector3& eventPos,
                                    const TVector3& pmtPos,
                                    const Double_t& localityVal )
{
  Double_t wavelength = 400.0;
  CalculatePath( eventPos, pmtPos, localityVal, wavelength );
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculatePathPartial( const TVector3& eventPos,
                                           const TVector3& pmtPos )
{
  Double_t localityVal = 0.0;
  Double_t wavelength = 400.0;
  CalculatePathPartial( eventPos, pmtPos, localityVal, wavelength );
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculatePathPartial( const TVector3& eventPos,
                                           const TVector3& pmtPos,
                                           const Double_t& localityVal )
{
  Double_t wavelength = 400.0;
  CalculatePathPartial( eventPos, pmtPos, localityVal, wavelength );
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculatePath( const TVector3& eventPos,
                                    const TVector3& pmtPos,
                                    const Double_t& localityVal,
                                    const Double_t& lambda )
{

  fStartPos = eventPos;
  fEndPos = pmtPos;
  fPathPrecision = localityVal;
  fLambda = lambda;
  
  // If locality Value is 0.0, use the straight path calculation
  if ( fPathPrecision <= 0.0 ){
    
    fLightPathEndPos = pmtPos ;
    fIncidentVecOnPMT = ( pmtPos - eventPos ).Unit();
    fInitialLightVec = ( pmtPos - eventPos ).Unit();

    CalculateStraightPath( eventPos, pmtPos );

    return;
  }
  
  // Ensure all Bool_tean variables are set to false to begin with
  fTIR = false;
  fResvHit = false;
  fXAVNeck = false;
  
  // The vectors to be used in the calculation. These are the vectors
  // for part of the light paths heading OUT OF the detector  
  TVector3 distScint; // vector through scintillator OUT OF detector
  TVector3 distAV; // vector through AV OUT OF detector
  TVector3 distWater; // vector through water OUT OF detector
  
  // The vectors to be used in the calculation. These are the vectors
  // for part of the light paths heading INTO the detector  
  TVector3 distWaterXDet; // vector through water INTO detector
  TVector3 distAVXDet; // vector through AV INTO detector
  
  // The Vector incident on the PMT  
  TVector3 incidentVecOnPMT;
  
  // Check the size of the loop, if not declared set to 20
  if ( fLoopCeiling <= 0 ){
    fLoopCeiling = 20;
  }
  
  // Check for the path precision - the proximity required to the PMT
  // If not declared, set to 10 mm to within the PMT.
  if ( fPathPrecision <= 0.0 ){
    fPathPrecision = 10.0;
  }
  
  // Begin with the initial light path direction as the straight line direction
  TVector3 initOffset = ( pmtPos - eventPos ).Unit();
  
  // Beginning of algorithm loop
  for ( Int_t i = 0; i < fLoopCeiling; i++ ){  
    
    // Calculate refracted path
    PathCalculation( eventPos, pmtPos, initOffset, 
                     distScint, distAV, distAVXDet, 
                     distWater, distWaterXDet, incidentVecOnPMT, lambda );
    
    // Check for locality or total internal reflection (TIR)
    if( ( LocalityCheck( pmtPos, distWater, i ) ) || fTIR ){
      DefineDistances( eventPos, pmtPos, distScint, 
                       distAV, distAVXDet, distWater, 
                       distWaterXDet, incidentVecOnPMT, i );
      break;
    }
    
    // Readjust initial photon vector to be used in the next iteration
    else
      {
        ReadjustOffset( eventPos, pmtPos, distWater, initOffset );
      }
  }
}

//////////////////////////////////////
//////////////////////////////////////

TVector3 LOCASLightPath::PathRefraction( const TVector3& incidentVec,
					 const TVector3& incidentSurfVec,
					 const Double_t& incRIndex,
					 const Double_t& refRIndex )
{
  const Double_t ratioRI = incRIndex / refRIndex;
  const Double_t cosTheta1 = incidentSurfVec.Dot( -1.0 * incidentVec );
  const Double_t cosTheta2 = TMath::Sqrt( 1 - TMath::Power( ratioRI, 2 ) * ( 1-TMath::Power( cosTheta1, 2 ) ) );
  
  // Check for TIR (= NaN radicand; a negative squareroot)
  if ( std::isnan( cosTheta2 ) ){
    fTIR = true;
  }
  
  // The refracted photon vector
  TVector3 refractedVec;
  
  if ( cosTheta1 >= 0.0 ){
    refractedVec = ratioRI * incidentVec + ( ratioRI * cosTheta1 - cosTheta2 ) * incidentSurfVec;
  }
  else {
    refractedVec = ratioRI * incidentVec - ( ratioRI * cosTheta1 - cosTheta2 ) * incidentSurfVec;
  }
  
  // Ensure the refracted vector is unit normalised
  TVector3 result = refractedVec.Unit();
  return result;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::PathCalculation( const TVector3& eventPos,
				      const TVector3& pmtPos,
				      const TVector3& initOffset,
				      TVector3& distScint,
				      TVector3& distAV,
				      TVector3& distAVXDet,
				      TVector3& distWater,
				      TVector3& distWaterXDet,
				      TVector3& incidentVecOnPMT,
				      const Double_t& lambda )
{
  
  // The refractive indices of the scintillator, av and water
  Double_t scRI = GetScintRI( lambda );
  Double_t avRI = GetAVRI( lambda );
  Double_t waterRI = GetWaterRI( lambda );
  
  // Calculation for events that originate within the scintillator
  if ( eventPos.Mag() < fAVInnerRadius ){
    
    // Calculate the vector from the event position to the inner AV edge
    distScint = VectorToSphereEdge( eventPos, initOffset, fAVInnerRadius, 0 );
    
    // Calculate the refraction between the scintillator and the acrylic
    TVector3 vec1 = PathRefraction( initOffset, ( -1.0 * distScint ).Unit(), scRI, avRI );
    
    // Calculate the vector from the scintillator/av intersection to 
    // the outer AV edge
    distAV = VectorToSphereEdge( distScint, vec1, fAVOuterRadius, 0 );
    
    // Calculate the refraction between the acrylic and the water
    TVector3 vec2 = PathRefraction( vec1, ( -1.0 * distAV ).Unit(), avRI, waterRI );
    
    SetAVNeckInformation( distScint, vec1 );
    incidentVecOnPMT = vec2;
    
    // Calculate the vector from the av/water intersection to the 
    // hypothesised PMT position
    distWater = VectorToSphereEdge( distAV, vec2, pmtPos.Mag(), 0 );
    
  }
  
  // Calculation for events that originate within the AV
  else if ( ( eventPos.Mag() > fAVInnerRadius ) && ( eventPos.Mag() < fAVOuterRadius  ) ){
    
    // First find the distance of closest approach
    Double_t approachAngle = ClosestAngle( eventPos, fAVInnerRadius );
    
    // If event enters the scintillator region
    if ( approachAngle > ( -1 * eventPos ).Angle( pmtPos - eventPos ) ){
      
      // Calculate the vector from the event position to the inner AV edge
      distAVXDet = VectorToSphereEdge( eventPos, initOffset, fAVInnerRadius, 1 );
      
      // Calculate the refraction between the acrylic and the scintillator
      TVector3 vec1 = PathRefraction( initOffset, ( distAVXDet ).Unit(), avRI, scRI );
      
      // Calculate the vector from the inner AV radius to the otherside
      // of the scintillator region
      distScint = VectorToSphereEdge( distAVXDet, vec1, fAVInnerRadius, 0 );    
      
      // Calculate the refraction between the scintillator and the acrylic
      TVector3 vec2 = PathRefraction( vec1, ( -1 * distScint ).Unit(), scRI, avRI );
      
      // Calculate the vector from the scintillator/av intersection to the
      // av/water intersection
      distAV = VectorToSphereEdge( distScint, vec2, fAVOuterRadius, 0 );
      
      // Calculate the refraction between the acrylic and the water
      TVector3 vec3 = PathRefraction( vec2, ( -1 * distAV ).Unit(), avRI, waterRI );
      
      SetAVNeckInformation( distScint, vec2 );
      incidentVecOnPMT = vec3;
      
      // Calculate the vector to the hypothesised PMT position
      distWater = VectorToSphereEdge( distAV, vec3, pmtPos.Mag(), 0 );
      
    }
    
    // Event exits the AV and immediately into the water
    else{
      
      // Calculate the vector from the event position to the outer AV edge
      distAV = VectorToSphereEdge( eventPos, initOffset, fAVOuterRadius, 0 );
      
      // Calculate the refraction between the acrylic and the water
      TVector3 vec1 = PathRefraction( initOffset, ( -1 * distAV ).Unit(), avRI, waterRI );
      
      SetAVNeckInformation( eventPos, initOffset );
      incidentVecOnPMT = vec1;
      
      // Calculate the vector from the outer AV radius to the hypothesised
      // PMT position
      distWater = VectorToSphereEdge( distAV, vec1, pmtPos.Mag(), 0 );        
    }
  }
  
  // Calculation for events that originate outside of the AV (in the water)
  else {
    
    // First find the distance of closest approach
    Double_t approachAngleInner = ClosestAngle( eventPos, fAVInnerRadius );
    Double_t approachAngleOuter = ClosestAngle( eventPos, fAVOuterRadius );
    
    // If the event enters the av AND scintillator region 
    if ( approachAngleInner > ( -1 * eventPos ).Angle( pmtPos - eventPos ) ){ 
      
      // Calculate the vector from the event position to the outer AV edge
      distWaterXDet = VectorToSphereEdge( eventPos, initOffset, fAVOuterRadius, 1 );
      
      // Calculate the refraction between the water and the acrylic
      TVector3 vec1 = PathRefraction( initOffset, ( distWaterXDet ).Unit(), waterRI, avRI );
      
      // Calculate path to the scintillator through the AV region
      distAVXDet = VectorToSphereEdge( distWaterXDet, vec1,  fAVInnerRadius, 1 );
      
      // Calculate the refraction between the acrylic and the scintillator
      TVector3 vec2 = PathRefraction( vec1, ( distAVXDet ).Unit(), avRI, scRI );
      
      // Calculate the vector from the inner AV radius to the otherside
      // of the scintillator region
      distScint = VectorToSphereEdge( distAVXDet, vec2, fAVInnerRadius, 0 );    
      
      // Calculate the refraction between the acrylic and the scintillator
      TVector3 vec3 = PathRefraction( vec2, ( -1 * distScint ).Unit(), avRI, scRI );
      
      // Calculate the vector from the scint/av intersection to the av/water
      // intersection
      distAV = VectorToSphereEdge( distScint, vec3, fAVOuterRadius, 0 );
      
      // Calculate the refrection between the acrylic and the water
      TVector3 vec4 =  PathRefraction( vec3, ( -1 * distAV ).Unit(), avRI, waterRI );
      
      incidentVecOnPMT = vec4;
      
      // Calculate the vector to the hypothesised PMT position
      distWater = VectorToSphereEdge( distAV, vec4, pmtPos.Mag(), 0 );  
      
    }
    
    // If event only enters the AV region only
    else if ( ( approachAngleInner < ( -1 * eventPos ).Angle( pmtPos - eventPos ) )
             &&
             ( approachAngleOuter > ( -1 * eventPos ).Angle( pmtPos - eventPos ) ) ){
      
      // Calculate the vector from the event position to the outer AV edge
      distWaterXDet = VectorToSphereEdge( eventPos, initOffset, fAVOuterRadius, 1 );
      
      // Calculate the refraction between the water and the acrylic
      TVector3 vec1 = PathRefraction( initOffset, ( distWaterXDet ).Unit(), waterRI, avRI );
      
      // Calculate the vector from the outer AV radius to the otherside
      // of the AV region
      distAV = VectorToSphereEdge( distWaterXDet, vec1, fAVOuterRadius, 0 );    
      
      // Calculate the refraction between the acrylic and the water
      TVector3 vec2 = PathRefraction( vec1, ( -1 * distScint ).Unit(), avRI, waterRI );
      
      incidentVecOnPMT = vec2;
      
      // Calculate the vector from the av/water intersection to the
      // hypothesised PMT position
      distWater = VectorToSphereEdge( distAV, vec2, pmtPos.Mag(), 0 ); 
      
    }
    
    // If event does not enter AV or scintillator region.
    else{
      
      distWater = pmtPos;
      incidentVecOnPMT = ( pmtPos - eventPos ).Unit();
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASLightPath::LocalityCheck( const TVector3& pmtPos,
				      const TVector3& hypEndPos,
				      const Int_t i )
  
{
  
  //Check the locality condition
  if ( ( ( hypEndPos - pmtPos ).Mag() < fPathPrecision ) || ( i == fLoopCeiling - 1 ) ){
    
    // Meets the conditions
    
    // If in-precise measurement (only occurs at final loop value i == fLoopCeiling-1)
    if ( ( hypEndPos - pmtPos).Mag() > fPathPrecision ){
      fResvHit = true;
    }    
    return true;
  }
  
  else{
    // Does no meet the conditions
    return false;
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculatePathPartial( const TVector3& eventPos,
					   const TVector3& pmtPos,
					   const Double_t& localityVal,
					   const Double_t& lambda )
{

  fStartPos = eventPos;
  fEndPos = pmtPos;
  fPathPrecision = localityVal;
  fLambda = lambda;
  
  if ( fPathPrecision == 0.0 ){

    fLightPathEndPos = pmtPos ;
    fIncidentVecOnPMT = ( pmtPos - eventPos ).Unit();
    fInitialLightVec = ( pmtPos - eventPos ).Unit();

    CalculateStraightPathPartial();

    return;
  }
  
  // Ensure the TIR variable and Resv variable are both FALSE to begin with
  fTIR = false;
  fResvHit = false;
  fXAVNeck = false;
  
  // Check the size of the loop, if not declared set to 20
  if ( !fLoopCeiling ){
    fLoopCeiling = 20;
  }
  
  // Check for the path precision - the proximity required to the PMT
  // If not declared, set to 10 mm to within the PMT.
  if ( fPathPrecision == 0.0 ){
    fPathPrecision = 10.0;
  }
  
  // Begin with the initial light path direction as the straight line direction
  TVector3 initialDir = ( pmtPos - eventPos ).Unit();
  
  // Beginning of algorithm loop
  for ( Int_t i = 0; i < fLoopCeiling; i++ ){  
    
    // Calculate refracted path
    TVector3 finalPos = PathCalculationPartial( initialDir );
    
    // Check for locality or total internal reflection (TIR)
    if( ( LocalityCheck( pmtPos, finalPos, i ) ) || fTIR ){

      if ( fTIR || fResvHit ){
        CalculateStraightPathPartial();
        fLightPathEndPos = pmtPos;
      }
      
      else{
        fLightPathEndPos = finalPos;
      }

      return;
    }
    
    // Readjust initial photon vector to be used in the next iteration
    else
      {
        ReadjustOffset( eventPos, pmtPos, finalPos, initialDir );
      }
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::DefineDistances( const TVector3& eventPos,
				      const TVector3& pmtPos,
				      const TVector3& distScint,
				      const TVector3& distAV,
				      const TVector3& distAVXDet,
				      const TVector3& distWater,
				      const TVector3& distWaterXDet,
				      const TVector3& incidentVecOnPMT,
				      const Int_t iVal )
{
  
  // For events that originate within the scintillator
  if ( eventPos.Mag() < fAVInnerRadius ){
    
    // IF: total internal reflection is detected, or the calculation
    // was difficult to resolve use straight line path calculation
    if ( fTIR || fResvHit ){

      CalculateStraightPath( eventPos, pmtPos );

      fLightPathEndPos = pmtPos;
      fIncidentVecOnPMT = ( pmtPos - eventPos ).Unit();
      fInitialLightVec = ( pmtPos - eventPos ).Unit();

      fFinalLoopValue = iVal;

      fLightPathType = 0;

    }
    
    // ELSE: Declare refracted path values.
    else{

      fDistInScint = ( distScint - eventPos ).Mag();
      fDistInAV = ( distAV - distScint ).Mag();
      fDistInWater = ( distWater - distAV ).Mag();

      fLightPathEndPos = distWater;
      fIncidentVecOnPMT = incidentVecOnPMT;
      fInitialLightVec = ( distScint - eventPos ).Unit();

      fFinalLoopValue = iVal;

      fPointOnAV1st = distScint;
      fPointOnAV2nd = distAV;

      fLightPathType = 1;
      
    }
  }
  
  // For events that originate within the AV
  else if ( ( eventPos.Mag() > fAVInnerRadius ) && ( eventPos.Mag() < fAVOuterRadius ) ){
    
    if ( fTIR || fResvHit ){

      CalculateStraightPath( eventPos, pmtPos );

      fLightPathEndPos = pmtPos;
      fIncidentVecOnPMT = ( pmtPos - eventPos ).Unit();
      fInitialLightVec = ( pmtPos - eventPos ).Unit();

      fFinalLoopValue = iVal;

      fLightPathType = 0;

    }
    
    else{
      
      Double_t approachAngleInner = ClosestAngle(eventPos, fAVInnerRadius);
      
      if ( approachAngleInner > ( -1.0 * eventPos ).Angle( pmtPos - eventPos ) ){

        fDistInScint = ( distAVXDet - distScint ).Mag();
        fDistInAV = ( eventPos - distAVXDet ).Mag() + ( distAV - distScint ).Mag();
        fDistInWater = ( distWater - distAV ).Mag();

        fFinalLoopValue = iVal;

        fLightPathEndPos = distWater;
        fIncidentVecOnPMT = incidentVecOnPMT;
        fInitialLightVec = ( distAVXDet - eventPos ).Unit();

	fPointOnAV1st = distAVXDet;
	fPointOnAV2nd = distScint;
	fPointOnAV3rd = distAV;

	fLightPathType = 3;

      }
      
      else{

        fDistInScint = 0.0;
        fDistInAV = ( eventPos - distAV ).Mag();
        fDistInWater = ( distWater - distAV ).Mag();

        fFinalLoopValue = iVal;

	fLightPathEndPos = distWater;
        fIncidentVecOnPMT = incidentVecOnPMT;
        fInitialLightVec = ( distAV - eventPos ).Unit();

	fPointOnAV1st = distAV;

	fLightPathType = 2;

      }
    }
  }
  
  // For events that originate outside of the AV
  else {
    if ( GetTIR() || GetResvHit() ){

      CalculateStraightPath( eventPos, pmtPos );

      fFinalLoopValue = iVal;

      fLightPathEndPos = pmtPos;
      fIncidentVecOnPMT = ( pmtPos - eventPos ).Unit();
      fInitialLightVec = ( pmtPos - eventPos ).Unit();

      fLightPathType = 0;
      
    }
    
    
    else{
      Double_t approachAngleInner = ClosestAngle( eventPos, fAVInnerRadius );
      Double_t approachAngleOuter = ClosestAngle( eventPos, fAVOuterRadius );
      
      if ( approachAngleInner > ( -1 * eventPos ).Angle( pmtPos - eventPos ) ){
        
        fDistInScint = ( distAVXDet - distScint ).Mag();
        fDistInAV = ( distWaterXDet - distAVXDet ).Mag() + ( distAV - distScint ).Mag();
        fDistInWater = ( eventPos - distWaterXDet ).Mag() + ( distWater - distAV ).Mag();

        fFinalLoopValue = iVal;

        fLightPathEndPos = distWater;
        fIncidentVecOnPMT = incidentVecOnPMT;
        fInitialLightVec = ( distWaterXDet - eventPos ).Unit();

	fPointOnAV1st = distWaterXDet;
	fPointOnAV2nd = distAVXDet;
	fPointOnAV3rd = distScint;
	fPointOnAV4th = distAV;

	fLightPathType = 4;

      }
      
      else if ( ( approachAngleInner < ( -1.0 * eventPos ).Angle( pmtPos - eventPos ) )
               &&
               ( approachAngleOuter > ( -1.0 * eventPos ).Angle( pmtPos - eventPos ) ) ){
        
        fDistInScint = 0.0;
        fDistInAV = ( distWaterXDet - distAV ).Mag();
        fDistInWater = ( eventPos - distWaterXDet ).Mag() + ( distWater - distAV ).Mag();

        fFinalLoopValue = iVal;

        fLightPathEndPos = distWater;
        fIncidentVecOnPMT = incidentVecOnPMT;
        fInitialLightVec = ( distWaterXDet - eventPos ).Unit();

	fPointOnAV1st = distWaterXDet;
	fPointOnAV2nd = distAV;

	fLightPathType = 5;

	

      }
      
      else{

        fDistInScint = 0.0;
        fDistInAV = 0.0;
        fDistInWater = ( eventPos - pmtPos ).Mag();

        fFinalLoopValue = iVal;

        fLightPathEndPos = distWater;
        fIncidentVecOnPMT = incidentVecOnPMT;
        fInitialLightVec = ( pmtPos - eventPos ).Unit();

	fLightPathType = 6;

      }
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::ReadjustOffset( const TVector3& eventPos,
				     const TVector3& pmtPos,
				     const TVector3& distWater,
				     TVector3& initOffset )
  
{
  
  Double_t scale = eventPos.Mag() / ( 10000.0 );
  
  if ( distWater.Theta() > pmtPos.Theta() ){
    Double_t thetaAdj =  initOffset.Theta() - scale * ( TMath::Abs( distWater.Theta() - pmtPos.Theta() ) );
    initOffset.SetTheta( thetaAdj );
  }
  
  else{
    Double_t thetaAdj =  initOffset.Theta() + scale * ( TMath::Abs( distWater.Theta() - pmtPos.Theta() ) );
    initOffset.SetTheta( thetaAdj );
  }
  
  if ( distWater.Phi() > pmtPos.Phi() ){
    Double_t phiAdj =  initOffset.Phi() - scale * ( TMath::Abs(distWater.Phi() - pmtPos.Phi() ) );
    initOffset.SetPhi( phiAdj );
  }
  
  else{
    Double_t phiAdj =  initOffset.Phi() + scale * ( TMath::Abs( distWater.Phi() - pmtPos.Phi() ) );
    initOffset.SetPhi( phiAdj );
  }
}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASLightPath::ClosestAngle( const TVector3& eventPos,
				       const Double_t& edgeRadius )
{
  Double_t edgeSide;
  edgeSide = TMath::Sqrt( ( TMath::Power( eventPos.Mag(),2 )
                            - TMath::Power( edgeRadius,2 ) ) );
  Double_t angle;
  angle = TMath::ACos( edgeSide / ( eventPos.Mag() ) );
  return angle;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculateStraightPath( const TVector3& eventPos,
					    const TVector3& pmtPos )
{ 
  if( std::isnan( eventPos.Mag() ) )
	{
	  fDistInScint = eventPos.Mag();
	  fDistInAV = eventPos.Mag();
	  fDistInWater = eventPos.Mag();
	  return;
	}

  fDistInScint = CalcDistInSphere( eventPos, pmtPos, fAVInnerRadius );
  fDistInAV = CalcDistInSphere( eventPos, pmtPos, fAVOuterRadius ) - fDistInScint;
  fDistInWater = ( pmtPos - eventPos ).Mag() - fDistInAV - fDistInScint;
}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASLightPath::CalcDistInSphere( const TVector3& eventPos,
					   const TVector3& pmtPos,
					   const Double_t sphereRadius )
{
  const TVector3 vectorToPMT = ( pmtPos - eventPos ).Unit();
  const Double_t xCosTheta = vectorToPMT.Dot( eventPos );
  const Double_t xCosTheta2 = xCosTheta * xCosTheta;
  const Double_t radius2 = sphereRadius * sphereRadius;
  const Double_t discriminant = radius2 + xCosTheta2 - eventPos.Mag2();
  if( discriminant < 0.0 ) // Path does not enter sphere
    return 0.0;
  const Double_t alpha1 = -xCosTheta + TMath::Sqrt( discriminant );
  const Double_t alpha2 = -xCosTheta - TMath::Sqrt( discriminant );
  
  Double_t dist = 0.0;
  
  if( eventPos.Mag2() < radius2 ) // Inside the sphere
    {
      if( alpha1 >= 0.0 )
        dist = alpha1;
      else if( alpha2 >= 0.0 )
        dist = alpha2;
	}
  else // Outside the Sphere
    {
      if( alpha1 < alpha2  && alpha1 >= 0.0 )
        dist = alpha2 - alpha1;
      else if( alpha2 < alpha1  && alpha2 >= 0.0 )
        dist = alpha1 - alpha2;
      else // Path does not enter the sphere (goes the other way)
        dist = 0.0;
    }
  return dist;
}

//////////////////////////////////////
//////////////////////////////////////

TVector3 LOCASLightPath::VectorToSphereEdge( const TVector3& startPos,
					     const TVector3& startDir,
					     const Double_t radiusFromCentre,
					     const Bool_t outside )
{
  
  // The a, b, and c coefficients of a typical quadratic equation
  const Double_t aCoeff = startDir.Mag2();
  const Double_t bCoeff = 2.0 * startPos.Dot( startDir );
  const Double_t cCoeff = startPos.Mag2() - TMath::Power( radiusFromCentre, 2 );
  
  // Parameter (set to 0.0 to begin with)
  Double_t distParam = 0.0;
  
  // Discriminant for quadratic equation
  const Double_t discrim = TMath::Power( bCoeff, 2 ) - 4 * aCoeff * cCoeff;
  
  // Intersection Calculation
  if ( discrim > 0.0 ){
    const Double_t discrimPlus = ( -bCoeff + TMath::Sqrt( discrim ) )/( 2 * aCoeff );
    const Double_t discrimMinus = ( -bCoeff - TMath::Sqrt( discrim ) )/( 2 * aCoeff );
    
    // If path is exiting detector (XDetector = FALSE), or in the scintillator region
    if ( ( discrimPlus > 0.0 && !outside )){
      distParam = discrimPlus;
    }
    
    // If path is entering detector (XDetector = TRUE)
    else {
      distParam = std::min( discrimPlus, discrimMinus );
    }
  }
  
  // Implementaiton of Paramterisation to find intersection point
  TVector3 endPointVec = startPos + distParam * startDir;
  return endPointVec;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::PathThroughTarget( const TVector3& enterPos,
					const TVector3& enterDir,
					TVector3& exitPos,
					TVector3& exitDir)
{
  Double_t fillZ = fAVInnerRadius - 2.0 * fAVInnerRadius * fFillFraction;
  Double_t firstRI, secondRI;
  Double_t avRI = GetAVRI( fLambda );
  // check which it starts in
  if ( enterPos.Z() > fillZ ){
    firstRI = GetUpperTargetRI( fLambda );
    secondRI = GetLowerTargetRI( fLambda );
  }else{
    firstRI = GetLowerTargetRI( fLambda );
    secondRI = GetUpperTargetRI( fLambda );
  }

  Double_t d = ( fillZ - enterPos.Z() ) / enterDir.Z();

  // check if it enters both targets
  if ( enterDir.Z() != 0 && d > 0 && ( pow( d * enterDir.X() + enterPos.X(), 2) + pow( d * enterDir.Y() + enterPos.Y(), 2) <= pow( fAVInnerRadius, 2 ) + pow( fillZ, 2) ) ){
    // calculate the vector from the event position to the lower target
    TVector3 firstTargetPos = d * enterDir + enterPos;
    // calculate the refraction between the upper target and the lower target
    TVector3 boundaryDir;
    if ( enterDir.Z() > 0 )
      boundaryDir = TVector3( 0, 0, -1 );
    else
      boundaryDir = TVector3( 0, 0, 1 );
    TVector3 vec0 = PathRefraction( enterDir, boundaryDir, firstRI, secondRI );
    // calculate the vector from the last point to the inner AV edge
    exitPos = VectorToSphereEdge( firstTargetPos, vec0, fAVInnerRadius, 0 );
    // calculate the refraction between the lower target and the acrylic
    exitDir = PathRefraction( vec0, ( -1.0 * exitPos ).Unit(), secondRI, avRI );
    if ( enterPos.Z() > fillZ ){
      fDistInUpperTarget = d;
      fDistInLowerTarget = ( exitPos - enterPos ).Mag() - d;
    }else{
      fDistInLowerTarget = d;
      fDistInUpperTarget = ( exitPos - enterPos ).Mag() - d;
    }
  }else{
    // Calculate the vector from the event position to the inner AV edge
    exitPos = VectorToSphereEdge( enterPos, enterDir, fAVInnerRadius, 0 );
    // Calculate the refraction between the scintillator and the acrylic
    exitDir = PathRefraction( enterDir,( -1.0 * exitPos ).Unit(), firstRI, avRI );
    if ( enterPos.Z() > fillZ )
      fDistInUpperTarget = ( exitPos - enterPos ).Mag();
    else
      fDistInLowerTarget = ( exitPos - enterPos ).Mag();
  }
}

//////////////////////////////////////
//////////////////////////////////////

TVector3 LOCASLightPath::PathCalculationPartial( const TVector3& initialDir )
{
  TVector3 exitWaterPos;

  // The refractive indices of the scintillator, av and water
  Double_t utRI = GetUpperTargetRI( fLambda );
  Double_t ltRI = GetLowerTargetRI( fLambda );
  Double_t avRI = GetAVRI( fLambda );
  Double_t waterRI = GetWaterRI( fLambda );

  Double_t fillZ = fAVInnerRadius - 2.0 * fAVInnerRadius * fFillFraction;

  // Calculations for events within target volume
  if ( ( fStartPos ).Mag() < fAVInnerRadius ){
    TVector3 exitTargetPos, exitTargetDir;
    PathThroughTarget( fStartPos, initialDir, exitTargetPos, exitTargetDir );
    // calculate the vector from av to outer av
    TVector3 exitAVPos = VectorToSphereEdge( exitTargetPos, exitTargetDir, fAVOuterRadius, 0 );
    fDistInAV = ( exitAVPos - exitTargetPos ).Mag();
    // calculate the refraction between the acrylic and the water
    TVector3 exitAVDir = PathRefraction( exitTargetDir, ( -1.0 * exitAVPos ).Unit(), avRI, waterRI );
    // calculate the vector from the av/water intersection to the hypothesised PMT position
    exitWaterPos = VectorToSphereEdge( exitAVPos, exitAVDir, ( fEndPos ).Mag(), 0);
    fDistInWater = ( exitWaterPos - exitAVPos).Mag();
  }
  // Calculation for events that originate within the AV
  else if ( ( ( fStartPos ).Mag() > fAVInnerRadius ) && ( ( fStartPos ).Mag() < fAVOuterRadius  ) ){

    // First find the distance of closest approach
    Double_t approachAngle = ClosestAngle( fStartPos, fAVInnerRadius );

    // If event enters the scintillator region
    if ( approachAngle > ( -1 * fStartPos ).Angle( fEndPos - fStartPos ) ){

      // Calculate the vector from the event position to the inner AV edge
      TVector3 enterTargetPos = VectorToSphereEdge( fStartPos, initialDir, fAVInnerRadius, 1 );

      Double_t targetRI;
      // Check which target we are intersecting
      if ( enterTargetPos.Z() > fillZ )
        targetRI = utRI;
      else
        targetRI = ltRI;

      // Calculate the refraction between the acrylic and the scintillator
      TVector3 enterTargetDir = PathRefraction( initialDir, ( enterTargetPos ).Unit(), avRI, targetRI );

      // Calculate the vector from the inner AV radius to the otherside
      // of the scintillator region
      TVector3 exitTargetPos, exitTargetDir;
      PathThroughTarget( enterTargetPos, enterTargetDir, exitTargetPos, exitTargetDir );

      // Calculate the vector from the scintillator/av intersection to the
      // av/water intersection
      TVector3 exitAVPos = VectorToSphereEdge( exitTargetPos, exitTargetDir, fAVOuterRadius, 0 );
      fDistInAV = ( enterTargetPos - fStartPos ).Mag() + ( exitAVPos - exitTargetPos ).Mag();

      // Calculate the refraction between the acrylic and the water
      TVector3 exitAVDir = PathRefraction( exitTargetDir, ( -1 * exitAVPos ).Unit(), avRI, waterRI );

      // Calculate the vector to the hypothesised PMT position
      exitWaterPos = VectorToSphereEdge( exitAVPos, exitAVDir, ( fEndPos ).Mag(), 0 );
      fDistInWater = ( exitWaterPos - exitAVPos ).Mag();

    }

    // Event exits the AV and immediately into the water
    else{

      // Calculate the vector from the event position to the outer AV edge
      TVector3 exitAVPos = VectorToSphereEdge( fStartPos, initialDir,  fAVOuterRadius, 0 );
      fDistInAV = ( exitAVPos - fStartPos ).Mag();

      // Calculate the refraction between the acrylic and the water
      TVector3 exitAVDir = PathRefraction( initialDir, ( -1 * exitAVPos ).Unit(), avRI, waterRI );

      // Calculate the vector from the outer AV radius to the hypothesised
      // PMT position
      exitWaterPos = VectorToSphereEdge( exitAVPos, exitAVDir, ( fEndPos.Mag() ), 0 );        
      fDistInWater = ( exitWaterPos - exitAVPos ).Mag();
    }
  }

  // Calculation for events that originate outside of the AV (in the water)
  else {

    // First find the distance of closest approach
    Double_t approachAngleInner = ClosestAngle( fStartPos, fAVInnerRadius );
    Double_t approachAngleOuter = ClosestAngle( fStartPos, fAVOuterRadius );

    // If the event enters the av AND scintillator region 
    if ( approachAngleInner > ( -1 * fStartPos ).Angle( fEndPos - fStartPos ) ){ 

      // Calculate the vector from the event position to the outer AV edge
      TVector3 enterAVPos = VectorToSphereEdge( fStartPos, initialDir, fAVOuterRadius, 1 );

      // Calculate the refraction between the water and the acrylic
      TVector3 enterAVDir = PathRefraction( initialDir, ( enterAVPos ).Unit(), waterRI, avRI );

      // Calculate path to the scintillator through the AV region
      TVector3 enterTargetPos = VectorToSphereEdge( enterAVPos, enterAVDir, fAVInnerRadius, 1 );

      Double_t targetRI;
      // Check which target we are intersecting
      if ( enterTargetPos.Z() > fillZ )
        targetRI = utRI;
      else
        targetRI = ltRI;

      // Calculate the refraction between the acrylic and the scintillator
      TVector3 enterTargetDir = PathRefraction( enterAVDir, ( enterTargetPos ).Unit(), avRI, targetRI );

      // Calculate the vector from the inner AV radius to the otherside
      // of the scintillator region
      TVector3 exitTargetPos,exitTargetDir;
      PathThroughTarget( enterTargetPos, enterTargetDir, exitTargetPos, exitTargetDir );

      // Calculate the vector from the scint/av intersection to the av/water
      // intersection
      TVector3 exitAVPos = VectorToSphereEdge( exitTargetPos, exitTargetDir, fAVOuterRadius, 0 );
      fDistInAV = ( enterTargetPos - enterAVPos ).Mag() + ( exitAVPos - exitTargetPos ).Mag();

      // Calculate the refrection between the acrylic and the water
      TVector3 exitAVDir =  PathRefraction(exitTargetDir, ( -1 * exitAVPos ).Unit(), avRI, waterRI);

      // Calculate the vector to the hypothesised PMT position
      exitWaterPos = VectorToSphereEdge( exitAVPos, exitAVDir, ( fEndPos ).Mag(), 0);  
      fDistInWater = ( enterAVPos - fStartPos ).Mag() + ( exitWaterPos - exitAVPos ).Mag();

    }

    // If event only enters the AV region only
    else if ( ( approachAngleInner < ( -1 * fStartPos ).Angle( fEndPos - fStartPos ) )
        &&
              ( approachAngleOuter > ( -1 * fStartPos ).Angle( fEndPos - fStartPos ) ) ){

      // Calculate the vector from the event position to the outer AV edge
      TVector3 enterAVPos = VectorToSphereEdge( fStartPos, initialDir, fAVOuterRadius, 1 );

      // Calculate the refraction between the water and the acrylic
      TVector3 enterAVDir = PathRefraction( initialDir, ( enterAVPos ).Unit(), waterRI, avRI );

      // Calculate the vector from the outer AV radius to the otherside
      // of the AV region
      TVector3 exitAVPos = VectorToSphereEdge( enterAVPos, enterAVDir, fAVOuterRadius, 0 );    
      fDistInAV = ( exitAVPos - enterAVPos ).Mag();

      // Calculate the refraction between the acrylic and the water
      TVector3 exitAVDir = PathRefraction( enterAVDir, ( -1 * exitAVPos ).Unit(), avRI, waterRI );

      // Calculate the vector from the av/water intersection to the
      // hypothesised PMT position
      exitWaterPos = VectorToSphereEdge( exitAVPos, exitAVDir, ( fEndPos ).Mag(), 0 ); 
    }

    // If event does not enter AV or scintillator region.
    else{
      exitWaterPos = fEndPos;
      fDistInWater = ( fEndPos - fStartPos ).Mag();
    }
  }
  return exitWaterPos;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculateStraightPathPartial()
{
  fDistInUpperTarget = 0.0;
  fDistInLowerTarget = 0.0;
  fDistInAV = 0.0;
  fDistInWater = 0.0;

  if( std::isnan( ( fStartPos ).Mag() ) )
	{
	  fDistInUpperTarget = fDistInLowerTarget = fDistInAV = fDistInWater = ( fStartPos ).Mag();
	  return;
	}
  CalcDistInSpheresPartial( fStartPos, fEndPos );
  fDistInAV = CalcDistInSphere( fStartPos, fEndPos, fAVOuterRadius ) - fDistInUpperTarget - fDistInLowerTarget;
  fDistInWater = ( fEndPos - fStartPos ).Mag() - fDistInAV - fDistInUpperTarget - fDistInLowerTarget;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalcDistInSpheresPartial( const TVector3& startPos,
                                               const TVector3& endPos)
{
  const TVector3 unitDir = ( endPos - startPos ).Unit();
  const Double_t xCosTheta = unitDir.Dot( startPos );
  const Double_t xCosTheta2 = xCosTheta * xCosTheta;
  const Double_t radius2 = fAVInnerRadius * fAVInnerRadius;
  const Double_t discriminant = radius2 + xCosTheta2 - startPos.Mag2();
  if( discriminant < 0.0 ) // Path does not enter sphere
    return;

  TVector3 enterPos;
  if ( startPos.Mag() < fAVInnerRadius )
    enterPos = startPos;
  else
    enterPos = VectorToSphereEdge( startPos, unitDir, fAVInnerRadius, 1 );    

  Double_t fillZ = fAVInnerRadius - 2.0 * fAVInnerRadius * fFillFraction;
  Double_t d = (fillZ - enterPos.Z())/unitDir.Z();


  // check if it enters both targets
  if ( unitDir.Z() != 0 && d > 0 && ( pow( d * unitDir.X() + enterPos.X(), 2) + pow( d * unitDir.Y() + enterPos.Y(), 2 ) <= pow( fAVInnerRadius, 2 ) + pow( fillZ, 2) ) ){
    // calculate the vector from the event position to the lower target
    TVector3 firstTargetPos = d * unitDir + enterPos;
    // calculate the vector from the last point to the inner AV edge
    TVector3 exitPos = VectorToSphereEdge( firstTargetPos, unitDir, fAVInnerRadius, 0 );

    if ( enterPos.Z() > fillZ ){
      fDistInUpperTarget = d;
      fDistInLowerTarget = ( exitPos - enterPos ).Mag() - d;
    }else{
      fDistInLowerTarget = d;
      fDistInUpperTarget = ( exitPos - enterPos ).Mag() - d;
    }
  }else{
    // Calculate the vector from the event position to the inner AV edge
    TVector3 exitPos = VectorToSphereEdge( enterPos, unitDir, fAVInnerRadius, 0 );
    if ( enterPos.Z() > fillZ )
      fDistInUpperTarget = ( exitPos - enterPos ).Mag();
    else
      fDistInLowerTarget = ( exitPos - enterPos ).Mag();
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::SetAVNeckInformation( const TVector3& pointOnAV,
                                           const TVector3& dirVec )
{

  Double_t zNeckBasePos = fAVInnerRadius * TMath::Cos( ( TMath::ASin( fAVNeckOuterRadius / fAVInnerRadius ) ) );
  TVector3 zDir( 0, 0, 1 );

  // Check if event lies within the neck region
  if ( pointOnAV.Z() < zNeckBasePos ){
    fXAVNeck = false;
    fDistInNeck = 0.0;
    return;
  }

  else{
    fXAVNeck = true;
    TVector3 pointOnAVXY( pointOnAV.X(), pointOnAV.Y(), 0 ); 
    TVector3 dirVecXY( dirVec.X(), dirVec.Y(), 0 );
    TVector3 nullVec( 0, 0, 0 );
    

    TVector3 outOfNeckVec = VectorToSphereEdge( pointOnAVXY, dirVecXY, fAVNeckOuterRadius, 0 );
    
    Double_t angleTmp = ( outOfNeckVec.Unit() ).Angle( dirVec );
    
    if ( TMath::Cos( angleTmp ) != 0 ){
      fDistInNeck = ( outOfNeckVec.Mag() )/( TMath::Cos( angleTmp ) );
    }
    
    else{
      fDistInNeck = 6421.0;
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculateSolidAngle( const TVector3& pmtNorm,
					  const Int_t nVal )
{

  if ( nVal != 0 ){
    CalculateSolidAnglePolygon( pmtNorm, nVal );
    return;
  }

  /// Angles across the PMT surface
  //double sinAlpha = 0.0, sinBeta = 0.0;
  const Double_t constPi = TMath::Pi();
  Double_t cosThetaAvg = 0.0;

  TVector3 av1, av2, av3, av4;

  CalculatePath( fStartPos, fEndPos, 10.0, fLambda );
  LOCASLightPath tmpStore = *this;

  TVector3 av = fAVInnerRadius * fInitialLightVec;
  
  TVector3 zPrime = pmtNorm;
  TVector3 yPrime = ( zPrime.Orthogonal() ).Unit();
  TVector3 xPrime = ( yPrime.Cross( zPrime ) ).Unit();

  TVector3 pmtVec1 = fEndPos + fPMTRadius * xPrime;
  TVector3 pmtVec2 = fEndPos - fPMTRadius * xPrime;
  TVector3 pmtVec3 = fEndPos + fPMTRadius * yPrime;
  TVector3 pmtVec4 = fEndPos - fPMTRadius * yPrime;

  if( !fTIR && !fResvHit ){

    CalculatePath( fStartPos, pmtVec1, fPathPrecision, fLambda );
    if ( !fTIR && !fResvHit ) {av1 = fPointOnAV1st; cosThetaAvg += fIncidentVecOnPMT.Dot( -1.0 * pmtNorm.Unit() );}
    else {av1 = av;}

    CalculatePath( fStartPos, pmtVec2, fPathPrecision, fLambda );
    if ( !fTIR && !fResvHit ) {av2 = fPointOnAV1st; cosThetaAvg += fIncidentVecOnPMT.Dot( -1.0 * pmtNorm.Unit() );}
    else {av2 = av;}

    CalculatePath( fStartPos, pmtVec3, fPathPrecision, fLambda );
    if ( !fTIR && !fResvHit ) {av3 = fPointOnAV1st; cosThetaAvg += fIncidentVecOnPMT.Dot( -1.0 * pmtNorm.Unit() );}
    else {av3 = av;}

    CalculatePath( fStartPos, pmtVec4, fPathPrecision, fLambda );
    if ( !fTIR && !fResvHit ) {av4 = fPointOnAV1st; cosThetaAvg += fIncidentVecOnPMT.Dot( -1.0 * pmtNorm.Unit() );}
    else {av4 = av;}

  }

  else{

    av1 = pmtVec1;
    av2 = pmtVec2;
    av3 = pmtVec3;
    av4 = pmtVec4;
    cosThetaAvg = ( pmtVec1.Unit() * zPrime + pmtVec2.Unit() * zPrime +
                    pmtVec3.Unit() * zPrime + pmtVec4.Unit() * zPrime );

  }

  *this = tmpStore;

  fCosThetaAvg = cosThetaAvg;
  fCosThetaAvg /= 4.0;

  double angAlpha = ( ( av1 - fStartPos ).Unit() ).Angle( ( ( av2 - fStartPos ).Unit() ) )/2;
  double angBeta = ( ( av3 - fStartPos ).Unit() ).Angle( ( ( av4 - fStartPos ).Unit() ) )/2;

  fSolidAngle = ( constPi * TMath::Sin( angAlpha ) * TMath::Sin( angBeta ) );


  if ( fSolidAngle < 0 || fSolidAngle > 4 * constPi ){

    std::cout << "LOCAS::LOCASLightPath: Solid Angle is out of range!" << std::endl;
    fSolidAngle = -1.0;

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculateSolidAnglePolygon( const TVector3& pmtNorm,
						 const Int_t nVal )
{

  const Double_t constPi = TMath::Pi();
  Double_t cosThetaAvg = 0.0;

  CalculatePath( fStartPos, fEndPos, fPathPrecision, fLambda );
  TVector3 avMid = fPointOnAV1st;

  LOCASLightPath tmpStore = *this;

  std::vector<TVector3> nVertices;
    
  TVector3 zPrime = pmtNorm;
  TVector3 yPrime = ( zPrime.Orthogonal() ).Unit();
  TVector3 xPrime = ( yPrime.Cross( zPrime ) ).Unit();

  for (int j = 0; j < nVal; j++){

    TVector3 tmpVec = yPrime;
    TVector3 nVec;
    double rotationStep = ( 2 * TMath::Pi() )/ nVal;
    tmpVec.Rotate( j * rotationStep, fEndPos );
    nVec = fEndPos + ( fPMTRadius * tmpVec );
    nVertices.push_back( nVec );

  }

  std::vector<TVector3> nAVPoints;

  if( !fTIR && !fResvHit && ( fDistInScint + fDistInAV > 1.0 ) ){

    for (int k = 0; k < nVal; k++){

      CalculatePath( fStartPos, nVertices[k], fPathPrecision, fLambda );

      if ( !fTIR && !fResvHit ) { nAVPoints.push_back( fPointOnAV1st ); cosThetaAvg += fIncidentVecOnPMT.Dot( -1.0 * pmtNorm );}
      else { nAVPoints.push_back( avMid );}

    }
  }
  
  else{
    for ( int k = 0; k < nVal; k++ ){
      nAVPoints.push_back( nVertices[k] );
    }
  }

  *this = tmpStore;
  fCosThetaAvg = cosThetaAvg;
  fCosThetaAvg /= (Double_t)nVal;

  TVector3 evToAVMid = ( avMid - fStartPos ).Unit();

  for ( int k = 0; k < nVal; k++ ){
    TVector3 vecA, vecB, vecAMid, vecBMid;
    if ( k == ( nVal-1 ) ){
      vecA =  ( nAVPoints[k] - fStartPos ).Unit();
      vecAMid = vecA - evToAVMid;
      vecB =  ( nAVPoints[0] - fStartPos ).Unit();
      vecBMid = vecB - evToAVMid;
    }
    
    else{
      vecA =  ( nAVPoints[k] - fStartPos ).Unit();
      vecAMid = vecA - evToAVMid;
      vecB =  ( nAVPoints[k+1] - fStartPos ).Unit();
      vecBMid = vecB - evToAVMid;
    }
    fSolidAngle += ( 0.5 * ( ( vecBMid.Cross( vecAMid ) ).Mag() ) );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::FresnelTRCoeff( const TVector3& dir,
				     const TVector3& norm,
				     const Double_t n1,
				     const Double_t n2,
				     Double_t& T,
				     Double_t& R )
{

  double cos1 = dir.Dot( norm );
  if ( cos1 < 0.0 ){ cos1 = -cos1; }

  const double sin1_2 = 1.0 - cos1 * cos1;
  const std::complex<double> n12 = n1 / n2;
  const std::complex<double> cos2 = std::sqrt( std::complex<double>( 1.0, 0.0 ) - ( n12 * n12 ) * sin1_2 );

  const std::complex<double> Ds = ( n2 * cos2 + n1 * cos1 );
  const std::complex<double> Dp = ( n2 * cos1 + n1 * cos2 );

  const double Rsc = std::abs( ( n1 * cos1 - n2 * cos2 ) / Ds );
  const double Rpc = std::abs( ( n1 * cos2 - n2 * cos1 ) / Dp );
  const double Rs = Rsc * Rsc;
  const double Rp = Rpc * Rpc;

  const std::complex<double> Nt = std::complex<double>( 4.0, 0.0 ) * n12 * cos1 * cos2;
  const double Ts = std::abs( n2 * n2 * Nt / ( Ds * Ds ) );
  const double Tp = std::abs( n2 * n2 * Nt / ( Dp * Dp ) );

  const double sFraction = 0.5; //GetSPolarisationFraction( norm, dir, pol );
  if ( sFraction < 0.0 || sFraction > 1.0 ){
    std::cout << "LOCASLightPath::CalculateFresnelTRCoeff incorrect polarisation calculates as : " << sFraction << std::endl;
  }

  T = sFraction * Ts + ( 1.0 - sFraction ) * Tp;
  R = sFraction * Rs + ( 1.0 - sFraction ) * Rp;

  if ( T > 1.1 || R < 0.0 ){
    std::cout << "LOCASLightPath::CalculateFrsnelTRCoeff, fFresnelRCoeff is too high " << Rs << " " << Rp << " " << R << std::endl;
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASLightPath::CalculateFresnelTRCoeff()
{

  // Type 0 - Straight Line Path
  if ( fLightPathType == 0 ){
    fFresnelTCoeff = -1.0;
    fFresnelRCoeff = -1.0;
  }

  // Type 1 - Scint -> AV -> Water -> PMT
  if ( fLightPathType == 1 ){
    Double_t T1 = 0.0;
    Double_t R1 = 0.0;

    Double_t T2 = 0.0;
    Double_t R2 = 0.0;
    
    FresnelTRCoeff( GetIncidentVecOn1stSurf(),
		    -1.0 * ( GetPointOnAV1st().Unit() ),
		    GetScintRI( fLambda ),
		    GetAVRI( fLambda ),
		    T1, R1 );

    FresnelTRCoeff( GetIncidentVecOn2ndSurf(),
		    -1.0 * ( GetPointOnAV2nd().Unit() ),
		    GetScintRI( fLambda ),
		    GetWaterRI( fLambda ),
		    T2, R2 );

    fFresnelTCoeff = T1 * T2;
    fFresnelRCoeff = R1 * R2;
  }

  // Type 2 - AV -> Water -> PMT
  if ( fLightPathType == 2 ){
    Double_t T1 = 0.0;
    Double_t R1 = 0.0;
    
    FresnelTRCoeff( GetIncidentVecOn1stSurf(),
		    -1.0 * ( GetPointOnAV1st().Unit() ),
		    GetScintRI( fLambda ),
		    GetAVRI( fLambda ),
		    T1, R1 );

    fFresnelTCoeff = T1;
    fFresnelRCoeff = R1;
  }

  // Type 3 - AV -> Scint -> AV -> Water -> PMT
  if ( fLightPathType == 3 ){
    Double_t T1 = 0.0;
    Double_t R1 = 0.0;

    Double_t T2 = 0.0;
    Double_t R2 = 0.0;

    Double_t T3 = 0.0;
    Double_t R3 = 0.0;
    
    FresnelTRCoeff( GetIncidentVecOn1stSurf(),
		    ( GetPointOnAV1st().Unit() ),
		    GetAVRI( fLambda ),
		    GetScintRI( fLambda ),
		    T1, R1 );

    FresnelTRCoeff( GetIncidentVecOn2ndSurf(),
		    -1.0 * ( GetPointOnAV2nd().Unit() ),
		    GetScintRI( fLambda ),
		    GetAVRI( fLambda ),
		    T2, R2 );

    FresnelTRCoeff( GetIncidentVecOn3rdSurf(),
		    -1.0 * ( GetPointOnAV3rd().Unit() ),
		    GetAVRI( fLambda ),
		    GetWaterRI( fLambda ),
		    T3, R3 );

    fFresnelTCoeff = T1 * T2 * T3;
    fFresnelRCoeff = R1 * R2 * R3;
  }

  // Type 3 - Water -> AV -> Scint -> AV -> Water -> PMT
  if ( fLightPathType == 4 ){
    Double_t T1 = 0.0;
    Double_t R1 = 0.0;

    Double_t T2 = 0.0;
    Double_t R2 = 0.0;

    Double_t T3 = 0.0;
    Double_t R3 = 0.0;

    Double_t T4 = 0.0;
    Double_t R4 = 0.0;

    FresnelTRCoeff( GetIncidentVecOn1stSurf(),
		    ( GetPointOnAV1st().Unit() ),
		    GetWaterRI( fLambda ),
		    GetAVRI( fLambda ),
		    T1, R1 );
    
    FresnelTRCoeff( GetIncidentVecOn2ndSurf(),
		    ( GetPointOnAV2nd().Unit() ),
		    GetAVRI( fLambda ),
		    GetScintRI( fLambda ),
		    T2, R2 );

    FresnelTRCoeff( GetIncidentVecOn3rdSurf(),
		    -1.0 * ( GetPointOnAV3rd().Unit() ),
		    GetScintRI( fLambda ),
		    GetAVRI( fLambda ),
		    T3, R3 );

    FresnelTRCoeff( GetIncidentVecOn4thSurf(),
		    -1.0 * ( GetPointOnAV4th().Unit() ),
		    GetAVRI( fLambda ),
		    GetWaterRI( fLambda ),
		    T4, R4 );

    fFresnelTCoeff = T1 * T2 * T3 * T4;
    fFresnelRCoeff = R1 * R2 * R3 * R4;
  }

  // Type 5 - Water -> AV -> Water -> PMT
  if ( fLightPathType == 5 ){
    Double_t T1 = 0.0;
    Double_t R1 = 0.0;

    Double_t T2 = 0.0;
    Double_t R2 = 0.0;
    
    FresnelTRCoeff( GetIncidentVecOn1stSurf(),
		    ( GetPointOnAV1st().Unit() ),
		    GetWaterRI( fLambda ),
		    GetAVRI( fLambda ),
		    T1, R1 );

    FresnelTRCoeff( GetIncidentVecOn2ndSurf(),
		    -1.0 * ( GetPointOnAV2nd().Unit() ),
		    GetAVRI( fLambda ),
		    GetWaterRI( fLambda ),
		    T2, R2 );

    fFresnelTCoeff = T1 * T2;
    fFresnelRCoeff = R1 * R2;
  }

  // Type 6 - Water -> PMT
  if ( fLightPathType == 6 ){
    fFresnelTCoeff = 1.0;
    fFresnelRCoeff = 0.0;
  }			     

}

				 

