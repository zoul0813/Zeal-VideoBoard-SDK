/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_sound.h"

void zvb_sound_set_volumes(sound_volume_t left, sound_volume_t right)
{
    zvb_map_peripheral(ZVB_PERI_SOUND_IDX);
    uint8_t val = (left == VOL_0) ? 0x40 : left;
    val |=  (right == VOL_0) ? 0x80 : (right << 2);
    zvb_peri_sound_master_vol = val;
}
