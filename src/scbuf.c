/*
======================================================================
SCREEN MODULE - CODE
Description:
Module to create a double screen buffer to control how things are
displayed on the terminal. Simple linked list with a "cell" that
stores the character to be shown along with its background and
foreground colors. The size of our buffer will be determined by
sc_rows x sc_columns.
Everything you want to show on screen will be printed to the
buffer first. Then the update routine will finally show the final
composition to the user.

@author : Velorek
@version : 1.0
Last modified : 15/04/2019 ScreeChanged added + update_ch.
               Update_smart corrected
=====================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>      /* wint_t */
#include "rterm.h"
//#include "keyb.h"
#include "scbuf.h"
/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

//MISC. values
#define FILL_CHAR 32
#define CHAR_NIL '\0'
#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80
#define SECONDARYBUFFER_ON 1
#define SECONDARYBUFFER_OFF 0
#define MAX_STR 100
#define MAX_TEXT 200
//Special characters (VT100 - ANSI CHARS)
#define NHOR_LINE -50
#define NVER_LINE -51
#define NUPPER_LEFT_CORNER -52
#define NLOWER_LEFT_CORNER -53
#define NUPPER_RIGHT_CORNER -54
#define NLOWER_RIGHT_CORNER -55

#define HOR_LINE 50
#define VER_LINE 51
#define UPPER_LEFT_CORNER 52
#define LOWER_LEFT_CORNER 53
#define UPPER_RIGHT_CORNER 54
#define LOWER_RIGHT_CORNER 55

//Unicode characters
#define HOR_LINE_UNICODE 9472
#define VER_LINE_UNICODE 9474
#define UPPER_LEFT_CORNER_UNICODE 9484
#define LOWER_LEFT_CORNER_UNICODE 9492
#define UPPER_RIGHT_CORNER_UNICODE 9488
#define LOWER_RIGHT_CORNER_UNICODE 9496
#define SPECIAL_CHARS_SET1 -61
#define SPECIAL_CHARS_SET2 -62
/*====================================================================*/
/* TYPEDEF DEFINITIONS                                                */
/*====================================================================*/

/*---------------*/
/* Screen buffer */
/*---------------*/

/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

/*-----------------------------*/
/* Global head & tail pointers */
/* of primary and secondary    */
/* buffers.                    */
/*-----------------------------*/

SCREENCELL *primary_start = NULL;
SCREENCELL *primary_end = NULL;
SCREENCELL *secondary_start = NULL; //backup secondary buffer
SCREENCELL *secondary_end = NULL;   //backup secondary buffer
SCREENCELL *screen, *old_screen;

int     sc_rows, sc_columns;
int     buffersize;
int     bufferON = 0;       //It keeps track of whether the 2nd buffer is full (active).

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*-------------------------*/
/* Create screen in memory */
/*-------------------------*/

