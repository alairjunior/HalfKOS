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
#ifndef __HKOS_SPI_HAL_H
#define __HKOS_SPI_HAL_H

#include <hkos_config.h>

// If HKOS_SPI_PORTS_ENABLE is not defined in hkos_config.h, define it as 0
#ifndef HKOS_SPI_PORTS_ENABLE
#define HKOS_SPI_PORTS_ENABLE        0
#endif

// SPI port interface is only enabled when there are SPI ports enabled
#if HKOS_SPI_PORTS_ENABLE > 0

#include <inttypes.h>
#include <stdbool.h>
#include "hkos_errors.h"

// SPI bit order
typedef enum {
    HKOS_SPI_MSB_FIRST,
    HKOS_SPI_LSB_FIRST
} hkos_spi_bitorder_t;

// SPI data mode
typedef enum {
    HKOS_SPI_MODE_0 = 0,
    HKOS_SPI_MODE_1,
    HKOS_SPI_MODE_2,
    HKOS_SPI_MODE_3
} hkos_spi_mode_t;

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
                                    hkos_spi_mode_t mode );


/**************************************************************************
 * Close a spi port
 *
 * @param[in]       port            Port number
 *
 * @return      HKOS_ERROR_NONE or error code
 *
 * ************************************************************************/
hkos_error_code_t hkos_spi_close( uint8_t port );


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
char hkos_spi_transfer( uint8_t port, char data );


#endif // HKOS_SERIAL_PORTS_ENABLE > 0

#endif //__HKOS_SPI_HAL_H
