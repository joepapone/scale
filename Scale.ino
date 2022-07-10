/////////////////////////////////////////////////////////////////////////////////////
// Web scale application
/////////////////////////////////////////////////////////////////////////////////////
//
//    FILE: Scale.ino
//  AUTHOR: Jose Ferreira
// VERSION: 1.0.1
// PURPOSE: Web scale application
//   NOTES:

// Epoch 1970-01-01 00:00:00 equals 0 seconds
long int unix_time;
// Formated unix epoch time
String time_stamp;
// Store last weight reading
int last_reading;

// Declare libraries
#include <SPI.h>
#include "Weight.h"
#include "SDCard.h"
#include "RFID.h"
#include "HttpServer.h"
#include "NTPClient.h"

#define STATUS_PIN 10

// Millis timers
unsigned long ticker;
unsigned long clock_timer = 0;
unsigned long log_timer = 0;

// Millis intervals (milliseconds)
const long clock_interval = 30000;       // 30 seconds
const long log_interval = 60000;         // 60 seconds

void setup()
{
   // Initialize Universal Asynchronous Receiver/Transmitter (UART)
   Serial.begin(9600);
   Serial.println();
   Serial.println("{Arduino System}");
   Serial.println();
   Serial.println("[ INIT ]");
   
   // Initialize Serial Peripheral Interface (SPI)
   SPI.begin();

   // Initialize http} server
   http_server_config();

   // Initialize NTP client
   NTPtime_config();

   // Initialize SD Card
   SD_config();

   // Initialize RFID reader
   rfid_config();  

   // Initialize weight measurement, with default calibration
   weight_config();

   Serial.println();
   Serial.println("[ LOAD ]");
   // Load calibration data from SD Card
   get_calibtration();
   
   Serial.println();
   Serial.println("[ START ]");
   // Server IP address
   Serial.print("http://");
   Serial.println(Ethernet.localIP());
   // Get unix time from internet
   unix_time = get_NTPtime();
}

void loop()
{
   // The millis() function stores millisecond pulse raye since program started
   // An overflow occurs after approx. 50 days, with millis() being reset to 0
   ticker = millis();

   // Update unix time with elaped millis time every 30 seconds
   if (ticker - clock_timer >= clock_interval) {
      long elapsed = (ticker - clock_timer) / 1000;

      // Set new staring count
      clock_timer = ticker;

      // Increment unix time with elapsed seconds
      unix_time += elapsed;

      // Convert unix time into YYYY-MM-DD HH:MM:SS (UTC+01:00)
      time_stamp = convert_Unix(unix_time);
   }

   // Get RFID tag
   get_rfid();
   
   // Send data via http server
   http_server();
   
   // Get measured weight
   get_weight();

   // Get unix time and log every 1 hour (3600 seconds)
   if (ticker - log_timer >= log_interval) {
   
      // Set new staring count
      log_timer = ticker;

      // Update unix time with internet time
      unix_time = get_NTPtime();

      // Convert unix time into YYYY-MM-DD HH:MM:SS (UTC+01:00)
      time_stamp = convert_Unix(unix_time);

      // Define logging threshold limits
      float percent = 0.05; // Log new reading if value +/- 5% of the current reading
      int threshold = 100*percent; // Threshold upper and lower limits equal to +/- 0,5 kg

      // Log reading only if it exceeds upper or lower threshold limits
      if((meas.weight() >= last_reading + threshold || meas.weight() <= last_reading - threshold)) {
         // Log data
         log_data();

         // Update last reading with new value
         last_reading = meas.weight();
      }
   }
}