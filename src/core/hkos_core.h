/******************************************************************************
 *
 * This file is part of HalfKOS.
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

#ifndef __HKOS_CORE_H
#define __HKOS_CORE_H

#include <hkos_arch_hal.h>

/******************************************************************************
 * Size data type is the same as the dynamic header size data type
 *
 * hkos_size_t is used accros HalfKOS core to store the stack sizes,
 * and other structures' sizes.
 *
 *****************************************************************************/
typedef hkos_dmem_header_t  hkos_size_t;

#endif //__HKOS_CORE_H