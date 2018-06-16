/* 
======================================================================
Module to make a circular linked list to make a selection menu in C.

@author : Velorek
@version : 1.0
Last modified : JUN 2018
======================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_cool.h"
#include "screen_buffer.h"

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

LISTCHOICE *head = NULL;
LISTCHOICE *tail = NULL;
LISTCHOICE *former = NULL;
LISTCHOICE *current = NULL;

/* List choice routines */
void add_item(LISTCHOICE * list_identifier, char *str, int x, int y,
	      int bcolor0, int fcolor0, int bcolor1, int fcolor1) {
  LISTCHOICE *newp;
  if(head == NULL && list_identifier == NULL) {
    //Reserve memory
    list_identifier = (LISTCHOICE *) malloc(sizeof(LISTCHOICE));
    head = list_identifier;
    tail = list_identifier;

    //Create first item of the list and assign properties to it.
    list_identifier->backcolor0 = bcolor0;
    list_identifier->backcolor1 = bcolor1;
    list_identifier->forecolor0 = fcolor0;
    list_identifier->forecolor1 = fcolor1;
    list_identifier->wherex = x;
    list_identifier->wherey = y;
    list_identifier->item = (char *)malloc(strlen(str) + 1);
    strcpy(list_identifier->item, str);
    list_identifier->next = head;
    list_identifier->back = head;
    list_identifier->index = 0;
  } else {
    //Circular list structure
    former = tail;		//pointer to previous item
    newp = (LISTCHOICE *) malloc(sizeof(LISTCHOICE));
    former->next = newp;	//previous item points to current item
    newp->back = former;	//new item back points to previous item
    tail = newp;		//tail always points to the last item
    head->back = tail;		// This way we close the circular list : First item back points to last item
    tail->next = head;		//Last item next points to first item

    //Create first item of the list and assign properties to it.
    newp->backcolor0 = bcolor0;
    newp->backcolor1 = bcolor1;
    newp->forecolor0 = fcolor0;
    newp->forecolor1 = fcolor1;
    newp->wherex = x;
    newp->wherey = y;
    newp->item = (char *)malloc(strlen(str) + 1);
    strcpy(newp->item, str);
    newp->index = former->index + 1;
  }

}

/* Remove list from memory */
void free_list(LISTCHOICE * list_identifier) {
  LISTCHOICE *aux, *p;
  aux = tail;
  do {
    p = aux;
    aux = aux->back;
    free(p->item);		//free off the string field
    free(p);			//remove item
  } while(aux != head->back);
  head = NULL;
  tail = NULL;
  former = NULL;
  current = NULL;
}

/* Displays the items contained in the list with the properties specified */

void display_list(LISTCHOICE * list_identifier) {
/* 
Displays the items contained in the list with the properties specified
in init_list.
until the auxiliary pointer reaches the first item again
First item is equal to tail->next.
*/

  LISTCHOICE *aux;
  aux = head;
  do {
    write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor0,
	      aux->forecolor0);
    aux = aux->next;
  } while(aux != tail->next);
  update_screen();

}

/* Unmark previous item and mark next item */
void move_right(LISTCHOICE * aux) {
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor0,
	    aux->forecolor0);
  //Highlight current item
  *aux = *aux->back;		//Go to next item
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor1,
	    aux->forecolor1);
  update_screen();
}

/* Unmark previous item and mark next item */
void move_left(LISTCHOICE * aux) {
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor0,
	    aux->forecolor0);
  //Highlight current item
  *aux = *aux->next;		//Go to next item
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor1,
	    aux->forecolor1);
  update_screen();
}

/* Unmark previous item and mark next item */
void move_up(LISTCHOICE * aux) {
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor0,
	    aux->forecolor0);
  //Highlight current item
  *aux = *aux->back;		//Go to next item
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor1,
	    aux->forecolor1);
  update_screen();
}

/* Unmark previous item and mark next item */
void move_down(LISTCHOICE * aux) {
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor0,
	    aux->forecolor0);
  //Highlight current item
  *aux = *aux->next;		//Go to next item
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor1,
	    aux->forecolor1);
  update_screen();
}

/* Starts the selection menu */
char start_vmenu(LISTCHOICE * list_data) {
  char    ch;
  LISTCHOICE aux;
  int     exitwhile = 0;
  int esc_key=0;
  display_list(list_data);	//display whole list

  //Highlight first item on the list
  aux = *tail;			//Auxiliary variable points to last item
  move_down(&aux);		//Move down to point to highlight first item.

  ch = 0;			//init ch value
  while(ch != 13)		// escape key or enter
  {
    ch = getch();
    if(ch == '\033') {
      esc_key=1;
      getch();
      switch (getch()) {
	case 'A':
	  move_up(&aux);
          esc_key=0;
	  break;
	case 'B':
	  move_down(&aux);
          esc_key=0;
	  break;
	case 'C':
	  ch = -1;		// this will be used in top-down menu. Mark for left arrow key
	  exitwhile = 1;
          esc_key=0;
	  break;
	case 'D':
	  ch = -2;		// this will be used in top-down menu. Mark for right arrow key
	  exitwhile = 1;
          esc_key=0;
	  break;
        default:
          esc_key=1;
          break;
      }
     }
  if (esc_key==1){
     break;
     }
  if(exitwhile == 1)
     break;
  }
 if(ch == 13||esc_key==1) {
    //Pass data of last item by value.
    *list_data = aux;
  }
  return ch;			// return the key or code pressed

}

/* Starts the selection menu */
char start_hmenu(LISTCHOICE * list_data) {
  char    ch;
  int exitloop=0;
  LISTCHOICE aux;

  display_list(list_data);	//display whole list

  //Highlight first item on the list
  aux = *tail;			//Auxiliary variable points to last item
  move_left(&aux);		//Move down to point to highlight first item.

  ch = 0;			//init ch value
  while(ch != 13)		// escape key or enter
  {
    ch=getch();
   if (exitloop==1) break;
     if(ch == '\033') {
      getch();
      switch (getch()) {
	case 'C':
	  move_left(&aux);
	  break;
	case 'D':
	  move_right(&aux);
	  break;
        default:
          exitloop=1;
          break;
      }
   } 
  }
  if(ch == 13) {
    //Pass data of last item by value.
    *list_data = aux;
  }
 return ch;
}
