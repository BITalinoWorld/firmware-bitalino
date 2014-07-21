/**
 * \file
 * \copyright  Copyright 2014 PLUX - Wireless Biosignals, S.A.
 * \author     Filipe Silva
 * \version    1.0
 * \date       July 2014
 *
 * \section LICENSE
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


#include "main.h"

#include <util/delay.h>


const char PROGMEM cmdName[] = "AT+NAMEbitalino";
const char PROGMEM cmdBaud[] = "AT+BAUD8";


uint8_t sampleADC(uint8_t ch)
{
   ADMUX = B(REFS0) | B(ADLAR) | ch;         // ADC Reference is AVcc; left adjusted result; select analog input
   ADCSRA |= B(ADSC);
   loop_until_bit_is_clear(ADCSRA, ADSC);
   return ADCH;
}

void testAndConfigure()
{
   // 1: Test LEDs and send results to test system through SPI
   
   // SPI IOs setup
   SET_DO1;   // set SS output high
   DDRB = BIT_DO1 | BIT_DO2 | BIT_DO4; // Set output for SS, MOSI and SCK
   
   // Internal ADC setup
   DIDR0 = B(ADC5D) | B(ADC4D);             // Digital Input Disable ADC5/SCL and ADC4/SDA
   ADCSRA = B(ADEN) | B(ADPS2) | B(ADPS1);   // Enable ADC with 1/64 prescaler - fAD = 8e6/64 = 125 kHz @ 8MHz
   
   // SPI module setup to transmit data to test system
   SPCR = B(SPE) | B(MSTR) | B(SPR1); // Enable SPI master mode, SCK freq = fosc/64 = 125 kHz @ 8 MHz
   
   CLR_DO1;   // set SS output low
   
   uint8_t valLdrBat = sampleADC(4);
   uint8_t valLdrSta = sampleADC(5);
   
   // send LDR values with both LEDs off
   SPDR = valLdrBat;
   loop_until_bit_is_set(SPSR, SPIF);
   SPDR = valLdrSta;
   loop_until_bit_is_set(SPSR, SPIF);
   
   SET_LED_BAT;
   
   _delay_ms(100); // wait 100 ms for the LDR to stabilize
   
   valLdrBat = sampleADC(4);
   valLdrSta = sampleADC(5);
   
   // send LDR values with battery LED on
   SPDR = valLdrBat;
   loop_until_bit_is_set(SPSR, SPIF);
   SPDR = valLdrSta;
   loop_until_bit_is_set(SPSR, SPIF);
   
   CLR_LED_BAT;
   SET_LED_STAT;
   
   _delay_ms(100); // wait 100 ms for the LDRs to stabilize
   
   valLdrBat = sampleADC(4);
   valLdrSta = sampleADC(5);
   
   ADCSRA = B(ADIF);    // Disable ADC and clear pending interrupt flag
   
   if (!ISSET_GP)   // GP should be high by now
      valLdrBat = 0xFF;   // identify GP error
   
   // send LDR values with status LED on
   SPDR = valLdrBat;
   loop_until_bit_is_set(SPSR, SPIF);
   SPDR = valLdrSta;
   loop_until_bit_is_set(SPSR, SPIF);

   SPCR = 0;   // Disable SPI
   
   
   // 2: Configure Bluetooth module
   
   // USART setup at 9600 baud (default in Bluetooth module)
   UBRR0 = 51;        // = [F_CPU / (16*baud)] - 1  (9600 bps @ 8MHz)
   UCSR0B = B(TXEN0); // Enable TX and keep default 8n1 serial data format
   
   _delay_ms(1000); // Bluetooth module startup delay
   sendProgmemStr(cmdName);
   
   _delay_ms(1000); // Bluetooth module delay between commands
   sendProgmemStr(cmdBaud);

   _delay_ms(1000); // Bluetooth module delay between commands

   CLR_LED_STAT;
}
