// rf22_specan
// A simple spectrum analyser for the RF22
// Uses the RSSI measurement to plot signal strength
// against frequency
// Specify the start and and requencies and the step size below.
// The output is suitable for a VT100 terminal emulator
// Note the baud rate is set to 115200 for better performance,
// but you can change this to suit your needs
//
// TO DO: add some interactivity
// Copyright Mike McCauley
#include <RF22.h>
#include <SPI.h>

// Singleton instance of the radio
RF22 rf22;

void setup() 
{
  Serial.begin(115200);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

float start = 395.0;
float end = 396.0;
float step = 0.02;

void loop()
{
  rf22.setModeRx();
  float freq;
  Serial.println("\033[H"); // Home
  for (freq = start; freq < end; freq += step)
  {
    rf22.setFrequency(freq);
    delay(10); // Let the freq settle
    uint8_t rssi = rf22.rssiRead();
    uint8_t stars = rssi / 8;

    Serial.print(freq);
    Serial.print(": ");
//    Serial.print(rssi, DEC);
    uint8_t i;
    for (i = 0; i < stars; i++)
      Serial.print('*');
    Serial.print("\033[K"); // DElete to EOL
    Serial.println("");
  }  
  Serial.println("------------------");
}
