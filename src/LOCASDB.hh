////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDB.hh
///
/// CLASS: LOCAS::LOCASDB
///
/// BRIEF: Class used to load data from the RAT or LOCAS database
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: This class loads information from the RAT
///         or LOCAS database. For instance, for all LOCASRun
///         objects, the PMT positions, types and normals are all
///         loaded using this class from the RAT database.
///
///         In addition, this class can also load specific geometry
///         parameters (e.g. the AV Inner/Outer Radius) or
///         refractive indices. These in particualr are used by 
///         the LOCASLightPath class.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDB_
#define _LOCASDB_

#include "TVector3.h"
#include "TGraph.h"

#include <map>
#include <string>

#include "RAT/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/DB.hh"


namespace LOCAS{

  class LOCASDB : public TObject
  {
  public:
    LOCASDB();
    virtual ~LOCASDB(){ }
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    void Initialise();
    void Clear();
    
    void LoadPMTPositions();
    void LoadPMTNormals();
    void LoadPMTTypes();
    
    void LoadRefractiveIndices();
    
    void LoadDetectorGeoParameters();
    void LoadPMTGeoParameters();
    
    void LoadGeoPMTShadowingVals( Int_t runID );
    void LoadAVHDRopePMTShadowingVals( Int_t runID );
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    TVector3 GetPMTPosition( Int_t pmtID ){ return fPMTPositions[ pmtID ]; }
    TVector3 GetPMTNormal( Int_t pmtID ){ return fPMTNormals[ pmtID ]; }
    Int_t GetPMTType( Int_t pmtID ){ return fPMTTypes[ pmtID ]; }
    Int_t GetNPMTs(){ return fNPMTs; }
    
    TGraph GetScintRI(){ return fScintRI; }
    TGraph GetAVRI(){ return fAVRI; }
    TGraph GetWaterRI(){ return fWaterRI; }
    
    Double_t GetAVInnerRadius(){ return fAVInnerRadius; }
    Double_t GetAVOuterRadius(){ return fAVOuterRadius; }
    Double_t GetAVNeckInnerRadius(){ return fAVNeckInnerRadius; }
    Double_t GetAVNeckOuterRadius(){ return fAVNeckOuterRadius; }
    Double_t GetPMTRadius(){ return fPMTRadius; }
    
    Double_t GetGeoPMTShadowingVal( Int_t pmtID ){ return fGeoPMTShadowingVals[ pmtID ]; }
    Double_t GetAVHDRopePMTShadowingVal( Int_t pmtID ){ return fAVHDRopePMTShadowingVals[ pmtID ]; }
    
    std::string GetSOCRunDir(){ return fSOCRunDir; }
    std::string GetLOCASRunDir(){ return fLOCASRunDir; }
    
  private:
    
    RAT::DB* fRATDB;
    RAT::DBLinkPtr fRATDBPtr;
    
    std::map<Int_t, TVector3> fPMTPositions;
    std::map<Int_t, TVector3> fPMTNormals;
    std::map<Int_t, Int_t> fPMTTypes;
    
    TGraph fScintRI;
    TGraph fAVRI;
    TGraph fWaterRI;
    
    Double_t fAVInnerRadius;
    Double_t fAVOuterRadius;
    Double_t fAVNeckInnerRadius;
    Double_t fAVNeckOuterRadius;
    Double_t fPMTRadius;

    Int_t fNPMTs;
    
    std::map<Int_t, Double_t> fGeoPMTShadowingVals;
    std::map<Int_t, Double_t> fAVHDRopePMTShadowingVals;
    
    std::string fSOCRunDir;
    std::string fLOCASRunDir;
    
    ClassDef( LOCASDB, 1 );
        
  };
}

#endif
