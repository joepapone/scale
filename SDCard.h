/////////////////////////////////////////////////////////////////////////////////////
// SD card parameter and logger storage
/////////////////////////////////////////////////////////////////////////////////////
//
//    FILE: Weight.h
//  AUTHOR: Jose Ferreira
// VERSION: 1.0.1
// PURPOSE: Library for SD card connectivity
//   NOTES: Text files on ETH shield SD Card slot

// SD Card conectivity
#include <SD.h>

#define SD_SS_PIN 4
#define RFID_SS_PIN 7
#define ETH_SS_PIN 8

// SD card file
File file;
// Event log authorized UID
String authorized_UID_event;

// Enable SPI communication with SD Card
void enable_SD_Card() {
   digitalWrite(SD_SS_PIN,LOW);
   digitalWrite(ETH_SS_PIN,HIGH);
   digitalWrite(RFID_SS_PIN,HIGH);
}

void SD_config() {
   // Enable SD Card
   enable_SD_Card();

   Serial.print("SD card:  ... ");
   // Start SD card
   if (!SD.begin(SD_SS_PIN)) {
      Serial.println("failure!");
      while (1);
   }
   Serial.println("active!");
}

// Check if RFID exists in file with authorized UID list
bool is_authorized_UID(byte rfid_UID[4], byte rfid_size) {
   // Enable SD Card for read and write
   enable_SD_Card();

   // Set file path and name
   String file_path = "UID.TXT";

   // Initialize UID verification
   byte authorizedUID[4] = {0x00, 0x00, 0x00, 0x00};
   bool authorized = false;

   // Open file for reading
   file = SD.open(file_path, FILE_READ);
   if (file) {
      Serial.print("Reading ");
      Serial.print(String(file_path));
      Serial.println(" ...");
      // Read data from begining to end of file
      while (file.available()) {
         // The \n character is discarded from buffer
         String line = file.readStringUntil('\n');

         // Get UID hex values starting from equals sign to end of line
         // Format UID=6053521B to 0x60, 0x53, 0x52, 0x1B
         if (line.startsWith("UID=")) {
            // Extract 8 digit hex string
            int start_index = line.indexOf("=");
            String UID = line.substring(start_index + 1);
            char charBuf[9];
            char charHex[4];
            UID.toCharArray(charBuf, 9);

            // Create hex string
            for (int i = 0; i < 4; i++) {
               charHex[0] = '0';
               charHex[1] = 'x';
               charHex[2] = charBuf[i*2];
               charHex[3] = charBuf[(i*2)+1];
               
               // Convert hex string to integer, example 0x60 -> 96
               int val = (int)strtol(charHex,NULL,16);
               authorizedUID[i] = val;
            }
         }

         // Check for authorized RFID
         if (rfid_UID[0] == authorizedUID[0] &&
            rfid_UID[1] == authorizedUID[1] &&
            rfid_UID[2] == authorizedUID[2] &&
            rfid_UID[3] == authorizedUID[3] ) {
            // Confirm authorized UID to unlock
            authorized = true;
            // Event log authorized UID
            authorized_UID_event = line;
            break;
         } else {
            authorized = false;
         }
      }

      // Report identification result
      if (authorized) {
         Serial.print("Authorized UID: ");
         for (int i = 0;i < rfid_size; i++) {
            Serial.print(rfid_UID[i], HEX);
            Serial.print(" ");
         }
         Serial.println();
      } else {
         Serial.print("Unauthorized UID: ");
         for (int i = 0;i < rfid_size; i++) {
            Serial.print(rfid_UID[i], HEX);
            Serial.print(" ");
         }
         Serial.println();      
      }

      // Close file:
      file.close();
      Serial.println("Read file complete!");
   } else {
      // If file didn't open, print an error:
      Serial.println("Error opening UID.TXT file!");
   }

   return authorized;
}

