#include <kilolib.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////
// SETUP

// declare motion variable type
typedef enum {
    STOP,
    FORWARD,
    LEFT,
    RIGHT
} motion_t;

// declare variables
typedef struct {
    uint32_t last_update;
    int N_Neighbors; 
} USERDATA;

extern USERDATA *mydata;

extern char* (*callback_botinfo) (void);
char *botinfo(void);

#ifdef SIMULATOR /////////

#include <stdio.h>    // for printf
int UserdataSize = sizeof(USERDATA);
USERDATA *mydata;

static char botinfo_buffer[10000];
char *botinfo(void) {
    int n;
    char *p = botinfo_buffer;
    n = sprintf (p, "ID: %d ", kilo_uid);
    p += n;
    n = sprintf (p, "Ns: %d, dist: %d\n ", mydata->N_Neighbors, find_nearest_N_dist());
    p += n;

    return botinfo_buffer;
}

#endif ///////////////////


////////////////////////////////////////////////////////////////////////
// CODE 

void setup() {
    // put your setup code here, to be run only once
    // Initialize the LED
    set_color(RGB(3,3,3)); // white 
    mydata->last_update = kilo_ticks;
    mydata->N_Neighbors = 0;
}

void loop() {    
    if (kilo_ticks > mydata->last_update + 31) {
        set_color(RGB(0,0,0)); // off
    }
    if (kilo_ticks > mydata->last_update + 62) {
        set_color(RGB(1,0,0)); // red
    }
    if (kilo_ticks > mydata->last_update + 93) {
        set_color(RGB(0,1,0)); // green
    }
    if (kilo_ticks > mydata->last_update + 124) {
        set_color(RGB(0,0,1)); // blue
    }
    if (kilo_ticks > mydata->last_update + 155) {
        set_color(RGB(1,1,0)); // yellow
        mydata->last_update = kilo_ticks;
        mydata->N_Neighbors = 0;
    }
}

int main() {
    // initialize hardware
    kilo_init();

    SET_CALLBACK(botinfo, botinfo);
    SET_CALLBACK(reset, setup);

    // register your program
    kilo_start(setup, loop);

    return 0;
}
