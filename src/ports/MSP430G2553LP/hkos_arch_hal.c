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
void start_tick_timer( void ) {
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
static inline void init_timerA( void ) {
    // Disable timer0 A0 interrupt
    TACCTL0 &= ~CCIE;

    // Disable timer0 A1 interrupt
    TACCTL1 &= ~CCIE;

    // Stop the timer
    TACTL = MC_0;

    // Set the counter
    TA0CCR0 = 1000*HKOS_TIME_SLICE;

    // DCO, DCO/8, up and down mode
    TACTL = TASSEL_2 | ID_3 | MC_3;
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
void* hkos_hal_init_stack( void* p_sp, void* p_pc, hkos_size_t stack_size ){

    uint16_t* p_stack = (uint16_t*)p_sp;


    *--p_stack = (uint16_t)p_pc;
    *--p_stack = (uint16_t)GIE;
    *--p_stack = (uint16_t)0xFFFF; // R15
    *--p_stack = (uint16_t)0xEEEE; // R14
    *--p_stack = (uint16_t)0xDDDD; // R13
    *--p_stack = (uint16_t)0xCCCC; // R12
    *--p_stack = (uint16_t)0xBBBB; // R11
    *--p_stack = (uint16_t)0xAAAA; // R10
    *--p_stack = (uint16_t)0x9999; // R9
    *--p_stack = (uint16_t)0x8888; // R8
    *--p_stack = (uint16_t)0x7777; // R7
    *--p_stack = (uint16_t)0x6666; // R6
    *--p_stack = (uint16_t)0x5555; // R5
    *--p_stack = (uint16_t)0x4444; // R4

    uint16_t* ret_stack = p_stack;

    // We paint the stack here so we can analyse stack usage.
    // to paint it, we include the context switch region too
    if ( HKOS_PAINT_TASK_STACK ) {
        stack_size += hkos_hal_get_min_stack_size();
        while ( (uint8_t*)p_stack > (uint8_t*)p_sp - stack_size ) {
            *--p_stack = HKOS_STACK_PAINT_VALUE | ( HKOS_STACK_PAINT_VALUE << 8 );
        }
    }

    // When creating the task, the stack will hold the PC + SR + GP registers
    return ret_stack;
}


/******************************************************************************
 * Get the minimal stack size
 *
 * Depending on the CPU, the minimum stack size can be different. Besides the
 * task's user operations, the stack needs to be big enough to store data
 * during the context switch
 *
 * Minimum stack size on MSP430:
 *          + 2 Bytes for PC
 *          + 2 Bytes for SR
 *          + 12*2 Bytes for GP registers
 *          + 2 Bytes for the context switch call
 *
 *          = 30 bytes
 *
 *****************************************************************************/
inline hkos_size_t hkos_hal_get_min_stack_size( void ) {
    return 30; // better define it here than at the beginning of this file.
               // less mind jumps when analysing the code.
}


/******************************************************************************
 * Jump to the operating system
 *
 * This function will point the stack pointer to the operating system's stack
 * and start the timer to do the context switch operation. The interrupt to
 * handle the context switch must do the following operations:
 *
 *      1. Save the current task's context (stored in p_hkos_current_task)
 *      2. Call hkos_scheduler_switch_context
 *      3. Restore the new current task's context
 *
 *****************************************************************************/
void hkos_hal_jump_to_os( void ) {
    // We won't return from this call, so we can mess with
    // the stack pointer and registers freely

    // We paint the OS stack to help debug
    if ( HKOS_PAINT_TASK_STACK ) {
        for ( hkos_size_t i = 0; i < arraysize(hkos_ram.os_stack); ++i ) {
            hkos_ram.os_stack[i] = HKOS_STACK_PAINT_VALUE;
        }
    }

    // This will prepare the system to be in idle. This is essentially an
    // idle task but it was not included as a task to save a couple of bytes
    // of memory. The way it works is the following:
    //      1. We point the SP (R1) to the memory region reserved for the OS
    //      2. We push the hkos_idle lable position to the stack. This will
    //         be our PC when there is no other task to run
    //      3. We push the SR to the stack. In this case, SR will have LPM1
    //         bits set. This will make the CPU to turn off whenever the idle
    //         task is running. Each task has its own SR, so it will not affect
    //         other tasks. Idle task also have global interrupts enabled (GIE)
    //      4. We save back the idle task pointer to be restored when there is
    //         no tasks running
    //      5. We disable the interrupts so the idle task is not interrupted
    //         after starting the tick timer
    //      6. We start the tick timer (timer0A0) to start handling the context
    //         switch
    //      7. We simulate a return from interrupt, so PC and SR are restored,
    //         so idle task goes to its code position, we enter LPM1 state and
    //         global interrupts are enabled again.
    //
    asm volatile (
        "mov.w      %0,                  r1 \n\t" // Point r1 to the os stack
        "add.w      %1,                  r1 \n\t" // move r1 to the stack's end
        "push       #hkos_idle              \n\t" // push the idle PC
        "push       %3                      \n\t" // push SR with LPM1 enabled
        "mov.w      r1,                  %2 \n\t" // save the new os SP
        "bic.w      %4,                  r2 \n\t" // Disable interrupts
        "nop                                \n\t" // Required after dis. int.
        "call       #start_tick_timer       \n\t" // starts the tick timer
        "reti                               \n\t" // RETI will update PC and SR
    "hkos_idle:                             \n\t"
        "jmp        hkos_idle               \n\t" // when idle, we do nothing
        :
        : "i" (&hkos_ram.os_stack[0]), "i" (sizeof(hkos_ram.os_stack)),
                    "m" (p_hkos_sp), "i" (GIE+LPM1_bits), "i" (GIE)
        :
    );
}


/******************************************************************************
 * Enter Critical Section
 *
 * When inside a critical section, the code cannot be preempted. This can be
 * done using software only (like Lamport's bakery algorithm), but hardware
 * support is the preferable approach. So we leave it for the HAL to decide.
 *
 * OBS: this function must not be called by user code, because it may affect
 * the time counting. All use of this function MUST be restricted to HalfKOS
 * core.
 *
 * In case of MSP430, since this is a single core / single thread CPU, we just
 * need to disable interrupts
 *
 *****************************************************************************/
void hkos_hal_enter_critical_section( void ) {
    __disable_interrupt();
}

/******************************************************************************
 * Exit Critical Section
 *
 * Exit critical section must always be called after a call to enter critical
 * section.
 *
 * OBS: this function must not be called by user code, because it may affect
 * the time counting. All use of this function MUST be restricted to HalfKOS
 * core,
 *
 * In case of MSP430, since this is a single core / single thread CPU, we just
 * need to enable interrupts so scheduler is called
 *
 *****************************************************************************/
void hkos_hal_exit_critical_section( void ) {
    __enable_interrupt();
}

/******************************************************************************
 * Save context for a context switch (interrupt version)
 *
 * This function will save the context before a context switch. It must save
 * all the context information for the current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler.
 *
 * This version is intended to be called from an interrupt.
 *
 *****************************************************************************/
__attribute__((naked))
static void save_context_from_interrupt( void ) {

    // This requires some explanation: we begin by checking if there is a
    // current task. If not, we don't need to save the context.
    // At the top of the stack we have the PC to which we will return at
    // the end of this function. However, we need to save the context in
    // the stack, but at the same time, we cannot change the register values
    // because we are saving the context. So, we do the following:
    //      1. Leave the PC at the top of the stack (TOS)
    //      2. Push R14 first, instead of R15
    //      3. Copy the PC, that is in the second to top stack position, to R14
    //      4. Copy the content of R15 to the second to top stack position
    //      5. Push the other registers from 13 to 4
    //      6. Save the stack pointer, saving the entire context
    //      7. Now, we push the return PC (stored in R14) to the TOS
    //      8. We execute a ret, that will return to the calling function
    asm (
        "hkos_hal_save_context_int:         \n\t"
        "   cmp     #0, p_hkos_current_task \n\t"
        "   jz      done_save_int           \n\t"
        "   push    r14                     \n\t"
        "   mov.w   2(r1),               r14\n\t"
        "   mov.w   r15,               2(r1)\n\t"
        "   push    r13                     \n\t"
        "   push    r12                     \n\t"
        "   push    r11                     \n\t"
        "   push    r10                     \n\t"
        "   push    r9                      \n\t"
        "   push    r8                      \n\t"
        "   push    r7                      \n\t"
        "   push    r6                      \n\t"
        "   push    r5                      \n\t"
        "   push    r4                      \n\t"
        "   mov.w   p_hkos_current_task, r15\n\t"
        "   mov.w   r1,               0(r15)\n\t"
        "   push    r14                     \n\t"
        "done_save_int:                     \n\t"
        "   ret                             \n\t"
    );
}


/******************************************************************************
 * Save context for a context switch (general version)
 *
 * This function will save the context before a context switch. It must save
 * all the context information for the current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler.
 *
 * This version is to be called out of the interrupt. This means, it should
 * simulate an interrupt by pushing SR (R2 to the stack).
 *
 *****************************************************************************/
__attribute__((naked))
void hkos_hal_save_context( void ) {

    // This requires some explanation: we begin by checking if there is a
    // current task. If not, we don't need to save the context.
    // At the top of the stack we have the PC to which we will return at
    // the end of this function. However, we need to save the context in
    // the stack, but at the same time, we cannot change the register values
    // because we are saving the context. So, we do the following:
    //      1. Leave the PC at the top of the stack (TOS)
    //      2. Push R15 first
    //      3. Copy the PC, that is in the second to top stack position, to R15
    //      4. Copy the content of SR (R2) to the second to top stack position
    //      5. Push the other registers from 14 to 4
    //      6. Save the stack pointer, saving the entire context
    //      7. Now, we push the return PC (stored in R15) to the TOS
    //      8. We execute a ret, that will return to the calling function
    asm (
        "hkos_hal_save_context:             \n\t"
        "   cmp     #0, p_hkos_current_task \n\t"
        "   jz      done_save               \n\t"
        "   push    r15                     \n\t"
        "   mov.w   2(r1),               r15\n\t"
        "   mov.w   r2,                 2(r1)\n\t"
        "   push    r14                     \n\t"
        "   push    r13                     \n\t"
        "   push    r12                     \n\t"
        "   push    r11                     \n\t"
        "   push    r10                     \n\t"
        "   push    r9                      \n\t"
        "   push    r8                      \n\t"
        "   push    r7                      \n\t"
        "   push    r6                      \n\t"
        "   push    r5                      \n\t"
        "   push    r4                      \n\t"
        "   mov.w   p_hkos_current_task, r14\n\t"
        "   mov.w   r1,               0(r14)\n\t"
        "   push    r15                     \n\t"
        "done_save:                         \n\t"
        "   ret                             \n\t"
    );
}

/******************************************************************************
 * Restore context after a context switch
 *
 * This function will restore the context after a context switch. It must
 * restore all the context of the new current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler. Most probably the function will be written using
 * inline assembly and some trickies will be needed to make it work properly.
 * Check the already ported platforms for insights on how to do it.
 *
 *****************************************************************************/
__attribute__((naked))
void hkos_hal_restore_context( void ) {
    asm (
        "hkos_hal_restore_context:          \n\t"
        "   pop     r14                     \n\t" // drop the return address
        "   cmp     #0, p_hkos_current_task \n\t"
        "   jz      go_idle                 \n\t"
        "   mov.w   p_hkos_current_task, r15\n\t"
        "   mov.w   0(r15),               r1\n\t"
        "   pop     r4                      \n\t"
        "   pop     r5                      \n\t"
        "   pop     r6                      \n\t"
        "   pop     r7                      \n\t"
        "   pop     r8                      \n\t"
        "   pop     r9                      \n\t"
        "   pop     r10                     \n\t"
        "   pop     r11                     \n\t"
        "   pop     r12                     \n\t"
        "   pop     r13                     \n\t"
        "   pop     r14                     \n\t"
        "   pop     r15                     \n\t"
        "   jmp     done_restore            \n\t"
        "go_idle:                           \n\t"
        "   mov.w   p_hkos_sp,            r1\n\t"
        "done_restore:                      \n\t"
        "   reti                            \n\t"
    );
}

/******************************************************************************
 * TIMER0_A0 ISR. This is the HalfKOS tick timer
 *
 * This interrupt is responsible for context switch and perform the following
 * operations:
 *
 *      1. Save the current task's context (stored in hkos_ram.current_task)
 *      2. Call hkos_scheduler_switch_context
 *      3. Restore the new current task's context
 *
 * OBS: RETI is executed by hkos_hal_restore_context
 *
 *****************************************************************************/
__attribute__((naked))
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void timer_a0_isr(void) {
    save_context_from_interrupt();
    hkos_scheduler_switch_context();
    hkos_hal_restore_context();
}

#endif // ARCH==MSP430G2553
