/////////////////////////////////////////////////////////////////////////////////////
// RFID/NFC reader
/////////////////////////////////////////////////////////////////////////////////////
//
//    FILE: RFID.h
//  AUTHOR: Jose Ferreira
// VERSION: 1.0.1
// PURPOSE: Library for reading RFID/NFC cards
//   NOTES:

#include <MFRC522.h>

#define RST_PIN     9
#define RFID_SS_PIN 8
#define RELAY_PIN   2

MFRC522 rfid(RFID_SS_PIN, RST_PIN);

// Key:   60 53 52 1B -> 0x60, 0x53, 0x52, 0x1B
// Card:  95 BA 0B AD -> 0x95, 0xBA, 0x0B, 0xAD
// Phone: 08 93 E7 ED -> 0x08, 0x93, 0xE7, 0xED

// Enable SPI communication with RFID
void enable_RFID() {
   digitalWrite(SD_SS_PIN,HIGH);
   digitalWrite(ETH_SS_PIN,HIGH);
   digitalWrite(RFID_SS_PIN,LOW);
}

// Config and initialize RFID card reader
void rfid_config() {
   // Enable RFID
   enable_RFID();

   // Start MFRC522 RFID reader
   rfid.PCD_Init();
   Serial.print("RFID:     ... ");
   Serial.println("active!");

   // Set pin output to relay
   pinMode(RELAY_PIN, OUTPUT);
   // Set relay deactivated
   digitalWrite(RELAY_PIN, LOW);
}

// Read RFID
void get_rfid() {
   // Enable RFID
   enable_RFID();

   // Chech for RFID/NFC card 
   if (rfid.PICC_IsNewCardPresent()) {
      // Read NUID
      if (rfid.PICC_ReadCardSerial()) {
         MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

         // Halt PICC
         rfid.PICC_HaltA();
         // Stop PCD encryption
         rfid.PCD_StopCrypto1();

         // Chech if authorized UID
         bool authorized = is_authorized_UID(rfid.uid.uidByte, rfid.uid.size);

         // Unlock gate for authorized UID
         if (authorized) {
            // Activate relay for 5 seconds
            digitalWrite(RELAY_PIN, HIGH);
            delay(2000);
            digitalWrite(RELAY_PIN, LOW);
            // Event log authorized UID
            log_event("Unlock via: " + authorized_UID_event, 202);  
         }
      }
   }
}
