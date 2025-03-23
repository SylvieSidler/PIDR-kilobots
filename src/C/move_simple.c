#include <kilombo.h>
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
    //p += n;
    //n = sprintf (p, "Ns: %d, dist: %d\n ", mydata->N_Neighbors, find_nearest_N_dist());
    p += n;

    return botinfo_buffer;
}

#endif ///////////////////


////////////////////////////////////////////////////////////////////////
// CODE 

void setup() {     
    // spinup motors
    spinup_motors();
    // move straight for 2 seconds (2000ms)
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);
    
    // spinup for 15ms to overcome friction
    spinup_motors();
    // turn left for 2 seconds (2000ms)
    set_motors(kilo_turn_left, 0);
    delay(2000);

    // spinup for 15ms to overcome friction
    spinup_motors();
    // turn right for 2 seconds (200 ms)
    set_motors(0, kilo_turn_right);
    delay(2000);
    set_motors(0,0);
}

void loop() {

}

int main() {
    kilo_init();

    SET_CALLBACK(botinfo, botinfo);
    SET_CALLBACK(reset, setup);

    kilo_start(setup, loop);

    return 0;
}