#include "QFit.h"
#include <stdlib.h>

//*-- Author : Chris Jillings
// Modified by M. Boulay                         

//____________________________________________________________________________
//
// QFit: Creates an object to contain the results of the position
//       and direction fits of an event, done in QPDF.
// 
ClassImp(QFit)


QFit::QFit(Float_t *xvars, Int_t aBits) {
  //Normal QFit constructor. Fills the fit-result data
  //as follows:
  //Begin_html
  //<l>strncpy(fName,aName,3);
  //<l>fPosition[0,1,2]  = xvars[0,1,2];
  //<l>fDirection[0,1,2] = xvars[3,4,5];
  //<l>fEnergy           = xvars[6];
  //<l>fTime             = xvars[7];
  //<l>fNumPMTsUsed      = (Int_t)xvars[8];
  //<l>fQualityOfFit     = xvars[9]; 
  //<l>fIndex            = aBits;
  //End_html
  Initialize();
  Set(xvars,aBits);

}

QFit::QFit() {
  Initialize();
}

QFit::QFit(Float_t *xvars, Int_t abits, char *name){
  Initialize();
  Set( xvars, abits );
  //  SetName( name );
}
QFit::~QFit() {
  // empty class
}

QFit::QFit( const QFit &fit )
{
  //Copy constructor for QFit objects. 

  //--MB
  
  fX = fit.fX;
  fY = fit.fY;
  fZ = fit.fZ;
  fU = fit.fU;
  fV = fit.fV;
  fW = fit.fW;
  fEnergy = fit.fEnergy;
  fTime = fit.fTime;
  fNumPMTsUsed = fit.fNumPMTsUsed;
  fQualityOfFit = fit.fQualityOfFit;
  fIndex = fit.fIndex;
	fOK = fit.fOK;
  C11 = fit.C11;
  C12 = fit.C12;
  C13 = fit.C13;
  C14 = fit.C14;
  C22 = fit.C22;
  C23 = fit.C23;
  C24 = fit.C24;
  C33 = fit.C33;
  C34 = fit.C34;
  C44 = fit.C44;

  int i;
  for(i=0;i<QFIT_MAX_FIT_OUTPUTS;i++)fFitOutput[i]=fit.fFitOutput[i];

  fThetaIJ=fit.fThetaIJ;
  fITR=fit.fITR;
  fSol_Dir=fit.fSol_Dir;
  fNeckLikelihood=fit.fNeckLikelihood;

  for(i=0;i<QFIT_MAX_BETAS;i++)fBeta[i]=fit.fBeta[i];
  for(i=0;i<QFIT_MAX_RESIDUAL_HARMONICS;i++)fResidualHarmonics[i]=fit.fResidualHarmonics[i];
  for(i=0;i<QFIT_MAX_RESIDUALS;i++)fResiduals[i]=fit.fResiduals[i];
  for(i=0;i<QFIT_MAX_LEGENDRES;i++)fLegendres[i]=fit.fLegendres[i];
  for(i=0;i<QFIT_MAX_ANG_CORRELATIONS;i++)fAngularCorrelation[i]=fit.fAngularCorrelation[i];
   fPhiKSAllHits = fit.fPhiKSAllHits;
   fPhiKSPromptHits = fit.fPhiKSPromptHits;
   fAngKSAllHits = fit.fAngKSAllHits;
   fAngKSPromptHits = fit.fAngKSPromptHits;
   fChargeThetaIJ = fit.fChargeThetaIJ;
   fPDTChargeProb = fit.fPDTChargeProb;
   fPDTMaxCharge = fit.fPDTMaxCharge;
   fPDTnTubes = fit.fPDTnTubes;
   fPDTnWindowTubes = fit.fPDTnWindowTubes;
   fTimeKS = fit.fTimeKS;
  }

