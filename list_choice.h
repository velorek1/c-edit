#ifndef _LIST_CHOICE_H_
#define _LIST_CHOICE_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "c_cool.h"

/* Screen buffer */
typedef struct _bcell
{
  int index; // Item number
  int backcolor0; // Back and Fore colors of each cell
  int forecolor0;
  char item; // Item string
  struct _bcell *next; // Pointer to next item
} BCELL;

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

void create_screen();
void write_ch(int x,int y,char ch,int backcolor,int forecolor);
void write_str(int x,int y,char *str,int backcolor,int forecolor);
void write_num(int x,int y,int num, int length, int backcolor,int forecolor);
void showcursor();
void hidecursor();
void save_buffer();
void restore_buffer();
void bscreen_color(int color);
void free_buffer();
void draw_window(int x1,int y1,int x2, int y2,int backcolor, int bordercolor,int border);
void close_window();
void update_screen();
void update_smart();
void add_item(LISTCHOICE *list_identifier, char *str, int x, int y, int bcolor0, int fcolor0, int bcolor1,int fcolor1);
void free_list(LISTCHOICE *list_identifier);
void display_list(LISTCHOICE *list_identifier);
char start_vmenu(LISTCHOICE *list_data);
void start_hmenu(LISTCHOICE *list_data);
void move_up(LISTCHOICE *aux);
void move_down(LISTCHOICE *aux);
void move_left(LISTCHOICE *aux);
void move_right(LISTCHOICE *aux);

#endif
