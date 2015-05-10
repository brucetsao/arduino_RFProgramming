/**
 * An Mirf example which copies back the data it recives.
 *
 * Arduino UNO Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
  * Arduino MEGA 2560 use Pins:
 * Hardware SPI:
 * MISO -> 50
 * MOSI -> 51
 * SCK -> 52
 * Configurable:
 * CSN -> 7
* CE -> 8
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display


#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#define ledpin 7 
byte recvdata ;

void setup() {
    //  pinMode(ledpin,OUTPUT) ;
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("RF Monitor!");
  /*
   * Set the SPI Driver.
   */


  Mirf.spi = &MirfHardwareSpi;
  /*
   * Setup pins / SPI.
   */

  Mirf.init();

  /*
   * Configure reciving address.
   */

  Mirf.setRADDR((byte *)"bruce");
   Mirf.setTADDR((byte *)"UNO");
  /*
   * Set the payload length to sizeof(unsigned long) the
   * return type of millis().
   *
   * NB: payload on client and server must be the same.
   */

 Mirf.payload =1;

  /*
   * Write channel and payload config then power up reciver.
   */

  Mirf.config();

  Serial.println("Listening...");
  pinMode(ledpin,OUTPUT) ;
}

void loop() {
    lcd.setCursor(0, 1) ;
     lcd.print("               ");
      lcd.setCursor(0, 1) ;
      lcd.print(millis()) ;

  //  Serial.println(millis()) ;
  /*
   * A buffer to store the data.
   */


  // byte data[Mirf.payload];
  //  Serial.println("now looping...");
  /*
   * If a packet has been recived.
   *
   * isSending also restores listening mode when it
   * transitions from true to false.
   */
Serial.println("Now waiting Get Data");
if (!Mirf.isSending() && Mirf.dataReady()) {
  //  if(Mirf.dataReady()){
   Serial.println("Get packet before");

    /*
     * Get load the packet into the buffer.
     */

       Mirf.getData(&recvdata);
   Serial.println("GOt packet after");

    /*
     * Set the send address.
     */
    lcd.setCursor(0, 2) ;
     lcd.print("               ");
      lcd.setCursor(0, 2) ;
      lcd.print(millis()) ;
    lcd.setCursor(0, 3) ;
     lcd.print("               ");
      lcd.setCursor(2, 3) ;
      lcd.print(recvdata) ;
      /*
    for (int i = 0 ; i < Mirf.payload; i++)
    {
      lcd.setCursor(2+(i)*4, 2) ;
      lcd.print(data[i],HEX) ;
   //   lcd.print((unsigned long)*data) ;   // use for unsign long
    }
    */
    
    //       delay(10);


  }
}

void messagecoming()
{
 
  digitalWrite(ledpin, HIGH) ;
  delay(3);
  digitalWrite(ledpin, LOW) ; 
  delay(3);

}


