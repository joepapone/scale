// Gas bottle scale with webserver
#include "weight.h"
#include "webserver.h"

void setup() {
    Serial.begin(9600);
    Serial.println("CAETANO - GAS BOTTLE SCALE");
    // Initialize weight measurement
    weight_config();
    // Initialize webserver
    webserver_config();
}

void loop() {
    // Get raw measurment and calibrated weight
    get_weight();
    // Send data via webserver
    send(raw, weight);
    //webserver();
}