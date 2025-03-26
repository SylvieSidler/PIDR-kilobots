#include <kilombo.h>
#include <math.h>
#include "naming.h"


// Kilobot & Communication related variables
#define NO_MSG 127
#define RED_MASK RGB(3,0,0)
#define GREEN_MASK RGB(0,3,0)
#define BLUE_MASK RGB(0,0,3)
uint32_t last_changed;
uint32_t last_changed_movement;
uint32_t last_changed_turning;
uint32_t last_changed_straight;
uint32_t last_changed_msg;
uint8_t recvd_message = 0;
uint8_t sent_message;
message_t msg_sent;
message_t message_recvd;
message_t * msg_ptr = &msg_sent;
uint8_t new_dist = 0;
enum MNG_STATE state1;
enum MOV_STATE action;


    

// MNG variables
uint8_t object_list[ND]; // List of all known objects by the agent. 0 at object_list[i] means that the i object is not known.
uint8_t inventory[ND*MM]; // List of all used words by the agent. 0 at inventory[i] means that the i words is not used.
relation_t memory[ND*MM*ND]; //  List of all object/word association known by the agent.
uint32_t last_changed_speaker; // Keeps track of time for trying to become a speaker.
message_t last_msg_rcvd; // Where the last mesage received is saved. Used to check whether a newly received message is idetical to it, in order to reduce spam.
uint16_t iter; // Current iteration value.
uint8_t speaker_encountered; // Boolean to describe if an encounter with a peaker already happened this iteration
uint8_t color = 0;



////////////////////////////////////////////////////////////////////////
// SETUP

// declare motion variable type
typedef enum {
    STOP,
    FORWARD,
    LEFT,
    RIGHT
} motion_t;

// declare variables
typedef struct {
    uint32_t last_update;
    int N_Neighbors; 
} USERDATA;

extern USERDATA *mydata;

extern char* (*callback_botinfo) (void);
char *botinfo(void);

#ifdef SIMULATOR /////////

#include <stdio.h>    // for printf
int UserdataSize = sizeof(USERDATA);
USERDATA *mydata;

static char botinfo_buffer[10000];
char *botinfo(void) {
    int n;
    char *p = botinfo_buffer;
    n = sprintf (p, "ID: %d ", kilo_uid);
    //p += n;
    //n = sprintf (p, "Ns: %d, dist: %d\n ", mydata->N_Neighbors, find_nearest_N_dist());
    p += n;

    return botinfo_buffer;
}

#endif ///////////////////


////////////////////////////////////////////////////////////////////////
// CODE 


// Generate a new word, not already used by the agent. Pick a random one among those.
uint8_t generate_new_word() {
    uint8_t lp_state = 1;
    uint8_t i = 0;
    uint8_t count = 0;
    uint8_t chosen_id = 255;
    uint8_t available_words[ND*MM];
    uint8_t rand_int = rand_soft();
    for(int k = 0; k< kilo_uid%255; k++) {
        rand_int = rand_soft();
    }
    while (lp_state) {
        if(i >= ND*MM) {
            lp_state = 0;
        } else {
            if (inventory[i] == 0) { 
                available_words[count] = i;
                count++;
            }
        }
        i++;
    }
    if (count > 0) {
        rand_int = rand_int % count;
        chosen_id = available_words[rand_int];
    }
    inventory[chosen_id] = 1;
    //if (chosen_id == 0) {chosen_id++;}
    color = chosen_id%255;
    return chosen_id;
}



// Return 1 if the word is already in use, 0 otherwise
uint8_t is_word_used(uint8_t word_id) {
    return inventory[word_id];
}

// Return 1 if the object is already known, 0 otherwise
uint8_t is_object_known(uint8_t object_id) {
    return object_list[object_id];
}

// Return a list of all word associated to an object, through a tab passed to the function.
// Return_tab[i] equals 1 if the i word is associated to that object, and 0 otherwise.
void get_relations(uint8_t object_id, uint8_t return_tab[ND*MM]) {
    for(int i = 0; i< ND*MM;i++) {
        return_tab[i] = memory[object_id+i*ND].active;
    }
}

// Return 1 if the object and the word are associated, 0 otherwise.
uint8_t are_object_word_related(uint8_t object_id, uint8_t word_id) {
    uint8_t tab[ND*MM];
    get_relations(object_id, tab);
    return tab[word_id];
}

// Remove a specific association from memory
uint8_t remove_relation(uint8_t object_id, uint8_t word_id) {
    memory[object_id+word_id*ND].active = 0;
    uint8_t used = 0;
    for(int o_id = 0; o_id < ND; o_id++) {
        used = used || memory[o_id+word_id*ND].active;
    }
    inventory[word_id] = used;
    return object_id+word_id*ND;
}

