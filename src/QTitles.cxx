////////////////////////////////////////////////////////////////////////////////////
//QTitles                                                                         //
//                                                                                //  
//QTitles allows titles files to be read into arrays of floats and integers       //
//Hollerith and hex formats are read in.                                          //
//                                                                                //
//All of the bizarre acceptable formats for zebra data that I know about are      //
//handled properly.                                                               //
//                                                                                //
//The only caveat is that banks are identified by their name and number. This is  //
//not necessarily unique. There are a couple of banks in snoman which have        //
//identical names and numbers. They are distinguished by data in the header. Thus //
//there are two banks with the same name and number in atitles file, only the     //
//first will be read in.                                                          // 
//                                                                                //
//Edited to load whichever bank matches the validity date in the header.  This    //
//can be extended to other criteria when necessary. -- rsd 2003.08.20             //
////////////////////////////////////////////////////////////////////////////////////

//*-- Author : Chris Jillings

#include <string.h>
#include "QTitles.h"


ClassImp(QTitles)

QTitles::QTitles() {
  Initialize( 100 );
}

QTitles::QTitles(char* aFilename, char* aBank, Int_t aBankNum, Int_t aSize) {
  //QTitles constructor
  //Loads title bank aBank from the full path
  Initialize(aSize);
  SetBankName(aBank);
  SetBankNum(aBankNum);
  SetFilename(aFilename);
  
  LoadBank();
}

QTitles::QTitles(char* aFilename, char* aBank, Int_t aBankNum, Int_t date, Int_t time, Int_t aSize) {
  // Overloaded constructer.  
  // Loads bank appropriate to date.
  Initialize(aSize);
  SetBankName(aBank);
  SetBankNum(aBankNum);
  SetFilename(aFilename);
  
  LoadBankInfo();
  Int_t bankIndex = findValidBank(date,time);
  LoadBank(bankIndex);
}

QTitles::~QTitles() {
  //QTitles destructor
  if (fDataF) {
    delete[] fDataF;
    fDataF = NULL;
  }
  //delete[] fDataI;
  if (fFilename) {
    delete[] fFilename;
    fFilename = NULL;
  }
  if (fValidDate) {
    delete[] fValidDate;
    fValidDate = NULL;
  }
  if (fValidTime) {
    delete[] fValidTime;
    fValidTime = NULL;
  }
}

void QTitles::Initialize(Int_t aSize) {
  fStartSize = aSize;
  fSize = aSize;
  fDataF = new Float_t[fSize];
  fDataI = (Int_t *)fDataF;  // use same memory for fDataF and fDataI
                             //new Int_t[fSize];
  fHeaderSize = 30;
  fHeader = new Int_t[fHeaderSize];
  
  fFilename = NULL;
  
  fValidDate = NULL;
  fValidTime = NULL;
  fNumData = 0;
}

void QTitles::SetBankName(char *aBank) {
  if(strlen(aBank)>4) fatalError("Bank name longer than 4 characters\n");
  strcpy(fBank,aBank);
}

void QTitles::SetBankNum(Int_t aBankNum) {
  fBankNum = aBankNum;
}

void QTitles::SetFilename(char *aFilename) {
  if( aFilename!=NULL ) {
    fFilename = new char[strlen(aFilename)+1];
    strcpy(fFilename,aFilename);
  }
}

void QTitles::LoadBank(Int_t bankIndex) {
  
  if(!fFilename) return;
  
  if (!openFile()) {
    char errorString[1024];
    sprintf(errorString,"QTitles::QTitles() could not find file %s\n",fFilename);
    fatalError(errorString);
    return;
  }
  
  findBank(bankIndex);
  readBank();
  fclose(fFin);
  
}

Bool_t QTitles::openFile() {
  fFin = fopen(fFilename,"r");
  if (fFin==NULL) return kFALSE;
  else return kTRUE;
  
}

void QTitles::readBank() {
  // have just called find bank. need to read in the data
  // for now skip header
  readHeader();
  readData();
  
}

void QTitles::readData() {
  fNumData = 0;
  
  for ( ; ; ) {
    // changed for single banks in a file
    // EOF indicates end of bank!
    if( getNextNonCommentLine()==kFALSE ) break;
    // start of next titles bank indicates end of this one 
    if(fFileline[0] == '*' && fFileline[1] == 'D' && fFileline[2]=='O' ) break;
    // now beleive that the line contains titles data
    //printf("%s\n",fFileline);
    readLine();
    
  }
  
}

