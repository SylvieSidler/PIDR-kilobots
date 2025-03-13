
#include <kilombo.h>
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
    uint32_t last_changed;
} USERDATA;

REGISTER_USERDATA(USERDATA)

void setup()
{
    // Put any setup code here. This is run once before entering the loop.
    mydata->state = 0;
    mydata->last_changed = kilo_ticks;
    
}

void loop() {
    // Put the main code here. This is run repeatedly.
#ifdef SIMULATOR
    printf("%d tick %d\n", kilo_uid, kilo_ticks) ;
#endif   
    
    if (kilo_ticks > mydata->last_changed + 32) {
        mydata->last_changed = kilo_ticks;
        if (mydata->state == 0) {
            set_color(RGB(1, 0, 0));
            mydata->state = 1;
#ifdef SIMULATOR
            printf("%d turning Red\n", kilo_uid);
#endif
        } else {
            set_color(RGB(0, 0, 1));
            mydata->state = 0;
#ifdef SIMULATOR
            printf("%d turning Blue\n", kilo_uid);
#endif
        }
    }
    
    // Set the LED red.
    //set_color(RGB(1, 0, 0));
    // Wait half a second (500 ms).
    //delay(500);
    
    // Set the LED blue.
    //set_color(RGB(0, 0, 1));
    // Wait half a second (500 ms).
    //delay(500);
}

int main()
{
    // Initialize the hardware.
    kilo_init();
    // Register the program.
    kilo_start(setup, loop);
    
    return 0;
}
