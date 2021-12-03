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

#include <inttypes.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <hkos_core.h>
#include <hkos_hal.h>
#include <hkos_scheduler.h>
#include <hkos_config.h>

// General Macros
//
#define align(x)        ( ( typeof( x ) )( ( size_t )( x + alignof(max_align_t) - 1 ) \
                            & ( size_t )( ~( alignof(max_align_t) -1 ) ) ) )


/******************************************************************************
 * RAM buffer definition
 *****************************************************************************/
hkos_ram_t hkos_ram;

/******************************************************************************
 * Definition of the pointer to the current task
 *****************************************************************************/
hkos_task_t* p_hkos_current_task;

/******************************************************************************
 * Definition of the pointer to the next task
 *****************************************************************************/
hkos_task_t* p_hkos_next_task;

/******************************************************************************
 * Pointer to the head of the running tasks list
 *****************************************************************************/
hkos_task_t* p_hkos_running_tasks;

/******************************************************************************
 * Pointer to the head of the waiting tasks list
 *****************************************************************************/
hkos_task_t* p_hkos_waiting_tasks;

/******************************************************************************
 * Pointer to the HalfKOS stack pointer (idle task)
 *****************************************************************************/
void* p_hkos_sp;

/******************************************************************************
 * Current tick count of HalfKOS
 *****************************************************************************/
uint16_t hkos_ticks_from_switch;

/**************************************************************************
 * Helper function to allocate a memory in the HKOS RAM buffer
 *
 * This is Naive implementation focused on reducing the control block
 * overhead (no pointers to previous block).
 *
 * ATTENTION: The caller MUST assure there is no concurrent calls to this
 * function. It is not thread safe by design
 *
 * @param[in]   size    size of the block being allocated
 *
 * @return Address of the block or NULL
 *
 * ************************************************************************/
static void* mem_alloc( hkos_dmem_header_t size ) {

    // we always search from the beginning because of our minimal block header
    // Also, the first block is always aligned
    uint8_t* block_addr = (uint8_t*)align(&hkos_ram.dynamic_buffer[0]);
    uint8_t* last_ram_addr = (uint8_t*) &hkos_ram.dynamic_buffer[sizeof(hkos_ram.dynamic_buffer)-1];

    // The block size needs to include the header size and must be aligned
    size = align( size + sizeof(hkos_dmem_header_t) );

    void* address = NULL;
    do {
        while( block_addr <= last_ram_addr ) {
            hkos_ram_block_t* block = (hkos_ram_block_t*)block_addr;

            // if the block is not used and fits the required size
            if ( ( block->header.used == false ) && ( size <= block->header.size ) )
            {
                // Can we split the block?
                if ( block->header.size > size + sizeof(hkos_dmem_header_t)) {
                    hkos_ram_block_t* next = (hkos_ram_block_t*)(block_addr + size);
                    next->header.used = false;
                    next->header.size = block->header.size - size;
                    block->header.size = size;
                }

                // mark the block as used
                block->header.used = true;
                address = &block->p_buffer;
                break;

            } else {
                // go to the next block
                block_addr += block->header.size;
            }
        }
    } while(0);

    // no block available for the requested size
    return address;

}

/**************************************************************************
 * Helper function to free a memory block in the HKOS RAM buffer
 *
 * This is Naive implementation focused on reducing the control block
 * overhead (no pointers to previous block).
 *
 * ATTENTION: The caller MUST assure there is no concurrent calls to this
 * function. It is not thread safe by design
 *
 * @param[in]   p_mem       pointer to the memory being freed
 *
 * TODO: Allow configuring mem_free to clear the freed memory region
 *
 * ************************************************************************/
