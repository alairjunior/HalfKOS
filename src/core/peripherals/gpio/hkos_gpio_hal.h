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
#ifndef __HKOS_GPIO_HAL_H
#define __HKOS_GPIO_HAL_H

#include <inttypes.h>
#include <stdbool.h>

/******************************************************************************
 * Enumeration for GPIO PIN mode
 *
 * GPIO pins can be configured as INPUT, INPUT with pull-up resistor,
 * INPUT with pull-down resistor, or OUTPUT
 *
 *****************************************************************************/
typedef enum {
    INPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN,
    OUTPUT,
} hkos_gpio_pin_mode_t ;

/******************************************************************************
 * Enumeration for GPIO PIN value
 *
 * GPIO pins can be either in HIGH or LOW
 *
 *****************************************************************************/
typedef enum {
    LOW,
    HIGH
} hkos_gpio_value_t;

/******************************************************************************
 * Configure a GPIO pin
 *
 * This function sets the mode of the provided GPIO pin. If pin has more than
 * one function, I/O function is selected.
 *
 * @param[in]   pin     pin number
 * @param[in]   mode    the mode selected from the pin mode enumeration
 *
 *****************************************************************************/
void hkos_gpio_config( uint8_t pin, hkos_gpio_pin_mode_t mode );


/******************************************************************************
 * Write a value to a GPIO pin
 *
 * This function write a value to the provided GPIO pin. It will not change
 * the pin mode.
 *
 * @param[in]   pin     pin number
 * @param[in]   value   the value selected from the pin value enumeration
 *
 *****************************************************************************/
void hkos_gpio_write( uint8_t pin, hkos_gpio_value_t value );


/******************************************************************************
 * Toggle the value of a GPIO pin
 *
 * This function toggles (switches) the value of the provided GPIO pin.
 * If pin is HIGH, it will become LOW, and vice-versa.
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
void hkos_gpio_toggle( uint8_t pin );


/******************************************************************************
 * Read the value of a GPIO pin
 *
 * This function reads the value of the provided GPIO pin.
 *
 * @param[in]   pin     pin number
 *
 * @return  The value of the GPIO pin taken from the pin value enumeration
 *
 *****************************************************************************/
hkos_gpio_value_t hkos_gpio_read( uint8_t pin );

#endif // __HKOS_GPIO_HAL_H
