/* Module to create a screen buffer
 * to act as an intermediate step between screen output and
 * the display so as to create a Text User Interface.
 * @author:velorek
 * Last modified: 1/02/2019
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rterm.h"
#include "scbuf.h"

#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80
#define FILL_CHAR 32

  int     sc_rows=0, sc_columns=0;
  int     buffersize=0;
 

/* SINGLE LINKED LIST ROUTINES */
// create new list element of type SCREENCELL from the supplied text string
SCREENCELL *newelement(SCREENCELL temp)
{
	SCREENCELL *newp;
	newp = (SCREENCELL *) malloc (sizeof(SCREENCELL));
	newp->index = temp.index;
	newp->backColor = temp.backColor;
	newp->foreColor = temp.foreColor;
        newp->toUpdate = 0;
	newp->ch = temp.ch;
        newp->next = NULL;
	return newp;
}


void deleteList(SCREENCELL **head) 
{ 
   /* deref head_ref to get the real head */
   SCREENCELL *current = *head; 
   SCREENCELL *next = NULL;
   while (current != NULL)  
   { 
       next = current->next; 
       free(current);
       current = next; 
   }
  
   free(current);
   free(next);
   current = NULL;
   next = NULL;
   *head = NULL; 
} 

/* addfront: add new SCREENCELL to front of list  */
/* example usage: start = (addfront(start, newelement("burgers")); */

SCREENCELL *addfront(SCREENCELL *head, SCREENCELL *newp)
{
	newp -> next = head;
	return newp;
}

/* addend: add new SCREENCELL to the end of a list  */
/* usage example: start = (addend(start, newelement("wine")); */

SCREENCELL *addend (SCREENCELL *head, SCREENCELL *newp)
{
	SCREENCELL *p2; 	
	if (head == NULL)
		return newp;
// now find the end of list
	for (p2 = head; p2 -> next != NULL; p2 = p2 -> next)
		;
	p2 -> next = newp;
	return head;
}


int length(SCREENCELL **head)
// this routine uses pointer-to-pointer techniques :-)
{

	int count=0;
	SCREENCELL **tracer = head;
	while ((*tracer) != NULL) {
		count = count +1;	
		tracer = &(*tracer)->next;
	}
       return count;
}

void reindex(SCREENCELL **head)
{
	int count=0;	
	SCREENCELL *p=NULL;
	SCREENCELL **tracer = head;
	while ((*tracer) != NULL) {
		p = *tracer;
		p->index=count;
		count = count +1;
		tracer = &(*tracer)->next;
	}           
}

/* SCREEN BUFFER ROUTINES */

int create_screen(SCREENCELL **newScreen){
 /*
     Get terminal dimensions and create a dynamic list with the size of the screen.
     if it is not possible to determine screen dimensions default is set to 80x25.
     Insertion happens at the end of the list.   */
  int     i=0;
  SCREENCELL   temp;
  SCREENCELL  **aux = newScreen;

  //We attempt to get terminal dimensions. If not
  //successful failsafe values (80x25) apply.
  get_terminal_dimensions(&sc_rows, &sc_columns);
  if(sc_rows == 0)
    sc_rows = ROWS_FAILSAFE;
  if(sc_columns == 0)
    sc_columns = COLUMNS_FAILSAFE;
  buffersize = sc_rows * sc_columns;

  // Assign dummy data to Screen buffer
   if(*aux == NULL) {
    //All cells have the same init data
       temp.ch = FILL_CHAR;
       temp.backColor = B_BLACK;
       temp.foreColor = F_WHITE; 
     for(i = 0; i < buffersize; i++) {
      //Except for their index
      temp.index = i;
      *aux = addend(*aux, newelement(temp)); 
     }
     return 1; 
   } else{
      //Pointer is not void
     return 0;
   }
}
void copy_screen(SCREENCELL *destination,SCREENCELL *source){
  int     i=0;
  SCREENCELL *aux1=destination;
  SCREENCELL *aux2=source;
   //We attempt to get terminal dimensions. If not
  //successful failsafe values (80x25) apply.
 if((aux1 != NULL && aux2 != NULL) && (length(&aux1) == length(&aux2)) ) {
   for(i = 0; i < buffersize; i++) {
      aux1->backColor = aux2->backColor;
      aux1->foreColor = aux2->foreColor;
      aux1->ch = aux2->ch;
      aux1 = aux1->next;
      aux2 = aux2->next;
    }
   }
}


