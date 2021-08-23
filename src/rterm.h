/*
========================================================================
- HEADER -
Module to control some display routines that implement ANSI functions.
@author : Velorek
@version : 1.0
Last modified: 14/04/2019 + Rename headers
========================================================================
*/

#ifndef _RAWTERM_H_
#define _RAWTERM_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/*====================================================================*/
/* COLOR CONSTANTS                                                    */
/*====================================================================*/

// Background colors low intensity
#define B_BLACK 40
#define B_RED 41
#define B_GREEN 42
#define B_YELLOW 43
#define B_BLUE 44
#define B_MAGENTA 45
#define B_CYAN 46
#define B_WHITE 47

// Foreground colors low intensity
#define F_BLACK 30
#define F_RED 31
#define F_GREEN 32
#define F_YELLOW 33
#define F_BLUE 34
#define F_MAGENTA 35
#define F_CYAN 36
#define F_WHITE 37
//#define F_WHITE 37
#define F_GREY 90

// Background colors high intensity
#define BH_BLACK 100
#define BH_RED 101
#define BH_GREEN 102
#define BH_YELLOW 103
#define BH_BLUE 104
#define BH_MAGENTA 105
#define BH_CYAN 106
#define BH_WHITE 107

// Foreground colors high intensity
#define FH_BLACK 90
#define FH_RED 91
#define FH_GREEN 92
#define FH_YELLOW 93
#define FH_BLUE 94
#define FH_MAGENTA 95
#define FH_CYAN 96
#define FH_WHITE 97

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

void    pushTerm();
int     resetTerm();
int     kbhit();
int     readch();
void    resetch();
//char    getch(void);
void    gotoxy(int x, int y);
void    outputcolor(int foreground, int background);
void    screencol(int x);
void    resetAnsi(int x);
int     get_terminal_dimensions(int *rows, int *columns);
void    showcursor();
void    hidecursor();
#endif
