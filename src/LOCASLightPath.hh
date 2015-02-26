////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASLightPath.hh
///
/// CLASS: LOCAS::LOCASLightPath
///
/// BRIEF: LOCAS implementation of RAT's 
///        LightPath Class
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This is a slightly modified version of RAT's
///         LightPath class. The LightPath is treated as
///         an object which is initalised
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASLightPath_
#define _LOCASLightPath_

#include <TObject.h>
#include <TGraph.h>
#include <TVector3.h>


namespace LOCAS{
    
  class LOCASLightPath : public TObject
  {
  public:

    // The constructors
    LOCASLightPath(){ Initialise(); }
    LOCASLightPath( const TVector3& startPos, const TVector3& endPos );
    LOCASLightPath( const TVector3& startPos, const TVector3& endPos, const Double_t localityVal );
    LOCASLightPath( const TVector3& startPos, const TVector3& endPos, const Double_t localityVal, const Double_t lambda );

    // The destructors - nothing to delete
    virtual ~LOCASLightPath(){ }
    
    LOCASLightPath& operator=( const LOCASLightPath & rhs );
    
    // Initialise the variables for the LightPath Class
    // as well as the geometry paramters of the detector 
    // ( e.g. PMT Radius, AV Radius etc. )
    void Initialise();
    
    // Clear all LightPath variables but NOT the 
    // geometry parameters ( there is no need to remove
    // these if the LightPath object is to be used again )
    void Clear();
    
    // CalculatePath prototype 1: Use this to calculate
    // the straight line path of the light from 'eventPos'
    // to the 'pmtPos'.
    void CalculatePath( const TVector3& eventPos,
                        const TVector3& pmtPos );
    
    // CalculatePath prototype 2: Use this to calculate
    // the refracted path of the light. The final argument,
    // 'localityVal' determines how close (in mm) to the 
    // final PMT position you want your path to be calculated to.
    // Defaults to 400.0 nm for the wavelength
    void CalculatePath( const TVector3& eventPos,
                        const TVector3& pmtPos,
                        const Double_t& localityVal );
    
    /// CalculatePath prototype 3: Same as prototype 2, except
    /// the wavelength can be specified.
    void CalculatePath( const TVector3& eventPos,
                        const TVector3& pmtPos,
                        const Double_t& localityVal,
                        const Double_t& lambda );
    
    // Calculate distances in partial 
    // fill for constant wavelength using
    // straight line calculations
    void CalculatePathPartial( const TVector3& eventPos,
                               const TVector3& pmtPos );
    
    // Calculate refracted path distances in 
    // partial fill for constant wavelength
    void CalculatePathPartial( const TVector3& eventPos,
                               const TVector3& pmtPos,
                               const Double_t& localityVal );
    
    // Calculate distances in partial fill
    void CalculatePathPartial( const TVector3& eventPos,
                               const TVector3& pmtPos,
                               const Double_t& localityVal,
                               const Double_t& lambda );
    
    // Calculate the solid angle for this light path.
    // A call to CalculatePath() must be made first before
    // calling this.
    void CalculateSolidAngle( const TVector3& pmtNorm,
                              const Int_t nVal );
    
    void CalculateFresnelTRCoeff();
    
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetTIR( const Bool_t val ){ fTIR = val; }
    void SetResvHit( const Bool_t val ){ fResvHit = val; }
    void SetXAVNeck( const Bool_t val ){ fXAVNeck = val; }
    void SetStraightLine( const Bool_t val ){ fStraightLine = val; }

    void SetLoopCeiling( const Int_t loopCeil ){ fLoopCeiling = loopCeil; }
    void SetFinalLoopValue( const Int_t val ){ fFinalLoopValue = val; }

    void SetFillFraction( const Double_t val ){ fFillFraction = val; }
    void SetPathPrecision( const Double_t val ){ fPathPrecision = val; }
    void SetLambda( const Double_t val ){ fLambda = val; }

    void SetDistInNeck( const Double_t val ){ fDistInNeck = val; }

    void SetDistInUpperTarget( const Double_t val ){ fDistInUpperTarget = val; }
    void SetDistInLowerTarget( const Double_t val ){ fDistInUpperTarget = val; }
    void SetDistInInnerAV( const Double_t val ){ fDistInInnerAV = val; }
    void SetDistInAV( const Double_t val ){ fDistInAV = val; }
    void SetDistInWater( const Double_t val ){ fDistInWater = val; }

    void SetSolidAngle( const Double_t val ){ fSolidAngle = val; }
    void SetCosThetaAvg( const Double_t val ){ fCosThetaAvg = val; }

    void SetFresnelTCoeff( const Double_t val ){ fFresnelTCoeff = val; }
    void SetFresnelRCoeff( const Double_t val ){ fFresnelRCoeff = val; }

