/* 
======================================================================
Module to manage user interface options like menus,
windows, textbox, etc.

@author : Velorek
@version : 1.0
 
LAST MODIFIED : 05/08/2018
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
#include "list_choice.h"
/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

#define K_ENTER 13
#define K_TAB 9
#define MAX_TEXT 150//MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define OPT_MENU 1
#define HELP_MENU 2
#define YESNO_MENU 3
#define OK_MENU 4
#define OK_MENU2 5
#define MAX_FILENAME 100

//DROP-DOWN MENUS
#define OPTION_1 0
#define OPTION_2 1
#define OPTION_3 2
#define OPTION_4 3
#define OPTION_5 4
#define OPTION_NIL -1 //Reset option
#define CONFIRMATION 1

// DISPLAY CONSTANTS
#define FILL_CHAR 32
#define UP_BOXCHAR -120
#define HOR_BOXCHAR -113
#define UPLEFT_BOXCHAR -108
#define LOWER_LEFT_CORNER -109
#define UPPER_RIGHT_CORNER -107
#define LOWER_RIGHT_CORNER -106


/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*--------------------------------------------*/
/* Load current menu into circular linked list*/ 
/*--------------------------------------------*/

void loadmenus(LISTCHOICE *mylist, int rows, int columns, int choice) {
  if(choice == HOR_MENU) {
    add_item(mylist, "File", 1, 1, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Options", 7, 1, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Help", 16, 1, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }

  if(choice == FILE_MENU) {
    add_item(mylist, "New", 3, 3, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Open", 3, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Save", 3, 5, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Save as..", 3, 6, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Exit", 3, 7, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
  if(choice == OPT_MENU) {
    add_item(mylist, "File Info.", 9, 3, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Find...", 9, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Refresh", 9, 5, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
  if(choice == HELP_MENU) {
    add_item(mylist, "Help...", 18, 3, B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
    add_item(mylist, "About", 18, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }

  if(choice == YESNO_MENU) {
    add_item(mylist, "<YES>", (columns/2)-6, (rows/2)+2, B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
    add_item(mylist, "<NO>", (columns/2)+4, (rows/2)+2, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
   if(choice == OK_MENU) {
    add_item(mylist, "<OK>", (columns/2)-1, (rows/2)+2 , B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
  }
   if(choice == OK_MENU2) {
    add_item(mylist, "<OK>", (columns/2)-1, (rows/2)+3 , B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
  }
  
}

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
       if (keypressed == 0) keypressed = kbhit();
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
resetch();
return charCount;
}

int infoWindow(LISTCHOICE *mylist, int rows, int columns, char *message)
{
   int window_x1=0, window_y1=0, window_x2 = 0, window_y2 =0;
   int ok=0; int i=0; int ix=0; int j=0;
   char tempChar;
   LISTCHOICE data;

   window_y1 = (rows / 2) - 3;
   window_y2 = (rows/2) + 3;
   window_x1 = (columns/2) -14;
   window_x2 = (columns/2) +14;
   loadmenus(mylist, rows, columns, OK_MENU);
   
   draw_window(window_x1, window_y1, window_x2, window_y2, B_WHITE, F_BLACK, 1);
  //Write message to info window  
   for (i=0; i<strlen(message);i++){
     tempChar = message[i];
     //Maximum four lines
     if (j<4) {
       //Split message if \n
       if (tempChar == '\n') {
         j++;
         ix=0;
       } else {
         write_ch(window_x1+1+ix, window_y1+1+j, tempChar, F_BLACK, B_WHITE);
         ix++;
       }
     } 
   }
   start_hmenu(&data);
   free_list(mylist);
   close_window();
   if(data.index == OPTION_1)
       ok = CONFIRMATION; //Confirmation has been given 
   return ok;
}


int alertWindow(LISTCHOICE *mylist, int rows, int columns, char *title, char *message)
{
   int window_x1=0, window_y1=0, window_x2 = 0, window_y2 =0;
   int tempx=0; int titlex=0;int ok=0; int i=0; int ix=0; int j=0;
   char tempChar;
   LISTCHOICE data;

   window_y1 = (rows / 2) - 4;
   window_y2 = (rows/2) + 4;
   window_x1 = (columns/2) -16;
   window_x2 = (columns/2) +16;

   loadmenus(mylist, rows, columns, OK_MENU2);
   
   draw_window(window_x1, window_y1, window_x2, window_y2, B_WHITE, F_BLACK, 1);
   //Window title
   tempx = (window_x2-window_x1) / 2; //Center title
   if (strlen(title) > 0) titlex = tempx - strlen(title) / 2;
   write_str(window_x1+titlex, window_y1+1, title, F_BLACK, B_WHITE);
   update_screen();

    for (i=window_x1+1;i<window_x2;i++){
    //Draw a horizontal line
    write_ch(i,window_y1+2, HOR_BOXCHAR,B_WHITE,F_BLACK);
    }
    //Corners of lines
    write_ch(window_x1,window_y1+2, LOWER_LEFT_CORNER, B_WHITE,F_BLACK);
    write_ch(window_x2,window_y1+2, LOWER_RIGHT_CORNER, B_WHITE,F_BLACK);
   update_screen();

   //Write message to alert window  
   for (i=0; i<strlen(message);i++){
     tempChar = message[i];
     //Maximum four lines
     if (j<4) {
       //Split message if \n
       if (tempChar == '\n') {
         j++;
         ix=0;
       } else {
         write_ch(window_x1+1+ix, window_y1+3+j, tempChar, F_BLACK, B_WHITE);
         ix++;
       }
     } 
   }
   start_hmenu(&data);
   free_list(mylist);
   close_window();
   if(data.index == OPTION_1)
       ok = CONFIRMATION; //Confirmation has been given 
   return ok;
}


int yesnoWindow(LISTCHOICE *mylist, int rows, int columns, char *message)
{

   int window_x1=0, window_y1=0, window_x2 = 0, window_y2 =0;
   int ok=0; int i=0; int ix=0; int j=0;
   char tempChar;
   LISTCHOICE data;
 
    window_y1 = (rows / 2) - 3;
    window_y2 = (rows/2) + 3;
    window_x1 = (columns/2) - 13;
    window_x2 = (columns/2) +13;
    loadmenus(mylist, rows,columns, YESNO_MENU);
    draw_window(window_x1, window_y1, window_x2, window_y2, B_WHITE, F_BLACK, 1);

   //Write message to yes/no window  
   for (i=0; i<strlen(message);i++){
     tempChar = message[i];
     //Maximum four lines
     if (j<4) {
       //Split message if \n
       if (tempChar == '\n') {
         j++;
         ix=0;
       } else {
         write_ch(window_x1+1+ix, window_y1+1+j, tempChar, F_BLACK, B_WHITE);
         ix++;
       }
     } 
   }
    start_hmenu(&data);
    free_list(mylist);
    if(data.index == OPTION_1)
      ok = CONFIRMATION; //Confirmation has been given
    close_window();
  return ok; 
}


int inputWindow(int rows, int columns, char *title, char *label, char *tempFile)
{
   int window_x1=0, window_y1=0, window_x2 = 0, window_y2 =0;
   int i; int tempx=0; int titlex=0; int count=0;

   window_y1 = (rows / 2) - 2;
   window_y2 = (rows/2) + 2;
   window_x1 = (columns/2) -14;
   window_x2 = (columns/2) +14;
  
   draw_window(window_x1, window_y1, window_x2, window_y2, B_WHITE, F_BLACK, 1);
   //Window title
   tempx = (window_x2-window_x1) / 2; //Center title
   if (strlen(title) > 0) titlex = tempx - strlen(title) / 2;
   write_str(window_x1+titlex, window_y1+1, title, F_BLACK, B_WHITE);
   update_screen();

    for (i=window_x1+1;i<window_x2;i++){
    //Draw a horizontal line
    write_ch(i,window_y1+2, HOR_BOXCHAR,B_WHITE,F_BLACK);
    }
    //Corners of lines
    write_ch(window_x1,window_y1+2, LOWER_LEFT_CORNER, B_WHITE,F_BLACK);
    write_ch(window_x2,window_y1+2, LOWER_RIGHT_CORNER, B_WHITE,F_BLACK);
    update_screen();
    count = textbox(window_x1+1,window_y1+3, 12, label, tempFile, B_WHITE, F_BLACK, F_BLACK); 
    close_window();
    return count;
}





