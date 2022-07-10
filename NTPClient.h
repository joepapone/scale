/////////////////////////////////////////////////////////////////////////////////////
// Http Web server
/////////////////////////////////////////////////////////////////////////////////////
//
//    FILE: NTPClient.h
//  AUTHOR: Jose Ferreira
// VERSION: 1.0.1
// PURPOSE: Library for UDP NTP Client
//   NOTES: Get time from a Network Time Protocol (NTP) time server according to RFC 5905.
//          Information on NTP time servers and messages for communication, see:
//          http://en.wikipedia.org/wiki/Network_Time_Protocol

#include <EthernetUdp.h>

// Local port to listen for UDP packets
unsigned int localPort = 8888;

// time.nist.gov NTP server
const char timeServer[] = "time.nist.gov";

// NTP time stamp within first 48 bytes of message
const int NTP_PACKET_SIZE = 48;

// Buffer to hold incoming and outgoing packets
byte packetBuffer[NTP_PACKET_SIZE];

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void NTPtime_config() {
  	Udp.begin(localPort);
   Serial.print("NTP time: ... ");
   Serial.println("active!");
}

// Send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
   // Set all bytes in the buffer to 0
   memset(packetBuffer, 0, NTP_PACKET_SIZE);
   // Initialize values needed to form NTP request
   // (see URL above for details on the packets)
   packetBuffer[0] = 0b11100011;    // LI, Version, Mode
   packetBuffer[1] = 0;             // Stratum, or type of clock
   packetBuffer[2] = 6;             // Polling Interval
   packetBuffer[3] = 0xEC;          // Peer Clock Precision
   // 8 bytes of zero for Root Delay & Root Dispersion
   packetBuffer[12] = 49;
   packetBuffer[13] = 0x4E;
   packetBuffer[14] = 49;
   packetBuffer[15] = 52;

   // All NTP fields have been given values, now
   // you can send a packet requesting a timestamp:
   Udp.beginPacket(address, 123); 	// NTP requests are to port 123
   Udp.write(packetBuffer, NTP_PACKET_SIZE);
   Udp.endPacket();
}

// Convert unix time to Human readable format
String convert_Unix(unsigned long unix_seconds) {

	// Initialize Human readable date time format
	String datetime;

	// Number of days in month in normal year
	int daysOfMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	long int daysTillNow, extraTime, extraDays;
	int currYear, month, day, hours, minutes, seconds, index, flag = 0;

	// Adds 1 hour to ajust for timezone UTC+01:00
	unix_seconds += 3600;
	// Calculate total days unix time
	daysTillNow = (long int)(unix_seconds / 86400UL);
	extraTime = (long int)(unix_seconds % 86400UL);
	currYear = 1970;

	// Calculating current year
	while (daysTillNow >= 365) {
		if (currYear % 400 == 0 || (currYear % 4 == 0 && currYear % 100 != 0)) {
			daysTillNow -= 366;
		}
		else {
			daysTillNow -= 365;
		}
		currYear += 1;
	}

	// Updating extradays because it will give days till previous day and we have include current day
	extraDays = daysTillNow + 1;

	// If leap year
	if (currYear % 400 == 0 || (currYear % 4 == 0 && currYear % 100 != 0))
		flag = 1;

	// Calculating month and day
	month = 0, index = 0;
	if (flag == 1) {
		while (true) {
			if (index == 1) {
				if (extraDays - 29 < 0)
					break;
				month += 1;
				extraDays -= 29;
			}
			else {
				if (extraDays - daysOfMonth[index] < 0) {
					break;
				}
				month += 1;
				extraDays -= daysOfMonth[index];
			}
			index += 1;
		}
	}
	else {
		while (true) {
			if (extraDays - daysOfMonth[index]
				< 0) {
				break;
			}
			month += 1;
			extraDays -= daysOfMonth[index];
			index += 1;
		}
	}
	// Current Month
	if (extraDays > 0) {
      month += 1;
      day = extraDays;
	}
	else {
		if (month == 2 && flag == 1)
			day = 29;
		else {
			day = daysOfMonth[month - 1];
		}
	}

	// Calculating HH:MM:SS
	hours = extraTime / 3600;
	minutes = (extraTime % 3600) / 60;
	seconds = (extraTime % 3600) % 60;

	// Format UTC datetime 
	datetime += currYear;
	datetime += "-";
	if (month < 10) {
		datetime += "0";
	}
	datetime += month;
	datetime += "-";
	if (day < 10) {
		datetime += "0";
	}
	datetime += day;
	datetime += " ";
	if (hours + 1 < 10) {
		datetime += "0";
	}
	datetime += hours;
	datetime += ":";
	if (minutes < 10) {
		datetime += "0";
	}
	datetime += minutes;
	datetime += ":";
	if (seconds < 10) {
		datetime += "0";
	}
	datetime += seconds;

	// View formated YYYY-MM-DD HH:MM:SS datetime
	Serial.print("UTC+01:00 time: ");
	Serial.println(datetime);

	// Return the timestamp
	return datetime;
}

unsigned long get_NTPtime() {
   // Send an NTP packet to a time server
   sendNTPpacket(timeServer);

   // Wait to see if a reply is available
   delay(1000);

	// Initialize unix time varable
	unsigned long epoch = 0;
	
   if (Udp.parsePacket()) {
      // Read recieved packet into buffer
      Udp.read(packetBuffer, NTP_PACKET_SIZE);

      // The timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, extract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // Combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;

      // Now convert NTP time into unix time
      Serial.print("Unix time: ");
      // Unix time starts on Jan 1 1970, which corresponds to 2208988800 seconds
      const unsigned long seventyYears = 2208988800UL;
      // Subtract seventy years
      epoch = secsSince1900 - seventyYears;
      // Update unix time in seconds
		Serial.println(epoch);
   }

	return epoch;
}
