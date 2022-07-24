/////////////////////////////////////////////////////////////////////////////////////
// Http Web server
/////////////////////////////////////////////////////////////////////////////////////
//
//    FILE: WebServer.h
//  AUTHOR: Jose Ferreira
// VERSION: 1.0.1
// PURPOSE: Library for http web server
//   NOTES:

// Internet conectivity
#include <Ethernet.h>

#define ETH_SS_PIN 7
#define RELAY_PIN 2

// Assign a MAC address for the Ethernet controller.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Assign an IP address for the controller:
IPAddress ip(192, 168, 1, 20);

// Initialize the Ethernet server library
// (port 80 is default for HTTP):
EthernetServer server(80);

// Enable SPI communication with ETH
void enable_ETH() {
   digitalWrite(SD_SS_PIN,HIGH);
   digitalWrite(ETH_SS_PIN,LOW);
   digitalWrite(RFID_SS_PIN,HIGH);
}

// Config and initialize http server
void http_server_config() {
   // Enable ETH
   enable_ETH();

   // Use Ethernet.init(pin) to configure the SS/CS pin
   Ethernet.init(ETH_SS_PIN); // MKR ETH shield

   // Start Ethernet connection
   Ethernet.begin(mac, ip);

   // Check for Ethernet hardware present
   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield not found. :(");
   while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
   }
   }
   if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable not connected.");
   }

   // Start listening for clients
   server.begin();
   Serial.print("Server:   ... ");
   Serial.println("active!");
   
   // Give Ethernet shield time to initialize
	delay(2000);
}

// HTML CSS styling
String html_style() {
   return "<style>\n"
          "body {\n"
          "font-family: Arial, Helvetica, sans-serif;\n"
          "margin: 0;\n"
          "}\n"
          ".navbar {\n"
          "overflow: hidden;\n"
          "background-color: #333333;\n"
          "}\n"
          ".navbar h1 {\n"
          "float: left;\n"
          "display: block;\n"
          "color: white;\n"
          "padding: 0px 20px;\n"
          "}\n"
          ".navbar a {\n"
          "float: right;\n"
          "padding: 30px;\n"
          "text-decoration: none;\n"
          "color: white;\n"
          "}\n"
          ".navbar a:hover {\n"
          "background-color: #4d4d4d;\n"
          "}\n"
          "h2 {\n"
          "color: #2F4F4F;\n"
          "padding: 0px 20px;\n"
          "}\n"
          "h3 {\n"
          "color: #8B0000;\n"
          "padding: 0px 20px;\n"
          "}\n"
          "div.indent {\n"
          "margin: 5px 25px;\n"
          "}\n"
          "div {\n"
          "margin: 5px;\n"
          "}\n"
          ".data {\n"
          "display: inline-block;\n"
          "}\n"
          "label {\n"
          "display: inline-block;\n"
          "width: 60px;\n"
          "margin: 0px 25px 0px 25px;\n"
          "color: #2F4F4F;\n"
          "}\n"
          "input {\n"
          "width: 100px;\n"
          "padding: 5px;;\n"
          "margin: 0px 10px 0px 10px;\n"
          "}\n"
          "@keyframes blinking {\n"
          "0% {background-color: #dcdcdc;}\n"
          "100% {background-color: #ff0000;}\n"
          "}\n"
          "#blink {\n"
          "padding: 10px;\nwidth: 200px;\ncolor: #FFFFFF;\ntext-align: center;\n"
          "animation: blinking 3s infinite;\n"
          "}\n"
          ".btn {\n"
          "background-color: #D7DBDD;\n"
          "border: none;\v"
          "border-radius: 5px;\n"
          "color: black;\n"
          "padding: 10px;\n"
          "width: 120px;\n"
          "text-align: center;\n"
          "text-decoration: none;\n"
          "display: inline-block;\n"
          "font-size: 14px;\n"
          "margin: 2px;\n"
          "cursor: pointer;\n"
          "}\n"
          ".btn, a{\n"
          "padding: 10px 0px;\n"
          "}\n"
          "ol {\n"
          "list-style-type: upper-roman;\n"
          "}\n"
          "li {\n"
          "margin: 10px;\n"
          "}\n"
          "</style>\n";
}

