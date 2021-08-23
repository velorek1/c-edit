/*
======================================================================
PROGRAM C Editor - An editor with top-down menus.
@author : Velorek
@version : 1.0
Last modified : 17/04/2021 - Added real MilliSecond timer (tm.c)
                 Added home/end keys
                 Clean -wExtra warnings
                 Added signedchar to makefile
======================================================================*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "rterm.h"
#include "listc.h"
#include "scbuf.h"
#include "opfile.h"
#include "uintf.h"
#include "fileb.h"
#include "keyb.h"
#include "tm.h"
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

//ABOUT - ASCII ART
#define ABOUT_ASC_1 "        _   __       \n"
#define ABOUT_ASC_2 "       /  _|__ _|.___\n"
#define ABOUT_ASC_3 "       \\_  |__(_]| |  v0.1 \n       Coded   by  V3l0rek\n "
#define ANIMATION "||//--\\\\"

//USER-DEFINED MESSAGES
#define UNKNOWN "UNTITLED"
#define WINDOWMSG "DISPLAY IS TOO SMALL. PLEASE, RESIZE WINDOW"
#define STATUS_BAR_MSG1  " [C-Edit] | F2,CTRL+L: MENU | F1: HELP"
#define STATUS_BAR_MSG2 " [C-Edit] Press ESC to exit menu.             "
#define STATUS_BAR_MSG3 " ENTER: SELECT | <- -> ARROW KEYS             "
#define WLEAVE_MSG "\n       Are you sure\n    you want to quit?"
#define WSAVE_MSG ":\nFile saved successfully!"
#define WSAVELABEL_MSG "[-] File:"
#define WINFO_NOPEN "Error:\nThere isn't any file open!"
#define WINFO_SIZE "-File size: "
#define WINFO_SIZE2 "\n-No. of lines: "
#define WINFO_SIZE3 "\n-File name: "
#define WCHECKFILE2_MSG " File does not exist!  \n\n A new buffer will be created. \n"
#define WCHECKFILE_MSG " This file isn't a  \n text file. Program \n may crash. Open anyway?"
#define WINFONOTYET_MSG "Not implemented yet!"
#define WCHECKLINES_MSG " File longer than 3000 \n lines. You'll view those \n lines as read Mode! "
#define WMODIFIED_MSG " File has been modified\n Save current buffer?"
#define WFILEEXISTS_MSG " File exists. \n Overwrite?"
#define WFILEINREADMODE_MSG " File is on read mode. \n"

//MISC. CONSTANTS
#define EXIT_FLAG -1
#define TAB_DISTANCE 8      //How many spaces TAB key will send.
#define START_CURSOR_X 2
#define START_CURSOR_Y 3
#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80
#define K_LEFTMENU -1       //Left arrow key pressed while in menu
#define K_RIGHTMENU -2      //Right arrow key pressed while in menu
#define MAX_TEXT 150
#define MAX_PATH 1024

//MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define OPT_MENU 1
#define HELP_MENU 2
#define YESNO_MENU 3
#define OK_MENU 4
#define MAX_FILENAME 100

//DROP-DOWN MENUS
#define OPTION_1 0
#define OPTION_2 1
#define OPTION_3 2
#define OPTION_4 3
#define OPTION_5 4
#define OPTION_NIL -1       //Reset option
#define CONFIRMATION 1

// DISPLAY CONSTANTS
#define FILL_CHAR 32

//Temporary implementation- Static Buffer
//EDIT AREA CONSTANTS
//Edit buffer is currently 3000 lines x 500 chars

#define MAX_CHARS 500       // 500 chars per line
#define MAX_LINES 3000      // 3000 lines per page (max mem 32750)

#define CHAR_NIL '\0'
#define END_LINE_CHAR 0x0A  // $0A
#define VSCROLL_ON 1
#define VSCROLL_OFF 0
#define DIRECTION_DOWN 1
#define DIRECTION_UP 0

//FILE CONSTANTS
#define FILE_MODIFIED 1
#define FILE_UNMODIFIED 0
#define FILE_READMODE 2

/*====================================================================*/
/* TYPEDEF DEFINITIONS                                                */
/*====================================================================*/

typedef struct _charint {
  char    ch;
  char     specialChar; //to account for chars with accents in Unix
} CHARINT;

typedef struct _editbuffer {
  CHARINT charBuf[MAX_LINES];
} EDITBUFFER;

typedef struct _editScroll {
  long totalLines; //Maximum no. of lines in File or Buffer
  long displayLength; //number of current display Lines
  float scrollRatio; //used for scroll indicator
  long scrollLimit; //limit of scroll
  long scrollPointer; //last pointed/shown line
  long pagePointer; //last page displayed
  char scrollActiveV; //whether there are lines to scroll or not.
  long bufferX; //coordinates within the buffer
  long bufferY; //coordinates within the buffer
} EDITSCROLL;


/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/

LISTCHOICE *mylist, data; //menus handler
SCREENCELL *my_screen; //display handler
SCROLLDATA openFileData; //openFile dialog
EDITBUFFER editBuffer[MAX_LINES]; //edit buffer
EDITSCROLL editScroll; //scroll Values

FILE   *filePtr;

int     rows = 0, columns = 0, old_rows = 0, old_columns = 0;   // Terminal dimensions
long    cursorX = START_CURSOR_X, cursorY = START_CURSOR_Y; //Cursor position
long    oldX = START_CURSOR_X, oldY = START_CURSOR_Y; //Cursor position blink
int     timerCursor = 0;    // Timer
char    kglobal = 0;        // Global variable for menu animation
char    currentFile[MAX_TEXT];
long    limitRow = 0, limitCol = 0; // These variables account for the current limits of the buffer.
int     c_animation = 0;    //Counter for animation
long    maxLineFile = 1;        //Last line of file
char    currentPath[MAX_PATH];
//FLAGS
int     exitp = 0;      // Exit flag for main loop
int     fileModified = FILE_UNMODIFIED; //Have we modified the buffer?
char    timerOnOFF = 1;
int forceBufferUpdate = 0; //To allow a smoother scroll animation.
NTIMER  _timer1; //Timer for animations and screen update
/*====================================================================*/
/* PROTOTYPES OF FUNCTIONS                                            */
/*====================================================================*/

//Display prototypes
void    credits();
int     main_screen();
int     refresh_screen(int force_refresh);
void    flush_editarea();
void    cleanStatusBar();

//Timers
void    draw_cursor(long whereX, long whereY, long oldX, long oldY);
//int     timer_1(int *timer1, long whereX, long whereY, char onOff);
int     _tick();
void    update_indicators();

//Dialogs & menus
void    filemenu();
void    optionsmenu();
void    helpmenu();
int     confirmation();
int     about_info();
int     help_info();
void    drop_down(char *kglobal);
char    horizontal_menu();
int     newDialog(char fileName[MAX_TEXT]);
int     saveDialog(char fileName[MAX_TEXT]);
int     saveasDialog(char fileName[MAX_TEXT]);
int     openFileHandler();
int     fileInfoDialog();

//Keyhandling and input prototypes
int     process_input(EDITBUFFER editBuffer[MAX_LINES], long *whereX,
              long *whereY, char ch);
int     special_keys(long *whereX, long *whereY, char ch);

//Edit prototypes
void    cleanBuffer(EDITBUFFER editBuffer[MAX_LINES]);
int     writetoBuffer(EDITBUFFER editBuffer[MAX_LINES], long whereX,
              long whereY, char ch);
int     findEndline(EDITBUFFER editBuffer[MAX_LINES], long line);

//Scroll prototypes
short   checkScrollValues(long lines);

//File-handling prototypes
int     writeBuffertoFile(FILE * filePtr,
              EDITBUFFER editBuffer[MAX_LINES]);
