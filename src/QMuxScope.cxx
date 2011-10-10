
//____________________________________________________
//
// QMuxScope
//*-- Author : Adam Cox
// UpDates: 
//	   10/01/02 -- original version
//         03/16/03 -- updated move towards QSNO coding standards compliance
//         01/27/05 -- Added correlation to Shaper event (ckrauss@owl.phy.queensu.ca)

//
// Compile with QSNO_MEM_DEBUG to do memory allocation debugging
//
// Default is now to use TClonesArray::Delete( "" ) in QEvent.cxx
// in which case neither QMuxScope::Clear nor QMuxScope::Delete are called
// ( the destructor is called instead ) 
// To restore the old behaviour re-compile QEvent.cxx and QMuxScope.cxx with
// -DUSE_TCLONES_CLEAR -DQSNO_MUXSCOPE_CLEAR -DQSNO_MUXSCOPE_DELETE 
//


#include "QMuxScope.h"
#include "NCDConstants.h"
#include <iostream>

using namespace std;

ClassImp(QMuxScope);

#ifdef QSNO_MUXSCOPE_CLEAR
void QMuxScope::Clear(Option_t *anOption)
{
  if(fQScopeAnalysis ) fQScopeAnalysis->Clear(anOption);

  fValues.Set( 0, NULL );          // This basically does a `delete' and NULL's the pointer
                                   // See code in TH1C::Set
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Clear [0x%x]\n", this );
#endif

}
#endif

QMuxScope::~QMuxScope()
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Destruct [0x%x]\n", this );
#endif
  //  fValues.Set( 0, NULL );          // This is safe even if fArray is NULL
#ifdef QSNO_MEM_DEBUG
  if( fValues.GetDirectory() ) printf( "QMuxScope::Destruct : Non-zero GetDirectory() !\n" );
                                   // I don't understand why the `delete' above does not always remove
                                   // the name from the directory but sometimes it is still there ...
#endif
	fValues.SetDirectory( 0 );	     // Just in case name is set in directory ....
}



#ifdef QSNO_MUXSCOPE_DELETE
void QMuxScope::Delete(Option_t *anOption)
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxSCope::Delete [0x%x]\n", this );
#endif

#ifdef USE_TCLONES_CLEAR
  printf( "QMuxScope : Attempting to call Delete() !\n" ); // We should _never_ come here
                                                           // if use Clear()
#endif
  
  if(fQScopeAnalysis) {
    delete fQScopeAnalysis;
    fQScopeAnalysis = NULL;
  }

	fValues.Set( 0, NULL );          // This is safe even if fArray is NULL ( see TH1C::Set )
#ifdef QSNO_MEM_DEBUG
  if( fValues.GetDirectory() ) printf( "QMuxScope::Delete : Non-zero GetDirectory() !\n" );
#endif
	fValues.SetDirectory( 0 );	     // Just in case name is set in directory ....
}
#endif

QMuxScope::QMuxScope(Bool_t bOffsetOption)
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Create [0x%x]\n", this );
#endif
  Initialize();
  SetIsPreTrigCalculated(bOffsetOption);
  
}

QMuxScope::QMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars, Bool_t bOffsetOption)
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Create/Snoman3 [0x%x]\n", this );
#endif
  fStatusWord=ivars[0];
  fMuxChannelMask=ivars[1];
  fScopeNumber=ivars[2];
  fMuxBusNumber=ivars[3];
  fMuxBoxNumber = ivars[4];
  fNCDStringNumber = ivars[5];
  //
  //  6 and 7 are spare words in NEMS
  //
  fClockRegister=*(Double_t *)(ivars+8);
  fLatchRegisterID=ivars[10];
  fSizeOfScopeTrace=ivars[11];
  fScopeChannel=ivars[12];
  //
  // 13 through 19 are spare words
  //
  Int_t len= 0 < ivars[11] ? ivars[11] : 1;
  fValues.SetBins(len,0,len);
  Char_t *cptr=(Char_t *)(ivars+20);  //ivars is actually character packed data
  
#ifdef SWAP_BYTES
  //the following two lines swap the byte order in a word.
  for(int i=0;i<len;i++)fValues.SetBinContent(i,cptr[i ^ 0x03]);
#else
  fValues.Set(len,cptr);
