/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_sound.h"

void zvb_sound_set_volume(sound_volume_t vol)
{
    zvb_map_peripheral(ZVB_PERI_SOUND_IDX);
    zvb_sound_set_volumes(vol, vol);
}
