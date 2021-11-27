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

// General Macros
//
#define align(x)        ( ( typeof( x ) )( ( size_t )( x + alignof(max_align_t) - 1 ) \
                            & ( size_t )( ~( alignof(max_align_t) -1 ) ) ) )

// RAM buffer definition
//
hkos_ram_t hkos_ram;

/**************************************************************************
 * Helper function to allocate a memory in the HKOS RAM buffer
 *
 * This is Naive implementation focused on reducing the control block
 * overhead (no pointers to previous block).
 *
 * @param[in]   size    size of the block being allocated
 *
 * @return Address of the block or NULL
 *
 * TODO: Make this safe to be called when scheduler is running
 *
 * ************************************************************************/
static void* mem_alloc( hkos_dmem_header_t size ) {

    // we always search from the beginning because of our minimal block header
    // Also, the first block is always aligned
    uint8_t* block_addr = (uint8_t*)align(&hkos_ram.dynamic_buffer[0]);
    uint8_t* last_ram_addr = (uint8_t*) &hkos_ram.dynamic_buffer[sizeof(hkos_ram.dynamic_buffer)-1];

    // The block size needs to include the header size and must be aligned
    size = align( size + sizeof(hkos_dmem_header_t) );

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

            return (void*)&block->p_buffer;
        } else {
            // go to the next block
            block_addr += block->header.size;
        }
    }

    // no block available for the requested size
    return 0;

}

/**************************************************************************
 * Helper function to free a memory block in the HKOS RAM buffer
 *
 * This is Naive implementation focused on reducing the control block
 * overhead (no pointers to previous block).
 *
 * @param[in]   p_mem       pointer to the memory being freed
 *
 * TODO: Make this safe to be called when scheduler is running
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
 * Initialize the HalfKOS Scheduler
 *
 * Initialize the HalfKOS Scheduler internal structures
 *
 * ************************************************************************/
void hkos_scheduler_init( void ) {
    // no tasks
    hkos_ram.p_current_task = NULL;
    hkos_ram.p_task_head = NULL;

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
 * TODO: Make this safe to be called when scheduler is running
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

    hkos_task_t* p_task = (hkos_task_t*)mem_alloc( total_size  );

    if ( p_task != NULL ) {
        p_task->p_next = hkos_ram.p_task_head;
        hkos_ram.p_task_head = p_task;

        // initialize the stack pointer at the top of task's memory
        p_task->p_sp = ( (uint8_t*)p_task ) + total_size;

        // Initialize the stack content and update the stack pointer
        if ( NULL != ( p_task->p_sp = hkos_hal_init_stack( p_task->p_sp, p_task_func, stack_size ) ) )
            return p_task;

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
 * Since the Task List is not doubly linked, we need to search from the
 * head so we can update the pointers.
 *
 * @param[in]   p_task_in       Pointer to the task structure returned by
 *                              hkos_add_task
 *
 * TODO: Make this safe to be called when scheduler is running
 * TODO: Even if thread safe, need to solve the issue when the thread
 *       removes itself
 *
 *****************************************************************************/
void hkos_scheduler_remove_task( void* p_task_in ) {

    if ( p_task_in != NULL ) {

        hkos_task_t* p_task = (hkos_task_t*)p_task_in;
        hkos_task_t* p_search_task;
        hkos_task_t* p_previous = NULL;

        for ( p_search_task = hkos_ram.p_task_head;
                    p_search_task != NULL;
                    p_search_task = p_search_task->p_next ) {

            if ( p_task == p_search_task ) {
                // found the task
                if ( p_previous == NULL )
                {
                    hkos_ram.p_task_head = p_task->p_next;
                } else {
                    p_previous->p_next = p_task->p_next;
                }
                if ( hkos_ram.p_current_task == p_task ) {
                    hkos_ram.p_current_task = p_task->p_next;
                }

                mem_free(p_task);

                break;
            }

            p_previous = p_search_task;
        }
    }
}

/******************************************************************************
 * Execute a context switch
 *
 * This function must be called by the tick timer interrupt, which should be
 * implemented in HalfKOS HAL.
 *
 * ************************************************************************/
void hkos_scheduler_switch( void ) {

    if ( hkos_ram.p_task_head != NULL ) {
        if ( hkos_ram.p_current_task != NULL ) {
            hkos_ram.p_current_task = hkos_ram.p_current_task->p_next;
        }

        if ( hkos_ram.p_current_task == NULL )
            hkos_ram.p_current_task = hkos_ram.p_task_head;

    }
}
