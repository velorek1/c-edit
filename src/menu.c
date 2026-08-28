/* Module to display Menus on C·edit 
 * for a Text User Interface
 * TextBox
 * Window
 * Last modified: 6/4/2024
 * @author:velorek
 */
#include <stdio.h>
#include "rterm.h"
#include "scbuf.h"
#include "ui.h"
#include "menu.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
#include "editor.h"
#include "opfile.h"
#include "fileb.h"
/*--------------------------------------------*/
/* Load current menu into circular linked list*/
/*--------------------------------------------*/

void loadmenus(int choice) {

  if(choice == HOR_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("File", 0, 1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Options", 7, 1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Help", 16, 1,-1,-1));
    }
  

  if(choice == FILE_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("New", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Open", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Quick load", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Save", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Save as...", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Exit", -1, -1,-1,-1));
 }
  if(choice == OPT_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("File Info", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Find...", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Go to line...", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("Colors", -1, -1,-1,-1));	
  }
  if(choice == HELP_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("Help...", -1, -1,-1,-1));
	listBox1 = addatend(listBox1, newitem("About", -1, -1,-1,-1));	
 }
/*
  if(choice == YESNO_MENU) {
    add_item(mylist, "[YES]", (columns / 2) - 11, (rows / 2) + 2, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "[NO]", (columns / 2) - 3, (rows / 2) + 2,  MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "[CANCEL]", (columns / 2) + 4, (rows / 2) + 2,  MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OK_MENU) {
    add_item(mylist, "[OK]", (columns / 2) - 1, (rows / 2) + 2, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OK_MENU2) {
    add_item(mylist, "[OK]", (columns / 2) - 1, (rows / 2) + 3, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == COLORS_MENU) {
    add_item(mylist, "C-Edit Theme", (columns / 2) - 6, (rows / 2) - 2, 
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Classic Theme", (columns / 2) - 6, (rows / 2) -1, 
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Dark Theme", (columns / 2) - 6, (rows / 2), 
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
*/
}

