/**************************************************************************
*
*  ArduinoCC3000Test.ino - Basic connection test between the TI CC3000
*                          and an Arduino.
*
*  Version 1.0
* 
*  Copyright (C) 2013 Chris Magagna - cmagagna@yahoo.com
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  Don't sue me if my code blows up your board and burns down your house
*
****************************************************************************



To connect an Arduino to the CC3000 you'll need to make these 6 connections
(in addition to the WiFi antenna, power etc).

Name / pin on CC3000 module / pin on CC3000EM board / purpose

SPI_CS     / 12 / J4-8 /  SPI Chip Select
                          The Arduino will set this pin LOW when it wants to 
                          exchange data with the CC3000. By convention this is
                          Arduino pin 10, but any pin can be used. In this
                          program it will be called WLAN_CS

SPI_DOUT   / 13 / J4-9 /  Data from the module to the Arduino
                          This is Arduino's MISO pin, and is how the CC3000
                          will get bytes to the Arduino. For most Arduinos
                          MISO is pin 12

SPI_IRQ    / 14 / J4-10 / CC3000 host notify
                          The CC3000 will drive this pin LOW to let the Arduino
                          know it's ready to send data. For a regular Arduino
                          (Uno, Nano, Leonardo) this will have to be connected
                          to pin 2 or 3 so you can use attachInterrupt(). In
                          this program it will be called WLAN_IRQ

SPI_DIN    / 15 / J4-11   Data from the Arduino to the CC3000
                          This is the Arduino's MOSI pin, and is how the Arduino
                          will get bytes to the CC3000. For most Arduinos
                          MOSI is pin 11

SPI_CLK    / 17 / J4-12   SPI clock
                          This is the Arduino's SCK pin. For most Arduinos
                          SCK is pin 13

VBAT_SW_EN / 26 / J5-5    Module enable
                          The Arduino will set this pin HIGH to turn the CC3000
                          on. Any pin can be used. In this program it will be
                          called WLAN_EN
                          
                          
WARNING #1: The CC3000 runs at 3.6V maximum so you can't run it from your
regular 5V Arduino power pin. Run it from 3.3V!


WARNING #2: When transmitting the CC3000 will use up to 275mA current. Most
Arduinos' 3.3V pins can only supply up to 50mA current, so you'll need a 
separate power supply for it (or a voltage regulator like the LD1117V33
connected to your Arduino's 5V power pin).


WARNING #3: The CC3000's IO pins are not 5V tolerant. If you're using a 5V
Arduino you will need a level shifter to convert these signals to 3.3V
so you don't blow up the module. 

You'll need to shift the pins for WLAN_CS, MOSI, SCK, and WLAN_EN. The other
2 pins (WLAN_IRQ and MISO) can be connected directly because they're input
pins for the Arduino, and the Arduino can read 3.3V signals directly.

You can use a level shifter chip like the 74LVC245 or TXB0104 or you can use
a pair of resistors to make a voltage divider like this:

Arduino pin -----> 560 Ohm -----> 1K Ohm -----> GND
                             |
                             |
                             +---> CC3000 pin


****************************************************************************/




#include <SPI.h>


#define WLAN_CS		10 // OK to change this

#define WLAN_IRQ	2 // Must be a pin supported by your Arduino's
                      // attachInterrupt() function
                      
#define WLAN_ENABLE	7 // OK to change this




/* I used the Teensy 3.0 to get the Arduino CC3000 library working but the
   Teensy's hardware SPI and the CC3000's SPI didn't like each other so I had
   to send the bits manually. For the Uno, Nano, etc. you can probably leave
   this unchanged. If your Arduino can't talk to the CC3000 and you're sure
   your wiring is OK then try changing this. */
   
#define USE_HARDWARE_SPI	true










/* These bytes and the startup protocol are taken from:

   http://processors.wiki.ti.com/index.php/CC3000_Serial_Port_Interface_(SPI)
*/


#define NUMBYTES	10

// The Arduino will send "HCI_CMND_SIMPLE_LINK_START"
byte send1[NUMBYTES]     = {0x01, 0x00, 0x05, 0x00, 0x00, 0x01, 0x00, 0x40, 0x01, 0x00};
byte expected1[NUMBYTES] = {0x02, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// The CC3000 will reply "HCI_CMND_SIMPLE_LINK_START command complete"
byte send2[NUMBYTES]     = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};
byte expected2[NUMBYTES] = {0x02, 0x00, 0x00, 0x00, 0x05, 0x04, 0x00, 0x40, 0x01, 0x00};

// These will hold what we actually received, so we can test
byte receive1[NUMBYTES], receive2[NUMBYTES];
















/* This sends a byte of data from the Arduino to the CC3000 and gets one back.
   If USE_HARDWARE_SPI is true the it will use the Arduino's hardware SPI,
   otherwise it hand crafts the bits and sends them with loving grace.
   
   For maximum performance it uses the digital*Fast library. If you don't
   have this library you can uncomment the 2 lines below, so it uses
   the regular digitalWrite & digitalRead routines
*/

#define digitalReadFast(pin)			digitalRead(pin)
#define digitalWriteFast(pin,state)		digitalWrite(pin,state)


