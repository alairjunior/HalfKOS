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
 * Helper function to blink both LEDs em case of error
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
 * Main task
 *
 * We use attribute optmize O0 to prevent the busy wait from being
 * optimized out.
 *
 * @param[in]   pin    pin number
 *
 * ************************************************************************/
static void hello_serial( void )
{

    hkos_error_code_t error = hkos_serial_open( 0,
                                                9600,
                                                HKOS_SERIAL_DATA_8,
                                                HKOS_SERIAL_STOP_1,
                                                HKOS_SERIAL_PAR_NONE );

    if ( error != HKOS_ERROR_NONE )
        blink_error();

    hkos_serial_println( 0, "Hello Serial! Press any key." );
    while(1) {
        char c = hkos_serial_read( 0 );
        hkos_serial_print( 0, "You pressed '" );
        hkos_serial_write( 0,  c );
        hkos_serial_println( 0, "'" );
    }
}

/**************************************************************************
 * Example Entry point
 *
 * ************************************************************************/
void setup( void ) {

    hkos_gpio_write( 2, LOW );
    hkos_gpio_write( 14, LOW );
    hkos_gpio_config( 2, OUTPUT );
    hkos_gpio_config( 14, OUTPUT );

    if ( hkos_add_task( hello_serial, 64 ) == 0 )
        blink_error();

}
