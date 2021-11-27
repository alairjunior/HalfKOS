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

#ifndef __HKOS_ARCH_HAL_H
#define __HKOS_ARCH_HAL_H

#include <inttypes.h>

// Configure how many bytes are available in RAM for HKOS.
//
// This should be 512, which is the size of MSP430G2553
// RAM, but the original scat file from TI reserves 4 bytes
// for the heap. We could remove that from the scat file
// but since it comes with msp430-gcc, we preferred to
// use 4 bytes less and keep the default scat file.
// If you need that 4 bytes, change the scat file and
// you'll be able to use 512 bytes here.
#define HKOS_AVAILABLE_RAM          508 // bytes


// Configure how many bytes are available for
// HKOS' dynamic memory allocation
// This must be HKOS_AVAILABLE_RAM minus the number
// of bytes HKOS has for its own hosekeeping
//
#define HKOS_DYNAMIC_RAM            494 // bytes

// Configure the data type of the dynamic memory
// allocation block header. Besides the size requested
// during the allocation, the number of bytes of the data
// type below will be included to the block. One bit of this
// header is used for marking the block as used/free and
// the remaining bits are used for the size of the block.
// Hence, blocks can be up to 2^(datatype bits - 1) long.
typedef uint16_t                    hkos_dmem_header_t;

#endif // __HKOS_ARCH_HAL_H