#include "global.h"
#include "rterm.h"
#include "time.h"

// GLOBALS : SCREEN1 : ANIMATED | SCREEN2 : BASE SCREEN
LISTCHOICE *listBox1 = NULL;
SCREENCELL *screen1=NULL;
SCREENCELL *screen2=NULL;
SCREENCELL *screen3=NULL;
NTIMER cursor_timer1;
NTIMER timer2;
NTIMER timer3;
//NTIMER timer4;
SCROLLDATA scrollData;
VLINES *edBuf1=NULL; //Buffer vector of lines(1022 chars)
VLINES tempLine;

//C-Edit Theme
int EDITAREACOL= B_BLUE;
int EDIT_FORECOLOR=FH_WHITE;
int STATUSBAR =B_BLACK;
int STATUSMSG =F_WHITE;
int MENU_PANEL =B_WHITE;
int MENU2_PANEL = B_BLACK;
int MENU_SELECTOR =B_RED;
int MENU_FOREGROUND0 =F_BLACK;
int MENU2_FOREGROUND0 = F_WHITE;
int MENU_FOREGROUND1= FH_WHITE;
int EDITWINDOW_BACK= B_BLUE;
int EDITWINDOW_FORE= F_WHITE;
int SCROLLBAR_BACK= B_WHITE;
int SCROLLBAR_FORE= FH_WHITE;
int SCROLLBAR_SEL= B_CYAN;
int SCROLLBAR_ARR= B_BLACK;
int WINDOW_TITLEB = B_BLACK; 
int WINDOW_TITLEF = F_WHITE;

int new_rows = 0, new_columns = 0, old_rows = 0, old_columns = 0;	// Terminal dimensions
int cursorX = START_CURSOR_X, cursorY = START_CURSOR_Y;
int old_cursorX = START_CURSOR_X, old_cursorY = START_CURSOR_Y;
long posBufX = 0, posBufY = 0;
long oldposBufX = 0, oldposBufY = 0;
long shiftH = 0;
int unwantedChars = 0; //try to avoid printing unwanted characgters from cursor chars counter
		
//TEXT BUFFER POINTERS
int  hdisplayLimit=0; //horizontal scroll, last char position in line that allows scroll
int  currentColumn=0; //horizontal scroll, pointer to current char position in line
long linesinFile=0; //vertical scroll
long vdisplayLimit=0; //vertical scroll, last line that allows scroll
int vdisplayArea = 0; //size in lines of the allowed displayed area // calculated when screen resizes 
int hdisplayArea = 0; //size in lines of the allowed displayed area // calculated when screen resizes 
long currentLine=0; //verticall scroll, pointer to current top line in scroll
int vscrollActive=0; //vertical scroll, vertical scroll is posible
//extern int hscrollActive; //horizontal scroll, horizontal scroll is posible
int     programStatus = 0; //signal for overall program Status
char fileName[MAXFILENAME]; 
char fullPath[MAXFILENAME]; 

FILE *filePointer;
int fileModified=FILE_UNMODIFIED;

char aboutMSG[7][MAXLINE] = {ABOUT_ASC_0,ABOUT_ASC_1,ABOUT_ASC_2,ABOUT_ASC_3,ABOUT_ASC_4,ABOUT_ASC_5};
char help[HELPLINES][MAXLINE] = { HELP0,
	HELP1, HELP2, HELP3,
	HELP4, HELP5, HELP6,
	HELP7, HELP8, HELP9,
	HELP10, HELP11, HELP12,
	HELP13, HELP14, HELP15,
	HELP16, HELP17, HELP18,
	HELP19, HELP20
};
int initCEDIT(){
   get_terminal_dimensions(&new_rows,&new_columns);
   //Init timer
   init_timer(&cursor_timer1,TIMER_SPEED);
   init_timer(&timer2,TIMER_SPEED2);
   init_timer(&timer3,TIMER_SPEED3);
   //init_timer(&timer4,ABOUT_SPEED);
   timer2.ticks=0;
   timer3.ticks=-1;
   //timer4.ticks=0;
   cursor_timer1.ticks=0;
   //Init Text buffer
   VLINES tempLine = {0};
  
   memset(&fileName, '\0',sizeof(fileName)); //Clear fileName
   memset(&tempLine, '\0',sizeof(tempLine)); //Clear memory for temporary line
   //Create one screen in memory
   if (screen1 != NULL) deleteList(&screen1);
   if (screen2 != NULL) deleteList(&screen2);
     //Init 2 : Create 2 Screens for a double buffer  approach  
   old_rows=new_rows;
   old_columns=new_columns;
   create_screen(&screen1);
   create_screen(&screen2);
   //how many lines are vertical display area consists of
   vdisplayArea = new_rows - 4;
   hdisplayArea = new_columns - 2;	
   shiftH=0; 
   return 0;
}	

int _animation(){ 
/* Timer for animations - Display time and clean cursor */
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
  char    temp[4];

  temp[0] = '[';
  temp[1] = ANIMATION[timer2.ticks];
  temp[2] = ']';
  temp[3] = '\0';
    get_terminal_dimensions(&new_rows,&new_columns);
    resetAnsi(0);
    time_str[strlen(time_str) - 1] = '\0';
    //display system time
    update_str(new_columns - strlen(time_str) - 1, 0, time_str, MENU_PANEL,
          MENU_FOREGROUND0);
    update_str(new_columns - strlen(time_str) - 5, 0, temp, MENU_PANEL,
          MENU_FOREGROUND0);
    printf("\n");
    //update only the screen bits that change
    if(timer2.ticks > 6)
      timer2.ticks = 0;
    //return a signal if screen size changes
     if (new_rows != old_rows || new_columns != old_columns)
		return -1;
	else
		return 0;
   
    

}


