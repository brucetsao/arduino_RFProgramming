// rf22_server_softwarespi.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RF22 class. RF22 class does not provide for addressing or reliability.
// It is designed to work with the other example rf22_client

#include <SPI.h>
#include <RF22.h>
#include <SoftwareSPI.h>

// Software SPI class allows us to talk to the radio
// via bit-bang software instead of a hardware SPI driver
SoftwareSPIClass Software_spi;

// Singleton instance of the radio, using softwaer SPI
RF22 rf22(SS, 0, &Software_spi);

void setup() 
{
  Serial.begin(9600);
  
    // Configure SoftwareSPI for the same pins as usual hardware
  Software_spi.setPins(12, 11, 13);

  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

void loop()
{
  if (rf22.available())
  {
    // Should be a message for us now   
    uint8_t buf[RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf22.recv(buf, &len))
    {
      Serial.print("got request: ");
      Serial.println((char*)buf);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf22.send(data, sizeof(data));
      rf22.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}

