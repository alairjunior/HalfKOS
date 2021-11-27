TOOLS_DIR := $${HOME}/embedded
GCC_DIR   := $(TOOLS_DIR)/msp430-gcc
HKOS_DIR  := ../../..

DEVICE   := MSP430G2553
VARIANT  := MSP430G2553LP
CXX      := $(GCC_DIR)/bin/msp430-elf-gcc
OBJDUMP  := $(GCC_DIR)/bin/msp430-elf-objdump
DEBUGGER := $(TOOLS_DIR)/mspdebug/mspdebug
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
SRC_DIR  := $(HKOS_DIR)/src
APP_DIR  := $(BUILD)/apps
CXXFLAGS := -mmcu=$(DEVICE) -Wall -DARCH=$(VARIANT)
LDFLAGS  := -Wl,-Map,$(APP_DIR)/$(DEVICE).map,--gc-sections
TARGET   := halfkos.elf
INCLUDE  := -I$(SRC_DIR) \
            -I$(SRC_DIR)/core \
			-I$(GCC_DIR)/include \
            -I$(SRC_DIR)/ports/$(VARIANT) \
			-I.
LIB      := -L$(GCC_DIR)/include
SRC      := $(wildcard $(SRC_DIR)/*.c) \
            $(wildcard $(SRC_DIR)/core/*.c) \
			$(wildcard $(SRC_DIR)/ports/$(VARIANT)/*.c) \

SRC_LOC  := $(wildcard ./*.c)

OBJECTS  := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
OBJECTS  += $(patsubst ./%.c,$(OBJ_DIR)/%.o,$(SRC_LOC))

DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

vpath %.c $(SRC_DIR) ./

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS) $(LIB)

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
	$(DEBUGGER) rf2500 "prog $(APP_DIR)/$(TARGET)"
	$(DEBUGGER) rf2500

disassemble:
	$(OBJDUMP) -S --disassemble $(APP_DIR)/$(TARGET) > $(APP_DIR)/$(TARGET).dump

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     		"
	@echo "[*] Object dir:      ${OBJ_DIR}    		"
	@echo "[*] Sources:         ${SRC} ${SRC_LOC} 	"
	@echo "[*] Objects:         ${OBJECTS}     		"
	@echo "[*] Dependencies:    ${DEPENDENCIES}		"

