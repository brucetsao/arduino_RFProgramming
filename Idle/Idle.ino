/*
Enerlib: easy-to-use wrapper for AVR's Sleep library.
By E.P.G. - 11/2010 - Ver. 1.0.0

Example showing how to enter in Idle mode and exit from it with INT0.
*/

#include <Enerlib.h>

Energy energy;

void INT0_ISR(void)
{

  /*
  The WasSleeping function will return true if Arduino
  was sleeping before the IRQ. Subsequent calls to
  WasSleeping will return false until Arduino reenters
  in a low power state. The WasSleeping function should
  only be called in the ISR.
  */
  if (energy.WasSleeping())
  {
    /*
    Arduino was waked up by IRQ.

    If you shut down external peripherals before sleeping, you
    can reinitialize them here. Look on ATMega's datasheet for
    hardware limitations in the ISR when microcontroller just
    leave any low power state.
    */
  }
  else
  {
    /*
    The IRQ happened in awake state.

    This code is for the "normal" ISR.
    */
  }
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Program Start") ;
  attachInterrupt(0, INT0_ISR, LOW);
  /*
  Pin 2 will be the "wake button". Due to uC limitations,
  it needs to be a level interrupt.
  For experienced programmers:
    ATMega's datasheet contains information about the rest of
    wake up sources. The Extended Standby is not implemented.
  */

   Serial.println("Now I am Sleeping") ;
   delay(500);
  energy.Idle();
}

void loop()
{

   Serial.println("I am waken ") ;
  delay(1000);
}

