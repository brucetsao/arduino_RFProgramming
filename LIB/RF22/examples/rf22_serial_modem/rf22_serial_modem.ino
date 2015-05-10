// rf22_serial_modem.pde
// -*- mode: C++ -*-
// Example sketch implements a bidirectional serial modem
// with the RF22ReliableDatagram class.
// It is designed to work with another instance of rf22_serial_modem.
// Pairs of arduino+radio running this program translate input Serial port data to
// radio and send it to the other side where the data is sent out on its serial port.
// So it acts like a transparent serial cable over radio.
// Pairs of radios share the same ADDRESS, and therefore you can have up to 256 such
// pairs sharing the same airspace.
// In the event of radio communications failure, it fails silently.

#include <RF22ReliableDatagram.h>
#include <RF22.h>
#include <SPI.h>

// This is one end of a peer-to-peer link, the pair is uniquely identified by the ADDRESS
// Both ends must have the same ADDRESS. Pairs with a different ADDRESSs can share the same airspace
// without incorrect delivery.
// It would be nice to be able to change this (and perhaps other things) using the serial port
// and a command set something like an AT modem?
#define ADDRESS 1

// Singleton instance of the radio
RF22ReliableDatagram rf22(ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}


uint8_t buf[RF22_MAX_MESSAGE_LEN]; // For both send and receive
uint8_t len;                       // How many bytes in buf

void loop()
{
    // Look for any data to be sent. Collect as much
    // as possible and send it in one message
    // Caution, the Serial port buffer is only 64 octets, so dont blast the data
    // at the arduino too fast.
    if (Serial.available())
    {
      len = 0;
      int ch;
      // Collect as many input chars as possible
      while (len < RF22_MAX_MESSAGE_LEN &&
            (ch = Serial.read()) != -1)
      {
        buf[len++] = ch;
      }
      
      // Send the data to the other end
      if (!rf22.sendtoWait(buf, len, ADDRESS))
      {
        // Silent failure
      }
    }
    
    // Look for any data from the other end
    uint8_t from;
    len = sizeof(buf);
    if (rf22.recvfromAck(buf, &len, &from))
    {
      // Received data from the other end. Expect from to be our ADDRESS
      Serial.write(buf, len); // Send out to our serial port
    }
}

