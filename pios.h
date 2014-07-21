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


#define def #define

#define NL /*
*/

#define ASSIGN_PIN(name, reg, bit) \
def SET_##name (PORT##reg |= B(bit)) NL \
def CLR_##name (PORT##reg &= ~B(bit)) NL \
def ISSET_##name (PIN##reg & B(bit)) NL \
def BIT_##name B(bit) NL \
def BITPOS_##name bit

// Pin assignments

ASSIGN_PIN(DI4    , B, 0)     // I (with pull-up)
ASSIGN_PIN(STAT_BT, B, 1)     // I (with pull-up)
ASSIGN_PIN(DO1    , B, 2)     // O
ASSIGN_PIN(DO2    , B, 3)     // O
ASSIGN_PIN(DO3    , B, 4)     // O
ASSIGN_PIN(DO4    , B, 5)     // O

ASSIGN_PIN(A4, C, 0)          // Analog I (ACC)
ASSIGN_PIN(A3, C, 1)          // Analog I (ECG)
ASSIGN_PIN(A2, C, 2)          // Analog I (EDA)
ASSIGN_PIN(A1, C, 3)          // Analog I (EMG)
// PC4-PC5 not used
ASSIGN_PIN(A6, C, 6)          // Analog I (BAT)
ASSIGN_PIN(A5, C, 7)          // Analog I (LUX)

// PD0: RXD
// PD1: TXD
ASSIGN_PIN(LED_BAT , D, 2)    // O
ASSIGN_PIN(LED_STAT, D, 3)    // O
ASSIGN_PIN(GP      , D, 4)    // I (with pull-up)
ASSIGN_PIN(DI1     , D, 5)    // I (with pull-up)
ASSIGN_PIN(DI2     , D, 6)    // I (with pull-up)
ASSIGN_PIN(DI3     , D, 7)    // I (with pull-up)
