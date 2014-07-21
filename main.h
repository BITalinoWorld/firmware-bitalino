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


#define F_CPU   8000000UL  // system frequency in Hz

#include <stdbool.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


// Macros and defines
#define B(bit)          _BV(bit)

#define LEDINCR_SLOW    5
#define LEDINCR_FAST    50

#define BATTHRES_BASE   527

#include "build/pios.h"       // build/pios.h is processed from pios.h

// Type definitions
typedef uint8_t  byte;
typedef uint16_t word;

// Program memory variables
extern const char PROGMEM versionStr[];

// Global variables
extern word batThres;
extern byte ledIncr, seq, simulSeq, nChannels, chTable[6];
extern bool simulated;


// Global functions
// crc.c
void sendFrameCRC(const byte *frame);

// live
void timer1TickLive(void);

// simul.c
void timer1TickSimul(void);

// test.c
void testAndConfigure(void);

// uart.c
void sendProgmemStr(const char *str);
