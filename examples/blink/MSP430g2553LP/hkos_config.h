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
#define HKOS_TIME_SLICE             2 // ms

// Paint the stack when creating the task for stack usage
// analysis
#define HKOS_PAINT_TASK_STACK       true
#define HKOS_STACK_PAINT_VALUE      0xFFFF

#endif // __HKOS_CONFIG_H
