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
 *      - hkos_hal_init
 *      - hkos_hal_init_stack
 *      - hkos_hal_get_min_stack_size
 *      - hkos_hal_jump_to_os
 *
 * 3. FUNCTIONS TO BE CALLED :
 *
 *      - hkos_scheduler_switch: must be called from the interrupt that
 *        handles the context switch, after saving the context and
 *        before restoring the context of the next thread.
 *
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
#include <hkos_hal_types.h>

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
 * @param[in]   p_sp        a pointer to the stack pointer indicating the
 *                          memory region of the task stack
 * @param[in]   p_pc        a pointer to the beginning of the task code
 * @param[in]   stack_size  the size of the task's stack
 *
 * @return  The value of stack pointer after the stack initialization
 *
 *****************************************************************************/
void* hkos_hal_init_stack( void* p_sp, void* p_pc, hkos_size_t stack_size );


/******************************************************************************
 * Get the minimal stack size
 *
 * Depending on the CPU, the minimum stack size can be different. Besides the
 * task's user operations, the stack needs to be big enough to store data
 * during the context switch
 *
 *****************************************************************************/
hkos_size_t hkos_hal_get_min_stack_size( void );


/******************************************************************************
 * Jump to the operating system
 *
 * This function will point the stack pointer to the operating system's stack
 * and start the timer to do the context switch operation. The interrupt to
 * handle the context switch must do the following operations:
 *
 *      1. Save the current task's context (stored in hkos_ram.current_task)
 *      2. Call hkos_scheduler_switch
 *      3. Restore the new current task's context
 *
 *****************************************************************************/
void hkos_hal_jump_to_os( void );


#endif // __HKOS_HAL_H
