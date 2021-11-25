#ifndef __HKOS_CONFIG_H
#define __HKOS_CONFIG_H

#include <inttypes.h>

// Configure how many bytes are available for
// HKOS' dynamic memory allocation
#define HKOS_AVAILABLE_RAM          500 // bytes

// Configure the data type of the dynamic memory
// allocation block. Besides the size requested during
// the allocation, the number of bytes of the data type
// below will also be added to the block. One bit of the
// header is used for marking the block as used/free and
// the remaining bits are used for the size of the block.
// Hence, blocks can be up to 2^(datatype bits - 1) long.
typedef uint16_t                    hkos_dmem_header_t;


// Configure HalfKOS time slice
#define HKOS_TIME_SLICE             32 // ms

#endif // __HKOS_CONFIG_H