Int_t QTitles::dataType(char* aStart) {
  // reads the string to WS looking for a "." a "*" or a "0#" 
  char* endPtr = aStart;
  Bool_t isMultiple = kFALSE;
  Bool_t isFloat = kFALSE;
  Bool_t isHex = kFALSE;
  Bool_t isInteger = kTRUE;
  Bool_t isHollerith = kFALSE;
  
  while( !isWS(*endPtr) ) {
    if( (*endPtr)=='.' ) { isFloat = kTRUE; isInteger = kFALSE; }
    if( (*endPtr)=='*' ) { isMultiple = kTRUE; }
    if( (*endPtr)=='#' ) {
      if( (*(endPtr+1))!='x' ) fatalError("Bad hex format");
      isHex = kTRUE;
      (*(endPtr)) = '0';  //correct bad hex format
      isInteger = kFALSE; 
    }
    if( (*endPtr)=='\'' ) isHollerith = kTRUE;
    if( (*endPtr)=='\"' ) isHollerith = kTRUE;
    endPtr++;
  }
  Int_t retvar = 0;
  if( isFloat ) retvar = 1;
  if( isHex ) retvar = 2;
  if( isHollerith ) retvar =3;
  if( isMultiple ) retvar += 10;
  
  return retvar;
  
  
}


void QTitles::readLine() {
  // have a line of unknown number of titles entries
  // check array size; augment if necessary
  
  char* cptr; // utility pointer for scanning along line
              //  char* endptr;
  Bool_t isEOL=kFALSE;
  Int_t i=0,j;
  cptr = fFileline;
  Int_t currentDataType;
  Int_t multipleCounter;  // used for entries in form $$*$$$
  Float_t multipleF;    
  Int_t multipleI;
  Int_t hollCount;
  
  Int_t buffLength;
  char *tempBuffer;
  Int_t scanCount; // number of words read by sscanf
  
  // this loop is infinite. Escape is via breaks if newline, null or
  // comment reaached. Otherwise just keep looping. Note the line will
  // contain some data as it was found with getNextNonCommentLine().
  while( 1 ) { 
    
    if( fNumData==fSize ) increaseArray();
    while( isWS(*cptr) ) { // do not uses elimWS here
      cptr=fFileline+i;
      i++;
    }
    j=1;
    
    // read current entry inline
    scanCount = 1;
    currentDataType = dataType(cptr);
    if( currentDataType<10 ) {  // not multiple( ie no *)
      if (currentDataType==0 ) scanCount = sscanf(cptr,"%d ",fDataI+fNumData);
      if (currentDataType==1 ) scanCount = sscanf(cptr,"%f ",fDataF+fNumData);
      if (currentDataType==2 ) scanCount = sscanf(cptr,"%x ",fDataI+fNumData);
      if( scanCount==0 ) fatalError("Bad sscanf in QTitles::ReadLine()\n");
      if (currentDataType==3 ) {
        if( (*cptr)=='\"'){
          char *strptr=(char *)(fDataI+fNumData);
          int ch;
          cptr++;
          for(ch=0; (cptr[ch]!='\"' && cptr[ch]!=0 && cptr[ch]!='\n'); ch++){
            strptr[ch]=cptr[ch];
            if(fNumData+ch/4 +4 >= fSize){
              increaseArray(); strptr=(char *)(fDataI + fNumData);
            }
          }	    
          cptr += ch;
          if(*cptr == '\"')cptr++;
          if(ch%4!=0)fatalError(" Strings must be multiples of 4 bytes\n");
          fNumData += ch/4 - 1;  //"-1" is necessary to allow fNumData++ for all types
          i=cptr-fFileline;
        } 
        else{
          int ctr;
          if( (*cptr)=='\'') {
            for(ctr=1;ctr<=5;ctr++)if(cptr[ctr]=='\'')break;
          }
          
          if( ctr>5 ) {	    
            fatalError("Bad Hollerith format in QTitles::readLine() - line 255\n");
          }
          fDataI[fNumData] = 0;
          for( hollCount=0 ; hollCount<ctr-1 ; hollCount++ ) {
            fDataI[fNumData] += cptr[hollCount+1] << (hollCount*8);
          }
        }
      }
      fNumData++;
    } else {
      if (currentDataType==10 ) scanCount = sscanf(cptr,"%d*%d",&multipleCounter,&multipleI);
      if (currentDataType==11 ) scanCount = sscanf(cptr,"%d*%f",&multipleCounter,&multipleF);
      if (currentDataType==12 ) scanCount = sscanf(cptr,"%d*%x",&multipleCounter,&multipleI);
      if( scanCount==0 )  fatalError("Bad sscanf in QTitles::ReadLine()\n");
      if (currentDataType==13 ) {
        buffLength = strlen(cptr)+1;
        if( buffLength>15 ) {
          fatalError("Bad multiple Hollerith in QTitles::readLine() - line 272\n");
        }
        tempBuffer = new char[buffLength];
        sscanf(cptr,"%d*%s",&multipleCounter,tempBuffer);
        if( (*tempBuffer)!='\'' || tempBuffer[5]!='\'' ) {
          fatalError("Bad Hollerith format in QTitles::readLine() - line 277\n");
        }
        multipleI = 0;
        for( hollCount=0 ; hollCount<4 ; hollCount++ ) {
          multipleI += tempBuffer[hollCount+1] << (hollCount*8);
        }
        
      }
      // check array sizes.
      for( j=0 ; j<multipleCounter ; j++ ) {
        if( fNumData == fSize ) increaseArray();
        if (currentDataType==10 || currentDataType==12 || currentDataType==13 ) {
          fDataI[fNumData] = multipleI;
          fNumData++;
        } 
        if( currentDataType == 11 ) {
          fDataF[fNumData] = multipleF;
          fNumData++;
        }
        
      }
    }
    
    // scan along line looking for WhiteSpace
    while( !isWS(*cptr) ) {
      /// Added by Chris Jillings, June 28, 2007 in case there is whitespace
      /// in a Hollerith (and cleaned up by P. Harvey)
      if( *cptr=='\'' || *cptr=='\"' ) {
        char ch = *(cptr++);
        for (int j=1; j<=5; j++, cptr++) {
            if (*cptr==ch) {
                ++cptr;
                break;
            }
            if (*cptr=='\0' || *cptr=='\n') break;
        }
        i = cptr - fFileline + 1;
      } else { // the else block is the original code
        cptr = fFileline + i;
        i++;
      }
    }
    // WS found. What is it? Is it a new line? a null? 
    if( *cptr=='\0' || *cptr=='\n' ) break;
    // skip ahead to start of next bit of text
    while( isWS(*cptr) ) {
      if( cptr[0]=='\n' || cptr[0]=='\0' ) { isEOL = kTRUE; break; }
      cptr=fFileline+i;
      i++;
    }
    // if text is a comment marker, we are done this line
    if( isEOL ) break;
    if( *cptr=='#' && *(cptr+1)=='.' ) break; 
    
    //    if( *cptr=='#' && *(cptr+1)!='.' ) fatalError(fFileline); 
    //     --MGB
  }
  
}

