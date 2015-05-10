// GenericSPI.h
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2011 Mike McCauley
// Contributed by Joanna Rutkowska
// $Id: GenericSPI.h,v 1.1 2014/04/01 05:06:44 mikem Exp mikem $

#ifndef GENERICSPI_H
#define GENERICSPI_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <wiring.h>
#include "pins_arduino.h"
#endif

/////////////////////////////////////////////////////////////////////
/// \class GenericSPIClass GenericSPI.h <GenericSPI.h>
/// \brief Base class for SPI interfaces
///
/// This generic abstract class is used to encapsulate hardware or software SPI interfaces.
/// The intention is so that the main RF22 class can be configured to use hardware or software SPI
/// without changing the main code.
///
/// You must provide a subclass of this class to RF22 constructor.
/// A concrete subclass theat encapsualtes the standard Arcuino hardware SPI is provided

class GenericSPIClass 
{
public:

    /// Transfer a single octet to and from the SPI interface
    /// \param[in] data The octet to send
    /// \return The octet read from SPI while the data octet was sent
    virtual uint8_t transfer(uint8_t data) = 0;

    // SPI Configuration methods
    /// Enable SPI interrupts (if supported)
    /// This can be used in an SPI slave to indicate when an SPI message has been received
    virtual void attachInterrupt() {};

    /// Disable SPI interrupts (if supported)
    /// This can be used to diable the SPI interrupt in slaves where that is supported.
    virtual void detachInterrupt() {};

    /// Initialise the SPI library.
    /// Call this before configuring or using the SPI library
    virtual void begin() {};

    /// Disables the SPI bus (leaving pin modes unchanged). 
    /// Call this after you have finished using the SPI interface
    virtual void end() {};

    /// Sets the bit order the SPI interface will use
    /// Sets the order of the bits shifted out of and into the SPI bus, either 
    /// LSBFIRST (least-significant bit first) or MSBFIRST (most-significant bit first). 
    /// \param[in] bitOrder Bit order to be used: LSBFIRST or MSBFIRST
    virtual void setBitOrder(uint8_t bitOrder) {};

    /// Sets the SPI data mode: that is, clock polarity and phase. 
    /// See the Wikipedia article on SPI for details. 
    /// \param[in] mode The mode to use: SPI_MODE0 SPI_MODE1 SPI_MODE2 SPI_MODE3 
    virtual void setDataMode(uint8_t mode) {};

    /// Sets the SPI clock divider relative to the system clock. 
    /// On AVR based boards, the dividers available are 2, 4, 8, 16, 32, 64 or 128. 
    /// The default setting is SPI_CLOCK_DIV4, which sets the SPI clock to one-quarter 
    /// the frequency of the system clock (4 Mhz for the boards at 16 MHz). 
    /// \param[in] rate The data rate to use: one of SPI_CLOCK_
    virtual void setClockDivider(uint8_t rate) {};
};
#endif
