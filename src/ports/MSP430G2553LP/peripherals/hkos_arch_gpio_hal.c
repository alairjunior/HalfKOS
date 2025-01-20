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

/**************************************************************************
 *
 * HalfKOS Hardware Abstraction Layer implementation for MSP430G2553
 * The pin definitions are based on the Launchpad EXP430G2
 *
 * ************************************************************************/
#include <msp430.h>
#include <core/hkos_hal.h>
#include <core/hkos_scheduler.h>
#include <core/peripherals/gpio/hkos_gpio_hal.h>
#include <stdbool.h>
#include <stddef.h>

// General Macros
//
#define BIT(x)          (1 << x)
#define arraysize(x)    (sizeof(x) / sizeof(x[0]))


/******************************************************************************
 * HalfKOS GPIO structure for MSP430G2553
 *
 * gpio_location_t is used to simplify the manipulation of GPIO pins.
 * Each pin has its port and a corresponding bit mask.
 *
 *****************************************************************************/
typedef struct {
    uint8_t    port;
    uint8_t    bit_mask;
} hkos_gpio_location_t;

/******************************************************************************
 * Pointers to use numbers to identify the proper PxDIR register
 *
 *****************************************************************************/
volatile unsigned char * const GPIO_PXDIR[] = {
    0,          // P0DIR does not exist
    &P1DIR,
    &P2DIR,
    &P3DIR
};

/******************************************************************************
 * Pointers to use numbers to identify the proper PxIN register
 *
 *****************************************************************************/
volatile unsigned char * const GPIO_PXIN[] = {
    0,          // P0IN does not exist
    &P1IN,
    &P2IN,
    &P3IN
};

/******************************************************************************
 * Pointers to use numbers to identify the proper PxOUT register
 *
 *****************************************************************************/
volatile unsigned char * const GPIO_PXOUT[] = {
    0,          // P0OUT does not exist
    &P1OUT,
    &P2OUT,
    &P3OUT
};

/******************************************************************************
 * Pointers to use numbers to identify the proper PxSEL register
 *
 *****************************************************************************/
volatile unsigned char * const GPIO_PXSEL[] = {
    0,          // P0SEL does not exist
    &P1SEL,
    &P2SEL,
    &P3SEL
};

/******************************************************************************
 * Pointers to use numbers to identify the proper PxSEL2 register
 *
 *****************************************************************************/
volatile unsigned char * const GPIO_PXSEL2[] = {
    0,          // P0SEL2 does not exist
    &P1SEL2,
    &P2SEL2,
    &P3SEL2
};

/******************************************************************************
 * Pointers to use numbers to identify the proper PxREN register
 *
 *****************************************************************************/
volatile unsigned char * const GPIO_PXREN[] = {
    0,          // P0REN does not exist
    &P1REN,
    &P2REN,
    &P3REN
};

/******************************************************************************
 * GPIO pin location definitions
 * {port, bit} for each pin
 *****************************************************************************/
const hkos_gpio_location_t GPIO_PIN[] = {
    {0, 0     },                        // PIN 0  does not exist
    {0, 0     },                        // PIN 1  is VCC
    {1, BIT(0)},                        // PIN 2  is P1.0
    {1, BIT(1)},                        // PIN 3  is P1.1
    {1, BIT(2)},                        // PIN 4  is P1.2
    {1, BIT(3)},                        // PIN 5  is P1.3
    {1, BIT(4)},                        // PIN 6  is P1.4
    {1, BIT(5)},                        // PIN 7  is P1.5
    {2, BIT(0)},                        // PIN 8  is P2.0
    {2, BIT(1)},                        // PIN 9  is P2.1
    {2, BIT(2)},                        // PIN 10 is P2.2
    {2, BIT(3)},                        // PIN 11 is P2.3
    {2, BIT(4)},                        // PIN 12 is P2.4
    {2, BIT(5)},                        // PIN 13 is P2.5
    {1, BIT(6)},                        // PIN 14 is P1.6
    {1, BIT(7)},                        // PIN 15 is P1.7
    {0, 0     },                        // PIN 16 has no GPIO
    {0, 0     },                        // PIN 17 has no GPIO
    {2, BIT(7)},                        // PIN 18 is P2.7
    {2, BIT(6)},                        // PIN 19 is P2.6
    {0, 0     },                        // PIN 20 is GND
};


/******************************************************************************
 * Helper function to retrieve the port number for a pin
 *
 * @param[in]   pin     pin number
 *
 * @return the port number
 *
 *****************************************************************************/
static inline uint8_t pin_get_port( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    return GPIO_PIN[pin].port;
}

