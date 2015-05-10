// RF22ReliableDatagram.cpp
//
// Define addressed datagram
// 
// Part of the Arduino RF22 library for operating with HopeRF RF22 compatible transceivers 
// (see http://www.hoperf.com)
// RF22Datagram will be received only by the addressed node or all nodes within range if the 
// to address is RF22_BROADCAST_ADDRESS
//
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2011 Mike McCauley
// $Id: RF22ReliableDatagram.cpp,v 1.9 2014/04/01 05:06:44 mikem Exp mikem $

#include <RF22ReliableDatagram.h>
#include <SPI.h>

////////////////////////////////////////////////////////////////////
// Constructors
RF22ReliableDatagram::RF22ReliableDatagram(uint8_t thisAddress, uint8_t slaveSelectPin, uint8_t interrupt) 
    : RF22Datagram(thisAddress, slaveSelectPin, interrupt)
{
    _retransmissions = 0;
    _lastSequenceNumber = 0;
    _timeout = 200;
    _retries = 3;
}

////////////////////////////////////////////////////////////////////
// Public methods
void RF22ReliableDatagram::setTimeout(uint16_t timeout)
{
    _timeout = timeout;
}

////////////////////////////////////////////////////////////////////
void RF22ReliableDatagram::setRetries(uint8_t retries)
{
    _retries = retries;
}

////////////////////////////////////////////////////////////////////
boolean RF22ReliableDatagram::sendtoWait(uint8_t* buf, uint8_t len, uint8_t address)
{
    // Assemble the message
    uint8_t thisSequenceNumber = ++_lastSequenceNumber;
    uint8_t retries = 0;
    while (retries++ <= _retries)
    {
	setHeaderId(thisSequenceNumber);
	setHeaderFlags(0);
	sendto(buf, len, address);
	waitPacketSent();

	// Never wait for ACKS to broadcasts:
	if (address == RF22_BROADCAST_ADDRESS)
	    return true;

	if (retries > 1)
	    _retransmissions++;
	unsigned long thisSendTime = millis(); // Timeout does not include original transmit time

	// Compute a new timeout, random between _timeout and _timeout*2
	// This is to prevent collisions on every retransmit
	// if 2 nodes try to transmit at the same time
	uint16_t timeout = _timeout + (_timeout * random(0, 256) / 256);
        while ((millis() - thisSendTime) < timeout)
	{
	    if (available())
	    {
		clearRxBuf(); // Not using recv, so clear it ourselves
		uint8_t from = headerFrom();
		uint8_t to = headerTo();
		uint8_t id = headerId();
		uint8_t flags = headerFlags();
		// Now have a message: is it our ACK?
		if (   from == address 
		    && to == _thisAddress 
		    && (flags & RF22_FLAGS_ACK) 
		    && (id == thisSequenceNumber))
		{
		    // Its the ACK we are waiting for
		    return true;
		}
		else if (   !(flags & RF22_FLAGS_ACK)
			 && (id == _seenIds[from]))
		{
		    // This is a request we have already received. ACK it again
		    acknowledge(id, from);
		}
		// Else discard it
	    }
	    // Not the one we are waiting for, maybe keep waiting until timeout exhausted
	}
	// Timeout exhausted, maybe retry
    }
    return false;
}

////////////////////////////////////////////////////////////////////
boolean RF22ReliableDatagram::recvfromAck(uint8_t* buf, uint8_t* len, uint8_t* from, uint8_t* to, uint8_t* id, uint8_t* flags)
{  
    uint8_t _from;
    uint8_t _to;
    uint8_t _id;
    uint8_t _flags;
    // Get the message before its clobbered by the ACK (shared rx anfd tx buffer in RF22
    if (available() && recvfrom(buf, len, &_from, &_to, &_id, &_flags))
    {
	// Never ACK an ACK
	if (!(_flags & RF22_FLAGS_ACK))
	{
	    // Its a normal message for this node, not an ACK
	    if (_to != RF22_BROADCAST_ADDRESS)
	    {
		// Its not a broadcast, so ACK it
		// Acknowledge message with ACK set in flags and ID set to received ID
		acknowledge(_id, _from);
	    }
	    // If we have not seen this message before, then we are interested in it
	    if (_id != _seenIds[_from])
	    {
		if (from)  *from =  _from;
		if (to)    *to =    _to;
		if (id)    *id =    _id;
		if (flags) *flags = _flags;
		_seenIds[_from] = _id;
		return true;
	    }
	    // Else just re-ack it and wait for a new one
	}
    }
    // No message for us available
    return false;
}

boolean RF22ReliableDatagram::recvfromAckTimeout(uint8_t* buf, uint8_t* len, uint16_t timeout, uint8_t* from, uint8_t* to, uint8_t* id, uint8_t* flags)
{
    unsigned long starttime = millis();
    while ((millis() - starttime) < timeout)
	if (recvfromAck(buf, len, from, to, id, flags))
	    return true;
    return false;
}

uint16_t RF22ReliableDatagram::retransmissions()
{
    return _retransmissions;
}

void RF22ReliableDatagram::acknowledge(uint8_t id, uint8_t from)
{
    setHeaderId(id);
    setHeaderFlags(RF22_FLAGS_ACK);
    // We would prefer to send a zero length ACK,
    // but if an RF22 receives a 0 length message with a CRC error, it will never receive
    // a 0 length message again, until its reset, which makes everything hang :-(
    // So we send an ACK of 1 octet
    // REVISIT: should we send the RSSI for the information of the sender?
    uint8_t ack = '!';
    sendto(&ack, sizeof(ack), from); 
    waitPacketSent();
}

