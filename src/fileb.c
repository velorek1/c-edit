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
#include "uintf.h"
#include "fileb.h"
/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*----------------------*/
/* Check if file exists */
/*----------------------*/

int file_exists(char *fileName) {
  int     ok;
  if(access(fileName, F_OK) != -1) {
    ok = 1;         //File exists
  } else {
    ok = 0;
  }
  return ok;
}

/*---------------*/
/* Get file size */
/*---------------*/

long getfileSize(FILE * filePtr) {
  long    sz;
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
  char    ch;
  long    counterA = 0;
  if(filePtr != NULL) {
    rewind(filePtr);        //Make sure we are at the beginning

    ch = getc(filePtr);     //Peek ahead in the file
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
  char    ch;
  long    counterA = 0;
  if(filePtr != NULL) {
    rewind(filePtr);        //Make sure we are at the beginning

    ch = getc(filePtr);     //Peek ahead in the file
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

/*-------------------*/
/* Checks config file*/
/*-------------------*/

void checkConfigFile(int setValue) {
  FILE   *fp;
  int     ok = 0;
  char    ch;

  if(setValue == -1) {
    if(file_exists(CONFIGFILE)) {
      //If we open the program for the first time.
      //Read config file
      ok = openFile(&fp, CONFIGFILE, "r");
      if(ok == 1) {
    ch = getc(fp);
    ch = ch - '0';
    setColorScheme(ch);
      } else {
    printf("Error opening configfile. Setting to defaults.\n");
    setColorScheme(0);
      }
      closeFile(fp);
    } else {
      //Create config file
      ok = openFile(&fp, CONFIGFILE, "w");
      if(ok == 1) {
    fprintf(fp, "%d", 0);
    setColorScheme(0);  //Create config file and set default color Scheme
      } else {
    printf("Error creating config file. Setting to defaults.\n");
    setColorScheme(0);
      }
      closeFile(fp);
    }
  } else {
    //If we call the function from the menu.
    //Set new value in config file
    ok = openFile(&fp, CONFIGFILE, "w");
    if(ok == 1) {
      fprintf(fp, "%d", setValue);
      //setColorScheme(setValue); //Create config file and set default color Scheme
    } else {
      printf("Error creating config file. Setting to defaults.\n");
      setColorScheme(0);
    }
    closeFile(fp);
  }
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
  int     ok;

  if(filePtr != NULL) {
    ok = fclose(filePtr);
  }

  return ok;
}