    void SetStartPos( const TVector3& startPos ){ fStartPos = startPos; }
    void SetEndPos( const TVector3& endPos ){ fEndPos = endPos; }
    void SetLightPathEndPos( const TVector3& lpEndPos ){ fLightPathEndPos = lpEndPos; }

    void SetIncidentVecOnPMT( const TVector3& val ){ fIncidentVecOnPMT = val; }
    void SetInitialLightVec( const TVector3& val ){ fInitialLightVec = val; }

    void SetPointOnAV1st( const TVector3& val ){ fPointOnAV1st = val; }
    void SetPointOnAV2nd( const TVector3& val ){ fPointOnAV2nd = val; }
    void SetPointOnAV3rd( const TVector3& val ){ fPointOnAV3rd = val; }
    void SetPointOnAV4th( const TVector3& val ){ fPointOnAV4th = val; }

    void SetLightPathType( const Int_t val ){ fLightPathType = val; }
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Bool_t GetTIR() const { return fTIR; }  
    Bool_t GetResvHit() const { return fResvHit; }
    Bool_t GetXAVNeck() const { return fXAVNeck; }
    Bool_t GetStraightLine() const { return fStraightLine; }
    
    Double_t GetLoopCeiling() const { return fLoopCeiling; }  
    Double_t GetFinalLoopValue() const { return fFinalLoopValue; }
    
    Double_t GetFillFraction() const { return fFillFraction; }
    Double_t GetPathPrecision() const { return fPathPrecision; }
    Double_t GetLambda() const { return fLambda; }
    
    Double_t GetDistInNeck() const { return fDistInNeck; }
    Double_t GetDistInUpperTarget() const { return fDistInUpperTarget; }
    Double_t GetDistInLowerTarget() const { return fDistInLowerTarget; }
    Double_t GetDistInInnerAV() const { return fDistInInnerAV; }
    Double_t GetDistInAV() const { return fDistInAV; }
    Double_t GetDistInWater() const { return fDistInWater; }
    
    Double_t GetTotalDist() const { return fDistInInnerAV + fDistInAV + fDistInWater; }
    Double_t GetTotalDistPartial() const { return fDistInUpperTarget + fDistInLowerTarget + fDistInAV + fDistInWater; }
    
    // A call to 'CalculateSolidAngle' must first be made before using this 'getter'
    Double_t GetSolidAngle() const { return fSolidAngle; }
    Double_t GetCosThetaAvg() const { return fCosThetaAvg; }
    Double_t GetFresnelTCoeff() const { return fFresnelTCoeff; }
    Double_t GetFresnelRCoeff() const { return fFresnelRCoeff; }
    
    TVector3 GetStartPos() const { return fStartPos; }
    TVector3 GetEndPos() const { return fEndPos; }
    TVector3 GetLightPathEndPos() const { return fLightPathEndPos; }
    TVector3 GetIncidentVecOnPMT() const { return fIncidentVecOnPMT; }
    TVector3 GetInitialLightVec() const { return fInitialLightVec; }

    TVector3 GetPointOnAV1st() const { return fPointOnAV1st; }
    TVector3 GetPointOnAV2nd() const { return fPointOnAV2nd; }
    TVector3 GetPointOnAV3rd() const { return fPointOnAV3rd; }
    TVector3 GetPointOnAV4th() const { return fPointOnAV4th; }
    
    Int_t GetLightPathType() const { return fLightPathType; }
    
    // Returns the incident vectors on each of the intersections the path makes
    // throughout the detector. 
    TVector3 GetIncidentVecOn1stSurf() const { return ( fPointOnAV1st - fStartPos ).Unit(); }
    TVector3 GetIncidentVecOn2ndSurf() const { return ( fPointOnAV2nd - fPointOnAV1st ).Unit(); }
    TVector3 GetIncidentVecOn3rdSurf() const { return ( fPointOnAV3rd - fPointOnAV2nd ).Unit(); }
    TVector3 GetIncidentVecOn4thSurf() const { return ( fPointOnAV4th - fPointOnAV3rd ).Unit(); }
    
    Double_t GetAVInnerRadius() const { return fAVInnerRadius; }
    Double_t GetAVOuterRadius() const { return fAVOuterRadius; }
    Double_t GetAVNeckInnerRadius() const { return fAVNeckInnerRadius; }
    Double_t GetAVNeckOuterRadius() const { return fAVNeckOuterRadius; }
    Double_t GetPMTRadius() const { return fPMTRadius; }
    
