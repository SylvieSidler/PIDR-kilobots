#include <kilombo.h>

uint8_t new_message = 0;
#define num_bots 10
#define com_radius 5

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
    uint8_t communicated_with[num_bots];
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

void message_rx(message_t *m, distance_measurement_t *d) {
    uint16_t distance = (d->high_gain << 8) | d->low_gain;
    if (distance < com_radius) {
        uint8_t neighbor_id = m->data[0];  // Get the bot ID from the received message
        if (mydata->communicated_with[neighbor_id] == 0) {  // If we haven't communicated with this bot yet
            mydata->communicated_with[neighbor_id] = 1;  // Mark as communicated with
            new_message = 1;  // Flag that a new message has been received
            printf("Message received from bot %d at distance %d\n", neighbor_id, distance);  // Debugging line
        }
    }
}

void setup() {
    mydata->transmit_msg.type = NORMAL;
    mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);
    mydata->message_sent = 0;
    mydata->last_update = kilo_ticks; // init timer
    mydata->N_Neighbors = 0;
    for (int i = 0; i < num_bots; i++) {
        mydata->communicated_with[i] = 0;
    }
}

void loop() { ////////////////// LOOP
    if (mydata->message_sent) {
        mydata->message_sent = 0;
        set_color(RGB(1,1,0));
        mydata->last_update = 0;
    }
    if (new_message) {
        new_message = 0;
        set_color(RGB(1,0,0));
        mydata->last_update = 0;
    }
    
    if (kilo_ticks > mydata->last_update + 32) { // attend 1 sec avant d'éteindre la led
        set_color(RGB(0,0,0));
    }
    for (int i = 0; i <num_bots; i++) {
        if (mydata->communicated_with[i] == 0) {  // Check if this neighbor has already communicated
            // Send message only if the bot is within communication radius
            mydata->transmit_msg.data[0] = kilo_uid;  // Include our bot ID in the message
            mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);  // Update the CRC
            kilo_message_tx();  // Send the message
            mydata->message_sent = 1;  // Mark message as sent
            mydata->last_update = kilo_ticks;  // Reset timer
            printf("Message sent to bot %d\n", i);
        }
    }
}


int main() {
    kilo_init();
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_success;
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);
    return 0;
}
