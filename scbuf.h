#ifndef _SCBUF_H_
#define _SCBUF_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include "rterm.h"

#define HOR_LINE 9472
#define VER_LINE 9474
#define UPPER_LEFT_CORNER 9484
#define LOWER_LEFT_CORNER 9492
#define UPPER_RIGHT_CORNER 9488
#define LOWER_RIGHT_CORNER 9496
#define UNICODEBAR1 0x2592

typedef struct _screencell 
{ 
	int index;
   	char backColor;
        char foreColor;
        unsigned char toUpdate;
        //int char attrib; 
        wchar_t ch;
	struct _screencell *next;
} SCREENCELL;


/* Adapted from Kernighan and Pike's "The Practice of Programming"  pp.46 et 
seq. (Addison-Wesley 1999) - computerPhile */

// create new list element of type SCREENCELL from the supplied text string
SCREENCELL *newelement(SCREENCELL temp);
SCREENCELL *addfront(SCREENCELL *head, SCREENCELL *newp);
SCREENCELL *addend (SCREENCELL *head, SCREENCELL *newp);
void deleteList(SCREENCELL **head);
int length(SCREENCELL **head);
void reindex(SCREENCELL **head);
//SCREEN BUFFER ROUTINES
int create_screen(SCREENCELL **newScreen);
void update_ch(int x, int y, wchar_t ch, char backcolor, char forecolor);
void update_screen(SCREENCELL *newScreen);
void dump_screen(SCREENCELL *newScreen);
void write_ch(SCREENCELL *newScreen, int x, int y, wchar_t ch, char backcolor, char forecolor, BOOL raw);
wchar_t read_char(SCREENCELL *newScreen, int x, int y);
SCREENCELL read_cell(SCREENCELL *newScreen, int x, int y);
void write_str(SCREENCELL *newScreen, int x, int y, char *str, char backcolor, char forecolor, BOOL raw);
void update_str(int x, int y, char *str, char backcolor, char forecolor);
int write_num(SCREENCELL *newScreen, int x, int y, int num, char backcolor,
	       char forecolor, BOOL raw);
void screen_color(SCREENCELL *newScreen, char bcolor, char fcolor, wchar_t ch);
void copy_screen(SCREENCELL *destination,SCREENCELL *source);
void xor_update(SCREENCELL *screen1, SCREENCELL *screen2);
void xor_copy(SCREENCELL *screen1, SCREENCELL *screen2);
void draw_window(SCREENCELL *newScreen, int x1, int y1, int x2, int y2, int backcolor, int bordercolor, int titlecolor, BOOL border, BOOL title, BOOL shadow, BOOL raw);
#endif
