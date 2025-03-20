#include <kilolib.h>

uint8_t new_message = 0;

void setup() { }

void loop() {
    if (new_message) {
        new_message = 0;
        set_color(RGB(1,1,1));
        delay(100);
        set_color(RGB(0,0,0));
    }
}

void message_rx(message_t *m, distance_measurement_t *d) {
    new_message = 1;
}

int main() {
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_start(setup, loop);

    return 0;
}
