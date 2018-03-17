#ifndef _LIST_CHOICE_H_
# define _LIST_CHOICE_H_

# include <stdio.h>
# include <stdlib.h>
# include <strings.h>
# include "c_cool.h"

/* Circular linked list for selection of menus */
typedef struct _listchoice {
  int     index;		// Item number
  int     backcolor0;		// Back and Fore colors when not selected
  int     forecolor0;
  int     backcolor1;		// Back and Fore colors when selected
  int     forecolor1;
  int     wherex;		// Position of the item
  int     wherey;
  char   *item;			// Item string
  struct _listchoice *next;	// Pointer to next item
  struct _listchoice *back;	// Pointer to previous item.
} LISTCHOICE;

void    add_item(LISTCHOICE * list_identifier, char *str, int x, int y,
		 int bcolor0, int fcolor0, int bcolor1, int fcolor1);
void    free_list(LISTCHOICE * list_identifier);
void    display_list(LISTCHOICE * list_identifier);
char    start_vmenu(LISTCHOICE * list_data);
char    start_hmenu(LISTCHOICE * list_data);
void    move_up(LISTCHOICE * aux);
void    move_down(LISTCHOICE * aux);
void    move_left(LISTCHOICE * aux);
void    move_right(LISTCHOICE * aux);

#endif