#endif
  fValues.UseCurrentStyle();
  fValues.SetEntries( (Int_t)len);
  fValues.SetDirectory(0);
  fMuxChannelNumber = GetMuxChannel();
  
  fGlobalMuxRegister = ( jvars[0] & 0xff ) | ( ( jvars[1] >> 8 ) & 0x300 ) | ( ( jvars[2] >> 10 ) & 0xc00 ) |
    ( ( jvars[ 3 ] >> 12 ) & 0x3000 ); // This is almost identical to what is in the original Global Mux
  // Record except the scope bits are in `natural' order
  fLogAmpParam_a = fvars[0];
  fLogAmpParam_b = fvars[1];
  fLogOffset = fvars[2];
  fLogAmpPreampRCFactor = fvars[3];  //these might be out of order!?
  fLogAmpElecDelayTime = fvars[4];   //out of order?
  fScopeOffset = fvars[5];
  fShaperADCcharge = -1;

  // fShaperEventGTID is initialized to KNECL_NO_GTID (=-99999).  Previous 
  // QSNO versions initialized this to 0xFFFFFFFF  
  fShapereventGTID = -99999;
  fShaperADCcharge = -1;
  fShaperDeltaT = -999999.;

  fQScopeAnalysis = NULL;
  
  SetIsPreTrigCalculated(bOffsetOption);
  
}

QMuxScope::QMuxScope(Int_t *ivars, Int_t *jvars, Float_t *fvars, 
                     Int_t *kvars, Float_t *rvars, Bool_t bOffsetOption)
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Create/Snoman5 [0x%x]\n", this );
#endif
  fStatusWord=ivars[0];
  fMuxChannelMask=ivars[1];
  fScopeNumber=ivars[2];
  fMuxBusNumber=ivars[3];
  fMuxBoxNumber = ivars[4];
  fNCDStringNumber = ivars[5];
  //
  //  6 and 7 are spare words in NEMS
  //
  fClockRegister=*(Double_t *)(ivars+8);
  fLatchRegisterID=ivars[10];
  fSizeOfScopeTrace=ivars[11];
  fScopeChannel=ivars[12];
  //
  // 13 through 19 are spare words
  //
  Int_t len= 0 < ivars[11] ? ivars[11] : 1;
  fValues.SetBins(len,0,len);
  Char_t *cptr=(Char_t *)(ivars+20);  //ivars is actually character packed data
  
#ifdef SWAP_BYTES
  //the following two lines swap the byte order in a word.
  for(int i=0;i<len;i++)fValues.SetBinContent(i,cptr[i ^ 0x03]);
#else
  fValues.Set(len,cptr);
#endif
  fValues.UseCurrentStyle();
  fValues.SetEntries( (Int_t)len);
  fValues.SetDirectory(0);
  fMuxChannelNumber = GetMuxChannel();
  
  fGlobalMuxRegister = ( jvars[0] & 0xff ) | ( ( jvars[1] >> 8 ) & 0x300 ) | ( ( jvars[2] >> 10 ) & 0xc00 ) |
    ( ( jvars[ 3 ] >> 12 ) & 0x3000 ); // This is almost identical to what is in the original Global Mux
  // Record except the scope bits are in `natural' order
  fLogAmpParam_a = fvars[0];
  fLogAmpParam_b = fvars[1];
  fLogOffset = fvars[2];
  fLogAmpPreampRCFactor = fvars[3];  //these might be out of order!?
  fLogAmpElecDelayTime = fvars[4];   //out of order?
  fScopeOffset = fvars[5];
  
  fShaperADCcharge = kvars[0];  
  fShapereventGTID = kvars[1];
  fShaperDeltaT = rvars[0];

  fQScopeAnalysis = NULL;
  
  SetIsPreTrigCalculated(bOffsetOption);
  
}

QMuxScope::QMuxScope(const QMuxScope &aQmux) : TObject(aQmux)
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Create/Set : [0x%x]\n", this );
#endif
  Initialize();
  *this = aQmux;
}

void QMuxScope::Initialize()
{
  fValues.SetNameTitle("Scope","Scope");
  fValues.SetBins((Int_t)kDefaultWaveformSize,0,kDefaultWaveformSize);
  fValues.UseCurrentStyle();
  fValues.SetEntries((Int_t)kDefaultWaveformSize);
  fValues.SetDirectory(0);
  fQScopeAnalysis = NULL;
  fSizeOfScopeTrace = 0;
  fShapereventGTID = 0xFFFFFFFF;
  fShaperADCcharge = -1;
}

