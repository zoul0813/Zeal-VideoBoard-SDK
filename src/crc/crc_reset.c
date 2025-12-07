/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_crc.h"


void zvb_crc_reset(void)
{
    zvb_peri_crc_ctrl = BIT(IO_CRC32_CTRL_RESET_BIT);
}
