/* C-edit project
* Last updated - 5/4/2024
* New drop-down menu implementation
* Added more modularisation: editor.c menu.c
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "listbox.h"
#include "scbuf.h"
#include "rterm.h"
#include "tm.h"
#include "global.h" 
#include "keyb.h"
#include "edbuf.h"
#include "ui.h"
#include "menu.h"
#include "editor.h"
#include "opfile.h"

//Prototypes
void draw_screen();
void cursor_tick();
int special_keys();
int control_keys(char ch);
void _resizeScreen();
wchar_t currentChar = 0;
char tempMessage[150];
int displayMessage(char *temporaryMessage, int x, int y, int fColor, int bColor, int timeDuration);

void draw_screen(){
//BASE SCREEN IS STORED IN SCREEN 2
     int i=0;
     if (screen1 != NULL) deleteList(&screen1);
     //if (screen2 != NULL) deleteList(&screen2);
     //Init 2 : Create 2 Screens for a double buffer  approach  
     old_rows=new_rows;
     old_columns=new_columns;
     create_screen(&screen1);
     create_screen(&screen2);
     //SCREEN 2
     screen_color(screen1, EDITAREACOL, EDIT_FORECOLOR, FILL_CHAR);
    //Failsafe just in case it can't find the terminal dimensions
    if(old_rows == 0)
      old_rows = ROWS_FAILSAFE;
    if(old_columns == 0)
    old_columns = COLUMNS_FAILSAFE;

  //Draw upper and lower bars
    for(i = 0; i < old_columns; i++) {
     write_ch(screen1, i, 1, FILL_CHAR, MENU_PANEL, MENU_PANEL,0);
    }

  for(i = 0; i < old_columns; i++) {
    write_ch(screen1, i, old_rows, FILL_CHAR, STATUSBAR, STATUSMSG,1);
  }
  // Text messages
  write_str(screen1, 0, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0,0);
  write_str(screen1, 0, 1, "F", MENU_PANEL, F_RED,0);
  write_str(screen1, 7, 1, "p", MENU_PANEL, F_RED,0);
  write_str(screen1, 15, 1, "H", MENU_PANEL, F_RED,0);
  write_str(screen1, 0, old_rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG,0);

  /* Frames */
  //window appearance and scroll bar
  for(i = 2; i < old_rows; i++) {
    write_ch(screen1,old_columns-1, i, ' ', SCROLLBAR_BACK, SCROLLBAR_FORE,0);	//Scroll bar
    write_ch(screen1,0, i, VER_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//upper vertical line box-like char 
  }
  for(i = 0; i < old_columns; i++) {
    write_ch(screen1,i, 2, HOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//horizontal line box-like char
    write_ch(screen1,i, old_rows - 1, ' ', EDITWINDOW_BACK, EDITWINDOW_FORE,0);
  }
  write_ch(screen1,0, 2, UPPER_LEFT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//upper-left box-like char
  //horizontal scroll bar
  for(i = 0; i < old_columns; i++) {
    write_ch(screen1,i, old_rows - 1, FILL_CHAR, SCROLLBAR_BACK, SCROLLBAR_FORE,0);
  }
  //Window-appearance
  write_ch(screen1,old_columns-1, 2, UPPER_RIGHT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//right window corner
  write_ch(screen1,old_columns-1, old_rows - 1, LOWER_RIGHT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE,0);
  write_ch(screen1, 0, old_rows - 1, LOWER_LEFT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE,0);

  //Scroll symbols
  write_ch(screen1,old_columns-1, 3, '^', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns-1, old_rows - 2, 'v', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns-1, 4, '*', SCROLLBAR_SEL, SCROLLBAR_FORE,0);
  write_ch(screen1, 2, old_rows - 1, '*', SCROLLBAR_SEL, SCROLLBAR_FORE,0);
  write_ch(screen1, 1, old_rows - 1, '<', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns - 2, old_rows - 1, '>', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  if (strlen(fileName) == 0) strcpy(fileName, "UNTITLED");
  write_str(screen1,(new_columns / 2) - (strlen(fileName) / 2), 2, fileName,
        MENU_PANEL, MENU_FOREGROUND0,0); 

  dump_screen(screen1);
  //Save screen for later
  //copy_screen(screen2,screen1);
}

void cursor_tick(void){
//CURSOR ANIMATION
char drawChar=FILL_CHAR;
char drawChar0=FILL_CHAR;
int attrib = 0;
wchar_t code_point;
    //GET CHAR BEING POINTED AT FROM EDIT BUFFER
   if (edBuf1 != NULL)_dumpLine(edBuf1, posBufY, &tempLine);
    drawChar = tempLine.linea[posBufX].ch; 
    drawChar0 = tempLine.linea[posBufX].specialChar; 
    attrib =  tempLine.linea[posBufX].attrib;
    //CHECK FOR SPECIAL CHARACTERS AND CONVERT THEM TO UNICODE TO PRINT
    //IF CURSOR IS ON A CHAR HIGHLIGHT IT IN YELLOW
    if (drawChar0 != 0 && drawChar != '\0') code_point = ((drawChar0 & 0x1F) << 6) | (drawChar & 0x3F);
    else {
            if (drawChar == 0 || drawChar == END_LINE_CHAR) drawChar = FILL_CHAR;
	    code_point = drawChar;
    }
    if (cursor_timer1.ticks % 2 == 0) update_ch(cursorX, cursorY, CURSOR_CHAR, B_BLUE, F_WHITE);
    else update_ch(cursorX, cursorY, code_point, EDITAREACOL, FH_YELLOW);
    
    //REST ANSI VALUES TO HAVE HIGH INTENSITY COLORS
    resetAnsi(0);

    //CLEAR LAST POSITION OF CURSOR WITH FILL_CHAR OR GO BACK TO PREVIOUS CHAR
    if ((old_cursorX != cursorX) || (old_cursorY != cursorY)){
      //old?
      if (edBuf1 != NULL)_dumpLine(edBuf1, oldposBufY, &tempLine);
           drawChar = tempLine.linea[oldposBufX].ch; 
           drawChar0 = tempLine.linea[oldposBufX].specialChar; 
           attrib =  tempLine.linea[oldposBufX].attrib;
    //always clear cursor cell (avoids false misprints)
    update_ch(old_cursorX, old_cursorY, ' ', EDITAREACOL, EDIT_FORECOLOR);

  
    //then try to update to its previous state
    if (drawChar0 != 0 && drawChar != '\0') code_point = ((drawChar0 & 0x1F) << 6) | (drawChar & 0x3F);
    else {
            if (drawChar == 0 || drawChar == END_LINE_CHAR) drawChar = FILL_CHAR;
	    code_point = drawChar;
    }
       update_ch(old_cursorX, old_cursorY, code_point, EDITAREACOL, attrib);
    } 
    if (old_cursorY != cursorY) {
         //Point to line in buffer when Y position changes
        if (edBuf1 != NULL) _dumpLine(edBuf1, posBufY, &tempLine);
    }
    //printf("\n");
    //write_num(screen1, 10,10, cursorX, B_GREEN, F_WHITE,1);
    //write_num(screen1, 10,11, cursorY, B_RED, F_WHITE,1);
    //write_num(screen1, 10,12, old_cursorX, B_GREEN, F_WHITE,1);
    //write_num(screen1, 10,12, old_cursorY, B_GREEN, F_WHITE,1);
    //write_num(screen1, 10,14, posBufX, B_RED, F_WHITE,1);
    //write_num(screen1, 10,15, posBufY, B_RED, F_WHITE,1);
    //write_num(screen1, 10,16, findEndline(tempLine), BH_BLUE, F_WHITE,1);
    //write_num(screen1, 10,17, _length(&edBuf1), B_RED, F_WHITE,1);
    //for (int i=0; i<findEndline(tempLine);i++) write_ch(screen1, 10+i,16, code_point, B_RED, F_WHITE,1);
    //dump_screen(screen1);
   //Buffer pointer position
     update_str(new_columns - 24, new_rows, "| L:        C:     ", STATUSBAR, STATUSMSG);
    write_num(screen1, new_columns - 10, new_rows, posBufX, STATUSBAR, STATUSMSG,1);
    write_num(screen1, new_columns - 20, new_rows, posBufY+1, STATUSBAR, STATUSMSG,1);
    update_str(new_columns - 39, new_rows, "| LINES:      ", STATUSBAR, STATUSMSG);
    write_num(screen1, new_columns - 31, new_rows, _length(&edBuf1), STATUSBAR, STATUSMSG,1);
}


int main(int argc, char *argv[]) {

char ch=0;
char old_ch=0;
int esc_key = 0;
int keypressed = 0;

    //Init Terminal
    init_term();
    initCEDIT();
    draw_screen();
    resetch();
    strcpy(fileName, "\0");
    //tempLine.linea[0].ch = END_LINE_CHAR;
    //tempLine.linea[posBufX].ch = END_LINE_CHAR;
    //tempLine.linea[posBufX].specialChar = 0;
    //tempLine.linea[posBufX].attrib = EDIT_FORECOLOR;
    if(argc > 1) {
     //Does the file exist? Open or create?
      strcpy(fileName, argv[1]);
      filetoBuffer(fileName);
      flush_editarea(0);
      buffertoScreen(0);
      dump_screen(screen1);
    } 
    do{    
	 //end flag from any part of the program
	 if (programStatus == ENDSIGNAL) break;
	 //Time animation & resize window 
	  if (timerC(&timer3) == TRUE){
		   displayMessage(tempMessage,  (new_columns/2)-strlen(tempMessage)/2, 1, FH_WHITE, B_RED, 5);
	  }
	 if (timerC(&timer2) == TRUE){
           _animation();
	    //Refresh screen size buffers if terminal dimension changes
	    if (new_rows != old_rows || new_columns != old_columns)
            {
	      _resizeScreen();
            }
	  }
         //Cursor Animation	 
	 if (timerC(&cursor_timer1) == TRUE){
	      //Animation
             cursor_tick();
	     //if (keypressed == FALSE) { esc_key = 0;}
           }
	//PROCESS INPUT
    	 keypressed = kbhit(1);
          if (keypressed == TRUE) {
	   keypressed = FALSE;
	   //try to catch and avoid printing unwanted chars with cursor keys
	   old_ch = ch;
	   ch=readch();
	   if (old_ch==ESC_KEY) {unwantedChars = 1; esc_key = 1;}
	   if (ch==ESC_KEY) {unwantedChars = 1; esc_key = 1;}
	   if (unwantedChars>0) esc_key = 1;

	   //Keys with a escape sequenece
           if (ch == ESC_KEY) {//buffertoScreen(0, 0,FALSE); 
			       esc_key = special_keys(); 
			       cursor_tick(); 
			       ch = 0;}       
           else {
		//Capture control keys   
		if ((ch>0 && ch< 0x0F) && (ch!=K_ENTER && ch != K_TAB)){buffertoScreen(FALSE); esc_key= control_keys(ch); ch=0;}   
		else 
	         //Process raw edit input from keyboard in editor.c		
		{
		  //try to avoid printing unwanted chars with cursor keys
		  if (ch != 0 && esc_key==0 && unwantedChars == 0&& posBufX <MAX_LINE_SIZE) editor_section(ch);
	        }
	   }
        }else{
	//if not keypressed reset values for unwanted characters
             esc_key=0; unwantedChars = 0;
	}//Check for ESC-related keys
      } while (esc_key != ENDSIGNAL);     
    //restore terminal
     credits();    
     return 0;
}


//CONTROL KEYS
int control_keys(char ch){
  char    returnMenuChar=0;
  int menuCounter = 0;
  int returnValue=0;
  char countCh=0;
  char tempfileName[MAXFILENAME];
    if(ch == K_CTRL_L) {
      //Akin to F2
	handlemenus(&returnMenuChar, &menuCounter,TRUE);
    } 
    if(ch == K_CTRL_C) {
      returnValue  = ENDSIGNAL;
    }
    if (ch == K_CTRL_A) {
      flush_editarea(0);
       buffertoScreen(0);
      countCh=inputWindow("File:", tempfileName,  "Quick load...",28,2,48);
      if (countCh>0) {
	 strcpy(fileName, tempfileName);
	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0);
        dump_screen(screen1);
     }//buffertoScreen(0, 0, 0);
    }

      if (ch == K_CTRL_N){
        if (openFileDialog(fileName,fullPath) == 1){
 	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0);
     }
        dump_screen(screen1);
    
    
    }
	     

 
  return returnValue;
}

//ESCAPE-RELATED KEYS
int special_keys() {
/* MANAGE SPECIAL KEYS */
/*
   New implementation: Trail of chars found in keyboard.c
   If K_ESCAPE is captured read a trail up to 5 characters from the console.
   This is to control the fact that some keys may change
   according to the terminal and expand the editor's possibilities.
   Eg: F2 can be either 27 79 81 or 27 91 91 82.
*/
      //cursorY : position on screen
      //posBufY : position in buffer
 
  int     esc_key = 0;
  char    chartrail[5];
  char    returnMenuChar=0;
  int menuCounter = 0;
  int countCh = 0;
  char tempfileName[MAXFILENAME];
    old_cursorX = cursorX;
    old_cursorY = cursorY;
    oldposBufX = posBufX;
    oldposBufY = posBufY;


    strcpy(chartrail, "\0");
    read_keytrail(chartrail);   //Read trail after ESC key
    //only ESC key detected, finish program
    if (chartrail[0] == ESC_KEY && chartrail[1]==0) return ENDSIGNAL;
    
    //Check key trails for special keys starting with ESC.
    //FUNCTION KEYS : F1 - F4
    if(strcmp(chartrail, K_F2_TRAIL) == 0 ||
       strcmp(chartrail, K_F2_TRAIL2) == 0) {
       handlemenus(&returnMenuChar, &menuCounter,TRUE);
    } else if(strcmp(chartrail, K_F3_TRAIL) == 0 ||
          strcmp(chartrail, K_F3_TRAIL2) == 0) {
    } else if(strcmp(chartrail, K_F1_TRAIL) == 0 ||
          strcmp(chartrail, K_F1_TRAIL2) == 0) {
	  flush_editarea(0);
          buffertoScreen(0); 
	    displayHelp();
    } else if(strcmp(chartrail, K_F4_TRAIL) == 0 ||
          strcmp(chartrail, K_F4_TRAIL2) == 0) {

      // ARROW KEYS
    } else if(strcmp(chartrail, K_LEFT_TRAIL) == 0) {
      //Left-arrow key
      unwantedChars++;
      if(cursorX > 1){
        cursorX = cursorX - 1;
        //editScroll.bufferX--;
      }
      if (posBufX>0) posBufX--;
      //adjust shiftH when going back to account for the fact that cursorX=1 is not enough
      if (cursorX == new_columns - 4) shiftH++;
      if (cursorX == 1 && shiftH>0) {shiftH--;buffertoScreen(1);}
    } else if(strcmp(chartrail, K_RIGHT_TRAIL) == 0) {
      //Right-arrow key
      unwantedChars++;
      if(cursorX < new_columns - 3){
        cursorX = cursorX + 1;
        //shiftH = 0;
      } else{
	if (posBufX<MAX_LINE_SIZE) shiftH++;
	buffertoScreen(1);      
      }
      if (posBufX<MAX_LINE_SIZE) posBufX++;

    } else if(strcmp(chartrail, K_UP_TRAIL) == 0) {
      //Up-arrow key
      unwantedChars++;
      if(cursorY > 2) {
       cursorY = cursorY - 1;
        
      }else{
	      //scrolling up
	        if (currentLine>0) {
			currentLine--;
	               buffertoScreen(1);
		}
      }
      if (posBufY > 0) posBufY--;
 
    } else if(strcmp(chartrail, K_DOWN_TRAIL) == 0) {
      //Down-arrow key
     //Check if there are more lines to go to
      unwantedChars++;
      if (posBufY<_length(&edBuf1)-1){
        if(cursorY < new_rows - 3) {
          //stay put if we are are the end of the viewing area
          cursorY = cursorY + 1;
        }
	else{  
                //scrolling down
	        if (_length(&edBuf1) > vdisplayArea ) {
		  currentLine++;
	          buffertoScreen(1);
		}
	}  
        posBufY++;
      }
    } else if(strcmp(chartrail, K_PAGEDOWN_TRAIL) == 0) {
      //Page Down key
     } else if(strcmp(chartrail, K_PAGEUP_TRAIL) == 0) {
      //Page Down key
     }else if(strcmp(chartrail, K_HOME_TRAIL) == 0 || strcmp(chartrail, K_HOME_TRAIL2) == 0 ) {
    } else if(strcmp(chartrail, K_DELETE) == 0) {
      //delete button;
    } else if(strcmp(chartrail, K_ALT_F) == 0) {
      returnMenuChar=FILE_MENU;
      menuCounter=FILE_MENU;
      handlemenus(&returnMenuChar, &menuCounter,FALSE);
    } else if(strcmp(chartrail, K_ALT_P) == 0) {
      returnMenuChar=OPT_MENU;
      menuCounter=OPT_MENU;
      handlemenus(&returnMenuChar, &menuCounter,FALSE);
    } else if(strcmp(chartrail, K_ALT_H) == 0) {
      returnMenuChar=HELP_MENU;
      menuCounter=HELP_MENU;
      handlemenus(&returnMenuChar, &menuCounter,FALSE);
    } else if(strcmp(chartrail, K_ALT_O) == 0) {
      //openFileHandler();    //Open file Dialog
         buffertoScreen(0);
        dump_screen(screen1);
       if (openFileDialog(fileName,fullPath) == 1){
 	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0);
     }
        dump_screen(screen1);
    } else if(strcmp(chartrail, K_ALT_N) == 0) {
      //newDialog(currentFile);   // New file
      //refresh_screen(-1);
      resetch();
    } else if(strcmp(chartrail, K_ALT_A) == 0) {
      //saveasDialog(currentFile);    //Save as.. file
      //refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_S) == 0) {
      //Save file
      flush_editarea(0);
      buffertoScreen(0);
      strcpy(tempMessage, "[File saved!]\0");
    	   if (strcmp(fileName, "UNTITLED") == 0) {
        countCh=inputWindow("File:", tempfileName,  "Save file as...",28,2,48);
        if (countCh>0) {
	   strcpy(fileName, tempfileName);
	   buffertoFile(fileName);
	   flush_editarea(0);
	   buffertoScreen(0);
           dump_screen(screen1);
           timer3.ticks=0; 
	}	
       } else{
	   buffertoFile(fileName);
	   flush_editarea(0);
	   buffertoScreen(0);
           dump_screen(screen1);
           timer3.ticks=0; 
       }
    } else if(strcmp(chartrail, K_ALT_W) == 0) {
      //if(strcmp(currentFile, UNKNOWN) == 0)
    //saveasDialog(currentFile);  //Write to file
      //else {
    //saveDialog(currentFile);
      //}
      //refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_X) == 0) {
    /*  
    if(fileModified == 1)
      exitp = confirmation(); //Shall we exit? Global variable!
      else
    exitp = EXIT_FLAG;
    } } 
    */
    }
    esc_key = 1;
  return esc_key;

}

/*
void linetoScreen(long whereY, VLINES tempLine, BOOL clean){
//dump temporary Line to screen buffer
   int i,j=0;
   for (i=0; i<findEndline(tempLine); i++){
   	if(tempLine.linea[i].specialChar != 0) {
	  //Special char ? print the two values to screen buffer.
	  write_ch(i+START_CURSOR_X, whereY,  tempLine.linea[i].specialChar, EDITAREACOL,
		   EDIT_FORECOLOR);
	  write_ch(i+START_CURSOR_X, whereY,  tempLine.linea[i].ch, EDITAREACOL,   EDIT_FORECOLOR);
	} else {
	  write_ch(i+START_CURSOR_X, whereY, tempLine.linea[i].ch, EDITAREACOL,
		   EDIT_FORECOLOR);
	}
	if (clean == TRUE) 
	  for (j=findEndline(tempLine); j<columns -2; j++)
		write_ch(j+START_CURSOR_X, whereY, FILL_CHAR, EDITAREACOL, EDIT_FORECOLOR); 	  
	}
	write_ch(findEndline(tempLine)+START_CURSOR_X, whereY, '|', B_GREEN,
		   F_WHITE);
}	
*/

/*---------*/
/* Credits */
/*---------*/

void credits() {
  //Frees memory and displays goodbye message 
  //Free selected path item/path from opfiledialog 
  size_t i; //to be compatible with strlen
  char auth[27] ="Coded by v3l0r3k 2018-2024";  
  //free_buffer();
  //free memory
  if (screen1 != NULL) deleteList(&screen1);
  if (screen2 != NULL) deleteList(&screen2);
  if (listBox1 != NULL) removeList(&listBox1);
  close_term();			//restore terminal settings from failsafe
  showcursor();
  resetAnsi(0);
  screencol(0);
  outputcolor(7, 0);
 
  printf(cedit_ascii_1);
  printf(cedit_ascii_2);
  printf(cedit_ascii_3);
  printf(cedit_ascii_4);
  printf(cedit_ascii_5);
  printf(cedit_ascii_6);

  outputcolor(0, 90);
  printf("\n%s",auth);
  outputcolor(0, 37);
  timer2.ms=10;
  timer2.ticks=0;
  i=0;

  outputcolor(0, 97);
  do{
    if (timerC(&timer2) == 1) { 
       gotoxy(i,8);
       if (i==strlen(auth)) outputcolor(0,93);
       if (i<10 || i>16) 
	if (i<=strlen(auth)) printf("%c", auth[i-1]);
       printf("\n");
       i++;
     }
    } while (timer2.ticks <30);
//log
// _printlist(&edBuf1);
//  printf("%ld:%ld\n", posBufX, posBufY);
//  printf("\n%ld\n",sizeof(&edBuf1));
  _deletetheList(&edBuf1); //free edit Buffer
  resetAnsi(0);
 // close_term();
}
void _resizeScreen(){
//redraw everything when screen size changes
	get_terminal_dimensions(&new_rows, &new_columns);
	if (screen1 != NULL) deleteList(&screen1);
	if (screen2 != NULL) deleteList(&screen2);
	create_screen(&screen1);
	create_screen(&screen1);
	draw_screen();
	flush_editarea(0);
	vdisplayArea = new_rows - 4;
	//if screen resizes file pointers are rewound to the beginning
        currentLine = 0;
	posBufX = 0;
	posBufY = 0;
	shiftH = 0;
	buffertoScreen(0);
        dump_screen(screen1);
	cursorX = START_CURSOR_X;
	cursorY = START_CURSOR_Y;
}

int displayMessage(char *temporaryMessage, int x, int y, int fColor, int bColor, int timeDuration){
//display a temporary nessage controlled by Timer3
   if (timer3.ticks == -1) {
           write_str(screen1, x,y, temporaryMessage, B_WHITE, F_WHITE,1);
	   return 0;
   }
  if (timer3.ticks  >= timeDuration) {
	  stop_timer(&timer3); 
          write_str(screen1, x,y, temporaryMessage, B_WHITE, F_WHITE,1);
	  return 0;
  } 
   write_str(screen1, x,y, temporaryMessage, bColor, fColor,1);
   return 1;
}


