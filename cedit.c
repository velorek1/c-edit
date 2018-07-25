/*
======================================================================
PROGRAM C Editor - An editor with top-down menus.

@author : Velorek
@version : 1.0
Last modified : 25/07/2018                                           
======================================================================*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "c_cool.h"
#include "list_choice.h"
#include "screen_buffer.h"

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

//GOODBYE MSG - ASCII ART
#define cedit_ascii_1 "  _____      ______    _ _ _   \n"   
#define cedit_ascii_2 " / ____|    |  ____|  | (_) |  \n"  
#define cedit_ascii_3 "| |   ______| |__   __| |_| |_ \n" 
#define cedit_ascii_4 "| |  |______|  __| / _` | | __|\n"
#define cedit_ascii_5 "| |____     | |___| (_| | | |_ \n" 
#define cedit_ascii_6 " \\_____|    |______\\__,_|_|\\__|\n"

//MISC. CONSTANTS
#define EXIT_FLAG 1 
#define FAILSAFE '$' //Failsafe char to activate menus.
#define TAB_DISTANCE 8 //How many spaces TAB key will send.
#define START_CURSOR_X 2
#define START_CURSOR_Y 3
#define TIME_MS 5000
#define LIMIT_TIMER 10
#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80
#define K_LEFTMENU -1 //Left arrow key pressed while in menu
#define K_RIGHTMENU -2 //Right arrow key pressed while in menu

//KEYS 
#define K_ENTER 13
#define K_CAPS 91
#define K_BACKSPACE 127
#define K_TAB 9
#define K_ESCAPE 27
#define K_F2 81
#define K_F3 82
#define ARROWKEYS_FLAG 91 //When cursor keys are being used, the scancode 91 is obtained 

//MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define OPT_MENU 1
#define HELP_MENU 2
#define EXIT_MENU 3
#define ABOUT_MENU 4

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

/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/

LISTCHOICE *mylist, data;
BCELL  *my_screen;

int     rows=0, columns=0, old_rows=0, old_columns=0; // Terminal dimensions
int     cursorX=START_CURSOR_X,cursorY=START_CURSOR_Y, timerCursor=0; // Cursor position and Timer
int     exitp = 0; // Exit flag for main loop
char    kglobal=0; // Global variable for menu animation

/*====================================================================*/
/* PROTOTYPES OF FUNCTIONS                                            */
/*====================================================================*/

void credits();
int main_screen();
void update_position();
void loadmenus(int choice);
char horizontal_menu();
void drop_down(char *kglobal);
void filemenu();
void optionsmenu();
void helpmenu();
int confirmation();
int about_info();
int process_input(int *whereX, int *whereY, char ch);
int special_keys(int *whereX, int *whereY,char ch);
void draw_cursor(int *whereX, int *whereY, int *timer);
int refresh_screen(int force_refresh);
int timer_1(int *timer1);

/*====================================================================*/
/* MAIN PROGRAM - CODE                                                */
/*====================================================================*/

int main() {
  char    ch=0;
  int esc_key =0; //To control key input and scan for keycodes.
  int keypressed =0;
  int timer1 =0; // Timer to display animation

  hidecursor(); 
  pushTerm(); //Save current terminal settings in failsafe
  create_screen(); //Create screen buffer to control display
  main_screen(); //Draw screen

  do {  
    /* CURSOR */
    draw_cursor(&cursorX,&cursorY,&timerCursor);   
    /* Query if screen dimensions have changed and resize screen */
    refresh_screen(0); 
    /* Wait for key_pressed to read key */
    keypressed = kbhit();
    /* Timer for animation to show system time and clean cursor */
    timer_1(&timer1); 

    if (keypressed==1){ 
  
        /* Process SPECIAL KEYS and other ESC-related issues */
        esc_key=special_keys(&cursorX,&cursorY,ch);
       
        
        ch=readch();

        /* EDIT */
        if (esc_key == 0) {
          //Process input and get rid of extra characters
          process_input(&cursorX,&cursorY, ch); //Edit
          keypressed=0;
        } 

     } //end if keypressed
 } while(exitp != EXIT_FLAG); //exit flag for the whole program
  credits();
  return 0;
}

