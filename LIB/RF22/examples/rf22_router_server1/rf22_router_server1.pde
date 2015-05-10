// rf22_router_server1.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, routed reliable messaging server
// with the RF22Router class.
// It is designed to work with the other example rf22_router_client

#include <RF22Router.h>
#include <SPI.h>

// In this small artifical network of 4 nodes,
// messages are routed via intermediate nodes to their destination
// node. All nodes can act as routers
// CLIENT_ADDRESS <-> SERVER1_ADDRESS <-> SERVER2_ADDRESS<->SERVER3_ADDRESS
#define CLIENT_ADDRESS 1
#define SERVER1_ADDRESS 2
#define SERVER2_ADDRESS 3
#define SERVER3_ADDRESS 4

// Singleton instance of the radio
RF22Router rf22(SERVER1_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  
  // Manually define the routes for this network
  rf22.addRouteTo(CLIENT_ADDRESS, CLIENT_ADDRESS);  
  rf22.addRouteTo(SERVER2_ADDRESS, SERVER2_ADDRESS);
  rf22.addRouteTo(SERVER3_ADDRESS, SERVER2_ADDRESS);
}

uint8_t data[] = "And hello back to you from server1";
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

