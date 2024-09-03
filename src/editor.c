/* Main editor section C·edit 
 * for a Text User Interface
 * TextBox
 * Window
 * Last modified: 6/04/2024
 * @author:velorek
 */
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include "rterm.h"
#include "scbuf.h"
#include "ui.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
#include "editor.h"
#include "fileb.h"
#include "edbuf.h"
int oldEndLine= 0;

wchar_t convertChar(char c1, char c2) {
    // Given the two-byte representation for a Char get the wchar convrsion
    setlocale(LC_ALL, "");

    // Combine the bytes into a single string
    char temp[3];
    temp[0] = c1;
    temp[1] = c2;
    temp[2] = '\0';

    wchar_t wchar;

    mbstowcs(&wchar, temp, 1);

    return wchar;
}
void linetoScreenRAW(long whereY, VLINES tempLine){
//dump temporary Line to screen buffer - RAW MODE
   int i=0;
   int attrib = EDIT_FORECOLOR;
   int tempEndLine = findEndline(tempLine);
   //if (tempEndLine <0) return;
   for (i=0; i<tempEndLine; i++){
	   attrib = tempLine.linea[i+shiftH].attrib;  
         //don't print beyond display!

	 if (i+START_CURSOR_X < new_columns-1){
	  if(tempLine.linea[i+shiftH].specialChar!= 0) {
	    //Special char ? print the two values to screen buffer.
            gotoxy(i+START_CURSOR_X+1, whereY+1);
            outputcolor(attrib, EDITAREACOL);
            printf("%c%c", tempLine.linea[i+shiftH].specialChar,tempLine.linea[i+shiftH].ch);
	  } else {
	    gotoxy(i+START_CURSOR_X+1, whereY+1);
            outputcolor(attrib, EDITAREACOL);
            printf("%c", tempLine.linea[i+shiftH].ch);
  	  }
       }
   } 
}

void linetoScreen(long whereY, VLINES tempLine){
//dump temporary Line to screen buffer - RAW MODE
   int i=0;
   int attrib = EDIT_FORECOLOR;
   wchar_t temp='0';
   int tempEndLine = findEndline(tempLine);
   for (i=0; i<tempEndLine; i++){
	   attrib = tempLine.linea[i+shiftH].attrib;  
         //don't print beyond display!
	 if (i+START_CURSOR_X < new_columns-1){
	  if(tempLine.linea[i].specialChar != 0) {
	    //Special char ? print the two values to screen buffer.
            //write_ch(screen1,i+START_CURSOR_X, whereY+1,temp,attrib,EDITAREACOL,0);
            temp = convertChar(tempLine.linea[i+shiftH].specialChar, tempLine.linea[i+shiftH].ch);         // Second char
            //if (temp<1 ) {temp = FILL_CHAR; attrib =F_BLUE;}
            if ((temp<1 || temp =='\0' || temp==END_LINE_CHAR)) {temp = FILL_CHAR; attrib =F_BLUE;}
            write_ch(screen1,i+START_CURSOR_X, whereY+1,temp,attrib,EDITAREACOL,0);
	    
            //rintf("%c%c", tempLine.linea[i].specialChar,tempLine.linea[i].ch);
	  } else {
            //write_ch(screen1,i+START_CURSOR_X, whereY+1,tempLine.linea[i].ch,attrib,EDITAREACOL,0);
            temp = tempLine.linea[i+shiftH].ch;
            //if (temp<1 ) temp = FILL_CHAR; 
            if ((temp<1 || temp =='\0' || temp==END_LINE_CHAR)) {temp = FILL_CHAR; attrib =F_BLUE;}
            write_ch(screen1,i+START_CURSOR_X, whereY+1,temp,attrib,EDITAREACOL,0);
            //printf("%c", tempLine.linea[i].ch);
  	  }
       }
   }

}


void cleanScreenLine(long whereY)
{
   int i=0;
   for (i=0; i<new_columns-2; i++){
	 	gotoxy(i+START_CURSOR_X+1, whereY+1);
         	outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          	printf("%c", FILL_CHAR);
	}
    resetAnsi(0);
   
}

