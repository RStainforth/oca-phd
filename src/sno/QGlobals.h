#ifndef _QGlobals_h
#define _QGlobals_h

#include <Rtypes.h>

enum operation { plus, minus, times};

class QViewer;
class QTrigger;
class QPMTxyz;
class QWater;
class QBrowser;
class QBank;
class QSnoCal;
class QCal;
class QSnoed;
class QSNO;

extern QPMTxyz   *gPMTxyz;
extern QTrigger  *gQTrigger;
extern QViewer   *gQViewer;
extern QBrowser  *gQBrowser;
extern QWater    *gQWater;
extern QBank     *gCRON;
extern QBank     *gVRLY;
extern QCal      *gCal;
extern QSnoed    *gSnoed;
extern QSnoCal   *gCalibrator;
extern QSNO      *gSNO;

//enumerations
enum Eqmtf_mode { kMinuit, kMrqmin };

enum EqmtfOptions {
  kReflections        = BIT(0), 
  kCutOnPromptPeak    = BIT(1),
  kTIR                = BIT(2),
  kRefractions        = BIT(3)
};

enum EPorts {
  kSDBPort            = 9090,
  kSnomanPort         = 9092
};

enum ECalibrationFlags {
  kStaticConstants   = BIT(0),
  kDynamicConstants  = BIT(1)
};

enum EPMTStatusFlags{
  kDiscard                         = BIT(0),   //discard this PMT
  kNoise                           = BIT(1),   //Noise Hit
  kEarly                           = BIT(2),   //Early PMT
  kLate                            = BIT(3),   //Late PMT
  kHiRailed                        = BIT(4),   //High railed charge
  kLoRailed                        = BIT(5),   //Low railed charge
  kDiscardSpecial                  = BIT(6),   //Billerization (or other) flag
 
  kDiscardFTT                      = BIT(7),   //discarded by FTT
  kDiscardEnergy                   = BIT(8),   //discard this PMT for energy calibration
  kDiscardMTF                      = BIT(12),  //discarded by MTF
  kDiscardPTF                      = BIT(16),  //discarded by PTF
  kDiscardETF                      = BIT(20),  //discarded by ETF
  kDiscardQTPi                     = BIT(24)   //discarded by QTPi
};

enum EEventFlags{
  kCalibrated                      = BIT(12),  //This event has been calibrated
  kECA1                            = BIT(13),  //This event has been calibrated with ECA mode 1
  kECA2                            = BIT(14),  //This event has been calibrated with ECA mode 2
  
  kWalk                            = BIT(15),  //PMTs have been walk-corrected
  kGain                            = BIT(16),  //PMTs have been gain-corrected
  kAlternate                       = BIT(17),  //This event has been calibrated with the Alternate (Ford) PCA

  //The following bits are for qsnoman control
  kCalibrate                       = BIT(20),  //Calibrate this event
  kFitFTT                          = BIT(21),  //Fit this event with snoman FTT
  kFitRSP                          = BIT(22)   //Calculate the event energy
 
};

enum ECutIDs{
  //These bits are used for QCut identification.
  kCutClean                      = BIT(0),  //This event has passed all cuts
  kCutRailed                     = BIT(1),  //This event contains a railed charge
  kCutENoise                     = BIT(2),  //This event fails the Enoise cut
  kCutBurst                      = BIT(3),  //This event is part of a burst 
  kCutWEB                        = BIT(4),  //This event is a web
  kCutGen                        = BIT(5),  //This event fails the Nph/Nhit cut
  kCutCraP                       = BIT(6),  //This is a Crate Perimeter event
  kCutMuon                       = BIT(13), //Muon cut 
  kCutNECK                       = BIT(16), //NECK cut
  kCutOD                         = BIT(17), //Outer detector cut
  kCutAMB                        = BIT(18), //AMB cut
  //additional bits
  kCutFlasher                    = BIT(7),  //This event is a flasher
  kCutBurst1                     = BIT(8),  //1st burst cut
  kCutBurst2                     = BIT(9),  //2nd burst cut
  kCutBurst3                     = BIT(10), //3rd burst cut
  kCutNph                        = BIT(11), //Nph/Nhits cut
  kCutCerenkov                   = BIT(12), //Light distribution cut
  kCutRinger                     = BIT(14), //Muon ringer event
  kCutSpallation                 = BIT(15)  //Muon spallation event
};

enum EPMTFlags{
  //Bit flags for PMTs.fBits
  kFlasher                         = BIT(10),
  kWEB                             = BIT(11)
};

