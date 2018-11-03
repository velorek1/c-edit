/*
========================================================================
- HEADER - 
Module to show user interface windows, textbox, etc.
@author : Velorek
@version : 1.0  
Last modified: 4/11/2018 + New keys                                                                
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

//HELP MESSAGE
#define HELP0 "Here is a list of the most common keys: \n\n"
#define HELP1 "ALT-X: EXIT PROGRAM  |   F1 OR ALT-H: HELP \n"
#define HELP2 "CTRL-L: OPEN MENUS   |   F2: OPEN MENUS \n"
#define HELP3 "ALT-O: OPEN FILE     |   F3: REFRESH SCREEN \n"
#define HELP4 "ALT-N: NEW FILE      |   F4: ABOUT DIALOG \n"
#define HELP5 "ALT-W: OPEN MENUS    |   ALT-A: SAVE AS \n"
#define HELP6 "ALT-D: FILE INFO     |   CTRL-C: EXIT \n"
#define HELP7 "CTRL-H: COLORS       |   ALT-V:  RESERVED \n"
#define HELP8 "ALT-F: RESERVED      |   ALT-D:  RESERVED \n"
#define HELP9 "CTRL-H: RESERVED     |   ALT-K:  RESERVED \n"
#define HELP10 "\nFor more info visit  \n [http://oldstuff286.blogspot.com]"

//END


#define MAX_TEXT 150

//MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define OPT_MENU 1
#define HELP_MENU 2
#define YESNO_MENU 3
#define OK_MENU 4
#define MAX_FILENAME 100

//DROP-DOWN MENUS
#define OPTION_1 0
#define OPTION_2 1
#define OPTION_3 2
#define OPTION_4 3
#define OPTION_5 4
#define OPTION_NIL -1		//Reset option
#define CONFIRMATION 1

//GLOBAL VARIABLES - COLOR SCHEME
extern int EDITAREACOL;
extern int EDIT_FORECOLOR;
extern int STATUSBAR;
extern int STATUSMSG;
extern int MENU_PANEL;
extern int MENU_SELECTOR;
extern int MENU_FOREGROUND0;
extern int MENU_FOREGROUND1;
extern int EDITWINDOW_BACK;
extern int EDITWINDOW_FORE;
extern int SCROLLBAR_BACK;
extern int SCROLLBAR_FORE;
extern int SCROLLBAR_SEL;
extern int SCROLLBAR_ARR;



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
int     helpWindow(LISTCHOICE * mylist, char *message);
void    setColorScheme(int colorCode);
int     colorsWindow(LISTCHOICE * mylist);

#endif