void cleanSection(long whereY, long start, int amount)
{
   int i=0;
   for (i=start; i<start+amount; i++){
	   if (i<new_columns-2){
	 	gotoxy(i+START_CURSOR_X+1, whereY+1);
        	outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          	printf("%c", FILL_CHAR);
	   }
	}
    resetAnsi(0);
   
}



void buffertoScreen(BOOL raw){
   long j=0;
   if (raw == 0) flush_editarea(0);
   for (j=0; j<vdisplayArea; j++){
	  _dumpLine(edBuf1, j+currentLine , &tempLine);
 	  if (raw==TRUE){ 
	      cleanScreenLine(j+START_CURSOR_Y);
	      linetoScreenRAW(j+START_CURSOR_Y, tempLine);
	  }else{ 
	      //cleanScreenLine(j+START_CURSOR_Y);
              linetoScreen(j+START_CURSOR_Y, tempLine);
	  }
	}
//restore tempLine to active line
     memset(&tempLine, '\0',sizeof(tempLine));
    _dumpLine(edBuf1, currentLine, &tempLine);
    resetAnsi(0);
}

void buffertoFile(char *fileName){
  long j=0,i=0; 
  wchar_t tempLongChar = 0;
  openFile(&filePointer, fileName, "w");
  for (j=0; j<_length(&edBuf1); j++){
       //tempLine = {0};
      _dumpLine(edBuf1, j, &tempLine);
      for (i=0; i<findEndline(tempLine); i++){

	  if(tempLine.linea[i].specialChar != 0) {
            tempLongChar = convertChar(tempLine.linea[i].specialChar, tempLine.linea[i].ch);         // Special character
            fprintf(filePointer, "%lc", tempLongChar); 
	  }else{		 
            //regular ASCII char
            fprintf(filePointer, "%c", tempLine.linea[i].ch); 
	  }
      }
      fprintf(filePointer,"%c" , END_LINE_CHAR);
  }
  closeFile(filePointer);
};

