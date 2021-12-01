/******************************************************************************
 *
 * This file is part of HalfKOS.
 * https://github.com/alairjunior/HalfKOS
 *
 * Copyright (c) 2021 Alair Dias Junior.
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

#ifndef __HKOS_CONFIG_H
#define __HKOS_CONFIG_H

#include <inttypes.h>
#include <stdbool.h>

// Configure HalfKOS time slice
#define HKOS_TIME_SLICE             5 // ms

// Paint the stack when creating the task for stack usage
// analysis
#define HKOS_PAINT_TASK_STACK       true
#define HKOS_STACK_PAINT_VALUE      0xFF

// Configure how many bytes are available in RAM for HKOS.
//
// This should be 512, which is the size of MSP430G2553
// RAM, but the original scat file from TI reserves 4 bytes
// for the heap. We could remove that from the scat file
// but since it comes with msp430-gcc, we preferred to
// use 4 bytes less and keep the default scat file.
// If you need that 4 bytes, change the scat file and
// you'll be able to use 512 bytes here.
#define HKOS_AVAILABLE_RAM          506 // bytes


// Configure how many bytes are available for
// HKOS' dynamic memory allocation
// This must be HKOS_AVAILABLE_RAM minus the number
// of bytes HKOS has for its own hosekeeping
//
#define HKOS_DYNAMIC_RAM            474 // bytes

#endif // __HKOS_CONFIG_H
