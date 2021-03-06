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
#ifndef __HKOS_H
#define __HKOS_H

#include <core/hkos_core.h>
#include <core/hkos_hal_types.h>

/******************************************************************************
 * Initialize HalfKOS
 *
 * This function must be called before calling any other HalfKOS function.
 *
 *****************************************************************************/
void hkos_init( void );


/******************************************************************************
 * Add a task to HalfKOS scheduler
 *
 * @param[in]   p_task_func     Pointer to the task address
 * @param[in]   stack_size      Size of the task's size
 *
 * @return  Pointer to the task structure or NULL if task cannot be created.
 *
 *****************************************************************************/
void* hkos_add_task( void (*p_task_func)(), hkos_size_t stack_size );


/******************************************************************************
 * Remove a task from HalfKOS scheduler
 *
 * @param[in]   p_task_in       Pointer to the task structure returned by
 *                              hkos_add_task
 *
 *****************************************************************************/
void hkos_remove_task( void* p_task_in );


/******************************************************************************
 * Start the HalfKOS scheduler
 *
 *****************************************************************************/
void hkos_start( void );


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
void hkos_gpio_config( uint8_t pin, gpio_pin_mode_t mode );


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
void hkos_gpio_write( uint8_t pin, gpio_value_t value );


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
gpio_value_t hkos_gpio_read( uint8_t pin );


/******************************************************************************
 * Create a mutex
 *
 * @return  Pointer to the mutex structure or NULL if task cannot be created.
 *
 * ***************************************************************************/
void* hkos_create_mutex( void );


/******************************************************************************
 * Lock a mutex
 *
 * If mutex is not free, suspend the task until it is free.
 *
 * @param[in]       p_mutex     Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_lock_mutex( void* p_mutex );


/******************************************************************************
 * Unlock a mutex
 *
 * @param[in]       p_mutex     Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_unlock_mutex( void* p_mutex );


/******************************************************************************
 * Destroy a mutex
 *
 * @param[in]       p_mutex     Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_destroy_mutex( void* p_mutex );


/******************************************************************************
 * Suspend the callee for the specified time
 *
 * @param[in]       time_ms     The time to suspend the task in milliseconds
 *
 * ***************************************************************************/
void hkos_sleep( uint16_t time_ms );


#endif //__HKOS_H