void create_screen() {
  /*
     Get terminal dimensions and create 2 dynamic lists with the size of the screen.
     if it is not possible to determine screen dimensions default is set to 80x25.
     Insertion happens at the end of the list. There are three pointers to keep track
     and make insertions. Primary_start : head of the list | primary_end : tail of the list.
     Old buffer works as a double buffer with its own pointers.
   */
  SCREENCELL *new_cell, *newo_cell = NULL;
  int     i;

  //We attempt to get terminal dimensions. If not
  //successful failsafe values (80x25) apply.
  get_terminal_dimensions(&sc_rows, &sc_columns);
  if(sc_rows == 0)
    sc_rows = ROWS_FAILSAFE;
  if(sc_columns == 0)
    sc_columns = COLUMNS_FAILSAFE;
  buffersize = sc_rows * sc_columns;

  // Primary Screen buffer
  if(primary_start == NULL && primary_end == NULL) {
    //First item
    screen = (SCREENCELL *) malloc(sizeof(SCREENCELL));
    screen->backcolor0 = B_BLACK;
    screen->forecolor0 = F_WHITE;
    screen->item = FILL_CHAR;
    screen->next = NULL;
    screen->index = 0;
    screen->sc_rows = sc_rows;
    screen->sc_columns = sc_columns;
    primary_start = screen; // Pointer to first cell
    primary_end = screen;   // Pointer to last/previous cell
    primary_end->next = NULL;   //Last item of the list next points to NULL
  }
  for(i = 1; i <= buffersize - 1; i++) {
    //Additional items
    new_cell = (SCREENCELL *) malloc(sizeof(SCREENCELL));
    new_cell->backcolor0 = B_BLACK;
    new_cell->forecolor0 = F_WHITE;
    new_cell->item = FILL_CHAR;
    new_cell->index = i;
    new_cell->sc_rows = sc_rows;    //Each cell stores screen dimensions
    new_cell->sc_columns = sc_columns;
    primary_end->next = new_cell;
    primary_end = new_cell;
    primary_end->next = NULL;   //Last item of the list next points to NULL
  }

  // Create secondary buffer to be used as an 'undo' buffer
  if(secondary_start == NULL && secondary_end == NULL) {
    //First item
    old_screen = (SCREENCELL *) malloc(sizeof(SCREENCELL));
    old_screen->backcolor0 = B_BLACK;
    old_screen->forecolor0 = F_WHITE;
    old_screen->item = FILL_CHAR;
    old_screen->next = NULL;
    old_screen->sc_rows = sc_rows;
    old_screen->sc_columns = sc_columns;
    old_screen->index = 0;
    secondary_start = old_screen;   // Pointer to first cell
    secondary_end = old_screen; // Pointer to last/previous cell
    secondary_end->next = NULL; //Last item of the list next points to NULL
  }
  for(i = 1; i <= buffersize - 1; i++) {
    //Additional items
    newo_cell = (SCREENCELL *) malloc(sizeof(SCREENCELL));
    newo_cell->backcolor0 = B_BLACK;
    newo_cell->forecolor0 = F_WHITE;
    newo_cell->item = FILL_CHAR;
    newo_cell->index = i;
    newo_cell->sc_rows = sc_rows;   //Each cell stores screen dimensions
    newo_cell->sc_columns = sc_columns;
    secondary_end->next = newo_cell;
    secondary_end = newo_cell;
    secondary_end->next = NULL; //Last item of the list next points to NULL
  }

}

/*--------------------------------------*/
/* Copy primary buffer to second buffer */
/*--------------------------------------*/

void save_buffer() {
  //Copies primary buffer into secondary buffer.
  //Thereby saving current screen to be retrieved later.
  int     i;
  SCREENCELL *aux, *aux2;
  bufferON = SECONDARYBUFFER_ON;    // =1
  aux = primary_start;      // Points to the first item of the first buffer
  aux2 = secondary_start;   //Points to the first item of the second buffer
  for(i = 0; i < buffersize; i++) {
    //Copy buffer 1 into buffer 2.
    aux2->forecolor0 = aux->forecolor0;
    aux2->backcolor0 = aux->backcolor0;
    aux2->item = aux->item;
    aux = aux->next;
    aux2 = aux2->next;
  }
}
/*----------------------*/
/* FLUSH primary buffer */
/*----------------------*/

void flush_buffer() {
  //Copies primary buffer into secondary buffer.
  //Thereby saving current screen to be retrieved later.
  int     i;
  SCREENCELL *aux;
  aux = primary_start;      // Points to the first item of the first buffer
  for(i = 0; i < buffersize; i++) {
    //Copy buffer 1 into buffer 2.
    aux->item = FILL_CHAR;
    aux = aux->next;
  }
}


/*---------------------------------------------------*/
/* Copy secondary buffer into primary buffer         */
/*---------------------------------------------------*/

void restore_buffer() {
  //Copies buffer 2 into buffer 1
  //Retrieve previous screen from memory.
  int     i;
  SCREENCELL *aux, *aux2;
  aux = primary_start;      // Points to the first item of the first buffer
  aux2 = secondary_start;   //Points to the first item of the second buffer
  bufferON = SECONDARYBUFFER_OFF;   //=0
  for(i = 0; i < buffersize; i++) {
    //Copy buffer 2 into buffer 1.
    aux->forecolor0 = aux2->forecolor0;
    aux->backcolor0 = aux2->backcolor0;
    aux->item = aux2->item;
    aux = aux->next;
    aux2 = aux2->next;
  }
}

/*---------------------------------------*/
/*Converts Alternative Char to Unicode 8 */
/*---------------------------------------*/