// HTML head
void html_head(EthernetClient client) {
   client.println("<!DOCTYPE html>\n"
                  "<html lang='en'>\n"
                  "<head>\n"
                  "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>\n"
                  "<meta name='viewport' content='width=device-width'>\n"
                  "<title>Smart home controller</title>\n");
   // HTML CSS styling
   client.print(html_style());
   client.print("</head>\n");
   client.print("<body>\n");
   // HTML Navigation bar
   client.print("<div class='navbar'>\n"
                  "<h1 >CAETANO</h1>\n"
                  "<a href='/list'>Files</a>\n"
                  "<a href='/cal'>Calibrate</a>\n"
                  "<a href='/'>Home</a>\n"
                  "</div>\n");
}   

// Handle get request for home page
void http_get_index(EthernetClient client) {
   Serial.print("Http GET ... ");
   // Http response header
   client.print("HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"   // Connection will be closed after response completion
                "Refresh: 5\r\n"          // Refresh page automatically every 5 sec
                "\r\n");                  // Blank line to split HTTP header and HTTP body
   // HTML head and style
   html_head(client);
   // HTML remaining body
   client.print("<h3>Scale:</h3>\n");
   client.print("<div class='indent'>\n");
   client.print("<h2>Raw: "); 
   client.print(meas.raw);
   client.print("</h2>\n");
   client.print("<h2> Weight: "); 
   client.print(meas.weight());
   client.print(" kg</h2>\n");
   if (meas.weight() <= cal.alarm) {
      client.print("<div id='blink'>Low level alarm</div>\n");
   }
   client.print("</div>\n");
   client.print("</body>\n"
                "</html>\n");
   Serial.println("done!");
}

// Handle get request for calibration page
void http_get_cal(EthernetClient client) {
   Serial.print("Http GET ... ");
   // Http response header
   client.print("HTTP/1.1 200 OK\r\n"
                "Location: /cal\r\n" 
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"    // Connection will be closed after response completion
                "\r\n");                   // Blank line to split HTTP header and HTTP body
   // HTML head and style
   html_head(client);
   // HTML remaining body
   client.print("<h3>Parameters:</h3>\n"
                "<div>\n"
                "<label>Zero:</label>\n"
                "<div class='data'>");
   client.print(cal.y0);
   client.print(" kg</div>\n"
                "<div class='data'>(");
   client.print(cal.x0);
   client.print(")</div>\n"
                "</div>\n"
                "<div>\n"
                "<label>Span:</label>\n"
                "<div class='data'>");
   client.print(cal.y1);
   client.print(" kg</div>\n"
                "<div class='data'>(");
   client.print(cal.x1);
   client.print(")</div>\n"
                "</div>\n"
                "<div>\n"
                "<label>Offset:</label>\n"
                "<div class='data'>");
   client.print(cal.bias);
   client.print(" kg</div>\n"
                "</div>\n"
                "<div>\n"   
                "<label>Alarm:</label>\n"
                "<div class='data'>");
   client.print(cal.alarm);
   client.print(" kg</div>\n"
                "</div>\n");
   // Calibration form
   client.print("<h3>Calibrate parameter:</h3>\n"
                "<div>\n"
                "<form method='post' action=' '>\n"
                "<label for='zero'>Zero:</label>\n"
                "<input type='number' step='0.1' id='zero' name='value'>\n"
                "<input class='btn' name='zero' type='submit' value='Set'>\n"
                "</form>\n"
                "</div>\n"
                "<div>\n"
                "<form method='post' action=' '>\n"
                "<label for='span'>Span:</label>\n"
                "<input type='number' step='0.1' id='span' name='value'>\n"
                "<input class='btn' name='span' type='submit' value='Set'>\n"
                "</form>\n"
                "</div>\n"
                "<div>\n"
                "<form method='post' action=' '>\n"
                "<label for='offset'>Offset:</label>\n"
                "<input type='number' step='0.01' id='offset' name='value'>\n"
                "<input class='btn' name='offset' type='submit' value='Set'>\n"
                "</form>\n"
                "</div>\n"
                "<div>\n"
                "<form method='post' action=' '>\n"
                "<label for='alarm'>Alarm:</label>\n"
                "<input type='number' step='0.1' id='alarm' name='value'>\n"
                "<input class='btn' name='alarm' type='submit' value='Set'>\n"
                "</form>\n"
                "</div>\n"
                "<h3>Gate:</h3>\n"
                "<div>\n"
                "<form method='post' action=' '>\n"
                "<input class='btn' name='gate' type='submit' value='Open'>\n"
                "</form>\n"
                "</div>\n");
   client.print("</body>\n"
                "</html>\n");
   Serial.println("done!");
}

