////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFitLBPosition.cc
///
/// CLASS: LOCAS::LOCASFitLBPosition
///
/// BRIEF: Object to fit the laserball position for a specific run
///        and/or set of runs
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

#include "QDQXX.h"
#include "QPMTxyz.h"
#include "QOptics.h"

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

  // Set the path prefix for the location of the DQXX files
  // This assumes that the envrionment variable $DQXXDIR is set
  fDQXXDirPrefix = getenv("DQXXDIR");
  fDQXXDirPrefix += "/DQXX_00000";
  
  //printf( "Initialising Laserball Position Manager\n" );
  //printf( "-----------------------------------------\n \n" );
  ////////////////////////////////////////////////////////////////////////////////////////////
  // First load all the PMT, LightPath and Group Velocity information from the RAT database //
  ////////////////////////////////////////////////////////////////////////////////////////////
  fRATDB = DB::Get();
  string data = getenv("GLG4DATA");
  Log::Assert( data != "", "DSReader::BeginOfRun: GLG4DATA is empty, where is the data?" );
  fRATDB->LoadDefaults();
  fRATDB->Load( geoFile );
  fRATDB->Load( fRATDB->GetLink( "DETECTOR" )->GetS( "pmt_info_file" ) );
  cout << "PMT Databse File: " << (fRATDB->GetLink( "DETECTOR" )->GetS( "pmt_info_file" )).c_str() << endl;

  fScintVolMat =  fRATDB->GetLink( "GEO", "scint" )->GetS( "material" );
  fAVVolMat = fRATDB->GetLink( "GEO", "av" )->GetS( "material" );
  fCavityVolMat = fRATDB->GetLink( "GEO", "cavity" )->GetS( "material" );

  //printf( "Inner AV material: %s\n", fScintVolMat.c_str() );
  //printf( "AV material: %s\n", fAVVolMat.c_str() );
  //printf( "Cavity material: %s\n", fCavityVolMat.c_str() );

  DU::Utility::Get()->BeginOfRun();
  fPMTInfo = DU::Utility::Get()->GetPMTInfo();
  fLightPath = DU::Utility::Get()->GetLightPathCalculator();
  fLightPathX = DU::Utility::Get()->GetLightPathCalculator();
  fLightPathY = DU::Utility::Get()->GetLightPathCalculator();
  fLightPathZ = DU::Utility::Get()->GetLightPathCalculator();
  fGVelocity = DU::Utility::Get()->GetGroupVelocity();
  //////////////////////////////////////////////////////////////

  fMrqX = LOCASVector( 1, 10000 );
  fMrqY = LOCASVector( 1, 10000 );
  fMrqSigma = LOCASVector( 1, 10000 );

  fMrqParameters = LOCASVector( 1, 4 );
  fMrqVary = LOCASIntVector( 1, 4 );

  for ( Int_t i = 1; i <= 4; i++ ){ fMrqVary[ i ] = 1; } // All parameters vary...

  fMrqCovariance = LOCASMatrix( 1, 4, 1, 4 );
  fMrqCurvature = LOCASMatrix( 1, 4, 1, 4 );

  fChiSquare = 0.0;
  fChiArray = new Float_t[ 10001 ];
  fResArray = new Float_t[ 10001 ];

  fNElements = 10000;
  fDelPos = 10.0;
}

