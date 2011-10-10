#include <math.h>
#include <iostream>
#include "QMath.h"
#include "nrutil.h"

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define NR_END 1
#define FREE_ARG char*
//*-- Author : Mark Boulay                          

//////////////////////////////////////////////////////////////////////////
// Base class containing general purpose math routines, many borrowed   //
// from Numerical  Recipes in C.  Anyone using this class should own    //
// a copy of Numerical Recipes, with associated license.                //                    
////////////////////////////////////////////////////////////////////////// 


ClassImp (QMath)


float *pcom, *xicom;
int ncom;
#define TOL 2.0e-4

QMath::QMath ()
{
  //Constructor for QMath
  
};
QMath::~QMath ()
{
  //Destructor for QMath
};

Float_t QMath::PeakFind(TH1F *th1, Float_t &tRMS, Float_t &PeakCounts, Float_t nSigmas)
{
  // Returns the Peak value in the th1 histogram by re-calculating the mean of the distribution
  // within +/- nSigmas until convergence is achieved. The number of counts and the RMS in this 
  // window are passed to PeakCounts and tRMS.  
 
Float_t eps = 0.05;
Int_t nxbins = (Int_t)th1->GetNbinsX(); 
Float_t maxnxbins = (Float_t)( th1->GetBinLowEdge(nxbins) + th1->GetBinWidth(nxbins) );
Int_t maxcounts = 0; 
Int_t i; 
Float_t mAvg = 0;
Float_t mPeak = 0;
Float_t mSigma = 0;
Float_t maxAt=0;

Int_t maxiters = 30;
Int_t lowBin  = 0;
Int_t highBin = 0;
Int_t maxBin;

Int_t hasConverged = 0;
Float_t rms= 0.0;
Float_t rms2;

Int_t nIter = 0;
Float_t OldPeak = 0.0;

Float_t fsumwx2, fsumw, fsumwx;
Float_t fsumwx2bin, fsumwxbin;
           
       maxBin = nxbins;

      highBin = nxbins;
       if (th1->GetEntries() > 5 )
       {
        while ( hasConverged == 0 && nIter < maxiters)
        {
         

         rms2 = 0.0;
         fsumwx2 = 0.0;
         fsumw = 0.0;
         fsumwx = 0.0;
         fsumwxbin = 0.0;
         fsumwx2bin = 0.0;
         for ( i = lowBin; i < highBin; i++ )
         
         {
          
           fsumwx2bin +=  ((Float_t)i)*((Float_t)i)*(Float_t)th1->GetBinContent(i);
           fsumwx2 += (Float_t)th1->GetBinCenter(i)*(Float_t)th1->GetBinCenter(i)*(Float_t)th1->GetBinContent(i);
           fsumwx += (Float_t)th1->GetBinCenter(i)*(Float_t)th1->GetBinContent(i);
           fsumwxbin  +=  ((Float_t)i) * (Float_t)th1->GetBinContent(i);
             fsumw   +=  (Float_t)th1->GetBinContent(i);
         
         }
                 
           if ( fsumw == 0 )
             { rms = 0.0;
               mAvg = 0.0;
	     }
           else
             {
           rms2 = fabs( fsumwx2bin/fsumw - fsumwxbin*fsumwxbin/(fsumw*fsumw));
           rms = sqrt(rms2);
           mAvg = fsumwxbin/fsumw;
           mSigma = fabs( fsumwx2/fsumw - fsumwx*fsumwx/(fsumw*fsumw));
           mSigma = sqrt( mSigma);
           mPeak = fsumwx/fsumw;
         
             }


   if( fabs(OldPeak - mPeak) < eps ) { hasConverged = 1; }
          if (lowBin == highBin ) { hasConverged = 1; }
         
          OldPeak = mPeak;
         
          // rescale search window
          if ( mPeak >  maxnxbins ) mPeak = maxnxbins + 200.0;

          lowBin = (Int_t)(mAvg - nSigmas* rms);
          highBin = (Int_t)(mAvg + nSigmas* rms);


          if ( lowBin < 0 ) lowBin = 0;
          if ( highBin >= maxBin ) highBin = maxBin-1;
         
          // printf("Peak at %f +/- %f, %i:%i\n",mPeak,mSigma);
          nIter++;
        }

       } // end if nPMT > 5
       else
         {

           mPeak = 0.0;
           rms = 0.0;
         }
      
       //Calculate the number of counts in Peak +/- nSigmas;

          fsumw = 0.0;
          lowBin = (Int_t)(mAvg - nSigmas* rms);
          highBin = (Int_t)(mAvg + nSigmas* rms);
          if ( lowBin < 0 ) lowBin = 0;
          if ( highBin >= maxBin ) highBin = maxBin-1;
          for ( i = lowBin; i < highBin; i++ )
         
         {
             fsumw   +=  (Float_t)th1->GetBinContent(i);
         
         }

       tRMS = mSigma;
       PeakCounts = fsumw;
       return mPeak;

}

