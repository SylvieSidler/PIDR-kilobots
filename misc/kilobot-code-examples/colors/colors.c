
#include <kilombo.h>
#ifdef SIMULATOR
#include <stdio.h> // for printf
#else
#include <avr/io.h>  // for microcontroller register defs
//  #define DEBUG          // for printf to serial port
//  #include "debug.h"
#endif

#define UPDATE_DELAY 64 
#define TX_DELAY 16
#define LIST_SIZE 32 


typedef struct 
{
    uint8_t state; // my current color
    uint8_t list_size;
    uint32_t tx_timer;
    uint32_t up_timer;
    message_t transmit_msg; 
    uint8_t list[LIST_SIZE];
   

} USERDATA;

REGISTER_USERDATA(USERDATA)


// Returns a pointer to my message buffer 
message_t *message_tx() {
    if (kilo_ticks <= mydata->tx_timer + TX_DELAY)
        return 0;
    mydata->tx_timer = kilo_ticks;

    mydata->transmit_msg.type = NORMAL;
    mydata->transmit_msg.data[0] = mydata->state;
    mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);
    return &mydata->transmit_msg;
}

// called when sending was succesfull
void message_tx_success(){ }

void message_rx(message_t *msg, distance_measurement_t *dist){
    uint8_t data = msg->data[0];
    uint8_t cur_distance = estimate_distance(dist);
    if( cur_distance > 100 ) {
        return;
    }
    
    uint8_t found = 0;
    for (int i=0; i<mydata->list_size; i++){
        if(mydata->list[i] == data){
            found=1;
            break;
        }
    }
    
    if(found==0){
        mydata->list[mydata->list_size] = data;
        mydata->list_size++;
    }    
}

void update_color(){
    uint8_t color = mydata->state%63 + 1;
    uint8_t r     = color/16;
    uint8_t rem1  = color%16;
    uint8_t g     = rem1/4;
    uint8_t b     = rem1%4;
    set_color(RGB(r,g,b));
}


void update_state(){
    if(mydata->list_size > 0){
        uint8_t i = rand_soft()%mydata->list_size; 
        mydata->state = mydata->list[i];
        update_color();
        mydata->list_size=0;
    }
}


void setup(){
    rand_seed(rand_hard()); //seed the random number generator

    mydata->tx_timer = rand_soft()%64;
    mydata->up_timer = rand_soft()%64;
    mydata->list_size = 0;
      
    // initial state & color
    mydata->state = rand_soft();
    update_color();
}

void loop() {
    // Put the main code here. This is run repeatedly.
        
    if ( kilo_ticks > mydata->up_timer + UPDATE_DELAY )  {
        mydata->up_timer = kilo_ticks;
        update_state();
    }

#ifdef SIMULATOR
    printf("%d,%d: %d (%d)", kilo_ticks, kilo_uid,
           mydata->state,mydata->list_size);
    if(mydata->list_size > 0){
        printf("[%d", mydata->list[0]);
        for (int i=1; i<mydata->list_size; i++)
            printf(" %d", mydata->list[i]);
        printf("]");
    }
    printf("\n");
#endif
}

int main() {
    // Initialize the hardware
    kilo_init();

    // Register the TX/RX callback functions
    kilo_message_rx = message_rx; 
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_success;
        
    // Register the program.
    kilo_start(setup, loop);
    
    return 0;
}
