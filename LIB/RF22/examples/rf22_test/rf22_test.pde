// rf22_test.pde
// -*- mode: C++ -*-
//
// Test code used during library development, showing how
// to do various things, and how to call various functions

#include <SPI.h>
#include <RF22.h>

// Singleton instance of the radio
RF22 rf22;

void setup() 
{
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
}

void
test_read_write()
{
  uint8_t val;
  rf22.spiWrite(RF22_REG_42_CHECK_HEADER0, 10);
  val = rf22.spiRead(RF22_REG_42_CHECK_HEADER0);
  Serial.println(val, DEC);

  rf22.spiWrite(RF22_REG_42_CHECK_HEADER0, 5);
  val = rf22.spiRead(RF22_REG_42_CHECK_HEADER0);
  Serial.println(val, DEC);
  Serial.println("-----");
}

void
test_adc()
{
  uint8_t val = rf22.adcRead(RF22_ADCSEL_GND);
  Serial.println(val, HEX);
}

void test_temp()
{
  uint8_t val = rf22.temperatureRead();
  Serial.println(val, HEX);
}

void
test_burst_read()
{
  uint8_t buf[127];
  rf22.spiBurstRead(0, buf, sizeof(buf));
  uint8_t i;
  for (i = 0; i < 127; i++)
  {
    Serial.print(i, HEX);
    Serial.print(": ");
    Serial.println(buf[i], HEX);
  }
}

void test_burst_write()
{
  uint8_t buf[] = "Hello";
  rf22.spiBurstWrite(RF22_REG_7F_FIFO_ACCESS, buf, sizeof(buf));
}

void
test_wut()
{
  Serial.println("WUT Start");
  rf22.setWutPeriod(10000); // 10000, 0, 0 -> 1 secs
  rf22.spiWrite(RF22_REG_07_OPERATING_MODE1, RF22_ENWT);
  // Wait for the interrupt to occur
  while (1)
  {
    uint8_t val = rf22.spiRead(RF22_REG_04_INTERRUPT_STATUS2);
    if (val & RF22_IWUT)
      break;
  }
  rf22.spiWrite(RF22_REG_07_OPERATING_MODE1, 0);
  
  Serial.println("WUT Interrupt bit detected OK");
}

void
test_set_frequency()
{
  if (!rf22.setFHStepSize(0))
      Serial.println("setFHStepSize 1 Fail");
  if (!rf22.setFHChannel(0))
      Serial.println("setFHChannel 1 Fail");
  if (!rf22.setFrequency(434.0))
    Serial.println("setFrequency 1 Fail");
  if (!rf22.setFrequency(240.0))
    Serial.println("setFrequency 2 Fail");
  if (!rf22.setFrequency(929.9999)) // Higher than this produces FREQERR on my 06 silicon
    Serial.println("setFrequency 3 Fail");
  if (rf22.setFrequency(960.1)) // Should fail
    Serial.println("setFrequency 4 Fail");

  Serial.println("-------------");
}

void
test_rssi()
{
  rf22.setModeRx();
  
  float f = 430.0;
  while (f < 435.0)
  {
    rf22.setFrequency(f);
    delayMicroseconds(200); // Wait for freq to settle
    uint8_t rssi = rf22.rssiRead();
    Serial.print(f);
    Serial.print(": ");
    Serial.println(rssi, DEC);
    f += 1.0;
  }
  rf22.setModeIdle();
  Serial.println("-------------");
}

// Sends 0.5 secs of PN9 modulated with with GFSK at full power
void test_tx_pn9()
{
  rf22.setFrequency(434.0);
  rf22.setTxPower(RF22_TXPOW_17DBM);  
  rf22.setModeRx();
  // Takes a little while to start
  delay(1);
  Serial.println(rf22.statusRead(), HEX);
  rf22.setModemConfig(RF22::FSK_PN9_Rb2Fd5);
//  rf22.setModemConfig(RF22::UnmodulatedCarrier);
  rf22.setModeTx(); // Turns off Rx
  // Takes a little while to start
  delay(1);
  Serial.println(rf22.statusRead(), HEX);
  delay(500);
  rf22.setModeIdle();
  // Takes a little while to turn off the transmitter
  delay(10);
  Serial.println(rf22.statusRead(), HEX);
  Serial.println("-------------");
  delay(500);
}

// Connect analog test points to GPIO1 and GPIO2 
void test_analog_gpio()
{
  rf22.setFrequency(434.0);
  rf22.setModeRx();
  // GPIO1 for Test N output
  rf22.spiWrite(RF22_REG_0C_GPIO_CONFIGURATION1, 0x0c);
  // GPIO2 for Test P output
  rf22.spiWrite(RF22_REG_0D_GPIO_CONFIGURATION2, 0x0d);
  rf22.spiWrite(RF22_REG_50_ANALOG_TEST_BUS_SELECT, 15); // Detector?
  while (1);
}

void test_modem_config()
{
  if (rf22.setModemConfig((RF22::ModemConfigChoice)255)) // Should fail
    Serial.println("setModemConfig 1 failed");
  if (!rf22.setModemConfig(RF22::FSK_Rb2Fd5))
    Serial.println("setModemConfig 2 failed");
  if (!rf22.setModemConfig(RF22::FSK_Rb2_4Fd36))
    Serial.println("setModemConfig 3 failed");
  if (!rf22.setModemConfig(RF22::GFSK_Rb2_4Fd36))
    Serial.println("setModemConfig 3 failed");
  if (!rf22.setModemConfig(RF22::OOK_Rb40Bw335))
    Serial.println("setModemConfig 4 failed");

  Serial.println("-------------");
}

// This works with test_rx below
void test_tx()
{
  if (!rf22.setFrequency(434.0))
    Serial.println("setFrequency failed");
  if (!rf22.setModemConfig(RF22::GFSK_Rb2Fd5))
    Serial.println("setModemConfig failed");
  uint8_t data[] = "hello";
  // 255 octets:
//  uint8_t data[] = "12345678901234567890123456789012345678901234567890123456789012312345678901234567890123456789012345678901234567890123456789012312345678901234567890123456789012345678901234567890123456789012312345678901234567890123456789012345678901234567890123456789012345";
  rf22.send(data, sizeof(data));
  rf22.waitPacketSent();
  Serial.println("-------------");

}

// This works with test_tx above
void test_rx()
{
  if (!rf22.setFrequency(434.0))
    Serial.println("setFrequency failed");
  if (!rf22.setModemConfig(RF22::GFSK_Rb2Fd5))
    Serial.println("setModemConfig failed");
  
  while (1)
  {
    uint8_t buf[RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf22.recv(buf, &len)) // Should fail, no message available
      Serial.println("recv 1 failed");

    rf22.waitAvailable();
    if (rf22.recv(buf, &len))
    {
       Serial.print("got one in user: ");
       Serial.println((char*)buf);
    }
    else
    {
       Serial.print("recv 2 failed");
    }
  }
}

void loop()
{
//  test_read_write();
//  test_adc();
//  test_temp();
//  test_burst_read();  
//  test_burst_write();
//  test_wut();
//  test_set_frequency();
//  test_rssi();
  test_tx_pn9();
//  test_analog_gpio();
//  test_modem_config();
//  test_tx();
//  test_rx();
  
//  while (1);
  delay(1000);
}

