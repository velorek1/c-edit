/* 
======================================================================
Module to manage user interface options like windows, textbox, etc.

@author : Velorek
@version : 1.0
 
LAST MODIFIED : 02/08/2018
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "screen_buffer.h"
#include "c_cool.h"

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

#define K_ENTER 13
#define K_TAB 9

/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/


/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/


int textbox(int wherex, int wherey, int displayLength, char label[MAX_TEXT], 
    char text[MAX_TEXT], int backcolor, int labelcolor, int textcolor){
int charCount=0;
int exitFlag = 0;
int cursorON = 1;
long cursorCount = 0;
int i;
int positionx=0;
int keypressed=0;

char ch;
     positionx = wherex+strlen(label);
     write_str(wherex,wherey,label,backcolor,labelcolor);
     write_ch(positionx,wherey, '[', backcolor, textcolor);
     for (i=positionx+1; i<=positionx+displayLength; i++){
        write_ch(i,wherey, '.', backcolor, textcolor);
     }
     write_ch(positionx+displayLength+1,wherey, ']', backcolor, textcolor);
     update_screen();
    if (kbhit() == 1) ch=readch();
    ch=0;
    do{
       keypressed = kbhit();
   //Cursor Animation
      cursorCount++;

      if (cursorCount == 100){ 
        cursorCount = 0;
          switch (cursorON){
            case 1:
              write_ch(positionx+1,wherey, '.', backcolor, textcolor);
              update_screen();
              cursorON=0;
              break;
            case 0:
              write_ch(positionx+1,wherey, '|', backcolor, textcolor);
              update_screen();
              cursorON=1;
              break;
         }
           //update_screen();
       }

   //Process keys     
   if (keypressed == 1){
     ch=readch();
     keypressed = 0;
       if (charCount < displayLength){
         if (ch>31 && ch<127){
             write_ch(positionx+1, wherey, ch, backcolor, textcolor);
             text[charCount] = ch; 
             positionx++;
             charCount++;
             update_screen();
          }
       }
      }
       if (ch==K_ENTER || ch==K_TAB) exitFlag = 1;
    
      //ENTER OR TAB FINISH LOOP
     } while (exitFlag != 1);
return charCount;
}


