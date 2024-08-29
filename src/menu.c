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
  write_str(screen1,0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  write_str(screen1,0, 1, "File", MENU_SELECTOR, MENU_FOREGROUND1,1);
  loadmenus(FILE_MENU);
  draw_window(screen1,0, 2, 13, 9, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 3, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
    // copy_screen(screen1,screen2);
    // dump_screen(screen1);
      
  //return if right and left arrow keys are pressed
  if (ch == K_RIGHTMENU || ch == K_LEFTMENU) return ch;

  if(scrollData.itemIndex == OPTION_1) {
    flush_editarea(0);
      buffertoScreen(0);
      countCh=inputWindow("File:", tempfileName,  "[+] New file",27,2,46);
      if (countCh>0) {
	 strcpy(fileName, tempfileName);
	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0);
        dump_screen(screen1);
     }//buffertoScreen(0, 0, 0);
      ch=0;
     return DONT_UPDATE;
  }
  
  if(scrollData.itemIndex == OPTION_2) {
     //openFile Dialog in opfile.c
    flush_editarea(0);
    buffertoScreen(0);
     if (openFileDialog(fileName,fullPath) == 1){
 	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0);
     }

        dump_screen(screen1);  
     return DONT_UPDATE;
  }
  if(scrollData.itemIndex == OPTION_3) {
    //External Module - Open file dialog.
    //openFileHandler();
      
      flush_editarea(0);
      buffertoScreen(0);
      countCh=inputWindow("File:", tempfileName,  "Quick load...",27,2,46);
      if (countCh>0) {
	 strcpy(fileName, tempfileName);
	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0);
        dump_screen(screen1);
     }//buffertoScreen(0, 0, 0);
     return DONT_UPDATE;

  }
  if(scrollData.itemIndex == OPTION_4) {
       //Save file
       flush_editarea(0);
      buffertoScreen(0);
    	
	if (strcmp(fileName, "UNTITLED") == 0) {
        countCh=inputWindow("File:", tempfileName,  "Save file as...",27,2,46);
        if (countCh>0) {
	  strcpy(fileName, tempfileName);
	   buffertoFile(fileName);
	   flush_editarea(0);
	   buffertoScreen(0);
          dump_screen(screen1);
	}	
       } else{
	   buffertoFile(fileName);
	   flush_editarea(0);
	   buffertoScreen(0);
          dump_screen(screen1);
       }


     return DONT_UPDATE;
  }
  if(scrollData.itemIndex == OPTION_5) {
    //Save as option
       flush_editarea(0);
      buffertoScreen(0);
  
    countCh=inputWindow("File:", fileName,  "Save file as...",27,2,46);
    if (countCh>0) {
      buffertoFile(fileName);
      flush_editarea(0);
      buffertoScreen(0);
      dump_screen(screen1);
    }
    return DONT_UPDATE;
  }

  if(scrollData.itemIndex == OPTION_6) {
    //Exit option
    //if(fileModified == 1)
      //exitp = confirmation();	//Shall we exit? Global variable!
    //else
      programStatus  = ENDSIGNAL;
  }
 //restore previous screen
 //dump_screen(screen1);
	
return ch;
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

char optionsmenu() {
  //int  setColor;
  char ch=0;

  write_str(screen1,6, 1, "Options", MENU_SELECTOR, MENU_FOREGROUND1,1);
  write_str(screen1, 0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  loadmenus(OPT_MENU);
  draw_window(screen1,6, 2, 19, 6, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 9, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
  if(scrollData.itemIndex == OPTION_1) {
    //File Info
    //fileInfoDialog();
  }
  if(scrollData.itemIndex == OPTION_3) {
    //Set Colors
  /*  setColor = colorsWindow(mylist,COLORSWTITLE);
    setColorScheme(setColor);
    checkConfigFile(setColor);	//save new configuration in config file
    *///refresh_screen(1);
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
  if(scrollData.itemIndex == OPTION_1) {
    //About info
    //help_info();
  }
  if(scrollData.itemIndex == OPTION_2) {
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


