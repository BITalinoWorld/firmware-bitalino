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


// Program memory variables
const char PROGMEM versionStr[] = "BITalino_v4.0\n";

// Global variables
word batThres = BATTHRES_BASE; // default battery threshold of 3.4 V
byte ledIncr = LEDINCR_SLOW;
byte seq, simulSeq, nChannels, chTable[6];
bool simulated;


int main(void)
{    
   // Assign PIO pull-ups to digital inputs
   PORTD = BIT_GP | BIT_DI1 | BIT_DI2 | BIT_DI3;
   PORTB = BIT_DI4 | BIT_STAT_BT;
   
   // Assign PIO outputs (phase 1)
   DDRD = BIT_LED_BAT | BIT_LED_STAT;
   
   if (!ISSET_GP)    testAndConfigure();
   
   // Assign PIO outputs (phase 2)
   DDRB = BIT_DO1 | BIT_DO2 | BIT_DO3 | BIT_DO4;
   
   // USART setup for Bluetooth module
   UBRR0 = 8;                                // = [F_CPU / (8*baud)] - 1  with U2X0 = 1  (115.2kbps @ 8MHz)
   UCSR0A = B(U2X0);
   UCSR0B = B(RXCIE0) | B(RXEN0) | B(TXEN0); // Enable RX, RX interrupt and TX and keep default 8n1 serial data format

   // Timer 0 setup for status LED update
   OCR0A = 200;                  // Interrupt frequency = F_CPU / 1024 / 201 = 38.9 Hz
   TCCR0A = B(WGM01);            // CTC mode
   TCCR0B = B(CS02) | B(CS00);   // Start timer with 1/1024 prescaling
   TIMSK0 = B(OCIE0A);           // Enable compare match interrupt
   
   // Timer 1 setup for sampling timing
   OCR1A = 125-1;                // default value for 1000 Hz
   TIMSK1 = B(OCIE1A);           // Enable compare match interrupt
   
   // Timer 2 setup for status LED PWM
   TCCR2A = B(COM2B1) | B(WGM21) | B(WGM20);    // Non-inverting Fast PWM mode on OC2B
   TCCR2B = B(CS20);                            // Start timer with no prescaling
   
   // Internal ADC setup
   DIDR0 = BIT_A4 | BIT_A3 | BIT_A2 | BIT_A1;   // Digital Input Disable in analog inputs A1-A4
   
   ACSR = B(ACD);    // Internal Analog Comparator Disable
   
   // Sleep Mode setup
   SMCR = B(SE);        // Idle Mode and Sleep Enabled
   
   // Stop unused modules
   PRR = B(PRTWI) | B(PRSPI);   // keep timers, USART and ADC running
   
   sei();   // Enable interrupts
   
	while(1)
      __asm volatile("sleep");

	return 0;
}
