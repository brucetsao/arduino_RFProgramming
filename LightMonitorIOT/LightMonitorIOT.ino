
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <BH1750.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};
IPAddress ip(192, 168, 30, 200);
IPAddress dnServer(168, 95, 1, 1);
// the router's gateway address:
IPAddress gateway(192, 168, 30, 254);
// the subnet:
IPAddress subnet(255, 255, 255, 0);


EthernetClient client;

BH1750 lightMeter;
IPAddress server(192, 168, 30, 100); 

  String connectstr ;

void setup() {
  lightMeter.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
      while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo only
      }

  // start the Ethernet connection:
      if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // no point in carrying on, so do nothing forevermore:
        // try to congifure using IP address instead of DHCP:
          Ethernet.begin(mac, ip, dnServer, gateway, subnet);
    
      }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");
  //Serial.println(Ethernet.localIP());
}


void loop() {
 connectstr = "" ;

  uint16_t lux = lightMeter.readLightLevel();
  connectstr = "light=" + String(lux);
     Serial.print(lux) ;
     Serial.print("/") ;
     Serial.println(connectstr) ;
   // client.connect(server,80) ;
  // if there are incoming bytes available
  // from the server, read them and print them:
 
	if (client.connect(server,80)) { // REPLACE WITH YOUR SERVER ADDRESS
		client.println("POST /iot/add.php HTTP/1.1"); 
                Serial.println("POST /iot/add.php HTTP/1.1"); 
		client.print("Host:"); // SERVER ADDRESS HERE TOO
		Serial.print("Host:"); // SERVER ADDRESS HERE TOO
		client.println(server); // SERVER ADDRESS HERE TOO
		Serial.println(server); // SERVER ADDRESS HERE TOO
		client.println("Content-Type: application/x-www-form-urlencoded"); 
		Serial.println("Content-Type: application/x-www-form-urlencoded"); 
		client.print("Content-Length: "); 
		Serial.print("Content-Length: "); 
		client.println(connectstr.length()); 
		Serial.println(connectstr.length()); 
		client.println(); 
		client.print(connectstr); 
		Serial.print(connectstr); 
	} 

	if (client.connected()) { 
		client.stop();	// DISCONNECT FROM THE SERVER
	}

	delay(3000); // WAIT FIVE MINUTES BEFORE SENDING AGAIN
}

