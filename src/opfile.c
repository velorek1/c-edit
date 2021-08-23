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
   Coded by Velorek.
   Target OS: Linux.                                                  */
/*====================================================================*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES */
/*====================================================================*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "keyb.h"
#include "uintf.h"
/*====================================================================*/
/* CONSTANTS */
/*====================================================================*/
//Scroll Control values.
#define SCROLL_ACTIVE 1
#define SCROLL_INACTIVE 0
#define CONTINUE_SCROLL -1
#define DOWN_SCROLL 1
#define UP_SCROLL 0
#define SELECT_ITEM 1
#define UNSELECT_ITEM 0
#define CIRCULAR_ACTIVE 1
#define CIRCULAR_INACTIVE 0

//Special characters (VT100 - ANSI CHARS).
#define FILL_CHAR 32

//Keys used.
#define K_ENTER 13
#define K_ENTER2 10
#define K_ESCAPE 27
#define K_UP_ARROW 'A'      // K_ESCAPE + 'A' -> UP_ARROW
#define K_DOWN_ARROW 'B'    // K_ESCAPE + 'B' -> DOWN_ARROW
//Directories
#define CURRENTDIR "."
#define CHANGEDIR ".."
#define MAX_ITEM_LENGTH 15
#define DIRECTORY 1
#define FILEITEM 0
#define MAX 150

/*====================================================================*/
/* TYPEDEF STRUCTS DEFINITIONS */
/*====================================================================*/
typedef struct _listbox {
  unsigned index;       // Item number
  char   *item;         // Item string
  char   *path;         // Item path
  unsigned isDirectory;     // Kind of item
  struct _listbox *next;    // Pointer to next item
  struct _listbox *back;    // Pointer to previous item
} LISTBOX;

typedef struct _scrolldata {
  unsigned scrollActive;    //To know whether scroll is active or not.
  unsigned scrollLimit;     //Last index for scroll.
  unsigned listLength;      //Total no. of items in the list
  unsigned currentListIndex;    //Pointer to new sublist of items when scrolling.
  unsigned displayLimit;    //No. of elements to be displayed.
  unsigned scrollDirection; //To keep track of scrolling Direction.
  unsigned wherex;
  unsigned wherey;
  unsigned selector;        //Y++
  unsigned backColor0;      //0 unselected; 1 selected
  unsigned foreColor0;
  unsigned backColor1;
  unsigned foreColor1;
  unsigned isDirectory;     // Kind of item
  char   *item;
  char   *path;
  char    fullPath[MAX];
  unsigned itemIndex;
 // LISTBOX *head;      //store head of the list
} SCROLLDATA;

/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/

LISTBOX *listBox1 = NULL;   //Head pointer.
int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;

/*====================================================================*/
/* CODE */
/*====================================================================*/

/* ------------------------------ */
/* Terminal manipulation routines */
/* ------------------------------ */
void cleanLine(int line, int backcolor, int forecolor, int startx,
           int numchars) {
//Cleans line of console.
  int     i;
  for(i = startx; i < numchars; i++) {
    //clean line where path is displayed.
    outputcolor(forecolor, backcolor);
    gotoxy(i, line);
    printf("%c", FILL_CHAR);    //space
  }
}

/* --------------------- */
/* Dynamic List routines */
/* --------------------- */

// create new list element of type LISTBOX from the supplied text string
LISTBOX *newelement(char *text, char *itemPath, unsigned itemType) {
  LISTBOX *newp;
  newp = (LISTBOX *) malloc(sizeof(LISTBOX));

  newp->item = (char *)malloc(sizeof(char) * strlen(text) + 1);
  newp->path = (char *)malloc(sizeof(char) * strlen(itemPath) + 1);
  strcpy(newp->item, text);
  strcpy(newp->path, itemPath);
  newp->isDirectory = itemType;
  newp->next = NULL;
  newp->back = NULL;
  return newp;
}