// Add a new association to memory
uint8_t generate_new_relation(uint8_t object_id, uint8_t word_id) {
     
    uint8_t tab[ND*MM];
    get_relations(object_id, tab);
    uint8_t rand_int = rand_soft();
    uint8_t associations_list[ND*MM];
    uint8_t count = 0;
    // Get id of all active associations for this object
    for (int i = 0; i<ND*MM; i++) {
        if (tab[i]) {
            associations_list[count] = i;
            count++;
        }
    }
    if (count >= 5) {
        
        rand_int = rand_int % count;
        remove_relation(memory[associations_list[rand_int]*ND+object_id].object_id,memory[associations_list[rand_int]*ND+object_id].word_id);
    }
    memory[object_id+word_id*ND].active = 1;
    return object_id+word_id*ND;
}


//Remove all associations but the one with the provided word_id for that object.
void remove_other_relations(uint8_t object_id, uint8_t word_id) {
    for(int i = 0; i < ND*MM; i++) {
        if (i != word_id) {
            remove_relation(object_id,i);
        }
    }
}

// Generate a full association, only based on object_id.
void generate_new_association(uint8_t object_id) {
    uint8_t new_word_id = generate_new_word();
    generate_new_relation(object_id,new_word_id);
}

// As a hearer, handle the message received from a speaker accordingly to the rules of the MNG.
// Return 1 if the game was a sucess, and return 0 otherwise.
uint8_t handle_message_speaker(uint8_t object_id, uint8_t word_id) {
    if (is_object_known(object_id) == 0) {
        object_list[object_id] = 1;
        if (is_word_used(word_id)) {
            generate_new_association(object_id);
        } else {
            inventory[word_id] = 1;
            generate_new_relation(object_id,word_id);
        }
    } else if (are_object_word_related(object_id, word_id)) {
        remove_other_relations(object_id,word_id);
        return 1;
    } else {
        if (is_word_used(word_id)) {
            generate_new_association(object_id);
        } else {
            inventory[word_id] = 1;
            generate_new_relation(object_id,word_id);
        }
    }
    return 0;
}

// As a hearer, handle the message received from an object.
void handle_message_object(uint8_t object_id) {
    if (is_object_known(object_id) == 0) {
        object_list[object_id] = 1;
        generate_new_association(object_id);
    }
}

// Reset the data of msg_sent
void reset_msg_sent() {
    for (int i =0 ; i<9; i++) {
        msg_sent.data[i] = 0;
    }
}

// Pick a random association from the agent memory, and set it as the next broadcasted message.
void send_message_speaker() {
    uint8_t rand_int = rand_soft();
    uint8_t associations_list[ND*MM];
    uint8_t count = 0;
    uint8_t chosen_object_id;
    uint8_t chosen_word_id;
    // Get id of all active associations
    for (int i = 0; i<ND*MM*ND; i++) {
        if (memory[i].active) {
            associations_list[count] = i;
            count++;
            // if (kilo_uid == 20) {
            //     fprintf(fptr,"B %d\n",memory[i].word_id);
            // }
        }
    }
    if (count > 0) {
        rand_int = rand_int % count;
        chosen_object_id = memory[associations_list[rand_int]].object_id;
        chosen_word_id = memory[associations_list[rand_int]].word_id;
        msg_sent.type=NORMAL;
        reset_msg_sent();
        msg_sent.data[0] = chosen_object_id;
        msg_sent.data[1] = chosen_word_id;
        msg_sent.data[2] = kilo_uid%255; 
        msg_sent.data[3] = state1;
        msg_sent.data[4] = 1; // Correction bit
        msg_sent.crc = message_crc(&msg_sent);
        msg_ptr = &msg_sent;

    }
}

// message transmission callback
message_t *tx_message() {
    return msg_ptr;
}
// successful transmission callback
void tx_message_success() {
    sent_message = 1;
}



// Receive message callback (limited to once every 0.25s)
void rx_message(message_t *msg, distance_measurement_t *d) {
    
    if (msg->crc == message_crc(msg)&& last_changed_msg + 8 < kilo_ticks) {
        last_changed_msg = kilo_ticks;
        uint8_t state = 0;
        if (msg->data[3] != OBJECT) {
            state = NORMAL == msg->type;
            last_msg_rcvd.type = msg->type;
            state = state && last_msg_rcvd.data[0] == msg->data[0];
            state = state && last_msg_rcvd.data[1] == msg->data[1];
            state = state && last_msg_rcvd.data[2] == msg->data[2];
            for (int i =0 ; i<9; i++) {
                last_msg_rcvd.data[i] = msg->data[i];
            }
        }

        // Don't "receive" a message if similar to the last one || it is corrupted
        if(state || msg->data[4] == 0 || speaker_encountered) {
            recvd_message = 0;
            message_recvd.type = NO_MSG;
            message_recvd.data[4] = 0;
            message_recvd.crc = message_crc(&message_recvd);
            new_dist = 0;
        } else {
            recvd_message = 1;
            message_recvd.type = msg->type;
            for (int i =0 ; i<9; i++) {
                message_recvd.data[i] = msg->data[i];
            }
            speaker_encountered = 1;
            message_recvd.crc = message_crc(&message_recvd);
        } 
    } else {
        message_recvd.type = NO_MSG;
        message_recvd.data[4] = 0;
        message_recvd.crc = message_crc(&message_recvd);
        new_dist = 0;
    }  
}