/*------------------------------------------*/
/* Update single char to screen for display */
/*------------------------------------------*/
void update_ch(int x, int y, wchar_t ch, char backcolor, char forecolor) {
//Write Unicode char to screen raw
   resetAnsi(0);
   gotoxy(x+1, y+1);
   outputcolor(forecolor, backcolor);
   printf("%lc", ch);  //unicode
}

void write_ch(SCREENCELL *newScreen, int x, int y, wchar_t ch, char backcolor, char forecolor,BOOL raw) {
/* Update cell on screenbuffer. It will be shown on screen when it is updated by calling update_screen */
  int     i, pos;

  SCREENCELL *aux = newScreen;
  resetAnsi(0);
  if (aux != NULL && buffersize <= length(&aux)){

    pos = (y - 1) * sc_columns + x;   //This is the formula to calculate the position index in the screen buffer
    if(pos < buffersize) {
      // If it is within buffer limits, otherwise do nothing.
      for(i = 0; i <= pos; i++) {
        //Run through the buffer until reaching desired position
        if (aux->index == pos)
      break;
        aux = aux->next;
      }
      //Update cell info at position selected.
      //Manage special charaters as well.
	  aux->ch = ch;
          aux->backColor = backcolor;
          aux->foreColor = forecolor;
          aux->toUpdate = 1;

        if(raw==TRUE) {
   		gotoxy(x+1, y);
   		outputcolor(forecolor, backcolor);
   		if (ch>31) printf("%lc", ch);  //unicode
         }
    }
  }
}

SCREENCELL read_cell(SCREENCELL *newScreen, int x, int y) {
/* read specific character with attributes from buffer */
  int     i=0, pos=0;
  SCREENCELL *aux=newScreen;
  SCREENCELL ret ={};
  if (aux != NULL && buffersize <= length(&aux)){
    pos = (y - 1) * sc_columns + x;	//this is the formula to calculate the position index in the screen buffer
   if(pos < buffersize) {
    for(i = 0; i <= pos; i++) {
        //run through the buffer until reaching desired position
        if(aux->index == pos)
	  break;
        aux = aux->next;
    }
    ret.index = aux->index;
    ret.ch = aux->ch;
    //for some unknown reason these values have to be inverted
    ret.backColor = aux->foreColor;
    ret.foreColor = aux->backColor;
    ret.next = NULL;
   }
 }
  return ret;
}


wchar_t read_char(SCREENCELL *newScreen, int x, int y) {
/* read specific character from buffer */
  int     i=0, pos=0;
  wchar_t ch = FILL_CHAR;
  SCREENCELL *aux=newScreen;
  if (aux != NULL && buffersize <= length(&aux)){
    pos = (y - 1) * sc_columns + x;	//this is the formula to calculate the position index in the screen buffer
   if(pos < buffersize) {
    for(i = 0; i <= pos; i++) {
        //run through the buffer until reaching desired position
        if(aux->index == pos)
	  break;
        aux = aux->next;
    }
    ch = aux->ch; 
   }
 }
  return ch;
}

/*------------------------------------------*/
/* Writes a string of characters to buffer. */
/*------------------------------------------*/

void write_str(SCREENCELL *newScreen, int x, int y, char *str, char backcolor, char forecolor, BOOL raw) {
  //Writes a string of characters to buffer.
  char   *astr=NULL;
  size_t     i=0;
  int wherex=0;
  SCREENCELL *aux = newScreen;

  if (aux!=NULL){
    wherex = x;
    astr = str;
    for(i = 0; i <= strlen(str) - 1; i++) {
      write_ch(aux, wherex, y, astr[i], backcolor, forecolor,raw);
      wherex = wherex + 1;
    }
 }
}

