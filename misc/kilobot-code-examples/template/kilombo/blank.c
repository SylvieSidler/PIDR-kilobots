
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
} USERDATA;

REGISTER_USERDATA(USERDATA)

void setup() {
    // Put any setup code here. This is run once before entering the loop.
    mydata->state = 0;
}

void loop() {
    // Put the main code here. This is run repeatedly.
#ifdef SIMULATOR
    printf("%d tick %d\n", kilo_uid, kilo_ticks) ;
#endif   
}
    
  


int main()
{
    // Initialize the hardware.
    kilo_init();
    // Register the program.
    kilo_start(setup, loop);
    
    return 0;
}