QFit & QFit::operator=(const QFit &rhs )
{
  // QFit assignment operator.

  //--MB
       fX = rhs.fX;
       fY = rhs.fY;
       fZ = rhs.fZ;
       fU = rhs.fU;
       fV = rhs.fV;
       fW = rhs.fW;
       fEnergy = rhs.fEnergy;
       fTime = rhs.fTime;
       fNumPMTsUsed = rhs.fNumPMTsUsed;
       fQualityOfFit = rhs.fQualityOfFit;
       fIndex = rhs.fIndex;
       //   fName = rhs.fName;
       //fSpareI = rhs.fSpareI;
       //fSpareF = rhs.fSpareF;
       fOK = rhs.fOK;
       fNumIter = rhs.fNumIter;
       C11 = rhs.C11;
       C12 = rhs.C12;
       C13 = rhs.C13;
       C14 = rhs.C14;
       C22 = rhs.C22;
       C23 = rhs.C23;
       C24 = rhs.C24;
       C33 = rhs.C33;
       C34 = rhs.C34;
       C44 = rhs.C44;

       int i;
       for(i=0;i<QFIT_MAX_FIT_OUTPUTS;i++)fFitOutput[i]=rhs.fFitOutput[i];

       fThetaIJ=rhs.fThetaIJ;
       fITR=rhs.fITR;
       fSol_Dir=rhs.fSol_Dir;
       fNeckLikelihood=rhs.fNeckLikelihood;

       for(i=0;i<QFIT_MAX_BETAS;i++) fBeta[i]=rhs.fBeta[i];
       for(i=0;i<QFIT_MAX_RESIDUAL_HARMONICS;i++) fResidualHarmonics[i]=rhs.fResidualHarmonics[i];
       for(i=0;i<QFIT_MAX_RESIDUALS;i++) fResiduals[i]=rhs.fResiduals[i];
       for(i=0;i<QFIT_MAX_LEGENDRES;i++) fLegendres[i]=rhs.fLegendres[i];
       for(i=0;i<QFIT_MAX_ANG_CORRELATIONS;i++) fAngularCorrelation[i]=rhs.fAngularCorrelation[i];
       fPhiKSAllHits = rhs.fPhiKSAllHits;
       fPhiKSPromptHits = rhs.fPhiKSPromptHits;
       fAngKSAllHits = rhs.fAngKSAllHits;
       fAngKSPromptHits = rhs.fAngKSPromptHits;
       fChargeThetaIJ = rhs.fChargeThetaIJ;
       fPDTChargeProb = rhs.fPDTChargeProb;
       fPDTMaxCharge = rhs.fPDTMaxCharge;
       fPDTnTubes = rhs.fPDTnTubes;
       fPDTnWindowTubes = rhs.fPDTnWindowTubes;
       fTimeKS = rhs.fTimeKS;

       return *this;
}

const char * QFit::GetName()const
{
  //return the name of this QFit.
  // Presently set to return "QFit" since name is not set in the constructor.
  
  //if ( !strcmp( (const char *)fName,"" ) || fName=="" || strlen( (const char *)fName ) > 8 ) return "QFIT";
  //else return (const char *)fName;
  return (const char *)"QFit";
}

void QFit::FromFTXC(Float_t *source, Float_t *destination, Int_t max, Float_t backfill){
  // A routine designed to fill arrays in the tree (destination) 
  //  from arrays (source) in the  SNOMAN FTXC bank.
  // The first entry in the FTXC array is the number of entries ( stored as a float ! )
  // valid entries.
  // Max is the maximum number of elements in the destination.
	// To be consistent with SNOMAN we backfill with -999999 instead of -9999
	//
	// Note : Not always true !  SNOMAN _does_ use -9999 in some instances ( fFitOutputs for example ) so
	// change code to use a backfill variable. Set default to -999999. for backward compatibility
	//

  int i, n;
	n=((Int_t)source[0]>max)?n=max:n=(Int_t)source[0];
  for(i=0;i<n;i++){destination[i]=source[i+1];}
  for(;i<max;i++)destination[i]=backfill;
}


