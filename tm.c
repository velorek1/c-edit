/* Module to create a milisecond timer
 * @author: velorek
 * Last modified: 10/08/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tm.h"

#include <sys/time.h>
#include <poll.h>

long long epoch_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + (tv.tv_usec + 500) / 1000;
}
int timerC(NTIMER *mytimer1){
    long long difference = 0;
    long long res;
    if (mytimer1->ticks == -1) return 0;
    if (mytimer1->ticks == 0){
        // First tick, set up values
        mytimer1->oldtime = epoch_ms();
        mytimer1->ticks = 1;
        return 1;
    } else {
        // Subsequent ticks
        long long now = epoch_ms();
        difference = now - mytimer1->oldtime;
        res = difference;
        if (res < mytimer1->ms) {
            return 0;
        } else {
            mytimer1->oldtime = now;
            mytimer1->ticks = mytimer1->ticks + 1;
            return 1;
        }
    }
}

void init_timer(NTIMER *mytimer1, int ms){
    // Init routine
    mytimer1->ticks = 0;
    mytimer1->ms = ms;
}

void stop_timer(NTIMER *mytimer1){
    // Stop routine
    mytimer1->ticks = -1;
}

void resume_timer(NTIMER *mytimer1){
    // Resume routine
    mytimer1->ticks = 0;
}

