#include <stdio.h>
#include <string.h>
#include "c_cool.h"
#include "list_choice.h"

LISTCHOICE *mylist,data;
BCELL *my_screen;

int rows, columns, old_rows, old_columns;
int exitp = 0;
char kglobal;

int main_screen()
{
  int i,j;
  //Draw the main screen
  rows=0;
  columns=0;

  //Save previous values
  old_rows = rows;
  old_columns = columns;
  get_terminal_dimensions(&rows,&columns);

  if (rows==0) rows=25; //just in case it can't find the terminal dimensions
  if (columns==0) columns=80;
 
  //Draw upper and lower bars
  for (i=1;i<=columns;i++)
  {
    write_ch(i,1,32,B_CYAN,F_WHITE);
  }

  for (i=1;i<columns;i++)
  {
    write_ch(i,rows,32,B_WHITE,F_WHITE);
  }
 
  write_str(1,1,"File  Options  Help",B_CYAN,F_BLACK);
  write_str(1,rows,"- Coded by Velorek",B_WHITE,F_BLACK);
  update_screen();
}

void loadmenus(int choice)
{
  //Load menus into memory  
  if (choice==0)
    {
       add_item(mylist,"File",1,1,B_CYAN,F_BLACK,B_RED,FH_WHITE);
       add_item(mylist,"Options",7,1,B_CYAN,F_BLACK,B_RED,FH_WHITE);
       add_item(mylist,"Help",16,1,B_CYAN,F_BLACK,B_RED,FH_WHITE);
    }

  if (choice==1)
    {
       add_item(mylist,"Option 1",3,3,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Option 2",3,4,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Option 3",3,5,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Option 4",3,6,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Option 5",3,7,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Exit",3,8,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
    }
  if (choice==2)
    {
       add_item(mylist,"Settings",8,3,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Colors",8,4,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"Profiles",8,5,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
    }
  if (choice==3)
    {
       add_item(mylist,"Help...F1",17,3,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
       add_item(mylist,"About",17,4,B_WHITE,F_BLACK,B_BLACK,F_WHITE);
    }

  if (choice==5)
    {
       add_item(mylist,"<YES>",28,12,B_WHITE,F_BLACK,B_BLACK,FH_YELLOW);
       add_item(mylist,"<NO>",40,12,B_WHITE,F_BLACK,B_BLACK,FH_YELLOW);
    }
}

int filemenu()
{
 loadmenus(1);
 write_str(1,1,"File",B_RED,FH_WHITE);
 draw_window(1,2,15,10,B_WHITE,F_BLACK,0);
 kglobal = start_vmenu(&data);
 close_window();
 write_str(1,1,"File  Options  Help",B_CYAN,F_BLACK);
 update_screen();
 free_list(mylist);
 if (data.index == 5)
     {
      loadmenus(5);
      draw_window(16,6,columns-20,rows-10,B_WHITE,F_BLACK,0);
      write_str(23,8,"Are you sure you want to quit?",F_BLACK,B_WHITE);
      start_hmenu(&data);
      free_list(mylist);
      if (data.index==0) exitp=1;
      close_window();
      }
 data.index=-1;
}
void optionsmenu()
{
 loadmenus(2);
 write_str(7,1,"Options",B_RED,FH_WHITE);
 draw_window(7,2,20,6,B_WHITE,F_BLACK,0);
 kglobal=start_vmenu(&data);
 close_window();
 write_str(1,1,"File  Options  Help",B_CYAN,F_BLACK);
 update_screen();
 free_list(mylist);
 data.index=-1;
}
void helpmenu()
{
 loadmenus(3);
 write_str(16,1,"Help",B_RED,FH_WHITE);
 draw_window(16,2,26,6,B_WHITE,F_BLACK,0);
 kglobal=start_vmenu(&data);
 close_window();
 write_str(1,1,"File  Options  Help",B_CYAN,F_BLACK);
 update_screen();
 free_list(mylist);
 data.index=-1;
}

int main()
{
  char ch;
  printf("\e7");
  hidecursor();
  create_screen();
  bscreen_color(B_BLUE);
  main_screen();
  write_num(columns-10,1,rows,3,B_CYAN,FH_YELLOW);
  write_str(2,3,"Press A to activate menus",B_BLACK,FH_YELLOW);
  update_screen();
  do {
    ch=getch(); 
    if (ch=='a' || ch=='A')
      {
	loadmenus(0); // horizontal menu
	start_hmenu(&data);
	free_list(mylist);
	kglobal = 0; //reset kglobal
     //drop-down menu loop
     do { 
       	if (data.index == 0)
	  {
	    filemenu();
	    if (kglobal==-1)
	      {
		data.index = 1;
	      }
	    if (kglobal==-2)
	      {
		data.index = 2;
	      }
       	  }
 	if (data.index == 1)
	  {
	    optionsmenu();
	    if (kglobal==-1)
	      {
		data.index = 2;
	      }
	    if (kglobal==-2)
	      {
		data.index = 0;
	      }
	  }
 	if (data.index == 2)
	  {
	    helpmenu();
	    if (kglobal==-1)
	      {
		data.index = 0;
	      }
	    if (kglobal==-2)
	      {
		data.index = 1;
	      }
	  }
     } while (kglobal !=10);
   }
     
  } while (exitp != 1);
  free_buffer();
  screencol(0);
  outputcolor(7,0);
  showcursor;
  printf("\e8");  
  return 0;
}

