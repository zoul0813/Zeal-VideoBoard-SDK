/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "zvb_hardware.h"
#include "zvb_dma.h"

dma_error zvb_dma_start_transfer(zvb_dma_descriptor_t *desc) {
    uint32_t desc_phys_addr = zvb_dma_virt_to_phys(desc);

    zvb_map_peripheral(ZVB_PERI_DMA_IDX);
    zvb_peri_dma_addr0 = (desc_phys_addr >> 0) & 0xFF;
    zvb_peri_dma_addr1 = (desc_phys_addr >> 8) & 0xFF;
    zvb_peri_dma_addr2 = (desc_phys_addr >> 16) & 0xFF;
    zvb_peri_dma_ctrl = ZVB_PERI_DMA_CTRL_START;

    return DMA_SUCCESS; // ERR_SUCCESS
}
