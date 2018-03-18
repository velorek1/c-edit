/* 

PROGRAM C Editor - An editor with top-down menus.

@author : Velorek
@version : 1.0
Last modified : 16/3/2018

*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "c_cool.h"
#include "list_choice.h"
#include "screen_buffer.h"

LISTCHOICE *mylist, data;
BCELL  *my_screen;

/* Global Variables */
int     rows=0, columns=0, old_rows=0, old_columns=0; // Terminal dimensions
int     cursorX=2,cursorY=3, timerCursor=0; // Cursor position and Timer
int     exitp = 0; // Exit flag for main loop
char    kglobal=0; // Global variable for menu animation

//Function declarations
void credits();
int main_screen();
void loadmenus(int choice);
char horizontal_menu();
void drop_down(char *kglobal);
void filemenu();
void optionsmenu();
void helpmenu();
int confirmation();
int process_input(int *whereX, int *whereY, char ch);
int special_keys(int *whereX, int *whereY,char *ch);
void draw_cursor(int *whereX, int *whereY, int *timer);
int refresh_screen(int force_refresh);
int timer_1(int *timer1);

//MAIN PROGRAM
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
    //Query if screen dimensions have changed
    refresh_screen(0); 
    /* Wait for key_pressed to read key */
    keypressed = kbhit();
    //Process especial keys and esc-related issues
    esc_key=special_keys(&cursorX,&cursorY,&ch);
    timer_1(&timer1); //Display system time
     if (keypressed==1){ 
  
        ch = readch();
        /* EDIT */
        if (esc_key == 0) {
          //process input and get rid of extra keys
          process_input(&cursorX,&cursorY, ch); //Edit
          keypressed=0;
        } 

     } //end if keypressed

  } while(exitp != 1); //exit flag for the whole program
  credits();
  return 0;
}

/* FUNCTION DEFINITIONS */

/* Edit */
int process_input(int *whereX, int *whereY,char ch)
{
 if (ch != 27) {
   if (ch>31 && ch<128) {
    //only print ASCII characters to screen.
     write_ch(*whereX,*whereY,ch,B_BLUE,F_WHITE);
     write_str(columns-13,rows,"| L:   C:  ",B_CYAN,FH_WHITE);
     write_num(columns-4,rows,*whereX-1,3,B_CYAN,FH_WHITE);
     write_num(columns-9,rows,*whereY-2,4,B_CYAN,FH_WHITE);
     update_screen();
     *whereX = *whereX + 1;
   }
 }
  return 0;
}

int timer_1(int *timer1){
/* Timer for animations - Display time and clean cursor */
  time_t mytime = time(NULL);
  char * time_str = ctime(&mytime);
  if (*timer1 == 5000){
    *timer1=0;
    time_str[strlen(time_str)-1] = '\0';
    write_str(columns-strlen(time_str),1,time_str,F_BLACK,B_WHITE);
    update_smart();
     return 0;
  } else
  {  
    *timer1 = *timer1 +1;
    return 1;
  }
}

int special_keys(int *whereX, int *whereY,char *ch){
/* MANAGE SPECIAL KEYS */ 
if (*ch==27){
  switch (getch()){
    case 'D':
    //Left-arrow key  
      *whereX=*whereX-1;
    break;
    case 'C':
    //Right-arrow key  
      *whereX=*whereX+1;
      break;
    case 81: 
    //F2 -> open drop-down menus
      if (horizontal_menu()==27) 
      {
        //Exit horizontal menu with ESC 3x
        kglobal=27;
        main_screen();
      } 
      /*  Drop-down menu loop */       
      drop_down(&kglobal); //animation
      *ch=0;
      break;
    case 82:
    //F3 -> Refresh screen
           refresh_screen(1);
        *ch=0;
        break;
    default:
       *ch=0;
       break;
    }
   return 1; //esc-key = 1
 } else {
   return 0; //esc-key = 0
 }
}

