#include <kilolib.h>

message_t transmit_msg;
uint8_t message_sent = 0;


message_t *message_tx() {
    return &transmit_msg;
} 
void message_tx_success() {
    message_sent = 1;
}

void setup() {
    transmit_msg.type = NORMAL;
    transmit_msg.crc = message_crc(&transmit_msg);
}

void loop() {
    if (message_sent) {
        message_sent = 0;
        set_color(RGB(1,1,1));
        delay(20);
        set_color(RGB(0,0,0));
    }
}

int main() {
    kilo_init();
    kilo_message_tx = message_tx;
    kilo_message_tx_success = message_tx_success;
    kilo_start(setup, loop);

    return 0;
}
