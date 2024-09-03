/*
========================================================================
- HEADER -
Module with different UI gadgets for LYNX.
@author : Velorek
@version : 1.0
Last modified: 06/02/2022
========================================================================
*/

#ifndef _UI_H_
#define _UI_H_
#define MAX_TEXT 255

#include "scbuf.h"
#include <stdio.h>

int textbox(SCREENCELL *newScreen,int wherex, int wherey, int displayLength,
        char *label, char text[MAX_TEXT], int backcolor,
        int labelcolor, int textcolor, BOOL raw, BOOL locked);

void window(SCREENCELL *screen1, int x1, int y1, int x2, int y2, int backcolor,
         int bordercolor, int titlecolor, int border, int title, int shadow);

int inputWindow(char *label, char *tempFile, char *windowTitle, int offsetX, int offsetY, int length);
#endif


