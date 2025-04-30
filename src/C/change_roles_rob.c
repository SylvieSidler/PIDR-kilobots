#include <kilombo.h>
#include "send_receive.h"

#define LISTENER 0
#define SPEAKER 1
uint8_t stateLS;
uint32_t last_update;


message_t *message_tx(){
    return 0;
}

void message_tx_success(){
}

void message_rx(message_t *msg, distance_measurement_t *dist){
   
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
