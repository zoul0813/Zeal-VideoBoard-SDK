/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "zvb_gfx.h"
#include "gfx.h"

gfx_error gfx_initialize(uint8_t mode, gfx_context* out)
{
    if (out == NULL ||
        mode > ZVB_CTRL_VID_MODE_GFX_LAST || mode < ZVB_CTRL_VID_MODE_BITMAP_256_MODE)
    {
        return GFX_INVALID_ARG;
    }
    out->video_mode = mode;
    /* 8bpp if mode 4 and 5, 4bpp else (6 and 7) */
    out->bpp = (mode & 2) ? 4 : 8;
    out->alpha_idx = 0;
    /* Backup of the virtual page 0 (where the OS is mapped) */
    out->backup_page = mmu_page0_ro;

    uint8_t* vram = VRAM_VIRT_ADDR;
    /* Create an empty tile */
    gfx_map_tileset(0);
    memset_vram(vram, 0, 256); // 256 would create 2 tiles for 4-bit mode
    /* Set the palette color 0 to alpha/black */
    gfx_map_vram();
    memset_vram(vram + VID_MEM_LAYER0_OFFSET, 0, MAX_LINE * MAX_COL);
    // memset_vram(vram + VID_MEM_LAYER1_OFFSET, 0, MAX_LINE * MAX_COL);
    /* Set the palette first color */
    vram[VID_MEM_PALETTE_OFFSET] = 0x00;
    vram[VID_MEM_PALETTE_OFFSET + 1] = 0x00;

    /* Switch mode */
    zvb_ctrl_video_mode = mode;

    gfx_demap_vram(out->backup_page);

    return GFX_SUCCESS;
}
