/*
========================================================================
- HEADER - 
Module to show user interface windows, textbox, etc.
@author : Velorek
@version : 1.0  
Last modified: 02/08/2018                                                                
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

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/


/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/


int textbox(int wherex, int wherey, int displayLength, char label[MAX_TEXT], 
    char text[MAX_TEXT], int backcolor, int labelcolor, int textcolor);

#endif
