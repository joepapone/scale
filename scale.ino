// Get raw measurment and calculate weight
# include "weight.h"
// Http web server for transmitting values
# include "webserver.h"

void setup() {
    Serial.begin(9600);
    Serial.println("CAETANO - GAS BOTTLE SCALE");
    weight_config();
    webserver_config();
}

void loop() {
    get_weight();
    send(raw, weight);
}