/******************************************************************************
 *
 * This file is part of HalfKOS.
 * https://github.com/alairjunior/HalfKOS
 *
 * Copyright (c) 2025 Alair Dias Junior.
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
#ifndef __HKOS_SERIAL_HAL_H
#define __HKOS_SERIAL_HAL_H

#include <hkos_config.h>

// If HKOS_SERIAL_PORTS_ENABLE is not defined in hkos_config.h, define it as 0
#ifndef HKOS_SERIAL_PORTS_ENABLE
#define HKOS_SERIAL_PORTS_ENABLE        0
#endif

// Serial port interface is only enabled when there are serial ports enabled
#if HKOS_SERIAL_PORTS_ENABLE > 0

#include <inttypes.h>
#include <stdbool.h>
#include "hkos_errors.h"

// Serial parity
typedef enum {
    HKOS_SERIAL_PAR_NONE,
    HKOS_SERIAL_PAR_ODD,
    HKOS_SERIAL_PAR_EVEN,
    HKOS_SERIAL_PAR_MARK,
    HKOS_SERIAL_PAR_SPACE
} hkos_serial_parity_t;

// Serial data bits
typedef enum {
    HKOS_SERIAL_DATA_8,
    HKOS_SERIAL_DATA_7
} hkos_serial_data_bits_t;

// Serial stop bits
typedef enum {
    HKOS_SERIAL_STOP_1,
    HKOS_SERIAL_STOP_2
} hkos_serial_stop_bits_t;


#ifndef HKOS_SERIAL_BUFFER_SIZE
#define HKOS_SERIAL_BUFFER_SIZE     16
#endif

// Ring buffer structure
typedef struct
{
    char buffer[HKOS_SERIAL_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} hkos_serial_ring_buffer;

/**************************************************************************
 * Open a serial port
 *
 * @param[in]       port            Port number
 * @param[in]       baud            Baud rate
 * @param[in]       data_bits       Number of data bits
 * @param[in]       stop_bits       Number of stop bits
 * @param[in]       parity          Parity
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_open( uint8_t port,
                                    uint32_t baud,
                                    hkos_serial_data_bits_t data_bits,
                                    hkos_serial_stop_bits_t stop_bits,
                                    hkos_serial_parity_t parity );


/**************************************************************************
 * Close a serial port
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_close( uint8_t port );


/**************************************************************************
 * Suspend the calling thread until bytes are available in the serial port
 * rx buffer. If bytes are already available, return immediately.
 *
 * @param[in]       port            Port number
 *
 * @return      number of bytes available
 *
 * ************************************************************************/
uint16_t hkos_serial_wait( uint8_t port );


/**************************************************************************
 * Signals all tasks that are waiting for characters to be received.
 * This function must be called by arch when new characters are put in
 * the rx buffer.
 *
 * @param[in]       port            Port number
 *
 * ************************************************************************/
void hkos_serial_signal_waiting_tasks( uint8_t port );


/**************************************************************************
 * Perform a non-blocking read of the number of bytes available in the
 * serial port
 *
 * @param[in]       port            Port number
 *
 * @return      number of bytes available
 *
 * ************************************************************************/
uint16_t hkos_serial_available( uint8_t port );


/**************************************************************************
 * Perform a non-blocking read of one byte from the serial port,
 * but don't remove it from receiving buffer.
 *
 * @param[in]       port            Port number
 *
 * @return      byte read or -1
 *
 * ************************************************************************/
int16_t hkos_serial_peek( uint8_t port );


/**************************************************************************
 * Perform a blocking read of one byte from the serial port. The byte is
 * removed from the port's rx buffer.
 *
 * @param[in]       port            Port number
 *
 * @return      byte read or -1
 *
 * ************************************************************************/
int16_t hkos_serial_read( uint8_t port );


/**************************************************************************
 * Write one byte to the serial port.
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_write( uint8_t port, char data );


/**************************************************************************
 * Write a buffer to the serial port.
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 * @param[in]       size            The size of the data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_write_buffer( uint8_t port, const char* data, uint16_t size );


/**************************************************************************
 * Write a NULL terminated string to the serial port.
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_print( uint8_t port, const char* data );


/**************************************************************************
 * Write a NULL terminated string to the serial port and append a "\n"
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_println( uint8_t port, const char* data );


/**************************************************************************
 * Wait all data in the TX buffer to be transmitted
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_flush( uint8_t port );

#endif // HKOS_SERIAL_PORTS_ENABLE > 0

#endif //__HKOS_SERIAL_HAL_H
