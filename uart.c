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


const byte PROGMEM mapping_adc_ch[] = {BITPOS_A1, BITPOS_A2, BITPOS_A3, BITPOS_A4, BITPOS_A5, BITPOS_A6};


byte TCCR1B_new = B(WGM12) | B(CS11) | B(CS10);   // default value for 1000 Hz


void sendProgmemStr(const char *str)
{
   while(1)
   {
      char b = pgm_read_byte(str);
      if (b == 0)    return;
      
      loop_until_bit_is_set(UCSR0A, UDRE0);
      UDR0 = b;
      
      str++;
   }
}

void usart0RX(void)  // Receive UARTO Data (Bluetooth)
{
   byte cmd = UDR0;
   
   if (TCCR1B == 0)  // idle mode
      switch (cmd & 3)
      {
         case 0:  // Battery threshold definition
            batThres = BATTHRES_BASE + (cmd >> 2);  // cmd arg = 0 -> 527 = 3.4 V ; cmd arg = 63 -> 527+63 = 590 = 3.8 V
            CLR_LED_BAT;   // battery LED may turn off with new threshold value
            break;
            
         case 1: // Start live mode
         case 2: // Start simulated mode
            if ((cmd & 3) == 2) // simulated mode
            {
               simulated = true;
               simulSeq = 0;
            }
            else
               simulated = false;
            
            seq = 0;
            nChannels = 0;
            for(byte i = 0; i < 6; i++)
            {
               if (cmd & 0x04)
                  chTable[nChannels++] = simulated ? i : pgm_read_byte(mapping_adc_ch + i);
               cmd >>= 1;
            }
            TCNT1 = 0;           // reset timer 1 counter
            TCCR1B = TCCR1B_new; // start timer 1
            ledIncr = LEDINCR_FAST;
            OCR2B = 100;
            break;
            
         case 3:
            if (cmd == 7) // Send version string
               sendProgmemStr(versionStr);
            else  // Sampling rate definition
               switch (cmd & 0xC0)
               {
                  case 0x00:  // 1 Hz
                     OCR1A = 31250-1;  // f = 8e6 / 256 / 31250 = 1 Hz
                     TCCR1B_new = B(WGM12) | B(CS12);  // timer 1 CTC mode with prescaling factor of 256
                     break;
                  case 0x40:  // 10 Hz
                     OCR1A = 3125-1;  // f = 8e6 / 256 / 3125 = 10 Hz
                     TCCR1B_new = B(WGM12) | B(CS12);  // timer 1 CTC mode with prescaling factor of 256
                     break;
                  case 0x80:  // 100 Hz
                     OCR1A = 1250-1;  // f = 8e6 / 64 / 1250 = 100 Hz
                     TCCR1B_new = B(WGM12) | B(CS11) | B(CS10);  // timer 1 CTC mode with prescaling factor of 64
                     break;
                  case 0xC0:  // 1000 Hz
                     OCR1A = 125-1;  // f = 8e6 / 64 / 125 = 1000 Hz
                     TCCR1B_new = B(WGM12) | B(CS11) | B(CS10);  // timer 1 CTC mode with prescaling factor of 64
                     break;
               }
            break;
      }
   else  // live mode
   {
      if (cmd == 0)  // Stop live / simulated mode
      {
         TCCR1B = 0;  // stop Timer 1
         ledIncr = LEDINCR_SLOW;
         OCR2B = 100;
      }
      else if ((cmd & 3) == 3)   // Set digital output
         PORTB = cmd;   // DI4 and STAT_BT bits are kept high by the command bits
   }
}

ISR(USART_RX_vect, ISR_NAKED)  // Receive UARTO Data (Bluetooth)
{
   usart0RX();
   reti();
}
