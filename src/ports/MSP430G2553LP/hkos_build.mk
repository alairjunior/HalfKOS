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

###############################################################################
# Change those variables to match your environment
###############################################################################
TOOLS_DIR := $${HOME}/embedded
GCC_DIR   := $(TOOLS_DIR)/msp430-gcc
###############################################################################

# This defines the msp430-gcc mmcu device name
DEVICE    := MSP430G2553

# This defines the target name
TARGET   := halfkos


HKOS_DIR  := ../..
CXX      := $(GCC_DIR)/bin/msp430-elf-gcc
OBJDUMP  := $(GCC_DIR)/bin/msp430-elf-objdump
DEBUGGER := $(TOOLS_DIR)/mspdebug/mspdebug
SIZE	 := $(GCC_DIR)/bin/msp430-elf-size
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
SRC_DIR  := $(HKOS_DIR)/src
APP_DIR  := $(BUILD)/apps
CXXFLAGS := -mmcu=$(DEVICE) -Wall
LDFLAGS  := -Wl,-Map,$(APP_DIR)/$(DEVICE).map,--gc-sections

INCLUDE  := -I$(SRC_DIR) \
            -I$(SRC_DIR)/core \
			-I$(GCC_DIR)/include \
            -I$(SRC_DIR)/ports/$(HKOS_PORT) \
			-I.

LIB      := -L$(GCC_DIR)/include
SRC      := $(wildcard $(SRC_DIR)/*.c) \
            $(shell find "$(SRC_DIR)/core" -name "*.c") \
			$(shell find "$(SRC_DIR)/ports/$(HKOS_PORT)" -name "*.c") \

SRC_LOC  := $(wildcard ./*.c)

OBJECTS  := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
OBJECTS  += $(patsubst ./%.c,$(OBJ_DIR)/%.o,$(SRC_LOC))

BINARY   := $(TARGET).elf

DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(BINARY)

vpath %.c $(SRC_DIR) ./

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(BINARY): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(BINARY) $^ $(LDFLAGS) $(LIB)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g -Og
debug: all

release: CXXFLAGS += -O3
release: all

run:
	$(DEBUGGER) rf2500 "prog $(APP_DIR)/$(BINARY)"
	$(DEBUGGER) rf2500

disassemble:
	$(OBJDUMP) -S --disassemble $(APP_DIR)/$(BINARY) > $(APP_DIR)/$(TARGET).dump

size:
	$(SIZE) $(APP_DIR)/$(BINARY)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
