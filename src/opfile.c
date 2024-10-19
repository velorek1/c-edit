/*====================================================================*/
/* OPEN FILE MODULE
   +ListFiles with double linked list and selection menu in C.
   +Scroll function added.
   +Integrated with C-EDIT
   A window is drawn to the buffer and all of the scroll animations
   are drawn to the terminal on raw mode to have a better scrolling
   animation. Once the file is selected, the window is closed and the
   previous screen is painted to the terminal again.
   Last modified : 11/1/2019 - Switch to readch() instead of getch()
                   06/04/2019 - Corrected all memory leaks
		   14/04/2019 - Rename headers
		   22/04/2020 - Added Set File to Open
		   23/04/2023 - Merged with Lynx and vastly simplified
   Coded by Velorek.
   Target OS: Linux.                                                  */
/*====================================================================*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES */
/*====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "scbuf.h"
#include "listbox.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
#include "opfile.h"


/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/

int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
int ndirs=0, nfiles=0;

/*====================================================================*/
/* CODE */
/*====================================================================*/

int listFiles(LISTCHOICE ** listBox1, char *directory) {
  DIR    *d;
  struct dirent *dir;
  int     i;
  char    temp[MAX_ITEM_LENGTH];
  int     lenDir;       //length of directory
  ndirs=0;
  nfiles=0;
  if (*listBox1 != NULL) removeList(listBox1);
  *listBox1 = addatend(*listBox1, newitem("[..]",-1,-1,FH_BLUE,B_WHITE));    // ".."

  //Start at current directory
  d = opendir(directory);
  //Find directories and add them to list first
  if(d) {
    while((dir = readdir(d)) != NULL) {
      if(dir->d_type == DT_DIR) {

    lenDir = strlen(dir->d_name);

    //Check length of directory
    //Directories are displayed between brackets [directory]
    if(lenDir > MAX_ITEM_LENGTH - 2) {
      //Directory name is long. CROP
      memset(&temp, '\0',sizeof(temp)); //Clear memory for temporary line
      strcpy(temp,"\0");
      strcpy(temp, "[");
      for(i = 1; i < MAX_ITEM_LENGTH - 1; i++) {
        temp[i] = dir->d_name[i - 1];
      }
      temp[MAX_ITEM_LENGTH - 1] = ']';
    } else {
      //Directory's name is shorter than display
      //Add spaces to item string.
      memset(&temp, '\0',sizeof(temp)); //Clear memory for temporary line
      strcpy(temp,"\0");
      strcpy(temp, "[");
      for(i = 1; i < lenDir + 1; i++) {
        temp[i] = dir->d_name[i - 1];
      }
      temp[lenDir + 1] = ']';
    }
    //Add all directories except CURRENTDIR and CHANGEDIR
    if(strcmp(dir->d_name, ".") != 0
       && strcmp(dir->d_name, "..") != 0)
    { ndirs++;
      *listBox1 =
          addatend(*listBox1, newitem(temp, -1,-1,FH_BLUE,B_WHITE));}
      }
    }
  }
  closedir(d);

  //Find files and add them to list after directories
  d = opendir(directory);
  if(d) {
    while((dir = readdir(d)) != NULL) {
      if(dir->d_type == DT_REG) {
    //only list valid fiels
    if(strlen(dir->d_name) > MAX_ITEM_LENGTH) {
      for(i = 0; i < MAX_ITEM_LENGTH; i++) {
        temp[i] = dir->d_name[i];
      }
    } else {
      memset(&temp, '\0',sizeof(temp)); //Clear memory for temporary line
      strcpy(temp,"\0");
      strcpy(temp, dir->d_name);
    }
    nfiles++;
    *listBox1 =
        addatend(*listBox1, newitem(temp, -1,-1,-1,-1));
      }
    }
    closedir(d);
  }
  
  return 0;
}

