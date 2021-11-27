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
#ifndef __HKOS_SCHEDULER_H
#define __HKOS_SCHEDULER_H

#include <hkos_hal.h>
#include <hkos_config.h>


/******************************************************************************
 * HalfKOS task structure
 *
 * hkos_task_t is used to store the task information.
 *
 * OBS: this structure is probably going to change as new features are
 * implemented in HalfKOS, like priorities, wait lists, etc.
 *
 *****************************************************************************/
typedef struct hkos_task_t hkos_task_t; // forward declaration due to next
typedef struct hkos_task_t {
    void*               p_sp;
    hkos_task_t*        p_next;
} hkos_task_t;


/******************************************************************************
 * HalfKOS ram memory structure
 *
 * hkos_ram_t is declared in such a way that it uses all memory available to
 * HalfKOS. Apart from the dynamic buffer, used for dynamic allocation,
 * there is also a pointer to the current task and a pointer to the task list
 * head.
 *
 * OBS: this structure is probably going to change as new features are
 * implemented in HalfKOS, like priorities, wait lists, etc.
 *
 *****************************************************************************/
typedef struct hkos_ram_t {
    hkos_task_t* volatile       p_current_task;
    hkos_task_t* volatile       p_task_head;
    void*                       p_os_sp;
    volatile uint8_t            dynamic_buffer[ HKOS_DYNAMIC_RAM ];
    volatile uint8_t            os_stack[
                                    HKOS_AVAILABLE_RAM
                                    - HKOS_DYNAMIC_RAM
                                    - sizeof( hkos_task_t* ) // p_current_task
                                    - sizeof( hkos_task_t* ) // p_task_head
                                    - sizeof( void* )        // p_os_sp
                                ];
} hkos_ram_t;


/******************************************************************************
 * HalfKOS ram memory dynamic allocation block header structure
 *
 * Dynamic memory allocation within HalfKOS uses a header to store information
 * about the memory block. The header has 1 bit to indicate if the block is
 * used and ( 8*sizeof(hkos_dmem_header_t) - 1 ) bits for the size of the
 * block.
 *
 * OBS: used bit comes last because it is easier to debug in the platform
 *      I am using for development. No other special reason for that.
 *
 *****************************************************************************/
typedef struct hkos_ram_block_header_t {
    hkos_dmem_header_t      size : 8*sizeof(hkos_dmem_header_t) - 1;
    uint8_t                 used : 1;
} hkos_ram_block_header_t;


/******************************************************************************
 * HalfKOS ram memory dynamic allocation block structure
 *
 *****************************************************************************/
typedef struct hkos_ram_block_t {
    hkos_ram_block_header_t header;
    uint8_t* volatile       p_buffer;
} hkos_ram_block_t;


/******************************************************************************
 * Initialize the Scheduler
 *
 * This function initializes the HalfKOS scheduler internal structures and
 * must be called before any of the other scheduler functions.
 *
 *****************************************************************************/
void  hkos_scheduler_init( void );


/******************************************************************************
 * Add a task to HalfKOS scheduler
 *
 * @param[in]   p_task_func     Pointer to the task address
 * @param[in]   stack_size      Size of the task's size
 *
 * @return  Pointer to the task structure or NULL if task cannot be created.
 *
 *****************************************************************************/
void* hkos_scheduler_add_task( void (*p_task_func)(), hkos_size_t stack_size );


/******************************************************************************
 * Remove a task from HalfKOS scheduler
 *
 * @param[in]   p_task_in       Pointer to the task structure returned by
 *                              hkos_add_task
 *
 *****************************************************************************/
void  hkos_scheduler_remove_task( void* p_task_in );


/******************************************************************************
 * Execute a context switch
 *
 * This function must be called by the tick timer interrupt, which should be
 * implemented in HalfKOS HAL.
 *
 *****************************************************************************/
void  hkos_scheduler_switch( void );

#endif // __HKOS_SCHEDULER_H
