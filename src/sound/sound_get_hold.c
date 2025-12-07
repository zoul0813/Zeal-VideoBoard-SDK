/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_sound.h"

extern uint8_t s_mst_hold;

sound_voice_t zvb_sound_get_hold(void)
{
    return s_mst_hold;
}
