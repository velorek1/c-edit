/*
========================================================================
- HEADER -
Module with main editor in C·edit
@author : Velorek
@version : 1.0
Last modified: 06/04/2024
========================================================================
*/

#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "scbuf.h"
#include "global.h"
#include "edbuf.h"
#include <stdio.h>
int process_input(char ch);
int special_keys();
int control_keys(char ch);
void linetoScreen(long whereY, VLINES tempLine);
void linetoScreenRAW(long whereY, VLINES tempLine);
void cleanScreenLine(long whereY);
void cleanSection(long whereY, long start, int amount);
void buffertoScreen(BOOL raw);
int editor_section(char ch);
wchar_t convertChar(char c1, char c2);
//int filetoBuffer(FILE * filePtr);
void buffertoFile(char *fileName);
int filetoBuffer(char *fileName);
void flush_editarea(int force_update);
#endif


