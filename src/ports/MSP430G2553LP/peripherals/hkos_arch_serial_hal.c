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

/**************************************************************************
 *
 * HalfKOS Hardware Abstraction Layer implementation for MSP430G2553
 * The pin definitions are based on the Launchpad EXP430G2
 *
 * ************************************************************************/
#include <msp430.h>
#include <hkos_errors.h>
#include <core/hkos_hal.h>
#include <core/hkos_scheduler.h>
#include <core/peripherals/serial/hkos_serial_hal.h>
#include <stdbool.h>
#include <stddef.h>

// Serial port interface is only enabled when there are serial ports enabled
#if HKOS_SERIAL_PORTS_ENABLE > 0

#if HKOS_SERIAL_PORTS_ENABLE > 1
#error MSP430G2553 has only one UART.
#endif

extern hkos_serial_ring_buffer hkos_serial_rx_buffer[HKOS_SERIAL_PORTS_ENABLE];
extern hkos_serial_ring_buffer hkos_serial_tx_buffer[HKOS_SERIAL_PORTS_ENABLE];

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
hkos_error_code_t hkos_arch_serial_open(    uint8_t port,
                                            uint32_t baud,
                                            hkos_serial_data_bits_t data_bits,
                                            hkos_serial_stop_bits_t stop_bits,
                                            hkos_serial_parity_t parity )
{
    if ( port > 1 )
        return HKOS_ERROR_INVALID_RESOURCE;

    if ( ( IE2 & UCA0RXIE ) != 0 )
        return HKOS_ERROR_RESOURCE_BUSY;

    // Reset UART
    UCA0CTL1 = UCSWRST;

    // Clear all config (should be 0 already)
    UCA0CTL0 = 0;

    switch( parity )
    {
        case HKOS_SERIAL_PAR_ODD:
            UCA0CTL0 |= UCPEN;
            break;

        case HKOS_SERIAL_PAR_EVEN:
            UCA0CTL0 |= ( UCPEN | UCPAR );
            break;

        case HKOS_SERIAL_PAR_NONE:
            // no configuration required
            break;

        case HKOS_SERIAL_PAR_MARK:
        case HKOS_SERIAL_PAR_SPACE:
        default:
            return HKOS_ERROR_NOT_SUPPORTED;
            break;
    }

    if ( data_bits == HKOS_SERIAL_DATA_7 )
    {
        UCA0CTL0 |= UC7BIT;
    }

    if ( stop_bits == HKOS_SERIAL_STOP_2 )
    {
        UCA0CTL0 |= UCSPB;
    }

    // Clock source
    UCA0CTL1 |= UCSSEL_2; //SMCLK = F_CPU

    // Baud rate
    uint32_t pres_N = F_CPU / baud;

    if ( pres_N >= 16 ) // use oversampling
    {
        pres_N = pres_N >> 4; // prescaler is N / 16
        // UCBRF0 receives the rest
        UCA0MCTL = ( ( 1 + ( F_CPU - ( ( pres_N * baud ) << 4 ) ) / ( baud ) ) & 0xF ) * UCBRF0;
        UCA0MCTL |= UCOS16; // use oversampling
    } else { // low frequency mode
        // prescaler is equal to N
        // UCBRS0 receives the rest
        UCA0MCTL = ( ( 1 + ( F_CPU - ( pres_N * baud ) ) / ( baud / 8 ) ) & 0x7 ) * UCBRS0;
        UCA0BR1 = ( ( pres_N >> 8 ) & 0xFF );
    }
    UCA0BR0 = ( pres_N & 0xFF );

    // Configure Ports for tertiary function (USCI)
    P1SEL  |= ( BIT1 | BIT2 );
    P1SEL2 |= ( BIT1 | BIT2 );

    // Release UART
    UCA0CTL1 &= ~UCSWRST;

    // Enable RX interrut
    IE2 |= UCA0RXIE;

    // Set the TX interrupt flag for the first data written
    IFG2 |= UCA0TXIFG;

    return HKOS_ERROR_NONE;
}


/**************************************************************************
 * Closes a serial port
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_arch_serial_close( uint8_t port )
{
    // wait for all data to be sent
    while ( hkos_serial_tx_buffer[port].head != hkos_serial_tx_buffer[port].tail );

    // disable interrupts
    IE2 &= ~( UCA0TXIE | UCA0RXIE );

    // discard all data in the tx buffer
    hkos_serial_rx_buffer[port].head = hkos_serial_rx_buffer[port].tail;

    return HKOS_ERROR_NONE;
}


/**************************************************************************
 * Signals that there is pending data in the tx buffer
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_arch_serial_tx_pending( uint8_t port )
{
    // enable interrupt to start sending data
    IE2 |= UCA0TXIE;
    return HKOS_ERROR_NONE;
}


/**************************************************************************
 * USCI A0 RX Interrupt vector
 *
 * ************************************************************************/
__attribute__((interrupt(USCIAB0RX_VECTOR)))
void USCIAB0RX_ISR(void)
{
    uint8_t port = 0;
    uint16_t i = (hkos_serial_rx_buffer[port].head + 1) % HKOS_SERIAL_BUFFER_SIZE;

    // Check if there is space before adding the character
	if (i != hkos_serial_rx_buffer[port].tail) {
		hkos_serial_rx_buffer[port].buffer[hkos_serial_rx_buffer[port].head] = UCA0RXBUF;
		hkos_serial_rx_buffer[port].head = i;
	}
}


/**************************************************************************
 * USCI A0 TX Interrupt vector
 *
 * ************************************************************************/
__attribute__((interrupt(USCIAB0TX_VECTOR)))
void USCIAB0TX_ISR(void)
{
    uint8_t port = 0;

    if (hkos_serial_tx_buffer[port].head == hkos_serial_tx_buffer[port].tail) {
		// Nothing more to transmit. Disable interrupt
		IE2 &= ~UCA0TXIE;
        // Set the flag for the next write. There we enable
        // interrupts and we will run this ISR again
		IFG2 |= UCA0TXIFG;
		return;
	}

	char c = hkos_serial_tx_buffer[port].buffer[hkos_serial_tx_buffer[port].tail];
	hkos_serial_tx_buffer[port].tail = (hkos_serial_tx_buffer[port].tail + 1) % HKOS_SERIAL_BUFFER_SIZE;
	UCA0TXBUF = c;
}

#endif// HKOS_SERIAL_PORTS_ENABLE > 0

