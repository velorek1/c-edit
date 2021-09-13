/*
========================================================================
- HEADER -
Module to implement a millisecond TIMER in C.
@author : Velorek
@version : 1.0
Last modified: 27/12/2020 + New module
========================================================================
*/

#ifndef _TM_H_
#define _TM_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/
typedef struct nTimer{
        int  ms; //milliseconds
    long oldtime; //to calculate increment in time
    long ticks; //how many ticks have been made
} NTIMER;

/* Miliseconds timer */
int timerC(NTIMER *mytimer1);

#endif
