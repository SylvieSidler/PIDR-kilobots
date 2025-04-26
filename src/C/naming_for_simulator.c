
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
        mydata->transmit_msg= globtransmit_msg;
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
    if (mydata->stateLH==LISTENER && received_message.data[4]==1 ){
        mydata->new_message = 1;
        printf("here!\n");
    }
    else{
        //mydata->rvd_message = 0;
        mydata->rvd_message.type = 127;
        mydata->rvd_message.data[4] = 0;
        mydata->rvd_message.crc = message_crc(&mydata->rvd_message);
    }
}


void initialize_kilobots() {
    mydata->stateLH = LISTENER;
    for(int i = 0; i<Nb_diff_obj; i++) {
        mydata->objects[i] = 1;
        for(int j = 0; j<Nb_max_diff_words_for_objs; j++) {
            mydata->words[i*Nb_max_diff_words_for_objs+j] = 0;
            for(int k = 0;k<Nb_diff_obj;k++) {
                mydata->list_links_obj_word[i*Nb_max_diff_words_for_objs*Nb_diff_obj+j*Nb_diff_obj+k].link = 0;
                mydata->list_links_obj_word[i*Nb_max_diff_words_for_objs*Nb_diff_obj+j*Nb_diff_obj+k].object = k;
                mydata->list_links_obj_word[i*Nb_max_diff_words_for_objs*Nb_diff_obj+j*Nb_diff_obj+k].word= i*Nb_max_diff_words_for_objs+j;
                
            }      
        }
    }

    for(int i = 0; i<Nb_diff_obj; i++) {
        generate_new_link_o(i);
    }
}
// void setup_message(){
//     globtransmit_msg.type= NORMAL;
//     globtransmit_msg.data[0]=0;
//     globtransmit_msg.data[1]=1;
//     globtransmit_msg.crc =message_crc(&globtransmit_msg);
//     mydata->transmit_msg = globtransmit_msg;

// }
void setup() {
    // Put any setup code here. This is run once before entering the loop.
    mydata->state = 0;
    // int16_t id = kilo_uid;
    // //printf("%d\n",id);
    // //printf("%d\n",id%2);
    // if (id%2==0){
    //     mydata->stateLH = LISTENER;
    //     set_color(RGB(3,0,0));
    //     //printf("here\n");
    // }
    // else{
    //     //mydata->stateLH = RIEN;
    //     mydata->stateLH = SPEAKER;
    //     set_color(RGB(0,0,3));
    //     //printf("003,state:%d\n",mydata->stateLH);
    // }
    mydata->new_message =0;
    mydata->message_sent=0;
    initialize_kilobots();
    mydata->colour = RGB(0,0,0);
    set_color(mydata->colour);
    
    //setup_message();
    
}


void loop() {
    set_color(mydata->colour);
    switch(mydata->stateLH){
        case SPEAKER:
            if (mydata->message_sent == 1){
                mydata->last_update = kilo_ticks;
                mydata->message_sent=0;
                //printf("message sent!\n");
                set_color(RGB(1,0,1));
                if (kilo_ticks > mydata->last_update + 124) {
                    set_color(mydata->colour);
                }
            }
            else{
                set_color(mydata->colour);
            }
            break;
        case LISTENER:
            if (mydata->new_message == 1){
                mydata->last_update = kilo_ticks;
                mydata->new_message=0;
                printf("message received %d\n", kilo_uid);
                set_color(RGB(1,1,0));
                uint8_t object_id = mydata->rvd_message.data[0];
                uint8_t word_id = mydata->rvd_message.data[1];
                if (mydata->rvd_message.data[3]) {
                    handle_message_as_hearer(object_id, word_id); 
                }
                else {
                    handle_message_object(object_id);
                    //printf("%d received a message from %d (%d); o_id = %d, w_id = %d\n",recipient_id,sender_id, message_recvd.data[3],object_id,word_id);
                }
                if (kilo_ticks > mydata->last_update + 124) {
                    set_color(mydata->colour);
                    mydata->last_update = kilo_ticks;
                    rand_seed(kilo_uid%255);
                    uint8_t rand_int = rand_soft();
                    printf("randint %d\n", rand_int);
                    if (rand_int <= (255*20)/100) {
                        mydata->stateLH = SPEAKER;
                        set_color(mydata->colour);
                        printf("%d\n", kilo_uid);
                        send_message_speaker();
                    } else {
                        mydata->stateLH = LISTENER;
                        set_color(mydata->colour);
                    }
                    uint8_t words_used = 0;
                    uint16_t word_value = 0;
                    uint8_t state = 0;
                    for (int k = 0; k < Nb_diff_obj*Nb_max_diff_words_for_objs; k++) {
                        words_used += mydata->words[k];
                        printf("words[k] %d, %d \n", mydata->words[k], kilo_uid);
                        state = state || mydata->words[k];
                        if (mydata->words[k]) {
                            word_value += k;
                        }
                    }   
        
                    if (words_used == Nb_diff_obj) {
                        mydata->colour = word_value%256;
                    }
                    else {
                        mydata->colour = 0;
                    }
                }
            }
            else{
                mydata->last_update = kilo_ticks;
                    uint8_t rand_int = rand_soft();
                    printf("randint %d\n", rand_int);
                    if (rand_int <= (255*20)/100) {
                        mydata->stateLH = SPEAKER;
                        set_color(mydata->colour);
                        printf("%d\n", kilo_uid);
                        send_message_speaker();
                    } else {
                        mydata->stateLH = LISTENER;
                        set_color(mydata->colour);
                    }
                    uint8_t words_used = 0;
                    uint16_t word_value = 0;
                    uint8_t state = 0;
                    for (int k = 0; k < Nb_diff_obj*Nb_max_diff_words_for_objs; k++) {
                        words_used += mydata->words[k];
                        printf("words[k] %d, %d \n", mydata->words[k], kilo_uid);
                        state = state || mydata->words[k];
                        if (mydata->words[k]) {
                            word_value += k;
                        }
                    }   
        
                    if (words_used == Nb_diff_obj) {
                        mydata->colour = word_value%256;
                    }
                    else {
                        mydata->colour = 0;
                    }
                set_color(mydata->colour);
            }
                break;
        case RIEN:
            set_color(mydata->colour);
            break;
            //printf("je suis censé passer ici et pas au dessus\n");
    }
    // Put the main code here. This is run repeatedly.
//#ifdef SIMULATOR
//    printf("%d tick %d\n", kilo_uid, kilo_ticks) ;
//#endif   
}
    
  


int main(){
    // Initialize the hardware.
    kilo_init();
    kilo_message_tx=message_tx;
    kilo_message_tx_success = message_tx_success;
    kilo_message_rx=message_rx;
    // Register the program.
    kilo_start(setup, loop);
    
    return 0;
}
