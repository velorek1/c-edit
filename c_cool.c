#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct winsize max;
static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

void gotoxy(int x,int y)
{
  printf("%c[%d;%df",0x1B,y,x);
}


void outputcolor(int foreground, int background)
{
  printf("%c[%d;%dm", 0x1b,foreground,background);
}

void screencol(int x)
{
  gotoxy(0,0);
  outputcolor(0,x);
  printf("%c[2J", 0x1b);
  outputcolor(0,0);
}

int get_terminal_dimensions(int *rows, int *columns)
{
//Get dimensions of screen by value.
  ioctl(0, TIOCGWINSZ , &max);
  *columns = max.ws_col;
  *rows = max.ws_row;
return 0;
}

