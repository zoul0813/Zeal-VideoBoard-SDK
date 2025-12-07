/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_hardware.h"
#include "zvb_dma.h"

dma_error zvb_dma_prepare_descriptor(zvb_dma_descriptor_t* desc, zvb_dma_descriptor_config_t* config) {
    desc->rd_addr_lo = config->rd_addr & 0xFFFF;
    desc->rd_addr_hi = (config->rd_addr >> 16) & 0xFF;
    desc->wr_addr_lo = config->wr_addr & 0xFFFF;
    desc->wr_addr_hi = (config->wr_addr >> 16) & 0xFF;
    desc->length = config->length;
    desc->flags = config->flags;

    return DMA_SUCCESS; // ERR_SUCCESS
}