void deleteList(LISTBOX **head)
{
   /* deref head_ref to get the real head */
   LISTBOX *current = *head;
   LISTBOX *next = NULL;

   while (current != NULL)
   {
       next = current->next;
       free(current->item);
       free(current->path);
       free(current);
       current = next;
   }

   /* deref head_ref to affect the real head back
      in the caller. */
   *head = NULL;
}


/* addend: add new LISTBOX to the end of a list  */
/* usage example: listBox1 = (addend(listBox1, newelement("Item")); */
LISTBOX *addend(LISTBOX * head, LISTBOX * newp) {
  LISTBOX *p2;
  if(head == NULL) {
    newp->index = 0;
    newp->back = NULL;
    return newp;
  }
// now find the end of list
  for(p2 = head; p2->next != NULL; p2 = p2->next) ;
  p2->next = newp;
  newp->back = p2;
  newp->index = newp->back->index + 1;
  return head;
}

/* ---------------- */
/* Listbox routines */
/* ---------------- */
void displayItem2(LISTBOX * aux, SCROLLDATA * scrollData, int select)
//Select or unselect item animation
{
  switch (select) {

    case SELECT_ITEM:
      if (aux->index != 0 && aux->index != 1) {
        gotoxy(scrollData->wherex, scrollData->selector);
        outputcolor(scrollData->foreColor1, scrollData->backColor1);
        printf("%s\n", aux->item);
      }
      else{//First&second items are buttons
        gotoxy(scrollData->wherex, scrollData->selector);
        outputcolor(scrollData->foreColor1, B_CYAN);
        printf("%s\n", aux->item);
      }
      break;

    case UNSELECT_ITEM:
      if (aux->index != 0 && aux->index != 1) {
        gotoxy(scrollData->wherex, scrollData->selector);
        outputcolor(scrollData->foreColor0, scrollData->backColor0);
        printf("%s\n", aux->item);
      }
      else{ //First and second items are buttons
        gotoxy(scrollData->wherex, scrollData->selector);
        outputcolor(scrollData->foreColor0, scrollData -> backColor0);
        printf("%s\n", aux->item);
      }
      break;
  }
}

void gotoIndex(LISTBOX ** aux, SCROLLDATA * scrollData,
           unsigned indexAt)
//Go to a specific location on the list.
{
  LISTBOX *aux2;
  unsigned counter = 0;
  *aux = listBox1;
  aux2 = *aux;
  while(counter != indexAt) {
    aux2 = aux2->next;
    counter++;
  }
  //Highlight current item

  displayItem2(aux2, scrollData, SELECT_ITEM);

  //Update pointer
  *aux = aux2;
}

void loadlist(LISTBOX * head, SCROLLDATA * scrollData, unsigned indexAt) {
/*
Displays the items contained in the list with the properties specified
in scrollData.
*/

  LISTBOX *aux;
  unsigned wherey, counter = 0;

  aux = head;
  gotoIndex(&aux, scrollData, indexAt);
  /* Save values */
  //wherex = scrollData->wherex;
  wherey = scrollData->wherey;
  do {
    displayItem2(aux, scrollData, UNSELECT_ITEM);
    aux = aux->next;
    counter++;
    scrollData->selector++; // wherey++
  } while(counter != scrollData->displayLimit);
  scrollData->selector = wherey;    //restore value
}

int query_length(LISTBOX ** head) {
//Return no. items in a list.
  {
    LISTBOX *aux;

    unsigned itemCount = 0;
    aux = *head;
    while(aux->next != NULL) {
      aux = aux->next;
      itemCount++;
    }
    return itemCount;
  }

}