Float_t QMath::PeakFind2( TH1 *th1, Float_t &tRMS, Float_t &PeakCounts, Float_t tWindow)
{
  // Find the main peak in th1 by using a sliding window (width must be
  // supplied as tWindow) over the histogram.
  // The center of the region with the most counts is returned.
  // The number of counts and RMS in the window are passed to PeakCounts and tRMS.
  if ( !th1 ) return -1.;
  Int_t nbins = th1->GetNbinsX(), binlo, binhi;
  Float_t maxcounts, startt = th1->GetBinLowEdge(1), stopt = th1->GetBinLowEdge( nbins-1 ) + th1->GetBinWidth( nbins - 1);
  Float_t t, deltat = 0.5;
  Float_t maxSum = 0., maxTlo=0., sumwin;
  Int_t binlopeak = 0 , binhipeak = 0;
  for ( t = startt; t < stopt; t+= deltat)
    {
      binlo  = th1->GetXaxis()->FindBin( t );
      binhi  = th1->GetXaxis()->FindBin( t + tWindow );
      sumwin = th1->Integral(binlo, binhi);
      if ( sumwin > maxSum )
	{
	  maxSum = sumwin;
	  maxTlo = t;
	  binlopeak = binlo;
	  binhipeak = binhi;
	}
    }
  //printf("BINLO: %i, BINHI %i\n",binlopeak, binhipeak);
  tRMS = 0.0;  //compute this!
  PeakCounts = maxSum;
  Float_t peak= maxTlo + tWindow/2.;
  return peak;
}

//The following are modified numerical recipes routines.

double QMath::
qsimpInt (double a, double b)
{
  //Perform integration using Simpson's rule between the limits a and b.
  // The function integrated is func(x), a virtual member of QMath.
  // The Class containing the member function func(x) to be integrated
  // should be a descendant of QMath.
  // EPS is the desired fractional accuracy and 2 to thepower JMAX-1 is the
  // maximum allowed number of steps.
  // void nrerror(char error_text[]);
  int j;
  double s, st, ost, os;
  int JMAX = 30;
  double EPS = 1.0e-4;
  ost = os = -1.0e30;
  double strap;
  for (j = 1; j <= JMAX; j++)
    {
      //    st=tTrap.trapzdInt(func,a,b,j);
      //*******************************trapzd*******************************
      double x, tnm, sum, del;
      int it, jtrap;

      if (j == 1)
	{
	  double temp = func (a) + func (b);
	  strap = 0.5 * (b - a) * temp;
	  st = strap;
	}
      else
	{
	  for (it = 1, jtrap = 1; jtrap < j - 1; jtrap++)
	    it <<= 1;
	  tnm = it;
	  del = (b - a) / tnm;	//This is the spacing of the points to be added
	  x = a + 0.5 * del;
	  for (sum = 0.0, jtrap = 1; jtrap <= it; jtrap++, x += del)
	    sum += func (x);
	  strap = 0.5 * (strap + (b - a) * sum / tnm);
	  st = strap;		// This replaces st by its refined value.
	}

      //***********************************************************************
      s = (4.0 * st - ost) / 3.0;
      if (fabs (s - os) < EPS * fabs (os))
	return s;
      os = s;
      ost = st;
    }
  nrerror("Too many steps in routine qsimp");
  return 0.0;
};

#define MAXSTP 1000000
#define TINY 1.0e-30