static void mem_free( void* p_mem ) {

    // the header comes before the block first user address
    p_mem -= sizeof(hkos_dmem_header_t);

    uint8_t* block_addr = (uint8_t*)align(&hkos_ram.dynamic_buffer[0]);
    uint8_t* last_ram_addr = (uint8_t*) &hkos_ram.dynamic_buffer[sizeof(hkos_ram.dynamic_buffer)-1];

    // Check if this is a valid header
    if ( ( (uint8_t*)p_mem >= block_addr ) &&
            ( (uint8_t*)p_mem <= last_ram_addr - sizeof(hkos_dmem_header_t) ) ) {

        // mark the block as not used
        ((hkos_ram_block_t*)p_mem)->header.used = false;

        // Merge the blocks as required
        // we search from the beginning because we don't use doubly
        // linked lists to save memory.
        while ( block_addr <= (uint8_t*)last_ram_addr ) {
            hkos_ram_block_t* block = (hkos_ram_block_t*) block_addr;
            hkos_ram_block_t* next = (hkos_ram_block_t*)( block_addr + block->header.size);

            // check if we can merge with next
            if ( ( block->header.used == false ) &&
                 ( (uint8_t*)next <= last_ram_addr - sizeof(hkos_dmem_header_t) ) &&
                 ( next->header.used == false ) )
            {
                // if we merged them, update the size
                block->header.size += next->header.size;
                // we don't need to jump to the next block
                // because it was merged. We go from the start
                // and compute again the next block
                continue;
            }

            // jump to the next block
            block_addr += block->header.size;
            break;
        }
    }
}

/**************************************************************************
 * Helper function to find the previous task on a list
 *
 * Caller is responsible for making sure this will not be preempted
 *
 * @param[in]       p_task          The task to be added
 * @param[inout]    p_head          The list the task is being added to
 *
 * @return      Pointer to the previous task or NULL if no previous
 *
 * ************************************************************************/
static hkos_task_t* find_previous( hkos_task_t* p_task, hkos_task_t** pp_head ) {

    if ( p_task == *pp_head )
        return NULL;

    hkos_task_t* search = *pp_head;
    for (; search != NULL; search = search->p_next ) {
        if ( search->p_next == p_task )
            return search;
    }

    return NULL;

}

/**************************************************************************
 * Helper function to add a task to the head of a list
 *
 * Caller is responsible for making sure this will not be preempted
 *
 * @param[in]       p_task          The task to be added
 * @param[inout]    p_head          The list the task is being added to
 *
 * ************************************************************************/
static void add_task_to_head( hkos_task_t* p_task, hkos_task_t** pp_head ) {

    if ( p_task == NULL || pp_head == NULL )
        return;

    // Add the task to the head of the list
    p_task->p_next = *pp_head;
    *pp_head = p_task;
}

/**************************************************************************
 * Helper function to add a task to the tail of a list
 *
 * Caller is responsible for making sure this will not be preempted
 *
 * @param[in]       p_task          The task to be added
 * @param[inout]    p_head          The list the task is being added to
 *
 * ************************************************************************/
static void add_task_to_tail( hkos_task_t* p_task, hkos_task_t** pp_head ) {

    if ( p_task == NULL || pp_head == NULL )
        return;

    if ( *pp_head == NULL ) {
        *pp_head = p_task;
    } else {
        // find the tail
        hkos_task_t* tail = *pp_head;
        for (; tail->p_next != NULL; tail = tail->p_next);

        // Add the task to the tail of the list
        tail->p_next = p_task;
    }

    // task is the new tail
    p_task->p_next = NULL;
}

/**************************************************************************
 * Helper function to remove a task from a list
 *
 * Caller is responsible for making sure this will not be preempted
 *
 * @param[in]       p_task          The task to be removed
 * @param[inout]    p_head          The list the task is being removed from
 *
 * ************************************************************************/
static void remove_task_from_list( hkos_task_t* p_task, hkos_task_t** pp_head ) {

    if ( p_task == NULL || pp_head == NULL || *pp_head == NULL )
        return;

    if ( p_task == *pp_head ){
        *pp_head = p_task->p_next;
    } else {
        hkos_task_t* previous = find_previous( p_task, pp_head );
        if ( previous != NULL ) {
            previous->p_next = p_task->p_next;
        }
    }

    // remove links from the task
    p_task->p_next = NULL;
}

