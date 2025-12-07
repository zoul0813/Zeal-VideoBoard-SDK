#
# SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
#
# SPDX-License-Identifier: Apache-2.0
#

SHELL := /bin/bash

SRC_DIR=src
OUTPUT_DIR=lib

ifndef ZVB_SDK_PATH
$(error "Please define ZVB_SDK_PATH environment variable. It must point to Zeal Video Board SDK path.")
endif
ZVB_INCLUDE=$(ZVB_SDK_PATH)/include/

CC=sdcc
AR=sdar
# Specify Z80 as the target, compile without linking, and place all the code in TEXT section
# (_CODE must be replace).
CFLAGS=-mz80 -c --codeseg TEXT -I$(ZVB_INCLUDE) --opt-code-speed

# Source files for each library
ZVB_CRC_SOURCES := $(wildcard $(SRC_DIR)/crc/*.c)
ZVB_DMA_SOURCES := $(wildcard $(SRC_DIR)/dma/*.c)
ZVB_SOUND_SOURCES := $(wildcard $(SRC_DIR)/sound/*.c)
ZVB_GFX_SOURCES := $(wildcard $(SRC_DIR)/gfx/*.c)

# Object files
ZVB_CRC_OBJS := $(patsubst $(SRC_DIR)/crc/%.c,$(OUTPUT_DIR)/%.rel,$(ZVB_CRC_SOURCES))
ZVB_DMA_OBJS := $(patsubst $(SRC_DIR)/dma/%.c,$(OUTPUT_DIR)/%.rel,$(ZVB_DMA_SOURCES))
ZVB_SOUND_OBJS := $(patsubst $(SRC_DIR)/sound/%.c,$(OUTPUT_DIR)/%.rel,$(ZVB_SOUND_SOURCES))
ZVB_GFX_OBJS := $(patsubst $(SRC_DIR)/gfx/%.c,$(OUTPUT_DIR)/%.rel,$(ZVB_GFX_SOURCES))

.PHONY: all clean

all: $(OUTPUT_DIR) $(OUTPUT_DIR)/zvb_gfx.lib $(OUTPUT_DIR)/zvb_crc.lib $(OUTPUT_DIR)/zvb_sound.lib $(OUTPUT_DIR)/zvb_dma.lib $(OUTPUT_DIR)/zvb.lib
	@bash -c 'echo -e "\x1b[32;1mSuccess, libraries generated\x1b[0m"'

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Pattern rule to compile C files
$(OUTPUT_DIR)/%.rel: $(SRC_DIR)/crc/%.c
	$(CC) $(CFLAGS) -o $(OUTPUT_DIR)/ $<

$(OUTPUT_DIR)/%.rel: $(SRC_DIR)/dma/%.c
	$(CC) $(CFLAGS) -o $(OUTPUT_DIR)/ $<

$(OUTPUT_DIR)/%.rel: $(SRC_DIR)/sound/%.c
	$(CC) $(CFLAGS) -o $(OUTPUT_DIR)/ $<

$(OUTPUT_DIR)/%.rel: $(SRC_DIR)/gfx/%.c
	$(CC) $(CFLAGS) -o $(OUTPUT_DIR)/ $<

# Library rules
$(OUTPUT_DIR)/zvb_crc.lib: $(ZVB_CRC_OBJS)
	$(AR) -rc $@ $^

$(OUTPUT_DIR)/zvb_dma.lib: $(ZVB_DMA_OBJS)
	$(AR) -rc $@ $^

$(OUTPUT_DIR)/zvb_sound.lib: $(ZVB_SOUND_OBJS)
	$(AR) -rc $@ $^

$(OUTPUT_DIR)/zvb_gfx.lib: $(ZVB_GFX_OBJS)
	$(AR) -rc $@ $^

# Combined library with all modules
$(OUTPUT_DIR)/zvb.lib: $(ZVB_CRC_OBJS) $(ZVB_DMA_OBJS) $(ZVB_SOUND_OBJS) $(ZVB_GFX_OBJS)
	$(AR) -rc $@ $^

clean:
	rm -f lib/*
