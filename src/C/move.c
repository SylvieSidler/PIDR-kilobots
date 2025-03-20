#include <kilolib.h>

////////////////////////////////////////////////////////////////////////
// SETUP

// declare constants
#define FORWARD_GREEN 0
#define LEFT_RED 1
#define RIGHT_BLUE  2

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
    uint8_t state;
    uint8_t state_needs_update ;
    uint8_t color_needs_update;
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
    mydata->state = FORWARD_GREEN;
    mydata->state_needs_update = 1;
    mydata->color_needs_update = 0;
    spinup_motors();
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);
    
    spinup_motors();
    set_motors(kilo_turn_left, 0);
    delay(2000);

    spinup_motors();
    set_motors(0, kilo_turn_right);
    delay(2000);
    set_motors(0,0);
    
    mydata->last_update = kilo_ticks; // init timer
}

void loop() {
    // Change state every 64 clock ticks (roughly 2 seconds)
    if (kilo_ticks > mydata->last_update + 64) {
        mydata->last_update = kilo_ticks;
        mydata->state = (mydata->state + 1) % 3;
        mydata->state_needs_update = 1;
        mydata->color_needs_update = 0;
    }

    if (kilo_ticks > mydata->last_update + 32 && !mydata->color_needs_update) {
        set_color(RGB(1,1,1));
        mydata->color_needs_update = 1;
    }

    if (mydata->state_needs_update) {
        switch(mydata->state) {
            case FORWARD_GREEN:
                set_color(RGB(0,1,0));
                spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case LEFT_RED:
                set_color(RGB(1,0,0)); 
                spinup_motors();
                set_motors(kilo_turn_left,0);
                break;
            case RIGHT_BLUE:
                set_color(RGB(0,0,1));
                spinup_motors();
                set_motors(0, kilo_turn_right);
                break;
        }
        mydata->state_needs_update = 0;
    }
}

int main() {
    kilo_init();
    kilo_start(setup, loop);
    return 0;
}