void QMath::odeint(Double_t ystart[], int nvar, Double_t x1, Double_t x2, Double_t eps, Double_t h1,
	Double_t hmin, int *nok, int *nbad, Int_t nsol, Double_t *xsol, Double_t *ysol)
{
  
  //added by mgb:

  // odeint: 	Integration of Ordinary Differential Equations
  //
  // Coments from Numerical Recipes:
  // "Runge-Kutta driver with adaptive stepsize control. Integrate starting values ystart[1..nvar]
  // from x1 to x2 with accuracy eps storing intermediate results in global variables. h1 should be
  // set as a guessed first stepsize, hmin as the minimum allowed stepsize (can be zero). On output
  // nok and nbad are the number of good and bad (but retried and fixed) steps taken, and ystart is
  // replaced by values at the end of the integration interval.  	
        
	Double_t Rsun = 6.96e5;
        int kmax = nsol - 1;
	int kount = -1;
	fNSaved = 0;
	Double_t dxsav= 0;
	if ( nsol > 0 ) dxsav = Rsun/(Double_t)nsol;
	//if (! xp ) xp = new dvector[1,1000];
	//if (! yp ) yp = new dmatrix[1,4,1,1000];

	int nstp,i;
	Double_t xsav,x,hnext,hdid,h;
	Double_t *yscal,*y,*dydx;

	yscal=dvector(1,nvar);
	y=dvector(1,nvar);
	dydx=dvector(1,nvar);
       
	x=x1;
	h=SIGN(h1,x2-x1);
	*nok = (*nbad) = 0;
	for (i=1;i<=nvar;i++) y[i]=ystart[i];
	if (kmax > 0) xsav=x-dxsav*2.0; 	// Assures storage of first step
	for (nstp=1;nstp<=MAXSTP;nstp++) {	// Take at most MAXSTP steps
		  derivs(x,y,dydx);
		for (i=1;i<=nvar;i++)	
			// Scaling is used to monitor accuracy. This general-purpose choice can be
			// modified if need be.	
			yscal[i]=fabs(y[i])+fabs(dydx[i]*h)+TINY;
		if (kmax > 0 && kount < kmax-1 && fabs(x-xsav) > fabs(dxsav)) {
		  if ( nsol > 0 )
		    {
		  //xp[++kount]=x; 	//Store intermediate results.	
		  //for (i=1;i<=nvar;i++) yp[i][kount]=y[i];
		      //fSolution.Fill(x/Rsun, y[1]*y[1]+y[3]*y[3] );
		      xsol[++kount] = x/Rsun;
		      ysol[kount  ] = y[1]*y[1]+y[3]*y[3];
		      fNSaved = kount;
		      //printf("ODE (%i): %.2f %.2f\n",kount,x/Rsun, y[1]*y[1]+y[3]*y[3]);
			xsav=x;
		    }
		}
		if ((x+h-x2)*(x+h-x1) > 0.0) h=x2-x;	//If stepsize can overshoot, decrease.
		 rkqs(y,dydx,nvar,&x,h,eps,yscal,&hdid,&hnext);
		if (hdid == h) ++(*nok); else ++(*nbad);
		if ((x-x2)*(x2-x1) >= 0.0) {		// Are we done?
			for (i=1;i<=nvar;i++) ystart[i]=y[i];
			if (kmax) {
			if ( nsol > 0 )
			  {
			  //xp[++kount]=x;		// Save final step.
				//for (i=1;i<=nvar;i++) yp[i][kount]=y[i];
			    //fSolution.Fill( x/Rsun, y[1]*y[1]+y[3]*y[3] );
			    if ( kount < kmax  )
			      {
				xsol[++kount] = x/Rsun;
				ysol[kount  ] = y[1]*y[1]+y[3]*y[3];
				fNSaved = kount;
				//printf("ODE2 (%i): %.2f %.2f\n",kount,x/Rsun,y[1]*y[1]+y[3]*y[3]);
			      }
			  }
			}
			free_dvector(dydx,1,nvar);
			free_dvector(y,1,nvar);
			free_dvector(yscal,1,nvar);
			
			return;
		}
		if (fabs(hnext) <= hmin) printf("Step size too small in odeint\n");
		h=hnext;
	}
        printf("Too many steps in routine odeint\n");
}
#undef MAXSTP
#undef TINY
#undef NRANSI



#define SAFETY 0.9
#define PGROW -0.2
#define PSHRNK -0.25
#define ERRCON 1.89e-4

void QMath::rkqs(Double_t y[], Double_t dydx[], int n, Double_t *x, Double_t htry, Double_t eps,
	Double_t yscal[], Double_t *hdid, Double_t *hnext)
{
  // Comments from Numerical Recipes:
  // 
  // Fifth-order Runge-Kutta step with monitoring of local truncation error to ensure accuraccy and
  // adjust stepsize. Input are the dependent variable vector y[1..n] and its derivative dydx[1..n]
  // at the starting value of the independent variable x. Also input are the stepsize to be 
  // attempted htry, the required accuraccy eps, and the vector yscal[1..n] against which the error
  // is scaled. On output, y and x are replaced bt their new values, hdid is the stepsize that was
  // actually accomplished, and hnext is the estimated next stepsize.  
  
  //void rkck(Double_t y[], Double_t dydx[], int n, Double_t x, Double_t h,
  //		Double_t yout[], Double_t yerr[], void (*derivs)(Double_t, Double_t [], Double_t []));
	int i;
	Double_t errmax,h,htemp,xnew,*yerr,*ytemp;

	yerr=dvector(1,n);
	ytemp=dvector(1,n);
      
	h=htry;
	for (;;) {
		rkck(y,dydx,n,*x,h,ytemp,yerr);
		errmax=0.0;
		for (i=1;i<=n;i++) errmax=FMAX(errmax,fabs(yerr[i]/yscal[i]));
		errmax /= eps;
		if (errmax > 1.0) {
			htemp=SAFETY*h*pow(errmax,PSHRNK);
			h=(h >= 0.0 ? FMAX(htemp,0.1*h) : FMIN(htemp,0.1*h));
			xnew=(*x)+h;
			if (xnew == *x) nrerror("stepsize underflow in rkqs\n");
			continue;
		} else {
			if (errmax > ERRCON) *hnext=SAFETY*h*pow(errmax,PGROW);
			else *hnext=5.0*h;
			*x += (*hdid=h);
			for (i=1;i<=n;i++) y[i]=ytemp[i];
			break;
		}
	}
	free_dvector(ytemp,1,n);
	free_dvector(yerr,1,n);
}
#undef SAFETY
#undef PGROW
#undef PSHRNK
#undef ERRCON
#undef NRANSI


