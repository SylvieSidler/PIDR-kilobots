#include <kilolib.h>

void setup() {     
    // spinup motors
    spinup_motors();
    // move straight for 2 seconds (2000ms)
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);
    
    // spinup for 15ms to overcome friction
    spinup_motors();
    // turn left for 2 seconds (2000ms)
    set_motors(kilo_turn_left, 0);
    delay(2000);

    // spinup for 15ms to overcome friction
    spinup_motors();
    // turn right for 2 seconds (200 ms)
    set_motors(0, kilo_turn_right);
    delay(2000);
    set_motors(0,0);
}

void loop() {

}

int main() {
    kilo_init();
    kilo_start(setup, loop);

    return 0;
}