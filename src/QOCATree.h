#ifndef _OCA_TREE_H
#define _OCA_TREE_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// QOCATree                                                             //
//                                                                      //
// Data structure usefull for storing large sets of optical calibration //
// laserball runs.                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*-- Author : Mark Boulay
//*-- Author : Bryce Moffat - updated 24-Apr-2000 for inclusion in libqoca
//*-- Copyright (c) 1999 SNO Software Project, All rights reserved

#include <TTree.h>
#include "QOCARun.h"

// ------------------------------------------------------------------------
//  QOCATree class definition
// ------------------------------------------------------------------------

class QOCATree : public TTree {
  
 private:
  QOCARun *fQOCARun;  //Single OCA run object

  void SetRunAddress(QOCARun *ocarun);

 public:
  QOCATree();
  QOCATree(const char *name, const char *title);
  ~QOCATree();

  QOCATree(QOCATree &rhs);  // copy constructor: header only!
  QOCATree &operator=(const QOCATree &rhs); // copies header only!

  //  QOCAPmt *GetQOCAPmt(Int_t entrycode=0);

  void SetQOCARun(QOCARun *orun = NULL);
  QOCARun *GetQOCARun();

  ClassDef( QOCATree,3) // OCA tree data structure
};

#endif
