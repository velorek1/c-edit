/*
========================================================================
- HEADER -
Module with all menus on C·EDIT
@author : Velorek
@version : 1.0
Last modified: 06/04/2024
========================================================================
*/

#ifndef _MENU_H_
#define _MENU_H_

#include "scbuf.h"
#include <stdio.h>

void loadmenus(int choice);
char horizontal_menu();
char filemenu();
char optionsmenu();
char helpmenu();
void credits();
void handlemenus(char *returnMenuChar, int *menuCounter, BOOL horizontalMenu);
int displayAbout();
int displayHelp(void);

#endif


