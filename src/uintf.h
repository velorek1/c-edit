/*
========================================================================
- HEADER -
Module to show user interface windows, textbox, etc.
@author : Velorek
@version : 1.0
Last modified: 14/04/2019 Rename headers

========================================================================
*/

#ifndef _UINTF_H_
#define _UINTF_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "scbuf.h"
#include "listc.h"
#include "keyb.h"

//HELP MESSAGE
#define HELP1 "ALT-X: EXIT PROGRAM  |   F1: HELP \n"
#define HELP2 "CTRL-L: OPEN MENUS   |   F2: OPEN MENUS \n"
#define HELP3 "ALT-O: OPEN FILE     |   F3: REFRESH SCREEN \n"
#define HELP4 "ALT-N: NEW FILE      |   F4: ABOUT DIALOG \n"
#define HELP5 "ALT-W: SAVE FILE     |   ALT-A: SAVE AS \n"
#define HELP6 "ALT-D: FILE INFO     |   CTRL-C: EXIT \n"
#define HELP7 "CTRL-H: COLORS       |   ALT-P:  OPTIONS MENU \n"
#define HELP8 "ALT-F: FILE MENU     |   ALT-H:  HELP MENU \n"
#define HELP9 "CTRL-H: COLORS       |   ALT-K:  RESERVED \n"
#define HELP10 "\nFor more info visit  \n [http://oldstuff286.blogspot.com]"

#define HELPWTITLE "[-] HELP: MOST COMMON KEYS"
#define ABOUTWTITLE "[-] ABOUT INFORMATION"
#define READMODEWTITLE "[-] ALERT INFORMATION"
#define INFOWTITLE "[-] C-EDIT INFO"
#define NOFILEOPENWTITLE "[-] C-EDIT INFO"
#define NEWFILEWTITLE "[-] NEW FILE INPUT"
#define COLORSWTITLE "[-] SET COLOR"
#define OPENWTITLE "[-] SELECT FILE"
#define SAVEWTITLE "[-] SAVE FILE AS"
#define FILESAVEDWTITLE "[-] C-EDIT INFO"
#define CREATEWTITLE "[-] NEW FILE INPUT"
#define CONFIRMWTITLE "[-] C-EDIT INFO"
#define FILETOBIGWTITLE "[-] C-EDIT INFO"
#define OPENANYWAYWTITLE "[-] C-EDIT INFO"
#define EXISTSWTITLE "[-] C-EDIT INFO"
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
#define OPTION_NIL -1       //Reset option
#define CONFIRMATION 1

//EDIT CONSTANTS
#define CHAR_NIL '\0'
#define END_LINE_CHAR 0x0A  // $0A
#define FILL_CHAR 32

//GLOBAL VARIABLES - COLOR SCHEME
extern int EDITAREACOL;
extern int EDIT_FORECOLOR;
extern int STATUSBAR;
extern int STATUSMSG;
extern int MENU_PANEL;
extern int MENU2_PANEL;
extern int MENU_SELECTOR;
extern int MENU_FOREGROUND0;
extern int MENU2_FOREGROUND0;
extern int MENU_FOREGROUND1;
extern int EDITWINDOW_BACK;
extern int EDITWINDOW_FORE;
extern int SCROLLBAR_BACK;
extern int SCROLLBAR_FORE;
extern int SCROLLBAR_SEL;
extern int SCROLLBAR_ARR;
extern int WINDOW_TITLEB; // to be accessible from opfile.c
extern int WINDOW_TITLEF;

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
int     alertWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
int     infoWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
int     inputWindow(char *label, char *tempFile, char *windowTitle);
int     yesnoWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
int     helpWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
void    setColorScheme(int colorCode);
int     colorsWindow(LISTCHOICE * mylist, char *windowTitle);

#endif
