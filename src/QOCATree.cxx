//*-- Author : Mark Boulay 28/11/99
//*--Author : Bryce Moffat - updated 24-Apr-2000 for libqoca from libqtree
//*-- Copyright (c) 1999 SNO Software Project, All Rights Reserved.

////////////////////////////////////////////////////////////////////////////////
// QOCATree                                                                   //
// Data structure usefull for storing large sets of optical calibration       //
// laserball runs.                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "QOCATree.h"

ClassImp(QOCATree)
;
//______________________________________________________________________________
//
// QOCATree class.
//
// Data structure usefull for storing large sets of optical calibration
// laserball runs.
//
//______________________________________________________________________________
QOCATree::QOCATree( const char *name, const char *title ) : TTree (name, title)
{
  // Default constructor for new OCA tree

  fQOCARun = new QOCARun(); //Create the QOCARun object and then branch the tree
  //Branch("Runs","QOCARun",&fQOCARun,4000,1);
  Branch("Runs","QOCARun",&fQOCARun,4000,99);
  SetBranchAddress("Runs",&fQOCARun);
}
//______________________________________________________________________________
QOCATree::QOCATree()
{
  // Default constructor used when a QOCATree is read in from file

  fQOCARun = new QOCARun();
}
//______________________________________________________________________________
QOCATree::~QOCATree()
{
  // Default destructor

  if (fQOCARun) delete fQOCARun;
}
//______________________________________________________________________________
QOCATree::QOCATree(QOCATree &rhs)
{
  // Copy constructor for QOCATree objects
  // Doesn't copy any run or pmt information

  fQOCARun = rhs.fQOCARun;
}
//______________________________________________________________________________
QOCATree & QOCATree::operator=(const QOCATree &rhs)
{
  // QOCATree assignment operator.
  // Only copies the base tree structure and header information, not the
  // individual runs or pmt's.

  fQOCARun = rhs.fQOCARun;
  return *this;
}
//______________________________________________________________________________
void QOCATree::SetRunAddress(QOCARun *ocarun)
{
  //Set the Run branch address.

  fQOCARun = ocarun;
  SetBranchAddress("Runs",&fQOCARun);
}
//______________________________________________________________________________
void QOCATree::SetQOCARun(QOCARun *orun)
{
  // Set the QOCARun branch address and the pointer fQOCARun.
  // If the (default) NULL pointer is passed, this points fQOCARun at fQOCARun
  // which is supposed to be used for storage of runs for this tree.

  if (!orun) {
    if (!fQOCARun) fQOCARun = new QOCARun();
    SetRunAddress(fQOCARun);
  } else {
    if (fQOCARun) delete fQOCARun;
    SetRunAddress(orun);
  }
}
//______________________________________________________________________________
QOCARun *QOCATree::GetQOCARun()
{
  // Return a pointer to the QOCARun used for GetEntry() calls (ie. fQOCARun)

  if (!fQOCARun) {
    printf("Had to create QOCARun for %s\n",GetTitle());
    fQOCARun = new QOCARun();
  }

  SetRunAddress(fQOCARun);

  return fQOCARun;
}
//______________________________________________________________________________
//  QOCAPmt *QOCATree::GetQOCAPmt(Int_t entrycode)
//  {
//    // Special get PMT based on an entry code

//    printf("Entry code %d\n",entrycode);
//    return NULL;
//  }
//______________________________________________________________________________
void QOCATree::Streamer(TBuffer &R__b)
{
  // Stream an object of class QOCATree.
  // No need to stream the fQOCARun pointer or object - this is only used for
  // filling the QOCATree.
  //
  // Updated 29-Jun-2001 for ROOT 3.00 "Automatic Schema Evolution" - BA Moffat

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if (R__v > 1) {
      QOCATree::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      return;
    }
    //====process old versions before automatic schema evolution
    TTree::Streamer(R__b);
    // fQOCARun = NULL;  // Should this be set to NULL on read? Hmmm... 28-Dec-2000
    // R__b >> fQOCARun;
    R__b.CheckByteCount(R__s, R__c, QOCATree::IsA());
    //====end of old versions
  } else {
    QOCATree::Class()->WriteBuffer(R__b,this);
  }
}
//______________________________________________________________________________
