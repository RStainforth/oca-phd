#ifndef QMUONFIT_h
#define QMUONFIT_h
#include "TObject.h"

class QMuonFit: public TObject{

 public:
  QMuonFit(){;};
  ~QMuonFit(){;};
  QMuonFit(Int_t * aFit, Int_t *aVertex, Int_t *aTrack,Int_t *aMes,Int_t &aFitter);
  void Set(Int_t * aFit, Int_t *aVertex, Int_t *aTrack,Int_t *aMes,Int_t &aFitter);
  void SetX(Float_t aX){fX=aX;}
  void SetY(Float_t aY){fY=aY;}
  void SetZ(Float_t aZ){fZ=aZ;}
  void SetTime(Float_t aTime){fTime=aTime;}
  void SetDeltaX(Float_t aDeltaX){fDeltaX=aDeltaX;}
  void SetDeltaY(Float_t aDeltaY){fDeltaY=aDeltaY;}
  void SetDeltaZ(Float_t aDeltaZ){fDeltaZ=aDeltaZ;}
  void SetDeltaTime(Float_t aDeltaTime){fDeltaTime=aDeltaTime;}
  void SetU(Float_t aU){fU=aU;}
  void SetV(Float_t aV){fV=aV;}
  void SetW(Float_t aW){fW=aW;}
  void SetDeltaU(Float_t aDeltaU){fDeltaU=aDeltaU;}
  void SetDeltaV(Float_t aDeltaV){fDeltaV=aDeltaV;}
  void SetDeltaW(Float_t aDeltaW){fDeltaW=aDeltaW;}
  void SetReturnCode(Int_t aReturnCode){fReturnCode=aReturnCode;}
  void SetPMTAvailable(Int_t aPMTAvailable){fPMTAvailable=aPMTAvailable;}
  void SetPMTUsed(Int_t aPMTUsed){fPMTUsed=aPMTUsed;}
  void SetEarlyHits(Int_t aEarlyHits){fEarlyHits=aEarlyHits;}
  void SetLateHits(Int_t aLateHits){fLateHits=aLateHits;}
  void SetIterations(Int_t aIterations){fIterations=aIterations;}
  void SetProbability(Int_t aProbability){fProbability=aProbability;}
  void SetChiDof(Float_t aChiDof){fChiDof=aChiDof;}
  void SetNTubesGoodCharge(Float_t aNTubesGoodCharge){fNTubesGoodCharge=aNTubesGoodCharge;}
  void SetNTubesGoodGuess(Float_t aNTubesGoodGuess){fNTubesGoodGuess=aNTubesGoodGuess;}
  void SetNTubesCutChisq(Float_t aNTubesCutChisq){fNTubesCutChisq=aNTubesCutChisq;}
  void SetPFit(Float_t aPFit){fPFit=aPFit;}
  void SetLength(Float_t aLength){fLength=aLength;}
  void SetEta(Float_t aEta){fEta=aEta;}
  void SetPsi(Float_t aPsi){fPsi=aPsi;}
  void SetExitX(Float_t aExitX){fExitX=aExitX;}
  void SetExitY(Float_t aExitY){fExitY=aExitY;}
  void SetExitZ(Float_t aExitZ){fExitZ=aExitZ;}
  void SetExitTime(Float_t aExitTime){fExitTime=aExitTime;}
  void SetMeanQ(Float_t aMeanQ){fMeanQ=aMeanQ;}
  void SetRMSQ(Float_t aRMSQ){fRMSQ=aRMSQ;}
  void SetMeanTime(Float_t aMeanTime){fMeanTime=aMeanTime;}
  void SetRMST(Float_t aRMST){fRMST=aRMST;}
  void SetPCorr(Float_t aPCorr){fPCorr=aPCorr;}
  void SetdEdX(Float_t adEdX){fdEdX=adEdX;}
  void SetDeltaT(Float_t aDeltaT){fDeltaT=aDeltaT;}
  void SetAllTubesChisq(Float_t aAllTubesChisq){fAllTubesChisq=aAllTubesChisq;}
  void SetNTubesAgree(Float_t aNTubesAgree){fNTubesAgree=aNTubesAgree;}
  void SetQMeanAgree(Float_t aQMeanAgree){fQMeanAgree=aQMeanAgree;}
  void SetChiSqAgree(Float_t aChiSqAgree){fChiSqAgree=aChiSqAgree;}
  void SetLogPdf(Float_t aLogPdf){fLogPdf=aLogPdf;}
  void SetPdfDof(Float_t aPdfDof){fPdfDof=aPdfDof;}
  void SetNShould(Float_t aNShould){fNShould=aNShould;}
  void SetFracShould(Float_t aFracShould){fFracShould=aFracShould;}
  void SetNShouldnt(Float_t aNShouldnt){fNShouldnt=aNShouldnt;}
  void SetQShouldnt(Float_t aQShouldnt){fQShouldnt=aQShouldnt;}
  void SetChisqShould(Float_t aChisqShould){fChisqShould=aChisqShould;}
  void SetShouldAndAgree(Float_t aShouldAndAgree){fShouldAndAgree=aShouldAndAgree;}
  void SetShouldntAndAgree(Float_t aShouldntAndAgree){fShouldntAndAgree=aShouldntAndAgree;}
  void SetShouldAndDisagree(Float_t aShouldAndDisagree){fShouldAndDisagree=aShouldAndDisagree;}
  void SetShouldntAndDisagree(Float_t aShouldntAndDisagree){fShouldntAndDisagree=aShouldntAndDisagree;}
  void SetOwlTubesOnline(Float_t aOwlTubesOnline){fOwlTubesOnline=aOwlTubesOnline;}
  void SetNeck1Charge(Float_t aNeck1Charge){fNeck1Charge=aNeck1Charge;}
  void SetNeck2Charge(Float_t aNeck2Charge){fNeck2Charge=aNeck2Charge;}
  void SetNeck1Time(Float_t aNeck1Time){fNeck1Time=aNeck1Time;}
  void SetNeck2Time(Float_t aNeck2Time){fNeck2Time=aNeck2Time;}
  void SetNExitWoundShould(Float_t aNExitWoundShould){fNExitWoundShould=aNExitWoundShould;}
  void SetNExitWoundDid(Float_t aNExitWoundDid){fNExitWoundDid=aNExitWoundDid;}
  void SetQExitWound(Float_t aQExitWound){fQExitWound=aQExitWound;}
  void SetMcP(Float_t aMcP){fMcP=aMcP;}
  void SetMcEta(Float_t aMcEta){fMcEta=aMcEta;}
  void SetMcPsi(Float_t aMcPsi){fMcPsi=aMcPsi;}
  void SetMcXStop(Float_t aMcXStop){fMcXStop=aMcXStop;}
  void SetMcYStop(Float_t aMcYStop){fMcYStop=aMcYStop;}
  void SetMcZStop(Float_t aMcZStop){fMcZStop=aMcZStop;}


