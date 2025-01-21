#******************************************************************************
 #
 # This file is part of HalfKOS.
 # https://github.com/alairjunior/HalfKOS
 #
 # Copyright (c) 2021-2025 Alair Dias Junior.
 #
 # HalfKOS is free software: you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # HalfKOS is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with HalfKOS.  If not, see <https://www.gnu.org/licenses/>.
 #
 #****************************************************************************/

ifneq ($(PLATFORM),)
HKOS_PORT := ${PLATFORM}
include ../../src/ports/${HKOS_PORT}/hkos_build.mk
else
$(info )
$(info Please, specify PLATFORM. For example: "make PLATFORM=MSP430G2553LP")
$(info )
endif