void QMath::rkck(Double_t y[], Double_t dydx[], int n, Double_t x, Double_t h, Double_t yout[],
	Double_t yerr[])
{
	// Comments from numerical Recipes
	//
	// Given values for n variables y[1..n] and their derivatives dydx[1..n] known at x, use the
	// fifth-order Cash-Karp Runge-Kutta method to advance the solution over an interval h and
	// return the incremental variables as yout[1..n]. Also return an estimate of the local
	// truncation error in yout using the embedded fourth-order method.
	
	int i;
	static Double_t a2=0.2,a3=0.3,a4=0.6,a5=1.0,a6=0.875,b21=0.2,
		b31=3.0/40.0,b32=9.0/40.0,b41=0.3,b42 = -0.9,b43=1.2,
		b51 = -11.0/54.0, b52=2.5,b53 = -70.0/27.0,b54=35.0/27.0,
		b61=1631.0/55296.0,b62=175.0/512.0,b63=575.0/13824.0,
		b64=44275.0/110592.0,b65=253.0/4096.0,c1=37.0/378.0,
		c3=250.0/621.0,c4=125.0/594.0,c6=512.0/1771.0,
		dc5 = -277.00/14336.0;
	Double_t dc1=c1-2825.0/27648.0,dc3=c3-18575.0/48384.0,
		dc4=c4-13525.0/55296.0,dc6=c6-0.25;
	Double_t *ak2,*ak3,*ak4,*ak5,*ak6,*ytemp;

	ak2=dvector(1,n);
	ak3=dvector(1,n);
	ak4=dvector(1,n);
	ak5=dvector(1,n);
	ak6=dvector(1,n);
	ytemp=dvector(1,n);

	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+b21*h*dydx[i];
	  derivs(x+a2*h,ytemp,ak2);
	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b31*dydx[i]+b32*ak2[i]);
	  derivs(x+a3*h,ytemp,ak3);
	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b41*dydx[i]+b42*ak2[i]+b43*ak3[i]);
	  derivs(x+a4*h,ytemp,ak4);
	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b51*dydx[i]+b52*ak2[i]+b53*ak3[i]+b54*ak4[i]);
	  derivs(x+a5*h,ytemp,ak5);
	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b61*dydx[i]+b62*ak2[i]+b63*ak3[i]+b64*ak4[i]+b65*ak5[i]);
	  derivs(x+a6*h,ytemp,ak6);
	for (i=1;i<=n;i++)
		yout[i]=y[i]+h*(c1*dydx[i]+c3*ak3[i]+c4*ak4[i]+c6*ak6[i]);
	for (i=1;i<=n;i++)
		yerr[i]=h*(dc1*dydx[i]+dc3*ak3[i]+dc4*ak4[i]+dc5*ak5[i]+dc6*ak6[i]);
	free_dvector(ytemp,1,n);
	free_dvector(ak6,1,n);
	free_dvector(ak5,1,n);
	free_dvector(ak4,1,n);
	free_dvector(ak3,1,n);
	free_dvector(ak2,1,n);
}
#undef NRANSI


Double_t *QMath::dvector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
        Double_t *v;
	v=(Double_t *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(Double_t)));
	if (!v) nrerror("allocation failure in dvector()\n");
	return v-nl+NR_END;
}

void QMath::free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
  free((FREE_ARG) (v+nl-NR_END));
}

void QMath::nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
//	exit(1);
	
}

float *QMath::vector(long nl, long nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
	float *v;

	v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl+NR_END;
}

int *QMath::ivector(long nl, long nh)
/* allocate an int vector with subscript range v[nl..nh] */
{
	int *v;

	v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl+NR_END;
}

float **QMath::matrix(long nrl, long nrh, long ncl, long nch)
/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	float **m;

	/* allocate pointers to rows */
	m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	/* allocate rows and set pointers to them */
	m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;
}
void QMath::free_vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}

void QMath::free_ivector(int *v, long nl, long nh)
/* free an int vector allocated with ivector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}

void QMath::free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
/* free a float matrix allocated by matrix() */
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}


Int_t QMath::MrqFit(float x[], float y[], float sig[], int ndata, float a[], int ia[],
  int ma, float **covar, float **alpha, float *chisq )
{
  //Fit these data using mrqmin repeatedly until convergence is achieved.

  Int_t maxiter = 1000, numiter = 0, gooditer = 0, retval = 0;
  Float_t oldchisq = 0, lamda = -1.0, tol = 1.e-6;
  *chisq = 0;
  //first, call mrqmin with lambda = -1 for initialization
  //  printf("Calling mrqmin for initialization...\n");
  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda);
  //printf("Done.  Chisq = %f\n",*chisq);
  oldchisq = *chisq;

  //printf("CHISQ at origin = %8.2f\n",*chisq);
  //Next set lambda to 0.01, and iterate until convergence is reached
  lamda = 0.01;
  while ( ( ( fabs( *chisq - oldchisq ) > tol || gooditer < 3 )  && (numiter < maxiter)) && retval == 0 && lamda != 0.)
    {
      oldchisq = *chisq;
      //printf("Iterating...\n");
      retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda );
      //printf("New chisq = %8.2f \n",*chisq);
      numiter++;
      if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
      else gooditer = 0;
    }
  
  //We're done.  Set lamda = 0 and call mrqmin one last time.  This attempts to calculate covariance (covar), and
  //curvature (alpha) matrices. It also frees up allocated memory.
  
  lamda = 0;
  mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lamda );
  return retval;
}