    // Return the refractive indices, argument 'lambda*' refers to the wavelength [nm]
    // The number '0.001243125' is to convert the wavelengths into energies [MeV] (?)
    Double_t GetScintRI( const Double_t lambdaSc ){ return fScintRI.Eval( lambdaSc ); }
    Double_t GetAVRI( const Double_t lambdaAV ){ return fAVRI.Eval( lambdaAV ); }
    Double_t GetWaterRI( const Double_t lambdaH20 ){ return fWaterRI.Eval( lambdaH20 ); }
    Double_t GetUpperTargetRI( const Double_t lambdaSc ){ return fUpperTargetRI.Eval( lambdaSc ); }
    Double_t GetLowerTargetRI( const Double_t lambdaSc ){ return fLowerTargetRI.Eval( lambdaSc ); }
    
    
  private:
    
    // Calculates the distance in a given sphere, used for straight line
    // path calculations only
    Double_t CalcDistInSphere( const TVector3& eventPos,
                               const TVector3& pmtPos,
                               const Double_t sphereRadius );
    
    // The straight line path calculation used internally by 'CalculateStraightPath'
    // prototype 1.
    void CalculateStraightPath( const TVector3& eventPos,
                                const TVector3& pmtPos );
    
    // Calculate the refracted path. Performs most of the work required to
    // obtained a refracted path
    void PathCalculation( const TVector3& eventPos,
                          const TVector3& pmtPos,
                          const TVector3& initOffset,
                          TVector3& distScint,
                          TVector3& distAV,
                          TVector3& distAVXDet,
                          TVector3& distWater,
                          TVector3& distWaterXDet,
                          TVector3& incidentVecOnPMT,
                          const Double_t& lambda );  
    
    // Defines the distances through the scintillator, AV and water based
    // on a refracted path which met the locality conditions
    void DefineDistances( const TVector3& eventPos,
                          const TVector3& pmtPos,
                          const TVector3& distScint,
                          const TVector3& distAV,
                          const TVector3& distAVXdet,
                          const TVector3& distWater,
                          const TVector3& distWaterXDet,
                          const TVector3& incidentVecOnPMT,
                          const Int_t iVal );
    
    // Readjust the inital photon direction. Used if the previous path
    // does not meet the locality conditions
    void ReadjustOffset( const TVector3& eventPos,
                         const TVector3& pmtPos,
                         const TVector3& distWater,
                         TVector3& initOffset );
    
    // Test the locality conditions for the hypothesised path end point ( hypEndPos )
    // against the actual PMT position ( pmtPos )
    Bool_t LocalityCheck( const TVector3& pmtPos,
                          const TVector3& hypEndPos,
                          const Int_t i );
    
    // Calculate the closest angular displacement of a path
    // close to a surface interface
    Double_t ClosestAngle( const TVector3& eventPos,
                           const Double_t& edgeRadius );
    
    
    // Calculate refracted vector direction for a light path
    // incident ( incidentVec - Unit Normalised ) on a surface
    // with unit normal 'incidentSurfVec' from within an incident
    // refractive medium with index 'incRIndex' to another
    // with index 'refRIndex'
    TVector3 PathRefraction( const TVector3& incidentVec,
                             const TVector3& incidentSurfVec,
                             const Double_t& incRIndex,
                             const Double_t& refRIndex );
    
    // Calculate vector from some initial point ('startPos'), with an initial
    // starting direction, 'startDir' to the edge of a sphere of given radius
    // 'radiusFromCentre'. The Boolean ( Bool_t outside ) is passed based on if the
    // path originates outside of the AV or not.
    TVector3 VectorToSphereEdge( const TVector3& startPos,
                                 const TVector3& startDir,
                                 const Double_t radiusFromCentre,
                                 const Bool_t outside);
    
    // Calculated the path through either the lower/upper target
    // [Used for Partial Fill]
    void PathThroughTarget( const TVector3& enterPos,
                            const TVector3& enterDir,
                            TVector3& exitPos,
                            TVector3& exitDir );
    
    // Calculate the refracted path. Performs most of the work required to
    // obtained a refracted path [Used for Partial Fill]
    TVector3 PathCalculationPartial( const TVector3& initialDir );
    
    // The straight line path calculation used internally by 'CalculatePathPartial'
    // prototype 1. [Used for Partial Fill]
    void CalculateStraightPathPartial();
    
    // Calculates the distance in a given partially filled sphere, used for
    // straight line path calculations only [Used for Partial Fill]
    void CalcDistInSpheresPartial( const TVector3& startPos,
                                   const TVector3& endPos);
    
    // Set the AV neck variables (i.e. those pertaining to whether the
    // path entered the neck or not, and if it did, the distance it travelled ).
    // This is called for all light paths, which, at some point, leave the AV region
    void SetAVNeckInformation( const TVector3& pointOnAV,
                               const TVector3& dirVec );
    
    // Calculates the solid angle using a more rigourous method.
    // This is reserved for calculating the solid angle for locations
    // close to the AV (i.e. near AV region, large R ( e.g. R > 5000 mm )
    void CalculateSolidAnglePolygon( const TVector3& pmtNorm,
                                     const Int_t nVal );
    
