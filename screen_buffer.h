/* 

- HEADER - Module to create a double screen buffer to control how things are displayed on the terminal.

Simple linked list with a "cell" that stores the character to be shown along with
its background and foreground colors.
The size of our buffer will be determined by rows x columns (simple linked list). 
This representation in memory of the screen will need subroutines to control the display. 
Everything you want to show on screen will be printed to the buffer first. 
Then the update routine will finally show the final composition to the user.

@author : Velorek
@version : 1.0
Last modified : 04/3/2018

*/

#ifndef _SCREEN_BUFFER_H_
# define _SCREEN_BUFFER_H_

# include <stdio.h>
# include <stdlib.h>
# include <strings.h>
# include "c_cool.h"

typedef struct _bcell {
  int     index;		// Item number
  int     backcolor0;		// Back and Fore colors of each cell
  int     forecolor0;
  char    item;			// Item string
  struct _bcell *next;		// Pointer to next item
} BCELL;

void    create_screen();
void    write_ch(int x, int y, char ch, int backcolor, int forecolor);
void    write_str(int x, int y, char *str, int backcolor, int forecolor);
void    write_num(int x, int y, int num, int length, int backcolor,
		  int forecolor);
void    save_buffer();
void    restore_buffer();
void    bscreen_color(int color);
void    free_buffer();
void    draw_window(int x1, int y1, int x2, int y2, int backcolor,
		    int bordercolor, int border);
void    close_window();
void    update_screen();
void    update_smart();

#endif
