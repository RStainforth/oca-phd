////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDataPoint.hh
///
/// CLASS: LOCAS::LOCASDataPoint
///
/// BRIEF: Data-level structure for data
///        points. These data-points are fed
///        into the LOCASMethod object alongside a
///        LOCASDataFilter in order to produce a
///        data point set
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDataPoint_
#define _LOCASDataPoint_

#include "LOCASPMT.hh"
#include "LOCASRawDataPoint.hh"
#include <string>

namespace LOCAS{

  class LOCASDataPoint : public TObject
  {
  public:
    LOCASDataPoint(){ };
    LOCASDataPoint( const LOCASRawDataPoint* dataPoint );
    ~LOCASDataPoint(){ };
    
    LOCASDataPoint& operator=( const LOCASDataPoint& rhs );
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Float_t GetOccRatio() const { return fOccRatio; }
    
    Float_t GetDeltaDistInScint() const { return fDeltaDistInScint; }
    Float_t GetDeltaDistInAV() const { return fDeltaDistInAV; }
    Float_t GetDeltaDistInWater() const { return fDeltaDistInWater; }

    Float_t GetSolidAngleRatio() const { return fSolidAngleRatio; }
    Float_t GetFresnelTCoeffRatio() const { return fFresnelTCoeffRatio; }

    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    Float_t GetLBPhi() const { return fLBPhi; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetLBIntensityNormRatio() const { return fLBIntensityNormRatio; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetOccRatio( Float_t val ) { fOccRatio = val; }
    
    void SetDeltaDistInScint( Float_t val ) { fDeltaDistInScint = val; }
    void SetDeltaDistInAV( Float_t val ) { fDeltaDistInAV = val; }
    void SetDeltaDistInWater( Float_t val ) { fDeltaDistInWater = val; }

    void SetSolidAngleRatio( Float_t val ) { fSolidAngleRatio = val; }
    void SetFresnelTCoeffRatio( Float_t val ) { fFresnelTCoeffRatio = val; }

    void SetLBTheta( Float_t val ) { fLBTheta = val; }
    void SetCentralLBTheta( Float_t val ) { fCentralLBTheta = val; }

    void SetLBPhi( Float_t val ) { fLBPhi = val; }
    void SetCentralLBPhi( Float_t val ) { fCentralLBPhi = val; }

    void SetLBIntensityNormRatio( Float_t val ) { fLBIntensityNormRatio = val; }

  private:

    Float_t fOccRatio;

    Float_t fDeltaDistInScint;
    Float_t fDeltaDistInAV;
    Float_t fDeltaDistInWater;

    Float_t fSolidAngleRatio;
    Float_t fFresnelTCoeffRatio;

    Float_t fLBTheta;
    Float_t fCentralLBTheta;

    Float_t fLBPhi;
    Float_t fCentralLBPhi;

    Float_t fLBIntensityNormRatio;

    ClassDef( LOCASDataPoint, 1 );
    
  };
  
}

#endif
