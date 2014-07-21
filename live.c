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


word batVal;


EMPTY_INTERRUPT(ADC_vect);             // Used only for wake up after internal ADC conversion

word convADC10(byte channel)  // 10-bit ADC conversion
{
   ADMUX = B(REFS0) | channel;         // ADC Reference is AVcc; right adjusted result; select analog input
   
   __asm volatile("sleep");   // autostart a new ADC conversion
   
   if (channel == BITPOS_A6) // BAT
   {
      batVal = ADC & 0x3FF;
      return batVal;
   }
   
   return ADC & 0x3FF;    // internal ADC value read
}

void timer1TickLive(void)
{
   // Acquire and send a new data frame
   
	byte frame[7];
   
   /*
   static byte t1 = 0;
   byte t0 = TCNT1;
   */
   
   // Disable all interrupts except ADC
   UCSR0B = B(RXEN0) | B(TXEN0);          // Disable USART0_RX interrupt
   TIMSK0 = 0;                            // Disable TIMER 0 compare match interrupt
   TIMSK1 = 0;                            // Disable TIMER 1 compare match interrupt
   
   sei();   // To enable ADC interrupt
   
   batVal = 0x8000;
   
   memset(frame, 0, 7);
   
   if (ISSET_DI1)  frame[6] |= 0x80;
   if (ISSET_DI2)  frame[6] |= 0x40;
   if (ISSET_DI3)  frame[6] |= 0x20;
   if (ISSET_DI4)  frame[6] |= 0x10;
   
   ADCSRA = B(ADEN) | B(ADIE) | B(ADPS2) | B(ADPS0); // Enable ADC with interrupts and 1/32 clock prescaler - fAD = 8e6/32 = 250 kHz (@8MHz)
   
   *(word*)(frame+5) |= convADC10(chTable[0]) << 2;
   if (nChannels > 1)
      *(word*)(frame+4) |= convADC10(chTable[1]);
   if (nChannels > 2)
      *(word*)(frame+2) |= convADC10(chTable[2]) << 6;
   if (nChannels > 3)
      *(word*)(frame+1) |= convADC10(chTable[3]) << 4;
   if (nChannels > 4)
      *(word*)frame |= (convADC10(chTable[4]) & 0x3F0) << 2;
   if (nChannels > 5)
      frame[0] |= convADC10(chTable[5]) >> 4;
   
   if (batVal == 0x8000 && !ISSET_LED_BAT)
      convADC10(BITPOS_A6);   // read battery value if not read before and if battery LED not already on
   
   ADCSRA = 0;    // Disable ADC
   
   /*
   *(word*)(frame+5) = t0 << 2;
   frame[4] = TCNT1;
   *(word*)(frame+2) = ((word)t1) << 6;
   */
   
   sendFrameCRC(frame);
   
   if (batVal < batThres)     SET_LED_BAT;
   
   // Re-enable all other interrupts (when this ISR ends)
   cli();                                    // Disable Global Interrupt enable bit
   UCSR0B = B(RXCIE0) | B(RXEN0) | B(TXEN0); // Reenable USART0_RX interrupt
   TIMSK0 = B(OCIE0A);                       // Reenable TIMER 0 compare match interrupt
   TIMSK1 = B(OCIE1A);                       // Reenable TIMER 1 compare match interrupt
   
   //t1 = TCNT1;
}
