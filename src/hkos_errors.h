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
#ifndef __HKOS_ERRORS_H
#define __HKOS_ERRORS_H

typedef enum {
    HKOS_ERROR_NONE = 0,
    HKOS_ERROR_NOT_IMPLEMENTED,
    HKOS_ERROR_INVALID_RESOURCE,
    HKOS_ERROR_RESOURCE_BUSY,
    HKOS_ERROR_NOT_SUPPORTED
} hkos_error_code_t;

#endif //__HKOS_ERRORS_H
