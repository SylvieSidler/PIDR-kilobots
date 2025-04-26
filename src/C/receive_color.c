#include <kilombo.h>
#include <math.h>

uint8_t new_message = 0;

/////////////// FONCTIONNE PAS 

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
    uint16_t gradient_value = UINT16_MAX;
    uint16_t recvd_gradient = 0;
    uint8_t new_message = 0;
    message_t msg;
} USERDATA;

extern USERDATA *mydata;

// rainbow colors
uint8_t colors[] = {
    RGB(0,0,0),  //0 - off
    RGB(2,0,0),  //1 - red
    RGB(2,1,0),  //2 - orange
    RGB(2,2,0),  //3 - yellow
    RGB(1,2,0),  //4 - yellowish green
    RGB(0,2,0),  //5 - green
    RGB(0,1,1),  //6 - cyan
    RGB(0,0,2),  //7 - blue
    RGB(1,0,1),  //8 - purple
    RGB(3,3,3)   //9  - bright white
}; 

extern char* (*callback_botinfo) (void);
char *botinfo(void);

#ifdef SIMULATOR /////////

#include <stdio.h>    // for printf
int UserdataSize = sizeof(USERDATA);
USERDATA *mydata;

static char botinfo_buffer[10000];
char *botinfo(void) {
    char *p = botinfo_buffer;
    p += sprintf (p, "ID: %d \n", kilo_uid);
    p += sprintf (p, "Gradient Value: %d\n", mydata->gradient_value);

    return botinfo_buffer;
}

#endif ///////////////////


////////////////////////////////////////////////////////////////////////
// CODE 

void setup() { 
    mydata->N_Neighbors = 0;
    mydata->gradient_value = UINT16_MAX;
    mydata->recvd_gradient = 0;
    mydata->new_message = 0;
    mydata->msg.type = NORMAL;
    mydata->last_update = kilo_ticks;
}

void loop() {
    if (new_message) {
        new_message = 0;
        set_color(RGB(1,1,1));
        delay(100);
        set_color(RGB(0,0,0));
    }
}

void message_rx(message_t *m, distance_measurement_t *d) {
    new_message = 1;
    
}

int main() {
    kilo_init();

    SET_CALLBACK(botinfo, botinfo);
    SET_CALLBACK(reset, setup);

    kilo_message_rx = message_rx;
    kilo_start(setup, loop);

    return 0;
}