Bool_t QTitles::isWS(char aCh) {
  Bool_t retvar;
  if( aCh==' ' || aCh=='\n' || aCh=='\0' || aCh=='\t') retvar = kTRUE;
  else retvar = kFALSE;
  return retvar;
  
}

void QTitles::readHeader() {
  fNumHeader = 0;
  
  for( ; ; ) {
    if( getNextNonCommentLine()==kFALSE ) 
      fatalError("End of file while reading header in QTitles\n");
    readHeaderLine();
    if( fNumHeader==fHeaderSize ) break;
  }
}

void QTitles::readHeaderLine() {
  // have a line of unknown number of titles entries
  // check array size; augment if necessary
  
  char* cptr; // utility pointer for scanning along line
  Int_t i=0;
  cptr = fFileline;
  
  while( 1 ) {
    // elim leading whitespece without using elimWS
    while( isWS(*cptr) ) {
      cptr=fFileline+i;
      i++;
    }
    
    // read first (only?) entry inline
    Int_t headerInteger; // number
    Int_t rpt;  //repeat counter
    
    // handle entries like 10*0,(rpt*f) 
    if(sscanf(cptr,"%i*%f",&rpt,(float *)&headerInteger)==2){
      for(;rpt>0;rpt--)fHeader[fNumHeader++]=headerInteger;
    }else{
      sscanf(cptr,"%d",(Int_t *)fHeader+fNumHeader);
      fNumHeader++;
    }
    
    // scan alng line looking for WhiteSpace
    while( !isWS(*cptr) ) { 
      cptr=fFileline+i;
      i++;
    }
    // WS found. What is it? Is it a new line? a null? 
    if( *cptr=='\0' || *cptr=='\n' ) break;
    // skip ahead to start of next bit of text
    while( isWS(*cptr) ) {
      cptr=fFileline+i;
      i++;
    }
    // if text is a comment marker, we are done this line
    if( *cptr=='#' && *(cptr+1)=='.' ) break; 
    if( *cptr=='#' && *(cptr+1)!='.' ) fatalError(fFileline); 
    
  }
  
}