/**************************************************************************
 * Helper function to remove a task from the running list
 *
 * Caller is responsible for making sure this will not be preempted
 *
 * @param[in]       p_task          The task to be removed
 *
 * ************************************************************************/
static void remove_task_from_running_list( hkos_task_t* p_task ) {

    if ( p_task != NULL && p_task == p_hkos_next_task ) {
        p_hkos_next_task = p_task->p_next;
    }
    remove_task_from_list( p_task, &p_hkos_running_tasks );
}


/**************************************************************************
 * Helper function to update the waiting list
 *
 * Caller is responsible for making sure this will not be preempted
 *
 * ************************************************************************/
static void update_waiting_list( void ) {
    hkos_task_t* task = p_hkos_waiting_tasks;

    while( task != NULL ) {
        if ( --task->delay_ticks == 0 ) {
            hkos_task_t* next = task->p_next;
            remove_task_from_list( task, &p_hkos_waiting_tasks );
            add_task_to_head( task, &p_hkos_running_tasks );
            task = next;
        } else {
            task = task->p_next;
        }
    }
}

/**************************************************************************
 * Initialize the HalfKOS Scheduler
 *
 * Initialize the HalfKOS Scheduler internal structures
 *
 * ************************************************************************/
void hkos_scheduler_init( void ) {
    // no tasks
    p_hkos_current_task = NULL;
    p_hkos_next_task = NULL;
    p_hkos_running_tasks = NULL;
    p_hkos_waiting_tasks = NULL;

    // all memory is free
    hkos_ram_block_t *first_block = (hkos_ram_block_t*) align(&hkos_ram.dynamic_buffer[0]);
    first_block->header.used = false;
    first_block->header.size = sizeof(hkos_ram.dynamic_buffer);
}

/******************************************************************************
 * Add a task to HalfKOS scheduler
 *
 * HalfKOS uses dynamic memory allocation, so this function also allocates
 * memory for the task being added.
 * Size of each task is stack_size + size of the task structure.
 *
 * @param[in]   p_task_func     Pointer to the task address
 * @param[in]   stack_size      Size of the task's size
 *
 * @return  Pointer to the task structure or NULL if task cannot be created.
 *
 *****************************************************************************/
void* hkos_scheduler_add_task( void (*p_task_func)(), hkos_size_t stack_size ) {

    // Allocate memory for the stack
    // In that memory region, besides the size requested by the user, we also
    // store the task data and the context switch data.
    // This reduces the number of memory blocks per task and, therefore,
    // the ram memory byte count per task
    hkos_size_t total_size = stack_size + sizeof(hkos_task_t) +
                                 hkos_hal_get_min_stack_size();

    hkos_task_t* p_task = (hkos_task_t*)mem_alloc( total_size );

    if ( p_task != NULL ) {
        // Initialize the delay_ticks.
        p_task->delay_ticks = 0;

        // initialize the stack pointer at the top of task's memory
        p_task->p_sp = ( (uint8_t*)p_task ) + total_size;

        // Initialize the stack content and update the stack pointer
        if ( NULL != ( p_task->p_sp = hkos_hal_init_stack(
                                            p_task->p_sp, p_task_func, stack_size
                                        ) ) )
        {
            // It is a round-robin. So, it doesn't matter where you add the task
            add_task_to_head( p_task, &p_hkos_running_tasks );
            return p_task;
        }

        // if something goes wrong, free the memory
        mem_free(p_task);
    }

    // return NULL in case of error
    return NULL;
}

/******************************************************************************
 * Remove a task from HalfKOS Scheduler
 *
 * Besides removing the task from Scheduler, it also frees the task memory.
 *
 * @param[in]   p_task_in       Pointer to the task structure returned by
 *                              hkos_add_task
 *
 * TODO: What if the task to be removed is in a list other than running?
 *
 *****************************************************************************/
void hkos_scheduler_remove_task( void* p_task_in ) {

    if ( p_task_in != NULL ) {

        hkos_task_t* p_task = (hkos_task_t*)p_task_in;

        remove_task_from_running_list( p_task );

        mem_free(p_task);
    }
}