int     writeBuffertoDisplay(EDITBUFFER editBuffer[MAX_LINES]);
int     writeBuffertoDisplayRaw(EDITBUFFER editBuffer[MAX_LINES]);
int     filetoBuffer(FILE * filePtr, EDITBUFFER editBuffer[MAX_LINES]);
int     handleopenFile(FILE ** filePtr, char *fileName, char *oldFileName);
int     createnewFile(FILE ** filePtr, char *fileName, int checkFile);

/*====================================================================*/
/* MAIN PROGRAM - CODE                                                */
/*====================================================================*/

int main(int argc, char *argv[]) {
  char    ch = 0, oldchar = 0;
  char *ok1 = NULL;
  int     esc_key = 0;      //To control key input and scan for keycodes.
  int     keypressed = 0;

  /*------------------------INITIAL VALUES----------------------------*/
  hidecursor();
  pushTerm();           //Save current terminal settings in failsafe
  create_screen();      //Create screen buffer to control display
  cleanBuffer(editBuffer);
  clearString(currentFile, MAX_TEXT);
  strcpy(currentFile, UNKNOWN);
  checkConfigFile(-1);      //Check config file for colorScheme. -1 -> first time
  ok1=getcwd(currentPath, sizeof(currentPath)); //Save current path
  ok1++; //to avoid warning
  editScroll.scrollActiveV = VSCROLL_OFF; //Scroll is inactive.
  editScroll.totalLines = 1; //There is just one line active in buffer
  editScroll.bufferX = 1;
  editScroll.bufferY = 1;
  openFileData.itemIndex = 0; //No file has been opened yet. Memory isn't freed.
  //editBuffer1 = addRear(editBuffer1, newEditCell(20,-67));
  _timer1.ms = 30;  // Timer 1 - Clock & animation
  _timer1.ticks = 0;
  main_screen();        //Draw screen
  update_screen();
  save_buffer();
  resetch();            //Clear keyboard and sets ENTER = 13
  /*------------------------------------------------------------------*/


  /*------------------------CHECK ARGUMENTS----------------------------*/
  if(argc > 1) {
    //Does the file exist? Open or create?
    if(file_exists(argv[1]) == 1) {
      //open
      clearString(currentFile, MAX_TEXT);
      strcpy(currentFile, argv[1]);
      handleopenFile(&filePtr, currentFile, UNKNOWN);
    } else {
      //create
      clearString(currentFile, MAX_TEXT);
      strcpy(currentFile, argv[1]);
      createnewFile(&filePtr, currentFile, 0);
    }
  }
  /*------------------------------------------------------------------*/

  /*------------------------MAIN PROGRAM LOOP-------------------------*/
  do {
    /* CURSOR is drawn in RAW MODE, i.e, directly to screen */
    draw_cursor(cursorX, cursorY, oldX, oldY);
    /* Query if screen dimensions have changed and if so, resize screen */
    refresh_screen(0);
    /* Timer for animation to show system time and update screen */
    if (timerC(&_timer1) == 1) {
        _tick();
        if (screenChanged()){
        update_smart();
            save_buffer();
    }
    }
   /* Wait for key_pressed to read key */
   keypressed = kbhit();
   if(keypressed == 1) {
      keypressed = 0;
      /* Process SPECIAL KEYS and other ESC-related issues */
      esc_key = special_keys(&cursorX, &cursorY, ch);

      //If arrow keys are used repeatedly. This avoids printing unwanted chars.
      if(oldchar == K_ESCAPE)
    esc_key = 1;

      oldchar = ch;

      ch = readch();

      /* EDIT */
      if(esc_key == 0) {
    //Process input and get rid of extra characters
    process_input(editBuffer, &cursorX, &cursorY, ch);  //Edit
    keypressed = 0;
      }
    } else {
      //Keypressed = 0 - Reset values
      oldX = cursorX;
      oldY = cursorY;
      esc_key = 0;
      ch = 0;
      oldchar = 0;
    }
  } while(exitp != EXIT_FLAG);  //exit flag for the whole program
  /*------------------------------------------------------------------*/

  //CREDITS
  if(filePtr != NULL) {
    closeFile(filePtr);
  }
  credits();
  return 0;
}

/*====================================================================*/
/* FUNCTION DEFINITIONS                                               */
/*====================================================================*/

/*-----------------------------------------*/
/* Draw cursor on screen and animate it    */
/*-----------------------------------------*/

void draw_cursor(long whereX, long whereY, long oldX, long oldY) {
/* CURSOR is drawn directly to screen and not to buffer */
  long     positionX = 0, limitCol = 0, positionY = 0, oldPositionX=0, oldPositionY=0;
  char    currentChar = FILL_CHAR;
  char    specialChar;


  //Calculate position
  limitCol = findEndline(editBuffer, editScroll.bufferY-1);
  positionX = editScroll.bufferX-1;
  positionY = editScroll.bufferY-1;
 //clean previous cursor
  if (oldX != whereX || oldY!=whereY){
     if(positionX < limitCol || oldY!=whereY) {
      oldPositionX = positionX+(oldX - whereX);
      oldPositionY = positionY+(oldY - whereY);
      currentChar = editBuffer[oldPositionY].charBuf[oldPositionX].ch;
     } else {
      currentChar = FILL_CHAR;
     }
   gotoxy(oldX, oldY);
   outputcolor(EDIT_FORECOLOR, EDITAREACOL);
  //Is it an accent or special char?
    if(currentChar < 0) {
      specialChar = editBuffer[oldPositionY].charBuf[oldPositionX].specialChar;
      printf("%c%c", specialChar,
         currentChar);
    } else {
     //Careful with null char or enter or tab chars
     if(currentChar == 0 || currentChar == 10 || currentChar == 9) currentChar=FILL_CHAR;
      printf("%c", currentChar);
    }
  }

   //draw new cursor
    gotoxy(whereX, whereY);
    outputcolor(EDIT_FORECOLOR, EDITAREACOL);
    printf("|");
    //Is the cursor at the end or in the middle of text?
    if(positionX < limitCol) {
      currentChar = editBuffer[positionY].charBuf[positionX].ch;
    } else {
      currentChar = FILL_CHAR;
    }
   //Display character in yellow.
    gotoxy(whereX, whereY);
    outputcolor(F_YELLOW, EDITAREACOL);
    //Is it an accent or special char?
    if(currentChar < 0) {
      specialChar = editBuffer[positionY].charBuf[positionX].specialChar;
      printf("%c%c", specialChar,
         editBuffer[positionY].charBuf[positionX].ch);
    } else {
     //Careful with null char or enter
     if(currentChar == 0 || currentChar == 10) currentChar=FILL_CHAR;
      printf("%c", currentChar);
    }
    resetAnsi(0);
}

/*-----------------------------------------*/
/* Timer 1 Animation. Clock and cursor     */
/*-----------------------------------------*/

int _tick() {
/* Timer for animations - Display time and clean cursor */
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
  char    temp[4];

  temp[0] = '[';
  temp[1] = ANIMATION[c_animation];
  temp[2] = ']';
  temp[3] = '\0';
  //save_buffer();
    time_str[strlen(time_str) - 1] = '\0';
    //display system time
    write_str(columns - strlen(time_str), 1, time_str, MENU_PANEL,
          MENU_FOREGROUND0);
    write_str(columns - strlen(time_str) - 5, 1, temp, MENU_PANEL,
          MENU_FOREGROUND0);
    update_indicators();    //update position, indicators display
    //update only the screen bits that change
    c_animation++;  //star animation
    if(c_animation > 6)
      c_animation = 0;
    return 0;
}


/*----------------------------------------- */
/* Update cursor position display on screen */
/*----------------------------------------- */