// Handle get request for view file data
void http_get_list(EthernetClient client) {
   Serial.print("Http GET ... ");
   // Http response header
   client.print("HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"   // Connection will be closed after response completion
                "\r\n");                  // Blank line to split HTTP header and HTTP body
   // HTML head and style
   html_head(client);
   // HTML remaining body
   client.print("<h3>File list:</h3>"
                "<div class='indent'>"
                "<ol>"
                "<li>&ensp;<a href='/file1'>CAL.TXT</a></li>"
                "<li>&ensp;<a href='/file2'>UID.TXT</a></li>"
                "<li>&ensp;<a href='/file3'>LOG.TXT</a></li>"
                "<li>&ensp;<a href='/file4'>EVENT.TXT</a></li>"
                "</ol>"
                "</div>");
   client.print("</body>\n"
                "</html>\n");
   Serial.println("done!");
}

// View file data
void http_get_file(EthernetClient client, String file_path) {
   Serial.print("Http GET ... ");
   // Http response header
   client.print("HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"   // Connection will be closed after response completion
                "\r\n");                  // Blank line to split HTTP header and HTTP body
   // HTML head and style
   html_head(client);
   // HTML remaining body
   client.print("<h3>File data:</h3>"
                "<div class='indent'>");

   // Enable SD Card for read and write
   enable_SD_Card();
   
   // Open file for reading
   file = SD.open(file_path, FILE_READ);
   if (file) {
      Serial.print("Reading ");
      Serial.print(String(file_path));
      Serial.println(" ...");
      // Read data from begining to end of file
      int line_count = 0;
      while (file.available()) {
         // Enable SD Card for read and write
         enable_SD_Card();
         // The \n character is discarded from buffer
         String line = file.readStringUntil('\n');

         // Enable ETH server
         enable_ETH();
         client.print("<div>");
         client.print(line);
         client.println("</div>");
      }
      // Enable SD Card for read and write
      enable_SD_Card();

      // Close file:
      file.close();
      Serial.println("Read complete!");
   } else {
      // If file didn't open, print an error:
      Serial.println("Error opening file!");
   }
   // Enable ETH server
   enable_ETH();

   client.print("</div>");
   client.print("</body>\n"
                "</html>\n");
   Serial.println("done!");
}

