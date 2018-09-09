/*
========================================================================
- HEADER - 
Module to show user interface windows, textbox, etc.
@author : Velorek
@version : 1.0  
Last modified: 09/08/2018                                                                
========================================================================
*/

#ifndef _USER_INTER_H_
# define _USER_INTER_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "screen_buffer.h"
#include "c_cool.h"
#include "list_choice.h"

# define MAX_TEXT 150

//MENU CONSTANTS
# define HOR_MENU -1
# define FILE_MENU 0
# define OPT_MENU 1
# define HELP_MENU 2
# define YESNO_MENU 3
# define OK_MENU 4
# define MAX_FILENAME 100

//DROP-DOWN MENUS
# define OPTION_1 0
# define OPTION_2 1
# define OPTION_3 2
# define OPTION_4 3
# define OPTION_5 4
# define OPTION_NIL -1		//Reset option
# define CONFIRMATION 1

// COLOR SCHEME - THEME 1

#define EDITAREACOL B_BLUE
#define EDIT_FORECOLOR FH_WHITE
#define STATUSBAR B_CYAN
#define STATUSMSG F_WHITE
#define MENU_PANEL B_WHITE
#define MENU_SELECTOR B_BLACK
#define MENU_FOREGROUND0 F_BLACK
#define MENU_FOREGROUND1 F_WHITE
#define EDITWINDOW_BACK B_BLACK
#define EDITWINDOW_FORE F_WHITE
#define SCROLLBAR_BACK B_WHITE
#define SCROLLBAR_FORE F_WHITE
#define SCROLLBAR_SEL B_CYAN
#define SCROLLBAR_ARR B_BLACK


// COLOR SCHEME - THEME 2
/*
#define EDITAREACOL B_BLUE
#define EDIT_FORECOLOR FH_WHITE
#define STATUSBAR B_CYAN
#define STATUSMSG F_WHITE
#define MENU_PANEL B_WHITE
#define MENU_SELECTOR B_CYAN
#define MENU_FOREGROUND0 F_BLACK
#define MENU_FOREGROUND1 F_WHITE
#define EDITWINDOW_BACK B_BLUE
#define EDITWINDOW_FORE F_WHITE
#define SCROLLBAR_BACK B_WHITE
#define SCROLLBAR_FORE F_WHITE
#define SCROLLBAR_SEL B_CYAN
#define SCROLLBAR_ARR B_BLACK
*/


// COLOR SCHEME - THEME 3
/*
#define EDITAREACOL B_BLACK
#define EDIT_FORECOLOR FH_YELLOW
#define STATUSBAR B_BLUE
#define STATUSMSG F_WHITE
#define MENU_PANEL B_BLUE
#define MENU_SELECTOR B_CYAN
#define MENU_FOREGROUND0 F_WHITE
#define MENU_FOREGROUND1 FH_WHITE
#define EDITWINDOW_BACK B_BLACK
#define EDITWINDOW_FORE F_WHITE
#define SCROLLBAR_BACK B_WHITE
#define SCROLLBAR_FORE F_BLACK
#define SCROLLBAR_SEL B_BLUE
#define SCROLLBAR_ARR B_WHITE
*/


/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/

void    loadmenus(LISTCHOICE * mylist, int choice);

int     textbox(int wherex, int wherey, int displayLength,
		char label[MAX_TEXT], char text[MAX_TEXT], int backcolor,
		int labelcolor, int textcolor);
int     alertWindow(LISTCHOICE * mylist, char *title, char *message);
int     infoWindow(LISTCHOICE * mylist, char *message);
int     inputWindow(char *title, char *label, char *tempFile);
int     yesnoWindow(LISTCHOICE * mylist, char *message);
#endif
