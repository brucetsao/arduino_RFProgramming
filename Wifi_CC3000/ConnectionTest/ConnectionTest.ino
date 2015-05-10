/****************************************************************
ConnectionTest.ino
CC3000 Connection Test
Shawn Hymel @ SparkFun Electronics
January 30, 2014
https://github.com/sparkfun/SFE_CC3000_Library

Connects to the access point given by the SSID and password and
waits for a DHCP-assigned IP address. To use a static IP
address, change the #define USE_DHCP from 1 to 0 and assign an
IP address to static_ip_addr in the Constants section.

NOTE: Static IP is not working at this time.

The security mode is defined by one of the following:
WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA, WLAN_SEC_WPA2

Hardware Connections:
 
 Uno Pin    CC3000 Board    Function
 
 +5V        VCC or +5V      5V
 GND        GND             GND
 2          INT             Interrupt
 7          EN              WiFi Enable
 10         CS              SPI Chip Select
 11         MOSI            SPI MOSI
 12         MISO            SPI MISO
 13         SCK             SPI Clock

Resources:
Include SPI.h and SFE_CC3000.h

Development environment specifics:
Written in Arduino 1.0.5
Tested with Arduino UNO R3

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/
 
#include <SPI.h>
#include <SFE_CC3000.h>
#define DEBUG_MODE  1 

// Pins
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno

// IP address assignment method
#define USE_DHCP        1   // 0 = static IP, 1 = DHCP

// Connection info data lengths
#define IP_ADDR_LEN     4   // Length of IP address in bytes
#define MAC_ADDR_LEN    6   // Length of MAC address in bytes

// Constants
char ap_ssid[] = "arduino";                  // SSID of network
char ap_password[] = "";          // Password of network
unsigned int ap_security = WLAN_SEC_UNSEC; // Security of network
unsigned int timeout = 30000;             // Milliseconds
//const char static_ip_addr[] = "0.0.0.0";

// Global Variables
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);

void setup() {
  
  ConnectionInfo connection_info;
  int i;
  
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("SparkFun CC3000 - Connection Test");
  Serial.println("---------------------------------");
  
  // Initialize CC3000 (configure SPI communications)
  if ( wifi.init() ) {
    Serial.println("CC3000 initialization complete");
  } else {
    Serial.println("Something went wrong during CC3000 init!");
  }

#if (USE_DHCP == 1)
  // Connect using DHCP
  Serial.print("Connecting to: ");
  Serial.println(ap_ssid);
  if(!wifi.connect(ap_ssid, ap_security, ap_password, timeout)) {
    Serial.println("Error: Could not connect to AP");
  }
#elif (USE_DHCP == 0)
  // Connect using static IP
  // ***TODO: Connect using static IP
#endif

  // Print out connection details
  if( !wifi.getConnectionInfo(connection_info) ) {
    Serial.println("Error: Could not obtain connection details");
  } else {
    Serial.println("Connected!");
    Serial.println();
    
    // Print MAC address
    Serial.print("CC3000 MAC Address: ");
    for ( i = 0; i < MAC_ADDR_LEN; i++ ) {
      if ( connection_info.mac_address[i] < 0x10 ) {
        Serial.print("0");
      }
      Serial.print(connection_info.mac_address[i], HEX);
      if ( i < MAC_ADDR_LEN - 1 ) {
        Serial.print(":");
      }
    }
    Serial.println();
    
    // Print IP Address
    Serial.print("IP Address: ");
    printIPAddr(connection_info.ip_address);
    Serial.println();
    
    // Print subnet mask
    Serial.print("Subnet Mask: ");
    printIPAddr(connection_info.subnet_mask);
    Serial.println();
    
    // Print default gateway
    Serial.print("Default Gateway: ");
    printIPAddr(connection_info.default_gateway);
    Serial.println();
    
    // Print DHCP server address
    Serial.print("DHCP Server: ");
    printIPAddr(connection_info.dhcp_server);
    Serial.println();
    
    // Print DNS server address
    Serial.print("DNS Server: ");
    printIPAddr(connection_info.dns_server);
    Serial.println();
    
    // Print SSID
    Serial.print("SSID: ");
    Serial.println(connection_info.ssid);
    Serial.println();
  }
  
  // Disconnect
  if ( wifi.disconnect() ) {
    Serial.println("Disconnected");
  } else {
    Serial.println("Error: Could not disconnect from network");
  }
  
  // Done!
  Serial.println("Finished connection test");
  
}

void loop() {
  
  // Do nothing
  delay(1000);
  
}

// Print out an IP Address in human-readable format
void printIPAddr(unsigned char ip_addr[]) {
  int i;
  
  for (i = 0; i < IP_ADDR_LEN; i++) {
    Serial.print(ip_addr[i]);
    if ( i < IP_ADDR_LEN - 1 ) {
      Serial.print(".");
    }
  }
}