void QFit::Set(Float_t* xvars, Int_t aBits) {

	//
	// This function assumes that the xvars[] array contains valid data ...
	//
	Int_t i = 0, j;

  fX  = xvars[i++];
  fY  = xvars[i++];
  fZ  = xvars[i++];
  fU  = xvars[i++];
  fV  = xvars[i++];
  fW  = xvars[i++];
  fEnergy       = xvars[i++];
  fTime         = xvars[i++];
  fNumPMTsUsed  = (Int_t)xvars[i++];
  fQualityOfFit = xvars[i++]; 
  fIndex         = aBits;
  fOK            = (Bool_t)xvars[i++];
  fNumIter       = (Int_t)xvars[i++];
  C11            = xvars[i++];
  C12            = xvars[i++];
  C13            = xvars[i++];
  C14            = xvars[i++];
  C22            = xvars[i++];
  C23            = xvars[i++];
  C24            = xvars[i++];
  C33            = xvars[i++];
  C34            = xvars[i++];
  C44            = xvars[i++];

  for(j=0;j<QFIT_MAX_FIT_OUTPUTS;j++) fFitOutput[j]=xvars[i++];
  fThetaIJ       = xvars[i++];
  fITR           = xvars[i++];
  fSol_Dir       = xvars[i++];
  fNeckLikelihood= xvars[i++];

  FromFTXC(xvars+i,fBeta,QFIT_MAX_BETAS);
	i += QFIT_MAX_BETAS + 1;

  FromFTXC(xvars+i,fResidualHarmonics,QFIT_MAX_RESIDUAL_HARMONICS);
	i += QFIT_MAX_RESIDUAL_HARMONICS + 1;

  FromFTXC(xvars+i,fResiduals,QFIT_MAX_RESIDUALS);
	i += QFIT_MAX_RESIDUALS + 1;

  FromFTXC(xvars+i,fLegendres,QFIT_MAX_LEGENDRES);
	i += QFIT_MAX_LEGENDRES + 1;

  FromFTXC(xvars+i,fAngularCorrelation,QFIT_MAX_ANG_CORRELATIONS);
	i += QFIT_MAX_ANG_CORRELATIONS + 1;

   fPhiKSAllHits = xvars[i++];
   fPhiKSPromptHits = xvars[i++];
   fAngKSAllHits = xvars[i++];
   fAngKSPromptHits = xvars[i++];
   fChargeThetaIJ = xvars[i++];
   fPDTChargeProb = xvars[i++];
   fPDTMaxCharge = xvars[i++];
   fPDTnTubes = xvars[i++];
   fPDTnWindowTubes = xvars[i++];
   fTimeKS = xvars[i++];

}

Bool_t QFit::IsSame(Int_t anIndex, const QFit *aFit, Int_t aPrintMode)const{
  const Int_t *data=Data();
  const Int_t *data2=aFit->Data();
  const Float_t *fdata=(Float_t *)data;
  const Float_t *fdata2=(Float_t *)data2;
  int i,j;
  Int_t n=DataN();
	Int_t iOK = (Int_t *)&fOK - Data();

  for(j=i=0;i<=n;i++)if( (i==iOK?(Bool_t)data[i]:data[i])!=(i==iOK?(Bool_t)data2[i]:data2[i]) )j++;
  if(j>0 &&aPrintMode==1){
    printf("QFit:%d  Differences in words ",anIndex);
    for(i=0;i<=n;i++)if( (i==iOK?(Bool_t)data[i]:data[i])!=(i==iOK?(Bool_t)data2[i]:data2[i]) )
			printf("Entry %d Values: 0x%x 0x%x, as floating point %f %f\n",i,i==iOK?(Bool_t)data[i]:data[i],i==iOK?(Bool_t)data2[i]:data2[i],fdata[i],fdata2[i]);
    printf("\n");
  }
  return (j==0);
}
 

