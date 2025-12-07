/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stddef.h>
#include "zvb_hardware.h"

/**
 * @brief VRAM will be mapped in page 0, which starts at address 0...
 */
#define VRAM_VIRT_ADDR  (0x0000)

#define MAX_LINE        39
#define MAX_COL         79
#define TILE_SIZE_8BIT  256

#ifndef MIN
#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif

/* Workaround to get the page 0 value from the MMU */
const __sfr __banked __at(0xF0) mmu_page0_ro;
__sfr __at(0xF0) mmu_page0;
__sfr __banked __at(0x9d) vid_ctrl_status;

void gfx_map_vram(void);
void gfx_map_tileset(uint8_t part);
void gfx_demap_vram(const uint8_t os);

void memset_vram(void* ptr, int a, uint16_t size) __naked;
void memaddcpy(uint8_t* dst, uint8_t* src, size_t size, uint8_t opacity, uint8_t offset);
void* mem_cpy(void* dst, const void* src, size_t size);