/*---------------
 Menu loop
It depends on two values passed by reference stored in the function from where this routine is called:
-returnMenuChar
-menuCounter
It does some tricky modulo operation to cycle back
*/
void handlemenus(char *returnMenuChar, int *menuCounter, BOOL horizontalMenu)
{      

      setselectorLimit(15);
      copy_screen(screen2,screen1);
      if (horizontalMenu) *returnMenuChar= horizontal_menu();  
      do{
      switch (*returnMenuChar) {
	case 0: *menuCounter=FILE_MENU; *returnMenuChar=filemenu();  if (*returnMenuChar != DONT_UPDATE) xor_update(screen2,screen1); break;
     	case 1: *menuCounter=OPT_MENU; *returnMenuChar=optionsmenu();  xor_update(screen2,screen1); break;
	case 2: *menuCounter=HELP_MENU; *returnMenuChar=helpmenu(); xor_update(screen2,screen1);  break;
	default:
		break;
      } 
      //convert -2 from right cursor to 1 to advance     
      if (*returnMenuChar == -2 ) *returnMenuChar = 1; 
      *menuCounter=*menuCounter + *returnMenuChar;  
      if (*returnMenuChar == K_ENTER) break;
      if (*returnMenuChar == DONT_UPDATE) break;
      if (*returnMenuChar != ESC_KEY ) {
     			copy_screen(screen1,screen2);
	      		//euclidian modulo, we circle back through the different switch cases
	                *returnMenuChar = ((*menuCounter % 3) + 3) % 3; 
	 }

     } while (*returnMenuChar != ESC_KEY);
     //xor_update(screen2,screen1);
    if (*returnMenuChar != DONT_UPDATE){
      copy_screen(screen1,screen2);
      dump_screen(screen1);
   }
}
//Entry menu- horizontal
char horizontal_menu() {
char ch=0;

         dump_screen(screen1);
         //Save current screen to screen2
         loadmenus(HOR_MENU);	
	//load menus from ui.c onto lisBox in (global.c)
	//dump_screen(screen1);
        ch = listBox(listBox1, 0, 1 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  3, HORIZONTAL,0,1); 
        //delete listbox
	if(ch == ESC_KEY) scrollData.itemIndex = ch;; //to avoid warning
	removeList(&listBox1);
        //return option passed in scrollData (global.c)
	return scrollData.itemIndex;
}
/*-------------------------*/
/* Display File menu       */
/*-------------------------*/
char filemenu() {  
  char ch=0;
  int countCh = 0;
  char tempfileName[MAXFILENAME];
  int ok2 = -1;
  int retvalue = 0;
  write_str(screen1,0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  write_str(screen1,0, 1, "File", MENU_SELECTOR, MENU_FOREGROUND1,1);
  loadmenus(FILE_MENU);
  draw_window(screen1,0, 2, 13, 9, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 3, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
      
  //return if right and left arrow keys are pressed
  if (ch == K_RIGHTMENU || ch == K_LEFTMENU) return ch;

  if(scrollData.itemIndex == OPTION_1) {
    flush_editarea(0);
    buffertoScreen(0);
    if (fileModified == FILE_MODIFIED) {
      ok2 = yesnoWindow("File is modified.|Save before creating new?", "New File");
      if (ok2 == 0) {
        if (strcmp(fileName, "UNTITLED") == 0) {
          countCh = inputWindow("File:", tempfileName, "Save file as...", 26, 2, 44);
          if (countCh > 0) {
            strcpy(fileName, tempfileName);
            buffertoFile(fileName);
          }
        } else {
          buffertoFile(fileName);
        }
      } else if (ok2 == 2) {
        dump_screen(screen1);
        return DONT_UPDATE;
      }
    }
    if (edBuf1 != NULL) _deletetheList(&edBuf1);
    memset(&tempLine, 0, sizeof(tempLine));
    tempLine.index = 0;
    tempLine.linea[0].ch = END_LINE_CHAR;
    edBuf1 = _addatend(edBuf1, _newline(tempLine));
    strcpy(fileName, "UNTITLED");
    fullPath[0] = '\0';
    posBufX = 0; posBufY = 0;
    cursorX = START_CURSOR_X; cursorY = START_CURSOR_Y;
    currentLine = 0; shiftH = 0;
    fileModified = FILE_UNMODIFIED;
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    strcpy(tempMessage, "[New file created]");
    timer3.ticks = 0;
    return DONT_UPDATE;
  }
  
  if(scrollData.itemIndex == OPTION_2) {
    flush_editarea(0);
    buffertoScreen(0);
    if (openFileDialog(fileName,fullPath) == 1){
      filetoBuffer(fileName);
      cursorX = START_CURSOR_X; cursorY = START_CURSOR_Y;
      currentLine = 0; shiftH = 0;
      posBufX = 0; posBufY = 0;
      flush_editarea(0);
      buffertoScreen(0);
      sprintf(tempMessage, "[Loaded %s]", fileName);
      timer3.ticks = 0;
    }
    dump_screen(screen1);  
    return DONT_UPDATE;
  }

  if(scrollData.itemIndex == OPTION_3) {
    flush_editarea(0);
    buffertoScreen(0);
    countCh=inputWindow("File:", tempfileName, "Quick load...",26,2,44);
    if (countCh>0) {
      if (openandcheckFile(tempfileName) == 1){
        ok2 = yesnoWindow(WCHECKFILE_MSG, "Alert window");
        if (ok2 == 0) retvalue = 1;
        else retvalue = 0;
      } else {
        retvalue = 1;
      }
      if (retvalue == 1){
        strcpy(fileName, tempfileName);
        fullPath[0] = '\0';
        filetoBuffer(fileName);
        cursorX = START_CURSOR_X; cursorY = START_CURSOR_Y;
        currentLine = 0; shiftH = 0;
        posBufX = 0; posBufY = 0;
        flush_editarea(0);
        buffertoScreen(0);
        dump_screen(screen1);
        sprintf(tempMessage, "[Loaded %s]", fileName);
        timer3.ticks = 0;
      }
    }
    dump_screen(screen1);
    return DONT_UPDATE;
  }

  if(scrollData.itemIndex == OPTION_4) {
    flush_editarea(0);
    buffertoScreen(0);
    if (strcmp(fileName, "UNTITLED") == 0) {
      countCh=inputWindow("File:", tempfileName, "Save file as...",26,2,44);
      if (countCh>0) {
        strcpy(fileName, tempfileName);
        buffertoFile(fileName);
        strcpy(tempMessage, "[File saved!]");
        timer3.ticks = 0;
      }	
    } else{
      buffertoFile(fileName);
      strcpy(tempMessage, "[File saved!]");
      timer3.ticks = 0;
    }
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    return DONT_UPDATE;
  }

  if(scrollData.itemIndex == OPTION_5) {
    flush_editarea(0);
    buffertoScreen(0);
    countCh=inputWindow("File:", tempfileName, "Save file as...",26,2,44);
    if (countCh>0) {
      strcpy(fileName, tempfileName);
      buffertoFile(fileName);
      strcpy(tempMessage, "[File saved!]");
      timer3.ticks = 0;
    }
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    return DONT_UPDATE;
  }

  if(scrollData.itemIndex == OPTION_6) {
    if(fileModified == FILE_MODIFIED) {
      ok2 = yesnoWindow(WMODIFIED_MSG, "Alert Window");
      if (ok2 == 0) {
        if (strcmp(fileName, "UNTITLED") == 0) {
          countCh = inputWindow("File:", tempfileName, "Save file as...", 26, 2, 44);
          if (countCh > 0) {
            strcpy(fileName, tempfileName);
            buffertoFile(fileName);
            programStatus = ENDSIGNAL;
          }
        } else {
          buffertoFile(fileName);
          programStatus = ENDSIGNAL;
        }
      } else if (ok2 == 1) {
        programStatus = ENDSIGNAL;
      }
    } else {
      programStatus = ENDSIGNAL;
    }
    return DONT_UPDATE;
  }
	
  return ch;
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

char optionsmenu() {
  char ch=0;

  write_str(screen1,6, 1, "Options", MENU_SELECTOR, MENU_FOREGROUND1,1);
  write_str(screen1, 0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  loadmenus(OPT_MENU);
  draw_window(screen1,6, 2, 23, 7, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 9, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
  
  if (ch == K_RIGHTMENU || ch == K_LEFTMENU) return ch;

  if(scrollData.itemIndex == OPTION_1 && ch > 0) {
    flush_editarea(0);
    buffertoScreen(0);
    fileInfoDialog();
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    return DONT_UPDATE;
  }
  if(scrollData.itemIndex == OPTION_2 && ch > 0) {
    flush_editarea(0);
    buffertoScreen(0);
    findDialog();
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    return DONT_UPDATE;
  }
  if(scrollData.itemIndex == OPTION_3 && ch > 0) {
    flush_editarea(0);
    buffertoScreen(0);
    gotoLineDialog();
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    return DONT_UPDATE;
  }
  if(scrollData.itemIndex == OPTION_4 && ch > 0) {
    flush_editarea(0);
    buffertoScreen(0);
    colorsDialog();
    flush_editarea(0);
    buffertoScreen(0);
    dump_screen(screen1);
    return DONT_UPDATE;
  }
  return ch;
}

/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

char helpmenu() { 
  char ch= 0;

  write_str(screen1,15, 1, "Help", MENU_SELECTOR, MENU_FOREGROUND1,1);
  write_str(screen1, 0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  loadmenus(HELP_MENU);
  draw_window(screen1,15, 2, 25, 5, MENU_PANEL, MENU_FOREGROUND0, 0,1,0,1,1);
  ch = listBox(listBox1, 18, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
  if(scrollData.itemIndex == OPTION_1 && ch>0) {
    //Help dialog
    //help_info();
    flush_editarea(0);
    buffertoScreen(0); 
     displayHelp();
  }
  if(scrollData.itemIndex == OPTION_2 && ch>0) {
    //About info
    flush_editarea(0);
    buffertoScreen(0);
  
   displayAbout();
  }
  return ch;
}

//ABOUT

int displayAbout(void)
{
	char ch = 0;
	int i = 0;
	int colAnimation = F_BLACK;
	int keypressed = 0;
        int colCounter = 0;
	copy_screen(screen2, screen1);

	window(screen1, (new_columns / 2) - 18, (new_rows / 2) - 6,
	       (new_columns / 2) + 17, (new_rows) / 2 + 3, B_WHITE, F_BLACK,
	       B_BLACK, 1, 1, 1);
	
	for (i = 0; i < 6; i++) {
		write_str(screen1, (new_columns / 2) - 15,
			  (new_rows / 2) - 5 + i, aboutMSG[i], B_WHITE,
			  F_BLACK, 0);
	}
	
	write_str(screen1, (new_columns / 2) - 11, (new_rows / 2) - 7,
		 "[+] ABOUT INFORMATION", B_BLACK, FH_WHITE, 1);
		 
//	write_str(screen1, (new_columns / 2) - 25, (new_rows / 2) + 2,
		  //aboutMSG[3], B_YELLOW, F_BLACK, 0);
	write_str(screen1, (new_columns / 2) - 1, (new_rows / 2) + 2, "[OK]",
		  B_RED, FH_WHITE, 1);
	
	dump_screen(screen1);
	
	if (kbhit(100) == 1)
		ch = readch();
	ch = 0;
	i = 0;
	colCounter = 0;
	//pulsating animation
	do {
		keypressed = kbhit(30);
		if (timerC(&timer2) == TRUE) {
			//About animation            
			write_str(screen1, (new_columns / 2) - 15,
				  (new_rows / 2) - 5 + i, aboutMSG[i], B_WHITE,
				  colAnimation, 1);
			i++;
			if (i == 6) {
				i = 0;
				switch (colCounter){
				case 1:
					colAnimation = FH_BLACK;
					break;
				case 2: 
					colAnimation = FH_BLACK;
					break;
				case 3: 
					colAnimation = FH_BLACK;
					break;
				case 4: 
					colAnimation = FH_BLACK;
					break;
				case 5: 
					colAnimation = FH_BLACK;
					break;
				case 6: 
					colAnimation = FH_WHITE;
					break;
				case 7: 
					colAnimation = FH_WHITE;
					break;
				case 8: 
					colAnimation = FH_WHITE;
					break;
				case 9: 
					colAnimation = F_BLACK;
					break;
				case 10: 
					colAnimation = F_BLACK;
					break;
					

				}
				colCounter++;
				if (colCounter ==11) colCounter = 0;
			}
			//if terminal resizes
			if (_animation() == -1)
				break;
		}
		//Process keys
		if (keypressed == 1) {
			ch = readch();
			keypressed = 0;

			//Read special keys
			if (ch == K_ESCAPE) {
				ch = readch();
				if (ch == ESC_KEY)
				  break;
			}

		}
	} while (ch != K_ENTER);
        resetch();
	copy_screen(screen1, screen2);
	if (listBox1 != NULL)
		removeList(&listBox1);
	dump_screen(screen1);
	return ch;
}
void addItemsHelp(LISTCHOICE **listBox1, char textarray[][MAXLINE], int rows)
{
	int h=0;
//Load items into the list.  
	//if (*listBox1 != NULL) removeList(listBox1);
	for (h = 0; h < rows-1; h++) {
		//*ch = textarray[h];
		*listBox1 = addatend(*listBox1, newitem(textarray[h],-1,-1,-1,-1));
	}
		*listBox1 = addatend(*listBox1, newitem(textarray[rows-1],-1,-1,B_WHITE,FH_BLUE));
}
int displayHelp(void)
{
	char ch = 0;
	resetScrollData(&scrollData);
	if (listBox1 != NULL)
		removeList(&listBox1);
	
	setselectorLimit(26*2-1);
	//scrollData.selectorLimit = (26 * 2) - 1;	//No. of chars per item display
	//scrollData.selectorLimit = 100;	//No. of chars per item display
 //create_screen(&screen2);
	copy_screen(screen2, screen1);
	window(screen1, (new_columns / 2) - 26, (new_rows / 2) - 8,
		    (new_columns / 2) + 26, (new_rows) / 2 + 8, B_WHITE, F_BLACK,
		    B_BLACK, 1, 1, 1);
	write_str(screen1, (new_columns / 2) - 12, (new_rows / 2) - 9,
		 "[+] MAIN HELP INFORMATION", B_BLACK, FH_WHITE, 1);
	
	write_str(screen1, (new_columns / 2) - 1, (new_rows / 2) + 7, "[OK]",
		  B_RED, F_WHITE, 1);
	dump_screen(screen1);
	addItemsHelp(&listBox1, help, HELPLINES);
	if (listBox1 != NULL)
		ch = listBox(listBox1, (new_columns / 2) - 24,
			     (new_rows / 2) - 7, &scrollData, B_WHITE, F_BLACK,
			     B_WHITE, FH_WHITE, 14, VERTICAL,1,LOCKED);
	copy_screen(screen1, screen2);
//	if (screen2 != NULL)
//		deleteList(&screen2);
	if (listBox1 != NULL)
		removeList(&listBox1);
	dump_screen(screen1);
	return ch;
}

int find_text(const char *query, int startY, int startX) {
  if (edBuf1 == NULL || query == NULL || query[0] == '\0') return 0;
  int total = _length(&edBuf1);
  if (total <= 0) return 0;
  int qlen = strlen(query);
  VLINES sLine;

  for (int y = startY; y < total; y++) {
    _dumpLine(edBuf1, y, &sLine);
    int lineLen = findEndline(sLine);
    int fromX = (y == startY) ? startX : 0;
    for (int x = fromX; x <= lineLen - qlen; x++) {
      int match = 1;
      for (int k = 0; k < qlen; k++) {
        if (sLine.linea[x + k].ch != query[k]) {
          match = 0;
          break;
        }
      }
      if (match) {
        posBufY = y;
        posBufX = x;
        if (posBufY < currentLine || posBufY >= currentLine + vdisplayArea) {
          currentLine = posBufY - vdisplayArea / 2;
          if (currentLine < 0) currentLine = 0;
          if (currentLine + vdisplayArea > total && total > vdisplayArea) {
            currentLine = total - vdisplayArea;
          }
        }
        cursorY = START_CURSOR_Y + (posBufY - currentLine);
        if (posBufX < shiftH || posBufX >= shiftH + hdisplayArea) {
          shiftH = posBufX - hdisplayArea / 4;
          if (shiftH < 0) shiftH = 0;
        }
        cursorX = START_CURSOR_X + (posBufX - shiftH);
        return 1;
      }
    }
  }

  for (int y = 0; y <= startY && y < total; y++) {
    _dumpLine(edBuf1, y, &sLine);
    int lineLen = findEndline(sLine);
    int toX = (y == startY) ? startX : lineLen - qlen;
    for (int x = 0; x <= toX; x++) {
      int match = 1;
      for (int k = 0; k < qlen; k++) {
        if (sLine.linea[x + k].ch != query[k]) {
          match = 0;
          break;
        }
      }
      if (match) {
        posBufY = y;
        posBufX = x;
        if (posBufY < currentLine || posBufY >= currentLine + vdisplayArea) {
          currentLine = posBufY - vdisplayArea / 2;
          if (currentLine < 0) currentLine = 0;
          if (currentLine + vdisplayArea > total && total > vdisplayArea) {
            currentLine = total - vdisplayArea;
          }
        }
        cursorY = START_CURSOR_Y + (posBufY - currentLine);
        if (posBufX < shiftH || posBufX >= shiftH + hdisplayArea) {
          shiftH = posBufX - hdisplayArea / 4;
          if (shiftH < 0) shiftH = 0;
        }
        cursorX = START_CURSOR_X + (posBufX - shiftH);
        return 2;
      }
    }
  }

  return 0;
}

int findDialog(void) {
  char searchPrompt[MAX_TEXT] = "";
  if (lastSearchStr[0] != '\0') {
    strcpy(searchPrompt, lastSearchStr);
  }
  int cnt = inputWindow("Find:", searchPrompt, "Find Text...", 26, 2, 40);
  if (cnt > 0 && searchPrompt[0] != '\0') {
    strcpy(lastSearchStr, searchPrompt);
    int res = find_text(lastSearchStr, posBufY, posBufX + 1);
    if (res == 1) {
      sprintf(tempMessage, "[Found '%s']", lastSearchStr);
      timer3.ticks = 0;
    } else if (res == 2) {
      sprintf(tempMessage, "[Found '%s' (wrapped)]", lastSearchStr);
      timer3.ticks = 0;
    } else {
      sprintf(tempMessage, "['%s' not found]", lastSearchStr);
      timer3.ticks = 0;
    }
  }
  return 0;
}

int gotoLineDialog(void) {
  char lineStr[32] = "";
  int cnt = inputWindow("Line:", lineStr, "Go to Line...", 20, 2, 12);
  if (cnt > 0 && lineStr[0] != '\0') {
    int target = atoi(lineStr);
    int total = _length(&edBuf1);
    if (target < 1) target = 1;
    if (target > total) target = total;

    posBufY = target - 1;
    posBufX = 0;
    shiftH = 0;
    cursorX = START_CURSOR_X;

    if (posBufY < currentLine || posBufY >= currentLine + vdisplayArea) {
      currentLine = posBufY - vdisplayArea / 2;
      if (currentLine < 0) currentLine = 0;
      if (currentLine + vdisplayArea > total && total > vdisplayArea) {
        currentLine = total - vdisplayArea;
      }
    }
    cursorY = START_CURSOR_Y + (posBufY - currentLine);
    sprintf(tempMessage, "[Line %d / %d]", target, total);
    timer3.ticks = 0;
  }
  return 0;
}

int fileInfoDialog(void) {
  char ch = 0;
  int keypressed = 0;
  copy_screen(screen2, screen1);

  int win_w = 46;
  int win_h = 12;
  int x1 = (new_columns / 2) - (win_w / 2);
  int x2 = x1 + win_w;
  int y1 = (new_rows / 2) - (win_h / 2);
  int y2 = y1 + win_h;

  window(screen1, x1, y1, x2, y2, MENU_PANEL, MENU_FOREGROUND0, WINDOW_TITLEB, 1, 1, 1);
  write_str(screen1, (x2 - x1) / 2 + x1 - 7, y1 - 1, "[+] FILE INFO", WINDOW_TITLEB, WINDOW_TITLEF, 1);

  int total_lines = _length(&edBuf1);
  long total_chars = 0;
  long total_words = 0;
  VLINES infoLine;
  for (int j = 0; j < total_lines; j++) {
    _dumpLine(edBuf1, j, &infoLine);
    int elen = findEndline(infoLine);
    total_chars += elen;
    int in_word = 0;
    for (int k = 0; k < elen; k++) {
      if (infoLine.linea[k].ch != ' ' && infoLine.linea[k].ch != '\t' && infoLine.linea[k].ch != 0) {
        if (!in_word) { in_word = 1; total_words++; }
      } else {
        in_word = 0;
      }
    }
  }

  char buf[128];
  sprintf(buf, "File Name   : %-26s", fileName[0] ? fileName : "UNTITLED");
  write_str(screen1, x1 + 3, y1 + 2, buf, MENU_PANEL, MENU_FOREGROUND0, 1);

  sprintf(buf, "Status      : %-26s", (fileModified == FILE_MODIFIED) ? "Modified (*)" : "Saved / Clean");
  write_str(screen1, x1 + 3, y1 + 3, buf, MENU_PANEL, (fileModified == FILE_MODIFIED) ? FH_RED : FH_GREEN, 1);

  sprintf(buf, "Total Lines : %-26d", total_lines);
  write_str(screen1, x1 + 3, y1 + 4, buf, MENU_PANEL, MENU_FOREGROUND0, 1);

  sprintf(buf, "Total Words : %-26ld", total_words);
  write_str(screen1, x1 + 3, y1 + 5, buf, MENU_PANEL, MENU_FOREGROUND0, 1);

  sprintf(buf, "Total Chars : %-26ld", total_chars);
  write_str(screen1, x1 + 3, y1 + 6, buf, MENU_PANEL, MENU_FOREGROUND0, 1);

  sprintf(buf, "Cursor Pos  : Line %ld, Col %ld", posBufY + 1, posBufX + 1);
  write_str(screen1, x1 + 3, y1 + 7, buf, MENU_PANEL, MENU_FOREGROUND0, 1);

  write_str(screen1, (x2 - x1) / 2 + x1 - 3, y2 - 2, "[  OK  ]", B_RED, FH_WHITE, 1);
  dump_screen(screen1);

  if (kbhit(100) == 1) ch = readch();
  ch = 0;

  do {
    keypressed = kbhit(50);
    if (timerC(&timer2) == TRUE) {
      if (_animation() == -1) break;
    }
    if (keypressed == 1) {
      ch = readch();
      keypressed = 0;
      if (ch == K_ESCAPE) {
        readch();
        break;
      }
      if (ch == K_ENTER || ch == ' ' || ch == 'q' || ch == 'Q') {
        break;
      }
    }
  } while (ch != K_ENTER && ch != K_ESCAPE);

  resetch();
  copy_screen(screen1, screen2);
  dump_screen(screen1);
  return 0;
}

int colorsDialog(void) {
  char ch = 0;
  copy_screen(screen2, screen1);

  int win_w = 38;
  int win_h = 11;
  int x1 = (new_columns / 2) - (win_w / 2);
  int x2 = x1 + win_w;
  int y1 = (new_rows / 2) - (win_h / 2);
  int y2 = y1 + win_h;

  window(screen1, x1, y1, x2, y2, MENU_PANEL, MENU_FOREGROUND0, WINDOW_TITLEB, 1, 1, 1);
  write_str(screen1, (x2 - x1) / 2 + x1 - 8, y1 - 1, "[+] COLOR THEMES", WINDOW_TITLEB, WINDOW_TITLEF, 1);
  dump_screen(screen1);

  if (listBox1 != NULL) removeList(&listBox1);
  listBox1 = addatend(listBox1, newitem("1. MS-DOS Edit (Classic Blue)", -1, -1, -1, -1));
  listBox1 = addatend(listBox1, newitem("2. Modern Dark (Cyber Slate)", -1, -1, -1, -1));
  listBox1 = addatend(listBox1, newitem("3. Borland Turbo C (Vintage)", -1, -1, -1, -1));
  listBox1 = addatend(listBox1, newitem("4. Matrix Terminal (Green)", -1, -1, -1, -1));
  listBox1 = addatend(listBox1, newitem("5. Retro Amber (Phosphor CRT)", -1, -1, -1, -1));
  listBox1 = addatend(listBox1, newitem("6. Ocean Navy (Solarized Blue)", -1, -1, -1, -1));

  setselectorLimit(32);
  ch = listBox(listBox1, x1 + 2, y1 + 1, &scrollData, MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1, 6, VERTICAL, 1, LOCKED);

  if (ch != ESC_KEY && scrollData.itemIndex >= 0 && scrollData.itemIndex <= 5) {
    set_color_theme(scrollData.itemIndex);
    rehighlight_buffer();
    strcpy(tempMessage, "[Color theme updated!]");
    timer3.ticks = 0;
  }

  if (listBox1 != NULL) removeList(&listBox1);
  resetScrollData(&scrollData);
  return 0;
}

