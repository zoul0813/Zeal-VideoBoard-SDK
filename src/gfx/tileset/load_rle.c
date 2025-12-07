/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_gfx.h"
#include "../gfx.h"

gfx_error gfx_tileset_load_rle(gfx_context* ctx, uint8_t* data, uint16_t size, uint16_t from, uint8_t pal_offset, uint8_t opacity) {
    uint8_t buffer[TILE_SIZE_8BIT + 16];
    uint8_t length = 0;
    uint16_t i = 0; // data index
    uint16_t j = 0; // buffer index
    uint16_t length_of_data = size - 1;

    uint16_t tile_count = 0;

    while (i < length_of_data) {
        length = data[i]; // RLE byte
        i++; // every other byte
        if(length >= 0x80) {
            length = (length - 0x80) + 1;
            uint8_t value = data[i];
            while(length--) {
                buffer[j++] = value;
            }
            i++;
        } else {
            length++;
            mem_cpy(&buffer[j], &data[i], length);
            i += length;
            j += length;
        }

        if(j >= TILE_SIZE_8BIT) {
            gfx_tileset_options options = {
                .compression = TILESET_COMP_NONE, // load uncompressed data
                .from_byte = from + (tile_count * TILE_SIZE_8BIT), // offset by the current tile index?
                .pal_offset = pal_offset, // copy over
                .opacity = opacity, // copy over
            };
            gfx_tileset_load(ctx, &buffer, TILE_SIZE_8BIT,  &options);
            tile_count++;
            j = 0;
        }
    }

    return GFX_SUCCESS;
}
