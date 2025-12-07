/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_gfx.h"
#include "gfx.h"

void gfx_enable_screen(uint8_t ena)
{
    vid_ctrl_status = ena ? 1 << 7 : 0;
}
