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
 *      - All APIs in the peripherals folder
 *
 * 2. MANDATORY functions: they are called by HalfKOS core functtions and
 *                          must be implemented otherwise a compilation time
 *                          error will arise.
 *
 *      - hkos_hal_init
 *      - hkos_hal_init_stack
 *      - hkos_hal_get_min_stack_size
 *      - hkos_hal_jump_to_os
 *      - hkos_hal_enter_critical_section
 *      - hkos_hal_exit_critical_section
 *
 * 3. FUNCTIONS TO BE CALLED :
 *
 *      - hkos_scheduler_tick_timer: must be called from the interrupt
 *        that handles the context switch, after saving the context and
 *        before restoring the context of the next task.
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

/******************************************************************************
 * Initialize HalfKOS Hardware abstraction layer (HAL)
 *
 * This function must be called before calling any other HalfKOS HAL function.
 *
 *****************************************************************************/
void hkos_hal_init( void );


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
 * Save context for a context switch
 *
 * This function will save the context before a context switch. It must save
 * all the context information for the current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler. Most probably the function will be written using
 * inline assembly and some trickies will be needed to make it work properly.
 * Check the already ported platforms for insights on how to do it.
 *
 *****************************************************************************/
void hkos_hal_save_context( void ) __attribute__((naked));


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
void hkos_hal_restore_context( void ) __attribute__((naked));


/******************************************************************************
 * Jump to the operating system
 *
 * This function will point the stack pointer to the operating system's stack
 * and start the timer to do the context switch operation. The interrupt to
 * handle the context switch must do the following operations:
 *
 *      1. Save the current task's context (stored in hkos_ram.current_task)
 *      2. Call hkos_scheduler_tick_timer
 *      3. Restore the new current task's context
 *
 *****************************************************************************/
void hkos_hal_jump_to_os( void );


/******************************************************************************
 * Enter Critical Section
 *
 * When inside a critical section, the code cannot be preempted. This can be
 * done using software only (like Lamport's bakery algorithm), but hardware
 * support is the preferable approach. So we leave it for the HAL to decide.
 *
 * OBS: this function must not be called by user code, because it may affect
 * the time counting. All use of this function MUST be restricted to HalfKOS
 * core,
 *
 *****************************************************************************/
void hkos_hal_enter_critical_section( void );


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
 *****************************************************************************/
void hkos_hal_exit_critical_section( void );


#endif // __HKOS_HAL_H