int move_display(LISTBOX ** selector, SCROLLDATA * scrollData) {
/*
Creates animation by moving a selector highlighting next item and
unselecting previous item
*/

  LISTBOX *aux;
  unsigned scrollControl = 0, continueScroll = 0, circular =
      CIRCULAR_INACTIVE;
  //Auxiliary pointer points to selector.
  aux = *selector;

  //Circular list animation when not scrolling.
  if(aux->index == scrollData->listLength - 1
     && scrollData->scrollActive == SCROLL_INACTIVE
     && scrollData->scrollDirection == DOWN_SCROLL) {
    //After last item go back to the top.
    displayItem2(aux, scrollData, UNSELECT_ITEM);
    scrollData->selector = scrollData->wherey;
    gotoIndex(&aux, scrollData, 0);
    *selector = aux;
    circular = CIRCULAR_ACTIVE;
  }

  if(aux->index == 0 && scrollData->scrollActive == SCROLL_INACTIVE
     && scrollData->scrollDirection == UP_SCROLL) {
    //Before first item go back to the bottom.
    displayItem2(aux, scrollData, UNSELECT_ITEM);
    scrollData->selector = scrollData->wherey + scrollData->listLength - 1;
    gotoIndex(&aux, scrollData, scrollData->listLength - 1);
    *selector = aux;
    circular = CIRCULAR_ACTIVE;
  }
  //Check if we do the circular list animation.
  //If active, we skip the code one time.

  if(circular == CIRCULAR_INACTIVE) {

    //Check if we are within boundaries.
    if((aux->next != NULL && scrollData->scrollDirection == DOWN_SCROLL)
       || (aux->back != NULL && scrollData->scrollDirection == UP_SCROLL)) {

      //Unselect previous item
      displayItem2(aux, scrollData, UNSELECT_ITEM);
      //Check whether we move UP or Down
      switch (scrollData->scrollDirection) {

    case UP_SCROLL:
      //Calculate new top index if scroll is active
      //otherwise it defaults to 0 (top)
      if(scrollData->scrollActive == SCROLL_ACTIVE)
        scrollControl = scrollData->currentListIndex;
      else
        scrollControl = 0;

      //Move selector
      if(aux->back->index >= scrollControl) {
        scrollData->selector--; //whereY--
        aux = aux->back;    //Go to previous item
      } else {
        if(scrollData->scrollActive == SCROLL_ACTIVE)
          continueScroll = 1;
        else
          continueScroll = 0;
      }
      break;

    case DOWN_SCROLL:
      //Calculate bottom index limit if scroll is ACTIVE
      //Otherwise it defaults to scrollData->ListLength-1

      if(scrollData->scrollActive == SCROLL_ACTIVE)
        scrollControl =
        scrollData->currentListIndex + (scrollData->displayLimit -
                        1);
      else
        scrollControl = scrollData->listLength - 1;

      //Move selector
      if(aux->next->index <= scrollControl) {
        aux = aux->next;    //Go to next item
        scrollData->selector++; //whereY++;
      } else {
        if(scrollData->scrollActive == SCROLL_ACTIVE)
          continueScroll = 1;
        else
          continueScroll = 0;
      }
      break;
      }

      //Metrics

      //cleanLine(window_y1 + 1, MENU_PANEL, MENU_FOREGROUND0, window_x1 + 1, window_x2);
      //outputcolor(MENU_FOREGROUND0, MENU_PANEL);
      gotoxy(window_x1 + 3, window_y2 - 1);
      outputcolor(MENU_FOREGROUND0, B_CYAN);
      if (aux->index != 0 && aux->index != 1) printf("    [%d/%d]    ", aux->index-1, scrollData->listLength - 2);
      else printf("               ");

      //Highlight new item
      displayItem2(aux, scrollData, SELECT_ITEM);

      //Update selector pointer
      *selector = aux;
    }
  }
  circular = CIRCULAR_INACTIVE;
  return continueScroll;
}

