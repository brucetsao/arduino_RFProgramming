#ifndef NRF24L01_h
#define NRF24L01_h

#include "API.h"

//---------------------------------------------
#define TX_ADR_WIDTH    5   
// 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  1  
// 20 unsigned chars TX payload
//---------------------------------------------
// You can change the define pin. 
#define CE       8             
// CE_BIT:   Digital Input     Chip Enable Activates RX or TX mode

#define CSN      9
// CSN BIT:  Digital Input     SPI Chip Select

#define IRQ      10
// IRQ BIT:  Digital Output    Maskable interrupt pin
//*********************************************
#endif