int editor_section(char ch){
char accentchar[2];
int insertMode=0;
VLINES *aux = NULL;
VLINES splitLine = {0};
int i,j=0;
int attrib=EDIT_FORECOLOR;
char newch=0;
int endLine=0;
       
       //Check whether we are on Readmode
       // if (fileModified != FILE_READMODE) {
       //if ((ch > 31 && ch < 127) || ch < 0) {

       // A KEY IS PRESSED AND ADDED TO EDIT BUFFER
       // FIRST check for negative chars and special characters
        newch=ch;
        accentchar[0] = 0;
        accentchar[1] = newch;
       //Check whether we are on Readmode
       // if (fileModified != FILE_READMODE) {
     //Process normal printable chars first
     if (ch == 27) return 0;

     if ((ch > 31 && ch < 127) || ch < 0) {
	if (ch < 0) {
	    read_accent(&newch, accentchar);
	    newch = accentchar[1];
      }
      //if ((posBufX != endLine) && (shiftH>0)) return 0;
      //check if we are at the limit of our display to print chars
      if (cursorX < new_columns-2) cursorX++;
      if (posBufX < MAX_LINE_SIZE && cursorX == new_columns-2) {shiftH++;}
      //write_num(screen1,20,2,currentLine,B_CYAN,F_WHITE,1);
      //SYNTAX HIGHLIGHTING DEMO
      //Highlight numbers in GREEN
      if ((accentchar[1] >= 48) && (accentchar[1] <=57)) attrib = FH_GREEN; 
      //Highlight special characters in CYAN
      
      if ((accentchar[1] >= 33) && (accentchar[1] <=47)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY);
      if ((accentchar[1] >= 58) && (accentchar[1] <=64)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY);
      if ((accentchar[1] >= 91) && (accentchar[1] <=96)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY); 
      if ((accentchar[1] >= 123) && (accentchar[1] <=126)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY);

      //FIRST TIME -> CREATE LINES IN BUFFER
      if (aux == NULL) {
	//FIRST CHAR - if not we create a new line in buffer

      	tempLine.index = posBufY;
	//Add spaces if cursor is not at (0,0)
 	if (posBufX != 0) {
		for (i=0; i<posBufX; i++) {
			tempLine.linea[i].ch = FILL_CHAR;
        		tempLine.linea[i].specialChar = 0;
			tempLine.linea[i].attrib = EDIT_FORECOLOR;
		}
	}	
	tempLine.linea[posBufX].ch = accentchar[1];
        tempLine.linea[posBufX].specialChar = accentchar[0];
        tempLine.linea[posBufX].attrib = attrib;
	//add end_line_char to line
	posBufX = posBufX + 1;
	tempLine.linea[posBufX].ch = END_LINE_CHAR;
        tempLine.linea[posBufX].specialChar = 0;
        tempLine.linea[posBufX].attrib = attrib;
	edBuf1 = _addatend(edBuf1, _newline(tempLine));
	linetoScreenRAW(cursorY,tempLine);          

     } else
	//LINE ALREADY EXISTS
	{
	//Locate the end of the line
	endLine = findEndline(tempLine);

	//Insert characters in the middle of other characters
	if (endLine > posBufX) {

	   tempLine.index = posBufY; //with every line index is incremented
	   if (insertMode == FALSE){
	     for (i=endLine; i>=posBufX; i--){ 
                 tempLine.linea[i+1].ch = tempLine.linea[i].ch;
                 tempLine.linea[i+1].specialChar = tempLine.linea[i].specialChar;
                 tempLine.linea[i+1].attrib = tempLine.linea[i].attrib;
              }
	    }
             tempLine.linea[posBufX].ch = accentchar[1];
             tempLine.linea[posBufX].specialChar = accentchar[0];
             tempLine.linea[posBufX].attrib = attrib;
              posBufX = posBufX + 1;
             _updateLine(edBuf1, posBufY, &tempLine);
	     linetoScreenRAW(cursorY,tempLine);	
	     	
	}
          	
	else {
	  // POSBUFX >= ENDLINE: Cursor is at the end or further away from latest text
          //ADD SPACES IF CURSOR IS NOT AT THE END OF THE LINE AND LINE ALREADY EXISTS
	  if(posBufX > endLine) {	  
	    for(i = endLine; i < posBufX; i++) {
	     tempLine.linea[i].ch = FILL_CHAR;
             tempLine.linea[i].specialChar = 0;
	     tempLine.linea[i].attrib = EDIT_FORECOLOR;
	   } 
          }
	  tempLine.linea[posBufX].ch = accentchar[1];
          tempLine.linea[posBufX].specialChar = accentchar[0];
          tempLine.linea[posBufX].attrib = attrib;
	   posBufX = posBufX + 1;
	  tempLine.linea[posBufX].ch = END_LINE_CHAR;
          tempLine.linea[posBufX].specialChar = 0;
          tempLine.linea[posBufX].attrib = attrib;
	  _updateLine(edBuf1, posBufY, &tempLine);  
	  linetoScreenRAW(cursorY,tempLine);
	  //the cursor returns to its place when scrolling horizontally
          if (posBufX < MAX_LINE_SIZE && cursorX == new_columns-2) {cursorX--;}
         }
      }
	if (shiftH>0){
		buffertoScreen(1);
	}

    }
    //record previous values
    old_cursorX = cursorX;
    old_cursorY = cursorY;
    oldposBufX = posBufX;
    oldposBufY = posBufY;
    oldEndLine = endLine;
   //HANDLE ENTER KEY 

   if (ch == K_ENTER) {
    // ENTER key
    update_ch(cursorX, cursorY, ' ', EDITAREACOL, EDITAREACOL);

    // Move cursor down if not at the bottom of the screen
    if (cursorY < new_rows - 3) {
        cursorY++;
    }
    cursorX = START_CURSOR_X;

    // If the buffer pointer is at the end, create a new empty line
    if (_length(&edBuf1) <= posBufY) {
        // Create a new line without characters in the current line in the edit buffer
        memset(&tempLine, '\0', sizeof(tempLine));
        tempLine.index = _length(&edBuf1);
        tempLine.linea[0].ch = END_LINE_CHAR;
        tempLine.linea[0].specialChar = 0;
        tempLine.linea[0].attrib = 0;
        edBuf1 = _addatend(edBuf1, _newline(tempLine));
    } else {
        // SPLIT LINE INTO TWO
        _dumpLine(edBuf1, posBufY, &tempLine);
        endLine = findEndline(tempLine);

        // Create a new line to insert the split content
        memset(&splitLine, '\0', sizeof(splitLine));
        memset(&tempLine, '\0', sizeof(tempLine));
        tempLine.index = _length(&edBuf1);
        edBuf1 = _addatend(edBuf1, _newline(tempLine));

        // Move lines below the current line down by one
        for (j = _length(&edBuf1) - 1; j > posBufY; j--) {
            _dumpLine(edBuf1, j, &tempLine);
            _updateLine(edBuf1, j + 1, &tempLine);
            if (j + START_CURSOR_Y < new_rows - 3) {
                cleanSection(j + START_CURSOR_Y, 0, findEndline(tempLine));
            }
        }

        // Split the current line at the cursor position
        _dumpLine(edBuf1, posBufY, &tempLine);
        memset(&splitLine, '\0', sizeof(splitLine));

        // Copy characters before the cursor to the current line
        for (i = 0; i < posBufX; i++) {
            splitLine.linea[i].ch = tempLine.linea[i].ch;
            splitLine.linea[i].specialChar = tempLine.linea[i].specialChar;
            splitLine.linea[i].attrib = tempLine.linea[i].attrib;
        }
        if (isLineTerminated(splitLine) == FALSE) {
            splitLine.linea[i].ch = END_LINE_CHAR;
        }
        _updateLine(edBuf1, posBufY, &splitLine);

        // Move characters after the cursor to the next line
        memset(&splitLine, '\0', sizeof(splitLine));
        j = 0;
        for (i = posBufX; i < endLine; i++) {
            splitLine.linea[j].ch = tempLine.linea[i].ch;
            splitLine.linea[j].specialChar = tempLine.linea[i].specialChar;
            splitLine.linea[j].attrib = tempLine.linea[i].attrib;
            j++;
        }
        if (isLineTerminated(splitLine) == FALSE) {
            splitLine.linea[j].ch = END_LINE_CHAR;
        }

        // Update the new line with the split content
        _updateLine(edBuf1, posBufY + 1, &splitLine);

        // Clean the screen for the current line and refresh the buffer
        cleanSection(cursorY - 1, 0, findEndline(tempLine));
        buffertoScreen(TRUE);
    }

    // Scroll if needed when cursor is at the bottom of the screen
    if (_length(&edBuf1) > vdisplayArea && cursorY == new_rows - 3) {
        buffertoScreen(1);
        currentLine++;
    }

    // Move buffer pointer positions
    posBufY++;
    posBufX = 0;
}

if(ch == K_BACKSPACE) {
    // BACKSPACE key
    update_ch(cursorX, cursorY, ' ', EDITAREACOL, EDITAREACOL);

    if (posBufX == findEndline(tempLine)) {
        // Cursor is at the end of the line
        tempLine.linea[posBufX-1].ch = 0;
        tempLine.linea[posBufX-1].specialChar = 0;
        tempLine.linea[posBufX-1].attrib = 0;
    } else {
        // Shift characters to the left if we are not at the end of the line
        if (posBufX != 0) {
            for(i = posBufX - 1; i < findEndline(tempLine) - 1; i++) {
                tempLine.linea[i].ch = tempLine.linea[i + 1].ch;
                tempLine.linea[i].specialChar = tempLine.linea[i + 1].specialChar;
                tempLine.linea[i].attrib = tempLine.linea[i + 1].attrib;
            }
            // Clear the last character in the line
            tempLine.linea[i].ch = 0;
            tempLine.linea[i].specialChar = 0;
            tempLine.linea[i].attrib = 0;
        }
    }

    // Remove line if we continue pressing backspace at the beginning of a line
    if(cursorX == START_CURSOR_X && cursorY > START_CURSOR_Y) {
        if (posBufY > 0) {
            memset(&tempLine, '\0', sizeof(tempLine));
            memset(&splitLine, '\0', sizeof(splitLine));
            _dumpLine(edBuf1, posBufY - 1, &tempLine);

            if (findEndline(tempLine) < 1) {
                // Previous line is empty, remove it
                for (j = posBufY - 1; j < _length(&edBuf1) - 1; j++) {
                    _dumpLine(edBuf1, j + 1, &tempLine);
                    _hardupdateLINE(&edBuf1, j, tempLine);
                }
                _deleteObject(&edBuf1, _length(&edBuf1) - 1, FALSE);
            } else {
                // Merge two lines
                endLine = findEndline(tempLine);
                _dumpLine(edBuf1, posBufY, &splitLine);
                
                for (i = 0; i < findEndline(splitLine); i++) {
                    tempLine.linea[endLine + i].ch = splitLine.linea[i].ch;
                    tempLine.linea[endLine + i].attrib = splitLine.linea[i].attrib;
                    tempLine.linea[endLine + i].specialChar = splitLine.linea[i].specialChar;
                }
                tempLine.linea[endLine + i].ch = END_LINE_CHAR;

                _hardupdateLINE(&edBuf1, posBufY - 1, tempLine);

                // Update subsequent lines
                for (j = posBufY; j < _length(&edBuf1) - 1; j++) {
                    _dumpLine(edBuf1, j + 1, &tempLine);
                    _hardupdateLINE(&edBuf1, j, tempLine);
                }
                _deleteObject(&edBuf1, _length(&edBuf1) - 1, FALSE);
            }

            posBufY--;
            cursorY--;
            if (cursorY == new_rows - 3 && currentLine > 0) currentLine--;
            buffertoScreen(1);
            _dumpLine(edBuf1, posBufY, &tempLine);

            // Adjust cursor position after merging lines
            if (findEndline(splitLine) != 0 && findEndline(splitLine) != findEndline(tempLine)) {
                posBufX = findEndline(tempLine) + 1;
                cursorX = posBufX + START_CURSOR_X;
            } else {
                if (endLine != findEndline(tempLine)) {
                    cursorX = START_CURSOR_X;
                    posBufX = cursorX - 1;
                } else {
                    posBufX = findEndline(tempLine) + 1;
                    cursorX = posBufX + START_CURSOR_X;
                }
            }
        }
    }

    // Normal backspace within a line
    if(cursorX > START_CURSOR_X) {     
        cursorX--;
        posBufX--;
        _updateLine(edBuf1, posBufY, &tempLine);
    }

    // Clean up and refresh the line on screen
    cleanSection(cursorY, findEndline(tempLine), 2);
    linetoScreenRAW(cursorY, tempLine);
}

if (deleteKeyPressed == 1) {
    deleteKeyPressed = 0;         	    
    // DEL key
    if (posBufX == findEndline(tempLine)) {
        // Cursor is at the end of the line
        if (posBufY < _length(&edBuf1) - 1) {
            // If there is a next line, merge it with the current line
            _dumpLine(edBuf1, posBufY + 1, &splitLine);
            endLine = findEndline(tempLine);
            
            // Copy the characters from the next line to the end of the current line
            for (i = 0; i < findEndline(splitLine); i++) {
                tempLine.linea[endLine + i].ch = splitLine.linea[i].ch;
                tempLine.linea[endLine + i].attrib = splitLine.linea[i].attrib;
                tempLine.linea[endLine + i].specialChar = splitLine.linea[i].specialChar;
            }
            tempLine.linea[endLine + i].ch = END_LINE_CHAR;
            
            // Update the buffer with the modified line
            _hardupdateLINE(&edBuf1, posBufY, tempLine);
            
            // Remove the next line from the buffer
            for (j = posBufY + 1; j < _length(&edBuf1); j++) {
                _dumpLine(edBuf1, j + 1, &tempLine);
                _hardupdateLINE(&edBuf1, j, tempLine);
            }
            _deleteObject(&edBuf1, _length(&edBuf1) - 1, FALSE);
            
            // Update the screen
            buffertoScreen(1);
        }
    } else {
        // Cursor is in the middle of the line, so delete the character at the cursor position
        for (i = posBufX; i < findEndline(tempLine); i++) {
            tempLine.linea[i].ch = tempLine.linea[i + 1].ch;
            tempLine.linea[i].specialChar = tempLine.linea[i + 1].specialChar;
            tempLine.linea[i].attrib = tempLine.linea[i + 1].attrib;
        }
        tempLine.linea[i].ch = 0; // Nullify the last character

        // Update the buffer with the modified line
        _updateLine(edBuf1, posBufY, &tempLine);

        // Update the screen
        cleanSection(cursorY, findEndline(tempLine), 2);
        linetoScreenRAW(cursorY, tempLine);
    }
}

    if(ch == K_TAB) {
      //TAB key sends spaces for convenience
      	      for (i=posBufX; i<posBufX+TAB_SPACES; i++)

		{
	            tempLine.linea[i].ch = FILL_CHAR;
                    tempLine.linea[i].specialChar = 0;
                    tempLine.linea[i].attrib = EDIT_FORECOLOR;
	      }      
	      posBufX=posBufX + TAB_SPACES;
	      if (cursorX < new_columns-8) {
                      for (i=0; i<TAB_SPACES; i++) update_ch(cursorX+i, cursorY, ' ', EDITAREACOL, EDITAREACOL);
		      cursorX = cursorX + TAB_SPACES;
	      }
        _updateLine(edBuf1, posBufY, &tempLine);    
     }

   return 0; 
}

