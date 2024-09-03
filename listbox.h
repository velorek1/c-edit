/*
========================================================================
- HEADER -
Module to implement a listBox integrated with LYNX.
@author : Velorek
@version : 1.0
Last modified: 26/12/2022
========================================================================
*/

#ifndef _LISTBOX_H_
#define _LISTBOX_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "scbuf.h"
/*====================================================================*/
/* CONSTANTS */
/*====================================================================*/
//Scroll Control values.
#define SCROLL_ACTIVE 1
#define SCROLL_INACTIVE 0
#define CONTINUE_SCROLL -1
#define DOWN_SCROLL 1
#define UP_SCROLL 0
#define SELECT_ITEM 1
#define UNSELECT_ITEM 0
#define CIRCULAR_ACTIVE 1
#define CIRCULAR_INACTIVE 0
#define LOCKED 1
#define NOT_LOCKED 0
#define VERTICAL 0
#define HORIZONTAL 1
#define VERTICALWITHBREAK 2
/*====================================================================*/
/* TYPEDEF STRUCTS DEFINITIONS */
/*====================================================================*/

typedef struct _listchoice {
  unsigned index;		// Item number
  char   *item;			// Item string
  int setX;		//X,Y position
  int setY;		//-1 to ignore
  int foreColor;        //-1 to ignore
  int backColor;        //-1 to ignore
  struct _listchoice *next;	// Pointer to next item
  struct _listchoice *back;	// Pointer to previous item
} LISTCHOICE;

typedef struct _scrolldata {
  unsigned scrollActive;	//To know whether scroll is active or not.
  unsigned scrollLimit;		//Last index for scroll.
  unsigned listLength;		//Total no. of items in the list
  unsigned currentListIndex;	//Pointer to new sublist of items when scrolling.
  unsigned displayLimit;	//No. of elements to be displayed.
  unsigned selectorLimit;       //how many items of string item can be displayed
  unsigned scrollDirection;	//To keep track of scrolling Direction.
  unsigned wherex;
  unsigned wherey;
  unsigned selector;		//Y++
  unsigned backColor0;		//0 unselected; 1 selected
  unsigned foreColor0;
  unsigned backColor1;
  unsigned foreColor1;
  unsigned addSpaces;          //whether to add spaces or not
  unsigned orientation;
  unsigned displayMetrics;     //to show metrics on screen/reuse code for text viewing
  char   *item;
  int      itemIndex;          //-1 means ESC was pressed
  LISTCHOICE *head;		//store head of the list
} SCROLLDATA;

/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/
extern LISTCHOICE *listBox1;	//Head pointer.


/*====================================================================*/
/* PROTOTYPES OF FUNCTIONS                                            */
/*====================================================================*/

//DYNAMIC LINKED LIST FUNCTIONS
void    removeList(LISTCHOICE ** head);
LISTCHOICE *addatend(LISTCHOICE * head, LISTCHOICE * newp);
LISTCHOICE *newitem(char *text,unsigned setX, unsigned setY,unsigned foreColor, unsigned backColor);

//LISTBOX FUNCTIONS
//void    addItems(LISTCHOICE ** listBox1);
char    listBox(LISTCHOICE * selector, unsigned whereX, unsigned whereY,
		SCROLLDATA * scrollData, unsigned bColor0,
		unsigned fColor0, unsigned bColor1, unsigned fColor1,
		unsigned displayLimit,unsigned listorientation, unsigned addSpaces, unsigned locked);
void    loadlist(LISTCHOICE * head, SCROLLDATA * scrollData,
		 unsigned indexAt);

void printlist(LISTCHOICE * head, SCROLLDATA * scrollData, unsigned displayLimit);

void    gotoIndex(LISTCHOICE ** aux, SCROLLDATA * scrollData,
		  unsigned indexAt);
int     query_length(LISTCHOICE ** head);
int     move_selector(LISTCHOICE ** head, SCROLLDATA * scrollData);
char    selectorMenu(LISTCHOICE * aux, SCROLLDATA * scrollData);
void    displayItem(LISTCHOICE * aux, SCROLLDATA * scrollData, int select);
void    resetScrollData(SCROLLDATA *scrollData);
void 	setselectorLimit(int num);

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

#endif