// Default Constructor
LOCASFitLBPosition::~LOCASFitLBPosition()
{
 
  // Delete the chisquare and chisquare residual arrays
  // in the destructor
  delete[] fChiArray;
  delete[] fResArray;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFitLBPosition::FitLBPosition( const Int_t runID )
{
  
  // Get the pointer to the LOCASRun object from the reader based on the supplied run ID.
  fCurrentRun = fRunReader.GetLOCASRun( runID );

  //printf( "LOCASFitLBPosition::FitLBPosition: Fitting for Run: %i\n", runID );
  //printf( "LOCASFitLBPosition::FitLBPosition: Using 500 nm Run: %i\n", fCurrentRun->GetWavelengthRunID() );
  stringstream tmpStream;
  tmpStream << (fCurrentRun->GetWavelengthRunID());
  string tmpStr = "";
  tmpStream >> tmpStr;

  std::string dqxxPath = fDQXXDirPrefix + tmpStr + ".dat";
  cout << "LOCASFitLBPosition::FitLBPosition: Loading DQXX file:\n" << dqxxPath << endl;
  fDQXX.ReadTitles( dqxxPath.c_str() );

  // Get the laserball position from the LOCASRun object. The laserball position
  // supplied here is from the manipulator
  fCurrentLBPos = fCurrentRun->GetWavelengthLBPos();

  // Initialise the Levenberg-Marquardt working arrays.
  InitialiseArrays();

  // Check that the arrays initialised properly
  if ( !fArraysInitialised ){
    printf("LOCASFitLBPosition::FitLBPosition: Error, the arrays for the minimisation routines were not initialised! Abort\n");
    return;
  }

  // Begin the fitting routine
  //printf("LOCASFitLBPosition::FitLBPosition: Initial Parameter Values for run %i (off-axis %i):\n\n", fCurrentRun->GetWavelengthRunID(), runID );
  //printf("Laserball x-coordinate: %f mm\n", fMrqParameters[ 1 ] );
  //printf("Laserball y-coordinate: %f mm\n", fMrqParameters[ 2 ] );
  //printf("Laserball z-coordinate: %f mm\n", fMrqParameters[ 3 ] );
  //printf("Laserball time (centered): %f ns\n", fMrqParameters[ 4 ] );

  //printf("LOCASFitLBPosition::FitLBPosition: About to start LOCASFitLBPosition::MrqFit...\n");
  //printf("ChiSquare Value is: %f", fChiSquare );

  MrqFit( fMrqX, fMrqY, fMrqSigma, 
          fNPMTs, fMrqParameters, fMrqVary, 
          4, fMrqCovariance, fMrqCurvature, 
          &fChiSquare );

  PrintFitInfo();

  //cout << "XPos: " << fMrqParameters[ 1 ] << endl;
  //cout << "YPos: " << fMrqParameters[ 2 ] << endl;
  //cout << "ZPos: " << fMrqParameters[ 3 ] << endl;
  //cout << "TPos: " << fMrqParameters[ 4 ] << endl; 

  //printf( "Number of PMTs in fit is: %d for 4 parameters\n", fNPMTs );
  //printf( "ChiSquare complete:\nChiSquare: %f\nReduced ChiSquare: %f\n", fChiSquare, fChiSquare / ( (Float_t)(fNPMTs - 3) ) );
  //printf( "-----------------------------------------\n \n" );

}


//////////////////////////////////////
//////////////////////////////////////


void LOCASFitLBPosition::InitialiseArrays()
{  
  
  fArraysInitialised = false;
  fLightPath.ResetValues();
  fLightPathX.ResetValues();
  fLightPathY.ResetValues();
  fLightPathZ.ResetValues();
  fChiSquare = 0.0;
  for ( Int_t iD = 1; iD <= 10000; iD++ ){
    fMrqX[ iD ] = 0.0;
    fMrqY[ iD ] = 0.0;
    fMrqSigma[ iD ] = 0.0;
    fChiArray[ iD ] = 0.0;
    fResArray[ iD ] = 0.0;
  }

  for ( Int_t iP = 1; iP <= 4; iP++ ){
    fMrqParameters[ iP ] = 0.0;
    for ( Int_t iJ = 1; iJ <= 4; iJ++ ){
      fMrqCovariance[ iP ][ iJ ] = 0.0;
      fMrqCurvature[ iP ][ iJ ] = 0.0;
    }
  }
  
  // Initialise the value of the counter for the number of PMTs in this fit.
  fNPMTs = 0;
  
  // These will count the number of 'good' PMTs: i.e. those with reasonable values and for which the light
  // path was wel calculated.
  Int_t nGoodPMTs = 0;
  Int_t nBadPMTs = 0;
  
  //printf("LOCASFitLBPosition::InitialiseArrays: About to initialise data point arrays for the laser ball fit...");
  
  // Get the value of Lambda for the laser ball run
  Float_t runLambda = fCurrentRun->GetWavelengthLambda();
  
  // The iterator object, the scan through the LOCASPMT map.
  std::map< Int_t, LOCASPMT >::iterator iPMT;
  
  // Calculate the mean prompt peak time width from all PMTs in the run.
  // Also calculate the sigma, of all the prompt peak time widths in the run.

  Float_t promptRunTimeWidthMean = 0.0;                // Mean prompt peak time width for the entire run
  Float_t promptRunTimeWidthMean2 = 0.0;               // Mean prompt peak time width (squared) for the entire run
  Double_t promptRunTimeWidth = 0.0;                   // Sigma of the prompt peak time widths

  Int_t iK = KCCC_TUBE_ON_LINE;
  Int_t nPMTsTmp = 0;
  for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin(); 
        iPMT != fCurrentRun->GetLOCASPMTIterEnd(); 
        iPMT++ ){
    if ( fDQXX.LcnInfo( iPMT->first, iK ) == 1 ){
      fCurrentPMT = &( iPMT->second );
      promptRunTimeWidthMean += fCurrentPMT->GetWavelengthPromptPeakWidth(); 
      promptRunTimeWidthMean2 += TMath::Power( fCurrentPMT->GetWavelengthPromptPeakWidth(), 2 );
      nPMTsTmp++;
    }
    
  }

  //printf("timesigmamean is: %f\n",promptRunTimeWidthMean);
  //printf("timesigmameansqd is: %f\n",promptRunTimeWidthMean2);
  
  Int_t nPMTs = fCurrentRun->GetNPMTs();
  // Divide through by the number of PMTs to obtain the mean
  if ( nPMTsTmp != 0 ){ 
    promptRunTimeWidthMean /= nPMTsTmp;
    promptRunTimeWidthMean2 /= nPMTsTmp;
  }
  else{
    printf("LOCASFitLBPosition::InitialiseArrays: Error, run contains no PMTs! Abort"); 
    return; 
  }

  // Calculate the sigma for the prompt peak time widths
  Double_t resSquared = ( promptRunTimeWidthMean2 - TMath::Power( promptRunTimeWidthMean, 2 ) );
  resSquared *= ( (Double_t)(nPMTsTmp / ( nPMTsTmp - 1.0 ) ) );
  promptRunTimeWidth = TMath::Sqrt( resSquared );
  
  cout << "Time Width Mean is: " << promptRunTimeWidthMean << endl;
  cout << "Time Width Deviation is: " << promptRunTimeWidth << endl;
  // Now screen the PMTs to be included in the fit.
  // This checks for bad light paths, reasonable occupancy errors (i.e. MPE corrections), and whether each individual
  // PMT's time width was within 3 * 'sigma' of the global prompt time peak width distribution.
  for ( iPMT = fCurrentRun->GetLOCASPMTIterBegin();
        iPMT != fCurrentRun->GetLOCASPMTIterEnd();
        iPMT++ ){
    
    fCurrentPMT = &( iPMT->second );
    
    // Check that the map index matches the PMT ID
    if ( fCurrentPMT->GetID() != iPMT->first ){
      printf("LOCASFitLBPosition::InitialiseArrays: Error, LOCASPMT map index doesn't match the PMT ID! Abort.");
      return;
    }

    
    // Check whether the PMT is suitable for inclusion in the fit.
    // See the definition of LOCASFitLBPosition::SkipPMT for the selection criteria
    if ( fDQXX.LcnInfo( fCurrentPMT->GetID(), iK ) == 0 
         || SkipPMT( fCurrentRun, fCurrentPMT )
         || ( fCurrentPMT->GetWavelengthPromptPeakWidth() < promptRunTimeWidthMean - ( 3.0 * promptRunTimeWidth ) )
         || ( fCurrentPMT->GetWavelengthPromptPeakWidth() > promptRunTimeWidthMean + ( 3.0 * promptRunTimeWidth ) ) ){ 
      nBadPMTs++; continue; 
    }
    
    // If it gets this far, the PMT must be 'good' - so we can use it in the fit.
    nGoodPMTs++;
    
    // Set the 'X' data point to be the reference to the PMT (i.e. the PMT index/ID)
    fMrqX[ fNPMTs + 1 ] = fCurrentPMT->GetID();

    // Set the 'Y' data point to be the prompt peak time + the time of flight
    fMrqY[ fNPMTs + 1 ] = ( fCurrentPMT->GetWavelengthPromptPeakTime() ) + ( fCurrentPMT->GetWavelengthTimeOfFlight() );

    // Set the 'sigma' value to be the prompt peak width of the PMT divided by the squareroot of the number
    // of bins used in the sliding window technique.
    fMrqSigma[ fNPMTs + 1 ] = fCurrentPMT->GetWavelengthPromptPeakWidth() / TMath::Sqrt( 32 - 1 );
    
    fNPMTs++;
  }
  
  // Check that the number of 'good' PMTs and the number of 'bad' PMTs matches the total number of PMTs
  // stroed on the LOCASRun object
  if ( ( fCurrentRun->GetNPMTs() - nBadPMTs ) != nGoodPMTs ){
    printf("LOCASFitLBPosition::InitiialiseArrays(): Mismatch between the number of (good) PMTs vs. (bad) PMTs: %d vs. %d", nGoodPMTs, nBadPMTs);
  }
  
  // Now set the initial values of the parameters. There are 5 parameters in total
  // to fit to the laserball model. The 5th parameter, the effective group velocity
  // of the light in the detector is currently held fixed. Thus, the value set here
  // for that parameter 'fMrqParameters[ 5 ]' is constant throughout the fit.

  // Parameter 1: x-coordinate of the laserball
  // Parameter 2: y-coordinate of the laserball
  // Parameter 3: z-coordinate of the laserball
  // Parameter 4: The time of the laserball prompt peak (ideally centered about zero)

  fMrqParameters[ 1 ] = ( fCurrentLBPos.X() );    // x-cordinate of the laserball
  fMrqParameters[ 2 ] = ( fCurrentLBPos.Y() );    // y-coordinate of the laserball
  fMrqParameters[ 3 ] = ( fCurrentLBPos.Z() );    // z-coordinate of the laserball
  fMrqParameters[ 4 ] = 0.0;                      // Laserball time ( = 0.0 )

  ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////

  // We make an initial guess of the effective group velocity by calculating
  // the weighted velocity from the centre of the detector to the PSUP (using approximate distances)

  // Get the laser wavelength
  Float_t lambda = fCurrentRun->GetWavelengthLambda();

  // convert the lambda value of the laser wavelength (nm) to 
  // the energy equivalent (MeV)
  Float_t energy = fLightPath.WavelengthToEnergy( lambda * 1e-6 );

  fVgScint = fGVelocity.GetScintGroupVelocity( energy );
  fVgAV = fGVelocity.GetAVGroupVelocity( energy );
  fVgWater = fGVelocity.GetWaterGroupVelocity( energy );

  ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  
  fArraysInitialised = true;

  //printf("done\n");
    
}

