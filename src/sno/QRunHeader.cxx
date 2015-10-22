//////////////////////////////////////////////////////////////////
//SNO run header.  Contains information from the Zebra run      //
//header bank, along with some useful utilities.                //
//                                                              //
// Written by:  M. Boulay, Queen's University                   //
//              02/07/1998                                      //
//////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay


#include "QRunHeader.h"

ClassImp(QRunHeader)

QRunHeader::QRunHeader()
{
Initialize();
//Default constructor.
};

QRunHeader::QRunHeader( const QRunHeader &header )
{
  //Copy constructor.
  Date = header.Date;
  mTime = header.mTime;
  DACVersion = header.DACVersion;
  RunNumber = header.RunNumber;
  CalTrial = header.CalTrial;
  SourceMask = header.SourceMask;
  RunMask = header.RunMask;
  GTCrateMask = header.GTCrateMask;
  FirstGTID = header.FirstGTID;
  ValidGTID = header.ValidGTID;
}

QRunHeader::QRunHeader( UInt_t *rawData )
{
  //Create a run header object with raw zdab data returned from
  //QZdabFile::GetZdabBank().

  Date = rawData[5];
  mTime = rawData[6];
  DACVersion = rawData[7];
  RunNumber = rawData[8];
  CalTrial = rawData[9];
  SourceMask = rawData[10];
  RunMask = rawData[11];
  GTCrateMask = rawData[12];
// these two below need to be fixed
  FirstGTID = -1;
  ValidGTID = -1;
  Initialize();
}

QRunHeader::~QRunHeader()
{
//Destructor.
};

QRunHeader &QRunHeader::operator=(const QRunHeader &rhs )
{
  //Assignment operator.
  Date = rhs.Date;
  mTime = rhs.mTime;
  DACVersion = rhs.DACVersion;
  RunNumber = rhs.RunNumber;
  CalTrial = rhs.CalTrial;
  SourceMask = rhs.SourceMask;
  RunMask = rhs.RunMask;
  GTCrateMask = rhs.GTCrateMask;
  FirstGTID = rhs.FirstGTID;
  ValidGTID = rhs.ValidGTID;
  return *this;
}

void QRunHeader::Initialize()
{
  //Initialize the QRunHeader object.

}

Int_t QRunHeader::GetRun()
{
  //Return the current run number.
  return RunNumber;
}

void QRunHeader::Set(Int_t *x)
{
  //Set the run header.   The following elements will be filled
  //Begin_html
  //<li>Date        = x[0] </li>
  //<li>mTime        = x[1] </li>
  //<li>DACVersion  = x[2] </li>
  //<li>RunNumber   = x[3] </li>
  //<li>CalTrial    = x[4] </li>
  //<li>SourceMask  = x[5] </li>
  //<li>RunMask     = x[6] </li>
  //<li>GTCrateMask = x[7] </li>
  //End_html

  Date        = x[0];
  mTime       = x[1];
  DACVersion  = x[2];
  RunNumber   = x[3];
  CalTrial    = x[4];
  SourceMask  = x[5];
  RunMask     = x[6];
  GTCrateMask = x[7];
  FirstGTID   = -1;
  ValidGTID   = -1;
}


void QRunHeader::Set(UInt_t *y)
{
  //Set the run header.   The following elements will be filled
  //Begin_html
  //<li>Date        = x[0] </li>
  //<li>mTime       = x[1] </li>
  //<li>DACVersion  = x[2] </li>
  //<li>RunNumber   = x[3] </li>
  //<li>CalTrial    = x[4] </li>
  //<li>SourceMask  = x[5] </li>
  //<li>RunMask     = x[6] </li>
  //<li>GTCrateMask = x[7] </li>
  //<li>FirstGTID   = x[8] </li>
  //<li>ValidGTID   = x[8] </li>
  //End_html

  Int_t* x = (Int_t*) y;

  Date        = x[0];
  mTime       = x[1];
  DACVersion  = x[2];
  RunNumber   = x[3];
  CalTrial    = x[4];
  SourceMask  = x[5];
  RunMask     = x[6];
  GTCrateMask = x[7];
  FirstGTID   = x[8];
  ValidGTID   = x[9];
}