void addItems(LISTCHOICE **listBox1)
{
	int h=0;
//Load items into the list.  
        //if (listBox1 != NULL) removeList(listBox1);
	for (h = 0; h <10; h++) {
		//*ch = textarray[h];
		*listBox1 = addatend(*listBox1, newitem("Test",-1,-1,-1,-1));
	}
}

int openFileDialog(char fileName[MAXFILENAME], char fullPath[MAXFILENAME]){
char ch=0;
char path[MAXFILENAME];
char bit[MAXFILENAME];
char ndirstr[100];
size_t i=0;
int retvalue=0;
char nfilestr[100];
char currentPath[4] = "./\0";
    retvalue =0;
    setselectorLimit(23);
    window_y1 = (new_rows / 2) - 10;
    window_y2 = (new_rows / 2) + 10;
    window_x1 = (new_columns / 2) - 13;
    window_x2 = (new_columns / 2) + 13;   
   copy_screen(screen2,screen1);
   old_rows = new_rows;
   old_columns = new_columns; 
   do{
     draw_window(screen1,window_x1, window_y1, window_x2, window_y2,B_WHITE,F_BLACK,B_BLACK,1,1,1,0);
     draw_window(screen1,window_x1, window_y2-3, window_x2, window_y2,B_CYAN,F_WHITE,B_BLACK,1,0,0,0);
     dump_screen(screen1);
      listFiles(&listBox1,currentPath);
      sprintf(ndirstr, "[%d] Directories", ndirs);
      sprintf(nfilestr, "[%d] Files", nfiles);
      write_str(screen1,window_x1+2,window_y2-2,ndirstr,B_CYAN,FH_WHITE,1);
      write_str(screen1,window_x1+2,window_y2-1,nfilestr,B_CYAN,F_WHITE,1);
      write_str(screen1,window_x1+1,window_y2-4, "   PRESS [ESC] TO EXIT   ",B_BLACK,FH_WHITE,1);
      write_str(screen1,window_x1+2,window_y1,"Select file...",B_BLACK,FH_WHITE,1);
      ch = listBox(listBox1, window_x1+3,window_y1+1, &scrollData, B_WHITE, F_BLACK,B_RED, FH_WHITE, 15, VERTICAL,1,LOCKED);
      if (_animation() == -1) {ch=K_ESCAPE; break;} 
      if (ch == 0x27) {break;} 
      if (ch == K_ENTER){

        if (scrollData.item[0] == '[') {
	 //directory   
            memset(&path, '\0',sizeof(path)); //Clear memory for temporary line
            memset(&bit, '\0',sizeof(bit)); //Clear memory for temporary line
    	    strcpy(bit,"\0");
            strcpy(path,"\0");
            for (i=1; i<strlen(scrollData.item)-1; i++) bit[i-1] = scrollData.item[i];
	    getcwd(path, sizeof(path));
	    strcat(path, "/");
	    strcat(path, bit);
	    chdir(path);
	    ch=0;
      }
      else {
	     break;
      }	 
    } 
   } while (ch!=ESC_KEY);   
    setselectorLimit(15);
   

    if (ch!=27){
    	    //we pass the values
            //memset(&fullPath, '\0',sizeof(fullPath)); //Clear memory for temporary line
            memset(fileName, '\0', strlen(fileName) + 1); // Clear memory for temporary line
            memset(fullPath, '\0', strlen(fullPath) + 1); // Clear memory for temporary line
           // memset(&fileName, '\0',sizeof(fileName)); //Clear memory for temporary line
    	    strcpy(bit,"\0");
            strcpy(path,"\0");
            strcpy(fileName, scrollData.item);
            strcpy(fullPath, path);
	    //reset scroll values
	    cursorX=START_CURSOR_X;
	    cursorY=START_CURSOR_Y;
	    currentLine=0;
	    shiftH=0;
	    posBufX=0;
	    posBufY = 0;
	    retvalue = 1;
    }
    if (listBox1 != NULL) removeList(&listBox1);
   copy_screen(screen1,screen2); 
   return retvalue;
}




