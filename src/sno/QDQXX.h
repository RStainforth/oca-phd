#ifndef __QDQXX_H
#define __QDQXX_H

//*--Author: Mark Boulay

#include "QBank.h"
#include "QTitles.h"

class QDQXX : public TObject
{
 public:
 QDQXX( const char *titles = NULL );
 ~QDQXX();

 QBank *GetDQCH(Int_t i){ if ( i >= 0 && i < 19 ) return fDQCH[i]; else return 0; }
 QBank *GetDQCR(Int_t i){ if ( i >= 0 && i < 19 ) return fDQCR[i]; else return 0; }
 
 Int_t GetDQXXByLcn(Int_t, Int_t &, Int_t &);
 void SetDQCH(Int_t i, QBank *bank = NULL );
 void SetDQCR(Int_t i, QBank *bank = NULL );
 Int_t ReadTitles( const char *titles = NULL );
 Int_t GetNFECs();
 Int_t LcnInfo( Int_t i_dqch, Int_t i_dqcr, Int_t i_mode, Int_t lcn);
 Int_t LcnInfo( Int_t lcn, Int_t i_mode );
 Int_t GetNonline();
 ClassDef(QDQXX,0)//DQXX channel status utility
   
   private:
 QBank **fDQCH;   //DQCH titles banks
 QBank **fDQCR;   //DQCR titles banks
 void Initialize();
 void Clear(const Option_t *option ="");
};

#endif







