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
    //uint32_t last_update;
    uint8_t new_message; // bool -> new message received or not
    uint8_t message_ready; // bool -> message sent or not
    message_t transmit_msg; // message to transmit
    message_t rvd_message; // message received
    uint8_t object; // object to name
    uint8_t personalWord; // word to name the object
    link_obj_word links[N]; // list of (un)active links between object and word
    uint32_t send_cpt;
    uint32_t receive_cpt;
    uint32_t state_cpt;
    uint32_t delay_start;
    enum direction dir;
    uint32_t move_cpt;
    uint32_t last_message_received_time;
} USERDATA;


REGISTER_USERDATA(USERDATA);




void generateWord(void) {
    int randn = rand_soft() % N;
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

void random_direction(){
    int randn= rand_soft()%4;
    mydata->dir = randn;
}

void move(){
    switch(mydata->dir){
        case RIGHT:
            spinup_motors();
            set_motors(0,kilo_straight_right);
            break;
        case LEFT:
            spinup_motors();
            set_motors(kilo_straight_left,0);
            break;
        case FORWARD:
            spinup_motors();
            set_motors(kilo_straight_left,kilo_straight_right);
            break;
        case BACKWARD:
            spinup_motors();
            set_motors(kilo_turn_left,0);
            break;
        case STOP:
            set_motors(0,0);
            break;
    }
}

void move_kilobot(){
    uint8_t rand = rand_soft()%100;
    if (rand> 50){
        random_direction();
        move();
        mydata->move_cpt = kilo_ticks;
    }
    else{
        move();
        mydata->move_cpt = kilo_ticks;
    }
}


message_t *message_tx(){
    //printf("%d,%d \n",kilo_uid, mydata->stateLH);
    if (mydata->message_ready == 1){
        return &mydata->transmit_msg;
    }
    return 0;
}

void message_tx_success(){
    mydata->message_ready =0;
}

void message_rx(message_t *msg, distance_measurement_t *dist ){
    if (mydata->new_message ==0) {
        mydata->rvd_message = *msg;
        mydata->new_message = 1;
    }
}


void setup() { 
    mydata->object = OBJECT;
    mydata->stateLS = LISTENER;
    mydata->message_ready =0;
    mydata->new_message = 0;
    //printf("setup object: %d, word: %d\n", mydata->object, mydata->personalWord);
    generateLink(mydata->object, mydata->personalWord);
    //mydata->last_update = kilo_ticks;
    //set_color(colours[0]);
    mydata->send_cpt=kilo_ticks;
    mydata->receive_cpt=kilo_ticks;
    mydata->state_cpt=kilo_ticks;
    mydata->delay_start=0;//rand()%320;
    mydata->move_cpt = kilo_ticks;
    mydata->last_message_received_time= kilo_ticks;
    mydata->dir = STOP;
    rand_seed(rand_hard());
    generateWord();
}


void loop() {
    if (mydata->delay_start--<=0){
        return;
    }
    //set_color(colours[mydata->personalWord]);
    switch(mydata->stateLS){
        case SPEAKER:
            set_color(RGB(3,0,0));
            if (kilo_ticks > mydata->send_cpt + SEND_DELAY){
                mydata->send_cpt =kilo_ticks;
                mydata->transmit_msg.type= NORMAL;
                mydata->transmit_msg.data[0]=mydata->object;
                mydata->transmit_msg.data[1]=mydata->personalWord;
                mydata->transmit_msg.data[2]=kilo_uid;
                mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);
                mydata->message_ready = 1;
            }
            move();
            break;
        case LISTENER:
            set_color(RGB(0,0,3));
            if (kilo_ticks > mydata->receive_cpt+ RECEIVE_DELAY){
                mydata->receive_cpt=kilo_ticks;
                if (mydata->new_message==1){
                    mydata->last_message_received_time=kilo_ticks;
                    mydata->dir=STOP;
                    move();
                    int random = rand_soft()%10;
                    if (random > 6){
                        mydata->personalWord = mydata->rvd_message.data[1];
                        generateLink(mydata->object,mydata->personalWord);
                        deleteLinksExceptWord(mydata->object,mydata->personalWord);
                    }
                    mydata->new_message=0;
                }
            }
            if (kilo_ticks>mydata->last_message_received_time+NO_RECEPTION){
                mydata->last_message_received_time=kilo_ticks;
                move_kilobot();
                if (kilo_ticks> mydata->move_cpt+MOVE_DELAY){
                    set_motors(0,0);
                    mydata->dir = STOP;
                    mydata->move_cpt = kilo_ticks;
                   
                }
            }

            break;
    }
    if (kilo_ticks> mydata->state_cpt +STATE_DELAY){
        mydata->state_cpt=kilo_ticks;
        uint8_t random = rand_soft()%100;
        if (random >50){
            if (mydata->stateLS ==SPEAKER) {
                mydata->stateLS = LISTENER;
                mydata->message_ready=0;
                mydata->receive_cpt=kilo_ticks;
                mydata->move_cpt = kilo_ticks;
                mydata->last_message_received_time=kilo_ticks;
                set_color(RGB(0,0,3));
            }
            else{
                mydata->stateLS = SPEAKER;
                mydata->send_cpt=kilo_ticks;
                set_color(RGB(3,0,0));
                mydata->dir=STOP;
                mydata->move_cpt = kilo_ticks;
                move();
            }
            
        }
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