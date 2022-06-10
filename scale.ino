// Gas bottle scale and webserver
#include "webserver.h"

void setup() {
    Serial.begin(9600);
    Serial.println("CAETANO - GAS BOTTLE SCALE");
    // Initialize webserver
    webserver_config();
}

void loop() {
    // Send data via webserver
    webserver();
}