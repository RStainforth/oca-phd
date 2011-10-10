//___________________________________________________________________________
//
// Class description for the QNCD class
//
// Stores basic geometrical information on single NCD counter.
// Calculates distance from a straight line to the counter axis. 
// 
//*-- Author : Aksel Hallin, Jose' Maneira
//
//
// 13.06.2005 -- os
// added centre of counter vector and index initialization in constructor.
// 
// 10.2005 -- os
// added methods to allow the user to shift the ncd center vector
// without modifying the positions loaded in the geometry file.
//
#include "QNCD.h"
ClassImp(QNCD)

#include "stdio.h"
#include "QNCDArray.h"

QNCD::QNCD(Float_t x,Float_t y,Float_t length,Int_t index){
	Float_t r2 = x*x + y*y;
	Float_t z  = sqrt(360000-r2);
	fBottom.SetXYZ(x,y,-z+25.04); fTop.SetXYZ(x,y,-z+length+52.22);
	fCenter.SetXYZ(x,y,0);	// this ncd x-y coordinates -- os
	fIndex = index;		// this ncd index -- os
	
	// shift flag
	fShifted = kFALSE;

}
// QNCD::QNCD(char *aName,QNCDArray *anArray, float aLength){
//   //aLength is the active length of the counters, to this we add the delay line(16.13 cm
//   //and the Bell end (11.05 cm)
//   fName[0]=aName[0];
//   fName[1]=aName[1];
//   fName[2]=0;
//   float x,y,z,r2;
//   if(NCDCoords(fName,x,y,anArray)){
//        r2=x*x+y*y;
//        z=sqrt(360000-r2);
//        fBottom.SetXYZ(x,y,-z+25.04); fTop.SetXYZ(x,y,-z+aLength+52.22);
//   }else{
//     printf("Bad array element %s\n",fName);
//   }
//   fPresent=1;
// };
// 
// int QNCD::NCDCoords(const char *aStr, float &ax, float &ay, QNCDArray *anArray){
//   int radius=aStr[0]-'A';
//   int phi=aStr[1]-'1';
//   int i;
//   int goodvalue=1;
//   if(anArray->diag[radius])i=phi*2; else i=phi;
//   ax=anArray->x1[radius]; ay=anArray->x2[radius];
//   if(radius>13 || radius<0 || phi<0 || phi > 7)i=8;
//   switch (i){
//   case 0: ax=-anArray->x2[radius]; ay=anArray->x1[radius]; break;
//   case 1: ax=-anArray->x1[radius]; ay=anArray->x2[radius]; break;
//   case 2: ax=-anArray->x1[radius]; ay=-anArray->x2[radius]; break;
//   case 3: ax=-anArray->x2[radius]; ay=-anArray->x1[radius]; break;
//   case 4: ax=anArray->x2[radius]; ay=-anArray->x1[radius]; break;
//   case 5: ax=anArray->x1[radius]; ay=-anArray->x2[radius]; break;
//   case 6: ax=anArray->x1[radius]; ay=anArray->x2[radius]; break;
//   case 7: ax=anArray->x2[radius]; ay=anArray->x1[radius]; break;
//   default: ax=0; ay=0; goodvalue=0; break;
//   }
//   //origin.setX(0.25);    printf("Coordinates of %s are %f %f\n",aStr,ax,ay);
//   return goodvalue;
// }

