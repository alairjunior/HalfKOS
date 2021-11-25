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

/******************************************************************************
 *
 * HAL types are types that needs to be exposed to user
 *
 *****************************************************************************/
#ifndef __HKOS_HAL_TYPES_H
#define __HKOS_HAL_TYPES_H

/******************************************************************************
 * Enumeration for GPIO PIN mode
 *
 * GPIO pins can be configured as INPUT, INPUT with pull-up resistor,
 * INPUT with pull-down resistor, or OUTPUT
 *
 *****************************************************************************/
typedef enum gpio_pin_mode_t {
    INPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN,
    OUTPUT,
} gpio_pin_mode_t;

/******************************************************************************
 * Enumeration for GPIO PIN value
 *
 * GPIO pins can be either in HIGH or LOW
 *
 *****************************************************************************/
typedef enum gpio_value_t {
    LOW,
    HIGH
} gpio_value_t;

#endif //__HKOS_HAL_TYPES_H