/*====================================================================*/
/* FUNCTION DEFINITIONS                                               */
/*====================================================================*/


/*----------------------------------------- */
/* Update cursor position display on screen */
/*----------------------------------------- */

void update_position()
{
//Update cursor position display on screen
 write_str(columns-13,rows,"| L:   C:  ",B_CYAN,FH_WHITE);
 write_num(columns-4,rows,cursorX-1,3,B_CYAN,FH_WHITE);
 write_num(columns-9,rows,cursorY-2,4,B_CYAN,FH_WHITE);
}

/*--------------*/
/* EDIT Section */
/*------------- */

int process_input(int *whereX, int *whereY,char ch)
{
 if (ch != K_ESCAPE) {
   if (ch>31 && ch<127) {
    //only print ASCII characters to screen.
     write_ch(*whereX,*whereY,ch,B_BLUE,F_WHITE);
     *whereX = *whereX + 1;
   }
  if (ch==K_ENTER) {
    //RETURN - ENTER
      if (*whereY < rows -2) {
        *whereY = *whereY + 1;
        *whereX = 2;
      }
   }
   if (ch==K_BACKSPACE) {
    //BACKSPACE key
      write_ch(*whereX,*whereY,' ',B_BLUE,F_BLUE);
      if (*whereX > 2) *whereX = *whereX - 1;
   }
   if (ch==K_TAB) {
    //TAB key
      if (*whereX < columns-TAB_DISTANCE) *whereX = *whereX + TAB_DISTANCE;
   }
   //*ch=0;
 if (ch==FAILSAFE) {
   /*'$' Failsafe menu for some terminals */
    //F2 -> open drop-down menus
      if (horizontal_menu()==K_ESCAPE) 
      {
        //Exit horizontal menu with ESC 3x
        kglobal=K_ESCAPE;
        main_screen();
      } 
      /*  Drop-down menu loop */       
      drop_down(&kglobal); //animation
  }
 }
  return 0;
}

/*-----------------------------------------*/
/* Timer 1 Animation. Clock and cursor     */
/*-----------------------------------------*/

int timer_1(int *timer1){
/* Timer for animations - Display time and clean cursor */
  time_t mytime = time(NULL);
  char * time_str = ctime(&mytime);
  if (*timer1 == TIME_MS){
    *timer1=0;
    time_str[strlen(time_str)-1] = '\0';
    //display system time
    write_str(columns-strlen(time_str),1,time_str,F_BLACK,B_WHITE);
    update_position(); //update position display
    update_screen();
     return 0;
  } else
  {  
    *timer1 = *timer1 +1;
    return 1;
  }
}

/*-----------------------------------------*/
/* Manage keys that send a ESC sequence    */
/*-----------------------------------------*/

int special_keys(int *whereX, int *whereY,char ch){
/* MANAGE SPECIAL KEYS */
  int esc_key=0;
if (ch==K_ESCAPE){
  switch (getch()){
    case 'D':
    //Left-arrow key  
      if (*whereX > 2) *whereX=*whereX-1;
       break;
    case 'C':
    //Left-arrow key  
      if (*whereX < columns-1) *whereX=*whereX+1;
      break;
    case 'A':
    //Up-arrow key  
      if (*whereY > 3) *whereY=*whereY-1;
      break;
    case 'B':
    //Down-arrow key  
      if (*whereY < rows-2) *whereY=*whereY+1;
      break;
    case K_F2: 
    //F2 -> open drop-down menus
      if (horizontal_menu()==K_ESCAPE) 
      {
        //Exit horizontal menu with ESC 3x
        kglobal=K_ESCAPE;
        main_screen();
      } 
      /*  Drop-down menu loop */       
      drop_down(&kglobal); //animation
     break;
    case K_F3:
    //F3 -> Refresh screen
     refresh_screen(1);
     break;
       
    default:
      break;
    }
   esc_key = 1;
 } else {
   if (ch==ARROWKEYS_FLAG)  
   esc_key = 1;
   else
   esc_key = 0;
 }
return esc_key;
}