// Handle post request
void http_post(EthernetClient client) {
   Serial.print("Http POST ... ");
   char str[30] = "\0";
   int i = 0;
   // Read http post
   while(client.available())
   {
      str[i]= client.read();
      ++i;
   }
   Serial.println("done!");

   // Check post data
   if (String(str).indexOf("zero=Set") != -1) {
      String val = String(str).substring(String(str).indexOf("value=") + 6, 
                                         String(str).indexOf("&zero=Set"));
      cal.x0 = scale.read_average(20);
      cal.y0 = val.toDouble();
      Serial.print("Received: ");
      Serial.println(str);
      Serial.print("Zero: ");
      Serial.print(cal.y0);
      Serial.print(" -> ");
      Serial.println(cal.x0);
      // Save calibration to file
      save_calibration();
      // Log event
      log_event("Set zero = " + val, 101);
   }
   else if (String(str).indexOf("span=Set") != -1) {
      String val = String(str).substring(String(str).indexOf("value=") + 6, 
                                         String(str).indexOf("&span=Set"));                                     
      cal.x1 = scale.read_average(20);
      cal.y1 = val.toDouble();
      Serial.print("Received: ");
      Serial.println(str);
      Serial.print("Span: ");
      Serial.print(cal.y1);
      Serial.print(" -> ");
      Serial.println(cal.x1);
      // Save calibration to file
      save_calibration();
      // Log event
      log_event("Set span = " + val, 102);
   }
   else if (String(str).indexOf("offset=Set") != -1) {
      String val = String(str).substring(String(str).indexOf("value=") + 6, 
                                         String(str).indexOf("&offset=Set"));
      cal.bias = val.toDouble();
      Serial.print("Received: ");
      Serial.println(str);
      Serial.print("Offset: ");
      Serial.println(cal.bias);
      // Save calibration to file
      save_calibration();
      // Log event
      log_event("Set offset = " + val, 103);   }
   else if (String(str).indexOf("alarm=Set") != -1) {
      String val = String(str).substring(String(str).indexOf("value=") + 6, 
                                         String(str).indexOf("&alarm=Set"));
      cal.alarm = val.toDouble();
      Serial.print("Received: ");
      Serial.println(str);
      Serial.print("Alarm: ");
      Serial.println(cal.alarm);
      // Save calibration to file
      save_calibration();
      // Log event
      log_event("Set alarm = "  + val, 104);
   }
   else if(String(str).indexOf("gate=Open") != -1) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.print("Received: ");
      Serial.println(str);
      Serial.print("Unlock: ");
      Serial.println(digitalRead(RELAY_PIN));
      delay(2000);
      digitalWrite(RELAY_PIN, LOW);
      Serial.print("Unlock: ");
      Serial.println(digitalRead(RELAY_PIN));
      Serial.print(" (after 2 sec.): ");
      // Log event
      log_event("Unlock via: http server", 201);   
   }
   // Redirect after POST
   http_get_cal(client);
}

// Serve data
void http_server() {
   // Enable ETH
   enable_ETH();

   // Listen for incoming client http requests
   EthernetClient client = server.available();
   if (client) {
      Serial.println();
      Serial.println("Client connected!");

      // An http request ends with a blank line
      boolean currentLineIsBlank = true;

      // Store 50 char from http header to compare
      String row, str;

      while (client.connected()) {
         if (client.available()) {
            char c = client.read();
            Serial.print(c);
            // Store first 50 characters of first row of html page
            if (row.length() < 50) {
               row += c;
            }
            
            // Check if http request has ended, with new line character and blank line.
            if (c == '\n' && currentLineIsBlank) {
               // Http response.
               if (row.startsWith("POST /cal")) {
                  http_post(client);

               } else if (row.startsWith("GET /cal")) {
                  http_get_cal(client);

               } else if (row.startsWith("GET /list")) {
                  http_get_list(client);

               } else if (row.startsWith("GET /file1")) {
                  http_get_file(client, "CAL.TXT");

               } else if (row.startsWith("GET /file2")) {
                  http_get_file(client, "UID.TXT");

               } else if (row.startsWith("GET /file3")) {
                  http_get_file(client, "LOG.TXT");

               } else if (row.startsWith("GET /file4")) {
                  http_get_file(client, "EVENT.TXT");

               } else {
                  http_get_index(client);
               }
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
      Serial.println("Client disconnected!");
      Serial.println();
   }
}