//////////////////////////////////////
//////////////////////////////////////


Bool_t LOCASFitLBPosition::SkipPMT( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Bool_t skipPMT = false;

  if ( ( fCurrentPMT->GetWavelengthMPECorrOccupancyErr() > 0.1 * fCurrentPMT->GetWavelengthMPECorrOccupancy() )
       || ( fCurrentPMT->GetWavelengthMPECorrOccupancyCorr() < 0.7 ) 
       || ( fCurrentPMT->GetWavelengthMPECorrOccupancyCorr() > 1.5 )
       || ( fCurrentPMT->GetWavelengthPromptPeakWidth() == 0 ) 
       || ( fCurrentPMT->GetWavelengthNeckFlag() ) ){ 
    skipPMT = true; return skipPMT; 
  }
  else{ return skipPMT; }
  
}

//////////////////////////////////////
//////////////////////////////////////


void LOCASFitLBPosition::PrintFitInfo()
{

  printf( "---------------------------------------------\n" );
  printf( "Laserball Position Fit for Runs: %i (Off-Axis) %i (Central) %i (500 nm)\n", 
          fCurrentRun->GetRunID(), fCurrentRun->GetCentralRunID(), fCurrentRun->GetWavelengthRunID() );
  printf( "Fitted Position using run: %i\n", fCurrentRun->GetWavelengthRunID() );
  printf( "---------------------------\n" );
  printf( "Seeded Laserball Position: ( %f, %f, %f ) mm\n", 
          fCurrentRun->GetWavelengthLBPos().X(), fCurrentRun->GetWavelengthLBPos().Y(), fCurrentRun->GetWavelengthLBPos().Z() );
  printf( "Seeded Laserball t0: %f ns\n", 0.0 );
  printf( "---------------------------\n" );
  printf( "Fitted Laserball Position: ( %f, %f, %f ) mm\n", 
          fMrqParameters[ 1 ], fMrqParameters[ 2 ], fMrqParameters[ 3 ] );
  printf( "Errors: ( %f, %f, %f ) mm\n", 
          TMath::Sqrt( fMrqCovariance[ 1 ][ 1 ] ), TMath::Sqrt( fMrqCovariance[ 2 ][ 2 ] ), TMath::Sqrt( fMrqCovariance[ 3 ][ 3 ] ) );
  printf( "Fitted Laserball t0: %f ns\n", fMrqParameters[ 4 ] );
  printf( "Error: %f ns\n", TMath::Sqrt( fMrqCovariance[ 4 ][ 4 ] ) );
  printf( "---------------------------\n" );
  printf( "No. PMTs in fit: %i || ChiSquare: %f || Reduced ChiSquare: %f / %i - 4 = %f\n", fNPMTs, fChiSquare, fChiSquare, fNPMTs, fChiSquare / ( (Float_t)fNPMTs - 4.0 ) );
  printf( "---------------------------------------------\n" );
  printf( "\n\n" );

  
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
  //printf("Calling mrqmin for initialization...\n");
  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lambda);
  //printf("Done.  Chisq = %f\n",*chisq);
  oldchisq = *chisq;
  //printf("CHISQ at origin = %12.5g\n",*chisq);
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lambda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
         && retval == 0 && lambda != 0.0) {
    oldchisq = *chisq;
    //printf("Iterating with lambda %g...\n",lambda);
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lambda );
    //printf("New chisq = %12.8g with lambda %g \n",*chisq,lambda);
    numiter++;
    
    if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lambda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  
  lambda = 0;
  retval = mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lambda );  
  //printf("Finished MrqFit\n");
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

  fCurrentLBPos.SetXYZ( a[1], a[2], a[3] );

  Int_t nBadChiSq = 0;
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
      }
    }
    chisqentry = dy*dy*sig2i;
    *chisq += chisqentry;
    if (fChiArray!=NULL && i>=0 && i<fNElements) fChiArray[i] = chisqentry;
    if (fResArray!=NULL && i>=0 && i<fNElements) fResArray[i] = dy;
  }
  //cout << "nBadChiSq is: " << nBadChiSq << endl;
  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
  LOCASFree_Vector(dyda,1,ma);
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFitLBPosition::mrqfuncs(Float_t x, Int_t i, Float_t a[], Float_t *y,
                                  Float_t dyda[], Int_t na )
{

  if ( !fCurrentRun ) { 
    printf("LOCASFitLBPosition::mrqfuncs: Error, no LOCASRun object being pointed to! Abort"); 
    return; 
  }
  
  Int_t ix = (Int_t)x;
  LOCASPMT* pmtPtr = &( fCurrentRun->GetPMT( ix ) );
  Int_t pmtID = pmtPtr->GetID();
  TVector3 srcPos( fCurrentLBPos );
  TVector3 pmtPos( fPMTInfo.GetPosition( pmtID ) );

  Float_t lbEnergy = fLightPath.WavelengthToEnergy( fCurrentRun->GetWavelengthLambda() * 1e-6 );
  fLightPath.CalcByPosition( srcPos, pmtPos, lbEnergy, 30.0 );
  Float_t distInScint = fLightPath.GetDistInScint();
  Float_t distInAV = fLightPath.GetDistInAV();
  Float_t distInWater = fLightPath.GetDistInWater();

  Float_t lightSpeed = ( ( fVgScint * distInScint ) + ( fVgAV * distInAV ) + ( fVgWater * distInWater ) ) 
    / ( distInScint + distInAV + distInWater );
  
  Float_t tFlight = ( distInScint / fVgScint ) + ( distInAV / fVgAV )  + ( distInWater / fVgWater );
  
  *y = tFlight + a[ 4 ];
  
  TVector3 srcvecX( a[ 1 ] + fDelPos, a[ 2 ], a[ 3 ] );
  fLightPathX.CalcByPosition( srcvecX, pmtPos, lbEnergy, 30.0 );
  if ( !fLightPathX.GetTIR() && !fLightPathX.GetResvHit() ){
    dyda[ 1 ] = ( ( ( fLightPathX.GetDistInScint() - fLightPath.GetDistInScint() ) / fVgScint )
                  + ( ( fLightPathX.GetDistInAV() - fLightPath.GetDistInAV() ) / fVgAV )
                  + ( ( fLightPathX.GetDistInWater() - fLightPath.GetDistInWater() ) / fVgWater ) ) / fDelPos;
    
  }
  else{ 
    dyda[ 1 ] = - ( pmtPos.X() - srcvecX.X() ) * ( 1.0 / lightSpeed ) / ( pmtPos - srcvecX ).Mag(); 
  }

  TVector3 srcvecY( a[ 1 ], a[ 2 ] + fDelPos, a[ 3 ] );
  fLightPathY.CalcByPosition( srcvecY, pmtPos, lbEnergy, 30.0 );
  if ( !fLightPathY.GetTIR() && !fLightPathY.GetResvHit() ){
    dyda[ 2 ] = ( ( ( fLightPathY.GetDistInScint() - fLightPath.GetDistInScint() ) / fVgScint )
                    + ( ( fLightPathY.GetDistInAV() - fLightPath.GetDistInAV() ) / fVgAV )
                  + ( ( fLightPathY.GetDistInWater() - fLightPath.GetDistInWater() ) / fVgWater ) ) / fDelPos;
    
  }
  else{ dyda[ 2 ] = - ( pmtPos.Y() - srcvecY.Y() ) * ( 1.0 / lightSpeed ) / ( pmtPos - srcvecY ).Mag(); }
  
  TVector3 srcvecZ( a[ 1 ], a[ 2 ], a[ 3 ] + fDelPos );
  fLightPathZ.CalcByPosition( srcvecZ, pmtPos, lbEnergy, 30.0 );
  if ( !fLightPathZ.GetTIR() && !fLightPathZ.GetResvHit() ){
    dyda[ 3 ] = ( ( ( fLightPathZ.GetDistInScint() - fLightPath.GetDistInScint() ) / fVgScint )
                  + ( ( fLightPathZ.GetDistInAV() - fLightPath.GetDistInAV() ) / fVgAV )
                  + ( ( fLightPathZ.GetDistInWater() - fLightPath.GetDistInWater() ) / fVgWater ) ) / fDelPos;
    
  }
  else{ dyda[ 3 ] = - ( pmtPos.Z() - srcvecZ.Z() ) * ( 1.0 / lightSpeed ) / ( pmtPos - srcvecZ ).Mag(); }
  
  dyda[ 4 ] = 1.0;
  
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
  indxc= LOCASIntVector(1,n);
  indxr= LOCASIntVector(1,n);
  ipiv= LOCASIntVector(1,n);
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
  LOCASFree_IntVector(ipiv,1,n);
  LOCASFree_IntVector(indxr,1,n);
  LOCASFree_IntVector(indxc,1,n);
  return retval;
}