char selectorMenu(LISTBOX * aux, SCROLLDATA * scrollData) {
  char    ch;
  int control = 0;
  unsigned continueScroll;
  unsigned counter = 0;
  unsigned keypressed = 0;
  char chartrail[5];
  //Go to and select expected item at the beginning

  gotoIndex(&aux, scrollData, scrollData->currentListIndex);
  //Metrics
  //cleanLine(window_y1 + 1, MENU_PANEL, MENU_FOREGROUND0, window_x1 + 1, window_x2);
  //outputcolor(MENU_FOREGROUND0, MENU_PANEL);
  //gotoxy(window_x1 + 2, window_y1 + 1);
  //printf("Open File: w/s ^/v");
  gotoxy(window_x1 + 3, window_y2 - 1);
  outputcolor(MENU_FOREGROUND0, B_CYAN);
  if (aux->index != 0 && aux->index != 1) printf("    [%d/%d]    ", aux->index-1, scrollData->listLength - 2);
  else printf("               ");

  if(scrollData->scrollDirection == DOWN_SCROLL
     && scrollData->currentListIndex != 0) {
    //If we are going down we'll select the last item
    //to create a better scrolling transition (animation)
    for(counter = 0; counter < scrollData->displayLimit; counter++) {
      scrollData->scrollDirection = DOWN_SCROLL;
      move_display(&aux, scrollData);
    }
  } else {
    //Do nothing if we are going up. Selector is always at the top item.
  }

  //It break the loop everytime the boundaries are reached.
  //to reload a new list to show the scroll animation.
  while(control != CONTINUE_SCROLL) {
    keypressed = kbhit();
   if (keypressed == 1){
      ch = readch();
      keypressed = 0;
      //if enter key pressed - break loop
      if(ch == K_ENTER)
        control = CONTINUE_SCROLL;  //Break the loop
      //fail-safe keys
      if (ch == 'w'){
      //Move selector up
      scrollData->scrollDirection = UP_SCROLL;
      continueScroll = move_display(&aux, scrollData);
      //Break the loop if we are scrolling
      if(scrollData->scrollActive == SCROLL_ACTIVE
         && continueScroll == 1) {
        control = CONTINUE_SCROLL;
        //Update data
        scrollData->currentListIndex =
        scrollData->currentListIndex - 1;
        scrollData->selector = scrollData->wherey;
        scrollData->itemIndex = aux->index;
        //Return value
        ch = control;
      }
       }
      if (ch == 's'){
      //Move selector down
      scrollData->scrollDirection = DOWN_SCROLL;
      continueScroll = move_display(&aux, scrollData);
      //Break the loop if we are scrolling
      if(scrollData->scrollActive == SCROLL_ACTIVE
         && continueScroll == 1) {
        control = CONTINUE_SCROLL;
        //Update data
        scrollData->currentListIndex =
        scrollData->currentListIndex + 1;
        scrollData->selector = scrollData->wherey;
        scrollData->itemIndex = aux->index;
        scrollData->scrollDirection = DOWN_SCROLL;
      }
      //Return value
          ch = control;
       }
      //Check arrow keys
      if(ch == K_ESCAPE)        // escape key
      {
        read_keytrail(chartrail);   // read key again for arrow key combinations
        if(strcmp(chartrail, K_UP_TRAIL) == 0) {
      // escape key + A => arrow key up
      //Move selector up
      scrollData->scrollDirection = UP_SCROLL;
      continueScroll = move_display(&aux, scrollData);
      //Break the loop if we are scrolling
      if(scrollData->scrollActive == SCROLL_ACTIVE
         && continueScroll == 1) {
        control = CONTINUE_SCROLL;
        //Update data
        scrollData->currentListIndex =
        scrollData->currentListIndex - 1;
        scrollData->selector = scrollData->wherey;
        scrollData->itemIndex = aux->index;
        //Return value
        ch = control;
      }
       }
      if(strcmp(chartrail, K_DOWN_TRAIL) == 0) {
    // escape key + B => arrow key down
      //Move selector down
      scrollData->scrollDirection = DOWN_SCROLL;
      continueScroll = move_display(&aux, scrollData);
      //Break the loop if we are scrolling
      if(scrollData->scrollActive == SCROLL_ACTIVE
         && continueScroll == 1) {
        control = CONTINUE_SCROLL;
        //Update data
        scrollData->currentListIndex =
        scrollData->currentListIndex + 1;
        scrollData->selector = scrollData->wherey;
        scrollData->itemIndex = aux->index;
        scrollData->scrollDirection = DOWN_SCROLL;
      }
      //Return value
          ch = control;
      }
      }
 // if (ch == K_TAB) break;
  if(ch == K_ENTER || ch == K_ENTER2)       // enter key
  {
    //Pass data of last item selected.
    //scrollData->item =
    //(char *)imalloc(sizeof(char) * strlen(aux->item) + 1);
    //scrollData->path = (char *)malloc(sizeof(char) *strlen(aux->path) + 1);
    scrollData->item = aux->item;
    scrollData->itemIndex = aux->index;

    //strcpy(scrollData->path, aux->path);
    scrollData->path = aux->path;
    scrollData->isDirectory = aux->isDirectory;
  }
  }
  }
  return ch;
}