    // Calculates the fresnel transmission 
    void FresnelTRCoeff( const TVector3& dir,
                         const TVector3& norm,
                         const Double_t n1,
                         const Double_t n2,
                         Double_t& T,
                         Double_t& R );
    
    
    Bool_t fTIR;                          // TRUE: Total Internal Reflection Occured ( straight line path calculated instead )
                                          // FALSE: Reflection did not occur and refracted path distances were calculated
    
    Bool_t fResvHit;                      // TRUE: Calculated Path finished over 1000 mm away from required PMT position [fEndPos]
                                          // FALSE: It didn't
    
    Bool_t fXAVNeck;                      // TRUE: Light path entered the AV Neck region
                                          // FALSE: It didn't

    Bool_t fStraightLine;                 // TRUE: Calculated Path is a straight line
                                          // FALSE: Calculated Path is a refracted path
    
    Int_t fLoopCeiling;                   // Iteration Ceiling for algortithm loop
    Int_t fFinalLoopValue;                // Final loop value which meets locality conditions
    
    Double_t fFillFraction;               // The fill fraction of the scintillator volume [Used for Partial Fill]
    Double_t fPathPrecision;              // The accepted path proximity to the PMT location
    Double_t fLambda;                     // The wavelength of the light path
    
    Double_t fDistInNeck;                 // Distance of the light path through the neck region [Scintillator + Acrylic distance]
                                          // Only if the light path entered the neck though ( i.e. fXAVNeck = TRUE )
    
    Double_t fDistInUpperTarget;          // Distance of the light path in the upper targer [Used for Partial Fill]
    Double_t fDistInLowerTarget;          // Distance of the light path in the lower target [Used for Partial Fill]
    Double_t fDistInInnerAV;                // Distance of the light path in the scintillator region
    Double_t fDistInAV;                   // Distance of the light path in the AV region
    Double_t fDistInWater;                // Distance of the light path in the water region
    
    Double_t fSolidAngle;                 // The solid angle subtended by the PMT for this light path
    Double_t fCosThetaAvg;                // Average incident angle on the PMT for this path.
                                          // This is only calculated after a call to CalculateSolidAngle
    
    Double_t fFresnelTCoeff;              // The combined Fresnel TRANSMISSION coefficent for this path
    Double_t fFresnelRCoeff;              // The combined Fresnel REFLECTIVITY coefficient for this path
    
    TVector3 fStartPos;                   // Start position of the light path
    TVector3 fEndPos;                     // Required end position of the light path
    TVector3 fLightPathEndPos;            // Calculated end position of the light path
    
    TVector3 fIncidentVecOnPMT;           // Final light path direction [unit normalised]  
    TVector3 fInitialLightVec;            // Initial light path direction [unit normalised]
    
    // Note: Depending on the light path type (see fLightPathType), the path may
    // intersect the AV once, twice, three (times a lady?)
    // ...or four times
    TVector3 fPointOnAV1st;               // Point on AV where light path first hits the AV
    TVector3 fPointOnAV2nd;               // Point on AV where light path hits the AV a second time
    TVector3 fPointOnAV3rd;               // Point on AV where light path hits the AV a third time
    TVector3 fPointOnAV4th;               // Point on AV where light path hits the AV a fourth time
    
    
    // Light Path types
    // Type 1 - Scint -> AV -> Water -> PMT
    // Type 2 - AV -> Water -> PMT
    // Type 3 - AV -> Scint -> AV -> Water -> PMT
    // Type 4 - Water -> AV -> Scint -> AV -> Water -> PMT
    // Type 5 - Water -> AV -> Water -> PMT
    // Type 6 - Water -> PMT
    Int_t fLightPathType;
    
    Double_t fAVInnerRadius;              // Inner Radius of the AV
    Double_t fAVOuterRadius;              // Outer Radius of the AV
    Double_t fAVNeckInnerRadius;          // Inner Radius of the AV Neck
    Double_t fAVNeckOuterRadius;          // Outer Radius fo the AV Neck
    Double_t fPMTRadius;                  // Radius of the PMT Face (Grey Disc Model)
    
    TGraph fScintRI;                      // Scintillator Refractive Indices [Function of Energy [MeV]]
    TGraph fAVRI;                         // AV Refractive Indices [Function of Energy [MeV]]
    TGraph fWaterRI;                      // Water Refractive Indices [Function of Energy [MeV]]
    TGraph fUpperTargetRI;                // Upper Target Refractive Indices [Used for Partial Fill][Function of Energy [MeV]]
    TGraph fLowerTargetRI;                // Lower Target Refractive Indices [Used for Partial Fill][Function of Energy [MeV]]
    
    ClassDef( LOCASLightPath, 1 );

  };
}

#endif