/*-----------------------------------------*/
/* Draw cursor on screen and animate it    */
/*-----------------------------------------*/

void draw_cursor(int *whereX, int *whereY, int *timer)
{
/* CURSOR is drawn directly to screen and not to buffer */
   *timer = *timer +1; //increase timer counter for animation
  if (*timer < LIMIT_TIMER)
  { 
    gotoxy(*whereX, *whereY);
    outputcolor(F_WHITE, B_BLUE);
    printf("|");
   } else
  {
    *timer=0; //reset timer
    gotoxy(*whereX, *whereY);
    outputcolor(B_BLUE, B_BLUE);
    printf(" ");
   }
}

/*-----------------*/
/* Refresh screen  */
/*-----------------*/

int refresh_screen(int force_refresh){
/* Query terminal dimensions again and check if resize 
   has been produced */
 get_terminal_dimensions(&rows, &columns);
  if (rows != old_rows || columns != old_columns || force_refresh)
  {
    free_buffer(); //delete structure from memory for resize
    create_screen(); //create new structure 
    main_screen(); //Refresh screen in case of resize
    return 1;
  } else
  {
    return 0;
  }
}

/*-------------------*/
/* Draw main screen  */
/*-------------------*/

int main_screen() {
  int     i;
 
  //Save previous values
  get_terminal_dimensions(&rows, &columns);
  old_rows = rows;
  old_columns = columns;
 
  //Failsafe just in case it can't find the terminal dimensions
  if(rows == 0)
    rows = ROWS_FAILSAFE;	
  if(columns == 0)
    columns = COLUMNS_FAILSAFE;

  bscreen_color(B_BLUE);
  //Draw upper and lower bars
  for(i = 1; i <= columns; i++) {
    write_ch(i, 1, FILL_CHAR, B_WHITE, F_WHITE);
  }

  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, B_CYAN, F_WHITE);
  }
  // Text messages
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  write_str(1, rows, ">> [C-Edit]", B_CYAN, FH_WHITE);
  write_str(14, rows, "| F2 or $ -> Menus |", B_CYAN, FH_WHITE);
  
  /* Frames */
  //window appearance and scroll bar
  for (i=2; i<rows; i++){
    write_ch(columns,i,' ',B_WHITE,F_BLACK);
    write_ch(1,i,UP_BOXCHAR,B_BLACK,F_WHITE); //upper vertical line box-like char 
  }
  for(i = 2; i < columns; i++) {
    write_ch(i, 2, HOR_BOXCHAR, B_BLACK, F_WHITE);//horizontal line box-like char
    write_ch(i, rows-1, ' ', B_BLACK, F_WHITE);
  }
  write_ch(1,2,UPLEFT_BOXCHAR,B_BLACK,F_WHITE); //upper-left box-like char
  //horizontal scroll bar
   for(i = 2; i < columns; i++) {
    write_ch(i, rows-1, FILL_CHAR, B_WHITE, F_WHITE);
  }
  
  write_str((columns/2)-4,2,"UNTITLED",B_WHITE,F_BLACK); 
  write_ch(columns,3,'^',B_BLACK,F_WHITE);
  write_ch(columns,rows-2,'v',B_BLACK,F_WHITE);
  write_ch(columns,4,'*',B_CYAN,F_WHITE);
  write_ch(3,rows-1,'*',B_CYAN,F_WHITE);
  write_ch(2,rows-1,'<',B_BLACK,F_WHITE);
  write_ch(columns-1,rows-1,'>',B_BLACK,F_WHITE);
  write_ch(columns,rows-1,'|',B_BLUE,F_WHITE);
  update_screen();
  return 0;
}

/*--------------------------------------------*/
/* Load current menu into circular linked list*/ 
/*--------------------------------------------*/

