// Internet conectivity
#include <Ethernet.h>
#include <SPI.h>
//#include "weight.h"

// Assign a MAC address for the Ethernet controller.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Assign an IP address for the controller:
IPAddress ip(192, 168, 1, 20);

// Initialize the Ethernet server library
// (port 80 is default for HTTP):
EthernetServer server(80);

void webserver_config() {

   // Use Ethernet.init(pin) to configure the SS/CS pin
   Ethernet.init(8); // MKR ETH shield

   // Start the SPI library:
   SPI.begin();

   // Start the Ethernet connection
   Ethernet.begin(mac, ip);

   // Check for Ethernet hardware present
   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
   while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
   }
   }
   if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
   }

   // Start listening for clients
   server.begin();
   Serial.print("Server available at http:// ");
   Serial.println(Ethernet.localIP());

   // Initialize weight measurement
   //weight_config();

   // Relay config
   pinMode(LED_BUILTIN, OUTPUT);
}

void http_get (EthernetClient client, float raw, float weight) {
   // Send standard http response header
   client.println("HTTP/1.1 200 OK\r\n" 
                  "Content-Type: text/html\r\n"
                  "Connection: close\r\n" // Connection will be closed after response completion
                  "Refresh: 5\r\n" // Refresh page automatically every 5 sec
                  "\r\n"); // Blank line to split HTTP header and HTTP body
   // Send web page
   client.println("<!doctype html>\n"
                  "<html lang='en'>\n"
                  "<head>\n"
                  "<meta charset='utf-8'>\n"
                  "<meta name='viewport' content='width=device-width'>\n"
                  "<title>Smart home controller</title>\n"
                  "</head>\n"
                  "<body style='font-family:Helvetica, sans-serif'>\n");
   // Send data
   client.print("<h1 style='border-bottom: 3px solid red;'>Scale:</>"); 
   client.print("<h2 style='color:#2F4F4F;' id='raw'>Raw: "); 
   client.print(raw);
   client.print("</h2>");
   client.print("<h2 style='color:#2F4F4F;' id='weight'>Weight: "); 
   client.print(weight);
   client.print(" kg</h2>");
   // Calibration form
   client.print("<form action='' method='post' enctype=text/plain' name='config'>"
                "<p style='color:#8B0000;'><b>Please select calibration option:</b></p>"
                "<input type='radio' id='zero' name='cal' value='zero'>"
                "<label for='zero'>Zero</label><br>"
                "<input type='radio' id='span' name='cal' value='span'>"
                "<label for='span'>Span</label><br>"
                "<input type='radio' id='offset' name='cal' value='offset'>"
                "<label for='offset'>Offset</label><br><br>"
                "<input name='cal' type='submit' value='Submit'>\n"
                "<p style='color:#8B0000;'><b>Gate operations:</b></p>\n"
                "<input name='gate' type='submit' value='open'>\n"
                "<input name='gate' type='submit' value='close'>\n"
                "</form>\n");
   client.print("</body>\n</html>");
   Serial.println("Response sent");
}

void http_post(EthernetClient client) {
   char str[100];
   int i = 0;
   // Read http post data
   while(client.available())
   {
      str[i]= client.read();
      ++i;
   }
   Serial.println(str);
   // Scale calibration
   if (String(str).indexOf("cal=zero") != -1) {
      Serial.println("Data received:");
      Serial.println(str);
      Serial.print("Zero calibration = ");
      //Serial.println(get_raw());
   }
   else if (String(str).indexOf("cal=span") != -1) {
      Serial.println("Data received:");
      Serial.println(str);
      Serial.print("Span calibration = ");
      //Serial.println(get_raw());
   }
   else if (String(str).indexOf("cal=offset") != -1) {
      Serial.println("Data received:");
      Serial.println(str);
      Serial.print("Offset calibration = ");
      //Serial.println(get_raw());
   }
   // Gate control
   else if (String(str).indexOf("gate=close") != -1) {
      Serial.println("Data received:");
      Serial.println(str);
      Serial.println("Close gate");
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println(digitalRead(LED_BUILTIN));
   }
   else if(String(str).indexOf("gate=open") != -1) {
      Serial.println("Data received:");
      Serial.println(str);
      Serial.println("Open gate");
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println(digitalRead(LED_BUILTIN));
   }
}

void send(float raw, float weight) {
   // Listen for incoming clients
   EthernetClient client = server.available();
   if (client) {
      Serial.println();
      Serial.println("Client");
      // An http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
         if (client.available()) {
            char c = client.read();
            Serial.print(c);
            // Check if http request has ended, with new line character and blank line.
            if (c == '\n' && currentLineIsBlank) {
               // Send http GET response.
               http_get(client, raw, weight);
               // Read http POST data.
               http_post(client);
               break;
            }
            if (c == '\n') {
               // Starting new line.
               currentLineIsBlank = true;
            } else if (c != '\r') {
               // Character on the current line
               currentLineIsBlank = false;
            }
         }
      }
      // Give the web browser time to receive the data
      delay(1);
      // Close the connection:
      client.stop();
      Serial.println("client disconnected");
      Serial.println();
   }
}

/*Send a standard http response header
   client.println("HTTP/1.1 200 OK");
   client.println("Content-Type: text/html");
   client.println("Connection: close");  // Connection will be closed after response completion
   client.println("Refresh: 5");  // Refresh page automatically every 5 sec
   client.println();
   client.println("<!DOCTYPE HTML>");
   client.println("<html>");
   // Get scale measurements
   client.print("<h1 id='raw'>Raw: "); 
   client.print(raw);
   client.print("</h1>");
   client.print("<h1 id='weight'>Weight: "); 
   client.print(weight);
   client.print(" kg</h1>");
   client.println("</html>");
   Serial.write(client.read());*/