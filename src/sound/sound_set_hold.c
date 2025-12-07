/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_sound.h"

extern uint8_t s_mst_hold;

void zvb_sound_set_hold(sound_voice_t voices, uint8_t hold)
{
    zvb_map_peripheral(ZVB_PERI_SOUND_IDX);
    if (hold == 0) {
        s_mst_hold &= ~voices;
    } else {
        s_mst_hold |= voices;
    }
    zvb_peri_sound_hold = s_mst_hold;
}
