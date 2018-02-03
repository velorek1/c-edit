#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_cool.h"

#define TRUE 1
#define FALSE 0
typedef int BOOL;
/* 
   Last modified : 31/8/2017
   Coded by Velorek.
   Circular linked list to
   make a selection menu in C.
   OS: Linux.
*/


typedef struct _listchoice
{
  
  int index; // Item number
  int backcolor0; // Back and Fore colors when not selected
  int forecolor0;
  int backcolor1; // Back and Fore colors when selected
  int forecolor1;
  int wherex; // Position of the item
  int wherey;
  char *item; // Item string
  struct _listchoice *next; // Pointer to next item
  struct _listchoice *back; // Pointer to previous item.
} LISTCHOICE;

/* Screen buffer */
typedef struct _bcell
{
  int index; // Item number
  int backcolor0; // Back and Fore colors of each cell
  int forecolor0;
  char item; // Item string
  struct _bcell *next; // Pointer to next item
} BCELL;

LISTCHOICE *head=NULL;
LISTCHOICE *tail=NULL;
LISTCHOICE *former=NULL;
LISTCHOICE *current=NULL;
BCELL *b_start=NULL;
BCELL *b_end=NULL;
BCELL *bo_start=NULL; //old buffer
BCELL *bo_end=NULL; //old buffer
BCELL *screen,*old_screen;

int rows,columns; // This should be global variables
int buffersize;
int bufferON=0; //It keeps track of whether the 2nd buffer is full.

/* Screen buffer routines */
/* ---------------------- */
void hidecursor()
{
  //ansi function hide cursor
  printf("\e[?25l");
}

void showcursor()
{
  //ansi function show cursor
  printf("\e[?25h");
}

void create_screen()
{
  /* 
     Get terminal dimensions and create 2 dynamic lists with the size of the screen.
     if it is not possible to determine screen dimensions default is set to 80x25.
     Insertion happens at the end of the list. There are thre pointers to keep track
     and make insertions. B_start : head of the list | b_end : tail of the list.
     Old buffer works as a double buffer with its own pointers.
  */
  BCELL *new_cell, *newo_cell=NULL;
  int i;
  get_terminal_dimensions(&rows,&columns);
  if (rows==0) rows=25;
  if (columns==0) columns=80;
  buffersize=rows*columns;
  
  // Primary Screen buffer
  if (b_start == NULL && b_end == NULL)
    {
	  screen = (BCELL *) malloc(sizeof(BCELL));
	  screen->backcolor0 = B_BLACK;
	  screen->forecolor0 = F_WHITE;
	  screen->item = 32;
	  screen->next = NULL;
	  screen->index = 0; 	  
	  b_start = screen; // Pointer to first cell
	  b_end = screen; // Pointer to last/previous cell
	  b_end->next = NULL; //Last item of the list next points to NULL
    }
   for (i=1;i<=buffersize-1;i++)
	{
	  new_cell = (BCELL *) malloc(sizeof(BCELL));
	  new_cell->backcolor0 = B_BLACK;
	  new_cell->forecolor0 = F_WHITE;
	  new_cell->item = 32;
	  new_cell->index = i; 	  
	  b_end->next = new_cell;
	  b_end = new_cell;
	  b_end->next = NULL; //Last item of the list next points to NULL
      }
   // Second buffer to be used as an 'undo' buffer
  if (bo_start == NULL && bo_end == NULL)
    {
	  old_screen = (BCELL *) malloc(sizeof(BCELL));
	  old_screen->backcolor0 = B_BLACK;
	  old_screen->forecolor0 = F_WHITE;
	  old_screen->item = 32;
	  old_screen->next = NULL;
	  old_screen->index = 0; 	  
	  bo_start = old_screen; // Pointer to first cell
	  bo_end = old_screen; // Pointer to last/previous cell
	  bo_end->next = NULL; //Last item of the list next points to NULL
    }
   for (i=1;i<=buffersize-1;i++)
	{
	  newo_cell = (BCELL *) malloc(sizeof(BCELL));
	  newo_cell->backcolor0 = B_BLACK;
	  newo_cell->forecolor0 = F_WHITE;
	  newo_cell->item = 32;
	  newo_cell->index = i; 	  
	  bo_end->next = newo_cell;
	  bo_end = newo_cell;
	  bo_end->next = NULL; //Last item of the list next points to NULL
      }

}

