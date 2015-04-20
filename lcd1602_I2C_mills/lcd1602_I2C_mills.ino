//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello, world!");
}

void loop()
{
  // 將游標設到 　第一行, 　第二列
  // (注意: 　第二列第五行，因為是從 0 開始數起):
  lcd.setCursor(5, 1);
  // 列印 Arduino 重開之後經過的秒數
  lcd.print(millis() / 1000);
  Serial.println(millis() / 1000);
  delay(200);
}
