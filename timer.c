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


void timer0Tick(void)
{
// Update status LED PWM duty cycle (LED intensity)
   
   static bool down = false;
   
   if (down)
   {
      OCR2B -= ledIncr;
      if (OCR2B == 0)   down = false;
   }
   else
   {
      OCR2B += ledIncr;
      if (OCR2B == 250)   down = true;
   }
   
   if ((TCCR1B == 0 || simulated) && !ISSET_LED_BAT)  // check battery value in idle or simulated mode and if battery LED not already on
   {
      ADCSRA = B(ADEN) | B(ADPS2) | B(ADPS0); // Enable ADC with 1/32 clock prescaler - fAD = 8e6/32 = 250 kHz (@8MHz)
      ADMUX = B(REFS0) | BITPOS_A6;           // ADC Reference is AVcc; right adjusted result; select BAT analog input
      ADCSRA |= B(ADSC);
      loop_until_bit_is_clear(ADCSRA, ADSC);
      if ((ADC & 0x3FF) < batThres)    SET_LED_BAT;
      ADCSRA = B(ADIF);    // Disable ADC and clear pending interrupt flag
   }
}

ISR(TIMER0_COMPA_vect, ISR_NAKED)   // Timer 0 compare match interrupt
{
   timer0Tick();
   reti();
}



ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
   if (!ISSET_STAT_BT)  // Bluetooth connection dropped -> switch to idle mode
   {
      TCCR1B = 0;  // stop Timer 1
      ledIncr = LEDINCR_SLOW;
      OCR2B = 100;
   }
   else
   {
      if (simulated)
         timer1TickSimul();
      else
         timer1TickLive();
   }
   
   reti();
}
