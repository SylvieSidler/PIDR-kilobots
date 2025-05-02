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
    switch (kilo_uid%10) {
        case 0:
            mydata->personalWord = 0; // off
            break;
        case 1:
            mydata->personalWord = 1; // red
            break;
        case 2:
            mydata->personalWord = 2; // orange 
            break;
        case 3:
            mydata->personalWord = 3; // yellow
            break;
        case 4:
            mydata->personalWord = 4; // yellowish green
            break;
        case 5:
            mydata->personalWord = 5; // green
            break;
        case 6:
            mydata->personalWord = 6; // cyan
            break;
        case 7:
            mydata->personalWord = 7; // blue
            break;
        case 8:
            mydata->personalWord = 8; // purple
            break;
        case 9:
            mydata->personalWord = 9; // bright white
            break;
    }
    set_color(colours[mydata->personalWord]);
}

void loop() {
}



int main() {
    kilo_init();    
    kilo_start(setup, loop);
    
    return 0;
}

