#ifndef __HKOS_CONFIG_H
#define __HKOS_CONFIG_H

#include <inttypes.h>
#include <stdbool.h>

// Configure how many bytes are available for
// HKOS' dynamic memory allocation
//
// This should be 512, which is the size of MSP430G2553
// RAM, but the original scat file from TI reserves 4 bytes
// for the heap. We could remove that from the scat file
// but since it comes with msp430-gcc, we preferred to
// use 4 bytes less and keep the default scat file.
// If you need that 4 bytes, change the scat file and
// you'll be able to use 512 bytes here.
#define HKOS_AVAILABLE_RAM          508 // bytes

// Configure the data type of the dynamic memory
// allocation block header. Besides the size requested
// during the allocation, the number of bytes of the data
// type below will be included to the block. One bit of this
// header is used for marking the block as used/free and
// the remaining bits are used for the size of the block.
// Hence, blocks can be up to 2^(datatype bits - 1) long.
typedef uint16_t                    hkos_dmem_header_t;


// Configure HalfKOS time slice
#define HKOS_TIME_SLICE             5 // ms

// Paint the stack when creating the task for stack usage
// analysis
#define HKOS_PAINT_TASK_STACK       true
#define HKOS_STACK_PAINT_VALUE      0xFFFF

#endif // __HKOS_CONFIG_H
