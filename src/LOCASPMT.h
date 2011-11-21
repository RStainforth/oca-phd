#ifndef _LOCASPMT_H_
#define _LOCASPMT_H_

#include <vector>

#include "TObject.h"
#include "Riostream.h"
#include "TVector.h"

//*-- Author : Simon JM Peeters
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// LOCASPMT                                                             //
//                                                                      //
// Singleton class to access information about individual PMTs          //
// I'm avoiding mentioning anything about RAT here to avoid             //
// rootcint complications                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//
// Revision History:
// 13 Oct 2011 - Created
//
class LOCASPMT : public TObject
{
public:
  virtual ~LOCASPMT();
	static LOCASPMT * Get(void);
	
	TVector * Position(UInt_t lcn);
	double  X(UInt_t lcn) {if(lcn<fX.size()){return fX[lcn];}else{return 0.;}}
	double  Y(UInt_t lcn) {if(lcn<fY.size()){return fY[lcn];}else{return 0.;}}
	double  Z(UInt_t lcn) {if(lcn<fZ.size()){return fZ[lcn];}else{return 0.;}}
	TVector * Direction(UInt_t lcn);
	double  dirX(UInt_t lcn) {if(lcn<fdirX.size()){return fdirX[lcn];}else{return 0.;}}
	double  dirY(UInt_t lcn) {if(lcn<fdirY.size()){return fdirY[lcn];}else{return 0.;}}
	double  dirZ(UInt_t lcn) {if(lcn<fdirZ.size()){return fdirZ[lcn];}else{return 0.;}}
	

protected:
  LOCASPMT( void);
  LOCASPMT( const LOCASPMT &pmt );
  LOCASPMT &operator=(const LOCASPMT &rhs);

	static LOCASPMT * fLOCASPMT;
	
	bool InitGeoPMT(void);
	
private:
	std::vector<double> fX;
	std::vector<double> fY;
	std::vector<double> fZ;
	std::vector<double> fdirX;
	std::vector<double> fdirY;
	std::vector<double> fdirZ;
};

#endif



