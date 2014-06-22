////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFitLBPosition.cc
///
/// CLASS: LOCAS::LOCASFitLBPosition
///
/// BRIEF: Object to fit the laserball position for a specific run
///        and or set of runs
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASDB.hh"
#include "LOCASRun.hh"
#include "LOCASRunReader.hh"
#include "LOCASPMT.hh"
#include "LOCASFitLBPosition.hh"


#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/GroupVelocity.hh"
#include "RAT/DU/Utility.hh"
#include "RAT/Log.hh"

#include <string>
#include <map>

using namespace LOCAS;
using namespace RAT;
using namespace std;

#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#endif

ClassImp( LOCASFitLBPosition )

// Default Constructor
LOCASFitLBPosition::LOCASFitLBPosition( const LOCASRunReader& lRunReader, const std::string& geoFile )
{
  fRunReader = lRunReader;
  SetGeometryFile( geoFile );

  ////////////////////////////////////////////////////////////////////////////////////////////
  // First load all the PMT, LightPath and Group Velocity information from the RAT database //
  ////////////////////////////////////////////////////////////////////////////////////////////
  fRATDB = DB::Get();
  string data = getenv("GLG4DATA");
  Log::Assert( data != "", "DSReader::BeginOfRun: GLG4DATA is empty, where is the data?" );
  fRATDB->LoadDefaults();
  fRATDB->Load( geoFile );
  fRATDB->Load( fRATDB->GetLink( "DETECTOR" )->GetS( "pmt_info_file" ) );

  DU::Utility::Get()->BeginOfRun();
  fPMTInfo = DU::Utility::Get()->GetPMTInfo();
  fLightPath = DU::Utility::Get()->GetLightPathCalculator();
  fLightPathX = DU::Utility::Get()->GetLightPathCalculator();
  fLightPathY = DU::Utility::Get()->GetLightPathCalculator();
  fLightPathZ = DU::Utility::Get()->GetLightPathCalculator();
  fGVelocity = DU::Utility::Get()->GetGroupVelocity();
  //////////////////////////////////////////////////////////////

  fMrqX = LOCASVector(1,10000);//new Float_t[10001];//LOCASVector( 1, 10000 );
  fMrqY = LOCASVector(1,10000);//new Float_t[10001];//LOCASVector( 1, 10000 );
  fMrqSigma = LOCASVector(1,10000);//new Float_t[10001];//LOCASVector( 1, 10000 );

  fMrqParameters = LOCASVector(1,10000);//new Float_t[6];//LOCASVector( 1, 5 );
  fMrqVary = LOCASIntVector(1,10000);//new Int_t[6];//LOCASIntVector( 1, 5 );

  // for ( int p = 0; p < 6; p++ ){
  //   fMrqParameters[ p ] = 0.0;
  //   fMrqVary[ p ] = 0;
  // }

  // for ( int p = 0; p < 10001; p++ ){
  //   fMrqX[ p ] = 0.0;
  //   fMrqY[ p ] = 0.0;
  //   fMrqSigma[ p ] = 0.0;
  // }

  for ( Int_t i = 1; i <= 5; i++ ){ fMrqVary[ i ] = 1; } // All parameters vary...
  fMrqVary[ 5 ] = 0; // ...except for the speed of light

  fMrqCovariance = LOCASMatrix(1,5,1,5);//new Float_t*[ 6 ]; for ( int p = 0; p < 6; p++ ){ fMrqCovariance[ p ] = new Float_t[ 6 ]; }
  fMrqCurvature = LOCASMatrix(1,5,1,5);//new Float_t*[ 6 ]; for ( int p = 0; p < 6; p++ ){ fMrqCurvature[ p ] = new Float_t[ 6 ]; }

  // for ( int p = 0; p <6; p++ ){
  //   for ( int ig = 0; ig <6; ig++ ){
  //     fMrqCovariance[p][ig] = 0.0;
  //     fMrqCurvature[p][ig] = 0.0;
  //   }
  // }

  fChiSquare = 0.0;
  fChiArray = new Float_t[ 10001 ];
  fResArray = new Float_t[ 10001 ];

  // for ( int p = 0; p < 10001; p++ ){
  //   fChiArray[ p ] = 0.0;
  //   fResArray[ p ] = 0.0;
  // }

  fNElements = 10000;
}

