#ifndef COLORS_H
#define COLORS_H

#include <kilombo.h>

// dictionnary of colors : 
uint8_t colours[] = {
    RGB(0,0,0),  // off
    RGB(3,0,0),  // red
    RGB(3,2,0),  // orange
    RGB(3,3,0),  // yellow
    RGB(2,3,0),  // yellowish green
    RGB(0,3,0),  // green
    RGB(0,3,3),  // cyan
    RGB(0,0,3),  // blue
    RGB(3,0,3),  // purple
    RGB(3,3,3)   // bright white
}; 

#endif // COLORS_H

typedef struct {
    uint8_t personalWord; // word to name the object
} USERDATA;
REGISTER_USERDATA(USERDATA);


void setup() {
    rand_seed(rand_hard(kilo_uid));
    mydata->personalWord = rand_soft() % 10; // Randomly assign a word from 0 to 9
    set_color(colours[mydata->personalWord]);
}

void loop() {
}



int main() {
    kilo_init();    
    kilo_start(setup, loop);
    
    return 0;
}