char listBox(LISTBOX * head,
         unsigned whereX, unsigned whereY,
         SCROLLDATA * scrollData, unsigned bColor0,
         unsigned fColor0, unsigned bColor1, unsigned fColor1,
         unsigned displayLimit) {

  unsigned list_length = 0;
  //unsigned currentIndex = 0;
  int     scrollLimit = 0;
  unsigned currentListIndex = 0;
  char    ch;
  LISTBOX *aux;

  // Query size of the list
  list_length = query_length(&head) + 1;

  //Save calculations for SCROLL and store DATA
  scrollData->displayLimit = displayLimit;
  scrollLimit = list_length - scrollData->displayLimit; //Careful with negative integers

  if(scrollLimit < 0)
    scrollData->displayLimit = list_length; //Failsafe for overboard values

  scrollData->scrollLimit = scrollLimit;
  scrollData->listLength = list_length;
  scrollData->wherex = whereX;
  scrollData->wherey = whereY;
  scrollData->selector = whereY;
  scrollData->backColor0 = bColor0;
  scrollData->backColor1 = bColor1;
  scrollData->foreColor0 = fColor0;
  scrollData->foreColor1 = fColor1;

  //Check whether we have to activate scroll or not
  //and if we are within bounds. [1,list_length)

  if(list_length > scrollData->displayLimit && scrollLimit > 0
     && displayLimit > 0) {
    //Scroll is possible

    scrollData->scrollActive = SCROLL_ACTIVE;
    aux = head;

    currentListIndex = 0;   //We listBox1 the scroll at the top index.
    scrollData->currentListIndex = currentListIndex;

    //Scroll loop animation. Finish with ENTER.
    do {
      currentListIndex = scrollData->currentListIndex;
      loadlist(aux, scrollData, currentListIndex);
      gotoIndex(&aux, scrollData, currentListIndex);
      ch = selectorMenu(aux, scrollData);
      //if (ch == K_TAB) break;
    } while(ch != K_ENTER);

  } else {
    //Scroll is not possible.
    //Display all the elements and create selector.
    scrollData->scrollActive = SCROLL_INACTIVE;
    scrollData->currentListIndex = 0;
    scrollData->displayLimit = list_length; //Default to list_length
    loadlist(head, scrollData, 0);
    ch = selectorMenu(head, scrollData);
  }
  return ch;
}

/* ---------------- */
/* List files       */
/* ---------------- */

int addSpaces(char temp[MAX_ITEM_LENGTH]) {
  int     i;
  for(i = strlen(temp); i < MAX_ITEM_LENGTH; i++) {
    strcat(temp, " ");
  }
  return 0;
}