// Default Constructor
LOCASFitLBPosition::~LOCASFitLBPosition()
{

  // delete[] fMrqX;
  // delete[] fMrqY;
  // delete[] fMrqSigma;
  // delete[] fMrqParameters;
  // delete[] fMrqVary;

  
  // for( int p = 0; p<6; p++ ){ delete[] fMrqCovariance[ p ]; }
  // delete[] fMrqCovariance;

  // for( int p = 0; p<6; p++ ){ delete[] fMrqCurvature[ p ]; }
  // delete[] fMrqCurvature;
  
  delete[] fChiArray;
  delete[] fResArray;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFitLBPosition::FitLBPosition( const Int_t runID )
{

  // Ge the run object from the reader based on the supplied run ID.
  fCurrentRun = fRunReader.GetLOCASRun( runID );
  fCurrentLBPos = fCurrentRun->GetLBPos();

  // Initialise the Levenberg-Marquardt working arrays.
  InitialiseArrays();
  // Check that the arrays initialised properly
  if ( !fArraysInitialised ){
    printf("LOCASFitLBPosition::FitLBPosition: Error, the arrays for the minimisation routines were not initialised! Abort");
    return;
  }

  printf("LOCASFitLBPosition::FitLBPosition: About to start LOCASFitLBPosition::MrqFit...");
  MrqFit( fMrqX, fMrqY, fMrqSigma, 
          fNPMTs, fMrqParameters, fMrqVary, 
          5, fMrqCovariance, fMrqCurvature, 
          &fChiSquare );

  printf("Number of PMTs in fit is: %d for 4 parameters", fNPMTs);
  printf("ChiSquare complete, ChiSquare: %f | Reduced ChiSquare: %f", fChiSquare, fChiSquare / ( (Float_t)(fNPMTs - 4) ));

}


//////////////////////////////////////
//////////////////////////////////////


void LOCASFitLBPosition::InitialiseArrays()
{
  
  // Initialise the value fo the counter for the number of PMTs in this fit.
  fNPMTs = 0;
  
  Int_t nGoodPMTs = 0;
  Int_t nBadPMTs = 0;
  
  printf("LOCASFitLBPosition::InitialiseArrays: About to initialise data point arrays for the laser ball fit...");
  
  // Get the value of Lambda for the laser ball run
  Float_t runLambda = fCurrentRun->GetLambda();
  
  std::map< Int_t, LOCASPMT >::iterator iPMT;
  
  // First calculate the mean Prompt Peak time and mean Prompt Peak width
  // These are the mean prompt peak times for the entire run
  // and the width of said prompt peak times
  Float_t promptRunTimeMean = 0.0;
  Float_t promptRunTimeWidth = 0.0;
  for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin();
        iPMT != fCurrentRun->GetLOCASPMTIterEnd();
        iPMT++ ){
    fCurrentPMT = &( iPMT->second );
    promptRunTimeMean += fCurrentPMT->GetPromptPeakTime();
  }
  
  if ( fCurrentRun->GetNPMTs() != 0 ){ promptRunTimeMean /= fCurrentRun->GetNPMTs(); }
  else{
    printf("LOCASFitLBPosition::InitialiseArrays: Error, run contains no PMTs!. Abort"); 
    return; 
  }
  
  // Now calculate the sigma for the run
  for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin();
        iPMT != fCurrentRun->GetLOCASPMTIterEnd();
        iPMT++ ){
    fCurrentPMT = &( iPMT->second );
    promptRunTimeWidth += TMath::Power( ( fCurrentPMT->GetPromptPeakWidth() - promptRunTimeMean ), 2 );
  }
  promptRunTimeWidth = TMath::Sqrt( promptRunTimeWidth / ( fCurrentRun->GetNPMTs() ) );
  
  // Now screen the PMTs to be included in the fit.
  for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin();
        iPMT != fCurrentRun->GetLOCASPMTIterEnd();
        iPMT++ ){
    
    fCurrentPMT = &( iPMT->second );
    
    // Check that the map index matches the PMT ID
    if ( fCurrentPMT->GetID() != iPMT->first ){
      printf("LOCASFitLBPosition::InitialiseArrays: Error, LOCASPMT map idnex doesn't match the PMT ID! Abort.");
      return;
    }
    
    // Check whether the PMT is suitable for inclusion in the fit.
    // See the definition of LOCASFitLBPosition::SkipPMT for the selection criteria
    if ( SkipPMT( fCurrentRun, fCurrentPMT ) ){ nBadPMTs++; continue; }
    else if ( fCurrentPMT->GetPromptPeakTime() < promptRunTimeMean - ( 3.0 * promptRunTimeWidth ) ){ nBadPMTs++; continue; }
    else if ( fCurrentPMT->GetPromptPeakTime() > promptRunTimeMean + ( 3.0 * promptRunTimeWidth ) ){ nBadPMTs++; continue; }
    else if ( fCurrentPMT->GetPromptPeakWidth() == 0 ){ nBadPMTs++; continue; }
    
    nGoodPMTs++;
    
    fMrqX[ fNPMTs + 1 ] = fCurrentPMT->GetID();
    fMrqY[ fNPMTs + 1 ] = ( fCurrentPMT->GetPromptPeakTime() ) + ( fCurrentPMT->GetTimeOfFlight() );
    fMrqSigma[ fNPMTs + 1 ] = fCurrentPMT->GetPromptPeakWidth() / TMath::Sqrt( (Float_t) (32 - 1 ) );
    
    fNPMTs++;
  }
  
  // Check that the number of 'good' PMTs and the number of 'bad' PMTs matches the total number of PMTs
  // stroed on the LOCASRun object
  if ( ( fCurrentRun->GetNPMTs() - nBadPMTs ) != nGoodPMTs ){
    printf("LOCASFitLBPosition::InitiialiseArrays(): Mismath between the number of (good) PMTs vs. (bad) PMTs: %d vs. %d", nGoodPMTs, nBadPMTs);
  }

  printf("N Good PMTs: %d",nGoodPMTs);
  
  // Seed the fit at a reasonable position
  fMrqParameters[ 1 ] = ( fCurrentLBPos.X() );
  fMrqParameters[ 2 ] = ( fCurrentLBPos.Y() );
  fMrqParameters[ 2 ] = ( fCurrentLBPos.Z() );
  // Laserball time ( = 0.0 )
  fMrqParameters[ 4 ] = 0.0;
  // Multiplier for the light speed
  fMrqParameters[ 5 ] = 1.0;
  
  fArraysInitialised = true;
    
}