Bool_t QTitles::nextBank() {
  // Find the next bank with the bank name fBank and number fBankNum.
  char* ptr;
  Int_t lBankNum;
  for( ; ; ) {
    if( !getNextNonCommentLine() ) {
      return kFALSE;
    }
    if( (ptr = strstr(fFileline,fBank)) ) {
      if( fFileline[0]=='*' && fFileline[1]=='D' && fFileline[2]=='O' ) {
        ptr+=strlen(fBank);
        sscanf(ptr,"%d ",&lBankNum);
        if( lBankNum==fBankNum ) return kTRUE;
      }
    }
  }
}

void QTitles::findBank(Int_t bankIndex) {
  // Goes to the bankIndex'th bank named fBank and numbered fBankNum
  rewind(fFin);
  char messageStr[1024];
  char messageStr1[1024];
  
  Int_t ibank;
  for (ibank=0; ibank<bankIndex+1; ibank++) {
    if (!nextBank()) {
      sprintf(messageStr,"End of File \n%s\n before bank %s found.\n",fFilename,fBank);
      sprintf(messageStr1,"\nStop at index %d -- wanted %d.\n",ibank+1,bankIndex);
      strcat(messageStr,messageStr1);
      fatalError(messageStr);
    }
  }
}

Int_t QTitles::countBanks() {
  
  // Counts the number of banks in fFilename named fBank and numbered fBankNum.
  Int_t nBankIndices = 0;
  rewind(fFin);
  while (nextBank()) nBankIndices++;
  return nBankIndices;
  
}

void QTitles::LoadBankInfo() {
  
  // Loops over all banks named fBank and numbered fBankNum.
  // Records the validity date and time from the headers.
  
  if(!fFilename) return;
  
  if (!openFile()) {
    char errorString[1024];
    sprintf(errorString,"QTitles::LoadBankInfo() could not find file %s\n",fFilename);
    fatalError(errorString);
  }
  
  fBankIndices = countBanks();
  fValidDate = new Int_t[fBankIndices];
  fValidTime = new Int_t[fBankIndices];
  
  rewind(fFin);
  
  Int_t ibank;
  for (ibank=0;ibank<fBankIndices;ibank++) {
    nextBank();
    readHeader();
    fValidDate[ibank] = GetHeaderWord(kQTitles_header_validDate);
    fValidTime[ibank] = GetHeaderWord(kQTitles_header_validTime);
    // cout << "validity " << ibank << " " 
    //      << fValidDate[ibank] << " " << fValidTime[ibank] << endl;
  }
  
  fclose(fFin);
  
}

Int_t QTitles::findValidBank(Int_t date, Int_t time) {
  
  // Returns bank index of desired bank, according to date and time.
  // Desired bank defined as follows:
  // If validity date and time match exactly, then take the bank.
  // Otherwise, take the latest bank that has a validity date/time
  // earlier than the requested one.
  
  Int_t ibank;
  Int_t validBank = -1;
  
  for (ibank=fBankIndices-1; ibank>=0; ibank--) {
    Int_t vd = GetValidDate(ibank);
    Int_t vt = GetValidTime(ibank);
    // cout << "for " << ibank << " " << vd << " " << vt 
    //      << " against " << date << " " << time << endl;
    if ( (date>vd) || ((date==vd)&&(time>=vt)) ) {
      validBank = ibank;
      break;
    }
  }
  
  if (validBank<0) {
    char errorString[1024];
    sprintf(errorString,"QTitles::findValidBank() could not find bank for %d %d\n",date,time);
    fatalError(errorString);
  }
  
  return validBank;
  
}

