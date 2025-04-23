
#include <kilombo.h>
#include "naming_for_simulator.h"
#ifdef SIMULATOR
#include <stdio.h> // for printf
#include <stdint.h>
#else
#include <avr/io.h>  // for microcontroller register defs
//  #define DEBUG          // for printf to serial port
//  #include "debug.h"
#endif

typedef struct {
    uint8_t state;
    uint32_t last_update;
    enum state stateLH;
    message_t transmit_msg;
    message_t rvd_message;
    int new_message;
    int message_sent;

    uint8_t objects[Nb_diff_obj];
    uint8_t words[Nb_diff_obj*Nb_max_diff_words_for_objs];
    link_obj_word_t list_links_obj_word[Nb_diff_obj*Nb_max_diff_words_for_objs*Nb_diff_obj];
    uint8_t colour;
} USERDATA;


message_t received_message;
message_t globtransmit_msg;

REGISTER_USERDATA(USERDATA)

uint8_t obj_known(uint8_t object){
    return mydata->objects[object];
}

uint8_t word_known(uint8_t word){
    return mydata->words[word];
}

void get_all_links(uint8_t object, uint8_t t[Nb_diff_obj*Nb_max_diff_words_for_objs]) {
    for(int i = 0; i< Nb_diff_obj*Nb_max_diff_words_for_objs; i++) {
        t[i] = mydata->list_links_obj_word[object+i*Nb_diff_obj].link;
    }
}

uint8_t link_word_obj_exists(uint8_t object, uint8_t word){
    uint8_t t[Nb_diff_obj*Nb_max_diff_words_for_objs];
    get_all_links(object,t);
    return t[word];
}



void remove_link(uint8_t object, uint8_t word){
    mydata->list_links_obj_word[object+word*Nb_diff_obj].link = 0;
    uint8_t used = 0;
    for(int o_id = 0; o_id < Nb_diff_obj; o_id++) {
        used = used || mydata->list_links_obj_word[o_id+word*Nb_diff_obj].link;
    }
    mydata->words[word] = used;
}

uint8_t generate_word(){
    uint8_t lp_state = 1;
    uint8_t i = 0;
    uint8_t count = 0;
    uint8_t chosen_word = 255;
    uint8_t available_words[Nb_diff_obj*Nb_max_diff_words_for_objs];
    uint16_t rand_int = rand_soft();
    for(int k = 0; k < (kilo_uid%255); k++) {
        rand_int = rand_soft();
    }
    while (lp_state) {
        if(i >= Nb_diff_obj*Nb_max_diff_words_for_objs) {
            lp_state = 0;
        } else {
            if (mydata->words[i] == 0) { 
                available_words[count] = i;
                count++;
            }
        }
        i++;
    }
    if (count > 0) {
        rand_int = rand_int % count;
        chosen_word = available_words[rand_int];
    }
    mydata->words[chosen_word] = 1;
    //if (chosen_id == 0) {chosen_id++;}
    mydata->colour = chosen_word%255;
    return chosen_word;
}

void generate_new_link_ow(uint8_t object, uint8_t word){ 
    uint8_t t[Nb_diff_obj*Nb_max_diff_words_for_objs];
    get_all_links(object, t);
    uint8_t rand_int = rand_soft();
    uint8_t list_links[Nb_diff_obj*Nb_max_diff_words_for_objs];
    uint8_t count = 0;
    for (int i = 0; i<Nb_diff_obj*Nb_max_diff_words_for_objs; i++) {
        if (t[i]) {
            list_links[count] = i;
            count++;
        }
    }
    if (count >= 5) {
        
        rand_int = rand_int % count;
        remove_link(mydata->list_links_obj_word[list_links[rand_int]*Nb_diff_obj+object].object,mydata->list_links_obj_word[list_links[rand_int]*Nb_diff_obj+object].word);
    }
    mydata->list_links_obj_word[object+word*Nb_diff_obj].link = 1;
    
}
void generate_new_link_o(uint8_t object) {
    uint8_t new_word = generate_word();
    generate_new_link_ow(object,new_word);
}

uint8_t handle_message_as_hearer(uint8_t object, uint8_t word) {
    if (obj_known(object) == 0) {
        mydata->objects[object] = 1;
        if (word_known(word)==1) {
            generate_new_link_o(object);
        } else {
            mydata->words[word] = 1;
            generate_new_link_ow(object,word);
        }
    } else if (link_word_obj_exists(object, word)) {
        remove_link(object,word);
        return 1;
    } else {
        if (word_known(word)==1) {
            generate_new_link_o(object);
        } else {
            mydata->words[word] = 1;
            generate_new_link_ow(object,word);
        }
    }
    return 0;
}

void handle_message_object(uint8_t object) {
    if (obj_known(object) == 0) {
        mydata->objects[object] = 1;
        generate_new_link_o(object);
    }
}

void reset_msg_sent() {
    for (int i =0 ; i<9; i++) {
        globtransmit_msg.data[i] = 0;
    }
}

void send_message_speaker() {
    uint8_t rand_int = rand_soft();
    uint8_t associations_list[Nb_diff_obj*Nb_max_diff_words_for_objs];
    uint8_t count = 0;
    uint8_t chosen_object;
    uint8_t chosen_word;
    // Get id of all active associations
    for (int i = 0; i<Nb_diff_obj*Nb_diff_obj*Nb_max_diff_words_for_objs; i++) {
        if (mydata->list_links_obj_word[i].link) {
            associations_list[count] = i;
            count++;
            // if (kilo_uid == 20) {
            //     fprintf(fptr,"B %d\n",memory[i].word_id);
            // }
        }
    }
    if (count > 0) {
        rand_int = rand_int % count;
        chosen_object= mydata->list_links_obj_word[associations_list[rand_int]].object;
        chosen_word = mydata->list_links_obj_word[associations_list[rand_int]].word;
        //mydata->transmit_message.type=NORMAL;
        globtransmit_msg.type =NORMAL;
        reset_msg_sent();
        //mydata->transmit_message.data[0] = chosen_object;
        //mydata->transmit_message.data[1] = chosen_word;
        //mydata->transmit_message.data[2] = kilo_uid%255; 
        //mydata->transmit_message.data[3] = mydata->stateLH;
        //mydata->transmit_message.data[4] = 1;
        //mydata->transmit_message.crc = message_crc(&mydata->message_sent);
        globtransmit_msg.data[0] = chosen_object;
        globtransmit_msg.data[1]=chosen_word;
        globtransmit_msg.data[2]=kilo_uid%255;
        globtransmit_msg.data[3]=mydata->stateLH;
        globtransmit_msg.data[4]=1;
        globtransmit_msg.crc=message_crc(&globtransmit_msg);
        //msg_ptr = &msg_sent;

    }
}

message_t *message_tx(){
    //printf("%d,%d \n",kilo_uid, mydata->stateLH);
    if (mydata->stateLH == SPEAKER){
        mydata->transmit_msg= message_sent;
        return &globtransmit_msg;
    }
    else{
        //printf("return: %d,%d \n",kilo_uid, mydata->stateLH);
        return NULL;
    }
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
