#include <kilombo.h>
#include "send_receive.h"
#ifdef SIMULATOR
#include <stdio.h> // for printf
#else
#include <avr/io.h>  // for microcontroller register defs
//  #define DEBUG          // for printf to serial port
//  #include "debug.h"
#endif

typedef struct 
{
    uint8_t state;
    uint32_t last_update;
    enum state stateLH;
    message_t transmit_msg;
    message_t rvd_message;
    int new_message;
    int message_sent;

} USERDATA;

REGISTER_USERDATA(USERDATA)


message_t *message_tx(){
  
    return 0;
}

void message_tx_success(){
    
}

void message_rx(message_t *msg, distance_measurement_t *dist){
    
}

void setup() {
    set_color(RGB(3,0,0));
    int16_t id = kilo_uid;
    if (id%2==0){
        set_color(RGB(0,3,0));
    }
    else{ 
        set_color(RGB(0,0,3));
    }

    if (id == 1171)
        set_color(RGB(3,0,0));
}

void loop() {
 
    
}

int main()
{
    // Initialize the hardware.
    kilo_init();
    kilo_message_tx=message_tx;
    kilo_message_tx_success = message_tx_success;
    kilo_message_rx=message_rx;
    // Register the program.
    kilo_start(setup, loop);
    
    return 0;
}