enum EMCVXFlags{
  //Bit flags for MCVXs.fBits
  kReflected                       = 1 //This particle has undergone a reflection in its lifetime.
};

enum EQFitIndex{
  kFTT           = BIT(5),
  kMTF           = BIT(12),
  kPTF           = BIT(16),
  kETF           = BIT(20),
  kCTF           = BIT(24),
  kQTPi          = BIT(28)
};

enum EQMTLevel{
  kMTT_COMPULSORY, kMTT_OPTIONAL
};

enum cc_mode { 
  total_bahcall, 
  differential_bahcall, 
  double_differential_bahcall, 
  total_lookup, 
  differential_lookup, 
  double_differential_lookup 
};

enum ccc_type {
  KUTL_CCC_PMT_NORMAL  = 1,
  KUTL_CCC_PMT_OWL     = 2,
  KUTL_CCC_PMT_LG      = 3,
  KUTL_CCC_PMT_BUTT    = 4,
  KUTL_CCC_PMT_NECK    = 5,
  KUTL_CCC_CALIBRATION = 6,
  KUTL_CCC_SPARE       = 10,
  KUTL_CCC_INVALID     = 11
};

enum pmt_info {
  KMIN_PMT_LCN         = 0,
  KMAX_PMT_LCN         = 9728
};

enum ge_mnemonics {
  KGE_PMT_MN_OWL       = 9601,
  KGE_PMT_MN_LG        = 9799,
  KGE_PMT_MN_BUTT      = 9801,
  KGE_PMT_MN_NECK      = 9901
};

enum ccc_info_mode {
  KCCC_THRESH       = 1,
  KCCC_RELAY        = 2,
  KCCC_STATUS       = 3,
  KCCC_ON_LINE      = 4,
  KCCC_THRESH_ZERO  = 5,
  KCCC_HV           = 6,
  KCCC_TUBE_ON_LINE = 7,
  KCCC_CHAN_ON_LINE = 8, 
  KCCC_UNDEFINED = -2147483647 
};

enum ccc_info_status {
  KCCC_B_CRATE           =  0,
  KCCC_B_SLOT_OP         =  1,
  KCCC_B_GT              =  2,
  KCCC_B_CR_ONLINE       =  3,
  KCCC_B_CR_HV           =  4,
  KCCC_B_MB              =  8,
  KCCC_B_DC              =  9,
  KCCC_B_DAQ             = 10,
  KCCC_B_SEQUENCER       = 16,
  KCCC_B_100NS           = 17,
  KCCC_B_20NS            = 18,
  KCCC_B_VTHR_MAX        = 19,
  KCCC_B_QINJ            = 20,
  KCCC_B_N100            = 21,
  KCCC_B_N20             = 22,
  KCCC_B_NOT_OP          = 23,
  KCCC_B_PMTIC           = 24,
  KCCC_B_RELAY           = 26,
  KCCC_B_PMTIC_RESISTOR  = 27,
  KCCC_B_PMT_CABLE       = 28,
  KCCC_B_75OHM           = 29 
};

enum ccc_info_lab
{
  KCCC_MX_CRATE = 18
};

enum ccc_info_dqch
{
  KDQCH_TABLE             = 11,
  KDQCH_B_PMT_CABLE       =  0,
  KDQCH_B_PMTIC_RESISTOR  =  1,
  KDQCH_B_SEQUENCER       =  2,
  KDQCH_B_100NS           =  3,
  KDQCH_B_20NS            =  4,
  KDQCH_B_75OHM           =  5,
  KDQCH_B_QINJ            =  6,
  KDQCH_B_N100            =  7,
  KDQCH_B_N20             =  8,
  KDQCH_B_NOT_OP          =  9,
  KDQCH_B_BAD             = 10,
  KDQCH_B_VTHR            = 16,
  KDQCH_B_VTHR_ZERO       = 24 
};

enum ccc_info_dqcr
{
 KDQCR_TABLE       = 11,
 KDQCR_B_CRATE     =  0,
 KDQCR_B_MB        =  1,
 KDQCR_B_PMTIC     =  2,
 KDQCR_B_DAQ       =  3,
 KDQCR_B_DC        =  4,
 KDQCR_B_SLOT_OP   =  8,
 KDQCR_B_GT        =  9,
 KDQCR_B_CR_ONLINE = 10,
 KDQCR_B_CR_HV     = 11,
 KDQCR_B_RELAY     = 12,
 KDQCR_B_HV        = 16 
};



#endif