int mapChartoU8(int character) {
//Map ANSI VT-100 characters to UNICODE characters
  int     rtvalue;
  switch (character) {
    case HOR_LINE:
      //horizontal line
      rtvalue = HOR_LINE_UNICODE;
      break;
    case VER_LINE:
      //vertical line
      rtvalue = VER_LINE_UNICODE;
      break;
    case UPPER_LEFT_CORNER:
      //upper left corner
      rtvalue = UPPER_LEFT_CORNER_UNICODE;
      break;
    case LOWER_LEFT_CORNER:
      //lower left corner
      rtvalue = LOWER_LEFT_CORNER_UNICODE;
      break;
    case UPPER_RIGHT_CORNER:
      //upper right corner
      rtvalue = UPPER_RIGHT_CORNER_UNICODE;
      break;
    case LOWER_RIGHT_CORNER:
      // lower right corner
      rtvalue = LOWER_RIGHT_CORNER_UNICODE;
      break;

    default:
      rtvalue = 0;
      break;
  }
  return rtvalue;
}


/*------------------------------------------*/
/* Reports whether screen has changed       */
/*------------------------------------------*/

int screenChanged() {

  int     i;
  int     changed=0;
  SCREENCELL *aux, *aux2;
  aux = primary_start;
  aux2 = secondary_start;
  for(i = 0; i <= buffersize; i++) {
    if(aux->item != aux2->item ||
    aux->forecolor0 != aux2->forecolor0 ||
    aux->backcolor0 != aux2->backcolor0 ) {
         changed=1;
     }

   if(aux->next != NULL)
      aux = aux->next;
   if(aux2->next != NULL)
      aux2 = aux2->next;
   }
 return changed;
}

/*--------------------------.*/
/* reads a char from buffer. */
/*--------------------------.*/


char read_char(int x, int y) {
/* read specific character from buffer */
  int     i, pos;
  char ch = FILL_CHAR;
  SCREENCELL *aux;
  if (bufferON==1)  //check whether savebuffer to use in the dynamic shadow
    aux = secondary_start;      // we set our auxiliary pointer at the beginning of the list.
  else
    aux = primary_start;
  pos = (y - 1) * sc_columns + x;   //this is the formula to calculate the position index in the screen buffer
 if(pos <= buffersize) {
  for(i = 0; i <= pos; i++) {
      //run through the buffer until reaching desired position
      if(aux->index == pos)
    break;
      aux = aux->next;
    }
  ch = aux->item;
 }
  return ch;
}

/*--------------------------.*/
/* flush cell from buffer.  */
/*--------------------------.*/


void flush_cell(int x, int y) {
/* read specific character from buffer */
  int     i, pos;
  SCREENCELL *aux;
  if (bufferON==1)  //check whether savebuffer to use in the dynamic shadow
    aux = secondary_start;      // we set our auxiliary pointer at the beginning of the list.
  else
    aux = primary_start;
  pos = (y - 1) * sc_columns + x;   //this is the formula to calculate the position index in the screen buffer
 if(pos <= buffersize) {
  for(i = 0; i <= pos; i++) {
      //run through the buffer until reaching desired position
      if(aux->index == pos)
    break;
      aux = aux->next;
    }
  aux->item = FILL_CHAR;
 }
}


/*-------------------------.*/
/* Writes a char to buffer. */
/*-------------------------.*/

void write_ch(int x, int y, char ch, int backcolor, int forecolor) {
/* It will be shown on screen when it is updated by calling update_screen */
  int     i, pos;
  SCREENCELL *aux;
  aux = primary_start;      // we set our auxiliary pointer at the beginning of the list.
  pos = (y - 1) * sc_columns + x;   //This is the formula to calculate the position index in the screen buffer

  if(pos <= buffersize) {
    // If it is within buffer limits, otherwise do nothing.
    for(i = 0; i <= pos; i++) {
      //Run through the buffer until reaching desired position
      if(aux->index == pos)
    break;
      aux = aux->next;
    }
    //Update cell info at position selected.
    //Manage special charaters as well.
    if(ch != SPECIAL_CHARS_SET1 && ch != SPECIAL_CHARS_SET2)
      aux->item = ch;
    else {
      aux->specialchar = ch;
    }
    aux->backcolor0 = backcolor;
    aux->forecolor0 = forecolor;
  }
}

