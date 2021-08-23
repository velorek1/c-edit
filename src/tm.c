#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tm.h"
/* Milliseconds timer */
int timerC(NTIMER *mytimer1){
struct timespec ts;
long get_ms;
long res, res2;

 if (mytimer1->ticks == 0){
    //First tick, set up values
    mytimer1->oldtime = 0;
    mytimer1->ticks = 1;
    mytimer1->oldticks = 1;
    mytimer1->elapsed = 0;
    return 1;
  } else{
    //subsequent ticks
    timespec_get(&ts, TIME_UTC);
    //Getms gets 10^-3 milleseconds
    get_ms = (ts.tv_nsec % 10000000) / 1000000;
    //Add a millisecond every time a change is detected in 10^-3
    res = get_ms-mytimer1->oldtime;
    if(labs(res) >= 1){
      mytimer1->elapsed = mytimer1->elapsed + 1;
      mytimer1->oldtime = get_ms;
    }
    //Calculate tick by divided time elapsed with ms requested
    res2 = mytimer1->elapsed / mytimer1->ms;
    if (res2 == mytimer1->oldticks) {
      mytimer1->ticks = mytimer1->ticks + 1;
      mytimer1->oldticks = mytimer1->oldticks+1;
      return 1;
    } else
      return 0;
  }
}


