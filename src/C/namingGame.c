#include "namingGame.h"
#ifdef SIMULATOR
#include <stdio.h> // for printf
#else
#include <avr/io.h>  // for microcontroller register defs
//  #define DEBUG          // for printf to serial port
//  #include "debug.h"
#endif

typedef struct {
    enum state stateLS; // state : LISTENER or SPEAKER
    uint32_t last_update;
    uint8_t new_message = 0; // bool -> new message received or not
    uint8_t message_sent = 0; // bool -> message sent or not
    message_t transmit_msg; // message to transmit
    message_t rvd_message; // message received
    uint8_t object; // object to name
    uint8_t personalWord; // word to name the object
    link_obj_word links[N]; // list of (un)active links between object and word
} USERDATA;


REGISTER(USERDATA);

message_t *message_tx(){
    //printf("%d,%d \n",kilo_uid, mydata->stateLH);
    if (mydata->message_sent == 0){
        if (mydata->stateLH == SPEAKER){
            int colour = choose_colour();
            mydata->transmit_msg.type= NORMAL;
            mydata->transmit_msg.data[0]=mydata->object;
            mydata->transmit_msg.data[1]=mydata->personalWord;
            mydata->transmit_msg.data[2]=kilo_uid;
            mydata->transmit_msg.crc =message_crc(&mydata->transmit_msg);
            return &mydata->transmit_msg;
        }
        return 0;
    }
    return 0;
}

void message_tx_success(){
    mydata->message_sent =1;
}

void message_rx(message_t *msg, distance_measurement_t *dist ){
    if (mydata->new_message ==0) {
        int random = 
    }
}