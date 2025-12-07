/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_hardware.h"
#include "zvb_dma.h"

dma_error zvb_dma_set_write(zvb_dma_descriptor_t* desc, uint32_t addr) {
    desc->wr_addr_lo = addr & 0xFFFF;
    desc->wr_addr_hi = (addr >> 16) & 0xFF;

    return DMA_SUCCESS; // ERR_SUCCESS
}