/******************************************************************************
 * Execute a context switch
 *
 * ************************************************************************/
void hkos_scheduler_switch_context( void ) {

    if ( p_hkos_running_tasks == NULL ) {
        p_hkos_current_task = NULL;
        p_hkos_next_task = NULL;
        return; // no task to run
    }

    p_hkos_current_task = p_hkos_next_task;

    if ( p_hkos_current_task == NULL ) {
        p_hkos_current_task = p_hkos_running_tasks;
    }

    p_hkos_next_task = p_hkos_current_task->p_next;


    hkos_ticks_from_switch = 0;
}

/******************************************************************************
 * Called by the HAL to mark the passage of time
 *
 * This function must be called by the tick timer interrupt, which should be
 * implemented in HalfKOS HAL.
 *
 * ************************************************************************/
void hkos_scheduler_tick_timer( void ) {

    update_waiting_list();

    ++hkos_ticks_from_switch;
    if (  hkos_ticks_from_switch >
            ( HKOS_HAL_TICKS_IN_A_SECOND * HKOS_TIME_SLICE / 1000 ) ) {
        hkos_scheduler_switch_context();
    }
}

/******************************************************************************
 * Yield the execution to another task
 *
 * ************************************************************************/
__attribute__((naked))
void hkos_scheduler_yield( void ) {
    hkos_hal_save_context();
    hkos_scheduler_switch_context();
    hkos_hal_restore_context();
}

/******************************************************************************
 * Create a mutex
 *
 * @return  Pointer to the mutex structure or NULL if task cannot be created.
 *
 * ***************************************************************************/
void* hkos_scheduler_create_mutex( void ) {

    hkos_mutex_t* p_mutex = mem_alloc( sizeof(hkos_mutex_t) );

    if ( p_mutex != NULL ) {
        p_mutex->p_task = NULL;
        p_mutex->locked = false;
    }

    return p_mutex;
}

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
void hkos_scheduler_lock_mutex( hkos_mutex_t* p_mutex ) {

    // This should never happen. If this is true, it means the "idle task"
    // is trying to lock the mutex. So we hang here to debug
    if ( p_hkos_current_task == NULL )
        while(1);

    if ( p_mutex->locked ) {
        remove_task_from_running_list( p_hkos_current_task );
        add_task_to_tail( p_hkos_current_task, &p_mutex->p_task );
        hkos_scheduler_yield();

    } else {

        p_mutex->locked = true;

    }
}

/******************************************************************************
 * Unlock a mutex
 *
 * @param[in]       Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_scheduler_unlock_mutex( hkos_mutex_t* p_mutex ) {

    if ( p_mutex != NULL ) {

        if ( p_mutex->p_task != NULL ) {
            hkos_task_t* released = p_mutex->p_task;
            remove_task_from_list( p_mutex->p_task, &p_mutex->p_task );
            add_task_to_head( released, &p_hkos_running_tasks );
        }

        if ( p_mutex->p_task == NULL )
            p_mutex->locked = false;
    }

}

/******************************************************************************
 * Destroy an unlocked mutex
 *
 * @param[in]       Pointer to the mutex
 *
 * ***************************************************************************/
void hkos_scheduler_destroy_mutex( hkos_mutex_t* p_mutex ) {

    if ( p_mutex != NULL && p_mutex->locked == false ) {
        mem_free( p_mutex );
    }

}

/******************************************************************************
 * Suspend the callee for the specified time
 *
 * @param[in]       time_ms     The time to suspend the task in milliseconds
 *
 * ***************************************************************************/
void hkos_scheduler_sleep( uint16_t time_ms ) {
    p_hkos_current_task->delay_ticks =
                 time_ms * ( 1000 / ( HKOS_HAL_TICKS_IN_A_SECOND ) );

    if ( p_hkos_current_task->delay_ticks > 0 ) {
        remove_task_from_running_list( p_hkos_current_task );
        add_task_to_head( p_hkos_current_task, &p_hkos_waiting_tasks );
        hkos_scheduler_yield();
    }
}