void cleanString(char *string, int max) {
  int     i;
  for(i = 0; i < max; i++) {
    string[i] = ' ';
  }
}
int listFiles(LISTBOX ** listBox1, char *directory) {
  DIR    *d;
  struct dirent *dir;
  int     i;
  char    temp[MAX_ITEM_LENGTH];
  int     lenDir;       //length of directory

  //Add elements to switch directory at the beginning for convenience.
  strcpy(temp, "[CLOSE WINDOW]");
  //Add spaces
  addSpaces(temp);
  *listBox1 =
      addend(*listBox1, newelement(temp, "[CLOSE WINDOW]", DIRECTORY));
  strcpy(temp, "[SET FILENAME]");
  //Add spaces
  addSpaces(temp);
 *listBox1 =
      addend(*listBox1, newelement(temp, "[SET FILENAME]", DIRECTORY));
  strcpy(temp, CHANGEDIR);
  //Add spaces
  addSpaces(temp);
  *listBox1 = addend(*listBox1, newelement(temp, CHANGEDIR, DIRECTORY));    // ".."

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
      cleanString(temp, MAX_ITEM_LENGTH);
      strcpy(temp, "[");
      for(i = 1; i < MAX_ITEM_LENGTH - 1; i++) {
        temp[i] = dir->d_name[i - 1];
      }
      temp[MAX_ITEM_LENGTH - 1] = ']';
    } else {
      //Directory's name is shorter than display
      //Add spaces to item string.
      cleanString(temp, MAX_ITEM_LENGTH);
      strcpy(temp, "[");
      for(i = 1; i < lenDir + 1; i++) {
        temp[i] = dir->d_name[i - 1];
      }
      temp[lenDir + 1] = ']';
      addSpaces(temp);
    }
    //Add all directories except CURRENTDIR and CHANGEDIR
    if(strcmp(dir->d_name, CURRENTDIR) != 0
       && strcmp(dir->d_name, CHANGEDIR) != 0)
      *listBox1 =
          addend(*listBox1, newelement(temp, dir->d_name, DIRECTORY));
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
      cleanString(temp, MAX_ITEM_LENGTH);
      strcpy(temp, dir->d_name);
      //Add spaces
      addSpaces(temp);
    }
    *listBox1 =
        addend(*listBox1, newelement(temp, dir->d_name, FILEITEM));
      }
    }
    closedir(d);
  }
  return 0;
}

void changeDir(SCROLLDATA * scrollData, char fullPath[MAX],
           char newDir[MAX]) {
//Change dir
  char    oldPath[MAX];
  if(scrollData->isDirectory == DIRECTORY) {
    if(scrollData->itemIndex == 1) {
      //cd ..
      cleanString(fullPath, MAX);
      cleanString(oldPath, MAX);
      cleanString(newDir, MAX);
      chdir("..");
      getcwd(oldPath, sizeof(oldPath));
      strcpy(newDir, oldPath);
      strcpy(fullPath, oldPath);
    } else {
      //cd newDir
      cleanString(fullPath, MAX);
      cleanString(newDir, MAX);
      cleanString(oldPath, MAX);
      getcwd(oldPath, sizeof(oldPath));
      strcat(oldPath, "/");
      strcat(oldPath, scrollData->path);
      chdir(oldPath);
      strcpy(newDir, oldPath);
      strcpy(fullPath, oldPath);
    }
  }
}
int setFileName(char fileName[MAX]) {
  char    tempFile[MAX];
  int     ok, count;

  clearString(tempFile, MAX);

  ok = 0;
  count = inputWindow("Set file:", tempFile, "[-] C-EDIT INFO");
  if(count > 0) {
    //Check whether file exists and create file.
      strcpy(fileName, tempFile);
      ok = 1;
  }
  return ok;
}


/* ---------------- */
/* Main             */
/* ---------------- */

/*========================================================================*/
/*
  ListBox with Scroll:
  ____________________

  Usage:

  listBox(headpointer, whereX, whereY, scrollData, backColor0, foreColor0,
backcolor1, forecolor1, displayLimit); */

/*========================================================================*/

