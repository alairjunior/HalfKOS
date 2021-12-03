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
#include <hkos.h>


/**************************************************************************
 * Helper function to blink a LED
 *
 * We use attribute optmize O0 to prevent the busy wait from being
 * optimized out.
 *
 * @param[in]   pin    pin number
 *
 * ************************************************************************/
static void blink( uint8_t pin )
{
    while(1) {
        hkos_gpio_toggle( pin );
        hkos_sleep( 1000 );
    }
}

/**************************************************************************
 * Helper function to blink the green LED (P1.6) on Launchpad EXP430G2
 *
 * ************************************************************************/
static void blink_green( void )
{
    blink( 14 );
}

/**************************************************************************
 * Helper function to blink the red LED (P1.0) on Launchpad EXP430G2
 *
 * ************************************************************************/
static void blink_red( void )
{
    blink( 2 );
}

/**************************************************************************
 * Helper function to blink both LEDs em case of error
 *
 * We use attribute optmize O0 to prevent the busy wait from being
 * optimized out.
 *
 * ************************************************************************/
static void blink_error( void )
{
    while(1) {
        hkos_gpio_toggle( 2 );
        hkos_gpio_toggle( 14 );

        hkos_sleep( 100 );
    }
}

/**************************************************************************
 * Example Entry point
 *
 * ************************************************************************/
int main( void ) {

    hkos_init();
    hkos_gpio_write( 2, LOW );
    hkos_gpio_write( 14, LOW );
    hkos_gpio_config( 2, OUTPUT );
    hkos_gpio_config( 14, OUTPUT );

    if ( hkos_add_task( blink_red, 16 ) == 0 )
        blink_error();

    if ( hkos_add_task( blink_green, 16 ) == 0 )
        blink_error();

    // Should not return
    hkos_start();

    // We shouldn't be here
    blink_error();
}
