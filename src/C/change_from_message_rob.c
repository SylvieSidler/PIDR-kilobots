#include <kilombo.h>
#include "send_receive.h"

#define LISTENER 0
#define SPEAKER 1
uint8_t stateLS;
uint32_t last_update;
uint8_t i=0;
message_t transmit_msg;
message_t rvd_message;
int new_message;
int message_sent;
int colour;
int colours[] = {RGB(0,0,3),RGB(3,0,0),RGB(0,3,0),RGB(3,0,3),RGB(0,3,3),RGB(3,3,0)};


int choose_colour(){
    rand_seed(kilo_uid%255);
    uint8_t random = rand_soft()%6;
    return colours[random];
}

message_t *message_tx(){
    if (message_sent ==0){
        if (stateLS == SPEAKER){
            int colour1 = choose_colour();
            transmit_msg.type=NORMAL;
            transmit_msg.data[0]=0;
            transmit_msg.data[1]=colour1;
            transmit_msg.crc = message_crc(&transmit_msg);
            return &transmit_msg;
        }
        return 0;
    }
    return 0;
}

void message_tx_success(){
    message_sent=1;
}

void message_rx(message_t *msg, distance_measurement_t *dist){
    if (new_message ==0) {
        rvd_message = *msg;
        if (stateLS==LISTENER && rvd_message.data[0]==0){
            colour = rvd_message.data[1];
            new_message = 1;
        }
}
}

void setup() {
    int16_t id = kilo_uid;
    if (id%2==0){
        stateLS = LISTENER;
        set_color(RGB(3,0,0));
    }
    else{ 
        stateLS = SPEAKER;
        set_color(RGB(0,0,3));
    }
}



void loop() {
    switch(stateLS){
        case SPEAKER:
            set_color(RGB(0,0,3));
            if (kilo_ticks> last_update+320){
                rand_seed(kilo_uid%255);
                uint8_t random = rand_soft();
                if (random<40){
                    stateLS = LISTENER;
                    set_color(RGB(3,0,0));
                }
            }
            last_update = kilo_ticks;
            break;
        case LISTENER:
        set_color(RGB(0,0,3));
        f (kilo_ticks> last_update+320){
            rand_seed(kilo_uid%255);
            uint8_t random = rand_soft();
            if (random<40){
                stateLS = SPEAKER;
                set_color(RGB(0,0,3));
            }
        }
        last_update = kilo_ticks;
        break;
            
    }
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
