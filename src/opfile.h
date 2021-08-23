/*
========================================================================
- HEADER -
Module to open a file by showing a dialogue that allows you to navigate
through directories with a list with scroll.
@author : Velorek
@version : 1.0
Last modified: 14/04/2019 Rename headers
========================================================================
*/

#ifndef _OPFILE_H_
#define _OPFILE_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "scbuf.h"
#include "uintf.h"

/*====================================================================*/
/* CONSTANTS */
/*====================================================================*/
#define MAX_ITEM_LENGTH 15
#define MAX 150
/*====================================================================*/
/* TYPEDEF STRUCTS DEFINITIONS */
/*====================================================================*/
typedef struct _listbox {
  unsigned index;       // Item number
  char   *item;         // Item string
  char   *path;         // Item path
  unsigned isDirectory;     // Kind of item
  struct _listchoice *next; // Pointer to next item
  struct _listchoice *back; // Pointer to previous item
} LISTBOX;

typedef struct _scrolldata {
  unsigned scrollActive;    //To know whether scroll is active or not.
  unsigned scrollLimit;     //Last index for scroll.
  unsigned listLength;      //Total no. of items in the list
  unsigned currentListIndex;    //Pointer to new sublist of items when scrolling.
  unsigned displayLimit;    //No. of elements to be displayed.
  unsigned scrollDirection; //To keep track of scrolling Direction.
  unsigned wherex;
  unsigned wherey;
  unsigned selector;        //Y++
  unsigned backColor0;      //0 unselected; 1 selected
  unsigned foreColor0;
  unsigned backColor1;
  unsigned foreColor1;
  unsigned isDirectory;     // Kind of item
  char   *item;
  char   *path;
  char    fullPath[MAX];
  unsigned itemIndex;
  LISTBOX *head;        //store head of the list
} SCROLLDATA;

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

//CONSOLE DISPLAY FUNCTIONS
void    cleanLine(int line, int backcolor, int forecolor, int startx,
          int numchars);

//DYNAMIC LINKED LIST FUNCTIONS
void    deleteList(LISTBOX ** head);
LISTBOX *addend(LISTBOX * head, LISTBOX * newp);
LISTBOX *newelement(char *text, char *itemPath, unsigned itemType);

//LISTBOX FUNCTIONS
char    listBox(LISTBOX * selector, unsigned whereX, unsigned whereY,
        SCROLLDATA * scrollData, unsigned bColor0,
        unsigned fColor0, unsigned bColor1, unsigned fColor1,
        unsigned displayLimit);
void    loadlist(LISTBOX * head, SCROLLDATA * scrollData,
         unsigned indexAt);

void    gotoIndex(LISTBOX ** aux, SCROLLDATA * scrollData,
          unsigned indexAt);
int     query_length(LISTBOX ** head);
int     move_display(LISTBOX ** head, SCROLLDATA * scrollData);
char    selectorMenu(LISTBOX * aux, SCROLLDATA * scrollData);
void    displayItem2(LISTBOX * aux, SCROLLDATA * scrollData, int select);

//LISTFILES FUNCTIONS
int     listFiles(LISTBOX ** listBox1, char *directory);
int     addSpaces(char temp[MAX_ITEM_LENGTH]);
void    cleanString(char *string, int max);
void    changeDir(SCROLLDATA * scrollData, char fullPath[MAX],
          char newDir[MAX]);
//OPEN FILE DIALOG
void    openFileDialog(SCROLLDATA * openFileData);
int setFileName(char fileName[MAX]);

#endif