void update_indicators() {
/*
   Update cursor position display on screen.
   Values are float to save decimal points.
- Formulas:
   scrollRatio = totalFileLines / displayLength;
   currentPosition = bufferY / scrollRatio;
   percentage = (currentPosition * 100) / displayLength;
   To adjust to the bar dimensions:
   scrollBar = ((displayLength-3) * percentage) / 100;
*/
  int i;
  float scrollIndicator=1.0, positionY, displayLength;
  float percentage, scrollBar;
  write_str(columns - 24, rows, "| L:        C:     ", STATUSBAR, STATUSMSG);
  write_num(columns - 10, rows, editScroll.bufferX, 3, STATUSBAR, STATUSMSG);
  write_num(columns - 20, rows, editScroll.bufferY, 4, STATUSBAR, STATUSMSG);
  //Scroll indicator and percentage display
  if (editScroll.scrollActiveV == VSCROLL_ON){
     //Clean scroll bar
     for(i = 4; i < rows-2; i++) {
      write_ch(columns, i, ' ', SCROLLBAR_BACK, SCROLLBAR_FORE);
     }
     positionY = editScroll.bufferY;
     displayLength = editScroll.displayLength;
     scrollIndicator = positionY / editScroll.scrollRatio;
     percentage = (scrollIndicator * 100) / displayLength;
     scrollBar = ((displayLength-3) * percentage)/100;
     if (positionY == 1) percentage = 0;
     write_ch(columns, 4+scrollBar, '*', SCROLLBAR_SEL, SCROLLBAR_FORE);
     write_str(columns-5,rows, "    ", STATUSBAR,F_BLACK);
     i=write_num(columns-5,rows, percentage, 3, STATUSBAR,F_YELLOW);
     write_ch(columns-5+i,rows, '%', STATUSBAR,F_YELLOW);
  }
}

/*-----------------*/
/* Refresh screen  */
/*-----------------*/

int refresh_screen(int force) {
/* Query terminal dimensions again and check if resize
   has been produced */
  get_terminal_dimensions(&rows, &columns);
  editScroll.displayLength = rows - 4; //update Scroll
  if (columns <55 && rows <17){
     //free_buffer();       //delete structure from memory
     resetAnsi(0);
     gotoxy(1,1);
     printf("%s", WINDOWMSG);
     timerOnOFF=0;
  }
  else{
  if(rows != old_rows || columns != old_columns || force == 1 || force == -1) {
      if(force != -1) {
        timerOnOFF=1;
        free_buffer();      //delete structure from memory for resize
        create_screen();        //create new structure
        main_screen();      //Refresh screen in case of resize
        checkScrollValues(editScroll.totalLines);      //Update scroll values
        writeBuffertoDisplay(editBuffer);
        update_screen();
       } else{
      //only update edit area to avoid flickering effect
       flush_editarea();
       writeBuffertoDisplay(editBuffer);
       update_screen();
       return 1;
       }
  } else {
    return 0;
  }
  }
return 0;
}


/*--------------*/
/* EDIT Section */
/*------------- */

int findEndline(EDITBUFFER editBuffer[MAX_LINES], long line) {
  char    ch = 0;
  long     i = 0;
  int     result = 0;

  do {
    ch = editBuffer[line].charBuf[i].ch;
    i++;
    if(ch == CHAR_NIL || ch == END_LINE_CHAR)
      break;
  } while(i < MAX_CHARS);
  result = i;
  ch = 0;
  return result;
}

int process_input(EDITBUFFER editBuffer[MAX_LINES], long *whereX,
          long *whereY, char ch) {
/* EDIT FUNCTIONS */
  char     accentchar[2];
  long     counter = 0;
  long     newPosition = 0;
  long     oldPosition = 0;
  long     positionX = 0;
  long     positionY = 0;
  int      i = 0;

  if(ch != K_ESCAPE) {

    //Calculate position values
    limitCol = findEndline(editBuffer, editScroll.bufferY-1);

    positionX = editScroll.bufferX-1;
    positionY = editScroll.bufferY-1;
    accentchar[0] = 0;
    accentchar[1] = 0;

    /* ---------------------------------------- */
    /*
       READ CHARS WITH AND WITHOUT ACCENTS.
       Accent value is stored in the
       specialChar field of every item in the
       buffer.
       Example:
       รก : specialChar  = -61
       Char = -95
       a : specialChar = 0
       Char = 97
     */

    /* ---------------------------------------- */
    //Check whether we are on Readmode
   if (fileModified != FILE_READMODE) {
    if((ch > 31 && ch < 127) || ch < 0) {
      //if a char has been read.
      read_accent(&ch, accentchar);

      //ADD SPACES IF CURSOR IS NOT AT THE END OF THE LINE
      if(positionX > limitCol) {
    for(i = limitCol - 1; i <= positionX; i++) {
      writetoBuffer(editBuffer, i, positionY, ' ');
      write_ch(i + START_CURSOR_X, *whereY, ' ', EDITAREACOL,
           EDIT_FORECOLOR);
    }
      }
      //INSERT CHARS AT THE END OF LINE
      if(*whereX < MAX_CHARS && positionX >= limitCol - 1) {
    if(accentchar[0] != 0) {
      //Special char ? print the two values to screen buffer.
      write_ch(*whereX, *whereY, accentchar[0], EDITAREACOL,
           EDIT_FORECOLOR);
      write_ch(*whereX, *whereY, accentchar[1], EDITAREACOL,
           EDIT_FORECOLOR);
    } else {
          write_ch(*whereX, *whereY, accentchar[1], EDITAREACOL,
           EDIT_FORECOLOR);
    }
    writetoBuffer(editBuffer, positionX, positionY, accentchar[0]);
    writetoBuffer(editBuffer, positionX, positionY, accentchar[1]);
    *whereX = *whereX + 1;
        editScroll.bufferX++;

      }
      //INSERT CHAR IN THE MIDDLE OF THE LINE
      if(*whereX < MAX_CHARS && positionX < limitCol - 1) {
    //move all chars one space to the side
    counter = 0;
    //move characters to the side
    while(counter <= (limitCol - positionX)) {
      newPosition = limitCol - counter + 1;
      oldPosition = limitCol - counter;
      editBuffer[positionY].charBuf[newPosition].ch =
          editBuffer[positionY].charBuf[oldPosition].ch;
      editBuffer[positionY].charBuf[newPosition].specialChar =
          editBuffer[positionY].charBuf[oldPosition].specialChar;
      //Don't print null characters to string or 0x0A
      if(editBuffer[positionY].charBuf[oldPosition].ch != CHAR_NIL &&
         editBuffer[positionY].charBuf[oldPosition].ch !=
         END_LINE_CHAR) {
        if(accentchar[0] != 0
           || editBuffer[positionY].charBuf[oldPosition].specialChar !=
           0) {
          //Special char ? print the two values to screen buffer.
          write_ch(newPosition + START_CURSOR_X, *whereY,
               editBuffer[positionY].charBuf[newPosition].
               specialChar, EDITAREACOL, EDIT_FORECOLOR);
          write_ch(newPosition + START_CURSOR_X, *whereY,
               editBuffer[positionY].charBuf[newPosition].ch,
               EDITAREACOL, EDIT_FORECOLOR);
        } else {
          write_ch(newPosition + START_CURSOR_X, *whereY,
               editBuffer[positionY].charBuf[newPosition].ch,
               EDITAREACOL, EDIT_FORECOLOR);
        }
      }
      counter++;
    }
    //insert new SPECIAL char
    if(accentchar[0] != 0) {
      //Special char ? print the two values to screen buffer.
      write_ch(*whereX, *whereY, accentchar[0], EDITAREACOL,
           EDIT_FORECOLOR);
      write_ch(*whereX, *whereY, accentchar[1], EDITAREACOL,
           EDIT_FORECOLOR);
    } else {
      write_ch(*whereX, *whereY, accentchar[1], EDITAREACOL,
           EDIT_FORECOLOR);
    }
    writetoBuffer(editBuffer, positionX, positionY, accentchar[0]);
    writetoBuffer(editBuffer, positionX, positionY, accentchar[1]);

    *whereX = *whereX + 1;
        editScroll.bufferX++;
      }
      //update_screen();
      fileModified = FILE_MODIFIED;
    }

    if(ch == K_ENTER) {
      //RETURN - ENTER
      if(*whereY < rows - 2) {
    writetoBuffer(editBuffer, positionX, positionY, END_LINE_CHAR);
    *whereY = *whereY + 1;
    *whereX = START_CURSOR_X;
        editScroll.bufferX = 1;
        editScroll.bufferY++;
        editScroll.totalLines = editScroll.totalLines + 1;
    fileModified = FILE_MODIFIED;
      }
    }

    if(ch == K_BACKSPACE) {
      //BACKSPACE key
      write_ch(*whereX, *whereY, ' ', EDITAREACOL, EDITAREACOL);
      editBuffer[positionY].charBuf[positionX - 1].ch = CHAR_NIL;
      editBuffer[positionY].charBuf[positionX - 1].specialChar = CHAR_NIL;
      //Remove line if we continue pressing backspace
      if(*whereX == START_CURSOR_X && *whereY > START_CURSOR_Y) {
    *whereY = *whereY - 1;
    *whereX =
        findEndline(editBuffer,
            *whereY - START_CURSOR_Y) + START_CURSOR_X;
        editScroll.bufferY--;
        editScroll.bufferX = findEndline(editBuffer,editScroll.bufferY);
      }
      if(*whereX > START_CURSOR_X){
        *whereX = *whereX - 1;
        editScroll.bufferX--;
      }
    }

    if(ch == K_TAB) {
      //TAB key
      if(*whereX < columns - TAB_DISTANCE) *whereX = *whereX + TAB_DISTANCE;
      editScroll.bufferX = editScroll.bufferX + TAB_DISTANCE;
      writetoBuffer(editBuffer, positionX, positionY, K_TAB);
    }
    //*ch=0;
  }
    if(ch == K_CTRL_L) {
      //Akin to F2
      if(horizontal_menu() == K_ESCAPE) {
    //Exit horizontal menu with ESC 3x
    kglobal = K_ESCAPE;
    //main_screen();
      }
      /*  Drop-down menu loop */
      drop_down(&kglobal);  //animation
    }
    if(ch == K_CTRL_C) {
      //Ask for confirmations CTRL-C -> Exit
      exitp = confirmation();   //Shall we exit? Global variable!
    }
    if(ch == K_CTRL_H) {
      //Change color Scheme
      setColorScheme(colorsWindow(mylist, COLORSWTITLE));
      refresh_screen(1);
    }
  }
  return 0;
}

