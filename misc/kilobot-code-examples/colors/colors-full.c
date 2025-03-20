
#include <kilombo.h>
#ifdef SIMULATOR
#include <stdio.h> // for printf
#else
#include <avr/io.h>  // for microcontroller register defs
//  #define DEBUG          // for printf to serial port
//  #include "debug.h"
#endif


typedef struct {
    uint16_t id;
    uint8_t state;
} received_message_t;

#ifdef SIMULATOR
#define PRINT_MSG(m) printf("(%d, %d)", m.id, m.state)
#endif


#define RB_SIZE 32 // ring buffer size 

#ifdef SIMULATOR
#define LOGMSG(msg) printf("%d,%d:%s", kilo_ticks, kilo_uid, msg)
#endif

typedef struct 
{
    uint8_t state; // my current color
    
    uint8_t message_sent; // flag for tx success
    uint8_t message_lock; // lock before sending
    message_t transmit_msg; // message struct

    int8_t RXHead, RXTail; // message ring buffer
    received_message_t RXBuffer[RB_SIZE]; 
    uint8_t seen[RB_SIZE];


    
    uint32_t timer1; // timer
} USERDATA;

#define TM1_DELAY 64 //
#define TM1_up() (kilo_ticks > mydata->timer1 + TM1_DELAY)
#define TM1_reset() (mydata->timer1 = kilo_ticks)


REGISTER_USERDATA(USERDATA)

// Ring buffer operations. Taken from kilolib's ringbuffer.h
// but adapted for use with mydata->

// Ring buffer operations indexed with head, tail
// These waste one entry in the buffer, but are interrupt safe:
//   * head is changed only in popfront
//   * tail is changed only in pushback
//   * RB_popfront() is to be called AFTER the data in RB_front()
//     has been used
//   * head and tail indices are uint8_t, which can be updated atomically
//     - still, the updates need to be atomic, especially in RB_popfront()

#define RB_init() {	\
    mydata->RXHead = 0; \
    mydata->RXTail = 0;\
}

#define RB_size() (mydata->RXTail - mydata->RXHead)

#define RB_empty() (mydata->RXHead == mydata->RXTail)

#define RB_full()  ((mydata->RXHead+1)%RB_SIZE == mydata->RXTail)

#define RB_front() mydata->RXBuffer[mydata->RXHead]

#define RB_back() mydata->RXBuffer[mydata->RXTail]

#define RB_popfront() mydata->RXHead = (mydata->RXHead+1)%RB_SIZE;

#define RB_pushback() {\
    mydata->RXTail = (mydata->RXTail+1)%RB_SIZE;\
    if (RB_empty())\
      { mydata->RXHead = (mydata->RXHead+1)%RB_SIZE; \
	 } \
  }
#ifdef SIMULATOR
#define RB_print() { int i; \
        for(i=mydata->RXHead; i<mydata->RXTail; i++) {  \
        printf("[%d]", i);                               \
        PRINT_MSG(mydata->RXBuffer[i]);                 \
        }\
    }
#endif

// Returns a pointer to my message buffer 
message_t *message_tx() {
    if (mydata->message_lock == 1)
        return 0;
    return &mydata->transmit_msg;
}

// called when sending was succesfull
void message_tx_success(){
    mydata->message_sent = 1;
#ifdef SIMULATOR
    LOGMSG("TX:");
    printf("state %d\n", mydata->state );
#endif
}



void message_rx(message_t *msg, distance_measurement_t *dist){

    uint16_t id = msg->data[0] | (msg->data[1] << 8);
    
#ifdef SIMULATOR
    LOGMSG("RX:");
    printf("from %d-%d ", id, msg->data[2]);
#endif
        
    if (mydata->seen[id]==0) {
        mydata->seen[id] = 1;
        received_message_t *rmsg = &RB_back();
        rmsg->id = id;
        rmsg->state = msg->data[2];
        RB_pushback();
#ifdef SIMULATOR
        printf("added\n");
#endif   
    }

    
#ifdef SIMULATOR
    else{
        printf("discarded\n");
    }
#endif


}

// make a message to send
void set_up_message(uint8_t data) {
    mydata->message_lock = 1;
    // The type is always NORMAL.
    mydata->transmit_msg.type = NORMAL;
    // The sender_id
    mydata->transmit_msg.data[0] = kilo_uid & 0xff; //0 low  ID;
    mydata->transmit_msg.data[1] = kilo_uid >> 8;   //1 high ID
    // Some data.
    mydata->transmit_msg.data[2] = data;
    // It's important that the CRC is computed after the data has
    // been set; otherwise it would be wrong and the message would
    // be dropped by the receiver.
    mydata->transmit_msg.crc = message_crc(&mydata->transmit_msg);
    mydata->message_lock = 0;
}

void pick_random_state(){


    if (RB_size() > 0 ){
        //uint8_t r = rand_soft();
        uint8_t i = rand_soft()%RB_size();
        
        uint8_t new_state = mydata->RXBuffer[i].state;
        
#ifdef SIMULATOR
        LOGMSG("Update:");
        printf("%d -> %d (id=%d)\n",
               mydata->state, new_state,i);
#endif
        
        mydata->state = new_state;
        uint8_t color = mydata->state%63 + 1;
        uint8_t r     = color/16;
        uint8_t rem1  = color%16;
        uint8_t g     = rem1/4;
        uint8_t b     = rem1%4;
        set_color(RGB(r,g,b));
                
        set_up_message(mydata->state);
        
        RB_init();
        for (i=0; i<RB_SIZE; i++)
            mydata->seen[i] = 0;
    }

    
}


void setup()
{
    uint8_t i=0;
    // This is run once before entering the loop.

    // set random seed
    rand_seed(rand_hard()); //seed the random number generator

    // initial color
    mydata->state = rand_soft(); 
    set_color(mydata->state);
    set_up_message(mydata->state);

    //initialize ring buffer & the seen list
    RB_init();
    
    for (i=0; i<RB_SIZE; i++)
        mydata->seen[i] = 0;
            
    //setup timers 
    TM1_reset();

#ifdef SIMULATOR
    LOGMSG("Init:");
    printf("state %d\n", mydata->state);
#endif
    
    
}

void loop() {
    // Put the main code here. This is run repeatedly.

    
#ifdef SIMULATOR
    LOGMSG("Buffer ");
    printf("(%d):", RB_size());
    RB_print();
    printf("\n");
    LOGMSG("Seen:");
    printf("%d",mydata->seen[0]);
    for (int i=1; i<RB_SIZE; i++)
        printf(" %d", mydata->seen[i]);
    printf("\n");
    
#endif

        
    if ( TM1_up() )  {
        TM1_reset();
        pick_random_state();
    }
    

}

int main()
{
    // Initialize the hardware.
    kilo_init();

    

    // Register the message_rx callback function.
    kilo_message_rx = message_rx; 
    
    // Register the message_tx callback function.
    kilo_message_tx = message_tx;
    // Register the message_tx_success callback function.
    kilo_message_tx_success = message_tx_success;
    

    
    // Register the program.
    kilo_start(setup, loop);
    
    return 0;
}
