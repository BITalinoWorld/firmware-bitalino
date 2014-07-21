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


const byte PROGMEM crcTable[16] = {0, 3, 6, 5, 12, 15, 10, 9, 11, 8, 13, 14, 7, 4, 1, 2};


void sendFrameCRC(const byte *frame)
{
   byte startPos = 6 - nChannels;
   if (nChannels >= 3 && nChannels <= 5)  startPos--;
   
   frame += startPos;
   
   // send frame and calculate CRC (except last byte (seq+CRC) )
   byte crc = 0;
   for(byte i = startPos; i < 7; i++)
   {
      byte b = *frame;
      frame++;
      
      // send byte
      loop_until_bit_is_set(UCSR0A, UDRE0);
      UDR0 = b;
      
      // calculate CRC nibble by nibble
      crc = pgm_read_byte(crcTable + crc) ^ (b >> 4);
      crc = pgm_read_byte(crcTable + crc) ^ (b & 0x0F);
   }
   
   // calculate CRC for last byte (seq+CRC)
   crc = pgm_read_byte(crcTable + crc) ^ (seq & 0x0F);
   crc = (seq << 4) | pgm_read_byte(crcTable + crc);
   
   // send last byte
   loop_until_bit_is_set(UCSR0A, UDRE0);
   UDR0 = crc;
   
	seq++;
}
