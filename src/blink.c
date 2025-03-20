#include <kilolib.h>

// declare motion variable type
typedef enum {
    STOP,
    FORWARD,
    LEFT,
    RIGHT
} motion_t;

// declare variables

typedef struct 
{
  uint16_t gradient_value;
  uint16_t recvd_gradient;
  uint8_t new_message;
  message_t msg;
 
} USERDATA;

extern USERDATA *mydata;

#ifdef SIMULATOR
int UserdataSize = sizeof(USERDATA);
USERDATA *mydata;
#endif

void setup() {
    // put your setup code here, to be run only once
    // Initialize the LED
    set_color(RGB(0,1,1)); // cyan ?
    delay(500);
    set_color(RGB(1,1,1));
    delay(1000);
}

void loop() {    
    // Turn led off for 1 sec
    set_color(RGB(0,0,0));
    delay(1000); 
    // Turn led red for 1 sec
    set_color(RGB(1,0,0));
    delay(1000);
    // Turn led green for 1 sec
    set_color(RGB(0,1,0));
    delay(1000);
    // Turn led blue for 1 sec
    set_color(RGB(0,0,1));
    delay(1000);
}

int main() {
    // initialize hardware
    kilo_init();
    // register your program
    kilo_start(setup, loop);

    return 0;
}