byte SPIPump(byte data) {

	if (USE_HARDWARE_SPI) {
		return(SPI.transfer(data));
		}
	
	byte receivedData=0;
	
	for (int8_t i=7; i>=0; i--) {
	
		receivedData <<= 1;
	
		if (data & (1<<i)) {
			digitalWriteFast(MOSI, HIGH);
			}
		else {
			digitalWriteFast(MOSI, LOW);
			}
			
		digitalWriteFast(SCK, HIGH);
		delayMicroseconds(1);	// This is the smallest Arduino-supported delay.
		                        // if you know how your chip's timing works you
		                        // can change this to a bunch of these
		                        //
                                // asm volatile("nop");
                                //
                                // to speed things up (up to the CC3000's 
                                // maximum SPI speed of 16 Mhz).
			
		digitalWriteFast(SCK, LOW);
		
		if (digitalReadFast(MISO)) {
			receivedData |= 1;
			}
		
		delayMicroseconds(1);
		//asm volatile("nop");
		
		}
			
	return(receivedData);
	}








void setup(void) {

	Serial.begin(9600);

	if (USE_HARDWARE_SPI) {
		SPI.begin();
		SPI.setDataMode(SPI_MODE1); // TI's documentation says the CC3000 wants
		                            // CPOL=0 CPHA=1, which for Arduino is MODE1
		SPI.setBitOrder(MSBFIRST);
		SPI.setClockDivider(SPI_CLOCK_DIV2); // Slow this down if your Arduino
		                                     // runs faster than 16 Mhz
		}
		
	else {
		pinMode(MOSI, OUTPUT);
		digitalWrite(MOSI, LOW);
		
		pinMode(SCK, OUTPUT);
		digitalWrite(SCK, LOW);
		
		pinMode(MISO, INPUT);	// Teensy's pins don't default to input, so we have to declare this here
		}

	pinMode(WLAN_CS, OUTPUT);
	digitalWrite(WLAN_CS, HIGH);	// turn off CS on powerup
	pinMode(WLAN_IRQ, INPUT_PULLUP);
	
	// The Teensy 3.0 doesn't reboot when you open the serial port, like a
	// regular Arduino, so I added this so the Arduino waits for me
	// before starting.
	do {
		Serial.println("Waiting for you before starting...");
		delay(1000);
		} while (!Serial.available());
	Serial.read();
	
	// WLAN_ENABLE is still high Z, so the CC3000 should be shut off. Confirm this by
	// making sure WLAN_IRQ is high (via the internal pullup)
	Serial.println("Confirming WLAN_IRQ is high...");
	while (digitalRead(WLAN_IRQ)!=HIGH);
	
	// Turn the CC3000 on
	Serial.println("Turning CC3000 on...");
	pinMode(WLAN_ENABLE, OUTPUT);
	digitalWrite(WLAN_ENABLE, HIGH);
	
	// Now wait until WLAN_IRQ goes low, which means the CC3000 has booted
	Serial.println("Waiting for the CC3000 to boot...");
	while (digitalRead(WLAN_IRQ)!=LOW);
	
	// TI says we now set CS low, wait 50 microseconds, send the first 4 bytes,
	// wait another 50 microseconds, then send the rest of the bytes
	
	digitalWrite(WLAN_CS, LOW);
	delayMicroseconds(50);
	
	for (int i=0; i<4; i++) {
		receive1[i] = SPIPump(send1[i]);
		}
	delayMicroseconds(50);
	for (int i=4; i<NUMBYTES; i++) {
		receive1[i] = SPIPump(send1[i]);
		}
	
	// We're done writing the string, so we can set CS back to high
	digitalWrite(WLAN_CS, HIGH);
	
	// Now wait for the CC3000 to set WLAN_IRQ high, meaning it ack'd the first packet
	while(digitalRead(WLAN_IRQ)!=HIGH);
	
	// Now wait for the CC3000 to set WLAN_IRQ low, meaning it wants to send us data
	while(digitalRead(WLAN_IRQ)!=LOW);
	
	// Read the data from the CC 3000
	
	digitalWrite(WLAN_CS, LOW);
	
	for (int i=0; i<NUMBYTES; i++) {
		receive2[i] = SPIPump(send2[i]);
		}
	
	digitalWrite(WLAN_CS, HIGH);
	
	// Wait for WLAN_IRQ to go high again, meaning it's done sending data
	while(digitalRead(WLAN_IRQ)!=HIGH);
	
	// Finally, let's see what we got
	
	byte problem1 = false;
	Serial.println();
	Serial.println("Testing first packet:");
	Serial.print("Expected: ");
	for (int i=0; i<NUMBYTES; i++) {
		Serial.print(expected1[i], HEX);
		Serial.print(" ");
		}
	Serial.println();
	Serial.print("Received: ");
	for (int i=0; i<NUMBYTES; i++) {
		Serial.print(receive1[i], HEX);
		Serial.print(" ");
		if (receive1[i]!=expected1[i]) {
			problem1=true;
			}
		}
	Serial.println();
	if (problem1) {
		Serial.println("PROBLEMS WITH THE FIRST PACKET!");
		}
	
	byte problem2 = false;
	Serial.println();
	Serial.println("Testing second packet:");
	Serial.print("Expected: ");
	for (int i=0; i<NUMBYTES; i++) {
		Serial.print(expected2[i], HEX);
		Serial.print(" ");
		}
	Serial.println();
	Serial.print("Received: ");
	for (int i=0; i<NUMBYTES; i++) {
		Serial.print(receive2[i], HEX);
		Serial.print(" ");
		if (receive2[i]!=expected2[i]) {
			problem2=true;
			}
		}
	Serial.println();
	if (problem2) {
		Serial.println("PROBLEMS WITH THE SECOND PACKET!");
		}
		
	if (!problem1 && !problem2) {
		Serial.println();
		Serial.print("ALL TESTS PASSED!!!");
		}
	}





void loop(void) {
	}