//-------------------------------------------------------------------------
TVector3 QNCD::GetClosestPoint(TVector3 &aPosition,TVector3 &aDirection, Bool_t shift){

	// This assumes that the ncds are vertical and the direction is not
	
	Double_t u,v,xp,xn,yp,yn,length;
	TVector3 normdir(aDirection.Unit());
	u = normdir.X();
	v = normdir.Y();
	if (u == 0 && v==0) return aPosition;
	xp = aPosition.X();
	yp = aPosition.Y();
	
	// 10.2005 -- os
	// allow the ncd center to be shifted by the user
	if(shift){ // use fCenter that can be modified
	  xn = fCenter.X();
	  yn = fCenter.Y();
	} else {   // use fTop that cannot be modified
	  xn = fTop.X();
	  yn = fTop.Y();
	}

	length = (u*(xn-xp) + v*(yn-yp))/(u*u + v*v);
	if (length < 0) length = 0.0;

	TVector3 closest(aPosition + length*normdir);
	return closest;
	
}
//-------------------------------------------------------------------------
Double_t QNCD::GetDistance(TVector3 &aPosition,TVector3 &aDirection,Int_t &status){

	// Returns the distance between the straight line and and the counter axis.
	// NOT the surface. This should be done by the calling routine, according to
	// the required tolerance.
	// The status flag can assume the values:
	//    status = 0  if the line is vertical
	//    status = 1  general case
	//    status = 10 the closest point to the axis is above the top
	//    status = 20 the closest point to the axis is below the AV 
	
	status = 1;
	TVector3 closest(0,0,0);
	if (aDirection.X() == 0 && aDirection.Y()==0) {
		closest = aPosition;
		status = 0;
	} else { 
		closest = GetClosestPoint(aPosition,aDirection);
	}
	
	if (closest.Z() > fTop.Z()) status = 10;
//	if (closest.Z() < fBottom.Z()) status = 20;
//  include rope length
	if (closest.Z() < (fBottom.Z()-25.04)) status = 20; 
	
	TVector3 counter(fTop);
	closest.SetZ(0.0);
	counter.SetZ(0.0);
	return (closest-counter).Mag();
		
}

//-------------------------------------------------------------------------
void QNCD::SetCenter(TVector3 aVector)
{
  // Modifies the fCenter vector and sets the shift bit.
  // Does not consider the z-component of the vector given
  // in argument.

  Double_t x = aVector.X();
  Double_t y = aVector.Y();
  
  Double_t radius = sqrt(pow(x,2) + pow(y,2));

  // check if new center is within AV limits
  if((radius <= 0.) || (radius > 600.)){
    return;
  }

  fCenter.SetXYZ(x,y,0.);
  fShifted = kTRUE; // ncd center has been shifted

  return;
}

//-------------------------------------------------------------------------
void QNCD::Shift(Double_t aShiftinX, Double_t aShiftinY)
{
  // Shifts the fCenter vector components by the amount given in argument.

  Double_t xshifted = fCenter.X();
  Double_t yshifted = fCenter.Y();

  xshifted += aShiftinX;
  yshifted += aShiftinY;

  Double_t radius = sqrt(pow(xshifted,2) + pow(yshifted,2));

  // check if shift is within AV limits
  if((radius <= 0.) || (radius > 600.)){
    return;
  } 

  fCenter.SetXYZ(xshifted,yshifted,0.);
  fShifted = kTRUE; // ncd center has been shifted
  
  return;
}

//-------------------------------------------------------------------------
void QNCD::Shift(TVector3 aShift)
{
  // Shifts the fCenter vector components by adding the vector passed
  // in argument. The z-component of aShift is ignored.

  Double_t xshifted = fCenter.X();
  Double_t yshifted = fCenter.Y();

  xshifted += aShift.X();
  yshifted += aShift.Y();

  Double_t radius = sqrt(pow(xshifted,2) + pow(yshifted,2));

  // check if shift is within AV limits
  if((radius <= 0.) || (radius > 600.)){
    return;
  } 

  fCenter.SetXYZ(xshifted,yshifted,0.);
  fShifted = kTRUE; // ncd center has been shifted
  
  return;
}

//-------------------------------------------------------------------------
void QNCD::CancelShift()
{
  // Cancel the shift applied by the user, if applicable.

  if(!fShifted) return;

  fCenter.SetXYZ(fTop.X(),fTop.Y(),0.); // fCenter has no z-component
  fShifted = kFALSE;
  
  return;
}
