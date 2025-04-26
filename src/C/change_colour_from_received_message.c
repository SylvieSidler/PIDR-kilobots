
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
    int colour;

} USERDATA;


int colours[] = {RGB(0,0,3),RGB(3,0,0),RGB(0,3,0),RGB(3,0,3),RGB(0,3,3),RGB(3,3,0)};
message_t received_message;
message_t globtransmit_msg;

REGISTER_USERDATA(USERDATA)


int choose_colour(){
    int random = rand()%6;
    //printf("random:%d\n", random);
    return colours[random];
}


message_t *message_tx(){
    //printf("%d,%d \n",kilo_uid, mydata->stateLH);
    if (mydata->message_sent == 0){
        if (mydata->stateLH == SPEAKER){
            int colour = choose_colour();
            globtransmit_msg.type= NORMAL;
            globtransmit_msg.data[0]=0;
            globtransmit_msg.data[1]=colour;
            globtransmit_msg.crc =message_crc(&globtransmit_msg);
            mydata->transmit_msg = globtransmit_msg;
            return &globtransmit_msg;
        }
        return 0;
    }
    return 0;
}

void message_tx_success(){
    mydata->message_sent=1;
    //printf("ici!\n");
}

void message_rx(message_t *msg, distance_measurement_t *dist){
    if (mydata->new_message ==0) {
        received_message = *msg;
        mydata->rvd_message=received_message;
        if (mydata->stateLH==LISTENER && received_message.data[0]==0){
            mydata->colour = mydata->rvd_message.data[1];
            mydata->new_message = 1;
            //printf("here!\n");
        }
}
}
// void setup_message(){
//     int colour = choose_colour();
//     globtransmit_msg.type= NORMAL;
//     globtransmit_msg.data[0]=0;
//     globtransmit_msg.data[1]=colour;
//     globtransmit_msg.crc =message_crc(&globtransmit_msg);
//     mydata->transmit_msg = globtransmit_msg;

// }
void setup() {
    // Put any setup code here. This is run once before entering the loop.
    mydata->state = 0;
    mydata->colour =0;
    mydata->stateLH = LISTENER;
    mydata->new_message =0;
    mydata->message_sent=0;
    //setup_message();
    mydata->last_update = kilo_ticks;
    set_color(mydata->colour);
    
}


void loop() {
    switch(mydata->stateLH){
        case SPEAKER:
            if (mydata->message_sent == 1){
                //printf("message sent!\n");
                set_color(RGB(1,0,1));
                if (kilo_ticks> mydata->last_update+320){
                    //printf("test\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLH = LISTENER;
                        set_color(mydata->colour);
                        
                    }
                    mydata->last_update = kilo_ticks;
                }
                mydata->message_sent=0;
            }
            else{
                set_color(mydata->colour);
                if (kilo_ticks> mydata->last_update+320){
                    //printf("test\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLH = LISTENER;
                        set_color(mydata->colour);
                       
                    }
                    mydata->last_update = kilo_ticks;
                }
            }
            break;
        case LISTENER:
            if (mydata->new_message == 1){
                //printf("message received\n");
                set_color(RGB(1,1,0));
                if (kilo_ticks> mydata->last_update+320){
                    //printf("test\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLH = SPEAKER;
                        set_color(mydata->colour);
                       
                    }
                    mydata->last_update = kilo_ticks;
                }
                mydata->new_message=0;
            }
            else{
                set_color(mydata->colour);
                if (kilo_ticks> mydata->last_update+320){
                    //printf("test\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLH = SPEAKER;
                        set_color(mydata->colour);
                       
                    }
                    mydata->last_update = kilo_ticks;
                }
            }
            break;
        case RIEN:
            set_color(RGB(0,3,0));
            //printf("je suis censé passer ici et pas au dessus\n");
    } 
    //mydata->last_update = kilo_ticks;
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
