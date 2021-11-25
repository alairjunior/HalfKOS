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

#include <core/hkos_hal.h>
#include <core/hkos_scheduler.h>

/******************************************************************************
 * Initialize HalfKOS
 *
 * This function must be called before calling any other HalfKOS function.
 *
 *****************************************************************************/
inline void hkos_init( void ) {
    hkos_hal_init();
    hkos_scheduler_init();
}

/******************************************************************************
 * Add a task to HalfKOS scheduler
 *
 * @param[in]   p_task_func     Pointer to the task address
 * @param[in]   stack_size      Size of the task's size
 *
 * @return  Pointer to the task structure or NULL if task cannot be created.
 *
 *****************************************************************************/
inline void* hkos_add_task( void (*p_task_func)(), hkos_size_t stack_size ) {
    return hkos_scheduler_add_task( p_task_func, stack_size );
}

/******************************************************************************
 * Remove a task from HalfKOS scheduler
 *
 * @param[in]   p_task_in       Pointer to the task structure returned by
 *                              hkos_add_task
 *
 *****************************************************************************/
inline void hkos_remove_task( void* p_task_in ) {
    hkos_scheduler_remove_task( p_task_in );
}

/******************************************************************************
 * Start the HalfKOS scheduler
 *
 *****************************************************************************/
inline void hkos_start( void ) {
    hkos_scheduler_start();
}

#endif //__HKOS_H