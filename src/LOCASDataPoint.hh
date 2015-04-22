////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataPoint.hh
///
/// CLASS: LOCAS::LOCASDataPoint
///
/// BRIEF: Data-level structure for data
///        points. These data points are used in a 
///        chisquare function which is minimised over.
///        These are the individual data points used in a fit
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: LOCASDataPoints are used by a LOCASChiSquare object 
///         which, combined with a LOCASOptics model to perform a fit
///         to produce the parameters which characterise the optical
///         response of the detector.
///         These are the individual data points used in that fit
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataPoint_
#define _LOCASDataPoint_

#include "LOCASPMT.hh"

#include <string>

namespace LOCAS{

  class LOCASDataPoint : public TObject
  {
  public:

    // The constructors and destructors for the LOCASDataPoint object.
    LOCASDataPoint(){ };
    LOCASDataPoint( const LOCASPMT& lPMT );
    ~LOCASDataPoint(){ };
    
    // Equality operator
    LOCASDataPoint& operator=( const LOCASDataPoint& rhs );
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the run ID for the run from whence the PMT, which this
    // data point represents, came.
    Int_t GetRunID() const { return fRunID; }

    // Get the PMT ID for the PMT from whence this data point came.
    Int_t GetPMTID() const { return fPMTID; }

    // Get the run index. This is set only when the data point becomes
    // part of a set of many data points from accross different runs in
    // a LOCASDataStore object. The index starts at 0.
    // For example, if the data point represents
    // a PMT from the 4th run in a list of runs to be used in a fit, 
    // the index would be [3]. 1st Run [0], 2nd Run [1], etc.
    Int_t GetRunIndex() const{ return fRunIndex; }

    // Get the Multi-Photoelectron (MPE) corrected occupancy from the
    // off-axis run.
    Float_t GetMPECorrOccupancy() const { return fMPECorrOccupancy; }

    // Get the Multi-Photoelectron (MPE) corrected occupancy from the
    // central run.
    Float_t GetCentralMPECorrOccupancy() const { return fCentralMPECorrOccupancy; }

    // Get the error on the Multi-Photoelectron (MPE) corrected occupancy
    // from the off-axis run.
    Float_t GetMPECorrOccupancyErr() const { return fMPECorrOccupancyErr; }

    // Get the error on the Multi-Photoelectron (MPE) corrected occupancy
    // from the central run.
    Float_t GetCentralMPECorrOccupancyErr() const { return fCentralMPECorrOccupancyErr; }
    
    // Get the distance of the light path for this data point in the
    // inner AV region from the off-axis run.
    Float_t GetDistInInnerAV() const { return fDistInInnerAV; }

    // Get the distance of the light path for this data point in the
    // inner AV region from the central run.
    Float_t GetCentralDistInInnerAV() const { return fCentralDistInInnerAV; }

    // Get the distance of the light path for this data point in the
    // AV region from the off-axis run.
    Float_t GetDistInAV() const { return fDistInAV; }

    // Get the distance of the light path for this data point in the
    // AV region from the central run.
    Float_t GetCentralDistInAV() const { return fCentralDistInAV; }

    // Get the distance of the light path for this data point in the
    // water region from the off-axis run.
    Float_t GetDistInWater() const { return fDistInWater; }

    // Get the distance of the light path for this data point in the
    // water region from the central run.
    Float_t GetCentralDistInWater() const { return fCentralDistInWater; }

    // Get the solid angle subtended by the PMT this data point represents
    // from the laserball position in the off-axis run.
    Float_t GetSolidAngle() const { return fSolidAngle; }

    // Get the solid angle subtended by the PMT this data point represents
    // from the laserball position in the central run.
    Float_t GetCentralSolidAngle() const { return fCentralSolidAngle; }

    // Get the Fresnel transmission coefficient for the PMT this 
    // data point represents from the off-axis run.
    Float_t GetFresnelTCoeff() const { return fFresnelTCoeff; }

    // Get the Fresnel transmission coefficient for the PMT this 
    // data point represents from the central run.
    Float_t GetCentralFresnelTCoeff() const { return fCentralFresnelTCoeff; }

    // Get the relative cos-theta value of the lightpath to the laserball
    // axis from the off-axis run.
    Float_t GetLBTheta() const { return fLBTheta; }

    // Get the relative cos-theta value of the lightpath to the laserball
    // axis from the central run.
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    // Get the relative phi coordinate of the lightpath to the laserball
    // axis from the off-axis run.
    Float_t GetLBPhi() const { return fLBPhi; }

