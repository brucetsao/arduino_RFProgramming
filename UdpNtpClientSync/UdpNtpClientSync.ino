/*

 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe

 This code is in the public domain.

 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
UDP UDP;

char string[ 17 ] = { "" };

int hour, minute, second;

unsigned int localPort = 123;
unsigned int timeZone = -5;

unsigned int serverNum = 0;
unsigned long timeout;

const char timeServer[] = "pool.ntp.org";

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};
IPAddress ip(192, 168, 30, 200);
IPAddress dnServer(168, 95, 1, 1);
// the router's gateway address:
IPAddress gateway(192, 168, 30, 254);
// the subnet:
IPAddress subnet(255, 255, 255, 0);


unsigned int localPort = 8888;       // local port to listen for UDP packets

char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
// start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);

  }
  Udp.begin(localPort);
  	Serial.begin(9600);
	delay(1000);
	Serial.println("NTP-clock");


}

void loop()
{
	sendNTPpacket(timeServer);  // send an NTP packet to a time server
	// wait to see if a reply is available

    delay(1000);
    if ( UDP.parsePacket() ) {
    	UDP.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer

	    //the timestamp starts at byte 40 of the received packet and is four bytes,
	    // or two words, long. First, esxtract the two words:

	    unsigned long highWord = (packetBuffer[40] << 8) + packetBuffer[41];
	    unsigned long lowWord = (packetBuffer[42] << 8) + packetBuffer[43];
	    // combine the four bytes (two words) into a long integer
	    // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
	    secsSince1900 += timeZone*60*60;

	    // now convert NTP time into everyday time:
	    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
	    const unsigned long seventyYears = 2208988800UL;    
	    // subtract seventy years:
	    unsigned long epoch = secsSince1900 - seventyYears; 

	    hour = (epoch % 86400L) / 3600;         
	    minute = (epoch % 3600) / 60;
	    second = (epoch % 60);

	    Serial.print (hour);
	    Serial.print (":");
	    Serial.print (minute);
	    Serial.print (":");
	    Serial.println (second);
        }

	while ( UDP.parsePacket() ) {               // clean-up buffer
		UDP.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
	}

	// wait ten seconds before asking for the time again
	delay(10000);
}


// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(const char *address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	UDP.beginPacket(address, 123);
	UDP.write(packetBuffer, NTP_PACKET_SIZE); //NTP requests are to port 123
	UDP.endPacket();

}
