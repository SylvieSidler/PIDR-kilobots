#ifndef COLORS_H
#define COLORS_H

#include <kilombo.h>

// dictionnary of colors : 
uint8_t colours[] = {
    RGB(0,0,0),  // off
    RGB(2,0,0),  // red
    RGB(2,1,0),  // orange
    RGB(2,2,0),  // yellow
    RGB(1,2,0),  // yellowish green
    RGB(0,2,0),  // green
    RGB(0,1,1),  // cyan
    RGB(0,0,2),  // blue
    RGB(1,0,1),  // purple
    RGB(3,3,3)   // bright white
}; 

#endif // COLORS_H

typedef struct {
    uint8_t personalWord; // word to name the object
} USERDATA;
REGISTER_USERDATA(USERDATA);


void setup() { 
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

