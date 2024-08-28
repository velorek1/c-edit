#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "edbuf.h"


// create new list element of type VLINES from the supplied text string
VLINES *_newline(VLINES temp)
{
	VLINES *newp;
	newp = (VLINES *) malloc (sizeof(VLINES));
	newp->index = temp.index;
	memcpy(newp->linea,temp.linea,sizeof(temp.linea)+1);
        newp->next = NULL;
	return newp;
}

// Delete first element on list whose item field matches the given text
// NOTE!! delete requests for elements not in the list are silently ignored :-)
void _RemoveThing(VLINES **head, int index)
{
	BOOL present = FALSE;
	VLINES *old;
	VLINES **tracer = head;
	if ((*tracer)->index==index) present=TRUE;
	while((*tracer) && !(present)){
		if ((*tracer)->index==index) present=TRUE;
		tracer = &(*tracer)->next;
	}

	if(present)
	{
		old = *tracer;
		*tracer = (*tracer)->next;
		free(old); // free up remainder of list element 
	}
}
void _hardupdateLINE(VLINES **head, int index, VLINES temp)
{
	int i=0;
	BOOL present = FALSE;
	VLINES **tracer = head;
	if ((*tracer)->index==index) present=TRUE;
	while((*tracer) && !(present)){
		if ((*tracer)->index==index) {present=TRUE; break;}
		tracer = &(*tracer)->next;
	}

	if(present)
	{
                for (i=0; i<MAX_LINE_SIZE; i++)
                {
                  (*tracer)->linea[i].ch = temp.linea[i].ch;
                  (*tracer)->linea[i].attrib = temp.linea[i].attrib;
                  (*tracer)->linea[i].specialChar = temp.linea[i].specialChar;
                }

	}
}
void _deletetheList(VLINES **head) 
{ 
   /* deref head_ref to get the real head */
   VLINES *current = *head; 
   VLINES *next = NULL;
   VLINES **tracer=head; 
   while (current != NULL)  
   { 
       next = current->next; 
       free(current);
       current = next; 
   } 
    
   /* deref head_ref to affect the real head back 
      in the caller. */

   *tracer = NULL;
} 


// updatelement: remove from list the first instance of an element 
VLINES *_update(VLINES *head, int index, VLINES temp)
{
	VLINES *p;
	for (p = head; p != NULL; p = p -> next) {
            if (p -> index == index) {
		break;
	   }
	}
	 p-> index = temp.index;	
	 return head;
	
}

// getObject 
VLINES *_getObject(VLINES *head, int index)
{
	VLINES *p;
	for (p = head; p != NULL; p = p -> next) {
            if (p -> index == index) {
		break;
	   }
	}
	 //p-> index = temp.index;	
	 return p;
	
}
// deleteline: remove from list the first instance of an element 
// containing a given text string
// NOTE!! delete requests for elements not in the list are silently ignored 
VLINES *_deleteline(VLINES *head, int index)
{
	VLINES *p, *prev;
	prev = NULL;
	for (p = head; p != NULL; p = p -> next) {
            if (p -> index == index) {
		if(prev == NULL)
		   head = p-> next;
		else
		   prev -> next = p -> next;
		free(p);	// remove rest of VLINES
		return head;
	   }
	   prev = p;	
	}
  return NULL;
}
/* addfront: add new VLINES to front of list  */
/* example usage: start = (addfront(start, newelement("burgers")); */

VLINES *_addfront(VLINES *head, VLINES *newp)
{
	newp -> next = head;
	return newp;
}

/* addend: add new VLINES to the end of a list  */
/* usage example: start = (addend(start, newelement("wine")); */

VLINES *_addatend (VLINES *head, VLINES *newp)
{
	VLINES *p2; 	
	if (head == NULL)
		return newp;
// now find the end of list
	for (p2 = head; p2 -> next != NULL; p2 = p2 -> next)
		;
	p2 -> next = newp;
	return head;
}

void _printlist(VLINES **head)
// this routine uses pointer-to-pointer techniques :-)
{
	VLINES **tracer = head;
	int index=0, i = 0;
	  while ((*tracer) != NULL) {
		index = (*tracer)->index;
		printf("LINE %d | ", index);
		for (i=0; i < MAX_LINE_SIZE; i++)
		      printf("%c",(*tracer)->linea[i].ch);
		printf("\n");
		tracer = &(*tracer)->next;
	  }  
}