void save_buffer()
{
  //Copies buffer1 into buffer 2
  //Save current screen to be retrieved later
  int i;
  BCELL *aux,*aux2;
  bufferON = 1;
  aux=b_start; // Points to the first item of the first buffer
  aux2=bo_start; //Points to the first item of the second buffer 
  for (i=0;i<buffersize;i++)
    {
      //Copy buffer 1 into buffer 2.
      aux2->forecolor0=aux->forecolor0;
      aux2->backcolor0=aux->backcolor0;
      aux2->item=aux->item;
      
      aux=aux->next;
      aux2=aux2->next;
    }
}

void restore_buffer()
{
  //Copies buffer2 into buffer 1
  //Retrieve previous screen
  int i;
  BCELL *aux,*aux2;
  aux=b_start; // Points to the first item of the first buffer
  aux2=bo_start; //Points to the first item of the second buffer 
  bufferON =0;
  for (i=0;i<buffersize;i++)
    {
      //Copy buffer 2 into buffer 1.
      aux->forecolor0=aux2->forecolor0;
      aux->backcolor0=aux2->backcolor0;
      aux->item=aux2->item;

      aux=aux->next;
      aux2=aux2->next;
    }
}

void update_smart()
{
  //Update only the parts that have changed.
  int i,wherex,wherey;
  BCELL *aux,*aux2;
  wherey=1;
  wherex=1;
  aux=b_start;
  aux2=bo_start;
  for (i=0;i<=buffersize;i++)
    {
      if (aux->item!=aux2->item)
	{
	  wherex =aux->index % columns;
	  wherey = (int) (aux->index/columns) +1; 
	  gotoxy(wherex,wherey);
	  outputcolor(aux->forecolor0,aux->backcolor0);
	  printf("%c", aux->item);
	}
      if (aux->backcolor0!=aux2->backcolor0)
	{
	  wherex =aux->index % columns;
	  wherey = (int) (aux->index/columns) +1;
	  gotoxy(wherex,wherey);
	  outputcolor(aux->forecolor0,aux->backcolor0);
	  printf("%c", aux->item);
	}
      if (aux->forecolor0!=aux2->forecolor0)
	{
	  wherex =aux->index % columns;
	  wherey = (int) (aux->index/columns) +1;
	  gotoxy(wherex,wherey);
	  outputcolor(aux->forecolor0,aux->backcolor0);
	  printf("%c", aux->item);
	}
	
      if (aux->next!=NULL) aux=aux->next;
      if (aux2->next!=NULL) aux2=aux2->next;
    }
}

void write_ch(int x,int y,char ch,int backcolor,int forecolor)
{
  /* Writes a char to buffer. It will be shown on screen when it is updated by calling update_screen */
  int i,pos;
  BCELL *aux;
  aux=b_start; // we set our auxiliary pointer at the beginning of the list.
  pos = (y-1) * columns + x; //This is the formula to calculate the position index in the screen buffer

  if (pos<=buffersize) {
    // If it is within buffer limits, otherwise do nothing.  
    for (i=0; i<=pos; i++)
      {
	//Run through the buffer until reaching desired position
	if (aux->index == pos) break;
	aux=aux->next;
      }
    //Update cell info at position selected.
    aux->item=ch;
    aux->backcolor0 = backcolor;
    aux->forecolor0 = forecolor;
  }
}
void write_str(int x,int y,char *str,int backcolor,int forecolor)
{
  //Writes a string of characters to buffer.
  char *astr;
  int i,wherex;
  wherex=x;
  astr = (char *)malloc(strlen(str) +1);
  astr = str;
  for (i=0;i<=strlen(str)-1;i++)
    {
      write_ch(wherex,y,astr[i],backcolor,forecolor);
      wherex=wherex+1;
    }
  
}

