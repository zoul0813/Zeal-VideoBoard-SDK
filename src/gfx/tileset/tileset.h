#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "zvb_gfx.h"

gfx_error gfx_tileset_load_bitmap(gfx_context* ctx, uint8_t* tileset, uint16_t size, uint16_t from, uint8_t pal_offset);
gfx_error gfx_tileset_load_2bit(gfx_context* ctx, uint8_t* tileset, uint16_t size, uint16_t from, uint8_t pal_offset, uint8_t opacity);
gfx_error gfx_tileset_load_nibble(gfx_context* ctx, uint8_t* tileset, uint16_t size, uint16_t from, uint8_t pal_offset, uint8_t opacity);
gfx_error gfx_tileset_load_rle(gfx_context* ctx, uint8_t* data, uint16_t size, uint16_t from, uint8_t pal_offset, uint8_t opacity);