/*-----------------------------------------*/
/* Manage keys that send a ESC sequence    */
/*-----------------------------------------*/

int special_keys(long *whereX, long *whereY, char ch) {
/* MANAGE SPECIAL KEYS */
/*
   New implementation: Trail of chars found in keyboard.c
   If K_ESCAPE is captured read a trail up to 5 characters from the console.
   This is to control the fact that some keys may change
   according to the terminal and expand the editor's possibilities.
   Eg: F2 can be either 27 79 81 or 27 91 91 82.
*/

  int     esc_key = 0;
  char    chartrail[5];
  long    oldX;
  if(ch == K_ESCAPE) {
    read_keytrail(chartrail);   //Read trail after ESC key

    //Check key trails for special keys.

    //FUNCTION KEYS : F1 - F4
    if(strcmp(chartrail, K_F2_TRAIL) == 0 ||
       strcmp(chartrail, K_F2_TRAIL2) == 0) {
      //update screen
      if(horizontal_menu() == K_ESCAPE) {
    //Exit horizontal menu with ESC 3x
    kglobal = K_ESCAPE;
    //main_screen();
      }
      //  Drop-down menu loop */
      drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_F3_TRAIL) == 0 ||
          strcmp(chartrail, K_F3_TRAIL2) == 0) {
      refresh_screen(1);
      writeBuffertoDisplay(editBuffer);
    } else if(strcmp(chartrail, K_F1_TRAIL) == 0 ||
          strcmp(chartrail, K_F1_TRAIL2) == 0) {
      help_info();
    } else if(strcmp(chartrail, K_F4_TRAIL) == 0 ||
          strcmp(chartrail, K_F4_TRAIL2) == 0) {
      about_info();
      // ARROW KEYS
    } else if(strcmp(chartrail, K_LEFT_TRAIL) == 0) {
      //Left-arrow key
      if(*whereX > 2){
    *whereX = *whereX - 1;
        editScroll.bufferX--;
      }
    } else if(strcmp(chartrail, K_RIGHT_TRAIL) == 0) {
      //Right-arrow key
      if(*whereX < columns - 1){
    *whereX = *whereX + 1;
        editScroll.bufferX++;
      }
    } else if(strcmp(chartrail, K_UP_TRAIL) == 0) {
      //Up-arrow key
      if(*whereY > 3) {
    *whereY = *whereY - 1;
        editScroll.bufferY--;
      }
      if (editScroll.scrollActiveV == VSCROLL_ON && *whereY == START_CURSOR_Y ) {
         if (editScroll.scrollPointer > 0){
            editScroll.scrollPointer = editScroll.scrollPointer - 1;
            editScroll.bufferY--;
            oldX = *whereX;
        writeBuffertoDisplayRaw(editBuffer);
        forceBufferUpdate = 1;
            *whereY = START_CURSOR_Y;
            *whereX = oldX;
          }
      }

    } else if(strcmp(chartrail, K_DOWN_TRAIL) == 0) {
      //Down-arrow key
      if(*whereY < rows - 2 && editScroll.totalLines > *whereY - START_CURSOR_Y+1) {
    *whereY = *whereY + 1;
        editScroll.bufferY++;
      }
      if (editScroll.scrollActiveV == VSCROLL_ON && *whereY == rows-2 ) {
          if (editScroll.scrollPointer < editScroll.scrollLimit){
            editScroll.scrollPointer = editScroll.scrollPointer + 1;
            editScroll.bufferY++;
            oldX = *whereX;
        writeBuffertoDisplayRaw(editBuffer);
        forceBufferUpdate = 1;
            *whereY = rows-2;
            *whereX = oldX;
          }
      }
     } else if(strcmp(chartrail, K_PAGEDOWN_TRAIL) == 0) {
      //Page Down key
      oldX = *whereX;
      if (editScroll.scrollActiveV == VSCROLL_ON) {
         if (editScroll.scrollPointer+editScroll.displayLength <= editScroll.scrollLimit) {
            editScroll.scrollPointer = editScroll.scrollPointer + (editScroll.displayLength);
            editScroll.bufferY = editScroll.bufferY + editScroll.displayLength;
          } else{
            editScroll.scrollPointer = editScroll.scrollLimit;
            editScroll.bufferY = editScroll.scrollLimit+1;
          }
      writeBuffertoDisplay(editBuffer);
          *whereY = START_CURSOR_Y;
          *whereX = oldX;
      }
     } else if(strcmp(chartrail, K_PAGEUP_TRAIL) == 0) {
      //Page Down key
      oldX = *whereX;
      if (editScroll.scrollActiveV == VSCROLL_ON) {
         if (editScroll.scrollPointer > editScroll.displayLength) {
            editScroll.scrollPointer = editScroll.scrollPointer - (editScroll.displayLength);
            editScroll.bufferY = editScroll.bufferY - editScroll.displayLength;
          } else{
            editScroll.scrollPointer = 0;
            editScroll.bufferY = 1;
          }
          //*whereY = rows-2;
      writeBuffertoDisplay(editBuffer);
          *whereX = oldX;
          *whereY = START_CURSOR_Y;
      }
     }else if(strcmp(chartrail, K_HOME_TRAIL) == 0 || strcmp(chartrail, K_HOME_TRAIL2) == 0 ) {
           editScroll.scrollPointer = 0;
           editScroll.bufferY = 1;
       writeBuffertoDisplay(editBuffer);
           //*whereX = oldX;
           *whereY = START_CURSOR_Y;
     }else if(strcmp(chartrail, K_END_TRAIL) == 0 || strcmp(chartrail, K_END_TRAIL2) == 0 ) {
           editScroll.scrollPointer = editScroll.scrollLimit;
           editScroll.bufferY = editScroll.scrollLimit+(rows-4);
       writeBuffertoDisplay(editBuffer);
           //*whereX = oldX;
           *whereY = rows-2;
    } else if(strcmp(chartrail, K_DELETE) == 0) {
      //delete button;
    } else if(strcmp(chartrail, K_ALT_F) == 0) {
      data.index=FILE_MENU;
      drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_ALT_P) == 0) {
      data.index=OPT_MENU;
      drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_ALT_H) == 0) {
      data.index=HELP_MENU;
      drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_ALT_O) == 0) {
      openFileHandler();    //Open file Dialog
    } else if(strcmp(chartrail, K_ALT_N) == 0) {
      newDialog(currentFile);   // New file
      refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_A) == 0) {
      saveasDialog(currentFile);    //Save as.. file
      refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_D) == 0) {
      fileInfoDialog();     //Info file
    } else if(strcmp(chartrail, K_ALT_W) == 0) {
      if(strcmp(currentFile, UNKNOWN) == 0)
    saveasDialog(currentFile);  //Write to file
      else {
    saveDialog(currentFile);
      }
      refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_X) == 0) {
      if(fileModified == 1)
    exitp = confirmation(); //Shall we exit? Global variable!
      else
    exitp = EXIT_FLAG;
    }
    esc_key = 1;
  } else {
    //Reset esc_key
    esc_key = 0;
  }
  return esc_key;
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
  screen_color(EDITAREACOL);
  //Draw upper and lower bars
  for(i = 1; i <= columns; i++) {
    write_ch(i, 1, FILL_CHAR, MENU_PANEL, MENU_PANEL);
  }

  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, STATUSBAR, STATUSMSG);
  }
  // Text messages
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);

  /* Frames */
  //window appearance and scroll bar
  for(i = 2; i < rows; i++) {
    write_ch(columns, i, ' ', SCROLLBAR_BACK, SCROLLBAR_FORE);  //Scroll bar
    write_ch(1, i, NVER_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE);    //upper vertical line box-like char
  }
  for(i = 2; i < columns; i++) {
    write_ch(i, 2, NHOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE);    //horizontal line box-like char
    write_ch(i, rows - 1, ' ', EDITWINDOW_BACK, EDITWINDOW_FORE);
  }
  write_ch(1, 2, NUPPER_LEFT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE); //upper-left box-like char
  //horizontal scroll bar
  for(i = 2; i < columns; i++) {
    write_ch(i, rows - 1, FILL_CHAR, SCROLLBAR_BACK, SCROLLBAR_FORE);
  }
  //Window-appearance
  write_ch(columns, 2, NUPPER_RIGHT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE);  //right window corner
  write_ch(columns, rows - 1, NLOWER_RIGHT_CORNER, EDITWINDOW_BACK,
       EDITWINDOW_FORE);
  write_ch(1, rows - 1, NLOWER_LEFT_CORNER, EDITWINDOW_BACK,
       EDITWINDOW_FORE);

  //Center and diplay file name
  write_str((columns / 2) - (strlen(currentFile) / 2), 2, currentFile,
        MENU_PANEL, MENU_FOREGROUND0);
  //Scroll symbols
  write_ch(columns, 3, '^', SCROLLBAR_ARR, SCROLLBAR_FORE);
  write_ch(columns, rows - 2, 'v', SCROLLBAR_ARR, SCROLLBAR_FORE);
  write_ch(columns, 4, '*', SCROLLBAR_SEL, SCROLLBAR_FORE);
  write_ch(3, rows - 1, '*', SCROLLBAR_SEL, SCROLLBAR_FORE);
  write_ch(2, rows - 1, '<', SCROLLBAR_ARR, SCROLLBAR_FORE);
  write_ch(columns - 1, rows - 1, '>', SCROLLBAR_ARR, SCROLLBAR_FORE);

  checkScrollValues(editScroll.totalLines); //make calculations for scroll.
  //Write editBuffer
  writeBuffertoDisplay(editBuffer);
  return 0;
}