/******************************************************************************
 * Helper function to retrieve the bit mask for a pin
 *
 * @param[in]   pin     pin number
 *
 * @return the pin mask value
 *
 *****************************************************************************/
static inline uint8_t pin_get_mask( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    return GPIO_PIN[pin].bit_mask;
}

/******************************************************************************
 * Helper function to configure a pin as general I/O
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline void pin_is_io( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    const uint8_t port = pin_get_port( pin );
    const uint8_t bit_mask = pin_get_mask( pin );

    *GPIO_PXSEL[port]  &= ~bit_mask;
    *GPIO_PXSEL2[port] &= ~bit_mask;
}

/******************************************************************************
 *  Helper function to configure a pin as input
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline void pin_is_input( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    const uint8_t port = pin_get_port( pin );
    const uint8_t bit_mask = pin_get_mask( pin );

    *GPIO_PXDIR[port] &= ~bit_mask;
}

/******************************************************************************
 *  Helper function to configure a pin as output
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline void pin_is_output( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    const uint8_t port = pin_get_port( pin );
    const uint8_t bit_mask = pin_get_mask( pin );

    *GPIO_PXDIR[port] |= bit_mask;
}

/******************************************************************************
 *  Helper function to disable a pin's output resistor
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline void pin_disable_resistor( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    const uint8_t port = pin_get_port( pin );
    const uint8_t bit_mask = pin_get_mask( pin );

    *GPIO_PXREN[port] &= ~bit_mask;
}

/******************************************************************************
 *  Helper function to enable a pin's pulldown resistor
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline void pin_enable_pulldown( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    const uint8_t port = pin_get_port( pin );
    const uint8_t bit_mask = pin_get_mask( pin );

    *GPIO_PXREN[port] |=  bit_mask;
    *GPIO_PXOUT[port] &= ~bit_mask;
}

/******************************************************************************
 *  Helper function to enable a pin's pullup resistor
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline void pin_enable_pullup( uint8_t pin ) {
    // We don't check if pin is valid, because this is internal only call
    const uint8_t port = pin_get_port( pin );
    const uint8_t bit_mask = pin_get_mask( pin );

    *GPIO_PXREN[port] |=  bit_mask;
    *GPIO_PXOUT[port] |=  bit_mask;
}

/******************************************************************************
 *  Helper function to check if the pin is valid
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
static inline uint8_t is_valid_pin( uint8_t pin )
{
    return ( pin < arraysize( GPIO_PIN ) ) && ( pin_get_port( pin ) > 0 );
}

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
void hkos_gpio_config( uint8_t pin, hkos_gpio_pin_mode_t mode ) {

    if ( is_valid_pin( pin ) )
    {
        pin_is_io( pin );

        switch( mode )
        {
            case INPUT:
                pin_is_input( pin );
                pin_disable_resistor( pin );
                break;

            case INPUT_PULLUP:
                pin_is_input( pin );
                pin_enable_pullup( pin );
                break;

            case INPUT_PULLDOWN:
                pin_is_input( pin );
                pin_enable_pulldown( pin );
                break;

            case OUTPUT:
                pin_is_output( pin );
                pin_disable_resistor( pin );
                break;

        }
    }
}

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
void hkos_gpio_write( uint8_t pin, hkos_gpio_value_t value ) {
    if ( is_valid_pin( pin ) ) {

        const uint8_t port = pin_get_port( pin );
        const uint8_t bit_mask = pin_get_mask( pin );

        switch ( value ) {
            case HIGH:
                *GPIO_PXOUT[port] |=  bit_mask;
                break;

            case LOW:
                *GPIO_PXOUT[port] &= ~bit_mask;
                break;
        }
    }
}

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
hkos_gpio_value_t hkos_gpio_read( uint8_t pin ) {
    if ( is_valid_pin( pin ) ) {

        const uint8_t port = pin_get_port( pin );
        const uint8_t bit_mask = pin_get_mask( pin );

        if ( ( *GPIO_PXIN[port] & bit_mask ) != 0 )
        {
            return HIGH;
        } else {
            return LOW;
        }
    } else {
        return LOW;
    }
}

/******************************************************************************
 * Toggle the value of a GPIO pin
 *
 * This function toggles (switches) the value of the provided GPIO pin.
 * If pin is HIGH, it will become LOW, and vice-versa.
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
void hkos_gpio_toggle( uint8_t pin ) {
    if ( hkos_gpio_read( pin ) == HIGH )
        hkos_gpio_write( pin, LOW );
    else
        hkos_gpio_write( pin, HIGH );
}