Int_t QMath::mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ia[],
	int ma, float **covar, float **alpha, float *chisq, float *alamda)
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
		atry=vector(1,ma);
		beta=vector(1,ma);
		da=vector(1,ma);
		for (mfit=0,j=1;j<=ma;j++)
			if (ia[j]) mfit++;
		oneda=matrix(1,mfit,1,1);
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
		free_matrix(oneda,1,mfit,1,1);
		free_vector(da,1,ma);
		free_vector(beta,1,ma);
		free_vector(atry,1,ma);
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
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

void QMath::covsrt(float **covar, int ma, int ia[], int mfit)
{
	// Commments from Numerical Recipes
	//
	// Expand in storage the covariance matrix covar, so as to take into account parameters
	// that are being held fixed. (For the latter, return zero covariances.) 
	
	
	int i,j,k;
	float swap;

	for (i=mfit+1;i<=ma;i++)
		for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
	k=mfit;
	for (j=ma;j>=1;j--) {
		if (ia[j]) {
			for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j])
			for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
			k--;
		}
	}
}
#undef SWAP
#include <math.h>
#include "nrutil.h"
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

Int_t QMath::gaussj(float **a, int n, float **b, int m)
{
	//
	// Linear equation solution by Gauss-Jordan elimination. a[1..n][1..n] is the input matrix.
	// b[1..n][1..n] is input containing the m right-hand side vectors. On output, a is
	// replaced by its matrix inverse, and b is replaced by the corresponding set of solution
	// vectors.
	
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,temp;
	Int_t retval = 0;
	indxc=ivector(1,n);
	indxr=ivector(1,n);
	ipiv=ivector(1,n);
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
					} else if (ipiv[k] > 1) 
					  {
					    //nrerror("gaussj: Singular Matrix-1");
					    //gSNO->Warning("gaussj","Singular Matrix-1");
					    retval = -1;
					  }
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
			for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) 
		  {
		    //nrerror("gaussj: Singular Matrix-2");
		    //gSNO->Warning("gaussj","Singular Matrix-2");
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
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
	return retval;
}
#undef SWAP
#undef NRANSI
#define NRANSI
#include "nrutil.h"

