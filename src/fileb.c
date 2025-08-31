/* 
======================================================================
Module to describe basic file operations.

@author : Velorek
@version : 1.0
 
LAST MODIFIED : 14/04/2019 - Rename headers 
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "global.h"
#include "fileb.h"
/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*----------------------*/
/* Check if file exists */
/*----------------------*/

int file_exists(char *fileName) {
  int     ok=0;
  if(access(fileName, F_OK) != -1) {
    ok = 1;			//File exists
  } else {
    ok = 0;
  }
  return ok;
}

/*---------------*/
/* Get file size */
/*---------------*/

long getfileSize(FILE * filePtr) {
  long    sz=0;
  if(filePtr != NULL) {
    fseek(filePtr, 0L, SEEK_END);
    sz = ftell(filePtr);
    rewind(filePtr);
  }

  return sz;
}

/*---------------------*/
/* Count lines in File */
/*---------------------*/

long countLinesFile(FILE * filePtr) {
  char    ch=0;
  long    counterA = 0;
  if(filePtr != NULL) {
    rewind(filePtr);		//Make sure we are at the beginning

    ch = getc(filePtr);		//Peek ahead in the file
    while(!feof(filePtr)) {
      if(ch == END_LINE_CHAR) {
	counterA++;
      }
      ch = getc(filePtr);
    }
  }
  return counterA;
}

/*-----------------*/
/* Check file type */
/*-----------------*/


long checkFile(FILE * filePtr) {
  char    ch=0;
  long    counterA = 0;
  if(filePtr != NULL) {
    rewind(filePtr);		//Make sure we are at the beginning

    ch = getc(filePtr);		//Peek ahead in the file
    while(!feof(filePtr)) {
      if(ch < 9) {
	//discard accents
	if(ch > -60)
	  counterA++;
      }
      ch = getc(filePtr);
    }
  }
  return counterA;
}

//check whether the file is a text file
int openandcheckFile(char *fileName) {
  unsigned char    ch=0;
  long    counterA = 0;
  FILE *fp;

  openFile(&fp, fileName, "r");
  if(fp != NULL) {
    rewind(fp);		//Make sure we are at the beginning

    ch = getc(fp);		//Peek ahead in the file
    while(!feof(fp)) {
 
      if (ch == 0x00) {closeFile(fp); fp = NULL; return 1;}
      if(ch < 9) {
	//discard accents
	if(ch > 196)
	  counterA++;
      }
      ch = getc(fp);
    }
  }
  //If there are more strange characters than half the size of the file probably a binary file
  if (counterA > (getfileSize(fp)/2)) {
     if (fp != NULL) closeFile(fp); 
     fp = NULL;
  return 1;
  }
   if (fp != NULL) closeFile(fp);
    fp = NULL; 
  return 0;
}
/*-----------*/
/* Open file */
/*-----------*/

int openFile(FILE ** filePtr, char fileName[], char *mode)
/* 
Open file.
@return ok = 1 ? 0 Success! 
*/
{
  int     ok = 0;
  *filePtr = fopen(fileName, mode);

  if(*filePtr != NULL) {
    //File opened correctly.
    ok = 1;
  } else {
    //Error
    ok = 0;
  }
  return ok;
}

/*------------*/
/* Close file */
/*------------*/

int closeFile(FILE * filePtr) {
/* 
   Close file
@return ok: 
*/
  int     ok=0;

  if(filePtr != NULL) {
    ok = fclose(filePtr);
  }

  return ok;
}

