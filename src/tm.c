#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tm.h"

/* Milliseconds timer */
int timerC(NTIMER *mytimer1){
clock_t difference = 0;
long res;

 if (mytimer1->ticks == 0){
    //First tick, set up values
    mytimer1->oldtime = clock();
    mytimer1->ticks = 1;
    return 1;
  } else{
    //subsequent ticks
    difference =  clock() - mytimer1->oldtime;
    res = difference * 1000 / CLOCKS_PER_SEC;
    if (res < mytimer1->ms) {
      return 0;
    } else{
      mytimer1->oldtime = clock();
      mytimer1->ticks = mytimer1->ticks + 1;
      return 1;
    }
  }
}

