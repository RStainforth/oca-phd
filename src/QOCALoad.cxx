//*-- Author :    Olivier Simard   2003
/*************************************************************************
 * Copyright(c) 2003, The SNO Software Project, All rights reserved.     *
 * Authors:  Olivier Simard                                              *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/

#include "QOCALoad.h"
#include <stdlib.h>

ClassImp(QOCALoad);
//
// QOCALoad - Option Loading Methods for OCA
//
// Contains methods to load options from cardfiles.
// Used in many classes found in /qoca.
//
// The QOCALoad constructor can be initialized with full path file name
// or directory and file name separately, both indicating the file to
// load the options from. The input file must have a specific format:
// see QOCALoad::GetDimension() for more details.
//
// -----------------
// Example of calls:
//
// [0] QOCALoad* ql = new QOCALoad();
// [1] ql->SetFullname("/usr/local/options.txt");  // options.txt full path name
// [2] ql->SetKeyword();                           // look for standard ascii file
// [3] ql->GetDimension();
// [4] Char_t** array = ql->CreateArray();         // loaded array  
//
// -----
// [0] QOCALoad* ql = new QOCALoad();
// [1] ql->SetFullname("/usr/local/options.txt");  // options.txt full path name
// [2] ql->SetKeyword();                           // look for standard ascii file
// [3] Char_t** array = ql->CreateArray(ql->GetFullname()); 
//                                                 // shortcut to loaded array
// [4] Int_t dim = ql->GetDimensionFast();         // get stored dimension
// [5] Float_t* floats = ConverttoFloats(array,dim);
//                                                 // convert to floats
// Each call of CreateArray() or Convertto**() uses 'new' to create 
// the exact array size: delete them after use.
// None of the following function delete the converted entries:
// it is the user responsability to do so.
//
// [6] for(Int_t i = 0 ; i < dim ; i++) delete [] array[i];
// [7] delete [] array;
//
//
//
//
//______________________________________________________________________________
QOCALoad::QOCALoad(Char_t* directory, Char_t* filename)
{
  // QOCALoad constructor : initializes all possible inputs

  // initialize all variables
  Initialize();
  if(directory != NULL) {SetDirectory(directory);}
  if(filename != NULL) {SetFilename(filename);}
  if((directory != NULL)&&(filename != NULL)){SetFullname(directory, filename);}

}

//______________________________________________________________________________
QOCALoad::~QOCALoad()
{
  // QOCALoad destructor : deletes what was initialized

  Close();

}

//______________________________________________________________________________
void QOCALoad::Initialize()
{
  //
  // Function called for initialization
  //

  fDimension = (Int_t) 0;
  fNline = (Int_t) 0;
  fNcolumn = (Int_t) 0;
  fDirectory = (Char_t*) NULL;
  fFilename = (Char_t*) NULL;
  fFullname = (Char_t*) NULL;
  fKeyword = (Char_t*) NULL;
  fKeyworddim = (Int_t) 0;
  fListofkeywords = (Char_t**) NULL;
  fListofdimensions = (Int_t*) NULL;
  fDir_qruns = (Char_t*) NULL ;
  fDir_input = (Char_t*) NULL ;
  fDir_output = (Char_t*) NULL ;
  fStep = (Char_t*) NULL ;
  fScan = (Char_t*) NULL ;
  fMeth = (Char_t*) NULL ;
  fWlen = (Char_t*) NULL ;
  fM = (Char_t*) NULL ;
  fFilelist = (Char_t**) NULL;
  fRunlist = (Char_t*) NULL;
  fNrun = (Int_t) 0;
  fRuns = (Int_t*) NULL;

  return;
}

//______________________________________________________________________________
void QOCALoad::Close()
{
  //
  // Function called to clear stuff
  //

  if(fKeyworddim != 0){
    for(Int_t i = 0 ; i < fKeyworddim ; i++){
      delete [] fListofkeywords[i];
    }
    delete [] fListofkeywords;
    fListofkeywords = NULL;
  }
  fKeyworddim = 0;

  if(fListofdimensions) {delete [] fListofdimensions; fListofdimensions = NULL;}
  if(fFilelist) {delete [] fFilelist; fFilelist = NULL;}
}

//______________________________________________________________________________
void QOCALoad::VerifyInputs()
{
  //
  // Verifies all character inputs explicitly before main functions
  // like GetDimensions() or CreateArray() are called.
  // Displays warning messages or quits in your face to avoid
  // segmentation faults.
  //

  // if nothing at all is set just abort safely
  if((fFullname == NULL)&&(fDirectory == NULL)&&(fFilename == NULL)){
    Error("VerifyInputs","No Inputs. Aborting."); exit(-1);
  }
  else{
    if(fFullname == NULL){
      if((fDirectory != NULL) && (fFilename != NULL)){
        SetFullname(fDirectory, fFilename);
      }
      else{
        if(fDirectory != NULL){
	  printf("Warning in QOCALoad::VerifyInputs():\n");
	  printf("\twill try to reset the input file name.\n");
	  SetFullname(fDirectory);
	}
      }
    }
    else{
      SetFullname(fFullname);
    }
  }

  return;
}

//______________________________________________________________________________
void QOCALoad::SetFullname(Char_t* directory, Char_t* filename)
{
  // Sets the full path file name

  if((directory != NULL)&&(filename != NULL)){

    SetDirectory(directory); SetFilename(filename);
    fFullname = AppendElement(fDirectory, fFilename);
  }
  else {fFullname = NULL;}

  return;
}

//______________________________________________________________________________
void QOCALoad::SetFullname(Char_t* fullname)
{
  // Argument is already the full path file name

  if(fullname != NULL){
    fFullname = fullname;
  }
  else {fFullname = NULL;}

  return;
}

//______________________________________________________________________________
void QOCALoad::Presentator(Char_t* programname)
{
  // Displays a box with the name of the program beeing run

  Int_t j = 0 ; Int_t i, k; Int_t spaceline = 2;
  Int_t namelength = strlen(programname);

  printf("\n  ++++++++");
  for(i = 0 ; i < namelength ; i++){printf("+");} printf("++++++++\n");
  while(j != spaceline){printf("  ++      " );
    for(Int_t k = 0 ; k < namelength ; k++){printf(" ");} printf("      ++\n");
    j++;
  }
  printf("  ++      %s      ++\n",programname);
  j = 0 ;
  while(j != spaceline){ printf("  ++      ");
    for(k = 0 ; k < namelength ; k++){printf(" ");} printf("      ++\n");
    j++;
  }
  printf("  ++++++++");
  for(i = 0 ; i < namelength ; i++){printf("+");} printf("++++++++\n\n");

  return;
}


//______________________________________________________________________________
void QOCALoad::GetDimensions()
{
  //
  // Main dimension calculating function of this class.
  //
  // - Opens an input file (fFullname) if it exists; quits otherwise.
  // - Loops over all elements of each line and set the number
  //   of columns (fNcolumn = number of element per line)
  // - Loops over all lines and set the number of lines
  //   (fNline = number of lines)
  // - Discard comments on first column (#) or at the end
  //   of a line (#.)
  // - Search for sub-array if a keyword (fKeyword) is specified:
  //   it fills the sub-array without the keywords.
  //   All data can be placed in a single input files and sub-arrays
  //   selected with the keywords.
  //   Note: keywords need to be placed like comments (#) on first column
  //         or they will not be recognized.
  //
  // Additional Notes:
  // - Only reads input files with symmetric arrays.
  //   For a given keyword, the user must be consistent:
  //
  //     Examples: some data in a .txt file
  //
  //     #				#
  //     # sub-array			# same sub-array
  //     # with SetKeyword("yo");	# with no keyword
  //     #				#
  //     yo 1 2 3 4 5			1 2 3 4 5
  //     yo 6 7 8 9 10			6 7 8 9 10
  //     #				#
  //
  //     this function will set fNcolumn = 5 and fNline = 2,
  //     a GetDimension() call would set fDimension = 10
  //
  //     #
  //     # sub-array
  //     #
  //     yo 1 2
  //     yo 3 4
  //     yo 5 6
  //     yo 7 8
  //     yo 9 10
  //     #
  //
  //     this function will set fNcolumn = 2 and fNline = 5,
  //     a GetDimension() call would set fDimension = 10
  //
  //     #
  //     # sub-array
  //     #
  //     yo 1 2 3 4
  //     yo 5 6 7 8 9 10
  //     #
  //
  //     this function would try to set fNcolumn = 6 based on the second
  //     column and would cause the program to crash later if the user
  //     tries to fill any array.
  //
  //
  // - sets fNline : number of lines in the input file
  // - sets fNcolumn : number of columns in the same file
  // - this function DOES NOT set fDimension
  //   set fDimension with overloaded functions GetDimension() (no "s")
  //

  VerifyInputs(); // makes sure fFullname is set properly
  ResetNBoth(); // resets fNline and fNcolumn

  // scans the file elements to extract dimensions
  const Int_t maxlinelength = 1024;		// line length limit
  Char_t* buffer = new Char_t[maxlinelength];	// buffer for lines
  Char_t* tok; 					// buffer for words
  Int_t colindex = 0, kwindex = 0;		// indices for counting

  // verifies if the file can be opened
  ifstream file(fFullname, ios::in);
  if(!file){
    Error("GetDimensions","Could not open file: \n\t%s",fFullname);
    return;
  }

  while(!file.eof()){				// stops when eof() is reached
    file.getline(buffer, maxlinelength, '\n');	// put each line in the buffer
    
    tok = (Char_t*) strtok(buffer, " \t");	// move the token in the buffer
    
    // comment for a whole line enabled (#) on the very first column
    if(tok && tok[0] != '#'){
      
      // make sure to reset the number of column to avoid overcounting
      // because the if() loops over lines
      colindex = 0;
      
      // --- token moving methods ---
      // tok = strtok(NULL, " ")  ==> move to the next element
      // tok = NULL  ==> move to the next line
      // ---
      
      // loop over all character in a word until NULL character is found
      while(tok != NULL){
	
	// look if fKeyword has been set
	if(fKeyword != NULL){
	  
	  // skip non-keyword tokens at colindex=0
	  // (only create a sub-array when fKeyword starts the line)
	  // and also end of line comments
	  if(((strcmp(tok, fKeyword))&&(colindex == 0))||(!strcmp(tok,"#."))){
            tok = NULL; //skip
	  }
	  else{
	    if(!strcmp(tok, fKeyword)){    // a keyword has been found
	      AddaLine(); 		 // add a line to fNline
	      kwindex++;                 // flag for detected keyword
              tok = strtok(NULL, " \t"); // move token to next element
              colindex++;                // that means a new column
	    }
	    else{                          // the element is data
	      if(kwindex == 1){            // the keyword for that line has been detected
		AddaColumn();	         // add a column
		tok = strtok(NULL, " \t"); // move token to next element
		colindex++;                // that means a new column
	      }
	      else{                        // either means no keyword or next
		tok = strtok(NULL, " \t");
		colindex++;
	      }
	    }
	  }
	} // if(fKeyword)
	
	else{ //standard loading if no fKeyword found
	  if(colindex == 0){
	    AddaLine(); 		// add a line
	    colindex++;
	    kwindex++;
	  }
	  else{
	    if(!strcmp(tok,"#.")){tok = NULL;
	    }
	    else{
	      if(kwindex == 1){
		AddaColumn();		// add a column
		tok = strtok(NULL, " \t");
		colindex++;
	      }
	      else{
		tok = strtok(NULL, " \t");
		colindex++;
	      }
	    }
	  }
	} // else(fKeyword)
      
      } // while(tok!=NULL)
    } // if(tok[0]) or loop on columns
  } // eof() or loop on lines
  
  return;
}

//______________________________________________________________________________
Int_t QOCALoad::GetDimension()
{
  //
  // Three GetDimension() overloaded functions
  // to enable one argument (fullname) or two arguments (directory+filename)
  //
  // For a faster return call (without the GetDimensions() call)
  // use GetDimensionFast() which returns the last fDimension set
  // (avoid multiple file access).
  //
  // GetDimension() assumes fFullname has been set
  // GetDimension(fullname) sets fFullname for you
  // GetDimension(directory, filename) sets everything
  //

  // Converts the dimensions fNline and fNcolumn
  // into only one variable for array filling

  GetDimensions(); 		// fNline and fNcolumn are set
  fDimension = (fNline)*(fNcolumn);

  return fDimension;
}

//______________________________________________________________________________
Int_t QOCALoad::GetDimension(Char_t* fullname)
{
  // Same GetDimension() function with one argument

  SetFullname(fullname);	// sets the fFullname variable
  GetDimensions(); 		// fNline and fNcolumn are set
  fDimension = (fNline)*(fNcolumn);

  return fDimension;
}

//______________________________________________________________________________
Int_t QOCALoad::GetDimension(Char_t* directory, Char_t* filename)
{
  // Same GetDimension() function with two arguments

  SetFullname(directory, filename);	// sets the fFullname variable
  GetDimensions(); 		// fNline and fNcolumn are set
  fDimension = (fNline)*(fNcolumn);

  return fDimension;
}


//______________________________________________________________________________
Char_t** QOCALoad::CreateArray()
{
  //
  // Main array filling function of this class.
  //
  // Mimics GetDimensions() in some way to fill directly an
  // array from an input file.
  // All elements are loaded as Char_t* so it makes sense
  // to return an array of Char_t* which is a Char_t**.
  //
  // The main purpose of this QOCALoad class is to load data
  // without knowing in advance the dimension of the array so that
  // the user doesn't have to declare just enough or too much space
  // in a program.
  // The following explicit calls set it all automatically for you:
  // (some wrappers exists below which does it all in once)
  //
  //   // --------------------------------------
  //   #include <QOCALoad.h> // or .L QOCALoad.cxx in root
  //   QOCALoad* ql = new QOCALoad("myfile");
  //   ql->SetKeyword("mykeyword");
  //   ql->GetDimensions(); // loops over the file elements and set enough space
  //   Char_t** my_array = ql->CreateArray(); // put the elements in a local array
  //   ql->DisplayMatrixContent(my_array); // display
  //   // --------------------------------------
  //
  // Functions also exist to convert this data into integers, floats and doubles
  // to be usable in other programs.
  //
  // This function cannot be called alone if any of the following members
  // have not been set properly:
  //  - fFullname
  //  - fDimension
  //  - fKeyword
  //
  // NOTE: Deletion of an array created with this function
  //       (even if character strings do not take much space)
  //       must be done in the following way:
  //  - Save the dimension of the array locally:
  //      Int_t dim = GetDimension();
  //  - loop over all element and delete them individually:
  //      for(i<dim){delete [] chararray[i];}
  //  - outside this loop delete the pointer to the array:
  //      delete [] chararray;
  //
  //

  VerifyInputs(); // makes sure fFullname is set properly

  Int_t maxlinelength = 1024;			// line length limit
  Char_t* buffer = new Char_t[maxlinelength];  	// buffer for lines
  Char_t* tok, *buf; 				// buffers for words
  Int_t index = 0, colindex = 0;		// counters
  Char_t** chararray = new Char_t*[fDimension];	// array to be returned
  Int_t count = 0;

  // verifies if the file can be opened
  ifstream file(fFullname, ios::in);
  if(!file){
    Error("CreateArray","Could not open file: \n\t%s",fFullname);
    return NULL;
  }

  if(!fDimension){
    Error("CreateArray","No dimension set! Impossible to create an array from input file!");
    return NULL;
  }

  while(!file.eof()){				// stops when eof() is reached
    ++count;
    file.getline(buffer, maxlinelength, '\n');	// put each line in the buffer
    tok = (Char_t*) strtok(buffer, " \t");	// move the token in the buffer
    
    // comment for a whole line enabled (#) on the very first column
    if(tok && tok[0] != '#'){
      colindex = 0;
      while(tok != NULL){
        if(fKeyword != NULL){
	  
          //skip non-keyword tokens at colindex=0 and also end of line comments
          if(((strcmp(tok, fKeyword))&&(colindex == 0))||(!strcmp(tok,"#."))){
	    tok = NULL; // skip
	  }
          else{
	    if(!strcmp(tok, fKeyword)){
              tok = strtok(NULL, " \t");
	      colindex++;
            }
            else{
              if (index >= fDimension) {
	      	Warning("CreateArray","Keyword mode : too many entries at line %d of file %s.",
			count,fFullname);
	      	return NULL;
	      }
              buf = new Char_t[strlen(tok)+1];
	      strcpy(buf, tok);
	      chararray[index] = buf;
              index++; tok = strtok(NULL, " \t");
            }
	  }
	} // if(fKeyword)

	else{
	  if(!strcmp(tok,"#.")){
	    tok = NULL;
	  }
	  else{
            if (index >= fDimension) {
	      Warning("CreateArray","Non-keyword mode : too many entries at line %d of file %s.",
		      count,fFullname);
	      return NULL;
	    }
	    buf = new Char_t[strlen(tok)+1];
	    strcpy(buf, tok);
	    chararray[index] = buf;
	    index++; tok = strtok(NULL, " \t");
	  }
	}
      }
    }
  }
  
  delete [] buffer;
  return chararray;
}

//______________________________________________________________________________
Char_t** QOCALoad::CreateArray(Char_t* fullname)
{
  //
  // Three CreateArray() overloaded functions
  // to enable one argument (fullname) or two arguments (directory+filename)
  //
  // Calls of GetDimension() are made. Easy to follow.
  //
  // 11.2006 - O.Simard
  // Modified to return a NULL pointer explicitely when the dimension is zero.

  Int_t dimension = GetDimension(fullname); // sets fDimension
  if(dimension<=0){
    Warning("CreateArray","Dimension of this array is %d: returned a NULL pointer.",dimension);
    Warning("CreateArray","Check keyword %s in file \n\t%s.",fKeyword,fullname);
    printf("QOCALoad will force the whole thing to quit ...\n");
    exit(-1); // avoid seg. fault
  }
  Char_t** charray = CreateArray();
  if(!dimension){
    printf("Warning in QOCALoad::CreateArray():\n");
    printf("\tMay have returned an empty array.\n");
    printf("Check keyword %s in file \n\t%s.\n",fKeyword,fullname);
  }
  return charray;
}

//______________________________________________________________________________
Char_t** QOCALoad::CreateArray(Char_t* directory, Char_t* filename)
{
  Int_t dimension = GetDimension(directory, filename); // sets fDimension
  if(dimension<=0){
    Warning("CreateArray","Dimension of this array is %d: returned a NULL pointer.",dimension);
    Warning("CreateArray","Check keyword %s in file \n\t%s.",fKeyword,filename);
    printf("QOCALoad will force the whole thing to quit ...\n");
    exit(-1); // avoid seg. fault
  }
  Char_t** charray = CreateArray();
  if(!dimension){
    printf("Warning in QOCALoad::CreateArray():\n");
    printf("\tMay have returned an empty array.\n");
    printf("Check keyword %s in file %s.\n",fKeyword,filename);
  }

  return charray;
}


//______________________________________________________________________________
Char_t*** QOCALoad::CreateBigArray()
{
  // Attempt to create an array of arrays : assumes fDirectory is set
  // Uses CreateArray() assuming fFullname and fDimension are already set

  Int_t dimension = GetDimension();		//get the dimension
  Char_t** smallarray = CreateArray();			//create the array
  Char_t*** bigarray = new Char_t**[dimension]; 	//declare the bigarray

  for(Int_t k = 0 ; k < dimension ; k++){		//loop over smallarray entries
     SetFullname(fDirectory, smallarray[k]);		//sets the new fullname
     bigarray[k] = CreateArray(fFullname);		//create an array with each one
     delete [] smallarray[k];				//delete each entry of the array
  }

  delete [] smallarray;					//delete the array
  return bigarray;

}

//______________________________________________________________________________
Char_t*** QOCALoad::CreateBigArray(Char_t* fullname)
{
  // Attempt to create an array of arrays from one file fullname argument
  // Uses CreateArray() assuming fFullname and fDimension are already set

  Int_t dimension = GetDimension(fullname);	//get the dimension
  Char_t** smallarray = CreateArray();			//create the array
  Char_t*** bigarray = new Char_t**[dimension];

  for(Int_t k = 0 ; k < dimension ; k++){
     bigarray[k] = CreateArray(fDirectory, smallarray[k]);
     delete [] smallarray[k];
  }

  delete [] smallarray;
  return bigarray;

}

//______________________________________________________________________________
Char_t*** QOCALoad::CreateBigArray(Char_t* directory, Char_t* filename)
{
  // Attempt to create an array of arrays from two arguments
  // containing a list of names of other card files

  SetFullname(directory, filename);
  Char_t*** bigarray = CreateBigArray(fFullname);

  return bigarray;

}


//______________________________________________________________________________
Int_t* QOCALoad::ConverttoIntegers(Char_t** chararray, Int_t dimension)
{
  //
  // Convert integers loaded as Char_t* into an Int_t array
  //

  if(!dimension){
    printf("In QOCALoad::ConverttoIntegers:\n");
    printf("\tUsing last dimension calculated: %d\n",fDimension);
    dimension = fDimension;
    if(!dimension){
      Error("ConverttoIntegers","Empty dimension: Nothing to convert."); exit(-1);
    }
  }

  Int_t* intarray = new Int_t[dimension]; 	//the array dim is pre-set

  // if converting Char_t*, a bunch of zeros are returned
  for(Int_t index = 0 ; index < dimension ; index++){
    intarray[index] = atoi(chararray[index]);	//char to int C++ conversion function
  }

  return intarray;

}

//______________________________________________________________________________
Float_t* QOCALoad::ConverttoFloats(Char_t** chararray, Int_t dimension)
{
  //
  // Convert floats loaded as Char_t* into a Float_t array
  //

  if(!dimension){
    printf("In QOCALoad::ConverttoFloats:\n");
    printf("\tUsing last dimension calculated: %d\n",fDimension);
    dimension = fDimension;
    if(!dimension){
      Error("ConverttoFloats","Empty dimension: Nothing to convert"); exit(-1);
    }
  }

  Float_t* floatarray = new Float_t[dimension]; //the array dim is pre-set

  // if converting Char_t*, a bunch of zeros are returned
  for(Int_t index = 0 ; index < dimension ; index++){
    floatarray[index] = atof(chararray[index]);	//char to float C++ conversion function
  }

  return floatarray;

}

//______________________________________________________________________________
Double_t* QOCALoad::ConverttoDoubles(Char_t** chararray, Int_t dimension)
{
  //
  // Convert doubles loaded as Char_t* into a Double_t array
  //

  if(!dimension){
    printf("In QOCALoad::ConverttoDoubles:\n");
    printf("\tUsing last dimension calculated: %d\n",fDimension);
    dimension = fDimension;
    if(!dimension){
      Error("ConverttoDoubles","Empty dimension: Nothing to convert"); exit(-1);
    }
  }

  Double_t* doublearray = new Double_t[dimension]; //the array dim is pre-set

  // if converting Char_t*, a bunch of zeros are returned
  for(Int_t index = 0 ; index < dimension ; index++){
    doublearray[index] = atof(chararray[index]);	//char to double C++ conversion function
  }

  return doublearray;

}

//______________________________________________________________________________
void QOCALoad::DisplayVectorContent(Char_t** array)
{
  //
  // Displays all elements of a created array (vector style)
  // Must set fDimension before use or it takes the last one found
  //

  for(Int_t i=0; i<fDimension; i++){
    printf("%s\n",array[i]);
  }

  return;
}


//______________________________________________________________________________
void QOCALoad::DisplayMatrixContent(Char_t** array)
{
  //
  // Displays all elements of a created array (matrix style)
  // It may not work if the number of lines and columns
  // are not properly set before use.
  // Call it right after an array creation to see the elements
  // as they appear in the input file.
  //

  for(Int_t i=0; i<fNline; i++){ Int_t j = i*fNcolumn;
    for(Int_t k=0; k<fNcolumn; k++){
      printf("%s ",array[j+k]);
    }
    printf("\n");
  }

  return;
}


//______________________________________________________________________________
Char_t** QOCALoad::GetLine(Char_t** array, Int_t line)
{
  //
  // Get a specified line from a matrix array
  // after a GetDimensions() call which sets fNcolumn and fNline.
  //
  // Returns the line as a vector.
  //
  // The first line has index 1 not 0
  //

  if(line<=0 && line>fNline){
    Error("GetLine(Char_t**)","Invalid line index: %d (max: %d).",line,fNline);
    exit(-1);
  }

  Char_t** vector = new Char_t*[fNcolumn];

  for(Int_t i=0; i<fNcolumn; i++){
    Int_t j = i + (fNcolumn*(line-1)); // position offset
    vector[i] = array[j];
  }

  return vector;

}

//______________________________________________________________________________
Int_t* QOCALoad::GetLine(Int_t* array, Int_t line)
{
  //
  // Same as GetLine() but for integers
  //
  // there should be a way to template this function
  //

  if(line<=0 && line>fNline){
    Error("GetLine(Int_t*)","Invalid line index: %d (max: %d).",line,fNline);
    exit(-1);
  }

  Int_t* vector = new Int_t[fNcolumn];

  for(Int_t i=0; i<fNcolumn; i++){
    Int_t j = i + (fNcolumn*(line-1)); // position offset
    vector[i] = array[j];
  }

  return vector;

}

//______________________________________________________________________________
Float_t* QOCALoad::GetLine(Float_t* array, Int_t line)
{
  //
  // Same as GetLine() but for floats or doubles
  //
  // there should be a way to template this function
  //

  if(line<=0 && line>fNline){
    Error("GetLine(Float_t*)","Invalid line index: %d (max: %d)",line,fNline);
    exit(-1);
  }

  Float_t* vector = new Float_t[fNcolumn];

  for(Int_t i=0; i<fNcolumn; i++){
    Int_t j = i + (fNcolumn*(line-1)); // position offset
    vector[i] = array[j];
  }

  return vector;

}

//______________________________________________________________________________
Char_t** QOCALoad::GetColumn(Char_t** array, Int_t colnumber)
{
  //
  // Get a specified column from a matrix array
  // after a GetDimensions() call which sets fNcolumn and fNline.
  //
  // Returns the selected vector.
  //
  // The first column has index 1 not 0
  //

  if((colnumber<=0)||(colnumber>fNcolumn)){
    Error("GetColumn(Char_t**)","Invalid column index: %d (max: %d).",colnumber,fNcolumn);
    exit(-1);
  }

  Char_t** vector = new Char_t*[fNline];

  for(Int_t i=0; i<fNline; i++){ Int_t j = i*fNcolumn; // position offset
    vector[i] = array[j+colnumber-1];
  }

  return vector;

}

//______________________________________________________________________________
Int_t* QOCALoad::GetColumn(Int_t* array, Int_t colnumber)
{
  //
  // Same as GetColumn() but for integers
  //
  // there should be a way to template this function
  //

  if((colnumber<=0)||(colnumber>fNcolumn)){
    Error("GetColumn(Int_t*)","Invalid column index: %d (max: %d).",colnumber,fNcolumn);
    exit(-1);
  }

  Int_t* vector = new Int_t[fNline];

  for(Int_t i=0; i<fNline; i++){ Int_t j = i*fNcolumn; // position offset
    vector[i] = array[j+colnumber-1];
  }

  return vector;
}

//______________________________________________________________________________
Float_t* QOCALoad::GetColumn(Float_t* array, Int_t colnumber)
{
  //
  // Same as GetColumn() but for floats or doubles
  //
  // there should be a way to template this function
  //

  if((colnumber<=0)||(colnumber>fNcolumn)){
    Error("GetColumn(Float_t*)","Invalid column index: %d (max: %d).",colnumber,fNcolumn);
    exit(-1);
  }

  Float_t* vector = new Float_t[fNline];

  for(Int_t i=0; i<fNline; i++){ Int_t j = i*fNcolumn; // position offset
    vector[i] = array[j+colnumber-1];
  }

  return vector;
}

//______________________________________________________________________________
Bool_t QOCALoad::SearchforKeyword(Char_t* keyword)
{
  //
  // Search for a keyword in an input file.
  //

  SetKeyword(keyword); // set the trial keyword
  GetDimensions(); // loop in the file (has to be set)
  // fNline automatically tells if keyword has been found at least once
  Bool_t check = kFALSE; // default is not found

  if(fNline==0){
    printf("Warning in QOCALoad::SearchforKeyword():\n");
    printf("\tKeyword %s not found!\n",keyword);
  }
  else{check = kTRUE;}

  return check;
}


//______________________________________________________________________________
void QOCALoad::CreateListofKeywords()
{
  //
  // Create a list of keywords from a card file.
  //
  // Method:
  // - A master keyword "kw" identifies an array of keywords
  //   so the user can specify a header of keywords in the input file.
  // - This array is stored.
  // - It can be reused with other functions to load multiple
  //   arrays at once, looping over the keywords.
  //


  SetKeyword("kw"); 		// keyword for keywords
  Int_t dimension = GetDimension();	// saves fDimension
  Char_t** keywordlist = new Char_t*[dimension];
  keywordlist = CreateArray();

  fKeyworddim = dimension;		// saves the keyword list dimension
  fListofkeywords = keywordlist;	// saves the keyword list

  return;
}

//______________________________________________________________________________
Bool_t QOCALoad::CheckKeywords()
{
  //
  // Check all the keywords from a keywordlist
  //

  CreateListofKeywords();	//sets fKeyworddim
  Bool_t check = kTRUE; 	//default value
  for(Int_t i = 0 ; i < fKeyworddim ; i++){
    //if only one of the keyword is not found the function return kFALSE
    if(!(SearchforKeyword(fListofkeywords[i]))){
    check = kFALSE;
    }
  }

  return check;
}


//______________________________________________________________________________
void QOCALoad::SetArrayofDimensions()
{
  //
  // Fill an array with dimensions for multiple keyword usage
  //

  SetDimension(0); 					// reset fDimension
  CreateListofKeywords();				// sets fKeyworddim
  Int_t* dimarray = new Int_t[fKeyworddim];

  for(Int_t i = 0 ; i < fKeyworddim ; i++)
  {
    if(SearchforKeyword(fListofkeywords[i])){	// verifies the keyword
      dimarray[i] = GetDimension();		// record fDimension for this keyword
    }
    else{
      dimarray[i] = 0;		// indicate a problem
    }
  }

  fListofdimensions = dimarray;		// saves those values

  return;
}

//______________________________________________________________________________
Char_t*** QOCALoad::LoadOptions(Bool_t display)
{
  //
  // Attempt to read the card file and to put all the data in only
  // one bigarray
  //

  CreateListofKeywords();				// sets fKeyworddim
  SetArrayofDimensions(); 				// saves all dimensions
  Int_t t = 1; // multiplicative iterator ; initialize to 1

  // to calculate the total dimension of the cardfile
  for(Int_t i = 0 ; i < fKeyworddim ; i++){
    t *= fListofdimensions[i]; // iterative multiplication
  }

  if(t<=1){
    Error("LoadOptions","Invalid dimension!");
    return NULL;
  }

  Char_t*** all = new Char_t**[t];

  if(display){
    printf("* ================================ *\n");
    printf("*  Card File Content Summary for: \n");
    printf("*  %s\n",fFilename);
    printf("* ================================ *\n");
  }

  for(Int_t i = 0 ; i < fKeyworddim ; i++){
    SetDimension(fListofdimensions[i]); // set this sub-array dimension
    SetKeyword(fListofkeywords[i]); // set its keyword back
    all[i] = CreateArray(); // create this sub-array and save it
    if(display){
      printf("* ----------------------------- *\n");
      DisplayVectorContent((Char_t**) all[i]);
    }
  }

  if(display){
    printf("* ================================ *\n");
    printf("*  End of card file.\n");
    printf("* ================================ *\n");
  }

  return all;
}

//______________________________________________________________________________
Char_t** QOCALoad::LoadFileList()
{
  //
  // QPath/QOCAFit specific file list creator
  // according to which analysis processing step is running.
  //
  // Load the file list which consists of only 3 files
  // arranged in a Char_t* array:
  //   [0] : the runlist
  //   [1] : the qpath_pos file if needed/available
  //   [2] : the output file
  //
  // Steps of the optics processing:
  //   ("pos"): create qpath_pos files from .rdt files
  //   ("tree"): create qpath_tree files from qpath_pos file
  //   ("fit"): do the fit for qpath_tree files
  //
  // The method identifies the way the position fits
  // are used in step "tree".
  //

  // the flags are 0 if not valid, 1 if valid
  Bool_t kP = (!strcmp(fStep,"pos")) || (!strcmp(fStep,"POS"));
  Bool_t kT = (!strcmp(fStep,"tree")) || (!strcmp(fStep,"TREE"));
  Bool_t kF = (!strcmp(fStep,"fit")) || (!strcmp(fStep,"FIT"));

  if(!kP && !kT && !kF){
    Error("LoadFileList","Step %s unknown.",fStep);
    exit(-1);
  }
  else{
    printf("====================================\n");
    printf("From QOCALoad::LoadFileList(Char_t*) :\n");
    printf("\t Step %s of the optics processing.\n",fStep);
  }

  if(kP){
    printf("\t Creating qpath_pos (positions) files from .rdt files.\n");
    return FilesforPositions();
  }

  if(kT){
    printf("\t Creating qpath_tree (trees) files from position fits files.\n");
    return FilesforTrees();
  }

  if(kF){
    printf("\t Creating qocafit files (fits) from qpath_tree files.\n");
    return FilesforFits();
  }

  return NULL;
}

//______________________________________________________________________________
Char_t** QOCALoad::FilesforPositions()
{
  // This step is: create qpath_pos files from .rdt files
  //
  // requires:
  //   - runlist (.txt)
  //   - no input files
  //   - ouptut file (.root)
  //

  Char_t** file = new Char_t*[3];

  for(Int_t i = 0 ; i < 3 ; i++){

    file[i] = new Char_t[1024];
    // for qrunlist 
    if(i == 0){
      if(!fM) sprintf(file[i],"%sqruns_%s_%s.txt",fDir_qruns,fScan,fWlen);
      else sprintf(file[i],"%sqruns_%s%s_%s.txt",fDir_qruns,fScan,fM,fWlen);
    }

    // for input files
    if(i == 1){
      // to be safe set it to the standard scan (no fM)
      sprintf(file[i],"%sqpath_%s_pos_%s.root",fDir_input,fScan,fWlen);
    }

    // for output files
    if(i == 2){
      if(!fM) sprintf(file[i],"%sqpath_%s_pos_%s.root",fDir_output,fScan,fWlen);
      else sprintf(file[i],"%sqpath_%s%s_pos_%s.root",fDir_output,fScan,fM,fWlen);
    }
  }

  return file;
}
//______________________________________________________________________________
Char_t** QOCALoad::FilesforTrees()
{
  // This step is: create qpath_tree files from qpath_pos files
  //
  // requires:
  //   - runlist (.txt)
  //   - input qpath position file (.root)
  //   - ouptut file (.root)
  //

  Char_t** file = new Char_t*[3];

  Char_t* wlen = "500"; // default
  if(!(strcmp(fMeth,"fits"))){wlen = fWlen;} // reset if "fits"
  else{ // if non-valid method, default is "fits"
    if(strcmp(fMeth,"fruns") && strcmp(fMeth,"eff")){
      fMeth = "fits";
      wlen = fWlen;
      printf("Warning in QOCALoad::FilesforTrees():\n");
      printf("\tInvalid position fits input method.\n");
      printf("\tSetting it to default: %s.\n",fMeth);
    }
  }

  for(Int_t i = 0 ; i < 3 ; i++){
   
    file[i] = new Char_t[1024];
    // for qrunlist
    if(i == 0){
      if(!fM) sprintf(file[i],"%sqruns_%s_%s.txt",fDir_qruns,fScan,fWlen);
      else sprintf(file[i],"%sqruns_%s%s_%s.txt",fDir_qruns,fScan,fM,fWlen);
    }

    // for input files
    if(i == 1){
      if(!fM) sprintf(file[i],"%sqpath_%s_pos_%s.root",fDir_input,fScan,wlen);
      else sprintf(file[i],"%sqpath_%s%s_pos_%s.root",fDir_input,fScan,fM,wlen);
    }

    // for output files
    if(i == 2){
      if(!fM) sprintf(file[i],"%sqpath_%s_%s_%s.root",fDir_output,fScan,fMeth,fWlen);
      else sprintf(file[i],"%sqpath_%s%s_%s_%s.root",fDir_output,fScan,fM,fMeth,fWlen);
    }
  }

  return file;
}
//______________________________________________________________________________
Char_t** QOCALoad::FilesforFits()
{
  // This step is: do the qocafit for qpath_tree files
  //
  // requires:
  //   - runlist (.txt)
  //   - input qpath_tree files (.root)
  //   - ouptut file (.root)
  //

  if(!strcmp(fDir_input, fDir_output)){
    printf("From QOCALoad::FilesforStepFits():\n");
    printf("\tSame directories for qpath and qocafit files!\n");
  }

  if(strcmp(fMeth,"fits") && strcmp(fMeth,"fruns") && strcmp(fMeth,"eff")){
    fMeth = "fits";
    printf("Warning in QOCALoad::FilesforFits():\n");
    printf("\tInvalid position fits input method.\n");
    printf("\tSetting it to default: %s.\n",fMeth);
  }


  Char_t** file = new Char_t*[3];

  for(Int_t i = 0 ; i < 3 ; i++){

    file[i] = new Char_t[1024];
    // for qrunlist
    if(i == 0){
      if(!fM) sprintf(file[i],"%sqruns_%s_%s.txt",fDir_qruns,fScan,fWlen);
      else sprintf(file[i],"%sqruns_%s%s_%s.txt",fDir_qruns,fScan,fM,fWlen);
    }

    // for input files
    if(i == 1){
      if(!fM) sprintf(file[i],"%sqpath_%s_%s_%s.root",fDir_input,fScan,fMeth,fWlen);
      else sprintf(file[i],"%sqpath_%s%s_%s_%s.root",fDir_input,fScan,fM,fMeth,fWlen);
    }

    // for output files
    if(i == 2){
      if(!fM) sprintf(file[i],"%sqocafit_%s_%s_%s.root",fDir_output,fScan,fMeth,fWlen);
      else sprintf(file[i],"%sqocafit_%s%s_%s_%s.root",fDir_output,fScan,fM,fMeth,fWlen);
    }
  }

  return file;
}

//______________________________________________________________________________
void QOCALoad::LoadRunList(Char_t* runlist)
{
  //
  // QPath/QOCAFit specific run list creator.
  //
  // Sets the run list full path file name
  // and the dimension which comes with it
  //

  if(runlist == NULL){
    printf("Warning in QOCALoad::LoadRunList():\n");
    printf("\tInvalid runlist name.\n");
    SetNrun();
    return;
  }

  fRunlist = runlist;

  // no keywords in run lists
  SetKeyword();
  // set the full path run list file name
  SetFullname(fRunlist);

  // get the dimension of the complete file
  GetDimension(); // sets fDimension

  // set the fNrun
  SetNrun((Int_t)GetNLines());

  // set the runs array (4 columns)
  SetRunsArray(ConverttoIntegers(CreateArray(), fDimension));

  return;
}

//______________________________________________________________________________
Int_t* QOCALoad::GetFitFlags()
{
  //
  // Returns the vector of integers containing flags
  // to pass to QOCAFit indicating if a run is used in the fit.
  //
  // LoadRunList() must be called prior to this function
  //

  if(!fRunlist){
    printf("Warning in QOCALoad::GetFitFlags():\n");
    printf("\tUnable to find the run list.\n");
  }

  // dimension is known, initialize the vector
  Int_t* flags = new Int_t[fNrun];
  // access the qrunlist
  flags = GetColumn(fRuns, 1);

  return flags;
}

//______________________________________________________________________________
Int_t* QOCALoad::GetRuns()
{
  //
  // Returns the vector of integers containing the run numbers
  // to pass to QPath and QOCAFit
  //
  // LoadRunList() must be called prior to this function
  //

  if(!fRunlist){
    printf("Warning in QOCALoad::GetRuns():\n");
    printf("\tUnable to find the run list.\n");
  }

  // dimension is known, initialize the vector
  Int_t* runs = new Int_t[(Int_t)fNrun];

  // access the qrunlist
  runs = GetColumn(fRuns, 2);

  return runs;

}

//______________________________________________________________________________
Int_t* QOCALoad::GetCruns()
{
  //
  // Returns the vector of integers containing the central run numbers
  // to pass to QPath
  //
  // LoadRunList() must be called prior to this function
  //

  if(!fRunlist){
    printf("Warning in QOCALoad::GetCruns():\n");
    printf("\tUnable to find the run list.\n");
  }

  // dimension is known, initialize the vector
  Int_t* cruns = new Int_t[(Int_t)fNrun];

  // access the qrunlist
  cruns = GetColumn(fRuns, 3);

  return cruns;
}

//______________________________________________________________________________
Int_t* QOCALoad::GetFruns()
{
  //
  // Returns the vector of integers containing the 500 nm run numbers
  // to pass to QPath
  //
  // SetRunList() must be called prior to this function
  //

  if(!fRunlist){
    printf("Warning in QOCALoad::GetFruns():\n");
    printf("\tUnable to find the run list.\n");
  }

  // dimension is known, initialize the vector
  Int_t* fruns = new Int_t[(Int_t)fNrun];

  // access the qrunlist
  fruns = GetColumn(fRuns, 4);

  return fruns;

}

//______________________________________________________________________________
void QOCALoad::ProduceRunLists(Char_t* scan,Int_t* cruns)
{
  //
  // QPath/QOCAFit specific run list generator.
  //
  // The input file name is determined entirely by the scan identifier
  // provided. The output lists are created in the same directory. If no
  // directory, the files are written in the current directory.
  // The function is looking for a six-column file readable
  // by QOCALoad as the following example:
  //
  // # 337	365	386	420	500	620
  // 11111	22222	33333	44444	55555	66666
  // 77777	0	0	88888	99999	0
  // # etc.
  // # ---------------
  // Notes:
  //  - run numbers are loaded with the default flag 1 (use it).
  //  - 0's are skipped and indicate there was no run there.
  //  - the comments in the runlists are not provided
  //	 it is the user's responsability togo back in the file and edit them.
  //  - the cruns array is the array with main central runs used in the
  //    occupancy ratio method.
  //

  if(fDirectory == NULL){SetDirectory((Char_t*)getenv("PWD"));}
  if(scan == NULL){
    Error("ProduceRunLists","No scan provided. Aborting."); exit(-1);
  }
  if(cruns == NULL){
    Error("ProduceRunLists","No main central runs provided. Aborting."); exit(-1);
  }

  Char_t input[1024],output[1024];

  sprintf(input,"%s/scan_%s.txt",fDirectory,scan);
  SetFullname(input);
  SetKeyword();

  // wavelengths
  Int_t wlen[6] = {337,365,386,420,500,620};

  Char_t** array = CreateArray(GetFullname());
  // from here:
  // - fNcolumn = number of wavelengths
  // - fNline = number of positions (or run per wavelength)
  if(fNcolumn != 6){
    Warning("ProduceRunLists","Bad number of columns in input file.");
  }
  Int_t* runs  = ConverttoIntegers(array); // all the run numbers converted
  Int_t* fruns = GetColumn(runs, 5); // 500 nm runs

  for(Int_t icol = 0 ; icol < fNcolumn ; icol++){

    printf("At wavelength %d / %d (%d nm)\n",icol+1,fNcolumn,wlen[icol]);
    Int_t* wruns = GetColumn(runs, icol+1); // each column (wlen)
    sprintf(output, "%s/qruns_%s_%d.txt",fDirectory,scan,wlen[icol]);
    FILE* list = fopen(output,"w");
    for(Int_t ipos = 0 ; ipos < fNline ; ipos++){
      if(wruns[ipos] == 0){
	  Warning("ProduceRunLists","Skipping index %d",ipos);
	  continue;
      }
      fprintf(list, "1 %d %d %d \n",wruns[ipos],cruns[icol],fruns[ipos]);
    }
    fclose(list);
    delete wruns;
  }

  return;
}

//______________________________________________________________________________
Char_t* QOCALoad::InsertElement(Char_t* originalstring, Char_t* identifier, const Char_t* word)
{
  // Attempt to replace an identifier by a word

  // first verify that the identifier is contained in the original string
  // if it is not, return the original string
  if(!(strstr(originalstring, identifier))){return originalstring;}
  else
  {

  // method to insert the word
  Char_t* rlname = new Char_t[strlen(originalstring)+strlen(word)+1];
  Char_t* cpy = new Char_t[strlen(originalstring)+1];
  strcpy(cpy, originalstring); 			//copy rlname
  strcpy(rlname, strtok(cpy, identifier));	//get to the identifier
  strcat(rlname, word); 			//insert the desired word
  strcat(rlname, strtok(NULL, identifier)); 	//replace the remaining a the end

  delete [] cpy;  				//delete the modified copy
  return rlname;  				//return the allocated string copy

  }
}

//______________________________________________________________________________
Char_t* QOCALoad::AppendElement(const Char_t* originalstring, Char_t* identifier, const Char_t* word)
{
  // Attempt to replace an identifier located at the end of a string by a word

  Char_t* rlname = new Char_t[strlen(originalstring)+strlen(word)+1];
  strcpy(rlname, originalstring);				//copy name
  strtok(rlname, identifier);			//get to the identifier
  strcat(rlname, word);				//replace identifier with word

  return rlname;  				//return the allocated string copy

}

//______________________________________________________________________________
Char_t* QOCALoad::AppendElement(const Char_t* originalstring, const Char_t* word)
{
  // Attempt to append a word at the end of a string

  Char_t* rlname = new Char_t[strlen(originalstring)+strlen(word)+1];
  strcpy(rlname, originalstring);
  strcat(rlname, word);


  return rlname;				// return the allocated string copy

}


// functions to insert an array of words using InsertElement
//______________________________________________________________________________
Char_t** QOCALoad::InsertElements(Char_t** array1, Char_t** array2, Int_t dim1, Int_t dim2, Char_t* identifier)
{
  // Attempt to insert more than one words in more than one string
  // Only one identifier is specified for each call

  const Int_t dim = (dim1)*(dim2);
  Char_t** modifiedarray = new Char_t*[dim];		//allocate enough space
  for(Int_t i = 0 ; i < dim1 ; i++){			//loop over the first array
	Int_t l = i*dim2;
	for(Int_t k = 0 ; k < dim2 ; k++){		//loop over the second array
	  //automatic insertion
	  modifiedarray[k+l] = InsertElement(array1[i], identifier, array2[k]);
	}
  //delete [] array1[i]; 		// DO NOT delete the old array elements
   }

  return modifiedarray;

}



