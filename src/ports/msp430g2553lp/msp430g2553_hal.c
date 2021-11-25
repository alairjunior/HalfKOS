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
#if ARCH==MSP430G2553

#include <msp430.h>
#include <core/hkos_hal.h>
#include <core/hkos_scheduler.h>
#include <stdbool.h>
#include <stddef.h>

// General Macros
//
#define BIT(x)          (1 << x)
#define arraysize(x)    (sizeof(x) / sizeof(x[0]))
#define MIN_STACK_SIZE  28

/******************************************************************************
 * HalfKOS GPIO structure for MSP430G2553
 *
 * gpio_location_t is used to simplify the manipulation of GPIO pins.
 * Each pin has its port and a corresponding bit mask.
 *
 *****************************************************************************/
typedef struct gpio_location_t {
    uint8_t    port;
    uint8_t    bit_mask;
} gpio_location_t;

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
const gpio_location_t GPIO_PIN[] = {
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
 * The HalfKOS ram structure needs to be accessed for saving and restoring
 * the context, which is done by the timer0A0 ISR
 *****************************************************************************/
extern hkos_ram_t hkos_ram;

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
 * @return the port number
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
 *  Helper function to disable the watchdog timer
 *
 *****************************************************************************/
static inline void disable_wdt( void ) {
    WDTCTL = WDTPW | WDTHOLD;
}

/******************************************************************************
 *  Helper function to initialize the DCO
 *
 *****************************************************************************/
static inline void init_dco( void ) {
	// uses the calibrated frequency of 16 MHz
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
}

/******************************************************************************
 *  Helper function to start the tick timer
 *
 *****************************************************************************/
static inline void start_tick_timer( void ) {
    // enable timer0 A0 interrupt
    TACCTL0 |= CCIE;
}

/******************************************************************************
 *  Helper function to stop the tick timer
 *
 *****************************************************************************/
static inline void stop_tick_timer( void ) {
    // enable timer0 A0 interrupt
    TACCTL0 &= ~CCIE;
}

/******************************************************************************
 *  Helper function to initialize timer A interrupts
 *
 *****************************************************************************/
static void init_timerA( void ) {
    // Disable timer0 A0 interrupt
    TACCTL0 &= ~CCIE;

    // Disable timer0 A1 interrupt
    TACCTL1 &= ~CCIE;

    // Stop the timer
    TACTL = MC_0;

    // Set the counter
    TA0CCR0 = 1000*HKOS_TIME_SLICE;

    // DCO, DCO/8, up and down mode
    TACTL = TASSEL_2 | ID_3 | MC_2;
}

/******************************************************************************
 *  Initialize the HAL and put the system at its initial state
 *
 *****************************************************************************/
void hkos_hal_init( void ) {
    disable_wdt();
    init_dco();
    init_timerA();
    __enable_interrupt();
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
void hkos_hal_gpio_config( uint8_t pin, gpio_pin_mode_t mode ) {

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
void hkos_hal_gpio_write( uint8_t pin, gpio_value_t value ) {
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
gpio_value_t hkos_hal_gpio_read( uint8_t pin ) {
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
void hkos_hal_gpio_toggle( uint8_t pin ) {
    if ( hkos_hal_gpio_read( pin ) == HIGH )
        hkos_hal_gpio_write( pin, LOW );
    else
        hkos_hal_gpio_write( pin, HIGH );
}

/******************************************************************************
 * Initialize the task stack.
 *
 * MSP430G2553 has 12 GP registers (r4-r15). Also, when returning from an
 * interrupt, it pops the SR (r2) and PC (r0). So, we organized the stack
 * in such a way that when returning from the interrupt, all the data is
 * already in the proper places.
 *
 * @param[in]   pp_sp       a pointer to the stack pointer indicating the
 *                          memory region of the task stack
 * @param[in]   p_pc        a pointer to the beginning of the task code
 * @param[in]   stack_size  the size of the task's stack
 *
 * @return  The value of stack pointer after the stack initialization
 *
 *****************************************************************************/
void* hkos_hal_init_stack( void** pp_sp, void* p_pc, hkos_size_t stack_size ){

    if ( stack_size < MIN_STACK_SIZE )
        return NULL;

    *--pp_sp = p_pc;
    *--pp_sp = (void*) GIE;
    *--pp_sp = (void*) 0x4;  /* r4 */
    *--pp_sp = (void*) 0x5;  /* r5 */
    *--pp_sp = (void*) 0x6;  /* r6 */
    *--pp_sp = (void*) 0x7;  /* r7 */
    *--pp_sp = (void*) 0x8;  /* r8 */
    *--pp_sp = (void*) 0x9;  /* r9 */
    *--pp_sp = (void*) 0xA;  /* r10 */
    *--pp_sp = (void*) 0xB;  /* r11 */
    *--pp_sp = (void*) 0xC;  /* r12 */
    *--pp_sp = (void*) 0xD;  /* r13 */
    *--pp_sp = (void*) 0xE;  /* r14 */
    *--pp_sp = (void*) 0xF;  /* r15 */

    return pp_sp;
}


/******************************************************************************
 * Enter the Microcontroller low power mode
 *
 * This function is called by the scheduler to put the Microcontroller in
 * low power mode. When there is no task to be executed, scheduler will call
 * it. LPM1 mode does not prevent the Timer0A0 (tick timer) from generating
 * interrupts.
 *
 *****************************************************************************/
void hkos_hal_enter_lp( void ) {
    LPM1;
}

/******************************************************************************
 * Enter the Microcontroller low power mode
 *
 * This function is called by the scheduler to pull the Microcontroller out of
 * low power mode.
 *
 *****************************************************************************/
void hkos_hal_exit_lp( void ) {
    // we need to clear the low power bits in the SR of the current task
    asm volatile(
        "mov.w  %0,         r4      \n\t"
        "bic.w  %1,     -24(r4)     \n\t"
                :
                : "m" (hkos_ram.p_current_task->p_sp), "i" (LPM1_bits)
                : "r4", "r5"
    );

}

/******************************************************************************
 * Start the tick timer responsible for the HalfKOS context switch
 *
 *****************************************************************************/
void hkos_hal_start_tick_timer( void ) {
    start_tick_timer();
}

/******************************************************************************
 * TIMER0_A0 ISR. This is the HalfKOS tick timer
 *
 * This interrupt is responsible for context switch and perform the following
 * operations:
 *
 *      1. Save the current task's context (stored in hkos_ram.current_task)
 *      2. Call hkos_scheduler_switch
 *      3. Restore the new current task's context
 *
 * Implementation of this ISR is tricky. The compiler usually saves the
 * registers used and restores them by the end. Some implementations use
 * the naked attribute to prevent this and other side effects. We decided
 * to use that in our favor and marked almost all registers in the clobber
 * section of the inline assembly (asm). Usually, registers r11-r15 are saved
 * by the compiler anyway. Saying that we are using registers r4-r10 makes
 * the compiler save them in the stack when entering the interrupt, saving
 * the context for us (step 1). If there is no current task, we just restore
 * the same values before moving to the next step (step 2).
 * After switching the current task, we just need to point the sp to the proper
 * location and let the epilogue defined by the compiler to restore all the
 * registers from the new stack.
 *
 * OBS: It is very instructive to see how this function is compiled to
 * assembly. Get your hands dirty and see how it is being done.
 *
 *****************************************************************************/
__attribute__((interrupt(TIMER0_A0_VECTOR)))
__attribute__((optimize("Og")))
void timer_a0_isr(void) {

    asm volatile(
        "cmp    #0, %1          \n\t"
        "jz     no_task         \n\t"
        "mov.w  r1, %0          \n\t"
        "jmp    done_save       \n\t"
    "no_task:                   \n\t"
        "pop    r4              \n\t"
        "pop    r5              \n\t"
        "pop    r6              \n\t"
        "pop    r7              \n\t"
        "pop    r8              \n\t"
        "pop    r9              \n\t"
        "pop    r10             \n\t"
        "pop    r11             \n\t"
        "pop    r12             \n\t"
        "pop    r13             \n\t"
        "pop    r14             \n\t"
        "pop    r15             \n\t"
    "done_save:                 \n\t"
            :   "=m" (hkos_ram.p_current_task->p_sp)
            :   "m"  (hkos_ram.p_current_task)
            :   "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

    hkos_scheduler_switch();

    asm volatile(
        "mov.w  %0, r1"
            :
            : "m" (hkos_ram.p_current_task->p_sp)
            : "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

}

#endif // ARCH==MSP430G2553