/**
 * A Mirf example to test the latency between two Ardunio.
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 10
 *
 * Note: To see best case latency comment out all Serial.println
 * statements not displaying the result and load
 * 'ping_server_interupt' on the server.
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
byte num = 0 ;
byte num1 = 0 ;
void setup() {
  Serial.begin(9600);
  /*
   * Setup pins / SPI.
   */

  /* To change CE / CSN Pins:
   *
   * Mirf.csnPin = 9;
   * Mirf.cePin = 7;
   */
  /*
  Mirf.cePin = 7;
  Mirf.csnPin = 8;
  */
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
   Mirf.payload = 1;      // this means length of send data
  /*
   * Configure reciving address.
   */
 
  /*
   * To change channel:
   *
   * Mirf.channel = 10;
   *
   * NB: Make sure channel is legal in your area.
   */

  Mirf.config();
  Mirf.setTADDR((byte *)"bruce");
  Mirf.setRADDR((byte *)"UNO");
  
  
  Serial.println("Beginning ... ");

}

void loop() {
  unsigned long time = millis();
 // Serial.println("get time data finish...");
//  Mirf.setTADDR((byte *)"bruce");
//  Serial.println("set target server finish...");
  if (num >= 255)
      num = 0 ;
    
    transmit((char)num)  ;
    Serial.print(num);
    Serial.print(" S    " );
    num ++ ;
   
if (!Mirf.isSending() && Mirf.dataReady()) {
        Mirf.getData((byte *) &num1);
           Serial.print("/");
          Serial.print(num1);
            Serial.print("  R ");
            Serial.print("\n");
      }
      else
      {
               Serial.print("\n");
      }
/*
  Serial.print("sending data: ");
  Serial.print(time);
  Serial.print(" to server finish...\n") ;

  while (Mirf.isSending()) {
    // wait data sending....until finished
  }

  Serial.println("data sending..finished");

  delay(10);
  while (!Mirf.dataReady()) {
    //Serial.println("Waiting");
    if ( ( millis() - time ) > 1000 ) {
      Serial.println("Timeout on response from server!");
      return;
    }
  }

  Mirf.getData((byte *) &time1);

  Serial.print("Ping: ");
  Serial.print(time1);
  //Serial.print("and cost time : ");
  //
  // Serial.print(time1 - time);
  Serial.print(" ; Get echo from server ... \n") ;

  Serial.print("==============\n\n\n") ;
  delay(1000);
  */
}

void transmit(char st)
{
  byte c; 
  
   // Serial.println("Now enter char sending");
    c = st;
    Mirf.send(&c);
    while( Mirf.isSending() ) ;
}


void transmit( float v)
{
  byte c; 
  char buf[10];
  
  dtostrf(v,9,3,buf);

  for( int i=0 ; i<8 ; i++ )
  { 
    c = buf[i];
    Mirf.send(&c);
    while( Mirf.isSending() ) ;
  }
}

// sends a string via the nRF24L01
void transmit(const char *string)
{
  byte c; 
  
  for( int i=0 ; string[i]!=0x00 ; i++ )
  { 
    c = string[i];
    Mirf.send(&c);
    while( Mirf.isSending() ) ;
  }
}



// send a CR/LF sequence via the nRF24L01
void transmitlf(void)
{
  byte c;
  
  c = '\r';
  Mirf.send(&c);
    while( Mirf.isSending() ) ;
  
  c = '\n';
  Mirf.send(&c);
    while( Mirf.isSending() ) ;
}


void waitforsending()
{
unsigned long  t1 = millis() ;
unsigned long  t2 = millis() ;
   while(Mirf.isSending())
      {
        t2 = millis() ;
            
            Serial.println(t2-t1) ;
        if ((t2-t1) >=200 )
            return ;
      }
}   

void waitforreceiving()
{
unsigned long  t1 = millis() ;
unsigned long  t2 = millis() ;
   while(Mirf.dataReady())
      {
        t2 = millis() ;
            
            Serial.println(t2-t1) ;
        if ((t2-t1) >=200 )
            return ;
      }
}   
