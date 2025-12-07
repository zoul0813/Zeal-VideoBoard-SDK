/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_sound.h"

void zvb_sound_set_channels(sound_voice_t left_voices, sound_voice_t right_voices)
{
    zvb_map_peripheral(ZVB_PERI_SOUND_IDX);
    zvb_peri_sound_left_channel = left_voices;
    zvb_peri_sound_right_channel = right_voices;
}
