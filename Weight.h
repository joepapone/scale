/////////////////////////////////////////////////////////////////////////////////////
// Weight measurement
/////////////////////////////////////////////////////////////////////////////////////
//
//    FILE: Weight.h
//  AUTHOR: Jose Ferreira
// VERSION: 1.0.1
// PURPOSE: Library for weight measurement
//   NOTES:

// HX711 amplifier module library
#include <HX711.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = A1; // DT
const int LOADCELL_SCK_PIN = A0;  // SCK

// Initialize scale library
HX711 scale;

// Scale calibration structure
struct Scale_Cal {
   double y0;           // Default 0.0
   double y1;           // Default 1.0
   long x0;             // Default 0;
   long x1;             // Default 1;
   double slope() { return (y1-y0)/(x1-x0); }
   double bias;         // Default 0.0
   double alarm;        // Default 20.0
} cal;

// Scale measurement structure
struct Scale_Meas {
   long raw;
   double weight() { return raw*cal.slope() + cal.bias; }
   bool is_alarm() {
      // Activate alarm for weight equal or greater than threshold
      if (weight() >= cal.alarm) {
         return true;
      } else {
         return false;
      }
   }
} meas;

// Load default calibration parameters
void load_default() {
   // Load default values 
   cal.y0 = 0.0;
   cal.y1 = 1.0;
   cal.x0 = 0;
   cal.x1 = 1;
   cal.bias = 0.0;
   cal.alarm = 20.0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
}

// Configure
void weight_config() {
   // Start scale monitoring via assigned pins
   //scale.begin(dataPin, clockPin);
   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
   Serial.print("HX711:    ... ");
   Serial.println("active!");
   
   // Load default calibration parameters
   load_default();
}

// Get calculated weight value
void get_weight() {
   if (scale.wait_ready_timeout(1000)) {
      // Get raw average vaue
      meas.raw = scale.read_average(10);
   } else {
      Serial.println("HX711 not found.");
   }

   // Output results
   Serial.println();
   Serial.print("Raw: ");
   Serial.println(meas.raw);
   Serial.print("Weight: ");
   Serial.print(meas.weight());
   Serial.println(" kg");
   Serial.println();
}