/*------------------------------------------*/
/* Writes a string of characters to buffer. */
/*------------------------------------------*/

void write_str(int x, int y, char *str, int backcolor, int forecolor) {
  //Writes a string of characters to buffer.
  char   *astr;
  unsigned int     i, wherex;
  wherex = x;
  //astr = (char *)malloc(sizeof(char) * strlen(str) + 1);
  astr = str;
  for(i = 0; i <= strlen(str) - 1; i++) {
    write_ch(wherex, y, astr[i], backcolor, forecolor);
    wherex = wherex + 1;

  }
  //astr = NULL;
  //free(astr);
}

/*-----------------------------------------------*/
/* Writes an integer value as a string on screen */
/*-----------------------------------------------*/

int write_num(int x, int y, int num, int length, int backcolor,
           int forecolor) {
  //the length of the string must be passed on the function
  char   *astr;
  char len;
  astr = (char *)malloc(sizeof(char) * length + 1);
  sprintf(astr, "%d", num);
  write_str(x, y, astr, backcolor, forecolor);
  len = strlen(astr);
  free(astr);
  return len;
}

/*------------------------------------------*/
/* Update single char to screen for display */
/*------------------------------------------*/
void update_ch(int x, int y, char ch, char specialChar, int backcolor, int forecolor) {
//It could be interepreted as write_ch - forced update to screen
//to be used to simplify code in update_smart.
signed char tempchar=0;
   gotoxy(x, y);
   outputcolor(forecolor, backcolor);
   if(ch > 0)
      // Check whether chars are negative
      printf("%c", ch);
   else {
      /*
         For convention, if we have negative chars that means that we
         are printing box-like characters or accents to screen.
         Box characters are mapped to Unicode. */
  //printf("%c(0",27); //Deprecated - Activate box-like characters in vt-100
  // printf("%c(B",27); //Deactivate box-like characters
    if(ch >= -55 && ch <= -50) {
    //Box chars.
    setlocale(LC_ALL, "");
    tempchar = ch * -1; //Change negative values to positive.
    printf("%lc", (wint_t) mapChartoU8(tempchar));  //unicode
    }
    if(ch <= -65)
    //Accents -61/-62 + char
    printf("%c%c", specialChar, ch);
    }
}
/*------------------------------------*/
/* Dumps buffer to screen for display */
/*------------------------------------*/

void update_screen() {
  int     i, wherex, wherey;
  SCREENCELL *aux;
  wherey = 1;
  wherex = 0;
  aux = primary_start;
  for(i = 0; i <= buffersize; i++) {
     //Next item
    update_ch(wherex,wherey, aux->item, aux->specialchar, aux->backcolor0,
    aux->forecolor0);

   if(aux->next != NULL)
      aux = aux->next;
   wherex = wherex + 1; //line counter
    if(wherex == sc_columns+1) {
      //new line
      wherex = 1;
      wherey = wherey + 1;
    }
 }
}

int update_smart() {
//Note: if wherex = 0 then wherex=sc_columns i
// and wherey--; because of the result of modulus operation
  int     i, wherex, wherey;
  int     changed=0;
  SCREENCELL *aux, *aux2;
  aux = primary_start;
  aux2 = secondary_start;
  for(i = 0; i <= buffersize; i++) {
    if(aux->item != aux2->item) {
      wherex = aux->index % sc_columns;
      wherey = (int)(aux->index / sc_columns) + 1;
      if (wherex==0) {wherex=sc_columns; wherey=wherey-1;}
      update_ch(wherex,wherey,aux->item,aux->specialchar,aux->backcolor0,aux->forecolor0);
      changed=1;
    }
    if(aux->backcolor0 != aux2->backcolor0) {
      wherex = aux->index % sc_columns;
      wherey = (int)(aux->index / sc_columns) + 1;
      if (wherex==0) {wherex=sc_columns; wherey=wherey-1;}
      update_ch(wherex,wherey,aux->item,aux->specialchar,aux->backcolor0,
      aux->forecolor0);
      changed=1;
    }
    if(aux->forecolor0 != aux2->forecolor0) {
      wherex = aux->index % sc_columns;
      wherey = (int)(aux->index / sc_columns) + 1;
      if (wherex==0) {wherex=sc_columns; wherey=wherey-1;}
      update_ch(wherex,wherey,aux->item,aux->specialchar,aux->backcolor0,
      aux->forecolor0);
      changed=1;
    }
   //move to next item
   if(aux->next != NULL)
      aux = aux->next;
   if(aux2->next != NULL)
      aux2 = aux2->next;
  }
return changed;
}

