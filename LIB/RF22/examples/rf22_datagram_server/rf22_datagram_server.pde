// rf22_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed messaging server
// with the RF22Datagram class. RF22Datagram class does not provide for reliability.
// It is designed to work with the other example rf22_datagram_client

#include <RF22Datagram.h>
#include <RF22.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio
RF22Datagram rf22(SERVER_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

// Dont put this on the stack:
uint8_t buf[RF22_MAX_MESSAGE_LEN];
void loop()
{
  while (1)
  {
    // Wait for a message addressed to us
    rf22.waitAvailable();
    
    // Should be a message for us now   
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t to;    
    if (rf22.recvfrom(buf, &len, &from, &to))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      
      // Send a reply back to the originator
      uint8_t data[] = "And hello back to you";
      rf22.sendto(data, sizeof(data), from);
      rf22.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}

