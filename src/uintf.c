/*
======================================================================
Module to manage user interface options like menus,
windows, textbox, etc.

@author : Velorek
@version : 1.0

LAST MODIFIED : 14/04/2019 - Rename headers
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "scbuf.h"
#include "listc.h"
#include "uintf.h"
#include "keyb.h"

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

#define K_ENTER 13
#define K_TAB 9
#define MAX_TEXT 150        //MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define OPT_MENU 1
#define HELP_MENU 2
#define YESNO_MENU 3
#define OK_MENU 4
#define OK_MENU2 5
#define COLORS_MENU 6
#define MAX_FILENAME 100

//DROP-DOWN MENUS
#define OPTION_1 0
#define OPTION_2 1
#define OPTION_3 2
#define OPTION_4 3
#define OPTION_5 4
#define OPTION_NIL -1       //Reset option
#define CONFIRMATION 1
#define CANCEL 2

// DISPLAY CONSTANTS
#define FILL_CHAR 32


/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/
//C-Edit Theme
int EDITAREACOL= B_BLUE;
int EDIT_FORECOLOR=F_WHITE;
int STATUSBAR =B_BLACK;
int STATUSMSG =F_WHITE;
int MENU_PANEL =B_WHITE;
int MENU2_PANEL = B_BLACK;
int MENU_SELECTOR =B_RED;
int MENU_FOREGROUND0 =F_BLACK;
int MENU2_FOREGROUND0 = F_WHITE;
int MENU_FOREGROUND1= F_WHITE;
int EDITWINDOW_BACK= B_BLUE;
int EDITWINDOW_FORE= F_WHITE;
int SCROLLBAR_BACK= B_WHITE;
int SCROLLBAR_FORE= F_WHITE;
int SCROLLBAR_SEL= B_CYAN;
int SCROLLBAR_ARR= B_BLACK;
int WINDOW_TITLEB = B_BLACK;
int WINDOW_TITLEF = F_WHITE;

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*--------------------------------------------*/
/* Load current menu into circular linked list*/
/*--------------------------------------------*/

