////////////////////////////////////////////////////////////
// Base class for calibrator objects                      //
// - Provides functions for PMT calibration               //
// - Global object gPCA is used by QPMT for calibration   //
////////////////////////////////////////////////////////////
//*-- Author :	Phil Harvey - 12/9/98
// 1/23/98   Renamed to QCal   M Boulay

#include "QCal.h"
#include "QEvent.h"
#include "TClonesArray.h"

ClassImp(QCal)

QCal::QCal()
{
	// QCal constructor
	// - sets global gCal pointer to 'this'
	
	gCal = this;	// set the global Cal object to this
}

QCal::~QCal()
{
	// QCal destructor
	
	if (gCal == this) gCal = NULL;
}

Int_t	QCal::Sett(QPMT *aPmt)	{ aPmt->Sett(-9999);	return(-1); }
Int_t	QCal::Sethl(QPMT *aPmt)	{ aPmt->Sethl(-9999);	return(-1); }
Int_t	QCal::Seths(QPMT *aPmt)	{ aPmt->Seths(-9999);	return(-1); }
Int_t	QCal::Setlx(QPMT *aPmt)	{ aPmt->Setlx(-9999);	return(-1); }

Int_t QCal::Calibrate(QPMT *aPmt)
{
	// Set all calibrated values (t,hl,hs,lx) in PMT object
	// - return value is less than zero on error
	// Note: individual Set() functions in derived classes should return 0
	//       if successful, <0 otherwise.
	
	return(Sett(aPmt) + Sethl(aPmt) + Seths(aPmt) + Setlx(aPmt));
}

Int_t QCal::Calibrate(QEvent *anEvent)
{
	// Calibrate all PMT's in an event
	// - calls Calibrate(QPMT*) for each PMT in event
	
	Int_t	i, n, rtnVal=0;
	QPMT	*aPmt;

	// calibrate regular PMTs
	n = anEvent->GetQPMTs()->GetLast() + 1;
	for (i=0; i<n; ++i) {
		aPmt = (QPMT *)anEvent->GetQPMTs()->At(i);
		rtnVal += Calibrate(aPmt);
	}
	// calibrate OWLs
	n = anEvent->GetQOWLs()->GetLast() + 1;
	for (i=0; i<n; ++i) {
		aPmt = (QPMT *)anEvent->GetQOWLs()->At(i);
		rtnVal += Calibrate(aPmt);
	}
	// calibrate low gain tubes
	n = anEvent->GetQLGs()->GetLast() + 1;
	for (i=0; i<n; ++i) {
		aPmt = (QPMT *)anEvent->GetQLGs()->At(i);
		rtnVal += Calibrate(aPmt);
	}
	// set calibrated flag for this event
	anEvent->SetBit(kCalibrated,kTRUE);

	return(rtnVal);
}













