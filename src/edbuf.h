#ifndef _EDBUF_H_
#define _EDBUF_H_

#include <stdio.h>
#include <stdlib.h>
#define MAX_LINE_SIZE 512
#define TRUE 1
#define FALSE 0
typedef int BOOL;

typedef struct _charbuf
{
    char ch;
    char specialChar;
    char attrib;    
}CHARBUF;

typedef struct _vlines 
{ 
	int  index;
   	CHARBUF linea[MAX_LINE_SIZE];
	struct _vlines *next;
} VLINES;

/* Adapted from Kernighan and Pike's "The Practice of Programming"  pp.46 et 
seq. (Addison-Wesley 1999) */

// create new list element of type VLINES from the supplied text string
VLINES *_newline(VLINES temp);
VLINES *_addfront(VLINES *head, VLINES *newp);
VLINES *_addatend (VLINES *head, VLINES *newp);
VLINES *_addmiddle (VLINES *head, VLINES *newp);
VLINES *_update(VLINES *head, int index, VLINES temp);
VLINES *_getObject(VLINES *head, int index);
void _hardupdateLINE(VLINES **head, int index, VLINES temp);
void _RemoveThing(VLINES **head, int index);
void _deletetheList(VLINES **head);
VLINES *_deleteline(VLINES *head, int index);
void _deleteObject(VLINES **head,int index, BOOL sort);
int _length(VLINES **head);
void _printlist(VLINES **head);
void _reindex(VLINES **head);
int _dumpLine(VLINES *head, long index, VLINES *line);
int _updateLine(VLINES *head, long index, VLINES *line);
CHARBUF _getSingleChar(VLINES *head, long X, long Y );
int findEndline(VLINES line);
BOOL isLineTerminated(VLINES line);
#endif
