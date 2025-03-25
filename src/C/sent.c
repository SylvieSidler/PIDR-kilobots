#include <kilombo.h>


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
    message_t transmit_msg;
    uint8_t message_sent;
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

    return botinfo_buffer;
}

#endif ///////////////////


////////////////////////////////////////////////////////////////////////
// CODE 



message_t *message_tx() {
    return &mydata->transmit_msg;
} 
void message_tx_success() {
    mydata->message_sent = 1;
}

void setup() {
    mydata->transmit_msg.type = NORMAL;
    mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);
    mydata->message_sent = 0;
    mydata->last_update = kilo_ticks; // init timer
}

void loop() { ////////////////// LOOP
    if (mydata->message_sent) {
        mydata->message_sent = 0;
        set_color(RGB(1,1,1));
        mydata->last_update = 0;
    }
    if (kilo_ticks > mydata->last_update + 32) { // attend 1 sec avant d'éteindre la led
        set_color(RGB(0,0,0));
    }
}

int main() {
    kilo_init();
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_success;
    kilo_start(setup, loop);

    return 0;
}
