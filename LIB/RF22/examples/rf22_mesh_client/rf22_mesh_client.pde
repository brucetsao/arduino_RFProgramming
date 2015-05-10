// rf22_mesh_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, routed reliable messaging client
// with the RF22Mesh class.
// It is designed to work with the other examples rf22_mesh_server*
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
RF22Mesh rf22(CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RF22_MESH_MAX_MESSAGE_LEN];

void loop()
{

  while (1)
  {
    Serial.println("Sending to rf22_mesh_server2");
    
    // Send a message to a rf22_router_server
    // A route to the destination will be automatically discovered.
    if (rf22.sendtoWait(data, sizeof(data), SERVER3_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      Serial.println("sendtoWait failed. Are the intermediate routers running?");
    else
    {
      // It has been reliably delivered to the next node.
      // Now wait for a reply from the ultimate server
      uint8_t len = sizeof(buf);
      uint8_t from;    
      if (rf22.recvfromAckTimeout(buf, &len, 3000, &from))
      {
        Serial.print("got reply from : 0x");
        Serial.print(from, HEX);
        Serial.print(": ");
        Serial.println((char*)buf);
      }
      else
      {
        Serial.println("No reply, is rf22_mesh_server1, rf22_mesh_server2 and rf22_mesh_server3 running?");
      }
    }
  }
}

