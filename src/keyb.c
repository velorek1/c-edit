/*
======================================================================
Module to control keyboard input.

@author : Velorek
@version : 1.0

LAST MODIFIED : 14/04/2019 Rename headers
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "rterm.h"
#include "keyb.h"

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*----------------------------------*/
/* Read ESC-key char with its trail */
/*----------------------------------*/

int read_keytrail(char chartrail[5]){
/*
   New implementation: Trail of chars found in keyboard.c
   If K_ESCAPE is captured read a trail up to 5 characters from the console.
   This is to control the fact that some keys may change
   according to the terminal and expand the editor's possibilities.
   Eg: F2 can be either 27 79 81 or 27 91 91 82.
*/
char ch;
int i;
   chartrail[0] = K_ESCAPE;
   for(i = 1; i < 5; i++) {
     if(kbhit() == 1) {
        ch=readch();
        chartrail[i] = ch;
     } else {
        chartrail[i] = 0;
     }
   }
   resetch();
   return 1;
}

/*----------------------------------*/
/* Read Accents and Special Chars   */
/*----------------------------------*/


int read_accent(char *ch, char accentchar[2])
{
/*
Input Ref: ch, accentchar
@return : 1 - SET 1 | 2 - SET 2 | 0 - NO ACCENT CHAR
*/
  int result;
  result = 0;
    //Accents and special chars
   accentchar[0] = 0;
   accentchar[1] = *ch;
    if(*ch == SPECIAL_CHARS_SET1) {
      accentchar[0] = SPECIAL_CHARS_SET1;   //Accents and special chars SET1
      accentchar[1] = readch();
      result = 1;
      resetch();
    }
    if(*ch == SPECIAL_CHARS_SET2) {
      accentchar[0] = SPECIAL_CHARS_SET2;   //Accents and special chars SET2
      accentchar[1] = readch();
      result = 2;
      resetch();
    }
    return result;
}
