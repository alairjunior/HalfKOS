/******************************************************************************
 *
 * This file is part of HalfKOS.
 * https://github.com/alairjunior/HalfKOS
 *
 * Copyright (c) 2025 Alair Dias Junior.
 *
 * HalfKOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HalfKOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HalfKOS.  If not, see <https://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef _ARDUINO_H
#define _ARDUINO_H

#include <inttypes.h>
#include <stdlib.h>

#include "Serial.h"

void pinMode( uint8_t pin, uint8_t mode );
int digitalRead( uint8_t pin );
void digitalWrite( uint8_t pin, uint8_t val );
void delayMicroseconds( uint32_t delay );
void delay( uint32_t delay );

#define min(x, y)       ( ( (x) < (y) ) ? (x) : (y) )
#define max(x, y)       ( ( (x) > (y) ) ? (x) : (y) )

#endif //_ARDUINO_H