  Float_t GetX()const {return fX;}
  Float_t GetY()const {return fY;}
  Float_t GetZ()const {return fZ;}
  Float_t GetTime()const {return fTime;}
  Float_t GetDeltaX()const {return fDeltaX;}
  Float_t GetDeltaY()const {return fDeltaY;}
  Float_t GetDeltaZ()const {return fDeltaZ;}
  Float_t GetDeltaTime()const {return fDeltaTime;}
  Float_t GetU()const {return fU;}
  Float_t GetV()const {return fV;}
  Float_t GetW()const {return fW;}
  Float_t GetDeltaU()const {return fDeltaU;}
  Float_t GetDeltaV()const {return fDeltaV;}
  Float_t GetDeltaW()const {return fDeltaW;}
  Int_t GetReturnCode()const {return fReturnCode;}
  Int_t GetPMTAvailable()const {return fPMTAvailable;}
  Int_t GetPMTUsed()const {return fPMTUsed;}
  Int_t GetEarlyHits()const {return fEarlyHits;}
  Int_t GetLateHits()const {return fLateHits;}
  Int_t GetIterations()const {return fIterations;}
  Int_t GetProbability()const {return fProbability;}
  Float_t GetChiDof()const {return fChiDof;}
  Float_t GetNTubesGoodCharge()const {return fNTubesGoodCharge;}
  Float_t GetNTubesGoodGuess()const {return fNTubesGoodGuess;}
  Float_t GetNTubesCutChisq()const {return fNTubesCutChisq;}
  Float_t GetPFit()const {return fPFit;}
  Float_t GetLength()const {return fLength;}
  Float_t GetEta()const {return fEta;}
  Float_t GetPsi()const {return fPsi;}
  Float_t GetExitX()const {return fExitX;}
  Float_t GetExitY()const {return fExitY;}
  Float_t GetExitZ()const {return fExitZ;}
  Float_t GetExitTime()const {return fExitTime;}
  Float_t GetMeanQ()const {return fMeanQ;}
  Float_t GetRMSQ()const {return fRMSQ;}
  Float_t GetMeanTime()const {return fMeanTime;}
  Float_t GetRMST()const {return fRMST;}
  Float_t GetPCorr()const {return fPCorr;}
  Float_t GetdEdX()const {return fdEdX;}
  Float_t GetDeltaT()const {return fDeltaT;}
  Float_t GetAllTubesChisq()const {return fAllTubesChisq;}
  Float_t GetNTubesAgree()const {return fNTubesAgree;}
  Float_t GetQMeanAgree()const {return fQMeanAgree;}
  Float_t GetChiSqAgree()const {return fChiSqAgree;}
  Float_t GetLogPdf()const {return fLogPdf;}
  Float_t GetPdfDof()const {return fPdfDof;}
  Float_t GetNShould()const {return fNShould;}
  Float_t GetFracShould()const {return fFracShould;}
  Float_t GetNShouldnt()const {return fNShouldnt;}
  Float_t GetQShouldnt()const {return fQShouldnt;}
  Float_t GetChisqShould()const {return fChisqShould;}
  Float_t GetShouldAndAgree()const {return fShouldAndAgree;}
  Float_t GetShouldntAndAgree()const {return fShouldntAndAgree;}
  Float_t GetShouldAndDisagree()const {return fShouldAndDisagree;}
  Float_t GetShouldntAndDisagree()const {return fShouldntAndDisagree;}
  Float_t GetOwlTubesOnline()const {return fOwlTubesOnline;}
  Float_t GetNeck1Charge()const {return fNeck1Charge;}
  Float_t GetNeck2Charge()const {return fNeck2Charge;}
  Float_t GetNeck1Time()const {return fNeck1Time;}
  Float_t GetNeck2Time()const {return fNeck2Time;}
  Float_t GetNExitWoundShould()const {return fNExitWoundShould;}
  Float_t GetNExitWoundDid()const {return fNExitWoundDid;}
  Float_t GetQExitWound()const {return fQExitWound;}
  Float_t GetMcP()const {return fMcP;}
  Float_t GetMcEta()const {return fMcEta;}
  Float_t GetMcPsi()const {return fMcPsi;}
  Float_t GetMcXStop()const {return fMcXStop;}
  Float_t GetMcYStop()const {return fMcYStop;}
  Float_t GetMcZStop()const {return fMcZStop;}