int _length(VLINES **head)
// this routine uses pointer-to-pointer techniques :-)
{

	int count=0;
	VLINES **tracer = head;
	if (*head == NULL) return -1;
	while ((*tracer) != NULL) {
		count = count +1;	
		tracer = &(*tracer)->next;
	}
       return count;
}
void _reindex(VLINES **head)
{
	int count=0;	
	VLINES *p=NULL;
	VLINES **tracer = head;
	while ((*tracer) != NULL) {
		p = *tracer;
		p->index=count;
		count = count +1;
		tracer = &(*tracer)->next;
	}           
}

void _deleteObject(VLINES **head,int index, BOOL sort){
   VLINES *p=*head;
  if (index == 0 || _length(head) <=1 || p->index == index )
    _RemoveThing(head,index);
  else 
    _deleteline(*head,index);
  if (sort == TRUE) _reindex(head); 
}

int _dumpLine(VLINES *head, long index, VLINES *line){
//Dumps contents of desired line from buffer into temporary line
   int i=-1; char ch=0; char attrib = 0; char specialChar = 0;
   VLINES *aux = NULL; //auxiliary pointer
   aux = _getObject(head, index);
   memset(line, '\0',sizeof(&line)); //Clear memory for temporary line
   //Does the line exist?
   if (aux != NULL) { 
     for (i=0; i<MAX_LINE_SIZE; i++)
     {
       ch = aux->linea[i].ch;
       attrib = aux->linea[i].attrib;
       specialChar = aux->linea[i].specialChar;
       line->linea[i].ch = ch;
       line->linea[i].attrib = attrib;
       line->linea[i].specialChar = specialChar;
     }
     i = aux->index;
  }
  return i;
}
int _updateLine(VLINES *head, long index, VLINES *line){
//Copies contents of desired line from temporary line to buffer
   int i=0; char ch=0; char specialChar = 0; 
   char attrib = 0;
   VLINES *aux = NULL; //auxiliary pointer
   aux = _getObject(head, index);
   //Does the line exist?
   if (aux != NULL) { 
     for (i=0; i<MAX_LINE_SIZE-1; i++)
     {
       ch = line->linea[i].ch;
       specialChar = line->linea[i].specialChar;
       attrib = line->linea[i].attrib;
       aux->linea[i].ch = ch;
       aux->linea[i].specialChar = specialChar;
       aux->linea[i].attrib = attrib;
      }
     i = aux->index;
  } 
  return i;
}

CHARBUF _getSingleChar(VLINES *head, long X, long Y ){
//Dumps contents of desired line from buffer into temporary line
   char ch=0; char attrib = 0; char specialChar = 0;
   CHARBUF retvalues;
   VLINES *aux = NULL; //auxiliary pointer
   aux = _getObject(head, Y);
   //Does the line exist?
   if (aux != NULL) { 
       ch = aux->linea[X].ch;
       specialChar = aux->linea[X].specialChar;
       attrib = aux->linea[X].attrib;

     } else
  {
       ch = '\0';
       specialChar = '\0';
       attrib = '\0';
  }
  retvalues.ch = ch;
  retvalues.attrib = attrib;
  retvalues.specialChar = specialChar;
  return retvalues;
}

int findEndline(VLINES line) {
  char    ch = 0;
  int     i=0;
  int     result = 0;

  do {
    ch = line.linea[i].ch;
    //write_ch(i,1,ch,F_RED,B_WHITE);
    if(ch == 0x00 || ch == 0x10 || ch == 0x0A) //ch = 00
      break;
    i++;
  } while(i < MAX_LINE_SIZE);
  result = i;
  ch = 0;
  return result;
}

BOOL isLineTerminated(VLINES line) {
  BOOL flag=FALSE;
  char ch=0;
  int i = 0;
  for (i=0; i< MAX_LINE_SIZE; i++){
    ch = line.linea[i].ch;
    if(ch == 0x10 || ch == 0x0A)
      flag = TRUE;
  }
  return flag;
}


