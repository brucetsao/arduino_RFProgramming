
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <VirtualWire.h>

void getRF433Data() ;

const int led_pin = 13;
const int transmit_pin = 6; //12
const int receive_pin = 7; //11
//const int transmit_en_pin = 3;

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

void setup()
{
   lcd.init();                      // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
     lcd.clear() ;
     Serial.begin(9600);	// Debugging only
    Serial.println("RF 433 Receive session Start");

    // Initialise the IO and ISR
    vw_set_tx_pin(transmit_pin);
    vw_set_rx_pin(receive_pin);
    //vw_set_ptt_pin(transmit_en_pin);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec

    vw_rx_start();       // Start the receiver PLL running

      pinMode(led_pin, OUTPUT);
       attachInterrupt(1, getRF433Data, HIGH );
}

void loop()
{      
      getRF433Data();
      Serial.println(millis() / 10);
     // delay(200);
}


void getRF433Data()
{
   if (vw_have_message() )
   {
        if (vw_get_message(buf, &buflen)) // Non-blocking
        {
       //     Serial.println("We Get Data from RF433");
    	int i;
    
            digitalWrite(led_pin, HIGH); // Flash a light to show received good message
    	// Message with a good checksum received, dump it.
    
    	  lcd.setCursor(1, 1);
    	Serial.print("Got: ");
    	    Serial.print('(');
    
    
    	for (i = 0; i < buflen; i++)
    	{
    	    Serial.print(buf[i], HEX);
    	    Serial.print('/');
    	    Serial.print(char(buf[i]));
    	    Serial.print(':');
                 lcd.print(char(buf[i]));
    	}
    	    Serial.print(') ');
              Serial.println();
            digitalWrite(led_pin, LOW);
        }
   }
  
}

