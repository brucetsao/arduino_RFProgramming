/*
 * UdpString.h: Library to send/receive UDP packets with the Arduino ethernet shield
 * using Tom Igoe/Hernando Barragan's WString library.
 * Drop UdpString.h/.cpp from this distribution into the Ethernet library directory at 
 * hardware/libraries/Ethernet/ 
 * Then copy directory /libraries/String from this distribution to /hardware/libraries/String in
 * your Arduino installation.
 *
 * NOTE: UDP is fast, but has some important limitations (thanks to Warren Gray for mentioning these)
 * 1) UDP does not guarantee the order in which assembled UDP packets are received. This
 * might not happen often in practice, but in larger network topologies, a UDP
 * packet can be received out of sequence. 
 * 2) UDP does not guard against lost packets - so packets *can* disappear without the sender being
 * aware of it. Again, this may not be a concern in practice on small local networks.
 * For more information, see http://www.cafeaulait.org/course/week12/35.html
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#ifndef UdpString_h
#define UdpString_h

#include "WString.h"



class UdpStringClass {
private:
	uint8_t _sock;  // socket ID for Wiz5100
	uint16_t _port; // local port to listen on
public:
	void begin(uint16_t);				// initialize, start listening on specified port
	int available();								// has data been received?

	// C-style buffer-oriented functions
	uint16_t sendPacket(uint8_t *, uint16_t, uint8_t *, uint16_t); //send a packet to specified peer 
	uint16_t sendPacket(const char[], uint8_t *, uint16_t);  //send a string as a packet to specified peer
	uint16_t readPacket(uint8_t *, uint16_t);		// read a received packet 
	uint16_t readPacket(uint8_t *, uint16_t, uint8_t *, uint16_t *);		// read a received packet, also return sender's ip and port 
	
	// WString functions
	int sendPacket(String str, byte * ip, unsigned int port);
	int sendPacket(String str, int length, byte * ip, unsigned int port);
	int readPacket(String &str);
	int readPacket(String &str, byte * ip, unsigned int * port);
};

extern UdpStringClass UdpString;

#endif
