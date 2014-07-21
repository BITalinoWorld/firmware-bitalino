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


const word PROGMEM ecg[50] = {440 ,440 ,440 ,443 ,456 ,468 ,477 ,480 ,472 ,451 ,440 ,440 ,440 ,440 ,437 ,415 ,579 ,703 ,484 ,392 ,439 ,440 ,440 ,440 ,440 ,440 ,440 ,440 ,440 ,446 ,456 ,466 ,474 ,482 ,488 ,494 ,497 ,499 ,499 ,493 ,482 ,467 ,448 ,440 ,440 ,441 ,450 ,450 ,441 ,440 };

const word PROGMEM random_signal[50] = {437, 977, 741, 594, 553, 722, 5, 801, 948, 8, 844, 785, 1020, 233, 941, 657, 108, 274, 781, 824, 107, 481, 224, 944, 328, 877, 266, 898, 193, 777, 32, 657, 580, 385, 217, 810, 149, 500, 13, 191, 496, 858, 144, 749, 707, 35, 500, 994, 115, 760};

const word PROGMEM sin50Hz[20] =  {511, 669, 811, 924, 997, 1022, 997, 924, 811, 669, 511, 353, 211, 98, 25, 0, 25, 98, 211, 353};

const word PROGMEM sin40Hz[25] = {511, 638, 757, 861, 942, 997, 1021, 1013, 973, 905, 811, 699, 575, 447, 323, 211, 117, 49, 9, 1, 25, 80, 161, 265, 384};

const byte PROGMEM sin20Hz[50] =  {31, 35, 39, 42, 46, 49, 52, 55, 57, 59, 60, 61, 62, 62, 61, 60, 59, 57, 55, 52, 49, 46, 42, 39, 35, 31, 27, 23, 20, 16, 13, 10, 7, 5, 3, 2, 1, 0, 0, 1, 2, 3, 5, 7, 10, 13, 16, 20, 23, 27};

const byte PROGMEM sin10Hz[100] =  {31, 33, 35, 37, 39, 41, 42, 44, 46, 48, 49, 51, 52, 54, 55, 56, 57, 58, 59, 60, 60, 61, 61, 62, 62, 62, 62, 62, 61, 61, 60, 60, 59, 58, 57, 56, 55, 54, 52, 51, 49, 48, 46, 44, 42, 41, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 16, 14, 13, 11, 10, 8, 7, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 10, 11, 13, 14, 16, 18, 20, 21, 23, 25, 27, 29};


word simulData(byte chPos, byte s50, byte s25, byte s20)
{
   switch(chTable[chPos])
   {
      case 0:
         return pgm_read_word(ecg + s50);
         
      case 1:
         return pgm_read_word(random_signal + s50);
         
      case 2:
         return pgm_read_word(sin50Hz + s20);
         
      case 3:
         return pgm_read_word(sin40Hz + s25);
         
      case 4:
         return pgm_read_byte(sin20Hz + s50);
         
      case 5:
         return pgm_read_byte(sin10Hz + simulSeq);
   }
   
   return 0;   // just to avoid warning
}

void timer1TickSimul(void)
{
   // Send a new simulated data frame
   
	byte frame[7];
   
   /*
   static byte t1 = 0;
   byte t0 = TCNT1;
   */
   
   memset(frame, 0, 6);
   
   frame[6] = seq << 4;
   
   const byte s50 = (simulSeq >= 50) ? simulSeq-50 : simulSeq;
   const byte s25 = (s50 >= 25) ? s50-25 : s50;
   const byte s20 = simulSeq % 20;
   
   *(word*)(frame+5) |= simulData(0, s50, s25, s20) << 2;
   if (nChannels > 1)
      *(word*)(frame+4) |= simulData(1, s50, s25, s20);
   if (nChannels > 2)
      *(word*)(frame+2) |= simulData(2, s50, s25, s20) << 6;
   if (nChannels > 3)
      *(word*)(frame+1) |= simulData(3, s50, s25, s20) << 4;
   if (nChannels > 4)
      *(word*)frame |= simulData(4, s50, s25, s20) << 6;
   if (nChannels > 5)
      frame[0] |= simulData(5, s50, s25, s20);
   
   /*
   *(word*)(frame+5) = t0 << 2;
   frame[4] = TCNT1;
   *(word*)(frame+2) = ((word)t1) << 6;
   */
   
   sendFrameCRC(frame);
   
   simulSeq++;
   if (simulSeq == 100)    simulSeq = 0;
   
   //t1 = TCNT1;
}

