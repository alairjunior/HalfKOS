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
#include <core/peripherals/spi/hkos_spi_hal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// SPI port interface is only enabled when there are SPI ports enabled
#if HKOS_SPI_PORTS_ENABLE > 0

#if HKOS_SPI_PORTS_ENABLE > 2
#error MSP430G2553 has only two SPI.
#endif

#define USCI_B0     0
#define USCI_A0     1


/**************************************************************************
 * Open a SPI port
 *
 * @param[in]       port            Port number
 * @param[in]       max_frequency   The maximum frequency of SPI
 * @param[in]       bitorder        The bit order
 * @param[in]       mode            The SPI mode
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_arch_spi_open(   uint8_t port,
                                        uint32_t max_frequency,
                                        hkos_spi_bitorder_t bitorder,
                                        hkos_spi_mode_t mode )
{
    if ( port >= HKOS_SPI_PORTS_ENABLE )
        return HKOS_ERROR_INVALID_RESOURCE;

    volatile uint8_t* UCxCTL0;
    volatile uint8_t* UCxCTL1;
    volatile uint8_t* UCBR0;
    volatile uint8_t* UCBR1;
    uint16_t clk_divider;

    if ( port == USCI_B0 )
    {
        UCxCTL0 = (uint8_t*)&UCB0CTL0;
        UCxCTL1 = (uint8_t*)&UCB0CTL1;
        UCBR0 = (uint8_t*)&UCB0BR0;
        UCBR1 = (uint8_t*)&UCB0BR1;
    } else {
        if ( ( IE2 & UCA0RXIE ) != 0 ) // being used by UART
            return HKOS_ERROR_RESOURCE_BUSY;

        UCxCTL0 = (uint8_t*)&UCA0CTL0;
        UCxCTL1 = (uint8_t*)&UCA0CTL1;
        UCBR0 = (uint8_t*)&UCA0BR0;
        UCBR1 = (uint8_t*)&UCA0BR1;
    }

    // Reset USCI and select SMCLK as source
    *UCxCTL1 = UCSWRST | UCSSEL_2;

    // set USCI as SPI master
    *UCxCTL0 = UCSYNC | UCMST;

    switch ( mode )
    {
        case HKOS_SPI_MODE_0:
            *UCxCTL0 |= UCCKPH;
            break;

        case HKOS_SPI_MODE_1:
            // nothing to set
            break;

        case HKOS_SPI_MODE_2:
            *UCxCTL0 |= (UCCKPL | UCCKPH);
            break;

        case HKOS_SPI_MODE_3:
            *UCxCTL0 |= UCCKPL;
            break;

        default:
            return HKOS_ERROR_NOT_SUPPORTED;
    }

    if ( bitorder == HKOS_SPI_MSB_FIRST )
    {
        *UCxCTL0 |= UCMSB;
    }

    if ( max_frequency >= F_CPU )
    {
        clk_divider = 1;
    } else {
        clk_divider = F_CPU / max_frequency;
    }

    *UCBR0 = clk_divider & 0xFF;
    *UCBR1 = (clk_divider >> 8) & 0xFF;


    // Configure Ports for USCI
    if ( port == USCI_B0 )
    {
        P1SEL  |= ( BIT5 | BIT6 | BIT7 );
        P1SEL2 |= ( BIT5 | BIT6 | BIT7 );
    } else {
        P1SEL  |= ( BIT1 | BIT2 | BIT4 );
        P1SEL2 |= ( BIT1 | BIT2 | BIT4 );
    }


    // Release USCI
    *UCxCTL1 &= ~UCSWRST;

    return HKOS_ERROR_NONE;
}


/**************************************************************************
 * Close a spi port
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_arch_spi_close( uint8_t port )
{
    if ( port >= HKOS_SPI_PORTS_ENABLE )
        return HKOS_ERROR_INVALID_RESOURCE;

    volatile uint8_t* UCxCTL1;

    if ( port == USCI_B0 )
    {
        UCxCTL1 = &UCB0CTL1;
    } else {
        UCxCTL1 = &UCA0CTL1;
    }

    *UCxCTL1 |= UCSWRST; // Put USCI in reset mode

    return HKOS_ERROR_NONE;
}


/**************************************************************************
 * Transfer one byte using SPI. This includes sending the byte and
 * getting one byte from the target device
 *
 * @param[in]       port            Port number
 * @param[in]       data            The data to be written
 *
 * @return      the data received from the SPI bus
 *
 * ************************************************************************/
char hkos_arch_spi_transfer( uint8_t port, char data )
{
    if ( port >= HKOS_SPI_PORTS_ENABLE )
        return HKOS_ERROR_INVALID_RESOURCE;

    volatile uint8_t* UCxTXBUF;
    volatile uint8_t* UCxRXBUF;
    uint8_t  UCxRXIFG;
    uint8_t  UCxTXIFG;

    if ( port == USCI_B0 )
    {
        UCxRXIFG = UCB0RXIFG;
        UCxTXIFG = UCB0TXIFG;
        UCxTXBUF = &UCB0TXBUF;
        UCxRXBUF = &UCB0RXBUF;
    } else {
        if ( ( IE2 & UCA0RXIE ) != 0 ) // being used by UART
            return HKOS_ERROR_RESOURCE_BUSY;

        UCxRXIFG = UCB0RXIFG;
        UCxTXIFG = UCB0TXIFG;
        UCxTXBUF = &UCA0TXBUF;
        UCxRXBUF = &UCA0RXBUF;
    }

	while (!(IFG2 & UCxTXIFG));
	IFG2 &= ~UCxTXIFG;

	*UCxTXBUF = data;

	while (!(IFG2 & UCxRXIFG));

	return *UCxRXBUF;
}

#endif// HKOS_SPI_PORTS_ENABLE > 0