void loadmenus(LISTCHOICE * mylist, int choice) {
  int     rows, columns;
  get_terminal_dimensions(&rows, &columns);

  if(choice == HOR_MENU) {
    add_item(mylist, "File", 1, 1, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Options", 7, 1, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Help", 16, 1, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }

  if(choice == FILE_MENU) {
    add_item(mylist, "New", 3, 3, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Open", 3, 4, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Save", 3, 5, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Save as..", 3, 6,MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Exit", 3, 7, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OPT_MENU) {
    add_item(mylist, "File Info.", 9, 3, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Find...", 9, 4, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Colors", 9, 5, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == HELP_MENU) {
    add_item(mylist, "Help...", 18, 3, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "About", 18, 4, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }

  if(choice == YESNO_MENU) {
    add_item(mylist, "[YES]", (columns / 2) - 11, (rows / 2) + 2, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "[NO]", (columns / 2) - 3, (rows / 2) + 2,  MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "[CANCEL]", (columns / 2) + 4, (rows / 2) + 2,  MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OK_MENU) {
    add_item(mylist, "[OK]", (columns / 2) - 1, (rows / 2) + 2, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OK_MENU2) {
    add_item(mylist, "[OK]", (columns / 2) - 1, (rows / 2) + 3, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == COLORS_MENU) {
    add_item(mylist, "C-Edit Theme", (columns / 2) - 6, (rows / 2) - 2,
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Classic Theme", (columns / 2) - 6, (rows / 2) -1,
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Dark Theme", (columns / 2) - 6, (rows / 2),
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }

}

/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/

int textbox(int wherex, int wherey, int displayLength,
        char label[MAX_TEXT], char text[MAX_TEXT], int backcolor,
        int labelcolor, int textcolor) {
  int     charCount = 0;
  int     exitFlag = 0;
  int     cursorON = 1;
  long    cursorCount = 0;
  int     i;
  int     limitCursor = 0;
  int     positionx = 0;
  int     posCursor = 0;
  int     keypressed = 0;
  char    chartrail[5];
 // char    accentchar[2];
  char    displayChar;
  char    ch;
  positionx = wherex + strlen(label);
  limitCursor = wherex+strlen(label)+displayLength+1;
  write_str(wherex, wherey, label, backcolor, labelcolor);
  write_ch(positionx, wherey, '[', backcolor, textcolor);
  for(i = positionx + 1; i <= positionx + displayLength; i++) {
    write_ch(i, wherey, '.', backcolor, textcolor);
  }
  write_ch(positionx + displayLength + 1, wherey, ']', backcolor,
       textcolor);
  update_screen();
  //Reset keyboard
  if(kbhit() == 1) ch = readch();
  ch = 0;

  do {
      keypressed = kbhit();
    //Cursor Animation
   if (keypressed == 0){
    cursorCount++;
    if(cursorCount == 10000) {
      cursorCount = 0;
      switch (cursorON) {
    case 1:
      posCursor = positionx + 1;
          displayChar = '.';
          if (posCursor == limitCursor) {
            posCursor = posCursor - 1;
            displayChar = ch;
          }
          write_ch(posCursor, wherey, displayChar, backcolor, textcolor);
          update_screen();
          cursorON = 0;
      break;
    case 0:
          posCursor = positionx + 1;
          if (posCursor == limitCursor) posCursor = posCursor - 1;
      write_ch(posCursor, wherey, '|', backcolor, textcolor);
          update_screen();
          cursorON = 1;
      break;
      }
     }
    }
    //Process keys
    if(keypressed == 1) {
      ch = readch();
      keypressed = 0;

      //Read special keys
      if (ch==K_ESCAPE) {
               read_keytrail(chartrail);
      }
      //Read accents
      //if (ch==SPECIAL_CHARSET_1) read_accentchar(&ch, accentchar);
      //if (ch==SPECIAL_CHARSET_2) read_accentchar(&ch, accentchar);

      if(charCount < displayLength) {
    if(ch > 31 && ch < 127) {
      write_ch(positionx + 1, wherey, ch, backcolor, textcolor);
      text[charCount] = ch;
      positionx++;
      charCount++;
      update_screen();
    }
      }
    }
    if (ch==K_BACKSPACE){
      if (positionx>0 && charCount>0){
       positionx--;
       charCount--;
       text[charCount] = '\0';
       write_ch(positionx + 1, wherey, '.', backcolor, textcolor);
       if (positionx < limitCursor-2) write_ch(positionx + 2, wherey, '.', backcolor, textcolor);
       update_screen();
       resetch();
      }
    }
    if(ch == K_ENTER || ch == K_TAB)
      exitFlag = 1;

    //ENTER OR TAB FINISH LOOP
  } while(exitFlag != 1);
  text[charCount] = '\0';
  resetch();
  return charCount;
}

int infoWindow(LISTCHOICE * mylist, char *message, char *windowTitle) {
  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  int     ok = 0;
  size_t   i = 0;
  int     ix = 0;
  int     j = 0;
  char    tempChar;
  LISTCHOICE data;
  int     rows, columns;

  get_terminal_dimensions(&rows, &columns);

  window_y1 = (rows / 2) - 3;
  window_y2 = (rows / 2) + 3;
  window_x1 = (columns / 2) - 14;
  window_x2 = (columns / 2) + 14;
  loadmenus(mylist, OK_MENU);

  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,
          WINDOW_TITLEB,1,1);
  write_str((window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF);
  //Write message to info window
  for(i = 0; i < strlen(message); i++) {
    tempChar = message[i];
    //Maximum four lines
    if(j < 4) {
      //Split message if \n
      if(tempChar == '\n') {
    j++;
    ix = 0;
      } else {
    write_ch(window_x1 + 1 + ix, window_y1 + 1 + j, tempChar, MENU_FOREGROUND0,
         MENU_PANEL);
    ix++;
      }
    }
  }
  start_hmenu(&data);
  free_list(mylist);
  close_window();
  if(data.index == OPTION_1)
    ok = CONFIRMATION;      //Confirmation has been given
  return ok;
}

int alertWindow(LISTCHOICE * mylist, char *message, char *windowTitle) {
  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  int     ok = 0;
  size_t     i = 0;
  int     ix = 0;
  int     j = 0;
  char    tempChar;
  LISTCHOICE data;
  int     rows, columns;
  get_terminal_dimensions(&rows, &columns);

  window_y1 = (rows / 2) - 4;
  window_y2 = (rows / 2) + 4;
  window_x1 = (columns / 2) - 16;
  window_x2 = (columns / 2) + 16;

  loadmenus(mylist, OK_MENU2);

  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,
          WINDOW_TITLEB,1,1);
  write_str((window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF);
  //Write message to alert window
  for(i = 0; i < strlen(message); i++) {
    tempChar = message[i];
    //Maximum four lines
    if(j < 4) {
      //Split message if \n
      if(tempChar == '\n') {
    j++;
    ix = 0;
      } else {
    write_ch(window_x1 + 1 + ix, window_y1 + 1 + j, tempChar, MENU_FOREGROUND0,
         MENU_PANEL);
    ix++;
      }
    }
  }
  start_hmenu(&data);
  free_list(mylist);
  close_window();
  if(data.index == OPTION_1)
    ok = CONFIRMATION;      //Confirmation has been given
  return ok;
}

int yesnoWindow(LISTCHOICE * mylist, char *message, char *windowTitle) {

  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  int     ok = 0;
  size_t     i = 0;
  int     ix = 0;
  int     j = 0;
  char    tempChar;
  LISTCHOICE data;
  int     rows, columns;

  get_terminal_dimensions(&rows, &columns);

  window_y1 = (rows / 2) - 3;
  window_y2 = (rows / 2) + 3;
  window_x1 = (columns / 2) - 13;
  window_x2 = (columns / 2) + 13;
  loadmenus(mylist, YESNO_MENU);
  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,
          WINDOW_TITLEB,1,1);
  write_str((window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF);
  //Write message to yes/no window
  for(i = 0; i < strlen(message); i++) {
    tempChar = message[i];
    //Maximum four lines
    if(j < 4) {
      //Split message if \n
      if(tempChar == '\n') {
    j++;
    ix = 0;
      } else {
    write_ch(window_x1 + 1 + ix, window_y1 + 1 + j, tempChar, MENU_FOREGROUND0,
         MENU_PANEL);
    ix++;
      }
    }
  }
  start_hmenu(&data);
  free_list(mylist);
  if(data.index == OPTION_1)
    ok = CONFIRMATION;      //Confirmation has been given
  if(data.index == OPTION_3)
    ok = CANCEL;
  close_window();
  return ok;
}

int inputWindow(char *label, char *tempFile, char *windowTitle) {
  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  int     count = 0;
  int     rows, columns;

  get_terminal_dimensions(&rows, &columns);

  window_y1 = (rows / 2) - 2;
  window_y2 = (rows / 2);
  window_x1 = (columns / 2) - 14;
  window_x2 = (columns / 2) + 14;

  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,
          WINDOW_TITLEB,1,1);
  write_str((window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF);
  count =
      textbox(window_x1 + 1, window_y1 + 1, 16, label, tempFile, MENU_PANEL,
          MENU_FOREGROUND0, MENU_FOREGROUND0);
  close_window();
  return count;
}

void setColorScheme(int colorCode)
{

  switch (colorCode){

    case 0: //C-edit Blue Theme
      EDITAREACOL= B_BLUE;
      EDIT_FORECOLOR=F_WHITE;
      STATUSBAR =B_BLACK;
      STATUSMSG =F_WHITE;
      MENU_PANEL =B_WHITE;
      MENU2_PANEL =B_BLACK;
      MENU_SELECTOR = B_RED;
      MENU_FOREGROUND0 =F_BLACK;
      MENU2_FOREGROUND0 =F_WHITE;
      MENU_FOREGROUND1= F_WHITE;
      EDITWINDOW_BACK= B_BLUE;
      EDITWINDOW_FORE= F_WHITE;
      SCROLLBAR_BACK= B_WHITE;
      SCROLLBAR_FORE= F_WHITE;
      SCROLLBAR_SEL= B_CYAN;
      SCROLLBAR_ARR= B_BLACK;
      WINDOW_TITLEB = B_BLACK;
      WINDOW_TITLEF = F_WHITE;
    break;
    case 1: //Classic color scheme
      EDITAREACOL= B_BLUE;
      EDIT_FORECOLOR=F_WHITE;
      STATUSBAR =B_CYAN;
      STATUSMSG =F_BLACK;
      MENU_PANEL =B_WHITE;
      MENU2_PANEL =B_WHITE;
      MENU_SELECTOR = B_BLACK;
      MENU_FOREGROUND0 = F_BLACK;
      MENU2_FOREGROUND0 = F_BLACK;
      MENU_FOREGROUND1= F_WHITE;
      EDITWINDOW_BACK= B_BLACK;
      EDITWINDOW_FORE= F_WHITE;
      SCROLLBAR_BACK= B_WHITE;
      SCROLLBAR_FORE= F_WHITE;
      SCROLLBAR_SEL= B_BLACK;
      SCROLLBAR_ARR= B_CYAN;
      WINDOW_TITLEB = B_BLACK;
      WINDOW_TITLEF = F_WHITE;
     break;
   case 2: //Dark color scheme
      EDITAREACOL= B_BLACK;
      EDIT_FORECOLOR=F_WHITE;
      STATUSBAR =B_BLUE;
      STATUSMSG =F_WHITE;
      MENU_PANEL =B_BLACK;
      MENU2_PANEL =B_BLACK;
      MENU_SELECTOR = B_WHITE;
      MENU_FOREGROUND0 = F_WHITE;
      MENU2_FOREGROUND0 = F_WHITE;
      MENU_FOREGROUND1 = F_BLACK;
      EDITWINDOW_BACK= B_BLACK;
      EDITWINDOW_FORE= F_WHITE;
      SCROLLBAR_BACK= B_WHITE;
      SCROLLBAR_FORE= F_BLACK;
      SCROLLBAR_SEL= B_BLUE;
      SCROLLBAR_ARR= B_WHITE;
      WINDOW_TITLEB = B_WHITE;
      WINDOW_TITLEF = F_BLACK;
      break;
   default:
      break;
  }
}

int colorsWindow(LISTCHOICE * mylist, char *windowTitle) {
  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  LISTCHOICE data;
  int     rows, columns;

  get_terminal_dimensions(&rows, &columns);

  window_y1 = (rows / 2) - 3;
  window_y2 = (rows / 2) + 1;
  window_x1 = (columns / 2) - 8;
  window_x2 = (columns / 2) + 8;
  loadmenus(mylist, COLORS_MENU);

  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,
          WINDOW_TITLEB,1,1);
 write_str((window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF);
  start_vmenu(&data);
  free_list(mylist);
  close_window();
  return data.index;
}

int helpWindow(LISTCHOICE * mylist, char *message, char *windowTitle) {
  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  int     ok = 0;
  size_t     i = 0;
  int     ix = 0;
  int     j = 0;
  char    tempChar;
  LISTCHOICE data;
  int     rows, columns;

  get_terminal_dimensions(&rows, &columns);

  window_y1 = (rows / 2) - 7;
  window_y2 = (rows / 2) + 8;
  window_x1 = (columns / 2) - 25;
  window_x2 = (columns / 2) + 25;

  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,WINDOW_TITLEB,1,1);
  write_str((window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF);

  //Write message to info window
  for(i = 0; i < strlen(message); i++) {
    tempChar = message[i];
    //Maximum four lines
    if(j < window_y2-2) {
      //Split message if \n
      if(tempChar == '\n') {
    j++;
    ix = 0;
      } else {
    write_ch(window_x1 + 2 + ix, window_y1 + 1 + j, tempChar, MENU_FOREGROUND0,
         MENU_PANEL);
    ix++;
      }
    }
  }
  add_item(mylist, "[OK]", (columns / 2)-2, window_y2-1, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  start_hmenu(&data);
  free_list(mylist);
  close_window();
  if(data.index == OPTION_1)
    ok = CONFIRMATION;      //Confirmation has been given
  return ok;
}

