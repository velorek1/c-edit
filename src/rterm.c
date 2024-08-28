/*
======================================================================
Module to control some display routines that implement ANSI functions.
on LINUX Terminals

@author : Velorek (routines extracted from the internet)
@version : 1.0

LAST MODIFIED : 18/01/2023 Kbhit with poll control added
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <poll.h>
#include "rterm.h"
#include "keyb.h"
/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

struct winsize max;
static struct termios term, failsafe;
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

int kbhit(int timeout_ms)
{
    struct pollfd fds = {STDIN_FILENO, POLLIN, 0};
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    int ret = poll(&fds, 1, timeout_ms);
    fcntl(STDIN_FILENO, F_SETFL, 0);
    if (ret > 0) {
        return 1;
    } else if (ret == 0) {
        return 0; // timeout occurred
    } else {
        return -1; // error occurred
    }
}

/*----------------------*/
/*Read char with control*/
/*----------------------*/
char readch() {
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
  term.c_cc[VMIN] = 0;
  tcsetattr(0, TCSANOW, &term);
  peek_character = 0;
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
    case 0:         //reset all colors and attributes
      printf("%c[0m", 0x1b);
      break;
    case 1:         //reset only attributes
      printf("%c[20m", 0x1b);
      break;
    case 2:         //reset foreg. colors and not attrib.
      printf("%c[39m", 0x1b);
      break;
    case 3:         //reset back. colors and not attrib.
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

/*--------------------------*/
/* Set up terminal          */
/*--------------------------*/

//For code simplification purposes

void init_term(){
  hidecursor();
  pushTerm();
  resetch();
  //Setup unicode  
  setlocale(LC_ALL, "");
}


void close_term(){
  showcursor();
  resetTerm();
  outputcolor(F_WHITE, B_BLACK);
  screencol(B_BLACK);
  resetAnsi(0);
  printf("\n");
}

