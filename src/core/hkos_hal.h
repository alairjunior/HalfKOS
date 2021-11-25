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
 * HAL must be implemented for any given microcontroller configuration
 * There are optional functions, mandatory functions, and functions that
 * must be called:
 *
 * 1. OPTIONAL functions: used by the user code (tasks) and may be left
 *                          unimplemented if user code does not call them.
 *
 *      - All GPIO functions
 *
 * 2. MANDATORY functions: they are called by HalfKOS core functtions and
 *                          must be implemented otherwise a compilation time
 *                          error will arise.
 *
 *      - hkos_hal_init_stack
 *      - hkos_hal_start_tick_timer
 *      - hkos_hal_lp_idle
 *
 * See the description of functions for details on how to implement them.
 * Also, check the already ported microcontrollers for better insights on
 * how to do it.
 *
 *****************************************************************************/
#ifndef __HKOS_HAL_H
#define __HKOS_HAL_H

#include <inttypes.h>
#include <stdbool.h>
#include <hkos_core.h>
#include <hkos_config.h>

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


/******************************************************************************
 * Initialize HalfKOS Hardware abstraction layer (HAL)
 *
 * This function must be called before calling any other HalfKOS HAL function.
 *
 *****************************************************************************/
void hkos_hal_init( void );


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
void hkos_hal_gpio_config( uint8_t pin, gpio_pin_mode_t mode );


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
void hkos_hal_gpio_write( uint8_t pin, gpio_value_t value );


/******************************************************************************
 * Toggle the value of a GPIO pin
 *
 * This function toggles (switches) the value of the provided GPIO pin.
 * If pin is HIGH, it will become LOW, and vice-versa.
 *
 * @param[in]   pin     pin number
 *
 *****************************************************************************/
void hkos_hal_gpio_toggle( uint8_t pin );


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
gpio_value_t hkos_hal_gpio_read( uint8_t pin );


/******************************************************************************
 * Initialize the task stack.
 *
 * When adding a task to the scheduler, HalfKOS allocates a memory to the
 * task and fills the stack part of the memory with values to allow the
 * first context switch to the task. Task stack initialization is highly
 * dependent on the target CPU and hence needs to be implemented for each
 * CPU supported.
 *
 * @param[in]   pp_sp       a pointer to the stack pointer indicating the
 *                          memory region of the task stack
 * @param[in]   p_pc        a pointer to the beginning of the task code
 * @param[in]   stack_size  the size of the task's stack
 *
 * @return  The value of stack pointer after the stack initialization
 *
 *****************************************************************************/
void* hkos_hal_init_stack( void** pp_sp, void* p_pc, hkos_size_t stack_size );


/******************************************************************************
 * Start the tick timer responsible for the HalfKOS context switch
 *
 * This function must start executing the timer that will handle HalfKOS
 * context switch. The timer interrupt also needs to be implemented and needs
 * perform the following operations:
 *
 *      1. Save the current task's context (stored in hkos_ram.current_task)
 *      2. Call hkos_scheduler_switch
 *      3. Restore the new current task's context
 *
 *****************************************************************************/
void hkos_hal_start_tick_timer( void );


/******************************************************************************
 * Enter the Microcontroller low power mode
 *
 * This function is called by the scheduler to put the Microcontroller in
 * low power mode. When there is no task to be executed, scheduler will call
 * it. IMPORTANT: this function must not prevent the tick timer from running
 * otherwise the system will not return from low-power state.
 *
 *****************************************************************************/
void hkos_hal_enter_lp( void );

/******************************************************************************
 * Enter the Microcontroller low power mode
 *
 * This function is called by the scheduler to pull the Microcontroller out of
 * low power mode.
 *
 *****************************************************************************/
void hkos_hal_exit_lp( void );


#endif // __HKOS_HAL_H
