/*
========================================================================
- HEADER - 
Module to open a file by showing a dialogue that allows you to navigate 
through directories with a list with scroll.
@author : Velorek
@version : 1.0  
Last modified: 23/04/2024 Vastly simplified and integraed with lynx
========================================================================
*/

#ifndef _OPFILE_H_
#define _OPFILE_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "scbuf.h"
#include "listbox.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"

/*====================================================================*/
/* CONSTANTS */
/*====================================================================*/
#define MAX_ITEM_LENGTH 15


/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

int listFiles(LISTCHOICE ** listBox1, char *directory);
void addItems(LISTCHOICE **listBox1);
int openFileDialog(char fileName[MAXFILENAME], char fullPath[MAXFILENAME]);


#endif
