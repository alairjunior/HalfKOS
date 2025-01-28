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
#ifndef _SPI_H
#define _SPI_H

extern "C" {
#include "hkos.h"
#include "hkos_config.h"
}

#if defined ( HKOS_ARDUINO_ABSTRACTION_LAYER )

#define SPI_MODE0       0
#define SPI_MODE1       1
#define SPI_MODE2       2
#define SPI_MODE3       3

#define LSBFIRST        HKOS_SPI_LSB_FIRST
#define MSBFIRST        HKOS_SPI_MSB_FIRST

typedef hkos_spi_bitorder_t     BitOrder;

class SPISettings {
public:
  SPISettings() :_clock(1000000), _bitOrder(LSBFIRST), _dataMode(SPI_MODE0){}
  SPISettings( uint32_t clock, hkos_spi_bitorder_t bitOrder, uint8_t dataMode ) :
                                    _clock(clock), _bitOrder(bitOrder), _dataMode(dataMode){}
  uint32_t _clock;
  hkos_spi_bitorder_t  _bitOrder;
  uint8_t  _dataMode;
};

class SPIClass {
public:
    SPIClass( void )
    {
        _port = 0;
        _isMsbFirst = 0;
    }

    void begin( void )
    {
        _port = 0;
    }

    void begin( uint8_t port )
    {
        _port = port;
    }

    void end( void )
    {
        (void)hkos_spi_close( _port );
    }

    void setBitOrder( uint8_t bitOrder )
    {
        // this function should not be used in new projects
    }

    void setDataMode( uint8_t dataMode )
    {
        // this function should not be used in new projects
    }

    void setClockDivider( uint32_t clockDiv )
    {
        // this function should not be used in new projects
    }

    void beginTransaction( SPISettings settings )
    {
        if ( settings._bitOrder == MSBFIRST )
        {
            _isMsbFirst = 1;
        } else {
            _isMsbFirst = 0;
        }
        (void)hkos_spi_open( _port, settings._clock, settings._bitOrder, (hkos_spi_mode_t)settings._dataMode );
    }

    uint8_t transfer( uint8_t data )
    {
        return hkos_spi_transfer( _port, data );
    }

    uint16_t transfer16(uint16_t data)
    {
        uint16_t val;

        if ( _isMsbFirst )
        {
            val = ( transfer( (data >> 8) & 0xFF ) << 8 );
            val |= ( transfer( data & 0xFF ) & 0xFF );
        } else {
            val = ( transfer( data & 0xFF ) & 0xFF );
            val |= ( transfer( (data >> 8) & 0xFF ) << 8 );
        }

        return val;
    }

    void transfer(void *buf, uint16_t count)
    {
        for ( uint16_t i = 0; i < count; ++i )
        {
            (void)hkos_spi_transfer( _port, ((char*)buf)[i] );
        }
    }

    void endTransaction(void)
    {
        (void)hkos_spi_close(_port);
    }

private:
    uint8_t _port;
    uint8_t _isMsbFirst;

};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
extern SPIClass SPI;
#endif

#endif // HKOS_ARDUINO_ABSTRACTION_LAYER

#endif //_SPI_H