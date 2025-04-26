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
    uint8_t new_message; // bool -> new message received or not
    uint8_t message_sent; // bool -> message sent or not
    message_t transmit_msg; // message to transmit
    message_t rvd_message; // message received
    uint8_t object; // object to name
    uint8_t personalWord; // word to name the object
    link_obj_word links[N]; // list of (un)active links between object and word
} USERDATA;


REGISTER_USERDATA(USERDATA);




void generateWord(void) {
    int randn = rand() % N;
    mydata->personalWord = globalWordList[randn];
}

// create a link between the object and the word in `mydata->links`
void generateLink(uint8_t object, uint8_t word) {
    link_obj_word link;
    link.object = object;
    link.word = word;
    link.boolean = 1;
    mydata->links[0] = link;
}

// deletes all links to the object except the word in parameters 
void deleteLinksExceptWord(uint8_t object, uint8_t keepWord) {
    for (uint8_t i = 0; i < N; i++) {
        if (mydata->links[i].word != keepWord) { // && mydata->links[i].object == object // si plusieurs objets
            mydata->links[i].boolean = 0;
        }
    }
}



message_t *message_tx(){
    //printf("%d,%d \n",kilo_uid, mydata->stateLH);
    if (mydata->message_sent == 0){
        if (mydata->stateLS == SPEAKER){
            mydata->transmit_msg.type= NORMAL;
            mydata->transmit_msg.data[0]=mydata->object;
            mydata->transmit_msg.data[1]=mydata->personalWord;
            mydata->transmit_msg.data[2]=kilo_uid;
            mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);
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
        int random = rand()%10;
        if (random > 6){
            mydata->rvd_message = *msg;
            if (mydata->stateLS==LISTENER){
                mydata->personalWord = mydata->rvd_message.data[1];
                generateLink(mydata->object,mydata->personalWord);
                deleteLinksExceptWord(mydata->object,mydata->personalWord);
                mydata->new_message = 1;
                //printf("here!\n");
            }
        }
    }
}


void setup() { 
    mydata->object = OBJECT;
    mydata->stateLS = LISTENER;
    mydata->message_sent =0;
    mydata->new_message = 0;
    generateWord();
    //printf("setup object: %d, word: %d\n", mydata->object, mydata->personalWord);
    generateLink(mydata->object, mydata->personalWord);
    mydata->last_update = kilo_ticks;
    set_color(colours[0]);
}


void loop() {
    switch(mydata->stateLS){
        case SPEAKER:
            if (mydata->message_sent == 1){
                //printf("message sent!\n");
                set_color(RGB(1,0,1));
                if (kilo_ticks> mydata->last_update+64){
                   //printf("testsp1\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLS = LISTENER;
                        set_color(colours[mydata->personalWord]);
                        
                    }
                    mydata->last_update = kilo_ticks;
                }
                mydata->message_sent=0;
            }
            else{
                set_color(colours[mydata->personalWord]);
                if (kilo_ticks> mydata->last_update+64){
                    //printf("testsp2\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLS = LISTENER;
                        set_color(colours[mydata->personalWord]);
                       
                    }
                    set_color(colours[mydata->personalWord]);
                }
            }
            break;
        case LISTENER:
            if (mydata->new_message == 1){
                //printf("message received\n");
                set_color(RGB(1,1,0));
                if (kilo_ticks> mydata->last_update+64){
                    //printf("testls1\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLS = SPEAKER;
                        set_color(colours[mydata->personalWord]);
                       
                    }
                    mydata->last_update = kilo_ticks;
                }
                mydata->new_message=0;
            }
            else{
                set_color(colours[mydata->personalWord]);
                if (kilo_ticks> mydata->last_update+64){
                    //printf("testls2\n");
                    uint8_t random = rand()%100;
                    if (random >85){
                        //printf("id=%d, random =%d\n", kilo_uid, random);
                        mydata->stateLS = SPEAKER;
                        set_color(colours[mydata->personalWord]);
                       
                    }
                    mydata->last_update = kilo_ticks;
                }
            }
            break;
    } 

}


int main()
{
    // Initialize the kilobots 
    kilo_init();

    // Set the callback functions
    kilo_message_tx=message_tx;
    kilo_message_tx_success = message_tx_success;
    kilo_message_rx=message_rx;
    
    kilo_start(setup, loop);
    
    return 0;
}