QMuxScope &QMuxScope::operator = ( const QMuxScope &aQmux )
{
#ifdef QSNO_MEM_DEBUG
  printf( "QMuxScope::Copy [0x%x]\n", this );
#endif

  fStatusWord = aQmux.fStatusWord;
  fMuxChannelMask = aQmux.fMuxChannelMask;
  fScopeNumber = aQmux.fScopeNumber;
  fMuxBusNumber = aQmux.fMuxBusNumber;
  fMuxBoxNumber = aQmux.fMuxBoxNumber;
  fNCDStringNumber = aQmux.fNCDStringNumber;
  fClockRegister = aQmux.fClockRegister;
  fLatchRegisterID = aQmux.fLatchRegisterID;
  fSizeOfScopeTrace = aQmux.fSizeOfScopeTrace;
  fScopeChannel = aQmux.fScopeChannel;
  fMuxChannelNumber = aQmux.fMuxChannelNumber;
  fGlobalMuxRegister = aQmux.fGlobalMuxRegister;
  fLogAmpParam_a = aQmux.fLogAmpParam_a;
  fLogAmpParam_b = aQmux.fLogAmpParam_b;
  fLogOffset = aQmux.fLogOffset;
  fLogAmpPreampRCFactor = aQmux.fLogAmpPreampRCFactor;
  fLogAmpElecDelayTime = aQmux.fLogAmpElecDelayTime;
  fScopeOffset = aQmux.fScopeOffset;

  fShapereventGTID = aQmux.fShapereventGTID;
  fShaperADCcharge = aQmux.fShaperADCcharge;

  //	fQScopeAnalysis = aQmux.fQScopeAnalysis;
  fQScopeAnalysis = NULL;      //  This is a temporary ( safe ) hack since QScopeAnalysis
	                             //  does not have a proper copy constructor.
  // Writing a copy constructor for QScopeAnalysis is not straightforward
  // as it contains a pointer to the original QMuxScope object (!)
  // It must also deal with copying TH1's and QTitles  ...

  fValues.SetDirectory(0);
  fValues = aQmux.fValues;
  fValues.UseCurrentStyle();
  bCalcNewOffset = aQmux.bCalcNewOffset;
  
  return *this;
}

Float_t QMuxScope::GetScopeOffset(Int_t nBins)
{
  //This returns the logamp parameter V_preTrig, also known as the scope offst.  
  //Sorry about the two names. 
  //
  //By default (as of July 25, 2007) this value is calculated from the first nBins of
  //the waveform (default nBins = 1000).  nBins should not exceed 1400 since at
  //bin 1500 the trigger pulse begins and there is sometimes a transient in the waveform before bin 1500
  //caused by the MUX gate opening.
  
  if(GetIsPreTrigCalculated()) 
  {
    //cout << "Calculating pre-trigger baseline." << endl;
    return CalculatePreTrigBaseline(nBins);
  }
    
  else 
    return fScopeOffset;
}

Float_t QMuxScope::CalculatePreTrigBaseline(Int_t fBins) 
{
	Float_t fAve = 0;
  Float_t fFloatNBins = fBins;
	for(Int_t i = 0; i < fBins; i++)
		fAve += fValues.GetBinContent(i);
	
	if(fAve) fAve = fAve/fFloatNBins;
	return fAve;
}

Int_t QMuxScope::GetMuxChannel(void)
{
  if (IsMultipleMuxChannels()) return -1;
  Int_t i;
  for (i=0;i<kNumMuxChans;i++) {
    if ((fMuxChannelMask>>i) & 0x1) break;
  }
  return i;
}

Bool_t QMuxScope::IsMultipleMuxChannels(void)
{
  Int_t numHitChans=0;
  for (Int_t i=0;i<kNumMuxChans;i++) {
    numHitChans += (fMuxChannelMask >> i) & 0x1;
  }
  if(numHitChans > 1) return true;
  else return false;
}

Bool_t QMuxScope::InitQScopeAnalysis(void)
{
  Bool_t noErr = true;
  if(fQScopeAnalysis == NULL) {
    fQScopeAnalysis = new QScopeAnalysis( &fValues, fLogAmpParam_a, 
                                          fLogAmpParam_b, fLogOffset, fLogAmpPreampRCFactor, 
                                          fLogAmpElecDelayTime, GetScopeOffset());
  }
  
  if(fQScopeAnalysis) noErr = true;
  else noErr = false;

  return noErr;
}

Float_t QMuxScope::GetCharge(void)
{
  Float_t theReturn = 0;
  if(InitQScopeAnalysis()){
      theReturn = fQScopeAnalysis->GetCharge();
  }
  
  return theReturn;
}

Int_t QMuxScope::GetRiseTime(void)
{
  Int_t theReturn = 0;
  if(InitQScopeAnalysis()){
      theReturn = fQScopeAnalysis->GetRiseTime();
  }
  
  return theReturn;
}

Int_t QMuxScope::GetPulseStartTime(void)
{
  Int_t theReturn = 0;
  if(InitQScopeAnalysis()){
      theReturn = fQScopeAnalysis->GetPulseStartTime();
  }
  
  return theReturn;
}

void QMuxScope::DrawRealPulse()
{
  if(InitQScopeAnalysis()){
    fQScopeAnalysis->GetNCDPulse()->Draw();
  }
}