/*-------------------------*/
/* Restore previous buffer */
/*-------------------------*/

void close_window() {
  restore_buffer();
  update_screen();
}

/*--------------------------*/
/* Changes background color */
/*--------------------------*/

void screen_color(int color) {
/* Changes the color of all the cells to create the effect of changing color in the background */
  int     i;
  SCREENCELL *aux;
  aux = primary_start;
  for(i = 0; i < buffersize; i++) {
    aux->backcolor0 = color;
    aux = aux->next;
  }
  update_screen();
}

/*-----------------------------*/
/* Destroy both screen buffers */
/*-----------------------------*/
/* Function to delete the entire linked list */
void free_buffer()
{
   /* deref head_ref to get the real head */
   SCREENCELL *current;
   SCREENCELL *next;
   current=primary_start;

   while (current != NULL)
   {
       next = current->next;
       //if (current != NULL){
         free(current);
       //}
       current = next;
   }
    current=secondary_start;

   while (current != NULL)
   {
       next = current->next;
       //if (current != NULL){
         free(current);
       //}
       current = next;
   }
   free(current);
   /* deref head_ref to affect the real head back
      in the caller. */
   primary_start = NULL;
  primary_end = NULL;
  secondary_start = NULL;
  secondary_end = NULL;
}

/*------------------------------------------*/
/* Draw window area with or without border. */
/*------------------------------------------*/

void draw_window(int x1, int y1, int x2, int y2, int backcolor,
         int bordercolor, int titlecolor, int border, int title) {
/*
   Chars for drawing box-like characters will be passed as negative values.
   When the update_screen routine is called, it will check for negative
   values and map these chars to Unicode characters.
 */
  int     i, j;
  char ch=FILL_CHAR;
  i = x1;
  j = y1;
  save_buffer();        //saves screen to be restored later in close_window
  //shadow
  for(j = y1 + 1; j <= y2 + 1; j++)
    for(i = x1 + 1; i <= x2 + 1; i++)
    {
      ch=read_char(i,j); //dynamic shadow
      if (ch==CHAR_NIL) ch=FILL_CHAR;
      write_ch(i, j, ch, B_BLACK, F_WHITE);
    }
  //window
  for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
      write_ch(i, j, ' ', backcolor, bordercolor);
  //borders
  if(border == 1) {
    //with borders. ANSI-ASCII 106-121
    for(i = x1; i <= x2; i++) {
      //upper and lower borders
      write_ch(i, y1, NHOR_LINE, backcolor, bordercolor);   //horizontal line box-like char
      write_ch(i, y2, NHOR_LINE, backcolor, bordercolor);
    }
    for(j = y1; j <= y2; j++) {
      //left and right borders
      write_ch(x1, j, NVER_LINE, backcolor, bordercolor);   //vertical line box-like char
      write_ch(x2, j, NVER_LINE, backcolor, bordercolor);
    }
    write_ch(x1, y1, NUPPER_LEFT_CORNER, backcolor, bordercolor);   //upper-left corner box-like char
    write_ch(x1, y2, NLOWER_LEFT_CORNER, backcolor, bordercolor);   //lower-left corner box-like char
    write_ch(x2, y1, NUPPER_RIGHT_CORNER, backcolor, bordercolor);  //upper-right corner box-like char
    write_ch(x2, y2, NLOWER_RIGHT_CORNER, backcolor, bordercolor);  //lower-right corner box-like char
  }
  if (title == 1) {
    for(i = x1; i <= x2; i++)
      write_ch(i, y1-1, ' ', titlecolor, titlecolor);
  }
  //update_smart();
}

/*--------------*/
/* Clear string */
/*--------------*/

void clearString(char *string, int max) {
  int     i;
  for(i = 0; i < max; i++) {
    string[i] = '\0';
  }
}
