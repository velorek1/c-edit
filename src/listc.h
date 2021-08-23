/*
======================================================================
HEADER:
Module to create vertical and horizontal selection menus with
double linked list.
@author : Velorek
@version : 1.0
-LAST MODIFIED : 14/04/2019 Rename headers
======================================================================
*/

#ifndef _LISTC_H_
#define _LISTC_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <string.h>
#include "rterm.h"
#include "scbuf.h"
#include "keyb.h"

/*====================================================================*/
/* TYPE DEFINITIONS                                                   */
/*====================================================================*/

typedef struct _listchoice {
  int     index;        // Item number
  int     backcolor0;       // Back and Fore colors when not selected
  int     forecolor0;
  int     backcolor1;       // Back and Fore colors when selected
  int     forecolor1;
  int     wherex;       // Position of the item
  int     wherey;
  char   *item;         // Item string
  struct _listchoice *next; // Pointer to next item
  struct _listchoice *back; // Pointer to previous item.
} LISTCHOICE;

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

void    add_item(LISTCHOICE * list_identifier, char *str, int x, int y,
         int bcolor0, int fcolor0, int bcolor1, int fcolor1);
void    free_list();
void    display_list();
char    start_vmenu(LISTCHOICE * list_data);
char    start_hmenu(LISTCHOICE * list_data);
void    move_selector(LISTCHOICE * aux, short direction);

#endif
