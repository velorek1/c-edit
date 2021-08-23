/*
======================================================================
Module to make a circular linked list to make a selection menu in C.

@author : Velorek
@version : 1.0
Last modified : 11/02/2019 - Switch to readch() instead of readch()
                06/04/2019 - Fixed memory errors and leaks.
        14/04/2019 - Rename headers
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <string.h>
#include "rterm.h"
#include "scbuf.h"
#include "keyb.h"

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

//DROP-DOWN MENU VALUES
#define K_LEFTMENU -1       //Left arrow key pressed while in menu
#define K_RIGHTMENU -2      //Right arrow key pressed while in menu
#define ESC_KEY 1
#define EXIT_FLAG 1

//DIRECTIONS
#define DIR_LEFT 2
#define DIR_RIGHT 4
#define DIR_UP 1
#define DIR_DOWN 0

/*====================================================================*/
/* TYPEDEF DEFINITIONS                                                */
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
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

LISTCHOICE *head = NULL;
LISTCHOICE *tail = NULL;
LISTCHOICE *former = NULL;
LISTCHOICE *current = NULL;

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*------------*/
/*Add new item*/
/*------------*/

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
    list_identifier->item = (char *)malloc(sizeof(char) * strlen(str) + 1);
    strcpy(list_identifier->item, str);
    list_identifier->next = head;
    list_identifier->back = head;
    list_identifier->index = 0;
  } else {
    //Circular list structure
    former = tail;      //pointer to previous item
    newp = (LISTCHOICE *) malloc(sizeof(LISTCHOICE));
    former->next = newp;    //previous item points to current item
    newp->back = former;    //new item back points to previous item
    tail = newp;        //tail always points to the last item
    head->back = tail;      // This way we close the circular list : First item back points to last item
    tail->next = head;      //Last item next points to first item

    //Create first item of the list and assign properties to it.
    newp->backcolor0 = bcolor0;
    newp->backcolor1 = bcolor1;
    newp->forecolor0 = fcolor0;
    newp->forecolor1 = fcolor1;
    newp->wherex = x;
    newp->wherey = y;
    newp->item = (char *)malloc(sizeof(char) * strlen(str) + 1);
    strcpy(newp->item, str);
    newp->index = former->index + 1;
  }

}

/*-----------------------*/
/*Remove list from memory*/
/*-----------------------*/
/* Function to delete the entire linked list */
void free_list()
{
   /* deref head_ref to get the real head */
   LISTCHOICE *cur;
   LISTCHOICE *back;
   cur = tail;
   while (cur != head)
   {
       back = cur->back;
       //if (current != NULL){
         free(cur->item);
         free(cur);
       //}
       cur = back;
   }
   free(head->item);
   free(head);
   /* deref head_ref to affect the real head back
      in the caller. */
   head = NULL;
   tail = NULL;
}
/*
void free_list(LISTCHOICE * list_identifier) {
  LISTCHOICE *aux, *p;
  aux = tail;
  while (aux != head->back) {
    p = aux;
    if (p->item != NULL) free(p->item);     //free off the string field
    if (p != NULL) free(p);         //remove item
  }
  head = NULL;
  tail = NULL;
  former = NULL;
  current = NULL;
}
*/

/*-----------------------------------------*/
/* Displays the items contained in the list
   with the properties specified.          */
/*-----------------------------------------*/

void display_list() {
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

/*---------------------------------------*/
/*Unmark previous item and mark next item*/
/*---------------------------------------*/

void move_selector(LISTCHOICE * aux, short direction) {
  //Unmark previous iten
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor0,
        aux->forecolor0);
  //Highlight current item
  switch (direction) {
    case DIR_UP:
      *aux = *aux->back;
      break;            //Go to next item
    case DIR_DOWN:
      *aux = *aux->next;
      break;            //Go to next item
    case DIR_LEFT:
      *aux = *aux->next;
      break;            //Go to next item
    case DIR_RIGHT:
      *aux = *aux->back;
      break;            //Go to next item
  }
  write_str(aux->wherex, aux->wherey, aux->item, aux->backcolor1,
        aux->forecolor1);
  update_screen();
}