/*-------------------------*/
/* Only refresh edit Area  */
/*-------------------------*/

void flush_editarea() {
int i,j;
  //Paint blue edit area
  for(j = START_CURSOR_Y; j < rows - 1; j++)
    for(i = START_CURSOR_X; i < columns - 1; i++)
    {
      gotoxy(i,j);
      outputcolor(EDITAREACOL,EDITAREACOL);
      printf("%c",FILL_CHAR);
      write_ch(i,j,FILL_CHAR,EDITAREACOL,EDITWINDOW_FORE);
     }
  for(i = 2; i < columns; i++) {
    write_ch(i, 2, NHOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE);    //horizontal line box-like char
  }
  //Center and diplay file name
  write_str((columns / 2) - (strlen(currentFile) / 2), 2, currentFile,
        MENU_PANEL, MENU_FOREGROUND0);
  //writeBuffertoDisplay(editBuffer);
  update_screen();
  update_smart();
}

/*-------------------*/
/* Clean Status Bar  */
/*-------------------*/


//Clean Status bar
void cleanStatusBar(){
  int i;
  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, STATUSBAR, STATUSMSG);
  }
}

/*--------------------------*/
/* Display horizontal menu  */
/*--------------------------*/

char horizontal_menu() {
  char    temp_char;
  kglobal=-1;
  if (forceBufferUpdate == 1 ) {
      writeBuffertoDisplay(editBuffer);
      forceBufferUpdate = 0;
   }
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG3, STATUSBAR, STATUSMSG);
  loadmenus(mylist, HOR_MENU);
  temp_char = start_hmenu(&data);
  free_list(mylist);
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  update_screen();
  return temp_char;
}

/*-------------------------*/
/* Display File menu       */
/*-------------------------*/

