/*
======================================================================
HEADER: Module to create a double screen buffer to control how things
are displayed on the terminal.

@author : Velorek
@version : 1.0
Last modified : 14/4/2019 Rename headers + screenChanged added
======================================================================
*/

#ifndef _SCBUF_H_
#define _SCBUF_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>      /* wint_t */
#include "rterm.h"
#include "keyb.h"


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

/*====================================================================*/
/* TYPE DEFINITIONS                                                   */
/*====================================================================*/

typedef struct _screencell {
  int     index;        // Item number
  int     backcolor0;       // Back and Fore colors of each cell
  int     forecolor0;
  int     sc_rows;          //Save screen dimensions
  int     sc_columns;
  char    item;         // Item string
  int     specialchar;      // Control for accents and special chars
  struct _screencell *next; // Pointer to next item
} SCREENCELL;

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

void    create_screen();
void    write_ch(int x, int y, char ch, int backcolor, int forecolor);
char    read_char(int x, int y);
void    write_str(int x, int y, char *str, int backcolor, int forecolor);
int     write_num(int x, int y, int num, int length, int backcolor,
          int forecolor);
void    save_buffer();
void    restore_buffer();
void    screen_color(int color);
void    free_buffer();
void    draw_window(int x1, int y1, int x2, int y2, int backcolor,
            int bordercolor, int titlecolor, int border, int title);
void    close_window();
int     mapChartoU8(int character);
void    update_screen();
void    update_ch(int x, int y, char ch, char specialChar, int backcolor, int forecolor);
int     screenChanged();
int     update_smart();
void    flush_buffer();
void    flush_cell(int x, int y);
void    clearString(char *string, int max);

#endif