void write_num(int x,int y,int num, int length, int backcolor,int forecolor)
{
  //writes an integer value as a string on screen
  //the length of the string must be passed on the function
  char *astr;
  int i,wherex;
  astr = (char *)malloc(length +1);
  sprintf(astr,"%d", num); 
  write_str(x,y,astr,backcolor,forecolor);
}
void update_screen()
{
  //dumps buffer to screen.
  int i,wherex,wherey;
  BCELL *aux;
  wherey=1;
  wherex=1;
  aux=b_start;
  for (i=0;i<=buffersize;i++)
    {
      gotoxy(wherex,wherey);
      wherex=wherex+1; //line counter
      if (aux->next!=NULL) aux=aux->next;
      if (wherex==columns+1)
	{
	  //new line
  	  wherex=1;
	  wherey=wherey+1;
	}
      outputcolor(aux->forecolor0,aux->backcolor0);
      printf("%c", aux->item);
    }
}
void close_window()
{
  //For now it just restores previous buffer
  restore_buffer();
  update_screen();
}

void bscreen_color(int color)
{
  int i;
  BCELL *aux;
  aux=b_start;
  //changes the color of all the cells to create the effect of changing color in the background
  for (i=0;i<buffersize;i++)
    {
      aux->backcolor0=color;
      aux=aux->next;
    }
  update_screen();
}

void free_buffer()
{
  //Destroy the 2 screen buffers
  BCELL *aux, *p;
  aux = b_start;
  do 
   {
     p=aux;
     aux = aux->next;
     free(p); //remove item
   } while (aux->next != NULL);
  aux = bo_start;
  do 
   {
     p=aux;
     aux = aux->next;
     free(p); //remove item
   } while (aux->next != NULL);
  b_start=NULL;
  b_end=NULL;
  bo_start=NULL;
  bo_end=NULL;
}

void draw_window(int x1,int y1,int x2, int y2,int backcolor, int bordercolor,int border)
{
  //Draw window area with or without border.
  int i,j;
  i=x1;
  j=y1;
  save_buffer(); //saves screen to be restored later in close_window
  //shadow
  for (j=y1+1;j<=y2+1;j++)
    for (i=x1+1;i<=x2+1;i++) 
      write_ch(i,j,' ',B_BLACK,F_BLACK);
  //window
  for (j=y1;j<=y2;j++)
    for (i=x1;i<=x2;i++) 
      write_ch(i,j,' ',backcolor,bordercolor);    
  //borders
  if (border==1)
    {
      //with borders. ANSI-ASCII 106-121
      for (i=x1;i<=x2;i++)
	{
	  //upper and lower borders
	  write_ch(i,y1,'-',backcolor,bordercolor);
	  write_ch(i,y2,'-',backcolor,bordercolor);	  
	}
      for (j=y1;j<=y2;j++)
	{
	  //left and right borders
	  write_ch(x1,j,'|',backcolor,bordercolor);
	  write_ch(x2,j,'|',backcolor,bordercolor);	  
	}
     write_ch(x1,y1,'[',backcolor,bordercolor); //upper-left corner
     write_ch(x1,y2,'[',backcolor,bordercolor); //lower-left corner
     write_ch(x2,y1,']',backcolor,bordercolor); //upper-right corner
     write_ch(x2,y2,']',backcolor,bordercolor); //lower-right corner      
    }

  update_smart();
}

/* List choice routines */
void add_item(LISTCHOICE *list_identifier, char *str, int x, int y, int bcolor0, int fcolor0, int bcolor1,int fcolor1)
{
  LISTCHOICE *newp;
  if (head == NULL && list_identifier == NULL) 
    {
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
      list_identifier-> item = (char *)malloc(strlen(str) +1);
      strcpy(list_identifier->item, str);
      list_identifier->next = head;
      list_identifier->back = head;
      list_identifier->index = 0; 
    }
  else
    {
      //Circular list structure
      former = tail; //pointer to previous item
      newp = (LISTCHOICE *) malloc(sizeof(LISTCHOICE));
      former->next = newp; //previous item points to current item
      newp->back = former; //new item back points to previous item
      tail = newp; //tail always points to the last item
      head->back = tail; // This way we close the circular list : First item back points to last item
      tail->next = head; //Last item next points to first item
      
      //Create first item of the list and assign properties to it.
      newp->backcolor0 = bcolor0;
      newp->backcolor1 = bcolor1;
      newp->forecolor0 = fcolor0;
      newp->forecolor1 = fcolor1;
      newp->wherex = x;
      newp->wherey = y;
      newp->item = (char*) malloc(strlen(str) + 1);
      strcpy(newp->item, str);
      newp->index = former->index + 1;
    }
  
}
void free_list(LISTCHOICE *list_identifier)
{
  LISTCHOICE *aux, *p;
  aux = tail;
  do 
   {
     p=aux;
     aux = aux->back;
     free(p->item); //free off the string field
     free(p); //remove item
   } while (aux != head->back);
  head=NULL;
  tail=NULL;
  former=NULL;
  current=NULL;
}

