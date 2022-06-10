// Scale amplifier and encoder (HX711)
#include "HX711.h"

// Initialize scale library
HX711 scale;

uint8_t dataPin = A1; // DT
uint8_t clockPin = A0; // CLK

// Scale calibration
struct Calibration {
  float x0;
  float x1;
  float y0;
  float y1;
  float slope;  
  float bias;
};

// Scale measured values
struct Values {
  float raw;
  float weight;
};

void weight_config() {
  Serial.print("HX711 LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  // Start scale monitoring via assigned pins
  scale.begin(dataPin, clockPin);
}

Calibration get_calibration() {
  Calibration cal;
  cal.x0 = -300000;
  cal.x1 = -117000;
  cal.y0 = 0;
  cal.y1 = 20;
  cal.slope = (cal.y1-cal.y0)/(cal.x1-cal.x0);
  cal.bias = -12.78;

  return cal;
}

float get_raw() {
  float raw;
  // Get 10 raw values
  for(int i=0; i<10; i++) raw =+ scale.get_units(), 10;
  return raw/10; // Return raw value average
}

Values get_weight() {
  Calibration cal = get_calibration();
  Values value;
  // Get raw value
  value.raw = get_raw();
  // Calculate weight
  value.weight = value.raw*cal.slope + cal.bias;
  // View result
  Serial.print("Raw: "); 
  Serial.print(value.raw);
  Serial.println();
  Serial.print("Weight: "); 
  Serial.print(value.weight);
  Serial.print(" kg");  
  Serial.println();

  return value;
}

/*void get_weight() {
  Calibration cal = load_calibration();
  Values value;

  // Get raw measurement
  for(int i=0; i<10; i++) value.raw =+ scale.get_units(), 10;
  value.raw = get_raw();
  // Calculate weight
  value.weight = value.raw*cal.slope + cal.bias;
  // View result
  Serial.print("Raw: "); 
  Serial.print(value.raw);
  Serial.println();
  Serial.print("Weight: "); 
  Serial.print(value.weight);
  Serial.print(" kg");  
  Serial.println();
}*/