/* Draw cursor and animate it*/
void draw_cursor(int *whereX, int *whereY, int *timer)
{
/* CURSOR is drawn directly to screen and not to buffer */
   *timer = *timer +1; //increase timer counter for animation
  if (*timer < 10)
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

/* Refresh screen */
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

/* Draw the main screen */
int main_screen() {
  int     i;
 
  //Save previous values
  get_terminal_dimensions(&rows, &columns);
  old_rows = rows;
  old_columns = columns;
 
  //Failsafe just in case it can't find the terminal dimensions
  if(rows == 0)
    rows = 25;	
  if(columns == 0)
    columns = 80;

  bscreen_color(B_BLUE);
  //Draw upper and lower bars
  for(i = 1; i <= columns; i++) {
    write_ch(i, 1, 32, B_WHITE, F_WHITE);
  }

  for(i = 1; i < columns; i++) {
    write_ch(i, rows, 32, B_CYAN, F_WHITE);
  }
  // Text messages
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  write_str(1, rows, ">> [C-Edit]", B_CYAN, FH_WHITE);
  write_str(14, rows, "| F2 -> Menus | F3 -> Resize screen", B_CYAN, FH_WHITE);
  
  /* Frames */
  //window appearance and scroll bar
  for (i=2; i<rows; i++){
    write_ch(columns,i,' ',B_WHITE,F_BLACK);
    write_ch(1,i,'|',B_BLACK,F_WHITE);
  }
  for(i = 2; i < columns; i++) {
    write_ch(i, 2, '-', B_BLACK, F_WHITE);
    write_ch(i, rows-1, ' ', B_BLACK, F_WHITE);
  }
  //horizontal scroll bar
   for(i = 2; i < columns; i++) {
    write_ch(i, rows-1, 32, B_WHITE, F_WHITE);
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

/* Load current menu into circular linked list */ 
void loadmenus(int choice) {
  if(choice == 0) {
    add_item(mylist, "File", 1, 1, B_WHITE, F_BLACK, B_BLACK, FH_WHITE);
    add_item(mylist, "Options", 7, 1, B_WHITE, F_BLACK, B_BLACK, FH_WHITE);
    add_item(mylist, "Help", 16, 1, B_WHITE, F_BLACK, B_BLACK, FH_WHITE);
  }

  if(choice == 1) {
    add_item(mylist, "New", 3, 3, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Open", 3, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Save", 3, 5, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Save as..", 3, 6, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Exit", 3, 7, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
  if(choice == 2) {
    add_item(mylist, "Settings", 9, 3, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Colors", 9, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
    add_item(mylist, "Refresh_F3", 9, 5, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
  if(choice == 3) {
    add_item(mylist, "Help_F1", 18, 3, B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
    add_item(mylist, "About", 18, 4, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }

  if(choice == 5) {
    add_item(mylist, "<YES>", columns-50, rows-12, B_WHITE, F_BLACK, B_BLACK,
	     F_WHITE);
    add_item(mylist, "<NO>", columns-35, rows-12, B_WHITE, F_BLACK, B_BLACK, F_WHITE);
  }
}

/* Horizontal menu */
char horizontal_menu(){
  char temp_char;
  write_str(14, rows, "Press ESC thrice [3x] to exit menu  ", B_CYAN, FH_WHITE);
  update_screen();
  loadmenus(0);		
  temp_char=start_hmenu(&data);
  free_list(mylist);
  return temp_char;
}

/* Display File Menu */
void filemenu() {
  loadmenus(1);
  write_str(1, 1, "File", B_BLACK, F_WHITE);
  draw_window(1, 2, 13, 8, B_WHITE, F_BLACK, 1);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  update_screen();
  free_list(mylist);
  if(data.index == 4) {
	exitp = confirmation(); //Shall we exit? Global variable (to be updated)
  }
  data.index = -1;
}

/* Display Options Menu */
void optionsmenu() {
  loadmenus(2);
  write_str(7, 1, "Options", B_BLACK, F_WHITE);
  draw_window(7, 2, 20, 6, B_WHITE, F_BLACK, 1);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  update_screen();
  free_list(mylist);
  if(data.index == 2) {
     //Refresh screen
      refresh_screen(1);
 }
  data.index = -1;
}

/* Display Help Menu */
void helpmenu() {
  loadmenus(3);
  write_str(16, 1, "Help", B_BLACK, F_WHITE);
  draw_window(16, 2, 26, 5, B_WHITE, F_BLACK, 1);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", B_WHITE, F_BLACK);
  update_screen();
  free_list(mylist);
  data.index = -1;
}

/* Displays a window to asks user for confirmation */
int confirmation(){
    int ok=0;
    loadmenus(5);
    draw_window(16, 6, columns - 20, rows - 10, B_WHITE, F_BLACK, 1);
    write_str(24, rows-16, "Are you sure you want to quit?", F_BLACK, B_WHITE);
    start_hmenu(&data);
    free_list(mylist);
    if(data.index == 0)
      ok = 1; //Confirmation has been given
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
*/
  
do {     
  if (*kglobal==27) {
    //Exit drop-down menu with ESC 3x          
    *kglobal=0;          
    main_screen();          
    break;     
  }
  if(data.index == 0) {
    filemenu();
      if(*kglobal == -1) {	      
        data.index = 1;
      }	   
      if(*kglobal == -2) {	      
        data.index = 2;	   
      } 	
  }
  if(data.index == 1) {
    optionsmenu();     
     if(*kglobal == -1) {
       data.index = 2;	   
     }	   
     if(*kglobal == -2) {
       data.index = 0;
     }
  }	
  if(data.index == 2) {
    helpmenu();
    if(*kglobal == -1) {
	 data.index = 0;	   
    }	 
    if(*kglobal == -2) {    
      data.index = 1;	  
    }	 
  }
 } while(*kglobal != 13);
}

/* Frees memory and displays goodbye message */
void credits(){
  free_buffer();
  resetTerm(); //restore terminal settings from failsafe
  showcursor();
  screencol(0);
  outputcolor(7, 0);
  printf("[C-Edit] coded by Velorek. \n");
}
