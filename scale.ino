// code for calibration
#include "HX711.h"

// Initialize scale library
HX711 scale;

uint8_t dataPin = A1; // DT
uint8_t clockPin = A0; // CLK

float y1 = 20;
float y0 = 0;
float x1 = -300000;
float x0 = -117000;
float weight;
float raw;
float cal = (y1-y0)/(x1-x0);
float offset = -12.78;

void setup() {
   Serial.begin(9600);
   Serial.println("CAETANO - GAS BOTTLE SCALE");
   Serial.print("LIBRARY VERSION: ");
   Serial.println(HX711_LIB_VERSION);
   Serial.println();
}

void loop() {
   for(int i=0; i<10; i++) raw =+ scale.get_units(), 10; // Get 10 raw measurements
   raw / 10; // Get average raw valve by dividing by 10
   weight = raw*cal + offset; // Calculate weight

   Serial.print(" Raw: "); 
   Serial.print(raw);
   Serial.println();

   Serial.print(" Weight: "); 
   Serial.print(weight);
   Serial.print(" kg");  
   Serial.println();
}