/* 
======================================================================
HEADER: Module to create an edit screen buffer.
Edit buffer is a dynamic simple linked list in memory. Lines
are divided by CHR(10).

@author : Velorek
@version : 1.0
Last modified : 17/03/2019 + Created 
======================================================================
*/

#ifndef _EDIT_BUFFER_H_
#define _EDIT_BUFFER_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "rawterm.h"


/*====================================================================*/
/* TYPE DEFINITIONS                                                   */
/*====================================================================*/

typedef struct _editcell {
  char     ch;			// Item string
  char     specialchar;		// Control for accents and special chars
  short    index;
  struct   _editcell *next;	// Pointer to next item
  struct   _editcell *back;	// Pointer to previous item
} EDITCELL;

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/
EDITCELL *newEditCell(char ch, char specialchar);
void deleteBuffer(EDITCELL ** head);
EDITCELL *addRear(EDITCELL * head, EDITCELL * newp);

#endif
