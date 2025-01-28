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

#include "hkos_spi_hal.h"
#include "hkos_errors.h"

// SPI port interface is only enabled when there are SPI ports enabled
#if HKOS_SPI_PORTS_ENABLE > 0

extern hkos_error_code_t hkos_arch_spi_open(    uint8_t port,
                                                uint32_t max_frequency,
                                                hkos_spi_bitorder_t bitorder,
                                                hkos_spi_mode_t mode );

extern hkos_error_code_t hkos_arch_spi_close( uint8_t port );

extern char hkos_arch_spi_transfer( uint8_t port, char data );

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
hkos_error_code_t hkos_spi_open(    uint8_t port,
                                    uint32_t max_frequency,
                                    hkos_spi_bitorder_t bitorder,
                                    hkos_spi_mode_t mode )
{
    return hkos_arch_spi_open( port, max_frequency, bitorder, mode );
}


/**************************************************************************
 * Close a spi port
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_spi_close( uint8_t port )
{
    return hkos_arch_spi_close( port );
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
char hkos_spi_transfer( uint8_t port, char data )
{
    return hkos_arch_spi_transfer( port, data );
}

#endif // HKOS_SPI_PORTS_ENABLE
