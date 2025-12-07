/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_sound.h"

extern uint8_t s_mst_hold;

void zvb_sound_set_voices(sound_voice_t voices, uint16_t divider, sound_waveform_t waveform)
{
    zvb_map_peripheral(ZVB_PERI_SOUND_IDX);

    /* Temporarily hold the voices */
    zvb_peri_sound_hold = s_mst_hold | voices;

    /* Map the voices */
    zvb_peri_sound_select = voices;
    /* Set the divider and the waveform */
    zvb_peri_sound_freq_low  = divider & 0xff;
    zvb_peri_sound_freq_high = (divider >> 8) & 0xff;
    zvb_peri_sound_wave = waveform;
    /* Unhold the voices if they were unhold before the call */
    zvb_peri_sound_hold = s_mst_hold;
}
