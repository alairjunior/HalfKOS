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
#include <core/peripherals/gpio/hkos_gpio_hal.h>

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


/******************************************************************************
 * Suspend the callee until it is signalled
 *
 * ***************************************************************************/
void hkos_suspend( void );


/******************************************************************************
 * Signal a suspended task
 *
 * @param[in]       Pointer to the task
 *
 * ***************************************************************************/
void hkos_signal( void* pTask );

#endif //__HKOS_H