void loadmenus(int choice) {
  if(choice == HOR_MENU) {
    add_item(mylist, "File", 1, 1, B_WHITE, F_BLACK, B_BLACK, FH_WHITE);
    add_item(mylist, "Options", 7, 1, B_WHITE, F_BLACK, B_BLACK, FH_WHITE);
    add_item(mylist, "Help", 16, 1, B_WHITE, F_BLACK, B_BLACK, FH_WHITE);
  }

  if(choice == FILE_MENU) {
    add_item(mylist, "New", 3, 3, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Open", 3, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Save", 3, 5, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Save as..", 3, 6, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Exit", 3, 7, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
  if(choice == OPT_MENU) {
    add_item(mylist, "Settings", 9, 3, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Colors", 9, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Refresh_F3", 9, 5, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
  if(choice == HELP_MENU) {
    add_item(mylist, "Help_F1", 18, 3, B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
    add_item(mylist, "About", 18, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }

  if(choice == EXIT_MENU) {
    add_item(mylist, "<YES>", (columns/2)-6, (rows/2)+2, B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
    add_item(mylist, "<NO>", (columns/2)+4, (rows/2)+2, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
   if(choice == ABOUT_MENU) {
    add_item(mylist, "<OK>", (columns/2)-1, (rows/2)+3 , B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
  }
  
}

/*--------------------------*/
/* Display horizontal menu  */
/*--------------------------*/

char horizontal_menu(){
  char temp_char;
  write_str(14, rows, "Press ESC thrice [3x] to exit menu  ", B_CYAN, FH_WHITE);
  update_screen();
  loadmenus(HOR_MENU);		
  temp_char=start_hmenu(&data);
  free_list(mylist);
  return temp_char;
}

/*-------------------------*/
/* Display File menu       */
/*-------------------------*/

void filemenu() {
  int i;
  data.index = OPTION_NIL;
  write_str(14, rows, "Press ESC thrice [3x] to exit menu  ", B_CYAN, FH_WHITE);
  loadmenus(FILE_MENU);
  write_str(1, 1, "File", B_BLACK, F_WHITE);
  draw_window(1, 2, 13, 8, B_WHITE, F_BLACK, 1);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  update_screen();
  free_list(mylist);
  if(data.index == OPTION_5) {
	exitp = confirmation(); //Shall we exit? Global variable! 
  }
  data.index = OPTION_NIL;
  //Restore message in status bar
  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, B_CYAN, F_WHITE);
  }
  write_str(1, rows, ">> [C-Edit]", B_CYAN, FH_WHITE);
  write_str(14, rows, "| F2 or $ -> Menus |", B_CYAN, FH_WHITE);
 
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

void optionsmenu() {
  int i;
  data.index = OPTION_NIL;
  write_str(14, rows, "Press ESC thrice [3x] to exit menu  ", B_CYAN, FH_WHITE);
  loadmenus(OPT_MENU);
  write_str(7, 1, "Options", B_BLACK, F_WHITE);
  draw_window(7, 2, 20, 6, B_WHITE, F_BLACK, 1);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  update_screen();
  free_list(mylist);
  if(data.index == OPTION_3) {
     //Refresh screen
      refresh_screen(1);
 }
  data.index = OPTION_NIL;
   //Restore message in status bar
  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, B_CYAN, F_WHITE);
  }
  write_str(1, rows, ">> [C-Edit]", B_CYAN, FH_WHITE);
  write_str(14, rows, "| F2 or $ -> Menus |", B_CYAN, FH_WHITE);
 
}

/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

void helpmenu() {
  int i;
  data.index = OPTION_NIL;
  write_str(14, rows, "Press ESC thrice [3x] to exit menu  ", B_CYAN, FH_WHITE);
  loadmenus(HELP_MENU);
  write_str(16, 1, "Help", B_BLACK, F_WHITE);
  draw_window(16, 2, 26, 5, B_WHITE, F_BLACK, 1);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  update_screen();
  free_list(mylist);
   if(data.index == OPTION_2) {
     //About info
      about_info();
 }
  data.index = -1;
  //Restore message in status bar
  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, B_CYAN, F_WHITE);
  }
  write_str(1, rows, ">> [C-Edit]", B_CYAN, FH_WHITE);
  write_str(14, rows, "| F2 or $ -> Menus |", B_CYAN, FH_WHITE);
}

/*-------------------------------*/
/* Display Confirmation Dialog   */
/*-------------------------------*/

/* Displays a window to asks user for confirmation */
int confirmation(){
    int ok=0;
    int window_x1=0, window_y1=0, window_x2 = 0, window_y2 =0;
    data.index = OPTION_NIL;
    window_y1 = (rows / 2) - 3;
    window_y2 = (rows/2) + 3;
    window_x1 = (columns/2) - 13;
    window_x2 = (columns/2) +13;
    loadmenus(EXIT_MENU);
    draw_window(window_x1, window_y1, window_x2, window_y2, B_WHITE, F_BLACK, 1);
    write_str(window_x1+3, window_y1+2, "Are you sure you want", F_BLACK, B_WHITE);
     write_str(window_x1+3, window_y1+3, "   want to quit?    ", F_BLACK, B_WHITE);
    
    start_hmenu(&data);
    free_list(mylist);
    if(data.index == OPTION_1)
      ok = CONFIRMATION; //Confirmation has been given
    close_window();
    return ok; 
}

/*--------------------------*/
/* Display About Dialog     */
/*--------------------------*/

int about_info(){
    int ok=0;
    int window_x1=0, window_y1=0, window_x2 = 0, window_y2 =0;
    data.index = OPTION_NIL;
    loadmenus(ABOUT_MENU);
    window_y1 = (rows / 2) - 4;
    window_y2 = (rows/2) + 4;
    window_x1= (columns/2) -8;
    window_x2 = (columns/2) +8;
    draw_window(window_x1, window_y1, window_x2, window_y2, B_WHITE, F_BLACK, 1);
    write_str(window_x1+2, window_y1+1, "  [C-EDIT]  ", F_BLACK, B_WHITE);
    write_str(window_x1+2, window_y1+3, "- Coded by :", F_BLACK, B_WHITE);
    write_str(window_x1+2, window_y1+4, " - V3l0r3k -", F_BLACK, B_WHITE);
    start_hmenu(&data);
    free_list(mylist);
    if(data.index == OPTION_1)
      ok = CONFIRMATION; //Confirmation has been given
    close_window();
    return ok; 
}


void drop_down(char *kglobal){
/* 
   Drop_down loop animation 
   -1 is used when right/left arrow keys are used
   so as to break vertical menu and 
   start the adjacent menu
   kglobal is changed by the menu functions
   FILE_MENU 0
   OPT_MENU 1
   HELP_MENU 2
 */
  
do {     
  if (*kglobal==K_ESCAPE) {
    //Exit drop-down menu with ESC 3x          
    *kglobal=0;          
    main_screen();          
    break;     
  }
  if(data.index == FILE_MENU) {
    filemenu();
      if(*kglobal == K_LEFTMENU) {	      
        data.index = OPT_MENU;
      }	   
      if(*kglobal == K_RIGHTMENU) {	      
        data.index = HELP_MENU;	   
      } 	
  }
  if(data.index == OPT_MENU) {
    optionsmenu();     
     if(*kglobal == K_LEFTMENU) {
       data.index = HELP_MENU;	   
     }	   
     if(*kglobal == K_RIGHTMENU) {
       data.index = FILE_MENU;
     }
  }	
  if(data.index == HELP_MENU) {
    helpmenu();
    if(*kglobal == K_LEFTMENU) {
	 data.index = FILE_MENU;	   
    }	 
    if(*kglobal == K_RIGHTMENU) {    
      data.index = OPT_MENU;	  
    }	 
  }
 } while(*kglobal != K_ENTER);
}

/*---------*/
/* Credits */
/*---------*/

void credits(){
/* Frees memory and displays goodbye message */
  free_buffer();
  resetTerm(); //restore terminal settings from failsafe
  showcursor();
  screencol(0);
  outputcolor(7, 0);
  
  printf(cedit_ascii_1);
  printf(cedit_ascii_2);
  printf(cedit_ascii_3);
  printf(cedit_ascii_4);
  printf(cedit_ascii_5);
  printf(cedit_ascii_6);

  printf("\nCoded by v3l0r3k 2018.\n");

}