  ClassDef (QMuonFit,2)

    private:
  // Note:  The order in each group below needs to follow the 
  // SNOMAN titles file, since we simply do a memory copy.
  // From Ftmv
  Float_t fX;
  Float_t fY;
  Float_t fZ;
  Float_t fTime;
  Float_t fDeltaX;
  Float_t fDeltaY;
  Float_t fDeltaZ;
  Float_t fDeltaTime;
  
  // From Track:
  Float_t fU;
  Float_t fV;
  Float_t fW;
  Float_t fDeltaU;
  Float_t fDeltaV;
  Float_t fDeltaW;

  Int_t fIndex;  // Fitter index

  // From FTM
  Int_t fReturnCode;
  Int_t fPMTAvailable;
  Int_t fPMTUsed;  
  Int_t fEarlyHits;            //  No of early PMT hits
  Int_t fLateHits;             //  Number of late PMT hits
  Int_t fIterations;           //  Number of Iterations
  Int_t fProbability;          //  Fit probability
  Float_t fChiDof;             //  Chisquare per degree of freedom
  Float_t fNTubesGoodCharge;   //  Number of tubes with good charge
  Float_t fNTubesGoodGuess;    //  Number of tubes inside time window of betst trial
  Float_t fNTubesCutChisq;     //  Number of tubes inside time window
  Float_t fPFit;               //  Impact parameters of final fit
  Float_t fLength;             //  Length of muon track inside PSUP
  Float_t fEta;                //  Muon direction vector zenith angle
  Float_t fPsi;                //  Muon directino azimuthal angle
  Float_t fExitX;              //  Exit position
  Float_t fExitY; 
  Float_t fExitZ; 
  Float_t fExitTime; 
  Float_t fMeanQ;              //  Mean charge on all tubes
  Float_t fRMSQ;               //  RMS of charge on all tubes
  Float_t fMeanTime;           //  Mean calibratedd time of all tues
  Float_t fRMST;               //  FMS of calibrated time of all tubes
  Float_t fPCorr;              //  Fitted Impact parameter, corrected for fitter pull.
  Float_t fdEdX;               //  Total charge / track length, pe/c
  Float_t fDeltaT;             //  Time elapsed since last event FTM processed.
  Float_t fAllTubesChisq;      //  Chi-squared per tube using all tubes in event.
  Float_t fNTubesAgree;        //  Number of all tubes for which fit time is good to within 1 sigma of measured time
  Float_t fQMeanAgree;         //  The fraction of all charge deposited in tubes that agree to within 1 sigma.
  Float_t fChiSqAgree;         //  The chi-squared statistic on time residual for tubes that agree to within 1 sigma
  Float_t fLogPdf;             //  Total log(pdf) of time residuals.
  Float_t fPdfDof;             //  log(pdf) of time residuals divided by number of tubes
  Float_t fNShould;            //  Number of tubes that should have fired (see note 1) and did.
  Float_t fFracShould;         //  Fraction of tubes that should have fired that DID fire
  Float_t fNShouldnt;          //  Number of tubes that shouldn't have fired but did
  Float_t fQShouldnt;  //          Total charge of tubes that shouldn't have fired but did
  Float_t fChisqShould;        //  Chi-squared of time residuals for tubes that should have fired and did
  Float_t fShouldAndAgree;   //    Number of tubes that should have fired, did fire, and had time residuals less than 1 sigma 
  Float_t fShouldntAndAgree;  //   Number of tubes that shouldn't have fired but did, and had a time residuals less than 1 sigma
  Float_t fShouldAndDisagree;  //  Number of tubes that should have fired, did, and had bad time residuals
  Float_t fShouldntAndDisagree; // Number of tubes that shouldn't have fired but did, and had bad time residuals
  Float_t fOwlTubesOnline;    //   Number of OWL tubes for active during event (IS_TUBE_ON())
  Float_t fNeck1Charge;      //    The charge recorded in the first neck tube
  Float_t fNeck2Charge;      //    The charge recorded in the second neck tube
  Float_t fNeck1Time;       //     The time of the first neck tube
  Float_t fNeck2Time;   //         The time of the second neck tube
  Float_t fNExitWoundShould;   //  The number of tubes in the exit wound 
  Float_t fNExitWoundDid;   //     The number of tubes in the exit wound that fired
  Float_t fQExitWound;          // The total charge in the tubes of the exit wound
  Float_t fMcEnergy;      // Energy in MC
  Int_t fMcMuMask;        // muon tags- see bank_ftrm.inc for definitions
  Int_t fMcPartMask;      // particle masks- see bank_ftrm.inc for definitions
  Float_t fMcP;           //       True impact parameter
  Float_t fMcEta;         //       True muon diretion vector zenith angle
  Float_t fMcPsi;             //   True muon diretion vector azimuthal angle
  Float_t fMcXStop;           //   |
  Float_t fMcYStop;           //   | The stopping position of the muon. (See note 4)
  Float_t fMcZStop;          //    |

