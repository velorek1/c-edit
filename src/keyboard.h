/*
========================================================================
- HEADER - 
Module to handle keyboard input in Linux and create
a further layer of abstraction.
@author : Velorek
@version : 1.0  
Last modified: 20/10/2018                                                                
========================================================================
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "c_cool.h"
//#include "screen_buffer.h"


/*====================================================================*/
/* KEYS - CONSTANTS                                                   */
/*====================================================================*/

#define K_ENTER 13
#define K_CAPS 91
#define K_BACKSPACE 127
#define K_TAB 9
#define K_ESCAPE 27
#define K_F2_TRAIL "\eOQ\0\0"
#define K_F2_TRAIL2 "\e[[B\0"
#define K_UP_TRAIL "\e[A\0\0"
#define K_DOWN_TRAIL "\e[B\0\0"
#define K_RIGHT_TRAIL "\e[C\0\0"
#define K_LEFT_TRAIL "\e[D\0\0"
#define K_F3_TRAIL "\eOR\0\0"
#define K_F3_TRAIL2 "\e[[C\0"

#define K_CTRL_C 0x03
#define K_CTRL_D 0x04
#define K_CTRL_J 0x0A
#define K_CTRL_L 0x0C

#define SPECIAL_CHARS_SET1 -61
#define SPECIAL_CHARS_SET2 -62

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

int read_keytrail(char chartrail[5]);
int read_accent(char *ch, char accentchar[2]);

#endif

