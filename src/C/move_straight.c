#include <kilombo.h>
#ifdef SIMULATOR
#include <stdio.h> // for printf
#else
#include <avr/io.h>  // for microcontroller register defs
//  #define DEBUG          // for printf to serial port
//  #include "debug.h"
#endif
#define MOVE_TIME 4*32

typedef struct{
    uint32_t move_cpt;
    uint8_t spinup;
}USERDATA;

REGISTER_USERDATA(USERDATA);

void move(){
    if (mydata->spinup==0){
        //printf("here\n");
        spinup_motors();
        //printf("kilo-straigh:%d\n",kilo_straight_left);
        set_motors(100,100);
        mydata->spinup=1;
        mydata->move_cpt=kilo_ticks;
    }
}

void setup(){
    //mydata->move_cpt=kilo_ticks;
    //mydata->spinup=0;
    spinup_motors();
    set_motors(100,100);
    delay(4000);
    set_motors(0,0);

}

void loop(){
    //printf("huhu");
    //move();
    //if (kilo_ticks>mydata->move_cpt +MOVE_TIME && mydata->spinup==1){
        //printf("ici\n");
        //set_motors(0,0);
        //mydata->move_cpt=kilo_ticks;
        //mydata->spinup=0;
   // }
}




int main()
    {
        kilo_init();
        kilo_start(setup, loop);
        
        return 0;
    }
    