  //From FTI, FTY, FTZ
  Float_t fNegativeLogLikelyhood; //
  Float_t fSingleToMultiphotonRatio; //Radio of single to multi photon pmts
  
  //From mes parameters
  Float_t fDetectedNPE ;//  Total number of detected photons
  Float_t fFitNPE;    //Fit number of detected photons (only filled for FTI)
  Float_t fTubesOnline; //    Number of tubes online (ANxx)
  Float_t fHitTubes; //       Number of hit tubes with good calibrations and no discard bit
  Float_t fImpact;          // Fit impact parameter (cm)
  Float_t fMITR;            // Muon In Time Ratio
  Float_t fMCITR;           // Muon Cone In Time Ratio
  Float_t fRICHAll;         // Ratio of In Cone Hits using all hits in the cone
  Float_t fRICHTrim;        // Ratio of In Cone Hits discarding the first and last 50cm of the track
  Float_t fTRESRMS;         // Time residual RMS of all tubes
  Float_t fConeTRESRMS;     // Time residual RMS of tubes in the cone
  Float_t fDEDX1;           // Estimate of muon dE/dx (MeV/cm)
  Float_t fDEDX2;           //RESERVED - not yet defined
  Float_t fPDFRename;       //RESERVED - not yet defined (and re-name it please!)
  Float_t fKSZenith;        // Kuiper's V parameter for zenith angle cumulative charge distribution
  Float_t fKSAzimuth;        //Kuiper's V parameter for azimuthal angle cumulative charge distribution
  Float_t fSoftAMBPeak;      //Software "AMB" type measurement of peak charge on ESUM line
  Float_t fSoftAMBDiff;      // Software "AMB" type measurement of steepest diff
  Float_t fPrepulseFraction; // Fraction of tubes in the cone which are prepulse hits
  Float_t fUpDownQRAT;       // Ratio of charge in entry half of detectorto exit half
  Float_t fRawTRMS;          // The RMS of the unfitted calibrated time distribution (Not the time residual!)
  Float_t fRawQRMS;           // The RMS of the calibrated charge distribution
};

#endif //QMUONFIT_h
 