void QMath::mrqcof(float x[], float y[], float sig[], int ndata, float a[], int ia[],
	int ma, float **alpha, float beta[], float *chisq)
{
	// Comments from Numerical Recipes
	//
	// Used by mrqmin to evaluate the linearized fitting matrix alpha, and vectro beta, and
	// calculate chi2.
	
	int i,j,k,l,m,mfit=0;
	float ymod,wt,sig2i,dy,*dyda;

	dyda=vector(1,ma);
	for (j=1;j<=ma;j++) {
		if (ia[j]) mfit++;
		dyda[j] = 0.0;
	}
	for (j=1;j<=mfit;j++) {
		for (k=1;k<=j;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}
	*chisq=0.0;
	for (i=1;i<=ndata;i++) {
	  //(*funcs)(x[i],a,&ymod,dyda,ma);
	  mrqfuncs( x[i],i,a,&ymod,dyda,ma);
	  //printf("dyda = %.2f %.2f %.2f %.2f\n",dyda[1],dyda[2], dyda[3],dyda[4]);
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
		*chisq += dy*dy*sig2i;
	}
	for (j=2;j<=mfit;j++)
		for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
	free_vector(dyda,1,ma);
}

Float_t  QMath::rtbis(float x1, float x2, float xacc)
{
  //Finds the zero of function rfunc( float x )
  //void nrerror(char error_text[]);
        Int_t JMAX = 40;
	int j;
	float dx,f,fmid,xmid,rtb;

	f=rfunc(x1);
	fmid=rfunc(x2);
	if (f*fmid >= 0.0) nrerror("Root must be bracketed for bisection in rtbis");
	rtb = f < 0.0 ? (dx=x2-x1,x1) : (dx=x1-x2,x2);
	for (j=1;j<=JMAX;j++) {
		fmid=this->rfunc(xmid=rtb+(dx *= 0.5));
		if (fmid <= 0.0) rtb=xmid;
		if (fabs(dx) < xacc || fmid == 0.0) return rtb;
	}
	nrerror("Too many bisections in rtbis");
	return 0.0;
}

#undef JMAX
#undef NRANSI

Float_t  QMath::rtbis1(TF1* f1, float x1, float x2, float xacc)
{
  //Finds the zero of function f1
  //does range checking
  Double_t f1xmin,f1xmax;

  f1->GetRange(f1xmin,f1xmax);
  if( f1xmin>x1 &&  f1xmax<x2 ) f1->SetRange(x1,x2);
  if( f1xmin>x1 &&  f1xmax>x2 ) f1->SetRange(x1,f1xmax);
  if( f1xmin<x1 &&  f1xmax<x2 ) f1->SetRange(f1xmin,x2);



        Int_t JMAX = 40;
	int j;
	float dx,f,fmid,xmid,rtb;

	f=f1->Eval(x1);
	fmid=f1->Eval(x2);
	if (f*fmid >= 0.0) nrerror("Root must be bracketed for bisection in rtbis");
	rtb = f < 0.0 ? (dx=x2-x1,x1) : (dx=x1-x2,x2);
	for (j=1;j<=JMAX;j++) {
		fmid=f1->Eval(xmid=rtb+(dx *= 0.5));
		if (fmid <= 0.0) rtb=xmid;
		if (fabs(dx) < xacc || fmid == 0.0) return rtb;
	}
	nrerror("Too many bisections in rtbis");
	return 0.0;
}



#define ITMAX 200
void QMath::powell(float p[], float **xi, int n, float ftol, int *iter, float *fret)
{

  //printf("Enterred powell.\n");
  //printf("Overiding FuncP:: %.2f\n",FuncP( p ) );
  // Minimization of a function func of n variables.  Input consists of
  // an initial starting point p[1..n]; an initial matrix xi[1..n][1..n], 
  // whose columns contain the initial set of directions (usually the n
  // unit vectors); and ftol, the fractional tolerance in the function
  // value such that failure to decrease by more than this amount on
  // one iteration signals doneness.  On output, p is set to the best
  // point found, xi is the then-current direction set, fret is the 
  // returned function value at p, and iter is the number of iterations
  // taken.  The routine linmin is used.

	int i,ibig,j;
	float del,fp,fptt,t,*pt,*ptt,*xit;
        //void linmin(float p[], float xi[], int n, float *fret,
        //float (*func)(float []));

	pt=vector(1,n);
	ptt=vector(1,n);
	xit=vector(1,n);
	*fret=FuncP(p);
	for (j=1;j<=n;j++) pt[j]=p[j];
	for (*iter=1;;++(*iter)) {
		fp=(*fret);
		ibig=0;
		del=0.0;
		for (i=1;i<=n;i++) {
			for (j=1;j<=n;j++) xit[j]=xi[j][i];
			fptt=(*fret);
			linmin(p,xit,n,fret);
			if (fabs(fptt-(*fret)) > del) {
				del=fabs(fptt-(*fret));
				ibig=i;
			}
		}
		if (2.0*fabs(fp-(*fret)) <= ftol*(fabs(fp)+fabs(*fret))) {
			free_vector(xit,1,n);
			free_vector(ptt,1,n);
			free_vector(pt,1,n);
			return;
		}
		if (*iter == ITMAX) nrerror("powell exceeding maximum iterations.");
		for (j=1;j<=n;j++) {
			ptt[j]=2.0*p[j]-pt[j];
			xit[j]=p[j]-pt[j];
			pt[j]=p[j];
		}
		fptt=FuncP(ptt);
		if (fptt < fp) {
			t=2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR(fp-fptt);
			if (t < 0.0) {
				linmin(p,xit,n,fret);
				for (j=1;j<=n;j++) {
					xi[j][ibig]=xi[j][n];
					xi[j][n]=xit[j];
				}
			}
		}
	}
}
#undef ITMAX


float QMath::f1dim(float x)
{
  //Must accompany linmin

	int j;
	int ncom = 4;  //HACK !!!!!
	float f,*xt;

	xt=vector(1,ncom);
	for (j=1;j<=ncom;j++) xt[j]=pcom[j]+x*xicom[j];
	f=FuncP(xt);
	free_vector(xt,1,ncom);
	return f;
}
  
#define ITMAX 100
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

float QMath::brent_linmin(float ax, float bx, float cx, float tol,
	float *xmin)
{
  // Modified from brent to not include the pointer to function in
  // the parameter list because I can't figure out how to call
  // it properly for a member function.  To be used only by linmin()

	int iter;
	float a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
	float e=0.0;

	a=(ax < cx ? ax : cx);
	b=(ax > cx ? ax : cx);
	x=w=v=bx;
	fw=fv=fx=f1dim(x);
	for (iter=1;iter<=ITMAX;iter++) {
		xm=0.5*(a+b);
		tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
		if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
			*xmin=x;
			return fx;
		}
		if (fabs(e) > tol1) {
			r=(x-w)*(fx-fv);
			q=(x-v)*(fx-fw);
			p=(x-v)*q-(x-w)*r;
			q=2.0*(q-r);
			if (q > 0.0) p = -p;
			q=fabs(q);
			etemp=e;
			e=d;
			if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
				d=CGOLD*(e=(x >= xm ? a-x : b-x));
			else {
				d=p/q;
				u=x+d;
				if (u-a < tol2 || b-u < tol2)
					d=SIGN(tol1,xm-x);
			}
		} else {
			d=CGOLD*(e=(x >= xm ? a-x : b-x));
		}
		u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
		fu=f1dim(u);
		if (fu <= fx) {
			if (u >= x) a=x; else b=x;
			SHFT(v,w,x,u)
			SHFT(fv,fw,fx,fu)
		} else {
			if (u < x) a=u; else b=u;
			if (fu <= fw || w == x) {
				v=w;
				w=u;
				fv=fw;
				fw=fu;
			} else if (fu <= fv || v == x || v == w) {
				v=u;
				fv=fu;
			}
		}
	}
	nrerror("Too many iterations in brent");
	*xmin=x;
	return fx;
}
#undef ITMAX
#undef CGOLD
#undef ZEPS
#undef SHFT




