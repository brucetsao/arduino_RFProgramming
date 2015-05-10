// IPGateway.ino
// -*- mode: C++ -*-
//
// This example Arduino sketch shows how to implement an RF22-IP gateway.
//
// UDP packets sent to this IP address will be sent to an RF22 radio. The radio will be selected
// based on the destination UDP port.
// eg UDP messages to port 2 will be sent to the RF22 radio with address 2. 
// The payload of the UDP message will be sent as data to the radio.
//
// If a message is sent from another radio to this one, it will be relayed to the internet.
// The destination IP address and port for the messages are deduced from the senders node ID.
// If a message was previouslt releayed from the internet to that id, then the reply wil be sent to the 
// senders IP address and port.
// If there has been no recent mesasge sent to that node, the radio message will be relayed to the 
// defaultReplyAddress and defaultReplyPort defined below
// The radio message contents are sent as the UDP payload.
//
// Test this with the example sketch rf22_datagram_server:
// Send  UDP mesage to this IP address, with UDP port 2. The mesage will be sent to the rf22_datagram_server.
// Its reply will be relayed back to the original IP address and port.
// For example, using sendUDP.pl
// # perl sendUDP.pl -s 203.63.154.101 -p 2 -w hello
// And hello back to you
//
// It requires hardware
//   RF22 radio
//   Arduino Uno + Ethernet Shield
//      or...
//   Arduino EtherTen
//
// Libraries required are:
//   Ethernet (included in Arduino 1.0)
//   EtherRaw from http://www.airspayce.com/mikem/arduino/EtherRaw
//   RF22 from http://www.airspayce.com/mikem/arduino/RF22
//
// This code will NOT work with old EtherShields such as this one:
//    http://www.dfrobot.com/index.php?route=product/product&product_id=52
// because the old ones do not play nicely with other SPI devices (such as the RF22): they do not disable their SPI pins 
// when not slave selected.
// Later EtherShields (such as the 05 and 06 models) are fine.
// EtherTen is also fine
//
// Also, you MUST connect the RF22 in a slightly different way to the RF22 library documentation.
// The RF22 Slave Select pin must be connected to Arduino pin 9 
// (not the usual pin 10: 10 is used as slave select by the Ethernet library for the ethernet w5100 chip)
//
// Caution: reply speed is usually dominated by the on-the-air radio speed. With FSK_Rb2_4Fd36 modulation
// and messages of 7 and 22 octets, the send-reply response time to another radio is about 180ms
//
// Tested with EtherTen, RF22 1.19, EtherRaw 1.0, arduino 1.0
//
// Copyright (C) 2012 Mike McCauley

#include <SPI.h>
#include <Ethernet.h>
#include <utility/socket.h>
#include <EtherRaw.h>
#include <RF22Datagram.h>
#include <RF22.h>

#define THIS_ADDRESS 1

// Singleton instance of the radio. You can use RF22Datagram, RF22ReliableDatagram
// or whatever, according to whatever the other radios support
RF22Datagram rf22(THIS_ADDRESS, 9); // Need different SS pin so dont conflict with EtherShield

// Enter a MAC address and IP address for this gateway below.
// The IP address will be dependent on your local network:
MACAddress mac(0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED);
IPAddress ip(203, 63, 154, 101);

// If a radio message is received and ther eis no previously recorded sou8rce adress, send the message to 
// to this default address
IPv4Address defaultReplyAddress(203, 63, 154, 29);
uint16_t defaultReplyPort = 9048;

uint8_t macraw_socket = 0;
uint8_t udp_reply_socket = 1;