/*------------------------------------------*/
/* Writes a string of characters to screen. */
/*------------------------------------------*/

void update_str(int x, int y, char *str, char backcolor, char forecolor) {
  //Writes a string of characters to buffer.
  char   *astr=NULL;
  size_t     i=0;
  int wherex=0;

    resetAnsi(0);
    wherex = x;
    astr = str;
    for(i = 0; i <= strlen(str) - 1; i++) {
      update_ch(wherex, y, astr[i], backcolor, forecolor);
      wherex = wherex + 1;
    }
}

/*-----------------------------------------------*/
/* Writes an integer value as a string on screen */
/*-----------------------------------------------*/

int write_num(SCREENCELL *newScreen, int x, int y, int num, char backcolor,
	       char forecolor, BOOL raw) {
  //the length of the string must be passed on the function
  char   astr[30];
  char len=0;
  SCREENCELL *aux = newScreen;

  if (aux!=NULL){
    sprintf(astr, "%d", num);
    write_str(newScreen, x, y, astr, backcolor, forecolor,raw);
    len = strlen(astr);
  }
  return len;
}

void screen_color(SCREENCELL *newScreen, char bcolor, char fcolor, wchar_t ch) {
/* Changes the color of all the cells to create the effect of changing color in the background */
  int     i=0;
  SCREENCELL *aux=newScreen;
  resetAnsi(0);
  //screen should be updated twice to get round the last row glitch
  if (aux != NULL && buffersize <= length(&aux)){ 
   for(i = 0; i < buffersize; i++) {
      aux->backColor = bcolor;
      aux->foreColor = fcolor;
      aux->ch = ch;
      aux->toUpdate = 1;
      aux = aux->next;
    }
 }
}

/*------------------------------------*/
/* Dumps buffer to screen for display */
/*------------------------------------*/


void dump_screen(SCREENCELL *newScreen) {
/* UPDATES ALL SCREEN CELLS TO DISPLAY */
  int     i=0;
  int     wherex=0, wherey=0;
  SCREENCELL *aux=newScreen;
  if (aux!=NULL && buffersize <= length(&aux)){
  for(i = 0; i < buffersize; i++) {
      update_ch(wherex,wherey,aux->ch,aux->backColor,aux->foreColor); 
      wherex = wherex + 1; //line counter
      if(wherex == sc_columns) {
        //new line
        wherex = 0;
        wherey = wherey + 1;
      }
      aux = aux->next;
    }
  }
}

void update_screen(SCREENCELL *newScreen) {
/* UPDATES ALL SCREEN CELLS TO DISPLAY */
  int     i=0;
  int     wherex=0, wherey=0;
  SCREENCELL *aux=newScreen;
  if (aux!=NULL && buffersize <= length(&aux)){
  for(i = 0; i < buffersize; i++) {
      if (aux->toUpdate ==1) {aux->toUpdate = 0; update_ch(wherex,wherey,aux->ch,aux->backColor,aux->foreColor);} 
      wherex = wherex + 1; //line counter
      if(wherex == sc_columns) {
        //new line
        wherex = 0;
        wherey = wherey + 1;
      }
      aux = aux->next;
    }
  }
}


void xor_update(SCREENCELL *screen1, SCREENCELL *screen2) {
/* UPDATE only the cells that are different */
  int     i=0;
  int     wherex=0, wherey=0;
  SCREENCELL *aux1=screen1;
  SCREENCELL *aux2=screen2;
  if ((aux1!=NULL && aux2!=NULL) && (length(&aux1) == length(&aux2))){
   for(i = 0; i < buffersize; i++) {
     if (aux1->ch != aux2->ch || aux1-> backColor != aux2 -> backColor || aux1-> foreColor != aux2 -> foreColor){ 
       update_ch(wherex,wherey,aux1->ch,aux1->backColor,aux1->foreColor);
      } 
      wherex = wherex + 1; //line counter
      if(wherex == sc_columns) {
        //new line
        wherex = 0;
        wherey = wherey + 1;
      }
      aux1 = aux1->next;
      aux2 = aux2->next;
    }
  }
}


