/* 
======================================================================
Module to control some display routines that implement ANSI functions.

@author : Velorek
@version : 1.0
 
LAST MODIFIED : 14/04/2019 Rename Headers
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

/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

struct winsize max;
static struct termios newp, failsafe;
static int peek_character = -1;

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*-------------------------------------*/
/* Initialize new terminal i/o settings*/
/*-------------------------------------*/
void pushTerm() {
//Save terminal settings in failsafe to be retrived at the end
  tcgetattr(0, &failsafe);
}

/*---------------------------*/
/* Reset terminal to failsafe*/
/*---------------------------*/
int resetTerm() {
  //tcsetattr(0, TCSANOW, &failsafe);
  /* flush and reset */
  if (tcsetattr(0,TCSAFLUSH,&failsafe) < 0) return -1;
  return 0;
}

/*--------------------------------------.*/
/* Detect whether a key has been pressed.*/
/*---------------------------------------*/
int kbhit() {
  unsigned char ch;
  int     nread;
  // tcgetattr(0, &old);               /* grab old terminal i/o settings */
  if(peek_character != -1)
    return 1;
  newp.c_cc[VMIN] = 0;
  tcsetattr(0, TCSANOW, &newp);
  nread = read(0, &ch, 1);
  newp.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &newp);
  if(nread == 1) {
    peek_character = ch;
    return 1;
  }
  return 0;
}


/*----------------------*/
/*Read char with control*/
/*----------------------*/
int readch() {
  char    ch;

  if(peek_character != -1) {
    ch = peek_character;
    peek_character = -1;
    return ch;
  }
  read(0, &ch, 1);
  return ch;
}

void resetch() {
//Clear ch  
  newp.c_cc[VMIN] = 0;
  tcsetattr(0, TCSANOW, &newp);
  peek_character = 0;
}

/*------------------------------------------ */
/* Read 1 character - echo defines echo mode */
/*------------------------------------------ */
char getch() {
  struct termios t;
  tcgetattr(0, &t);
  tcflag_t old_flag = t.c_lflag;
  t.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(0, TCSANOW, &t);
  int     c = getchar();
  t.c_lflag = old_flag;
  tcsetattr(0, TCSANOW, &t);
  return c;
}

/*----------------------------------*/
/* Move cursor to specified position*/
/*----------------------------------*/
void gotoxy(int x, int y) {
  printf("%c[%d;%df", 0x1B, y, x);
}

/*---------------------*/
/* Change colour output*/
/*---------------------*/
void outputcolor(int foreground, int background) {
  printf("%c[%d;%dm", 0x1b, foreground, background);
}

/*-----------------------------------------------------*/
/* Change the whole color of the screen by applying CLS*/
/*-----------------------------------------------------*/
void screencol(int x) {
  gotoxy(0, 0);
  outputcolor(0, x);
  printf("%c[2J", 0x1b);
  outputcolor(0, 0);
}

/*-----------------------*/
/* Reset ANSI ATTRIBUTES */
/*-----------------------*/
void resetAnsi(int x) {
  switch (x) {
    case 0:			//reset all colors and attributes
      printf("%c[0m", 0x1b);
      break;
    case 1:			//reset only attributes
      printf("%c[20m", 0x1b);
      break;
    case 2:			//reset foreg. colors and not attrib.
      printf("%c[39m", 0x1b);
      break;
    case 3:			//reset back. colors and not attrib.
      printf("%c[49m", 0x1b);
      break;
    default:
      break;
  }
}

/*------------------------*/
/* Get terminal dimensions*/
/*------------------------*/
int get_terminal_dimensions(int *rows, int *columns) {
  ioctl(0, TIOCGWINSZ, &max);
  *columns = max.ws_col;
  *rows = max.ws_row;
  return 0;
}

/*--------------------------*/
/* Ansi function hide cursor*/
/*--------------------------*/
void hidecursor() {
  printf("\e[?25l");
}

/*--------------------------*/
/* Ansi function show cursor*/
/*--------------------------*/
void showcursor() {
  printf("\e[?25h");
}
