// Internet conectivity
#include <Ethernet.h>
#include <SPI.h>

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
}

void send(float raw, float weight) {
   // Listen for incoming clients
   EthernetClient client = server.available();
   if (client) {
      Serial.println("New client");
      // An http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
         if (client.available()) {
            char c = client.read();
            Serial.write(c);
            // Send reply when http request has ended, with reception of
            // newline character and blank line.
            if (c == '\n' && currentLineIsBlank) {
               // Send a standard http response header
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
               break;
            }
            if (c == '\n') {
               // Starting new line
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
   }
}