void display_list(LISTCHOICE *list_identifier)
{
/* 
Displays the items contained in the list with the properties specified
in init_list.
until the auxiliary pointer reaches the first item again
First item is equal to tail->next.
*/

  LISTCHOICE *aux;
  aux = head;
  do 
   {
     write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor0,aux->forecolor0);
     aux = aux->next;
   } while (aux != tail->next);
  update_screen();
  		 
}
void move_right(LISTCHOICE *aux)
{
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor0,aux->forecolor0);
      //Highlight current item
      *aux=*aux->back; //Go to next item
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor1,aux->forecolor1);
      update_screen();
}
void move_left(LISTCHOICE *aux)
{
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor0,aux->forecolor0);
      //Highlight current item
      *aux=*aux->next; //Go to next item
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor1,aux->forecolor1);
      update_screen();
}
void move_up(LISTCHOICE *aux)
{
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor0,aux->forecolor0);
      //Highlight current item
      *aux=*aux->back; //Go to next item
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor1,aux->forecolor1);
      update_screen();
}
void move_down(LISTCHOICE *aux)
{
     write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor0,aux->forecolor0);
      //Highlight current item
      *aux=*aux->next; //Go to next item
      write_str(aux->wherex,aux->wherey,aux->item,aux->backcolor1,aux->forecolor1);
      update_screen();
}

char start_vmenu(LISTCHOICE *list_data)
{
  /* Starts the selection menu */
  char ch;
  LISTCHOICE aux;
  int exitwhile=0;
 
  display_list(list_data); //display whole list

  //Highlight first item on the list
  aux=*tail; //Auxiliary variable points to last item
  move_down(&aux); //Move down to point to highlight first item.
  
  ch=0; //init ch value
  while (ch != 10) // escape key or enter
    {
      ch=getch();
      if (ch=='\033') 
	{
	  getch();
	  switch(getch()) 
	    {
	    case 'A': 
	      move_up(&aux);
	      break;
	    case 'B': 
	      move_down(&aux);
	      break;
	    case 'C':
	      ch=-1; // this will be used in top-down menu. Mark for left arrow key
	      exitwhile=1;
	      break;
	    case 'D':
	      ch=-2; // this will be used in top-down menu. Mark for right arrow key
	      exitwhile=1;
	      break;	   
	    }
	}
      if (exitwhile==1) break;
    }
      if (ch==10) 
	{
	  //Pass data of last item by value.
	  *list_data = aux;
	}
      return ch; // return the key or code pressed

}
void start_hmenu(LISTCHOICE *list_data)
{
  /* Starts the selection menu */
  char ch;
  LISTCHOICE aux;
 
  display_list(list_data); //display whole list

  //Highlight first item on the list
  aux=*tail; //Auxiliary variable points to last item
  move_left(&aux); //Move down to point to highlight first item.
  
  ch=0; //init ch value
  while (ch != 10) // escape key or enter
    {
      ch=getch();
      if (ch=='\033') 
	{
	  getch();
	  switch(getch()) 
	    {
	    case 'C': 
	      move_left(&aux);
	      break;
	    case 'D': 
	      move_right(&aux);
	      break;
	    }
	}
    }
      if (ch==10) 
	{
	  //Pass data of last item by value.
	  *list_data = aux;
	}

}