// Get scale calibration from file
void get_calibtration() {
   // Enable SD Card for read and write
   enable_SD_Card();

   // Set file path and name
   String file_path = "CAL.TXT";

   // Open file for reading
   file = SD.open(file_path, FILE_READ);
   if (file) {
      Serial.print("Reading ");
      Serial.print(String(file_path));
      Serial.println(" ...");
      while (file.available()) {
         // The \n character is discarded from buffer
         String line = file.readStringUntil('\n');

         // Get parameter value after equals sign until end of line
         int start_index = line.indexOf("=") + 1;
         String data = line.substring(start_index);

         // Load calibration parameters
         if (line.startsWith("y1")) cal.y1 = data.toDouble();
         else if (line.startsWith("y0")) cal.y0 = data.toDouble();
         else if (line.startsWith("x1")) cal.x1 = data.toInt();
         else if (line.startsWith("x0")) cal.x0 = data.toInt();
         else if (line.startsWith("bias")) cal.bias = data.toDouble();
         else if (line.startsWith("alarm")) cal.alarm = data.toDouble();
         Serial.print(line.substring(0,start_index));
         Serial.println(line.substring(start_index));
      }
      // Close file:
      file.close();
      Serial.println("Read complete!");
   } else {
      // If file didn't open, print an error:
      Serial.println("Error opening CAL.TXT file!");
   }
}

// Save calibration to file
void save_calibration() {
   // Enable SD Card for read and write
   enable_SD_Card();

   // Set file path and name
   String file_path = "CAL.TXT";

   // Delete existing file
   if (SD.exists(file_path)) {
      SD.remove(file_path);
      Serial.print("File ");
      Serial.print(file_path);
      Serial.println(" deleted!");
   }

   // Write new calibration data
   file = SD.open(file_path, FILE_WRITE);
   if (file) {
      Serial.print("Writing to ");
      Serial.print(String(file_path));
      Serial.println(" ...");

      // Seek char=/ (dec=47) to set position to begining of file
      file.seek(47);
      // Write parameters to file
      file.println("// Calibration data");
      file.println(time_stamp);
      file.print("y1=");
      file.println(cal.y1);
      file.print("y0=");
      file.println(cal.y0);
      file.print("x1=");
      file.println(cal.x1);
      file.print("x0=");
      file.println(cal.x0);
      file.print("bias=");
      file.println(cal.bias);
      file.print("alarm=");
      file.println(cal.alarm);

      // Close file:
      file.close();
      Serial.println("Write complete!");
   } else {
      // If file didn't open, print an error:
      Serial.println("Error opening CAL.TXT file!");
   }
}

// Log measured weight and calibration parameters
void log_data() {
   // Enable SD Card for read and write
   enable_SD_Card();

   // Set file path and name
   String file_path = "LOG.TXT";

   // Open file for writing
   file = SD.open(file_path, FILE_WRITE);
   if (file) {
      Serial.print("Writing to ");
      Serial.print(String(file_path));
      Serial.println(" ...");

      // Set column headers for new file
      if (file.size() < 2) {
         file.print("Date");
         file.print(",");
         file.print("Raw");
         file.print(",");
         file.print("Weight");
         file.print(",");
         file.print("slope");
         file.print(",");
         file.println("bias");
      }

      // Write comma seperated values to text file
      file.print(time_stamp);
      file.print(",");
      file.print(meas.raw);
      file.print(",");
      file.print(meas.weight());
      file.print(",");
      file.print(cal.slope(),8);
      file.print(",");
      file.println(cal.bias);

      // Close file:
      file.close();
      Serial.println("Write complete!");
   } else {
      // If file didn't open, print an error:
      Serial.println("Error opening LOG.TXT file!");
   }
}

// Log event
void log_event(String description, int code) {
   // Enable SD Card for read and write
   enable_SD_Card();

   // Set file path and name
   String file_path = "EVENT.TXT";

   // Open file for writing
   file = SD.open(file_path, FILE_WRITE);
   if (file) {
      Serial.print("Writing to ");
      Serial.print(String(file_path));
      Serial.println(" ...");

      // Set column headers for new file
      if (file.size() < 2) {
         file.print("Date");
         file.print(",");
         file.print("Description");
         file.print(",");
         file.println("Code");
      }

      // Write comma seperated values to text file
      file.print(time_stamp);
      file.print(",");
      file.print(description);
      file.print(",");
      file.println(code);

      // Close file:
      file.close();
      Serial.println("Write complete!");
   } else {
      // If file didn't open, print an error:
      Serial.println("Error opening EVENT.TXT file!");
   }
}