void xor_copy(SCREENCELL *screen1, SCREENCELL *screen2)
{
/* UPDATES ALL SCREEN CELLS TO DISPLAY */
	int i = 0;
	int wherex = 0, wherey = 0;
	SCREENCELL *aux1 = screen1;
	SCREENCELL *aux2 = screen2;
	if ((aux1 != NULL && aux2 != NULL) && (length(&aux1) == length(&aux2))) {
		for (i = 0; i < buffersize; i++) {

			if (aux1->ch != aux2->ch
			    || aux1->backColor != aux2->backColor
			    || aux1->foreColor != aux2->foreColor) {
				aux1->ch = aux2->ch;
				aux1->backColor = aux2->backColor;
				aux1->foreColor = aux2->foreColor;
				wherex = wherex + 1;	//line counter

				if (wherex == sc_columns) {
					//new line
					wherex = 0;
					wherey = wherey + 1;
				}

				aux1 = aux1->next;
				aux2 = aux2->next;
			}
		}
	}
}
/*------------------------------------------*/
/* Draw window area with or without border. */
/*------------------------------------------*/

void draw_window(SCREENCELL *newScreen, int x1, int y1, int x2, int y2, int backcolor, int bordercolor, int titlecolor, BOOL  border, BOOL title, BOOL shadow, BOOL raw) {
/*
   Draw a box on screen
 */
  SCREENCELL *aux = newScreen;
  int     i, j;
  wchar_t ch=FILL_CHAR;
  i = x1;
  j = y1;
  //shadow
  resetAnsi(0);
  if (shadow == TRUE){
  for(j = y1 + 1; j <= y2 + 1; j++)
    for(i = x1 + 1; i <= x2 + 1; i++)
    {
      ch=read_char(aux, i,j); //dynamic shadow
      if ((ch=='\0') || (ch == UNICODEBAR1) || (ch<0)) ch=FILL_CHAR;
      //ch=FILL_CHAR;
      //if (ch<0) ch = (tempLine.linea[i].specialChar << 8) | tempLine.linea[i].ch;
      write_ch(aux, i, j, ch, B_BLACK, F_WHITE,raw);
    }
  }
  //window
  for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
      write_ch(aux, i, j, FILL_CHAR, backcolor, bordercolor,raw);
 
 //borders
  if(border == TRUE) {
    for(i = x1; i <= x2; i++) {
      //upper and lower borders
      write_ch(aux, i, y1, HOR_LINE, backcolor, bordercolor,raw);   //horizontal line box-like char
      write_ch(aux, i, y2, HOR_LINE, backcolor, bordercolor,raw);
    }
    for(j = y1; j <= y2; j++) {
      //left and right borders
      write_ch(aux, x1, j, VER_LINE, backcolor, bordercolor,raw);   //vertical line box-like char
      write_ch(aux, x2, j, VER_LINE, backcolor, bordercolor,raw);
    }
    write_ch(aux, x1, y1, UPPER_LEFT_CORNER, backcolor, bordercolor,raw);   //upper-left corner box-like char
    write_ch(aux, x1, y2, LOWER_LEFT_CORNER, backcolor, bordercolor,raw);   //lower-left corner box-like char
    write_ch(aux, x2, y1, UPPER_RIGHT_CORNER, backcolor, bordercolor,raw);  //upper-right corner box-like char
    write_ch(aux, x2, y2, LOWER_RIGHT_CORNER, backcolor, bordercolor,raw);  //lower-right corner box-like char
  }

  if (title == TRUE) {
    for(i = x1+1; i <= x2-1; i++)
      write_ch(aux, i, y1, ' ', titlecolor, titlecolor,raw);
  }
}


