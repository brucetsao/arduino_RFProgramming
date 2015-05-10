// rf22_mesh_server3.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, routed reliable messaging server
// with the RF22Mesh class.
// It is designed to work with the other examples rf22_mesh_*
// Hint: you can simulate other network topologies by setting the 
// RF22_TEST_NETWORK define in RF22Router.h

// Mesh has much greater memory requirements, and you may need to limit the
// max message length to prevent wierd crashes
#define RF22_MAX_MESSAGE_LEN 50

#include <RF22Router.h>
#include <RF22Mesh.h>
#include <SPI.h>

// In this small artifical network of 4 nodes,
#define CLIENT_ADDRESS 1
#define SERVER1_ADDRESS 2
#define SERVER2_ADDRESS 3
#define SERVER3_ADDRESS 4

// Singleton instance of the radio
RF22Mesh rf22(SERVER3_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

uint8_t data[] = "And hello back to you from server3";
// Dont put this on the stack:
uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
void loop()
{
  while (1)
  {
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (rf22.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);

      // Send a reply back to the originator client
      if (rf22.sendtoWait(data, sizeof(data), from) != RF22_ROUTER_ERROR_NONE)
        Serial.println("sendtoWait failed");
    }
  }
}

