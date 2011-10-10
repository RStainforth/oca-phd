//////////////////////////////////////////////////////////////////
//Unpacked trigger word from an event.                          //
//Written by M. Boulay 06/98                                    //
//The follwing trigger bits will be unpacked:                   //
//Begin_html                                                    //
//   <l>NHIT_100_LO:  Low Nhit, 100nsec windowed trigger        //
//   <l>NHIT_100_MED: Medium Nhit, 100nsec windowed trigger     //
//   <l>NHIT_100_HI:  High Nhit, 100nsec windowed trigger       // 
//   <l>NHIT_20:      20 nsec windowed Nhit trigger             //
//   <l>NHIT_20_LB:   20 nsec windowed LB Nhit trigger          //
//   <l>ESUM_LO:      Low Esum trigger                          //
//   <l>ESUM_HI:      High Esum trigger                         //
//   <l>OWLN:         Outward looking tube Nhit trigger         //
//   <l>OWLE_LO:      Outward looking tube Esum low trigger     //
//   <l>OWLE_HI:      Outward looking tube Esum high trigger    //
//   <l>PULSE_GT:     Pulser generated trigger                  //
//   <l>PRESCALE:     Prescaled trigger                         //
//   <l>PEDESTAL:     ECA pedestal task trigger                 //
//   <l>PONG:         GPS PONG trigger                          //
//   <l>SYNC:         Syncronous trigger                        //
//   <l>EXT_ASYNC:    Externally generated (a)syncronous trigger//
//   <l>EXT2:         External trigger 2                        //
//   <l>EXT3:         External trigger 3                        //
//   <l>EXT4:         External trigger 4                        //
//   <l>EXT5:         External trigger 5                        //
//   <l>EXT6:         External trigger 6                        //
//   <l>EXT7:         External trigger 7                        //
//   <l>PULSE_ASYNC:  Pulser generated asyncronous trigger      //
//   <l>SPECIAL_RAW:  Special trigger                           //
//   <l>NCD:          Neutral current detector trigger          //
//   <l>SOFT_GT:      Software generated trigger                //
//   <l>MISTRIGGER:   Second trigger delayed by more than 20 nsec//
//End_html                                                       //
//////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay                          
//* Copyright (c) 1998 Mark Boulay, All Rights Reserved.  

#include "QTrigger.h"
ClassImp(QTrigger)
  

QTrigger *gQTrigger = new QTrigger();
QTrigger::QTrigger()
{
  //Default constructor.
  cTriggerNames = new char*[27];
  for (Int_t k = 0; k<27; k++)
  {
   cTriggerNames[k] = new char[40];
  };
  // cTriggerNames = new TString[27];
 strcpy(cTriggerNames[0],"NHIT_100_LO");
 strcpy(cTriggerNames[1],"NHIT_100_MED");
 strcpy(cTriggerNames[2],"NHIT_100_HI");
 strcpy(cTriggerNames[3],"NHIT_20");
 strcpy(cTriggerNames[4],"NHIT_20_LB");
 strcpy(cTriggerNames[5],"ESUM_LO");
 strcpy(cTriggerNames[6],"ESUM_HI");
 strcpy(cTriggerNames[7],"OWLN");
 strcpy(cTriggerNames[8],"OWLE_LO");
 strcpy(cTriggerNames[9],"OWLE_HI");
 strcpy(cTriggerNames[10],"PULSE_GT");
 strcpy(cTriggerNames[11],"PRESCALE");
 strcpy(cTriggerNames[12],"PEDESTAL");
 strcpy(cTriggerNames[13],"PONG");
 strcpy(cTriggerNames[14],"SYNC");
 strcpy(cTriggerNames[15],"EXT_ASYNC");
 strcpy(cTriggerNames[16],"EXT2");
 strcpy(cTriggerNames[17],"EXT3");
 strcpy(cTriggerNames[18],"EXT4"); 
 strcpy(cTriggerNames[19],"EXT5");
 strcpy(cTriggerNames[20],"EXT6");
 strcpy(cTriggerNames[21],"EXT7");
 strcpy(cTriggerNames[22],"PULSE_ASYNC");
 strcpy(cTriggerNames[23],"SPECIAL_RAW");
 strcpy(cTriggerNames[24],"NCD");
 strcpy(cTriggerNames[25],"SOFT_GT");
 strcpy(cTriggerNames[26],"MISTRIGGER");

};

QTrigger::~QTrigger(){
  //Destructor.;
if (gQTrigger==this) gQTrigger = NULL;
};

Int_t QTrigger::HasTrig(Int_t mTrigWord, char *cTrig)
{
  //Boolean function returns true or false based
  //on whether mTrigWord has the bit corresponding
  //to the trigger cTrig set.
  //
  //Begin_html
  //eg:  if we have a trigger word t,
  //    <l> root[0]: QTrigger a;
  //    <l> root[1]: Int_t m=a.HasTrig(t,"SOFT_GT");
  //    <l> m = 1 if bit 25 of t is set, else m = 0.
  //End_html
  
   Int_t i, jtrig= -1;//number of bit corresponding to mTrigword;
  for (i=0; i < 27; i++)
    {

      //char *tempName = cTriggerNames[i];
      if ( !strcmp(cTriggerNames[i],cTrig) )
	   {
	     jtrig = i;
	   }   
    }
  if (jtrig == -1 )
    {
      PLog("Error in QTrigger: no such trigger.",1);
      return jtrig;
    }
  
 
  if ( 1<<jtrig&mTrigWord ) return 1;
  else return 0;
};     
/*Int_t QTrigger::HasTrig(char *cTrig)
{

  //Boolean function returns true or false based
  //on whether mTrigWord has the bit corresponding
  //to the trigger cTrig set.

 Int_t i, jtrig= -1;//number of bit corresponding to mTrigword;
  for (i=0; i < 27; i++)
    {

      //char *tempName = cTriggerNames[i];
      if ( !strcmp(cTriggerNames[i],cTrig) )
	   {
	     jtrig = i;
	   }   
    }
    //if (jtrig == -1 ) return jtrig;
  if (jtrig == -1 )
  {
    PLog("Error in QTrigger: no such trigger.",1);
    return 0;
  }
  //Int_t iTrigWord = (Int_t)mTrigWord;
  if ( 1<<jtrig&Trig_type ) return 1;
  else return 0;
};
*/















