/*
========================================================================
- HEADER - 
Module to handle basic file operations
@author : Velorek
@version : 1.0  
Last modified: 09/03/2019 Module created                                                               
========================================================================
*/

#ifndef _FILE_BASICS_H_
#define _FILE_BASICS_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "user_inter.h"
#include "screen_buffer.h"

/*====================================================================*/
/* CONSTANTS                                                          */
/*====================================================================*/
#define CONFIGFILE "cedit.cfg"

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

int     openFile(FILE ** filePtr, char fileName[], char *mode);
int     closeFile(FILE * filePtr);
long    getfileSize(FILE * filePtr);
long    countLinesFile(FILE * filePtr);
long    checkFile(FILE * filePtr);
int     file_exists(char *fileName);
void    checkConfigFile(int setValue);

#endif