void openFileDialog(SCROLLDATA * openFileData) {
  SCROLLDATA scrollData;
  char    ch;
  char    fullPath[MAX];
  char    fileName[MAX];
  char    newDir[MAX];
  int     exitFlag = 0;
  int     i,ok;
  int     rows, columns;

//init values
  scrollData.scrollActive=0;    //To know whether scroll is active or not.
  scrollData.scrollLimit=0;     //Last index for scroll.
  scrollData.listLength=0;      //Total no. of items in the list
  scrollData.currentListIndex=0;    //Pointer to new sublist of items when scrolling.
  scrollData.displayLimit=0;    //No. of elements to be displayed.
  scrollData.scrollDirection=0; //To keep track of scrolling Direction.
  scrollData.selector=0;        //Y++
  scrollData.wherex=0;
  scrollData.wherey=0;
  scrollData.backColor0=0;      //0 unselected; 1 selected
  scrollData.foreColor0=0;
  scrollData.backColor1=0;
  scrollData.foreColor1=0;
  scrollData.isDirectory=0;     // Kind of item
  scrollData.item =NULL;
  scrollData.path =NULL;
  scrollData.itemIndex=0;
  get_terminal_dimensions(&rows, &columns);
  //Check if the screen is active in memory first.

  window_y1 = (rows / 2) - 7;
  window_y2 = (rows / 2) + 7;
  window_x1 = (columns / 2) - 10;
  window_x2 = (columns / 2) + 10;

  //Change background color

  strcpy(newDir, ".");      //We start at current dir
  getcwd(fullPath, sizeof(fullPath));   //Get path

  //Directories loop
  draw_window(window_x1, window_y1, window_x2, window_y2, MENU_PANEL, MENU_FOREGROUND0, WINDOW_TITLEB,1,1);
  write_str((window_x2-window_x1) /2 + window_x1 - (strlen(OPENWTITLE)/2) , window_y1-1, OPENWTITLE, WINDOW_TITLEB, WINDOW_TITLEF);
  update_screen();

  for (i=window_y1+1; i<window_y2-1;i++){
    gotoxy(window_x1 + 1, i);
    outputcolor(MENU_FOREGROUND0, MENU_PANEL);
    printf("                   ");
  }

  outputcolor(MENU2_FOREGROUND0, B_CYAN);
  gotoxy(window_x1 + 1, window_y2-1);
  printf("                   ");
  do {
    //Add items to list
    listFiles(&listBox1, newDir);
    ch = listBox(listBox1, window_x1 + 3, window_y1 + 1, &scrollData,
         MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1, 12);


    //Scroll Loop exit conditions
    if(scrollData.itemIndex == 0)
      exitFlag = 1;     //First item is selected

    //Second item is selected - set file
    if(scrollData.itemIndex == 1)
    {
       ok = setFileName(fileName);
    if (ok==1) {
       //Let's send data to main module
        strcpy(scrollData.item, "\0");
        strcpy(scrollData.item, fileName);
        strcpy(scrollData.path, "\0");
        strcpy(scrollData.path, fileName);
        exitFlag = 1;
    }

    } else
    {
      //Change Dir. New directory is copied in newDir
      changeDir(&scrollData, fullPath, newDir);
    }

    //Clean all lines on the window
    for(i = window_y1 + 3; i < window_y2; i++) {
      cleanLine(i, MENU_PANEL, MENU_FOREGROUND0, window_x1 + 1, window_x2);
    }
    outputcolor(MENU2_FOREGROUND0, B_CYAN);
    gotoxy(window_x1 + 1, window_y2-1);
    printf("                   ");

    if(ch == K_ENTER && scrollData.isDirectory == FILEITEM)
      exitFlag = 1;     //File selected
    //if (ch == K_TAB) break;
    openFileData->item = scrollData.item;
    openFileData->itemIndex = scrollData.itemIndex;
    openFileData->path = scrollData.path;
    //Save full path
    strcpy(openFileData->fullPath, fullPath);
    strcat(openFileData->fullPath, "/");
    strcat(openFileData->fullPath, scrollData.path);
    openFileData->isDirectory = scrollData.isDirectory;

    if(listBox1 != NULL && exitFlag != 1) {
        deleteList(&listBox1);
        listBox1 = NULL;
    }
  } while(exitFlag != 1);

  //Return file selected by copying into fileToOpen -> currentFile
  close_window();
  resetch();
  if(scrollData.itemIndex != 0){
   openFileData->item =
      (char *)malloc(sizeof(char) * strlen(scrollData.item)+1);
   openFileData->path =
      (char *)malloc(sizeof(char) * strlen(scrollData.path)+1);
    strcpy(openFileData->item, scrollData.item);
    strcpy(openFileData->path, scrollData.path);
  }
  deleteList(&listBox1);
}


