/*
========================================================================
- HEADER -
Module to handle keyboard input in Linux and create
a further layer of abstraction.
@author : Velorek
@version : 1.0
Last modified: 14/04/2019 Rename Headers
========================================================================
*/

#ifndef _KEYB_H_
#define _KEYB_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "rterm.h"

/*====================================================================*/
/* KEYS - CONSTANTS                                                   */
/*====================================================================*/

#define K_ENTER 13
#define K_CAPS 91
#define K_BACKSPACE 127
#define K_TAB 9
#define K_ESCAPE 27

#define K_F1_TRAIL "\eOP\0\0"
#define K_F1_TRAIL2 "\e[[A\0"
#define K_F2_TRAIL "\eOQ\0\0"
#define K_F2_TRAIL2 "\e[[B\0"
#define K_F3_TRAIL "\eOR\0\0"
#define K_F3_TRAIL2 "\e[[C\0"
#define K_F4_TRAIL "\eOS\0\0"
#define K_F4_TRAIL2 "\e[[D\0"

#define K_UP_TRAIL "\e[A\0\0"
#define K_DOWN_TRAIL "\e[B\0\0"
#define K_RIGHT_TRAIL "\e[C\0\0"
#define K_LEFT_TRAIL "\e[D\0\0"

#define K_PAGEDOWN_TRAIL "\e[6~"
#define K_PAGEUP_TRAIL "\e[5~"
#define K_HOME_TRAIL "\e[H"
#define K_END_TRAIL "\e[F"
#define K_HOME_TRAIL2 "\e[1~"
#define K_END_TRAIL2 "\e[4~"


#define K_DELETE "\e[3~"

#define K_CTRL_A 0x01
#define K_CTRL_B 0x02
#define K_CTRL_C 0x03
#define K_CTRL_D 0x04
#define K_CTRL_E 0x05
#define K_CTRL_J 0x0A
#define K_CTRL_L 0x0C
#define K_CTRL_H 0x08
#define K_CTRL_M 0x0D
#define K_CTRL_N 0x0E
#define K_CTRL_O 0x0F
#define K_CTRL_S 0x14

#define K_ALT_X "\ex"
#define K_ALT_L "\el"
#define K_ALT_S "\es"
#define K_ALT_W "\ew"
#define K_ALT_H "\eh"
#define K_ALT_N "\en"
#define K_ALT_O "\eo"
#define K_ALT_W "\ew"
#define K_ALT_A "\ea"
#define K_ALT_I "\ei"
#define K_ALT_C "\ec"
#define K_ALT_F "\ef"
#define K_ALT_P "\ep"
#define K_ALT_S "\es"
#define K_ALT_V "\ev"
#define K_ALT_D "\ed"
#define ESC3X  "\e\e\e"
#define ESC2X  "\e\e"
#define SPECIAL_CHARS_SET1 -61
#define SPECIAL_CHARS_SET2 -62

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

int     read_keytrail(char chartrail[5]);
int     read_accent(char *ch, char accentchar[2]);

#endif
