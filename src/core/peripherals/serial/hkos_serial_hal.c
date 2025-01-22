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

#include "hkos_serial_hal.h"
#include <core/hkos_scheduler.h>

// Serial port interface is only enabled when there are serial ports enabled
#if HKOS_SERIAL_PORTS_ENABLE > 0

// Architecture dependent functions
extern hkos_error_code_t hkos_arch_serial_open( uint8_t port,
                                                uint32_t baud,
                                                hkos_serial_data_bits_t data_bits,
                                                hkos_serial_stop_bits_t stop_bits,
                                                hkos_serial_parity_t parity );

extern hkos_error_code_t hkos_arch_serial_close( uint8_t port );

extern hkos_error_code_t hkos_arch_serial_tx_pending( uint8_t port );


// Ring buffers
hkos_serial_ring_buffer hkos_serial_rx_buffer[HKOS_SERIAL_PORTS_ENABLE]  =  { 0 };
hkos_serial_ring_buffer hkos_serial_tx_buffer[HKOS_SERIAL_PORTS_ENABLE]  =  { 0 };

// Waiting tasks list
hkos_task_t*    hkos_serial_waiting_tasks[HKOS_SERIAL_PORTS_ENABLE] = { 0 };

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
                                    hkos_serial_parity_t parity )
{
    hkos_serial_rx_buffer[port].buffer[0] = 0;
    hkos_serial_rx_buffer[port].tail = 0;
    hkos_serial_rx_buffer[port].head = 0;
    hkos_serial_tx_buffer[port].buffer[0] = 0;
    hkos_serial_tx_buffer[port].tail = 0;
    hkos_serial_tx_buffer[port].head = 0;

    return hkos_arch_serial_open( port, baud, data_bits, stop_bits, parity );
}


/**************************************************************************
 * Close a serial port
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_close( uint8_t port )
{
    return hkos_arch_serial_close( port );
}


/**************************************************************************
 * Suspend the calling thread until bytes are available in the serial port
 * rx buffer. If bytes are already available, return immediately.
 *
 * @param[in]       port            Port number
 *
 * @return      number of bytes available
 *
 * ************************************************************************/
uint16_t hkos_serial_wait( uint8_t port )
{
    // This should never happen. If this is true, it means the "idle task"
    // is trying to call serial. So we hang here to debug
    if ( hkos_ram.runtime_data.p_current_task == 0 )
        while(1);

    bool buffer_empty = false;
    hkos_hal_enter_critical_section();
    if ( hkos_serial_available( port ) == 0 )
    {
        hkos_serial_waiting_tasks[port] = hkos_ram.runtime_data.p_current_task;
        buffer_empty = true;
    }
    hkos_hal_exit_critical_section();

    if ( buffer_empty )
        hkos_scheduler_sleep( HKOS_WAIT_FOREVER );

    return hkos_serial_available( port );
}


/**************************************************************************
 * Signals all tasks that are waiting for characters to be received.
 * This function must be called by arch when new characters are put in
 * the rx buffer.
 *
 * @param[in]       port            Port number
 *
 * ************************************************************************/
void hkos_serial_signal_waiting_tasks( uint8_t port )
{
    if ( hkos_serial_waiting_tasks[port] != 0 )
    {
        hkos_scheduler_signal( hkos_serial_waiting_tasks[port] );
        // if this is the last character, we don't
        if ( hkos_serial_available( port ) == 1 )
            hkos_serial_waiting_tasks[port] = 0;
    }
}


/**************************************************************************
 * Read the number of bytes available in the serial port
 *
 * @param[in]       port            Port number
 *
 * @return      number of bytes available
 *
 * ************************************************************************/
uint16_t hkos_serial_available( uint8_t port )
{
    return (uint16_t)(HKOS_SERIAL_BUFFER_SIZE + hkos_serial_rx_buffer[port].head
                 - hkos_serial_rx_buffer[port].tail) % HKOS_SERIAL_BUFFER_SIZE;
}


/**************************************************************************
 * Read one byte from the serial port, but don't remove it from receiving
 * buffer
 *
 * @param[in]       port            Port number
 *
 * @return      byte read or -1
 *
 * ************************************************************************/
int16_t hkos_serial_peek( uint8_t port )
{
    if ( hkos_serial_rx_buffer[port].head == hkos_serial_rx_buffer[port].tail )
    {
        return -1;
    } else {
        return hkos_serial_rx_buffer[port].buffer[hkos_serial_rx_buffer[port].tail];
    }
}


/**************************************************************************
 * Read one byte from the serial port.
 *
 * @param[in]       port            Port number
 *
 * @return      byte read or -1
 *
 * ************************************************************************/
int16_t hkos_serial_read( uint8_t port )
{
    (void)hkos_serial_wait( port );
    char c = hkos_serial_rx_buffer[port].buffer[hkos_serial_rx_buffer[port].tail];
    hkos_serial_rx_buffer[port].tail = (unsigned int)(hkos_serial_rx_buffer[port].tail + 1) % HKOS_SERIAL_BUFFER_SIZE;
    return c;
}

/**************************************************************************
 * Write one byte to the serial port.
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_write( uint8_t port, char data )
{
    uint8_t i = (hkos_serial_tx_buffer[port].head + 1) % HKOS_SERIAL_BUFFER_SIZE;
    while (i == hkos_serial_tx_buffer[port].tail);
    hkos_serial_tx_buffer[port].buffer[hkos_serial_tx_buffer[port].head] = data;
    hkos_serial_tx_buffer[port].head = i;

    return hkos_arch_serial_tx_pending( port );
}


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
hkos_error_code_t hkos_serial_write_buffer( uint8_t port, char* data, uint16_t size )
{
    hkos_error_code_t error_code = HKOS_ERROR_NONE;
    for ( uint16_t i = 0; i < size; ++i )
    {
        if ( ( error_code = hkos_serial_write( port, data[i] ) ) != HKOS_ERROR_NONE )
            break;
    }

    return error_code;
}


/**************************************************************************
 * Write a NULL terminated string to the serial port.
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_print( uint8_t port, char* data )
{
    hkos_error_code_t error_code = HKOS_ERROR_NONE;
    for (; *data != 0; ++data )
    {
        if ( ( error_code = hkos_serial_write( port, *data ) ) != HKOS_ERROR_NONE )
            break;
    }

    return error_code;
}


/**************************************************************************
 * Write a NULL terminated string to the serial port and append a "\n"
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_println( uint8_t port, char* data )
{
    hkos_error_code_t error_code;

    error_code = hkos_serial_print( port, data );
    if ( error_code == HKOS_ERROR_NONE )
    {
        error_code = hkos_serial_print( port, "\n" );
    }

    return error_code;
}


/**************************************************************************
 * Wait all data in the TX buffer to be transmitted
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_serial_flush( uint8_t port )
{
    while (hkos_serial_tx_buffer[port].head != hkos_serial_tx_buffer[port].tail);

    return HKOS_ERROR_NONE;
}

#endif // HKOS_SERIAL_PORTS_ENABLE > 0