void QFit::Initialize() {
  fOK = kTRUE;
  //fName = "QFIT" ; 
}

Float_t QFit::GetTheta1()
{
  //Return the cosine of the angle between the fit direction vector, and a 
  //radial vector from the origin to the reconstructed vertex.
  Float_t cos_theta1;
  Float_t rfit = GetR();
  if ( rfit >= 0 )
    {
      cos_theta1 = 1./rfit*( fX*fU + fY*fV + fZ*fW );
    }
  else cos_theta1 = -1.1;
  return cos_theta1;
}

void QFit::SetCovar(Int_t ndata, Float_t *data)
{
  //Set the elements of the covariance matrix.
  C11 = data[0];
  C12 = data[1];
  C13 = data[2];
  C14 = data[3];
  C22 = data[4];
  C23 = data[5];
  C24 = data[6];
  C33 = data[7];
  C34 = data[8];
  C44 = data[9];

}
Float_t QFit::GetSigmaR()
{
  //Return sigma_r for this fit.
  
  Float_t diag = C11*fX*fX + C22*fY*fY + C33*fZ*fZ;
  Float_t offdiagxy = C12*fX*fY;
  Float_t offdiagxz = C13*fX*fZ;
  Float_t offdiagyz = C23*fY*fZ;
  
  Float_t sigR = 4*diag +  8*(offdiagxy+offdiagyz+offdiagxz);
  sigR = sqrt(sigR/(4*(fX*fX + fY*fY + fZ*fZ)));
  return sigR;
}

Float_t QFit::Beta14()
{
  //Return beta14 = beta1 + 4 beta4 for the current fit.
  if (!fBeta ) return -9999.;
  else
    return 4*acos(-1.0)*(0.02*fBeta[0] + 0.016) + 4*4*acos(-1.0)/9*(0.034*fBeta[3] - 0.002);
}

void QFit::Streamer(TBuffer &R__b)
{
   // Stream an object of class QFit. --M Boulay

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
      if (R__v > 8) 
	{
	  //Use ROOT 3 automatic I/O.
	  QFit::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
	  return;
	}
      TObject::Streamer(R__b); 	// Used only with files made with old versions
      R__b >> fX;		// of root and kept for back-compatibility.
      R__b >> fY;		// New versions of root have an automatic I/O
      R__b >> fZ;
      R__b >> fU;
      R__b >> fV;
      R__b >> fW;
      R__b >> fEnergy;
      R__b >> fTime;
      R__b >> fNumPMTsUsed;
      R__b >> fQualityOfFit;
      R__b >> fIndex;
  if (R__v > 4 )
	{
	  //  fName.Streamer(R__b);
	  if ( R__v < 7)
	    {
	      TArrayI d1;
	      TArrayF d2;
	      d1.Streamer(R__b);
	      d2.Streamer(R__b);
	    }
          R__b >> fOK;
	}
  if ( R__v > 5 )
    {
      R__b >> fNumIter;
      //fCurvature.Streamer( R__b );
      R__b >> C11;
      R__b >> C12;
      R__b >> C13;
      R__b >> C14;
      R__b >> C22;
      R__b >> C23;
      R__b >> C24;
      R__b >> C33;
      R__b >> C34;
      R__b >> C44;

    }
  if ( R__v > 7 ) 
    {
      R__b >> fFitOutput[PDStatAng2D]; 
      R__b >> fFitOutput[PDStatAngPhi];
      R__b >> fFitOutput[PDStatTim];
      R__b >> fFitOutput[PKSProbAng2D];
      R__b >> fFitOutput[PKSProbAngPhi];
      R__b >> fFitOutput[PKSProbTim];
      R__b >> fThetaIJ;
      R__b >> fITR;
    }
 } else {
   QFit::Class()->WriteBuffer(R__b, this);
   }
}
