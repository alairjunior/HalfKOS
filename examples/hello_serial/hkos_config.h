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

// Configure HalfKOS time slice
#define HKOS_TIME_SLICE             5 // ms

// Paint the stack when creating the task for stack usage
// analysis
#define HKOS_PAINT_TASK_STACK       true
#define HKOS_STACK_PAINT_VALUE      0xFF

// Configure how many bytes are available in RAM for HKOS.
//
// This should be 512 less all user global variable space.
// However, the original scat file from TI reserves 4 bytes
// for the heap. We could remove that from the scat file
// but since it comes with msp430-gcc, we preferred to
// use 4 bytes less and keep the default scat file.
//
// 512 - 4 ( TI's heap ) - serial buffers - serial wait lists = 470
#define HKOS_AVAILABLE_RAM          470 // bytes


// Configure how many bytes are available for
// HalfKOS idle stack, used for HalfKOS housekeeping
// Except in case you are doing something really
// exotic, 32 bytes for HalfKOS idle stack should be
// sufficient.
//
#define HKOS_IDLE_STACK             32 // bytes


// Configure 1 serial port
#define HKOS_SERIAL_PORTS_ENABLE    1

#endif // __HKOS_CONFIG_H
