/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_gfx.h"
#include "gfx.h"

gfx_error gfx_wait_vblank(gfx_context* ctx)
{
    (void) ctx;
    while((vid_ctrl_status & 2) == 0) {
    }
    return GFX_SUCCESS;
}

gfx_error gfx_wait_end_vblank(gfx_context* ctx)
{
    (void) ctx;
    while(vid_ctrl_status & 2) {
    }
    return GFX_SUCCESS;
}