// Initialize all MNG variables at empty for the agent.
void initialize_agent() {
    last_changed_speaker = kilo_ticks;
    state1 = HEARER;
    speaker_encountered = 0;
    iter = 0;
    for(int i = 0; i<ND; i++) {
        object_list[i] = 1;
        for(int j = 0; j<MM; j++) {
            inventory[i*MM+j] = 0;
            for(int k = 0;k<ND;k++) {
                memory[i*MM*ND+j*ND+k].active = 0;
                memory[i*MM*ND+j*ND+k].object_id = k;
                memory[i*MM*ND+j*ND+k].word_id = i*MM+j;
                
            }      
        }
    }

    for(int i = 0; i<ND; i++) {
        generate_new_association(i);
    }
    // if (kilo_uid == 20) {
    //     for(int i = 0; i< MM*ND*ND; i++) {
    //         fprintf(fptr, "B mem %d = %d,%d\n",i, memory[i].object_id,memory[i].word_id);
    //     }
    // }
}


void setup() {
    last_changed = kilo_ticks;
    action = STRAIGHT;
    recvd_message = 0;
    msg_sent.type = NORMAL;
    msg_sent.data[0] = 0;
    msg_sent.crc = message_crc(&msg_sent);

    message_recvd.type = NO_MSG;
    message_recvd.data[0] = 0;
    message_recvd.crc = message_crc(&message_recvd);

    last_msg_rcvd.type = NO_MSG;
    last_msg_rcvd.data[0] = 0;
    last_msg_rcvd.crc = message_crc(&last_msg_rcvd);
    initialize_agent();
    msg_ptr = 0;
    rand_seed(kilo_uid%255);
}

void loop() {

    set_color(color);
    switch(state1) {
        case HEARER: 
            set_color(RED_MASK);
            if (recvd_message ) {
               recvd_message = 0;
               uint8_t object_id = message_recvd.data[0];
               uint8_t word_id = message_recvd.data[1];
               if (message_recvd.data[3]) {
                    handle_message_speaker(object_id, word_id); 
               }
               else {
                    handle_message_object(object_id);
                    //printf("%d received a message from %d (%d); o_id = %d, w_id = %d\n",recipient_id,sender_id, message_recvd.data[3],object_id,word_id);
               }
               
            }      
            break;
        
        case SPEAKER:
            set_color(GREEN_MASK);
             // Try to send a message every 1 seconds, with a probability of 1
            if (kilo_ticks > last_changed_speaker + 1*SEC) {
                last_changed_speaker = kilo_ticks;
                uint8_t rand_int = rand_soft();
                if (rand_int >= 0) {
                    msg_ptr = &msg_sent;
                }
            }
            break;
        default:
            break;
    }

    // If a message is successfully sent (!!!! not necessarily received), "disable" message broadcast for now
    if (sent_message) {
        msg_ptr = 0;
        sent_message = 0;
    }
    
    // Random movement from the robot. Every 10 seconds, agent has a 50% chance to turn in a random direction for 10 seconds.
    //Otherwise, they go straight for 5 seconds
    if (iter < MAXT) {
    switch (action) {
        case STRAIGHT :
            if (kilo_ticks > last_changed_movement + 10*SEC) {
                last_changed_movement = kilo_ticks;
                last_changed_turning = kilo_ticks;
                last_changed_straight = kilo_ticks;
                uint8_t rand_int = rand_soft();
                if (rand_int <= 127) {
                    action = TURNING;
                    if(rand_int <= 63) {
                        spinup_motors();
                        set_motors(kilo_turn_left, 0);
                    } else {
                        spinup_motors();
                        set_motors(0, kilo_turn_right);
                    }
                } else {
                    spinup_motors();
                    set_motors(kilo_straight_left, kilo_straight_right);
                }
            }
            else if (kilo_ticks > last_changed_straight + 5*SEC) {
                set_motors(0,0);
            }
            break;
        case TURNING:
            if (kilo_ticks > last_changed_turning + 10*SEC) {
                action = STRAIGHT;
                last_changed_straight = kilo_ticks;
                spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
            }
            break;
    } } else {set_motors(0,0);}

    // If TS sec has passed, try to become a speaker
    if (kilo_ticks > last_changed + TS*SEC && iter < MAXT) {
        last_changed = kilo_ticks;
        uint8_t rand_int = rand_soft();
        if (rand_int <= (255*PS)/100) {
            state1 = SPEAKER;
            send_message_speaker();
        } else {
            state1 = HEARER;
        }
        speaker_encountered = 0;
        uint8_t words_used = 0;
        uint16_t word_value = 0;
        uint8_t state = 0;
        for (int k = 0; k < ND*MM; k++) {
            words_used += inventory[k];
            state = state || inventory[k];
            if (inventory[k]) {
                word_value += k;
            }
        }
        
        if (words_used == ND) {
            color = word_value%256;
        }
        else {
            color = 0;
        }

    }
    
}

int main() {
    kilo_init();
    // register message callback
    kilo_message_rx = rx_message;
    // register message transmission calback
    kilo_message_tx = tx_message;
    // register tranmsission success callback
    kilo_message_tx_success = tx_message_success;
    
    kilo_start(setup, loop);

    return 0;
}
