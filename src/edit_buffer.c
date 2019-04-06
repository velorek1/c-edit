/* 
======================================================================
EDIT BUFFER MODULE
Description:
Edit buffer is a dynamic simple linked list in memory. Lines
are divided by CHR(10).

@author : Velorek
@version : 1.0
Last modified : 17/03/2019 + Created 
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawterm.h"
#include "edit_buffer.h"

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

//MISC. values
#define FILL_CHAR 32
#define CHAR_NIL '\0'
#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80

/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

/*-----------------------------*/
/* Global head & tail pointers */
/* for quick access&control    */
/*-----------------------------*/

int     rows, columns;

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*-------------------------*/
/* Create buffer in memory */
/*-------------------------*/
/* --------------------- */
/* Dynamic List routines */
/* --------------------- */

// create new list element of type LISTBOX from the supplied text string
EDITCELL *newEditCell(char ch, char specialchar) {
  EDITCELL *newp;
  newp = (EDITCELL *) malloc(sizeof(EDITCELL));
  newp->ch = ch;
  newp->specialchar = specialchar;
  newp->index = 0;
  newp->next = NULL;
  newp->back = NULL;
  return newp;
}

// deleleteList: remove list from memory
void deleteBuffer(EDITCELL ** head) {
  EDITCELL *p, *aux;
  aux = *head;
  while(aux->next != NULL) {
    p = aux;
    aux = aux->next;
    free(p);			//remove item
  }
  *head = NULL;
}

/* addend: add new EDITCELL to the end of a list  */
/* usage example: editCell1 = (addend(editCell, newelement(ch,specialChar)); */
EDITCELL *addRear(EDITCELL * head, EDITCELL * newp) {
  EDITCELL *p2;
  if(head == NULL) {
    newp->index = 0;
    newp->back = NULL;
    return newp;
  }
// now find the end of list
  for(p2 = head; p2->next != NULL; p2 = p2->next) ;
  p2->next = newp;
  newp->back = p2;
  newp->index = newp->back->index + 1;
  return head;
}