void setup()
{
  delay(100); // EtherTen likes a delay like this so the w5100 starts OK
  
  // start the Ethernet chip. You can also specifiy the internet gateway address etc here, if you want
  Ethernet.begin(mac.address(), ip);  
  Serial.begin(115200);
  
  // Create a MACRAW socket to listen for incoming UDP packets
  // Cant use ordinary UDP socket, since they can only listen to one port at a time.
  socket(macraw_socket, SnMR::MACRAW, 0, 0);

  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

// recvfrom in socket.cpp is guilty of buffer overflows
// This version is specifically for MACRAW
// and will only fill buf up to max of len octets
// returns the amount actually copied to buf
uint16_t macraw_read(SOCKET s, uint8_t *buf, uint16_t len)
{
  uint8_t head[2];
  uint16_t data_len=0;
  uint16_t ptr=0;

  if (!W5100.getRXReceivedSize(s))
    return 0;
  ptr = W5100.readSnRX_RD(s);
  W5100.read_data(s, (uint8_t*)ptr, head, 2);
  ptr+=2;
  data_len = head[0];
  data_len = (data_len<<8) + head[1] - 2;
  // Only copy as much as will fil the buffer
  if (len > data_len)
    len = data_len;
  W5100.read_data(s, (uint8_t*)ptr, buf, len);
  ptr += data_len;
  W5100.writeSnRX_RD(s, ptr);
  W5100.execCmdSn(s, Sock_RECV);
  return len;
}

// Structure for recording where requests come from and are sent to
typedef struct Source
{
  IPv4Address  source;
  uint16_t    sourcePort;
  uint16_t    destPort;
} Source;

#define NUM_SOURCE_RECORDS 4
Source sources[NUM_SOURCE_RECORDS];
uint8_t next_source_record = 0;

struct Source* findSourceRecord(uint16_t destPort)
{
  uint8_t i;
  
  for (i =0; i < NUM_SOURCE_RECORDS; i++)
  {
    if (sources[i].destPort == destPort)
      return &sources[i];
  }
  return NULL;
}

// Record the most recent sender to the dest port
void recordSource(const IPv4Address& source, uint16_t sourcePort, uint16_t destPort)
{
  Source* s = findSourceRecord(destPort);
  
  if (!s)
  {
    s = &sources[next_source_record];
    next_source_record = (next_source_record + 1) % NUM_SOURCE_RECORDS;
  }
  
  s->source = source;
  s->sourcePort = sourcePort;
  s->destPort = destPort;

}

void loop()
{
  uint16_t len;
  uint8_t buf[200];

  // First see if anyone sent a UDP message for transmit to a radio node
  // Disable interrupts while we poll the w5100
  // else if an RF22 interrupt occurs, the ISR may not be able to contact the RF22 due to the state of the
  // SPI interface at the time 
  cli();
  len = macraw_read(macraw_socket, buf, sizeof(buf));
  sei();
  
  if (len)
  {
    EthernetHeader* h = (EthernetHeader*)buf;
      
//    h->printTo(Serial);
    // Is it for our MAC? Ignore broadcasts
    if (!(h->dest() == mac))
      return;
    // Is it IPv4?
    if (h->ethertype() != EthernetHeader::ETH_P_IP)
      return;
    PacketIPv4* p = (PacketIPv4*)h->payload();  
    // Is it really IPv4?
    if (p->version() != IPVERSION)
      return;
    // Is it addressed to our IP address?
    if (!(ip == p->dest().address()))
      return;
      
//    p->printTo(Serial);
    // Is it UDP?
    if (p->protocol() != PacketIPv4::IPPROTO_UDP)
      return;
    // Get UDP data
    PacketUDP* u = (PacketUDP*)p->payload();
//    u->printTo(Serial);
//    RF22::printBuffer("payload:", buf, len);
    
    // Only send if its for another radio
    // REVISIT: if the message was for this radio, could implement command to configure the radio
    if (u->destPort() != THIS_ADDRESS)
    {
 //     Serial.println((const char*)u->payload());
       recordSource(p->source(), u->sourcePort(), u->destPort());
      if (rf22.sendto(u->payload(), u->payload_length(), u->destPort()))
        rf22.waitPacketSent();
    }
  }

  // Now see if anyone has sent us a message
  // And send it out on the internet
  uint8_t recvlen = sizeof(buf);
  uint8_t from;
  uint8_t to;      
  if (rf22.recvfrom(buf, &recvlen, &from, &to))
  {
 //   Serial.print("got message from : 0x");
 //   Serial.print(from, HEX);
 //   Serial.print(": ");
 //   Serial.println((char*)buf);
    
    // Forward the payload to the internet
    cli();
    socket(udp_reply_socket, SnMR::UDP, from, 0);
    // If we have recorded a source address/port for this radio, use it
    // Else use the default address/port
    Source* s = findSourceRecord(from);
    if (s)
      sendto(udp_reply_socket, buf, recvlen, (uint8_t*)s->source.address(), s->sourcePort);
    else
       sendto(udp_reply_socket, buf, recvlen, (uint8_t*)defaultReplyAddress.address(), defaultReplyPort);
    // We have to close this socket again, otherwise it will take priority over the MACRAW socket and MACRAW wont 
    // receive UDP requests for this port again.
    close(udp_reply_socket);
    sei();
  }
}