/*------------------------------*/
/* Open file and dump to buffer */
/*------------------------------*/

int filetoBuffer(char *fileName) { //EDBUF*
  long     inlineChar = 0, lineCounter = 0;
  //wchar_t tempLongChar = 0;
  char    ch;
  int attrib = EDIT_FORECOLOR;
  fileModified = FILE_UNMODIFIED;
  
  openFile(&filePointer, fileName, "r");
  memset(&tempLine, '\0',sizeof(tempLine));
  tempLine.index = 0;
  if (edBuf1 != NULL) _deletetheList(&edBuf1);
  edBuf1 = _addatend(edBuf1, _newline(tempLine));

  //Check if pointer is valid
  if(filePointer != NULL) {
    rewind(filePointer);        //Make sure we are at the beginning
    ch = getc(filePointer);     //Peek ahead in the file
    while(!feof(filePointer)) {
      if(ch != '\0') {
    //Temporary restrictions until scroll is implemented.
    //if(lineCounter == rows - 4)
    //  break;

    if(ch == SPECIAL_CHARS_SET1 || ch == SPECIAL_CHARS_SET2) {
      tempLine.linea[inlineChar].specialChar = ch;
      //Read accents
      ch = getc(filePointer);
      tempLine.linea[inlineChar].ch = ch;
    } else {
      if(ch > 0)
       tempLine.linea[inlineChar].ch = ch;
    }
       //SYNTAX HIGHLIGHTING DEMO
      //Highlight numbers in GREEN
      attrib = EDIT_FORECOLOR;
      if ((ch >= 48) && (ch <=57)) attrib = FH_GREEN; 
      //Highlight special characters in CYAN
      
      if ((ch >= 33) && (ch <=47)) attrib = FH_CYAN; 
      if ((ch >= 58) && (ch <=64)) attrib = FH_CYAN; 
      if ((ch >= 91) && (ch <=96)) attrib = FH_CYAN; 
      if ((ch >= 123) && (ch <=126)) attrib = FH_CYAN; 

   
     tempLine.linea[inlineChar].attrib = attrib;
        //TABs are converted into spaces
      /*  if(ch == K_TAB) {
        for (tabcount=0;tabcount<TAB_DISTANCE;tabcount++){
          ch = FILL_CHAR;
          writetoBuffer(editBuffer, inlineChar, lineCounter, ch);
          inlineChar++;
        }*/

    inlineChar++; //NEXT CHARACTER

    if(ch == END_LINE_CHAR) {
      inlineChar = 0;
      ch = 0;
      _updateLine(edBuf1, lineCounter, &tempLine);
      lineCounter++;
      memset(&tempLine, '\0',sizeof(tempLine));
      tempLine.index = _length(&edBuf1);
      edBuf1 = _addatend(edBuf1, _newline(tempLine));
    }
       if (lineCounter == MAX_LINES) {
           //open as readMode
           fileModified = FILE_READMODE;
           break;
       }
       //If file is bigger than buffer
      //break loop at last allowed line.
      }
      ch = getc(filePointer);
    }
  }
  closeFile(filePointer);
  return 1;
}

