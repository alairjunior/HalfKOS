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
extern "C" {
#include <hkos.h>
}

#include <Arduino.h>

void pinMode(uint8_t pin, uint8_t mode)
{
    hkos_gpio_config( pin, mode );
}

int digitalRead(uint8_t pin)
{
    return hkos_gpio_read( pin );
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    hkos_gpio_write( pin, val );
}

void delayMicroseconds( uint32_t delay )
{
    // TODO
}

void delay( uint32_t delay )
{
    // TODO
}
