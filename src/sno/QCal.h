// File:	QCal.h
// Author:	Phil Harvey - 12/9/98


//              Mark Boulay - 1/23/99 Moved from QPCA to QCal

#ifndef __QCAL__
#define __QCAL__

#include "TObject.h"
#include "QSNO.h"

class QEvent;
class QPMT;

class QCal : public QSNO {
public:

	QCal();
	virtual ~QCal();
	
	virtual Int_t	Sett(QPMT *aPmt);	//{ aPmt->Sett(0.0);	return(-1); }
	virtual Int_t	Sethl(QPMT *aPmt);	//{ aPmt->Sethl(0.0);	return(-1); }
	virtual Int_t	Seths(QPMT *aPmt);      //{ aPmt->Seths(0.0);	return(-1); }
	virtual Int_t	Setlx(QPMT *aPmt);      //{ aPmt->Setlx(0.0);	return(-1); }
	
	virtual Int_t	Calibrate(QPMT *aPmt);
	virtual Int_t	Calibrate(QEvent *anEvent);
	
	ClassDef(QCal,0)	// Base class for PMT calibration
};

extern QCal	*gCal;

#endif