void QMath::linmin(float p[], float xi[], int n, float *fret)
{
  // Given an n-dimensional point p[1..n] and an n-dimensional direction
  // xi[1..n], moves and resets p to where the function func(p) takes on
  // a minimum along the direction xi from p, and replaces xi by the actual
  // vector displacement that p was moved.  Also returns as fret the value
  // of func at the returned location p.  This is actually all accompllished
  // by calling the routines mnbrak and brent.

	int j;
	float xx,xmin,fx,fb,fa,bx,ax;
  //float brent(float ax, float bx, float cx,
  //	float (*f)(float), float tol, float *xmin);
  //float (*pf1dim)(float x);
  //pf1dim = f1dim;
	//void mnbrak(float *ax, float *bx, float *cx, float *fa, float *fb,
	//	float *fc, float (*func)(float));
	//int j;
	//float xx,xmin,fx,fb,fa,bx,ax;

	ncom=n;
	pcom=vector(1,n);
	xicom=vector(1,n);
	//nrfunc=func;
	for (j=1;j<=n;j++) {
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}
	ax=0.0;
	xx=1.0;
	//mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f1dim);
	//*fret=brent(ax,xx,bx,f1dim,TOL,&xmin);
	mnbrak_linmin(&ax,&xx,&bx,&fa,&fx,&fb);
	*fret=brent_linmin(ax,xx,bx,TOL,&xmin);
	for (j=1;j<=n;j++) {
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free_vector(xicom,1,n);
	free_vector(pcom,1,n);
}
#undef TOL

#define GOLD 1.618034
#define GLIMIT 100.0
#define TINY 1.0e-20
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

void QMath::mnbrak_linmin(float *ax, float *bx, float *cx, float *fa, float *fb, float *fc)
{
  // Modified from mnbrak to not include the pointer to function in
  // the parameter list because I can't figure out how to call
  // it properly for a member function.  To be used only by linmin()

	float ulim,u,r,q,fu,dum;

	*fa=f1dim(*ax);
	*fb=f1dim(*bx);
	if (*fb > *fa) {
		SHFT(dum,*ax,*bx,dum)
		SHFT(dum,*fb,*fa,dum)
	}
	*cx=(*bx)+GOLD*(*bx-*ax);
	*fc=f1dim(*cx);
	while (*fb > *fc) {
		r=(*bx-*ax)*(*fb-*fc);
		q=(*bx-*cx)*(*fb-*fa);
		u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
			(2.0*SIGN(FMAX(fabs(q-r),TINY),q-r));
		ulim=(*bx)+GLIMIT*(*cx-*bx);
		if ((*bx-u)*(u-*cx) > 0.0) {
			fu=f1dim(u);
			if (fu < *fc) {
				*ax=(*bx);
				*bx=u;
				*fa=(*fb);
				*fb=fu;
				return;
			} else if (fu > *fb) {
				*cx=u;
				*fc=fu;
				return;
			}
			u=(*cx)+GOLD*(*cx-*bx);
			fu=f1dim(u);
		} else if ((*cx-u)*(u-ulim) > 0.0) {
			fu=f1dim(u);
			if (fu < *fc) {
				SHFT(*bx,*cx,u,*cx+GOLD*(*cx-*bx))
				SHFT(*fb,*fc,fu,f1dim(u))
			}
		} else if ((u-ulim)*(ulim-*cx) >= 0.0) {
			u=ulim;
			fu=f1dim(u);
		} else {
			u=(*cx)+GOLD*(*cx-*bx);
			fu=f1dim(u);
		}
		SHFT(*ax,*bx,*cx,u)
		SHFT(*fa,*fb,*fc,fu)
	}
}
#undef GOLD
#undef GLIMIT
#undef TINY
#undef SHFT

#define NMAX 5000
#define GET_PSUM \
					for (j=1;j<=ndim;j++) {\
					for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];\
					psum[j]=sum;}
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

