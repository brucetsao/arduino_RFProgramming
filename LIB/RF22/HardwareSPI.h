// HardwareSPI.h
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2011 Mike McCauley
// Contributed by Joanna Rutkowska
// $Id: HardwareSPI.h,v 1.1 2014/04/01 05:06:44 mikem Exp mikem $

#include "GenericSPI.h"

// These defs cause trouble on some versions of Arduino
#undef round
#undef double

#include <SPI.h>


/////////////////////////////////////////////////////////////////////
/// \class HardwareSPIClass HardwareSPI.h <HardwareSPI.h>
/// \brief Encapsulate the Arduino hardware SPI interface
///
/// This concrete subclass of GenericSPIClass ncapsulates the standard Arduino hardware SPI interface
class HardwareSPIClass : public GenericSPIClass 
{

public:

    /// Transfer a single octet to and from the SPI interface
    /// \param[in] data The octet to send
    /// \return The octet read from SPI while the data octet was sent
    uint8_t transfer(uint8_t data) 
    {
        return SPI.transfer(data);
    }

    // SPI Configuration methods
    /// Enable SPI interrupts
    /// This can be used in an SPI slave to indicate when an SPI message has been received
    /// It will cause the SPI_STC_vect interrupt vectr to be executed
    void attachInterrupt() 
    {
        return SPI.attachInterrupt();
    }

    /// Disable SPI interrupts
    /// This can be used to diable the SPI interrupt in slaves where that is supported.
    void detachInterrupt() 
    {
        return SPI.detachInterrupt();
    }
    
    /// Initialise the SPI library
    /// Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high. 
    void begin() 
    {
        return SPI.begin();
    }

    /// Disables the SPI bus (leaving pin modes unchanged). 
    /// Call this after you have finished using the SPI interface
    void end() 
    {
        return SPI.end();
    }

    /// Sets the bit order the SPI interface will use
    /// Sets the order of the bits shifted out of and into the SPI bus, either 
    /// LSBFIRST (least-significant bit first) or MSBFIRST (most-significant bit first). 
    /// \param[in] bitOrder Bit order to be used: LSBFIRST or MSBFIRST
    void setBitOrder(uint8_t bitOrder) 
    {
        SPI.setBitOrder (bitOrder);
    }

    /// Sets the SPI data mode: that is, clock polarity and phase. 
    /// See the Wikipedia article on SPI for details. 
    /// \param[in] mode The mode to use: SPI_MODE0 SPI_MODE1 SPI_MODE2 SPI_MODE3 
    void setDataMode(uint8_t mode) 
    {
        SPI.setDataMode (mode);
    }

    /// Sets the SPI clock divider relative to the system clock. 
    /// On AVR based boards, the dividers available are 2, 4, 8, 16, 32, 64 or 128. 
    /// The default setting is SPI_CLOCK_DIV4, which sets the SPI clock to one-quarter 
    /// the frequency of the system clock (4 Mhz for the boards at 16 MHz). 
    /// \param[in] rate The data rate to use: one of SPI_CLOCK_
    void setClockDivider(uint8_t rate) 
    {
        SPI.setClockDivider (rate);
    }
};

extern HardwareSPIClass Hardware_spi;
