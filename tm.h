/*
========================================================================
- HEADER -
Module to implement a millisecond TIMER in C.
@author : Velorek
@version : 1.0
Last modified: 15/09/2021 + New technique + init timer
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
void init_timer(NTIMER *mytimer, int ms);
void stop_timer(NTIMER *mytimer);
void resume_timer(NTIMER *mytimer);

#endif
