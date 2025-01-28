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

#ifndef _SERIAL_H
#define _SERIAL_H

#include <inttypes.h>
#include <Stream.h>
extern "C" {
#include "hkos.h"
}

typedef enum {
    SERIAL_5N1,
    SERIAL_6N1,
    SERIAL_7N1,
    SERIAL_8N1,
    SERIAL_5N2,
    SERIAL_6N2,
    SERIAL_7N2,
    SERIAL_8N2,
    SERIAL_5E1,
    SERIAL_6E1,
    SERIAL_7E1,
    SERIAL_8E1,
    SERIAL_5E2,
    SERIAL_6E2,
    SERIAL_7E2,
    SERIAL_8E2,
    SERIAL_5O1,
    SERIAL_6O1,
    SERIAL_7O1,
    SERIAL_8O1,
    SERIAL_5O2,
    SERIAL_6O2,
    SERIAL_7O2,
    SERIAL_8O2,
} serial_config_t;

class HardwareSerial : public Stream
{
    public:
        HardwareSerial( uint8_t port ) : _port( port ) {}

        void begin( unsigned long baud, uint8_t config=SERIAL_8N1 )
        {
            hkos_serial_parity_t parity;
            hkos_serial_data_bits_t data_bits;
            hkos_serial_stop_bits_t stop_bits;
            switch( config )
            {
                case SERIAL_7N1:
                    parity = HKOS_SERIAL_PAR_NONE;
                    data_bits = HKOS_SERIAL_DATA_7;
                    stop_bits = HKOS_SERIAL_STOP_1;
                    break;
                case SERIAL_7N2:
                    parity = HKOS_SERIAL_PAR_NONE;
                    data_bits = HKOS_SERIAL_DATA_7;
                    stop_bits = HKOS_SERIAL_STOP_2;
                    break;
                case SERIAL_8N2:
                    parity = HKOS_SERIAL_PAR_NONE;
                    data_bits = HKOS_SERIAL_DATA_8;
                    stop_bits = HKOS_SERIAL_STOP_2;
                    break;
                case SERIAL_7E1:
                    parity = HKOS_SERIAL_PAR_EVEN;
                    data_bits = HKOS_SERIAL_DATA_7;
                    stop_bits = HKOS_SERIAL_STOP_1;
                    break;
                case SERIAL_8E1:
                    parity = HKOS_SERIAL_PAR_EVEN;
                    data_bits = HKOS_SERIAL_DATA_8;
                    stop_bits = HKOS_SERIAL_STOP_1;
                    break;
                case SERIAL_7E2:
                    parity = HKOS_SERIAL_PAR_EVEN;
                    data_bits = HKOS_SERIAL_DATA_7;
                    stop_bits = HKOS_SERIAL_STOP_2;
                    break;
                case SERIAL_8E2:
                    parity = HKOS_SERIAL_PAR_EVEN;
                    data_bits = HKOS_SERIAL_DATA_8;
                    stop_bits = HKOS_SERIAL_STOP_2;
                    break;
                case SERIAL_7O1:
                    parity = HKOS_SERIAL_PAR_ODD;
                    data_bits = HKOS_SERIAL_DATA_7;
                    stop_bits = HKOS_SERIAL_STOP_1;
                    break;
                case SERIAL_8O1:
                    parity = HKOS_SERIAL_PAR_ODD;
                    data_bits = HKOS_SERIAL_DATA_8;
                    stop_bits = HKOS_SERIAL_STOP_1;
                    break;
                case SERIAL_7O2:
                    parity = HKOS_SERIAL_PAR_ODD;
                    data_bits = HKOS_SERIAL_DATA_7;
                    stop_bits = HKOS_SERIAL_STOP_2;
                    break;
                case SERIAL_8O2:
                    parity = HKOS_SERIAL_PAR_ODD;
                    data_bits = HKOS_SERIAL_DATA_8;
                    stop_bits = HKOS_SERIAL_STOP_2;
                    break;

                case SERIAL_5N1:
                case SERIAL_6N1:
                case SERIAL_5N2:
                case SERIAL_6N2:
                case SERIAL_5E1:
                case SERIAL_6E1:
                case SERIAL_5E2:
                case SERIAL_6E2:
                case SERIAL_5O1:
                case SERIAL_6O1:
                case SERIAL_5O2:
                case SERIAL_6O2:
                    // Not supported, fallback to 8N1
                case SERIAL_8N1:
                default:
                    parity = HKOS_SERIAL_PAR_NONE;
                    data_bits = HKOS_SERIAL_DATA_8;
                    stop_bits = HKOS_SERIAL_STOP_1;
                    break;
            }
            hkos_serial_open( _port, baud, data_bits, stop_bits, parity );
        }

        void end( void )
        {
            hkos_serial_close( _port );
        }

        virtual int available( void )
        {
            return hkos_serial_available( _port );
        }

        virtual int peek( void )
        {
            return hkos_serial_peek( _port );
        }

        virtual int read( void )
        {
            return hkos_serial_read( _port );
        }

        virtual void flush( void )
        {
            hkos_serial_flush( _port );
        }

        virtual size_t write( uint8_t c )
        {
            if ( HKOS_ERROR_NONE ==  hkos_serial_write( _port, c ) )
            {
                return 1;
            } else {
                return 0;
            }
        }

        using Print::write; // pull in write(str) and write(buf, size) from Print

        operator bool()
        {
            return true;
        }

    private:
        uint8_t _port;
};

extern HardwareSerial Serial;

#endif // _SERIAL_H