//////////////////////////////////////
//////////////////////////////////////


Bool_t LOCASFitLBPosition::SkipPMT( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Bool_t skipPMT = false;

  if ( fCurrentPMT->GetBadPath() ){ skipPMT = true; return skipPMT; }
  else if ( fCurrentPMT->GetMPECorrOccupancyErr() > ( 0.1 * fCurrentPMT->GetMPECorrOccupancy() ) ){ skipPMT = true; return skipPMT; }
  else{ return skipPMT; }
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFitLBPosition::MrqFit(float x[], float y[], float sig[], int ndata, float a[],
                                 int ia[], int ma, float **covar, float **alpha, float *chisq )
{
  //Fit these data using mrqmin() repeatedly until convergence is achieved.
  
  Int_t maxiter = 1000;
  Int_t numiter = 0;
  Int_t gooditer = 0;
  Int_t retval = 0;
  
  Float_t oldchisq = 0;
  Float_t lambda = -1.0;
  Float_t tol = 1.0;    // Chisquared must change by tol to warrant another iteration
  
  *chisq = 0;
  
  // First, call mrqmin with lambda = -1 for initialization
  printf("Calling mrqmin for initialization...\n");
  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lambda);
  printf("Done.  Chisq = %f\n",*chisq);
  oldchisq = *chisq;
  printf("CHISQ at origin = %12.5g\n",*chisq);
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lambda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
         && retval == 0 && lambda != 0.0) {
    oldchisq = *chisq;
    printf("Iterating with lambda %g...\n",lambda);
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lambda );
    printf("New chisq = %12.8g with lambda %g \n",*chisq,lambda);
    numiter++;
    
    if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lambda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  
  lambda = 0;
  retval = mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lambda );
  printf("Finished MrqFit");
  return retval;
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFitLBPosition::mrqmin( float x[], float y[], float sig[], int ndata, float a[],
                                  int ia[], int ma, float **covar, float **alpha, float *chisq,
                                  float *alamda)
{
  // mrqmin: Non-Linear Least Squares Minimization following the Levenberg-Marquardt method
  //
  // Comments from Numerical Recipes
  //
  // Levenberg-Marquardt method, attempting to reduce the value chi2 of a fit between a set of
  // data points x[1..ndata], y[1..ndata] with individual standard deviations sig[1..ndata], and a
  // nonlinear function dependent on ma coefficients a[a..ma]. The input array ia[1..ma] indicates
  // by nonzero entries those components of a that should be fitted for, and by zero entries those
  // components that should be held fixed at their input values. The program returns current
  // best-fit values for the paramenters a[1..ma], and chi2 = chisq. The arrays
  // covar[1..ma][1..ma],  alpha[1..ma][1..ma] are used as working space during most iterations.
  // On the first call provide an initial guess for the parameters a, and set alamda<0 for
  // initialization (which then sets alamda=.001). If a step succeeds chisq becomes smaller and
  // alamda decreases by a factor of 10. If a step fails alamda grows by a factor 10. You must call
  // this routine repeatedly until convergence is achieved. Then make one final call with alamda=0,
  // so that covar[1..ma][1..ma] returns the covariance matrix, and alpha the curvature matrix.
  // (Parameters held fixed will return zero covariances).   
  
  //void covsrt(float **covar, int ma, int ia[], int mfit);
  //void gaussj(float **a, int n, float **b, int m);
  //void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
  //	int ia[], int ma, float **alpha, float beta[], float *chisq,
  //	void (*funcs)(float, float [], float *, float [], int));
	int j,k,l,m, retval = 0;
	static int mfit;
	static float ochisq,*atry,*beta,*da,**oneda;

	if (*alamda < 0.0) {
		atry=LOCASVector(1,ma);
		beta=LOCASVector(1,ma);
		da=LOCASVector(1,ma);
		for (mfit=0,j=1;j<=ma;j++)
			if (ia[j]) mfit++;
		oneda=LOCASMatrix(1,mfit,1,1);
		*alamda=0.001;
		mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq);
		ochisq=(*chisq);
		for (j=1;j<=ma;j++) atry[j]=a[j];
	}
	for (j=0,l=1;l<=ma;l++) {
		if (ia[l]) {
			for (j++,k=0,m=1;m<=ma;m++) {
				if (ia[m]) {
					k++;
					covar[j][k]=alpha[j][k];
				}
			}
			covar[j][j]=alpha[j][j]*(1.0+(*alamda));
			oneda[j][1]=beta[j];
		}
	}
	retval = gaussj(covar,mfit,oneda,1);
	for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
	if (*alamda == 0.0 ) {
		covsrt(covar,ma,ia,mfit);
		LOCASFree_Matrix(oneda,1,mfit,1,1);
		LOCASFree_Vector(da,1,ma);
		LOCASFree_Vector(beta,1,ma);
		LOCASFree_Vector(atry,1,ma);
		return retval;
	}
	for (j=0,l=1;l<=ma;l++)
		if (ia[l]) atry[l]=a[l]+da[++j];
	//printf("TRY VECTOR: %8.2f %8.2f %8.2f %8.2f\n",atry[1],atry[2],atry[3],atry[4]);
	mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq);
	//printf("mrqmin:  chisq = %f\n",*chisq);
	if (*chisq < ochisq) {
		*alamda *= 0.1;
		ochisq=(*chisq);
		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				for (j++,k=0,m=1;m<=ma;m++) {
					if (ia[m]) {
						k++;
						alpha[j][k]=covar[j][k];
					}
				}
				beta[j]=da[j];
				a[l]=atry[l];
			}
		}
	} else {
		*alamda *= 10.0;
		*chisq=ochisq;
	}
	return retval;

  // // Minimization routine for a single iteration over the data points.

  // // Required helper routines:
  // // void covsrt(float **covar, int ma, int ia[], int mfit);
  // // void gaussj(float **a, int n, float **b, int m);
  // // void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
  // //	int ia[], int ma, float **alpha, float beta[], float *chisq,
  // //	void (*funcs)(float, float [], float *, float [], int));

  // int j,k,l,m, retval = 0;
  // static int mfit = 0;
  // static float ochisq,*atry,*beta,*da,**oneda;

  // //--------------------
  // // Initialization  
  // if (*alambda < 0.0) {
  //   atry = LOCASVector( 1, ma );
  //   beta = LOCASVector( 1, ma );
  //   da = LOCASVector( 1, ma );
  //   //atry= new Float_t[ma+1];for ( int ik = 0; ik < ma+1; ik++ ) atry[ ik ] = 0.0;
  //   //beta= new Float_t[ma+1]; for ( int ik = 0; ik < ma+1; ik++ ) beta[ ik ] = 0.0;
  //   //da= new Float_t[ma+1]; for ( int ik = 0; ik < ma+1; ik++ ) da[ ik ] = 0.0;
  //   for (mfit=0,j=1;j<=ma;j++)
  //     if (ia[j]) mfit++;
  //   oneda = LOCASMatrix( 1, ma, 1, 1 );//new Float_t*[ mfit + 1]; for ( int p = 0; p<mfit+1; p++ ){ oneda[ p ] = new Float_t[mfit + 1]; }
  //   // for ( int p = 0; p <mfit+1; p++ ){
  //   //   for ( int ig = 0; ig <mfit+1; ig++ ){
  //   //     oneda[p][ig] = 0.0;
  //   //   }
  //   // }
  //   *alambda=0.001;
  //   mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq);
  //   ochisq=(*chisq);
  //   for (j=1;j<=ma;j++) atry[j]=a[j];
  // }

  // //--------------------
  // // Bookkeeping on covariance and derivatives to prepare next parameter set.
  // for (j=0,l=1;l<=ma;l++) {
  //   //printf("Step1.\n");
  //   if (ia[l]) {
  //     //printf("Step2.\n");
  //     for (j++,k=0,m=1;m<=ma;m++) {
  //       //printf("Step3.\n");
  //       if (ia[m]) {
  //         //printf("Step4.\n");
  //         k++;
  //         covar[j][k]=alpha[j][k];
  //         //printf("Step5.\n");
  //       }
  //     }
  //     //printf("Step6.\n");
  //     covar[j][j]=alpha[j][j]*(1.0+(*alambda));
  //     //printf("Step7.\n");
  //     //printf("beta[j] is %f", beta[j]);
  //     //printf("Value of j is: %i", j );
  //     //printf("Value of beta[j] is: %f", beta[j]);
  //     //printf("oneda[j][1]: %f", oneda[j][1]);
  //     oneda[j][1]=beta[j];
  //     //printf("Step8.\n");
  //   }
  //   //printf("Step8.\n");
  // }
  // //printf("Step9.\n");
  // retval = gaussj(covar,mfit,oneda,1);
  // //printf("Step10.\n");
  // for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
  // //printf("Step11.\n");

  // printf("value of lambda is %f", *alambda );

  // //--------------------
  // // Final call to prepare covariance matrix and deallocate memory.
  // if ((*alambda) == 0.0 ) {
  //   printf("HELLO?");
  //   printf("IS THIS EVER CALLED?");
  //   covsrt(covar,ma,ia,mfit);
  //   //for ( int p = 0; p < mfit+1; p++ ){ delete[] oneda[ p ]; }
  //   //delete[] da;
  //   //delete[] beta;
  //   //delete[] atry;
  //   LOCASFree_Matrix(oneda,1,mfit,1,1);
  //   LOCASFree_Vector(da,1,ma);
  //   LOCASFree_Vector(beta,1,ma);
  //   LOCASFree_Vector(atry,1,ma);
  //   return retval;
  // }

  // //--------------------
  // // Set up the trial parameters and try them
  // for (j=0,l=1;l<=ma;l++)
  //   if (ia[l]) atry[l]=a[l]+da[++j];
  // mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq);
  // if (*chisq < ochisq) {
  //   *alambda *= 0.1;
  //   ochisq=(*chisq);
  //   for (j=0,l=1;l<=ma;l++) {
  //     if (ia[l]) {
  //       for (j++,k=0,m=1;m<=ma;m++) {
  //         if (ia[m]) {
  //           k++;
  //           alpha[j][k]=covar[j][k];
  //         }
  //       }
  //       beta[j]=da[j];
  //       a[l]=atry[l];
  //     }
  //   }
  // } 
  // else {
  //   *alambda *= 10.0;
  //   *chisq=ochisq;
  // }
  // return retval;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFitLBPosition::mrqcof(float x[], float y[], float sig[], int ndata, float a[],
                                int ia[], int ma, float **alpha, float beta[], float *chisq)
{
  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  float chisqentry;  // chi-squared for single entry in list
  
  dyda=LOCASVector(1,ma);
  for (j=1;j<=ma;j++) {
    if (ia[j]) mfit++;
    dyda[j] = 0.0;
  }
  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) alpha[j][k]=0.0;
    beta[j]=0.0;
  }
  *chisq=0.0;

  fCurrentLBPos.SetXYZ(a[1], a[2], a[3]);
  //fQOpticsX->SetSource(a[1] + fdelpos, a[2], a[3]); 
  //fQOpticsY->SetSource(a[1], a[2] + fdelpos, a[3]);
  //fQOpticsZ->SetSource(a[1], a[2], a[3] + fdelpos);

  for (i=1;i<=ndata;i++) {
    mrqfuncs( x[i],i,a,&ymod,dyda,ma);
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;
    for (j=0,l=1;l<=ma;l++) {
      if (ia[l]) {
        wt=dyda[l]*sig2i;
        for (j++,k=0,m=1;m<=l;m++)
          if (ia[m]) alpha[j][++k] += wt*dyda[m];
        beta[j] += dy*wt;
        //printf( "beta[j]: %f \n", beta[1] );
        //printf( "dy: %f \n", dy );
        //printf( "wt: %f \n -----------", wt );
      }
    }
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    if (i%1==2000) printf(" %d %f %f\n",i,x[i],chisqentry);
    if (fChiArray!=NULL && i>=0 && i<fNElements) fChiArray[i] = chisqentry;
    if (fResArray!=NULL && i>=0 && i<fNElements) fResArray[i] = dy;
  }
  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
  LOCASFree_Vector(dyda,1,ma);
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFitLBPosition::mrqfuncs(Float_t x, Int_t i, Float_t a[], Float_t *y,
                                  Float_t dyda[], Int_t )
{
  //Function used by mrqcof()
  //   a[]    = parameters to be fit:
  //              x, y, z of the laserball
  //              t of the laserball light pulse
  //   x[]    = indices of pmts in fCurrentrun
  //   y      = model value = D_path / V_group + t0
  //   dyda[] = derivative of y w.r.t. input parameters
  //   na     = number of parameters
  //


  if ( !fCurrentRun ) {
    printf("LOCASFitLBPosition::mrqfuncs: Error, now LOCASRun object being pointed to! Abort");
    return;
  }
   
  Int_t ix = (Int_t)x;
  LOCASPMT *pmt = &( fCurrentRun->GetPMT(ix) );
  Int_t pmtn = pmt->GetID();
  TVector3 srcvec(fCurrentLBPos);          // just in case GetDistances
  TVector3 pmtvec(fPMTInfo.GetPosition(pmtn)); // falls through...

  Int_t panel;
  Double_t dd2o,dh2o,dacr,cospmt,transpwr; // For the model value
  Double_t dd2od,dh2od,dacrd;              // For derivatives.

  // NB. fQOptics source position set in mrqcof above, once for the whole loop!

  Double_t psmag; // Magnitude of source->pmt vector

  psmag = (pmtvec - srcvec).Mag();
  *y = psmag * a[5] +a[4];
  dyda[1] = - (pmtvec(0) - srcvec(0)) * a[5] / psmag;
  dyda[2] = - (pmtvec(1) - srcvec(1)) * a[5] / psmag;
  dyda[3] = - (pmtvec(2) - srcvec(2)) * a[5] / psmag;
  dyda[4] = 1.0;
  dyda[5] = psmag;
  
  //--------------------
  // Double_t tflight; // Time of flight along QOptics path
//   Double_t lightspeed; // Distance weighted speed of light...

//   if (fFitLBPosition >= 2) {
//     if (fQOptics->GetDistances(pmtn,dd2o,dacr,dh2o,cospmt,panel,transpwr)) {
      
//       tflight = (dd2o/fVgroupd2o + dacr/fVgroupacr + dh2o/fVgrouph2o);
//       lightspeed =
// 	(fVgroupd2o*dd2o + fVgroupacr*dacr + fVgrouph2o*dh2o) / (dd2o+dacr+dh2o);
//       *y = tflight * a[5] + a[4];

//       //ALH: 2/19/2005--- i commented out the following lines, which are to implement the cos-theta timing correction for large angle hits in the light water.
//       // we do want this, but I dislike calculating this every time-- so when we get to 
//       // light water optics again we should think about efficiency.  We also 
//       //probably want to correct the time calibration, and not the model, but 
//       // we can discuss that.  

          
// //       Float_t mymanx = fCurrentrun->GetManipx();
// //       Float_t mymany = fCurrentrun->GetManipy();
// //       Float_t mymanz = fCurrentrun->GetManipz();
// //       Float_t mypmx = pmt->GetPmtx();
// //       Float_t mypmy = pmt->GetPmty();
// //       Float_t mypmz = pmt->GetPmtz();

// //       Float_t mydpmt = sqrt((mymanx-mypmx)*(mymanx-mypmx) +(mymany-mypmy)*(mymany-mypmy) +(mymanz-mypmz)*(mymanz-mypmz));
     

// //      if (mydpmt >700 && mydpmt < 1100) {
// //	if (cospmt >=0.45 && cospmt<0.75) {
// //	  *y = tflight* a[5] +a[4] + (-2.6045+3.47265*cospmt);}

	
// // 	Double_t xvar = -21.9651*cospmt+7.61297;
	
// // 	if (cospmt < 0.45 && cospmt>0.25) {
// // 	  *y = tflight * a[5] + a[4] - exp(xvar);}
// //       }

// //       if (mydpmt<300) {
// // 	if (cospmt<=0.45 && cospmt>0.125){
// // 	  *y = tflight*a[5] +a[4] +(22.767-300.197*cospmt+1590.37*cospmt*cospmt-3638.38*cospmt*cospmt*cospmt+2970.86*cospmt*cospmt*cospmt*cospmt);}
// //       }


      
//       if (i%1000==0 && fPrint>=2)
// 	printf("%10d %10d %10.3f %10.3f %10.3f %10.3f\n",i,pmtn,dd2o/fVgroupd2o,
// 	       dacr/fVgroupacr, dh2o/fVgrouph2o, *y);
      
//       //--------------------

//       if (fQOpticsX->GetDistances(pmtn,dd2od,dacrd,dh2od,cospmt,panel,transpwr)) {
// 	dyda[1] =
// 	  ((dd2od-dd2o)/fVgroupd2o + (dacrd-dacr)/fVgroupacr + (dh2od-dh2o)/fVgrouph2o)
// 	  / fdelpos * a[5];
//       } else {
// 	dyda[1] = - (pmtvec(0) - srcvec(0))*(a[5]/lightspeed)/(pmtvec - srcvec).Mag();
//       }
      
//       //--------------------
//       if (fQOpticsY->GetDistances(pmtn,dd2od,dacrd,dh2od,cospmt,panel,transpwr)) {
// 	dyda[2] =
// 	  ((dd2od-dd2o)/fVgroupd2o + (dacrd-dacr)/fVgroupacr + (dh2od-dh2o)/fVgrouph2o)
// 	  / fdelpos * a[5];
//       } else {
// 	dyda[2] = - (pmtvec(1) - srcvec(1))*(a[5]/lightspeed)/(pmtvec - srcvec).Mag();
//       }
      
//       //--------------------
//       if (fQOpticsZ->GetDistances(pmtn,dd2od,dacrd,dh2od,cospmt,panel,transpwr)) {
// 	dyda[3] =
// 	  ((dd2od-dd2o)/fVgroupd2o + (dacrd-dacr)/fVgroupacr + (dh2od-dh2o)/fVgrouph2o)
// 	  / fdelpos * a[5];
//       } else {
// 	dyda[3] = -(pmtvec(2) - srcvec(2))*(a[5]/lightspeed)/(pmtvec - srcvec).Mag();
//       }
      
//       //--------------------
//       dyda[4] = 1.0;
//       dyda[5] = tflight;

//     } else {
//       if (fPrint>=2) printf("QOptics failure %d !\n",pmtn);
//       *y = fmrqy[i];  // If we can't calculate the path, set model = data - Hmmmm...
//       dyda[1] = 0.0;
//       dyda[2] = 0.0;
//       dyda[3] = 0.0;
//       dyda[4] = 0.0;
//       dyda[5] = 0.0;
//     }
//   }
  
  //--------------------
  if (i%1000==0)
    printf("%12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g %12.6g\n",
	   a[1], dyda[1], a[2], dyda[2], a[3], dyda[3], a[4], dyda[4], a[5], dyda[5]);
  
  // fNcalls++;
  // if (fNcalls%fNmessage == 0 && fPrint>=1)
  //   printf("In QPath::mrqfuncs() at call %d\n",fNcalls);
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFitLBPosition::covsrt(float **covar, int ma, int ia[], int mfit)
{
  // Covariance matrix sorting.  Helper routine for mrqmin()

  int i,j,k;
  float swap;

  for (i=mfit+1;i<=ma;i++)
    for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
  k=mfit;
  for (j=ma;j>=1;j--) {
    if (ia[j]) {
      for (i=1;i<=ma;i++) { SWAP(covar[i][k],covar[i][j]) }
      for (i=1;i<=ma;i++) { SWAP(covar[k][i],covar[j][i]) }
      k--;
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFitLBPosition::gaussj(float **a, int n, float **b, int m)
{
  // Gauss-Jordan matrix solution helper routine for mrqmin.

  int *indxc,*indxr,*ipiv;
  int i,icol,irow,j,k,l,ll = 0;
  float big,dum,pivinv,swap = 0.0;
  Int_t retval = 0;
  indxc= LOCASIntVector(1,n);//new Int_t[n+1]; for ( int p = 0; p <n+1; p++ ){ indxc[ p ] = 0.0; }
  indxr= LOCASIntVector(1,n);//new Int_t[n+1]; for ( int p = 0; p <n+1; p++ ){ indxr[ p ] = 0.0; }
  ipiv= LOCASIntVector(1,n);//new Int_t[n+1]; for ( int p = 0; p <n+1; p++ ){ ipiv[ p ] = 0.0; }
  for (j=1;j<=n;j++) ipiv[j]=0;
  for (i=1;i<=n;i++) {
    big=0.0;
    for (j=1;j<=n;j++)
      if (ipiv[j] != 1)
        for (k=1;k<=n;k++) {
          if (ipiv[k] == 0) {
            if (fabs(a[j][k]) >= big) {
              big=fabs(a[j][k]);
              irow=j;
              icol=k;
            }
          } 
          else if (ipiv[k] > 1){ 
            //nrerror("gaussj: Singular Matrix-1");
            printf("gaussj - Singular Matrix-1");
            retval = -1;
          }
        }
    ++(ipiv[icol]);
    if (irow != icol) {
      for (l=1;l<=n;l++) { SWAP(a[irow][l],a[icol][l]) }
      for (l=1;l<=m;l++) { SWAP(b[irow][l],b[icol][l]) }
    }
    indxr[i]=irow;
    indxc[i]=icol;
    if (a[icol][icol] == 0.0) 
      {
        //nrerror("gaussj: Singular Matrix-2");
        printf("gaussj - Singular Matrix-2");
        retval = -2;
      }
    pivinv=1.0/a[icol][icol];
    a[icol][icol]=1.0;
    for (l=1;l<=n;l++) a[icol][l] *= pivinv;
    for (l=1;l<=m;l++) b[icol][l] *= pivinv;
    for (ll=1;ll<=n;ll++)
      if (ll != icol) {
        dum=a[ll][icol];
        a[ll][icol]=0.0;
        for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
        for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
      }
  }
  for (l=n;l>=1;l--) {
    if (indxr[l] != indxc[l])
      for (k=1;k<=n;k++)
        SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  }
  //delete[] ipiv;
  //delete[] indxr;
  //delete[] indxc;
  LOCASFree_IntVector(ipiv,1,n);
  LOCASFree_IntVector(indxr,1,n);
  LOCASFree_IntVector(indxc,1,n);
  return retval;
}