Bool_t QMuxScope::DeleteQScopeAnalysis(void)
{
  if(fQScopeAnalysis) {
    delete fQScopeAnalysis;
    fQScopeAnalysis = NULL;
    return true;
  } 
  else {
    return false;
  }
}

Bool_t QMuxScope::SetLogAmpParameter_a(Float_t aValue)
{
  fLogAmpParam_a = aValue;
  if(fQScopeAnalysis){
    fQScopeAnalysis->SetLogAmpParameter_a(fLogAmpParam_a);
    return true;
  }
  else return false;
}

Bool_t QMuxScope::SetLogAmpParameter_b(Float_t aValue)
{
  fLogAmpParam_b = aValue;
  if(fQScopeAnalysis){
    fQScopeAnalysis->SetLogAmpParameter_b(fLogAmpParam_b);
    return true;
  }
  else return false;
}

Bool_t QMuxScope::SetLogOffset(Float_t aValue)
{
  fLogOffset = aValue;
  if(fQScopeAnalysis){
    fQScopeAnalysis->SetLogOffset(fLogOffset);
    return true;
  }
  else return false;
}

Bool_t QMuxScope::SetLogAmpPreampRCFactor(Float_t aValue)
{
  fLogAmpPreampRCFactor = aValue; 
  if(fQScopeAnalysis){
    fQScopeAnalysis->SetLogAmpPreampRC(fLogAmpPreampRCFactor);
    return true;
  }
  else return false;
}

Bool_t QMuxScope::SetLogAmpElecDelayTime(Float_t aValue)
{
  fLogAmpElecDelayTime = aValue; 
  if(fQScopeAnalysis){
    fQScopeAnalysis->SetLogAmpElecDelayTime(fLogAmpElecDelayTime);
    return true;
  }
  else return false;
}

Bool_t QMuxScope::SetScopeOffset(Float_t aValue)
{
  //calling this method turns OFF the option to calculate the pre-trigger baseline offset.
  //to turn it back on and disregard the value set by this method, call QMuxScope::SetIsPreTrigCalculated();
  
  fScopeOffset = aValue;
  SetIsPreTrigCalculated(false);
  if(fQScopeAnalysis){
    fQScopeAnalysis->SetScopeOffset(GetScopeOffset());
    return true;
  }
  else return false;
}

Bool_t QMuxScope::IsGoodLogAmpParameters(void)
{
  //If the QScopeAnalysis class has been initialized, it will check for the 
  //logamp parameters in QScopeAnalysis.  Otherwise, it checks the local parameters
  //for valid values.  This way, the logamp parameters can be checked without
  //having to allocate memory for QScopeAnalysis.
  
  Bool_t theReturn;
  
  if(fQScopeAnalysis){
    theReturn = fQScopeAnalysis->IsGoodLogAmpParameters();
  }
  else {
    if(fLogAmpParam_a == kBadCalibrationValue || fLogAmpParam_b == kBadCalibrationValue ||
       fLogOffset == kBadCalibrationValue || 
       GetScopeOffset() == kBadCalibrationValue) theReturn = false;
    
    else if(fLogAmpParam_a*fLogAmpParam_b*fLogOffset*GetScopeOffset() == 0.0) theReturn = false;
    else theReturn = true;  //could put in some sanity checks here since we know approximately what the ranges of the
  //logamp parameters should be.  We should at least know the sign of the parameters!!    
    
  }

  return theReturn;
}

void QMuxScope::GetAltRiseTimeFWHM(Int_t &riset, Int_t &FWHM)
{
  // Hamish Leslie's alternative risetime and FHHM calculation
  if(InitQScopeAnalysis()){
    fQScopeAnalysis->GetAltRiseTimeFWHM(riset, FWHM);
  }
  
  return;
}


Bool_t QMuxScope::IsSame(Int_t anIndex, const QMuxScope *aMuxScope, Int_t aPrintMode)const{
  const Int_t *data=Data();
  const Int_t *data2=aMuxScope->Data();
  // skip over histogram
  Int_t nmin=(Int_t *)&fValues-data;
  Int_t nmax=nmin+sizeof(fValues);
  int i,j;
  Int_t n=DataN();
  for(j=i=0;i<=n;i++)if((i<nmin || i>= nmax) && data[i]!=data2[i])j++;
  if(j>0 &&aPrintMode==1){
    printf("QMuxScope:%d  Differences in words ",anIndex);
    for(i=0;i<=n;i++)if((i<nmin || i>= nmax) && data[i]!=data2[i])printf(" %d",i);
    printf("\n");
  }
  return (j==0);
}

  Bool_t QMuxScope::IsHe4(){
    return ( fNCDStringNumber==3 || fNCDStringNumber==10 ||
              fNCDStringNumber==20 || fNCDStringNumber==30 );  //true if string is a Helium 4 string
  }