Bool_t QTitles::getNextNonCommentLine() {
  Bool_t isComment;
  char* ptr=NULL;
  for( ; ; ) {
    isComment = kFALSE;
    if( !fgets(fFileline,99,fFin) ) return kFALSE;
    ptr = elimWS();
    if( ptr[0]=='\n' ) isComment=kTRUE;
    if (!isComment) {
      if( strncmp(ptr,"*LOG",4)==0 ) isComment=kTRUE;
      if(!isComment && strncmp(ptr,"*---",4)==0 ) isComment=kTRUE;
      // there are some titles files which need the following line event though
      // according the thesnoman docs, it is unnecssary.
      if(!isComment && strncmp(ptr,"*--",3)==0 ) isComment=kTRUE; 
      if(!isComment && strncmp(ptr,"#.",2)==0 ) isComment=kTRUE; 
    }
    if( isComment==kFALSE ) break;
  } 
  return kTRUE;
}

void QTitles::fatalError(char* aMessage) {
  Error("fatalError",aMessage);
  //exit(-1);
}

char* QTitles::elimWS() {
  Int_t i = 0;
  while( fFileline[i] == '\t' || fFileline[i] == ' ' ) {
    i++;
  }
  return &(fFileline[i]);
  
}

void QTitles::increaseArray() {
  Int_t* lTmp = fDataI;
  
  //  new Float_t[fSize];
  //  if( lTmp==NULL ) fatalError("Could not increase titles array.\n");
  //for( i=0 ; i<fSize ; i++) lTmp[i] = fDataF[i];
  //delete[] fDataF;
  
  // Copy 
  fDataF = new Float_t[fSize+fStartSize];
  // Copying integers as floats will corrupt some bit patterns that are invalid as floats
  // --> much better (and faster) to do a memcpy anyway - PH 12/21/00
  //  for( i=0 ; i<fSize ; i++) fDataF[i] = lTmp[i];
  if (fDataF) {
    memcpy(fDataF, lTmp, fSize*sizeof(Float_t));
  }
  
  delete[] lTmp;
  fDataI=(Int_t *)fDataF;
  
  /*  Int_t* lTmp1 = new Int_t[fSize];
  if( lTmp1==NULL ) fatalError("Could not increase titles array.\n");
  for( i=0 ; i<fSize ; i++) lTmp1[i] = fDataI[i];
  delete[] fDataI;
  fDataI = new Int_t[fSize+fStartSize];
  for( i=0 ; i<fSize ; i++) fDataI[i] = lTmp1[i];
  delete[] lTmp1; 
  */
  
  fSize += fStartSize;
  
}


Int_t QTitles::GetHeaderWord(Int_t word) {
  return fHeader[word-1];  // snoman numbering
  
}

Int_t QTitles::GetIWord(Int_t word) {
  Int_t lData = fDataI[word-1];
  return lData;
  
}

Float_t QTitles::GetFWord(Int_t word) {
  Float_t lData = fDataF[word-1];
  return lData;
  
}

void QTitles::GetHWord(Int_t word, char* charString) {
  if( charString==NULL ) {
    fatalError("charString is NULL in QTitles::GetHWord()\n");
  }
  Int_t lw = fDataI[word-1];
  charString[0] = lw & 0xff;
  charString[1] = (lw & 0xff00) >> 8;
  charString[2] = (lw & 0xff0000) >> 16;
  charString[3] = (lw & 0xff000000) >> 24;
  charString[4] = '\0';
}

void QTitles::GetCWord(Int_t word, Int_t n1, Int_t nchar, Char_t* charString) {
  // grab a substring of length nchar+1 ('\0') starting at n1 from hollerith
  if( charString==NULL ) {
    fatalError("charString is NULL in QTitles::GetCWord()\n");
  }
  Int_t lw = fDataI[word-1];
  for (Int_t i=n1;i<n1+nchar;i++){
    charString[i-n1] = (lw & (255*(Int_t)pow(256,(Double_t)i))) >> (i*8);
  }
  charString[nchar]='\0';
  return;
}


Int_t QTitles::GetValidDate(Int_t ibank) {
  
  Int_t validDate = -1;
  if (fValidDate) validDate = fValidDate[ibank];
  
  return validDate;
}

Int_t QTitles::GetValidTime(Int_t ibank) {
  
  Int_t validTime = -1;
  if (fValidTime) validTime = fValidTime[ibank];
  
  return validTime;
}

QBank * QTitles::GetBank(){
  //Pack these data into a QBank.
  
  QBank *bank = new QBank(fBank, fBankNum );
  
  TArrayI *data = bank->GetData();
  data->Set(fNumData, fDataI);
  
  return bank;
}