void QMath::amoeba(float **p, float y[], int ndim, float ftol,int *nfunk)
{
  // Multidimensional minimization of the function funk(x) where x[1..ndim]
  // is a vector in ndim dimensions, by the downhill simplex method of Nelder
  // and Mead.  The matrix p[1..ndim+1][1..ndim] is input.  Its ndim+1 rows
  // are ndim-dimensional vectors which are the vertices of the starting
  // simplex.  Also input is the vector y[1..ndim+1], whose components
  // must be pre-initialized to the values of funk evaluated at the 
  // ndim+1 vertices (rows) of p; and ftol the fractional convergence 
  // tolerance to be achieved in the function value (n.b.!).  On output, p
  // and y will have been reset to ndim+1 new points all within ftol of
  // a minimum function value, and nfunk gives the number of function 
  // evaluations taken. 

  
	int i,ihi,ilo,inhi,j,mpts=ndim+1;
	float rtol,sum,swap,ysave,ytry,*psum;
  //float amotry(float **p, float y[], float psum[], int ndim,
  //	float (*funk)(float []), int ihi, float fac);

	psum=vector(1,ndim);
	*nfunk=0;
	GET_PSUM
	for (;;) {
		ilo=1;
		ihi = y[1]>y[2] ? (inhi=2,1) : (inhi=1,2);
		for (i=1;i<=mpts;i++) {
			if (y[i] <= y[ilo]) ilo=i;
			if (y[i] > y[ihi]) {
				inhi=ihi;
				ihi=i;
			} else if (y[i] > y[inhi] && i != ihi) inhi=i;
		}
		rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo]));
		if (rtol < ftol) {
			SWAP(y[1],y[ilo])
			for (i=1;i<=ndim;i++) SWAP(p[1][i],p[ilo][i])
			break;
		}
		if (*nfunk >= NMAX) nrerror("NMAX exceeded");
		*nfunk += 2;
		ytry=amotry(p,y,psum,ndim,ihi,-1.0);
		if (ytry <= y[ilo])
			ytry=amotry(p,y,psum,ndim,ihi,2.0);
		else if (ytry >= y[inhi]) {
			ysave=y[ihi];
			ytry=amotry(p,y,psum,ndim,ihi,0.5);
			if (ytry >= ysave) {
				for (i=1;i<=mpts;i++) {
					if (i != ilo) {
						for (j=1;j<=ndim;j++)
							p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
						y[i]=FuncP(psum);
					}
				}
				*nfunk += ndim;
				GET_PSUM
			}
		} else --(*nfunk);
	}
	free_vector(psum,1,ndim);
}
#undef SWAP
#undef GET_PSUM
#undef NMAX

//-------------------------------


float QMath::amotry(float **p, float y[], float psum[], int ndim,
	int ihi, float fac)
{
  // Extrapolates by a factor fac through the face of the simplex across
  // from the high point, tries it, and replaces the high point if
  // the new point is better.
	int j;
	float fac1,fac2,ytry,*ptry;

	ptry=vector(1,ndim);
	fac1=(1.0-fac)/ndim;
	fac2=fac1-fac;
	for (j=1;j<=ndim;j++) ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
	ytry=FuncP(ptry);
	if (ytry < y[ihi]) {
		y[ihi]=ytry;
		for (j=1;j<=ndim;j++) {
			psum[j] += ptry[j]-p[ihi][j];
			p[ihi][j]=ptry[j];
		}
	}
	free_vector(ptry,1,ndim);
	return ytry;
}


Double_t QMath::plgndr(const int l, const int m, const Double_t x)
{
  int i,ll;
  Double_t fact,pll,pmm,pmmp1,somx2;

  if (m < 0 || m > l || fabs(x) > 1.0)
    nrerror("Bad arguments in routine plgndr");
  pmm=1.0;
  if (m > 0) {
    somx2=sqrt((1.0-x)*(1.0+x));
    fact=1.0;
    for (i=1;i<=m;i++) {
      pmm *= -fact*somx2;
      fact += 2.0;
    }
  }
  if (l == m)
    return pmm;
  else {
    pmmp1=x*(2*m+1)*pmm;
    if (l == (m+1))
      return pmmp1;
    else {
      for (ll=m+2;ll<=l;ll++) {
	pll=(x*(2*ll-1)*pmmp1-(ll+m-1)*pmm)/(ll-m);
	pmm=pmmp1;
	pmmp1=pll;
      }
      return pll;
    }
  }
}


#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

void QMath::four1(float *data, unsigned int nn, const int isign)
{
  //fourier transform from numerical recipes. nn is the number of data points (really the number of entries in the array divided by two, since this is assumed to be complex, with real[1], im[1], real[2], etc making up the data array. MUST be 2^n), isign = 1 does the fft, isign -1 undoes it

	int n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;
	float swap;

	
	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
		  SWAP(data[j-1],data[i-1]);
		  SWAP(data[j],data[i]);
		}
		m=nn;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j-1]-wi*data[j];
				tempi=wr*data[j]+wi*data[j-1];
				data[j-1]=data[i-1]-tempr;
				data[j]=data[i]-tempi;
				data[i-1] += tempr;
				data[i] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

#undef SWAP

void QMath::realft(float *data, unsigned long n, const int isign)
{
  //real fft from numerical recipes. replaces data with the positive frequency half of the complex fourier spectrum. Requires 2^n data arrays.

	int i,i1,i2,i3,i4;
	double c1=0.5,c2,h1r,h1i,h2r,h2i,wr,wi,wpr,wpi,wtemp,theta;

	theta=3.141592653589793238/(double)(n>>1);
	if (isign == 1) {
		c2 = -0.5;
		four1(data,n>>1,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	for (i=1;i<(n>>2);i++) {
		i2=1+(i1=i+i);
		i4=1+(i3=n-i1);
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r= -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i;
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4]= -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[0] = (h1r=data[0])+data[1];
		data[1] = h1r-data[1];
	} else {
		data[0]=c1*((h1r=data[0])+data[1]);
		data[1]=c1*(h1r-data[1]);
		four1(data,n>>1,-1);
	}
}
