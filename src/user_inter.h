/*
========================================================================
- HEADER - 
Module to show user interface windows, textbox, etc.
@author : Velorek
@version : 1.0  
Last modified: 04/08/2018                                                                
========================================================================
*/

#ifndef _USER_INTER_H_
#define _USER_INTER_H_

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
#define OPTION_NIL -1 //Reset option
#define CONFIRMATION 1

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/


/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/

void loadmenus(LISTCHOICE *mylist, int rows, int columns, int choice);

int textbox(int wherex, int wherey, int displayLength, char label[MAX_TEXT], 
    char text[MAX_TEXT], int backcolor, int labelcolor, int textcolor);
int alertWindow(LISTCHOICE *mylist, int rows, int columns,char *title, char *message);
int infoWindow(LISTCHOICE *mylist, int rows, int columns, char *message);
int inputWindow(int rows, int columns, char *title, char *label, char *tempFile);
int yesnoWindow(LISTCHOICE *mylist, int rows, int columns, char *message);
#endif