/*------------------------------------*/
/*Starts the selection menu - Vertical*/
/*------------------------------------*/

char start_vmenu(LISTCHOICE * list_data) {
  char    ch;
  LISTCHOICE aux;
  int     exitwhile = 0;
  int     esc_key = 0;
  char chartrail[5];
  unsigned keypressed = 0;
  display_list(list_data);  //display whole list

  //Highlight first item on the list
  aux = *tail;          //Auxiliary variable points to last item
  move_selector(&aux, DIR_DOWN);    //Move down to point to highlight first item.

  ch = 0;           //init ch value
  while(ch != K_ENTER)      // escape key or enter
  {
   keypressed = kbhit();
   if (keypressed == 1){
      ch = readch();
      keypressed = 0;

      if(ch == K_ESCAPE)        // escape key
      {
        read_keytrail(chartrail);   // read key again for arrow key combinations
        esc_key = ESC_KEY;
        if(strcmp(chartrail, K_UP_TRAIL) == 0) {
      move_selector(&aux, DIR_UP);
      esc_key = 0;
    }
    if(strcmp(chartrail, K_DOWN_TRAIL) == 0) {
      move_selector(&aux, DIR_DOWN);
      esc_key = 0;
    }
        if(strcmp(chartrail, K_RIGHT_TRAIL) == 0) {
      ch = K_LEFTMENU;  // this will be used in top-down menu. Mark for left arrow key
      exitwhile = EXIT_FLAG;
      esc_key = 0;
    }
        if(strcmp(chartrail, K_LEFT_TRAIL) == 0) {
      ch = K_RIGHTMENU; // this will be used in top-down menu. Mark for right arrow key
      exitwhile = EXIT_FLAG;
      esc_key = 0;
    }
      }
    }
    if(esc_key == ESC_KEY) {
      break;
    }
    if(exitwhile == EXIT_FLAG)
      break;
  }

  if(ch == K_ENTER) {
    //Pass data of last item by value.
    *list_data = aux;
    resetch();
  }
  return ch;            // return the key or code pressed

}

/*--------------------------------------*/
/*Starts the selection menu - horizontal*/
/*--------------------------------------*/
char start_hmenu(LISTCHOICE * list_data) {
  char    ch;
  //int     exitloop = 0;
  unsigned keypressed = 0 ;
  char chartrail[5];
  LISTCHOICE aux;

  display_list(list_data);  //display whole list

  //Highlight first item on the list
  aux = *tail;          //Auxiliary variable points to last item
  move_selector(&aux, DIR_LEFT);    //Move down to point to highlight first item.

  ch = 0;           //init ch value
  while(ch != K_ENTER)      // escape key or enter
  {
     keypressed = kbhit();
     if (keypressed == 1){
      ch = readch();
      keypressed = 0;

      if(ch == K_ESCAPE)        // escape key
      {
        read_keytrail(chartrail);   // read key again for arrow key combinations
        //esc_key = ESC_KEY;
        if(strcmp(chartrail, K_RIGHT_TRAIL) == 0) {
      move_selector(&aux, DIR_LEFT);
      //esc_key = 0;
    }
        if(strcmp(chartrail, K_LEFT_TRAIL) == 0) {
      move_selector(&aux, DIR_RIGHT);
      //esc_key = 0;
    }

//    if(exitloop == EXIT_FLAG)
  //    break;
//  default:
//    exitloop = EXIT_FLAG;
//    break;
      }

       if (ch == K_TAB) move_selector(&aux, DIR_LEFT);
    }
  }
  if(ch == K_ENTER) {
    //Pass data of last item by value.
    *list_data = aux;
    resetch();
  }
  return ch;
}