void flush_editarea(int force_update) {
int i;
  //Paint blue edit area
  //draw_screen();
  screen_color(screen1, EDITAREACOL, EDITAREACOL, FILL_CHAR);
  //Draw upper and lower bars
    for(i = 0; i < old_columns; i++) {
     write_ch(screen1, i, 1, FILL_CHAR, MENU_PANEL, MENU_PANEL,0);
    }

  for(i = 0; i < old_columns; i++) {
    write_ch(screen1, i, old_rows, FILL_CHAR, STATUSBAR, STATUSMSG,1);
  }
  // Text messages
  write_str(screen1, 0, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0,0);
  write_str(screen1, 0, 1, "F", MENU_PANEL, F_RED,0);
  write_str(screen1, 7, 1, "p", MENU_PANEL, F_RED,0);
  write_str(screen1, 15, 1, "H", MENU_PANEL, F_RED,0);
  write_str(screen1, 0, old_rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG,0);

  /* Frames */
  //window appearance and scroll bar
  for(i = 2; i < old_rows; i++) {
    write_ch(screen1,old_columns-1, i, ' ', SCROLLBAR_BACK, SCROLLBAR_FORE,0);	//Scroll bar
    write_ch(screen1,0, i, VER_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//upper vertical line box-like char 
  }
  for(i = 0; i < old_columns; i++) {
    write_ch(screen1,i, 2, HOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//horizontal line box-like char
    write_ch(screen1,i, old_rows - 1, ' ', EDITWINDOW_BACK, EDITWINDOW_FORE,0);
  }
  write_ch(screen1,0, 2, UPPER_LEFT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//upper-left box-like char
  //horizontal scroll bar
  for(i = 0; i < old_columns; i++) {
    write_ch(screen1,i, old_rows - 1, FILL_CHAR, SCROLLBAR_BACK, SCROLLBAR_FORE,0);
  }
  //Window-appearance
  write_ch(screen1,old_columns-1, 2, UPPER_RIGHT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//right window corner
  write_ch(screen1,old_columns-1, old_rows - 1, LOWER_RIGHT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE,0);
  write_ch(screen1, 0, old_rows - 1, LOWER_LEFT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE,0);

  //Scroll symbols
  write_ch(screen1,old_columns-1, 3, '^', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns-1, old_rows - 2, 'v', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns-1, 4, '*', SCROLLBAR_SEL, SCROLLBAR_FORE,0);
  write_ch(screen1, 2, old_rows - 1, '*', SCROLLBAR_SEL, SCROLLBAR_FORE,0);
  write_ch(screen1, 1, old_rows - 1, '<', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns - 2, old_rows - 1, '>', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  if (strlen(fileName) == 0) strcpy(fileName,"UNTITLED");
  write_str(screen1,(new_columns / 2) - (strlen(fileName) / 2), 2, fileName,
        MENU_PANEL, MENU_FOREGROUND0,0);
 if (force_update) dump_screen(screen1);
}