    // Get the relative phi coordinate of the lightpath to the laserball
    // axis from the central run.
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    // Get the incident angle (in degrees) to the PMT bucket plane 
    // from the light path from the off-axis run.
    Float_t GetIncidentAngle() const { return fIncidentAngle; }

    // Get the incident angle (in degrees) to the PMT bucket plane 
    // from the light path from the central run.
    Float_t GetCentralIncidentAngle() const { return fCentralIncidentAngle; }

    // Get the sum of all the prompt MPE occupancy values for 
    // the PMTs in the run from whence this data point came 
    // for the off-axis run.
    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }

    // Get the sum of all the prompt MPE occupancy values for 
    // the PMTs in the run from whence this data point came 
    // for the central run.
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }

    // Get the CSS flag for the PMT which this data point represents
    // from the off-axis run (CSS: Channel Software Status (AQXX)).
    Int_t GetCSSFlag() const { return fCSSFlag; }

    // Get the CSS flag for the PMT which this data point represents
    // from the central run (CSS: Channel Software Status (AQXX)).
    Int_t GetCentralCSSFlag() const { return fCentralCSSFlag; }

    // Get the CHS flag for the PMT which this data point represents
    // from the off-axis run (CHS: Channel Hardware Status (DQXX)).
    Int_t GetCHSFlag() const { return fCHSFlag; }

    // Get the CHS flag for the PMT which this data point represents
    // from the central run (CHS: Channel Hardware Status (DQXX)).
    Int_t GetCentralCHSFlag() const { return fCentralCHSFlag; }

    // Get the path flag status for this data point from the
    // off axis run.
    Bool_t GetBadPathFlag() const { return fBadPathFlag; }

    // Get the path flag status for this data point from the
    // central run.
    Bool_t GetCentralBadPathFlag() const { return fCentralBadPathFlag; }

    // Get the total number of prompt counts within the timing window
    // across all the PMTs from which this run came.
    Float_t GetTotalNRunPromptCounts() const { return fTotalNRunPromptCounts; }

    // Get the occupancy ratio (MPE corrected value ratio) for this
    // data-point, ( ratio = off-axis / central ).
    Float_t GetOccupancyRatio() const { return fOccupancyRatio; }

    // Get the error on the occupancy ratio (MPE corrected value ratio) 
    // for this data-point, ( ratio = off-axis / central ).
    Float_t GetOccupancyRatioErr() const { return fOccupancyRatioErr; }

    // Get the model predicted ratio for this
    // data-point, ( ratio = off-axis / central ). The value for this is
    // only initialised once a LOCASChiSquare object has computed the 
    // chi-square value for this data point.
    Float_t GetModelOccupancyRatio() const { return fModelOccupancyRatio; }

    // Get the value fo the efficiency as calculated by 'locas2fit2eff'
    Float_t GetEfficiency() const { return fEfficiency; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    // Set the run ID for the run from whence the PMT, which this
    // data point represents, came.
    void SetRunID( const Int_t val ){ fRunID = val; }

    // Set the PMT ID for the PMT from whence this data point came.
    void SetPMTID( const Int_t val ){ fPMTID = val; }

    // Set the run index. This is set only when the data point becomes
    // part of a set of many data points from accross different runs in
    // a LOCASDataStore object. The index starts at 0.
    // For example, if the data point represents
    // a PMT from the 4th run in a list of runs to be used in a fit, 
    // the index would be [3]. 1st Run [0], 2nd Run [1], etc.
    void SetRunIndex( const Int_t val ){ fRunIndex = val; }

    // Set the Multi-Photoelectron (MPE) corrected occupancy from the
    // off-axis run.
    void SetMPECorrOccupancy( const Float_t val ){ fMPECorrOccupancy = val; }

    // Set the Multi-Photoelectron (MPE) corrected occupancy from the
    // central run.
    void SetCentralMPECorrOccupancy( const Float_t val ){ fCentralMPECorrOccupancy = val; }

    // Set the error on the Multi-Photoelectron (MPE) corrected occupancy
    // from the off-axis run.
    void SetMPECorrOccupancyErr( const Float_t val ){ fMPECorrOccupancyErr = val; }

    // Set the error on the Multi-Photoelectron (MPE) corrected occupancy
    // from the central run.
    void SetCentralMPECorrOccupancyErr( const Float_t val ){ fCentralMPECorrOccupancyErr = val; }
    
    // Set the distance of the light path for this data point in the
    // inner AV region from the off-axis run.
    void SetDistInInnerAV( const Float_t val ){ fDistInInnerAV = val; }

    // Set the distance of the light path for this data point in the
    // inner AV region from the central run.
    void SetCentralDistInInnerAV( const Float_t val ){ fCentralDistInInnerAV = val; }

    // Set the distance of the light path for this data point in the
    // AV region from the off-axis run.
    void SetDistInAV( const Float_t val ){ fDistInAV = val; }

    // Set the distance of the light path for this data point in the
    // AV region from the central run.
    void SetCentralDistInAV( const Float_t val ){ fCentralDistInAV = val; }

    // Set the distance of the light path for this data point in the
    // water region from the off-axis run.
    void SetDistInWater( const Float_t val ){ fDistInWater = val; }

    // Set the distance of the light path for this data point in the
    // water region from the central run.
    void SetCentralDistInWater( const Float_t val ){ fCentralDistInWater = val; }

    // Set the solid angle subtended by the PMT this data point represents
    // from the laserball position in the off-axis run.
    void SetSolidAngle( const Float_t val ){ fSolidAngle = val; }

    // Set the solid angle subtended by the PMT this data point represents
    // from the laserball position in the central run.
    void SetCentralSolidAngle( const Float_t val ){ fCentralSolidAngle = val; }

    // Set the Fresnel transmission coefficient for the PMT this 
    // data point represents from the off-axis run.
    void SetFresnelTCoeff( const Float_t val ){ fFresnelTCoeff = val; }

    // Set the Fresnel transmission coefficient for the PMT this 
    // data point represents from the central run.
    void SetCentralFresnelTCoeff( const Float_t val ){ fCentralFresnelTCoeff = val; }

    // Set the relative cos-theta value of the lightpath to the laserball
    // axis from the off-axis run.
    void SetLBTheta( const Float_t val ){ fLBTheta = val; }

    // Set the relative cos-theta value of the lightpath to the laserball
    // axis from the central run.
    void SetCentralLBTheta( const Float_t val ){ fCentralLBTheta = val; }

    // Set the relative phi coordinate of the lightpath to the laserball
    // axis from the off-axis run.
    void SetLBPhi( const Float_t val ){ fLBPhi = val; }

    // Set the relative phi coordinate of the lightpath to the laserball
    // axis from the central run.
    void SetCentralLBPhi( const Float_t val ){ fCentralLBPhi = val; }

    // Set the incident angle (in degrees) to the PMT bucket plane 
    // from the light path from the off-axis run.
    void SetIncidentAngle( const Float_t val ){ fIncidentAngle = val; }

    // Set the incident angle (in degrees) to the PMT bucket plane 
    // from the light path from the central run.
    void SetCentralIncidentAngle( const Float_t val ){ fCentralIncidentAngle = val; }

    // Set the sum of all the prompt MPE occupancy values for 
    // the PMTs in the run from whence this data point came 
    // for the off-axis run.
    void SetLBIntensityNorm( const Float_t val ){ fLBIntensityNorm = val; }

    // Set the sum of all the prompt MPE occupancy values for 
    // the PMTs in the run from whence this data point came 
    // for the central run.
    void SetCentralLBIntensityNorm( const Float_t val ){ fCentralLBIntensityNorm = val; }

    // Set the CSS flag for the PMT which this data point represents
    // from the off-axis run (CSS: Channel Software Status (AQXX)).
    void SetCSSFlag( const Int_t val ){ fCSSFlag = val; }
    
    // Set the CSS flag for the PMT which this data point represents
    // from the central run (CSS: Channel Software Status (AQXX)).
    void SetCentralCSSFlag( const Int_t val ){ fCentralCSSFlag = val; }

    // Set the CHS flag for the PMT which this data point represents
    // from the off-axis run (CHS: Channel Hardware Status (DQXX)).
    void SetCHSFlag( const Int_t val ){ fCHSFlag = val; }

    // Set the CHS flag for the PMT which this data point represents
    // from the central run (CHS: Channel Hardware Status (DQXX)).
    void SetCentralCHSFlag( const Int_t val ){ fCentralCHSFlag = val; }

    // Set the path flag status for this data point from the
    // off axis run.
    void SetBadPathFlag( const Bool_t val ){ fBadPathFlag = val; }
   
    // Set the path flag status for this data point from the
    // central run.
    void SetCentralBadPathFlag( const Bool_t val ){ fCentralBadPathFlag = val; }

    // Set the total number of prompt counts within the timing window
    // across all the PMTs from which this run came.
    void SetTotalNRunPromptCounts( const Float_t val ) { fTotalNRunPromptCounts = val; }

    // Set the occupancy ratio (MPE corrected value ratio) for this
    // data-point, ( ratio = off-axis / central ).
    void SetOccupancyRatio( const Float_t val ){ fOccupancyRatio = val; }

    // Set the error on the occupancy ratio (MPE corrected value ratio) 
    // for this data-point, ( ratio = off-axis / central ).
    void SetOccupancyRatioErr( const Float_t val ){ fOccupancyRatioErr = val; }

    // Set the model predicted ratio for this
    // data-point, ( ratio = off-axis / central ). The value for this is
    // only initialised once a LOCASChiSquare object has computed the 
    // chi-square value for this data point.
    void SetModelOccupancyRatio( const Float_t val ){ fModelOccupancyRatio = val; }

    // Set the value fo the efficiency as calculated by 'locas2fit2eff'
    void SetEfficiency( const Float_t val ){ fEfficiency = val; }

  private:

    Int_t fRunID;                           // The run ID this datapoint originated from
    Int_t fPMTID;                           // The PMT ID from whence this data point came
    Int_t fRunIndex;                        // The 'run index' is just the index for the order in which the run files
                                            // were loaded into the fit. For example, if this data point represented
                                            // a PMT from the first run loaded into the fit, then the index would be 0.
                                            // If this data point represented a PMT from the second run to be loaded into
                                            // the fit then the index would be 1. etc.

    Float_t fMPECorrOccupancy;              // The MPE corrected occupancy of the off-axis run
    Float_t fCentralMPECorrOccupancy;       // The MPE corrected occupancy from the central run

    Float_t fMPECorrOccupancyErr;           // The error on the MPE corrected occupancy of the off-axis run
    Float_t fCentralMPECorrOccupancyErr;    // The error on the MPE corrected occupancy from the central run

    Float_t fDistInInnerAV;                 // The distance in the inner AV region from the off-axis run
    Float_t fCentralDistInInnerAV;          // The distance in the inner AV region from the central run
    Float_t fDistInAV;                      // The distance in the AV region from the off-axis run
    Float_t fCentralDistInAV;               // The distance in the AV region from the central run
    Float_t fDistInWater;                   // The distance in the water region from the off-axis run
    Float_t fCentralDistInWater;            // The distance in the water region from the central run

    Float_t fSolidAngle;                    // The solid angle subtended from the source positions from the off-axis run
    Float_t fCentralSolidAngle;             // The solid angle subtended from the source positions from the central run

    Float_t fFresnelTCoeff;                 // The fresnel transmission coefficient from the off-axis run
    Float_t fCentralFresnelTCoeff;          // The fresnel transmission coefficient from the central run

    Float_t fLBTheta;                       // The theta value of the light path leaving the source in the source coordinate system
    Float_t fCentralLBTheta;                // The theta value of the light path leaving the source in the source coordinate system from the central run

    Float_t fLBPhi;                         // The phi value of the light path leaving the source in the source coordinate system
    Float_t fCentralLBPhi;                  // The phi value of the light path leaving the source in the source coordinate system from the central run

    Float_t fIncidentAngle;                 // The incident angle of the light path on the PMT in degrees
    Float_t fCentralIncidentAngle;          // The incident angle of the light path on the PMT from the central run in degrees

    Float_t fLBIntensityNorm;               // The run intensity normalisation value for the off-axis run (calculated by LOCASRun::CalculateLBIntensityNorm)
    Float_t fCentralLBIntensityNorm;        // The run intensity normalisation value for the central run (calculated by LOCASRun::CalculateLBIntensityNorm)

    Int_t fCSSFlag;                         // The CSS flag (ANXX) for the off-axis run
    Int_t fCentralCSSFlag;                  // The CSS flag (ANXX) for the central run
    
    Int_t fCHSFlag;                         // The CHS flag (DQXX) for the off-axis run
    Int_t fCentralCHSFlag;                  // The CHS flag (DQXX) for the central run

    Bool_t fBadPathFlag;                    // The path status from the off-axis run (i.e. shadowed, subject to reflections etc. true: bad, false: good)
    Bool_t fCentralBadPathFlag;             // The path status from the central run (i.e. shadowed, subject to reflections etc. true: bad, false: good)

    Float_t fTotalNRunPromptCounts;         // The total number of prompt counts within the timing window across all PMTs from the run this data point came from.

    Float_t fOccupancyRatio;                // The data occupancy ratio for this data point
    Float_t fOccupancyRatioErr;             // The error on the data occupancy ratio for this data point
    Float_t fModelOccupancyRatio;           // The model prediction for the occupancy ratio for this data point

    Float_t fEfficiency;                    // The estimate for the raw efficieny
                                            // of this data point. ( fMPECorrOccupancy / LOCASOpticsModel::ModelPrediction [after minimisation] ).

    ClassDef( LOCASDataPoint, 1 );
    
  };
  
}

#endif
