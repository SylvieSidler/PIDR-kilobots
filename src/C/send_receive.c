
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



message_t received_message;
message_t globtransmit_msg;

REGISTER_USERDATA(USERDATA)





message_t *message_tx(){
    //printf("%d,%d \n",kilo_uid, mydata->stateLH);
    if (mydata->stateLH == SPEAKER){
        mydata->transmit_msg= globtransmit_msg;
        return &globtransmit_msg;
    }
    return 0;
}

void message_tx_success(){
    mydata->message_sent=1;
    //printf("ici!\n");
}

void message_rx(message_t *msg, distance_measurement_t *dist){
    received_message = *msg;
    mydata->rvd_message=received_message;
    if (mydata->stateLH==LISTENER && received_message.data[0]==0 && received_message.data[1]==1 ){
        mydata->new_message = 1;
        //printf("here!\n");
    }
}
void setup_message(){
    globtransmit_msg.type= NORMAL;
    globtransmit_msg.data[0]=0;
    globtransmit_msg.data[1]=1;
    globtransmit_msg.crc =message_crc(&globtransmit_msg);
    mydata->transmit_msg = globtransmit_msg;

}
void setup() {
    // Put any setup code here. This is run once before entering the loop.
    mydata->state = 0;
    int16_t id = kilo_uid;
    //printf("%d\n",id);
    //printf("%d\n",id%2);
    if (id%2==0){
        mydata->stateLH = LISTENER;
        set_color(RGB(3,0,0));
        //printf("here\n");
    }
    else{
        //mydata->stateLH = RIEN;
        mydata->stateLH = SPEAKER;
        set_color(RGB(0,0,3));
        //printf("003,state:%d\n",mydata->stateLH);
    }
    mydata->new_message =0;
    mydata->message_sent=0;
    setup_message();
    
}


void loop() {
    switch(mydata->stateLH){
        case SPEAKER:
            if (mydata->message_sent == 1){
                mydata->last_update = kilo_ticks;
                mydata->message_sent=0;
                //printf("message sent!\n");
                set_color(RGB(1,0,1));
                if (kilo_ticks > mydata->last_update + 124) {
                    set_color(RGB(0,0,3));
                }
            }
            else{
                set_color(RGB(0,0,3));
            }
            break;
        case LISTENER:
            if (mydata->new_message == 1){
                mydata->last_update = kilo_ticks;
                mydata->new_message=0;
                //printf("message received\n");
                set_color(RGB(1,1,0));
                if (kilo_ticks > mydata->last_update + 124) {
                    set_color(RGB(3,0,0));
                }
            }
            else{
                set_color(RGB(3,0,0));
            }
            break;
        case RIEN:
            set_color(RGB(0,3,0));
            //printf("je suis censé passer ici et pas au dessus\n");
    } 
    mydata->last_update = kilo_ticks;
    // Put the main code here. This is run repeatedly.
//#ifdef SIMULATOR
//    printf("%d tick %d\n", kilo_uid, kilo_ticks) ;
//#endif   
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