void filemenu() {
  cleanStatusBar();
  data.index = OPTION_NIL;
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  loadmenus(mylist, FILE_MENU);
  write_str(1, 1, "File", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(1, 2, 13, 8, MENU_PANEL, MENU_FOREGROUND0,0, 1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  update_screen();
  free_list(mylist);

  if(data.index == OPTION_1) {
    //New file option
    newDialog(currentFile);
    //Update new global file name
    refresh_screen(-1);
  }
  if(data.index == OPTION_2) {
    //External Module - Open file dialog.
    openFileHandler();
    //flush_editarea();
  }
  if(data.index == OPTION_3) {
    //Save option
    if (fileModified != FILE_READMODE){
      if(strcmp(currentFile, UNKNOWN) == 0)
        saveasDialog(currentFile);
      else {
        saveDialog(currentFile);
      }
    }
    else{
      //Error: We are on readmode
      infoWindow(mylist, WFILEINREADMODE_MSG, READMODEWTITLE);
    }
    //Update new global file name
    refresh_screen(-1);
  }
  if(data.index == OPTION_4) {
    //Save as option
      saveasDialog(currentFile);
      refresh_screen(-1);
  }

  if(data.index == OPTION_5) {
    //Exit option
    if(fileModified == 1)
      exitp = confirmation();   //Shall we exit? Global variable!
    else
      exitp = EXIT_FLAG;
  }
  data.index = OPTION_NIL;
  cleanStatusBar();
   //Restore message in status bar
   write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

void optionsmenu() {
  int  setColor;
  data.index = OPTION_NIL;
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  loadmenus(mylist, OPT_MENU);
  write_str(7, 1, "Options", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(7, 2, 20, 6, MENU_PANEL, MENU_FOREGROUND0,0, 1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  update_screen();

  free_list(mylist);
  if(data.index == OPTION_1) {
    //File Info
    fileInfoDialog();
  }
  if(data.index == OPTION_3) {
    //Set Colors
    setColor = colorsWindow(mylist,COLORSWTITLE);
    setColorScheme(setColor);
    checkConfigFile(setColor);  //save new configuration in config file
    refresh_screen(1);
  }
  data.index = OPTION_NIL;
  //Restore message in status bar
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);

}

/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

void helpmenu() {
  cleanStatusBar();
  data.index = OPTION_NIL;
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  loadmenus(mylist, HELP_MENU);
  write_str(16, 1, "Help", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(16, 2, 26, 5, MENU_PANEL, MENU_FOREGROUND0, 0,1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  update_screen();
  free_list(mylist);
  if(data.index == OPTION_1) {
    //About info
    help_info();
  }
  if(data.index == OPTION_2) {
    //About info
    about_info();
  }
  data.index = -1;
  //Restore message in status bar
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);
}

/*-------------------------------*/
/* Display Confirmation Dialog   */
/*-------------------------------*/

/* Displays a window to asks user for confirmation */
int confirmation() {
  int     ok = 0;
   if (forceBufferUpdate == 1 ) {
      writeBuffertoDisplay(editBuffer);
      forceBufferUpdate = 0;
   }
  if(fileModified == 1) {
    ok = yesnoWindow(mylist, WMODIFIED_MSG, CONFIRMWTITLE);
    data.index = OPTION_NIL;
    //save file if modified?
    if(ok == 1) {
      if(strcmp(currentFile, UNKNOWN) == 0)
    saveasDialog(currentFile);
      else {
    saveDialog(currentFile);
      }
      ok = EXIT_FLAG;
    } else {
      if (ok != 2) ok = EXIT_FLAG;
    }
  } else {
    ok = -1;            //Exit without asking.
  }
  return ok;
}

/*--------------------------*/
/* Display About Dialog     */
/*--------------------------*/

int about_info() {
  int     ok = 0;
  char    msg[105];
  if (forceBufferUpdate == 1) {
    writeBuffertoDisplay(editBuffer);
    forceBufferUpdate = 0;
  }
  msg[0] = '\0';
  strcat(msg, ABOUT_ASC_1);
  strcat(msg, ABOUT_ASC_2);
  strcat(msg, ABOUT_ASC_3);
  strcat(msg, "\0");
  alertWindow(mylist, msg,ABOUTWTITLE);
  return ok;
}

/*--------------------------*/
/* Display About Dialog     */
/*--------------------------*/

int help_info() {
  int     ok = 0;
  char    msg[500];
  if (forceBufferUpdate == 1) {
    writeBuffertoDisplay(editBuffer);
    forceBufferUpdate = 0;
  }
 //writeBuffertoDisplay(editBuffer);
  msg[0] = '\0';
  strcat(msg, HELP1);       //located in user_inter.h
  strcat(msg, HELP2);       //located in user_inter.h
  strcat(msg, HELP3);       //located in user_inter.h
  strcat(msg, HELP4);       //located in user_inter.h
  strcat(msg, HELP5);       //located in user_inter.h
  strcat(msg, HELP6);       //located in user_inter.h
  strcat(msg, HELP7);       //located in user_inter.h
  strcat(msg, HELP8);       //located in user_inter.h
  strcat(msg, HELP9);       //located in user_inter.h
  strcat(msg, HELP10);      //located in user_inter.h
  strcat(msg, "\0");
  helpWindow(mylist, msg, HELPWTITLE);
  refresh_screen(0);
  return ok;
}

/*----------------------*/
/* Drop_Down Menu Loop  */
/*----------------------*/

void drop_down(char *kglobal) {
/*
   Drop_down loop animation.
   K_LEFTMENU/K_RIGHTMENU -1 is used when right/left arrow keys are used
   so as to break vertical menu and start the adjacent menu
   kglobal is changed by the menu functions.
*/
  if (forceBufferUpdate == 1) {
    writeBuffertoDisplay(editBuffer);
    forceBufferUpdate = 0;
  }
 //  writeBuffertoDisplay(editBuffer);
  do {
    if(*kglobal == K_ESCAPE) {
      //Exit drop-down menu with ESC
      *kglobal = 0;
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
void credits() {
/* Frees memory and displays goodbye message */
  //Free selected path item/path from opfiledialog
  size_t i; //to be compatible with strlen
  char auth[27] ="Coded by v3l0r3k 2019-2021";
if (openFileData.itemIndex != 0) {
    free(openFileData.item);
    free(openFileData.path);
  }
  free_buffer();
  resetTerm();          //restore terminal settings from failsafe
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
  _timer1.ms=10;
  _timer1.ticks=0;
  i=0;

  outputcolor(0, 97);
  do{
    if (timerC(&_timer1) == 1) {
       gotoxy(i,8);
       if (i==strlen(auth)) outputcolor(0,93);
       if (i<10 || i>16)
    if (i<=strlen(auth)) printf("%c", auth[i-1]);
       printf("\n");
       i++;
     }
    } while (_timer1.ticks <30);

  printf("\n");
  outputcolor(7, 0);
}

/*=================*/
/*BUFFER functions */
/*=================*/

/*----------------------------------*/
/* Initialize and clean edit buffer */
/*----------------------------------*/

void cleanBuffer(EDITBUFFER editBuffer[MAX_LINES]) {
//Clean and initialize the edit buffer
  long     i, j;

  for(j = 0; j < MAX_LINES; j++)
    for(i = 0; i < MAX_CHARS; i++) {
      editBuffer[j].charBuf[i].specialChar = 0;
      editBuffer[j].charBuf[i].ch = CHAR_NIL;
    }
}

int writetoBuffer(EDITBUFFER editBuffer[MAX_LINES], long whereX, long whereY,
          char ch) {
  int     oldValue;
  oldValue = editBuffer[whereY].charBuf[whereX].specialChar;
  if(ch == SPECIAL_CHARS_SET1) {
    editBuffer[whereY].charBuf[whereX].specialChar = SPECIAL_CHARS_SET1;
    editBuffer[whereY].charBuf[whereX].ch = ch;
  } else if(ch == SPECIAL_CHARS_SET2) {
    editBuffer[whereY].charBuf[whereX].specialChar = SPECIAL_CHARS_SET2;
    editBuffer[whereY].charBuf[whereX].ch = ch;
  } else {
    //Special char example -61 -95 : รก
    if(oldValue == SPECIAL_CHARS_SET1 || oldValue == SPECIAL_CHARS_SET2)
      editBuffer[whereY].charBuf[whereX].specialChar = oldValue;
    else
      editBuffer[whereY].charBuf[whereX].specialChar = 0;

    editBuffer[whereY].charBuf[whereX].ch = ch;
  }
  return 1;
}

/*========================*/
/*FILE handling functions */
/*========================*/

/*--------------------------*/
/* Write EditBuffer to File */
/*--------------------------*/

int writeBuffertoFile(FILE * filePtr, EDITBUFFER editBuffer[MAX_LINES]) {
  char    tempChar=0, oldChar=0;
  long     lineCounter = 0;
  long     inlineChar = 0;
  int     specialChar = 0;

  //Check if pointer is valid
  if(filePtr != NULL) {
    rewind(filePtr);        //Make sure we are at the beginning
    lineCounter = 0;
    inlineChar = 0;
    do {
      oldChar = tempChar;
      tempChar = editBuffer[lineCounter].charBuf[inlineChar].ch;
      specialChar =
      editBuffer[lineCounter].charBuf[inlineChar].specialChar;
      inlineChar++;
      if(tempChar == END_LINE_CHAR) {
    inlineChar = 0;
    lineCounter++;
      }
      if(tempChar > 0) {
    fprintf(filePtr, "%c", tempChar);
      } else {
    //Special accents
    fprintf(filePtr, "%c%c", specialChar, tempChar);
      }
    } while(tempChar != CHAR_NIL);
    //Check if last line finishes in 0x0A - oldChar = 0x0A
    if(oldChar != END_LINE_CHAR)
      fprintf(filePtr, "%c", END_LINE_CHAR);
  }
  return 1;
}

/*------------------*/
/* Save File Dialog */
/*------------------*/

int saveDialog(char fileName[MAX_TEXT]) {
  int     ok = 0;
  char    tempMsg[MAX_TEXT];
  data.index = OPTION_NIL;
  openFile(&filePtr, fileName, "w");
  writeBuffertoFile(filePtr, editBuffer);
  strcpy(tempMsg, fileName);
  strcat(tempMsg, WSAVE_MSG);
  ok = infoWindow(mylist, tempMsg, FILESAVEDWTITLE);
  fileModified = 0;     //reset modified flag

  return ok;
}

int saveasDialog(char fileName[MAX_TEXT]) {
  int     ok=0, count=0;
  char    tempFile[MAX_TEXT], tempMsg[MAX_TEXT];
  data.index = OPTION_NIL;

  clearString(tempFile, MAX_TEXT);

  count = inputWindow(WSAVELABEL_MSG, tempFile, SAVEWTITLE);

  if(count > 0) {
    //Save file
    data.index = OPTION_NIL;
    clearString(fileName, MAX_TEXT);
    strcpy(fileName, tempFile);

    //Check whether file exists.
    if(file_exists(fileName)) {
      ok = yesnoWindow(mylist, WFILEEXISTS_MSG, EXISTSWTITLE);
      if(ok == CONFIRMATION) {
    //Save anyway.
    closeFile(filePtr);
    openFile(&filePtr, fileName, "w");
    writeBuffertoFile(filePtr, editBuffer);
    strcpy(tempMsg, fileName);
    strcat(tempMsg, WSAVE_MSG);
    ok = infoWindow(mylist, tempMsg, FILESAVEDWTITLE);
    fileModified = 0;
      } else {
    //Do nothing.
      }
    } else {
      closeFile(filePtr);
      openFile(&filePtr, fileName, "w");
      writeBuffertoFile(filePtr, editBuffer);
      strcpy(tempMsg, fileName);
      strcat(tempMsg, WSAVE_MSG);
      ok = infoWindow(mylist, tempMsg, FILESAVEDWTITLE);
      fileModified = 0;
    }
  }
  return ok;
}

/*-----------------*/
/* New File Dialog */
/*-----------------*/

int newDialog(char fileName[MAX_TEXT]) {
  char    tempFile[MAX_TEXT];
  int     ok, count;

  clearString(tempFile, MAX_TEXT);

  data.index = OPTION_NIL;

  count = inputWindow(WSAVELABEL_MSG, tempFile, NEWFILEWTITLE);
  if(count > 0) {
    //Check whether file exists and create file.
    ok = createnewFile(&filePtr, tempFile, 1);
    if(ok == 1) {
      cleanBuffer(editBuffer);
      strcpy(fileName, tempFile);
    editScroll.scrollActiveV = VSCROLL_OFF; //Scroll is inactive.
    editScroll.totalLines = 1; //There is just one line active in buffer
    editScroll.bufferX = 1;
    editScroll.bufferY = 1;
    cursorX=START_CURSOR_X;
        cursorY=START_CURSOR_Y;
    }
    ok = 1;
  }
  return ok;
}

/*--------------------------*/
/* OPEN File Dialog HANDLER */
/*--------------------------*/

int openFileHandler() {
  char    oldFile[MAX_TEXT];

  int ok=chdir(currentPath);        //Go back to original path
  ok++; //to avoid warning
  //Update screen if not scrolling
  if (forceBufferUpdate == 1) {
    writeBuffertoDisplay(editBuffer);
    forceBufferUpdate = 0;
  }
  //free current item if it exists
  if (openFileData.itemIndex != 0){
        free(openFileData.item);
        free(openFileData.path);
  }
  openFileDialog(&openFileData);
 //Update new global file name
  //refresh_screen(1);
  flush_editarea();
  if(openFileData.itemIndex != 0) {
    //check if file exists
    if (!file_exists(openFileData.path)) {
    alertWindow(mylist, WCHECKFILE2_MSG, INFOWTITLE);
    }
    //Change current File Name
    //if the index is different than CLOSE_WINDOW
    clearString(oldFile, MAX_TEXT);
    strcpy(oldFile, currentFile);   //save previous file to go back
    clearString(currentFile, MAX_TEXT);
    strcpy(currentFile, openFileData.path);
    cleanBuffer(editBuffer);
    //Open file and dump first page to buffer - temporary
    if(filePtr != NULL)
      closeFile(filePtr);
    handleopenFile(&filePtr, currentFile, oldFile);
  }

  //Write current buffer
  writeBuffertoDisplay(editBuffer);
  return 1;
}

/*------------------*/
/* File Info Dialog */
/*------------------*/

int fileInfoDialog() {
  long    size = 0, lines = 0;
  int     i;
  char    sizeStr[20];
  char    linesStr[20];
  char    tempMsg[150];
  char    pathtxt[60];
  if(filePtr != NULL) {
    closeFile(filePtr);
    openFile(&filePtr, currentFile, "r");
    size = getfileSize(filePtr);
    lines = countLinesFile(filePtr);
    if(size <= 0)
      size = 0;
    if(lines <= 0)
      lines = 0;
    sprintf(sizeStr, "%ld", size);
    sprintf(linesStr, "%ld", lines);
    strcpy(tempMsg, WINFO_SIZE);
    strcat(tempMsg, sizeStr);
    strcat(tempMsg, " bytes.");
    strcat(tempMsg, WINFO_SIZE2);
    strcat(tempMsg, linesStr);
    strcat(tempMsg, " lines.\n");
    for (i=0;i<60;i++){
        if (i!=31) pathtxt[i] = openFileData.fullPath[i];
        else pathtxt[31] = '\n';
    }
    pathtxt[59] = CHAR_NIL;
    strcat(tempMsg, pathtxt);
    alertWindow(mylist, tempMsg, INFOWTITLE);
  } else {
    infoWindow(mylist, WINFO_NOPEN, NOFILEOPENWTITLE);
  }
  return 0;
}


/*-----------------------------*/
/* Calculate Scroll Values     */
/*-----------------------------*/

short checkScrollValues(long lines){
 float totalLines, displayLength;
 editScroll.displayLength = rows - 5;
 editScroll.totalLines = lines;
 editScroll.scrollLimit = (editScroll.totalLines - editScroll.displayLength)-1;
 if (editScroll.totalLines > editScroll.displayLength)
   {
     editScroll.scrollActiveV = VSCROLL_ON;
     //Calculate scrollRatio for scroll indicator
     totalLines = editScroll.totalLines;
     displayLength = editScroll.displayLength+1;
     editScroll.scrollRatio = totalLines  / displayLength;
     return VSCROLL_ON;
 } else{
     editScroll.scrollActiveV = VSCROLL_OFF;
   return VSCROLL_OFF;
 }
}
/*-----------------------------*/
/* Write EditBuffer to Display */
/*-----------------------------*/

int writeBuffertoDisplay(EDITBUFFER editBuffer[MAX_LINES]) {
  long     i = 0, j = 0, z=0,clean=0;
  char    tempChar, specialChar;
  //Dump edit buffer to screen
  if (editScroll.scrollActiveV == 1)
    z=editScroll.scrollPointer;
  else
    z=0;
  do {
    tempChar = editBuffer[z+j].charBuf[i].ch;
    specialChar = editBuffer[z+j].charBuf[i].specialChar;
    if(tempChar != CHAR_NIL) {
       if(tempChar != END_LINE_CHAR) {
    if(i == columns - 4) {
      //temporary restriction until vertical scroll is implemented
      i = 0;
      if (j<editScroll.displayLength-1) j++;
          else
            tempChar = END_LINE_CHAR;
    }
        write_ch(i + START_CURSOR_X, j + START_CURSOR_Y, specialChar,
         EDITAREACOL, EDIT_FORECOLOR);
    write_ch(i + START_CURSOR_X, j + START_CURSOR_Y, tempChar,
         EDITAREACOL, EDIT_FORECOLOR);
    i++;
      }
      if(tempChar == END_LINE_CHAR) {
    //clean remaining screen cells
        for (clean=i+START_CURSOR_X; clean<columns; clean++)
        write_ch(clean,j+START_CURSOR_Y,FILL_CHAR, EDITAREACOL, EDIT_FORECOLOR);
    i = 0;
    j++;
        if (j>editScroll.displayLength-1) break;
      }
    }
  } while(tempChar != CHAR_NIL);
  update_screen();
  return 1;
}
/*----------------------------------------*/
/* Write EditBuffer to Display (Raw mode) */
/*----------------------------------------*/

int writeBuffertoDisplayRaw(EDITBUFFER editBuffer[MAX_LINES]) {
  long     i = 0, j = 0, z=0,clean=0;
  char    tempChar, specialChar;
  //Dump edit buffer to screen
  if (editScroll.scrollActiveV == 1)
    z=editScroll.scrollPointer;
  else
    z=0;
  do {
    tempChar = editBuffer[z+j].charBuf[i].ch;
    specialChar = editBuffer[z+j].charBuf[i].specialChar;
    if(tempChar != CHAR_NIL) {
       if(tempChar != END_LINE_CHAR) {
    if(i == columns - 4) {
      //temporary restriction until vertical scroll is implemented
      i = 0;
      if (j<editScroll.displayLength-1) j++;
          else
            tempChar = END_LINE_CHAR;
    }
    gotoxy(i + START_CURSOR_X, j + START_CURSOR_Y);
    outputcolor(EDITAREACOL, EDIT_FORECOLOR);
    printf("%c%c",specialChar,tempChar);
    i++;
      }
      if(tempChar == END_LINE_CHAR) {
    //clean remaining screen cells
        for (clean=i+START_CURSOR_X; clean<columns; clean++){
      gotoxy(clean, j + START_CURSOR_Y);
      outputcolor(EDITAREACOL, EDIT_FORECOLOR);
      printf("%c",FILL_CHAR);
        }
        //write_ch(clean,j+START_CURSOR_Y,FILL_CHAR, EDITAREACOL, EDIT_FORECOLOR);
    i = 0;
    j++;
        if (j>editScroll.displayLength-1) break;
      }
    }
  } while(tempChar != CHAR_NIL);
  //update_screen();
  return 1;
}


/*------------------------------*/
/* Open file and dump to buffer */
/*------------------------------*/

int filetoBuffer(FILE * filePtr, EDITBUFFER editBuffer[MAX_LINES]) {
  long     inlineChar = 0, lineCounter = 0;
  char    ch;
  int tabcount;
  fileModified = FILE_UNMODIFIED;
  //Check if pointer is valid
  if(filePtr != NULL) {
    rewind(filePtr);        //Make sure we are at the beginning
    ch = getc(filePtr);     //Peek ahead in the file
    while(!feof(filePtr)) {
      if(ch != CHAR_NIL) {
    //Temporary restrictions until scroll is implemented.
    //if(lineCounter == rows - 4)
    //  break;

    if(ch == SPECIAL_CHARS_SET1 || ch == SPECIAL_CHARS_SET2) {
      writetoBuffer(editBuffer, inlineChar, lineCounter, ch);
      //Read accents
      ch = getc(filePtr);
      writetoBuffer(editBuffer, inlineChar, lineCounter, ch);
    } else {
      if(ch > 0)
        writetoBuffer(editBuffer, inlineChar, lineCounter, ch);
    }
        //TABs are converted into spaces
        if(ch == K_TAB) {
        for (tabcount=0;tabcount<TAB_DISTANCE;tabcount++){
          ch = FILL_CHAR;
          writetoBuffer(editBuffer, inlineChar, lineCounter, ch);
          inlineChar++;
        }
    } else

    inlineChar++; //NEXT CHARACTER

    if(ch == END_LINE_CHAR) {
      inlineChar = 0;
      ch = 0;
      lineCounter++;
    }
       if (lineCounter == MAX_LINES) {
           //open as readMode
           fileModified = FILE_READMODE;
           break;
       }
       //If file is bigger than buffer
      //break loop at last allowed line.
      }
      ch = getc(filePtr);
    }
  }
  return 1;
}

/*------------------*/
/* Open file checks */
/*------------------*/

int handleopenFile(FILE ** filePtr, char *fileName, char *oldFileName) {
  long    checkF = 0;
  long    linesinFile =0;
  int     ok = 0;

  openFile(filePtr, fileName, "r");
  //Check for binary characters to determine filetype
  checkF = checkFile(*filePtr);
  if(checkF > 5) {
    //File doesn't seem to be a text file. Open anyway?
    ok = yesnoWindow(mylist, WCHECKFILE_MSG, OPENANYWAYWTITLE);
    if(ok == 1) {
     filetoBuffer(*filePtr, editBuffer);
      refresh_screen(-1);
      //fileModified = FILE_READMODE; // Open as readmode
    } else {
      //Go back to previous file
      ok = -1;
      clearString(fileName, MAX_TEXT);
      strcpy(fileName, oldFileName);
      //Open file again and dump first page to buffer - temporary
      closeFile(*filePtr);
      openFile(filePtr, currentFile, "r");
      filetoBuffer(*filePtr, editBuffer);
    }
  } else {
    //Check wheter file is bigger than buffer. 32700 lines
    linesinFile = countLinesFile(*filePtr);
    if (linesinFile > MAX_LINES)
    ok=infoWindow(mylist, WCHECKLINES_MSG, FILETOBIGWTITLE);
    //Reset values
    editScroll.scrollPointer = 0; //Set pointer to 0
    editScroll.pagePointer =0; //set page Pointer to 0
    checkScrollValues(linesinFile); //make calculations for scroll.
    editScroll.bufferX = 1;
    editScroll.bufferY = 1;
    cursorX=START_CURSOR_X;
    cursorY=START_CURSOR_Y;
    //Dump file into edit buffer.
    filetoBuffer(*filePtr, editBuffer);
    refresh_screen(-1);
  }
  return ok;
}

/*-----------------*/
/* New file checks */
/*-----------------*/

int createnewFile(FILE ** filePtr, char *fileName, int checkFile) {
  int     ok=0;

//Check if file exists if indicated.
  if(checkFile == 1) {
    if(file_exists(fileName)) {
      ok = yesnoWindow(mylist, WFILEEXISTS_MSG, CREATEWTITLE);
      if(ok == CONFIRMATION) {
    //Overwrite anyway.
    openFile(filePtr, fileName, "w");
    ok = 1;
      } else {
    //Do nothing. Don't overwrite.
    ok = 0;
      }
    } else {
      //File does not exist.
      //clearString(fileName, MAX_TEXT);
      //strcpy(fileName,tempFile);
      openFile(filePtr, fileName, "w");
      ok = 1;
    }

  } else {
    //Don't check if file exists.
    openFile(filePtr, fileName, "w");
    refresh_screen(-1);
  }
  return ok;
}


