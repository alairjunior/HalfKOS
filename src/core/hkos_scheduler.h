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
 *****************************************************************************/
typedef struct hkos_task_t hkos_task_t; // forward declaration due to pointers
typedef struct hkos_task_t {
    void*               p_sp;
    hkos_task_t*        p_prev;
    hkos_task_t*        p_next;
    uint16_t            delay_ticks;
} hkos_task_t;


/******************************************************************************
 * HalfKOS mutex structure
 *
 * hkos_mutex_t is used to store the mutex information.
 *
 *****************************************************************************/
typedef struct hkos_mutex_t {
    hkos_task_t*        p_task;
    uint8_t             locked;
} hkos_mutex_t;


/******************************************************************************
 * HalfKOS ram memory structure
 *
 * hkos_ram_t is declared in such a way that it uses all memory available to
 * HalfKOS. Apart from the dynamic buffer, used for dynamic allocation,
 * there is also the stack region.
 *
 * OBS: It would be interesting to have the task pointers inside this structure
 * as well for maintainability. However, for the ISR be able to be declared
 * as naked and, at the same time, we continue to have access to the tasks
 * directly in the basic ASM, the only solution is to have their symbols
 * as globals. Extended ASM cannot be used inside naked functions.
 *
 *****************************************************************************/
typedef struct hkos_ram_t {
    uint8_t    dynamic_buffer[ HKOS_DYNAMIC_RAM ];
    uint8_t    os_stack[
                            HKOS_AVAILABLE_RAM
                            - HKOS_DYNAMIC_RAM
                            - sizeof( hkos_task_t* ) // p_hkos_current_task
                            - sizeof( hkos_task_t* ) // p_hkos_task_list_head
                            - sizeof( void* )        // p_os_sp
                        ];
} hkos_ram_t;

/******************************************************************************
 * The HalfKOS ram structure needs to be accessed for saving and restoring
 * the context, which is done by the tick timer ISR
 *****************************************************************************/
extern hkos_ram_t hkos_ram;

/******************************************************************************
 * Pointer to the current task
 *****************************************************************************/
extern hkos_task_t*  p_hkos_current_task;

/******************************************************************************
 * Pointer to the head of the running tasks list
 *****************************************************************************/
extern hkos_task_t*  p_hkos_running_task_head;

/******************************************************************************
 * Pointer to the HalfKOS stack pointer (idle task)
 *****************************************************************************/
extern void* p_hkos_sp;

/******************************************************************************
 * Current tick count of HalfKOS
 *****************************************************************************/
extern uint16_t hkos_ticks_from_switch;

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
    uint8_t*                p_buffer;
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
 *****************************************************************************/
void  hkos_scheduler_switch_context( void );


/******************************************************************************
 * Called by the HAL to mark the passage of time
 *
 * This function must be called by the tick timer interrupt, which should be
 * implemented in HalfKOS HAL.
 *
 *****************************************************************************/
void  hkos_scheduler_tick_timer( void );


/******************************************************************************
 * Create a mutex
 *
 * @return  Pointer to the mutex structure or NULL if task cannot be created.
 *
 * ***************************************************************************/
void* hkos_scheduler_create_mutex( void );


/******************************************************************************
 * Lock a mutex
 *
 * If mutex is not free, suspend the task until it is free.
 *
 * @param[in]       Pointer to the mutex
 *
 * TODO: Add timeout option
 *
 * ***************************************************************************/
void hkos_scheduler_lock_mutex( hkos_mutex_t* p_mutex );

/******************************************************************************
 * Unlock a mutex
 *
 * @param[in]       Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_scheduler_unlock_mutex( hkos_mutex_t* p_mutex );

/******************************************************************************
 * Destroy an unlocked mutex
 *
 * @param[in]       Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_scheduler_destroy_mutex( hkos_mutex_t* p_mutex );


#endif // __HKOS_SCHEDULER_H
