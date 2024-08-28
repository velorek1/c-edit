/* Module to create different widgets/gadgets 
 * for a Text User Interface
 * TextBox
 * Window
 * Last modified: 15/04/2023
 * @author:velorek
 */
#include <stdio.h>
#include "rterm.h"
#include "scbuf.h"
#include "ui.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/
int textbox(SCREENCELL *newScreen,int wherex, int wherey, int displayLength,
        char *label, char text[MAX_TEXT], int backcolor,
        int labelcolor, int textcolor, BOOL raw, BOOL locked) {
  int     charCount = 0;
  int     exitFlag = 0;
  int     cursorON = 1;
  int     i;
  int     limitCursor = 0;
  int     positionx = 0;
  int     posCursor = 0;
  int     keypressed = 0;
  char    chartrail[5];
 // char    accentchar[2];
  char    displayChar;
  char    ch;
  NTIMER  cursorTime;
  SCREENCELL *aux = newScreen;
  resetAnsi(0); 
  init_timer(&cursorTime,150);
  positionx = wherex + strlen(label);
  limitCursor = wherex+strlen(label)+displayLength+1;
  write_str(aux,wherex, wherey, label, backcolor, labelcolor,raw);
  write_ch(aux,positionx, wherey, '[', backcolor, textcolor,raw);
  for(i = positionx + 1; i <= positionx + displayLength; i++) {
    write_ch(aux,i, wherey, '.', backcolor, textcolor,raw);
  }
  write_ch(aux,positionx + displayLength + 1, wherey, ']', backcolor,
       textcolor,raw);
  if (raw != TRUE) dump_screen(aux);
  //Reset keyboard
  if(kbhit(1) == 1) ch = readch();
  ch = 0;

  do {
	 if (locked == 0) break;
      keypressed = kbhit(1);
       if (timerC(&timer2) == TRUE) _animation();
    //Cursor Animation
   if (keypressed == 0){
    
    if (timerC(&cursorTime) == TRUE){
      switch (cursorON) {
    case 1:
      posCursor = positionx + 1;
          displayChar = '.';
          if (posCursor == limitCursor) {
            posCursor = posCursor - 1;
            displayChar = ch;
          }
          write_ch(aux,posCursor, wherey, displayChar, backcolor, textcolor,raw);
          //update_screen(aux);
          if (raw != TRUE) dump_screen(aux);
          cursorON = 0;
      break;
    case 0:
          posCursor = positionx + 1;
          if (posCursor == limitCursor) posCursor = posCursor - 1;
          write_ch(aux,posCursor, wherey, '|', backcolor, textcolor,raw);
          //update_screen(aux);
          if (raw != TRUE) dump_screen(aux);
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
      write_ch(aux,positionx + 1, wherey, ch, backcolor, textcolor,raw);
      text[charCount] = ch;
      positionx++;
      charCount++;
      //update_screen(aux);
      if (raw != TRUE) dump_screen(aux);
    }
      }
    }
    if (ch==K_CTRL_C){
	    return ENDSIGNAL;
    }
    if (ch==K_BACKSPACE){
      if (positionx>0 && charCount>0){
       ch=0;
       positionx--;
       charCount--;
       text[charCount] = '\0';
       write_ch(aux,positionx + 1, wherey, '.', backcolor, textcolor,raw);
       if (positionx < limitCursor-2) write_ch(aux,positionx + 2, wherey, '.', backcolor, textcolor,raw);
       //update_screen(aux);
       if (raw != TRUE) dump_screen(aux);
       //resetch();
      }
    }
    if(ch == K_ENTER || ch == K_TAB || ch == K_ESCAPE)
      exitFlag = 1;

    //ENTER OR TAB FINISH LOOP
  } while(exitFlag != 1);
  text[charCount] = '\0';
  //Clear field
  positionx = wherex + strlen(label);
  for(i = positionx + 1; i <= positionx + displayLength; i++) {
    write_ch(aux,i, wherey, '.', backcolor, textcolor,raw);
  }
  write_ch(aux,positionx + displayLength + 1, wherey, ']', backcolor,
       textcolor,raw);
 
  //resetch();
  if (ch == K_ESCAPE) charCount = 0;
  return charCount;
}


void window(SCREENCELL *screen1, int x1, int y1, int x2, int y2, int backcolor,
         int bordercolor, int titlecolor, int border, int title, int shadow) {
/*
   Chars for drawing box-like characters will be passed as negative values.
   When the update_screen routine is called, it will check for negative
   values and map these chars to Unicode characters.
 */
  int     i, j;
  //char ch=0x20;
  wchar_t ch=0x20;
  i = x1;
  j = y1;
  
  resetAnsi(0);
  //shadow
  if (shadow==1){
   for(j = y1 + 1; j <= y2 + 1; j++)
    for(i = x1 + 1; i <= x2 + 1; i++)
    {
      ch=read_char(screen1, i,j); //dynamic shadow
      if ((ch=='\0') || (ch==UNICODEBAR1)) ch=0x20;
      write_ch(screen1, i, j, ch, B_BLACK, F_WHITE,0);
    }
  }
  //window
  for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
      write_ch(screen1, i, j, ' ', backcolor, bordercolor,0);
  //borders
  if(border == 1) {
    //with borders. ANSI-ASCII 106-121
    for(i = x1; i <= x2; i++) {
      //upper and lower borders
      write_ch(screen1, i, y1, HOR_LINE, backcolor, bordercolor,0);   //horizontal line box-like char
      write_ch(screen1, i, y2, HOR_LINE, backcolor, bordercolor,0);
    }
    for(j = y1; j <= y2; j++) {
      //left and right borders
      write_ch(screen1,x1, j, VER_LINE, backcolor, bordercolor,0);   //vertical line box-like char
      write_ch(screen1,x2, j, VER_LINE, backcolor, bordercolor,0);
    }
    write_ch(screen1, x1, y1, UPPER_LEFT_CORNER, backcolor, bordercolor,0);   //upper-left corner box-like char
    write_ch(screen1, x1, y2, LOWER_LEFT_CORNER, backcolor, bordercolor,0);   //lower-left corner box-like char
    write_ch(screen1, x2, y1, UPPER_RIGHT_CORNER, backcolor, bordercolor,0);  //upper-right corner box-like char
    write_ch(screen1, x2, y2, LOWER_RIGHT_CORNER, backcolor, bordercolor,0);  //lower-right corner box-like char
  }
  if (title == 1) {
    for(i = x1; i <= x2; i++)
      write_ch(screen1, i, y1-1, ' ', titlecolor, titlecolor,0);
  }
 // dump_screen(screen1);
}

int inputWindow(char *label, char *tempFile, char *windowTitle,  int offsetX, int offsetY, int length) {
  int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
  int     count = 0;
  resetAnsi(0);
  copy_screen(screen2,screen1);
  window_y1 = (new_rows / 2) - offsetY;
  window_y2 = (new_rows / 2);
  window_x1 = (new_columns / 2) - offsetX;
  window_x2 = (new_columns / 2) + offsetX;
  
  window(screen1,window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0,
          WINDOW_TITLEB,1,1,1);
  write_str(screen1, (window_x2-window_x1) /2 + window_x1 - (strlen(windowTitle)/2) , window_y1-1, windowTitle, WINDOW_TITLEB, WINDOW_TITLEF,0);
  dump_screen(screen1);
  count =
      textbox(screen1, window_x1 + 1, window_y1 + 1, length, label, tempFile, MENU_PANEL,
          MENU_FOREGROUND0, MENU_FOREGROUND0,1,1);
  copy_screen(screen1,screen2);
  dump_screen(screen1);
  return count;
}

